/**
 * @file Call.ino
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief 
 * This is example file for the "Async Call" of MicroAsync library.
 * Very simple to use and lightweight tool for creating async chains of statements.
 * @version 0.1
 * @date 2022-04-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <MicroAsync.h>

/*
    Async call is simple method to distribute events
    in time in asynchronous way without too much overhead.
*/

/* Create method to blink few times. */
void blink_led(){
    /*
        Create asynchronous call by method "call" form namespace "async".
        async::call

        Use function "then" to add next lambda statement to queue.
        Use function "wait" to add certain amount of time between statements. 
    */
    async::call()->then([](){
        Serial.println("Led builtin have turned on");
        digitalWrite(LED_BUILTIN, LOW);
    })->wait(500)->then([](){
        Serial.println("Led builtin have turned off");
        digitalWrite(LED_BUILTIN, HIGH);
    })->wait(1000)->then([](){
        Serial.println("Led builtin have turned on");
        digitalWrite(LED_BUILTIN, LOW);
    })->wait(300)->then([](){
        Serial.println("Led builtin have turned off");
        digitalWrite(LED_BUILTIN, HIGH);
    });
}

void setup(){
    Serial.begin(115200);
    while (!Serial){};

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Press 'b' to trigger blink event.");
}

void loop(){
    async::Runtime::loop();

    /*
        repeat on "b" pressed
    */
    if(Serial.available() > 0){
        static String serial_out;
        serial_out = Serial.readString();
        serial_out.trim();
        if(serial_out == "b"){
            blink_led();
        }
    }
}