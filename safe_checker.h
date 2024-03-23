#ifndef SAFE_CHECKER
#define SAFE_CHECKER

#include <cstdint>
#include <map>
#include <vector>
#include <unordered_map>

namespace mirrors_lasers {

enum class MirrorOrientation : std::int8_t
{
  LeftToUp, // Mirror "/" //TODO!!! Change documentation format
  LeftToDown // Mirror "\"
};

using MirrorsLine = std::map<std::uint32_t, MirrorOrientation>;
using MirrorsField = std::unordered_map<std::uint32_t, MirrorsLine>;

struct RaySegment final {
  std::uint32_t first_coordinate{0U};
  std::uint32_t second_coordinate_start{0U};
  std::uint32_t second_coordinate_end{0U};
};

using RaySegments = std::vector<RaySegment>;

struct Point final {
  std::uint32_t row{0U};
  std::uint32_t col{0U};
};

struct RayState final {
  Point position{0U, 0U};
  bool is_positive{false};  // Left to right or up to down
  bool is_horizontal{false};
};

enum class SafeCheckResultType : std::int8_t {
  OpensWithoutInserting,
  CanNotBeOpened,
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
  void throw_if_out_of_bounds_(const Point& point) const;

  void trace_the_ray_(const RayState& start_state,
                      RayState& end_state,
                      RaySegments& row_wise_segments,
                      RaySegments& col_wise_segments) const;

  bool has_mirror_(const Point& point) const;

  std::vector<Point> find_intersections_(const RaySegments& forward_row_wise_segments,
                                         const RaySegments& forward_col_wise_segments,
                                         const RaySegments& backward_row_wise_segments,
                                         const RaySegments& backward_col_wise_segments) const;

  std::uint32_t rows_;
  std::uint32_t cols_;
  MirrorsField row_wise_mirrors_;
  MirrorsField col_wise_mirrors_;
};

}  // mirrors_lasers

#endif // SAFE_CHECKER
