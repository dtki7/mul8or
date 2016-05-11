#include <sys/user.h>

#include <map>
#include <vector>

#define ST_SPACE_COUNT 20
#define ST_SPACE2_COUNT 32
#define XMM_SPACE_COUNT 32
#define PADDING_COUNT 56
#define U_COMM_COUNT 32
#define U_DEBUGREG_COUNT 8

enum user_reg_names_x86 {
  EBX, ECX, EDX, ESI, EDI, EBP, EAX, DS, ES, FS, GS, ORIG_EAX, EIP, CS, EFLAGS,
  ESP, SS
};

using namespace std;

class register_x86 {

public:
  register_x86(user_regs_struct user_regs, user_fpregs_struct user_fpregs,
               user_fpxregs_struct user_fpxregs);
  void print_all();
  static void diff_same(vector<register_x86> regs);
  static void diff_change(vector<register_x86> regs, bool all);
  static string get_user_reg_name(enum user_reg_names_x86 name);
protected:
  map<string, unsigned long int> regs;
};
