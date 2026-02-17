#include "Call.hpp"

namespace async{

Call::Call(){
    Runtime::async_calls.push_back(this);
}
Call::~Call(){

}

Call* Call::expire(uintmax_t expire_time){
    expire_timer.set_time(expire_time);
    expire_timer.start();
    return this;
}

Call* Call::expired(std::function<void()> callback){
    expired_callback = std::move(callback);
    return this;
}

Call* Call::extend_wait(uintmax_t extend_time){
    wait_timer.add_time(extend_time);
    return this;
}

Call* Call::reset_wait(){
    wait_timer.start();
    return this;
}

Call* Call::extend(uintmax_t extend_time){
    expire_timer.add_time(extend_time);
    return this;
}

Call* Call::await(std::function<bool()> code_block){
    this->code_chain.push_back(std::move(code_block));
    return this;
}
Call* Call::then(std::function<void()> code_block){
    this->code_chain.push_back([cb = std::move(code_block)](){
        cb();
        return true;
    });
    return this;
}

void Call::cancel(){
    this->cancelled = true;
}

void Call::cancel(std::function<void()> cancelCallback){
    cancel_callback = std::move(cancelCallback);
    this->cancel();
}

bool Call::exist(async::Call* call){
    if (call == nullptr) { return false; }
    for (size_t i = 0; i < Runtime::async_calls.size(); i++){
        if (Runtime::async_calls[i] == call){
            return true;
        }
    }
    return false;
}

Call* Call::wait(uint32_t time){
    // This pushes to chain function which uses timer built in this call
    // if timer finishes it passes true, going forward.
    code_chain.push_back([this, time](){
        switch (wait_timer.get_state()){
        case Timer::State::idle:
            wait_timer.set_time(time);
            wait_timer.start();
            return false;
        case Timer::State::finished:
            wait_timer.clear();
            return true;
        default:
            return false;
        }
        return false;
    });

    return this;
}
void Call::loop(){
    if(cancelled || chain_link == code_chain.size() || expire_timer.get_state() == Timer::State::finished){
        // If call got expired run callback
        if(cancelled) { cancel_callback(); Serial.println("Async CANCELLED"); }
        if(expire_timer.get_state() == Timer::State::finished){ expired_callback(); }
        // Remove call from runtime
        bool found = false;
        for (size_t i = 0; i < Runtime::async_calls.size(); i++){
            if (Runtime::async_calls[i] == this){
                Runtime::async_calls.erase(Runtime::async_calls.begin() + i);
                found = true;
                break; // Break loop to avoid index issues, then delete
            }
        }
        delete this;
        // WARNING: 'this' is now deleted. Return immediately.
        return;
    }else{
        // empty function always == true, it pushes forward,
        // bool functions evaluate until they get true,
        // if push forward == true, go to next function.
        if(push_forward == false){
            // Ensure bounds
            if(chain_link < code_chain.size()) {
                this->push_forward = this->code_chain[chain_link]();
            }
        }else{
            push_forward = false;
            chain_link++;
        }
    }
}
}
