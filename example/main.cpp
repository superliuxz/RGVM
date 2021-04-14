//
// Created by William Liu on 2021-04-07.
//

#include <parser.h>

int main() {
  const std::string regexp = "(a*b|c)+";
  RGVM::RegexPtr rp;
  assert(RGVM::Parse(regexp, rp));
  RGVM::PrintRegexpAST(rp);
  return 0;
}
