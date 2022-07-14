#include <stdio.h>
#include "circularBuffer.h"

void circular_buffer_init(sCircularBuffer *apArray)
{
    apArray->readIndex = 0;
    apArray->writeIndex = 0;
    apArray->isEmpty = 1;
    apArray->isFull = 0;
    apArray->head = 0;
    apArray->tail = 0;
}

int circular_buffer_put(sCircularBuffer *apArray, char aValue)
{
    // LFL_SIGNATURE 0x04034b50
    printf("apArray->tail = %d\n", apArray->tail);
    if (apArray->tail == 0)
    {
        if (aValue == 0x50)
        {
            apArray->data[apArray->tail++] = aValue;
            return 0;
        }
    }

    if (apArray->tail == 1)
    {
        if (aValue == 0x4b)
        {
            apArray->data[apArray->tail++] = aValue;
            return 0;
        }
        else
        {
            apArray->tail = 0;
            return 0;
        }
    }

    if (apArray->tail == 2)
    {
        if (aValue == 0x03)
        {
            apArray->data[apArray->tail++] = aValue;
            return 0;
        }
        else
        {
            apArray->tail = 0;
            return 0;
        }
    }

    if (apArray->tail == 3)
    {
        if (aValue == 0x04)
        {
            apArray->data[apArray->tail++] = aValue;
            return 1;
        }
        else
        {
            apArray->tail = 0;
            return 0;
        }
    }

    return 0;
}

char circular_buffer_get(sCircularBuffer *apArray)
{
    if (apArray->isEmpty)
        return -1;

    apArray->isFull = 0;

    if (apArray->readIndex >= BUFFER_SIZE)
        apArray->readIndex = 0;

    int res = apArray->data[apArray->readIndex++];

    if (apArray->readIndex == apArray->writeIndex)
        apArray->isEmpty = 1;

    return res;
}

void circular_buffer_clear(sCircularBuffer *apArray)
{
    apArray->isEmpty = 1;
    apArray->isFull = 0;
    apArray->writeIndex = 0;
    apArray->readIndex = 0;
    apArray->head = 0;
}

int circular_buffer_isEmpty(sCircularBuffer *apArray)
{
    return apArray->isEmpty;
}

int circular_buffer_isFull(sCircularBuffer *apArray)
{
    return apArray->isFull;
}

// int circular_buffer_is_equal_signature(sCircularBuffer *apArray)
// {
//     // LFL_SIGNATURE 0x04034b50
//     // 0x50 == circular_buffer_get(buff) && 0x4b == circular_buffer_get(buff) && 0x03 == circular_buffer_get(buff) && 0x04 == circular_buffer_get(buff)
//     // if (apArray[])
// }