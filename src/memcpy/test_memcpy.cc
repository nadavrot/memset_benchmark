#include <cstring>
#include <iostream>
#include <vector>

#include "decl.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////////
// This is a small program that checks if some memcpy implementation is correct.
////////////////////////////////////////////////////////////////////////////////

#define MAGIC_VALUE0 '#'
#define MAGIC_VALUE1 '='

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
  std::cout << " Expected \"" << val << "\" at index " << ptr - start
            << std::endl;
}

// Make sure that the whole buffer, from \p start to \p end, is set to \p val.
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

// Make sure that two buffers contain the same memory content.
void assert_buffers_match(const char *buff1, const char *buff2, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (buff1[i] != buff2[i]) {
      print_buffer(buff1, buff1 + len, buff2[i], &buff1[i]);
      abort();
    }
  }
}

void test_impl(memcpy_ty handle, const std::string &name, unsigned chunk_size) {
  std::vector<char> src(chunk_size + 512);
  std::vector<char> dest(chunk_size + 512, MAGIC_VALUE0);

  // Fill the buffer with a running counter of printable chars.
  for (unsigned i = 0; i < src.size(); i++) {
    src[i] = 'A' + (i % 26);
  }

  // Start copying memory at different offsets.
  for (int src_offset = 0; src_offset < 32; src_offset++) {
    for (int dest_offset = 0; dest_offset < 32; dest_offset++) {
      const char *dest_start = &*dest.begin();
      const char *dest_end = &*dest.end();

      const char *src_region_start = &src[src_offset];
      const char *dest_region_start = &dest[dest_offset];
      const char *dest_region_end = &dest[dest_offset + chunk_size];

      void *res =
          (handle)((void *)dest_region_start, src_region_start, chunk_size);
      if (res != dest_region_start) {
        std::cout << "Invalid return value." << std::endl;
        abort();
      }

      // Check the chunk.
      assert_buffers_match(dest_region_start, src_region_start, chunk_size);
      // Check before chunk.
      assert_uniform_value(dest_start, dest_region_start, MAGIC_VALUE0);
      // Check after chunk.
      assert_uniform_value(dest_region_end, dest_end, MAGIC_VALUE0);

      // Reset the dest buffer:
      std::fill(dest.begin(), dest.end(), MAGIC_VALUE0);
    }
  }
}

int main(int argc, char **argv) {
  std::cout << "Testing memcpy... \n";

#define TEST(FUNC, SIZE) test_impl(FUNC, #FUNC, SIZE);

  for (int i = 0; i < 1024; i++) {
    TEST(&memcpy, i);
    TEST(&__folly_memcpy, i);
  }
  std::cout << "Done.\n";

  return 0;
}
