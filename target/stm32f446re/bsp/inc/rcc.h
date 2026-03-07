/**
 * @file      rcc.h
 * @brief     Reset and Clock Control (RCC) Driver for STM32F446RE.
 * @details   This header defines the register map and configuration macros for the
 * RCC and PWR peripherals. It is responsible for setting up the 
 * System Clock (SYSCLK) to 180 MHz using the PLL.
 * * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>

/**
 * @defgroup RCC_Memory_Map RCC Memory Base Addresses
 * @brief    Base addresses for RCC, PWR, and Flash Interface.
 * @{
 */
#define PERIPH_BASE         0x40000000U               /*!< Peripheral Base Address */
#define AHB1PERIPH_BASE     (PERIPH_BASE + 0x00020000U) /*!< AHB1 Bus Base Address */

#define RCC_BASE            (AHB1PERIPH_BASE + 0x3800U) /*!< RCC Register Base Address */
#define FLASH_R_BASE        (AHB1PERIPH_BASE + 0x3C00U) /*!< Flash Interface Register Base */
#define PWR_BASE            (PERIPH_BASE + 0x7000U)     /*!< Power Controller Base Address (APB1) */
/** @} */


/**
 * @brief  RCC (Reset and Clock Control) Register Map.
 * @note   Structure alignment mimics the hardware register layout (RM0390).
 */
typedef struct {
    volatile uint32_t CR;        /*!< Clock Control Register (Offset: 0x00) */
    volatile uint32_t PLLCFGR;   /*!< PLL Configuration Register (Offset: 0x04) */
    volatile uint32_t CFGR;      /*!< Clock Configuration Register (Offset: 0x08) */
    volatile uint32_t CIR;       /*!< Clock Interrupt Register (Offset: 0x0C) */
    volatile uint32_t AHB1RSTR;  /*!< AHB1 Peripheral Reset Register (Offset: 0x10) */
    volatile uint32_t AHB2RSTR;  /*!< AHB2 Peripheral Reset Register (Offset: 0x14) */
    volatile uint32_t AHB3RSTR;  /*!< AHB3 Peripheral Reset Register (Offset: 0x18) */
    volatile uint32_t Reserved0; /*!< Reserved (0x1C) */
    volatile uint32_t APB1RSTR;  /*!< APB1 Peripheral Reset Register (Offset: 0x20) */
    volatile uint32_t APB2RSTR;  /*!< APB2 Peripheral Reset Register (Offset: 0x24) */
    volatile uint32_t Reserved1; /*!< Reserved (0x28) */
    volatile uint32_t Reserved2; /*!< Reserved (0x2C) */
    volatile uint32_t AHB1ENR;   /*!< AHB1 Peripheral Clock Enable Register (Offset: 0x30) */
    volatile uint32_t AHB2ENR;   /*!< AHB2 Peripheral Clock Enable Register (Offset: 0x34) */
    volatile uint32_t AHB3ENR;   /*!< AHB3 Peripheral Clock Enable Register (Offset: 0x38) */
    volatile uint32_t Reserved3; /*!< Reserved (0x3C) */
    volatile uint32_t APB1ENR;   /*!< APB1 Peripheral Clock Enable Register (Offset: 0x40) */
    volatile uint32_t APB2ENR;   /*!< APB2 Peripheral Clock Enable Register (Offset: 0x44) */
} RCC_TypeDef;


/**
 * @brief  PWR (Power Control) Register Map.
 * @note   Used to configure voltage scaling for high-speed operation.
 */
typedef struct {
    volatile uint32_t CR;        /*!< Power Control Register (Offset: 0x00) */
    volatile uint32_t CSR;       /*!< Power Control/Status Register (Offset: 0x04) */
} PWR_TypeDef;


/* --- Hardware Pointers --- */
#define RCC   ((RCC_TypeDef *) RCC_BASE)  /*!< Pointer to RCC Struct */
#define PWR   ((PWR_TypeDef *) PWR_BASE)  /*!< Pointer to PWR Struct */

/* Note: FLASH pointer is defined in flash.h to avoid recursive includes */


/**
 * @defgroup RCC_Bit_Definitions RCC Register Bit Definitions
 * @{
 */

/* RCC_CR (Clock Control Register) */
#define RCC_CR_HSEON            (1U << 16) /*!< Enable High Speed External (HSE) Oscillator */
#define RCC_CR_HSERDY           (1U << 17) /*!< HSE Ready Flag */
#define RCC_CR_PLLON            (1U << 24) /*!< Enable Main PLL */
#define RCC_CR_PLLRDY           (1U << 25) /*!< PLL Ready Flag */

/* RCC_PLLCFGR (PLL Configuration Register) */
#define RCC_PLLCFGR_PLLSRC_HSE  (1U << 22) /*!< PLL Source: HSE Oscillator */

/* RCC_CFGR (Clock Configuration Register) */
#define RCC_CFGR_SW_PLL         (2U << 0)  /*!< System Clock Switch: Select PLL */
#define RCC_CFGR_SWS_PLL        (2U << 2)  /*!< System Clock Switch Status: PLL Used */

/** @} */


/**
 * @defgroup Flash_ACR_Bits Flash Access Control Bits
 * @brief    Wait states and cache settings needed for high-speed operation.
 * @{
 */
#define FLASH_ACR_LATENCY_5WS   (5U << 0)  /*!< 5 Wait States (Required for 180MHz @ 3.3V) */
#define FLASH_ACR_PRFTEN        (1U << 8)  /*!< Prefetch Enable */
#define FLASH_ACR_ICEN          (1U << 9)  /*!< Instruction Cache Enable */
#define FLASH_ACR_DCEN          (1U << 10) /*!< Data Cache Enable */
/** @} */


/**
 * @defgroup PWR_CR_Bits Power Control Register Bits
 * @{
 */
#define PWR_CR_VOS_SCALE1       (3U << 14) /*!< Voltage Scaling Scale 1 (Required for >144MHz) */
/** @} */


/* --- Function Prototypes --- */

/**
 * @brief  Initializes the System Clock to 180 MHz.
 * @note   Sequence:
 * 1. Enable HSE (8 MHz).
 * 2. Configure Voltage Scaling (Scale 1).
 * 3. Configure Flash Wait States (5WS) + Cache.
 * 4. Configure PLL (HSE -> PLL -> 180MHz).
 * 5. Select PLL as System Clock.
 */
void RCC_Init(void);

#endif // RCC_H