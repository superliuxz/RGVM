#include "parser.h"

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iostream>

namespace RGVM {

using namespace boost::spirit;
namespace phx = boost::phoenix;

namespace {
RegexPtr CreateRegexNode(RegexType type, char ch, RegexPtr left,
                         RegexPtr right) {
  RegexPtr rp = std::make_shared<RegexNode>();
  rp->type = type;
  rp->c = ch;
  rp->left = std::move(left);
  rp->right = std::move(right);
  return rp;
}
}  // namespace

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
  return CreateRegexNode(RegexType::Alt, 0, std::move(left), std::move(right));
}

RegexPtr ConcatRegex(RegexPtr left, RegexPtr right) {
  return CreateRegexNode(RegexType::Concat, 0, std::move(left),
                         std::move(right));
}

RegexPtr StarRegex(RegexPtr left) {
  return CreateRegexNode(RegexType::Star, 0, std::move(left), nullptr);
}

RegexPtr PlusRegex(RegexPtr left) {
  return CreateRegexNode(RegexType::Plus, 0, std::move(left), nullptr);
}

RegexPtr QuestRegex(RegexPtr left) {
  return CreateRegexNode(RegexType::Quest, 0, std::move(left), nullptr);
}

RegexPtr ParenRegex(RegexPtr left) {
  return CreateRegexNode(RegexType::Paren, 0, std::move(left), nullptr);
}

// Lit and Dot are leaves.
RegexPtr LitRegex(char c) {
  return CreateRegexNode(RegexType::Lit, c, nullptr, nullptr);
}

RegexPtr DotRegex() {
  return CreateRegexNode(RegexType::Dot, 0, nullptr, nullptr);
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
    alt = (concat >> '|' >> alt)[_val = phx::bind(AltRegex, _1, _2)] |
          concat[_val = _1];

    concat = (repeat >> concat)[_val = phx::bind(ConcatRegex, _1, _2)] |
             repeat[_val = _1];

    repeat = (single >> '*')[_val = phx::bind(StarRegex, _1)] |
             (single >> '+')[_val = phx::bind(PlusRegex, _1)] |
             (single >> '?')[_val = phx::bind(QuestRegex, _1)] |
             single[_val = _1];

    single = ('(' >> alt >> ')')[_val = phx::bind(ParenRegex, _1)] |
             (qi::alnum)[_val = phx::bind(LitRegex, _1)] |
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
