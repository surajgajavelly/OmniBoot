#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "ymodem.h"
#include "systick.h"

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
    SysTick_Init(180000); 
    GPIO_Init();
    UART2_Init();

    UART_Print("\r\n============================\r\n");
    UART_Print(" OmniBoot v1.0 - Active\r\n");
    UART_Print("============================\r\n");

    // Check if Blue Button is pressed (Active Low)
    if (GPIO_Read(GPIOC, BUTTON_PIN) == 0) {
        UART_Print("[MODE] Update Requested. Line silent...\r\n");
        Bootloader_Update(); 
    } else {
        UART_Print("[MODE] Normal Boot. Checking App...\r\n");
        Bootloader_Jump();
    }

    while (1);
}

void Bootloader_Update(void) {
    YMODEM_Status status = YMODEM_Receive();
    
    if (status == YM_OK) {
        // CRITICAL: Wait 500ms for PC to close the YMODEM session
        for(volatile int i=0; i<5000000; i++); 
        
        UART_Print("\r\n[SUCCESS] Transfer Finished.\r\n");
        Bootloader_Jump(); 
    } else {
        UART_Print("\r\n[FAIL] Transfer Error.\r\n");
    }
}

void Bootloader_Jump(void) {
    uint32_t app_msp = *(volatile uint32_t*)APP_START_ADDRESS;
    uint32_t app_reset_handler = *(volatile uint32_t*)(APP_START_ADDRESS + 4);
    
    // Safety check: Is the Stack Pointer inside RAM?
    if (app_msp < 0x20000000 || app_msp > 0x20020000) {
        UART_Print("[HALT] No Valid Firmware Found at 0x08004000.\r\n");
        while(1);
    }

    // 1. Cleanup Peripherals
    SysTick->CTRL = 0; 
    
    // 2. Relocate Vector Table for the new App
    SCB_VTOR = APP_START_ADDRESS;
    
    // 3. Set MSP and branch to App Reset Handler
    __set_MSP(app_msp);
    void (*app_jump_func)(void) = (void (*)(void))app_reset_handler;
    app_jump_func();
}