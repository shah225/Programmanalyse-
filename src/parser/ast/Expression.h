#pragma once

#include <SMT.h>

namespace ast {
    class Expression final {
    public:
        ~Expression() = default;

        const z3::expr expression;

        explicit Expression(z3::expr expression) : expression(std::move(expression)) {
        }

        [[nodiscard]] std::string toString() const { return expression.to_string(); }
    };
}
