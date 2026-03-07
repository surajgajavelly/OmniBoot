/**
 * @file      ymodem.h
 * @brief     YMODEM Protocol parser for OmniBoot
 * @details   Handles the state machine for receiving firmware packets over UART.
 */

#ifndef YMODEM_H
#define YMODEM_H

#include <stdint.h>

/* --- YMODEM Control Characters --- */
#define YM_SOH      0x01  // Start of Header (128-byte packet)
#define YM_STX      0x02  // Start of Header (1024-byte packet)
#define YM_EOT      0x04  // End of Transmission
#define YM_ACK      0x06  // Acknowledge (I got the packet, it's good)
#define YM_NAK      0x15  // Negative Acknowledge (Packet corrupted, resend)
#define YM_CAN      0x18  // Cancel (Abort the transfer)
#define YM_C        0x43  // ASCII 'C' (I am ready, start sending with CRC)

/* --- Packet Sizes --- */
#define PACKET_SIZE_128     128
#define PACKET_SIZE_1024    1024
#define PACKET_HEADER_SIZE  3   // [SOH/STX] [PacketNum] [~PacketNum]
#define PACKET_TRAILER_SIZE 2   // [CRC_High] [CRC_Low]
#define PACKET_OVERHEAD     (PACKET_HEADER_SIZE + PACKET_TRAILER_SIZE)

/* --- Status Codes --- */
typedef enum {
    YM_OK = 0,
    YM_ERROR,
    YM_ABORT,
    YM_TIMEOUT,
    YM_DATA_READY,
    YM_END
} YMODEM_Status;

/* --- Function Prototypes --- */

/**
 * @brief  Receives a file using the YMODEM protocol.
 * @return YM_OK on success, or an error code.
 */
uint32_t YMODEM_Receive(void);

#endif // YMODEM_H