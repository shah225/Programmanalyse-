#pragma once
#include <optional>
#include <sstream>
#include <z3++.h>

#include "../Program.h"
#include "../Expression.h"

namespace ast {
class IfElse final : public Program {
public:
  const std::shared_ptr<Expression> condition;
  const std::shared_ptr<Program> thenBody;
  const std::optional<std::shared_ptr<Program> > elseBody;

  IfElse(std::shared_ptr<Expression> condition, std::shared_ptr<Program> thenBody)
      : condition(std::move(condition)),
        thenBody(std::move(thenBody)),
        elseBody(std::nullopt) {
  }

  [[nodiscard]] z3::expr wp(const z3::expr& postcondition) const override {
    z3::expr thenWP = thenBody->wp(postcondition);
    //else kann semantisch skip sein, deshalb prüfen ob has_vlaue
    z3::expr elseWP = elseBody.has_value() ? elseBody.value()->wp(postcondition) : postcondition;

    //umsetzung mit implikation - wie assume des jeweiligen zweigs
    return z3::implies(condition->expression, thenWP)
      && z3::implies(!condition->expression, elseWP);
  }

  IfElse(std::shared_ptr<Expression> condition, std::shared_ptr<Program> thenBody,
         std::shared_ptr<Program> elseBody)
      : condition(std::move(condition)),
        thenBody(std::move(thenBody)),
        elseBody(std::make_optional(elseBody)) {
  }

  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    ss << std::string(4 * indent, ' ') << "if (" << condition->toString() << ") {\n";
    ss << thenBody->toString(indent + 1) << "\n";
    ss << std::string(4 * indent, ' ') << "}";
    if (elseBody.has_value()) {
      ss << " else {\n";
      ss << elseBody.value()->toString(indent + 1) << "\n";
      ss << std::string(4 * indent, ' ') << "}";
    }
    return ss.str();
  }
};
}
