#include "Boogie.h"

int main(const int argc, char *argv[]) {
    const auto program = parser::parseBoogie("benchmarks/addition.boogie");
    program->print();
}
