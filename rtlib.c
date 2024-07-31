#include <stdio.h>
#include <stdlib.h>
// extern unsigned long *shared_mem;
// extern int *buffer_size;
#include "shared_header.h"
extern int* buffer_size;
// extern unsigned long *shared_mem;
void populate_shared_mem(void *addr, int size) {
    printf("captured addr: %p, size: %d\n", addr, size);

    // if (*buffer_size != NUM_SLOTS){
    //     *buffer_size++;
    // } else {
    //     *buffer_size = 0;
    // }
    // int position = *buffer_size % NUM_SLOTS;
    printf("after forking: %p\n", shared_mem);
    // printf("buffer_size: %d\n", *buffer_size);
    // shared_mem[position] = (unsigned long)addr;
    // *buffer_size++;
}
// int main()
// {
//     printf("after forking: %p\n", shared_mem);
//     return 0;
// }