#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include <cstdint>

#include "Arduino.h"
#include "ILoopable.hpp"
#include "Runtime.hpp"
#include "Params.hpp"
#include "TimeSource.hpp"

namespace async{
class Timer : 
public ILoopable
{
public:
    enum class State : uint8_t {
        idle = 0,
        countdown = 1,
        finished = 2
    }; 

    enum class Resolution : uint8_t {
        millis_res = 0,
        micros_res = 1
    };

protected:
    uint32_t read_time() const;
    /// @brief Flag telling if timer should work i pulsing mode.
    // TODO make more descriptive comment.
    bool pulse = false;
    State state = State::idle;
    Resolution resolution = Resolution::millis_res;
    uint32_t base_value = 0;
    /// @brief Milis/Micros snapshot when timer started.
    uint32_t start_time = 0;
    /// @brief Milis/Micros value for Timer to strive to (start_time + base_value).
    uint32_t target_time = 0;

public:
    /** @brief Construct a new Timer (inactive). */
    Timer();
    /** @brief Construct using Params builder. */
    Timer(const Params& builder);
    /** @brief Construct with duration and options. */
    Timer(uint32_t time, bool auto_start = false, bool pulse = false);
    Timer(const Timer& other);
    Timer(Timer&& other) noexcept;
    Timer& operator=(const Timer& other);
    Timer& operator=(Timer&& other) noexcept;
    virtual ~Timer() override;

    /** @brief Get current state. */
    State get_state() const;
    /** @brief Set duration. */
    void set_time(uint32_t time);
    /** @brief Add/subtract time from duration. */
    void add_time(int32_t time);
    /** @brief Get base duration. */
    uint32_t get_base_time() const;
    /** @brief Get start timestamp. */
    uint32_t get_start_time() const;
    /** @brief Get elapsed time since start. */
    uint32_t get_time_passed() const;
    /** @brief Set resolution (millis/micros). */
    void set_resolution(Resolution resolution);
    /** @brief Enable/disable pulsing (auto-restart). */
    void set_pulsing(bool is_pulsing = true);
    
    /** @brief Start the timer. */
    void start();
    /** @brief Reset the timer. */
    void reset();
    /** @brief Stop the timer. */
    void stop();
    /** @brief Clear the timer state. */
    virtual void clear();
    
    /** @brief Check if finished. */
    bool is_finished() const;
    /** @brief Check if in specific state. */
    bool at_state(State state) const;
    /** @brief Check if specific time has passed. */
    bool have_passed(uint32_t time) const;
    
    virtual void loop() override;
};
}


#endif