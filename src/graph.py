import argparse
from typing import Dict

import matplotlib.pyplot as plt
import pandas

parser = argparse.ArgumentParser(
	description="Generate a graph with the execution times of the MPI/OMP MinHash implementations."
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
		if len(df) > 1:
			# Proper distribution, plot it
			plt.plot(df[x_label], df[y_label], label=name, marker='o', markersize=3)
		elif len(df) == 1:
			# Single point, plot it as a line
			plt.axline((0, df[y_label].values[0]), slope=0, label=name)

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


def get_time_dataframes(csv_path: str) -> Dict[str, pandas.DataFrame]:
	"""
	Reads a CSV time report file and returns its content as a dict {lib:DataFrame}.
	The times are averaged for each n_processes.

	Args:
	csv_path: Path to the CSV file.

	Returns:
	DataFrame containing the content of the CSV file.
	"""

	# Read the CSV file
	df = pandas.read_csv(csv_path, usecols=["lib", "n_processes", "time_elapsed"])

	# Remove the last letter from the "time_elapsed" column and convert it to float
	df["time_elapsed"] = df["time_elapsed"].str[:-1].astype(float)

	# Average the time for each n_processes
	df = df.groupby(["lib", "n_processes"]).mean().reset_index()

	# Divide the data into separate DataFrames
	dists = {
		lib: group_df
		for lib, group_df in df.groupby("lib")
	}

	# Return the DataFrame
	return dists


def main():
	# Parse the command-line arguments
	args = parser.parse_args()

	# Read the CSV files
	data = get_time_dataframes(f"{args.in_csv_path}/time_{args.dataset}.csv")

	# Compute save path
	save_path = f"{args.out_png_path}/time_{args.dataset}.svg" if args.out_png_path else None

	# Draw a graph
	draw_graph(
		dists=data,
		x_label="n_processes",
		y_label="time_elapsed",
		title=f"Execution time ({args.dataset})",
		save_path=save_path
	)


if __name__ == "__main__":
	main()
