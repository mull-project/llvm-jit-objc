#pragma once

#include <memory>
#include <llvm/IR/Module.h>

std::unique_ptr<llvm::Module> loadModuleAtPath(const std::string &path,
                                               llvm::LLVMContext &llvmContext);
