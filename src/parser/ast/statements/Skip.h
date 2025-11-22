#pragma once
#include <sstream>

#include "../programs/Statement.h"

namespace ast {
class Skip final : public Statement {
public:
  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    ss << std::string(4 * indent, ' ') << "skip;";
    return ss.str();
  }

  [[nodiscard]] z3::expr wp(const z3::expr& postcondition) const override {
    //zustand unveränder, postcondition muss schon vorher gelten
    return postcondition;
  };
};
}
