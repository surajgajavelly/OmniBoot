/**
 * @file      rcc.c
 * @brief     Implementation of the Reset and Clock Control (RCC) Driver.
 * @details   Configures the STM32F446RE System Clock to 180 MHz using the PLL.
 * Manages bus prescalers (AHB, APB1, APB2) and Flash latency.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include "rcc.h"
#include "flash.h"

/**
 * @brief  Initializes the System Clock to 180 MHz.
 * @note   This function assumes an 8 MHz High-Speed External (HSE) crystal
 * source (typically provided by the ST-Link MCO on Nucleo boards).
 * * Target Frequencies:
 * - SYSCLK: 180 MHz
 * - AHB:    180 MHz
 * - APB1:   45 MHz (Max 45 MHz)
 * - APB2:   90 MHz (Max 90 MHz)
 */
void RCC_Init(void) {
    /* -------------------------------------------------------------------------
     * 1. Enable HSE (High Speed External) Clock
     * ------------------------------------------------------------------------- */
    RCC->CR |= RCC_CR_HSEON;
    
    // Wait for the External Crystal/Clock to stabilize
    while (!(RCC->CR & RCC_CR_HSERDY)); 

    /* -------------------------------------------------------------------------
     * 2. Configure Power Controller & Voltage Scaling
     * ------------------------------------------------------------------------- */
    // Enable Power Controller Clock (PWREN is Bit 28 in APB1ENR)
    RCC->APB1ENR |= (1U << 28); 
    
    // Set Voltage Scale 1 (High Performance) to support 180 MHz
    // Bits 14:15 in PWR_CR must be '11'
    PWR->CR |= PWR_CR_VOS_SCALE1; 

    /* -------------------------------------------------------------------------
     * 3. Configure Flash Latency
     * ------------------------------------------------------------------------- */
    // CPU @ 180 MHz is too fast for Flash memory. We need 5 Wait States (WS).
    // Also enable Prefetch (PRFTEN), Instruction Cache (ICEN), and Data Cache (DCEN)
    // to mitigate performance loss from wait states.
    FLASH->ACR = FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;

    /* -------------------------------------------------------------------------
     * 4. Configure PLL (Phase Locked Loop)
     * ------------------------------------------------------------------------- */
    // Formula: F_vco   = HSE_Freq * (PLLN / PLLM)
    //          SysClk  = F_vco / PLLP
    //
    // Input (HSE): 8 MHz
    // 
    // Settings:
    // M = 4    -> VCO Input = 8 MHz / 4 = 2 MHz (Must be between 1 and 2 MHz)
    // N = 180  -> VCO Output = 2 MHz * 180 = 360 MHz
    // P = 2    -> SysClk = 360 MHz / 2 = 180 MHz
    // Q = 7    -> USB Clk = 360 MHz / 7 ≈ 51.4 MHz (Not used for now)
    //
    // Register Mapping (PLLCFGR):
    // Bit 0-5:   PLLM
    // Bit 6-14:  PLLN
    // Bit 16-17: PLLP (00 = /2, 01 = /4, etc.)
    // Bit 22:    PLLSRC (1 = HSE)
    // Bit 24-27: PLLQ
    
    RCC->PLLCFGR = (4U << 0)               | // PLLM = 4
                   (180U << 6)             | // PLLN = 180
                   (0U << 16)              | // PLLP = 2 (Bits: 00)
                   RCC_PLLCFGR_PLLSRC_HSE  | // Source = HSE
                   (7U << 24);               // PLLQ = 7

    /* -------------------------------------------------------------------------
     * 5. Enable PLL and Wait for Lock
     * ------------------------------------------------------------------------- */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)); 

    /* -------------------------------------------------------------------------
     * 6. Configure Bus Prescalers (AHB, APB1, APB2)
     * ------------------------------------------------------------------------- */
    // AHB  Prescaler = 1 (SysClk / 1 = 180 MHz)
    // APB1 Prescaler = 4 (AHB / 4 = 45 MHz). Max allowed is 45 MHz.
    // APB2 Prescaler = 2 (AHB / 2 = 90 MHz). Max allowed is 90 MHz.
    
    // Bit 10-12 (PPRE1): 101 = Div 4
    // Bit 13-15 (PPRE2): 100 = Div 2
    RCC->CFGR |= (5U << 10) | (4U << 13); 

    /* -------------------------------------------------------------------------
     * 7. Select PLL as System Clock Source
     * ------------------------------------------------------------------------- */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    
    // Wait until the hardware confirms PLL is the system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL); 
}