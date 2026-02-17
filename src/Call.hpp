#ifndef _CALL_HPP_
#define _CALL_HPP_

#include <functional>
#include <vector>

#include "Timer.hpp"
#include "ILoopable.hpp"

namespace async{
/**
 * @brief 
 * Asychronous call containing chains of lambdas
 * that are executed one by one in order.
 */
class Call :
public ILoopable
{
private:
    uint32_t chain_link = 0;
    bool push_forward   = false;
    std::vector<std::function<bool()>> code_chain;
    std::function<void()> expired_callback { [](){} };
    std::function<void()> cancel_callback { [](){} };
    async::Timer wait_timer = async::Timer(1000);
    async::Timer expire_timer = async::Timer(1000);
    bool cancelled { false };
public:

    struct Finished {
        operator bool() const {
            return true;
        }
    };

    Call();
    ~Call() override = default;
    
    /**
     * @brief 
     * Sets expire time for whole async call chain.
     * 'expired([](){})' method can be used to set optional callback on expiry.
     * @param expire_time Time after which whole async call chain get deleted.
     * @return Call* - This call to manipulate further. 
     */
    Call* expire(uintmax_t expire_time);
    /**
     * @brief 
     * Sets callback which will be called after expiry time ( set by expire(time) )
     * @param callback Callback which will be called after expiry time.
     * @return Call* This call to manipulate further. 
     */
    Call* expired(std::function<void()> callback);
    /**
     * @brief 
     * Allows to dynamically extend expiry time ( set by expire(time) ).
     * @param extend_time Additional time which will for expiry.
     * @return Call* This call to manipulate further.
     */
    Call* extend(uintmax_t extend_time);
    /**
     * @brief 
     * Allows to dynamically extend currently processed wait time.
     * @param extend_time Additional time to current wait.
     * @return Call* This call to manipulate further.
     */
    Call* extend_wait(uintmax_t extend_time);
    /**
     * @brief 
     * Allows to dynamically reset currently processed wait time.
     * @return Call* This call to manipulate further.
     */
    Call* reset_wait();
    /**
     * @brief 
     * Sets additional code part which will be executed in async call chain,
     * in async loop untill it will return "true". return "async::Call::Finished"
     * can be used for syntactic suggaring. 
     * @param code_block 
     * @return Call* This call to manipulate further.
     */
    Call* await(std::function<bool()> code_block);
    /**
     * @brief 
     * Sets additional code part which will be executed in async call chain. 
     * @param code_block additional code, passed by lambda or void function.
     * @return Call* This call to manipulate further.
     */
    Call* then(std::function<void()> code_block);

    void cancel();
    void cancel(std::function<void()> code_block);

    static bool exist(async::Call* call);

    /**
     * @brief 
     * Async call chain will wait set ammount of time before executing next function.
     * @param time Ammount of time to wait.
     * @return Call* This call to manipulate further.
     */
    Call* wait(uint32_t time);

    void loop() override;
};

inline Call* call(){
    return new Call();
};

}
#endif