#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <climits>
#include <string>

#include "x86.h"

using namespace std;

cpu_x86_32::cpu_x86_32(string instr)  : instr(instr) {}

bool cpu_x86_32::execute() {
  int wstatus;
  int pid = fork();

  if(pid == 0) {
    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    if (execlp("./execute", "./execute", instr.c_str()) < 0) {
      cerr << "ERROR: Execution of file \"execute\" failed!" << endl;
      return false;
    }
  }
  waitpid(pid, &wstatus, 0);
  ptrace(PTRACE_SETOPTIONS, pid, nullptr, PTRACE_O_EXITKILL);
  ptrace(PTRACE_SETOPTIONS, pid, nullptr, PTRACE_O_TRACEEXIT);

  read_regs(pid);
  read_ram(pid);
  int counter = 0;
  while(true) {
    counter++;
    ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr);
    waitpid(pid, &wstatus, 0);
    if(wstatus >> 8 == (SIGTRAP | (PTRACE_EVENT_EXIT << 8))) {  // caught exit
      cout << counter << endl;
      return true;
    }
  }

  waitpid(pid, &wstatus, 0);
  if(wstatus >> 8 == (SIGTRAP | (PTRACE_EVENT_EXIT << 8))) {  // caught exit
    read_regs(pid);
    read_ram(pid);
    ptrace(PTRACE_CONT, pid, nullptr, nullptr);
  } else {
    cerr << "ERROR: Catching exit failed!" << endl;
    return false;
  }
  return true;
}

// PTRACE_PEEKUSER (REGISTER)
// PTRACE_GETREGS, PTRACE_GETFPREGS (GENERAL PURPOSE AND FLOATING POINT REGISTER)
// PTRACE_GETREGSET (REGISTER THROUGH ADDR)

bool cpu_x86_32::read_regs(int pid) {
  return false;
}

bool cpu_x86_32::read_ram(int pid) {
  string path = "/proc/" + to_string(pid) + "/";

  ifstream maps_f;
  maps_f.open(path + "maps", ios::in);
  if(!maps_f.is_open()) {
    cerr << "ERROR: Failed to open \"maps\"-File \" of process " << pid << endl;
    return false;
  }

  vector<pair<uint32_t, uint32_t>> limits_l;
  for(string line; getline(maps_f, line); ) {
    cout << line << endl;
    limits_l.push_back(make_pair(stoul(line.substr(0, 8), nullptr, 16),
                                 stoul(line.substr(9, 8), nullptr, 16)));
  }
  maps_f.close();

  ifstream mem_f;
  mem_f.open(path + "mem", ios::in | ios::binary);
  if(!mem_f.is_open()) {
    cerr << "ERROR: Failed to open \"mem\"-File \" of process " << pid << endl;
    return false;
  }

  map<uint32_t, unsigned char> map_m;
  for(pair<uint32_t, uint32_t> limits : limits_l) {
    uint32_t addr = limits.first;
    mem_f.seekg(limits.first);
    while(mem_f.tellg() < limits.second) {
      char val; mem_f.read(&val, 1);
      long val2 = ptrace(PTRACE_PEEKDATA, pid, addr, nullptr); addr++;
      if(val2 == -1 && errno) {
        cerr << hex << "ERROR: Failed to read 0x" << limits.first << "-0x"
             << limits.second << " :: " << addr << " :: "<< dec << val2 << endl;
        break;
      }
      // if(mem_f.tellg() < 0) {
      //   cerr << hex << "ERROR: Failed to read 0x" << limits.first << "-0x"
      //        << limits.second << endl;
      //   break;
      // }
      map_m.insert(make_pair(mem_f.tellg(), val));
    }
  }
  ram.push_back(map_m);

  mem_f.close();
  return true;
}
