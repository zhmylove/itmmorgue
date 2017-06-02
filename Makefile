EXECUTABLE=itmmorgue
SRC=itmmorgue.c
SOURCES=$(addprefix src/, $(SRC))
OBJ=$(SRC:.c=.o)
OBJECTS=$(addprefix $(WORKDIR)/, $(OBJ))
WORKDIR=bin

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror
LDFLAGS=

all: $(EXECUTABLE)

$(WORKDIR):
	mkdir $(WORKDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(WORKDIR)/$@

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $< -o $@

$(SOURCES): $(WORKDIR)

clean:
	$(RM) -rf $(WORKDIR)

.SUFFIXES: .c .o
