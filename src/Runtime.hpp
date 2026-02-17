#ifndef _RUNTIME_HPP_
#define _RUNTIME_HPP_

#include <vector>
#include <memory>
#include <algorithm>
#include "IAsyncFunction.hpp"
#include "ILoopable.hpp"
#include "TimeSource.hpp"

namespace async {

/**
 * @brief The core engine that drives all asynchronous operations.
 * 
 * The Runtime class maintains lists of all active asynchronous objects
 * (Timers, Processes, Calls, Functions) and iterates through them
 * in its loop() method. You must call async::Runtime::loop() in your
 * main Arduino loop() for the library to function.
 */
class Runtime {
    Runtime() = delete;
    ~Runtime() = delete;

    template <typename T>
    static void loop_vector(std::vector<T*>& vec) {
        for (size_t i = 0; i < vec.size(); ) {
            T* current = vec[i];
            if (current) {
                current->loop();
            }
            // Check if element at i is still current (wasn't removed)
            // If removed, vec[i] would be the next element (or i is out of bounds)
            if (i < vec.size() && vec[i] == current) {
                i++;
            }
        }
    }

public:
    static inline std::vector<ILoopable*> loopables;      ///< Generic loopable objects (e.g., Timers)
    static inline std::vector<ILoopable*> async_calls;    ///< One-time async calls
    static inline std::vector<ILoopable*> processes;      ///< Cyclic processes
    static inline std::vector<IAsyncFunction*> async_functions; ///< Async functions

    /**
     * @brief Executes one iteration of the async loop.
     * 
     * Calls loop() on all registered objects. Must be called frequently.
     * Includes a call to yield() to keep the ESP8266/ESP32 watchdog happy.
     */
    static void loop() {
    #if __has_include(<Arduino.h>)
        ::yield();
    #else
        if (loopables.empty() && processes.empty() &&
            async_calls.empty() && async_functions.empty()) {
            TimeSource::idle(); // def sleep when idle to not burn CPU
            return;
        }
    #endif

        loop_vector(loopables);
        loop_vector(processes);
        loop_vector(async_calls);
        loop_vector(async_functions);
    }

};

} // namespace async

#endif
