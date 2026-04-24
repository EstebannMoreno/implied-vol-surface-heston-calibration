#include "pricers/HestonFourierPricer.hpp"

#include <cmath>
#include <complex>
#include <stdexcept>
#include <algorithm>

namespace quant::pricers {

using quant::core::OptionType;
using quant::models::HestonParams;

namespace {

constexpr double PI = 3.14159265358979323846;
const std::complex<double> I(0.0, 1.0);

void validate_heston_params(const HestonParams& p) {
    if (p.v0 <= 0.0) {
        throw std::invalid_argument("Heston v0 must be positive.");
    }
    if (p.kappa <= 0.0) {
        throw std::invalid_argument("Heston kappa must be positive.");
    }
    if (p.theta <= 0.0) {
        throw std::invalid_argument("Heston theta must be positive.");
    }
    if (p.sigma <= 0.0) {
        throw std::invalid_argument("Heston sigma must be positive.");
    }
    if (p.rho <= -1.0 || p.rho >= 1.0) {
        throw std::invalid_argument("Heston rho must be in (-1, 1).");
    }
}

double p1_integrand(
    double u,
    double spot,
    double strike,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params
) {
    const std::complex<double> uc(u, 0.0);

    const auto phi_u_minus_i =
        HestonFourierPricer::characteristic_function(
            uc - I, spot, rate, dividend_yield, maturity, params
        );

    const auto phi_minus_i =
        HestonFourierPricer::characteristic_function(
            -I, spot, rate, dividend_yield, maturity, params
        );

    const auto numerator =
        std::exp(-I * uc * std::log(strike)) * phi_u_minus_i;

    const auto denominator = I * uc * phi_minus_i;

    return std::real(numerator / denominator);
}

double p2_integrand(
    double u,
    double spot,
    double strike,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params
) {
    const std::complex<double> uc(u, 0.0);

    const auto phi_u =
        HestonFourierPricer::characteristic_function(
            uc, spot, rate, dividend_yield, maturity, params
        );

    const auto numerator =
        std::exp(-I * uc * std::log(strike)) * phi_u;

    const auto denominator = I * uc;

    return std::real(numerator / denominator);
}

} // namespace

std::complex<double> HestonFourierPricer::characteristic_function(
    std::complex<double> u,
    double spot,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params
) {
    validate_heston_params(params);

    if (spot <= 0.0 || maturity <= 0.0) {
        throw std::invalid_argument("Invalid spot or maturity in Heston characteristic function.");
    }

    const double v0 = params.v0;
    const double kappa = params.kappa;
    const double theta = params.theta;
    const double sigma = params.sigma;
    const double rho = params.rho;

    const std::complex<double> iu = I * u;

    const std::complex<double> a = kappa * theta;
    const std::complex<double> b = kappa;

    const std::complex<double> d =
        std::sqrt(
            std::pow(rho * sigma * iu - b, 2.0)
            + sigma * sigma * (iu + u * u)
        );

    const std::complex<double> g =
        (b - rho * sigma * iu - d)
        / (b - rho * sigma * iu + d);

    const std::complex<double> exp_minus_dT = std::exp(-d * maturity);

    const std::complex<double> C =
        iu * (std::log(spot) + (rate - dividend_yield) * maturity)
        + (a / (sigma * sigma))
        * (
            (b - rho * sigma * iu - d) * maturity
            - 2.0 * std::log((1.0 - g * exp_minus_dT) / (1.0 - g))
        );

    const std::complex<double> D =
        ((b - rho * sigma * iu - d) / (sigma * sigma))
        * ((1.0 - exp_minus_dT) / (1.0 - g * exp_minus_dT));

    return std::exp(C + D * v0);
}

double HestonFourierPricer::integrate_simpson(
    double (*integrand)(
        double,
        double,
        double,
        double,
        double,
        double,
        const HestonParams&
    ),
    double spot,
    double strike,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params,
    double upper_bound,
    int n
) {
    if (n <= 0 || n % 2 != 0) {
        throw std::invalid_argument("Simpson integration requires a positive even number of intervals.");
    }

    const double eps = 1e-8;
    const double a = eps;
    const double b = upper_bound;
    const double h = (b - a) / n;

    double sum = integrand(a, spot, strike, rate, dividend_yield, maturity, params)
               + integrand(b, spot, strike, rate, dividend_yield, maturity, params);

    for (int i = 1; i < n; ++i) {
        const double x = a + i * h;
        const double weight = (i % 2 == 0) ? 2.0 : 4.0;

        sum += weight * integrand(
            x,
            spot,
            strike,
            rate,
            dividend_yield,
            maturity,
            params
        );
    }

    return sum * h / 3.0;
}

double HestonFourierPricer::probability_p1(
    double spot,
    double strike,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params
) {
    const double integral = integrate_simpson(
        p1_integrand,
        spot,
        strike,
        rate,
        dividend_yield,
        maturity,
        params,
        100.0,
        10000
    );

    return 0.5 + integral / PI;
}

double HestonFourierPricer::probability_p2(
    double spot,
    double strike,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params
) {
    const double integral = integrate_simpson(
        p2_integrand,
        spot,
        strike,
        rate,
        dividend_yield,
        maturity,
        params,
        100.0,
        10000
    );

    return 0.5 + integral / PI;
}

double HestonFourierPricer::price(
    double spot,
    double strike,
    double rate,
    double dividend_yield,
    double maturity,
    const HestonParams& params,
    OptionType option_type
) {
    validate_heston_params(params);

    if (spot <= 0.0 || strike <= 0.0 || maturity <= 0.0) {
        throw std::invalid_argument("Invalid market inputs in Heston pricer.");
    }

    const double p1 = probability_p1(
        spot,
        strike,
        rate,
        dividend_yield,
        maturity,
        params
    );

    const double p2 = probability_p2(
        spot,
        strike,
        rate,
        dividend_yield,
        maturity,
        params
    );

    const double call =
        spot * std::exp(-dividend_yield * maturity) * p1
        - strike * std::exp(-rate * maturity) * p2;

    if (option_type == OptionType::Call) {
        return call;
    }

    const double put =
        call
        - spot * std::exp(-dividend_yield * maturity)
        + strike * std::exp(-rate * maturity);

    return put;
}

} // namespace quant::pricers