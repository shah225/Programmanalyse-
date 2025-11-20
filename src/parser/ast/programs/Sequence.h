#pragma once
#include <sstream>

namespace ast {
    class Sequence final : public Program {
    public:
        const std::vector<std::shared_ptr<const Program> > programs;

        explicit Sequence(const std::vector<std::shared_ptr<const Program> > &programs)
            : programs(programs) {
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
