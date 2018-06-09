#pragma once

#include "ObjCRuntime.h"

#include <llvm/ADT/StringRef.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

class ObjCEnabledMemoryManager: public llvm::SectionMemoryManager {

private:
  struct ObjectSectionEntry {
    uint8_t *pointer;
    uintptr_t size;
    llvm::StringRef section;

    ObjectSectionEntry(uint8_t *pointer,
                       uintptr_t size,
                       llvm::StringRef section)
                     : pointer(pointer), size(size), section(section) {}
  };

  std::vector <ObjectSectionEntry> objcSections;
  mull::objc::Runtime runtime;

  void registerObjC();

public:
  ObjCEnabledMemoryManager();

  uint8_t *allocateDataSection(uintptr_t Size,
                               unsigned Alignment,
                               unsigned SectionID,
                               llvm::StringRef SectionName,
                               bool isReadOnly) override;

  bool finalizeMemory(std::string *ErrMsg = nullptr) override;

};
