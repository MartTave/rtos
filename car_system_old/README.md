# Car System RTOS Project

## Project Structure

```
car_system/
├── boards/                    # Board-specific configurations
├── src/
│   ├── main.cpp              # Entry point, creates CarSystem
│   ├── car_system.cpp         # Periodic & aperiodic task implementation
│   ├── car_system.hpp        # CarSystem class, task configs
│   ├── sporadic_task_generator.cpp  # Aperiodic task generator
│   └── sporadic_task_generator.hpp   # SporadicTaskGenerator class
├── CMakeLists.txt            # Build configuration
├── Kconfig                   # Kconfig options (includes CONFIG_PHASE_B)
├── prj.conf                  # Project config (CONFIG_PHASE_B, thread pool, etc.)
└── tasks.md                  # Task specifications
```

## Tasks

### Periodic (4 tasks)
| Task | WCET | Period | Priority |
|------|------|--------|----------|
| Engine | 10ms | 50ms | Realtime (7) |
| Display | 15ms | 125ms | High (8) |
| Tire | 10ms | 200ms | AboveNormal (9) |
| Rain | 25ms | 250ms | Normal (10) |

### Aperiodic (2 tasks) - CONFIG_PHASE_B
| Task | WCET | Priority |
|------|------|----------|
| Presence Detection (PD) | 50ms | Low (12) |
| Search Peripheral Devices (SPD) | 200ms | Low (12) |

## Configuration

### Enable Phase B (Aperiodic Tasks)
In `prj.conf`, set:
```
CONFIG_PHASE_B=y
```

### Key Configuration Options (prj.conf)
- `CONFIG_PHASE_B=y` - Enable aperiodic tasks
- `CONFIG_ZPP_THREAD_POOL_SIZE=8` - Thread pool size (6+ tasks)
- `CONFIG_MAIN_STACK_SIZE=4096` - Main thread stack size
- `CONFIG_SEGGER_SYSTEMVIEW=y` - Enable SystemView tracing
- `CONFIG_SEGGER_RTT_BUFFER_SIZE_UP=4096` - RTT buffer size (reduces overflow)

## Implementation Details

- **Periodic tasks**: Use `_periodicTaskInfos[]` array with period and WCET
- **Aperiodic tasks**: Use `SporadicTaskGenerator` with message queue
- **Thread priorities**: Engine (Realtime) > Display (High) > Tire (AboveNormal) > Rain (Normal) > PD/SPD (Low)
- **Synchronization**: Barrier for start, semaphore for task release
- **Aperiodic scheduling**: Tasks generated at 60ms, 300ms, 630ms, 900ms per major cycle (1000ms)

## Key Code Locations

- `car_system.hpp:67-79` - Task constants and barrier/semaphore (conditional on CONFIG_PHASE_B)
- `car_system.hpp:81-93` - Periodic task info array
- `car_system.hpp:95-102` - Aperiodic task info + generator
- `car_system.cpp:49-75` - Thread creation (6 threads when CONFIG_PHASE_B)
- `sporadic_task_generator.cpp:66-72` - Aperiodic task generation schedule

## Building

```bash
# Clean build recommended
rm -rf build/
west build -b nrf5340dk/nrf5340/cpuapp car_system
```