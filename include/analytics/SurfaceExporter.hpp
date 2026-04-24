#pragma once

#include "market/OptionQuote.hpp"
#include <string>
#include <vector>

namespace quant::analytics {

class SurfaceExporter {
public:
    static void export_implied_vols(
        const std::vector<quant::market::OptionQuoteWithIV>& quotes_with_iv,
        const std::string& filepath
    );
};

} // namespace quant::analytics