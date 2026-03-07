/**
 * @file      crc.h
 * @brief     Hardware CRC32 Driver for STM32F446RE.
 * @details   Provides access to the built-in CRC calculation unit.
 * The STM32 uses the standard Ethernet CRC-32 polynomial: 0x04C11DB7.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#ifndef CRC_H
#define CRC_H

#include <stdint.h>
#include "rcc.h"

/**
 * @defgroup CRC_Memory_Map CRC Base Address
 * @brief    CRC is located on the AHB1 Bus.
 * @{
 */
#define CRC_BASE      (AHB1PERIPH_BASE + 0x3000U)
/** @} */

/**
 * @brief CRC Register Map (RM0390).
 */
typedef struct {
    volatile uint32_t DR;   /*!< Data Register (Offset: 0x00). Write data here, Read result here. */
    volatile uint32_t IDR;  /*!< Independent Data Register (Offset: 0x04). General purpose 8-bit storage. */
    volatile uint32_t CR;   /*!< Control Register (Offset: 0x08). Used to reset the calculator. */
} CRC_TypeDef;

/* --- Pointer Definition --- */
#define CRC           ((CRC_TypeDef *) CRC_BASE)

/**
 * @defgroup CRC_Bits Control & Clock Bits
 * @{
 */
#define RCC_AHB1ENR_CRCEN   (1U << 12) /*!< RCC: Enable CRC Clock */
#define CRC_CR_RESET        (1U << 0)  /*!< CRC_CR: Reset Calculation Unit */
/** @} */


/* --- Function Prototypes --- */

/**
 * @brief  Initializes the CRC Engine.
 * @note   Enables the CRC clock on the AHB1 bus.
 */
void CRC_Init(void);

/**
 * @brief  Calculates the CRC32 checksum for a buffer of 32-bit words.
 * @param  data: Pointer to the data buffer (must be 32-bit aligned).
 * @param  len:  Number of 32-bit words to process.
 * @return The calculated 32-bit CRC value.
 * @note   This function resets the CRC unit before starting.
 */
uint32_t CRC_Calculate(uint32_t *data, uint32_t len);

#endif // CRC_H