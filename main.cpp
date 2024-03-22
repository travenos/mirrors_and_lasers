#include "safe_checker.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
#include <stdexcept>

static_assert (std::numeric_limits<std::uint32_t>::max() > 1000000U,
               "std::uint32_t size is not sufficient");
static_assert (std::numeric_limits<std::int32_t>::max() > 1000000U,
               "std::int32_t size is not sufficient");
static_assert (std::numeric_limits<std::int32_t>::max() < std::numeric_limits<std::size_t>::max(),
               "max value of std::int32_t should be lower than max_value of std::size_t");

namespace {

constexpr std::int32_t MAX_SIDE{1000000};
constexpr std::int32_t MAX_MIRRORS{200000};

std::vector<mirrors_lasers::Point> input_mirrors(std::int32_t count, std::int32_t r, std::int32_t c)
{
  std::vector<mirrors_lasers::Point> mirrors;
  mirrors.reserve(static_cast<std::size_t>(count));
  std::int32_t ri{};
  std::int32_t ci{};
  for (std::int32_t i = 0; i < count; ++i) {
    std::cin >> ri >> ci;
    if (ri < 1 || ri > r) {
      throw std::invalid_argument("Incorrect ri value");
    }
    if (ci < 1 || ci > c) {
      throw std::invalid_argument("Incorrect ci value");
    }
    mirrors_lasers::Point mirror{static_cast<std::uint32_t>(ri), static_cast<std::uint32_t>(ci)};
    mirrors.push_back(mirror);
  }
  return mirrors;
}

}  // namespace

int main()
{
  std::int32_t r{};
  std::int32_t c{};
  std::int32_t m{};
  std::int32_t n{};
  std::cin >> r >> c >> m >> n;
  if (r < 1 || r > MAX_SIDE) {
    std::cerr << "Incorrect r value" << std::endl;
    return EXIT_FAILURE;
  }
  if (c < 1 || c > MAX_SIDE) {
    std::cerr << "Incorrect c value" << std::endl;
    return EXIT_FAILURE;
  }
  if (m < 0 || m > MAX_MIRRORS) {
    std::cerr << "Incorrect m value" << std::endl;
    return EXIT_FAILURE;
  }
  if (n < 0 || n > MAX_MIRRORS) {
    std::cerr << "Incorrect n value" << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<mirrors_lasers::Point> left_to_up_mirrors = input_mirrors(m, r, c);
  std::vector<mirrors_lasers::Point> left_to_down_mirrors = input_mirrors(n, r, c);

  const mirrors_lasers::SafeChecker checker{static_cast<std::uint32_t>(r),
                                            static_cast<std::uint32_t>(c),
                                            left_to_up_mirrors,
                                            left_to_down_mirrors};

  const mirrors_lasers::SafeCheckResult check_result = checker.check_safe();

  if (check_result.result_type == mirrors_lasers::SafeCheckResultType::OpensWithoutInserting) {
    std::cout << 0 << std::endl;
  } else if (check_result.result_type == mirrors_lasers::SafeCheckResultType::CanNotBeOpened) {
    std::cout << -1 << std::endl;
  } else if (check_result.result_type == mirrors_lasers::SafeCheckResultType::RequiresMirrorInsertion) {
    std::cout << check_result.positions << " " << check_result.mirror_row << " " << check_result.mirror_col << std::endl;
  }

  return EXIT_SUCCESS;
}
