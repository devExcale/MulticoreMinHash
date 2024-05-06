# Shell used by make
SHELL = /bin/zsh
# Compiler settings
CC = mpicc
CFLAGS = -g -O3 -Wall -fopenmp -I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include
LDFLAGS =

# Source and compiled directories
SRC_DIR = src
OBJ_DIR = obj

# Source and compiled objects
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
EXEC = minhash

# Running settings
processes?=8
dataset?=medical
repeat?=1

arguments_medical = --docs 1989 \
--offset 1 \
--shingle 2 \
--verbose 0 \
--signature 300 \
--bandrows 3 \
--seed 31 \
--threshold 0.24 \
".datasets/medical"

arguments_environment = --docs 29090 \
--offset 1 \
--shingle 4 \
--verbose 0 \
--signature 300 \
--bandrows 3 \
--seed 7 \
--threshold 0.4 \
".datasets/environment"

arguments_medpub = --docs 106330 \
--offset 1 \
--shingle 3 \
--verbose 1200 \
--signature 120 \
--bandrows 4 \
--seed 11 \
--threshold 0.4 \
".datasets/medpub"

RUN_COMMAND = mpiexec -n $(processes) --oversubscribe ./$(EXEC) $(arguments_$(dataset))

# Compile targets
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled objects
clean:
	-rm -rf $(OBJ_DIR) $(EXEC) csv

# Run the program
run:
	mkdir -p csv
	$(RUN_COMMAND)
	-mv results.csv csv/results_$(dataset)_$(processes).csv

debug:
	gdb --args $(RUN_COMMAND)

# Time the program
time:
	time $(RUN_COMMAND)

# Run the program multiple time with different number of processes
# and save the execution times in a csv file
report:
	mkdir -p csv
	echo "dataset,n_processes,time_elapsed,cpu_user,cpu_kernel,cpu_percent" > csv/time_$(dataset)_$(processes).csv
	@for i in {1..$(processes)} ; do \
		for _ in {1..$(repeat)} ; do \
			export TIMEFMT="$(dataset),$$i,%E,%U,%S,%P" ; \
			echo "Running with $$i processes" ; \
			{ time mpiexec -n $$i --oversubscribe ./$(EXEC) $(arguments_$(dataset)) 2> /dev/null ; } 2>> csv/time_$(dataset)_$(processes).csv ; \
			mv results.csv csv/results_$(dataset)_$$i.csv ; \
		done ; \
	done

extract-medpub:
	@echo "--- Exporting medpub dataset ---"
	@cd .datasets && python extract_csv.py --offset 1 --threshold 600 --column "abstract" train.csv medpub
	@echo "--- Done ---"