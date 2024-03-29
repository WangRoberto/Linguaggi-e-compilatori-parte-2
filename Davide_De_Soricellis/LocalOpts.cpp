//===-- LocalOpts.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LocalOpts.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
// L'include seguente va in LocalOpts.h
#include <llvm/IR/Constants.h>

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
  
  // first optimization  

  std::vector<Instruction*> InstructionsToRemove;

  for( Instruction &I : B ){
    Value *Op1 = I.getOperand(0);                   // primo operando
    Value *Op2 = I.getOperand(1);                   // secondo operando
    ConstantInt *C1 = dyn_cast<ConstantInt>(Op1);   // cast a costante
    ConstantInt *C2 = dyn_cast<ConstantInt>(Op2);

    if( I.getOpcode() == Instruction::Add ){        //ottimizzazione add
      
        if ( C1 != nullptr && C1->isZero() ){
          I.replaceAllUsesWith(Op2);
           if (I.user_empty()) {
                // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                InstructionsToRemove.push_back(&I);
         
        }
        else if ( C2 != nullptr && C2->isZero() ){
          I.replaceAllUsesWith(Op1);
           if (I.user_empty()) {
                // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                InstructionsToRemove.push_back(&I);
            }
        
        }


    } 

    if( I.getOpcode() == Instruction::Mul ){        //ottimizzazione mul
      
        if ( C1 != nullptr && C1->isOne() ){        //controllo che l' operando sia una costante e con valore 1
          I.replaceAllUsesWith(Op2);                //rimpiazzo l' istruzione con l' operando diverso da 1 nelle
           if (I.user_empty()) {                    //istruzioni successive
                // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                InstructionsToRemove.push_back(&I);
            }
          
        }
        else if ( C2 != nullptr && C2->isOne() ){   //controllo che l' operando sia una costante e con valore 1
          I.replaceAllUsesWith(Op1);                //rimpiazzo l' istruzione con l' operando diverso da 1 nelle
           if (I.user_empty()) {                    //istruzioni successive
                // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                InstructionsToRemove.push_back(&I);
            }
        
        }
    } 





  }

    // Rimuovi le istruzioni che non hanno utilizzatori
  for (Instruction *I : InstructionsToRemove) {
      I->eraseFromParent();
  }

  //end first optimization

    return true;
}

bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses LocalOpts::run(Module &M,
                                      ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
  

}

