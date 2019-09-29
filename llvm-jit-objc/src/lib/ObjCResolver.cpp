#include "llvm-jit-objc/ObjCResolver.h"

#include "llvm-jit-objc/ObjCType.h"

#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/Support/raw_ostream.h>

#include <objc/message.h>
#include <objc/runtime.h>

using namespace llvm;
using namespace mull::objc;

extern "C" int objc_printf( const char * format, ... ) {
  errs() << "**** objc_printf ****" << "\n";

  va_list arguments;
  va_start(arguments, format);
  int res = vprintf(format, arguments);
  va_end(arguments);

  return res;
}

JITSymbol ObjCResolver::findSymbol(const std::string &Name) {
  //errs() << "ObjCResolver::findSymbol> " << Name << "\n";
  if (Name == "_printf") {
    return
      JITSymbol((uint64_t)objc_printf,
                JITSymbolFlags::Exported);
  }

  if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(Name)) {
    return JITSymbol(SymAddr, JITSymbolFlags::Exported);
  }

  return JITSymbol(nullptr);
}

JITSymbol
ObjCResolver::findSymbolInLogicalDylib(const std::string &Name) {
    //errs() << "ObjCResolver::findSymbolInLogicalDylib> " << Name << "\n";

  return JITSymbol(nullptr);
}
