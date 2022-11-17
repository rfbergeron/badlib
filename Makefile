CWARN ?= -Wall -Wextra -Wpedantic -Wno-declaration-after-statement
CFLAGS ?= -std=c90
MKFILE := Makefile
EXE := test
SRC := badmap.c badllist.c badalist.c badset.c units.c
HDR := badmap.h badllist.h badalist.h badset.h badlib.h
OBJ := ${SRC:.c=.o} murmur3.o

vpath murmur3.c murmur3.h murmur3/

.PHONY: all debug clean ci format murmur3

all: CFLAGS += -O1
all: ${EXE}

debug: CFLAGS += -Og -pg -ggdb
debug: ${EXE}

${EXE}: LDFLAGS += -lcunit
${EXE}: ${OBJ}
	${CC} ${CWARN} ${LDFLAGS} ${CFLAGS} -o $@ $^

murmur3:
	git submodule update --init murmur3
	patch --directory murmur3/ <./murmur3.patch

murmur3.o: CWARN += -Wno-implicit-fallthrough
%.o: %.c ${HDR} murmur3/murmur3.h
	${CC} ${CWARN} ${CPPFLAGS} ${CFLAGS} -c $<

clean:
	rm -f ${OBJ} test

ci: 
	git add ${HDR} ${SRC} ${MKFILE} murmur3 murmur3.patch .gitignore .gitmodules TODO.md

format:
	clang-format --style=Google -i ${SRC} ${HDR}
