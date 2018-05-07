
#pragma once

#include "ObjCType.h"

#include <objc/objc.h>

#include <llvm/Support/raw_ostream.h>

#include <queue>
#include <vector>
#include <set>

using namespace llvm;

#pragma mark -

namespace mull { namespace objc {

class Runtime {
  std::queue<class64_t **> classesToRegister;

  std::vector<class64_t *> classRefs;
  std::vector<class64_t *> metaclassRefs;

  std::set<Class> runtimeClasses;
  std::vector<std::pair<class64_t **, Class>> oldAndNewClassesMap;

  Class registerOneClass(class64_t **classrefPtr, Class superclass);
  void parsePropertyAttributes(const char *const attributesStr,
                               char *const stringStorage,
                               objc_property_attribute_t *attributes,
                               size_t *count);

public:
  ~Runtime();

  void registerSelectors(void *selRefsSectionPtr,
                         uintptr_t selRefsSectionSize);

  void addClassesFromSection(void *sectionPtr,
                             uintptr_t sectionSize);
  void addClassesFromClassRefsSection(void *sectionPtr,
                                      uintptr_t sectionSize);
  void addClassesFromSuperclassRefsSection(void *sectionPtr,
                                           uintptr_t sectionSize);

  void registerClasses();
};

} }
