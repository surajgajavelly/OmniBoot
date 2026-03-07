/**
 * @file      gpio.h
 * @brief     General Purpose I/O (GPIO) Driver for STM32F446RE.
 * @details   Provides low-level access to GPIO peripherals. 
 * Supports Initialization, Atomic Write, Toggle, and Read operations.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
 * @defgroup GPIO_Memory_Map GPIO Base Addresses
 * @brief    Base addresses for GPIO Ports on the AHB1 Bus.
 * @{
 */
#define GPIOA_BASE      0x40020000UL
#define GPIOC_BASE      0x40020800UL
/** @} */

/**
 * @brief  GPIO Register Map (RM0390 Reference Manual).
 */
typedef struct {
    volatile uint32_t MODER;    /*!< Mode register (Input/Output/Alt/Analog) Offset: 0x00 */
    volatile uint32_t OTYPER;   /*!< Output type register (Push-Pull/Open-Drain) Offset: 0x04 */
    volatile uint32_t OSPEEDR;  /*!< Output speed register Offset: 0x08 */
    volatile uint32_t PUPDR;    /*!< Pull-up/pull-down register Offset: 0x0C */
    volatile uint32_t IDR;      /*!< Input data register (Read-Only) Offset: 0x10 */
    volatile uint32_t ODR;      /*!< Output data register (Read/Write) Offset: 0x14 */
    volatile uint32_t BSRR;     /*!< Bit set/reset register (Atomic Write) Offset: 0x18 */
    volatile uint32_t LCKR;     /*!< Configuration lock register Offset: 0x1C */
    volatile uint32_t AFR[2];   /*!< Alternate function registers (Low & High) Offset: 0x20-0x24 */
} GPIO_TypeDef;

/* --- Pointer Definitions --- */
#define GPIOA   ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOC   ((GPIO_TypeDef *) GPIOC_BASE)

/**
 * @defgroup GPIO_Pins GPIO Pin Definitions
 * @brief    Bitmasks for specific pins (1U << PinNumber).
 * @{
 */
#define GPIO_PIN_0      (1U << 0)
#define GPIO_PIN_1      (1U << 1)
#define GPIO_PIN_2      (1U << 2)
#define GPIO_PIN_3      (1U << 3)
#define GPIO_PIN_4      (1U << 4)
#define GPIO_PIN_5      (1U << 5)
#define GPIO_PIN_6      (1U << 6)
#define GPIO_PIN_7      (1U << 7)
#define GPIO_PIN_8      (1U << 8)
#define GPIO_PIN_9      (1U << 9)
#define GPIO_PIN_10     (1U << 10)
#define GPIO_PIN_11     (1U << 11)
#define GPIO_PIN_12     (1U << 12)
#define GPIO_PIN_13     (1U << 13)
#define GPIO_PIN_14     (1U << 14)
#define GPIO_PIN_15     (1U << 15)
/** @} */

/**
 * @defgroup GPIO_Logic_Levels Logic State Definitions
 * @{
 */
#define GPIO_PIN_SET    1
#define GPIO_PIN_RESET  0
/** @} */

/* --- Function Prototypes --- */

/**
 * @brief  Initializes the GPIO peripherals.
 * @note   Currently hardcoded to configure PA5 (LED) and PC13 (Button).
 * Does NOT accept arguments yet to keep the driver simple for now.
 */
void GPIO_Init(void); 

/**
 * @brief  Writes a state (High/Low) to a specific pin.
 * @param  GPIOx: Pointer to GPIO Port (e.g., GPIOA)
 * @param  pin:   Pin mask (e.g., GPIO_PIN_5)
 * @param  state: 1 (High) or 0 (Low)
 * @note   Uses BSRR for atomic operations, preventing Read-Modify-Write errors.
 */
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t state);

/**
 * @brief  Toggles the state of a specific pin.
 * @param  GPIOx: Pointer to GPIO Port
 * @param  pin:   Pin mask
 */
void GPIO_Toggle(GPIO_TypeDef *GPIOx, uint16_t pin);

/**
 * @brief  Reads the logic level of a specific pin.
 * @param  GPIOx: Pointer to GPIO Port
 * @param  pin:   Pin mask
 * @return 1 if High, 0 if Low.
 */
uint8_t GPIO_Read(GPIO_TypeDef *GPIOx, uint16_t pin);

#endif // GPIO_H