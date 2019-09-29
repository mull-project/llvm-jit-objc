#include "TestHelpers.h"

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

std::unique_ptr<llvm::Module> loadModuleAtPath(const std::string &path,
                                               llvm::LLVMContext &llvmContext) {

  auto BufferOrError = llvm::MemoryBuffer::getFile(path);
  if (!BufferOrError) {
    errs() << "ModuleLoader> Can't load module " << path << '\n';
    return nullptr;
  }
  errs() << "ModuleLoader> module " << path << '\n';

  auto llvmModule = llvm::parseBitcodeFile(BufferOrError->get()->getMemBufferRef(),
                                           llvmContext);

  if (!llvmModule) {
    errs() << "ModuleLoader> Can't load module " << path << '\n';
    return nullptr;
  }

  return std::move(llvmModule.get());
}
