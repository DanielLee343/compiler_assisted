#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

namespace {
struct SharedModuleChecker : public ModulePass {
    static char ID;
    SharedModuleChecker() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {
        bool isSharedLibrary = false;

        // Check for position-independent code (PIC)
        for (const Function &F : M) {
            if (F.hasFnAttribute(Attribute::NonLazyBind) ||
                F.hasFnAttribute(Attribute::NoInline)) {
                isSharedLibrary = true;
                break;
            }
        }

        // Check for common shared library symbols
        if (!isSharedLibrary) {
            for (const GlobalVariable &GV : M.globals()) {
                StringRef Name = GV.getName();
                if (Name == "_init" || Name == "_fini" || Name == "__dso_handle") {
                    isSharedLibrary = true;
                    break;
                }
            }
        }

        if (isSharedLibrary) {
            errs() << "Module " << M.getName() << " is a shared library.\n";
        } else {
            errs() << "Module " << M.getName() << " is not a shared library.\n";
        }

        return false; // Return false as we are not modifying the module
    }
};
} // namespace

char SharedModuleChecker::ID = 0;
static RegisterPass<SharedModuleChecker> X("sharedmodulechecker", "Shared Module Checker", false, false);