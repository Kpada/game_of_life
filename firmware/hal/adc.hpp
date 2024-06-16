#ifndef FIRMWARE_HAL_ADC_HPP_
#define FIRMWARE_HAL_ADC_HPP_

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

#include <cstdint>

namespace hal {

/// @brief Manages the ADC peripheral.
/// The class is implemented as a singleton.
class Adc {
 public:
  /// @brief Deleted copy and move constructors and assignment operators.
  /// @{
  Adc(const Adc&) = delete;
  Adc& operator=(const Adc&) = delete;
  Adc(Adc&&) = delete;
  Adc& operator=(Adc&&) = delete;
  /// @}

  /// @brief Default destructor.
  ~Adc() noexcept = default;

  /// @brief Gets the singleton instance of the class.
  /// @return The singleton instance of the @c Adc class.
  static Adc& Instance() noexcept {
    static Adc instance;
    return instance;
  }

  /// @brief Reads the ADC value from the specified channel.
  /// @param channel The ADC channel to read.
  /// @return The ADC value from the specified channel.
  std::uint16_t Read(std::uint8_t channel) const noexcept {  // NOLINT(readability-convert-member-functions-to-static)
    adc_set_regular_sequence(ADC1, 1, &channel);

    adc_start_conversion_direct(kAdcAddress);
    while (!adc_eoc(kAdcAddress)) {
    }
    return adc_read_regular(kAdcAddress);
  }

 private:
  /// @brief Private constructor to enforce singleton pattern.
  Adc() noexcept { Initialize(); }

  /// @brief Initializes the ADC peripheral.
  static void Initialize() noexcept {
    // Enable ADC clock.
    static_assert(kAdcAddress == ADC1, "Only ADC1 is supported");
    rcc_periph_clock_enable(RCC_ADC1);

    // Configure the ADC.
    adc_power_off(kAdcAddress);
    adc_disable_scan_mode(kAdcAddress);
    adc_set_single_conversion_mode(kAdcAddress);
    adc_enable_temperature_sensor();
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC);
    adc_power_on(kAdcAddress);

    // Wait for ADC to stabilize.
    constexpr std::int32_t kWaitCycles{800'000};  // A magic number from the example code.
    for (volatile std::int32_t i{0}; i < kWaitCycles; i++) {
      __asm__("nop");
    }

    // Calibrate the ADC
    adc_reset_calibration(kAdcAddress);
    adc_calibrate(kAdcAddress);
  }

  /// @brief The ADC peripheral address.
  static constexpr std::uint32_t kAdcAddress{ADC1};
};

}  // namespace hal

#endif  // FIRMWARE_HAL_ADC_HPP_
