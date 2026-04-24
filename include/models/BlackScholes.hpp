#pragma once

#include "core/Types.hpp"

namespace quant::models {

class BlackScholes {
public:
    static double normal_pdf(double x);
    static double normal_cdf(double x);

    static double d1(const quant::core::BlackScholesInputs& inputs);
    static double d2(const quant::core::BlackScholesInputs& inputs);

    static double price(const quant::core::BlackScholesInputs& inputs);

    static double delta(const quant::core::BlackScholesInputs& inputs);
    static double gamma(const quant::core::BlackScholesInputs& inputs);
    static double vega(const quant::core::BlackScholesInputs& inputs);
};

} // namespace quant::models