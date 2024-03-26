#ifndef LLVM_TRANSFORMS_LOCALOPTS_H
#define LLVM_TRANSFORMS_LOCALOPTS_H
#include "llvm/IR/PassManager.h"
// L'include seguente va in LocalOpts.h
#include <llvm/IR/Constants.h>
namespace llvm {

/*
class TestPass : public PassInfoMixin<TestPass> {
public:
PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};*/

class LocalOpts : public PassInfoMixin<LocalOpts> {
public:
PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

} // namespace llvm
#endif // LLVM_TRANSFORMS_TESTPASS _H
