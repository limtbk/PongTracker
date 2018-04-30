#include <stdio.h>
#include <stdlib.h>

#define CONV_SIZE 180
#define RB_SIZE (CONV_SIZE*2)
#define BUFF_SIZE 4096

int8_t a[RB_SIZE];
int8_t b[RB_SIZE];

int16_t conv_matr[CONV_SIZE][CONV_SIZE];

int16_t r[CONV_SIZE];

int16_t offset;

int16_t acorr(int8_t ai, int8_t bi)
{
    a[offset%RB_SIZE] = ai;
    b[offset%RB_SIZE] = bi;

    for (int i = 0; i < ; ++i)
    {
        /* code */
    }

    return 0;
}

void init()
{
    offset = 0;
}

int main()
{
    FILE *f;
    f = fopen("../testdata8bit9600.raw", "rb");
        if (f == NULL){
        printf("Error reading file f\n");
        exit (0);
    }

    int8_t buffer[BUFF_SIZE];

    size_t bytes_read = fread(buffer, 1, BUFF_SIZE, f);
    while (bytes_read) {
        for (int i = 0; i < bytes_read / 2; i++) {
            acorr(buffer[i*2+0], buffer[i*2+1]);
            printf("%02X ", buffer[i]);
        }
        bytes_read = fread(buffer, 1, BUFF_SIZE0, f);
    }

    fclose(f);
    return 0;
}