#include "calibration/HestonCalibrator.hpp"
#include "analytics/HestonSurfaceAnalyzer.hpp"

#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>

namespace quant::calibration {

using quant::models::HestonParams;

HestonParams HestonCalibrator::clamp_params(const HestonParams& p) {
    HestonParams out = p;

    out.v0    = std::clamp(out.v0,    0.005, 0.50);
    out.kappa = std::clamp(out.kappa, 0.100, 10.0);
    out.theta = std::clamp(out.theta, 0.005, 0.50);
    out.sigma = std::clamp(out.sigma, 0.010, 2.00);
    out.rho   = std::clamp(out.rho,  -0.95, 0.95);

    return out;
}

double HestonCalibrator::objective_rmse(
    const std::vector<quant::market::OptionQuoteWithIV>& market_quotes,
    double spot,
    double rate,
    double dividend_yield,
    const HestonParams& params
) {
    const auto safe_params = clamp_params(params);

    auto results = quant::analytics::HestonSurfaceAnalyzer::analyze(
        market_quotes,
        spot,
        rate,
        dividend_yield,
        safe_params
    );

    double mse = 0.0;
    int count = 0;

    for (const auto& r : results) {
        if (std::isfinite(r.heston_iv) && std::isfinite(r.iv_error)) {
            mse += r.iv_error * r.iv_error;
            count++;
        }
    }

    if (count == 0) {
        return std::numeric_limits<double>::infinity();
    }

    return std::sqrt(mse / count);
}

HestonCalibrationResult HestonCalibrator::calibrate(
    const std::vector<quant::market::OptionQuoteWithIV>& market_quotes,
    double spot,
    double rate,
    double dividend_yield,
    const HestonParams& initial_params,
    int max_iterations
) {
    HestonParams best = clamp_params(initial_params);

    double best_rmse = objective_rmse(
        market_quotes,
        spot,
        rate,
        dividend_yield,
        best
    );

    HestonParams step {
        0.02,   // v0
        0.50,   // kappa
        0.02,   // theta
        0.10,   // sigma
        0.10    // rho
    };

    int iterations = 0;

    for (int iter = 0; iter < max_iterations; ++iter) {
        bool improved = false;

        double* params_array[5] = {
            &best.v0,
            &best.kappa,
            &best.theta,
            &best.sigma,
            &best.rho
        };

        double steps_array[5] = {
            step.v0,
            step.kappa,
            step.theta,
            step.sigma,
            step.rho
        };

        for (int i = 0; i < 5; ++i) {
            for (double direction : {-1.0, 1.0}) {
                HestonParams candidate = best;

                double* candidate_array[5] = {
                    &candidate.v0,
                    &candidate.kappa,
                    &candidate.theta,
                    &candidate.sigma,
                    &candidate.rho
                };

                *candidate_array[i] += direction * steps_array[i];
                candidate = clamp_params(candidate);

                const double rmse = objective_rmse(
                    market_quotes,
                    spot,
                    rate,
                    dividend_yield,
                    candidate
                );

                if (rmse < best_rmse) {
                    best = candidate;
                    best_rmse = rmse;
                    improved = true;
                }
            }
        }

        iterations++;

        std::cout << "Iteration " << iter + 1
                  << " | RMSE = " << best_rmse
                  << " | v0=" << best.v0
                  << " kappa=" << best.kappa
                  << " theta=" << best.theta
                  << " sigma=" << best.sigma
                  << " rho=" << best.rho
                  << "\n";

        if (!improved) {
            step.v0    *= 0.5;
            step.kappa *= 0.5;
            step.theta *= 0.5;
            step.sigma *= 0.5;
            step.rho   *= 0.5;
        }

        if (step.v0 < 1e-4 &&
            step.kappa < 1e-3 &&
            step.theta < 1e-4 &&
            step.sigma < 1e-3 &&
            step.rho < 1e-3) {
            break;
        }
    }

    return {best, best_rmse, iterations};
}

} // namespace quant::calibration