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

void track_mem_access(Instruction *I)
{
  bool is_write = false;
  uint32_t access_size = 0;
  Value *addr = nullptr;
  Instruction *instr = nullptr;
  if (auto *LI = dyn_cast<LoadInst>(I))
  {
    is_write = false;
    Type *ptrType = LI->getType();
    access_size = ptrType->getScalarType()->getScalarSizeInBits() / 8;
    // errs() << "load size is: " << static_cast<uint32_t>(access_size) << "\n";
    addr = LI->getPointerOperand();
    instr = LI;
  }
  else if (auto *SI = dyn_cast<StoreInst>(I))
  {
    is_write = true;
    Type *ptrType = SI->getValueOperand()->getType();
    access_size = ptrType->getScalarType()->getScalarSizeInBits() / 8;
    // errs() << "load size is: " << static_cast<uint32_t>(access_size) << "\n";
    addr = SI->getPointerOperand();
    instr = SI;
  }
  if (addr == (void *)0 || access_size == 0)
  {
    return;
  }
  if (isa<AllocaInst>(addr))
    return;
  // errs() << "filtered addr: " << addr << " access size: " << access_size
  //        << "\n";

  IRBuilder<> builder(instr);
  builder.SetInsertPoint(instr);
  Value *access_size_val = builder.getInt32(access_size);
  builder.CreateCall(logFunc, {addr, access_size_val});
  func_call_num++;
}

PreservedAnalyses RuntimeAddr::run(llvm::Module &M,
                                   llvm::ModuleAnalysisManager &)
{
  TD = M.getDataLayout();
  Ctx = &(M.getContext());
  retType = Type::getVoidTy(*Ctx);
  getInt32Type = Type::getInt32Ty(*Ctx);
  logFuncType = FunctionType::get(retType, {getInt32Type, getInt32Type}, false);
  for (auto &F : M.functions())
  {
    // func_call_num = 0;
    logFunc = M.getOrInsertFunction("populate_shared_mem", logFuncType);
    std::string exclude_func_0 = "printf";
    std::string exclude_func_1 = "main";
    if (std::string(F.getName()).find(exclude_func_0) != std::string::npos ||
        std::string(F.getName()).find(exclude_func_1) != std::string::npos)
    {
      continue;
    }
    errs() << "instrumenting: " << std::string(F.getName()) << "\n";
    for (auto &B : F)
    {
      for (auto &I : B)
      {
        // if (CallInst *callInst = dyn_cast<CallInst>(&I))
        // {
        //   Function *calledFunction = callInst->getCalledFunction();
        //   if (calledFunction && calledFunction->getName() == "malloc")
        //   {
        //     errs() << "Found dynamic memory allocation: " << *callInst << "\n";
        //     if (BitCastInst *bitcastInst = dyn_cast<BitCastInst>(callInst->getNextNode()))
        //     {
        //       for (auto *U : bitcastInst->users())
        //       {
        //         if (LoadInst *loadInst = dyn_cast<LoadInst>(U))
        //         {
        //           errs() << "  Used in load: " << *loadInst << "\n";
        //         }
        //         else if (StoreInst *storeInst = dyn_cast<StoreInst>(U))
        //         {
        //           errs() << "  Used in store: " << *storeInst << "\n";
        //         }
        //       }
        //     }
        //   }
        // }
        // if (CallInst *callInst = dyn_cast<CallInst>(&I))
        // {
        //   Function *calledFunction = callInst->getCalledFunction();
        //   if (calledFunction && calledFunction->getName() == "malloc")
        //   {
        //     errs() << "Found dynamic memory allocation: " << *callInst << "\n";
        //     for (auto *U : callInst->users())
        //     {
        //       if (BitCastInst *bitcastInst = dyn_cast<BitCastInst>(U))
        //       {
        //         // Follow the bitcast instruction uses
        //         for (auto *U2 : bitcastInst->users())
        //         {
        //           if (StoreInst *storeInst = dyn_cast<StoreInst>(U2))
        //           {
        //             errs() << "  Malloc result stored: " << *storeInst << "\n";
        //           }
        //           else if (LoadInst *loadInst = dyn_cast<LoadInst>(U2))
        //           {
        //             errs() << "  Malloc result loaded: " << *loadInst << "\n";
        //           }
        //         }
        //       }
        //       else if (StoreInst *storeInst = dyn_cast<StoreInst>(U))
        //       {
        //         errs() << "  Malloc result stored directly: " << *storeInst << "\n";
        //       }
        //       else if (LoadInst *loadInst = dyn_cast<LoadInst>(U))
        //       {
        //         errs() << "  Malloc result loaded directly: " << *loadInst << "\n";
        //       }
        //     }
        //   }
        // }
        if (isa<AllocaInst>(&I) ||
            isa<GlobalVariable>(&I))
        { // TODO: double check if correct
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
llvmGetPassPluginInfo()
{
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "runtime-addr-cc",
          .PluginVersion = "v0.1",
          .RegisterPassBuilderCallbacks = [](PassBuilder &PB)
          {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level)
                {
                  MPM.addPass(RuntimeAddr());
                });
          }};
}
