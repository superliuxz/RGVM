//
// Created by William Liu on 2021-04-15.
//

#ifndef RGVM_RGVM_H
#define RGVM_RGVM_H

#include <utility>
#include <vector>

#include "instructions.h"
#include "parser.h"

namespace RGVM {

struct Thread {
  unsigned pc = 0;
  unsigned begin = 0, end = 0;  // indices of the current substring.
  std::vector<std::string::const_iterator>
      saved;  // string indices of the capturing groups.

  explicit Thread(unsigned pc) : pc(pc) {}
  Thread(unsigned pc, unsigned begin, unsigned end,
         std::vector<std::string::const_iterator> saved)
      : pc(pc), begin(begin), end(end), saved(std::move(saved)) {}
  ~Thread() = default;

  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  Thread(Thread&&) = default;
  Thread& operator=(Thread&&) = default;

  // Fork the current thread with a new PC value.
  Thread Fork(unsigned new_pc) const {
    return Thread(new_pc, begin, end, saved);
  }
};

class VM {
 public:
  VM() = default;
  ~VM() = default;

  VM(const VM&) = delete;
  VM& operator=(const VM&) = delete;

  VM(VM&&) = default;
  VM& operator=(VM&&) = default;

  // Creates the new VM, and compiles the input regular expression into
  // instructions.
  bool Compile(const std::string& regexp);

  // Searches the target string against
  bool Search(const std::string& target_string);

  void SetGreedy(bool greedy) { greedy_ = greedy; }

  const std::vector<std::string>& Captures() const { return captures_; }

 private:
  // Constructs captured strings from |target_string| and saves them into
  // |captures_|.
  std::vector<std::string> ConstructCaptures(const Thread& thread, bool ok);

  bool greedy_ = true;
  RegexPtr regex_root_;
  std::vector<Instruction> instructions_;
  // Populated if the regexp contains capture.
  std::vector<std::string> captures_;

  // Record the current matched substring. Updated whenever a MATCH state is
  // reached.
  unsigned begin_ = 0;
  unsigned end_ = 0;
};

}  // namespace RGVM

#endif  // RGVM_RGVM_H
