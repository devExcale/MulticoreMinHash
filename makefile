# Shell used by make
SHELL = /bin/zsh

# Multiprocessing library name (MPI or OMP)
whichmp?=MPI

CC_NONE = gcc
CFLAGS_NONE = -g -O3 -Wall -D__MP_NONE__

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

# Use OMP sources if NONE is selected
ifeq ($(whichmp),NONE)
	SRC_DIR = src/OMP
endif

# Source and compiled objects
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
EXEC = obj/minhash_$(whichmp)

## Running settings ##
# Number of processes to run the program (or max number during report)
processes?=8
# Name of the dataset to run the algorithm on
dataset?=medical
# Number of times to repeat the execution during report
repeat?=1
# Starting number of processes during report
pstart?=1
# Whether to save the algorithm results during report
saveres?=0

arguments_medical = --docs 1989 \
--offset 1 \
--shingle 3 \
--verbose 0 \
--signature 300 \
--bandrows 3 \
--seed 13 \
--threshold 0.10 \
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
--verbose 0 \
--signature 200 \
--bandrows 4 \
--seed 11 \
--threshold 0.3 \
".datasets/medpub"

RUN_NONE = ./$(EXEC) -n 1 $(arguments_$(dataset))
RUN_OMP = ./$(EXEC) -n $(processes) $(arguments_$(dataset))
RUN_MPI = mpiexec -n $(processes) --oversubscribe ./$(EXEC) $(arguments_$(dataset))

RESULTS_FILE = csv/minhash_$(whichmp)_$(dataset)_$(processes).csv
TIME_FILE = csv/time_$(dataset).csv

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
run: exists-dataset
	@mkdir -p csv
	@echo "Running on $(whichmp) with $(processes) processes"
	$(RUN_$(whichmp))
	-mv results.csv $(RESULTS_FILE)

debug: exists-dataset
	gdb --args $(RUN_$(whichmp))

# Time the program
time: exists-dataset
	time $(RUN_$(whichmp))

# Run the program multiple time with different number of processes
# and save the execution times in a csv file
report: exists-dataset
	@echo "Report: $(whichmp) with range [$(pstart), $(processes)] x$(repeat)"
	@mkdir -p csv
	@if [[ ! -f $(TIME_FILE) ]]; then \
  		echo "Creating $(TIME_FILE)" ; \
		echo "dataset,lib,n_processes,time_elapsed,cpu_user,cpu_kernel,cpu_percent" > $(TIME_FILE) ; \
	fi
	@for i in {$(pstart)..$(processes)}; do \
		for _ in {1..$(repeat)}; do \
\
			export TIMEFMT="$(dataset),$(whichmp),$$i,%E,%U,%S,%P" ; \
			echo "Running on $(whichmp) with $$i processes" ; \
\
			if [[ "$(whichmp)" == "NONE" ]]; then \
				{ time ./$(EXEC) -n 1 $(arguments_$(dataset)) 2> /dev/null ; } 2>> $(TIME_FILE) ; \
			elif [[ "$(whichmp)" == "OMP" ]]; then \
				{ time ./$(EXEC) -n $$i $(arguments_$(dataset)) 2> /dev/null ; } 2>> $(TIME_FILE) ; \
			elif [[ "$(whichmp)" == "MPI" ]]; then \
				{ time mpiexec -n $$i --oversubscribe ./$(EXEC) $(arguments_$(dataset)) 2> /dev/null ; } 2>> $(TIME_FILE) ; \
			fi ; \
\
			results_file_i=$(RESULTS_FILE) ; \
			if [[ $(saveres) > 0 ]]; then \
				mv results.csv $${results_file_i/$(processes).csv/$$i.csv} ; \
			else \
				rm results.csv ; \
			fi ; \
\
		done ; \
	done

exists-dataset:
	@if [[ ! -d .datasets/$(dataset) ]]; then \
		echo "Dataset $(dataset) does not exist" ; \
		exit 1 ; \
	fi

report-check:
	@base_filename=$(RESULTS_FILE) ; \
	base_filename=$${base_filename/$(processes).csv/} ; \
	for i in {2..$(processes)} ; do \
		cmp "$${base_filename}1.csv" "$${base_filename}$${i}.csv" ; \
		if [ $$? -ne 0 ]; then \
			break ; \
		fi ; \
	done ; \

graph:
	@echo "Generating graph for $(dataset)"
	@python src/graph.py -d $(dataset) csv csv

extract-medpub:
	@echo "--- Exporting medpub dataset ---"
	@cd .datasets && python extract_csv.py --offset 1 --threshold 600 --column "abstract" train.csv medpub
	@echo "--- Done ---"
