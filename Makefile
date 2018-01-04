CC = gcc
CFLAGS = -Wall -Werror
OBJECTS = dlist.o event.o parser.o server.o threadpool.o wrappers.o requests.o
HEADERS = dlist.h event.h parser.h server.h threadpool.h wrappers.h requests.h
LDLIBS = -lpthread
PROGRAM = phdserver

all: $(PROGRAM)

$(OBJECTS): $(HEADERS)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

clean:
	rm -f *~ *.o $(PROGRAM) core a.out
