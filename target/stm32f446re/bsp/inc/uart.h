/**
 * @file      uart.h
 * @brief     USART2 Driver for STM32F446RE.
 * @details   Handles Serial Communication via the Virtual COM Port (USB).
 * Configured for 115200 Baud, 8 Data bits, No Parity, 1 Stop bit (8N1).
 * Includes timeout protection for robust reception.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

/**
 * @defgroup UART_Memory_Map USART2 Base Address
 * @brief    USART2 is connected to the APB1 Bus (45 MHz Max).
 * @{
 */
#define USART2_BASE  (PERIPH_BASE + 0x4400U)
/** @} */

/**
 * @brief USART Register Map (Reference Manual RM0390).
 */
typedef struct {
    volatile uint32_t SR;    /*!< Status Register (Offset: 0x00) */
    volatile uint32_t DR;    /*!< Data Register (Offset: 0x04) */
    volatile uint32_t BRR;   /*!< Baud Rate Register (Offset: 0x08) */
    volatile uint32_t CR1;   /*!< Control Register 1 (Offset: 0x0C) */
    volatile uint32_t CR2;   /*!< Control Register 2 (Offset: 0x10) */
    volatile uint32_t CR3;   /*!< Control Register 3 (Offset: 0x14) */
    volatile uint32_t GTPR;  /*!< Guard Time & Prescaler (Offset: 0x18) */
} USART_TypeDef;

/* --- Pointer Definition --- */
#define USART2 ((USART_TypeDef *) USART2_BASE)


/**
 * @defgroup UART_Bits Control & Status Bits
 * @{
 */
/* CR1 (Control Register 1) */
#define USART_CR1_UE     (1U << 13) /*!< USART Enable */
#define USART_CR1_M      (1U << 12) /*!< Word Length (0=8 bits, 1=9 bits) */
#define USART_CR1_PCE    (1U << 10) /*!< Parity Control Enable */
#define USART_CR1_TE     (1U << 3)  /*!< Transmitter Enable */
#define USART_CR1_RE     (1U << 2)  /*!< Receiver Enable */
#define USART_CR1_RXNEIE (1U << 5)  /*!< RX Not Empty Interrupt Enable */

/* SR (Status Register) */
#define USART_SR_TXE     (1U << 7)  /*!< Transmit Data Register Empty */
#define USART_SR_RXNE    (1U << 5)  /*!< Read Data Register Not Empty */
/** @} */


/* --- Function Prototypes --- */

/**
 * @brief  Initialize USART2 at 115200 baud.
 * @note   Configures GPIO PA2 (TX) and PA3 (RX) to Alternate Function 7.
 * Enables the APB1 clock and calculates the Baud Rate Register (BRR).
 */
void UART2_Init(void);

/**
 * @brief  Transmits a single character.
 * @param  ch: Character to send (uint8_t).
 * @note   Blocking call. Waits until the Transmit Buffer is empty before writing.
 */
void UART2_Write(int ch);

/**
 * @brief  Receives a single character with timeout protection.
 * @param  timeout: Maximum time to wait in milliseconds.
 * @return The received character (0-255) or -1 if a timeout occurred.
 */
int UART2_Read(uint32_t timeout);

#endif // UART_H