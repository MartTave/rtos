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
 * @file deferrable_server.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief DeferrableServer implementation
 *
 * @date 2025-07-01
 * @version 1.0.0
 ***************************************************************************/

#if CONFIG_PHASE_B

#include "deferrable_server.hpp"

#include "zpp_include/this_thread.hpp"
#include "zpp_include/time.hpp"

#ifdef CONFIG_SEGGER_SYSTEMVIEW
#include "SEGGER_SYSVIEW.h"
#endif  // CONFIG_SEGGER_SYSTEMVIEW

namespace car_system {

void DeferrableServer::start(zpp_lib::Barrier& barrier,
                             const PeriodicTaskInfo& taskInfo,
                             SporadicTaskGenerator& taskGenerator) {
    auto startTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(barrier.wait());

    using std::literals::chrono_literals::operator""ms;
    using std::literals::chrono_literals::operator""s;

    std::chrono::milliseconds serverPeriod   = taskInfo._period;
    std::chrono::milliseconds serverCapacity = taskInfo._computationTime;
    static constexpr auto timeOut            = 500ms;

    auto nextPeriodStart = startTime;

    while (!_stopFlag.load()) {
        std::chrono::milliseconds remainingBudget = serverCapacity;

#ifdef CONFIG_SEGGER_SYSTEMVIEW
        SEGGER_SYSVIEW_MarkStart(4);
#endif  // CONFIG_SEGGER_SYSTEMVIEW

        while (remainingBudget > std::chrono::milliseconds(0) &&
               !_stopFlag.load()) {
            AperiodicTaskInfo taskInfo;
            auto boolRes = taskGenerator.get_sporadic_task(taskInfo, timeOut);

            if (boolRes &&
                taskInfo.computationTime > std::chrono::milliseconds(0)) {
                if (taskInfo.computationTime <= remainingBudget) {
                    zpp_lib::ThisThread::busy_wait(taskInfo.computationTime);
                    remainingBudget -= taskInfo.computationTime;
                } else {
                    taskGenerator.resubmit_sporadic_task(taskInfo);
                }
            }
        }

#ifdef CONFIG_SEGGER_SYSTEMVIEW
        SEGGER_SYSVIEW_MarkStop(4);
#endif  // CONFIG_SEGGER_SYSTEMVIEW

        nextPeriodStart += serverPeriod;
        zpp_lib::ThisThread::sleep_until(nextPeriodStart);
    }
}

void DeferrableServer::stop() { _stopFlag = true; }

}  // namespace car_system

#endif  // CONFIG_PHASE_B