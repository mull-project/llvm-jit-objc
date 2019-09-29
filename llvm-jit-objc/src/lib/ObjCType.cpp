#include "llvm-jit-objc/ObjCType.h"

#include "llvm-jit-objc/ObjCRuntimeHelpers.h"
#include "llvm-jit-objc/DebugUtils.h"

#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

using namespace llvm;
using namespace objc;

namespace mull { namespace objc {

  std::string method64_t::getDebugDescription(int level) const {
    std::ostringstream os;
    std::string padding = std::string(level * 4, ' ');

    os << padding << "[method64_t]\n";
    os << padding << "\t" << "name: " << sel_getName(name) << "\n";
    os << padding << "\t" << "types: " << (const char *)types << "\n";
    os << padding << "\t" << "imp: " <<  (const void * const *)&imp << " / " << (const void *)imp << " / " << to_hex16(imp) << "\n";

    return os.str();
  }

  std::string method_list64_t::getDebugDescription(int level) const {
    std::ostringstream os;
    std::string padding = std::string(level * 4, ' ');

    if (count > 100 || entsize == 0) {
      os << padding << "[error]" << "\n";
      return os.str();
    }

    os << padding << "[method_list64_t]\n";
    os << padding << "entsize: " << entsize << "\n";
    os << padding << "count: " << count << "\n";
    for (uint32_t i = 0; i < count; i++) {
      const method64_t *method = getFirstMethodPointer() + i;
      os << method->getDebugDescription(level + 1);
    }

    return os.str();
  }

  std::string class_ro64_t::getDebugDescription(int level) const {
    std::ostringstream os;
    std::string padding = std::string(level * 4, ' ');

    os << padding << "[class_ro64_t] (metaclass: " << (isMetaClass() ? "yes" : "no") << ")\n";
    os << padding << "name: " << getName() << "\n";
    os << padding << "baseMethods: " << (void *)baseMethods << "\n";
    if (method_list64_t *methodListPointer = getMethodListPtr()) {
      os << methodListPointer->getDebugDescription(level + 1);
    }

    return os.str();
  }

  std::string class64_t::getDebugDescription(Description level) const {
    std::ostringstream os;
    std::string padding = std::string(level * 4, ' ');

    os << padding << "[class64_t]" << "\n";
    os << padding << "this: " << (void *)this << " / " << to_hex16(this) << "\n";
    os << padding << "isa: " << (void *)isa << "\n";
    if (level == Clazz) {
      os << getIsaPointer()->getDebugDescription(IsaOrSuperclass);
    }
    os << padding << "superclass: " << (void *const *)&superclass << "/" << (void *)superclass << "\n";
    if (level == Clazz) {
      Class superClz = (Class)getSuperclassPointer();
      if (objc_classIsRegistered(superClz)) {
        os << padding << "\t(registered) " << object_getClassName((id)superClz) << "\n";
      } else {
        class64_t *superclassPointer = getSuperclassPointer();
        os << superclassPointer->getDebugDescription(IsaOrSuperclass);
      }
    }
    os << padding << "cache: " << (void *const *)&cache << "/" << (void *)cache << "\n";

    os << padding << "vtable: " << (void *const *)&vtable << "/" << (void *)vtable << "\n";

    os << padding << "data: " << (void *const *)&data << "/" << (void *)data << "\n";

    auto dataPtr = getDataPointer();
    os << dataPtr->getDebugDescription(level + 1);

    return os.str();
  }

} }

