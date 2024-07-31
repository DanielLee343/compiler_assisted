//==============================================================================
// FILE:
//    RuntimeAddr.h
//
// DESCRIPTION:
//  Get the runtime address of variables experiencing load and store
//
// License: MIT
//==============================================================================
#ifndef LLVM_TUTOR_INSTRUMENT_BASIC_H
#define LLVM_TUTOR_INSTRUMENT_BASIC_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
struct RuntimeAddr : public llvm::PassInfoMixin<RuntimeAddr> {
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &);
  bool runOnModule(llvm::Module &M);
  static bool isRequired() { return true; }
  void logAddress(llvm::Value *Addr, llvm::Instruction *InsertBefore);
};

#endif
