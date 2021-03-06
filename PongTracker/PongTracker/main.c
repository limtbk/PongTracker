//
//  main.c
//  PongTracker
//
//  Created by lim on 3/7/18.
//  Copyright © 2018 lim. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#define CONV_SIZE 180
//#define CONV_SIZE 10
#define RB_SIZE (CONV_SIZE*2)
#define BUFF_SIZE 4096

int8_t a[RB_SIZE];
int8_t b[RB_SIZE];

int16_t conv_matr[CONV_SIZE][CONV_SIZE];

int16_t r[CONV_SIZE];

int32_t offset;

void printConvMatr() {
    for (int i = 0; i < CONV_SIZE; i++) {
        for (int j = 0; j < CONV_SIZE; j++) {
            //            printf("%04X ", (uint16_t)conv_matr[j][i]);
        }
        printf(" %04X", (uint16_t)r[i]);
        //        printf("\n");
    }
    printf("\n");
}

int findMax()
{
    int res = 0;
    int max = 0;
    for (int i = 0; i < CONV_SIZE; i++) {
        int val = r[i];
        if (val<0) {
            val = -val;
        }
        if (max<val) {
            max = val;
            res = i;
        }
    }
    return res;
}

int16_t acorr(int8_t ai, int8_t bi)
{
    a[offset%RB_SIZE] = ai;
    b[offset%RB_SIZE] = bi;
    
    //    printf("%04X ", (uint32_t)offset);
    
    int i = offset - (RB_SIZE-CONV_SIZE)/2;
    for (int j = i - (CONV_SIZE/2); j < i + (CONV_SIZE/2); j++)
    {
        int k = i - (RB_SIZE-CONV_SIZE)/2;
        int l = j - k;
        int16_t m = a[(i+RB_SIZE)%RB_SIZE] * b[(j+RB_SIZE)%RB_SIZE];
        r[(l+CONV_SIZE)%CONV_SIZE]=r[(l+CONV_SIZE)%CONV_SIZE] - conv_matr[(k+CONV_SIZE)%CONV_SIZE][(l+CONV_SIZE)%CONV_SIZE] + m;
        conv_matr[(k+CONV_SIZE)%CONV_SIZE][(l+CONV_SIZE)%CONV_SIZE] = m;
        //        printf("%04X ", (uint16_t)j);
    }
    //    printConvMatr();
    int max = findMax();
    int maxv = r[max] > 0 ?: -r[max];
    printf("%04X %04X ", (uint16_t)max, (uint16_t)maxv);
    printf("\n");
    
    offset = (offset + 1);
    return 0;
}

void init()
{
    offset = 0;
}

void analyze()
{
    FILE *f;
    f = fopen("testdata8bit9600.raw", "rb");
    if (f == NULL){
        printf("Error reading file f\n");
        exit (0);
    }
    
    int8_t buffer[BUFF_SIZE];
    
    size_t bytes_read = fread(buffer, 1, BUFF_SIZE, f);
    while (bytes_read) {
        for (int i = 0; i < (bytes_read / 2); i++) {
            acorr(buffer[i*2+0], buffer[i*2+1]);
            //printf("%02X ", (uint8_t)buffer[i]);
        }
        bytes_read = fread(buffer, 1, BUFF_SIZE, f);
        //        printf("\n%04X ", (uint16_t)bytes_read);
    }
    
    fclose(f);
}

int main()
{
    analyze();
    return 0;
}


/*
 
 RB_SIZE 8
 CONV_SIZE 4
 
 offset = 5
 0  1  2  3  4  5  x  x
 0 00 10 20 .. .. .. .. ..
 1 01 11 21 31 .. .. .. ..
 2 02 12 22 32 .. .. .. ..
 3 .. 13 23 33 .. .. .. ..
 4 .. .. 24 34 .. .. .. ..
 5 .. .. .. 35 .. .. .. ..
 x 0x .. .. .. .. .. .. ..
 x 0x 1x .. .. .. .. .. ..
 
 offset = 6
 0  1  2  3  4  5  6  x
 0 .. 10 20 .. .. .. .. ..
 1 .. 11 21 31 .. .. .. ..
 2 .. 12 22 32 42 .. .. ..
 3 .. 13 23 33 43 .. .. ..
 4 .. .. 24 34 44 .. .. ..
 5 .. .. .. 35 45 .. .. ..
 6 .. .. .. .. 46 .. .. ..
 x .. 1x .. .. .. .. .. ..
 
 offset = 7
 i : [offset-(RB_SIZE-CONV_SIZE)/2-CONV_SIZE+1 .. offset-(RB_SIZE-CONV_SIZE)/2]  [7-5 .. 7-2]
 i = offset-(RB_SIZE-CONV_SIZE)/2 // i=7-(8-4)/2 = 5
 j = [i - (CONV_SIZE/2) .. i + (CONV_SIZE/2) - 1] // j = 3 -
 j : [i-2 .. i+1]
 K = i-(RB_SIZE-CONV_SIZE)/2
 L = j-K
 0  1  2. 3. 4. 5. 6  7  i             K
 0 .. .. 20 .. .. .. .. ..     20+31+42+53
 1 .. .. 21 31 .. .. .. ..     21+32+43+54
 2 .. .. 22 32 42 .. .. ..     22+33+44+55
 3 .. .. 23 33 43 53 .. ..   L 23+34+45+56
 4 .. .. .. 34 44 54 .. ..
 5 .. .. .. .. 45 55 .. ..
 6 .. .. .. .. .. 56 .. ..
 7 .. .. .. .. .. .. .. ..
 j
 
 offset = 8
 8  1  2  3  4  5  6  7
 8 .. .. .. .. .. .. .. ..
 1 .. .. .. 31 .. .. .. .. 31+42+53+64
 2 .. .. .. 32 42 .. .. .. 32+43+54+65
 3 .. .. .. 33 43 53 .. .. 33+44+55+66
 4 .. .. .. 34 44 54 64 .. 34+45+56+67
 5 .. .. .. .. 45 55 65 ..
 6 .. .. .. .. .. 56 66 ..
 7 .. .. .. .. .. .. 67 ..
 
 8  9  2  3  4  5  6  7
 8 .. .. .. .. .. .. .. 78
 9 .. .. .. .. .. .. .. ..
 2 .. .. .. .. 42 .. .. .. 42+53+64+75
 3 .. .. .. .. 43 53 .. .. 43+54+65+76
 4 .. .. .. .. 44 54 64 .. 44+55+66+77
 5 .. .. .. .. 45 55 65 75 45+56+67+78
 6 .. .. .. .. .. 56 66 76
 7 .. .. .. .. .. .. 67 77
 
 8  9  A  3  4  5  6  7
 8 88 .. .. .. .. .. .. 78
 9 89 .. .. .. .. .. .. ..
 A .. .. .. .. .. .. .. ..
 3 .. .. .. .. .. 53 .. .. 53+64+75+86
 4 .. .. .. .. .. 54 64 .. 54+65+76+87
 5 .. .. .. .. .. 55 65 75 55+66+77+88
 6 86 .. .. .. .. 56 66 76 56+67+78+89
 7 87 .. .. .. .. .. 67 77
 
 */

