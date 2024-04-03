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
#include <vector>
#include <functional>
#include <cstring>

using namespace llvm;

bool optimizeSDiv(BinaryOperator &BinaryI) {
  // Check if the second operand is an immediate
  ConstantInt *Immediate = dyn_cast<ConstantInt>(BinaryI.getOperand(1));
  Value *Val = BinaryI.getOperand(0);
  if (!Immediate) {
    return false;
  }

  // Check if the second operand is a power of 2
  APInt ImmediateValue = Immediate->getValue();
  if (!ImmediateValue.isPowerOf2()) {
    return false;
  }

  // Create lshr instruction
  int32_t N = ImmediateValue.exactLogBase2();
  ConstantInt *Shifts = ConstantInt::get(Immediate->getType(), N);

  Instruction *NewInst = BinaryOperator::Create(
  BinaryOperator::LShr, Val, Shifts);
  
  // Insert new instruction
  NewInst->insertAfter(&BinaryI);
  BinaryI.replaceAllUsesWith(NewInst);

  outs() << BinaryI << " has been replaced by a lshr instruction (strength reduction)\n";
  return true;
}

bool optimizeMul(BinaryOperator &BinaryI) {
  // Check if there are immediate operands powers of 2 or "almost" power of 2
  int32_t diff = 1;
  for (unsigned i = 0; i < 3; ++i, diff = (diff+1)%3) {
    for (unsigned j = 0; j != BinaryI.getNumOperands(); ++j) {
      ConstantInt *Immediate = dyn_cast<ConstantInt>(BinaryI.getOperand(j));
      if (!Immediate) {
        continue;
      }
  
      // Check if it is power of 2 or "almost" power of 2
      APInt ImmediateValue = Immediate->getValue();
      ImmediateValue += (diff-1);
      if (!ImmediateValue.isPowerOf2()) {
        continue;
      }
  
      Value* Val = BinaryI.getOperand((j+1)%(BinaryI.getNumOperands()));

      // Create shl instruction
      int32_t N = ImmediateValue.exactLogBase2();
      ConstantInt *Shifts = ConstantInt::get(Immediate->getType(), N);
  
      Instruction *NewInst = BinaryOperator::Create(BinaryOperator::Shl, Val, Shifts);
  
      // Insert shl instruction
      NewInst->insertAfter(&BinaryI);

      // Insert eventual add or sub instruction
      std::string str = "";
      if ((diff-1) == 1) {
        Instruction &prev = *NewInst;
	NewInst = BinaryOperator::Create(BinaryOperator::Sub, &prev, Val);
	NewInst->insertAfter(&prev);
	str = " and a sub";
      } else if ((diff-1) == -1) {
        Instruction &prev = *NewInst;
	NewInst = BinaryOperator::Create(BinaryOperator::Add, &prev, Val);
	NewInst->insertAfter(&prev);
	str = " and an add";
      }
      BinaryI.replaceAllUsesWith(NewInst);
  
      outs() << BinaryI << " has been replaced by a shl" << str << " instruction (strength reduction)\n";
  
      return true;
    }
  }
  return false;
}

bool runOnBasicBlockStrengthReduction(BasicBlock &B) {
  bool Transformed = false;
  std::vector<Instruction*> toErase;

  for (auto Iter = B.begin(); Iter != B.end(); ++Iter) {
    Instruction &I = *Iter;
    // Check if the instruction is a BinaryOperator
    BinaryOperator *BinaryI = dyn_cast<BinaryOperator>(&I);
    if (!BinaryI) {
      continue;
    }

    // Check if the instruction in a mul or sdiv
    if (BinaryI->getOpcode() != Instruction::Mul && BinaryI->getOpcode() != Instruction::SDiv) {
      continue;
    }

    // Optimize the instruction
    bool optimized = false;
    if (BinaryI->getOpcode() != Instruction::Mul) {
      optimized = optimizeSDiv(*BinaryI);
    } else {
      optimized = optimizeMul(*BinaryI);
    }

    // Add old instruction to vector of instructions to be erased
    if (optimized) {
      toErase.push_back(&I);
      Transformed = true;
    }
  }

  // Erase old instructions
  for (auto Iter = toErase.begin(); Iter != toErase.end(); ++Iter) {
    Instruction &InstToErase = **Iter;
    InstToErase.eraseFromParent();
  }

  return Transformed;
}

bool runOnBasicBlockAlgebraicIdentity(BasicBlock &B) {
  bool Transformed = false;
  std::vector<Instruction*> toErase;

  for (auto Iter = B.begin(); Iter != B.end(); ++Iter) {
    Instruction &I = *Iter;
    // Check if the instruction is a BinaryOperator
    BinaryOperator *BinaryI = dyn_cast<BinaryOperator>(&I);
    if (!BinaryI) {
      continue;
    }
    
    // Check if the instruction is an add or a mul
    int32_t Identity;
    if (BinaryI->getOpcode() == Instruction::Add) {
      Identity = 0;
    } else if (BinaryI->getOpcode() == Instruction::Mul) {
      Identity = 1;
    } else {
      continue;
    }

    // Check if it is an algebraic identity
    Value *Val = nullptr;
    for (unsigned i = 0; i < BinaryI->getNumOperands(); ++i) {
      // Check if operand is an immediate
      ConstantInt *Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(i));
      if (!Immediate) {
        continue;
      }

      // Check if immediate is an identity
      APInt ImmediateValue = Immediate->getValue();
      if (ImmediateValue != Identity) {
        continue;
      }

      Val = BinaryI->getOperand((i+1)%(BinaryI->getNumOperands()));
      break;
    }
      
    if (!Val) {
      continue;
    }

    // Replace the uses of the instruction with its operand
    I.replaceAllUsesWith(Val);

    // Add algebraic identities to vector of instructions to be erased
    toErase.push_back(&I);

    outs() << *BinaryI << " has been erased (algebraic identity)\n";
    Transformed = true;
  }

  // Erase algebraic identities
  for (auto Iter = toErase.begin(); Iter != toErase.end(); ++Iter) {
    Instruction &InstToErase = **Iter;
    InstToErase.eraseFromParent();
  }

  return Transformed;
}

bool runOnFunction(Function &F, std::function<bool(BasicBlock&)> runOnBasicBlock) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses StrengthReduction::run(Module &M, ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter, runOnBasicBlockStrengthReduction))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}

PreservedAnalyses AlgebraicIdentity::run(Module &M, ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter, runOnBasicBlockAlgebraicIdentity))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}
