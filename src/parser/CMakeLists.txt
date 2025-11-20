# ANTLR parser generator
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/lib/antlr/antlr-4.13.1-complete.jar) # set antlr parser generator location
# ANTLR runtime
add_definitions(-DANTLR4CPP_STATIC) # required if linking to static library
set(ANTLR4_WITH_STATIC_CRT OFF) # using /MD flag for antlr4_runtime (for Visual C++ compilers only)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/lib/antlr/cmake) # add ./cmake folder to cmake module path
find_package(ANTLR REQUIRED) # require antlr commands
include(ExternalAntlr4Cpp) # build and add runtime for antlr4 -> variable: antlr4_static

antlr_target(BoogieParser Boogie.g4 VISITOR) # generate parser -> output: ${ANTLR_BoogieParser_CXX_OUTPUTS}
message(STATUS "Generated Boogie Parser: ${ANTLR_BoogieParser_CXX_OUTPUTS}")

set(PARSER_SOURCES
        ast/programs/IfElse.h
        ast/programs/Sequence.h
        ast/programs/Statement.h
        ast/programs/While.h
        ast/statements/Assert.h
        ast/statements/Assignment.h
        ast/statements/Assume.h
        ast/statements/Return.h
        ast/statements/Skip.h
        ast/Expression.h
        ast/FunctionDefinition.h
        ast/Program.h
        Ast2Cfg.h
        Boogie.h
        Boogie.cpp
)
add_library(parser ${PARSER_SOURCES} ${ANTLR_BoogieParser_CXX_OUTPUTS})

# linking
set_target_properties(parser PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) # export Boogie.h
target_include_directories(parser PUBLIC ${ANTLR_BoogieParser_OUTPUT_DIR}) # add generated parser files
target_include_directories(parser PUBLIC ${ANTLR_CParser_OUTPUT_DIR}) # add generated parser files
target_include_directories(parser PUBLIC ${ANTLR4_INCLUDE_DIRS}) # add antrl4cpp artifacts to project environment
target_link_libraries(parser PUBLIC antlr4_static smtlib) # link with antlr runtime
