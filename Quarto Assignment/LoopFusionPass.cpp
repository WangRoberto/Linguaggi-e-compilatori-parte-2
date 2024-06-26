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

#include "llvm/ADT/StringRef.h"

#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include <memory>

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

    outs() << "\n --------- ROOT: " << *DT.getRoot() << " --------- \n";

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

      outs() << "\n CI SONO!!! \n";


  const SCEV * TC0 = SE.getBackedgeTakenCount(L0);
  const SCEV * TC1 = SE.getBackedgeTakenCount(L1);

  if(TC0 == TC1){
    return true;
  }

  return false;
  
}

const SCEVAddRecExpr* convertSCEVToAddRecExpr(const SCEV* mySCEV, const Loop *L, ScalarEvolution &SE){
  SmallPtrSet<const SCEVPredicate *, 4> Predicates;
  return SE.convertSCEVToAddRecWithPredicates(mySCEV, L, Predicates);
}

const SCEV* getBaseSCEV(const SCEVAddRecExpr *addRecExpr){
  if(!addRecExpr){
    return nullptr;
  }

  const SCEVAddExpr *addExpr = dyn_cast<SCEVAddExpr>(addRecExpr->getOperand(0));
  if(!addExpr){
    return addRecExpr->getOperand(0);
  }
  return addExpr->getOperand(1);
}

const SCEVConstant* getOffsetSCEV(const SCEVAddRecExpr *addRecExpr, const Instruction *I, ScalarEvolution &SE){
  if(!addRecExpr){
    return nullptr;
  }

  const SCEVAddExpr *addExpr = dyn_cast<SCEVAddExpr>(addRecExpr->getOperand(0));
  if(!addExpr){
    int32_t N = 0;
    LLVMContext &context = I->getContext();
    IntegerType *Int32Type = IntegerType::get(context, 32);
    ConstantInt *Val = ConstantInt::get(Int32Type, N);
    const SCEV *constSCEV = SE.getConstant(Val);
    return dyn_cast<SCEVConstant>(constSCEV);
  }
  return dyn_cast<SCEVConstant>(addExpr->getOperand(0));
}

bool isDistanceNegative(std::unique_ptr<Dependence> &dep, const Loop *L0, const Loop *L1, ScalarEvolution &SE){
  outs() << "\n -------- Negative distance dependency analysis -------- \n";
  if(!dep->isFlow() && !dep->isAnti()){
    return false;
  }

  Instruction *I0 = dep->getSrc();
  Instruction *I1 = dep->getDst();
  if(!I0 || !I1){
    return true;
  }

  // Get SCEV of getelementptr
  const SCEV *I0SCEV = SE.getSCEVAtScope(I0->getOperand(dep->isFlow()), L0);
  const SCEV *I1SCEV = SE.getSCEVAtScope(I1->getOperand(dep->isAnti()), L1);

  // Check if SCEV can compute and represent them
  if(isa<SCEVCouldNotCompute>(I0SCEV) || isa<SCEVCouldNotCompute>(I1SCEV)){
    return true;
  }

  // SCEVTypes 5 == AddExpr, SCEVTypes 8 == AddRecExpr
  outs() << "I0SCEV: " << *I0SCEV << "\t" << I0SCEV->getSCEVType() << "\n";
  outs() << "I1SCEV: " << *I1SCEV << "\t" << I1SCEV->getSCEVType() << "\n";

  // Check if they are AddRecExpr
  const SCEVAddRecExpr *I0AddRecExpr = convertSCEVToAddRecExpr(I0SCEV, L0, SE);
  const SCEVAddRecExpr *I1AddRecExpr = convertSCEVToAddRecExpr(I1SCEV, L1, SE);

  if(!I0AddRecExpr || !I1AddRecExpr){
    return true;
  }
  
  outs() << "I0AddRecExpr: " << *I0AddRecExpr << "\n";
  outs() << "I1AddRecExpr: " << *I1AddRecExpr << "\n";

  // Check if they share same step (if two SCEV expressions are equivalent, they are pointer equal)
  const SCEV *I0Step = I0AddRecExpr->getOperand(1);
  const SCEV *I1Step = I1AddRecExpr->getOperand(1);

  if(I0Step != I1Step){
    return true;
  }

  // Compute starting values: base and offset
  const SCEV *I0Base = getBaseSCEV(I0AddRecExpr);
  const SCEV *I1Base = getBaseSCEV(I1AddRecExpr);
  const SCEVConstant *I0Offset = getOffsetSCEV(I0AddRecExpr, I0, SE);
  const SCEVConstant *I1Offset = getOffsetSCEV(I1AddRecExpr, I1, SE);
  outs() << "I0Base: " << *I0Base << "\n";
  outs() << "I1Base: " << *I1Base << "\n";
  outs() << "I0Offset: " << *I0Offset << "\n";
  outs() << "I1Offset: " << *I1Offset << "\n";
  if(!(I0Base && I1Base && I0Offset && I1Offset)){
    return true;
  }

  // Don't share same base
  if(I0Base != I1Base){
    return true;
  }

  // Check if distance is negative
  return SE.isKnownPredicate(ICmpInst::ICMP_SLT, I0Offset, I1Offset);
}

