#include <sys/user.h>
#include <linux/user.h>

#include <map>
#include <vector>

#define INSTR_POS 0x3f0
#define BRACE_OPEN 0x80483f0
#define BRACE_CLOSE 0x80483f7

#define REG_COUNT 40

// #define EBX 0x00
// #define ECX 0x04
// #define EDX 0x08
// #define ESI 0x0c
// #define EDI
// #define EBP
// #define EAX 0x18
// #define EIP 0x30

using namespace std;

typedef struct {
  uint64_t low = 0;
  uint64_t high = 0;
  uint64_t xlow = 0;
  uint64_t xhigh = 0;
} uint256_t;

// enum user_reg_names_x86 {
//   // General Purpose register
//   EAX, EBX, ECX, EDX,
//   // Segment register (size: 16 Bit)
//   CS, DS, ES, FS, GS, SS,
//   // Index and Pointers
//   ESI, EDI, ESP, EBP, EIP,
//   // Flag Register (size: 32 Bit; 22 Bit used)
//   FLAGS,
//   // x87 register (floating point) (size: 80 Bit)
//   ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,
//   // MMX (Multi Media Extension) register (size: 64 Bit)
//   MM0, MM1, MM2, MM3, MM4, MM5, MM6, MM7,
//   // YMM (Advanced Vector Extensions) register (size: XMM register + 128 Bit = 256 Bit) (x86-64 => + YMM8 - YMM15)
//   YMM0, YMM1, YMM2, YMM3, YMM4, YMM5, YMM6, YMM7
//   // // Control register
//   // CR0, CR1, CR2, CR3, CR4, CR5, CR6, CR7,
//   // // Debug register
//   // DR0, DR1, DR2, DR3, DR4, DR5, DR6, DR7,
//   // // Bound register
//   // BND0, BND1, BND2, BND3
//
//   // CR, DR => Kernel Module!?
//   // 4 BND register ? (size: 128 Bit) (Check?, BTE?)
// };

class cpu_x86_32 {

public:
  cpu_x86_32(char* instr, int len, bool log = false);
  bool execute();
protected:
  char* instr;
  int len;

  vector<user_regs_struct> regists; // vector<uint256_t*> regs;  // first no space efficiency
  vector<map<uint32_t, uint8_t>> ram;
private:
  bool log;

  bool build_exe();
  uint32_t getReg(int pid, int reg);
  bool read_regs(int pid);
  bool read_ram(int pid);
};
