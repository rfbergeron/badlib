CC ?= gcc
CWARN = -Wall -Wextra -Wpedantic -Wshadow -Wno-declaration-after-statement
CFLAGS += -ansi
MKFILE = Makefile
EXE = test
SRC = badmap.c badllist.c badalist.c badset.c
HDR = badmap.h badllist.h badalist.h badset.h badlib.h
OBJ = ${SRC:.c=.o} murmur3.o

.PHONY: all debug clean ci format murmur3

all: CFLAGS += -O1
all: ${EXE}

debug: CFLAGS += -Og -pg -ggdb -fsanitize=address
debug: ${EXE}

${EXE}: ${OBJ} units.o
	${CC} ${CWARN} ${CFLAGS} -lcunit -o $@ $^

murmur3:
	git submodule update --init murmur3
	sed -i 's/\(.*\)\/\/\(.*\)/\1/' murmur3/murmur3.h

murmur3.o: murmur3
	make -C murmur3/ murmur3.o
	cp murmur3/murmur3.o ./

murmur3/murmur3.h: murmur3

%.o: %.c ${HDR} murmur3/murmur3.h
	${CC} ${CWARN} ${CFLAGS} -c $<

clean:
	make -C murmur3/ clean
	rm -f ${OBJ} units.o test

ci: 
	git add ${HDR} ${SRC} ${MKFILE} units.c murmur3 .gitignore .gitmodules TODO.md

format:
	clang-format --style=Google -i ${SRC} ${HDR} units.c
