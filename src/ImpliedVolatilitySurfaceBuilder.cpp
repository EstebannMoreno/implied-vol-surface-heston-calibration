#include "analytics/ImpliedVolatilitySurfaceBuilder.hpp"
#include "calibration/ImpliedVolSolver.hpp"

#include <stdexcept>

namespace quant::analytics {

std::vector<quant::market::OptionQuoteWithIV> ImpliedVolatilitySurfaceBuilder::build(
    const quant::market::MarketSurface& surface,
    double spot,
    double rate,
    double dividend_yield,
    double initial_guess
) {
    if (surface.empty()) {
        throw std::runtime_error("Cannot build implied vol surface from an empty market surface.");
    }

    std::vector<quant::market::OptionQuoteWithIV> result;
    result.reserve(surface.size());

    for (const auto& q : surface.quotes()) {
        quant::core::BlackScholesInputs inputs;
        inputs.spot = spot;
        inputs.strike = q.strike;
        inputs.rate = rate;
        inputs.dividend_yield = dividend_yield;
        inputs.volatility = 0.0;
        inputs.maturity = q.maturity;
        inputs.option_type = q.option_type;

        const double iv = quant::calibration::ImpliedVolSolver::solve(
            inputs,
            q.market_price,
            initial_guess
        );

        result.push_back({
            q.maturity,
            q.strike,
            q.market_price,
            iv,
            q.option_type
        });
    }

    return result;
}

} // namespace quant::analytics