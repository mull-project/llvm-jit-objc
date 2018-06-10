  //#include "ObjCEnabledMemoryManager.h"

#include "ObjCEnabledMemoryManager.h"
#include "ObjCResolver.h"
#include "ObjCRuntime.h"
#include "TestHelpers.h"
#include "ObjCRuntimeHelpers.h"
#include "SwiftRuntimeSetup.h"

#include <objc/message.h>

#include <gtest/gtest.h>

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>
#include <llvm/ExecutionEngine/Orc/OrcError.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallSite.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

#include <fstream>

using namespace llvm;
using namespace llvm::orc;

static
llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter getMemoryManager() {
  llvm::orc::RTDyldObjectLinkingLayer::MemoryManagerGetter GetMemMgr =
  []() {
    return std::make_shared<SectionMemoryManager>();
  };
  return GetMemMgr;
}

#include <dirent.h>

std::vector<std::string> loadBitcodeFileListFromPath(std::string path) {
  std::vector<std::string> bitcodePaths;

  std::ifstream ifs(path);

  for (std::string path; getline(ifs, path); ) {
    if (path.at(0) == '#') {
      continue;
    }

    bitcodePaths.push_back(path);
  }

  assert(bitcodePaths.size() > 0);

  return bitcodePaths;
}

static void createDummyFunction(StringRef Name, llvm::Module &M) {
   auto &Context = M.getContext();
   Function *F = cast<Function>(M.getOrInsertFunction(
     Name, FunctionType::get(Type::getVoidTy(Context), false)));
   BasicBlock *BB = BasicBlock::Create(Context, "entry", F);
  new UnreachableInst(Context, BB);
}

TEST(DISABLED_Sandbox, Test) {
    // These lines are needed for TargetMachine TM to be created correctly.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  SwiftRuntimeSetup::loadSwiftLibraries();

  llvm::LLVMContext llvmContext;

  RTDyldObjectLinkingLayer objectLayer(getMemoryManager());

  std::unique_ptr<TargetMachine> TM(
                                    EngineBuilder().selectTarget(llvm::Triple(),
                                                                 "",
                                                                 "",
                                                                 SmallVector<std::string, 1>())
                                    );

  TM->setOptLevel(CodeGenOpt::Level::None);

  assert(TM.get());
  SimpleCompiler compiler(*TM);

  std::shared_ptr<ObjCResolver> objcResolver = std::make_shared<ObjCResolver>();

  std::vector<std::string> bitcodeFilePaths = loadBitcodeFileListFromPath("/opt/bitcode-targets/swift-protobuf-bitcode/bitcode.list.yml");

  auto resolver = llvm::orc::createLambdaResolver(
    [&](std::string name) {
      if (JITSymbol js = objectLayer.findSymbol(name, false)) {
        return js;
      }

      if (auto SymAddr = RTDyldMemoryManager::getSymbolAddressInProcess(name)) {
        return JITSymbol(SymAddr, JITSymbolFlags::Exported);
      }

      return JITSymbol(nullptr);
    },
    [&](std::string name) {
      return JITSymbol(nullptr);
    }
  );

  std::vector<llvm::orc::RTDyldObjectLinkingLayer::ObjHandleT> handles;
  for (std::string &bitcodeFilePath: bitcodeFilePaths) {
    errs() << "loading bitcode path: " << bitcodeFilePath << "\n";
    auto bitcodeModule = loadModuleAtPath(bitcodeFilePath, llvmContext);

    createDummyFunction("dummy", *bitcodeModule);

    RTDyldObjectLinkingLayer::ObjectPtr compiledModule =
      std::make_shared<object::OwningBinary<object::ObjectFile>>(compiler(*bitcodeModule));
    assert(compiledModule);
    assert(compiledModule->getBinary());
    assert(compiledModule->getBinary()->isMachO());

    llvm::orc::RTDyldObjectLinkingLayer::ObjHandleT handle = objectLayer.addObject(compiledModule, resolver).get();
    handles.push_back(handle);
//    Error err = objectLayer.emitAndFinalize(handle);
//
//    std::string cacheName("/tmp/_Sandbox.o");
//    std::error_code EC;
//    raw_fd_ostream outfile(cacheName, EC, sys::fs::F_None);
//    outfile.write(compiledModule->getBinary()->getMemoryBufferRef().getBufferStart(),
//                  compiledModule->getBinary()->getMemoryBufferRef().getBufferSize());
//    outfile.close();
  }

  for (uint32_t i = 0; i < handles.size(); i++) {
    auto &handle = handles.at(i);
    errs() << "Resolving handle: " << handle->get() << " / " << i << "\n";

    auto symb = (*handle)->getSymbol("_dummy", false);
    assert(symb.getAddress().get() > 0);
      //    Error err = objectLayer.emitAndFinalize(handle);
//    //assert(!err);
  }

  std::string functionName = "_CustomXCTestRunnerRunAll";
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
  EXPECT_EQ(result, 0);

//  void *runnerPtr = sys::DynamicLibrary::SearchForAddressOfSymbol("CustomXCTestRunnerRunAll");
//  auto runnerFPtr = ((int (*)(void))runnerPtr);
//  if (runnerFPtr == nullptr) {
//    errs() << "Could not find CustomXCTestRunner function: CustomXCTestRunnerRunAll()" << "\n";
//    exit(1);
//  }
//  int result = runnerFPtr();
//  ASSERT_EQ(result, 0);
}

