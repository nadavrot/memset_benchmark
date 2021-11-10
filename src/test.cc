#include <cstring>
#include <iostream>
#include <vector>

#include "timer_utils.h"
#include "utils.h"

#define MAGIC_VALUE0 'x'
#define MAGIC_VALUE1 'y'

extern "C" {
void *memset(void *s, int c, size_t n);
void *libc_memset(void *s, int c, size_t n);
void *local_memset(void *s, int c, size_t n);
void *musl_memset(void *s, int c, size_t n);
}

using memset_ty = void *(void *s, int c, size_t n);

void print_buffer(const char *start, const char *end, char val,
                  const char *ptr) {
  const char* it = start;
  while (it != end) {
    std::cout << *it;
    it++;
  }
  std::cout<<"\n";
  it = start;
  while (it != ptr) {
    std::cout << " ";
    it++;
  }
  std::cout<<"^\n";
  std::cout<<"expected \"" << val << "\" at index " << ptr - start << "\n";
}

void assert_uniform_value(const char *start, const char *end, char val) {
  const char *ptr = start;
  while (ptr != end) {
    if (val != *ptr) {
      print_buffer(start, end, val, ptr);
      abort();
    }
    ptr++;
  }
}

void test_impl(memset_ty handle, const std::string &name, unsigned chunk_size) {
  std::vector<char> memory(chunk_size + 256, MAGIC_VALUE0);

  std::cout << "Testing " << name << " of size " << chunk_size << "\n";

  // Start mem-setting the array at different offsets.
  for (int offset = 0; offset < 64; offset++) {
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
  std::cout << "Name, size, alignment, offset,\n";

#define TEST(FUNC, SIZE) test_impl(FUNC, #FUNC, SIZE);

  for (int i = 0; i < 256; i++) {
    TEST(libc_memset, i);
    TEST(local_memset, i);
    TEST(musl_memset, i);
  }

  return 0;
}
