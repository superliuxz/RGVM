//
// d by William Liu on 2021-04-08.
//

#include "gtest/gtest.h"
#include "parser.h"

using namespace RGVM;

TEST(RGVM, ComparisonOperator_NULL) {
  RegexPtr a, b;
  EXPECT_TRUE(a == b);
  b = std::make_shared<RegexNode>();
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Lit) {
  RegexPtr a = LitRegex('a');
  RegexPtr b = LitRegex('a');
  EXPECT_TRUE(a == b);
  b = LitRegex('b');
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Alt) {
  RegexPtr a = AltRegex(LitRegex('a'), LitRegex('b'));
  RegexPtr b = AltRegex(LitRegex('a'), LitRegex('b'));
  EXPECT_TRUE(a == b);
  b = AltRegex(LitRegex('c'), LitRegex('b'));
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Concat) {
  RegexPtr a = ConcatRegex(LitRegex('a'), LitRegex('b'));
  RegexPtr b = ConcatRegex(LitRegex('a'), LitRegex('b'));
  EXPECT_TRUE(a == b);
  b = ConcatRegex(LitRegex('c'), LitRegex('b'));
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Dot) {
  RegexPtr a = DotRegex();
  RegexPtr b = DotRegex();
  EXPECT_TRUE(a == b);
}

TEST(RGVM, ComparisonOperator_Paren) {
  RegexPtr a = ParenRegex(LitRegex('a'));
  RegexPtr b = ParenRegex(LitRegex('a'));
  EXPECT_TRUE(a == b);
  b = ParenRegex(LitRegex('b'));
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Star) {
  RegexPtr a = StarRegex(LitRegex('a'));
  RegexPtr b = StarRegex(LitRegex('a'));
  EXPECT_TRUE(a == b);
  b = StarRegex(LitRegex('b'));
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Plus) {
  RegexPtr a = PlusRegex(LitRegex('a'));
  RegexPtr b = PlusRegex(LitRegex('a'));
  EXPECT_TRUE(a == b);
  b = PlusRegex(LitRegex('b'));
  EXPECT_TRUE(a != b);
}

TEST(RGVM, ComparisonOperator_Quest) {
  RegexPtr a = QuestRegex(LitRegex('a'));
  RegexPtr b = QuestRegex(LitRegex('a'));
  EXPECT_TRUE(a == b);
  b = QuestRegex(LitRegex('b'));
  EXPECT_TRUE(a != b);
}

TEST(RGVM, Parser_Lit) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a", a));
  EXPECT_TRUE(a == LitRegex('a'));
}

TEST(RGVM, Parser_Concat) {
  RegexPtr a;
  EXPECT_TRUE(Parse("abc", a));
  EXPECT_TRUE(a == ConcatRegex(LitRegex('a'),
                               ConcatRegex(LitRegex('b'), LitRegex('c'))));
}

TEST(RGVM, Parser_Alt) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a|b|c", a));
  EXPECT_TRUE(a ==
              AltRegex(LitRegex('a'), AltRegex(LitRegex('b'), LitRegex('c'))));
}

TEST(RGVM, Parser_Paren) {
  RegexPtr a;
  EXPECT_TRUE(Parse("(a)", a));
  EXPECT_TRUE(a == ParenRegex(LitRegex('a')));
}

TEST(RGVM, Parser_Star) {
  RegexPtr a;
  EXPECT_TRUE(Parse("(a|bc)*", a));
  EXPECT_TRUE(a ==
              StarRegex(ParenRegex(AltRegex(
                  LitRegex('a'), ConcatRegex(LitRegex('b'), LitRegex('c'))))));
}

TEST(RGVM, Parser_Plus) {
  RegexPtr a;
  EXPECT_TRUE(Parse("(a*bc)+", a));
  EXPECT_TRUE(a == PlusRegex(ParenRegex(ConcatRegex(
                       StarRegex(LitRegex('a')),
                       ConcatRegex(LitRegex('b'), LitRegex('c'))))));
}

TEST(RGVM, Parser_Quest) {
  RegexPtr a;
  EXPECT_TRUE(Parse("(a+b*c)?", a));
  EXPECT_TRUE(a == QuestRegex(ParenRegex(ConcatRegex(
                       PlusRegex(LitRegex('a')),
                       ConcatRegex(StarRegex(LitRegex('b')), LitRegex('c'))))));
}

TEST(RGVM, Parser_Dot) {
  RegexPtr a;
  EXPECT_TRUE(Parse(".+", a));
  EXPECT_TRUE(a == PlusRegex(DotRegex()));
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}