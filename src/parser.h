#ifndef RGVM_PARSER_H
#define RGVM_PARSER_H

#include <memory>
#include <string>

namespace RGVM {

// Supported types of regex.
enum RegexType { Alt, Concat, Lit, Dot, Paren, Star, Plus, Quest };

// AST node that represents a single regex.
struct RegexNode {
  RegexType type;
  char c;
  std::shared_ptr<RegexNode> left;
  std::shared_ptr<RegexNode> right;
};

using RegexPtr = std::shared_ptr<RegexNode>;

bool operator==(const RegexPtr& a, const RegexPtr& b);

// Declared in header for testing's convenience.
RegexPtr AltRegex(RegexPtr left, RegexPtr right);
RegexPtr ConcatRegex(RegexPtr left, RegexPtr right);
RegexPtr StarRegex(RegexPtr left);
RegexPtr PlusRegex(RegexPtr left);
RegexPtr QuestRegex(RegexPtr left);
RegexPtr ParenRegex(RegexPtr left);
// Lit and Dot are leaves of the AST.
RegexPtr LitRegex(char c);
RegexPtr DotRegex();

// Parse the regular expression string into AST using Boost. AST's root is saved
// as |rp|.
bool Parse(const std::string& regexp, RegexPtr& rp);

// Print out the parsed regexp.
void PrintRegexpAST(const RegexPtr& rp);

}  // namespace RGVM

#endif  // RGVM_PARSER_H
