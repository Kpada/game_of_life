#ifndef FIRMWARE_DISPLAY_TOOLS_HPP_
#define FIRMWARE_DISPLAY_TOOLS_HPP_

#include <algorithm>
#include <array>
#include <cstdint>

#include "drivers/display/sh_1106.hpp"

namespace utils {
namespace details {

/// @brief The width of the character font.
constexpr std::uint8_t kFontWidth{5U};
/// @brief The height of the character font.
constexpr std::uint8_t kFontHeight{8U};

/// @brief A 5x8 pixel character font pattern.
using FontType = std::array<std::uint8_t, kFontWidth>;

/// @brief Retrieves the font for a given character.
///
/// @param character The character for which to retrieve the font.
/// @return The font for the given character.
constexpr FontType GetCharacterFont(char character) noexcept {
  constexpr char kCharMin{'0'};
  constexpr char kCharMax{'9'};

  // Simple font for digits '0' to '9'.
  constexpr std::array<FontType, 10> kFont{{
      {0x3E, 0x51, 0x49, 0x45, 0x3E},  // '0'
      {0x00, 0x42, 0x7F, 0x40, 0x00},  // '1'
      {0x42, 0x61, 0x51, 0x49, 0x46},  // '2'
      {0x21, 0x41, 0x45, 0x4B, 0x31},  // '3'
      {0x18, 0x14, 0x12, 0x7F, 0x10},  // '4'
      {0x27, 0x45, 0x45, 0x45, 0x39},  // '5'
      {0x3E, 0x49, 0x49, 0x49, 0x30},  // '6'
      {0x01, 0x71, 0x09, 0x05, 0x03},  // '7'
      {0x36, 0x49, 0x49, 0x49, 0x36},  // '8'
      {0x06, 0x49, 0x49, 0x29, 0x1E}   // '9'
  }};

  if ((character < kCharMin) || (character > kCharMax)) {
    character = kCharMin;
  }

  return kFont[character - kCharMin];
}

}  // namespace details

/// @brief Displays a character on the SH1106 display at specified coordinates.
/// @param character The character to display (only '0' to '9' are supported).
/// @param coord_x The x-coordinate on the display.
/// @param coord_y The y-coordinate on the display.
/// @param display Reference to the SH1106 display object.
inline void DisplayCharacter(char character, std::uint8_t coord_x, std::uint8_t coord_y, SH1106& display) noexcept {
  const auto font = details::GetCharacterFont(character);

  for (std::uint8_t i{0U}; i < static_cast<std::uint8_t>(font.size()); ++i) {
    const auto byte = font[i];
    for (std::uint8_t bit{0U}; bit < details::kFontHeight; ++bit) {
      const bool set{static_cast<bool>((byte >> bit) & 0x01U)};
      display.SetPixel(coord_x + i, coord_y + bit, set);
    }
  }
}

/// @brief Displays a number on the SH1106 display.
/// @param number The number to display.
/// @param coord_x The x coordinate to start the display.
/// @param coord_y The y coordinate to start the display.
/// @param display Reference to the SH1106 display object.
inline void DisplayNumber(std::uint32_t number, std::uint8_t coord_x, std::uint8_t coord_y, SH1106& display) noexcept {
  // Convert number to string
  constexpr std::size_t kMaxLength{12U};
  std::array<char, kMaxLength> buffer{0};
  std::size_t index{0};

  while (number > 0U) {
    constexpr std::uint32_t kDivider{10U};
    buffer[index] = '0' + (number % kDivider);
    number /= kDivider;
    ++index;
  }

  // Reverse the string
  std::reverse(buffer.begin(), buffer.begin() + index);

  // Display each character in the string
  for (std::size_t i{0U}; i < index; ++i) {
    constexpr std::uint8_t kOffsetX{details::kFontWidth + 1U};
    DisplayCharacter(buffer[i], coord_x + (i * kOffsetX), coord_y, display);
  }
}

}  // namespace utils

#endif  // FIRMWARE_DISPLAY_TOOLS_HPP_
