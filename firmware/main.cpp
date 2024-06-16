#include <libopencm3/stm32/rcc.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "drivers/display/sh_1106.hpp"
#include "game_of_life.hpp"
#include "hal/adc.hpp"
#include "hal/i2c.hpp"

namespace {

/// @brief Initializes system clock and peripherals.
void InitializeSystem() { rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]); }

/// @brief Renders the game grid of the Game of Life onto the SH1106 display.
/// @tparam Width The width of the game grid.
/// @tparam Height The height of the game grid.
/// @param game The GameOfLife object containing the grid to render.
/// @param display The SH1106 display object used for rendering.
template <std::uint8_t Width, std::uint8_t Height>
void RenderGameGrid(const GameOfLife<Width, Height>& game, SH1106& display) noexcept {
  const auto& grid = game.GetGameGrid();
  for (std::uint8_t coord_y{0U}; coord_y < game.kGridHeight; ++coord_y) {
    for (std::uint8_t coord_x{0U}; coord_x < game.kGridWidth; ++coord_x) {
      const bool set{grid[coord_y][coord_x] != 0U};
      display.SetPixel(coord_x, coord_y, set);
    }
  }
}

/// @brief Returns a random number.
/// The function uses an ADC to generate a random number. It mixes values from several unconnected ADC channels.
/// @return A random number.
std::uint32_t GetRandomNumber() noexcept {
  auto& adc = hal::Adc::Instance();
  std::uint32_t number{0};
  for (std::size_t i{0}; i < sizeof(number); ++i) {
    constexpr std::uint32_t kShift{8U};
    const auto adc_channel = static_cast<std::uint8_t>(i);
    number <<= kShift;
    number |= adc.Read(adc_channel);
  }
  return number;
}

}  // namespace

/// @brief Main function.
int main() {
  InitializeSystem();

  hal::I2cBus i2c_bus(hal::I2cBusNumber::kOne);
  SH1106 display(i2c_bus);

  constexpr std::size_t kGameWidth{128U};
  constexpr std::size_t kGameHeight{64U};
  static_assert(kGameWidth <= SH1106::kDisplayWidth, "Display width too small");
  static_assert(kGameHeight <= SH1106::kDisplayHeight, "Display height too small");

  const std::uint32_t seed{GetRandomNumber()};
  GameOfLife<kGameWidth, kGameHeight> game(seed);

  while (true) {
    RenderGameGrid(game, display);
    display.Refresh();
    game.UpdateGameGrid();
  }
}
