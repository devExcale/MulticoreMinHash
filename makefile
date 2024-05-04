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
processes?=1
dataset?=medical

arguments_medical = --docs 1989 \
--offset 1 \
--shingle 2 \
--verbose 400 \
--signature 300 \
--bandrows 3 \
--seed 31 \
--threshold 0.24 \
".datasets/medical"

arguments_environment = --docs 29090 \
--offset 1 \
--shingle 4 \
--verbose 1000 \
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

RUN_COMMAND = ./$(EXEC) $(arguments_$(dataset))

# Compile targets
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled objects
clean:
	rm -rf $(OBJ_DIR) $(EXEC) *.csv

# Run the program
run:
	$(RUN_COMMAND)

debug:
	gdb --args $(RUN_COMMAND)

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
		{ time ./$(EXEC) $(arguments_$(dataset)) 2> /dev/null ; } 2>> time_report.csv ; \
	done

extract-medpub:
	@echo "--- Exporting medpub dataset ---"
	@cd .datasets && python extract_csv.py --offset 1 --threshold 600 --column "abstract" train.csv medpub
	@echo "--- Done ---"