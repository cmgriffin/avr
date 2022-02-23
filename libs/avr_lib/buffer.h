/**
 * @file buffer.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-02-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#define BUFFER_EMPTY_VAL 0

/**
 * @brief
 *
 */
typedef struct
{
    uint8_t *valuesptr;
    uint8_t head, tail, num_entries, size;

} buffer_t;

/**
 * @brief abstracted constructor for buffer that doesn't require malloc
 *
 */
#define BUFFER_CREATE(SIZE, VALUES_PTR)                                               \
    {                                                                                 \
        .valuesptr = VALUES_PTR, .head = 0, .tail = 0, .num_entries = 0, .size = SIZE \
    }

/**
 * @brief Check if the buffer is empty
 *
 * @param b pointer to the buffer struct
 * @return true empty
 * @return false not empty
 */
static inline bool BUFFER_empty(buffer_t *b)
{
    bool v;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { v = (b->num_entries == 0); }
    return v;
}

/**
 * @brief
 *
 * @param b
 * @return true
 * @return false
 */
static inline bool BUFFER_full(buffer_t *b)
{
    bool v;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { v = (b->num_entries == b->size); }
    return v;
}

/**
 * @brief
 *
 * @param b
 * @param value
 * @return true
 * @return false
 */
bool BUFFER_enqueue(buffer_t *b, uint8_t value);

/**
 * @brief
 *
 * @param b
 * @return uint8_t
 */
uint8_t BUFFER_dequeue(buffer_t *b);

/**
 * @brief
 *
 */
uint8_t BUFFER_peek(buffer_t *b, uint8_t offset);

#endif