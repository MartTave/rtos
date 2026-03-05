/**
******************************************************************************
* @file        : main.cpp
* @brief       : Led blinking module
* @author      : Martin Tavernier <martin.tavernier@hevs.ch>
* @date        : 05.03.2026
******************************************************************************
* @copyright   : Copyright (c) 2026
*                HEVS
* @attention   : SPDX-License-Identifier: Apache-2.0
******************************************************************************
* @details
* Small code to make a LED blink
******************************************************************************
*/

// stl
#include <chrono>

// segger
// #include "SEGGER_SYSVIEW.h"

// zephyr
#include <zephyr/logging/log.h>

// zpp-lib
#include "zpp_include/digital_out.hpp"
#include "zpp_include/this_thread.hpp"
#include "zpp_include/thread.hpp"

LOG_MODULE_REGISTER(blinky, CONFIG_APP_LOG_LEVEL);

void blink() {
    zpp_lib::DigitalOut led(zpp_lib::DigitalOut::PinName::LED0);
    // using namespace std::chrono_literals;
    using std::chrono_literals::operator""ms;
    static std::chrono::milliseconds blinkInterval = 1000ms;
    while (true) {
        // SEGGER_SYSVIEW_PrintfHost("LED state changed");
        led = !led;
        LOG_DBG("Blink ?");
        zpp_lib::ThisThread::sleep_for(blinkInterval);
    }
}

int main(void) {
    LOG_DBG("Running on board %s", CONFIG_BOARD_TARGET);
    zpp_lib::Thread thread;
    auto res = thread.start(blink);
    if (!res) {
        LOG_ERR("Could not start thread");
        return -1;
    }
    res = thread.join();
    if (!res) {
        LOG_ERR("Could not terminate thread");
        return -1;
    }
    return 0;
}
