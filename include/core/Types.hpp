#pragma once

namespace quant::core {

enum class OptionType {
    Call,
    Put
};

struct BlackScholesInputs {
    double spot;
    double strike;
    double rate;
    double dividend_yield;
    double volatility;
    double maturity;
    OptionType option_type;
};

} // namespace quant::core