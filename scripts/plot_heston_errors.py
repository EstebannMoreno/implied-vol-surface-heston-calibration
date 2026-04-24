import csv
import os
from collections import defaultdict
import matplotlib.pyplot as plt

INPUT_FILE = "data/output/heston_calibrated_surface.csv"
OUTPUT_FILE = "data/output/heston_iv_errors.png"

def load_data(filepath):
    grouped = defaultdict(list)

    with open(filepath, "r", newline="") as f:
        reader = csv.DictReader(f)

        for row in reader:
            maturity = float(row["maturity"])
            strike = float(row["strike"])
            market_iv = float(row["market_iv"])
            heston_iv = float(row["heston_iv"])
            iv_error = float(row["iv_error"])

            grouped[maturity].append((strike, market_iv, heston_iv, iv_error))

    for maturity in grouped:
        grouped[maturity].sort(key=lambda x: x[0])

    return grouped

def plot_iv_comparison(data):
    plt.figure(figsize=(10, 6))

    for maturity, points in sorted(data.items()):
        strikes = [p[0] for p in points]
        market_ivs = [p[1] for p in points]
        heston_ivs = [p[2] for p in points]

        plt.plot(strikes, market_ivs, marker="o", linestyle="-", label=f"Market T={maturity}")
        plt.plot(strikes, heston_ivs, marker="x", linestyle="--", label=f"Heston T={maturity}")

    plt.xlabel("Strike")
    plt.ylabel("Implied Volatility")
    plt.title("Market vs Heston Implied Volatility")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    os.makedirs("data/output", exist_ok=True)
    plt.savefig("data/output/heston_iv_comparison.png", dpi=150)
    print("Saved plot to data/output/heston_iv_comparison.png")

def plot_errors(data):
    plt.figure(figsize=(10, 6))

    for maturity, points in sorted(data.items()):
        strikes = [p[0] for p in points]
        errors = [p[3] for p in points]

        plt.plot(strikes, errors, marker="o", label=f"T={maturity}")

    plt.axhline(0.0, linestyle="--")
    plt.xlabel("Strike")
    plt.ylabel("Heston IV - Market IV")
    plt.title("Heston Implied Volatility Errors")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    plt.savefig(OUTPUT_FILE, dpi=150)
    print(f"Saved plot to {OUTPUT_FILE}")

def main():
    data = load_data(INPUT_FILE)
    plot_iv_comparison(data)
    plot_errors(data)

if __name__ == "__main__":
    main()