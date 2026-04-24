#pragma once

#include "core/Types.hpp"

namespace quant::calibration {

class ImpliedVolSolver {
public:
    static double solve(
        const quant::core::BlackScholesInputs& inputs_without_vol,
        double market_price,
        double initial_guess = 0.20,
        double tolerance = 1e-8,
        int max_iterations = 100
    );
};

} // namespace quant::calibration