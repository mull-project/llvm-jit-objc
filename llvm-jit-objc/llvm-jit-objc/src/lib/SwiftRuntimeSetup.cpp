#include "SwiftRuntimeSetup.h"

#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace SwiftDyLibPath {
  static const char *const Core              = "libswiftCore.dylib";
  static const char *const Darwin            = "libswiftDarwin.dylib";
  static const char *const ObjectiveC        = "libswiftObjectiveC.dylib";
  static const char *const Dispatch          = "libswiftDispatch.dylib";
  static const char *const CoreFoundation    = "libswiftCoreFoundation.dylib";

  static const char *const IOKit             = "libswiftIOKit.dylib";
  static const char *const CoreGraphics      = "libswiftCoreGraphics.dylib";
  static const char *const Foundation        = "libswiftFoundation.dylib";
  static const char *const CoreData          = "libswiftCoreData.dylib";

  static const char *const XPC               = "libswiftXPC.dylib";
  static const char *const os                = "libswiftos.dylib";
  static const char *const Metal             = "libswiftMetal.dylib";

  static const char *const CoreImage         = "libswiftCoreImage.dylib";
  static const char *const QuartzCore        = "libswiftQuartzCore.dylib";
  static const char *const AppKit            = "libswiftAppKit.dylib";

  static const char *const XCTest            = "libswiftXCTest.dylib";
  static const char *const SwiftOnoneSupport = "libswiftSwiftOnoneSupport.dylib";
}

const char *const SwiftLibrariesPath = "/Applications/Xcode-9.2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/swift/macosx";

const char *const SwiftLibraries[] = {
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

void SwiftRuntimeSetup::loadFoundation() {
  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/System/Library/Frameworks/Foundation.framework/Versions/Current/Foundation"
  ));
}

void SwiftRuntimeSetup::loadXCTest() {
  assert(!sys::DynamicLibrary::LoadLibraryPermanently(
    "/Applications/Xcode-9.2.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks/XCTest.framework/XCTest"
  ));
}

void SwiftRuntimeSetup::loadSwiftLibraries() {
  char fullLibraryPath[255];
  std::string errorMessage;

  for (const char *const library: SwiftLibraries) {
    snprintf(fullLibraryPath, sizeof(fullLibraryPath), "%s/%s", SwiftLibrariesPath, library);

    if (sys::DynamicLibrary::LoadLibraryPermanently(fullLibraryPath,
                                                    &errorMessage)) {
      errs() << errorMessage << "\n";
      exit(1);
    }
  }
}
