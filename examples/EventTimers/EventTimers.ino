/**
 * @file EventTimers.cpp
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief 
 * This is example file for the "EventTimer" of MicroAsync library.
 * EventTimer is works similarly to classic "Timer".
 * You can attach to it events by passing lambdas to "add_event" method.
 * @date 2022-05-23
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include <MicroAsync.h>


/*
    Create EventTimer instance by passing:
*/
auto event_timer = async::EventTimer(5000,  /* Countdown time */
                                     true,  /* Auto start timer after creation of instance  (optional - def. false) */
                                     true); /* Auto reastart timer after countdown finishes (optional - def. false) */

void setup(){
    Serial.begin(115200);
    while (!Serial){};

    /*
        Add few dummy events to EventTimer for testing purposes by "add_event(at_time, lambda<void(void)>) method"
    */
    event_timer.add_event(2000, [](){
        Serial.println("2000 millis have passed since timer start!");
    });
    event_timer.add_event(3500, [](){
        Serial.println("3500 millis have passed since timer start!");
    });
}

    /*
        After 2000, first event will be executed,
        after 3500, second event will be executed,
        when timer reach 5000 millis, it will auto restart itself and cycle will repeat.
    */

void loop(){
    async::Runtime::loop();
}