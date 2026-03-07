/**
 * @file      uart.c
 * @brief     Implementation of USART2 Driver.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include "uart.h"
#include "systick.h"

/**
 * @brief  Configures GPIO Pins for UART.
 * @note   On the Nucleo-F446RE:
 * - PA2 is USART2_TX
 * - PA3 is USART2_RX
 * We must set them to "Alternate Function" mode and select AF7 (0111).
 */
static void uart2_gpio_init(void) {
    // 1. Enable GPIOA Clock (Bit 0 in AHB1ENR)
    RCC->AHB1ENR |= (1U << 0);

    // 2. Configure PA2 (TX) to Alternate Function Mode
    // MODER: Clear bits 4-5, Set bit 5 (10 = AF)
    GPIOA->MODER   &= ~(3U << 4); 
    GPIOA->MODER   |=  (2U << 4);
    
    // Select AF7 (USART2) in AFR[0] (Low Register)
    // AFR[0] Bits 8-11 correspond to Pin 2
    GPIOA->AFR[0]  &= ~(0xFU << 8); // Clear old AF
    GPIOA->AFR[0]  |=  (7U << 8);   // Set AF7 (0111)

    // 3. Configure PA3 (RX) to Alternate Function Mode
    // MODER: Clear bits 6-7, Set bit 7 (10 = AF)
    GPIOA->MODER   &= ~(3U << 6); 
    GPIOA->MODER   |=  (2U << 6);

    // Select AF7 (USART2) in AFR[0]
    // AFR[0] Bits 12-15 correspond to Pin 3
    GPIOA->AFR[0]  &= ~(0xFU << 12); // Clear old AF
    GPIOA->AFR[0]  |=  (7U << 12);   // Set AF7 (0111)
}

void UART2_Init(void) {
    // 1. Enable USART2 Clock (APB1 Bus)
    RCC->APB1ENR |= (1U << 17);

    // 2. Configure GPIO Pins
    uart2_gpio_init();

    // 3. Reset CR1 (Clear any pre-existing configuration)
    USART2->CR1 = 0;

    /* -------------------------------------------------------------------------
     * 4. BAUD RATE CALCULATION
     * -------------------------------------------------------------------------
     * Formula: BRR = (PCLK + (Baud / 2)) / Baud
     * This essentially rounds the result to the nearest integer.
     *
     * - System Clock: 180 MHz
     * - APB1 Clock:   45 MHz (180 / 4)
     * - Target Baud:  115200
     *
     * Calculation:
     * (45,000,000 + 57,600) / 115,200 = 391.125 -> 391
     * Hex: 0x187.
     */
    uint32_t apb1_clk = 45000000U; 
    uint32_t baud = 115200U;
    
    // Simple rounding formula suitable for high clock speeds
    uint32_t brr = (apb1_clk + (baud/2U)) / baud; 
    USART2->BRR = brr;

    // 5. Enable UART Module
    // UE (Enable), TE (Transmitter), RE (Receiver)
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void UART2_Write(int ch) {
    // Wait until Transmit Data Register is Empty (TXE bit = 1)
    while (!(USART2->SR & USART_SR_TXE));
    
    // Write data to Data Register (Low 8 bits)
    USART2->DR = (uint8_t)ch;
}

int UART2_Read(uint32_t timeout) {
    uint32_t start_tick = GetTick();

    // Loop until RXNE (Read Data Register Not Empty) flag is set
    while (!(USART2->SR & USART_SR_RXNE)) {
        // Check if timeout duration has passed
        if ((GetTick() - start_tick) > timeout) {
            return -1; // Indicate Timeout
        }
    }
    
    // Read and return data (Mask to 8 bits)
    return (int)(USART2->DR & 0xFF);
}