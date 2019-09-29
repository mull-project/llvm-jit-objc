#include "llvm-jit-objc/ObjCEnabledMemoryManager.h"
#include "llvm-jit-objc/ObjCResolver.h"
#include "llvm-jit-objc/ObjCRuntime.h"
#include "llvm-jit-objc/ObjCRuntimeHelpers.h"
#include "llvm-jit-objc/SwiftRuntimeSetup.h"

#include "CustomXCTestRunnerBinding.h"
#include "TestHelpers.h"

#include <objc/message.h>
#include <gtest/gtest.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallSite.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

using namespace llvm;
using namespace llvm::orc;

static const char *const FixturesPath = "/opt/llvm-jit-objc/fixtures/bitcode";

static
llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter getMemoryManager() {
  llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter GetMemMgr =
  []() {
    return std::make_shared<ObjCEnabledMemoryManager>();
  };
  return GetMemMgr;
}

TEST(XCTest_Swift, Test_001_Minimal) {
    // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  SwiftRuntimeSetup::loadFoundation();
  SwiftRuntimeSetup::loadXCTest();
  SwiftRuntimeSetup::loadSwiftLibraries();


  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    CustomXCTestRunnerBinding::getCustomXCTestRunnerPath().c_str()
  ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "swift_001_minimal_xctestcase_run.bc");

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
  std::vector<object::ObjectFile*> objcSet;
  auto objcHandle = objectLayer.addObject(objcCompiledModule, objcResolver).get();
  assert(objcHandle->get());

  Error err = objectLayer.emitAndFinalize(objcHandle);

  id runtimeClass = (id)objc_getRequiredClass("BinarySearchTest");
  id allocatedInstance = objc_msgSend(runtimeClass, sel_registerName("alloc"));
  id initializedInstance = objc_msgSend(allocatedInstance, sel_registerName("init"));
  assert((id)object_getClass(initializedInstance) == runtimeClass);

  Ivar iv = class_getInstanceVariable((Class)runtimeClass, "searchList");
  assert(iv);
  id ivv = object_getIvar(initializedInstance, iv);
  assert(iv);

  const char *clzName = object_getClassName(ivv);
  assert(strcmp((char *)clzName, "Swift.__EmptyArrayStorage") == 0);

  void *runnerPtr = sys::DynamicLibrary::SearchForAddressOfSymbol("CustomXCTestRunnerRunAll");
  auto runnerFPtr = ((int (*)(void))runnerPtr);
  if (runnerFPtr == nullptr) {
    errs() << "Could not find CustomXCTestRunner function: CustomXCTestRunnerRunAll()" << "\n";
    exit(1);
  }
  int result = runnerFPtr();
  ASSERT_EQ(result, 0);
}
