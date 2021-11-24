add_executable(test_memcpy
                 test_memcpy.cc
                 folly.S
                 impl.S
                 impl.c
                 )

target_link_libraries(test_memcpy PUBLIC)

add_executable(bench_memcpy
                 bench_memcpy.cc
                 folly.S
                 impl.S
                 impl.c
                 )

install(TARGETS bench_memcpy DESTINATION bin)
install(TARGETS test_memcpy DESTINATION bin)

