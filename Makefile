EXECUTABLE=itmmorgue
SOURCES=itmmorgue.c
OBJ=$(SOURCES:.c=.o)
OBJECTS=$(addprefix $(WORKDIR)/, $(OBJ))
WORKDIR=bin
VPATH=src:$(WORKDIR)

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror
LDFLAGS=

all: $(EXECUTABLE)

$(WORKDIR):
	mkdir $(WORKDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(WORKDIR)/$@

$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) -c $< -o $(WORKDIR)/$@

clean:
	$(RM) -rf $(WORKDIR)/

.SUFFIXES: .c .o
