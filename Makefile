CC = gcc
CFLAGS = -Wall -Wextra -Iinclude $(shell sdl2-config --cflags 2>/dev/null)
LDFLAGS = $(shell sdl2-config --libs 2>/dev/null || echo -lSDL2) -lSDL2_ttf

EXEC = prog
OBJS = main.o memory.o cpu.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC)