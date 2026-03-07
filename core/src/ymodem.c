#include "ymodem.h"
#include "uart.h"
#include "flash.h"
#include "gpio.h"

#define APP_START_ADDRESS 0x08004000

uint16_t CRC16_Update(uint16_t crc, uint8_t data) {
    crc ^= (uint16_t)data << 8;
    for (uint8_t i = 0; i < 8; i++) {
        if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
        else crc <<= 1;
    }
    return crc;
}

uint16_t YMODEM_CalculateCRC(uint8_t *data, uint32_t len) {
    uint16_t crc = 0;
    for (uint32_t i = 0; i < len; i++) crc = CRC16_Update(crc, data[i]);
    return crc;
}

YMODEM_Status YMODEM_Receive(void) {
    uint8_t packet_data[1024 + 10];
    uint32_t current_address = APP_START_ADDRESS;
    uint8_t expected_pkt = 0;
    
    // Preparation
    Flash_EraseSector(1);
    Flash_EraseSector(2);
    while (UART2_Read(10) != -1); // Flush line
    UART2_Write(0x43); // Send 'C'

    while (1) {
        int head = UART2_Read(3000);
        if (head == -1) { UART2_Write(0x43); continue; }
        if (head == 0x04) { UART2_Write(0x06); return YM_OK; } // EOT -> ACK

        uint32_t size = (head == 0x02) ? 1024 : 128;
        
        // Read Full Packet
        uint8_t err = 0;
        for (uint32_t i = 0; i < (size + 4); i++) {
            int c = UART2_Read(1000);
            if (c == -1) { err = 1; break; }
            packet_data[i] = (uint8_t)c;
        }

        // Validate
        if (err || (uint8_t)(packet_data[0] + packet_data[1]) != 0xFF) {
            UART2_Write(0x15); // NAK
            continue;
        }

        uint16_t crc_r = (packet_data[size+2] << 8) | packet_data[size+3];
        if (YMODEM_CalculateCRC(&packet_data[2], size) != crc_r) {
            UART2_Write(0x15); // NAK
            continue;
        }

        // Process
        if (packet_data[0] == expected_pkt) {
            if (expected_pkt != 0) {
                Flash_Write(current_address, &packet_data[2], size);
                current_address += size;
            }
            UART2_Write(0x06); // ACK
            if (expected_pkt == 0) UART2_Write(0x43); // Send 'C' for Packet 1
            expected_pkt++;
            GPIO_Toggle(GPIOA, GPIO_PIN_5);
        } else {
            UART2_Write(0x15);
        }
    }
}