#ifndef FIRMWARE_GAME_OF_LIFE_HPP_
#define FIRMWARE_GAME_OF_LIFE_HPP_

#include <array>
#include <cstdint>
#include <random>

/// @brief Manages the Conway's Game of Life logic.
/// @tparam Width The width of the game grid.
/// @tparam Height The height of the game grid.
template <std::uint8_t Width, std::uint8_t Height>
class GameOfLife {
 public:
  /// @brief The width of the game grid.
  static constexpr std::uint8_t kGridWidth{Width};

  /// @brief The height of the game grid.
  static constexpr std::uint8_t kGridHeight{Height};

  /// @brief Type of the game grid.
  using GameBuffer = std::array<std::array<std::uint8_t, kGridWidth>, kGridHeight>;

  /// @brief Constructs a game grid from an existing grid.
  explicit GameOfLife(const GameBuffer& game_grid) noexcept : gameGrid_{game_grid} {}

  /// @brief Constructs a game grid with a random pattern.
  /// @param seed Seed for the random number generator.
  explicit GameOfLife(std::uint32_t seed) noexcept { InitializeGameGrid(seed); }

  /// @brief Updates the game grid to the next generation.
  void UpdateGameGrid() noexcept {
    for (std::uint8_t coord_y{0U}; coord_y < kGridHeight; ++coord_y) {
      for (std::uint8_t coord_x{0U}; coord_x < kGridWidth; ++coord_x) {
        constexpr std::uint8_t kMaxNeighbors{4U};
        const auto num_alive_neighbors = CountLivingNeighbors(coord_x, coord_y, kMaxNeighbors);
        const auto is_alive = gameGrid_[coord_y][coord_x] == 1U;

        // Rules:
        // 1. Any live cell with fewer than two live neighbors dies, as if caused by under-population.
        // 2. Any live cell with two or three live neighbors lives on to the next generation.
        // 3. Any live cell with more than three live neighbors dies, as if by over-population.
        const auto should_be_alive = (num_alive_neighbors == 3U) || (is_alive && (num_alive_neighbors == 2U));
        nextGrid_[coord_y][coord_x] = should_be_alive ? 1U : 0U;
      }
    }

    gameGrid_ = nextGrid_;
  }

  /// @brief Gets the current game grid.
  /// @return The game grid.
  const GameBuffer& GetGameGrid() const noexcept { return gameGrid_; }

 private:
  /// @brief Initializes the game grid with a random pattern.
  /// @param seed Seed for the random number generator.
  void InitializeGameGrid(std::uint32_t seed) noexcept {
    std::mt19937 generator(seed);
    std::uniform_int_distribution<std::uint8_t> distribution(0, 1);

    for (auto& row : gameGrid_) {
      for (auto& cell : row) {
        cell = distribution(generator);
      }
    }
  }

  /// @brief Counts the living neighbors of a cell.
  /// @param coord_x X coordinate of the cell.
  /// @param coord_y Y coordinate of the cell.
  /// @param max_neighbors Maximum number of neighbors. Once this number is reached, the search is stopped.
  /// @return Number of living neighbors.
  std::uint8_t CountLivingNeighbors(std::uint8_t coord_x, std::uint8_t coord_y,
                                    std::uint8_t max_neighbors) const noexcept {
    static constexpr std::array<std::int8_t, 3> kOffsets{-1, 0, 1};
    std::uint8_t count{0};

    for (auto offset_x : kOffsets) {
      for (auto offset_y : kOffsets) {
        if ((offset_x == 0U) && (offset_y == 0U)) {
          continue;
        }

        const auto new_x = static_cast<std::int8_t>(coord_x) + offset_x;
        const auto new_y = static_cast<std::int8_t>(coord_y) + offset_y;

        if ((new_x >= 0) && (new_x < kGridWidth) && (new_y >= 0) && (new_y < kGridHeight)) {
          count += gameGrid_[new_y][new_x] ? 1U : 0U;
          if (count >= max_neighbors) {
            return count;
          }
        }
      }
    }

    return count;
  }

  /// @brief Game grid.
  GameBuffer gameGrid_{};

  /// @brief Next generation of the game grid.
  GameBuffer nextGrid_{};
};

#endif  // FIRMWARE_GAME_OF_LIFE_HPP_
