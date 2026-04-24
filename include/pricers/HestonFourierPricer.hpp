#pragma once

#include "models/HestonParams.hpp"
#include "core/Types.hpp"

#include <complex>

namespace quant::pricers {

class HestonFourierPricer {
public:
    static double price(
        double spot,
        double strike,
        double rate,
        double dividend_yield,
        double maturity,
        const quant::models::HestonParams& params,
        quant::core::OptionType option_type
    );

    static std::complex<double> characteristic_function(
        std::complex<double> u,
        double spot,
        double rate,
        double dividend_yield,
        double maturity,
        const quant::models::HestonParams& params
    );

private:
    static double probability_p1(
        double spot,
        double strike,
        double rate,
        double dividend_yield,
        double maturity,
        const quant::models::HestonParams& params
    );

    static double probability_p2(
        double spot,
        double strike,
        double rate,
        double dividend_yield,
        double maturity,
        const quant::models::HestonParams& params
    );

    static double integrate_simpson(
        double (*integrand)(
            double,
            double,
            double,
            double,
            double,
            double,
            const quant::models::HestonParams&
        ),
        double spot,
        double strike,
        double rate,
        double dividend_yield,
        double maturity,
        const quant::models::HestonParams& params,
        double upper_bound,
        int n
    );
};

} // namespace quant::pricers