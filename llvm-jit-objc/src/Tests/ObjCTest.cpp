#include "ObjCEnabledMemoryManager.h"

#include "ObjCRuntime.h"
#include "ObjCResolver.h"
#include "TestHelpers.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

#include <gtest/gtest.h>

using namespace llvm;
using namespace llvm::orc;

const char *const FixturesPath = "/opt/mull-jit-lab/lab-jit-objc/fixtures/bitcode";

static
llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter getMemoryManager() {
  llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter GetMemMgr =
  []() {
    return std::make_shared<ObjCEnabledMemoryManager>();
  };
  return GetMemMgr;
}

TEST(LLVMJIT, Test001_BasicTest) {
  // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
  ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "001_minimal_test.bc");

  auto objcModule = loadModuleAtPath(fixturePath, llvmContext);

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  assert(TM.get());

  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  RTDyldObjectLinkingLayer::ObjectPtr objcCompiledModule =
    std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*objcModule));
  assert(objcCompiledModule);
  assert(objcCompiledModule->getBinary());
  assert(objcCompiledModule->getBinary()->isMachO());
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  std::string functionName = "_run";
  JITSymbol symbol = objectLayer.findSymbol(functionName, false);

  void *fpointer =
    reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress().get()));

  if (fpointer == nullptr) {
    errs() << "CustomTestRunner> Can't find pointer to function: "
    << functionName << "\n";
    exit(1);
  }

  auto runnerFunction = ((int (*)(void))(intptr_t)fpointer);

  int result = runnerFunction();
  EXPECT_EQ(result, 1234);
}

TEST(LLVMJIT, Test002_ClassMethodCall) {
  // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
                                                      "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
                                                      ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "002_calling_class_method.bc");

  auto objcModule = loadModuleAtPath(fixturePath, llvmContext);

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  assert(TM.get());

  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  RTDyldObjectLinkingLayer::ObjectPtr objcCompiledModule =
  std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*objcModule));
  assert(objcCompiledModule);
  assert(objcCompiledModule->getBinary());
  assert(objcCompiledModule->getBinary()->isMachO());
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  std::string functionName = "_run";
  JITSymbol symbol = objectLayer.findSymbol(functionName, false);

  void *fpointer =
  reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress().get()));

  if (fpointer == nullptr) {
    errs() << "CustomTestRunner> Can't find pointer to function: "
    << functionName << "\n";
    exit(1);
  }

  auto runnerFunction = ((int (*)(void))(intptr_t)fpointer);

  int result = runnerFunction();
  EXPECT_EQ(result, 1234);
}

TEST(LLVMJIT, Test003_CallingASuperMethodOnInstance) {
  // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
                                                      "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
                                                      ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "003_calling_a_super_method_on_instance.bc");

  auto objcModule = loadModuleAtPath(fixturePath, llvmContext);

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  assert(TM.get());

  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  RTDyldObjectLinkingLayer::ObjectPtr objcCompiledModule =
  std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*objcModule));
  assert(objcCompiledModule);
  assert(objcCompiledModule->getBinary());
  assert(objcCompiledModule->getBinary()->isMachO());
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  std::string functionName = "_run";
  JITSymbol symbol = objectLayer.findSymbol(functionName, false);

  void *fpointer =
  reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress().get()));

  if (fpointer == nullptr) {
    errs() << "CustomTestRunner> Can't find pointer to function: "
    << functionName << "\n";
    exit(1);
  }

  auto runnerFunction = ((int (*)(void))(intptr_t)fpointer);

  int result = runnerFunction();
  EXPECT_EQ(result, 111);
}

TEST(LLVMJIT, Test004_CallingASuperMethodOnClass) {
    // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
                                                      "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
                                                      ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "004_calling_a_super_method_on_class.bc");

  auto objcModule = loadModuleAtPath(fixturePath, llvmContext);

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  assert(TM.get());

  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  RTDyldObjectLinkingLayer::ObjectPtr objcCompiledModule =
  std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*objcModule));
  assert(objcCompiledModule);
  assert(objcCompiledModule->getBinary());
  assert(objcCompiledModule->getBinary()->isMachO());
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  std::string functionName = "_run";
  JITSymbol symbol = objectLayer.findSymbol(functionName, false);

  void *fpointer =
  reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress().get()));

  if (fpointer == nullptr) {
    errs() << "CustomTestRunner> Can't find pointer to function: "
    << functionName << "\n";
    exit(1);
  }

  auto runnerFunction = ((int (*)(void))(intptr_t)fpointer);

  int result = runnerFunction();
  EXPECT_EQ(result, 111);
}

TEST(LLVMJIT, Test005_IvarsOfClassAndSuperclass) {
    // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
  ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "005_ivars_of_class_and_superclass.bc");

  auto objcModule = loadModuleAtPath(fixturePath, llvmContext);

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  assert(TM.get());

  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  RTDyldObjectLinkingLayer::ObjectPtr objcCompiledModule =
  std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*objcModule));
  assert(objcCompiledModule);
  assert(objcCompiledModule->getBinary());
  assert(objcCompiledModule->getBinary()->isMachO());
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  std::string functionName = "_run";
  JITSymbol symbol = objectLayer.findSymbol(functionName, false);

  void *fpointer =
  reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress().get()));

  if (fpointer == nullptr) {
    errs() << "CustomTestRunner> Can't find pointer to function: "
    << functionName << "\n";
    exit(1);
  }

  auto runnerFunction = ((int (*)(void))(intptr_t)fpointer);

  int result = runnerFunction();
  EXPECT_EQ(result, 1111);
}

TEST(LLVMJIT, Test006_PropertiesOfClassAndSuperclass) {
    // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
                                                      "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
                                                      ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "006_properties_of_class_and_superclass.bc");

  auto objcModule = loadModuleAtPath(fixturePath, llvmContext);

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  assert(TM.get());

  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  RTDyldObjectLinkingLayer::ObjectPtr objcCompiledModule =
  std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*objcModule));
  assert(objcCompiledModule);
  assert(objcCompiledModule->getBinary());
  assert(objcCompiledModule->getBinary()->isMachO());
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  std::string functionName = "_run";
  JITSymbol symbol = objectLayer.findSymbol(functionName, false);

  void *fpointer =
  reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.getAddress().get()));

  if (fpointer == nullptr) {
    errs() << "CustomTestRunner> Can't find pointer to function: "
    << functionName << "\n";
    exit(1);
  }

  auto runnerFunction = ((int (*)(void))(intptr_t)fpointer);

  int result = runnerFunction();
  EXPECT_EQ(result, 1111);
}
