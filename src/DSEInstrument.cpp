#include "DSEInstrument.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

namespace dse {

    static const char *DSE_INIT_FUNCTION_NAME = "__DSE_Init__";
    static const char *DSE_ALLOCA_FUNCTION_NAME = "__DSE_Alloca__";
    static const char *DSE_STORE_FUNCTION_NAME = "__DSE_Store__";
    static const char *DSE_LOAD_FUNCTION_NAME = "__DSE_Load__";
    static const char *DSE_CONST_FUNCTION_NAME = "__DSE_Const__";
    static const char *DSE_REGISTER_FUNCTION_NAME = "__DSE_Register__";
    static const char *DSE_ICMP_FUNCTION_NAME = "__DSE_ICmp__";
    static const char *DSE_BRANCH_FUNCTION_NAME = "__DSE_Branch__";
    static const char *DSE_BINOP_FUNCTION_NAME = "__DSE_BinOp__";


/**
 * @brief Instrument to initialize the Z3 solver.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param F Function to Instrument
 * @param I Instrumentation location
 */
    void instrumentDSEInit(Module *Mod, Function &F, Instruction &I) {
        auto Fun = Mod->getFunction(DSE_INIT_FUNCTION_NAME);
        CallInst::Create(Fun, {}, "", &I);
    }

/**
 * @brief Instrument to Alloca Instructions.
 *
 * @param Mod Current Module
 * @param AI Instrumentation location
 */
    void instrumentAlloca(Module *Mod, AllocaInst *AI) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);

        Value *VarID = ConstantInt::get(Int32Type, getRegisterID(AI));
        std::vector<Value *> Args = {VarID, AI};

        auto Fun = Mod->getFunction(DSE_ALLOCA_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", AI->getNextNonDebugInstruction());
    }

/**
 * @brief Instrument to Store Instructions.
 *
 * @param Mod Current Module
 * @param SI Instrumentation location
 */
    void instrumentStore(Module *Mod, StoreInst *SI) {
        std::vector<Value *> Args = {SI->getPointerOperand()};

        auto Fun = Mod->getFunction(DSE_STORE_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", SI->getNextNonDebugInstruction());
    }

/**
 * @brief Instrument to Load Instructions.
 *
 * @param Mod Current Module
 * @param LI Instrumentation location
 */
    void instrumentLoad(Module *Mod, LoadInst *LI) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);

        Value *VarID = ConstantInt::get(Int32Type, getRegisterID(LI));
        std::vector<Value *> Args = {VarID, LI->getPointerOperand()};

        auto Fun = Mod->getFunction(DSE_LOAD_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", LI->getNextNonDebugInstruction());
    }

/**
 * @brief Instrument Constant values.
 *
 * @param Mod Current Module
 * @param ConstInt Constant
 * @param I Instrumentation location.
 */
    void instrumentConstantValue(Module *Mod, ConstantInt *ConstInt, Instruction *I) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);

        Value *ConstValue = ConstantInt::get(Int32Type, ConstInt->getSExtValue());
        std::vector<Value *> Args = {ConstValue};

        auto Fun = Mod->getFunction(DSE_CONST_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", I->getNextNonDebugInstruction());
    }

/**
 * @brief Instrument Registers.
 *
 *
 * @param Mod Current Module
 * @param Var Variable
 * @param I Instrumentation location
 */
    void instrumentRegister(Module *Mod, Value *Var, Instruction *I) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);

        Value *VarID = ConstantInt::get(Int32Type, getRegisterID(Var));
        std::vector<Value *> Args = {VarID};

        auto Fun = Mod->getFunction(DSE_REGISTER_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", I->getNextNonDebugInstruction());
    }

/**
 * @brief Instrument a Value
 *
 *
 * Hint: Values are a stored in registers;
 *       some values may also be a constant.
 *       Use the function you just defined above.
 *
 * @param Mod Current Module
 * @param Val Value
 * @param I Instrumentation location
 */
    void instrumentValue(Module *Mod, Value *Val, Instruction *I) {
        if (ConstantInt *ConstInt = dyn_cast<ConstantInt>(Val)) {
            instrumentConstantValue(Mod, ConstInt, I);
        } else {
            instrumentRegister(Mod, Val, I);
        }
    }

/**
 * @brief Instrument Comparision Instructions.
 *
 *
 * @param Mod Current Module
 * @param CI Instrumentation location
 */
    void instrumentICmp(Module *Mod, ICmpInst *CI) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);

        Value *VarID = ConstantInt::get(Int32Type, getRegisterID(CI));
        CmpInst::Predicate predicate = CI->getPredicate();
        //errs()<<predicate<<"\n";
        unsigned int opcode = static_cast<unsigned int>(predicate);
        errs()<<opcode<<"\n";
        Value *CIOpCode = ConstantInt::get(Int32Type, opcode);
        std::vector<Value *> Args = {VarID, CIOpCode};

        auto Fun = Mod->getFunction(DSE_ICMP_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", CI->getNextNonDebugInstruction());


    }

