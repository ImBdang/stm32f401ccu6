#include "bdang_datatructure.h"


void init_ring_buffer(Ring_buffer* ring_buffer, uint8_t* buffer, size_t size){
    if (size < 2) return;
    ring_buffer->buffer = buffer;
    ring_buffer->size = size;
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
}

uint8_t enqueue_ring_buffer(Ring_buffer* ring_buffer, uint8_t data){
    if (ring_isFull(ring_buffer) != 0)
        return;
    ring_buffer->buffer[ring_buffer->head] = data;
    ring_buffer->head = (ring_buffer->head + 1) % ring_buffer->size;
}

uint8_t dequeue_ring_buffer(Ring_buffer* ring_buffer, uint8_t* data_return){
    if (ring_isEmpty(ring_buffer) == 1)
        return 0;
    *data_return = ring_buffer->buffer[ring_buffer->tail];
    ring_buffer->tail = (ring_buffer->tail + 1) % ring_buffer->size;
    return 1;
}

uint8_t ring_isFull(Ring_buffer *ring){
    return ((ring->head + 1) % ring->size) == ring->tail;
}

uint8_t ring_isEmpty(Ring_buffer *ring){
    return ring->head == ring->tail;
}