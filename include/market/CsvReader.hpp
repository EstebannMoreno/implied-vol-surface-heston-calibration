#pragma once

#include "market/MarketSurface.hpp"
#include <string>

namespace quant::market {

class CsvReader {
public:
    static MarketSurface read_option_quotes(const std::string& filepath);
};

} // namespace quant::market