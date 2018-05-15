#include "ObjCRuntime.h"

#include "DebugUtils.h"
#include "ObjCRuntimeHelpers.h"

#include <objc/message.h>

#include <iostream>
#include <inttypes.h>

extern "C" Class objc_readClassPair(Class bits, const struct objc_image_info *info);

using namespace objc;

namespace mull { namespace objc {

mull::objc::Runtime::~Runtime() {
  for (const Class &clz: runtimeClasses) {
    errs() << "disposing class: " << class_getName(clz) << "\n";
    //assert(objc_classIsRegistered(clz));

    objc_disposeClassPair(clz);

    assert(objc_classIsRegistered(clz) == false);
  }
}

void mull::objc::Runtime::registerSelectors(void *selRefsSectionPtr,
                                            uintptr_t selRefsSectionSize) {
  uint8_t *sectionStart = (uint8_t *)selRefsSectionPtr;

  for (uint8_t *cursor = sectionStart;
       cursor < (sectionStart + selRefsSectionSize);
       cursor = cursor + sizeof(SEL)) {
    SEL *selector = (SEL *)cursor;

      // TODO: memory padded/aligned by JIT.
    if (*selector == NULL) {
      continue;
    }
    const char *name = sel_getName(*selector);
    errs() << "Trying to register selector: " << *selector << "/" << name << "\n";
    *selector = sel_registerName(name);
  }
}

void mull::objc::Runtime::addClassesFromSection(void *sectionPtr,
                                                uintptr_t sectionSize) {
  errs() << "mull::objc::Runtime> adding section for later registration: "
         << sectionPtr << ", " << sectionSize << "\n";

  class64_t **classes = (class64_t **)sectionPtr;

  uint32_t count = sectionSize / 8;
  // 16 is "8 + alignment" (don't know yet what alignment is for).
  for (uintptr_t i = 0; i < count / 2; i += 1) {
    class64_t **clzPointerRef = &classes[i];
    class64_t *clzPointer = *clzPointerRef;

    errs() << "mull::objc::Runtime> adding class for registration: "
           << clzPointer->getDataPointer()->getName() << "\n";

    classesToRegister.push(clzPointerRef);
  }
}

void mull::objc::Runtime::addClassesFromClassRefsSection(void *sectionPtr,
                                                         uintptr_t sectionSize) {
  errs() << "Runtime> addClassesFromClassRefsSection" << "\n";

  Class *classrefs = (Class *)sectionPtr;
  uint32_t count = sectionSize / 8;

  for (uint32_t i = 0; i < count / 2; i++) {
    Class *classrefPtr = (&classrefs[i]);
    const char *className = object_getClassName(*classrefPtr);
    assert(className);
    errs() << "Considering " << className << "\n";

    Class newClz = objc_getRequiredClass(className);
    assert(class_isMetaClass(newClz) == false);
    errs() << "\tclass has address: " << (void *)newClz << "\n";

    if (*classrefPtr != newClz) {
      *classrefPtr = objc_getRequiredClass(className);
    }
  }
}

void mull::objc::Runtime::addClassesFromSuperclassRefsSection(void *sectionPtr,
                                                              uintptr_t sectionSize) {
  errs() << "Runtime> addClassesFromSuperclassRefsSection" << "\n";

  Class *classrefs = (Class *)sectionPtr;
  uint32_t count = sectionSize / 8;

  for (uint32_t i = 0; i < count / 2; i++) {
    int refType = 0; // 0 - unknown, 1 - class, 2 - metaclass

    Class *classrefPtr = (&classrefs[i]);
    Class classref = *classrefPtr;

    class64_t *ref = NULL;
    for (auto &clref: classRefs) {
      if ((void *)clref == (void *)classref) {
        errs() << "Ref is class" << "\n";
        refType = 1;
        ref = clref;

        assert(ref);

        break;
      }
    }

    if (refType == 0) {
      for (auto &metaclref: metaclassRefs) {
        if ((void *)metaclref == (void *)classref) {
          errs() << "Ref is metaclass" << "\n";
          refType = 2;
          ref = metaclref;
          break;
        }
      }
    }

    assert(refType != 0);
    const char *className;
    if (ref == NULL) {
      className = object_getClassName(classref);

      errs() << "Class is already registered (not by us) - " << className << "\n";
    } else {
      className = ref->getDataPointer()->getName();
    }
    here_objc_class *objcClass = (here_objc_class *)classref;
    if (refType == 1 && runtimeClasses.count(classref) > 0) {
      className = objcClass->data()->ro->name;
    }
    else if (refType == 2) {
      Class classForMetaClass = objc_getClass(object_getClassName(classref));
      assert(classForMetaClass);
      className = objcClass->data()->ro->name;
    }
    assert(className);

    errs() << "Considering " << className << "\n";
    Class newClz = objc_getClass(className);

    assert(refType != 0);
    if (refType == 2) {
      newClz = objc_getMetaClass(className);
      assert(newClz);
      assert(class_isMetaClass(newClz));
    }

    if (*classrefPtr != newClz) {
      *classrefPtr = newClz;
    }
  }
}

void mull::objc::Runtime::addCategoriesFromSection(void *sectionPtr,
                                                   uintptr_t sectionSize) {
  errs() << "Runtime> addCategoriesFromSection" << "\n";

  category_t **categories = (category_t **)sectionPtr;
  uint32_t count = sectionSize / 8;

  for (uint32_t i = 0; i < count / 2; i++) {
    category_t *category = categories[i];
    Class clz = (Class)category->cls;
    Class metaClz = objc_getMetaClass(class_getName(clz));

    /* Instance methods */
    const method_list64_t *clzMethodListPtr = (const method_list64_t *)category->instanceMethods;
    if (clzMethodListPtr) {
      const method64_t *methods = (const method64_t *)clzMethodListPtr->getFirstMethodPointer();

      for (uint32_t i = 0; i < clzMethodListPtr->count; i++) {
        const method64_t *methodPtr = &methods[i];

        IMP imp = (IMP)methodPtr->imp;

        BOOL success = class_addMethod(clz,
                                       sel_registerName(sel_getName(methodPtr->name)),
                                       (IMP)imp,
                                       (const char *)methodPtr->types);
        assert(success);
      }
    }

    /* Class methods */
    const method_list64_t *metaclassMethodListPtr = (const method_list64_t *)category->classMethods;
    if (metaclassMethodListPtr) {
      const method64_t *methods = (const method64_t *)metaclassMethodListPtr->getFirstMethodPointer();

      for (uint32_t i = 0; i < clzMethodListPtr->count; i++) {
        const method64_t *methodPtr = &methods[i];

        IMP imp = (IMP)methodPtr->imp;

        BOOL success = class_addMethod(metaClz,
                                       sel_registerName(sel_getName(methodPtr->name)),
                                       (IMP)imp,
                                       (const char *)methodPtr->types);
        assert(success);
      }
    }
  }
}

#pragma mark - Private

void mull::objc::Runtime::registerClasses() {
  while (classesToRegister.empty() == false) {
    class64_t **classrefPtr = classesToRegister.front();
    class64_t *classref = *classrefPtr;
    classesToRegister.pop();

    errs() << "registerClasses() " << classref->getDataPointer()->getName() << "\n";

    class64_t *superClz64 = classref->getSuperclassPointer();
    Class superClz = (Class)superClz64;
    if (objc_classIsRegistered(superClz) == false) {
      const char *superclzName = superClz64->getDataPointer()->getName();
      if (Class registeredSuperClz = objc_getClass(superclzName)) {
        errs() << "registerClasses() " << "superclass is registered" << "\n";
        superClz = registeredSuperClz;
      } else {
        errs() << "registerClasses() " << "superclass is not registered" << "\n";

        classesToRegister.push(classrefPtr);
        continue;
      }
    }

    errs() << "registerClasses() "
           << "superclass is registered for class: " << classref->getDataPointer()->getName()
           << "\n";
    //errs() << classref->getDebugDescription() << "\n";

    assert(superClz);

    errs() << classref->getDebugDescription() << "\n";

    Class runtimeClass = registerOneClass(classrefPtr, superClz);
    assert(objc_classIsRegistered(runtimeClass));

    runtimeClasses.insert(runtimeClass);

    oldAndNewClassesMap.push_back(std::pair<class64_t **, Class>(classrefPtr, runtimeClass));
  }

  assert(classesToRegister.empty());
}

Class mull::objc::Runtime::registerOneClass(class64_t **classrefPtr,
                                            Class superclass) {

  class64_t *classref = *classrefPtr;
  class64_t *metaclassRef = classref->getIsaPointer();

  assert(strlen(classref->getDataPointer()->name) > 0);
  printf("registerOneClass: %s \n", classref->getDataPointer()->name);
  printf("\t" "source ptr: 0x%016" PRIxPTR "\n", (uintptr_t)classref);
  printf("0x%016" PRIXPTR "\n", (uintptr_t) classref);

  classRefs.push_back(classref);
  metaclassRefs.push_back(metaclassRef);

  if (objc_getClass(classref->getDataPointer()->name) != nullptr) {
    errs() << "JIT Error: Objective-C class is already registered: "
           << classref->getDataPointer()->name << "\n";
    exit(1);
  }
  assert(objc_classIsRegistered((Class)classref) == false);

  Class runtimeClass = objc_readClassPair((Class)classref, NULL);
  assert(runtimeClass);

  // The following might be wrong:
  // The class is registered by objc_readClassPair but we still hack on its
  // `flags` below and call objc_registerClassPair to make sure we can dispose
  // it with objc_disposeClassPair when JIT deallocates.
  assert(objc_classIsRegistered((Class)runtimeClass));

  here_objc_class *runtimeClassInternal = (here_objc_class *)runtimeClass;
  here_objc_class *runtimeMetaclassInternal = (here_objc_class *)runtimeClassInternal->ISA();

#define RW_CONSTRUCTING       (1<<26)
  here_class_rw_t *sourceClassData = runtimeClassInternal->data();
  here_class_rw_t *sourceMetaclassData = (here_class_rw_t *)runtimeMetaclassInternal->data();
  sourceClassData->flags |= RW_CONSTRUCTING;
  sourceMetaclassData->flags |= RW_CONSTRUCTING;
  objc_registerClassPair(runtimeClass);

  return runtimeClass;
}

} }
