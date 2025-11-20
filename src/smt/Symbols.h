#pragma once
#include <map>

#include "SMT.h"

enum PrimitiveType { integer, boolean };

typedef std::string Identifier;

inline std::map<Identifier, std::shared_ptr<z3::expr> > symbolTable;
inline std::map<Identifier, PrimitiveType> symbolTypes;

inline std::shared_ptr<z3::expr> identifierToSymbol(const Identifier &identifier) {
    return symbolTable.at(identifier);
}

// overwrites symbol if already exists
inline void newSymbol(const Identifier &identifier, const PrimitiveType type) {
    switch (type) {
        case integer:
            symbolTable[identifier] = std::make_shared<z3::expr>(z3Ctx.int_const(identifier.c_str()));
            symbolTypes[identifier] = integer;
            break;
        case boolean:
            symbolTable[identifier] = std::make_shared<z3::expr>(z3Ctx.bool_const(identifier.c_str()));
            symbolTypes[identifier] = boolean;
            break;
        default: throw std::logic_error("unreachable");
    }
}

inline std::string to_string(const PrimitiveType type) {
    switch (type) {
        case integer: return "int";
        case boolean: return "bool";
        default: throw std::logic_error("unreachable");
    }
}
