#include <iostream>
#include <fstream>

#include "cpu_x86_32.h"

int main(int argc, const char* argv[]) {
  if(!cpu_x86_32::data_exist("data/blank")) {
    cpu_x86_32::dump_blank();
  }

  ifstream exe(argv[1], ios::in | ios::binary);
  exe.seekg(0x429);

  int i = 0;
  uint8_t buf[8] = { 0x00 };
  cpu_x86_32 cpu;
  cpu.load_blank();
  while(true) {
    exe.read((char*)buf + i++, 1);
    if(exe.tellg() > 0x463 || i > 7) {
      return 0;
    }
    if(cpu.load_change(buf, i)) {
      i = 0;
      cpu.print();
      cin.get();
    }
  }
}
