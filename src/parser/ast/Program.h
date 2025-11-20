#pragma once
#include <iostream>
#include <SMT.h>

namespace ast {
    class Program {
    public:
        virtual ~Program() = default;

        [[nodiscard]] virtual std::string toString(int indent) const = 0;

        void print() const { std::cout << toString(0); }
    };
}


