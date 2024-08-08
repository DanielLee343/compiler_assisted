#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace
{

    class SharedModuleCheckerPass : public PassInfoMixin<SharedModuleCheckerPass>
    {
    public:
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &)
        {
            bool isSharedLibrary = false;

            // Check for position-independent code (PIC)
            for (const Function &F : M)
            {
                if (F.hasFnAttribute(Attribute::NonLazyBind) ||
                    F.hasFnAttribute(Attribute::NoInline))
                {
                    isSharedLibrary = true;
                    break;
                }
            }

            // Check for common shared library symbols
            if (!isSharedLibrary)
            {
                for (const GlobalVariable &GV : M.globals())
                {
                    StringRef Name = GV.getName();
                    if (Name == "_init" || Name == "_fini" || Name == "__dso_handle")
                    {
                        isSharedLibrary = true;
                        break;
                    }
                }
            }

            if (isSharedLibrary)
            {
                errs() << "Module " << M.getName() << " is a shared library.\n";
            }
            else
            {
                errs() << "Module " << M.getName() << " is not a shared library.\n";
            }

            return PreservedAnalyses::all();
        }
    };

} // namespace

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {LLVM_PLUGIN_API_VERSION, "SharedModuleCheckerPass", LLVM_VERSION_STRING,
            [](PassBuilder &PB)
            {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, ModulePassManager &MPM,
                       ArrayRef<PassBuilder::PipelineElement>)
                    {
                        if (Name == "shared-module-checker")
                        {
                            MPM.addPass(SharedModuleCheckerPass());
                            return true;
                        }
                        return false;
                    });
            }};
}