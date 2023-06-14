#include "buffer.h"
#include <util/atomic.h>

bool BUFFER_enqueue(buffer_t *b, uint8_t value)
{
    if (BUFFER_full(b))
    {
        return false;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        b->valuesptr[b->tail] = value;
        b->num_entries++;
        b->tail = (b->tail + 1) % b->size;
    }
    return true;
}

uint8_t BUFFER_dequeue(buffer_t *b)
{
    if (BUFFER_empty(b))
    {
        return BUFFER_EMPTY_VAL;
    }
    uint8_t value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        value = b->valuesptr[b->head];
        b->num_entries--;
        b->head = (b->head + 1) % b->size;
    }
    return value;
}
uint8_t BUFFER_peek(buffer_t *b, uint8_t offset)
{
    if (b->num_entries < offset)
    {
        return BUFFER_EMPTY_VAL;
    }
    uint8_t index = (b->head + offset) % b->size;
    return b->valuesptr[index];
}
