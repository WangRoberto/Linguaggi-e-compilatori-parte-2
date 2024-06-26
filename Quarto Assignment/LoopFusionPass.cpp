//===-- LoopFusionPass.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/LoopFusionPass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/GenericLoopInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

//#include "llvm/Analysis/ScalarEvolutionExpressions.h"

//#include <memory>

using namespace llvm;

/*Stampa:
1. Il numero di Loop
2. Il PreHeader
3. Blocchi del Loop
4. Successore del Loop(Exit Block)*/

void myPrintLoop(Loop * loop, int cont){
  outs() << "\n ------------------------ Loop L" << cont << " ------------------------ \n";
  outs() << "\n" << *loop << "\n -------- PreHeader -------- \n";
  
  //outs() << loop << "\n";
  //outs() << *(**L).getLoopPreheader() << "\n ---------------- \n";
  
  if(loop->getLoopPreheader()){
    outs() << *loop->getLoopPreheader() << "\n -------- Blocchi del Loop -------- \n";
  }

  for(auto B = loop->block_begin(); B != loop->block_end(); ++B){
    outs() << **B;
  }

  outs() << "\n -------- Successore(Exit Block) -------- \n" << *loop->getExitBlock();

}

/*Controlla se i Loop sono adiacenti:
1. Confronta il successore del L0 con il PreHeader/Guardia di L1*/

bool checkLoopAdiacenti(BasicBlock * successoreLoop0, BasicBlock * BBTopL1){
  if(successoreLoop0 == BBTopL1){
    return true;
  }
  return false;
}

/*Si ottiene il BasicBlock della Guardia oppure quello del PreHeader*/

BasicBlock * topLoopBB(Loop * loop, BasicBlock * exitBlock){
  if(loop->isGuarded()){
    outs() << "\n -------- Loop è Guarded --------- \n";
    return loop->getLoopGuardBranch()->getParent();
  }
  
  outs() << "\n -------- Loop Unguarded -------- \n";
  return loop->getLoopPreheader();
}

/*Controlla se L0 e L1 sono Control Flow Equivalenti:
1. L0 Domina L1
2. L1 PostDomina L0*/

bool checkLoopControlFlowEquivalent(DominatorTree & DT, PostDominatorTree & PDT, BasicBlock * BBTopL1, BasicBlock * BBTopL0, BasicBlock * exitBlock, int cont){
  bool isDominated = false;
  bool isPostDominated = false;

  if(BBTopL0){
    Instruction & I0 = *BBTopL0->begin();
    if(DT.dominates(&I0, BBTopL1)){
      isDominated = true;
    }

    Instruction & I1 = *BBTopL1->begin();

    if(PDT.dominates(&I1, &I0)){
      isPostDominated = true;
    }

    outs() << "\n L0 Domina L1? " << (isDominated? "True\n" : "False\n");
    outs() << "\n L1 PostDomina L0? " << (isPostDominated? "True\n" : "False\n");
  }

  return isDominated & isPostDominated;
}

/*Controlla che il Trip Count di L0 e L1 siano uguali*/
bool checkLoopTripCount(ScalarEvolution & SE, Loop * L0, Loop * L1){
  if(!L0 || !L1){
    return false;
  }

  const SCEV * TC0 = SE.getBackedgeTakenCount(L0);
  const SCEV * TC1 = SE.getBackedgeTakenCount(L1);

  if(TC0 == TC1){
    return true;
  }

  return false;
  
}

