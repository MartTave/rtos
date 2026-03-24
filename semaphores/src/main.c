/**
******************************************************************************
* @file        : main.c
* @brief       : Semaphore testing
* @author      : Martin Tavernier <martin.tavernier@hevs.ch>
* @date        : 05.03.2026
******************************************************************************
* @copyright   : Copyright (c) 2026
*                HEVS
* @attention   : SPDX-License-Identifier: Apache-2.0
******************************************************************************
* @details
* Small code to test the user/kernel separation of ZephyrOS
******************************************************************************
*/


#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app);

// create kernel objects as global variables
// other thread related data
static struct k_thread other_thread;
static const size_t OTHER_THREAD_STACKSIZE = 2048;
K_THREAD_STACK_DEFINE(other_thread_stack, OTHER_THREAD_STACKSIZE);
// semaphores used by both threads
K_SEM_DEFINE(SEM1, 1, 1)
K_SEM_DEFINE(SEM2, 0, 1)

// constants used in both threads
static const uint32_t DELAY_MS = 1000;
static const uint8_t NUM_ITERATIONS = 5;

void other_thread_entry() {
    // synchronize with the main thread
    for (uint8_t i = 0; i < NUM_ITERATIONS; i++) {
        z_impl_k_sem_take(&SEM1, K_FOREVER);
        printk("Iteration %d: SEM 1 taken by other thread\n", i);
        k_msleep(DELAY_MS);
        k_sem_give(&SEM2);
    }
}

int main(void) {
    // create a thread with the same priority as the current thread
    int prio = k_thread_priority_get(k_current_get());
    uint32_t options = K_INHERIT_PERMS;
#if CONFIG_USERSPACE == 1
    options |= K_USER;
#endif
    k_timeout_t delay = K_FOREVER;
    k_tid_t other_thread_tid = k_thread_create(
        &other_thread, other_thread_stack, OTHER_THREAD_STACKSIZE,
        other_thread_entry, nullptr, nullptr, nullptr, prio, options, delay);

#if CONFIG_USERSPACE == 1
    k_thread_access_grant(&other_thread, &SEM1, &SEM2);
#endif

    // give a name to the new thread
    k_thread_name_set(other_thread_tid, "Other Thread");

    // start the new thread
    k_thread_start(other_thread_tid);

    // synchronize with the other thread
    for (uint8_t i = 0; i < NUM_ITERATIONS; i++) {
        k_sem_take(&SEM2, K_FOREVER);
        printk("Iteration %d: SEM 2 taken by main thread\n", i);
        k_msleep(DELAY_MS);
        k_sem_give(&SEM1);
    }

    // wait for the other thread to finish
    k_thread_join(other_thread_tid, K_FOREVER);

    printk("Done\n");

    return 0;
}

/*
 * AP field in RBAR [2:1] — from arm_mpu_v8.h / mpu_armv8.h
 *
 *  AP bits: RO | NP
 *   RO=0, NP=0 → P:RW / U:--   (0b00)
 *   RO=0, NP=1 → P:RW / U:RW   (0b01)  NP bit set = non-privileged allowed
 *   RO=1, NP=0 → P:RO / U:--   (0b10)
 *   RO=1, NP=1 → P:RO / U:RO   (0b11)
 */
static const char* ap_to_str(uint32_t ap) {
    switch (ap) {
        case 0b00:
            return "P:RW / U:-- ";
        case 0b01:
            return "P:RW / U:RW ";
        case 0b10:
            return "P:RO / U:-- ";
        case 0b11:
            return "P:RO / U:RO ";
        default:
            return "P:?? / U:?? ";
    }
}

void dump_mpu_regions(void) {
    uint32_t num_regions = (MPU->TYPE >> 8) & 0xFF;

    printk("MPU type: %d regions\n", num_regions);

    for (uint32_t i = 0; i < num_regions; i++) {
        MPU->RNR = i;
        uint32_t rbar = MPU->RBAR;
        uint32_t rlar = MPU->RLAR;

        if (!(rlar & MPU_RLAR_EN_Msk)) {
            printk("Region %d: disabled\n", i);
            continue;
        }

        uint32_t base = rbar & MPU_RBAR_BASE_Msk;
        uint32_t limit =
            (rlar & MPU_RLAR_LIMIT_Msk) | 0x1F; /* 32-byte granule */
        uint32_t ap = (rbar & MPU_RBAR_AP_Msk) >> MPU_RBAR_AP_Pos;
        uint32_t xn = (rbar & MPU_RBAR_XN_Msk) >> MPU_RBAR_XN_Pos;

        printk("Region %d: 0x%08x-0x%08x  access=%s  exec=%s\n", i, base, limit,
               ap_to_str(ap), xn ? "XN (never)" : "allowed   ");
    }
}

// Fatal error handler
void k_sys_fatal_error_handler(unsigned int reason,
                               const struct arch_esf* esf) {
    dump_mpu_regions();
}
