# Fast Memset and Memcpy implementations

*UPDATE*: Ilya Albrecht landed the memset implementation from this repo into [Folly](https://github.com/facebook/folly/blob/main/folly/memset.S).

This repository contains high-performance implementations of memset and memcpy.
These implementations outperform the folly and glibc implementations.  This
repository contains several reference implementations in C and assembly.  The
high-performance implementations are found in the files called "impl.S".

Before reading the source code in this repository you probably want to read an
excellent blog [post](https://msrc-blog.microsoft.com/2021/01/11/building-faster-amd64-memset-routines/)
by Joe Bialek about his work to optimize memset for windows.

The charts below compare the code in this repo with other implementations:
folly, musl, and glibc.  The glibc implementations are measured with and without
the elf indirection, as suggested by Dave Zarzycki.

## Memset
![Memset](docs/memset_bench.png)

## Memcpy
![Memcpy](docs/memcpy_bench.png)

The chart below compares the performance of different memset implementations on
buffers of varying sizes and offsets. Unlike the hot loop that hammers a single
value, this benchmark is more realistic and takes into account mispredicted
branches and the performance of the cpu decoder. The buffers are in the size
range 0 to 256. The random function is made of pre-computed random values, to
lower the overhead of the random function.  This was suggested by Yann Collet.
The 'nop' function is used to compute the benchmark setup and call overhead. The
numbers below represent the implementation execution time minus the nop function
time.

![memset](docs/memset_r.png) ![memcpy](docs/memcpy_r.png)

The size of the buffer that memset and memcpy mutates is typically small. The
picture below presents the buffer length distribution in google-chrome. Vim,
Python, and even server workloads have a similar distribution. The values in the
chart represent the power of two buffer size (10 represents the values between
512 and 1024).
 
![Histogram](docs/hist.png)


The chart below presents a histogram of pointer alignment (from the game
minecraft). Most of the pointers that are called by memset and memcpy are
aligned to 8-byte values. Some programs have histograms that are not as sharp,
meaning that there are more values that are not aligned to 4 or 8-byte boundary.

![Pointer Alignment](docs/align.png)


Memcpy and Memset and frequently called by low-level high-performance libraries.
Here is one example of one stack trace from the Firefox codebase:

```
  (gdb) bt
  #0  __memmove_avx_unaligned_erms () at ../sysdeps/x86_64/multiarch/memmove-vec-unaligned-erms.S:225
  #1  in memcpy (__dest=, __src=, __len=40) at /usr/include/x86_64-linux-gnu/bits/string_fortified.h:34
  #2  mozilla::BufferList<InfallibleAllocPolicy>::ReadBytes(mozilla::BufferList<InfallibleAllocPolicy>::IterImpl&, char*, unsigned long) const
  #3  Pickle::ReadBytesInto(PickleIterator*, void*, unsigned int) const (this=, iter=, data=, length=<optimized out>)
  #4  in IPC::Message::ReadFooter(void*, unsigned int, bool) (this=, buffer=, buffer_len=40, truncate=true)
  #5  in mozilla::ipc::NodeController::DeserializeEventMessage(mozilla::UniquePtr<IPC::Message, mozilla::DefaultDelete<IPC::Message> >) (this=, aMessage=...)
  #6  in mozilla::ipc::NodeController::OnEventMessage(mojo::core::ports::NodeName const&, mozilla::UniquePtr<IPC::Message, mozilla::DefaultDelete<IPC::Message> >)
  #7  in mozilla::ipc::NodeChannel::OnMessageReceived(IPC::Message&&) (this=<optimized out>, aMessage=...)
  #8  in IPC::Channel::ChannelImpl::ProcessIncomingMessages() (this=<optimized out>)
  #9  in IPC::Channel::ChannelImpl::OnFileCanReadWithoutBlocking(int) (this=, fd=)
  #10 in base::MessagePumpLibevent::OnLibeventNotification(int, short, void*) (fd=, flags=, context=)
  #11 in event_persist_closure (base=, ev=) at /build/firefox-HSiFn6/firefox-94.0+build3/ipc/chromium/src/third_party/libevent/event.c:1580
  #12 event_process_active_single_queue (base=, activeq=, max_to_process=, endtime=)

```

The repository contains a few utilities for testing and measuring the
performance and correctness of memset and memcpy.

## Test tool

This is a small test harness that verifies the correctness of the
implementations. It's really easy to make mistakes with off-by-one errors and
run into alignment issues. The exhaustive tester catches these issues.

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
results. It's a good idea to run the benchmark tool and compare some
implementation to itself to assess the noise level in the system. The
benchmarking tool uses a trampoline to prevent the compiler from inlining and
expanding the memset.

## Histogram tool

The histogram tool is a shared object that collects records calls to memset and
memcpy and creates a histogram of the length parameter. It prints the histogram
when the program exits cleanly. The shared object can be loaded using
LD\_PRELOAD (on Linux) or DYLD\_INSERT\_LIBRARIES (on Mac). Each bucket in the
output represents the log2 size of the buffer, and each value represents the
number of hits for the bucket.

## Proxy tool

This is a small utility that swaps the builtin call to memset and memcpy with
the local implementation from this project. The shared object can be loaded
using LD\_PRELOAD (on Linux) or DYLD\_INSERT\_LIBRARIES (on Mac).

