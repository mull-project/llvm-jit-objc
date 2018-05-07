#include "ObjCEnabledMemoryManager.h"

#include "ObjCResolver.h"
#include "ObjCRuntime.h"
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

static const char *const FixturesPath = "/opt/mull-jit-lab/lab-jit-objc/fixtures/bitcode";

static
llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter getMemoryManager() {
  llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter GetMemMgr =
  []() {
    return std::make_shared<ObjCEnabledMemoryManager>();
  };
  return GetMemMgr;
}

TEST(XCTest_ObjC, Test_001_Minimal) {
  // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
  ));
  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks/XCTest.framework/XCTest"
  ));
  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/opt/CustomXCTestRunner/CustomXCTestRunner.dylib"
  ));

  llvm::LLVMContext llvmContext;

  char fixturePath[255];
  snprintf(fixturePath, sizeof(fixturePath), "%s/%s", FixturesPath, "xctest_objc_001_minimal_xctestcase_run.bc");
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

  void *runnerPtr = sys::DynamicLibrary::SearchForAddressOfSymbol("CustomXCTestRunnerRun");
  auto runnerFPtr = ((int (*)(void))runnerPtr);
  int result = runnerFPtr();

  ASSERT_EQ(result, 0);
}
