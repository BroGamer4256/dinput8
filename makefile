CFLAGS = -std=c99 -pedantic -Wall -Ofast
LDFLAGS = -shared -luuid -s
CC = x86_64-w64-mingw32-gcc
SRC = dllmain.c
OBJ = ${SRC:.c=.o}

all: options dinput8

options:
	@echo dinput8 build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

dinput8: ${OBJ}
	${CC} -o $@.dll ${OBJ} ${LDFLAGS}

clean:
	rm -f dinput8.dll ${OBJ}
