# Mirrors and Lasers

## Description
Description
Safeguard Systems is a company that manufactures high quality safes. Its latest invention is a
safe with an optical closure mechanism that uses a laser beam passing through a rectangular
grid with several mirrors.

The grid consists of r rows and c columns. There are m mirrors with orientation /, and n mirrors
with orientation \.
When the laser is activated, a beam enters the top row of the grid horizontally from the left. The
beam is reflected by every mirror that it hits. Each mirror has a 45 degree diagonal orientation,
either / or \. If the beam exits the bottom row of the grid horizontally to the right, it is detected
and the safe opens. Otherwise, the safe remains closed
and an alarm is raised.

Each safe has a missing mirror, which prevents the laser beam from traveling successfully
through the grid. The safe has a mechanism that enables
the user to drop a single mirror into any empty grid cell. A legitimate user knows the correct
position and orientation of the missing mirror and can thus open the
safe. Without this knowledge the user has to guess correctly, which can be difficult for safes with
large grids.

The problem is to determine if particular safes are actually secure. A secure safe does not open
right away without inserting a mirror, and there is at least one valid location and orientation for
the missing mirror. There may indeed be multiple such locations and orientations.

## Building
A compiler that supports C++14 and the CMake build system are required.  
The GoogleTest library is required for unit tests. By default, tests are not built.  
Building the project without unit tests:
```
cd safe_laser
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build . --target all -- -j 12
```
To build with unit tests (GoogleTest library is required), pass the `-DBUILD_TESTS=ON` flag to CMake:
```
cd safe_laser
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build . --target all -- -j 12
ctest --output-on-failure
```

## Running
```
./safe_laser
```
The program has a command line interface. After start, it outputs short information about input data format and meaning
of the result.  
The user needs to enter input data to the command line according to the input format.

## Input Format
Each test case describes a single safe and starts with a line containing four integer numbers r, c, m, and n
where (1 ≤ r , c ≤ 1000000 and 0 ≤ m, n ≤ 200000).  
The mechanism grid has r rows and c columns.
Each of the next m lines contains two integer numbers ri and ci (1 ≤ ri ≤ r and 1 ≤ ci ≤ c) specifying that there is a /
mirror in row ri column ci.
The following n lines specify the positions of the \ mirrors in the same way. The m + n positions of the mirrors are
pairwise distinct.

## Output Format
- 0 if the safe opens without inserting a mirror. 
- -1 if the safe cannot be opened with or without inserting a mirror. 
- k r c if the safe does not open without inserting a mirror, there are exactly k positions where inserting a mirror
opens the safe, and (r, c) is the lexicographically smallest such row, column position. A position where both a / and a
\ mirror open the safe counts just once.

## Algorithm

### The main idea
The algorithm calculates the trajectory of a beam, starting from the laser position (1, 1).
If it turns out that the beam hits the detector located right after the position (r, c), it is assumed that the safe
opens without additional mirror inserts.  
Otherwise, the algorithm calculates the reverse trajectory of the beam, starting from the detector's position (r, c).  
Next, the intersections of the direct and reverse trajectories (trajectories from the laser and trajectories from the
detector) are found. Only those intersection points where there are no mirrors are selected.  
If no trajectory intersections are found, then it is impossible to open the safe.  
If there are intersection points, their count (k) is considered, as well as the coordinates of the lexicographically
smallest trajectory intersection point.

### Implementation details
The implementation is based on the requirement that the complexity of calculations and the amount of memory allocated
should not depend on the size of the grid, and also the complexity should be minimal relative to the number of existing
mirrors.

#### 1. Data structure for storing information about mirrors
An ordered dictionary (`std::map`), where the key is the mirror's position and the value is the type of mirror, is used
to store positions of mirrors in each row and column. In such a dictionary, searching for the nearest mirror has
logarithmic complexity, but its construction has O(log(n)) complexity.  
Rows with mirrors are combined into a dictionary based on a hash table (`std::unordered_map`), where the keys are the
numbers of rows containing mirrors, and the values are dictionaries with the positions of mirrors in the row.
In such a dictionary, access to the required row has constant complexity.  
Columns with mirrors are combined into a dictionary in a similar manner.

#### 2. Constructing the trajectory of the beam from the laser
Next, the trajectory of the beam from the laser is constructed. For horizontal sections of the trajectory, the nearest
mirror is searched for in the first dictionary (which contains rows with mirrors), and for vertical ones - in the second
(which contains columns with mirrors).  
The obtained trajectory segments are placed in arrays - vertical ones in one array, horizontal ones in another array.
If, as a result of tracing the path of the beam, the beam hits the detector - the program execution ends.

#### 3. Constructing the trajectory of the beam from the detector
The trajectory is constructed in the opposite direction - from the detector.

#### 4. Searching for intersection points of the constructed trajectories
Intersection points for the constructed trajectories are searched for. For each horizontal segment of the trajectory
from the detector, intersections with vertical segments of the trajectory from the laser are searched for.
Similarly, for each vertical segment of the trajectory from the detector, intersections with horizontal segments of the
trajectory from the laser are searched for.

To simplify the complexity of the search, the trajectory segments from the laser are placed in special data structures
based on ordered dictionaries (`std::map`).
Two objects with the same structure are created - for horizontal and vertical segments.
Each row or column is placed in an object `IntersectionSearchHelper`, which is a wrapper over `std::map`.
In `std::map`, the ends of the trajectory segments in a given row/column are used as keys, and their beginnings as
values. As a result of using `std::map`, `IntersectionSearchHelper` can determine with logarithmic complexity whether
there is an intersection at a certain point of a row/column.
`IntersectionSearchHelper` objects are also placed in `std::map` dictionaries, where the key is the number of the row or
column, and the values are `IntersectionSearchHelper` objects for that row/column.  
When searching for intersections, all rows or columns from this dictionary are sequentially checked, starting from the
row/column corresponding to the end and ending with the row/column corresponding to the beginning of the segment for
which intersections are being searched. The complexity of searching for the row/column from which to start the iteration
has logarithmic complexity, the iteration itself has linear complexity relative to the number of potentially possible
intersections, and the determination of an intersection with a row/column has logarithmic complexity.  
Intersection points are checked for the presence of a mirror. The check has logarithmic complexity.
If there is no mirror at the intersection point, its coordinates are placed in an array.

If, as a result, an empty array is obtained, the decision is made that it is impossible to open the safe.
Otherwise, the number of elements in the array is counted, and the lexicographically smallest coordinates in the array
are selected as the result.

Barashkov A.A., 2024