#include "ymodem.h"
#include "uart.h"
#include "flash.h"
#include "gpio.h"

#define APP_START_ADDRESS 0x08004000

/* Software CRC-16 CCITT (Polynomial 0x1021) 
 * Required because Hardware CRC unit is 32-bit only */
uint16_t YMODEM_CalculateCRC(uint8_t *data, uint32_t len)
{
    uint16_t crc = 0;
    while (len--)
    {
        crc ^= (uint16_t)*data++ << 8;
        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else crc <<= 1;
        }
    }
    return crc;
}

/* Updated return type to uint32_t to report file size */
uint32_t YMODEM_Receive(void)
{
    uint8_t packet_data[1024 + 4]; 
    uint32_t current_address = APP_START_ADDRESS;
    uint32_t total_received_size = 0; // NEW: Track total bytes written
    uint8_t expected_pkt = 0;
    
    // 1. Prepare Flash (Erase enough sectors for a standard app)
    Flash_EraseSector(1); // 0x0800 4000
    Flash_EraseSector(2); // 0x0800 8000
    Flash_EraseSector(3); // 0x0800 C000
    
    // 2. Critical: Flush UART RX buffer and sync line
    while (UART2_Read(10) != -1); 
    UART2_Write(0x43); // Initial 'C'

    while (1)
    {
        // Wait for start of packet (SOH, STX, EOT)
        int head = UART2_Read(3000); 
        
        if (head == -1)
        { 
            UART2_Write(0x43); // Timeout: Request again
            continue; 
        }
        
        if (head == 0x04)
        { // EOT (End of Transmission)
            UART2_Write(0x06); // ACK EOT
            for(volatile int i = 0; i < 5000000; i++);
            return total_received_size; // SUCCESS: Return total bytes
        } 

        uint32_t data_size = (head == 0x02) ? 1024 : 128;
        
        // 3. Read Packet Body: [Num][Inv][Data...][CRC_H][CRC_L]
        uint8_t err = 0;
        for (uint32_t i = 0; i < (data_size + 4); i++)
        {
            int c = UART2_Read(1000);
            if (c == -1) { err = 1; break; }
            packet_data[i] = (uint8_t)c;
        }

        if (err)
        { 
            UART2_Write(0x15); // NAK: Request resend
            continue; 
        }

        // 4. Validate Packet Number Integrity
        if ((uint8_t)(packet_data[0] + packet_data[1]) != 0xFF)
        {
            UART2_Write(0x15);
            continue;
        }

        // 5. Validate CRC-16
        uint16_t crc_received = (packet_data[data_size + 2] << 8) | packet_data[data_size + 3];

        if (YMODEM_CalculateCRC(&packet_data[2], data_size) != crc_received)
        {
            UART2_Write(0x15);
            continue;
        }

        // 6. PACKET 0 LOGIC (Header or End of Session)
        if (packet_data[0] == 0)
        {
            // Check if data field is all zeros (Empty Packet 0 = End of Session)
            uint8_t is_empty = 1;
            for(int i=2; i < 10; i++)
            {
                if(packet_data[i] != 0)
                {
                    is_empty = 0;
                }
            }

            if (is_empty)
            {
                UART2_Write(0x06); // ACK end
                return total_received_size; // Return size (will be 0 if only session end)
            }
            else
            {
                // Filename Packet 0 - Start of Session
                UART2_Write(0x06); // ACK the header
                
                // CRITICAL SYNC: Give PC tool time to switch from Metadata to Data mode
                for(volatile int i=0; i<800000; i++); 
                while (UART2_Read(10) != -1); // Clear any line noise
                
                UART2_Write(0x43); // Send 'C' for Packet 1
                expected_pkt = 1; 
                continue;
            }
        } 
        // 7. DATA PACKET LOGIC (Packet 1, 2, 3...)
        else if (packet_data[0] == expected_pkt)
        {
            Flash_Write(current_address, &packet_data[2], data_size);
            current_address += data_size;
            total_received_size += data_size; // NEW: Accumulate size
            
            UART2_Write(0x06); // ACK
            expected_pkt++;
            GPIO_Toggle(GPIOA, (1U << 5)); // Visual feedback on board
        } 
        else if (packet_data[0] == (expected_pkt - 1))
        {
            // Duplicate packet (Sender missed our last ACK)
            UART2_Write(0x06);
        }
        else
        {
            // Total out of sync
            UART2_Write(0x15);
        }
    }
}