enable_testing()
find_package(GTest REQUIRED)

set(TEST_NAME safe_checker_test)

add_executable(
  ${TEST_NAME}
  safe_checker_test.cpp
)

target_link_libraries(
  ${TEST_NAME}
  PRIVATE
    ${LIBRARY_NAME}
    GTest::GTest
    GTest::Main
)

gtest_discover_tests(${TEST_NAME})
