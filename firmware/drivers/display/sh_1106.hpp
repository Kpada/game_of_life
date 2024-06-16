#ifndef FIRMWARE_DRIVERS_DISPLAY_SH_1106_HPP_
#define FIRMWARE_DRIVERS_DISPLAY_SH_1106_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

#include "../../hal/i2c.hpp"

/// @brief Class for controlling the SH1106 128x64 OLED display.
///
/// The SH1106 is a monochrome OLED display with a 128x64 pixel resolution. It's similar to the commonly used SSD1306.
/// If the connected display has uninitialized columns, it is likely that the display is actually an SH1106.
///
/// The class uses an internal buffer to store the display data.
class SH1106 {
 public:
  /// @brief The default I2C address of the display.
  static constexpr std::uint8_t kDefaultI2cAddress{0x3C};
  /// @brief The width of the display in pixels.
  static constexpr std::uint32_t kDisplayWidth{128U};
  /// @brief The height of the display in pixels.
  static constexpr std::uint32_t kDisplayHeight{64U};

  /// @brief Constructs an object.
  /// @param bus Reference to an @c I2cBus object.
  explicit SH1106(hal::I2cBus& bus, std::uint8_t address = kDefaultI2cAddress) noexcept
      : bus_{bus}, i2c_address_{address} {
    Initialize();
  }

  /// @brief Default destructor.
  ~SH1106() = default;

  /// @brief Deleted copy and move constructors and assignment operators.
  /// @{
  SH1106(const SH1106&) = delete;
  SH1106(SH1106&&) = delete;
  SH1106& operator=(const SH1106&) = delete;
  SH1106& operator=(SH1106&&) = delete;
  /// @}

  /// @brief Clears the display buffer.
  void Clear() noexcept { std::fill(displayBuffer_.begin(), displayBuffer_.end(), 0x00); }

  /// @brief Sets a data page in the display buffer.
  /// @param page Page number.
  void SetPage(std::uint8_t page) noexcept {
    const std::array<std::uint8_t, 4> cmds{
        kByteCommand,
        static_cast<std::uint8_t>(0xB0U + page),  // Set page address
        0x02U,                                    // Set lower column address
        0x10U,                                    // Set higher column address
    };
    bus_.Send(i2c_address_, cmds);
  }

  /// @brief Updates the display with the current buffer data.
  void Refresh() noexcept {
    for (std::uint8_t page{0U}; page < kPageNumber; ++page) {
      SetPage(page);

      const std::uint32_t pos{page * kDisplayWidth};

      // The I2C data package should always start with the special byte to indicate this is a data package. Instead of
      // creating a temporary buffer with one extra byte and the exactly same payload for every page, we extended the
      // display buffer with one extra byte. Thus, we can temporary modify the byte before the actual data, and then
      // restore its value.
      const std::uint8_t tmp_byte = displayBuffer_[pos];
      displayBuffer_[pos] = kByteData;

      bus_.Send(i2c_address_, &displayBuffer_[pos], kDisplayWidth + 1);

      displayBuffer_[pos] = tmp_byte;
    }
  }

  /// @brief Sets a pixel in the display buffer.
  /// @param coord_x X coordinate.
  /// @param coord_y Y coordinate.
  /// @param set @c true for set, @c false otherwise.
  void SetPixel(std::uint8_t coord_x, std::uint8_t coord_y, bool set) noexcept {
    if ((coord_x >= kDisplayWidth) || (coord_y >= kDisplayHeight)) {
      return;
    }

    const std::uint8_t page{static_cast<std::uint8_t>(coord_y / __CHAR_BIT__)};
    const std::uint8_t bit{static_cast<std::uint8_t>(coord_y % __CHAR_BIT__)};

    // Calculate the position in the display buffer, taking into account the start position of the data in the buffer
    const std::uint32_t pos{page * kDisplayWidth + coord_x + kBufferDataStartPosition};

    if (set) {
      displayBuffer_[pos] |= (1U << bit);
    } else {
      displayBuffer_[pos] &= ~(1U << bit);
    }
  }

 private:
  /// @brief Initializes the display.
  void Initialize() noexcept {
    constexpr std::array<std::uint8_t, 20> kCommands = {
        kByteCommand,
        0xAE,  // Display off
        0xA8,  // Set multiplex ratio
        0x3F,  // 1/64 duty
        0xD3,  // Set display offset
        0x00,  // No offset
        0x40,  // Set start line address
        0xA1,  // Set segment re-map
        0xC8,  // COM output scan direction
        0xDA,  // COM pins hardware configuration
        0x12,
        0x81,  // Contrast control
        0x7F,
        0xA4,  // Entire display on
        0xA6,  // Normal display
        0xD5,  // Set display clock divide ratio/oscillator frequency
        0x80,
        0x8D,  // Enable charge pump regulator
        0x14,
        0xAF,  // Display on
    };
    bus_.Send(i2c_address_, kCommands);

    Clear();
    Refresh();
  }

  /// @brief Sends a command to the display.
  /// @param command The command to send.
  void SendCommand(std::uint8_t command) noexcept {
    const std::array<std::uint8_t, 2> buffer{kByteCommand, command};
    bus_.Send(i2c_address_, buffer);
  }

  /// @brief The command byte. Every sequence of commands should start with this byte.
  static constexpr std::uint8_t kByteCommand{0x00U};

  /// @brief The data byte. Every sequence of data should start with this byte.
  static constexpr std::uint8_t kByteData{0x40U};

  /// @brief The number of pages in the display.
  static constexpr std::uint8_t kPageNumber{8U};

  /// @brief The position of the first byte in the display buffer.
  ///
  /// The I2C data package should always start with the special byte @c byteData to indicate this is a data package.
  /// Instead of creating a temporary buffer with one extra byte and the exactly same payload for every page, we
  /// extended the display buffer with one extra byte. Thus, we can temporary modify the byte before the actual data,
  /// and then restore its value.
  static constexpr std::uint32_t kBufferDataStartPosition{1U};

  /// @brief The Size of the display buffer.
  static constexpr std::size_t kBufferSize{kBufferDataStartPosition + kDisplayWidth * (kDisplayHeight / __CHAR_BIT__)};

  /// @brief The display buffer.
  std::array<std::uint8_t, kBufferSize> displayBuffer_{};

  /// @brief The I2C bus.
  hal::I2cBus& bus_;

  /// @brief The I2C address.
  std::uint8_t i2c_address_;
};

#endif  // FIRMWARE_DRIVERS_DISPLAY_SH_1106_HPP_
