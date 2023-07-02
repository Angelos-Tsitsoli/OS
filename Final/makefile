
CC = gcc
CFLAGS = -Wall -Werror -g -pthread -lrt

OBJS = parent.o child.o

EXEC = a.out

         
ARGS = 10 2 1

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(CFLAGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)

clean:
	rm -f $(OBJS) $(EXEC)

valgrind: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)