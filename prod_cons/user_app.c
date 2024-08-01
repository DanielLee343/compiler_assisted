#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#define DEVICE_PATH "/dev/chardev"
#define BUFFER_SIZE 4096

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    char *object = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (object == MAP_FAILED) {
        perror("Mapping failed");
        close(fd);
        return -1;
    }

    printf("Reading from kernel space: \"%s\"\n", object);
    char user_input[1024];
    printf("Enter data to write to kernel space: ");
    fgets(user_input, sizeof(user_input), stdin);
    snprintf(object, BUFFER_SIZE, "%s", user_input);

    if (munmap(object, BUFFER_SIZE) == -1) {
        perror("Error un-mapping the file");
    }

    close(fd);
    return 0;
}