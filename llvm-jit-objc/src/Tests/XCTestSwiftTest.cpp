//#include "ObjCEnabledMemoryManager.h"

#include "ObjCEnabledMemoryManager.h"
#include "ObjCResolver.h"
#include "ObjCRuntime.h"
#include "TestHelpers.h"
#include "ObjCRuntimeHelpers.h"

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

namespace SwiftDyLibPath {
  static const char *const Core =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftCore.dylib";
  static const char *const Darwin =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftDarwin.dylib";
  static const char *const ObjectiveC =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftObjectiveC.dylib";
  static const char *const Dispatch =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftDispatch.dylib";
  static const char *const CoreFoundation =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftCoreFoundation.dylib";
  static const char *const IOKit =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftIOKit.dylib";
  static const char *const CoreGraphics =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftCoreGraphics.dylib";

    //  -load=/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftFoundation.dylib
    //  -load=/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftXPC.dylib

  static const char *const Foundation =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftFoundation.dylib";
  static const char *const CoreData =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftCoreData.dylib";

  static const char *const XPC =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftXPC.dylib";
  static const char *const os =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftos.dylib";
  static const char *const Metal =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftMetal.dylib";

  static const char *const CoreImage =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftCoreImage.dylib";
  static const char *const QuartzCore =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftQuartzCore.dylib";
  static const char *const AppKit =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftAppKit.dylib";

  static const char *const XCTest =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftXCTest.dylib";
  static const char *const SwiftOnoneSupport =
  "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx/libswiftSwiftOnoneSupport.dylib";


}

static void loadSwiftLibrariesOrExit() {
  const char *const libraries[] = {
    SwiftDyLibPath::Core,
    SwiftDyLibPath::Darwin,
    SwiftDyLibPath::ObjectiveC,
    SwiftDyLibPath::Dispatch,
    SwiftDyLibPath::CoreFoundation,
    SwiftDyLibPath::IOKit,
    SwiftDyLibPath::CoreGraphics,
    SwiftDyLibPath::Foundation,
    SwiftDyLibPath::CoreData,

    SwiftDyLibPath::XPC,
    SwiftDyLibPath::os,
    SwiftDyLibPath::Metal,
    SwiftDyLibPath::CoreImage,
    SwiftDyLibPath::QuartzCore,
    SwiftDyLibPath::AppKit,

    SwiftDyLibPath::XCTest,
    SwiftDyLibPath::SwiftOnoneSupport,
  };

  for (const char *const lib: libraries) {
    std::string errorMessage;
    if (sys::DynamicLibrary::LoadLibraryPermanently(lib,
                                                    &errorMessage)) {
      errs() << errorMessage << "\n";
      exit(1);
    }
  }
}

TEST(XCTest_Swift, Test_001_Minimal) {
    // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
  ));
  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/Applications/Xcode-9.2.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks/XCTest.framework/XCTest"
  ));
  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/opt/CustomXCTestRunner/CustomXCTestRunner.dylib"
  ));
  loadSwiftLibrariesOrExit();

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
  assert(strcmp((char *)clzName, "Swift._EmptyArrayStorage") == 0);

  void *runnerPtr = sys::DynamicLibrary::SearchForAddressOfSymbol("CustomXCTestRunnerRunAll");
  auto runnerFPtr = ((int (*)(void))runnerPtr);
  if (runnerFPtr == nullptr) {
    errs() << "Could not find CustomXCTestRunner function: CustomXCTestRunnerRunAll()" << "\n";
    exit(1);
  }
  int result = runnerFPtr();
  ASSERT_EQ(result, 0);
}
