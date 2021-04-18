//
// Created by William Liu on 2021-04-17.
//

#include "RGVM.h"

#include <queue>

#include "iostream"

namespace RGVM {

bool VM::Compile(const std::string& regexp) {
  if (!RGVM::Parse(regexp, regex_root_)) {
    std::cerr << "Failed to parse regular expression." << std::endl;
    return false;
  }
  instructions_ = RGVM::Compile(regex_root_);
  return true;
}

bool VM::Search(const std::string& target_string) {
  std::queue<Thread> current;
  bool ok = false;

  // <= because we need one extra iteration to complete all the threads in
  // |current| queue.
  for (auto i = 0; i <= target_string.size(); ++i) {
    std::queue<Thread> next;

    current.push(Thread(0));

    while (!current.empty()) {
      auto thread = current.front();
      current.pop();

      auto instruction = instructions_[thread.pc];
      switch (instruction.opcode) {
        case Char:
          if (target_string[i] != instruction.c) break;
          next.push(Thread(thread.pc + 1, thread.saved));
          break;
        case Match: {
          std::vector<std::string> temp;
          for (auto j = 0; j < thread.saved.size(); j += 2) {
            auto begin = target_string.cbegin() + thread.saved[j];
            auto end = target_string.cbegin() + thread.saved[j + 1];
            temp.emplace_back(begin, end);
          }
          ok = true;
          captures_ = std::move(temp);
          break;
        }
        case Jmp:
          current.push(Thread(instruction.jmp, thread.saved));
          break;
        case Split:  // TODO: properly implement greedy matching.
          current.push(Thread(instruction.x, thread.saved));
          current.push(Thread(instruction.y, thread.saved));
          break;
        case Any:
          next.push(Thread(thread.pc + 1, thread.saved));
          break;
        case Save:
          if (thread.saved.size() <= instruction.saved)
            thread.saved.resize(instruction.saved + 1);
          thread.saved[instruction.saved] = i;
          current.push(Thread(thread.pc + 1, thread.saved));
          break;
        default:
          assert(false);
      }
    }

    current = std::move(next);
  }

  return ok;
}

}  // namespace RGVM
