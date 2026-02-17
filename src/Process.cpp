#include "Process.hpp"

namespace async {

Process::Process() {
    this->new_id();
    async::Runtime::processes.push_back(this);
}

Process::Process(const Params& builder) {
    this->new_id();
    builder.builder();
    this->on_create();
    async::Runtime::processes.push_back(this);
}

Process::Process(const Process& other)
    : id(-1),
      state(other.state),
      internal_timer(other.internal_timer),
      name(other.name),
      on_create(other.on_create),
      on_start(other.on_start),
      on_working(other.on_working),
      on_stop(other.on_stop),
      on_shutdown(other.on_shutdown)
{
    this->new_id();
    async::Runtime::processes.push_back(this);
}

Process::Process(Process&& other) noexcept
    : id(other.id),
      state(other.state),
      internal_timer(std::move(other.internal_timer)),
      name(std::move(other.name)),
      on_create(std::move(other.on_create)),
      on_start(std::move(other.on_start)),
      on_working(std::move(other.on_working)),
      on_stop(std::move(other.on_stop)),
      on_shutdown(std::move(other.on_shutdown))
{
    // Do not generate new ID for moved process, it takes identity of other.
    async::Runtime::processes.push_back(this);
    
    // Invalidate other
    other.state = State::stopped;
    // other.internal_timer is moved, so it's in valid state.
}

Process& Process::operator=(const Process& other) {
    if (this != &other) {
        state = other.state;
        internal_timer = other.internal_timer;
        name = other.name;
        on_create = other.on_create;
        on_start = other.on_start;
        on_working = other.on_working;
        on_stop = other.on_stop;
        on_shutdown = other.on_shutdown;
        // Keep own ID
    }
    return *this;
}

Process& Process::operator=(Process&& other) noexcept {
    if (this != &other) {
        state = other.state;
        internal_timer = std::move(other.internal_timer);
        name = std::move(other.name);
        on_create = std::move(other.on_create);
        on_start = std::move(other.on_start);
        on_working = std::move(other.on_working);
        on_stop = std::move(other.on_stop);
        on_shutdown = std::move(other.on_shutdown);
        // Keep own ID or take other's ID? Usually move assignment takes state.
        // ID is identity. If we assign, we overwrite identity?
        // Let's keep ID stable for 'this'.
        
        other.state = State::stopped;
    }
    return *this;
}

Process::~Process() {
}

void Process::kill() {
    for (size_t i = 0; i < async::Runtime::processes.size(); i++) {
        if (async::Runtime::processes[i] == this) {
            async::Runtime::processes.erase(async::Runtime::processes.begin() + i);
            break;
        }
    }
}

void Process::new_id() {
    static int id_counter = 0;
    this->id = id_counter++;
}

void Process::work() {
    internal_timer.start();
    on_start();
    state = State::working;
    on_working();
}

void Process::start() {
    on_start();
    state = State::working;
}

void Process::stop() {
    on_stop();
    state = State::stopped;
}

void Process::toggle() {
    if (this->state == State::working) {
        stop();
    } else if (this->state == State::stopped) {
        start();
    }
}

void Process::shutdown() {
    on_shutdown();
    state = State::shutting_down;
}

String Process::get_name() const { 
    return this->name; 
}

void Process::set_name(const String& name) {
    this->name = name;
}

int16_t Process::get_id() const {
    return this->id;
}

uint32_t Process::get_interval() const {
    return this->internal_timer.get_base_time();
}

void Process::set_interval(uint32_t interval) {
    this->internal_timer.set_time(interval);
}

void Process::set_resolution(Timer::Resolution resolution) {
    this->internal_timer.set_resolution(resolution);
}

Process::State Process::get_state() const {
    return this->state;
}

bool Process::is_working() const {
    return this->state == State::working;
}

void Process::loop() {
    switch (this->state) {
        case State::working:
            if (this->internal_timer.is_finished()) on_working();
            break;
        case State::created:
            work();
            break;
        case State::stopped:
            break;
        case State::shutting_down:
            break;
        default:
            break;
    }
}

}
