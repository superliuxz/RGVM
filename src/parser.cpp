#include "parser.h"

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iostream>

namespace RGVM {

using namespace boost::spirit;
using namespace boost::phoenix;

bool operator==(const RegexPtr& a, const RegexPtr& b) {
  if (!a.get() && !b.get()) return true;
  if (!a.get() ^ !b.get()) return false;
  if (a->type != b->type) return false;

  switch (a->type) {
    case Alt:
    case Concat:
      return a->left == b->left && a->right == b->right;
    case Lit:
      return a->c == b->c;
    case Dot:  // always true.
      return true;
    case Paren:
    case Star:
    case Plus:
    case Quest:
      return a->left == b->left;
    default:
      // Not reachable.
      assert(false);
  }
  // Not reachable.
  assert(false);
}

RegexPtr AltRegex(RegexPtr left, RegexPtr right) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Alt;
  rp->left = std::move(left);
  rp->right = std::move(right);
  return rp;
}

RegexPtr ConcatRegex(RegexPtr left, RegexPtr right) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Concat;
  rp->left = std::move(left);
  rp->right = std::move(right);
  return rp;
}

RegexPtr StarRegex(RegexPtr left) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Star;
  rp->left = std::move(left);
  return rp;
}

RegexPtr PlusRegex(RegexPtr left) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Plus;
  rp->left = std::move(left);
  return rp;
}

RegexPtr QuestRegex(RegexPtr left) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Quest;
  rp->left = std::move(left);
  return rp;
}

RegexPtr ParenRegex(RegexPtr left) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Paren;
  rp->left = std::move(left);
  return rp;
}

// Lit and Dot are leaves.
RegexPtr LitRegex(char c) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Lit;
  rp->c = c;
  return rp;
}

RegexPtr DotRegex() {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = RegexType::Dot;
  return rp;
}

void PrintRegexpImpl(const RegexPtr& rp, int depth) {
  if (rp == nullptr) return;
  std::cout << std::string(depth * 4, ' ') << "|-- ";
  switch (rp->type) {
    case RegexType::Alt:
      std::cout << "Alt" << std::endl;
      PrintRegexpImpl(rp->left, depth + 1);
      PrintRegexpImpl(rp->right, depth + 1);
      break;
    case RegexType::Concat:
      std::cout << "Concat" << std::endl;
      PrintRegexpImpl(rp->left, depth + 1);
      PrintRegexpImpl(rp->right, depth + 1);
      break;
    case RegexType::Plus:
      std::cout << "Plus" << std::endl;
      PrintRegexpImpl(rp->left, depth + 1);
      break;
    case RegexType::Star:
      std::cout << "Star" << std::endl;
      PrintRegexpImpl(rp->left, depth + 1);
      break;
    case RegexType::Quest:
      std::cout << "Quest" << std::endl;
      PrintRegexpImpl(rp->left, depth + 1);
      break;
    case RegexType::Paren:
      std::cout << "Paren" << std::endl;
      PrintRegexpImpl(rp->left, depth + 1);
      break;
    case RegexType::Lit:
      std::cout << "Lit '" << rp->c << "'" << std::endl;
      break;
    case RegexType::Dot:
      std::cout << "Dot" << std::endl;
    default:
      assert(false);
  }
}

void PrintRegexpAST(const RegexPtr& rp) { PrintRegexpImpl(rp, 0); }

// Regular expression grammar.
template <typename Iterator>
struct RegexGrammar : qi::grammar<Iterator, RegexPtr()> {
  qi::rule<Iterator, RegexPtr()> alt;
  qi::rule<Iterator, RegexPtr()> concat;
  qi::rule<Iterator, RegexPtr()> repeat;
  qi::rule<Iterator, RegexPtr()> single;

  RegexGrammar() : RegexGrammar::base_type(alt) {
    // The order of "or" matters in each rule. Must match concatenated rule
    // before single rule.

    // clang-format off
    alt = (concat >> '|' >> alt)[_val = bind(AltRegex, _1, _2)] |
          concat[_val = _1];

    concat = (repeat >> concat)[_val = bind(ConcatRegex, _1, _2)] |
             repeat[_val = _1];

    repeat = (single >> '*')[_val = bind(StarRegex, _1)] |
             (single >> '+')[_val = bind(PlusRegex, _1)] |
             (single >> '?')[_val = bind(QuestRegex, _1)] |
             single[_val = _1];

    single = ('(' >> alt >> ')')[_val = bind(ParenRegex, _1)] |
             (qi::alnum)[_val = bind(LitRegex, _1)] |
             (qi::char_('.'))[_val = DotRegex()];
    // clang-format on
  }
};

bool Parse(const std::string& regexp, RegexPtr& rp) {
  RegexGrammar<std::string::const_iterator> grammar;
  return qi::phrase_parse(regexp.begin(), regexp.end(), grammar, ascii::space,
                          rp);
}
}  // namespace RGVM
