SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
CC = gcc
FLAGS = -lpthread
EXEC = start

all:$(EXEC)

$(EXEC) : $(OBJ) $(FLAGS)
	$(CC) $^ -o $@

%.o : %.c %.h $(FLAGS)
	$(CC) -c $<

#.PHONY: clean
clean:
	rm -f $(OBJ) $(EXEC)
