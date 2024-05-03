# Shell used by make
SHELL = /bin/zsh
# Compiler settings
CC = gcc
CFLAGS = -g -O3 -Wall
LDFLAGS =

# Source and compiled directories
SRC_DIR = src
OBJ_DIR = obj

# Source and compiled objects
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
EXEC = minhash

# Running settings
processes = 1
arguments = --docs 1989 \
--shingle 2 \
--verbose 0 \
--signature 200 \
--bandrows 4 \
--seed 7 \
--threshold 0.2
doc_directory = ".datasets/med"
RUN_COMMAND = ./$(EXEC) $(arguments) $(doc_directory)

# Compile targets
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled objects
clean:
	rm -rf $(OBJ_DIR) $(EXEC) time_report.csv

# Run the program
run:
	$(RUN_COMMAND)

# Time the program
time:
	time $(RUN_COMMAND)

# Run the program multiple time with different number of processes
# and save the execution times in a csv file
report:
	echo "n_processes,time_elapsed,cpu_user,cpu_kernel,cpu_percent,max_mem" > time_report.csv
	for i in {1..$(processes)} ; do \
		export TIMEFMT="$$i,%E,%U,%S,%P,%M" ; \
		echo "Running with $$i processes" ; \
		{ time ./$(EXEC) $(arguments) $(doc_directory) 2> /dev/null ; } 2>> time_report.csv ; \
	done
