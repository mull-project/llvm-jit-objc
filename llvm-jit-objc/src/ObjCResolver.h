#pragma once

#include <llvm/ExecutionEngine/RuntimeDyld.h>

class ObjCResolver : public llvm::JITSymbolResolver {
public:
  ObjCResolver() {}
  llvm::JITSymbol findSymbol(const std::string &Name);
  llvm::JITSymbol findSymbolInLogicalDylib(const std::string &Name);
};
