/**
 * @file ProcessGroupControl.ino
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief 
 * This example creates two LED blinker processes and adds them to a ProcessGroup.
 * The group is then used to start, stop, and toggle both processes simultaneously.
 * @date 2022-05-23
 * @copyright Copyright (c) 2022
 */
 
#include <Arduino.h>
#include <MicroAsync.h>

// Define pins for LEDs (using built-in LED and a virtual one for demo)
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// Process 1: Blinks quickly
async::Process fast_blinker = async::Process(Params([](){
    fast_blinker.set_name("Fast Blinker");
    fast_blinker.set_interval(200);
    fast_blinker.on_working = [](){
        Serial.println("Fast blink!");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    };
}));

// Process 2: Prints a message slowly
async::Process slow_logger = async::Process(Params([](){
    slow_logger.set_name("Slow Logger");
    slow_logger.set_interval(1000);
    slow_logger.on_working = [](){
        Serial.println("Slow logger is working...");
    };
}));

// Create a group and add processes
async::ProcessGroup my_group(fast_blinker, slow_logger);

// Controller timer to switch states of the group
async::Timer controller(5000, true); 
int state = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial){};

    pinMode(LED_BUILTIN, OUTPUT);
    
    Serial.println("Starting Process Group Demo");
    
    // Start all processes in the group
    my_group.start_all();
}

void loop() {
    async::Runtime::loop();

    // Every 5 seconds, change the state of the group
    if (controller.is_finished()) {
        state++;
        controller.reset();
        
        switch (state % 4) {
            case 0:
                Serial.println(">>> Starting all processes <<<");
                my_group.start_all();
                break;
            case 1:
                Serial.println(">>> Toggling all processes (should stop) <<<");
                my_group.toggle_all();
                break;
            case 2:
                Serial.println(">>> Toggling all processes (should start) <<<");
                my_group.toggle_all();
                break;
            case 3:
                Serial.println(">>> Stopping all processes <<<");
                my_group.stop_all();
                break;
        }
    }
}
