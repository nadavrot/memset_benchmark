add_library(hist_tool SHARED
            hist_tool.c
           )

set_target_properties(hist_tool PROPERTIES
     VERSION ${PROJECT_VERSION}
     SOVERSION 1
     )

target_compile_options(hist_tool PRIVATE "-fno-builtin")

install(TARGETS hist_tool LIBRARY DESTINATION bin)
