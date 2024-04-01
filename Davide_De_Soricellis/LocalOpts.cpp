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

  /*std::vector<Instruction*> InstructionsToRemove;

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

  */

  //second optimization

  /*std::vector<Instruction*> InstructionsToRemove;

  for (Instruction &I : B) {
        // Controlla se l'istruzione è di tipo moltiplicazione
        if (I.getOpcode() == Instruction::Mul) {                              //ottimizzazione moltiplicazione

            Value *Op1 = I.getOperand(0);
            Value *Op2 = I.getOperand(1);
            ConstantInt *C1 = dyn_cast<ConstantInt>(Op1);
            ConstantInt *C2 = dyn_cast<ConstantInt>(Op2);
            ConstantInt *shift = nullptr;
            
            if( C1 != nullptr ){
               
               
               unsigned int sh = C1->getValue().nearestLogBase2();                        // shift intero
               int val = C1->getSExtValue();
               int pot2 = 1 << sh;
               shift = ConstantInt::get(C1->getType(),C1->getValue().nearestLogBase2());  // ConstantInt shift
               
               Instruction *S = BinaryOperator::Create(Instruction::Shl,I.getOperand(1),shift); //instruction shift
               S->insertAfter(&I);
               
               if( pot2 < val ){                                                            //aggiungo delle add
                  for( int i = pot2 ; i < val ; i++ ){
                    Instruction* prec = S;
                    S = BinaryOperator::Create(Instruction::Add, prec, I.getOperand(1));
                    S->insertAfter(prec);
                  }

               }
               else if( pot2 > val ){
                  for( int i = val ; i < pot2 ; i++ ){                                      //aggiungo delle sub
                    Instruction* prec = S;
                    S = BinaryOperator::Create(Instruction::Sub, prec, I.getOperand(1));
                    S->insertAfter(prec);
                  }

               }
              



             I.replaceAllUsesWith(S);
             if (I.user_empty()) {
                // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                InstructionsToRemove.push_back(&I);
            }
                
            }
            else if( C2 != nullptr ){
                
               unsigned int sh = C2->getValue().nearestLogBase2();                        // shift intero
               int val = C2->getSExtValue();
               int pot2 = 1 << sh;
               shift = ConstantInt::get(C2->getType(),C2->getValue().nearestLogBase2());  // ConstantInt shift
               
               Instruction *S = BinaryOperator::Create(Instruction::Shl,I.getOperand(0),shift); //instruction shift
               S->insertAfter(&I);
               
               if( pot2 < val ){
                  for( int i = pot2 ; i < val ; i++ ){
                    Instruction* prec = S;
                    S = BinaryOperator::Create(Instruction::Add, prec, I.getOperand(0));
                    S->insertAfter(prec);
                  }
               }
               else if( pot2 > val ){
                  for( int i = val ; i < pot2 ; i++ ){
                    Instruction* prec = S;
                    S = BinaryOperator::Create(Instruction::Sub, prec, I.getOperand(0));
                    S->insertAfter(prec);
                  }
               }


              I.replaceAllUsesWith(S);
              if (I.user_empty()) {
                // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                InstructionsToRemove.push_back(&I);
              }
            }
        }

        else if(I.getOpcode() == Instruction::SDiv ){                         //ottimizzazione divisione
          Value *Op1 = I.getOperand(0);
          Value *Op2 = I.getOperand(1);
          //ConstantInt *C1 = dyn_cast<ConstantInt>(Op1);
          ConstantInt *C2 = dyn_cast<ConstantInt>(Op2);
          ConstantInt *shift = nullptr;

          if( C2 != nullptr && C2->getValue().isPowerOf2() ){                               //controllo che il divisore sia una potenza di 2
            shift = ConstantInt::get(C2->getType(),C2->getValue().exactLogBase2());
            Instruction *NewInst = BinaryOperator::Create(Instruction::AShr, Op1, shift);
            NewInst->insertAfter(&I);
            I.replaceAllUsesWith(NewInst);
            if (I.user_empty()) {
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
    */

    // third optimization
    std::vector<Instruction*> InstructionsToRemove;
    
    for ( Instruction &I : B ){
      Value *Op1 = I.getOperand(0);
      Value *Op2 = I.getOperand(1);
      ConstantInt *C1 = dyn_cast<ConstantInt>(Op1);
      ConstantInt *C2 = dyn_cast<ConstantInt>(Op2);

      ConstantInt *Cu1 = nullptr;
      ConstantInt *Cu2 = nullptr;
      
      if ( I.getOpcode() == Instruction::Add ){              
           for (auto Iter = I.user_begin(); Iter != I.user_end(); ++Iter){ 
              User *Inst = *Iter;
              Instruction *InstUser = dyn_cast<Instruction>(Inst);
              if( InstUser != nullptr && InstUser->getOpcode() == Instruction::Sub){
                //Cu1 = dyn_cast<ConstantInt>(InstUser->getOperand(0));
                Cu2 = dyn_cast<ConstantInt>(InstUser->getOperand(1));
                
             

                if( Cu2 != nullptr ){
                  if( C1 != nullptr && C1->getSExtValue() == Cu2->getSExtValue() ){
                    //errs() << "Ottimizzo istruzione(primo op) " << *InstUser << " con " << I << "\n";
                    InstUser->replaceAllUsesWith(Op2);
                    if (InstUser->user_empty()) {
                        // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                        InstructionsToRemove.push_back(InstUser);
                    }
                  }
                  else if( C2 != nullptr && C2->getSExtValue() == Cu2->getSExtValue() ){
                    //errs() << "Ottimizzo istruzione(secondo op) " << *InstUser << " con " << I << "\n";
                    InstUser->replaceAllUsesWith(Op1);
                    if (InstUser->user_empty()) {
                        // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                        InstructionsToRemove.push_back(InstUser);
                    }
                  }
                
                }     
              
              }
            }
           
      }

      else if ( I.getOpcode() == Instruction::Sub ){              
           for (auto Iter = I.user_begin(); Iter != I.user_end(); ++Iter){ 
              User *Inst = *Iter;
              Instruction *InstUser = dyn_cast<Instruction>(Inst);
              if( InstUser != nullptr && InstUser->getOpcode() == Instruction::Add){
                Cu1 = dyn_cast<ConstantInt>(InstUser->getOperand(0));
                Cu2 = dyn_cast<ConstantInt>(InstUser->getOperand(1));
                
             

                if( C2 != nullptr ){
                  if( (Cu1 != nullptr && Cu1->getSExtValue() == C2->getSExtValue() )||( Cu2 != nullptr && Cu2->getSExtValue() == C2->getSExtValue())){
                    //errs() << "Ottimizzo istruzione(primo op) " << *InstUser << " con " << I << "\n";
                    InstUser->replaceAllUsesWith(Op1);
                    if (InstUser->user_empty()) {
                        // Se non ha utilizzatori, aggiungila alla lista delle istruzioni da rimuovere
                        InstructionsToRemove.push_back(InstUser);
                    }
                  }
                  
                
                }     
              
              }
            }
         
      }

    }
    // Rimuovi le istruzioni che non hanno utilizzatori
    for (Instruction *I : InstructionsToRemove)
        I->eraseFromParent();

    // end third optimization


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

