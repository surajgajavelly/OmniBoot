/**
 * @file      crc.c
 * @brief     Implementation of the Hardware CRC Driver.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include "crc.h"

void CRC_Init(void) {
    /* -------------------------------------------------------------------------
     * 1. Enable CRC Clock
     * -------------------------------------------------------------------------
     * The CRC unit is on the AHB1 bus. We must enable its clock before usage.
     * Bit 12 of AHB1ENR.
     */
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
}

uint32_t CRC_Calculate(uint32_t *data, uint32_t len) {
    /* -------------------------------------------------------------------------
     * 2. Reset the CRC Calculation Unit
     * -------------------------------------------------------------------------
     * Setting the RESET bit in CRC_CR clears the Data Register to 0xFFFFFFFF.
     * This is required to start a new calculation fresh.
     */
    CRC->CR |= CRC_CR_RESET;

    /* -------------------------------------------------------------------------
     * 3. Feed Data
     * -------------------------------------------------------------------------
     * The hardware updates the checksum in real-time as we write to the DR.
     * We feed 32-bit words at a time.
     */
    for (uint32_t i = 0; i < len; i++) {
        CRC->DR = data[i];
    }

    /* -------------------------------------------------------------------------
     * 4. Return Result
     * -------------------------------------------------------------------------
     * Reading the DR register returns the final accumulated checksum.
     */
    return CRC->DR;
}