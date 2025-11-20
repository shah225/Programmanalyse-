# Z3
# provides library 'z3'

if (USE_SYSTEM_Z3)
    message(STATUS "Using system-installed Z3")
    # requires installed Z3
    # mac: brew install z3, ubuntu: sudo apt install libz3-dev

    find_package(Z3 CONFIG)
    if (Z3_FOUND)
        message(STATUS "Z3 version ${Z3_VERSION_STRING} found at ${Z3_DIR}")
        message(STATUS "Z3_INCLUDE_DIRS: ${Z3_CXX_INCLUDE_DIRS}")
        message(STATUS "Z3_LIB: ${Z3_LIBRARIES}")
        add_library(z3 INTERFACE IMPORTED)
        target_include_directories(z3 INTERFACE ${Z3_CXX_INCLUDE_DIRS})
        target_link_libraries(z3 INTERFACE ${Z3_LIBRARIES})
    else ()
        # try harder
        message(STATUS "Try to find Z3 via find_path/find_library.")
        find_path(Z3_INCLUDE_DIR NAMES z3++.h z3.h)
        find_library(Z3_LIBRARY NAMES z3)
        if (NOT Z3_INCLUDE_DIR OR NOT Z3_LIBRARY)
            message(FATAL_ERROR "Could not find Z3 on this system.")
        else ()
            message(STATUS "Z3_INCLUDE_DIR = ${Z3_INCLUDE_DIR}")
            message(STATUS "Z3_LIBRARY     = ${Z3_LIBRARY}")
            add_library(z3 INTERFACE IMPORTED)
            target_include_directories(z3 INTERFACE ${Z3_INCLUDE_DIR})
            target_link_libraries(z3 INTERFACE ${Z3_LIBRARY})
        endif ()
    endif ()
else ()
    message(STATUS "Using bundled Z3")

    include(FetchContent)
    FetchContent_Declare(z3
            GIT_REPOSITORY https://github.com/Z3Prover/z3
            GIT_TAG z3-4.13.3
    )
    FetchContent_MakeAvailable(z3)

    add_library(z3 INTERFACE IMPORTED)
    include_directories(z3 ${z3_SOURCE_DIR}/src/api ${z3_SOURCE_DIR}/src/api/c++)
    target_link_libraries(z3 INTERFACE libz3)
endif ()