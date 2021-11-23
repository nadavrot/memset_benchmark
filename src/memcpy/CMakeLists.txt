add_executable(test_memcpy
                 test_memcpy.cc
                 )

target_link_libraries(test_memcpy PUBLIC)

install(TARGETS test_memcpy DESTINATION bin)

