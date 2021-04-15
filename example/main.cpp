//
// Created by William Liu on 2021-04-07.
//

#include <RGVM.h>

int main() {
  const std::string regexp = "((a))";
  RGVM::RegexPtr rp;
  assert(RGVM::Parse(regexp, rp));
  RGVM::PrintRegexpAST(rp);

  const auto instructions = RGVM::Compile(rp);
  RGVM::PrintInstructions(instructions);

  return 0;
}
