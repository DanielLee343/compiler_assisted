#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h" // Include this header for LLVMContext
#include "llvm/IR/Type.h"        // Include this header for Type
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;
int instru_num = 0;

#define DEBUG_TYPE "runtime-addr-pr"
DataLayout TD = DataLayout(llvm::StringRef());
LLVMContext *Ctx;
Type *retType;
Type *getInt32Type;
FunctionType *logFuncType;
FunctionCallee logFunc;

int analyzeFunction_demangle(Function &F)
{
  std::string mangledName = F.getName().str();
  const char *mangledNameChar = mangledName.c_str();
  size_t Size = 1;
  char *Buf = static_cast<char *>(std::malloc(Size));

  llvm::ItaniumPartialDemangler Mangler;
  if (Mangler.partialDemangle(mangledNameChar))
  {
    errs() << "Failed to demangle: " << mangledName << "\n";
    return 1;
  }
  char *Result_func = Mangler.getFunctionName(Buf, &Size);
  errs() << " Funcname: " << Result_func << " org: " << F.getName() << "\n";
  return 0;
}

void track_mem_access(Instruction *I, Function &F)
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

  IRBuilder<> builder(instr);
  builder.SetInsertPoint(instr);
  Value *access_size_val = builder.getInt32(access_size);
  builder.CreateCall(logFunc, {addr, access_size_val});
  errs() << "instrumented: " << std::string(F.getName()) << "\n";
  analyzeFunction_demangle(F);
  instru_num++;
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

struct RuntimeAddr : public llvm::PassInfoMixin<RuntimeAddr>
{
  static bool isRequired() { return true; }
  PreservedAnalyses run(llvm::Module &M,
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
      // if (analyzeFunction_demangle(F))
      //   continue;
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
      // if (std::string(F.getName()).find(exclude_main) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_pass_start) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_rtlib) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_gnu) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_vector) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_std) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_mersenne) != std::string::npos ||
      //     std::string(F.getName()).find(exclude_pass_end) != std::string::npos)
      if (std::string(F.getName()).find("PageRankPullGS") == std::string::npos &&
          std::string(F.getName()).find("fabs") == std::string::npos &&
          std::string(F.getName()).find("in_degree") == std::string::npos &&
          std::string(F.getName()).find("out_degree") == std::string::npos)
        continue;

      for (auto &B : F)
      {
        for (auto &I : B)
        {
          if (isa<AllocaInst>(&I) ||
              isa<GlobalVariable>(&I))
          {
            continue;
          }
          track_mem_access(&I, F);
        }
      }
    }
    errs() << "instru_num: " << instru_num << "\n";
    return PreservedAnalyses::none();
  }
};
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "runtime-addr-pr",
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
