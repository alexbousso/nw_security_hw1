‫‪CC = gcc
EXEC = protector
DEBUG_FLAG = -g # assign -g for debug
CFLAGS = -pedantic-errors -Werror -Wall $(DEBUG_FLAG) #-DNDEBUG

$(EXEC) :
	$(CC) -o $@ *.c

clean:
	rm -f *.o $(EXEC)
