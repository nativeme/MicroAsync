#include "Timer.hpp"

namespace async{

Timer::Timer(){
    Runtime::loopables.push_back(this);
}
Timer::Timer(const Params& builder){
    builder.builder();
    Runtime::loopables.push_back(this);
}
Timer::Timer(uint32_t time, bool auto_start, bool pulse){
    this->base_value = time;
    this->set_pulsing(pulse);
    Runtime::loopables.push_back(this);
    if(auto_start)
        this->start();
}

Timer::Timer(const Timer& other) 
    : pulse(other.pulse),
      state(other.state),
      resolution(other.resolution),
      base_value(other.base_value),
      start_time(other.start_time),
      target_time(other.target_time)
{
    Runtime::loopables.push_back(this);
}

Timer::Timer(Timer&& other) noexcept
    : pulse(other.pulse),
      state(other.state),
      resolution(other.resolution),
      base_value(other.base_value),
      start_time(other.start_time),
      target_time(other.target_time)
{
    Runtime::loopables.push_back(this);
    
    other.state = State::idle;
    other.start_time = 0;
    other.target_time = 0;
}

Timer& Timer::operator=(const Timer& other) {
    if (this != &other) {
        pulse = other.pulse;
        state = other.state;
        resolution = other.resolution;
        base_value = other.base_value;
        start_time = other.start_time;
        target_time = other.target_time;
    }
    return *this;
}

Timer& Timer::operator=(Timer&& other) noexcept {
    if (this != &other) {
        pulse = other.pulse;
        state = other.state;
        resolution = other.resolution;
        base_value = other.base_value;
        start_time = other.start_time;
        target_time = other.target_time;
        
        other.state = State::idle;
        other.start_time = 0;
        other.target_time = 0;
    }
    return *this;
}

Timer::~Timer(){
    for (size_t i = 0; i < async::Runtime::loopables.size(); i++){
        if(async::Runtime::loopables[i] == this) {
            async::Runtime::loopables.erase(async::Runtime::loopables.begin() + i);
            break;
        }
    }
}

void Timer::set_resolution(Resolution resolution){
    this->resolution = resolution;
}
void Timer::set_pulsing(bool is_pulsing){
    this->pulse = is_pulsing;
}

uint32_t Timer::read_time() const {
    switch (resolution)
    {
    case Resolution::millis_res:
        return millis();
    case Resolution::micros_res:
        return micros();
    default:
        break;
    }
    return millis();
}

void Timer::set_time(uint32_t time){
    this->base_value = time;
    target_time = start_time + base_value;
}
void Timer::add_time(int32_t time){
    target_time += time;
}

uint32_t Timer::get_base_time() const {
    return this->base_value;
}

uint32_t Timer::get_start_time() const {
    return this->start_time;
}

uint32_t Timer::get_time_passed() const {
    // Timer have not started so asked time haven't passed.
    if(start_time == 0){
        return 0;
    }
    return read_time() - start_time;
}

void Timer::start(){
    start_time = read_time();
    target_time = start_time + base_value;
    state = State::countdown;
}

void Timer::reset(){
    start();
}

void Timer::stop(){
    state = State::idle;
}

void Timer::clear(){
    state = State::idle;
    start_time = 0;
    target_time = 0;
}

Timer::State Timer::get_state() const {
    return this->state;
}

bool Timer::is_finished() const {
    return this->state == Timer::State::finished;
}

bool Timer::at_state(State state) const {
    return this->state == state;
}

bool Timer::have_passed(uint32_t time) const {
    // Timer have not started so asked time haven't passed.
    if(start_time == 0){
        return false;
    }
    return read_time() - start_time >= time;
}

void Timer::loop() {
    switch (state){
    case State::countdown:
        if(read_time() >= target_time){
            state = State::finished;
        }
        break;
    case State::finished:
        if(pulse) start();
        break;
    case State::idle:
        break;
    default:
        break;
    }
}

}