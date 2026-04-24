#include "analytics/HestonSurfaceAnalyzer.hpp"
#include "pricers/HestonFourierPricer.hpp"
#include "calibration/ImpliedVolSolver.hpp"

#include <fstream>
#include <stdexcept>
#include <cmath>

namespace quant::analytics {

std::vector<HestonSurfacePoint> HestonSurfaceAnalyzer::analyze(
    const std::vector<quant::market::OptionQuoteWithIV>& market_quotes,
    double spot,
    double rate,
    double dividend_yield,
    const quant::models::HestonParams& params
) {
    std::vector<HestonSurfacePoint> results;
    results.reserve(market_quotes.size());

    for (const auto& q : market_quotes) {
        const double heston_price = quant::pricers::HestonFourierPricer::price(
            spot,
            q.strike,
            rate,
            dividend_yield,
            q.maturity,
            params,
            q.option_type
        );

        quant::core::BlackScholesInputs iv_input {
            spot,
            q.strike,
            rate,
            dividend_yield,
            0.0,
            q.maturity,
            q.option_type
        };

        double heston_iv = 0.0;

        try {
            heston_iv = quant::calibration::ImpliedVolSolver::solve(
                iv_input,
                heston_price,
                q.implied_vol
            );
        }
        catch (...) {
            heston_iv = std::nan("");
        }

        results.push_back({
            q.maturity,
            q.strike,
            q.market_price,
            q.implied_vol,
            heston_price,
            heston_iv,
            heston_iv - q.implied_vol,
            q.option_type
        });
    }

    return results;
}

void HestonSurfaceAnalyzer::export_results(
    const std::vector<HestonSurfacePoint>& results,
    const std::string& filepath
) {
    std::ofstream out(filepath);

    if (!out.is_open()) {
        throw std::runtime_error("Failed to open Heston analysis output file: " + filepath);
    }

    out << "maturity,strike,market_price,market_iv,heston_price,heston_iv,iv_error,option_type\n";

    for (const auto& r : results) {
        out << r.maturity << ","
            << r.strike << ","
            << r.market_price << ","
            << r.market_iv << ","
            << r.heston_price << ","
            << r.heston_iv << ","
            << r.iv_error << ","
            << (r.option_type == quant::core::OptionType::Call ? "C" : "P")
            << "\n";
    }
}

} // namespace quant::analytics