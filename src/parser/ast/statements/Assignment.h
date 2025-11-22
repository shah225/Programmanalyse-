#pragma once
#include <sstream>

#include "../../../smt/Symbols.h"
#include "../programs/Statement.h"
#include "ast/Expression.h"

namespace ast {
class Assignment final : public Statement {
public:
  const Identifier identifier;
  const std::shared_ptr<Expression> expression;

  Assignment(Identifier identifier, std::shared_ptr<Expression> expression)
      : identifier(std::move(identifier)),
        expression(std::move(expression)) {
  }

  [[nodiscard]] z3::expr wp(const z3::expr& postcondition) const override {
    const z3::expr& var = *identifierToSymbol(identifier);
    const z3::expr& rhs = expression->expression;

    z3::expr_vector from(z3Ctx);
    from.push_back(var);

    z3::expr_vector to(z3Ctx);
    to.push_back(rhs);

    z3::expr post_copy = postcondition;

    //in postcondition alle vorkommen der var durch rhs ersetzen
    return post_copy.substitute(from, to);

  }

  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    ss << std::string(4 * indent, ' ') << identifier << " := " << expression->toString() << ";";
    return ss.str();
  }
};
}


