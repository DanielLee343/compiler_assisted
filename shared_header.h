#ifndef SHARED_H
#define SHARED_H

#define DEVICE_PATH "/dev/my_char_device"
#define BUFFER_SIZE 4096
#define NUM_SLOTS BUFFER_SIZE / 8
#define NUM_ADDRESSES 10 // for testing
#define COPIES 64
int syscall_shared_mem_start = 451;
int syscall_shared_mem_end = 452;
unsigned long *shared_mem = NULL;
int *buffer_size = NULL;

#endif // SHARED_H