/**
 * @file      startup_stm32.c
 * @brief     STM32F446RE Startup Code (Reset Handler & Vector Table).
 * @details   This file defines the Interrupt Vector Table and the Reset Handler.
 * The Reset Handler is the first code to run on boot. It:
 * 1. Copies the `.data` section from Flash to RAM.
 * 2. Zero-initializes the `.bss` section in RAM.
 * 3. Calls `main()`.
 * @author    Gajavelly Sai Suraj
 * @date      2026-01-10
 * @copyright MIT License
 */

#include <stdint.h>

/* --- Memory Definitions (From Datasheet) --- */
#define SRAM_START  0x20000000U               /*!< Start address of SRAM1 */
#define SRAM_SIZE   (128U * 1024U)            /*!< 128 KB SRAM size */
#define SRAM_END    ((SRAM_START) + (SRAM_SIZE))
#define STACK_START SRAM_END                  /*!< Stack Pointer starts at the end of SRAM (grows down) */

/* --- Linker Script Symbols --- */
/**
 * @brief These symbols are defined in the Linker Script (stm32f446re.ld).
 * They mark the boundaries of memory sections for copying data.
 */
extern uint32_t _etext;   /*!< End of code in Flash (Start of data to copy) */
extern uint32_t _sdata;   /*!< Start of data section in RAM */
extern uint32_t _edata;   /*!< End of data section in RAM */
extern uint32_t _sbss;    /*!< Start of bss section in RAM */
extern uint32_t _ebss;    /*!< End of bss section in RAM */
extern uint32_t _sidata;  /*!< Load address of data section in Flash */

/* --- External Function Prototypes --- */
extern void main(void);
extern void SysTick_Handler(void); /*!< SysTick Interrupt Handler (Implemented in systick.c) */

/* --- Internal Function Prototypes --- */
void Reset_Handler(void);
void Default_Handler(void);

/* --- Vector Table --- */
/**
 * @brief  STM32F4 Vector Table.
 * @note   This array is placed at address 0x08000000 (Flash Start) by the linker.
 * The CPU reads the first two entries on boot:
 * 1. Initial Stack Pointer (MSP)
 * 2. Reset Handler Address (PC)
 */
uint32_t vectors[] __attribute__((section(".isr_vector"))) = {
    STACK_START,                   /* 0x0000 0000 : Initial Stack Pointer */
    (uint32_t)Reset_Handler,       /* 0x0000 0004 : Reset Handler */
    (uint32_t)Default_Handler,     /* 0x0000 0008 : NMI Handler */
    (uint32_t)Default_Handler,     /* 0x0000 000C : Hard Fault Handler */
    (uint32_t)Default_Handler,     /* 0x0000 0010 : MPU Fault Handler */
    (uint32_t)Default_Handler,     /* 0x0000 0014 : Bus Fault Handler */
    (uint32_t)Default_Handler,     /* 0x0000 0018 : Usage Fault Handler */
    0,                             /* 0x0000 001C : Reserved */
    0,                             /* 0x0000 0020 : Reserved */
    0,                             /* 0x0000 0024 : Reserved */
    0,                             /* 0x0000 0028 : Reserved */
    (uint32_t)Default_Handler,     /* 0x0000 002C : SVCall Handler */
    (uint32_t)Default_Handler,     /* 0x0000 0030 : Debug Monitor Handler */
    0,                             /* 0x0000 0034 : Reserved */
    (uint32_t)Default_Handler,     /* 0x0000 0038 : PendSV Handler */
    (uint32_t)SysTick_Handler,     /* 0x0000 003C : SysTick Handler */
    
    /* Peripheral Interrupts would follow here (UART, DMA, Timer, etc.) */
};

/* --- Functions implementation --- */

/**
 * @brief  Reset Handler.
 * @note   This is the entry point of the application.
 * It performs the following C-Runtime (CRT) initialization:
 * 1. Copies initialized data from Flash to RAM.
 * 2. Zeros out uninitialized data (.bss) in RAM.
 * 3. Jumps to main().
 */
void Reset_Handler(void) {
    /* 1. Copy .data section from FLASH to RAM */
    uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint8_t *pDst = (uint8_t*)&_sdata;    // RAM start
    uint8_t *pSrc = (uint8_t*)&_sidata;   // Flash start

    for (uint32_t i = 0; i < size; i++) {
        *pDst++ = *pSrc++;
    }

    /* 2. Init .bss section to zero in RAM */
    size = (uint32_t)&_ebss - (uint32_t)&_sbss;
    pDst = (uint8_t*)&_sbss;

    for (uint32_t i = 0; i < size; i++) {
        *pDst++ = 0;
    }

    /* 3. Call main() */
    main();
}

/**
 * @brief  Default Interrupt Handler.
 * @note   Any interrupt not explicitly handled will end up here.
 * We enter an infinite loop to catch bugs during development.
 */
void Default_Handler(void) {
    while (1);
}