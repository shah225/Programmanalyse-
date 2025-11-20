#pragma once
#include <z3++.h>

inline z3::context z3Ctx;
inline z3::solver solver(z3Ctx);

inline bool isSatisfiable(const z3::expr &expression) {
    solver.push();
    solver.add(expression);
    const auto result = solver.check();
    solver.pop();
    return result == z3::sat;
}

inline bool isValid(const z3::expr &expression) {
    return !isSatisfiable(!expression);
}
