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

namespace car_system {

struct PeriodicTaskInfo {
    std::chrono::milliseconds period;
    std::chrono::milliseconds deadline;
    std::chrono::milliseconds computationTime;
};

class CarSystem {
   public:
    CarSystem();

    void start();
    void stop();

   private:
    void task_method(size_t taskIndex);

    static constexpr size_t kNumberOfTasks = 3;

    std::unique_ptr<zpp_lib::Thread> _threads[kNumberOfTasks];
    std::function<void()> _taskMethods[kNumberOfTasks];

    zpp_lib::Barrier _barrier{kNumberOfTasks};
    zpp_lib::Semaphore _startSemaphore{0, kNumberOfTasks};

    std::atomic<bool> _stopFlag{false};

    PeriodicTaskInfo _taskInfos[kNumberOfTasks] = {
        {std::chrono::milliseconds(25),
         std::chrono::milliseconds(25),
         std::chrono::milliseconds(15)},
        {std::chrono::milliseconds(50),
         std::chrono::milliseconds(50),
         std::chrono::milliseconds(10)},
        {std::chrono::milliseconds(100),
         std::chrono::milliseconds(100),
         std::chrono::milliseconds(5)}};
};

}  // namespace car_system
