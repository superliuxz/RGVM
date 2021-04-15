//
// Created by William Liu on 2021-04-15.
//

#ifndef RGVM_INSTRUCTIONS_H
#define RGVM_INSTRUCTIONS_H

#include <memory>
#include <vector>

#include "parser.h"

namespace RGVM {

enum Opcode { Char, Match, Jmp, Split, Any, Save };

struct Instruction {
  Opcode opcode;

  char c;          // Char
  unsigned x, y;   // Split
  unsigned jmp;    // Jmp
  unsigned saved;  // Save
};

// Calculate the number of instructions required, given an AST root.
unsigned Count(const RegexPtr& rp);

// Compile
std::vector<Instruction> Compile(const RegexPtr& rp);

void PrintInstructions(const std::vector<Instruction>& instructions);
};  // namespace RGVM

#endif  // RGVM_INSTRUCTIONS_H
