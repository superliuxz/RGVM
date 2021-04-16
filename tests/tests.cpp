//
// Created by William Liu on 2021-04-08.
//

#include "RGVM.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

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

TEST(RGVM, Compiler_Concat) {
  RegexPtr a;
  EXPECT_TRUE(Parse("abc", a));
  const auto instructions = Compile(a);
  // I1: CHAR 'a'
  // I2: CHAR 'b'
  // I3: CHAR 'c'
  // I4: MATCH
  ASSERT_THAT(instructions,
              ::testing::ElementsAre(CharInstr('a'), CharInstr('b'),
                                     CharInstr('c'), MatchInstr()));
}

TEST(RGVM, Compiler_Alt) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a|b|c", a));
  const auto instructions = Compile(a);
  // I0: SPLIT I1 I3
  // I1: CHAR 'a'
  // I2: JMP I7
  // I3: SPLIT I4 I6
  // I4: CHAR 'b'
  // I5: JMP I7
  // I6: CHAR 'c'
  // I7: MATCH
  ASSERT_THAT(instructions, ::testing::ElementsAre(
                                SplitInstr(1, 3), CharInstr('a'), JmpInstr(7),
                                SplitInstr(4, 6), CharInstr('b'), JmpInstr(7),
                                CharInstr('c'), MatchInstr()));
}

TEST(RGVM, Compiler_Star) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a|b*", a));
  const auto instructions = Compile(a);
  // I0: SPLIT I1 I3
  // I1: CHAR 'a'
  // I2: JMP I6
  // I3: SPLIT I4 I6
  // I4: CHAR 'b'
  // I5: JMP I3
  // I6: MATCH
  ASSERT_THAT(instructions,
              ::testing::ElementsAre(
                  SplitInstr(1, 3), CharInstr('a'), JmpInstr(6),
                  SplitInstr(4, 6), CharInstr('b'), JmpInstr(3), MatchInstr()));
}

TEST(RGVM, Compiler_Plus) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a|b+", a));
  const auto instructions = Compile(a);
  // I0: SPLIT I1 I3
  // I1: CHAR 'a'
  // I2: JMP I5
  // I3: CHAR 'b'
  // I4: SPLIT I3 I5
  // I5: MATCH
  ASSERT_THAT(
      instructions,
      ::testing::ElementsAre(SplitInstr(1, 3), CharInstr('a'), JmpInstr(5),
                             CharInstr('b'), SplitInstr(3, 5), MatchInstr()));
}

TEST(RGVM, Compiler_Quest) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a|b?", a));
  const auto instructions = Compile(a);
  // I0: SPLIT I1 I3
  // I1: CHAR 'a'
  // I2: JMP I5
  // I3: SPLIT I4 I5
  // I4: CHAR 'b'
  // I5: MATCH
  ASSERT_THAT(
      instructions,
      ::testing::ElementsAre(SplitInstr(1, 3), CharInstr('a'), JmpInstr(5),
                             SplitInstr(4, 5), CharInstr('b'), MatchInstr()));
}

TEST(RGVM, Compiler_Any) {
  RegexPtr a;
  EXPECT_TRUE(Parse("a|.+", a));
  const auto instructions = Compile(a);
  // I0: SPLIT I1 I3
  // I1: CHAR 'a'
  // I2: JMP I5
  // I3: ANY
  // I4: SPLIT I3 I5
  // I5: MATCH
  ASSERT_THAT(instructions, ::testing::ElementsAre(
                                SplitInstr(1, 3), CharInstr('a'), JmpInstr(5),
                                AnyInstr(), SplitInstr(3, 5), MatchInstr()));
}

TEST(RGVM, Compiler_Paren) {
  RegexPtr a;
  EXPECT_TRUE(Parse("((a+))", a));
  const auto instructions = Compile(a);
  // I0: SAVE 0
  // I1: SAVE 2
  // I2: CHAR 'a'
  // I3: SPLIT I2 I4
  // I4: SAVE 3
  // I5: SAVE 1
  // I6: MATCH
  ASSERT_THAT(instructions,
              ::testing::ElementsAre(SaveInstr(0), SaveInstr(2), CharInstr('a'),
                                     SplitInstr(2, 4), SaveInstr(3),
                                     SaveInstr(1), MatchInstr()));
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}