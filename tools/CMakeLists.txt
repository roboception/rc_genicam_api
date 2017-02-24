
# Author: Heiko Hirschmueller

project(tools CXX)

# build programs

add_executable(gc_info gc_info.cc)
target_link_libraries(gc_info rcgcapi_static)

add_executable(gc_config gc_config.cc)
target_link_libraries(gc_config rcgcapi_static)

add_executable(gc_stream gc_stream.cc)
target_link_libraries(gc_stream rcgcapi_static)

# install tools

install(TARGETS gc_info COMPONENT bin DESTINATION bin)
install(TARGETS gc_config COMPONENT bin DESTINATION bin)
install(TARGETS gc_stream COMPONENT bin DESTINATION bin)