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

void myPrintLoop(Loop & loop, int cont){
  outs() << "\n -------- Loop n°" << cont << " -------- \n";
  outs() << "\n" << loop << "\n -------- PreHeader -------- \n";
  
  //outs() << loop << "\n";
  //outs() << *(**L).getLoopPreheader() << "\n ---------------- \n";
  outs() << *loop.getLoopPreheader() << "\n -------- Blocchi del Loop -------- \n";

  for(auto B = loop.block_begin(); B != loop.block_end(); ++B){
    outs() << **B;
  }
}

BasicBlock * checkLoopAdiacenti(BasicBlock * successoreLoop1, BasicBlock * BBloop2){
  if(successoreLoop1 == BBloop2){
    outs() << "\n -------- Loop Adiecenti -------- \n";
  }
  else{
    outs() << "\n -------- Loop non Adiacenti -------- \n";
  }
  return BBloop2;
}

BasicBlock * topLoopBB(Loop & loop, BasicBlock * exitBlock){
  BasicBlock * BBTop;
  if(loop.isGuarded()){
    outs() << "\n -------- Loop è Guarded --------- \n";
    BasicBlock * BBGuard = loop.getLoopGuardBranch()->getParent();
    BBTop = checkLoopAdiacenti(exitBlock, BBGuard);
  }
  else{
    outs() << "\n -------- Loop Unguarded -------- \n";
    BasicBlock * BBPreHeader = loop.getLoopPreheader();
    BBTop = checkLoopAdiacenti(exitBlock, BBPreHeader);
  }
  return BBTop;
}

void checkLoopControlFlowEquivalenti(DominatorTree & DT, PostDominatorTree & PDT, BasicBlock * BBTop, BasicBlock * BBPreHeaderPrecedente, BasicBlock * exitBlock, int cont){
  bool isDominated = false;

  if(BBPreHeaderPrecedente){
    for(auto & I : *BBPreHeaderPrecedente){
      if(DT.dominates(&I, BBTop)){
        isDominated = true;
        break;
      }
    }
  }

  bool isPostDominated = false;
  
  for(auto & I: *BBTop){
    if(exitBlock){
      for(auto & I2: *exitBlock){
        if(PDT.dominates(&I, &I2)){
          isPostDominated = true;
          break;
        }
      }
    }
  }

  outs() << "\n L0 domina L1? " << (isDominated? "True\n" : "False\n");
  outs() << "\n L1 postdomina L0? " << (isPostDominated? "True\n" : "False\n");

  if(isDominated && isPostDominated){
    outs() << "\n -------- Loop n°" << (cont - 1) << " e Loop n°" << cont << " sono control flow equivalenti -------- \n";
  }
}

PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {

  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree & DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree & PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

  int cont = 1; //Numera i loop

  //SmallVector <BasicBlock *> exitingBlocks;
  
  BasicBlock * BBPreHeaderPrecedente = NULL;
  BasicBlock * exitBlock = NULL;
  //BasicBlock * exitingBlock = NULL;

  for(auto L = LI.rbegin(); L != LI.rend(); ++L){
    Loop &loop = **L;
    
    /*Stampa dei blocchi del Loop*/

    myPrintLoop(loop, cont);
    
    /*Punto 1: si assume che ci sia solo un successore, ovvero un solo
    exitBlock*/

    BasicBlock * BBTop = topLoopBB(loop, exitBlock);

    /*Punto 3: si assume che ci sia solo un successore, ovvero un solo
    exitBlock*/

    checkLoopControlFlowEquivalenti(DT, PDT, BBTop, BBPreHeaderPrecedente, exitBlock, cont);
    
    /*Variabili si aggiornano solo al termine di una iterazione, 
    in modo tale da contenere le informazioni della iterazione precedente*/

    BBPreHeaderPrecedente = loop.getLoopPreheader();
    //exitingBlock = loop.getExitingBlock();
    exitBlock = loop.getExitBlock();
    
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

