#include <sys/wait.h>
#include <sys/stat.h>

#include <fstream>
#include <sstream>
#include <boost/format.hpp>

#include "cpu_x86_32.h"

using namespace std;

cpu_x86_32::cpu_x86_32(uint8_t instr[], int len, bool log)
                      : instr(instr), len(len), log(log) {}

cpu_x86_32::~cpu_x86_32() {
  ptrace(PTRACE_KILL, pid, nullptr, nullptr);
  cout << endl;
}

bool cpu_x86_32::execute() {
  // Build executable: insert instr into template
  if(!build_exe()) {
    cerr << "ERROR: Build of file \"execute\" failed!" << endl;
    return false;
  }

  // Create new process and load the executable to invest
  int wstatus;
  pid = fork();
  if(pid == 0) {
    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    if (execlp("./execute", "./execute") < 0) {
      cerr << "ERROR: Execution of file \"execute\" failed!" << endl;
      return false;
    }
  }
  waitpid(pid, &wstatus, 0);
  if(wstatus >> 8 != SIGTRAP) {
    cerr << "ERROR: Failed to catch exec!" << endl;
    return false;
  }
  ptrace(PTRACE_SETOPTIONS, pid, nullptr, PTRACE_O_EXITKILL);

  // Get process information before and after the instruction
  for(int i = 0; i < 2; i++) {
    uint32_t brace = 0;
    switch(i) {
      case 0: brace = BRACE_OPEN; break;
      case 1: brace = BRACE_CLOSE; break;
    }
    while(true) {
      uint32_t eip = user_reg(PTRACE_PEEKUSER, EIP, 0);
      if(eip == brace) {
        break;
      } else if(eip == 0) {
        return false;
      } else if(wstatus >> 8 == SIGSEGV) {
        cout << hex << "Catched Segmentation Fault in address "
             << eip << "!" << endl;
        len = eip - BRACE_OPEN;
        if(len <= 0) {
          cerr << "ERROR: Length of instruction is 0!" << endl;
          return false;
        }
        if(!user_reg(PTRACE_POKEUSER,
                     EFLAGS,
                     user_reg(PTRACE_PEEKUSER, EFLAGS, 0)
                        & ~(1 << RESUME_FLAG))) {
          cerr << "Failed to change EFLAGS register!" << endl;
          return false;
        }
        break;
      }
      ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr);
      waitpid(pid, &wstatus, 0);
    }
    if(!(read_regs() && read_ram())) {
      cerr << "ERROR: Failed to read memory of process " << pid << "!"<< endl;
      return false;
    }
  }
  ptrace(PTRACE_KILL, pid, nullptr, nullptr);
  return true;
}

bool cpu_x86_32::dump() {
  string path = "data/" + get_instr();

  struct stat buf;
  if (stat(path.c_str(), &buf) != -1)
  {
    cout << "Data alread exist!" << endl;
    return false;
  }

  ofstream file(path, ios::out);
  if(!file.is_open()) {
    cerr << "ERROR: Failed to create file data/" << path << "!" << endl;
    return false;
  }
  for(register_x86 reg: regs) {
    reg.dump(file);
  }
  for(map<uint32_t, uint8_t> mp: ram) {
    file << "##" << endl;
    for(pair<uint32_t, uint8_t> par: mp) {
      file << par.first << ":" << par.second << endl;
    }
  }
  return true;
}

bool cpu_x86_32::load(uint8_t* instr, int len) {
  this->instr = instr;
  this->len = len;

  string path = "data/" + get_instr();
  struct stat buf;
  if (stat(path.c_str(), &buf) == -1)
  {
    if(log) {
      cout << "Data " << path << " does not exist!" << endl;
    }
    return false;
  }

  string line;
  ifstream file(path, ios::in);
  while(getline(file, line, ':')) {
    if(line == "#") {
      register_x86 reg;
      regs.push_back(reg);
      regs.back().load(file);
    }
  }
  return true;
}

void cpu_x86_32::print() {
  register_x86::diff_change(regs, false);
  diff_ram();
}

uint32_t cpu_x86_32::user_reg(enum __ptrace_request mode,
                              enum user_reg_names_x86 reg,
                              uint32_t data) {
  if(reg > SS) {
    cerr << "ERROR: Register " << register_x86::get_user_reg_name(reg)
         << " is not a user register!" << endl;
         return 0;
  }
  if(mode != PTRACE_PEEKUSER && mode != PTRACE_POKEUSER) {
    cerr << "ERROR: " << mode << "is no user regisert request!" << endl;
  }
  long val = ptrace(mode, pid, reg * sizeof(int), data);
  if(val == -1 && errno) {
    cerr << "ERROR: Failed to execute action " << mode << " on register "
         << register_x86::get_user_reg_name(reg) << "!" << endl;
    return 0;
  }
  if(mode == PTRACE_PEEKUSER) {
    return val;
  } else {  // PTRACE_POKEUSER
    return 1;  // true
  }
}