/*Controlla se ci sono istruzioni di L1 che dipendono da L0*/
bool checkDependence(const Loop *L0, const Loop *L1, DependenceInfo &DI, ScalarEvolution &SE){
  int cont = 0;
  bool check = false;

  if(L0){
    for(auto BB0 = L0->block_begin(); BB0 != L0->block_end(); ++BB0){
      for(auto I0 = (*BB0)->begin(); I0 != (*BB0)->end(); ++I0){
        outs() << "\n -------------------------------- \n Istruzione L0: " << *I0 << "\n -------------------------------- \n";
        for(auto BB1 = L1->block_begin(); BB1 != L1->block_end(); ++BB1){
          for(auto I1 = (*BB1)->begin(); I1 != (*BB1)->end(); ++I1){
            outs() << "\n Istruzione L1: " << *I1 << "\n";
            std::unique_ptr<Dependence> dep = DI.depends(&*I0, &*I1, true);

	    if(!dep || dep->isConfused()){
	      continue;
	    }

	    if(isDistanceNegative(dep, L0, L1, SE)){
              outs() << "\n -------- Dipendenze -------- \n";
	      dep->dump(outs());
              outs() << "\n Istruzione L0: " << *I0 << "\n";
              outs() << "\n Istruzione L1: " << *I1 << "\n";
              outs() << "\n -------------------------------- \n";
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

/*
bool matchLivello(Loop * L, int livelloLoop){
  //Idea per il caso generale, esempio in cui ci sono più 2 livelli
  SmallVector<Loop *> innerLoops;

  if(L->getLoopDepth() != livelloLoop){
    innerLoops = L->getSubLoops();

    if(!innerLoops){
      return false;
    }

    if(innerLoop.size() < 2){
      return false;
    }

    for(auto innerLoop = innerLoops.begin(); innerLoop != innerLoops.end(); ++innerLoop){
      matchLivello(innerLoop, livelloLoop);
    }

  }
  return true;
}
*/

PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {

  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);

  int cont = 0; //Numera i loop
  int contNPassi = 0;

  //SmallVector <BasicBlock *> exitingBlocks;
  
  BasicBlock * BBTopL0 = NULL;
  BasicBlock * BBTopL1;
  BasicBlock * exitBlock = NULL;
  Loop * L0 = NULL;
  Loop * loop;

  bool Transformed = true;

  int livelloLoop = 1;
  
  //BasicBlock * exitingBlock = NULL;

  while(Transformed != false){

    BBTopL0 = NULL;
    BBTopL1 = NULL;
    exitBlock = NULL;
    L0 = NULL;
    loop = NULL;
    
    outs() << "\n -------------------------------- Passo N°" << contNPassi << " -------------------------------- \n";
    Transformed = false;
    for(auto L = LI.rbegin(); L != LI.rend(); L0 = loop, BBTopL0 = BBTopL1, exitBlock = loop->getExitBlock(), cont++, ++L){
      
      if(livelloLoop == 2){

      }

      loop = *L;

      /*
      if(!matchLivello(loop, livelloLoop)){
        outs() << "\n -------- Livello: NO MATCH -------- \n";
        Transformed = false;
        continue;
      }

      outs() << "\n -------- Livello: MATCH -------- \n";
      */
      

      /*Stampa informazioni inerenti al Loop*/

      myPrintLoop(loop, cont);
      
      /*Punto 1: si assume che ci sia solo un successore, ovvero un solo
      exitBlock*/

      BBTopL1 = topLoopBB(loop, exitBlock);

      if(!checkLoopAdiacenti(exitBlock, BBTopL1)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON sono Adiacenti -------- \n";
        //outs() << *exitBlock;
        //outs() << *BBTopL1;
        //Transformed = false;
        continue;
      }

      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " sono Adiacenti -------- \n";

      /*Punto 3: si assume che ci sia solo un successore, ovvero un solo
      exitBlock*/
      
      if(!checkLoopControlFlowEquivalent(DT, PDT, BBTopL1, BBTopL0, exitBlock, cont)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON sono Control Flow Equivalenti -------- \n";
        //Transformed = false;
        continue;
      }
      
      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " sono Control Flow Equivalenti -------- \n";


      /*Punto 2: si assume che i cicli FOR abbiano un numero costante di cicli e non N*/
      //exitingBlock = loop.getExitingBlock();

      //outs() << "\n -------- Loop Trip Count: " << TC1 << " --------- \n";
      if(!checkLoopTripCount(SE, L0, loop)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON hanno lo stesso Trip Count  -------- \n";
        //Transformed = false;
        continue;
      }

      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " hanno lo stesso Trip Count  -------- \n";


      /*Punto 4*/
      if(!checkDependence(L0, loop, DI, SE)){
        outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " hanno delle istruzioni che dipendono tra di loro -------- \n";
        //Transformed = false;
        continue;
      }

      outs() << "\n -------- L" << (cont - 1) << " e L" << cont << " NON hanno delle istruzioni che dipendono tra di loro -------- \n";


      if(fuseLoops(L0, loop)){
        Transformed = true;
        //break;
      }
      
    
      /*Variabili si aggiornano solo al termine di una iterazione, 
      in modo tale da contenere le informazioni della iterazione precedente*/

      //BBTopL0 = topLoopBB(loop, exitBlock);
      
      //exitingBlock = loop.getExitingBlock();

      //exitingBlocks.clear();
      //loop.getExitingBlocks(exitingBlocks);
    }

    if(Transformed){
      outs() << "\n -------- STO PULENDO -------- \n";
      AM.clear();
     
      if(AM.empty()){
        outs() << "\n -------- VUOTO -------- \n";
      }

      outs() << "\n -------- OTTENGO I RISULTATI --------\n";

      /*Aggiorna le analisi*/
      AM.getResult<LoopAnalysis>(F);
      AM.getResult<DominatorTreeAnalysis>(F);
      AM.getResult<PostDominatorTreeAnalysis>(F);
      AM.getResult<ScalarEvolutionAnalysis>(F);
      AM.getResult<DependenceAnalysis>(F);
      
      cont = 0;
      contNPassi++;
      livelloLoop++;  
    }
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

