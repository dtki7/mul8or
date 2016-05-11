#include "x86.h"

#include <climits>

using namespace std;

cpu_x86_32::cpu_x86_32(unsigned char* instr)  : instr(instr) {}

bool cpu_x86_32::execute() {
  future<bool> ret = async(&cpu_x86_32::thread_exec, this);
  return ret.get();
}

bool cpu_x86_32::thread_exec() {
  ((void(*)(void))(instr))();
  read_regs();
  read_ram();
  return true;
}

void cpu_x86_32::read_regs() {
  registers.insert(registers.end(), new uint256_t[REG_COUNT]);
  for(int i = 0; i < REG_COUNT; i++) {
    uint256_t out;
    switch(i) {
      case EAX:
        asm volatile("mov %%eax, %0" : "=g" (out.low)); break;
      case EBX:
        asm volatile("mov %%ebx, %0" : "=g" (out.low)); break;
      case ECX:
        asm volatile("mov %%ecx, %0" : "=g" (out.low)); break;
      case EDX:
        asm volatile("mov %%edx, %0" : "=g" (out.low)); break;
      case CS:
        asm volatile("mov %%cs, %0" : "=g" (out.low)); break;
      case DS:
        asm volatile("mov %%ds, %0" : "=g" (out.low)); break;
      case ES:
        asm volatile("mov %%es, %0" : "=g" (out.low)); break;
      case FS:
        asm volatile("mov %%fs, %0" : "=g" (out.low)); break;
      case GS:
        asm volatile("mov %%gs, %0" : "=g" (out.low)); break;
      case SS:
        asm volatile("mov %%ss, %0" : "=g" (out.low)); break;
      case ESI:
        asm volatile("mov %%esi, %0" : "=g" (out.low)); break;
      case EDI:
        asm volatile("mov %%edi, %0" : "=g" (out.low)); break;
      case ESP:
        asm volatile("mov %%esp, %0" : "=g" (out.low)); break;
      case EBP:
        asm volatile("mov %%ebp, %0" : "=g" (out.low)); break;
      case EIP:
        out.low = getEIP();
        break;
      case FLAGS:
        asm volatile("pushf");
        asm volatile("popl %0" : "=g" (out));
      case ST0:
      case ST1:
      case ST2:
      case ST3:
      case ST4:
      case ST5:
      case ST6:
      case ST7:
        asm volatile("fstpt %0" : "=g" (out)); break;  // "t" for "m80fp"
      case MM0:
        asm volatile("movq %%mm0, %0" : "=g" (out)); break;
      case MM1:
        asm volatile("movq %%mm1, %0" : "=g" (out)); break;
      case MM2:
        asm volatile("movq %%mm2, %0" : "=g" (out)); break;
      case MM3:
        asm volatile("movq %%mm3, %0" : "=g" (out)); break;
      case MM4:
        asm volatile("movq %%mm4, %0" : "=g" (out)); break;
      case MM5:
        asm volatile("movq %%mm5, %0" : "=g" (out)); break;
      case MM6:
        asm volatile("movq %%mm6, %0" : "=g" (out)); break;
      case MM7:
        asm volatile("movq %%mm7, %0" : "=g" (out)); break;
      case YMM0:
        asm volatile("vmovdqu %%ymm0, %0" : "=g" (out)); break;
      case YMM1:
        asm volatile("vmovdqu %%ymm1, %0" : "=g" (out)); break;
      case YMM2:
        asm volatile("vmovdqu %%ymm2, %0" : "=g" (out)); break;
      case YMM3:
        asm volatile("vmovdqu %%ymm3, %0" : "=g" (out)); break;
      case YMM4:
        asm volatile("vmovdqu %%ymm4, %0" : "=g" (out)); break;
      case YMM5:
        asm volatile("vmovdqu %%ymm5, %0" : "=g" (out)); break;
      case YMM6:
        asm volatile("vmovdqu %%ymm6, %0" : "=g" (out)); break;
      case YMM7:
        asm volatile("vmovdqu %%ymm7, %0" : "=g" (out)); break;
    }
    registers.back()[i] = out;
  }
}

int cpu_x86_32::getEIP()
{
    return (size_t)__builtin_return_address(0);
}

void cpu_x86_32::read_ram() {
  ram.insert(ram.end(), map<unsigned char*, unsigned char>());
  for(unsigned char* i = 0; i <= (unsigned char*) ~0; i++) {
    future<unsigned char> ret = async(&cpu_x86_32::read_address, this, i);
    unsigned char val = ret.get();
    if(val) {
      ram.back().insert(make_pair((unsigned char*)i, val));
    }
  }
}

unsigned char cpu_x86_32::read_address(unsigned char* ptr) {
  return *ptr;
}
