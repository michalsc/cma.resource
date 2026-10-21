cmake_minimum_required(VERSION 3.14.0)
project(cma VERSION 1.0.0)

include(cmake/bin2h.cmake)
include(cmake/verstring.cmake)
include(cmake/sfdc.cmake)

if(NOT TARGET devicetree)
    add_subdirectory(devicetree.resource EXCLUDE_FROM_ALL)
endif()

get_verstring(VERSTRING)
sfdc(sfd/cma_lib.sfd)

add_executable(cma.resource
    src/main.c
    src/init.c
    src/tlsf.c
    src/avail.c
    src/alloc.c
)

bin_to_header(cma.resource)

target_compile_options(cma.resource PRIVATE -Os -m68040 -msmall-code -fomit-frame-pointer)
target_compile_definitions(cma.resource PRIVATE VERSION_STRING="${VERSTRING}")
target_include_directories(cma.resource PRIVATE include)
target_link_options(cma.resource PRIVATE -ffreestanding -nostdlib -nostartfiles -Wl,-e_code_start -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/ldscript.lds)
target_link_libraries(cma.resource devicetree)

set_target_properties(cma.resource PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ldscript.lds)

# no install for devicetree.resource itself, it goes to ROM
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sfd DESTINATION Developer)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION Developer/include)
