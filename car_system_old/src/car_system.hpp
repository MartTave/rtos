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
 * @file car_system.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Car System header file
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#pragma once

// stl
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

// zpp_lib
#include "zpp_include/barrier.hpp"
#include "zpp_include/non_copyable.hpp"
#include "zpp_include/semaphore.hpp"
#include "zpp_include/thread.hpp"

#if CONFIG_PHASE_B
#include "deferrable_server.hpp"
#include "periodic_task_info.hpp"
#include "sporadic_task_generator.hpp"
#endif  // CONFIG_PHASE_B

namespace car_system {

class CarSystem {
   public:
    CarSystem();

    void start();
    void stop();

   private:
    void task_method(size_t taskIndex);

    static constexpr size_t kNumberOfPeriodicTasks = 4;
    static constexpr size_t kNumberOfTasks =
        kNumberOfPeriodicTasks + 1;  // +1 for DeferrableServer

    std::unique_ptr<zpp_lib::Thread> _threads[kNumberOfTasks];
    std::function<void()> _taskMethods[kNumberOfTasks];

#if CONFIG_PHASE_B
    zpp_lib::Barrier _barrier{kNumberOfTasks + 1};
    zpp_lib::Semaphore _startSemaphore{0, kNumberOfTasks};
#else
    zpp_lib::Barrier _barrier{kNumberOfPeriodicTasks};
    zpp_lib::Semaphore _startSemaphore{0, kNumberOfPeriodicTasks};
#endif

    std::atomic<bool> _stopFlag{false};

    PeriodicTaskInfo _periodicTaskInfos[kNumberOfPeriodicTasks] = {
        {std::chrono::milliseconds(10),
         std::chrono::milliseconds(50),
         std::chrono::milliseconds(50),
         zpp_lib::PreemptableThreadPriority::PriorityRealtime,
         "Engine"},
        {std::chrono::milliseconds(15),
         std::chrono::milliseconds(125),
         std::chrono::milliseconds(125),
         zpp_lib::PreemptableThreadPriority::PriorityHigh,
         "Display"},
        {std::chrono::milliseconds(10),
         std::chrono::milliseconds(200),
         std::chrono::milliseconds(200),
         zpp_lib::PreemptableThreadPriority::PriorityAboveNormal,
         "Tire"},
        {std::chrono::milliseconds(25),
         std::chrono::milliseconds(250),
         std::chrono::milliseconds(250),
         zpp_lib::PreemptableThreadPriority::PriorityNormal,
         "Rain"}};

#if CONFIG_PHASE_B
    AperiodicTaskInfo _aperiodicTaskInfos[2] = {
        {AperiodicTaskType::PresenceDetection, std::chrono::milliseconds(50)},
        {AperiodicTaskType::SearchPeripheralDevices,
         std::chrono::milliseconds(200)}};

    SporadicTaskGenerator _sporadicTaskGenerator;

    PeriodicTaskInfo _deferrableServerInfo = {
        std::chrono::milliseconds(250),
        std::chrono::milliseconds(1000),
        std::chrono::milliseconds(1000),
        zpp_lib::PreemptableThreadPriority::PriorityBelowNormal,
        "DeferrableServer"};

    DeferrableServer _deferrableServer;
#endif  // CONFIG_PHASE_B
};

}  // namespace car_system
