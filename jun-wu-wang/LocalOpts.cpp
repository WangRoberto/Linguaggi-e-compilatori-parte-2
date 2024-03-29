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

bool runOnInstruction(Instruction &I) {
  // Check if the instruction is a BinaryOperator
  BinaryOperator *BinaryI = dyn_cast<BinaryOperator>(&I);
  if (!BinaryI) {
    return false;
  }

  // Check if the instruction is a mul
  if (BinaryI->getOpcode() != Instruction::Mul) {
    return false;
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
      return false;
    }
  }

  // Check if immediate is a power of 2
  APInt ImmediateValue = Immediate->getValue();
  if (!ImmediateValue.isPowerOf2()) {
    outs() << "\thas no immediate that is power of 2\n";
    return false;
  }

  // Create a shl instruction
  int32_t N = ImmediateValue.exactLogBase2();
  LLVMContext &context = I.getContext(); 
  IntegerType *Int32Type = IntegerType::get(context, 32);
  ConstantInt *Shifts = ConstantInt::get(Int32Type, N);

  Instruction *NewInst = BinaryOperator::Create(
  BinaryOperator::Shl, Val, Shifts);

  // Insert the new instruction
  NewInst->insertAfter(&I);
  I.replaceAllUsesWith(NewInst);
  outs() << "\thas immediate that is power of 2\n";

  return true;
}

bool runOnBasicBlock(BasicBlock &B) {
  bool Transformed = false;

  for (auto Iter = B.begin(); Iter != B.end(); ++Iter) {
    if (runOnInstruction(*Iter)) {
      Transformed = true;
    }
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

