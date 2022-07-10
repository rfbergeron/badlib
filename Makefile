CC ?= gcc
CWARN = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wno-declaration-after-statement
CFLAGS = -ansi -Og -pg -ggdb -fsanitize=address
#CFLAGS = -ansi -O2
MKFILE = Makefile
SRC = badmap.c badllist.c badalist.c badset.c
HDR = badmap.h badllist.h badalist.h badset.h badlib.h
OBJ = ${SRC:.c=.o} murmur3.o
GCH = ${HDR:=.gch}

.PHONY: clean ci format objects

test: ${OBJ} units.o
	${CC} ${CWARN} ${CFLAGS} -lcunit -o $@ $^

objects: ${OBJ}

murmur3.o:
	make -C murmur3/ murmur3.o
	cp murmur3/murmur3.o ./

%.o: %.c ${ALLHDR}
	${CC} ${CWARN} ${CFLAGS} -c $^

clean:
	rm -f ${OBJ} ${GCH} units.o test

ci: 
	git add ${HDR} ${SRC} ${MKFILE} units.c murmur3 .gitignore .gitmodules TODO.md

format:
	clang-format --style=Google -i ${SRC} ${HDR} units.c
