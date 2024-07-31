#include "RuntimeAddr.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h" // Include this header for LLVMContext
#include "llvm/IR/Type.h"        // Include this header for Type
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

using namespace llvm;
int func_call_num = 0;

#define DEBUG_TYPE "runtime-addr-cc"
DataLayout TD = DataLayout(llvm::StringRef());
LLVMContext *Ctx;
Type *retType;
Type *getInt32Type;
FunctionType *logFuncType;
FunctionCallee logFunc;

bool RuntimeAddr::runOnModule(Module &M) {}

void track_mem_access(Instruction *I) {
  bool is_write = false;
  uint32_t access_size = 0;
  Value *addr = nullptr;
  Instruction *instr = nullptr;
  if (auto *LI = dyn_cast<LoadInst>(I)) {
    is_write = false;
    Type *ptrType = LI->getType();
    access_size = ptrType->getScalarType()->getScalarSizeInBits() / 8;
    // errs() << "load size is: " << static_cast<uint32_t>(access_size) << "\n";
    addr = LI->getPointerOperand();
    instr = LI;
  } else if (auto *SI = dyn_cast<StoreInst>(I)) {
    is_write = true;
    Type *ptrType = SI->getValueOperand()->getType();
    access_size = ptrType->getScalarType()->getScalarSizeInBits() / 8;
    // errs() << "load size is: " << static_cast<uint32_t>(access_size) << "\n";
    addr = SI->getPointerOperand();
    instr = SI;
  }
  if (addr == (void *)0 || access_size == 0) {
    return;
  }
  // errs() << "filtered addr: " << addr << " access size: " << access_size
  //        << "\n";

  IRBuilder<> builder(instr);
  builder.SetInsertPoint(instr);
  Value *access_size_val = builder.getInt32(access_size);
  builder.CreateCall(logFunc, {addr, access_size_val});
  func_call_num++;
}

PreservedAnalyses RuntimeAddr::run(llvm::Module &M,
                                   llvm::ModuleAnalysisManager &) {
  TD = M.getDataLayout();
  Ctx = &(M.getContext());
  retType = Type::getVoidTy(*Ctx);
  getInt32Type = Type::getInt32Ty(*Ctx);
  logFuncType = FunctionType::get(retType, {getInt32Type, getInt32Type}, false);
  for (auto &F : M.functions()) {
    // func_call_num = 0;
    logFunc = M.getOrInsertFunction("populate_shared_mem", logFuncType);
    // logFunc = cast<Function>(
    //     M.getOrInsertFunction("logAddress", logFuncType).getCallee());
    std::string target_0 = "cxx";
    std::string target_1 = "ector";
    std::string target_2 = "Pair";
    if (std::string(F.getName()).find(target_0) != std::string::npos ||
        std::string(F.getName()).find(target_1) != std::string::npos ||
        std::string(F.getName()).find(target_2) != std::string::npos) {
      continue;
    }
    // if (std::string(F.getName()).find(target_1) == std::string::npos) {
    //   continue;
    // }
    // if (std::string(F.getName()).find(target_2) == std::string::npos) {
    //   continue;
    // }
    // if (std::string(F.getName()) !=
    //     "_Z10BCVerifierRK8CSRGraphIiiLb1EER12SourcePickerIS0_EiRK7pvectorIfE")
    //   continue;
    errs() << std::string(F.getName()) << "\n";
    for (auto &B : F) {
      for (auto &I : B) {
        if (isa<AllocaInst>(&I) ||
            isa<GlobalVariable>(&I)) { // TODO: double check if correct
          // errs() << "stack or global obj, skipping" << "\n";
          continue;
        }
        track_mem_access(&I);
      }
    }
  }
    errs() << "func_call_num: " << func_call_num << "\n";
  return PreservedAnalyses::none();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
// llvm::PassPluginLibraryInfo getRuntimeAddrPluginInfo() {
//   return {LLVM_PLUGIN_API_VERSION, "runtime-addr-cc", LLVM_VERSION_STRING,
//           [](PassBuilder &PB) {
//             PB.registerPipelineParsingCallback(
//                 [](StringRef Name, ModulePassManager &MPM,
//                    ArrayRef<PassBuilder::PipelineElement>) {
//                   if (Name == "runtime-addr-cc") {
//                     MPM.addPass(RuntimeAddr());
//                     return true;
//                   }
//                   return false;
//                 });
//           }};
// }

// extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
// llvmGetPassPluginInfo() {
//   return getRuntimeAddrPluginInfo();
// }

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "runtime-addr-cc",
          .PluginVersion = "v0.1",
          .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                  MPM.addPass(RuntimeAddr());
                });
          }};
}
