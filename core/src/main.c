#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "ymodem.h"
#include "systick.h"

#define LED_PIN       (1U << 5)
#define BUTTON_PIN    (1U << 13)
#define APP_START_ADDRESS 0x08004000
#define SCB_VTOR (*(volatile uint32_t *)0xE000ED08)

static inline void __set_MSP(uint32_t topOfMainStack) {
    __asm__ volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}

void Bootloader_Update(void);
void Bootloader_Jump(void);

void UART_Print(char *msg) {
    while (*msg) UART2_Write(*msg++);
}

int main(void) {
    RCC_Init();
    SysTick_Init(180000); // 1ms tick
    GPIO_Init();
    UART2_Init();

    UART_Print("\r\n============================\r\n");
    UART_Print(" OmniBoot v1.0 - Wired Ready\r\n");
    UART_Print("============================\r\n");

    /* main.c */
if (GPIO_Read(GPIOC, BUTTON_PIN) == 0) {
    // Last print allowed before protocol starts
    UART_Print("Starting YMODEM...\r\n"); 
    Bootloader_Update(); 
}

    while (1);
}

void Bootloader_Update(void) {
    UART_Print("\r\n>> Waiting for YMODEM transfer...\r\n");
    YMODEM_Status status = YMODEM_Receive();
    
    if (status == YM_OK) {
        UART_Print("\r\n[SUCCESS] Firmware Downloaded!\r\n");
        Bootloader_Jump(); 
    } else {
        UART_Print("\r\n[ERROR] YMODEM Transfer Failed.\r\n");
    }
    while(1);
}

void Bootloader_Jump(void) {
    UART_Print(">> Executing Jump...\r\n");
    
    uint32_t app_msp = *(volatile uint32_t*)APP_START_ADDRESS;
    uint32_t app_reset_handler = *(volatile uint32_t*)(APP_START_ADDRESS + 4);
    
    if (app_msp < 0x20000000 || app_msp > 0x20020000) {
        UART_Print("[ERROR] No valid app found at Sector 1!\r\n");
        while(1);
    }

    // 1. CLEANUP: Disable SysTick to prevent interrupts during jump
    SysTick->CTRL = 0; 
    
    // 2. Relocate Vector Table
    SCB_VTOR = APP_START_ADDRESS;
    
    // 3. Set MSP and Jump
    __set_MSP(app_msp);
    void (*app_jump_func)(void) = (void (*)(void))app_reset_handler;
    app_jump_func();
}