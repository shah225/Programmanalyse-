#pragma once
#include "../programs/Statement.h"

class Return final : public ast::Statement {
public:
    [[nodiscard]] std::string toString(const int indent) const override {
        std::stringstream ss;
        ss << std::string(4 * indent, ' ') << "return;";
        return ss.str();
    }
};
