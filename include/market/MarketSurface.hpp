#pragma once

#include "market/OptionQuote.hpp"
#include <vector>

namespace quant::market {

class MarketSurface {
public:
    void add_quote(const OptionQuote& quote);
    const std::vector<OptionQuote>& quotes() const;

    std::vector<OptionQuote> quotes_for_maturity(double maturity, double tolerance = 1e-10) const;
    std::vector<double> unique_maturities(double tolerance = 1e-10) const;

    bool empty() const;
    std::size_t size() const;

private:
    std::vector<OptionQuote> quotes_;
};

} // namespace quant::market