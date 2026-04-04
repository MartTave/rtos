/**
 ******************************************************************************
 * @file        : main.cpp
 * @brief       : Car System with Monotonic Rate Scheduling
 * @author      : Martin Tavernier <martin.tavernier@hevs.ch>
 * @date        : 05.03.2026
 ******************************************************************************
 * @copyright   : Copyright (c) 2026
 *                HEVS
 * @attention   : SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @details
 * Multi-tasking program using Monotonic Rate Scheduling
 ******************************************************************************
 */

// zephyr
#include <zephyr/kernel.h>

// car_system
#include "car_system.hpp"

int main(void) {
    car_system::CarSystem carSystem;
    carSystem.start();

    k_sleep(K_FOREVER);

    return 0;
}
