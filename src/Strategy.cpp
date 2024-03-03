#include "Strategy.h"
#include <set>
#include <iostream>
#include <string>
#include <z3++.h>
#include <vector>
/*******************************************************
 * NOTE: You are free to edit this file as you see fit *
 *******************************************************/

/**
 * Strategy to explore various paths of execution.
 *
 * TODO: Implement your search strategy.
 *
 * @param OldVec Vector of Z3 expressions.
 */

class ExprCmp
{
public:
    bool operator()(const z3::expr &a, const z3::expr &b) const
    {
        return a.hash() < b.hash();
    }
};

std::set<z3::expr, ExprCmp> visited;
void searchStrategydfs(z3::expr_vector &OldVec)
{ /* Add your code here */
    if (OldVec.empty()) return;
    while (!OldVec.empty()){
        std::cout<<"The number of Conditions: "<<OldVec.size()<<std::endl;
        for (const auto &E : OldVec) {
            std::cout<<" Before Expr: "<<E<<std::endl;
        }
        z3::expr tmpExp = OldVec.back();
        //z3::expr tmpExp = OldVec.back();
        std::cout<<" The Vector's last  Expr: "<<tmpExp<<std::endl;
        std::cout<<" The Vector's last simplify Expr: "<<tmpExp.simplify()<<std::endl;
        if (visited.find((!tmpExp).simplify()) == visited.end()){
            std::cout<<" The Slected  Expr is : "<<tmpExp<<std::endl;
            std::cout<<" The Slected simplify Expr is : "<<tmpExp<<std::endl;
            visited.insert(tmpExp.simplify());
            visited.insert((!tmpExp).simplify());
            //visited.insert((!tmpExp));
           //std::cout <<" The  Filped  Expr is: "<< (!tmpExp) << "\n";
            std::cout <<" The  Filped  simplify Expr is: "<< (!tmpExp).simplify() << "\n";
            OldVec.pop_back();
            OldVec.push_back((!tmpExp).simplify());
            break;
        }
        else{
            //std::cout << "visited find:\n" << (!tmpExp) << std::endl;
            OldVec.pop_back();
        }
        std::cout<<std::endl;
    }
//    for (const auto &E : OldVec) {
//        std::cout<<" The Final Expr: "<<E<<std::endl;
//    }
//   std::cout<<std::endl;
    if (OldVec.empty()){
        printf("Finish but not find bug!\n");
    }
    std::cout<<std::endl;
}

std::vector<z3::expr> condition;
void searchStrategybfs(z3::expr_vector &OldVec)
{ /* Add your code here */
    if (OldVec.empty()) return;
//    for (const auto &E : OldVec) {
//        std::cout<<" Before Expr: "<<E<<std::endl;
//        //condition.insert(E);
//    }

    while (!OldVec.empty()){
        for (const auto &E : OldVec) {
            std::cout<<" Before Expr: "<<E<<std::endl;
        }
        z3::expr tmpExp = OldVec[0].simplify();
        std::cout<<" The Vector's first simplify Expr: "<<tmpExp<<std::endl;
        if (visited.find((!tmpExp).simplify()) == visited.end()){
            std::cout<<" The Slected simplify Expr is : "<<tmpExp<<std::endl;
            visited.insert(tmpExp.simplify());
            visited.insert((!tmpExp).simplify());
            std::cout <<" The  Filped  simplify Expr is: "<< (!tmpExp).simplify() << "\n";
            //OldVec.erase(OldVec.begin());
            for(int i =0;i<OldVec.size()-1;i++){
                //std::cout <<" wei "<< OldVec[i+1].simplify()<< "\n";
                z3::expr s=OldVec[i+1];
               // std::cout <<" s:"<< s<< "\n";
                OldVec.set(i, s);
                //std::cout <<" wei shen mo mei you gai bian "<< OldVec[i].simplify()<<" "<<i<< "\n";
            }
            OldVec.pop_back();
            OldVec.push_back((!tmpExp).simplify());
            break;
        }
        else{
            std::cout << "visited find:\n" << tmpExp << std::endl;
              z3::expr begin = OldVec[0];
            for(int i =0;i<OldVec.size()-1;i++){
                //std::cout <<" wei "<< OldVec[i+1].simplify()<< "\n";
                z3::expr s=OldVec[i+1];
               // std::cout <<" s:"<< s<< "\n";
                OldVec.set(i, s);
                //std::cout <<" wei shen mo mei you gai bian "<< OldVec[i].simplify()<<" "<<i<< "\n";
            }
            OldVec.set(OldVec.size()-1,begin);

        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
    if (OldVec.empty()){
        printf("Finish but not find bug!\n");
    }
    std::cout<<std::endl;
}



