#include <string.h>

#include <iostream>
#include <boost/format.hpp>

#include "cpu_x86_32.h"

using namespace std;

int main(int argc, const char* argv[]) {
  uint64_t start = 0x0;  // 0xf475ff
  bool log = false;
  if(argc > 1 && string(argv[1]) == "-l") {
    log = true;
  } else if(argc > 1) {
    start = __builtin_bswap64(stoull(argv[1], nullptr, 16))
            >> ((16 - strlen(argv[1])) * 4);
  }

  uint8_t instr[8] = {0x00};
  for(uint64_t i = start; i <= ~(uint64_t)0; i++) {
    *(uint64_t*)instr = i;
    for(uint32_t j = 0; j < sizeof(instr); j++) {
      cout << boost::format("0x%02x") % (uint32_t)instr[j] << " ";
    }
    cout << endl;
    cpu_x86_32 cpu(instr, sizeof(instr), log);
    if(cpu.execute() && cpu.dump_change()) {
      cout << "SUCCESS!" << endl;
    }
  }
}
