add_library(print_ip_lib INTERFACE
   include/print_ip.h
)

target_link_libraries(print_ip_lib INTERFACE compiler_flags)

target_include_directories(print_ip_lib INTERFACE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>
)
