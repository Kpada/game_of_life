# The firmware target name
set(This "game_of_life_fw")

project(${This} C CXX)

# # # # # Compiler # # # # #
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-as")

# Flags for STM32F103C8 microcontroller Common C and C++ flags
set(COMMON_C_CXX_FLAGS
    "-march=armv7-m -mthumb -ffunction-sections -fdata-sections -fstack-usage -fmessage-length=0 \
  -Wall -Wextra -Og -g3")

# Workaround for missing include paths in compile commands. It's required to
# explicitly specify the include paths to make clangd and clang-tidy work.
set(CXX_COMPILE_COMMANDS_INCLUDE_FLAGS
    "-isystem /usr/include/newlib/c++/9.2.1/arm-none-eabi \
  -isystem /usr/include/newlib/c++/9.2.1 \
  -isystem /usr/include/newlib")

# Set flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_C_CXX_FLAGS}")

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${COMMON_C_CXX_FLAGS} \
  -fno-rtti -fno-use-cxa-atexit -fno-exceptions \
  ${CXX_COMPILE_COMMANDS_INCLUDE_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} \
    -T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld -nostartfiles -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs"
)

# # # # # Download Dependencies # # # # #
include(FetchContent)

# Download libopencm3
FetchContent_Declare(
  libopencm3
  GIT_REPOSITORY https://github.com/libopencm3/libopencm3.git
  GIT_TAG bb4c5d7324554fe7a3137bfbcf3b5200ee2648fa)
FetchContent_MakeAvailable(libopencm3)

include_directories(${libopencm3_SOURCE_DIR}/include)
link_directories(${libopencm3_SOURCE_DIR}/lib)

# # # # # Target # # # # #
add_definitions(-DSTM32F1)
set(LIBOPENCM3_TARGET "stm32/f1")

# # # # # Build # # # # #

# Build libopencm3
add_custom_target(
  libopencm3_build ALL
  COMMAND make TARGETS=${LIBOPENCM3_TARGET}
  WORKING_DIRECTORY ${libopencm3_SOURCE_DIR})

set(LIBOPENCM3_LIB "${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f1.a")

# Create the executable target
add_executable(${This} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# Link the libopencm3 library
add_dependencies(${This} libopencm3_build)
target_link_libraries(${This} PRIVATE ${LIBOPENCM3_LIB})

# Command to create a binary file
add_custom_command(
  TARGET ${This}
  POST_BUILD
  COMMAND arm-none-eabi-objcopy -O binary "${CMAKE_CURRENT_BINARY_DIR}/${This}"
          "${CMAKE_CURRENT_BINARY_DIR}/${This}.bin"
  COMMAND arm-none-eabi-objdump -d -S "${CMAKE_CURRENT_BINARY_DIR}/${This}" >
          "${CMAKE_CURRENT_BINARY_DIR}/${This}.list"
  COMMAND
    arm-none-eabi-objcopy -O elf32-littlearm
    "${CMAKE_CURRENT_BINARY_DIR}/${This}"
    "${CMAKE_CURRENT_BINARY_DIR}/${This}.elf"
  COMMAND arm-none-eabi-size ${This}
  COMMENT "Generating bin, list and elf files")
