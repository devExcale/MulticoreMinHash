import argparse
from typing import Dict, Literal

import matplotlib.pyplot as plt
import pandas
from matplotlib.colors import TABLEAU_COLORS

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

colors = list(TABLEAU_COLORS.values())


def draw_graph(
		dists: Dict[str, pandas.DataFrame],
		x_label: str,
		y_label: str,
		title: str = None,
		save_path: str = None,
		peakline: Literal['min', 'max', None] = None,
		legend_loc: str = 'best',
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

	# Iterate over non-empty distributions
	_dists = filter(lambda t: len(t[1]) > 0, dists.items())

	for i, (name, df) in enumerate(_dists):
		len_df = len(df)

		# Get dist color
		color = colors[i % len(colors)]

		if len_df > 1:
			# Proper distribution, plot it
			plt.plot(
				df[x_label],
				df[y_label],
				label=name,
				marker='o',
				markersize=3,
				color=color,
			)

		# Check if we need to plot a peakline
		if peakline:
			peakline = str(peakline).lower()

			# Get peak value
			if peakline == 'min':
				y = df[y_label].min()
			elif peakline == 'max':
				y = df[y_label].max()
			else:
				raise ValueError(f"Invalid peakline value: {peakline}")

			# Plot the mix/max line
			if len_df == 1:
				# Plot with label
				plt.axline(
					(0, y),
					slope=0,
					label=name,
					color=color,
					linestyle='dashed',
				)
			else:
				# Plot without label
				plt.axline(
					(0, y),
					slope=0,
					color=color,
					linestyle='dashed',
				)

	# Add labels
	plt.title(title)
	plt.xlabel(x_label)
	plt.ylabel(y_label)

	# Increase the size of the plot
	plt.gcf().set_size_inches(10, 5)

	# Add legend
	plt.legend(loc=legend_loc)

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


def get_speedup_dataframes(time_dists: Dict[str, pandas.DataFrame]) -> Dict[str, pandas.DataFrame]:
	"""
	Computes the speedup for each library and returns the results as a dict {lib:DataFrame}.

	Args:
	time_df: DataFrame containing the time data.

	Returns:
	DataFrame containing the speedup for each library.
	"""

	# Copy the DataFrame
	speedup_df = time_dists.copy()

	# Get baseline time
	if "NONE" in time_dists:
		baseline_time = time_dists["NONE"]["time_elapsed"].item()
	else:
		baseline_time = None
		for df in time_dists.values():
			if len(df) <= 0:
				continue
			t = df["time_elapsed"].min()
			if baseline_time is None or t < baseline_time:
				baseline_time = t

	# Compute the speedup
	for lib, df in speedup_df.items():
		df["speedup"] = df["time_elapsed"].map(lambda time: baseline_time / time)

	# Return the DataFrame
	return speedup_df


def main():
	# Parse the command-line arguments
	args = parser.parse_args()

	# Read the CSV files
	exec_dists = get_time_dataframes(f"{args.in_csv_path}/time_{args.dataset}.csv")
	speedup_dists = get_speedup_dataframes(exec_dists)

	# Compute save path
	path_exec = f"{args.out_png_path}/exec_{args.dataset}.svg" if args.out_png_path else None
	path_speedup = f"{args.out_png_path}/speedup_{args.dataset}.svg" if args.out_png_path else None

	# Draw execution graph
	draw_graph(
		dists=exec_dists,
		x_label="n_processes",
		y_label="time_elapsed",
		title=f"Execution time ({args.dataset})",
		save_path=path_exec,
		peakline="min",
		legend_loc='upper right',
	)

	# Draw speedup graph
	draw_graph(
		dists=speedup_dists,
		x_label="n_processes",
		y_label="speedup",
		title=f"Speedup ({args.dataset})",
		save_path=path_speedup,
		peakline="max",
		legend_loc='lower right',
	)


if __name__ == "__main__":
	main()
