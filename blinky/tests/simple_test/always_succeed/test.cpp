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

#include <zephyr/ztest.h>

ZTEST(always_succeed, test_equality) {
    // this is the always succeed test
    zassert_true(4 == 2 * 2);
}

ZTEST_SUITE(always_succeed, nullptr, nullptr, nullptr, nullptr, nullptr);
