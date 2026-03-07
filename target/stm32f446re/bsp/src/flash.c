/**
 * @file      flash.c
 * @brief     Implementation of Internal Flash Driver.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include "flash.h"

/**
 * @brief  Waits for the Busy (BSY) flag to clear.
 * @note   Flash operations are slow (Erase ~1-2s). 
 * The CPU must wait for the Flash Controller to finish.
 */
static void flash_wait_for_busy(void) {
    while (FLASH->SR & FLASH_SR_BSY) {
        // Ideally, we would add a timeout counter here to prevent
        // infinite loops in case of hardware failure.
        // For bare-metal learning, we assume hardware is healthy.
    }
}

void Flash_Unlock(void) {
    // Only unlock if it is currently locked
    if (FLASH->CR & FLASH_CR_LOCK) {
        // The hardware requires this exact sequence to unlock
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

void Flash_Lock(void) {
    FLASH->CR |= FLASH_CR_LOCK;
}

uint8_t Flash_EraseSector(uint8_t sector) {
    // 1. Wait for any pending operations to finish
    flash_wait_for_busy();

    // 2. Unlock the Flash
    Flash_Unlock();

    // 3. Clear Previous Error Flags (Write 1 to clear)
    FLASH->SR |= FLASH_SR_ERRORS;

    // 4. Configure Erase Parameters
    // - Clear SNB (Sector Number) bits [6:3]
    // - Set new SNB
    // - Set SER (Sector Erase) bit
    // - Set PSIZE to x32 (Voltage Range 2.7V - 3.6V)
    uint32_t cr_val = FLASH->CR;
    cr_val &= ~(0xF << 3);        // Clear SNB
    cr_val |= (sector << 3);      // Set SNB
    cr_val |= FLASH_CR_SER;       // Set Sector Erase
    cr_val &= ~(3U << 8);         // Clear PSIZE
    cr_val |= FLASH_CR_PSIZE_32;  // Set PSIZE x32
    FLASH->CR = cr_val;

    // 5. Trigger the Erase (The "GO" button)
    FLASH->CR |= FLASH_CR_STRT;

    // 6. Wait for it to finish (Blocking)
    flash_wait_for_busy();

    // 7. Check for Errors
    uint8_t status = 0;
    if (FLASH->SR & FLASH_SR_ERRORS) {
        status = (uint8_t)FLASH->SR; // Capture error flags
    }

    // 8. Cleanup
    FLASH->CR &= ~FLASH_CR_SER; // Clear SER bit
    Flash_Lock();               // Re-lock for safety

    return status;
}

uint8_t Flash_Write(uint32_t address, uint8_t *data, uint32_t len) {
    // 1. Wait for pending ops
    flash_wait_for_busy();
    Flash_Unlock();

    // 2. Clear Errors
    FLASH->SR |= FLASH_SR_ERRORS;

    // 3. Set Programming Mode to x8 (Byte Access)
    // Even though we are on 3.3V, setting PSIZE to x8 allows us to write
    // individual bytes (uint8_t). If we used x32, we'd have to write 4 bytes at a time.
    FLASH->CR &= ~(3U << 8);      // Clear PSIZE -> Becomes 00 (x8)
    FLASH->CR |= FLASH_CR_PG;     // Program Mode Enable

    // 4. Loop through data bytes
    for (uint32_t i = 0; i < len; i++) {
        // Write the byte directly to the address
        // The hardware detects the write to the address bus and starts the programming sequence.
        *(volatile uint8_t*)(address + i) = data[i];

        // Wait for the write to finish
        flash_wait_for_busy();
        
        // Check errors immediately after every byte
        if (FLASH->SR & FLASH_SR_ERRORS) {
            FLASH->CR &= ~FLASH_CR_PG; // Turn off PG
            Flash_Lock();
            return (uint8_t)FLASH->SR;
        }
    }

    // 5. Cleanup
    FLASH->CR &= ~FLASH_CR_PG; // Disable Program Mode
    Flash_Lock();
    
    return 0; // Success
}