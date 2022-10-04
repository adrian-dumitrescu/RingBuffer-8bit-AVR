#include <avr/io.h>
#include <stdint.h>

#define DEFAULT_BUFFER_SIZE  8

enum RingBufferStatus {
    BUFFER_OK, BUFFER_EMPTY, BUFFER_FULL
};

typedef struct RingBuffer {
    uint8_t data[DEFAULT_BUFFER_SIZE]; // default value DEFAULT_BUFFER_SIZE
    uint8_t head_index; // index pointing to newest data in the buffer
    uint8_t tail_index; // index pointing to oldest data in the buffer
} ringbuffer_t;


enum RingBufferStatus bufferWriteByte(volatile struct RingBuffer *buffer, volatile uint8_t dataByte);
enum RingBufferStatus bufferReadByte(volatile struct RingBuffer *buffer, volatile uint8_t *dataByte);
enum RingBufferStatus bufferPeek(volatile struct RingBuffer *buffer, volatile uint8_t *peekByte);
enum RingBufferStatus bufferReadCommand(volatile struct RingBuffer *buffer, volatile uint8_t index, volatile uint8_t *commandData, volatile uint8_t *commandLength);
enum RingBufferStatus getBufferStatus(volatile struct RingBuffer *buffer);
uint8_t getHeadIndex(volatile struct RingBuffer *buffer);
uint8_t getTailIndex(volatile struct RingBuffer *buffer);


