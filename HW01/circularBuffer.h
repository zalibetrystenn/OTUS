#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#define BUFFER_SIZE 4
typedef struct
{
    int isEmpty;
    int isFull;
    int head;
    int tail;
    char data[BUFFER_SIZE];
} sCircularBuffer;

void circular_buffer_init(sCircularBuffer *apArray);
int circular_buffer_put(sCircularBuffer *apArray, char aValue);
void circular_buffer_clear(sCircularBuffer *apArray);
int circular_buffer_isEmpty(sCircularBuffer *apArray);
int circular_buffer_isFull(sCircularBuffer *apArray);

#endif
