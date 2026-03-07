/**
 * @file      flash.h
 * @brief     Internal Flash Driver for STM32F446RE.
 * @details   Provides functions to Erase Sectors and Write Data to the internal Flash memory.
 * Implements the Flash Key unlock sequence and Busy flag polling.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include "rcc.h" // For FLASH_R_BASE definition

/**
 * @brief Flash Register Map (RM0390 Reference Manual).
 */
typedef struct {
    volatile uint32_t ACR;      /*!< Access Control Register (Offset: 0x00) */
    volatile uint32_t KEYR;     /*!< Key Register (Offset: 0x04) */
    volatile uint32_t OPTKEYR;  /*!< Option Key Register (Offset: 0x08) */
    volatile uint32_t SR;       /*!< Status Register (Offset: 0x0C) */
    volatile uint32_t CR;       /*!< Control Register (Offset: 0x10) */
    volatile uint32_t OPTCR;    /*!< Option Control Register (Offset: 0x14) */
} FLASH_TypeDef;

/* --- Pointer Definition --- */
/* Connects the struct to the physical address 0x40023C00 */
#ifndef FLASH
#define FLASH ((FLASH_TypeDef *) FLASH_R_BASE)
#endif

/**
 * @defgroup FLASH_Status_Flags Status Register (SR) Flags
 * @{
 */
#define FLASH_SR_BSY        (1U << 16)  /*!< Busy: 1 = Operation in progress */
#define FLASH_SR_PGSERR     (1U << 7)   /*!< Programming Sequence Error */
#define FLASH_SR_PGPERR     (1U << 6)   /*!< Programming Parallelism Error */
#define FLASH_SR_PGAERR     (1U << 5)   /*!< Programming Alignment Error */
#define FLASH_SR_WRPERR     (1U << 4)   /*!< Write Protection Error */
#define FLASH_SR_EOP        (1U << 0)   /*!< End of Operation (Success) */

/* Error Mask: Used to check/clear all error flags at once */
#define FLASH_SR_ERRORS     (FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR)
/** @} */


/**
 * @defgroup FLASH_Control_Bits Control Register (CR) Bits
 * @{
 */
#define FLASH_CR_LOCK       (1U << 31)  /*!< Lock Bit: 1 = Locked, 0 = Unlocked */
#define FLASH_CR_STRT       (1U << 16)  /*!< Start: Trigger Erase Operation */
#define FLASH_CR_PSIZE_32   (2U << 8)   /*!< Program Size x32 (Required for 3.3V supply) */
#define FLASH_CR_SER        (1U << 1)   /*!< Sector Erase Mode */
#define FLASH_CR_PG         (1U << 0)   /*!< Programming Mode */
/** @} */


/**
 * @defgroup FLASH_Keys Unlock Keys
 * @brief    Magic values to unlock the Flash Control Register.
 * @{
 */
#define FLASH_KEY1          0x45670123U
#define FLASH_KEY2          0xCDEF89ABU
/** @} */


/* --- Function Prototypes --- */

/**
 * @brief  Unlocks the Flash Control Register (CR) allowing writes/erases.
 * @note   Writes KEY1 then KEY2 to the KEYR register.
 */
void Flash_Unlock(void);

/**
 * @brief  Locks the Flash Control Register (CR) to prevent accidental writes.
 */
void Flash_Lock(void);

/**
 * @brief  Erases a specific Flash Sector.
 * @param  sector: Sector number (0-7). 
 * @return 0 on success, or Error Flags (SR) on failure.
 * @note   This function blocks the CPU until the erase is complete (can take >1 second).
 */
uint8_t Flash_EraseSector(uint8_t sector);

/**
 * @brief  Writes data to Flash memory byte-by-byte.
 * @param  address: Destination address in Flash.
 * @param  data: Pointer to source data.
 * @param  len: Number of bytes to write.
 * @return 0 on success, or Error Flags (SR) on failure.
 */
uint8_t Flash_Write(uint32_t address, uint8_t *data, uint32_t len);

#endif // FLASH_H