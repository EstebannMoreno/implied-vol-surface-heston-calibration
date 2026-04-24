#include "market/CsvReader.hpp"
#include "analytics/ImpliedVolatilitySurfaceBuilder.hpp"
#include "analytics/SurfaceExporter.hpp"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>

int main(int argc, char** argv) {
    try {
        const std::string input_path =
            (argc > 1) ? argv[1] : "data/sample_option_chain_rich.csv";

        const std::string output_path =
            (argc > 2) ? argv[2] : "data/output/implied_vols.csv";

        const double spot = 100.0;
        const double rate = 0.03;
        const double dividend_yield = 0.00;

        auto surface = quant::market::CsvReader::read_option_quotes(input_path);

        auto quotes_with_iv = quant::analytics::ImpliedVolatilitySurfaceBuilder::build(
            surface,
            spot,
            rate,
            dividend_yield,
            0.20
        );

        std::sort(quotes_with_iv.begin(), quotes_with_iv.end(),
                  [](const auto& a, const auto& b) {
                      if (a.maturity != b.maturity) {
                          return a.maturity < b.maturity;
                      }
                      return a.strike < b.strike;
                  });

        quant::analytics::SurfaceExporter::export_implied_vols(quotes_with_iv, output_path);

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "=== Market Analysis V3 ===\n";
        std::cout << "Input file    : " << input_path << "\n";
        std::cout << "Output file   : " << output_path << "\n";
        std::cout << "Loaded quotes : " << surface.size() << "\n\n";

        const auto maturities = surface.unique_maturities();
        std::cout << "Available maturities:\n";
        for (double m : maturities) {
            std::cout << "  - " << m << "\n";
        }

        std::cout << "\nMaturity    Strike      Price       ImpliedVol   Type\n";
        std::cout << "--------------------------------------------------------\n";

        for (const auto& q : quotes_with_iv) {
            std::cout
                << std::setw(8)  << q.maturity     << "    "
                << std::setw(8)  << q.strike       << "    "
                << std::setw(10) << q.market_price << "    "
                << std::setw(10) << q.implied_vol  << "    "
                << (q.option_type == quant::core::OptionType::Call ? "Call" : "Put")
                << "\n";
        }

        std::cout << "\nExport completed.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}