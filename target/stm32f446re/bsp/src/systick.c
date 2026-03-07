/**
 * @file      systick.c
 * @brief     Implementation of the ARM Cortex-M4 SysTick Driver.
 * @details   Provides blocking delay functionality and system uptime tracking
 * using the core system timer.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include "systick.h"

/**
 * @brief Global tick counter.
 * @note  Marked as `volatile` because it is modified in an Interrupt Service Routine (ISR)
 * and read in the main thread. Without `volatile`, the compiler might cache
 * this value in a register, causing infinite loops in `Delay()`.
 */
static volatile uint32_t ms_ticks = 0;

/**
 * @brief  Initializes the SysTick timer to trigger an interrupt every 1ms.
 * @param  ticks: Number of CPU cycles between interrupts.
 * (e.g., for 180 MHz CPU and 1ms tick: ticks = 180,000).
 */
void SysTick_Init(uint32_t ticks) {
    // 1. Disable SysTick during configuration to prevent accidental interrupts
    SysTick->CTRL = 0;

    // 2. Set the Reload Value
    // The counter counts from N down to 0, so the total period is N+1 cycles.
    // We subtract 1 to get the exact requested duration.
    SysTick->LOAD = ticks - 1;

    // 3. Clear Current Value Register
    // Writing any value to the VAL register clears it to 0.
    SysTick->VAL = 0;

    // 4. Enable SysTick
    // - CLKSOURCE_Msk (Bit 2): Set to 1 to use Processor Clock (AHB)
    // - TICKINT_Msk   (Bit 1): Set to 1 to enable SysTick Interrupt
    // - ENABLE_Msk    (Bit 0): Set to 1 to start the counter
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
                    SysTick_CTRL_TICKINT_Msk   | 
                    SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief  SysTick Interrupt Handler.
 * @note   This function name is **special**. It overrides the weak symbol defined
 * in the startup file (`startup_stm32.c`). The Hardware Vector Table
 * points to this function address.
 * Called automatically by the CPU when the SysTick counter reaches 0.
 */
void SysTick_Handler(void) {
    ms_ticks++;
}

/**
 * @brief  Returns the number of milliseconds since system startup.
 * @return Current uptime in milliseconds.
 */
uint32_t GetTick(void) {
    return ms_ticks;
}

/**
 * @brief  Blocking delay in milliseconds.
 * @param  ms: Duration to wait.
 * @note   This function handles `uint32_t` overflow gracefully due to standard
 * unsigned arithmetic rules (Two's Complement).
 */
void Delay(uint32_t ms) {
    uint32_t start_time = GetTick();
    
    // Busy-wait loop
    // Logic: (Current - Start) will be correct even if Current wraps around 0.
    while ((GetTick() - start_time) < ms) {
        // Do nothing (Blocking)
    }
}