/**
 * @file Params.hpp
 * @author Łukasz Kaniak - nativeme (lukaszkaniak@gamil.com)
 * @brief 
 * Parametric constructor structure for method driven initialization.
 * @version 0.1
 * @date 2022-04-10
 * @copyright Copyright (c) 2022
 */

#ifndef _PARAMS_HPP_
#define _PARAMS_HPP_

#include "Arduino.h"

/**
 * @brief 
 * Parametric constructor structure for method driven initialization.
 * To use it create constructor in other class that takes "Params" object
 * as (rvalue) argument and executes it in its own constructor.
 * 
 * MyClass::MyClass(const Params &&builder){
 *     builder();
 * }
 */
struct Params{
    std::function<void(void)> builder;
    template<typename Lambda>
    Params(Lambda &&builder) : builder(builder){};
    void operator()(){this->builder();}
};

#endif