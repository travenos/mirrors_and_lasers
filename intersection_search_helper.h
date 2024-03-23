#ifndef INTERSECTION_SEARCH_HELPER
#define INTERSECTION_SEARCH_HELPER

#include <cstdint>
#include <map>

namespace mirrors_lasers {

class IntersectionSearchHelper final {
public:
  void add_segment(std::uint32_t start, std::uint32_t end);
  bool has_intersection(std::uint32_t orthogonal_line_position) const;

private:
  std::map<std::uint32_t, std::uint32_t> segments_map_;
};

using IntersectinSearchHelperMap = std::map<std::uint32_t, IntersectionSearchHelper>;

}  // mirrors_lasers

#endif // INTERSECTION_SEARCH_HELPER
