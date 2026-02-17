#include "TimeSource.hpp"

#if defined(__has_include)
  #if __has_include(<Arduino.h>)
    #include <Arduino.h>
    #define MYLIB_HAS_ARDUINO 1
  #endif
#endif

#if !defined(MYLIB_HAS_ARDUINO)
  #include <chrono>
  #include <thread>
#endif

namespace async::detail {

[[nodiscard]] std::uint64_t default_micros() noexcept {
#if defined(MYLIB_HAS_ARDUINO)
  return std::uint64_t{micros()};
#else
  using clock = std::chrono::steady_clock;
  const auto us = std::chrono::duration_cast<std::chrono::microseconds>(
    clock::now().time_since_epoch()
  );
  return std::uint64_t{static_cast<std::uint64_t>(us.count())};
#endif
}

[[nodiscard]] std::uint64_t default_millis() noexcept {
#if defined(MYLIB_HAS_ARDUINO)
  return std::uint64_t{millis()};
#else
  using clock = std::chrono::steady_clock;
  const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
    clock::now().time_since_epoch()
  );
  return std::uint64_t{static_cast<std::uint64_t>(ms.count())};
#endif
}

void default_delay(async::TimeSource::delay_ms_t ms) noexcept {
#if defined(MYLIB_HAS_ARDUINO)
  ::delay(static_cast<unsigned long>(ms));
#else
  std::this_thread::sleep_for(std::chrono::milliseconds{ms});
#endif
}

} // namespace async::detail

#ifndef MYLIB_TIME_MICROS_FN
  #define MYLIB_TIME_MICROS_FN ::async::detail::default_micros
#endif

#ifndef MYLIB_TIME_MILLIS_FN
  #define MYLIB_TIME_MILLIS_FN ::async::detail::default_millis
#endif

#ifndef MYLIB_DELAY_FN
  #define MYLIB_DELAY_FN ::async::detail::default_delay
#endif


#ifndef MYLIB_IDLE_DELAY_MS
  #define MYLIB_IDLE_DELAY_MS 1u
#endif

namespace async {

TimeSource::micros_fn_t TimeSource::micros_fn_{MYLIB_TIME_MICROS_FN};
TimeSource::millis_fn_t TimeSource::millis_fn_{MYLIB_TIME_MILLIS_FN};
TimeSource::delay_fn_t TimeSource::delay_fn_{MYLIB_DELAY_FN};
TimeSource::delay_ms_t TimeSource::idle_delay_ms_{TimeSource::delay_ms_t{MYLIB_IDLE_DELAY_MS}};

TimeSource::tick_t TimeSource::start_us_{TimeSource::tick_t{0}};
TimeSource::tick_t TimeSource::start_ms_{TimeSource::tick_t{0}};
bool TimeSource::started_{false};

void TimeSource::configure(std::optional<micros_fn_t> micros_f,
                           std::optional<millis_fn_t> millis_f,
                           std::optional<delay_fn_t> delay_f,
                           std::optional<delay_ms_t> idle_delay_ms) noexcept {
  if (micros_f.has_value()) { micros_fn_ = *micros_f; }
  if (millis_f.has_value()) { millis_fn_ = *millis_f; }
  if (delay_f.has_value()) { delay_fn_ = *delay_f; }
  if (idle_delay_ms.has_value()) { idle_delay_ms_ = *idle_delay_ms; }
  reset();
}

TimeSource::tick_t TimeSource::elapsedMicros() noexcept {
  ensureStarted_();
  const tick_t now{micros_fn_()};
  return tick_t{now - start_us_};
}

TimeSource::tick_t TimeSource::elapsedMillis() noexcept {
  ensureStarted_();
  const tick_t now{millis_fn_()};
  return tick_t{now - start_ms_};
}

void TimeSource::delay(delay_ms_t ms) noexcept {
  delay_fn_(ms);
}

void TimeSource::idle() noexcept {
  delay_fn_(idle_delay_ms_);
}

void TimeSource::reset() noexcept {
  start_us_ = tick_t{micros_fn_()};
  start_ms_ = tick_t{millis_fn_()};
  started_ = true;
}

void TimeSource::ensureStarted_() noexcept {
  if (!started_) { reset(); }
}

} // namespace async
