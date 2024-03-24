#include "safe_checker.h"
#include "intersection_search_helper.h"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

namespace mirrors_lasers {

constexpr std::uint32_t START_POSITION{1U};

static IntersectionSearchHelperMap beam_segments_to_map(const BeamSegments& beam_segments)
{
  IntersectionSearchHelperMap result;
  for (const auto& segment : beam_segments) {
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
  if (rows_ < START_POSITION) {
    throw std::invalid_argument{"Incorrect rows count: " + std::to_string(rows_)};
  }
  if (cols_ < START_POSITION) {
    throw std::invalid_argument{"Incorrect columns count: " + std::to_string(rows_)};
  }

  const std::size_t mirrors_count = left_to_up_mirrors.size() + left_to_down_mirrors.size();
  row_wise_mirrors_.reserve(mirrors_count);
  col_wise_mirrors_.reserve(mirrors_count);

  // Fill the data
  for (const auto& left_to_up_mirror : left_to_up_mirrors) {
    throw_if_out_of_bounds_(left_to_up_mirror);
    row_wise_mirrors_[left_to_up_mirror.row][left_to_up_mirror.col] = MirrorOrientation::LeftToUp;
    col_wise_mirrors_[left_to_up_mirror.col][left_to_up_mirror.row] = MirrorOrientation::LeftToUp;
  }
  for (const auto& left_to_down_mirror : left_to_down_mirrors) {
    throw_if_out_of_bounds_(left_to_down_mirror);
    row_wise_mirrors_[left_to_down_mirror.row][left_to_down_mirror.col] = MirrorOrientation::LeftToDown;
    col_wise_mirrors_[left_to_down_mirror.col][left_to_down_mirror.row] = MirrorOrientation::LeftToDown;
  }
}

SafeCheckResult SafeChecker::check_safe() const
{
  SafeCheckResult result{};

  // Find beam segments of direct direction
  BeamState forward_start_state{};
  forward_start_state.position = Point{START_POSITION, START_POSITION};
  forward_start_state.is_positive = true;
  forward_start_state.is_horizontal = true;
  BeamState forward_end_state{};
  BeamSegments forward_horizontal_segments{};
  BeamSegments forward_vertical_segments{};
  trace_the_beam_(forward_start_state,
                  forward_end_state,
                  forward_horizontal_segments,
                  forward_vertical_segments);

  // Check if the safe can be opened without any mirror insertion
  if (forward_end_state.position.row == rows_ &&
      forward_end_state.position.col == cols_ &&
      forward_end_state.is_positive &&
      forward_end_state.is_horizontal) {
    result.result_type = SafeCheckResultType::OpensWithoutInserting;
    return result;
  }

  // Find beam segments of reverse direction
  BeamState backward_start_state;
  backward_start_state.position = Point{rows_, cols_};
  backward_start_state.is_positive = false;
  backward_start_state.is_horizontal = true;
  BeamState backward_end_state{};
  BeamSegments backward_horizontal_segments{};
  BeamSegments backward_vertical_segments{};
  trace_the_beam_(backward_start_state,
                  backward_end_state,
                  backward_horizontal_segments,
                  backward_vertical_segments);

  // Find intersections
  const std::vector<Point> intersections = find_intersections_(forward_horizontal_segments,
                                                               forward_vertical_segments,
                                                               backward_horizontal_segments,
                                                               backward_vertical_segments);

  // Can not be opened if no intersections
  if (intersections.empty()) {
    result.result_type = SafeCheckResultType::CanNotBeOpened;
    return result;
  }

  // Find the lexicographically smallest mirror position
  result.result_type = SafeCheckResultType::RequiresMirrorInsertion;
  if (intersections.size() < std::numeric_limits<std::uint32_t>::max()) {
    result.positions = static_cast<std::uint32_t>(intersections.size());
  } else {
    throw std::logic_error{"Internal logic error: intersections count is greater than maximum uint32"};
  }
  auto points_comparer = [] (const Point& first, const Point& second) -> bool {
    return first.row != second.row ? first.row < second.row : first.col < second.col;
  };
  const auto min_element_iter =
      std::min_element(intersections.begin(), intersections.end(), points_comparer);

  if (min_element_iter == intersections.end()) {
    throw std::logic_error{"Failed to found min possible intersection"};
  }
  result.mirror_row = min_element_iter->row;
  result.mirror_col = min_element_iter->col;

  return result;
}

void SafeChecker::throw_if_out_of_bounds_(const Point& point) const
{
  if (point.row < START_POSITION || point.row > rows_) {
    throw std::invalid_argument{"Incorrect row value: " + std::to_string(point.row)};
  }
  if (point.col < START_POSITION || point.col > cols_) {
    throw std::invalid_argument{"Incorrect column value: " + std::to_string(point.col)};
  }
}

void SafeChecker::trace_the_beam_(const BeamState& start_state,
                                  BeamState& end_state,
                                  BeamSegments& horizontal_segments,
                                  BeamSegments& vertical_segments) const
{
  horizontal_segments.clear();
  vertical_segments.clear();

  BeamState current_state = start_state;

  // Check the initial position
  const auto first_row_iter = row_wise_mirrors_.find(current_state.position.row);
  if (first_row_iter != row_wise_mirrors_.end()) {
    const auto& mirrors_line = first_row_iter->second;
    const auto first_col_iter = mirrors_line.find(current_state.position.col);
    if (first_col_iter != mirrors_line.end()) {
      const MirrorOrientation mirror = first_col_iter->second;
      current_state.is_horizontal = !current_state.is_horizontal;
      if (mirror == MirrorOrientation::LeftToUp) {
        current_state.is_positive = !current_state.is_positive;
      }
    }
  }

  bool should_continue{true};
  while(should_continue) {
    if (current_state.is_horizontal) {
      Point next_position{};
      next_position.row = current_state.position.row;
      const auto row_iter = row_wise_mirrors_.find(current_state.position.row);
      if (row_iter == row_wise_mirrors_.end()) {
        next_position.col = current_state.is_positive ? cols_ : START_POSITION;
        should_continue = false;
      } else {
        const auto& mirrors_line = row_iter->second;
        auto closest_mirror_iter = mirrors_line.lower_bound(current_state.position.col);
        if (current_state.is_positive) {
          if (closest_mirror_iter != mirrors_line.end() &&
              closest_mirror_iter->first == current_state.position.col) {
            ++closest_mirror_iter;
          }
        } else {
          if (closest_mirror_iter != mirrors_line.begin()) {
            --closest_mirror_iter;
          } else {
            closest_mirror_iter = mirrors_line.end();
          }
        }
        if (closest_mirror_iter == mirrors_line.end()) {
          next_position.col = current_state.is_positive ? cols_ : START_POSITION;
          should_continue = false;
        } else {
          next_position.col = closest_mirror_iter->first;
          // Change direction
          current_state.is_horizontal = false;
          const MirrorOrientation mirror = closest_mirror_iter->second;
          if (mirror == MirrorOrientation::LeftToUp) {
            current_state.is_positive = !current_state.is_positive;
          }
        }
      }
      // Add a segment
      const auto min_max_cols_pair = std::minmax(current_state.position.col, next_position.col);
      BeamSegment segment{current_state.position.row, min_max_cols_pair.first, min_max_cols_pair.second};
      horizontal_segments.push_back(segment);
      // Go to the next position
      current_state.position = next_position;
    } else {
      Point next_position{};
      next_position.col = current_state.position.col;
      const auto col_iter = col_wise_mirrors_.find(current_state.position.col);
      if (col_iter == col_wise_mirrors_.end()) {
        next_position.row = current_state.is_positive ? rows_ : START_POSITION;
        should_continue = false;
      } else {
        const auto& mirrors_line = col_iter->second;
        auto closest_mirror_iter = mirrors_line.lower_bound(current_state.position.row);
        if (current_state.is_positive) {
          if (closest_mirror_iter != mirrors_line.end() &&
              closest_mirror_iter->first == current_state.position.row) {
            ++closest_mirror_iter;
          }
        } else {
          if (closest_mirror_iter != mirrors_line.begin()) {
            --closest_mirror_iter;
          } else {
            closest_mirror_iter = mirrors_line.end();
          }
        }
        if (closest_mirror_iter == mirrors_line.end()) {
          next_position.row = current_state.is_positive ? rows_ : START_POSITION;
          should_continue = false;
        } else {
          next_position.row = closest_mirror_iter->first;
          // Change direction
          current_state.is_horizontal = true;
          const MirrorOrientation mirror = closest_mirror_iter->second;
          if (mirror == MirrorOrientation::LeftToUp) {
            current_state.is_positive = !current_state.is_positive;
          }
        }
      }
      // Add a segment
      const auto min_max_rows_pair = std::minmax(current_state.position.row, next_position.row);
      BeamSegment segment{current_state.position.col, min_max_rows_pair.first, min_max_rows_pair.second};
      vertical_segments.push_back(segment);
      // Go to the next position
      current_state.position = next_position;
    }
  }
  end_state = current_state;
}

bool SafeChecker::has_mirror_(const Point& point) const
{
  const auto mirror_row_iter = row_wise_mirrors_.find(point.row);
  if (mirror_row_iter == row_wise_mirrors_.end()) {
    return false;
  }
  const auto& mirror_row = mirror_row_iter->second;
  const auto mirror_col_iter = mirror_row.find(point.col);
  return mirror_col_iter != mirror_row.end();
}

std::vector<Point> SafeChecker::find_intersections_(const BeamSegments& forward_horizontal_segments,
                                                    const BeamSegments& forward_vertical_segments,
                                                    const BeamSegments& backward_horizontal_segments,
                                                    const BeamSegments& backward_vertical_segments) const
{
  std::vector<Point> intersections;
  const IntersectionSearchHelperMap forward_horizontal_segments_map = beam_segments_to_map(forward_horizontal_segments);
  const IntersectionSearchHelperMap forward_vertical_segments_map = beam_segments_to_map(forward_vertical_segments);

  for (const auto& segment : backward_horizontal_segments) {
    const std::uint32_t row = segment.first_coordinate;
    auto col_iter = forward_vertical_segments_map.lower_bound(segment.second_coordinate_start);
    while (col_iter != forward_vertical_segments_map.end() && col_iter->first <= segment.second_coordinate_end) {
      const std::uint32_t col = col_iter->first;
      if (col_iter->second.has_intersection(row)) {
        const Point intersection{row, col};
        if (!has_mirror_(intersection)) {
          intersections.push_back(intersection);
        }
      }
      ++col_iter;
    }
  }
  for (const auto& segment : backward_vertical_segments) {
    const std::uint32_t col = segment.first_coordinate;
    auto row_iter = forward_horizontal_segments_map.lower_bound(segment.second_coordinate_start);
    while (row_iter != forward_horizontal_segments_map.end() && row_iter->first <= segment.second_coordinate_end) {
      const std::uint32_t row = row_iter->first;
      if (row_iter->second.has_intersection(col)) {
        const Point intersection{row, col};
        if (!has_mirror_(intersection)) {
          intersections.push_back(intersection);
        }
      }
      ++row_iter;
    }
  }
  return intersections;
}

}  // namespace mirrors_lasers
