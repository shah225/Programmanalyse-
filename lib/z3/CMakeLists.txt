# Z3
# provides library 'z3'

message(STATUS "Using system-installed Z3")
# requires installed Z3
# mac: brew install z3, ubuntu: sudo apt install libz3-dev

find_path(Z3_INCLUDE_DIR NAMES z3++.h z3.h)
find_library(Z3_LIBRARY NAMES z3)

if (NOT Z3_INCLUDE_DIR OR NOT Z3_LIBRARY)
    message(FATAL_ERROR "Could not find Z3 on this system. Please install libz3-dev.")
else ()
    message(STATUS "Z3_INCLUDE_DIR = ${Z3_INCLUDE_DIR}")
    message(STATUS "Z3_LIBRARY     = ${Z3_LIBRARY}")
    add_library(z3 INTERFACE IMPORTED)
    target_include_directories(z3 INTERFACE ${Z3_INCLUDE_DIR})
    target_link_libraries(z3 INTERFACE ${Z3_LIBRARY})
endif ()
