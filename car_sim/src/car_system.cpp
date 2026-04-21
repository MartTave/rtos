// Copyright 2025 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file car_system.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Car System implementation file
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

// zpp_lib
#include "zpp_include/this_thread.hpp"
#include "zpp_include/time.hpp"

// car_system
#include "car_system.hpp"

#ifdef CONFIG_SEGGER_SYSTEMVIEW
#include "SEGGER_SYSVIEW.h"
#endif  // CONFIG_SEGGER_SYSTEMVIEW

namespace car_system {

namespace {

constexpr const char* kEngineName  = "Engine";
constexpr const char* kDisplayName = "Display";
constexpr const char* kTireName    = "Tire";
constexpr const char* kRainName    = "Rain";

}  // namespace

CarSystem::CarSystem() {
    _taskMethods[0] = [this]() { task_method(0); };
    _taskMethods[1] = [this]() { task_method(1); };
    _taskMethods[2] = [this]() { task_method(2); };
    _taskMethods[3] = [this]() { task_method(3); };

    _threads[0] = std::make_unique<zpp_lib::Thread>(
        zpp_lib::PreemptableThreadPriority::PriorityRealtime, kEngineName);
    _threads[1] = std::make_unique<zpp_lib::Thread>(
        zpp_lib::PreemptableThreadPriority::PriorityHigh, kDisplayName);
    _threads[2] = std::make_unique<zpp_lib::Thread>(
        zpp_lib::PreemptableThreadPriority::PriorityAboveNormal, kTireName);
    _threads[3] = std::make_unique<zpp_lib::Thread>(
        zpp_lib::PreemptableThreadPriority::PriorityNormal, kRainName);
}

void CarSystem::start() {
    using std::literals::chrono_literals::operator""s;
    zpp_lib::ThisThread::sleep_for(9s);
    for (size_t i = 0; i < kNumberOfTasks; i++) {
        auto res = _threads[i]->start(_taskMethods[i]);
        if (!res) {
            return;
        }
    }

    for (size_t i = 0; i < kNumberOfTasks; i++) {
        auto result = _startSemaphore.release();
    }
}

void CarSystem::stop() { _stopFlag.store(true); }

void CarSystem::task_method(size_t taskIndex) {
    auto result = _startSemaphore.acquire();

    _barrier.wait();

    auto nextPeriodStart = zpp_lib::Time::get_uptime();

    while (!_stopFlag.load()) {
#ifdef CONFIG_SEGGER_SYSTEMVIEW
        SEGGER_SYSVIEW_MarkStart(taskIndex);
#endif  // CONFIG_SEGGER_SYSTEMVIEW

        zpp_lib::ThisThread::busy_wait(
            _periodicTaskInfos[taskIndex]._computationTime);

#ifdef CONFIG_SEGGER_SYSTEMVIEW
        SEGGER_SYSVIEW_MarkStop(taskIndex);
#endif  // CONFIG_SEGGER_SYSTEMVIEW

        nextPeriodStart += _periodicTaskInfos[taskIndex]._period;
        zpp_lib::ThisThread::sleep_until(nextPeriodStart);
    }
}

}  // namespace car_system
