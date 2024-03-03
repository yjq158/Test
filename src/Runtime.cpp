#include <iostream>

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"

#include "SymbolicInterpreter.h"
#include "llvm/Support/raw_ostream.h"
#include "z3++.h"

extern SymbolicInterpreter SI;

z3::expr eval(z3::expr &E) {
  if (E.kind() == Z3_NUMERAL_AST) {
    return E;
  } else {
    MemoryTy Mem = SI.getMemory();
    Address Register(E);
    if (Mem.find(Register) != Mem.end()) {
      return Mem.at(Register);
    } else {
      std::cout << "Warning: Cannot find register " << Register << " in memory "
                << std::endl;
      return E;
    }
  }
}

/**
 * @brief Symbolically evaluate Alloca
 *
 * @param R RegisterID
 * @param Ptr Address
 */
extern "C" void __DSE_Alloca__(int R, int *Ptr) {
  MemoryTy &Mem = SI.getMemory();
  Address Register(R);
  z3::expr SE = SI.getContext().int_val((uintptr_t)Ptr);
  Mem.insert(std::make_pair(Register, SE));
}

/**
 * @brief Symbolically evaluate Store
 *
 * @param X Address
 */
extern "C" void __DSE_Store__(int *X) {
  MemoryTy &Mem = SI.getMemory();
  Address Addr(X);
  z3::expr SE = eval(SI.getStack().top());
  SI.getStack().pop();
  Mem.erase(Addr);
  Mem.insert(std::make_pair(Addr, SE));
}

/**
 * @brief Symbolically evaluate Load
 *
 * TODO: Implement this.
 *
 * @param Y Address of destination
 * @param X Address of Load source
 */
extern "C" void __DSE_Load__(int Y, int *X) {
    MemoryTy &Mem = SI.getMemory();
    Address Load_Source(X);
    Address Destionation(Y);
    z3::expr Source = Mem.at(Load_Source);

    Mem.erase(Destionation);
    Mem.insert(std::make_pair(Destionation, Source));
}

/**
 * @brief Symbolically evaluate Comparisions
 *
 * TODO: Implement this.
 *
 * @param R Address of Comparision result
 * @param Op Operator Kind
 */
extern "C" void __DSE_ICmp__(int R, int Op) {
    MemoryTy &Mem = SI.getMemory();
    llvm::CmpInst::Predicate predicate = static_cast<llvm::CmpInst::Predicate>(Op);
    Address Register(R);
    z3::expr Z = eval(SI.getStack().top());
    SI.getStack().pop();
    z3::expr Y = eval(SI.getStack().top());
    SI.getStack().pop();
    std::string Name = "Cmp" + std::to_string(R);
    z3::expr X =  SI.getContext().bool_const(Name.c_str());
    switch (predicate) {
        case llvm::CmpInst::ICMP_EQ: X = (Y == Z); break;
        case llvm::CmpInst::ICMP_NE: X = (Y != Z); break;
        case llvm::CmpInst::ICMP_UGT: X =(Y > Z);break;
        case llvm::CmpInst::ICMP_ULT: X = (Y < Z);break;
        case llvm::CmpInst::ICMP_UGE: X = (Y >= Z);break;
        case llvm::CmpInst::ICMP_ULE: X = (Y <= Z);break;

        case llvm::CmpInst::ICMP_SGT: X = (Y > Z);break;
        case llvm::CmpInst::ICMP_SLT: X = (Y < Z);break;
        case llvm::CmpInst::ICMP_SGE: X = (Y >= Z);break;
        case llvm::CmpInst::ICMP_SLE: X = (Y <= Z);break;

        case llvm::CmpInst::FCMP_OEQ: X = (Y == Z); break;  // 浮点数相等
        case llvm::CmpInst::FCMP_ONE: X = (Y != Z); break;  // 浮点数不等
        case llvm::CmpInst::FCMP_UGT: X = (Y > Z); break;   // 无符号浮点数大于
        case llvm::CmpInst::FCMP_ULT: X = (Y < Z); break;   // 无符号浮点数小于
        case llvm::CmpInst::FCMP_UGE: X = (Y >= Z); break;  // 无符号浮点数大于等于
        case llvm::CmpInst::FCMP_ULE: X = (Y <= Z); break;  // 无符号浮点数小于等于
        case llvm::CmpInst::FCMP_UEQ: X = (Y == Z); break;  // 浮点数不等或者有一个为 NaN
        case llvm::CmpInst::FCMP_UNE: X = (Y != Z); break;  // 浮点数等或者两者都为 NaN

        case llvm::CmpInst::FCMP_OGT: X = (Y > Z); break;    // 浮点数大于
        case llvm::CmpInst::FCMP_OLT: X = (Y < Z); break;    // 浮点数小于
        case llvm::CmpInst::FCMP_OGE: X = (Y >= Z); break;   // 浮点数大于等于
        case llvm::CmpInst::FCMP_OLE: X = (Y <= Z); break;   // 浮点数小于等于
        default:
            // 处理未知的比较指令谓词
            llvm::errs() << "Unknown comparison predicate is "<<predicate<<" \n";
            break;
    }
    if(Mem.find(Register) != Mem.end())
        Mem.erase(Register);
    Mem.insert(std::make_pair(Register, X));
}

/**
 * @brief Symbolically evaluate Binary Operation.
 *
 * TODO: Implement this.
 *
 * @param R Address of Binary Operation result.
 * @param Op Operator Kind
 */
extern "C" void __DSE_BinOp__(int R, int Op) {
    MemoryTy &Mem = SI.getMemory();
    Address Register(R);
    z3::expr Z = eval(SI.getStack().top());
    SI.getStack().pop();
    z3::expr Y = eval(SI.getStack().top());
    SI.getStack().pop();
    std::string Name = "Bin" + std::to_string(R);
    z3::expr X =  SI.getContext().int_const(Name.c_str());
    llvm::Instruction::BinaryOps Binop= static_cast<llvm::Instruction::BinaryOps>(Op);
    switch (Binop) {
        case llvm::Instruction::Add:
            X = (Y + Z);
            break;
        case llvm::Instruction::Sub:
            X = (Y - Z);
            break;
        case llvm::Instruction::Mul:
            X = (Y * Z);
            break;
        case llvm::Instruction::SDiv:
        case llvm::Instruction::UDiv:
            X = (Y / Z);
            break;
        case llvm::BinaryOperator::SRem:
            X = (Y % Z);
            break;
        case llvm::BinaryOperator::URem:
            X = (Y % Z);  // 无符号取余
            break;
        case llvm::BinaryOperator::Shl:
            // 使用 Z3 的 shl 函数执行逻辑左移
            X = z3::shl(Y, Z);
            break;

        case llvm::BinaryOperator::LShr:
            // 使用 Z3 的 lshr 函数执行逻辑右移
            X = z3::lshr(Y, Z);
            break;

        case llvm::BinaryOperator::AShr:
            // 使用 Z3 的 ashr 函数执行算术右移
            X = z3::ashr(Y, Z);
            break;

        case llvm::BinaryOperator::And:
            X = (Y & Z);  // 位与
            break;
        case llvm::BinaryOperator::Or:
            X = (Y | Z);  // 位或
            break;
        case llvm::BinaryOperator::Xor:
            X = (Y ^ Z);  // 位异或
            break;
        default:
            // 处理未知的二元操作
            llvm::errs() << "Unknown BinaryOps is " << Binop << "\n";
            break;
    }
    if(Mem.find(Register)!=Mem.end())
        Mem.erase(Register);
    Mem.insert(std::make_pair(Register, X));

}
