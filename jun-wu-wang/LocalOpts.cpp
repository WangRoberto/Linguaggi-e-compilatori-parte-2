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

using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
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
    outs() << *BinaryI << ":\n";
    
    // Check if there is and eventually which operand is an immediate
    ConstantInt *Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(0));
    Value *Val = BinaryI->getOperand(1);
    if (!Immediate) {
      Immediate = dyn_cast<ConstantInt>(BinaryI->getOperand(1));
      Val = BinaryI->getOperand(0);
      if (!Immediate) {
        outs() << "\thas no immediate\n";
	continue;
      }
    }
  
    // Check if immediate is a power of 2
    APInt ImmediateValue = Immediate->getValue();
    if (!ImmediateValue.isPowerOf2()) {
      outs() << "\thas no immediate that is power of 2\n";
      continue;
    }
    outs() << "\thas immediate that is power of 2\n";
  
    // Create shl instruction
    int32_t N = ImmediateValue.exactLogBase2();
    LLVMContext &context = I.getContext(); 
    IntegerType *Int32Type = IntegerType::get(context, 32);
    ConstantInt *Shifts = ConstantInt::get(Int32Type, N);
  
    Instruction *NewInst = BinaryOperator::Create(
    BinaryOperator::Shl, Val, Shifts);
  
    // Insert new instruction
    NewInst->insertAfter(&I);
    I.replaceAllUsesWith(NewInst);

    // Add old instruction to vector of instructions to be erased
    toErase.push_back(&I);

    Transformed = true;
  }

  // Erase old instructions
  for (auto it = toErase.rbegin(); it != toErase.rend(); ++it) {
    Instruction &InstToErase = **it;
    InstToErase.eraseFromParent();
  }

  return Transformed;
}

bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses LocalOpts::run(Module &M, ModuleAnalysisManager &AM) {
  for (auto Fiter = M.begin(); Fiter != M.end(); ++Fiter)
    if (runOnFunction(*Fiter))
      return PreservedAnalyses::none();
  
  return PreservedAnalyses::all();
}

