/**
 * @file      systick.h
 * @brief     ARM Cortex-M4 System Timer (SysTick) Driver.
 * @details   Provides blocking delays and system timekeeping (uptime) 
 * using the core internal timer.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/**
 * @defgroup Cortex_Core_Map ARM Cortex-M4 System Control Space
 * @{
 */
#define SCS_BASE            0xE000E000UL            /*!< System Control Space Base Address */
#define SYSTICK_BASE        (SCS_BASE + 0x0010UL)   /*!< SysTick Base Address (Offset +0x10) */
/** @} */


/**
 * @brief  SysTick Register Map (Standard ARMv7-M Architecture).
 */
typedef struct {
    volatile uint32_t CTRL;   /*!< Control and Status Register (Offset: 0x00) */
    volatile uint32_t LOAD;   /*!< Reload Value Register       (Offset: 0x04) */
    volatile uint32_t VAL;    /*!< Current Value Register      (Offset: 0x08) */
    volatile uint32_t CALIB;  /*!< Calibration Value Register  (Offset: 0x0C) */
} SysTick_TypeDef;

/* --- Pointer Definition --- */
#define SysTick ((SysTick_TypeDef *) SYSTICK_BASE)


/**
 * @defgroup SysTick_Bit_Defs SysTick Control Register Bits
 * @{
 */
#define SysTick_CTRL_ENABLE_Pos     0U
#define SysTick_CTRL_ENABLE_Msk     (1UL << SysTick_CTRL_ENABLE_Pos)    /*!< Counter Enable */

#define SysTick_CTRL_TICKINT_Pos    1U
#define SysTick_CTRL_TICKINT_Msk    (1UL << SysTick_CTRL_TICKINT_Pos)   /*!< Interrupt Enable */

#define SysTick_CTRL_CLKSOURCE_Pos  2U
#define SysTick_CTRL_CLKSOURCE_Msk  (1UL << SysTick_CTRL_CLKSOURCE_Pos) /*!< Clock Source (1 = Processor Clock) */

#define SysTick_CTRL_COUNTFLAG_Pos  16U
#define SysTick_CTRL_COUNTFLAG_Msk  (1UL << SysTick_CTRL_COUNTFLAG_Pos) /*!< Count Flag (Returns 1 if timer counted to 0) */
/** @} */


/* --- Function Prototypes --- */

/**
 * @brief  Initializes SysTick to trigger an interrupt every 1ms.
 * @param  ticks: Number of CPU cycles between interrupts.
 * (For 1ms at 180MHz, pass 180,000).
 */
void SysTick_Init(uint32_t ticks);

/**
 * @brief  SysTick Interrupt Handler.
 * @note   This function is called automatically by the CPU every 1ms.
 * It MUST match the name in the startup file (vector table).
 */
void SysTick_Handler(void);

/**
 * @brief  Returns the system uptime in milliseconds.
 * @return Time in ms since startup.
 */
uint32_t GetTick(void);

/**
 * @brief  Blocking delay.
 * @param  ms: Time to wait in milliseconds.
 * @note   Pauses execution until the time has passed.
 */
void Delay(uint32_t ms);

#endif // SYSTICK_H