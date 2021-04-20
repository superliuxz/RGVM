//
// Created by William Liu on 2021-04-07.
//

#include <RGVM.h>

#include <iostream>

int main() {
  const std::string regexp = "(23*)4(5+)";
  const std::string target_string = "a22222333345555555b";

  // By default, the VM is greedy.
  RGVM::VM vm;

  if (!vm.Compile(regexp)) {
    std::cerr << regexp << " fails to compile..." << std::endl;
    return 1;
  }

  if (vm.Search(target_string))
    std::cout << "Search " << regexp << " in " << target_string << " successful"
              << std::endl;
  else
    std::cout << "Search " << regexp << " in " << target_string << " failed"
              << std::endl;

  // If search fails, the capture is always empty.
  if (!vm.Captures().empty()) {
    std::cout << std::endl << "Greedy..." << std::endl;
    std::cout << "Captures:" << std::endl;
    for (const auto& capture : vm.Captures()) std::cout << capture << std::endl;
  }

  std::cout << std::endl << "Non-greedy..." << std::endl;
  vm.SetGreedy(false);
  vm.Search(target_string);
  std::cout << "Captures:" << std::endl;
  for (const auto& capture : vm.Captures()) std::cout << capture << std::endl;

  return 0;
}
