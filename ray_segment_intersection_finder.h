#ifndef RAY_SEGMENT_INTERSECTION_FINDER
#define RAY_SEGMENT_INTERSECTION_FINDER

#include <cstdint>
#include <map>

namespace mirrors_lasers {

class RaySegmentsIntersectionFinder final {
public:
  void add_segment(std::uint32_t start, std::uint32_t end);
  bool has_intersection(std::uint32_t orthogonal_line_position) const;

private:
  std::map<std::uint32_t, std::uint32_t> segments_map_;
};

using RaySegmentsIntersectionFinderMap = std::map<std::uint32_t, RaySegmentsIntersectionFinder>;

}  // mirrors_lasers

#endif // RAY_SEGMENT_INTERSECTION_FINDER
