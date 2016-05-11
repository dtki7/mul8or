#include <stdint.h>

#include <list>

using namespace std;

typedef struct {
  uint64_t high;
  uint16_t low;
} fpu80_t;

class cpu_x86_32 {

public:
  cpu_x86_32(unsigned char* instr);
  bool execute();
protected:
  // instructions
  unsigned char* instr;
  // general registers
  list<uint32_t> eax;
  list<uint32_t> ebx;
  list<uint32_t> ecx;
  list<uint32_t> edx;
  // segment registers
  list<uint32_t> cs;
  list<uint32_t> ds;
  list<uint32_t> es;
  list<uint32_t> fs;
  list<uint32_t> gs;
  list<uint32_t> ss;
  // index and pointers
  list<uint32_t> esi;
  list<uint32_t> edi;
  list<uint32_t> esp;
  list<uint32_t> ebp;
  list<uint32_t> eip;
  // FPU registers
  list<fpu80_t> st0;
  list<fpu80_t> st1;
  list<fpu80_t> st2;
  list<fpu80_t> st3;
  list<fpu80_t> st4;
  list<fpu80_t> st5;
  list<fpu80_t> st6;
  list<fpu80_t> st7;
  // RAM
  list<char*> ram;
};
