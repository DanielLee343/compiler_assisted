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
    if (shared_mem_start(parent_pid) < 0)
    {
        perror("shared_mem_start here");
        exit(EXIT_FAILURE);
    }
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0)
    {
        perror("open");
        shared_mem_end();
        return EXIT_FAILURE;
    }

    shared_mem = (unsigned long *)mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        shared_mem_end();
        return EXIT_FAILURE;
    }
    buffer_size = (int *)((char *)shared_mem + BUFFER_SIZE - sizeof(int));
    printf("before forking: %p\n", shared_mem);
    pid = fork();
    if (pid == 0)
    {
        // Child process
        int inner_pid = getpid();
        printf("child pid: %d\n", inner_pid);
        // usleep(10*1000);
        printf("before execvp: %p\n", shared_mem);
        execvp(argv[1], &argv[1]);
        return EXIT_SUCCESS;
    }
    else if (pid > 0)
    {
        waitpid(pid, &state, 0);

        munmap(shared_mem, BUFFER_SIZE); // Unmap before exiting
        close(fd);
        shared_mem_end();
        return EXIT_SUCCESS;
    }
    else
    {
        perror("fork");
        return EXIT_FAILURE;
    }
    return 0;
}
