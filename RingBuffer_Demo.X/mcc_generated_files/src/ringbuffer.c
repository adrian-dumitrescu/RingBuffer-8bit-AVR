#include "../include/ringbuffer.h"
#include <stdio.h>

uint8_t getHeadIndex(volatile struct RingBuffer *buffer) {
    return buffer->head_index;

}

uint8_t getTailIndex(volatile struct RingBuffer *buffer) {
    return buffer->tail_index;
}

enum RingBufferStatus bufferWriteByte(volatile struct RingBuffer *buffer, volatile uint8_t dataByte) {
    uint8_t next_index = (((buffer->head_index) + 1) % DEFAULT_BUFFER_SIZE);

    if (next_index == buffer->tail_index) {
        return BUFFER_FULL;
    }

    buffer->data[buffer->head_index] = dataByte;
    buffer->head_index = next_index;
    return BUFFER_OK;
}


enum RingBufferStatus bufferReadCommand(volatile struct RingBuffer *buffer, volatile uint8_t index, volatile uint8_t *commandData, volatile uint8_t *commandLength) {
    
    *commandLength = 0;
    //uint8_t length = 0;
    
    if (buffer->head_index == buffer->tail_index) {
        return BUFFER_EMPTY;
    }
    
    while (buffer->tail_index != index) {
        *commandData++ = buffer->data[buffer->tail_index];
        buffer->tail_index = ((buffer->tail_index + 1) % DEFAULT_BUFFER_SIZE);
        *commandLength += 1;
        if (buffer->head_index == buffer->tail_index) {
            return BUFFER_EMPTY;
        }
    }
    return BUFFER_OK;
}

enum RingBufferStatus bufferReadByte(volatile struct RingBuffer *buffer, volatile uint8_t *dataByte) {

    if (buffer->head_index == buffer->tail_index) {
        return BUFFER_EMPTY;
    }

    *dataByte = buffer->data[buffer->tail_index];
    buffer->tail_index = ((buffer->tail_index + 1) % DEFAULT_BUFFER_SIZE);

    return BUFFER_OK;
}

enum RingBufferStatus getBufferStatus(volatile struct RingBuffer *buffer) {

    if (buffer->head_index == buffer->tail_index) {
        return BUFFER_EMPTY;
    }

    //**********************************************************************

    uint8_t next_index = (((buffer->head_index) + 1) % DEFAULT_BUFFER_SIZE);
    if (next_index == buffer->tail_index) {
        return BUFFER_FULL;
    } else {

        return BUFFER_OK;
    }
}

enum RingBufferStatus bufferPeek(volatile struct RingBuffer *buffer, volatile uint8_t *peekByte) {

    // ***** CUSOM SEQUENCE *****
    uint8_t next_index = (((buffer->head_index) + 1) % DEFAULT_BUFFER_SIZE);
    uint8_t last_index = ((DEFAULT_BUFFER_SIZE + (buffer->head_index) - 1) % DEFAULT_BUFFER_SIZE);

    if (next_index == buffer->tail_index) {
        *peekByte = buffer->data[last_index];
        return BUFFER_FULL;
    }

    if (buffer->head_index == buffer->tail_index) {
        return BUFFER_EMPTY;
    }
    *peekByte = buffer->data[last_index];
    return BUFFER_OK;
}



