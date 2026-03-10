#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "ymodem.h"
#include "systick.h"
#include "crc.h" // Ensure your CRC driver header is included

#define BUTTON_PIN    (1U << 13)
#define APP_START_ADDRESS 0x08004000
#define SCB_VTOR (*(volatile uint32_t *)0xE000ED08)

void Bootloader_Update(void);
void Bootloader_Jump(void);

/* Set the value of the MSP register */
static inline void __set_MSP(uint32_t topOfMainStack)
{
    __asm__ volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}

/* Print a 32-bit value in hex format */
void UART_PrintHex(uint32_t val)
{
    char hex_chars[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--)
    {
        UART2_Write(hex_chars[(val >> (i * 4)) & 0x0F]);
    }
}

/* Print a string to the UART*/
void UART_Print(char *msg)
{
    while (*msg)
    {
        UART2_Write(*msg++);
    }
}

/* Main function */
int main(void)
{
    RCC_Init();
    SysTick_Init(180000); 
    GPIO_Init();
    UART2_Init();
    CRC_Init();

    UART_Print("\r\n============================\r\n");
    UART_Print(" OmniBoot v1.0 - Secure\r\n");
    UART_Print("============================\r\n");

    if (GPIO_Read(GPIOC, BUTTON_PIN) == 0) {
        UART_Print("[MODE] Update Requested...\r\n");
        Bootloader_Update(); 
    } else {
        UART_Print("[MODE] Normal Boot. Checking App...\r\n");
        Bootloader_Jump();
    }

    while (1);
}


uint32_t downloaded_size = 0;   // Global variable to hold the size of the downloaded firmware

/* Update the bootloader */
void Bootloader_Update(void) {
    // YMODEM_Receive returns the uint32_t size (total bytes)
    downloaded_size = YMODEM_Receive();
    
    if (downloaded_size > 0) {
        for(volatile int i = 0; i < 1000000; i++);
        
        UART_Print("\r\n[1/2] YMODEM Transfer: OK\r\n");

        // --- HARDWARE CRC32 CHECK ---
        UART_Print("[2/2] Hardware CRC32 Verify: ");
        
        uint32_t *app_ptr = (uint32_t *)APP_START_ADDRESS;
        
        // Calculate CRC32 of the downloaded firmware
        uint32_t final_checksum = CRC_Calculate(app_ptr, (downloaded_size / 4));
        
        UART_Print("DONE\r\n[INFO] App Signature: 0x");
        UART_PrintHex(final_checksum);
        UART_Print("\r\n");
        
        //
        for(volatile int i=0; i<8000000; i++); 
        
        Bootloader_Jump(); 
    }
    else
    {
        UART_Print("\r\n[FAIL] Transfer Error.\r\n");
    }
}

void Bootloader_Jump(void)
{
    uint32_t app_msp = *(volatile uint32_t*)APP_START_ADDRESS;
    uint32_t app_reset_handler = *(volatile uint32_t*)(APP_START_ADDRESS + 4);
    
    if (app_msp < 0x20000000 || app_msp > 0x20020000)
    {
        UART_Print("[HALT] No Valid Firmware Found.\r\n");
        while(1);
    }

    SysTick->CTRL = 0; 
    SCB_VTOR = APP_START_ADDRESS;
    __set_MSP(app_msp);
    void (*app_jump_func)(void) = (void (*)(void))app_reset_handler;
    app_jump_func();
}