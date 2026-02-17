/**
 * @file Functions.ino
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gmail.com)
 * @brief 
 * This is example file for the "Async Function" of MicroAsync library.
 * "Async Function" isn't fully tested yet, so have in mind that isn't production ready feature.
 * @version 0.1
 * @date 2022-04-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <MicroAsync.h>

/*
    This example will mime event of people
    going to the party.
*/

/* 
    Lets create small data structure for party member.
*/
struct PartyMamber{
    int id = 0;
    const char* name;
    async::Timer is_arrived_timer = async::Timer(1000);

    PartyMamber(const char*&& name, const uint32_t&& time_for_arrive)
    :   name(name)
    {
        this->is_arrived_timer.set_time(time_for_arrive);
    }

    void get_to_party(){
        is_arrived_timer.start();
    }

    bool arrived() const { 
        return is_arrived_timer.is_finished();
    }
};

auto john   = PartyMamber("John"  , 6000);
auto jane   = PartyMamber("Jane"  , 2500);
auto peter  = PartyMamber("Peter" , 1000);
auto nataly = PartyMamber("Nataly", 4000);
auto adam   = PartyMamber("Adam"  , 8000);
auto olivia = PartyMamber("Olivia", 9000);
auto josh   = PartyMamber("Josh"  , 2000);

/*
    Lets create async function that returns greetings for every person who arrive.

    async::function<ReturnType(ParameterType, ParameterType...)> function_name = [](ParameterType param_name, ParameterType... param_name...){
    
        BODY THAT WILL BE EXECUTED IN ASYNCHRONOUS LOOP UNTIL CONDITION IS MET.
    
        returns(function_name, [bool]done_condition, return_value);
    }
*/

async::function<String(const PartyMamber&)> person_arrived = [](const PartyMamber& member){

    if(member.arrived()){
        char welcome_msg[64];
        sprintf(welcome_msg, "Hello %s! Welcome!", member.name);
        
        async_return(person_arrived, member.arrived(), String(welcome_msg));
    }
};

void setup(){
    Serial.begin(115200);
    while (!Serial){};

    Serial.printf("entered setup block at %lu millis\n", millis());

    josh.get_to_party();

    auto josh_greet = person_arrived(josh);
    Serial.println(josh_greet->await()->value());

    peter.get_to_party();
    jane.get_to_party();
    nataly.get_to_party();

    /* 
        Long types of "Futures" can be easily evaluated by auto 
    */
    auto peter_great  = person_arrived(peter);
    auto nataly_great = person_arrived(nataly);
    auto jane_great   = person_arrived(jane);

    async::gather(jane_great, peter_great, nataly_great);
    
    Serial.println(jane_great->value());
    Serial.println(peter_great->value());
    Serial.println(nataly_great->value());
    
    olivia.get_to_party();
    Serial.println(person_arrived(olivia)->await()->value());

    Serial.printf("left setup block at %lu millis\n", millis());
}

void loop(){
    async::Runtime::loop();    
}