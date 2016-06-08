#include <stdio.h>

int main() {
  int i = 0x77;
  i = 0x33;
  i += 0x55;
  int j = 0x99;
  j -= 0x44;
  i = i - 0x11 - 0x11 + j + 0x11;
  j = i + 0x22;
  printf("%i\n", i);
  return i;
}
