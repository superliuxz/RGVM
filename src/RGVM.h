//
// Created by William Liu on 2021-04-15.
//

#ifndef RGVM_RGVM_H
#define RGVM_RGVM_H

#include <vector>

#include "instructions.h"
#include "parser.h"

namespace RGVM {

struct Thread {
  unsigned pc = 0;
  std::vector<unsigned> saved;  // string indices of the capturing groups.

  explicit Thread(unsigned pc) : pc(pc) {}
  Thread(unsigned pc, std::vector<unsigned> saved)
      : pc(pc), saved(std::move(saved)) {}
};

class VM {
 public:
  // Creates the new VM, and compiles the input regular expression into
  // instructions.
  bool Compile(const std::string& regexp);

  // Searches the target string against
  bool Search(const std::string& target_string);

  const std::vector<std::string>& Captures() const { return captures_; }

 private:
  RegexPtr regex_root_;
  std::vector<Instruction> instructions_;
  std::vector<std::string> captures_;
};

}  // namespace RGVM

#endif  // RGVM_RGVM_H
