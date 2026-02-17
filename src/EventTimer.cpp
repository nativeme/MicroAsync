#include "EventTimer.hpp"

namespace async{
    EventTimer::EventTimer(const Params &&builder)
    :   Timer(std::move(builder))
    {
        builder.builder();
    };
    
    EventTimer::EventTimer(const uint32_t &time, const bool &auto_start, const bool &pulse)
    :   Timer(time, auto_start, pulse)
    {

    };
    
    

    void EventTimer::clear(){
        state = State::idle;
        start_time = 0;
        target_time = 0;
        for (auto &&event : this->events){
            event.done = false;
        }
    }

    void EventTimer::add_event(const uint32_t& execute_at, std::function<void(void)>&& event){
        this->events.push_back(TimedEvent(execute_at, std::move(event)));
    }

    void EventTimer::loop(){
        switch (state){
        case State::finished:
            if(pulse) start();
            break;
        case State::countdown:
            for (auto &&event : events){
                if(!event.done && this->have_passed(event.execute_at)){
                    event.event();
                    event.done = true;
                } 
            }

            if(read_time() >= target_time){
                state = State::finished;
                for (auto &&event : events){
                    event.done = false;
                }
            };
            break;
        case State::idle:
            break;
        default:
            break;
        }
    }

}
