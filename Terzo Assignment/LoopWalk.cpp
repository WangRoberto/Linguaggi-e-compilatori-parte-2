#include "llvm/Transforms/Utils/LoopWalk.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Dominators.h"
#include <vector>
using namespace llvm;

bool isLoopInvariant(Loop const &L, Instruction &I, std::vector<Instruction*> const &LoopInvariantInstructions) {
  if (I.isTerminator()) {
    return false;
  }

  if (I.getOpcode() == Instruction::PHI) {
    return false;
  }

  for (auto Iter = I.op_begin(); Iter != I.op_end(); ++Iter) {
    Value *Operand = *Iter;

    // Check if operand is constant
    ConstantInt *Immediate = dyn_cast<ConstantInt>(Operand);
    if (Immediate) {
      continue;
    }

    // Check if reaching definition of operand is outside the loop (could also be an Argument)
    Instruction *Def = dyn_cast<Instruction>(Operand);
    if (!Def || !L.contains(Def)) {
      continue;
    }

    // Check if operand is loop invariant
    if (std::find(LoopInvariantInstructions.begin(), LoopInvariantInstructions.end(), Def) != LoopInvariantInstructions.end()) {
      continue;
    }

    return false;
  }

  return true;
}

void findLoopInvariantInstructions(Loop const &L, std::vector<Instruction*> &LoopInvariantInstructions) {
  for (Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
    BasicBlock &BB = **BI;
    for (auto I = BB.begin(); I != BB.end(); ++I) {
      if (isLoopInvariant(L, *I, LoopInvariantInstructions)) {
        LoopInvariantInstructions.push_back(&*I);
      }
    }
  }
}

void findCodeMotionInstructions(Loop const &L, LoopStandardAnalysisResults &AR, std::vector<Instruction*> const &LoopInvariantInstructions, std::vector<Instruction*> &CodeMotionInstructions) {
  SmallVector<BasicBlock*> ExitingBlocks;
  L.getExitingBlocks(ExitingBlocks);
  DominatorTree &DT = AR.DT;

  for (auto &I : LoopInvariantInstructions) {
    // Check if instruction is dead code
    if (I->getNumUses() == 0) {
      I->eraseFromParent();
      continue;
    }
    
    // Check if instruction is dead outside the loop
    bool isDead = true;
    for (auto Iter = I->user_begin(); Iter != I->user_end(); ++Iter) {
      User *InstUser = *Iter;
      Instruction *Inst = dyn_cast<Instruction>(InstUser);
      BasicBlock *BB = Inst->getParent();

      bool isInsideLoop = false;
      for (Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
        if (BB == *BI) {
          isInsideLoop = true;
	  break;
	}
      }

      if (!isInsideLoop) {
        isDead = false;
	break;
      }
    }

    if (isDead) {
      CodeMotionInstructions.push_back(I);
      continue;
    }

    // If instruction is not dead, check if instruction dominates all exits
    bool dominatesAllExits = true;
    for (auto &BB : ExitingBlocks) {
      if (I->getParent() != BB && !DT.dominates(I, BB)) {
	dominatesAllExits = false;
        break;
      }
    }

    if (dominatesAllExits) {
      CodeMotionInstructions.push_back(I);
    }
  }
}

PreservedAnalyses LoopWalk::run(Loop &L, LoopAnalysisManager &AM, LoopStandardAnalysisResults &AR, LPMUpdater &U) {
  outs() << "\n---------- PROGRAM CFG ----------\n";
  BasicBlock *Head = L.getHeader();
  Function *F = Head->getParent();
  for (auto &BB : *F) {
    outs() << BB;
  }
  
  outs() << "\n---------- LOOP CFG ----------\n";
  for (Loop::block_iterator BI = L.block_begin(); BI != L.block_end(); ++BI) {
    outs() << **BI;
  }

  outs() << "\n---------- LOOP-INVARIANT INSTRUCTIONS ----------\n\n";
  std::vector<Instruction*> LoopInvariantInstructions;
  findLoopInvariantInstructions(L, LoopInvariantInstructions);
  for (auto &I : LoopInvariantInstructions) {
    outs() << *I << "\n";
  }

  outs() << "\n---------- CODE MOTION CANDIDATE INSTRUCTIONS ----------\n\n";
  std::vector<Instruction*> CodeMotionInstructions;
  findCodeMotionInstructions(L, AR, LoopInvariantInstructions, CodeMotionInstructions);
  for (auto &I : CodeMotionInstructions) {
    outs() << *I << "\n";
  }

  outs() << "\n---------- LOOP CFG AFTER LICM ----------\n";
  BasicBlock *Preheader = L.getLoopPreheader();
  Instruction &PreheaderLastI = *(Preheader->rbegin());
  bool Transformed = false;
  for (auto &I : CodeMotionInstructions) {
    Transformed = true;
    I->moveBefore(&PreheaderLastI);
  }

  for (auto &BB : *F) {
    outs() << BB;
  }

  if (!Transformed) {
    return PreservedAnalyses::none();
  }
  return PreservedAnalyses::all();
}
