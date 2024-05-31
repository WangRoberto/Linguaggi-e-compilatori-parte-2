//===-- LocalOpts.cpp - Example Transformations --------------------------===//
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
3. Blocchi del Loop*/

void myPrintLoop(Loop & loop, int cont){
  outs() << "\n ------------------------ Loop n°" << cont << " ------------------------ \n";
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
    //outs() << "\n -------- Loop è Guarded --------- \n";
    return loop.getLoopGuardBranch()->getParent();
  }
  
  //outs() << "\n -------- Loop Unguarded -------- \n";
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

    outs() << "\n L0 domina L1? " << (isDominated? "True\n" : "False\n");
    outs() << "\n L1 postdomina L0? " << (isPostDominated? "True\n" : "False\n");
  }

  return isDominated & isPostDominated;
}

PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {

  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree & DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree & PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

  int cont = 1; //Numera i loop

  //SmallVector <BasicBlock *> exitingBlocks;
  
  BasicBlock * BBTopL0 = NULL;
  BasicBlock * exitBlock = NULL;
  //BasicBlock * exitingBlock = NULL;

  for(auto L = LI.rbegin(); L != LI.rend(); ++L){
    Loop &loop = **L;
    
    /*Stampa dei blocchi del Loop*/

    myPrintLoop(loop, cont);
    
    /*Punto 1: si assume che ci sia solo un successore, ovvero un solo
    exitBlock*/

    BasicBlock * BBTopL1 = topLoopBB(loop, exitBlock);
    bool isLoopAdiacenti = checkLoopAdiacenti(exitBlock, BBTopL1);
    if(isLoopAdiacenti){
      outs() << "\n -------- Loop Adiecenti -------- \n";
      //outs() << *exitBlock;
      //outs() << *BBTopL1;
    }

    /*Punto 3: si assume che ci sia solo un successore, ovvero un solo
    exitBlock*/

    bool isLoopControlFlowEquivalent = checkLoopControlFlowEquivalent(DT, PDT, BBTopL1, BBTopL0, exitBlock, cont);
    
    if(isLoopControlFlowEquivalent){
      outs() << "\n -------- Loop n°" << (cont - 1) << " e Loop n°" << cont << " sono control flow equivalenti -------- \n";
    }

    /*Variabili si aggiornano solo al termine di una iterazione, 
    in modo tale da contenere le informazioni della iterazione precedente*/

    BBTopL0 = topLoopBB(loop, exitBlock);
    exitBlock = loop.getExitBlock();
    //exitingBlock = loop.getExitingBlock();
    
    cont++;

    //exitingBlocks.clear();
    //loop.getExitingBlocks(exitingBlocks);
  }
  
  /*for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();*/
  return PreservedAnalyses::all();
}

