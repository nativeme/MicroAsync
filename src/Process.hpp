/**
 * @file Process.hpp
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief
 * Defines the "Process" class for "MicroAsync".
 * @version 0.1
 * @date 2022-04-03
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef _PROCESS_HPP_
#define _PROCESS_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include "Params.hpp"
#include "Timer.hpp"
#include "ILoopable.hpp"
#include "Runtime.hpp"

namespace async {
class Process : public ILoopable {
public:
    enum class State : uint8_t {
        created,
        stopped,
        working,
        shutting_down,
    };

private:
    int16_t id = -1;
    State state = State::created;

    void new_id();     
    Timer internal_timer = Timer(1000, true, true);
    std::string name = "";

public:
    /** @brief Callback executed when the process is created. */
    std::function<void()> on_create   = [](){};
    /** @brief Callback executed when the process starts. */
    std::function<void()> on_start    = [](){};
    /** @brief Callback executed on every interval loop. */
    std::function<void()> on_working  = [](){};
    /** @brief Callback executed when the process stops. */
    std::function<void()> on_stop     = [](){};
    /** @brief Callback executed when the process is shut down. */
    std::function<void()> on_shutdown = [](){};

    Process();
    Process(const Params& builder);
    Process(const Process& other);
    Process(Process&& other) noexcept;
    Process& operator=(const Process& other);
    Process& operator=(Process&& other) noexcept;
    ~Process() override = default;

    /** @brief Get the process name. */
    std::string_view get_name() const;
    /** @brief Set the process name. */
    void set_name(std::string_view name);
    
    /** @brief Get the unique process ID. */
    int16_t get_id() const;
    
    /** @brief Get the execution interval (timer base time). */
    uint32_t get_interval() const;
    /** @brief Set the execution interval. */
    void set_interval(uint32_t interval);

    /** @brief Set the timer resolution (millis/micros). */
    void set_resolution(Timer::Resolution resolution);

    /** @brief Get current state. */
    State get_state() const;
    /** @brief Check if the process is in working state. */
    bool is_working() const;

    /** @brief Start the process. */
    void start();
    /** @brief Toggle between start and stop. */
    void toggle();
    /** @brief Force a single work iteration (triggers on_working). */
    void work();
    /** @brief Stop the process. */
    void stop();
    /** @brief Shut down the process (triggers on_shutdown and stops). */
    void shutdown();
    /** @brief Kill the process (removes from runtime, stops execution). */
    void kill();

    void loop() override;
};
}
#endif
