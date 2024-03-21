#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <vector>
#include <unordered_map>

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
using MirrorsField = std::unordered_map<std::uint32_t, MirrorsLine>;

//TODO!!!=========================================

class RaySegmentsInLine final {
public:
  void add_segment(std::uint32_t start, std::uint32_t end);
  bool has_intersection(std::uint32_t orthogonal_line_position) const;

private:
  std::map<std::uint32_t, std::uint32_t> segments_map_;
};

//TODO!!!=========================================

void RaySegmentsInLine::add_segment(std::uint32_t start, std::uint32_t end)
{
  const auto min_max_pair = std::minmax(start, end);
  segments_map_[min_max_pair.second] = min_max_pair.first;
}

bool RaySegmentsInLine::has_intersection(std::uint32_t orthogonal_line_position) const
{
  const auto segment_iter = segments_map_.lower_bound(orthogonal_line_position);
  if (segment_iter == segments_map_.end()) {
    return false;
  }
  const std::uint32_t segment_begin = segment_iter->second;
  return (segment_begin <= orthogonal_line_position);
}

//TODO!!!=========================================

using RaySegmentsMap = std::unordered_map<std::uint32_t, RaySegmentsInLine>;

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
  void trace_the_ray_(const Point& start_point,
                      bool start_direction_positive,
                      bool start_direction_horizontal,
                      Point& end_point,
                      bool& end_direction_horizontal,
                      RaySegmentsMap& row_wise_segments_map,
                      RaySegmentsMap& col_wise_segments_map) const;

  std::vector<Point> find_intersections(const RaySegmentsMap& forward_row_wise_segments_map,
                                        const RaySegmentsMap& forward_col_wise_segments_map,
                                        const RaySegmentsMap& backward_row_wise_segments_map,
                                        const RaySegmentsMap& backward_col_wise_segments_map) const;

  std::uint32_t rows_;
  std::uint32_t cols_;
  MirrorsField row_wise_mirrors_;
  MirrorsField col_wise_mirrors_;
};

//TODO!!!=========================================

SafeChecker::SafeChecker(std::uint32_t rows, std::uint32_t columns,
                         const std::vector<Point>& left_to_up_mirrors,
                         const std::vector<Point>& left_to_down_mirrors)
  : rows_{rows}
  , cols_{columns}
{
  const std::size_t mirrors_count = left_to_up_mirrors.size() + left_to_down_mirrors.size();
  row_wise_mirrors_.reserve(mirrors_count);
  col_wise_mirrors_.reserve(mirrors_count);

  // Fill the data
  for (const auto& left_to_up_mirror : left_to_up_mirrors) {
    row_wise_mirrors_[left_to_up_mirror.row][left_to_up_mirror.col] = MirrorOrientation::LeftToUp;
    col_wise_mirrors_[left_to_up_mirror.col][left_to_up_mirror.row] = MirrorOrientation::LeftToUp;
  }
  for (const auto& left_to_down_mirror : left_to_down_mirrors) {
    row_wise_mirrors_[left_to_down_mirror.row][left_to_down_mirror.col] = MirrorOrientation::LeftToDown;
    col_wise_mirrors_[left_to_down_mirror.col][left_to_down_mirror.row] = MirrorOrientation::LeftToDown;
  }
}

SafeCheckResult SafeChecker::check_safe() const
{
  SafeCheckResult result{};

  // Find ray segments of forward direction
  Point forward_start_position{1U, 1U};
  bool forward_direction_is_horizontal = true;
  bool forward_direction_is_positive = true;  // Left to right or up to down
  Point forward_end_point;
  bool forward_end_direction_horizontal = false;
  RaySegmentsMap forward_row_wise_segments_map;
  RaySegmentsMap forward_col_wise_segments_map;
  trace_the_ray_(forward_start_position,
                 forward_direction_is_horizontal,
                 forward_direction_is_positive,
                 forward_end_point,
                 forward_end_direction_horizontal,
                 forward_row_wise_segments_map,
                 forward_col_wise_segments_map);

  // Check if the safe can be opened without any mirror insertion
  if (forward_end_direction_horizontal &&
      forward_end_point.row == rows_ &&
      forward_end_point.col == cols_) {
    result.result_type = SafeCheckResultType::OpensWithoutInserting;
    return result;
  }

  // Find ray segments of backward direction
  Point backward_start_position{rows_, cols_};
  bool backward_direction_is_horizontal = true;
  bool backward_direction_is_positive = false;  // Left to right or up to down
  Point backward_end_point;
  bool backward_end_direction_horizontal = false;
  RaySegmentsMap backward_row_wise_segments_map;
  RaySegmentsMap backward_col_wise_segments_map;
  trace_the_ray_(backward_start_position,
                 backward_direction_is_horizontal,
                 backward_direction_is_positive,
                 backward_end_point,
                 backward_end_direction_horizontal,
                 backward_row_wise_segments_map,
                 backward_col_wise_segments_map);

  // Find intersections
  // TODO!!!
  return result;
}


