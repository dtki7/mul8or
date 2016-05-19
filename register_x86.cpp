#include <sstream>
#include <regex>
#include <boost/format.hpp>

#include "register_x86.h"

using namespace std;

register_x86::register_x86() {}

register_x86::register_x86(user_regs_struct user_regs, user_fpregs_struct user_fpregs,
             user_fpxregs_struct user_fpxregs) {
  this->regs.insert(make_pair("EBX", user_regs.ebx));
  this->regs.insert(make_pair("ECX", user_regs.ecx));
  this->regs.insert(make_pair("EDX", user_regs.edx));
  this->regs.insert(make_pair("ESI", user_regs.esi));
  this->regs.insert(make_pair("EDI", user_regs.edi));
  this->regs.insert(make_pair("EBP", user_regs.ebp));
  this->regs.insert(make_pair("EAX", user_regs.eax));
  this->regs.insert(make_pair("XDS", user_regs.xds));
  this->regs.insert(make_pair("XES", user_regs.xes));
  this->regs.insert(make_pair("XFS", user_regs.xfs));
  this->regs.insert(make_pair("XGS", user_regs.xgs));
  this->regs.insert(make_pair("EIP", user_regs.eip));
  this->regs.insert(make_pair("XCS", user_regs.xcs));
  this->regs.insert(make_pair("EFLAGS", user_regs.eflags));
  this->regs.insert(make_pair("ESP", user_regs.esp));
  this->regs.insert(make_pair("XSS", user_regs.xss));

  this->regs.insert(make_pair("CWD", user_fpregs.cwd));
  this->regs.insert(make_pair("SWD", user_fpregs.swd));
  this->regs.insert(make_pair("TWD", user_fpregs.twd));
  this->regs.insert(make_pair("FIP", user_fpregs.fip));
  this->regs.insert(make_pair("FCS", user_fpregs.fcs));
  this->regs.insert(make_pair("FOO", user_fpregs.foo));
  this->regs.insert(make_pair("FOS", user_fpregs.fos));
  long int* st_space = user_fpregs.st_space;
  for(int i = 0; i < ST_SPACE_COUNT; i++) {
    this->regs.insert(make_pair("ST" + str(boost::format("%02i") % i),
                                st_space[i]));
  }

  this->regs.insert(make_pair("CWD2", user_fpxregs.cwd));
  this->regs.insert(make_pair("SWD2", user_fpxregs.swd));
  this->regs.insert(make_pair("TWD2", user_fpxregs.twd));
  this->regs.insert(make_pair("FOP", user_fpxregs.fop));
  this->regs.insert(make_pair("FIP2", user_fpxregs.fip));
  this->regs.insert(make_pair("FCS2", user_fpxregs.fcs));
  this->regs.insert(make_pair("FOO2", user_fpxregs.foo));
  this->regs.insert(make_pair("FOS2", user_fpxregs.fos));
  this->regs.insert(make_pair("MXCSR", user_fpxregs.mxcsr));
  this->regs.insert(make_pair("RESERVED", user_fpxregs.reserved));
  long int* st_space2 = user_fpxregs.st_space;
  for(int i = 0; i < ST2_SPACE_COUNT; i++) {
    this->regs.insert(make_pair("ST2_" + str(boost::format("%02i") % i),
                                st_space2[i]));
  }
  long int* xmm_space = user_fpxregs.xmm_space;
  for(int i = 0; i < XMM_SPACE_COUNT; i++) {
    this->regs.insert(make_pair("XMM" + str(boost::format("%02i") % i),
                      xmm_space[i]));
  }
  long int* padding = user_fpxregs.padding;
  for(int i = 0; i < PADDING_COUNT; i++) {
    this->regs.insert(make_pair("PADDING" + str(boost::format("%02i") % i),
                      padding[i]));
  }
}

register_x86::register_x86(const register_x86& obj) {
  regs.insert(obj.regs.begin(), obj.regs.end());
}

void register_x86::dump_full(ofstream& file) {
  file << "#" << endl;
  for(pair<string, unsigned long> reg: regs) {
    file << reg.first << ":" << reg.second << endl;
  }
  file << endl;
}

void register_x86::load_full(ifstream& file) {
  string line;
  regex regx("([A-Z0-9_]+?):([0-9]+)");
  while(getline(file, line)) {
    if(line == "") break;
    smatch sm;
    regex_search(line, sm, regx);
    regs.insert(make_pair(sm[1], strtoul(string(sm[2]).c_str(), nullptr, 10)));
  }
}

void register_x86::diff_same(vector<register_x86> regs) {
  cout << hex;
  for(unsigned int i = 0; i < regs.size(); i++) {
    map<string, unsigned long> reg = regs.at(i).regs;
    cout << "Following registers got the same value (Set " << i << "):" << endl;
    for(pair<string, unsigned long> pair1: reg) {
      for(pair<string, unsigned long> pair2: reg) {
          if(pair1.first.compare(pair2.first) < 0
              && pair1.second == pair2.second) {
                cout << pair1.first << " == " << pair2.first << " == "
                     << pair1.second << endl;
          }
      }
    }
  }
}

string register_x86::diff_change(vector<register_x86> regs, bool all) {
  cout << "Following registers changed:" << endl;
  stringstream full_ss;
  map<string, unsigned long> reg = regs.at(0).regs;
  for(pair<string, unsigned long> pair: reg) {
    bool check = all;
    stringstream ss;
    ss << pair.first << ":\t" << boost::format("0x%08x") % pair.second
       << "\t";
    for(unsigned int i = 1; i < regs.size(); i++) {
      unsigned long val = regs.at(i).regs[pair.first];
      if(regs.at(i-1).regs[pair.first] != val) {
        check = true;
        ss << to_string(i) << "->\t" << boost::format("0x%08x") % val << "\t";
      }
    }
    if(check) {
      cout << ss.str() << endl;
      full_ss << ss.str() << endl;
    }
  }
  return full_ss.str();
}

string register_x86::get_user_reg_name(enum user_reg_names_x86 name) {
  switch(name) {
    case EBX: return "EBX";
    case ECX: return "ECX";
    case EDX: return "EDX";
    case ESI: return "ESI";
    case EDI: return "EDI";
    case EBP: return "EBP";
    case EAX: return "EAX";
    case DS: return "DS";
    case ES: return "ES";
    case FS: return "FS";
    case GS: return "GS";
    case ORIG_EAX: return "ORIG_EAX";
    case EIP: return "EIP";
    case CS: return "CS";
    case EFLAGS: return "EFLAGS";
    case ESP: return "ESP";
    case SS: return "SS";
    default: return "NO_SUCH_REG";
  }
}
