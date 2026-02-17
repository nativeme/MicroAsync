/**
 * @file Processes.ino
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief 
 * This is example file for the "Async Process" of MicroAsync library.
 * "Process" is designed to perform tasks cyclically, within a specified interval.
 * It have few event methods that simplify state managment like "on_start", "on_stop" etc.
 * @version 0.1
 * @date 2022-04-07
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include <MicroAsync.h>

/*
    Process will execute "on_working" codeblock every interval.
    There are few event actions that can be defined with process:
    
    - on_create     Triggers when process is created.
    - on_start      Trigers always when process is started.
    - on_working    Triggers on every interval.
    - on_stop       Trigers always when process is stopped.
    - on_shutdown   Trigers when process get "shutdown" command.

    You can define them by assigning lambdas.
*/

async::Process blinker = async::Process(Params([](){
    blinker.set_name("blinker");
    blinker.set_interval(300);
    blinker.on_create = [](){
        pinMode(LED_BUILTIN, OUTPUT);
    };
    blinker.on_start = [](){
        digitalWrite(LED_BUILTIN, LOW);
    };
    blinker.on_working = [](){
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    };
    blinker.on_stop = [](){
        digitalWrite(LED_BUILTIN, HIGH);
    };
}));

async::Process clock_time = async::Process(Params([](){
    clock_time.set_name("clock_time");
    clock_time.set_interval(1000);
    clock_time.on_create = [](){
        Serial.begin(115200);
    };
    clock_time.on_working = [](){
        Serial.printf("Time passed: %lu seconds\n", millis() / 1000);
    };
}));


void setup(){
    /*
        Exaple of process groups usage:
    */
    async::ProcessGroup pg = {blinker, clock_time};
    pg.set_name("blink_clock");
    Serial.printf("Process group '%s' contains %d processes:\n", pg.get_name(), pg.size());
    for (auto &&process : pg){
        Serial.printf("id: %d name: %s\n", process->get_id(), process->get_name().data());
    }
    auto bl = pg.select_process("blinker");
    Serial.printf("Found process id: %d name: %s interval: %d\n", bl->get_id(), bl->get_name().data(), bl->get_interval());
}

void loop(){
    async::Runtime::loop();
}