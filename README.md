# memset_benchmark

This repository holds my experiment to accelerate memset and other string
functions. After reading a blog post by Joe Bialek, I decided to try to optimize
memset. It turns out that the libc library is not very efficient, and it's not
hard to write an implementation that outperforms the default libc
implementation. The repository contains a few utilities for testing and measuring the
performance of memset.

The implementation of memset in this repository is around 25% faster (geometric
mean for sizes 0..512) on Linux.

## Test tool

First, it contains a small test harness that verifies the
correctness of memset. It's really easy to make mistakes with off-by-one errors
and run into alignment issues. The exhaustive tester catches these issues.

This is a sample output:
```
OOOOOOOOOOOXX
           ^
Filling a buffer of length 13. Expected "O" at index 11
```

## Benchmark tool

The benchmark tool measures the performance of the system libc and the local
implementation. The benchmarking tool runs each of the implementations in a loop
millions of times. It runs the benchmark several times and picks the least noisy
results. It's a good idea to run the benchmark tool and compare the same
implementation to assess the noise level in the system. The benchmarking tool
uses a trampoline to prevent the compiler from inlining and expanding the
memset.

![Benchmark](docs/bench.png)

## Histogram tool

The histogram tool is a shared object that collects records calls to memset and
creates a histogram of the lengths of calls to memset. It prints the histogram
when the program exits cleanly. The shared object can be loaded using
LD\_PRELOAD (on Linux) or DYLD\_INSERT\_LIBRARIES (on Mac). Each bucket in the
output represents the log2 size of the buffer, and each value represents the
number of hits for the bucket.

Sample output:
```
  [0] = 4
  [1] = 14
  [2] = 8
  [3] = 14795
  [4] = 22496
  [5] = 10585
  [6] = 11176
  [7] = 5981
  [8] = 464
  [9] = 1044
  [10] = 137
  [11] = 55
  [12] = 18
  [13] = 12
  [14] = 15
  [15] = 5
  [16] = 2
  [17] = 2
  [18] = 0
  ```

## Proxy tool

This is a small utility that swaps the builtin call to memset with the local
implementation of memset from this project. .  The shared object can be loaded
using LD\_PRELOAD (on Linux) or DYLD\_INSERT\_LIBRARIES (on Mac).

## Known issues

MacOS The benchmarking and testing tools use a trampoline to prevent the
compiler from inlining the call to memset. Apple has different mangling rules
for their assembly. To solve this problem you need to add underscore to the
function names in the assembly file (.s file), like this:

```
.text
.global _libc_memset
.p2align  4, 0x90
_libc_memset:
        jmp _memset
```


