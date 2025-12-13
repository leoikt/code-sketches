CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c17 -O2 -I.
AR = ar
ARFLAGS = rcs

LIBRARY = libring.a
SOURCES = ring.c
OBJECTS = $(SOURCES:.c=.o)
HEADER = ring.h

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBRARY)

install: $(LIBRARY)
	cp $(LIBRARY) /usr/local/lib/
	cp $(HEADER) /usr/local/include/

uninstall:
	rm -f /usr/local/lib/$(LIBRARY) /usr/local/include/$(HEADER)

clean:
	rm *.o *.a example

.PHONY: all clean install uninstall