/*Controlla se ci sono istruzioni di L1 che dipendono da L0*/
bool checkDependence(Loop * L0, Loop * L1, DependenceInfo & DI){
  int cont = 0;
  bool check = false;

  if(!L0 || !L1){
    return false;
  }


  for(auto B0 = L0->block_begin(); B0 != L0->block_end(); ++B0){
    BasicBlock & BB0 = **B0;
    
    for(auto I0 = BB0.begin(); I0 != BB0.end(); ++I0){
      Instruction & Instr0 = *I0;
      outs() << "\n -------------------------------- \n Istruzione L0: " << Instr0 << "\n -------------------------------- \n";
      
      for(auto B1 = L1->block_begin(); B1 != L1->block_end(); ++B1){
        BasicBlock & BB1 = **B1;
        
        for(auto I1 = BB1.begin(); I1 != BB1.end(); ++I1){
          Instruction & Instr1 = *I1;
          //outs() << "\n Istruzione L1: " << Instr1 << "\n";
          auto dep = DI.depends(&Instr0, &Instr1, true);
          
          /*if(dep){
            bool isNegativeDependence = false;
            for(unsigned i = 1; i <= dep->getLevels(); i++){
              if(dep->getDirection(i) == Dependence::DVEntry::NEGATIVE){
                isNegativeDependence =true;
                break;
              }
            }
          }*/

          /*if(!dep){
            continue;
          }*/

          /*
          if(dep){
            outs() << "\n -------- Livello: " << dep->getLevels() << " --------- \n";
            for(unsigned Level = 0; Level <= dep->getLevels(); ++Level){
              const SCEV *Distance = dep->getDistance(Level);
              if(Distance && !isa <SCEVCouldNotCompute> (Distance)){
              const auto * ConstDist = dyn_cast<SCEVConstant>(Distance);
              
                if(ConstDist && ConstDist->getValue()->isNegative()){
                  cont ++;
                  check = true;
                } 
              }
            }
          }*/
          
          /*
          if(dep){   
            outs() << "\n -------- Dipendenze -------- \n";
            dep->dump(outs());
            outs() << "\n Istruzione L0: " << Instr0 << "\n";
            outs() << "\n Istruzione L1: " << Instr1 << "\n";
            outs() << "\n -------------------------------- \n";           
            outs() << "\n -------- Livello: " << dep->getLevels() << " -------- \n";
            cont++;
          }*/

          /*
          if(dep->isConfused()){
            //check = true;
            return true;
          }*/
          
          if(dep && !dep->isConfused() && dep->isOrdered()){

              outs() << "\n -------- Dipendenze -------- \n";
              dep->dump(outs());
              outs() << "\n Istruzione L0: " << Instr0 << " Livello: " << L0->getLoopDepth() << "\n";
              outs() << "\n Livelli: " << dep->getLevels() << "\n";
              outs() << "\n Istruzione L1: " << Instr1 << " Livello: " << L1->getLoopDepth() << "\n";
              outs() << "\n -------------------------------- \n";
              
              auto fullDep = FullDependence(&Instr0, &Instr1, true, 1);
              outs() << "\n Livelli: " << fullDep.getLevels() << "\n";
              const SCEV * D = fullDep.getDistance(2);

              if(fullDep.isDirectionNegative()){
                outs() << "\n Direzione: " << fullDep.getDirection(fullDep.getLevels()) << "\n";
              }

              if(D){
                outs() << "\n Distanza: " << D << "\n";
              }

              cont++;
              check = true;
            
          }
        }
      }
    }
  }
  
  
  outs() << "\n Numero di dipendenze: " << cont << "\n";
  return check;
}

bool fuseLoops(Loop * L0, Loop * L1){

  if(!L0 || !L1){
    return false;
  }

  BasicBlock * latch0 = L0->getLoopLatch();
  BasicBlock * latch1 = L1->getLoopLatch();
  //BasicBlock * exitingBlock0 = L0->getExitingBlock();
  //BasicBlock * exitingBlock1 = L1->getExitingBlock();
  BasicBlock * header0 = L0->getHeader();
  BasicBlock * header1 = L1->getHeader();
  
  //BasicBlock * exitBlock0 = L0->getExitBlock();
  BasicBlock * exitBlock1 = L1->getExitBlock();

  BasicBlock * preHeader1 = L1->getLoopPreheader();

  //1: Header0 --> Exit1
  //outs() << "\n -------- L0 Latch -------- \n" << *latch0;
  //outs() << "\n -------- L1 Latch -------- \n" << *latch1;

  Instruction & lastHeader0 = *header0->rbegin();
  outs() << "\n -------- Last Instruction Header0 (before) -------- \n" << lastHeader0 << "\n";
  lastHeader0.setOperand(1, exitBlock1);
  outs() << "\n -------- Last Instruction Header0 (after) -------- \n" << lastHeader0 << "\n";

  //2: Body0 --> Body1 --> Latch0
  Instruction & lastBody0 = *(latch0->getSinglePredecessor()->rbegin());
  outs() << "\n -------- Last Instruction Body0 (before) -------- \n" << lastBody0 << "\n";
  Instruction & lastHeader1 = *header1->rbegin();
  //outs() << "\n -------- Operand Header1 -------- \n" << *lastHeader1.getOperand(2) << "\n";
  lastBody0.setOperand(0, lastHeader1.getOperand(2));
  Instruction & firstHeader0 = *header0->begin();
  Instruction & firstHeader1 = *header1->begin();
  firstHeader1.replaceAllUsesWith(&firstHeader0);
  outs() << "\n -------- Last Instruction Body0 (after) -------- \n" << lastBody0 << "\n";
  
  Instruction & lastBody1 = *(latch1->getSinglePredecessor()->rbegin());
  lastBody1.setOperand(0, latch0);

  //3: Header1 --> Latch1
  outs() << "\n -------- Last Instruction Header1 (before) -------- \n" << lastHeader1 << "\n";
  lastHeader1.setOperand(1, latch1);
  outs() << "\n -------- Last Instruction Header1 (after) -------- \n" << lastHeader1 << "\n";

  //Eliminazione dei basicblock non legati al resto
  outs() << "\n -------- Last Instruction PreHeader1 -------- \n" << *preHeader1->rbegin() << "\n";

  outs() << "\n -------- Last Instruction Header1 -------- \n" << *header1->rbegin() << "\n";

  outs() << "\n -------- Last Instruction Latch1 -------- \n" << *latch1->rbegin() << "\n";
  
  /*
  SmallVector <BasicBlock *> needToEraseBB;
  needToEraseBB.push_back(preHeader1);
  needToEraseBB.push_back(header1);
  needToEraseBB.push_back(latch1);
  SmallVector <Instruction *> needToEraseI;

  for(auto B = needToEraseBB.begin(); B != needToEraseBB.end(); ++B){
    BasicBlock & BB = **B;
    for(auto I = BB.begin(); I != BB.end(); ++I){
      //Instruction * Instr = **I;
      needToEraseI.push_back(&*I);
    }
  }

  for(auto I = needToEraseI.begin(); I != needToEraseI.end(); ++I){
    (*I)->eraseFromParent();
  }
  */

  preHeader1->rbegin()->eraseFromParent();
  header1->rbegin()->eraseFromParent();
  latch1->rbegin()->eraseFromParent();

  preHeader1->eraseFromParent();
  header1->eraseFromParent();
  latch1->eraseFromParent();


  /*
  DeleteDeadBlock(preHeader1, nullptr, false);
  DeleteDeadBlock(header1, nullptr, false);
  DeleteDeadBlock(latch1, nullptr, false);*/

  return true;
}



PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {

  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

  DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

  int cont = 0; //Numera i loop

  //SmallVector <BasicBlock *> exitingBlocks;
  
  BasicBlock * BBTopL0 = NULL;
  BasicBlock * BBTopL1;
  BasicBlock * exitBlock = NULL;
  Loop * L0 = NULL;
  Loop * loop;

  bool Transformed = true;
  
  //BasicBlock * exitingBlock = NULL;

  while(Transformed != false){
    
    outs() << "\nOK\n";
    //Transformed = false;
    for(auto L = LI.rbegin(); L != LI.rend(); L0 = loop, BBTopL0 = BBTopL1, exitBlock = loop->getExitBlock(), cont++, ++L){
      
      loop = *L;
      /*Stampa informazioni inerenti al Loop*/

      myPrintLoop(loop, cont);
      
      /*Punto 1: si assume che ci sia solo un successore, ovvero un solo
      exitBlock*/

      BBTopL1 = topLoopBB(loop, exitBlock);

      if(!checkLoopAdiacenti(exitBlock, BBTopL1)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON sono Adiacenti -------- \n";
        //outs() << *exitBlock;
        //outs() << *BBTopL1;
        Transformed = false;
        continue;
      }

      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " sono Adiacenti -------- \n";

      /*Punto 3: si assume che ci sia solo un successore, ovvero un solo
      exitBlock*/
      
      if(!checkLoopControlFlowEquivalent(DT, PDT, BBTopL1, BBTopL0, exitBlock, cont)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON sono Control Flow Equivalenti -------- \n";
        Transformed = false;
        continue;
      }
      
      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " sono Control Flow Equivalenti -------- \n";


      /*Punto 2: si assume che i cicli FOR abbiano un numero costante di cicli e non N*/
      //exitingBlock = loop.getExitingBlock();

      //outs() << "\n -------- Loop Trip Count: " << TC1 << " --------- \n";
      if(!checkLoopTripCount(SE, L0, loop)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON hanno lo stesso Trip Count  -------- \n";
        Transformed = false;
        continue;
      }

      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " hanno lo stesso Trip Count  -------- \n";


      /*Punto 4*/
      if(checkDependence(L0, loop, DI)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " hanno delle istruzioni che dipendono tra di loro -------- \n";
        Transformed = false;
        continue;
      }

      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON hanno delle istruzioni che dipendono tra di loro -------- \n";


      if(fuseLoops(L0, loop)){
        Transformed = true;
        AM.clear();
        cont = 0;
      }
      
    
      /*Variabili si aggiornano solo al termine di una iterazione, 
      in modo tale da contenere le informazioni della iterazione precedente*/

      //BBTopL0 = topLoopBB(loop, exitBlock);
      
      //exitingBlock = loop.getExitingBlock();

      //exitingBlocks.clear();
      //loop.getExitingBlocks(exitingBlocks);
    }

    AM.getResult<LoopAnalysis>(F);
  }

  outs() << "\n";
  
  /*
  outs() << "\n---------- PROGRAM CFG ----------\n";
  for (auto &BB : F) {
    outs() << BB;
  }
  */

  //LoopInfo & LI = LoopAnalysis::run(F, AM);
  //AM.clear();
/*
  cont = 0;
  myPrintLoop(L0, cont);
*/
  /*AM.getResult<LoopAnalysis>(F);
  
  cont = 0;

  for(auto L = LI.rbegin(); L != LI.rend(); ++L){
    Loop * loop = *L;
    
    //Stampa informazioni inerenti al Loop

    myPrintLoop(loop, cont);
    cont++;
  }
  */

  if(Transformed){
    return PreservedAnalyses::none();
  }
  

  /*for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();*/
  return PreservedAnalyses::all();
}

