#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char **argv)
{
  volatile uint64_t modified;
  uint8_t buffer[16];

  modified = 0;
  strcpy(buffer, argv[1]);

  if(modified == 0x656b616d68636574) {
      printf("You made it. Contact us at info@techmaker.ua ;)\n");
  } else {
      printf("Try again, you got 0x%16x\n", modified);
  }
}
