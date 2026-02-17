# MicroAsync

Library providing asynchrony to the microcontrollers in few popular trends known from higher level programing languages.

1) Calls
2) Processes
3) Async functions

## Async call
Simple one-time call that will execute statements in order.
Very useful in situations, where one event happens, and you want to trigger
a series of events spread out over time without blocking.

See full example: [examples](/../../tree/master/examples/Call/Call.ino)
##### Sample snippet:
``` c++
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
```
## Async process
Processes are designed for maintaining cyclic events that happen in certain interval of time.
That could be for example a sensor reading and sending data over the network.
If you declare few blinker processes like shown below all leds will blink asynchronously independent.

See full example: [examples](/../../tree/master/examples/Processes/Processes.ino) 
##### Sample snippet:
``` c++
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
```
## Async function
An approach that allows the use of linguistically similar mechanisms
to high-level languages such as Python, C # etc ...
##### Sample snippet:
``` c++
async::function<String(void)> async_readline = [](){
    static String buf;
    static bool clear = false;
    int c = Serial.read();
    bool return_if = c == '\r';

    if(clear){
        buf.clear();
        clear = false;
    } else {
        if(c >= 0 && c != '\r')
            buf += (char)c;

        if(return_if)
            clear = true;
    }
    returns(async_readline, return_if, buf);
};

String out = async_readline()->await()->value();
```
See full example: [examples](/../../tree/master/examples/FunctionAndProcess.cpp)

See all examples: [examples](/../../tree/master/examples)