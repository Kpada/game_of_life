#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

#include "game_of_life.hpp"

template <std::uint8_t Width, std::uint8_t Height>
using GameBuffer = typename GameOfLife<Width, Height>::GameBuffer;

template <std::uint8_t Width, std::uint8_t Height>
std::string ToString(const GameBuffer<Width, Height>& grid) {
  std::string result;
  for (std::uint8_t coord_y{0U}; coord_y < Height; ++coord_y) {
    for (std::uint8_t coord_x{0U}; coord_x < Width; ++coord_x) {
      result += std::to_string(grid[coord_y][coord_x]);
    }
    result += '\n';
  }
  return result;
}

TEST(GameOfLifeTest, Constructor) {
  constexpr std::uint8_t kWidth{3U};
  constexpr std::uint8_t kHeight{3U};
  constexpr GameBuffer<kWidth, kHeight> kExpected{{{{0U, 1U, 0U}}, {{0U, 0U, 1U}}, {{1U, 1U, 1U}}}};

  const GameOfLife<kWidth, kHeight> game(kExpected);
  const auto& grid = game.GetGameGrid();

  ASSERT_TRUE(std::equal(kExpected.begin(), kExpected.end(), grid.begin())) << ToString<kWidth, kHeight>(grid);
}

TEST(GameOfLifeTest, Glider) {
  constexpr std::uint8_t kWidth{4U};
  constexpr std::uint8_t kHeight{4U};
  constexpr GameBuffer<kWidth, kHeight> kInitial{{{{0U, 1U, 0U, 0U}},  //
                                                  {{0U, 0U, 1U, 0U}},
                                                  {{1U, 1U, 1U, 0U}},
                                                  {{0U, 0U, 0U, 0U}}}};

  GameOfLife<kWidth, kHeight> game(kInitial);
  game.UpdateGameGrid();
  const auto& grid = game.GetGameGrid();

  constexpr GameBuffer<kWidth, kHeight> kExpected{{{{0U, 0U, 0U, 0U}},  //
                                                   {{1U, 0U, 1U, 0U}},
                                                   {{0U, 1U, 1U, 0U}},
                                                   {{0U, 1U, 0U, 0U}}}};
  ASSERT_TRUE(std::equal(kExpected.begin(), kExpected.end(), grid.begin())) << ToString<kWidth, kHeight>(grid);
}

TEST(GameOfLifeTest, StableStructure) {
  constexpr std::uint8_t kWidth{4U};
  constexpr std::uint8_t kHeight{4U};
  constexpr GameBuffer<kWidth, kHeight> kInitialGrid{{{{1U, 1U, 0U, 0U}},  //
                                                      {{1U, 1U, 0U, 0U}},
                                                      {{0U, 0U, 0U, 0U}},
                                                      {{0U, 0U, 0U, 0U}}}};

  GameOfLife<kWidth, kHeight> game(kInitialGrid);
  game.UpdateGameGrid();

  constexpr std::uint8_t kIterations{10U};
  for (std::uint8_t i{0U}; i < kIterations; ++i) {
    const auto& grid = game.GetGameGrid();
    ASSERT_TRUE(std::equal(kInitialGrid.begin(), kInitialGrid.end(), grid.begin())) << ToString<kWidth, kHeight>(grid);
  }
}

TEST(GameOfLifeTest, Blinker) {
  constexpr std::uint8_t kWidth{5U};
  constexpr std::uint8_t kHeight{5U};
  constexpr GameBuffer<kWidth, kHeight> kBlinkerState1{{{{0U, 0U, 0U, 0U, 0U}},
                                                        {{0U, 0U, 1U, 0U, 0U}},
                                                        {{0U, 0U, 1U, 0U, 0U}},
                                                        {{0U, 0U, 1U, 0U, 0U}},
                                                        {{0U, 0U, 0U, 0U, 0U}}}};
  constexpr GameBuffer<kWidth, kHeight> kBlinkerState2{{{{0U, 0U, 0U, 0U, 0U}},
                                                        {{0U, 0U, 0U, 0U, 0U}},
                                                        {{0U, 1U, 1U, 1U, 0U}},
                                                        {{0U, 0U, 0U, 0U, 0U}},
                                                        {{0U, 0U, 0U, 0U, 0U}}}};

  GameOfLife<kWidth, kHeight> game(kBlinkerState1);

  constexpr std::uint8_t kIterations{10U};
  for (std::uint32_t i{0U}; i < kIterations; ++i) {
    game.UpdateGameGrid();
    const auto& grid = game.GetGameGrid();
    const auto expected = (i % 2 == 0) ? kBlinkerState2 : kBlinkerState1;
    ASSERT_TRUE(std::equal(expected.begin(), expected.end(), grid.begin()))
        << ToString<kWidth, kHeight>(grid) << "\n, i=" << i;
  }
}

TEST(GameOfLifeTest, Tub) {
  constexpr std::uint8_t kWidth{3U};
  constexpr std::uint8_t kHeight{3U};
  constexpr GameBuffer<kWidth, kHeight> kInitial{{{{1U, 0U, 1U}},  // pre-Tub
                                                  {{0U, 1U, 0U}},
                                                  {{1U, 0U, 1U}}}};

  GameOfLife<kWidth, kHeight> game(kInitial);

  constexpr std::uint8_t kIterations{10U};
  for (std::uint32_t i{0U}; i < kIterations; ++i) {
    constexpr GameBuffer<kWidth, kHeight> kExpected{{{{0U, 1U, 0U}},  // Tub
                                                     {{1U, 0U, 1U}},
                                                     {{0U, 1U, 0U}}}};

    game.UpdateGameGrid();
    const auto& grid = game.GetGameGrid();
    ASSERT_TRUE(std::equal(kExpected.begin(), kExpected.end(), grid.begin())) << ToString<kWidth, kHeight>(grid);
  }
}
