import pandas as pd
import matplotlib.pyplot as plt

df1 = pd.read_csv('single_stream.csv')
plt.figure(figsize=(10, 5))
plt.plot(df1['prefix_percent'], df1['F0'], label='Точное F₀', linewidth=2, color='blue')
plt.plot(df1['prefix_percent'], df1['N'], label='Оценка HLL', linestyle='--', linewidth=2, color='red')
plt.xlabel('Длина префикса (%)')
plt.ylabel('Число уникальных элементов')
plt.title('HyperLogLog: один поток')
plt.legend()
plt.grid(alpha=0.3)
plt.tight_layout()
plt.savefig('plot1.png', dpi=150)
plt.close()
print("Сохранено: plot1.png")

df2 = pd.read_csv('stats.csv')
plt.figure(figsize=(10, 5))
plt.plot(df2['prefix_percent'], df2['F0_mean'], label='Среднее точное F₀', linewidth=2, color='green')
plt.plot(df2['prefix_percent'], df2['N_mean'], label='Средняя оценка HLL', linestyle='--', linewidth=2, color='orange')
plt.fill_between(
    df2['prefix_percent'],
    df2['N_mean'] - df2['N_std'],
    df2['N_mean'] + df2['N_std'],
    alpha=0.3,
    color='orange',
    label='±1σ'
)
plt.xlabel('Длина префикса (%)')
plt.ylabel('Число уникальных элементов')
plt.title('HyperLogLog: 100 потоков (среднее ± стандартное отклонение)')
plt.legend()
plt.grid(alpha=0.3)
plt.tight_layout()
plt.savefig('plot2.png', dpi=150)
plt.close()
print("Сохранено: plot2.png")
