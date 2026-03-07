/**
 * @file      gpio.c
 * @brief     Implementation of the GPIO Driver.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include "gpio.h"
#include "rcc.h" 

/**
 * @brief  Initializes GPIOA (LED) and GPIOC (Button).
 * @note   Configuration Steps:
 * 1. Enable AHB1 Clock for GPIOA and GPIOC.
 * 2. Configure PA5 as Output (LED).
 * 3. Configure PC13 as Input (Button).
 * 4. Enable Pull-Up resistor for PC13 (Active Low button).
 */
void GPIO_Init(void) {
    // 1. Enable Clock for GPIOA (Bit 0) and GPIOC (Bit 2)
    // AHB1ENR is defined in the RCC struct (rcc.h)
    RCC->AHB1ENR |= (1U << 0) | (1U << 2);

    /* -------------------------------------------------------------------------
     * 2. Configure PA5 (LED) as Output
     * -------------------------------------------------------------------------
     * MODER Register: 2 bits per pin.
     * 00 = Input
     * 01 = Output  <-- Target
     * 10 = Alternate Function
     * 11 = Analog
     * * Shift calculation: Pin 5 * 2 bits = Bit 10.
     */
    GPIOA->MODER &= ~(3U << (5 * 2)); // Clear bits 10 and 11
    GPIOA->MODER |=  (1U << (5 * 2)); // Set bit 10 to 1 (Output Mode)

    /* -------------------------------------------------------------------------
     * 3. Configure PC13 (Button) as Input
     * -------------------------------------------------------------------------
     * MODER Register: Pin 13 * 2 bits = Bit 26.
     * Target: 00 (Input Mode)
     */
    GPIOC->MODER &= ~(3U << (13 * 2)); // Clear bits 26 and 27 -> Input Mode
    
    /* -------------------------------------------------------------------------
     * 4. Enable Pull-Up for Button (PC13)
     * -------------------------------------------------------------------------
     * The Nucleo button connects PC13 to Ground when pressed.
     * We need an internal Pull-Up to keep it High when not pressed.
     * PUPDR Register: 2 bits per pin.
     * 01 = Pull-Up.
     */
    GPIOC->PUPDR &= ~(3U << (13 * 2)); // Clear old setting
    GPIOC->PUPDR |=  (1U << (13 * 2)); // Set Pull-Up
}

/**
 * @brief  Writes to a pin using Atomic Bit Set/Reset.
 * @note   Why use BSRR instead of ODR?
 * Writing to ODR requires "Read-Modify-Write" (reading the whole register, 
 * changing one bit, writing it back). This can be interrupted.
 * Writing to BSRR is atomic. The hardware handles it instantly.
 */
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t pin, uint8_t state) {
    if (state == GPIO_PIN_SET) {
        // Write to the lower 16 bits of BSRR to SET the pin High
        GPIOx->BSRR = pin;
    } else {
        // Write to the upper 16 bits of BSRR to RESET the pin Low
        GPIOx->BSRR = (uint32_t)pin << 16;
    }
}

void GPIO_Toggle(GPIO_TypeDef *GPIOx, uint16_t pin) {
    // Standard Toggle: Read ODR, XOR with pin mask, Write back.
    // Note: This is NOT atomic, but acceptable for simple LED blinking.
    GPIOx->ODR ^= pin;
}

uint8_t GPIO_Read(GPIO_TypeDef *GPIOx, uint16_t pin) {
    // Read IDR, mask the specific pin to see if it is High or Low.
    return (GPIOx->IDR & pin) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}