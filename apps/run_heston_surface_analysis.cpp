#include "market/CsvReader.hpp"
#include "analytics/ImpliedVolatilitySurfaceBuilder.hpp"
#include "analytics/HestonSurfaceAnalyzer.hpp"
#include "models/HestonParams.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <cmath>

int main(int argc, char** argv) {
    try {
        const std::string input_path =
            (argc > 1) ? argv[1] : "data/sample_option_chain_rich.csv";

        const std::string output_path =
            (argc > 2) ? argv[2] : "data/output/heston_surface_analysis.csv";

        const double spot = 100.0;
        const double rate = 0.03;
        const double dividend_yield = 0.00;

        quant::models::HestonParams params {
            0.04,   // v0
            2.0,    // kappa
            0.04,   // theta
            0.30,   // sigma
            -0.70   // rho
        };

        auto surface = quant::market::CsvReader::read_option_quotes(input_path);

        auto market_quotes = quant::analytics::ImpliedVolatilitySurfaceBuilder::build(
            surface,
            spot,
            rate,
            dividend_yield,
            0.20
        );

        auto results = quant::analytics::HestonSurfaceAnalyzer::analyze(
            market_quotes,
            spot,
            rate,
            dividend_yield,
            params
        );

        std::sort(results.begin(), results.end(),
                  [](const auto& a, const auto& b) {
                      if (a.maturity != b.maturity) {
                          return a.maturity < b.maturity;
                      }
                      return a.strike < b.strike;
                  });

        quant::analytics::HestonSurfaceAnalyzer::export_results(results, output_path);

        double mse = 0.0;
        int count = 0;

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "=== Heston Surface Analysis V5 ===\n";
        std::cout << "Input file  : " << input_path << "\n";
        std::cout << "Output file : " << output_path << "\n\n";

        std::cout << "Parameters:\n";
        std::cout << "v0    = " << params.v0 << "\n";
        std::cout << "kappa = " << params.kappa << "\n";
        std::cout << "theta = " << params.theta << "\n";
        std::cout << "sigma = " << params.sigma << "\n";
        std::cout << "rho   = " << params.rho << "\n\n";

        std::cout << "Maturity    Strike      MarketIV    HestonIV    Error\n";
        std::cout << "----------------------------------------------------------\n";

        for (const auto& r : results) {
            if (!std::isnan(r.heston_iv)) {
                mse += r.iv_error * r.iv_error;
                count++;
            }

            std::cout
                << std::setw(8)  << r.maturity   << "    "
                << std::setw(8)  << r.strike     << "    "
                << std::setw(8)  << r.market_iv  << "    "
                << std::setw(8)  << r.heston_iv  << "    "
                << std::setw(8)  << r.iv_error   << "\n";
        }

        if (count > 0) {
            const double rmse = std::sqrt(mse / count);
            std::cout << "\nIV RMSE: " << rmse << "\n";
        }

        std::cout << "\nExport completed.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}