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
#include <iostream>
#include <vector>

using namespace llvm;


bool runOnBasicBlock(BasicBlock &B){
  int cont = 0;
  int nonConstante;
  std::vector<Instruction*> istruzioniDaEliminare;
  //for(Instruction Instr : B){
  for(auto Iter = B.begin(); Iter != B.end(); ++Iter){
    Instruction &Instr = *Iter;
    cont ++;
    outs() << "Istruzione n°" <<cont << ":" << Instr<< "\n";
    
    //First optimization
    if(Instr.getOpcode() == Instruction::Add){
      //outs() << "Addizione:" << "\n";
      for(unsigned int i = 0; i < Instr.getNumOperands(); i++){
        nonConstante = 0;
        if(ConstantInt* conInt = dyn_cast<ConstantInt>(Instr.getOperand(i))){
            APInt c = conInt->getValue();
            if(i == 0){
            nonConstante = 1;
            } 
            if(c.isZero()){
              
              Instr.replaceAllUsesWith(Instr.getOperand(nonConstante));
              //Instr.eraseFromParent();
              istruzioniDaEliminare.push_back(&Instr);
          
          }
        }
      }
    }

    //Second optimization
    //In caso sia un operatore binario
      /*
      if(dyn_cast<BinaryOperator>(Instr)){
        if(ConstantInt* conInt = dyn_cast<ConstantInt>(Instr.getOperand(1))){
              APInt c = conInt->getValue();
              if(c.isPowerOf2()){
                unsigned int cont = c.exactLogBase2(); // cambiare con exact
                Value* op = reinterpret_cast<Value*>(cont);
                Instruction *NewInst = BinaryOperator::Create(
                Instruction::Shl, Instr.getOperand(0), op);
                NewInst->insertAfter(&Instr);
                Instr.replaceAllUsesWith(NewInst);
                return true;
            }
          }
      }*/

    if(Instr.getOpcode() == Instruction::Mul){
      //outs() << "Moltiplicazione:" << "\n";
      for(unsigned int i = 0; i < Instr.getNumOperands(); i++){//itera gli operandi per vedere se c'è una costante
        nonConstante = 0;
        if(ConstantInt* conInt = dyn_cast<ConstantInt>(Instr.getOperand(i))){
          APInt c = conInt->getValue();
          
          if(i == 0){
            nonConstante = 1;
          }

          if(c.isOne()){
            
            Instr.replaceAllUsesWith(Instr.getOperand(nonConstante));
            //Instr.eraseFromParent();
            istruzioniDaEliminare.push_back(&Instr);
            
          }

          if(c.isPowerOf2()){
            //unsigned int cont = c.exactLogBase2(); // cambiare con exact
            //Value* op = reinterpret_cast<Value*>(cont);
            Constant *op = ConstantInt::get(conInt->getType(), c.exactLogBase2());
            Instruction *NewInst = BinaryOperator::Create(
            Instruction::Shl, Instr.getOperand(nonConstante), op);
            NewInst->insertAfter(&Instr);
            
            Instr.replaceAllUsesWith(NewInst);
            Iter = std::next(Iter, 1);
            //Instr.eraseFromParent();
            istruzioniDaEliminare.push_back(&Instr);
            
          }
          unsigned int cont = c.nearestLogBase2();
          //outs() << "Nearest:" << cont << "\n";
          unsigned int nearestValue = 1 << cont;
          //outs() << "Nearest Value:" << nearestValue << "\n";
          unsigned int costanteOriginale = c.getLimitedValue(UINT64_MAX);
          //outs() << "costanteOriginale:" << costanteOriginale << "\n";
          
          if(nearestValue < costanteOriginale){
            unsigned int dif = costanteOriginale - nearestValue;

            //outs() << "Dif:" << dif << "\n";
            
            Constant *op = ConstantInt::get(conInt->getType(), cont);
            Constant *op2 = ConstantInt::get(conInt->getType(), dif);
            
            Instruction *operando1 = BinaryOperator::Create(
              Instruction::Shl, Instr.getOperand(nonConstante), op);          
            Instruction *operando2 = BinaryOperator::Create(
              Instruction::Mul, Instr.getOperand(nonConstante), op2);

            /* BasicBlock* temp = BasicBlock::Create(B.getContext(), "entrypoint");
            operando1->insertInto(temp, temp->begin());
            operando2->insertAfter(operando1); */

            Instruction *NewInst = BinaryOperator::Create(
            Instruction::Sub, operando1, operando2);

            //outs() << "Nuova istruzione:" << NewInst << "\n";

            operando1->insertAfter(&Instr);
            operando2->insertAfter(operando1);
            NewInst->insertAfter(operando2);
            Instr.replaceAllUsesWith(NewInst);
            Iter = std::next(Iter, 3);

            istruzioniDaEliminare.push_back(&Instr);
            //istruzioniDaEliminare.push_back(operando1);
            //istruzioniDaEliminare.push_back(operando2);
            //Instr.eraseFromParent();
            //operando1->eraseFromParent();
            //operando2->eraseFromParent();
            
          }     
          else{
            unsigned int dif = nearestValue - costanteOriginale ;

            //outs() << "Dif:" << dif << "\n";
            
            Constant *op = ConstantInt::get(conInt->getType(), cont);
            Constant *op2 = ConstantInt::get(conInt->getType(), dif);

            Instruction *operando1 = BinaryOperator::Create(
              Instruction::Shl, Instr.getOperand(nonConstante), op);          
            Instruction *operando2 = BinaryOperator::Create(
              Instruction::Mul, Instr.getOperand(nonConstante), op2);

            /* BasicBlock* temp = BasicBlock::Create(B.getContext(), "entrypoint");
            operando1->insertInto(temp, temp->begin());
            operando2->insertAfter(operando1); */
          
            Instruction *NewInst = BinaryOperator::Create(
            Instruction::Add, operando1, operando2);

            //outs() << "Nuova istruzione:" << "\n";

            operando1->insertAfter(&Instr);
            operando2->insertAfter(operando1);
            NewInst->insertAfter(operando2);
            Instr.replaceAllUsesWith(NewInst);
            Iter = std::next(Iter, 3);
            //Instr.eraseFromParent();
            //operando1->eraseFromParent();
            //operando2->eraseFromParent();

            istruzioniDaEliminare.push_back(&Instr);
            //istruzioniDaEliminare.push_back(operando1);
            //istruzioniDaEliminare.push_back(operando2);
          }
        }      
      }
    }

    //outs() << Instr.isIntDivRem() << "\n";

    if(Instr.isIntDivRem()){
      //outs() << "Ci sono!" << "\n";
      if(ConstantInt* conInt = dyn_cast<ConstantInt>(Instr.getOperand(1))){
        APInt c = conInt->getValue();
        //outs() << "Vi è l'immediato nell'istruzione:" << "\n";
        if(c.isOne()){
          //outs() << "Elimino:" << "\n";

          Instr.replaceAllUsesWith(Instr.getOperand(0));
          
          //Instr.eraseFromParent();
          
        }
        if(c.isPowerOf2()){
            //unsigned int cont = c.exactLogBase2(); // cambiare con exact
            //Value* op = reinterpret_cast<Value*>(cont);
            Constant *op = ConstantInt::get(conInt->getType(), c.exactLogBase2());
            Instruction *NewInst = BinaryOperator::Create(
            Instruction::LShr, Instr.getOperand(0), op);
            NewInst->insertAfter(&Instr);
            Instr.replaceAllUsesWith(NewInst);
            Iter = std::next(Iter, 1);
            //Instr.eraseFromParent();
            istruzioniDaEliminare.push_back(&Instr);

            
          }
      }
    }
  }

  outs() << "Istruzioni a cui sono state applicate delle ottimizzazioni (verranno sostituite):\n";
  for(Instruction * Instr:istruzioniDaEliminare){
    Instruction &temp = *Instr;
    outs() << temp << "\n";
    Instr->eraseFromParent();
  }

  return true;
}

