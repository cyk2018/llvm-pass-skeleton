


#include <vector>
#include "llvm/Analysis/InlineCost.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
// using namespace llvm;

namespace llvm{
const int INLINE_THRESHOLD = 100;
const int NUM_RUNS = 1;

class FunctionInliningPass : public PassInfoMixin<FunctionInliningPass> {

 public:
  // static char ID;


  PreservedAnalyses run(Function& F, FunctionAnalysisManager& AM) {
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
          if (fun != nullptr && isInlineViable(*fun).isSuccess() &&
              fun->getInstructionCount() <= INLINE_THRESHOLD) {
            InlineFunctionInfo info;
            auto call_base = dyn_cast<CallBase>(call);
            InlineFunction(*call_base, info);
            modified = true;
          }
        }
      }
    }
    return PreservedAnalyses::all();
  }

};

}  // namespace

// char FunctionInliningPass::ID = 0;

// Register the pass so `opt -function-inlining` runs it.
// static RegisterPass<FunctionInliningPass> X("function-inlining",
//                                             "a useful pass");

// Register the pass

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {

  return {

    LLVM_PLUGIN_API_VERSION, "function-inlining", "v0.1",

    [](llvm::PassBuilder &PB) {

      PB.registerPipelineParsingCallback(

        [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,

           llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {

          if(Name == "function-inlining"){

            FPM.addPass(llvm::FunctionInliningPass());

            return true;

          }

          return false;

        }

      );

    }

  };

}


