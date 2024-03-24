#ifndef INTERSECTION_SEARCH_HELPER
#define INTERSECTION_SEARCH_HELPER

#include <cstdint>
#include <map>

namespace mirrors_lasers {

/// @brief Class determines with logarithmic complexity whether there is an intersection at
/// a certain point of a row/column
class IntersectionSearchHelper final {
public:
  /// @brief Adds a beam segment to the row/column
  ///
  /// @param start Coordinate of the segment start
  /// @param end Coordinate of the segment end
  void add_segment(std::uint32_t start, std::uint32_t end);

  /// @brief Checks whether there is a beam segment in a certain coordinate of the row/column
  ///
  /// @param orthogonal_line_position Coordinate for which the check should be performed
  ///
  /// @return true if there is an intersection with some segment in the requested position
  bool has_intersection(std::uint32_t orthogonal_line_position) const;

private:
  /// @brief Container, containing information about the line segments
  ///
  /// @details Ends of the trajectory segments in a given row/column are used as keys, and their beginnings as values
  std::map<std::uint32_t, std::uint32_t> segments_map_;
};

/// @brief Data structure used to simplify the complexity of the search of beam segments intersections in the grid
///
/// @details where the key is the number of the row or column, and the values are IntersectionSearchHelper objects
/// for that row/column
using IntersectionSearchHelperMap = std::map<std::uint32_t, IntersectionSearchHelper>;

}  // namespace mirrors_lasers

#endif  // INTERSECTION_SEARCH_HELPER
