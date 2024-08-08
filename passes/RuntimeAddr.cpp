#include "RuntimeAddr.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h" // Include this header for LLVMContext
#include "llvm/IR/Type.h"        // Include this header for Type
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Demangle/Demangle.h"
#include <unordered_set>
#include <vector>

using namespace llvm;
int instru_num = 0;

#define DEBUG_TYPE "runtime-addr-cc"
DataLayout TD = DataLayout(llvm::StringRef());
LLVMContext *Ctx;
Type *retType;
Type *getInt32Type;
FunctionType *logFuncType;
FunctionCallee logFunc;

bool RuntimeAddr::runOnModule(Module &M) {}

void track_mem_access(Instruction *I, Function *F)
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
  errs() << "instrumented: " << std::string(F->getName()) << "\n";
  instru_num++;
}
int analyzeFunction_demangle(Function &F)
{
  // for (BasicBlock &BB : F)
  // {
  //   for (Instruction &I : BB)
  //   {
  //     if (CallInst *callInst = dyn_cast<CallInst>(&I))
  //     {
  //       if (Function *calledFunction = callInst->getCalledFunction())
  //       {
  std::string mangledName = F.getName().str();
  const char *mangledNameChar = mangledName.c_str();
  // int status = 0;
  // char *demangledName = llvm::itaniumDemangle(mangledName.c_str(), nullptr, nullptr, &status);
  // if (status == 0)
  // {
  //   errs() << "Function " << F.getName() << " calls function " << demangledName << "\n";
  //   free(demangledName);
  // }
  // else
  // {
  //   errs() << "Function " << F.getName() << " calls function " << mangledName << "\n";
  // }
  size_t Size = 1;
  char *Buf = static_cast<char *>(std::malloc(Size));

  llvm::ItaniumPartialDemangler Mangler;
  if (Mangler.partialDemangle(mangledNameChar))
  {
    errs() << "Failed to demangle: " << mangledName << "\n";
    return 1;
  }

  // char *Result_base = Mangler.getFunctionBaseName(Buf, &Size);
  char *Result_func = Mangler.getFunctionName(Buf, &Size);

  // errs() << "Basename: " << Result_base << "\n";
  errs() << " Funcname: " << Result_func << "\n";
  //       }
  //     }
  //   }
  // }
  return 0;
}
int analyzeFunction_direct(Function &F)
{
  for (BasicBlock &BB : F)
  {
    for (Instruction &I : BB)
    {
      if (CallInst *callInst = dyn_cast<CallInst>(&I))
      {
        if (Function *calledFunction = callInst->getCalledFunction())
        {
          return 1;
          // errs() << "Function " << F.getName()
          //        << " directly function " << calledFunction->getName() << "\n";
        }
      }
    }
  }
  return 0;
}

