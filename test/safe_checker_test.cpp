#include <safe_checker.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <vector>

TEST(SafeCheckerTest, TwoPossibleSolutions)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{2U, 3U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 2U}, {2U, 5U}, {4U, 2U}, {5U, 5U}};

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
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 77U}, {100U, 77U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::OpensWithoutInserting);
}

TEST(SafeCheckerTest, CanNotBeOpened)
{
  constexpr std::uint32_t R{100U};
  constexpr std::uint32_t C{100U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, OneCell)
{
  constexpr std::uint32_t R{1U};
  constexpr std::uint32_t C{1U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::OpensWithoutInserting);
}

TEST(SafeCheckerTest, OneCellWithMirror)
{
  constexpr std::uint32_t R{1U};
  constexpr std::uint32_t C{1U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{1U, 1U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, InsertMirrorInTheEnd)
{
  constexpr std::uint32_t R{3U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 4U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 3U);
  EXPECT_EQ(check_result.mirror_col, 4U);
}

TEST(SafeCheckerTest, InsertMirrorInTheEndOfFirstRow)
{
  constexpr std::uint32_t R{4U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{4U, 4U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 1U);
  EXPECT_EQ(check_result.mirror_col, 4U);
}

TEST(SafeCheckerTest, OneMirrorInEndCanNotOpen)
{
  constexpr std::uint32_t R{4U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{4U, 4U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, IntersectionInMirror)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 4U}, {3U, 4U}, {5U, 4U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, InsertMirrorInTheBegin)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{5U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 1U);
  EXPECT_EQ(check_result.mirror_col, 1U);
}

TEST(SafeCheckerTest, MirrorInLowerCornerCanNotBeOpened)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{5U, 1U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, MirrorsInCornersOpensWithoutInsertion)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{5U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}, {5U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::OpensWithoutInserting);
}

TEST(SafeCheckerTest, MirrorsInCornersInsertInLeftBottomCorner)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{5U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 5U);
  EXPECT_EQ(check_result.mirror_col, 1U);
}

TEST(SafeCheckerTest, TwoPossibleSolutions2)
{
  constexpr std::uint32_t R{4U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{4U, 3U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 2U}, {4U, 2U}, {2U, 4U}, {4U, 4U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 2U);
  EXPECT_EQ(check_result.mirror_row, 2U);
  EXPECT_EQ(check_result.mirror_col, 2U);
}

TEST(SafeCheckerTest, MirrorsInRightCornersOpensWithoutInsertion)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{5U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 5U}, {5U, 5U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::OpensWithoutInserting);
}

TEST(SafeCheckerTest, MultipleMirrorsNotOpens)
{
  constexpr std::uint32_t R{4U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{1U, 2U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}, {1U, 4U}, {4U, 4U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, OneInsertionExample)
{
  constexpr std::uint32_t R{3U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{1U, 3U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{3U, 2U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 1U);
  EXPECT_EQ(check_result.mirror_col, 2U);
}

TEST(SafeCheckerTest, InsertToTheBegin)
{
  constexpr std::uint32_t R{4U};
  constexpr std::uint32_t C{4U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{1U, 4U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{4U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 1U);
  EXPECT_EQ(check_result.mirror_col, 1U);
}

TEST(SafeCheckerTest, OneColumnRequiresInsertion)
{
  constexpr std::uint32_t R{2U};
  constexpr std::uint32_t C{1U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 2U);
  EXPECT_EQ(check_result.mirror_col, 1U);
}

TEST(SafeCheckerTest, OneColumnCanNotOpen)
{
  constexpr std::uint32_t R{2U};
  constexpr std::uint32_t C{1U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{1U, 1U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, OneColumnOpensWithoutInsertion)
{
  constexpr std::uint32_t R{2U};
  constexpr std::uint32_t C{1U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}, {2U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::OpensWithoutInserting);
}

TEST(SafeCheckerTest, LongRayTracing)
{
  constexpr std::uint32_t R{6U};
  constexpr std::uint32_t C{7U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{2U, 2U}, {2U, 6U}, {4U, 2U}, {4U, 6U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 6U}, {3U, 2U}, {3U, 6U}, {5U, 6U}, {6U, 6U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 5U);
  EXPECT_EQ(check_result.mirror_col, 2U);
}

TEST(SafeCheckerTest, LongRayTracingNearSides)
{
  constexpr std::uint32_t R{6U};
  constexpr std::uint32_t C{7U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{2U, 1U}, {2U, 7U}, {4U, 1U}, {4U, 7U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 7U}, {3U, 1U}, {3U, 7U}, {5U, 7U}, {6U, 7U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 5U);
  EXPECT_EQ(check_result.mirror_col, 1U);
}

TEST(SafeCheckerTest, LongRayTracingMultipleIntersections)
{
  constexpr std::uint32_t R{6U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{2U, 2U}, {2U, 6U}, {4U, 2U}, {4U, 6U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 6U}, {3U, 2U}, {3U, 6U}, {5U, 2U}, {6U, 3U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 5U);
  EXPECT_EQ(check_result.mirror_row, 1U);
  EXPECT_EQ(check_result.mirror_col, 3U);
}

TEST(SafeCheckerTest, MirrorsInOppositeCorners)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{5U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 5U}, {5U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 2U);
  EXPECT_EQ(check_result.mirror_row, 1U);
  EXPECT_EQ(check_result.mirror_col, 1U);
}

TEST(SafeCheckerTest, MirrorsInOppositeCornersNotOpens)
{
  constexpr std::uint32_t R{5U};
  constexpr std::uint32_t C{5U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}, {5U, 5U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, DoubleReflection)
{
  constexpr std::uint32_t R{6U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{1U, 4U}, {4U, 4U}, {4U, 6U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 2U}, {4U, 2U}, {1U, 6U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 1U);
  EXPECT_EQ(check_result.mirror_row, 6U);
  EXPECT_EQ(check_result.mirror_col, 4U);
}

TEST(SafeCheckerTest, NoMirrors)
{
  constexpr std::uint32_t R{10U};
  constexpr std::uint32_t C{10U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, IncorrectColumn)
{
  constexpr std::uint32_t R{3U};
  constexpr std::uint32_t C{1U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{3U, 1U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 1U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::CanNotBeOpened);
}

TEST(SafeCheckerTest, LargeField)
{
  constexpr std::uint32_t R{9000000U};
  constexpr std::uint32_t C{9500000U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{5600000U, 3200011U}, {5600000U, 4500000U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 4500000U}, {7700025U, 6700000U},
                                                                {8912398U, 3200011U}, {9000000U, 6700000U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 2U);
  EXPECT_EQ(check_result.mirror_row, 7700025U);
  EXPECT_EQ(check_result.mirror_col, 3200011U);
}

TEST(SafeCheckerTest, MultipleHorizontalIntersections)
{
  constexpr std::uint32_t R{6U};
  constexpr std::uint32_t C{6U};
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{{6U, 3U}, {1U, 3U}};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{{1U, 2U}, {1U, 4U}, {3U, 6U}, {6U, 6U},
                                                                {6U, 2U}};

  const mirrors_lasers::SafeChecker checker{R, C, left_to_up_mirrors, left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();
  ASSERT_EQ(check_result.result_type, mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion);
  EXPECT_EQ(check_result.positions, 3U);
  EXPECT_EQ(check_result.mirror_row, 3U);
  EXPECT_EQ(check_result.mirror_col, 2U);
}

TEST(SafeCheckerTest, ZeroSpace)
{
  const std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  const std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  EXPECT_THROW((mirrors_lasers::SafeChecker{0U, 1U, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);

  EXPECT_THROW((mirrors_lasers::SafeChecker{1U, 0U, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);

  EXPECT_THROW((mirrors_lasers::SafeChecker{0U, 0U, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
}

TEST(SafeCheckerTest, IncorrectMirrorsPositions)
{
  constexpr std::uint32_t R{6U};
  constexpr std::uint32_t C{5U};
  std::vector<mirrors_lasers::Point> left_to_up_mirrors{};
  std::vector<mirrors_lasers::Point> left_to_down_mirrors{};

  left_to_up_mirrors.push_back(mirrors_lasers::Point{0U, 1U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_up_mirrors.clear();

  left_to_down_mirrors.push_back(mirrors_lasers::Point{0U, 1U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_down_mirrors.clear();

  left_to_up_mirrors.push_back(mirrors_lasers::Point{7U, 1U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_up_mirrors.clear();

  left_to_down_mirrors.push_back(mirrors_lasers::Point{7U, 1U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_down_mirrors.clear();

  left_to_up_mirrors.push_back(mirrors_lasers::Point{1U, 0U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_up_mirrors.clear();

  left_to_down_mirrors.push_back(mirrors_lasers::Point{1U, 0U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_down_mirrors.clear();

  left_to_up_mirrors.push_back(mirrors_lasers::Point{1U, 6U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_up_mirrors.clear();

  left_to_down_mirrors.push_back(mirrors_lasers::Point{1U, 6U});
  EXPECT_THROW((mirrors_lasers::SafeChecker{R, C, left_to_up_mirrors, left_to_down_mirrors}),
               std::invalid_argument);
  left_to_down_mirrors.clear();
}
