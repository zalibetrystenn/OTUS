#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

typedef struct sCircularBuffer sCircularBuffer;

sCircularBuffer *sCircularBuffer_alloc();
void circular_buffer_init(sCircularBuffer *apArray);
int circular_buffer_put(sCircularBuffer *apArray, char aValue);
void circular_buffer_clear(sCircularBuffer *apArray);
int circular_buffer_isEmpty(sCircularBuffer *apArray);
int circular_buffer_isFull(sCircularBuffer *apArray);

#endif
