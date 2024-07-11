#ifndef LLVM_TRANSFORMS_LOOPFUSION_H
#define LLVM_TRANSFORMS_LOOPFUSION_H
#include "llvm/IR/PassManager.h"
#include <llvm/IR/Constants.h>
#include "llvm/Analysis/ScalarEvolution.h" 
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/GenericLoopInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/LoopNestAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

namespace llvm {

class LoopFusionPass : public PassInfoMixin<LoopFusionPass> {
public:
PreservedAnalyses run(Function &F, FunctionAnalysisManager  &AM);
};

} // namespace llvm
#endif // LLVM_TRANSFORMS_TESTPASS _H
