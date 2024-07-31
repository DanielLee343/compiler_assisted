#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

#define DEVICE_NAME "/dev/my_char_device"
#define DEVICE_SIZE 4096 // 4 KB device memory

// Define the syscall number (ensure this matches your custom syscall number)
#define SYS_read_device_memory 453
#define NUM_ADDRESSES 10
int syscall_shared_mem_start = 451;
int syscall_shared_mem_end = 452;
long shared_mem_start(pid_t pid)
{
    return syscall(syscall_shared_mem_start, pid);
}

long shared_mem_end()
{
    return syscall(syscall_shared_mem_end);
}

int main()
{
    int fd;
    char *mapped_mem;
    int *buffer_size;
    int pid = getpid();
    shared_mem_start(pid);

    // Open the device file
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Memory-map the device
    mapped_mem = mmap(NULL, DEVICE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_mem == MAP_FAILED)
    {
        perror("Failed to mmap device");
        close(fd);
        return EXIT_FAILURE;
    }
    buffer_size = (int *)((char *)mapped_mem + DEVICE_SIZE - sizeof(int));

    // Write data to the mapped memory
    // strcpy(mapped_mem, "Hello");
    unsigned long addresses_1[NUM_ADDRESSES] = {
        0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555,
        0x66666666, 0x77777777, 0x88888888, 0x99999999, 0x0};
    unsigned long addresses_2[NUM_ADDRESSES] = {
        0xAABBCCDD, 0xDDBBCCAA, 0x9988DDFF, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0};
    for (int i = 0; i < 6; i++)
    {
        if (i % 2 == 0)
            memcpy(mapped_mem, addresses_1, sizeof(addresses_1));
        else
            memcpy(mapped_mem, addresses_2, sizeof(addresses_2));
        *buffer_size = 10;
        usleep(500 * 1000);
        printf("buffer_size: %d\n", *buffer_size);
    }
    // syscall(SYS_read_device_memory);

    if (munmap(mapped_mem, DEVICE_SIZE) == -1)
    {
        perror("Failed to unmap memory");
    }

    close(fd);
    shared_mem_end();
    return EXIT_SUCCESS;
}