import matplotlib.pyplot as plt
import pandas


def draw_graph(
		data: pandas.DataFrame,
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

	# Plot the data
	plt.plot(data[x_label], data[y_label])

	# Add labels
	plt.title(title)
	plt.xlabel(x_label)
	plt.ylabel(y_label)

	# Increase the size of the plot
	plt.gcf().set_size_inches(10, 5)

	# Save the plot
	if save_path:
		plt.savefig(save_path)
	else:
		plt.show()


if __name__ == "__main__":
	# Read data from a CSV file
	csv_file = 'C:\\Users\\escac\\Projects\\Academics\\MulticoreMinHash\\csv\\time_environment_8.csv'
	graph_file = 'C:\\Users\\escac\\Projects\\Academics\\MulticoreMinHash\\csv\\time_environment_8.png'
	data = pandas.read_csv(csv_file, usecols=["n_processes", "time_elapsed"])

	# Remove the last letter from "time_elapsed" column and convert it to float
	data["time_elapsed"] = data["time_elapsed"].str[:-1].astype(float)

	# Average the time for each n_processes
	data = data.groupby("n_processes").mean().reset_index()

	# Draw a graph
	draw_graph(
		data=data,
		x_label="n_processes",
		y_label="time_elapsed",
		title="Execution time",
		save_path=graph_file
	)
