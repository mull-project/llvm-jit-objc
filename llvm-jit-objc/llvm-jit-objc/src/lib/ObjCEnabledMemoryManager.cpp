#include <llvm-jit-objc/ObjCEnabledMemoryManager.h>

#include <llvm/Support/raw_ostream.h>

using namespace llvm;

ObjCEnabledMemoryManager::ObjCEnabledMemoryManager()
  : runtime(mull::objc::Runtime()) {}

uint8_t *
ObjCEnabledMemoryManager::allocateDataSection(uintptr_t Size,
                                              unsigned Alignment,
                                              unsigned SectionID,
                                              llvm::StringRef SectionName,
                                              bool isReadOnly) {
  uint8_t *pointer = SectionMemoryManager::allocateDataSection(Size,
                                                               Alignment,
                                                               SectionID,
                                                               SectionName,
                                                               isReadOnly);

  int numRegisteredClasses = objc_getClassList(NULL, 0);
  assert(numRegisteredClasses > 0);
  //errs() << "OBJC CREATED CLASSES constr: " << numRegisteredClasses << "\n";

  if (SectionName.find("objc") != llvm::StringRef::npos) {
    errs() << "MullMemoryManager::allocateDataSection(objc) -- "
           << SectionName << " "
           << "pointer: " << pointer << " "
           << "size: " << Size
           << "\n";

    ObjectSectionEntry entry(pointer, Size, SectionName);

    objcSections.push_back(entry);
  }

  return pointer;
}

bool ObjCEnabledMemoryManager::finalizeMemory(std::string *ErrMsg) {
  registerObjC();

  int numRegisteredClasses = objc_getClassList(NULL, 0);
  assert(numRegisteredClasses > 0);

  Class *buffer = (Class *)malloc(sizeof(Class) * numRegisteredClasses);
  objc_getClassList(buffer, numRegisteredClasses);

  errs() << "OBJC CREATED CLASSES finalizeMemory: " << numRegisteredClasses << "\n";
  for (int i = 0; i < numRegisteredClasses; i++) {
//    errs() << "created class: " << class_getName(buffer[i]) << "\n";
  }

  bool success = SectionMemoryManager::finalizeMemory(ErrMsg);

  return success;
}

void ObjCEnabledMemoryManager::registerObjC() {
  errs() << "MullMemoryManager::registerObjC()" << "\n";

  for (ObjectSectionEntry &entry: objcSections) {
    errs() << entry.section << "\n";

    if (entry.section.find("__objc_selrefs") != StringRef::npos) {
      runtime.registerSelectors(entry.pointer, entry.size);
    }
  }

  for (ObjectSectionEntry &entry: objcSections) {
    if (entry.section.find("__objc_classlist") != StringRef::npos) {
      runtime.addClassesFromSection(entry.pointer, entry.size);
    }
  }

  runtime.registerClasses();

  for (ObjectSectionEntry &entry: objcSections) {
    if (entry.section.find("__objc_classrefs") != StringRef::npos) {
      runtime.addClassesFromClassRefsSection(entry.pointer, entry.size);
    }
  }

  for (ObjectSectionEntry &entry: objcSections) {
    if (entry.section.find("__objc_superrefs") != StringRef::npos) {
      runtime.addClassesFromSuperclassRefsSection(entry.pointer, entry.size);
    }
  }

  for (ObjectSectionEntry &entry: objcSections) {
    if (entry.section.find("__objc_catlist") != StringRef::npos) {
      runtime.addCategoriesFromSection(entry.pointer, entry.size);
    }
  }
}
