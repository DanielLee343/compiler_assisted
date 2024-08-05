#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define DEVICE_PATH "/dev/shmchar"

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    char *shared_memory = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("Failed to map memory");
        close(fd);
        return -1;
    }

    printf("Writing to shared memory: Hello from User Space!\n");
    snprintf(shared_memory, 4096, "Hello from User Space!");

    // Simulate work
    sleep(10);  // Sleep to allow kernel to read

    munmap(shared_memory, 4096);
    close(fd);
    return 0;
}