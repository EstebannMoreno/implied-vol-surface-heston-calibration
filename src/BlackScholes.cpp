#include "models/BlackScholes.hpp"
#include "core/Constants.hpp"

#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace quant::models {

using quant::core::BlackScholesInputs;
using quant::core::OptionType;

double BlackScholes::normal_pdf(double x) {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * quant::core::PI);
}

double BlackScholes::normal_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double BlackScholes::d1(const BlackScholesInputs& inputs) {
    if (inputs.spot <= 0.0 || inputs.strike <= 0.0 || inputs.volatility <= 0.0 || inputs.maturity <= 0.0) {
        throw std::invalid_argument("Invalid Black-Scholes inputs in d1.");
    }

    const double numerator =
        std::log(inputs.spot / inputs.strike) +
        (inputs.rate - inputs.dividend_yield + 0.5 * inputs.volatility * inputs.volatility) * inputs.maturity;

    const double denominator = inputs.volatility * std::sqrt(inputs.maturity);

    return numerator / denominator;
}

double BlackScholes::d2(const BlackScholesInputs& inputs) {
    return d1(inputs) - inputs.volatility * std::sqrt(inputs.maturity);
}

double BlackScholes::price(const BlackScholesInputs& inputs) {
    if (inputs.spot <= 0.0 || inputs.strike <= 0.0) {
        throw std::invalid_argument("Spot and strike must be positive.");
    }

    if (inputs.maturity <= 0.0) {
        if (inputs.option_type == OptionType::Call) {
            return std::max(inputs.spot - inputs.strike, 0.0);
        }
        return std::max(inputs.strike - inputs.spot, 0.0);
    }

    if (inputs.volatility <= 0.0) {
        const double forward_intrinsic =
            std::exp(-inputs.rate * inputs.maturity) *
            ((inputs.option_type == OptionType::Call)
                ? std::max(inputs.spot * std::exp((inputs.rate - inputs.dividend_yield) * inputs.maturity) - inputs.strike, 0.0)
                : std::max(inputs.strike - inputs.spot * std::exp((inputs.rate - inputs.dividend_yield) * inputs.maturity), 0.0));
        return forward_intrinsic;
    }

    const double d1_val = d1(inputs);
    const double d2_val = d2(inputs);

    const double discounted_spot = inputs.spot * std::exp(-inputs.dividend_yield * inputs.maturity);
    const double discounted_strike = inputs.strike * std::exp(-inputs.rate * inputs.maturity);

    if (inputs.option_type == OptionType::Call) {
        return discounted_spot * normal_cdf(d1_val) - discounted_strike * normal_cdf(d2_val);
    }

    return discounted_strike * normal_cdf(-d2_val) - discounted_spot * normal_cdf(-d1_val);
}

double BlackScholes::delta(const BlackScholesInputs& inputs) {
    const double d1_val = d1(inputs);
    const double dq = std::exp(-inputs.dividend_yield * inputs.maturity);

    if (inputs.option_type == OptionType::Call) {
        return dq * normal_cdf(d1_val);
    }

    return dq * (normal_cdf(d1_val) - 1.0);
}

double BlackScholes::gamma(const BlackScholesInputs& inputs) {
    const double d1_val = d1(inputs);
    return std::exp(-inputs.dividend_yield * inputs.maturity) * normal_pdf(d1_val)
           / (inputs.spot * inputs.volatility * std::sqrt(inputs.maturity));
}

double BlackScholes::vega(const BlackScholesInputs& inputs) {
    const double d1_val = d1(inputs);
    return inputs.spot * std::exp(-inputs.dividend_yield * inputs.maturity)
           * normal_pdf(d1_val) * std::sqrt(inputs.maturity);
}

} // namespace quant::models