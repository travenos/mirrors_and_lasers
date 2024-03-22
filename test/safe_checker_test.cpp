#include <safe_checker.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

TEST(SafeCheckerTest, TwoPossibleSolutions)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{6U};
  const std::vector< mirrors_lasers::Point> left_to_up_mirrors{{2U, 3U}};
  const std::vector< mirrors_lasers::Point> left_to_down_mirrors{{1U, 2U}, {2U, 5U}, {4U, 2U}, {5U, 5U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 2U);
  EXPECT_EQ(check_result.mirror_row, 4U);
  EXPECT_EQ(check_result.mirror_col, 3U);
}

TEST(SafeCheckerTest, OpenWithoutInserting)
{
  constexpr std::uint32_t R{100U};
  constexpr std::uint32_t C{100U};
  const std::vector< mirrors_lasers::Point> left_to_up_mirrors{{}};
  const std::vector< mirrors_lasers::Point> left_to_down_mirrors{{1U, 77U}, {100U, 77U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::OpensWithoutInserting);
}

TEST(SafeCheckerTest, CanNotBeOpened)
{
  constexpr std::uint32_t R{100U};
  constexpr std::uint32_t C{100U};
  const std::vector< mirrors_lasers::Point> left_to_up_mirrors{{}};
  const std::vector< mirrors_lasers::Point> left_to_down_mirrors{{}, {}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}
