#include "cpu_x86_32.h"

#include <iostream>
#include <boost/format.hpp>

using namespace std;

int main(int argc, const char* argv[]) {
  bool log = false;
  if(argc > 1 && string(argv[1]) == "-l") {
    log = true;
  }

  uint8_t instr[8] = {0x00};
  for(uint64_t i = 0; i <= ~(uint64_t)0; i++) {
    *(uint64_t*)instr = i;
    cout << boost::format("0x%016lx:") % __builtin_bswap64(*(uint64_t*)instr)
         << endl;
    cpu_x86_32 cpu(instr, sizeof(instr), log);
    if(cpu.execute() && cpu.save()) {
      cpu.print();
    }
  }
}
