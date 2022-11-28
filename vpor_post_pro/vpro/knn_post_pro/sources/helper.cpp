#include "helper.h"
#include "simulator/helper/debugHelper.h"

void printProgress(double progress, int size){
    // progressbar
    if (progress < 0) {
        printf(RED);
        progress += 100;
    }
    printf(" [");
    for(float i = 0 ; i <= 100; i += 100./size){
        if (progress > i)
            printf("#");
        else
            printf(" ");
    }
    printf("]");
    printf(RESET_COLOR);
}

//assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
}
