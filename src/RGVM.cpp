//
// Created by William Liu on 2021-04-17.
//

#include "RGVM.h"

#include <queue>
#include <string>

namespace RGVM {

namespace {

// See https://swtch.com/~rsc/regexp/regexp2.html "Ambiguous Submatching".
//
// TLDR: this recursive function mimics the behavior of backtrack implementation
// who respect the thread order, which allows us to implement the greedy
// matching.
void AddThread(const std::vector<Instruction>& instructions,
               std::string::const_iterator str_itr, bool greedy,
               Thread&& thread, std::queue<Thread>& queue) {
  queue.emplace(std::move(thread));
  auto& t = queue.back();
  const auto& instruction = instructions[t.pc];

  switch (instruction.opcode) {
    case Jmp:
      AddThread(instructions, str_itr, greedy, t.Fork(instruction.jmp), queue);
      break;

    case Split:
      if (greedy) {
        AddThread(instructions, str_itr, greedy, t.Fork(instruction.x), queue);
        AddThread(instructions, str_itr, greedy, t.Fork(instruction.y), queue);
      } else {
        AddThread(instructions, str_itr, greedy, t.Fork(instruction.y), queue);
        AddThread(instructions, str_itr, greedy, t.Fork(instruction.x), queue);
      }
      break;

    case Save:
      if (t.saved.size() <= instruction.saved)
        t.saved.resize(instruction.saved + 1);
      t.saved[instruction.saved] = str_itr;
      AddThread(instructions, str_itr, greedy, t.Fork(t.pc + 1), queue);
      break;

    // Handled in the main loop.
    case Char:
    case Any:
    case Match:
      break;
    default:
      assert(false);
  }
}
}  // namespace

bool VM::Compile(const std::string& regexp) {
  if (!RGVM::Parse(regexp, regex_root_)) return false;
  instructions_ = RGVM::Compile(regex_root_);
  return true;
}

std::vector<std::string> VM::ConstructCaptures(const Thread& thread, bool ok) {
  std::vector<std::string> captures;
  // Update the captured substrings if:
  // 1. !ok
  // 2. ok && current begin > thread begin (new substring starts earlier than
  //    current)
  // 3. ok && current begin == thread begin && current end < thread end (new
  //    substring ends later than current)
  if (ok) {
    if (begin_ < thread.begin) return captures;
    if (begin_ == thread.begin && end_ >= thread.end) return captures;
  }
  begin_ = thread.begin;
  end_ = thread.end;
  for (unsigned j = 0; j < thread.saved.size(); j += 2) {
    captures.emplace_back(thread.saved[j], thread.saved[j + 1]);
  }
  return captures;
}

bool VM::Search(const std::string& target_string) {
  std::queue<Thread> current;
  bool ok = false;

  // <= because we need one extra iteration to complete all the threads in
  // |current| queue.
  for (unsigned i = 0; i <= target_string.size(); ++i) {
    auto str_itr = (target_string.cbegin() + i);
    std::queue<Thread> next;

    if (i < target_string.size()) current.emplace(Thread(0, i, i, {}));

    while (!current.empty()) {
      // Once we have found a match in the current queue, we can skip all the
      // low priority threads in the queue.
      bool matched = false;
      auto& thread = current.front();

      const auto& instruction = instructions_[thread.pc];
      switch (instruction.opcode) {
        case Match: {
          std::vector<std::string> temp = ConstructCaptures(thread, ok);

          ok = true;
          matched = true;
          if (!temp.empty()) captures_ = std::move(temp);

          break;
        }

        case Char:
          if (target_string[i] == instruction.c) {
            ++thread.end;
            next.emplace(thread.Fork(thread.pc + 1));
          }
          break;

        case Any:
          ++thread.end;
          next.emplace(thread.Fork(thread.pc + 1));
          break;

        case Jmp:
          AddThread(instructions_, str_itr, greedy_,
                    thread.Fork(instruction.jmp), current);
          break;

        case Split:
          if (greedy_) {
            AddThread(instructions_, str_itr, greedy_,
                      thread.Fork(instruction.x), current);
            AddThread(instructions_, str_itr, greedy_,
                      thread.Fork(instruction.y), current);
          } else {
            AddThread(instructions_, str_itr, greedy_,
                      thread.Fork(instruction.y), current);
            AddThread(instructions_, str_itr, greedy_,
                      thread.Fork(instruction.x), current);
          }
          break;

        case Save:
          if (thread.saved.size() <= instruction.saved)
            thread.saved.resize(instruction.saved + 1);
          thread.saved[instruction.saved] = (target_string.cbegin() + i);
          AddThread(instructions_, str_itr, greedy_, thread.Fork(thread.pc + 1),
                    current);
          break;

        default:
          assert(false);
      }
      current.pop();
      // Break out the while loop if current thread is a match.
      if (matched) break;
    }  // While
    current = std::move(next);
  }  // For
  return ok;
}

}  // namespace RGVM
