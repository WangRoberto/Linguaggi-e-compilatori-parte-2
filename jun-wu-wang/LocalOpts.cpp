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

using namespace llvm;

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
  
    // Check if the instruction is a mul
    if (BinaryI->getOpcode() != Instruction::Mul) {
      continue;
    }
    
    // Check if there is and eventually which operand is an immediate
    ConstantInt *Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(0));
    Value *Val = BinaryI->getOperand(1);
    if (!Immediate) {
      Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(1));
      Val = BinaryI->getOperand(0);
      if (!Immediate) {
	continue;
      }
    }
  
    // Check if immediate is a power of 2
    APInt ImmediateValue = Immediate->getValue();
    if (!ImmediateValue.isPowerOf2()) {
      continue;
    }
  
    // Create shl instruction
    int32_t N = ImmediateValue.exactLogBase2();
    ConstantInt *Shifts = ConstantInt::get(Immediate->getType(), N);
  
    Instruction *NewInst = BinaryOperator::Create(
    BinaryOperator::Shl, Val, Shifts);
    
    // Insert new instruction
    NewInst->insertAfter(&I);
    I.replaceAllUsesWith(NewInst);

    // Add old instruction to vector of instructions to be erased
    toErase.push_back(&I);

    outs() << *BinaryI << " has been replaced by a shl instruction (strength reduction)\n";
    Transformed = true;
  }

  // Erase old instructions
  for (auto it = toErase.rbegin(); it != toErase.rend(); ++it) {
    Instruction &InstToErase = **it;
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

    // Check if there is and eventually which operand is an immediate
    ConstantInt *Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(0));
    Value *Val = BinaryI->getOperand(1);
    if (!Immediate) {
      Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(1));
      Val = BinaryI->getOperand(0);
      if (!Immediate) {
	continue;
      }
    }

    // Check if the instruction is an add or a mul, and also an algebraic identity
    APInt ImmediateValue = Immediate->getValue();
    if (!(BinaryI->getOpcode() == Instruction::Add && ImmediateValue == 0) && !(BinaryI->getOpcode() == Instruction::Mul && ImmediateValue == 1)) {      
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
  for (auto it = toErase.rbegin(); it != toErase.rend(); ++it) {
    Instruction &InstToErase = **it;
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
