#pragma once
#include <sstream>

#include "../Expression.h"
#include "../Program.h"

namespace ast {
class While final : public Program {
public:
  const std::shared_ptr<Expression> condition;
  const std::shared_ptr<const Program> body;
  const std::shared_ptr<Expression> invariant;

  While(std::shared_ptr<Expression> condition,
      std::shared_ptr<const Program> body,
      std::shared_ptr<Expression> invariant)
    : condition(std::move(condition)),
      body(std::move(body)),
      invariant(std::move(invariant)) {}


  [[nodiscard]] z3::expr wp(const z3::expr &postcondition) const override {
    z3::expr inv = invariant->expression;
    z3::expr cond = condition->expression;

    //wenn inv & bed gelten, gilt es auch nach dem body
    z3::expr preservation = z3::implies(inv && cond, body->wp(inv));

    //inv gilt, aber cond nicht, dann gilt postcondition
    z3::expr termination = z3::implies(inv && !cond, postcondition);

    //wp durch invariante, und bedingung kombinieren
    return inv && preservation && termination;
}

  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    ss << std::string(4 * indent, ' ') << "while (" << condition->toString() << ") { \n";
    ss << std::string(4 * indent, ' ') << "[" << invariant->toString() << "]\n";
    ss << body->toString(indent + 1) << "\n";
    ss << std::string(4 * indent, ' ') << "}";
    return ss.str();
  }
};
}
