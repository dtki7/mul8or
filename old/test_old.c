// #include <stdio.h>
// #include "x86.h"
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t low;
  uint64_t high;
  uint64_t xlow;
  uint64_t xhigh;
} uint256_t;

int main(int argc, const char* argv[])
{
  uint256_t out;
  uint256_t in;
  in.low = 0x56482233F;
  in.high = ~0;
  in.xlow = 0x33333333;
  in.xhigh = 0x44444444;

  //printf("%u\n", sizeof(long double));
  //asm volatile("mov $0x12345678, %%eax" : );
  asm volatile("vmovdqu %0, %%ymm1" : : "g"(in));
  //asm volatile("call %P0" : : "i"(tst));
  //asm volatile("mov %%cr0, %%eax" : );
  //asm volatile("mov %%eax, %0" : "=g" (out));

  //printf("%#llx %#llx %#llx %#llx\n", out.low, out.high, out.xlow, out.xhigh);

  //return 1;

  // char i = 'c';
  // unsigned char j = i;
  // cout << i << " - " << j << endl;
  // printf("%c - %c\n", i, j);

}
