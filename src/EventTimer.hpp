#ifndef _EVENTTIMER_HPP_
#define _EVENTTIMER_HPP_
#include "Arduino.h"
#include "Timer.hpp"
#include "Params.hpp"

namespace async{
/**
 * @brief 
 * Classic timer expanded by possibility of executing
 * events at certain moment after start of the timer.
 * Pass events by event_timer.add_event( time, [](){ some_logic } );
 */
class EventTimer
:   public Timer
{
private:
    struct TimedEvent{
        const uint32_t execute_at;
        const std::function<void(void)> event;
        bool done;
        TimedEvent(const uint32_t& execute_at, const std::function<void(void)>&& event)
        :   execute_at(execute_at),
            event(event),
            done(false)
        {};
    };

    std::vector<TimedEvent> events;
    
public:
    EventTimer(const Params &&builder);
    
    /** @brief Construct EventTimer with duration and options. */
    EventTimer(const uint32_t &time, const bool &auto_start = false, const bool &pulse = false);
    
    ~EventTimer();

    virtual void clear() override;

    /**
     * @brief Add an event to be executed at a specific time.
     * @param execute_at Time offset from start when event should run.
     * @param event Lambda function to execute.
     */
    void add_event(const uint32_t& execute_at, std::function<void(void)>&& event);

    virtual void loop();
};
}
#endif
