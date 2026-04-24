#include "calibration/ImpliedVolSolver.hpp"
#include "models/BlackScholes.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace quant::calibration {

using quant::core::BlackScholesInputs;

double ImpliedVolSolver::solve(
    const BlackScholesInputs& inputs_without_vol,
    double market_price,
    double initial_guess,
    double tolerance,
    int max_iterations
) {
    if (market_price <= 0.0) {
        throw std::invalid_argument("Market price must be positive.");
    }

    BlackScholesInputs inputs = inputs_without_vol;
    double sigma = std::max(1e-4, initial_guess);

    for (int i = 0; i < max_iterations; ++i) {
        inputs.volatility = sigma;

        const double model_price = quant::models::BlackScholes::price(inputs);
        const double diff = model_price - market_price;

        if (std::fabs(diff) < tolerance) {
            return sigma;
        }

        const double vega = quant::models::BlackScholes::vega(inputs);

        if (std::fabs(vega) < 1e-10) {
            break;
        }

        sigma -= diff / vega;
        sigma = std::clamp(sigma, 1e-6, 5.0);
    }

    double low = 1e-6;
    double high = 5.0;

    for (int i = 0; i < max_iterations; ++i) {
        const double mid = 0.5 * (low + high);
        inputs.volatility = mid;

        const double price_mid = quant::models::BlackScholes::price(inputs);
        const double diff_mid = price_mid - market_price;

        if (std::fabs(diff_mid) < tolerance) {
            return mid;
        }

        inputs.volatility = low;
        const double price_low = quant::models::BlackScholes::price(inputs);
        const double diff_low = price_low - market_price;

        if (diff_low * diff_mid <= 0.0) {
            high = mid;
        } else {
            low = mid;
        }
    }

    throw std::runtime_error("Implied volatility solver did not converge.");
}
} // namespace quant::calibration