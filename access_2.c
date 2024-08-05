#include "shared_header.h"

extern void populate_shared_mem(void *addr, int size);

void launch_bench();
int main(int argc, char **argv)
{
    int state;
    int fd;

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
    buffer_index = (int *)((char *)shared_mem + BUFFER_SIZE - sizeof(int));
    buffer_is_ready = (bool *)((char *)shared_mem + BUFFER_SIZE - sizeof(int) - sizeof(bool));
    *buffer_index = 0;
    *buffer_is_ready = false;

    int pid = getpid();
    printf("parent pid: %d\n", pid);
    shared_mem_start(pid);
    // execvp(argv[1], &argv[1]);
    launch_bench(); // benchmark entry

    shared_mem_end();
    munmap(shared_mem, BUFFER_SIZE); // Unmap before exiting
    close(fd);
    return 0;
}

void launch_bench()
{
    int *a_mem = malloc(8 * sizeof(int));
    for (int i = 0; i < 100; i++)
    {
        // printf("i's address: %p\n", &i);
        // populate_shared_mem(&a_mem[i], sizeof(int));
        // printf("buffer_index: %d\n", *buffer_index);
        a_mem[i] = i * 123;
        // printf("runtime addr: %p\n", &a_mem[i]);
        usleep(50 * 1000);
    }
}