#include "market/MarketSurface.hpp"

#include <cmath>
#include <algorithm>

namespace quant::market {

void MarketSurface::add_quote(const OptionQuote& quote) {
    quotes_.push_back(quote);
}

const std::vector<OptionQuote>& MarketSurface::quotes() const {
    return quotes_;
}

std::vector<OptionQuote> MarketSurface::quotes_for_maturity(double maturity, double tolerance) const {
    std::vector<OptionQuote> result;
    for (const auto& q : quotes_) {
        if (std::fabs(q.maturity - maturity) <= tolerance) {
            result.push_back(q);
        }
    }
    return result;
}

std::vector<double> MarketSurface::unique_maturities(double tolerance) const {
    std::vector<double> maturities;

    for (const auto& q : quotes_) {
        bool found = false;
        for (double m : maturities) {
            if (std::fabs(q.maturity - m) <= tolerance) {
                found = true;
                break;
            }
        }
        if (!found) {
            maturities.push_back(q.maturity);
        }
    }

    std::sort(maturities.begin(), maturities.end());
    return maturities;
}

bool MarketSurface::empty() const {
    return quotes_.empty();
}

std::size_t MarketSurface::size() const {
    return quotes_.size();
}

} // namespace quant::market