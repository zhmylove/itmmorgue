EXECUTABLE=itmmorgue
SOURCES=itmmorgue.c
OBJECTS=$(SOURCES:.c=.o)
WORKDIR=bin/

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror
LDFLAGS=

all: $(WORKDIR) $(SOURCES) $(EXECUTABLE)

$(WORKDIR): $(WORKDIR)
	mkdir $(WORKDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(WORKID)/$@

.c.o:
	$(CC) $(CFLAGS) -c src/$< -o $(WORKDIR)/$@

clean:
	$(RM) -rf $(WORKDIR)/

.SUFFIXES: .c .o
