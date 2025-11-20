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

        [[nodiscard]] std::string toString(const int indent) const override {
            std::stringstream ss;
            ss << std::string(4 * indent, ' ') << "assume(" << condition->toString() << ");";
            return ss.str();
        }
    };
}
