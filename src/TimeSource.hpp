#pragma once

#include <cstdint>
#include <optional>

namespace async {

class TimeSource final {
public:
  using tick_t = std::uint64_t;

  using micros_fn_t = tick_t (*)() noexcept;
  using millis_fn_t = tick_t (*)() noexcept;

  using delay_ms_t = std::uint32_t;
  using delay_fn_t = void (*)(delay_ms_t) noexcept;

  TimeSource() = delete;

  /** @param[in] micros_f Micros fn. */
  /** @param[in] millis_f Millis fn. */
  /** @param[in] delay_f Delay fn (ms). */
  /** @param[in] idle_delay_ms Idle sleep (ms). */
  static void configure(std::optional<micros_fn_t> micros_f = std::nullopt,
                        std::optional<millis_fn_t> millis_f = std::nullopt,
                        std::optional<delay_fn_t> delay_f = std::nullopt,
                        std::optional<delay_ms_t> idle_delay_ms = std::nullopt) noexcept;

  [[nodiscard]] static tick_t elapsedMicros() noexcept;
  [[nodiscard]] static tick_t elapsedMillis() noexcept;

  static void delay(delay_ms_t ms) noexcept;

  /** @brief Sleeps when idle. */
  static void idle() noexcept;

  static void reset() noexcept;

private:
  static void ensureStarted_() noexcept;

  static micros_fn_t micros_fn_;
  static millis_fn_t millis_fn_;
  static delay_fn_t delay_fn_;
  static delay_ms_t idle_delay_ms_;

  static tick_t start_us_;
  static tick_t start_ms_;
  static bool started_;
};

} // namespace async
