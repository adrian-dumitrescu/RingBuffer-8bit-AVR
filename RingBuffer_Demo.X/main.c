/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */

#include "mcc_generated_files/mcc.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "mcc_generated_files/include/ringbuffer.h"

/* DEFINES */
#define UART_RX_DELIMITER 0x3B // AKA ";" (ASCII) // AKA "59" (DEC)
#define DATA_BUFFER_SIZE 16
#define UART_RX_DATA        USART0.RXDATAL
#define UART_TX_DATA        USART0.TXDATAL
#define UART_IsRxReady      USART0_IsRxReady

#define UART_RXC_Enable()   { USART0.CTRLA |= USART_RXCIE_bm; }
#define UART_RXC_Disable()  { USART0.CTRLA &= ~USART_RXCIE_bm; }

#define ENABLE_GLOBAL_INTERRUPTS()  {sei();}
#define DISABLE_GLOBAL_INTERRUPTS() {cli();}

/* FUNCTIONS */
void fetchCommand();
void volatileMemset(volatile uint8_t *array, volatile int value, volatile size_t sizeOfArray);

// Declare new buffer to hold RX data
volatile ringbuffer_t _rxBuffer = {
    {}, 0, 0
};

// Declare new buffer to hold delimiter indexes
volatile ringbuffer_t _delimiterBuffer = {
    {}, 0, 0
};

volatile static uint8_t _cmdBuffer_data[DATA_BUFFER_SIZE];

enum RingBufferStatus _rxBuffer_status;
enum RingBufferStatus _delimiterBuffer_status;

volatile uint8_t delimiterIndex = 0;
volatile uint8_t commandLength = 0;

void volatileMemset(volatile uint8_t *array, volatile int value, volatile size_t sizeOfArray) {
    for (int i = 0; i < sizeOfArray; i++) {
        array[i] = value;
    }
}

void fetchCommand() {
    // Check if there is any delimiter in the delimiterBuffer, this function also returns the value of the peeked byte (if any))
    _delimiterBuffer_status = bufferPeek(&_delimiterBuffer, &delimiterIndex);

    // If the delimiter buffer is not empty, there is a command pending
    if (_delimiterBuffer_status != BUFFER_EMPTY) {

        // Read the delimiter index value from _delimiterBuffer
        _delimiterBuffer_status = bufferReadByte(&_delimiterBuffer, &delimiterIndex);

        // Copy command from _rxBuffer
        _rxBuffer_status = bufferReadCommand(&_rxBuffer, delimiterIndex, _cmdBuffer_data, &commandLength);

        // Output the command on Serial
        USART0_EchoCommand(_cmdBuffer_data, commandLength);

        // Clear the command array after handling the data
        volatileMemset(_cmdBuffer_data, 0, sizeof (_cmdBuffer_data));
    }
}

ISR(USART0_RXC_vect) {
    cli();
    if (UART_IsRxReady()) {
        uint8_t rxData = UART_RX_DATA;
        if (rxData == UART_RX_DELIMITER) {
            if (_rxBuffer_status == BUFFER_FULL && _delimiterBuffer_status != BUFFER_EMPTY) {
                /* do nothing * This prevents the a case where the buffer is full and delimiters are saved without any message attached to them */
            }
            else{
                _delimiterBuffer_status = bufferWriteByte(&_delimiterBuffer, getHeadIndex(&_rxBuffer));
            }
        } else {
            _rxBuffer_status = bufferWriteByte(&_rxBuffer, rxData);
        }
    }
    sei();
}

/*
 * ==================================================
 * ************** RingBuffer DEMO Main **************
 * ================================================== 
 */
int main(void) {
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();

    /* Enable the RX interrupt */
    UART_RXC_Enable();

    /* Enable Global Interrupts */
    ENABLE_GLOBAL_INTERRUPTS();

    /* Replace with your application code */
    while (1) {

        /* If delimiter is found, print command to serial */
        //_delay_ms(500);
        fetchCommand();

    }
}
/**
    End of File
 */