int analyzeFunction(Function &F)
{
  std::unordered_set<Function *> visited;
  std::vector<Function *> worklist;
  worklist.push_back(&F);

  while (!worklist.empty())
  {
    Function *currentFunc = worklist.back();
    worklist.pop_back();

    // If we've already visited this function, skip it
    if (!visited.insert(currentFunc).second)
      continue;

    // Analyze all call instructions within the current function
    for (BasicBlock &BB : *currentFunc)
    {
      for (Instruction &I : BB)
      {
        if (CallInst *callInst = dyn_cast<CallInst>(&I))
        {
          if (Function *calledFunction = callInst->getCalledFunction())
          {
            if (visited.find(calledFunction) == visited.end())
            {
              worklist.push_back(calledFunction);
              errs() << "Function " << currentFunc->getName()
                     << " calls function " << calledFunction->getName() << "\n";
              return 1;
            }
          }
        }
      }
    }
  }
  return 0;
}
PreservedAnalyses RuntimeAddr::run(llvm::Module &M,
                                   llvm::ModuleAnalysisManager &)
{
  TD = M.getDataLayout();
  Ctx = &(M.getContext());
  retType = Type::getVoidTy(*Ctx);
  getInt32Type = Type::getInt32Ty(*Ctx);
  logFuncType = FunctionType::get(retType, {getInt32Type, getInt32Type}, false);
  // std::string exclude_generator = "generator";
  // std::string exclude_make_graph = "make_graph";
  // if (std::string(M.getName()).find(exclude_generator) != std::string::npos ||
  //     std::string(M.getName()).find(exclude_make_graph) != std::string::npos)
  // {
  //   return PreservedAnalyses::all();
  // }
  errs() << "Inspecting module: " << M.getName() << "\n";
  for (auto &F : M.functions())
  {
    // enable this to XSBench
    // if (std::string(F.getName()).find("grid_init_do_not_profile") != std::string::npos)
    // {
    //   if (analyzeFunction_direct(F))
    //     continue;
    // }
    // enable this to gapbs-pr
    if (analyzeFunction_demangle(F))
      continue;
    if (F.isDeclaration())
    {
      errs() << "external func name: " << F.getName() << "\n";
      //   continue;
    }
    // instru_num = 0;
    logFunc = M.getOrInsertFunction("populate_shared_mem", logFuncType);
    std::string exclude_main = "main";
    std::string exclude_pass_start = "shared_mem_start";
    std::string exclude_pass_end = "shared_mem_end";
    std::string exclude_omp = "omp";
    std::string exclude_clone = "clone";
    std::string exclude_thread = "start_thread";
    std::string exclude_alloc = "alloc";
    std::string exclude_rtlib = "populate_shared_mem";
    std::string exclude_weird_0 = "mrg_step";
    std::string exclude_weird_1 = "mrg_orig_step";
    std::string exclude_weird_2 = "generate_kronecker_range";
    std::string exclude_weird_3 = "mrg";
    std::string exclude_weird_4 = "make_graph";
    std::string exclude_gnu = "gnu";
    std::string exclude_vector = "vector";
    std::string exclude_std = "std::";
    std::string exclude_mersenne = "mersenne";
    std::string target_func = "launch_bench";
    // if (std::string(F.getName()).find(exclude_func_0) != std::string::npos ||
    //     std::string(F.getName()).find(exclude_func_1) != std::string::npos)
    // {
    //   continue;
    // }
    // if (std::string(F.getName()).find(target_func) == std::string::npos)
    // {
    //   continue;
    // }
    if (std::string(F.getName()).find(exclude_main) != std::string::npos ||
        std::string(F.getName()).find(exclude_omp) != std::string::npos ||
        std::string(F.getName()).find(exclude_pass_start) != std::string::npos ||
        std::string(F.getName()).find(exclude_rtlib) != std::string::npos ||
        std::string(F.getName()).find(exclude_gnu) != std::string::npos ||
        std::string(F.getName()).find(exclude_vector) != std::string::npos ||
        std::string(F.getName()).find(exclude_std) != std::string::npos ||
        std::string(F.getName()).find(exclude_mersenne) != std::string::npos ||
        // std::string(F.getName()).find(exclude_thread) != std::string::npos ||
        // std::string(F.getName()).find(exclude_alloc) != std::string::npos ||
        // std::string(F.getName()).find(exclude_weird_0) != std::string::npos ||
        // std::string(F.getName()).find(exclude_weird_1) != std::string::npos ||
        // std::string(F.getName()).find(exclude_weird_2) != std::string::npos ||
        // std::string(F.getName()).find(exclude_weird_3) != std::string::npos ||
        // std::string(F.getName()).find(exclude_weird_4) != std::string::npos ||
        std::string(F.getName()).find(exclude_pass_end) != std::string::npos)
      continue;

    // errs() << "instrumenting: " << std::string(F.getName()) << "\n";
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
        {
          continue;
        }
        track_mem_access(&I, &F);
      }
    }
  }
  errs() << "instru_num: " << instru_num << "\n";
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
