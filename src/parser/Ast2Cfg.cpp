#include "Ast2Cfg.h"
#include "ControlFlowGraph.h"

#include "programs/Statement.h"
#include "programs/Sequence.h"
#include "programs/IfElse.h"
#include "programs/While.h"

#include "statements/Assignment.h"
#include "statements/Assert.h"
#include "statements/Assume.h"
#include "statements/Skip.h"


struct BuildSubgraph {
    int entry;
    int exit;
};

class CFGBuilder {
public:
    explicit CFGBuilder(ControlFlowGraph &cfg) : cfg(cfg) {}

  BuildSubgraph build(const ast::Program &prog) {
      if (auto p = dynamic_cast<const ast::Sequence*>(&prog))   return buildSequence(*p);
      if (auto p = dynamic_cast<const ast::Assignment*>(&prog)) return buildAssignment(*p);
      if (auto p = dynamic_cast<const ast::IfElse*>(&prog))     return buildIfElse(*p);
      if (auto p = dynamic_cast<const ast::While*>(&prog))      return buildWhile(*p);
      if (auto p = dynamic_cast<const ast::Assert*>(&prog))     return buildAssert(*p);
      if (auto p = dynamic_cast<const ast::Assume*>(&prog))     return buildAssume(*p);
      if (auto p = dynamic_cast<const ast::Skip*>(&prog))       return buildSkip(*p);

      int e = cfg.addNode("stmt entry");
      int x = cfg.addNode("stmt exit");
      cfg.addEdge(e, x, "unknown");
      return {e, x};
    }



    BuildSubgraph buildAssignment(const ast::Assignment &assign) {
    int e = cfg.addNode("assign entry");
    int x = cfg.addNode("assign exit");
    // Identifier + Expression → "x = 5"
    cfg.addEdge(e, x, assign.identifier + " = " + assign.expression->toString());
    return {e, x};
}

BuildSubgraph buildIfElse(const ast::IfElse &ifelse) {
    int test = cfg.addNode("if test");

    BuildSubgraph thenG = build(*ifelse.thenBody);
    BuildSubgraph elseG;
    if (ifelse.elseBody.has_value()) {
        elseG = build(*ifelse.elseBody.value());
    } else {
        // leeres else → Skip
        int e = cfg.addNode("else entry");
        int x = cfg.addNode("else exit");
        cfg.addEdge(e, x, "");
        elseG = {e, x};
    }

    cfg.addEdge(test, thenG.entry, ifelse.condition->toString());
    cfg.addEdge(test, elseG.entry, "!(" + ifelse.condition->toString() + ")");

    int join = cfg.addNode("if join");
    cfg.addEdge(thenG.exit, join, "");
    cfg.addEdge(elseG.exit, join, "");
    return {test, join};
}

BuildSubgraph buildWhile(const ast::While &wh) {
    int header = cfg.addNode("while header");

    BuildSubgraph bodyG = build(*wh.body);

    cfg.addEdge(header, bodyG.entry, wh.condition->toString());
    int exit = cfg.addNode("while exit");
    cfg.addEdge(header, exit, "!(" + wh.condition->toString() + ")");
    cfg.addEdge(bodyG.exit, header, "");

    // invariant als Label
    if (wh.invariant) {
        cfg.addEdge(header, header, "invariant: " + wh.invariant->toString());
    }

    return {header, exit};
}

BuildSubgraph buildAssert(const ast::Assert &as) {
    int e = cfg.addNode("assert entry");
    int x = cfg.addNode("assert exit");
    cfg.addEdge(e, x, "assert(" + as.condition->toString() + ")");
    return {e, x};
}

BuildSubgraph buildAssume(const ast::Assume &as) {
    int e = cfg.addNode("assume entry");
    int x = cfg.addNode("assume exit");
    cfg.addEdge(e, x, "assume(" + as.condition->toString() + ")");
    return {e, x};
}

BuildSubgraph buildSkip(const ast::Skip &) {
    int e = cfg.addNode("skip entry");
    int x = cfg.addNode("skip exit");
    cfg.addEdge(e, x, "");
    return {e, x};
}

BuildSubgraph buildSequence(const ast::Sequence &seq) {
    int passthrough = cfg.addNode("seq entry");
    BuildSubgraph acc{passthrough, passthrough};
    for (auto &prog : seq.programs) {
        BuildSubgraph sg = build(*prog);
        cfg.addEdge(acc.exit, sg.entry, "");
        acc.exit = sg.exit;
    }
    return acc;
}


private:
    ControlFlowGraph &cfg;
};

ControlFlowGraph ast2cfg(const ast::Program &program) {
    ControlFlowGraph cfg;
    int entry = cfg.addNode("program entry");
    int exit  = cfg.addNode("program exit");
    cfg.setEntry(entry);
    cfg.setExit(exit);

    CFGBuilder builder(cfg);
    BuildSubgraph g = builder.build(program);

    cfg.addEdge(entry, g.entry, "");
    cfg.addEdge(g.exit, exit, "");

    return cfg;
}
