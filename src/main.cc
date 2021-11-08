#include <cstring>
#include <iostream>
#include <vector>

#include "timer_utils.h"

void test() {
  TimerGuard T("simple");
  std::vector<char> memory(1000, 0);

  for (int i = 0; i < 10000 * 109; i++) {
    memset(&memory[0], 0, memory.size());
  }
}

int main(int argc, char **argv) {
  test();
  return 0;
}
