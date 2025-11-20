#pragma once
#include <sstream>

#include "../Expression.h"
#include "../Program.h"

namespace ast {
    class While final : public Program {
    public:
        const std::shared_ptr<Expression> condition;
        const std::shared_ptr<const Program> body;
        const std::shared_ptr<Expression> invariant;

        While(std::shared_ptr<Expression> condition, std::shared_ptr<const Sequence> body,
              std::shared_ptr<Expression> invariant)
            : condition(std::move(condition)),
              body(std::move(body)),
              invariant(std::move(invariant)) {
        }

        [[nodiscard]] std::string toString(const int indent) const override {
            std::stringstream ss;
            ss << std::string(4 * indent, ' ') << "while (" << condition->toString() << ") { \n";
            ss << std::string(4 * indent, ' ') << "[" << invariant->toString() << "]\n";
            ss << body->toString(indent + 1) << "\n";
            ss << std::string(4 * indent, ' ') << "}";
            return ss.str();
        }
    };
}
