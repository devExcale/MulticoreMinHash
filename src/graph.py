import argparse

import matplotlib.pyplot as plt
import pandas
from typing import Dict

parser = argparse.ArgumentParser(
	description="Generate a graph with the execution times of the MPI/OMP MinHash implementations."
)

parser.add_argument(
	"-p", "--processes",
	required=True,
	type=int,
	help="Max number of processes in the csv file",
)
parser.add_argument(
	"-d", "--dataset",
	type=str,
	required=True,
	help="Name of the dataset used in the csv file",
)
parser.add_argument(
	"in_csv_path",
	type=str,
	help="Path to the directory containing the CSV files (dot for current directory)",
)
parser.add_argument(
	"out_png_path",
	type=str,
	default=None,
	nargs="?",
	help="Path to the directory where the PNG file will be saved (dot for current directory)",
)


def draw_graph(
		dists: Dict[str, pandas.DataFrame],

		x_label: str,
		y_label: str,
		title: str = None,
		save_path: str = None
):
	"""
	Draws a graph from the given data.

	Args:
	data: DataFrame containing the data to plot.
	title: Title of the graph.
	x_label: Column from the data to use as the x-axis.
	y_label: Column from the data to use as the y-axis.
	save_path: Path to save the plot as an image.
	"""

	# Create a new figure
	plt.figure()

	# Add Style
	plt.style.use('classic')

	# Plot the data
	for name, df in dists.items():
		plt.plot(df[x_label], df[y_label], label=name, marker='o', markersize=3)


	# Add labels
	plt.title(title)
	plt.xlabel(x_label)
	plt.ylabel(y_label)

	# Increase the size of the plot
	plt.gcf().set_size_inches(10, 5)

	# Add legend
	plt.legend()

	# Add Grid
	plt.grid()

	# Save the plot
	if save_path:
		plt.savefig(save_path)
	else:
		plt.show()


def get_dataframe(csv_path: str) -> pandas.DataFrame:
	"""
	Reads a CSV file and returns its content as a DataFrame.

	Args:
	csv_path: Path to the CSV file.

	Returns:
	DataFrame containing the content of the CSV file.
	"""

	# Read the CSV file
	df = pandas.read_csv(csv_path, usecols=["n_processes", "time_elapsed"])

	# Remove the last letter from the "time_elapsed" column and convert it to float
	df["time_elapsed"] = df["time_elapsed"].str[:-1].astype(float)

	# Average the time for each n_processes
	df = df.groupby("n_processes").mean().reset_index()

	# Return the DataFrame
	return df


if __name__ == "__main__":
	# Parse the command-line arguments
	args = parser.parse_args()

	# Read the CSV files
	csv_paths = {
		lib: f"{args.in_csv_path}/time_{lib}_{args.dataset}_{args.processes}.csv"
		for lib in ["MPI", "OMP"]
	}
	data = {
		lib: get_dataframe(csv_path)
		for lib, csv_path in csv_paths.items()
	}

	# Compute save path
	save_path = f"{args.out_png_path}/time_{args.dataset}_{args.processes}.png" if args.out_png_path else None

	# Draw a graph
	draw_graph(
		dists=data,
		x_label="n_processes",
		y_label="time_elapsed",
		title="Execution time",
		save_path=save_path
	)
