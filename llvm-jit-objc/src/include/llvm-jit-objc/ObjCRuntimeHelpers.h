#pragma once

#include <objc/runtime.h>

namespace objc {
  bool objc_classIsRegistered(Class cls);
  void objc_dumpClass(Class clz);
  void objc_dumpClasses();
} // namespace objc
