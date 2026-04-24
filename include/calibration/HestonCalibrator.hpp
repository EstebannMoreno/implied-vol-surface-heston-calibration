#pragma once

#include "market/OptionQuote.hpp"
#include "models/HestonParams.hpp"

#include <vector>

namespace quant::calibration {

struct HestonCalibrationResult {
    quant::models::HestonParams params;
    double rmse;
    int iterations;
};

class HestonCalibrator {
public:
    static HestonCalibrationResult calibrate(
        const std::vector<quant::market::OptionQuoteWithIV>& market_quotes,
        double spot,
        double rate,
        double dividend_yield,
        const quant::models::HestonParams& initial_params,
        int max_iterations = 30
    );

    static double objective_rmse(
        const std::vector<quant::market::OptionQuoteWithIV>& market_quotes,
        double spot,
        double rate,
        double dividend_yield,
        const quant::models::HestonParams& params
    );

private:
    static quant::models::HestonParams clamp_params(
        const quant::models::HestonParams& params
    );
};

} // namespace quant::calibration