void SafeChecker::trace_the_ray_(const Point& start_point,
                                 bool start_direction_positive,
                                 bool start_direction_horizontal,
                                 Point& end_point,
                                 bool& end_direction_horizontal,
                                 RaySegmentsMap& row_wise_segments_map,
                                 RaySegmentsMap& col_wise_segments_map) const
{
  row_wise_segments_map.clear();
  col_wise_segments_map.clear();

  bool direction_is_positive = start_direction_positive;
  bool direction_is_horizontal = start_direction_horizontal;
  Point current_position = start_point;
  bool should_continue = true;
  while(should_continue) {
    if (direction_is_horizontal) {
      Point next_position;
      next_position.row = current_position.row;
      const auto row_iter = row_wise_mirrors_.find(current_position.row);
      if (row_iter == row_wise_mirrors_.end()) {
        next_position.col = direction_is_positive ? cols_ : 1U;
        should_continue = false;
      } else {
        const auto& mirrors_line = row_iter->second;
        auto closest_mirror_iter = mirrors_line.lower_bound(current_position.col);
        if (direction_is_positive) {
          if (closest_mirror_iter != mirrors_line.end() &&
              closest_mirror_iter->first == current_position.col) {
            ++closest_mirror_iter;
          }
        } else {
          if (closest_mirror_iter != mirrors_line.begin()) {
            --closest_mirror_iter;
          }
        }
        if (closest_mirror_iter == mirrors_line.end()) {
          next_position.col = direction_is_positive ? cols_ : 1U;
          should_continue = false;
        } else {
          next_position.col = closest_mirror_iter->first;
          // Change direction
          direction_is_horizontal = false;
          const MirrorOrientation mirror = closest_mirror_iter->second;
          if (direction_is_positive) {
            direction_is_positive = (mirror == MirrorOrientation::LeftToDown);
          } else {
            direction_is_positive = (mirror == MirrorOrientation::LeftToUp);
          }
        }
        // Add a segment
        row_wise_segments_map[current_position.row].add_segment(current_position.col, next_position.col);
        // Go to the next position
        current_position = next_position;
      }
    } else {
      Point next_position;
      next_position.col = current_position.col;
      const auto col_iter = col_wise_mirrors_.find(current_position.col);
      if (col_iter == col_wise_mirrors_.end()) {
        next_position.row = direction_is_positive ? rows_ : 1U;
        should_continue = false;
      } else {
        const auto& mirrors_line = col_iter->second;
        auto closest_mirror_iter = mirrors_line.lower_bound(current_position.row);
        if (direction_is_positive) {
          if (closest_mirror_iter != mirrors_line.end() &&
              closest_mirror_iter->first == current_position.row) {
            ++closest_mirror_iter;
          }
        } else {
          if (closest_mirror_iter != mirrors_line.begin()) {
            --closest_mirror_iter;
          }
        }
        if (closest_mirror_iter == mirrors_line.end()) {
          next_position.row = direction_is_positive ? rows_ : 1U;
          should_continue = false;
        } else {
          next_position.row = closest_mirror_iter->first;
          // Change direction
          direction_is_horizontal = true;
          const MirrorOrientation mirror = closest_mirror_iter->second;
          if (direction_is_positive) {
            direction_is_positive = (mirror == MirrorOrientation::LeftToDown);
          } else {
            direction_is_positive = (mirror == MirrorOrientation::LeftToUp);
          }
        }
        // Add a segment
        col_wise_segments_map[current_position.col].add_segment(current_position.row, next_position.row);
        // Go to the next position
        current_position = next_position;
      }
    }
  }
  end_point = current_position;
  end_direction_horizontal = direction_is_horizontal;
}

std::vector<Point> SafeChecker::find_intersections(const RaySegmentsMap& forward_row_wise_segments_map,
                                                   const RaySegmentsMap& forward_col_wise_segments_map,
                                                   const RaySegmentsMap& backward_row_wise_segments_map,
                                                   const RaySegmentsMap& backward_col_wise_segments_map) const
{

}

}  // namespace mirrors_lasers

int main()
{
  // Check input range
  std::cout << "Hello World!" << std::endl;
  return 0;
}