/*
bool runOnBasicBlock(BasicBlock &B) {
    
    // Preleviamo le prime due istruzioni del BB
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());

    // L'indirizzo della prima istruzione deve essere uguale a quello del 
    // primo operando della seconda istruzione (per costruzione dell'esempio)
    assert(&Inst1st == Inst2nd.getOperand(0));

    // Stampa la prima istruzione
    outs() << "PRIMA ISTRUZIONE: " << Inst1st << "\n";
    // Stampa la prima istruzione come operando
    outs() << "COME OPERANDO: ";
    Inst1st.printAsOperand(outs(), false);
    outs() << "\n";

    // User-->Use-->Value
    outs() << "I MIEI OPERANDI SONO:\n";
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
      Value *Operand = *Iter;

      if (Argument *Arg = dyn_cast<Argument>(Operand)) {
        outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() 
	       <<" DELLA FUNZIONE " << Arg->getParent()->getName()
               << "\n";
      }
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
        outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue()
               << "\n";
      }
    }

    outs() << "LA LISTA DEI MIEI USERS:\n";
    for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) {
      outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
    }

    outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
    for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) {
      outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
    }


    // Manipolazione delle istruzioni
    Instruction *NewInst = BinaryOperator::Create(
        Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));

    NewInst->insertAfter(&Inst1st);
    // Si possono aggiornare le singole references separatamente? Sì
    // Controlla la documentazione e prova a rispondere.
    Inst1st.replaceAllUsesWith(NewInst);

    return true;
  }
*/

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

