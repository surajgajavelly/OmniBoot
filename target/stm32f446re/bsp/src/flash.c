/**
 * @file      flash.c
 * @brief     Implementation of Internal Flash Driver.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 */

#include "flash.h"

static void flash_wait_for_busy(void) {
    while (FLASH->SR & FLASH_SR_BSY);
}

void Flash_Unlock(void) {
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

void Flash_Lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

uint8_t Flash_EraseSector(uint8_t sector) {
    flash_wait_for_busy();
    Flash_Unlock();

    FLASH->SR |= FLASH_SR_ERRORS;

    uint32_t cr_val = FLASH->CR;
    cr_val &= ~(0xF << 3);         // Clear SNB
    cr_val |= (sector << 3);       // Set SNB
    cr_val |= FLASH_CR_SER;        // Set Sector Erase
    cr_val &= ~(3U << 8);          // Clear PSIZE
    cr_val |= FLASH_CR_PSIZE_32;   // Set PSIZE x32 (Faster Erase)
    FLASH->CR = cr_val;

    FLASH->CR |= FLASH_CR_STRT;
    flash_wait_for_busy();

    uint8_t status = 0;
    if (FLASH->SR & FLASH_SR_ERRORS) {
        status = (uint8_t)FLASH->SR;
    }

    FLASH->CR &= ~FLASH_CR_SER;
    Flash_Lock();
    return status;
}

uint8_t Flash_Write(uint32_t address, uint8_t *data, uint32_t len) {
    flash_wait_for_busy();
    Flash_Unlock();

    FLASH->SR |= FLASH_SR_ERRORS;

    /* * CRITICAL FIX: Ensure PSIZE is set to x8 (00) for byte-level writing.
     * If PSIZE remains at x32 from the Erase operation, writing a single 
     * byte will trigger a Programming Alignment Error (PGAERR).
     */
    FLASH->CR &= ~(3U << 8);      // Set PSIZE to x8 (00)
    FLASH->CR |= FLASH_CR_PG;     // Enable Programming Mode

    for (uint32_t i = 0; i < len; i++) {
        *(volatile uint8_t*)(address + i) = data[i];

        flash_wait_for_busy();
        
        if (FLASH->SR & FLASH_SR_ERRORS) {
            FLASH->CR &= ~FLASH_CR_PG;
            Flash_Lock();
            return (uint8_t)FLASH->SR;
        }
    }

    FLASH->CR &= ~FLASH_CR_PG;
    Flash_Lock();
    return 0;
}