bool cpu_x86_32::build_exe() {
  ifstream templ;
  ofstream exe;
  templ.open("./template", ios::in | ios::ate | ios::binary);
  exe.open("./execute", ios::out | ios::trunc | ios::binary);
  if(!templ.is_open()) {
    cerr << "ERROR: Failed to open ./template!" << endl;
    return false;
  }
  if(!exe.is_open()) {
    cerr << "ERROR: Failed to open ./execute!" << endl;
    return false;
  }

  int size = templ.tellg();
  templ.seekg(0);

  char lead[INSTR_POS];
  templ.read(lead, sizeof(lead));
  exe.write(lead, sizeof(lead));

  templ.seekg((int)templ.tellg() + len);
  exe.write((char*)instr, len);

  char trail[size - templ.tellg()];
  templ.read(trail, sizeof(trail));
  exe.write(trail, sizeof(trail));

  templ.close();
  exe.close();

  if(chmod("./execute", S_IRWXU
                      | S_IRGRP
                      | S_IXGRP
                      | S_IROTH
                      | S_IXOTH) == -1) {  // Change file permissons to 755
    cerr << "ERROR: Failed to change file permissions!" << endl;
    return false;
  }
  return true;
}

bool cpu_x86_32::read_regs() {
  long ret;

  user_regs_struct user_regs;
  ret = ptrace(PTRACE_GETREGS, pid, nullptr, &user_regs);
  if(ret == -1 && errno) {
    cerr << "ERROR: Failed to read user register!" << endl;
    return false;
  }

  user_fpregs_struct user_fpregs;
  ret = ptrace(PTRACE_GETFPREGS, pid, nullptr, &user_fpregs);
  if(ret == -1 && errno) {
    cerr << "ERROR: Failed to read user floating point register!" << endl;
    return false;
  }

  user_fpxregs_struct user_fpxregs;
  ret = ptrace(PTRACE_GETFPXREGS, pid, nullptr, &user_fpxregs);
  if(ret == -1 && errno) {
    cerr << "ERROR: Failed to read user extended floating point register!"
         << endl;
         return false;
  }

  register_x86 reg(user_regs, user_fpregs, user_fpxregs);
  regs.push_back(reg);

  return true;
}

bool cpu_x86_32::read_ram() {
  ifstream maps_f;
  maps_f.open("/proc/" + to_string(pid) + "/maps", ios::in);
  if(!maps_f.is_open()) {
    cerr << "ERROR: Failed to open \"maps\"-File \" of process "
         << pid << "!" << endl;
    return false;
  }
  vector<pair<uint32_t, uint32_t>> limits_l;
  for(string line; getline(maps_f, line); ) {
    limits_l.push_back(make_pair(stoul(line.substr(0, 8), nullptr, 16),
                                 stoul(line.substr(9, 8), nullptr, 16)));
  }
  maps_f.close();

  map<uint32_t, uint8_t> map_m;
  for(pair<uint32_t, uint32_t> limits : limits_l) {
    uint32_t addr = limits.first;
    while(addr < limits.second) {
      long val = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr);  // change when kernel level
      if(val != -1 && !errno) {
        map_m.insert(make_pair(addr, val));
      } else if(log) {
        cerr << hex << "Failed to fetch address " << addr << endl;
      }
      addr++;
    }
  }
  ram.push_back(map_m);

  return true;
}

void cpu_x86_32::diff_ram() {
  cout << "Following ram address values changed:" << endl;
  map<uint32_t, uint8_t> reg = ram.at(0);
  for(pair<uint32_t, uint8_t> pair: reg) {
    bool check = false;
    stringstream ss;
    ss << boost::format("0x%02x") % (int)pair.first << ":\t"
       << boost::format("0x%02x") % (int)pair.second << "\t";
    for(unsigned int i = 1; i < regs.size(); i++) {
      uint8_t val = ram.at(i)[pair.first];
      if(check || pair.second != val) {
        check = true;
        ss << to_string(i) << "->\t" << boost::format("0x%02x") % (int)val << "\t";
      }
    }
    if(check) {
      cout << ss.str() << endl;
    }
  }
}

string cpu_x86_32::get_instr() {
  stringstream ss;
  for(int i = 0; i < len; i++) {
    ss << boost::format("%02x") % (uint32_t)instr[i];
  }
  return ss.str();
}
