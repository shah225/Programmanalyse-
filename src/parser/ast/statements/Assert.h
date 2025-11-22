#pragma once
#include <sstream>

#include "../statements/Assume.h"
#include "../Expression.h"

namespace ast {
class Assert final : public Statement {
public:
  const std::shared_ptr<Expression> condition;

  explicit Assert(std::shared_ptr<Expression> condition) : condition(std::move(condition)) {
  }

  [[nodiscard]] z3::expr wp(const z3::expr& postcondition) const override {
    //assert als konjuknktion
    return condition->expression && postcondition;
  }

  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    ss << std::string(4 * indent, ' ') << "assert(" << condition->toString() << ");";
    return ss.str();
  }
};
}
