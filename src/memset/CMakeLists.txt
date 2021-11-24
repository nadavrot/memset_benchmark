add_library(mem_shim SHARED
            shims.c
            impl.S
            impl.c
           )

set_target_properties(mem_shim PROPERTIES
     VERSION ${PROJECT_VERSION}
     SOVERSION 1
     )

add_executable(bench_memset
                 bench_memset.cc
                 impl.S
                 impl.c
                 )

add_executable(test_memset
                 test_memset.cc
                 impl.S
                 impl.c
                 )

target_link_libraries(bench_memset PUBLIC)
target_link_libraries(test_memset PUBLIC)

install(TARGETS bench_memset DESTINATION bin)
install(TARGETS test_memset DESTINATION bin)
install(TARGETS mem_shim LIBRARY DESTINATION bin)

