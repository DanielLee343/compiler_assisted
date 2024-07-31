#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <err.h>
#include <sys/wait.h>
#include <time.h>
#include "shared_header.h"
extern int *buffer_size;
// extern unsigned long *shared_mem;
long shared_mem_start(pid_t pid)
{
    return syscall(syscall_shared_mem_start, pid);
}

long shared_mem_end()
{
    return syscall(syscall_shared_mem_end);
}

int main(int argc, char **argv)
{
    pid_t pid;
    int state;
    int fd;
    int parent_pid = getpid();
    printf("parent pid: %d\n", parent_pid);

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return EXIT_FAILURE;
    }

    shared_mem = (unsigned long *)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }
    buffer_size = (int *)((char *)shared_mem + BUFFER_SIZE - sizeof(int));

    shared_mem_start(parent_pid);
    // execvp(argv[1], &argv[1]);
    {
        int *a_mem = malloc(8 * sizeof(int));
        for (int i = 0; i < 8; i++)
        {
            a_mem[i] = i * 123;
            printf("runtime addr: %p\n", &a_mem[i]);
            usleep(250 * 1000);
        }
    }

    // simulate some workload
    munmap(shared_mem, BUFFER_SIZE); // Unmap before exiting
    close(fd);
    shared_mem_end();
    return EXIT_SUCCESS;
    return 0;
}
