#ifndef FIRMWARE_HAL_I2C_HPP_
#define FIRMWARE_HAL_I2C_HPP_

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

#include <array>
#include <cstdint>

namespace hal {

/// @brief I2C bus number enumeration.
enum class I2cBusNumber {
  kOne,
};

/// @brief I2C bus management class.
class I2cBus {
 public:
  /// @brief Constructs an I2cBus object.
  /// @param bus The @c I2cBusNumber number to initialize.
  explicit I2cBus(I2cBusNumber bus) noexcept : base_address_{I2C1} {
    if (bus == I2cBusNumber::kOne) {
      // GPIOB is used as SCL and SDA for I2C1.
      // SCL: PB6, SDA: PB7
      rcc_periph_clock_enable(RCC_I2C1);
      rcc_periph_clock_enable(RCC_GPIOB);
      gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO_I2C1_SCL | GPIO_I2C1_SDA);
    }

    Initialize();
  }

  /// @brief Deleted copy and move constructors and assignment operators.
  /// @{
  I2cBus(const I2cBus&) = delete;
  I2cBus& operator=(const I2cBus&) = delete;
  I2cBus(I2cBus&&) = delete;
  I2cBus& operator=(I2cBus&&) = delete;
  /// @}

  /// @brief Default destructor.
  ~I2cBus() noexcept = default;

  /// @brief Sends a buffer of data to a specified address.
  /// @tparam Size The size of the buffer.
  /// @param address The I2C address to send data to.
  /// @param buffer The data buffer to send.
  template <std::size_t Size>
  void Send(std::uint8_t address, const std::array<std::uint8_t, Size>& buffer) const noexcept {
    i2c_transfer7(base_address_, address, buffer.data(), buffer.size(), nullptr, 0);
  }

  /// @brief Sends a buffer of data to a specified address.
  /// @param address The I2C address to send data to.
  /// @param buffer The data buffer to send.
  /// @param size The size of the data buffer.
  void Send(std::uint8_t address, const uint8_t* buffer, std::size_t size) const noexcept {
    i2c_transfer7(base_address_, address, buffer, size, nullptr, 0);
  }

  /// @brief Sends a single byte to a specified address.
  /// @param address The I2C address to send the byte to.
  /// @param byte The byte to send.
  void Send(std::uint8_t address, std::uint8_t byte) const noexcept {
    i2c_transfer7(base_address_, address, &byte, 1, nullptr, 0);
  }

 private:
  /// @brief Initializes the I2C hardware.
  void Initialize() const noexcept {
    constexpr std::uint32_t kClockFrequency{36U};  // 36 MHz
    constexpr std::uint32_t kCcrValue{180U};       // Setup for fast mode (400 kHz)
    constexpr std::uint32_t kTriseValue{37U};      // 1000 ns / (1/36 MHz) + 1

    i2c_peripheral_disable(base_address_);
    i2c_set_clock_frequency(base_address_, kClockFrequency);
    i2c_set_fast_mode(base_address_);
    i2c_set_ccr(base_address_, kCcrValue);
    i2c_set_trise(base_address_, kTriseValue);
    i2c_peripheral_enable(base_address_);
  }

  std::uint32_t base_address_;
};

}  // namespace hal

#endif  // FIRMWARE_HAL_I2C_HPP_
