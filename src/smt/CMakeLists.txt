add_library(smtlib INTERFACE)
target_include_directories(smtlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(smtlib INTERFACE z3)