#pragma once

#include "market/MarketSurface.hpp"
#include <vector>

namespace quant::analytics {

class ImpliedVolatilitySurfaceBuilder {
public:
    static std::vector<quant::market::OptionQuoteWithIV> build(
        const quant::market::MarketSurface& surface,
        double spot,
        double rate,
        double dividend_yield,
        double initial_guess = 0.20
    );
};

} // namespace quant::analytics