#include "market/CsvReader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cctype>

namespace quant::market {

namespace {
std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }

    std::size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }

    return s.substr(start, end - start);
}

quant::core::OptionType parse_option_type(const std::string& token) {
    const std::string t = trim(token);

    if (t == "C" || t == "CALL" || t == "Call" || t == "call") {
        return quant::core::OptionType::Call;
    }
    if (t == "P" || t == "PUT" || t == "Put" || t == "put") {
        return quant::core::OptionType::Put;
    }

    throw std::runtime_error("Unknown option type in CSV: " + token);
}
}

MarketSurface CsvReader::read_option_quotes(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file: " + filepath);
    }

    MarketSurface surface;
    std::string line;

    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (trim(line).empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string maturity_str, strike_str, price_str, type_str;

        std::getline(ss, maturity_str, ',');
        std::getline(ss, strike_str, ',');
        std::getline(ss, price_str, ',');
        std::getline(ss, type_str, ',');

        OptionQuote quote;
        quote.maturity = std::stod(trim(maturity_str));
        quote.strike = std::stod(trim(strike_str));
        quote.market_price = std::stod(trim(price_str));
        quote.option_type = parse_option_type(type_str);

        surface.add_quote(quote);
    }

    return surface;
}

} // namespace quant::market