#ifndef _IASYNCFUNCTION_HPP_
#define _IASYNCFUNCTION_HPP_

#include <cstdint>

namespace async{

class IAsyncFunction
{
private:
    
public:
    int id = 0;
    uint8_t state = 0;

    IAsyncFunction() = default;
    virtual ~IAsyncFunction() = default;
    
    virtual int new_id(){
        static int id_counter = 0;
        return id_counter++;
    }

    virtual void loop() = 0;
};
}
#endif