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

using namespace llvm;


bool runOnBasicBlock(BasicBlock &B){
  for(Instruction &Instr : B){
  //for(auto &Instr = B.begin(); Instr != B.end(); ++Instr){

    int nonConstante;

    //First optimization
    if(Instr.getOpcode() == Instruction::Add){
      for(unsigned int i = 0; i < Instr.getNumOperands(); i++){
        nonConstante = 0;
        if(ConstantInt* conInt = dyn_cast<ConstantInt>(Instr.getOperand(1))){
            APInt c = conInt->getValue();
            if(i == 0){
              nonConstante = 1;
            }
            if(c.isZero()){
              Instruction *NewInst = Instr.getOperand(nonConstante);
              NewInst->insertAfter(&Instr);
              Instr.replaceAllUsesWith(NewInst);
              return true;
          }
        }
      }
    }

    //Second optimization
    //In caso sia un operatore binario
      /*
      if(dyn_cast<BinaryOperator>(&Instr)){
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
      for(unsigned int i = 0; i < Instr.getNumOperands(); i++){//itera gli operandi per vedere se c'è una costante
        nonConstante = 0;
        if(ConstantInt* conInt = dyn_cast<ConstantInt>(Instr.getOperand(i))){
          APInt c = conInt->getValue();
          if(i == 0){
            nonConstante = 1;
          }
          if(c.isOne()){
            Instruction *NewInst = Instr.getOperand(nonConstante);
            NewInst->insertAfter(&Instr);
            Instr.replaceAllUsesWith(NewInst);
            return true;
          }
          if(c.isPowerOf2()){
            unsigned int cont = c.exactLogBase2(); // cambiare con exact
            Value* op = reinterpret_cast<Value*>(cont);
            //Constant *op = ConstantInt::get(conInt->getType(), conInt->getValue().exactLogBase2());
            Instruction *NewInst = BinaryOperator::Create(
            Instruction::Shl, Instr.getOperand(nonConstante), op);
            NewInst->insertAfter(&Instr);
            Instr.replaceAllUsesWith(NewInst);
            return true;
          }
        }
      }
    }
  }
  return false;
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
    // Si possono aggiornare le singole references separatamente?
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

