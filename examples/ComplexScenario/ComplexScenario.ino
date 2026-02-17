/**
 * @file ComplexScenario.ino
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief 
 * This example simulates a sensor reading process.
 * An async::Call chain monitors the sensor value.
 * When the value exceeds a threshold, it triggers an alarm (rapid LED blink).
 * @date 2022-05-23
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include <MicroAsync.h>

// Simulated sensor value
int sensorValue = 0;

// Process 1: Sensor Simulation
// Increments sensorValue every 100ms
async::Process sensorReader = async::Process(Params([](){
    sensorReader.set_name("Sensor Reader");
    sensorReader.set_interval(100);
    sensorReader.on_start = [](){
        Serial.println("[Sensor] Started reading...");
        sensorValue = 0;
    };
    sensorReader.on_working = [](){
        sensorValue += random(1, 5);
        Serial.printf("[Sensor] Value: %d\n", sensorValue);
    };
    sensorReader.on_stop = [](){
        Serial.println("[Sensor] Stopped reading.");
    };
}));

// Process 2: Alarm
// Blinks LED rapidly
async::Process alarm = async::Process(Params([](){
    alarm.set_name("Alarm");
    alarm.set_interval(50);
    alarm.on_start = [](){
        Serial.println("[Alarm] ALARM TRIGGERED!");
    };
    alarm.on_working = [](){
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    };
    alarm.on_stop = [](){
        digitalWrite(LED_BUILTIN, LOW); // Off
        Serial.println("[Alarm] Alarm silenced.");
    };
}));

void setup() {
    Serial.begin(115200);
    while (!Serial){};

    pinMode(LED_BUILTIN, OUTPUT);
    
    Serial.println("Starting Complex Scenario Demo");

    // Start the scenario using a Call chain
    async::Call* scenario = new async::Call();
    
    scenario->then([](){
        Serial.println("[Main] Starting sensor process...");
        sensorReader.start();
    })
    ->wait(2000) // Let it run for 2 seconds blindly
    ->then([](){
        Serial.println("[Main] 2 seconds passed. Waiting for value > 50...");
    })
    ->await([](){
        // specific 'await' that blocks the chain (but not the loop) until true
        return sensorValue > 50;
    })
    ->then([](){
        Serial.println("[Main] Threshold reached! Stopping sensor, starting alarm.");
        sensorReader.stop();
        alarm.start();
    })
    ->wait(3000) // Let alarm ring for 3 seconds
    ->then([](){
        Serial.println("[Main] Silencing alarm.");
        alarm.stop();
        Serial.println("[Main] Scenario complete. Restarting in 5 seconds...");
    })
    ->wait(5000)
    ->then([](){
        // Restart the whole scenario by creating a new call or resetting?
        // Ideally, we'd wrap this in a function to call again.
        // For simplicity, we just print "Done".
        Serial.println("[Main] Done. Reset board to repeat.");
        // To repeat, we could call a function that creates this chain again.
    });
}

void loop() {
    async::Runtime::loop();
}
