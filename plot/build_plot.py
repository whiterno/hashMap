import seaborn as sns
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def readData(filename, lines):
    with open(filename, "r") as file:

        data = [x for x in map(lambda x: x.split(), file.readlines())]

        x_coords = []
        y_coords = []

        for i in range(lines):
            x_coords.append(int(data[i][0]))
            y_coords.append(float(data[i][1]))

    return [x_coords, y_coords]

def drawPlot(all_data, labels, plot_name, png_name):
    colors = plt.cm.rainbow([i for i in range(1)])

    plt.plot(x, y, color=colors[0], label=labels[0])

    plt.grid()

    plt.title(plot_name)
    plt.ylabel("Elements in bucket")
    plt.xlabel("HashMap capacity")
    plt.legend()

    plt.savefig(png_name)
    plt.close()

def main():
    data = pd.read_csv("crc32.csv")

    sns.barplot(data, x="Bucket index", y="Bucket size")

    plt.xticks(ticks=range(0, max(data["Bucket index"]) + 1, 100))

    plt.savefig("crc32.png", dpi=600, bbox_inches='tight')
    plt.close()

    return

main()
