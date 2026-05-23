import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv")

datasets = {
    "random": "случайный массив",
    "reversed": "обратно отсортированный массив",
    "nearly_sorted": "почти отсортированный массив"
}

for dataset in df["dataset"].unique():
    part = df[df["dataset"] == dataset]
    dataset_name = datasets.get(dataset, dataset)

    plt.figure(figsize=(10, 6))

    for alg in part["algorithm"].unique():
        cur = part[part["algorithm"] == alg]
        plt.plot(cur["n"], cur["time_us"], label=alg)

    plt.title(f"Время работы алгоритмов: {dataset_name}")
    plt.xlabel("Размер массива")
    plt.ylabel("Время, микросекунды")
    plt.legend()
    plt.grid(True)
    plt.savefig(f"time_{dataset}.png", dpi=200)
    plt.close()

    plt.figure(figsize=(10, 6))

    for alg in part["algorithm"].unique():
        cur = part[part["algorithm"] == alg]
        plt.plot(cur["n"], cur["symbol_ops"], label=alg)

    plt.title(f"Количество посимвольных операций: {dataset_name}")
    plt.xlabel("Размер массива")
    plt.ylabel("Количество посимвольных операций")
    plt.legend()
    plt.grid(True)
    plt.savefig(f"ops_{dataset}.png", dpi=200)
    plt.close()