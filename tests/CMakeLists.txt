project(tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra -Werror")

# Download google test
include(FetchContent)
FetchContent_Declare(
  gtest
  QUIET
  URL https://github.com/google/googletest/archive/release-1.12.1.tar.gz)

# Configure build of google test
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
set(BUILD_GMOCK
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

# Sources
set(SOURCES test_game_of_life.cpp)

include_directories(${CMAKE_SOURCE_DIR}/firmware/)

# Executable
add_executable(tests ${SOURCES})

target_link_libraries(tests PRIVATE gtest gtest_main)

# Automatic discovery of unit tests
include(GoogleTest)
gtest_discover_tests(tests)
