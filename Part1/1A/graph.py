import os
import pandas as pd
import matplotlib.pyplot as plt

base_dir = os.path.dirname(os.path.abspath(__file__))
csv_path = os.path.join(base_dir, "results.csv")
output_dir = os.path.join(base_dir, "graphs")

os.makedirs(output_dir, exist_ok=True)

df = pd.read_csv(csv_path)
df["TimeMilliseconds"] = df["TimeSeconds"] * 1000

for pattern_type in df["PatternType"].unique():
    subset = df[df["PatternType"] == pattern_type]

    plt.figure(figsize=(12, 7))

    for algorithm in subset["Algorithm"].unique():
        if algorithm == "FSM":
            continue
        alg_data = subset[subset["Algorithm"] == algorithm].sort_values("TextLength")
        plt.plot(
            alg_data["TextLength"],
            alg_data["TimeMilliseconds"],
            marker="o",
            label=algorithm
        )

    plt.title(f"Running Time vs Text Length ({pattern_type} Pattern)")
    plt.xlabel("Text Length (characters)")
    plt.ylabel("Time (milliseconds)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    save_path = os.path.join(output_dir, f"{pattern_type.lower()}_pattern_comparison.png")
    plt.savefig(save_path, dpi=300)
    plt.close()

    print(f"Saved {save_path}")

    # -------- FULL GRAPH WITH FSM --------
for pattern_type in df["PatternType"].unique():
    subset = df[df["PatternType"] == pattern_type]

    plt.figure(figsize=(12, 7))

    for algorithm in subset["Algorithm"].unique():
        alg_data = subset[subset["Algorithm"] == algorithm].sort_values("TextLength")
        plt.plot(
            alg_data["TextLength"],
            alg_data["TimeMilliseconds"],
            marker="o",
            label=algorithm
        )

    plt.title(f"ALL Algorithms (Including FSM) - {pattern_type} Pattern")
    plt.xlabel("Text Length (characters)")
    plt.ylabel("Time (milliseconds)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    save_path = os.path.join(output_dir, f"{pattern_type.lower()}_with_fsm.png")
    plt.savefig(save_path, dpi=300)
    plt.close()

    print(f"Saved {save_path}")