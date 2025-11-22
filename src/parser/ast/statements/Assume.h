#pragma once
#include <sstream>

#include "../programs/Statement.h"
#include "../Expression.h"

namespace ast {
class Assume final : public Statement {
public:
  const std::shared_ptr<Expression> condition;

  explicit Assume(std::shared_ptr<Expression> condition) : condition(std::move(condition)) {
  }

  [[nodiscard]] z3::expr wp(const z3::expr &postcondition) const override {
    //assume als implikation: nur false, wenn true -> false
    return z3::implies(condition->expression, postcondition);
};


  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    ss << std::string(4 * indent, ' ') << "assume(" << condition->toString() << ");";
    return ss.str();
  }
};
}
