#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"

using namespace llvm;

namespace {
struct FunctionInliningPass : public FunctionPass {
  static char ID;
  FunctionInliningPass() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    for (auto &B : F) {
      for (auto &I : B) {
        errs() << "<> Instruction: ";
        I.print(llvm::errs());
        CallInst *call = dyn_cast<CallInst>(&I);
        if (call != nullptr) {
          errs() << "\t\t<----- detected call instruction.\n";
          InlineFunctionInfo info;
          if (InlineFunction(call, info))
            errs() << "<> INLINE SUCCESSFUL B)\n";
          else
            errs() << "<> INLINE failed >:(\n";
          break;
        }
        errs() << "\n";
      }
    }
    return false;
  }
};
}  // namespace

char FunctionInliningPass::ID = 0;

// Register the pass so `opt -skeleton` runs it.
static RegisterPass<FunctionInliningPass> X("function-inlining", "a useful pass");
