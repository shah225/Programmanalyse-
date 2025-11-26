#pragma once
#include <iostream>
#include <../smt/SMT.h>

#include "z3++.h"

namespace ast {
class Program {
public:
  virtual ~Program() = default;

  [[nodiscard]] virtual std::string toString(int indent) const = 0;

  [[nodiscard]] virtual z3::expr wp(const z3::expr& postcondition) const {

  };

  void print() const { std::cout << toString(0); }
};
}


