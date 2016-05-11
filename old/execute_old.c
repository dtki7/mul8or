#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, const char* argv[]) {
  void* exc = mmap(0, sizeof(argv[1]) + 1, PROT_READ|PROT_WRITE|PROT_EXEC,
             MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  memcpy(exc, argv[1], sizeof(argv[1]) + 1);

  raise(SIGTRAP);
  ((void(*)(void))(exc))();
  raise(SIGTRAP);
}
