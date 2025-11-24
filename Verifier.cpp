#include "Boogie.h"
#include <z3++.h>
#include <iostream>
#include "./src/smt/SMT.h"
#include "Ast2Cfg.h"
#include "ControlFlowGraph.h"

int main(int argc, char *argv[]) {
  const auto program = parser::parseBoogie("benchmarks/addition.boogie");

  program->print();

  z3::expr post = z3Ctx.bool_val(true);

  z3::expr wp = program->wp(post);

  // raw and simplified wp
  std::cout << "Weakest Precondition: " << wp << std::endl;
  std::cout << "Result (simplify): " << wp.simplify() << std::endl;

  z3::solver s(z3Ctx);
  s.add(wp);

  auto result = s.check();
  if (result == z3::sat) {
    std::cout << "SAT: model exists" << std::endl;
    std::cout << s.get_model() << std::endl;
  } else if (result == z3::unsat) {
    std::cout << "UNSAT: no model satisfies the WP" << std::endl;
  } else {
    std::cout << "UNKNOWN: solver could not decide" << std::endl;
  }


  {
    ControlFlowGraph cfg = ast2cfg(*program);
    std::ofstream out("/app/output/cfg.dot");

    cfg.dumpDot(out);
    out.close();
    std::cout << "CFG in cfg.dot geschrieben. Mit Graphviz:\n";
    std::cout << "dot -Tpng cfg.dot -o cfg.png\n";
  }
  return 0;
}
