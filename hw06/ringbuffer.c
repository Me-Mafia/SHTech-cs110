#include "ringbuffer.h"
//#include <stdio.h>

ringbuffer_t *ringbuffer_create(size_t size) {
    ringbuffer_t *ringbuffer = malloc(sizeof(ringbuffer_t));
    if (ringbuffer == NULL) {
        return NULL;
    }
    ringbuffer->size = size;
    ringbuffer->head = 0;
    ringbuffer->tail = 0;
    ringbuffer->count = 0;
    ringbuffer->buffer = malloc(size * sizeof(threadpool_task_t));
    if (ringbuffer->buffer == NULL) {
        free(ringbuffer);
        return NULL;
    }
    return ringbuffer;
}

void ringbuffer_destroy(ringbuffer_t *ringbuffer) {
    free(ringbuffer->buffer);
    free(ringbuffer);
}

bool ringbuffer_is_empty(ringbuffer_t *ringbuffer) {
    return ringbuffer->count == 0;
}

bool ringbuffer_is_full(ringbuffer_t *ringbuffer) {
    return ringbuffer->count == ringbuffer->size;
}

bool ringbuffer_push(ringbuffer_t *ringbuffer, threadpool_task_t value) {
    // TODO: implement
    if (!ringbuffer_is_full(ringbuffer)) 
    { 
        ringbuffer->buffer[ringbuffer->tail] = value;
        ringbuffer->tail++;
        ringbuffer->tail %= ringbuffer->size;
        ringbuffer->count++;  
        //printf("push: %d \t", (int)ringbuffer->count);
        return true;
    }
    //printf("\n");
    //printf("push failed! \n");
    return false;
}

bool ringbuffer_pop(ringbuffer_t *ringbuffer, threadpool_task_t *value) {
    if (!ringbuffer_is_empty(ringbuffer))
    {
        *value = ringbuffer->buffer[ringbuffer->head];
        ringbuffer->head++;
        ringbuffer->head %= ringbuffer->size;
        ringbuffer->count--; 
        //printf("pop: %d \t", (int)ringbuffer->count);
        return true;
    }
    //printf("\n");
    //printf("pop failed! \n");
    return false;
}
