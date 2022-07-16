#include <stdio.h>
#include <stdlib.h>
#include "circularBuffer.h"

#define BUFFER_SIZE 4
struct sCircularBuffer
{
    int isEmpty;
    int isFull;
    int head;
    int tail;
    char data[BUFFER_SIZE];
};

sCircularBuffer *sCircularBuffer_alloc()
{
    sCircularBuffer *result = (sCircularBuffer *)malloc(sizeof(sCircularBuffer));
    return result;
}

void circular_buffer_init(sCircularBuffer *apArray)
{
    apArray->isEmpty = 1;
    apArray->isFull = 0;
    apArray->head = 0;
    apArray->tail = 0;
}

int circular_buffer_put(sCircularBuffer *apArray, char aValue)
{
    // LFL_SIGNATURE 0x04034b50
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

void circular_buffer_clear(sCircularBuffer *apArray)
{
    apArray->isEmpty = 1;
    apArray->isFull = 0;
    apArray->head = 0;
    apArray->tail = 0;
}

int circular_buffer_isEmpty(sCircularBuffer *apArray)
{
    return apArray->isEmpty;
}

int circular_buffer_isFull(sCircularBuffer *apArray)
{
    return apArray->isFull;
}
