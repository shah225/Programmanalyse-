#pragma once
#include <ast/Program.h>

class ControlFlowGraph;

ControlFlowGraph ast2cfg(const ast::Program &program);

