#include "Boogie.h"
#include <z3++.h>
#include <iostream>
#include "./src/smt/SMT.h"
#include "Ast2Cfg.h"
#include "ControlFlowGraph.h"

#include "./src/parser/ast/programs/Sequence.h"
#include "./src/parser/ast/programs/IfElse.h"
#include "./src/parser/ast/programs/While.h"

#include "./src/parser/ast/statements/Assignment.h"
#include "./src/parser/ast/statements/Assert.h"
#include "./src/parser/ast/statements/Assume.h"
#include "./src/parser/ast/statements/Skip.h"

inline z3::expr collectAssertions(const ast::Program &prog, z3::context &ctx) {
    z3::expr post = ctx.bool_val(true);

    if (auto seq = dynamic_cast<const ast::Sequence*>(&prog)) {
        for (auto &sub : seq->programs) {
            post = post && collectAssertions(*sub, ctx);
        }
    } else if (auto as = dynamic_cast<const ast::Assert*>(&prog)) {
        z3::expr cond = as->condition->expression;
        post = post && cond;
    } else if (auto ifelse = dynamic_cast<const ast::IfElse*>(&prog)) {
        post = post && collectAssertions(*ifelse->thenBody, ctx);
        if (ifelse->elseBody.has_value()) {
            post = post && collectAssertions(*ifelse->elseBody.value(), ctx);
        }
    } else if (auto wh = dynamic_cast<const ast::While*>(&prog)) {
        post = post && collectAssertions(*wh->body, ctx);
    }

    return post;
}

int main(int argc, char *argv[]) {
  const auto program = parser::parseBoogie("benchmarks/vc.boogie");

  program->print();

  z3::expr post = collectAssertions(*program, z3Ctx);

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
