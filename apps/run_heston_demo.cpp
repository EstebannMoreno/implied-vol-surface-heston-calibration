#include "models/BlackScholes.hpp"
#include "models/HestonParams.hpp"
#include "pricers/HestonFourierPricer.hpp"
#include "core/Types.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    using quant::core::BlackScholesInputs;
    using quant::core::OptionType;
    using quant::models::HestonParams;
    using quant::models::BlackScholes;
    using quant::pricers::HestonFourierPricer;

    const double spot = 100.0;
    const double strike = 100.0;
    const double rate = 0.03;
    const double dividend_yield = 0.0;
    const double maturity = 1.0;

    BlackScholesInputs bs_input {
        spot,
        strike,
        rate,
        dividend_yield,
        0.20,
        maturity,
        OptionType::Call
    };

    const double bs_price = BlackScholes::price(bs_input);

    HestonParams heston {
        0.04,   // v0
        2.0,    // kappa
        0.04,   // theta
        0.30,   // sigma
        -0.70   // rho
    };

    const double heston_call = HestonFourierPricer::price(
        spot,
        strike,
        rate,
        dividend_yield,
        maturity,
        heston,
        OptionType::Call
    );

    const double heston_put = HestonFourierPricer::price(
        spot,
        strike,
        rate,
        dividend_yield,
        maturity,
        heston,
        OptionType::Put
    );

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "=== Heston Fourier Demo ===\n";
    std::cout << "Spot        : " << spot << "\n";
    std::cout << "Strike      : " << strike << "\n";
    std::cout << "Maturity    : " << maturity << "\n";
    std::cout << "Rate        : " << rate << "\n";
    std::cout << "Dividend    : " << dividend_yield << "\n\n";

    std::cout << "Black-Scholes Call : " << bs_price << "\n";
    std::cout << "Heston Call        : " << heston_call << "\n";
    std::cout << "Heston Put         : " << heston_put << "\n";

    const double parity_lhs = heston_call - heston_put;
    const double parity_rhs =
        spot * std::exp(-dividend_yield * maturity)
        - strike * std::exp(-rate * maturity);

    std::cout << "\nPut-Call Parity Check:\n";
    std::cout << "Call - Put : " << parity_lhs << "\n";
    std::cout << "S e^-qT - K e^-rT : " << parity_rhs << "\n";

    return 0;
}