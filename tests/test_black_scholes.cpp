#include "models/BlackScholes.hpp"
#include "calibration/ImpliedVolSolver.hpp"

#include <gtest/gtest.h>
#include <cmath>

TEST(BlackScholesTest, CallPutParityHolds) {
    using quant::core::BlackScholesInputs;
    using quant::core::OptionType;

    BlackScholesInputs call_input {100.0, 100.0, 0.05, 0.00, 0.20, 1.0, OptionType::Call};
    BlackScholesInputs put_input  {100.0, 100.0, 0.05, 0.00, 0.20, 1.0, OptionType::Put};

    const double call_price = quant::models::BlackScholes::price(call_input);
    const double put_price  = quant::models::BlackScholes::price(put_input);

    const double lhs = call_price - put_price;
    const double rhs = 100.0 - 100.0 * std::exp(-0.05 * 1.0);

    EXPECT_NEAR(lhs, rhs, 1e-10);
}

TEST(BlackScholesTest, ImpliedVolRecoversOriginalVol) {
    using quant::core::BlackScholesInputs;
    using quant::core::OptionType;

    BlackScholesInputs input {100.0, 110.0, 0.02, 0.00, 0.35, 1.5, OptionType::Call};

    const double market_price = quant::models::BlackScholes::price(input);

    BlackScholesInputs iv_input = input;
    iv_input.volatility = 0.0;

    const double implied_vol =
        quant::calibration::ImpliedVolSolver::solve(iv_input, market_price, 0.20);

    EXPECT_NEAR(implied_vol, 0.35, 1e-7);
}

TEST(BlackScholesTest, GammaIsPositive) {
    using quant::core::BlackScholesInputs;
    using quant::core::OptionType;

    BlackScholesInputs input {100.0, 100.0, 0.01, 0.00, 0.25, 0.5, OptionType::Call};

    const double gamma = quant::models::BlackScholes::gamma(input);

    EXPECT_GT(gamma, 0.0);
}