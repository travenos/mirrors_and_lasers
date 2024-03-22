#include "safe_checker.h"
#include "ray_segment_intersection_finder.h"

#include <algorithm>
#include <stdexcept>
#include <limits>

namespace mirrors_lasers {

static RaySegmentsIntersectionFinderMap ray_segments_to_map(const RaySegments& ray_segments)
{
  RaySegmentsIntersectionFinderMap result;
  for (const auto& segment : ray_segments) {
    result[segment.first_coordinate]
        .add_segment(segment.second_coordinate_start, segment.second_coordinate_end);
  }
  return result;
}

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
  RaySegments forward_row_wise_segments;
  RaySegments forward_col_wise_segments;
  trace_the_ray_(forward_start_position,
                 forward_direction_is_horizontal,
                 forward_direction_is_positive,
                 forward_end_point,
                 forward_end_direction_horizontal,
                 forward_row_wise_segments,
                 forward_col_wise_segments);

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
  RaySegments backward_row_wise_segments;
  RaySegments backward_col_wise_segments;
  trace_the_ray_(backward_start_position,
                 backward_direction_is_horizontal,
                 backward_direction_is_positive,
                 backward_end_point,
                 backward_end_direction_horizontal,
                 backward_row_wise_segments,
                 backward_col_wise_segments);

  // Find intersections
  const std::vector<Point> intersections = find_intersections_(forward_row_wise_segments,
                                                               forward_col_wise_segments,
                                                               backward_row_wise_segments,
                                                               backward_col_wise_segments);

  // Collect all intersections, which don't already contain a mirror
  std::vector<Point> valid_intersections;
  for (const auto& intersection: intersections) {
    const auto mirror_row_iter = row_wise_mirrors_.find(intersection.row);
    if (mirror_row_iter == row_wise_mirrors_.end()) {
      valid_intersections.push_back(intersection);
    } else {
      const auto& mirror_row = mirror_row_iter->second;
      const auto mirror_col_iter = mirror_row.find(intersection.col);
      if (mirror_col_iter == mirror_row.end()) {
        valid_intersections.push_back(intersection);
      }
    }
  }

  // Can not be opened if no intersections
  if (valid_intersections.empty()) {
    result.result_type = SafeCheckResultType::CanNotBeOpened;
    return result;
  }

  // Find the lexicographically smallest mirror position
  result.result_type = SafeCheckResultType::RequiresMirrorInsertion;
  if (valid_intersections.size() < std::numeric_limits<std::uint32_t>::max()) {
    result.positions = static_cast<std::uint32_t>(valid_intersections.size());
  } else {
    throw std::logic_error{"Internal logic error: intersections count is greater than maximum uint32"};
  }
  auto points_comparer = [] (const Point& first, const Point& second) {
    return first.row != second.row ? first.row < second.row : first.col < second.col;
  };
  const auto min_element_iter =
      std::min_element(valid_intersections.begin(), valid_intersections.end(), points_comparer);

  if (min_element_iter == valid_intersections.end()) {
    throw std::logic_error{"Failed to found min possible intersection"};
  }
  result.mirror_row = min_element_iter->row;
  result.mirror_col = min_element_iter->col;

  return result;
}


void SafeChecker::trace_the_ray_(const Point& start_point,
                                 bool start_direction_positive,
                                 bool start_direction_horizontal,
                                 Point& end_point,
                                 bool& end_direction_horizontal,
                                 RaySegments& row_wise_segments,
                                 RaySegments& col_wise_segments) const
{
  row_wise_segments.clear();
  col_wise_segments.clear();

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
      }
      // Add a segment
      const auto min_max_cols_pair = std::minmax(current_position.col, next_position.col);
      RaySegment segment{current_position.row, min_max_cols_pair.first, min_max_cols_pair.second};
      row_wise_segments.push_back(segment);
      // Go to the next position
      current_position = next_position;
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
      }
      // Add a segment
      const auto min_max_rows_pair = std::minmax(current_position.row, next_position.row);
      RaySegment segment{current_position.col, min_max_rows_pair.first, min_max_rows_pair.second};
      row_wise_segments.push_back(segment);
      // Go to the next position
      current_position = next_position;
    }
  }
  end_point = current_position;
  end_direction_horizontal = direction_is_horizontal;
}

std::vector<Point> SafeChecker::find_intersections_(const RaySegments& forward_row_wise_segments,
                                                    const RaySegments& forward_col_wise_segments,
                                                    const RaySegments& backward_row_wise_segments,
                                                    const RaySegments& backward_col_wise_segments) const
{
  std::vector<Point> intersections;
  const RaySegmentsIntersectionFinderMap forward_row_wise_segments_map = ray_segments_to_map(forward_row_wise_segments);
  const RaySegmentsIntersectionFinderMap forward_col_wise_segments_map = ray_segments_to_map(forward_col_wise_segments);

  for (const auto& segment : backward_row_wise_segments) {
    const std::uint32_t row = segment.first_coordinate;
    const auto col_iter = forward_col_wise_segments_map.lower_bound(segment.second_coordinate_start);
    while (col_iter != forward_col_wise_segments_map.end() && col_iter->first <= segment.second_coordinate_end) {
      const std::uint32_t col = col_iter->first;
      if (col_iter->second.has_intersection(row)) {
        Point intersection{row, col};
        intersections.push_back(intersection);
      }
    }
  }
  for (const auto& segment : backward_col_wise_segments) {
    const std::uint32_t col = segment.first_coordinate;
    const auto row_iter = forward_row_wise_segments_map.lower_bound(segment.second_coordinate_start);
    while (row_iter != forward_row_wise_segments_map.end() && row_iter->first <= segment.second_coordinate_end) {
      const std::uint32_t row = row_iter->first;
      if (row_iter->second.has_intersection(col)) {
        Point intersection{row, col};
        intersections.push_back(intersection);
      }
    }
  }
  return intersections;
}

}  // namespace mirrors_lasers
