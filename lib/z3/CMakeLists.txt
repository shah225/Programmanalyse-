# Z3
# TODO: use shared? find_package(z3 REQUIRED)
include(FetchContent)
FetchContent_Declare(z3
        GIT_REPOSITORY https://github.com/Z3Prover/z3
        GIT_TAG z3-4.13.3
)
FetchContent_MakeAvailable(z3)
include_directories(${z3_SOURCE_DIR}/src/api ${z3_SOURCE_DIR}/src/api/c++)