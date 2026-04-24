#include "models/BlackScholes.hpp"
#include "calibration/ImpliedVolSolver.hpp"

#include <iostream>
#include <iomanip>

int main() {
    using quant::core::BlackScholesInputs;
    using quant::core::OptionType;

    BlackScholesInputs call_input {
        100.0,   // spot
        100.0,   // strike
        0.03,    // rate
        0.00,    // dividend_yield
        0.20,    // volatility
        1.0,     // maturity
        OptionType::Call
    };

    const double call_price = quant::models::BlackScholes::price(call_input);
    const double delta = quant::models::BlackScholes::delta(call_input);
    const double gamma = quant::models::BlackScholes::gamma(call_input);
    const double vega  = quant::models::BlackScholes::vega(call_input);

    BlackScholesInputs iv_input = call_input;
    iv_input.volatility = 0.0;

    const double implied_vol =
        quant::calibration::ImpliedVolSolver::solve(iv_input, call_price, 0.25);

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "=== Black-Scholes Demo ===\n";
    std::cout << "Call Price   : " << call_price << "\n";
    std::cout << "Delta        : " << delta << "\n";
    std::cout << "Gamma        : " << gamma << "\n";
    std::cout << "Vega         : " << vega << "\n";
    std::cout << "Implied Vol  : " << implied_vol << "\n";

    return 0;
}