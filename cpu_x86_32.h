#include <sys/ptrace.h>

#include "register_x86.h"

#define INSTR_POS 0x3f0
#define BRACE_OPEN 0x80483f0
#define BRACE_CLOSE 0x80483f8

#define RESUME_FLAG 16

using namespace std;

class cpu_x86_32 {

public:
  cpu_x86_32(uint8_t instr[] = { 0x0 }, int len = 0, bool log = false);
  ~cpu_x86_32();
  bool execute();

  bool dump_change();
  bool dump_full(string prefix = "");
  bool load_change(uint8_t* instr, int len);
  bool load_full(uint8_t* instr, int len);

  static bool dump_blank();
  bool load_blank();

  void print();

  static bool data_exist(string path);
protected:
  uint8_t* instr;
  int len;
  bool log;
  int pid;

  vector<register_x86> regs;
  vector<map<uint32_t, uint8_t>> ram;

  uint32_t user_reg(enum __ptrace_request mode,
                    enum user_reg_names_x86 reg,
                    uint32_t data);
  bool build_exe();
  bool read_regs();
  bool read_ram();
  string diff_ram();

  string get_instr();
};
