#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int *a_mem;
int main()
{
    a_mem = malloc(8 * sizeof(int));
    for (int i = 0; i < 8; i++)
    {
        printf("i's address: %p\n", &i);
        a_mem[i] = 123;
        printf("runtime addr: %p\n", &a_mem[i]);
        usleep(125 * 1000);
    }
}