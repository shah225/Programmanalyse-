#pragma once
#include <string>

namespace ast {
    typedef std::vector<Identifier> IdentifierList;
    typedef std::pair<PrimitiveType, Identifier> TypedIdentifier;
    typedef std::vector<TypedIdentifier> TypedIdentifierList;
    typedef std::tuple<Identifier, TypedIdentifierList, TypedIdentifierList> Signature;

    class FunctionDefinition final {
    public:
        const Identifier identifier;
        const TypedIdentifierList arguments;
        const std::shared_ptr<const Program> body;

        FunctionDefinition(Identifier identifier,
                           TypedIdentifierList arguments,
                           const std::shared_ptr<const Program> &body)
            : identifier(std::move(identifier)),
              arguments(std::move(arguments)),
              body(body) {
        }

        [[nodiscard]] std::string toString(const int indent) const {
            std::stringstream ss;
            ss << std::string(4 * indent, ' ') << "function " << identifier << "(";
            for (const auto &argument: arguments) {
                const auto &[type, identifier] = argument;
                ss << to_string(type) << " " << identifier;
                if (argument != arguments.back()) {
                    ss << ", ";
                }
            }
            ss << ") {\n";
            ss << body->toString(indent + 1) << "\n";
            ss << std::string(4 * indent, ' ') << "}\n";
            return ss.str();
        }
    };
}
