#include <iostream>
#include <fstream>

#include "cpu_x86_32.h"

int main(int argc, const char* argv[]) {
  ifstream exe(argv[1], ios::in | ios::binary);
  exe.seekg(0x41d);

  int i = 0;
  uint8_t buf[7] = { 0x0 };
  cpu_x86_32 cpu;
  while(true) {
    exe.read((char*)buf + i++, 1);
    if(cpu.load(buf, i) || i > 6) {
      i = 0;
      cpu.print();
      cin.get();
    }
  }
}
