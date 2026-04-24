#pragma once

namespace quant::models {

struct HestonParams {
    double v0;     // initial variance
    double kappa;  // mean reversion speed
    double theta;  // long-run variance
    double sigma;  // vol of variance
    double rho;    // correlation
};

} // namespace quant::models