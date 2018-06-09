#include "llvm-jit-objc/ObjCRuntimeHelpers.h"

#include <llvm/Support/raw_ostream.h>

#include <assert.h>

using namespace llvm;

namespace objc {

void objc_dumpClasses() {
  int numRegisteredClasses = objc_getClassList(NULL, 0);
  assert(numRegisteredClasses > 0);

  Class *classes = (Class *)malloc(sizeof(Class) * numRegisteredClasses);

  numRegisteredClasses = objc_getClassList(classes, numRegisteredClasses);

  for (int i = 0; i < numRegisteredClasses; i++) {
    errs() << "registered class: " << class_getName(classes[i]) << "\n";
  }

  free(classes);
}

bool objc_classIsRegistered(Class cls) {
  int numRegisteredClasses = objc_getClassList(NULL, 0);
  assert(numRegisteredClasses > 0);

  Class *classes = (Class *)malloc(sizeof(Class) * numRegisteredClasses);

  numRegisteredClasses = objc_getClassList(classes, numRegisteredClasses);

  for (int i = 0; i < numRegisteredClasses; i++) {
    if (classes[i] == cls) {
      free(classes);
      return true;
    }
  }

  free(classes);
  return false;
}

void objc_dumpClass(Class clz) {
  printf("class_dumpMethods() dumping class: %p, is meta class: %d\n", (void *)clz, class_isMetaClass(clz));

  unsigned int methodCount = 0;
  Method *methods = class_copyMethodList(clz, &methodCount);

  printf("Found %d methods on '%s'\n", methodCount, class_getName(clz));

  for (unsigned int i = 0; i < methodCount; i++) {
    Method method = methods[i];

    printf("\t'%s' has method named '%s' of encoding '%s' %p\n",
           class_getName(clz),
           sel_getName(method_getName(method)),
           method_getTypeEncoding(method),
           (void *)method);
  }

  free(methods);
}

}
