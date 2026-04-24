import csv
import os
from collections import defaultdict
import matplotlib.pyplot as plt

INPUT_FILE = "data/output/implied_vols.csv"
OUTPUT_FILE = "data/output/smiles.png"

def load_data(filepath):
    grouped = defaultdict(list)

    with open(filepath, "r", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            maturity = float(row["maturity"])
            strike = float(row["strike"])
            implied_vol = float(row["implied_vol"])
            grouped[maturity].append((strike, implied_vol))

    for maturity in grouped:
        grouped[maturity].sort(key=lambda x: x[0])

    return grouped

def main():
    data = load_data(INPUT_FILE)

    plt.figure(figsize=(10, 6))

    for maturity, points in sorted(data.items()):
        strikes = [p[0] for p in points]
        ivs = [p[1] for p in points]
        plt.plot(strikes, ivs, marker="o", label=f"T={maturity}")

    plt.xlabel("Strike")
    plt.ylabel("Implied Volatility")
    plt.title("Implied Volatility Smiles by Maturity")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    plt.savefig(OUTPUT_FILE, dpi=150)
    print(f"Saved plot to {OUTPUT_FILE}")

if __name__ == "__main__":
    main()