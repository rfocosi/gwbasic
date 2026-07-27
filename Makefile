CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude
LDFLAGS = -lm

SRCS = src/main.c src/tokenizer.c src/eval.c src/runtime.c src/io_screen.c src/io_disk.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/gwbasic

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

test: $(TARGET)
	@chmod +x tests/run_tests.sh
	@./tests/run_tests.sh

.PHONY: all clean test
