
# generate core.h
file(GLOB_RECURSE HEADERS RELATIVE ${PROJECT_SOURCE_DIR}/src *.h)
list(REMOVE_ITEM HEADERS "core.h")
file(WRITE core.h "// Generated headers include\n")
file(APPEND core.h "#ifndef __LXQSERVER_CORE_H__\n#define __LXQSERVER_CORE_H__\n\n")
foreach(header ${HEADERS})
    file(APPEND core.h "#include \"${header}\"\n")
endforeach()
file(APPEND core.h "\n\n#endif")

# config library output path
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/lib)

# find all cpps
file(GLOB_RECURSE SOURCES *.cpp)

# add include paths
include_directories(${PROJECT_SOURCE_DIR}/src)

# generate dynamic library
add_library(core_so SHARED ${SOURCES})
set_target_properties(core_so PROPERTIES OUTPUT_NAME "core")

## add include paths
#include_directories(${PROJECT_SOURCE_DIR}/src)
#
## generate static library
#add_library(core_a STATIC ${SOURCES})
#set_target_properties(core_a PROPERTIES OUTPUT_NAME "core")
