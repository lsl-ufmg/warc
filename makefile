NAME = warc
CC = gcc
CFLAGS = -O0 -g -Wall -Wextra -Wpedantic -I/usr/include/libxml2 -D_GNU_SOURCE
#CFLAGS = -O3 -march=native -Wall -Wextra -pedantic -flto -D_GNU_SOURCE -I/usr/include/libxml2
LDFLAGS = -ljson-c -lz -lssl -lcrypto -lxml2 -lpthread

SRC = .
DEPS = $(wildcard $(SRC)/*.h)
CODE = $(wildcard $(SRC)/*.c)
OBJ = $(patsubst %.c,%.o,$(CODE))
ARGS= 50000000 0 /tmp/temp
VALARGS=--leak-check=full --show-leak-kinds=all --track-origins=yes

.PHONY: install clean dist-gzip dist-bzip2 dist-xz dist
.SILENT: install clean dist-gzip dist-bzip2 dist-xz dist

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(NAME)
	./$(NAME) $(ARGS)

val: $(NAME)
	valgrind $(VALARGS) ./$(NAME) $(ARGS)

clean:
	rm -f $(SRC)/*.o *~ $(SRC)/*~ $(NAME)
