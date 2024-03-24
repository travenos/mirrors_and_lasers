#include "intersection_search_helper.h"

#include <algorithm>

namespace mirrors_lasers {

void IntersectionSearchHelper::add_segment(std::uint32_t start, std::uint32_t end)
{
  const auto min_max_pair = std::minmax(start, end);
  segments_map_[min_max_pair.second] = min_max_pair.first;
}

bool IntersectionSearchHelper::has_intersection(std::uint32_t orthogonal_line_position) const
{
  const auto segment_iter = segments_map_.lower_bound(orthogonal_line_position);
  if (segment_iter == segments_map_.end()) {
    return false;
  }
  const std::uint32_t segment_begin = segment_iter->second;
  return segment_begin <= orthogonal_line_position;
}

}  // namespace mirrors_lasers
