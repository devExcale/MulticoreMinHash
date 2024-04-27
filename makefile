CC = gcc
CFLAGS = -g -O3 -Wall
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
EXEC = minhash
#processes = 2
arguments = --docs 1989 \
--shingle 3 \
--verbose 200 \
--signature 420 \
--bandrows 3 \
--seed 7 \
--threshold 0.2 \
".datasets/articles"

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(EXEC)

run:
	./$(EXEC) $(arguments)