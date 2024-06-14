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

using namespace llvm;

/*Stampa:
1. Il numero di Loop
2. Il PreHeader
3. Blocchi del Loop
4. Successore del Loop(Exit Block)*/

void myPrintLoop(Loop & loop, int cont){
  outs() << "\n ------------------------ Loop L" << cont << " ------------------------ \n";
  outs() << "\n" << loop << "\n -------- PreHeader -------- \n";
  
  //outs() << loop << "\n";
  //outs() << *(**L).getLoopPreheader() << "\n ---------------- \n";
  outs() << *loop.getLoopPreheader() << "\n -------- Blocchi del Loop -------- \n";

  for(auto B = loop.block_begin(); B != loop.block_end(); ++B){
    outs() << **B;
  }

  outs() << "\n -------- Successore(Exit Block) -------- \n" << *loop.getExitBlock();

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

BasicBlock * topLoopBB(Loop & loop, BasicBlock * exitBlock){
  if(loop.isGuarded()){
    outs() << "\n -------- Loop è Guarded --------- \n";
    return loop.getLoopGuardBranch()->getParent();
  }
  
  outs() << "\n -------- Loop Unguarded -------- \n";
  return loop.getLoopPreheader();
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
bool checkLoopTripCount(int TC0, int TC1){
  if(TC0 == TC1){
    return true;
  }
  return false;
}

bool checkDependence(Loop * L0, Loop & L1, DependenceInfo & DI){
  int cont = 0;
  bool check = false;
  if(L0){
    
    for(auto B0 = L0->block_begin(); B0 != L0->block_end(); ++B0){
      BasicBlock & BB0 = **B0;
      for(auto I0 = BB0.begin(); I0 != BB0.end(); ++I0){
        Instruction & Instr0 = *I0;
        for(auto B1 = L1.block_begin(); B1 != L1.block_end(); ++B1){
          BasicBlock & BB1 = **B1;
          for(auto I1 = BB1.begin(); I1 != BB1.end(); ++I1){
            Instruction & Instr1 = *I1;
            auto dep = DI.depends(&Instr0, &Instr1, true);
            if(dep){
              outs() << "\n Istruzione L0: " << Instr0 << "\n";
              outs() << "\n Istruzione L1: " << Instr1 << "\n";
              cont++;
              check = true;
            }        
          }
        }
      }
    }
  }
  outs() << "\n Numero di dipendenze:" << cont << "\n";
  return check;
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
  BasicBlock * exitBlock = NULL;
  int TC0 = -1;
  Loop * L0 = NULL;
  
  //BasicBlock * exitingBlock = NULL;

  for(auto L = LI.rbegin(); L != LI.rend(); ++L){
    Loop & loop = **L;
    
    /*Stampa informazioni inerenti al Loop*/

    myPrintLoop(loop, cont);
    
    /*Punto 1: si assume che ci sia solo un successore, ovvero un solo
    exitBlock*/

    BasicBlock * BBTopL1 = topLoopBB(loop, exitBlock);
    bool isLoopAdiacenti = checkLoopAdiacenti(exitBlock, BBTopL1);
    if(isLoopAdiacenti){
      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " sono Adiacenti -------- \n";
      //outs() << *exitBlock;
      //outs() << *BBTopL1;
    }

    /*Punto 3: si assume che ci sia solo un successore, ovvero un solo
    exitBlock*/

    bool isLoopControlFlowEquivalent = checkLoopControlFlowEquivalent(DT, PDT, BBTopL1, BBTopL0, exitBlock, cont);
    
    if(isLoopControlFlowEquivalent){
      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " sono Control Flow Equivalenti -------- \n";
    }

    /*Punto 2: si assume che i cicli FOR abbiano un numero costante di cicli e non N*/
    //exitingBlock = loop.getExitingBlock();
    int TC1 = SE.getSmallConstantTripCount(&loop);
    outs() << "\n -------- Loop Trip Count: " << TC1 << " --------- \n";
    if(checkLoopTripCount(TC0, TC1)){
      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " hanno lo stesso Trip Count (" << TC0 << ") -------- \n";
    }
    TC0 = TC1;

    /*Punto 4*/
    if(checkDependence(L0, loop, DI)){
      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " hanno delle istruzioni che dipendono tra di loro -------- \n";
    }
    L0 = &loop;
  
    /*Variabili si aggiornano solo al termine di una iterazione, 
    in modo tale da contenere le informazioni della iterazione precedente*/

    //BBTopL0 = topLoopBB(loop, exitBlock);
    BBTopL0 = BBTopL1;
    exitBlock = loop.getExitBlock();
    //exitingBlock = loop.getExitingBlock();
    
    cont++;

    //exitingBlocks.clear();
    //loop.getExitingBlocks(exitingBlocks);
  }

  outs() << "\n";

  /*for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();*/
  return PreservedAnalyses::all();
}

