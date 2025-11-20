#pragma once
#include <sstream>

#include "Symbols.h"
#include "../programs/Statement.h"

namespace ast {
    class Assignment final : public Statement {
    public:
        const Identifier identifier;
        const std::shared_ptr<Expression> expression;

        Assignment(Identifier identifier, std::shared_ptr<Expression> expression)
            : identifier(std::move(identifier)),
              expression(std::move(expression)) {
        }

        [[nodiscard]] std::string toString(const int indent) const override {
            std::stringstream ss;
            ss << std::string(4 * indent, ' ') << identifier << " := " << expression->toString() << ";";
            return ss.str();
        }
    };
}


