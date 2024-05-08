# Shell used by make
SHELL = /bin/zsh

# Multiprocessing library name (MPI or OMP)
whichmp?=MPI

# Compiler settings (OpenMP)
CC_OMP = gcc
CFLAGS_OMP = -g -O3 -Wall -fopenmp

# Compiler settings (MPI)
CC_MPI = mpicc
CFLAGS_MPI = -g -O3 -Wall -fopenmp -I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include

# Compiler settings (switch)
CC = $(CC_$(whichmp))
CFLAGS = $(CFLAGS_$(whichmp))

# Source and compiled directories
SRC_DIR = src/$(whichmp)
OBJ_DIR = obj/$(whichmp)

# Source and compiled objects
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
EXEC = obj/minhash_$(whichmp)

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

RUN_OMP = ./$(EXEC) -n $(processes) $(arguments_$(dataset))
RUN_MPI = mpiexec -n $(processes) --oversubscribe ./$(EXEC) $(arguments_$(dataset))

RESULTS_FILE = csv/minhash_$(whichmp)_$(dataset)_$(processes).csv
TIME_FILE = csv/time_$(whichmp)_$(dataset)_$(processes).csv

# Compile targets
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled objects
clean:
	-rm -rf obj

# Run the program
run:
	echo "Running on $(whichmp) with $(processes) processes"
	mkdir -p csv
	$(RUN_$(whichmp))
	-mv results.csv $(RESULTS_FILE)

debug:
	gdb --args $(RUN_$(whichmp))

# Time the program
time:
	time $(RUN_$(whichmp))

# Run the program multiple time with different number of processes
# and save the execution times in a csv file
report:
	mkdir -p csv
	echo "dataset,lib,n_processes,time_elapsed,cpu_user,cpu_kernel,cpu_percent" > $(TIME_FILE)
	@for i in {1..$(processes)} ; do \
		for _ in {1..$(repeat)} ; do \

			export TIMEFMT="$(dataset),$(whichmp),$$i,%E,%U,%S,%P" ; \
			echo "Running on $(whichmp) with $$i processes" ; \

			if [ "$(whichmp)" == "OMP" ]; then \
				{ time ./$(EXEC_OMP) -n $$i $(arguments_$(dataset)) 2> /dev/null ; } 2>> $(TIME_FILE) ; \
			elif [ "$(whichmp)" == "MPI" ]; then \
				{ time mpiexec -n $$i --oversubscribe ./$(EXEC_MPI) $(arguments_$(dataset)) 2> /dev/null ; } 2>> $(TIME_FILE) ; \
			fi ; \

			mv results.csv $(RESULTS_FILE) ; \

		done ; \
	done

extract-medpub:
	@echo "--- Exporting medpub dataset ---"
	@cd .datasets && python extract_csv.py --offset 1 --threshold 600 --column "abstract" train.csv medpub
	@echo "--- Done ---"