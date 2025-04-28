import seaborn as sns
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

def main():
    # collisions
    if os.path.exists(sys.argv[1] + "/" + sys.argv[1] + "_collisions.csv"):
        data_collisions = pd.read_csv(sys.argv[1] + "/" + sys.argv[1] + "_collisions.csv")
        sns.barplot(data_collisions, x="Bucket index", y="Bucket size")

        plt.xticks(ticks=range(0, max(data_collisions["Bucket index"]) + 2, 100))

        plt.savefig(sys.argv[1] + "/" + sys.argv[1] + "_collisions.svg", dpi=600, bbox_inches='tight', format="svg")
        plt.close()

    # search time
    if os.path.exists(sys.argv[1] + "/" + sys.argv[1] + "_search_time.csv"):
        data_search = pd.read_csv(sys.argv[1] + "/" + sys.argv[1] + "_search_time.csv")
        sns.scatterplot(data_search, x="Test index", y="Time ms", s=2)

        plt.xticks(ticks=range(0, max(data_search["Test index"]) + 2, 1000))

        plt.savefig(sys.argv[1] + "/" + sys.argv[1] + "_search_time.svg", dpi=600, bbox_inches='tight', format="svg")
        plt.close()

    return

main()
