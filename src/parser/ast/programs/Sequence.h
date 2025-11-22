#pragma once
#include <sstream>

#include "z3++.h"
#include "ast/Program.h"

namespace ast {
class Sequence final : public Program {
public:
  const std::vector<std::shared_ptr<const Program> > programs;

  explicit Sequence(const std::vector<std::shared_ptr<const Program> > &programs)
      : programs(programs) {
  }

  [[nodiscard]] z3::expr wp(const z3::expr& postcondition) const override {
    z3::expr tmp = postcondition;
    //verkettung von statements
    //von hinten nach vorne prüfen: rückwärts iteration
    for (auto i = programs.rbegin(); i != programs.rend(); ++i) {
      //für objekt i der iteration wp berechnen
      tmp = (*i)->wp(tmp);
    }
    return tmp;
  }

  [[nodiscard]] std::string toString(const int indent) const override {
    std::stringstream ss;
    for (const auto &program: programs) {
      ss << program->toString(indent);
      if (program != programs.back()) {
        ss << "\n";
      }
    }
    return ss.str();
  }
};
}
