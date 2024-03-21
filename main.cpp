#include <cstdint>
#include <iostream>
#include <map>
#include <vector>
#include <limits>

static_assert (std::numeric_limits<std::uint32_t>::max() > 1000000U,
               "std::uint32_t size is not sufficient");

//TODO!!! add documentation, rewrite comments

namespace mirrors_lasers {

enum class MirrorOrientation : std::int8_t
{
  LeftToUp, // Mirror "/"
  LeftToDown // Mirror "\"
};

using MirrorsLine = std::map<std::uint32_t, MirrorOrientation>;
using MirrorsField = std::map<std::uint32_t, MirrorsLine>;

enum class Direction : std::int8_t
{
  Positive,  // Left to right or up to down
  Negative  // Right to left or down to up
};

//TODO!!!=========================================

class RaySegmentsLine final {
public:
  void add_segment(std::uint32_t start, std::uint32_t end);
  bool has_intersection(std::uint32_t orthogonal_line_position) const;

private:
  std::map<std::uint32_t, std::uint32_t> segments_map_;
};

//TODO!!!=========================================

void RaySegmentsLine::add_segment(std::uint32_t start, std::uint32_t end)
{
  segments_map_[end] = start;
}

bool RaySegmentsLine::has_intersection(std::uint32_t orthogonal_line_position) const
{
  const auto segment_iter = segments_map_.lower_bound(orthogonal_line_position);
  if (segment_iter == segments_map_.end()) {
    return false;
  }
  const std::uint32_t segment_begin = segment_iter->second;
  return (segment_begin <= orthogonal_line_position);
}

//TODO!!!=========================================

using RaySegmentsMap = std::map<std::uint32_t, RaySegmentsLine>;

struct Point final {
  std::uint32_t row{0U};
  std::uint32_t col{0U};
};

enum class SafeCheckResultType : std::int8_t {
  OpensWithoutInserting,
  CannotBeOpened,
  RequiresMirrorInsertion
};

struct SafeCheckResult final {
  SafeCheckResultType result_type{SafeCheckResultType::OpensWithoutInserting};
  std::uint32_t positions{0U};
  std::uint32_t mirror_row{0U};
  std::uint32_t mirror_col{0U};
};

class SafeChecker final {
public:
  SafeChecker(std::uint32_t rows, std::uint32_t columns,
              const std::vector<Point>& left_to_up_mirrors,
              const std::vector<Point>& left_to_down_mirrors);

  SafeCheckResult check_safe() const;

private:
  std::uint32_t rows_;
  std::uint32_t columns_;
  std::vector<Point> left_to_up_mirrors_;
  std::vector<Point> left_to_down_mirrors_;
};

//TODO!!!=========================================

SafeChecker::SafeChecker(std::uint32_t rows, std::uint32_t columns,
                         const std::vector<Point>& left_to_up_mirrors,
                         const std::vector<Point>& left_to_down_mirrors)
  : rows_{rows}
  , columns_{columns}
  , left_to_up_mirrors_{left_to_up_mirrors}
  , left_to_down_mirrors_{left_to_down_mirrors}
{
}

SafeCheckResult SafeChecker::check_safe() const
{
  return SafeCheckResult{};
}


}  // namespace mirrors_lasers

int main()
{
  // Check input range
  std::cout << "Hello World!" << std::endl;
  return 0;
}
