/**
******************************************************************************
* @file        : main.cpp
* @brief       : Fetch data from a BME280 sensor
* @author      : Martin Tavernier <martin.tavernier@hevs.ch>
* @date        : 05.03.2026
******************************************************************************
* @copyright   : Copyright (c) 2026
*                HEVS
* @attention   : SPDX-License-Identifier: Apache-2.0
******************************************************************************
* @details
* Print the value fetched from the bme280 sensor to the console
******************************************************************************
*/


// stl
#include <chrono>

// zpp-lib
#include "zpp_include/digital_out.hpp"
#include "zpp_include/this_thread.hpp"
#include "zpp_include/thread.hpp"

// zephyr
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_bm280, CONFIG_APP_LOG_LEVEL);

static const struct device* bme280_device = DEVICE_DT_GET(DT_INST(0, bosch_bme280));

void read_sensor() {

    using std::chrono_literals::operator""ms;
    static std::chrono::milliseconds readInterval = 1000ms;

    if (!device_is_ready(bme280_device)) {
        LOG_ERR("Device %s not found", bme280_device->name);
        return;
    }

    struct sensor_value temperature_sv, humidity_sv, pressure_sv;

    while (true) {
        sensor_sample_fetch(bme280_device);

        sensor_channel_get(
            bme280_device, SENSOR_CHAN_AMBIENT_TEMP, &temperature_sv);
        sensor_channel_get(bme280_device, SENSOR_CHAN_HUMIDITY, &humidity_sv);
        sensor_channel_get(bme280_device, SENSOR_CHAN_PRESS, &pressure_sv);

        LOG_INF("T=%.2f [deg C] P=%.2f [kPa] H=%.1f [%%]",
                sensor_value_to_double(&temperature_sv),
                sensor_value_to_double(&pressure_sv),
                sensor_value_to_double(&humidity_sv));

        zpp_lib::ThisThread::sleep_for(readInterval);
    }
}

int main(void) {
    LOG_DBG("Running on board %s", CONFIG_BOARD_TARGET);

    zpp_lib::Thread thread(zpp_lib::PreemptableThreadPriority::PriorityNormal,
                           "Sensor");
    auto res = thread.start(read_sensor);
    if (!res) {
        return -1;
    }

    res = thread.join();
    if (!res) {
        LOG_ERR("Could not join thread: %d", static_cast<int>(res.error()));
        return -1;
    }

    return 0;
}
