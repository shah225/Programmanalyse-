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

        [[nodiscard]] std::string toString(const int indent) const override {
            std::stringstream ss;
            ss << std::string(4 * indent, ' ') << "assert(" << condition->toString() << ");";
            return ss.str();
        }
    };
}
