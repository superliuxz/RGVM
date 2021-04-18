//
// Created by William Liu on 2021-04-15.
//

#include "instructions.h"

#include <iostream>

namespace RGVM {

namespace {

// Tracks the global state of the compiler: current instruction idx and saved
// paren index.
struct State {
  unsigned pc = 0;
  unsigned saved = 0;
};

Instruction CreateInstr(Opcode op, char c, unsigned x, unsigned y, unsigned j,
                        unsigned s) {
  Instruction instr{};
  instr.opcode = op;
  instr.c = c;
  instr.x = x;
  instr.y = y;
  instr.jmp = j;
  instr.saved = s;
  return instr;
}
}  // namespace

bool operator==(const Instruction& a, const Instruction& b) {
  return a.opcode == b.opcode && a.x == b.x && a.y == b.y && a.jmp == b.jmp &&
         a.saved == b.saved;
}

unsigned Count(const RegexPtr& rp) {
  if (rp == nullptr) return 0;
  switch (rp->type) {
    case Alt:
      return 2 + Count(rp->left) + Count(rp->right);
    case Concat:
      return Count(rp->left) + Count(rp->right);
    case Lit:  // Fall through on purpose.
    case Dot:
      return 1;
    case Plus:  // Fall through on purpose.
    case Quest:
      return 1 + Count(rp->left);
    case Paren:  // Fall through on purpose.
    case Star:
      return 2 + Count(rp->left);
    default:  // Not reachable.
      assert(false);
  }
  // Not reachable.
  assert(false);
}

Instruction SplitInstr(unsigned x, unsigned y) {
  return CreateInstr(Opcode::Split, 0, x, y, 0, 0);
}

Instruction JmpInstr(unsigned j) {
  return CreateInstr(Opcode::Jmp, 0, 0, 0, j, 0);
}

Instruction CharInstr(char c) {
  return CreateInstr(Opcode::Char, c, 0, 0, 0, 0);
}

Instruction AnyInstr() { return CreateInstr(Opcode::Any, 0, 0, 0, 0, 0); }

Instruction SaveInstr(unsigned saved) {
  return CreateInstr(Opcode::Save, 0, 0, 0, 0, saved);
}

Instruction MatchInstr() { return CreateInstr(Opcode::Match, 0, 0, 0, 0, 0); }

void CompileImpl(const RegexPtr& rp, State& st,
                 std::vector<Instruction>& instructions) {
  if (rp == nullptr) return;

  unsigned& pc = st.pc;
  unsigned& saved = st.saved;

  switch (rp->type) {
    case Alt: {
      unsigned idx = pc++;
      CompileImpl(rp->left, st, instructions);
      instructions[idx] = SplitInstr(idx + 1, pc + 1);

      idx = pc++;
      CompileImpl(rp->right, st, instructions);
      instructions[idx] = JmpInstr(pc);

      break;
    }
    case Concat:
      CompileImpl(rp->left, st, instructions);
      CompileImpl(rp->right, st, instructions);
      break;
    case Lit:
      instructions[pc++] = CharInstr(rp->c);
      break;
    case Dot:
      instructions[pc++] = AnyInstr();
      break;
    case Paren: {
      unsigned old_saved = saved;
      saved += 2;  // must increment saved in st before the recursion.
      instructions[pc++] = SaveInstr(old_saved);
      CompileImpl(rp->left, st, instructions);
      instructions[pc++] = SaveInstr(old_saved + 1);

      break;
    }
    case Star: {
      unsigned idx = pc++;
      CompileImpl(rp->left, st, instructions);
      instructions[pc++] = JmpInstr(idx);
      instructions[idx] = SplitInstr(idx + 1, pc);

      break;
    }
    case Plus: {
      unsigned idx = pc;
      CompileImpl(rp->left, st, instructions);
      ++pc;
      instructions[pc - 1] = SplitInstr(idx, pc);

      break;
    }

    case Quest: {
      unsigned idx = pc++;
      CompileImpl(rp->left, st, instructions);
      instructions[idx] = SplitInstr(idx + 1, pc);

      break;
    }
  }
}

std::vector<Instruction> Compile(const RegexPtr& rp) {
  unsigned size = Count(rp) + 1;
  std::vector<Instruction> instructions(size);
  State st;

  CompileImpl(rp, st, instructions);
  instructions.back() = MatchInstr();

  return instructions;
}

void PrintInstructions(const std::vector<Instruction>& instructions) {
  for (auto i = 0; i < instructions.size(); ++i) {
    const auto& instr = instructions[i];
    std::cout << "I" << i << ": ";
    switch (instr.opcode) {
      case Opcode::Any:
        std::cout << "ANY";
        break;
      case Opcode::Char:
        std::cout << "CHAR '" << instr.c << "'";
        break;
      case Opcode::Jmp:
        std::cout << "JMP I" << instr.jmp;
        break;
      case Opcode::Match:
        std::cout << "MATCH";
        break;
      case Opcode::Save:
        std::cout << "SAVE " << instr.saved;
        break;
      case Opcode::Split:
        std::cout << "SPLIT I" << instr.x << " I" << instr.y;
        break;
      default:
        assert(false);
    }
    std::cout << std::endl;
  }
}

}  // namespace RGVM
