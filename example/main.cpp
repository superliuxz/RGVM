//
// Created by William Liu on 2021-04-07.
//

#include <RGVM.h>

#include <iostream>

int main() {
  const std::string regexp = "(23+)";
  const std::string target_string = "a233b";

  RGVM::VM vm;
  if (!vm.Compile(regexp)) {
    std::cerr << regexp << " fails to compile..." << std::endl;
    return 1;
  }

  if (!vm.Search(target_string)) {
    std::cerr << "Search failed..." << std::endl;
    return 1;
  }

  std::cout << "Search " << regexp << " in " << target_string << " successful."
            << std::endl;
  if (!vm.Captures().empty()) {
    std::cout << "Captures:" << std::endl;
    for (const auto& capture : vm.Captures()) {
      std::cout << capture << std::endl;
    }
  }

  return 0;
}
