//
// Created by Stanislav Pankevich on 2019-09-29.
//

#include "CustomXCTestRunnerBinding.h"

#include <fstream>

std::string CustomXCTestRunnerBinding::getCustomXCTestRunnerPath() {
  // Value is provided by CMake. See CMakeLists.txt.
  std::string customXCTestRunnerPath = "$<TARGET_FILE:CustomXCTestRunner>";

  assert(std::ifstream(customXCTestRunnerPath.c_str()).good());

  return customXCTestRunnerPath;
}