/**
 * @brief Instrument Branch Instructions.
 *
 * TODO: Implement this.
 *
 * @param Mod Current Module
 * @param BI Instrumentation location
 */
    void instrumentBranch(Module *Mod, BranchInst *BI) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);
        llvm::Value* condition = BI->getCondition();
        Value *BID = ConstantInt::get(Int32Type, getBranchID(BI));
        Value *RID = ConstantInt::get(Int32Type, getRegisterID(condition));

        std::vector<Value *> Args = {BID, RID,condition};

        auto Fun = Mod->getFunction(DSE_BRANCH_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", BI);
    }

/**
 * @brief Instrument BinaryOperator.
 *
 *
 * @param Mod Current Module
 * @param BO Instrumentation location
 */
    void instrumentBinOp(Module *Mod, BinaryOperator *BO) {
        auto &Context = Mod->getContext();
        auto *Int32Type = Type::getInt32Ty(Context);

        Value *VarID = ConstantInt::get(Int32Type, getRegisterID(BO));

        Value *OpCode = ConstantInt::get(Int32Type, BO->getOpcode());
        std::vector<Value *> Args = {VarID, OpCode};

        auto Fun = Mod->getFunction(DSE_BINOP_FUNCTION_NAME);
        CallInst::Create(Fun, Args, "", BO->getNextNonDebugInstruction());
    }


    bool DSEInstrument::runOnFunction(Function &F) {
        LLVMContext &Context = F.getContext();
        Module *Mod = F.getParent();
        Type *VoidType = Type::getVoidTy(Context);
        Type *Int1Type = Type::getInt1Ty(Context);
        auto *Int32Type = Type::getInt32Ty(Context);
        Type *Int32PtrType = Type::getInt32PtrTy(Context);

        // Insert all the DSE function declarations into Module.
        Mod->getOrInsertFunction(DSE_INIT_FUNCTION_NAME, VoidType);
        Mod->getOrInsertFunction(DSE_ALLOCA_FUNCTION_NAME, VoidType, Int32Type, Int32PtrType);
        Mod->getOrInsertFunction(DSE_STORE_FUNCTION_NAME, VoidType, Int32PtrType);
        Mod->getOrInsertFunction(DSE_LOAD_FUNCTION_NAME, VoidType, Int32Type, Int32PtrType);
        Mod->getOrInsertFunction(DSE_CONST_FUNCTION_NAME, VoidType, Int32Type);
        Mod->getOrInsertFunction(DSE_REGISTER_FUNCTION_NAME, VoidType, Int32Type);
        Mod->getOrInsertFunction(DSE_ICMP_FUNCTION_NAME, VoidType, Int32Type, Int32Type);
        Mod->getOrInsertFunction(DSE_BRANCH_FUNCTION_NAME, VoidType, Int32Type, Int32Type, Int1Type);
        Mod->getOrInsertFunction(DSE_BINOP_FUNCTION_NAME, VoidType, Int32Type, Int32Type);

        if (F.getName().equals("main")) {
            // TODO: Initilize the DSE Engine
            BasicBlock &entryBlock = F.getEntryBlock();
            Instruction *FInstruction = entryBlock.getFirstNonPHIOrDbgOrLifetime();
            instrumentDSEInit(Mod,F,*FInstruction);
        }

        // Instrument each instruction
        for (inst_iterator It = inst_begin(F), E = inst_end(F); It != E; ++It) {
            Instruction *I = &(*It);
            if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
                instrumentAlloca(Mod, AI);
            }
            else if (StoreInst *SI = dyn_cast<StoreInst>(I)) {
                Value *Val1 = SI->getValueOperand();
                instrumentStore(Mod, SI);
                instrumentValue(Mod, Val1, SI);

            }
            else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
                instrumentLoad(Mod, LI);
            }
            else if (ICmpInst *CI = dyn_cast<ICmpInst>(I)) {
                instrumentICmp(Mod, CI);
                instrumentValue(Mod, CI->getOperand(1),CI);
                instrumentValue(Mod, CI->getOperand(0),CI);

            }
            else if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
                if (BI->isUnconditional());
                else
                instrumentBranch(Mod, BI);
            }
            else if (BinaryOperator *BO = dyn_cast<BinaryOperator>(I)) {
                instrumentBinOp(Mod, BO);
                instrumentValue(Mod, BO->getOperand(1),BO);
                instrumentValue(Mod, BO->getOperand(0),BO);

            }
        }
        return true;
    }

    char DSEInstrument::ID = 1;
    static RegisterPass<DSEInstrument>
            X("DSEInstrument", "Instrumentations for Dynamic Symbolic Execution", false,
              false);

} // namespace dse
