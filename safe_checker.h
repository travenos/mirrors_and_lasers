#ifndef SAFE_CHECKER
#define SAFE_CHECKER

#include <cstdint>
#include <map>
#include <vector>
#include <unordered_map>

namespace mirrors_lasers {

/// @brief Enumeration for determination of mirror types ("/" or "\\")
enum class MirrorOrientation : std::int8_t
{
  /// @brief Mirror "/"
  LeftToUp,
  /// @brief Mirror "\\"
  LeftToDown
};

/// @brief Data structure to store positions of mirrors in each row and column
using MirrorsLine = std::map<std::uint32_t, MirrorOrientation>;

/// @brief Data structure to store positions of all mirrors in the grid
using MirrorsField = std::unordered_map<std::uint32_t, MirrorsLine>;

/// @brief Structure containing base information about beam segment
struct BeamSegment final {
  /// @brief Row number if the segment is horizontal or column number if the segment is vertical
  std::uint32_t first_coordinate{0U};
  /// @brief Coordinate of a start position of the beam segment.
  ///
  /// @details Is a column number if the segment is horizontal or row number if the segment is vertical
  std::uint32_t second_coordinate_start{0U};
  /// @brief Coordinate of an end position of the beam segment.
  ///
  /// @details Is a column number if the segment is horizontal or row number if the segment is vertical
  std::uint32_t second_coordinate_end{0U};
};

/// @brief Array containing beam segments
using BeamSegments = std::vector<BeamSegment>;

/// @brief Structure containing coordinates of a point on the mechanism grid
struct Point final {
  /// @brief Row number
  std::uint32_t row{0U};
  /// @brief Column number
  std::uint32_t col{0U};
};

/// @brief Structure containing information about state of the beam in a certain position
struct BeamState final {
  /// @brief Position on the mechanism grid for which information about the beam is provided
  Point position{0U, 0U};
  /// @brief Direction of the beam. Left to right or up to down directions are considered positive
  bool is_positive{false};
  /// @brief True if the beam direction is horizontal, false - if vertical
  bool is_horizontal{false};
};

/// @brief Enumeration describing the check result
enum class SafeCheckResultType : std::int8_t {
  /// @brief The safe opens without inserting a mirror
  OpensWithoutInserting,
  /// @brief The safe cannot be opened with or without inserting a mirror
  CanNotBeOpened,
  /// @brief The safe does not open without inserting a mirror,
  /// and there are positions where inserting a mirror opens the safe
  RequiresMirrorInsertion
};

/// @brief Structure containing complete information describing the check result
struct SafeCheckResult final {
  /// @brief Type of the check result
  SafeCheckResultType result_type{SafeCheckResultType::OpensWithoutInserting};
  /// @brief Number of positions where inserting a mirror opens the safe
  ///
  /// @details The field value is valid only if the result_type is SafeCheckResultType::RequiresMirrorInsertion
  std::uint32_t positions{0U};
  /// @brief Row of the lexicographically smallest position, where a mirror, opening the safe can be inserted
  ///
  /// @details The field value is valid only if the result_type is SafeCheckResultType::RequiresMirrorInsertion
  std::uint32_t mirror_row{0U};
  /// @brief Column of the lexicographically smallest position, where a mirror, opening the safe can be inserted
  ///
  /// @details The field value is valid only if the result_type is SafeCheckResultType::RequiresMirrorInsertion
  std::uint32_t mirror_col{0U};
};

/// @brief Class implementing the logic of checking how the safe can be opened
class SafeChecker final {
public:
  /// @brief Constructs the safe checker object from the input information about the mechanism grid
  ///
  /// @param rows Number of rows in the mechanism grid
  /// @param columns Number of columns in the mechanism grid
  /// @param left_to_up_mirrors List of positions where the "/" mirrors are placed
  /// @param left_to_down_mirrors List of positions where the "\\" mirrors are placed
  /// @throw std::invalid_argument if the input is incorrect
  SafeChecker(std::uint32_t rows, std::uint32_t columns,
              const std::vector<Point>& left_to_up_mirrors,
              const std::vector<Point>& left_to_down_mirrors);

  /// @brief Performs the check how the safe can be opened
  ///
  /// @return A SafeCheckResult object, containing complete information describing the check result
  SafeCheckResult check_safe() const;

private:
  /// @brief Checks that the point lies on the working grid
  ///
  /// @param point Coordinates of the point
  ///
  /// @throw std::invalid_argument if the point is out of grid bounds
  void throw_if_out_of_bounds_(const Point& point) const;

  /// @brief Constructs all the beam segments on the grid, starting from a certain beam state
  ///
  /// @param start_state Beam state from which the beam starts
  /// @param end_state Output parameter. Final beam state, after which it exits the grid
  /// @param horizontal_segments Output parameter. List of all horizontal beam segments
  /// @param vertical_segments Output parameter. List of all vertical beam segments
  void trace_the_beam_(const BeamState& start_state,
                       BeamState& end_state,
                       BeamSegments& horizontal_segments,
                       BeamSegments& vertical_segments) const;

  /// @brief Checks that there is a mirror in a certain point of the grid
  ///
  /// @param point Coordinates of the point
  ///
  /// @return true if there is a mirror in the given point, false otherwise
  bool has_mirror_(const Point& point) const;

  /// @brief Finds all valid intersections of the direct and reverse trajectories
  ///
  /// @param forward_horizontal_segments List of all horizontal segments of the direct beam trajectory
  /// @param forward_vertical_segments List of all vertical segments of the direct beam trajectory
  /// @param backward_horizontal_segments List of all horizontal segments of the reverse beam trajectory
  /// @param backward_vertical_segments List of all vertical segments of the reverse beam trajectory
  ///
  /// @return List of coordinates of all intersections of the direct and reverse trajectories on the grid.
  /// The result doesn't include positions already containing mirrors.
  std::vector<Point> find_intersections_(const BeamSegments& forward_horizontal_segments,
                                         const BeamSegments& forward_vertical_segments,
                                         const BeamSegments& backward_horizontal_segments,
                                         const BeamSegments& backward_vertical_segments) const;

  /// @brief Number of rows in the mechanism grid
  std::uint32_t rows_;
  /// @brief Number of columns in the mechanism grid
  std::uint32_t cols_;
  /// @brief Key-value data structure, containing information about all coordinates of the mirrors.
  /// First coordinate is the row number
  MirrorsField row_wise_mirrors_;
  /// @brief Key-value data structure, containing information about all coordinates of the mirrors.
  /// First coordinate is the column number
  MirrorsField col_wise_mirrors_;
};

}  // namespace mirrors_lasers

#endif  // SAFE_CHECKER
