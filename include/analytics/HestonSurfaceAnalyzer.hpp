#pragma once

#include "market/OptionQuote.hpp"
#include "models/HestonParams.hpp"

#include <string>
#include <vector>

namespace quant::analytics {

struct HestonSurfacePoint {
    double maturity;
    double strike;
    double market_price;
    double market_iv;
    double heston_price;
    double heston_iv;
    double iv_error;
    quant::core::OptionType option_type;
};

class HestonSurfaceAnalyzer {
public:
    static std::vector<HestonSurfacePoint> analyze(
        const std::vector<quant::market::OptionQuoteWithIV>& market_quotes,
        double spot,
        double rate,
        double dividend_yield,
        const quant::models::HestonParams& params
    );

    static void export_results(
        const std::vector<HestonSurfacePoint>& results,
        const std::string& filepath
    );
};

} // namespace quant::analytics