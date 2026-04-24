#pragma once

#include "core/Types.hpp"

namespace quant::market {

struct OptionQuote {
    double maturity;
    double strike;
    double market_price;
    quant::core::OptionType option_type;
};

struct OptionQuoteWithIV {
    double maturity;
    double strike;
    double market_price;
    double implied_vol;
    quant::core::OptionType option_type;
};

} // namespace quant::market