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

        plt.savefig(sys.argv[1] + "/" + sys.argv[1] + "_collisions.png", dpi=600, bbox_inches='tight')
        plt.close()

    # build time
    if os.path.exists(sys.argv[1] + "/" + sys.argv[1] + "_build_time.csv"):
        data_build = pd.read_csv(sys.argv[1] + "/" + sys.argv[1] + "_build_time.csv")
        sns.scatterplot(data_build, x="Test index", y="Time", s=2)

        plt.xticks(ticks=range(0, max(data_build["Test index"]) + 2, 1000))

        plt.savefig(sys.argv[1] + "/" + sys.argv[1] + "_build_time.png", dpi=600, bbox_inches='tight')
        plt.close()

    # search time
    if os.path.exists(sys.argv[1] + "/" + sys.argv[1] + "_search_time.csv"):
        data_search = pd.read_csv(sys.argv[1] + "/" + sys.argv[1] + "_search_time.csv")
        sns.scatterplot(data_search, x="Test index", y="Time", s=2)

        plt.xticks(ticks=range(0, max(data_search["Test index"]) + 2, 1000))

        plt.savefig(sys.argv[1] + "/" + sys.argv[1] + "_search_time.png", dpi=600, bbox_inches='tight')
        plt.close()

    return

main()
