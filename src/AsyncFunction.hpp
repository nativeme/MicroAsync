/**
 * @file AsyncFunction.hpp
 * @author Łukasz Kaniak (lukaszkaniak@gmail.com)
 * @brief 
 * Describes AsyncFunction and AsyncFunction's Future class.
 * Used as one of features in MicroAsync.
 * @version 0.1
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _ASYNCFUNCTION_HPP_
#define _ASYNCFUNCTION_HPP_

#include <cstdint>

#include "ILoopable.hpp"
#include "IAsyncFunction.hpp"
#include "Runtime.hpp"

#define async_return(function, condition, value) switch(condition){ case true: function.return_buffer.push_back(value); return true; default: return false;}

namespace async{

template<typename T>
class function{};

template<typename T, typename... ArgTypes>
class function<T(ArgTypes...)> :
    public ILoopable,
    public IAsyncFunction
{
private:

public:
    class Future : 
    public ILoopable
    {
    private:
        bool fulfilled = false;
        T val;

    public:
        uint8_t state = 0;
        enum State{
            triggered = 0,
            processing = 1,
            completed = 2,
            to_destroy = 3
        };

        template<typename Future, typename... Pack>
        friend inline bool is_fulfilled(const Future& future, const Pack& ...rest);

        async::function<T(ArgTypes...)>* async_function;
        std::function<void(void)> wrapped_function_nonarged;

        int fid = 0;

        Future(){
        }

        Future(async::function<T(ArgTypes...)>* async_function, ArgTypes... args)
        :   state(Future::State::processing),
            async_function(async_function),
            wrapped_function_nonarged(
            [this, args...](){
                this->fulfilled = this->async_function->wrapped_function(args...);
            })
        {}

        ~Future(){
        }

        T value() {
            this->state = Future::State::to_destroy;
            return val;
        };

        Future* await(){
            while(state != Future::State::completed){
                async::Runtime::loop();
                yield();
            }
            return this;
        }

        void loop() override {
            switch (state)
            {
            case Future::State::processing:
                wrapped_function_nonarged();
                if(fulfilled){
                    // this->val = async_function->return_buffer[async_function->return_buffer.size() - 1];
                    // async_function->return_buffer.pop_back();
                    this->val = async_function->return_buffer[0];
                    async_function->return_buffer.clear();
                    state = Future::State::completed;
                }
                break;
            case Future::State::completed:
                break;
            case Future::State::to_destroy:
                {
                    auto found = std::find(async_function->function_calls.begin(), async_function->function_calls.end(), this);
                    if(found != async_function->function_calls.end()){
                        async_function->function_calls.erase(found);
                        delete this;
                    }
                }
                break;
            default:
                break;
            }
        }
    };

    std::function<bool(ArgTypes...)> wrapped_function;
    std::vector<Future*> function_calls;
    std::vector<T> return_buffer;

    function(){
        id = new_id();
    }
    
    template<typename Lambda>
    function(Lambda&& wrapped_function)
    :   wrapped_function(std::move(wrapped_function))
    {   
        id = new_id();
        async::Runtime::async_functions.push_back(this);
    }

    ~function(){};
    
    Future* operator()(ArgTypes... args){
        auto new_future_call = new Future(this, args...);
        this->function_calls.push_back(new_future_call);
        return new_future_call;
    }

    void loop() override {
        for (size_t i = 0; i < function_calls.size(); ) {
            auto call = function_calls[i];
            call->loop();
            if (i < function_calls.size() && function_calls[i] == call) {
                i++;
            }
        }
    }
};

    /**
     * @brief 
     * Checks if all of passed async functions features are fulfilled.
     * @param future(s) One or more async futures.
     * @return <bool> true if all given functions features are fulfilled.
     */    
    template<typename Future, typename... Pack>
    inline bool is_fulfilled(const Future& future, const Pack& ...rest){
        if constexpr (sizeof...(Pack) > 0){
            if(!future->fulfilled)
                return false;
            is_fulfilled(rest...);
        }else{
            if(!future->fulfilled)
                return false;
            return true;
        }
        return true;
    }

    template<typename Future, typename ...FuturePack>
    inline bool gather(const Future& future, const FuturePack& ...futures){
        if constexpr (sizeof...(FuturePack) > 0) {
            future->await();
            gather(futures...);
            return false;
        } else {
            future->await();
            return true;
        }
    }
};
#endif