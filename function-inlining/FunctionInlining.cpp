#include <vector>
#include "llvm/Analysis/InlineCost.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"
using namespace llvm;

namespace {
const int INLINE_THRESHOLD = 1000;
const int NUM_RUNS = 1;
struct FunctionInliningPass : public FunctionPass {
  static char ID;
  FunctionInliningPass() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) {
    bool modified = false;
    for (int i = 0; i < NUM_RUNS; ++i) {
      std::vector<Instruction *> worklist;
      for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
        worklist.push_back(&*I);
      }
      for (Instruction *I : worklist) {
        CallInst *call = dyn_cast<CallInst>(I);
        if (call != nullptr) {
          Function *fun = call->getCalledFunction();
          if (fun != nullptr && isInlineViable(*fun) &&
              fun->getInstructionCount() <= INLINE_THRESHOLD) {
            InlineFunctionInfo info;
            InlineFunction(call, info);
            modified = true;
          }
        }
      }
    }
    return modified;
  }
};
}  // namespace

char FunctionInliningPass::ID = 0;

// Register the pass so `opt -function-inlining` runs it.
static RegisterPass<FunctionInliningPass> X("function-inlining",
                                            "a useful pass");
