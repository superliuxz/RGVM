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

  Instruction() = default;

  Instruction(const Instruction&) = delete;
  Instruction& operator=(const Instruction&) = delete;

  Instruction(Instruction&&) = default;
  Instruction& operator=(Instruction&&) = default;
};

// For testing.
bool operator==(const Instruction& a, const Instruction& b);

// For testing.
Instruction SplitInstr(unsigned x, unsigned y);
Instruction JmpInstr(unsigned j);
Instruction CharInstr(char c);
Instruction AnyInstr();
Instruction SaveInstr(unsigned saved);
Instruction MatchInstr();

// Calculates the number of instructions required, given an AST root.
unsigned Count(const RegexPtr& rp);

// Compiles the AST rooted at |rp| into a vector of instructions.
std::vector<Instruction> Compile(const RegexPtr& rp);

void PrintInstructions(const std::vector<Instruction>& instructions);
};  // namespace RGVM

#endif  // RGVM_INSTRUCTIONS_H
