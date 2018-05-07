#include "ObjCResolver.h"

#include <llvm/ExecutionEngine/RTDyldMemoryManager.h>
#include <llvm/Support/raw_ostream.h>

#include <objc/message.h>
#include <objc/runtime.h>

#include "ObjCType.h"

using namespace llvm;
using namespace mull::objc;

extern "C" void *custom_getInitializedObjCClass(Class clz) {
  assert(clz);

  printf("custom_getInitializedObjCClass> clz is %p", (void *)clz);

  class64_t *objcClassRef = (class64_t *)clz;

  const char *const className = objcClassRef->getDataPointer()->getName();
  printf("custom_getInitializedObjCClass> clz's name is %s\n", className);

  Class runtimeClz = objc_getClass(className);
  assert(runtimeClz);

  return runtimeClz;
}

extern "C" int objc_printf( const char * format, ... ) {
  errs() << "**** objc_printf ****" << "\n";

  va_list arguments;
  va_start(arguments, format);
  int res = vprintf(format, arguments);
  va_end(arguments);

  return res;
}

JITSymbol ObjCResolver::findSymbol(const std::string &Name) {
  errs() << "ObjCResolver::findSymbol> " << Name << "\n";
  if (Name == "_printf") {
    return
      JITSymbol((uint64_t)objc_printf,
                JITSymbolFlags::Exported);
  }

  if (Name == "_custom_getInitializedObjCClass") {
    return
      JITSymbol((uint64_t)custom_getInitializedObjCClass,
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
