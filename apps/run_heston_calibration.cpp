#include "market/CsvReader.hpp"
#include "analytics/ImpliedVolatilitySurfaceBuilder.hpp"
#include "analytics/HestonSurfaceAnalyzer.hpp"
#include "calibration/HestonCalibrator.hpp"
#include "models/HestonParams.hpp"

#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char** argv) {
    try {
        const std::string input_path =
            (argc > 1) ? argv[1] : "data/sample_option_chain_rich.csv";

        const std::string output_path =
            (argc > 2) ? argv[2] : "data/output/heston_calibrated_surface.csv";

        const double spot = 100.0;
        const double rate = 0.03;
        const double dividend_yield = 0.00;

        auto surface = quant::market::CsvReader::read_option_quotes(input_path);

        auto market_quotes = quant::analytics::ImpliedVolatilitySurfaceBuilder::build(
            surface,
            spot,
            rate,
            dividend_yield,
            0.20
        );

        quant::models::HestonParams initial {
            0.04,   // v0
            1.50,   // kappa
            0.04,   // theta
            0.30,   // sigma
            -0.50   // rho
        };

        std::cout << std::fixed << std::setprecision(8);
        std::cout << "=== Heston Calibration V6 ===\n";
        std::cout << "Input file  : " << input_path << "\n";
        std::cout << "Output file : " << output_path << "\n\n";

        auto result = quant::calibration::HestonCalibrator::calibrate(
            market_quotes,
            spot,
            rate,
            dividend_yield,
            initial,
            30
        );

        std::cout << "\n=== Final Calibrated Parameters ===\n";
        std::cout << "v0    = " << result.params.v0 << "\n";
        std::cout << "kappa = " << result.params.kappa << "\n";
        std::cout << "theta = " << result.params.theta << "\n";
        std::cout << "sigma = " << result.params.sigma << "\n";
        std::cout << "rho   = " << result.params.rho << "\n";
        std::cout << "RMSE  = " << result.rmse << "\n";
        std::cout << "Iterations = " << result.iterations << "\n";

        auto calibrated_surface = quant::analytics::HestonSurfaceAnalyzer::analyze(
            market_quotes,
            spot,
            rate,
            dividend_yield,
            result.params
        );

        quant::analytics::HestonSurfaceAnalyzer::export_results(
            calibrated_surface,
            output_path
        );

        std::cout << "\nExport completed.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}