#include "analytics/SurfaceExporter.hpp"

#include <fstream>
#include <stdexcept>

namespace quant::analytics {

void SurfaceExporter::export_implied_vols(
    const std::vector<quant::market::OptionQuoteWithIV>& quotes_with_iv,
    const std::string& filepath
) {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output CSV file: " + filepath);
    }

    out << "maturity,strike,market_price,implied_vol,option_type\n";

    for (const auto& q : quotes_with_iv) {
        out << q.maturity << ","
            << q.strike << ","
            << q.market_price << ","
            << q.implied_vol << ","
            << (q.option_type == quant::core::OptionType::Call ? "C" : "P")
            << "\n";
    }
}

} // namespace quant::analytics