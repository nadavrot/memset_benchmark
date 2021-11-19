#include <cstring>
#include <iostream>
#include <vector>

#include "decl.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// This is a small program that checks if some memset implementation is correct.
// The tool currently checks libc, musl and the local implementation.
////////////////////////////////////////////////////////////////////////////////

#define MAGIC_VALUE0 'X'
#define MAGIC_VALUE1 'O'

void print_buffer(const char *start, const char *end, char val,
                  const char *ptr) {
  const char *it = start;
  while (it != end) {
    std::cout << *it;
    it++;
  }
  std::cout << "\n";
  it = start;
  while (it != ptr) {
    std::cout << " ";
    it++;
  }
  std::cout << "^\n";
  std::cout << "Filling a buffer of length " << end - start << ".";
  std::cout << " Expected \"" << val << "\" at index " << ptr - start << "\n";
}

void assert_uniform_value(const char *start, const char *end, char val) {
  const char *ptr = start;
  while (ptr != end) {
    if (val != *ptr) {
      print_buffer(start, end, val, ptr);
      fflush(stdout);
      abort();
    }
    ptr++;
  }
}

void test_impl(memset_ty handle, const std::string &name, unsigned chunk_size) {
  std::vector<char> memory(chunk_size + 512, MAGIC_VALUE0);
  // Start mem-setting the array at different offsets.
  for (int offset = 0; offset < 128; offset++) {
    const char *buffer_start = &*memory.begin();
    const char *buffer_end = &*memory.end();

    const char *region_start = &memory[offset];
    const char *region_end = region_start + chunk_size;

    assert_uniform_value(buffer_start, buffer_end, MAGIC_VALUE0);

    (handle)((void *)region_start, MAGIC_VALUE1, chunk_size);

    // Check the chunk.
    assert_uniform_value(region_start, region_end, MAGIC_VALUE1);
    // Check before chunk.
    assert_uniform_value(buffer_start, region_start, MAGIC_VALUE0);
    // Check after chunk.
    assert_uniform_value(region_end, buffer_end, MAGIC_VALUE0);

    // Reset the buffer:
    std::fill(memory.begin(), memory.end(), MAGIC_VALUE0);
    assert_uniform_value(buffer_start, buffer_end, MAGIC_VALUE0);
  }
}

int main(int argc, char **argv) {
  std::cout << "Testing memset... \n";

#define TEST(FUNC, SIZE) test_impl(FUNC, #FUNC, SIZE);

  for (int i = 0; i < 1024; i++) {
    TEST(libc_memset, i);
    TEST(local_memset, i);
    TEST(musl_memset, i);
    TEST(asm_memset, i);
  }
  std::cout << "Done.\n";

  return 0;
}
