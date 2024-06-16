cmake_minimum_required(VERSION 3.14)
project(GameOfLife C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "")

# Add subdirectories
add_subdirectory(firmware)
add_subdirectory(tests)

# Copy compile_commands.json to project root
add_custom_target(
  copy_compile_commands ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
  DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
  COMMENT "Copying compile_commands.json to project root")
