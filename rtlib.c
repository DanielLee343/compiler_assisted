#include <stdio.h>
#include <stdlib.h>

// extern int *buffer_size;
#include "shared_header.h"
// extern unsigned long *shared_mem;
void populate_shared_mem(void *addr, int size)
{
    // printf("captured addr: %p, size: %d\n", addr, size);
    if (buffer_index == NULL)
        return;
    shared_mem[*buffer_index] = (unsigned long)addr;
    (*buffer_index)++;
    // printf("buffer_index: %d\n", *buffer_index);
    if (*buffer_index >= NUM_SLOTS)
    {
        *buffer_index = 0;
        *buffer_is_ready = true;
    }
}
// int main()
// {
//     printf("after forking: %p\n", shared_mem);
//     return 0;
// }