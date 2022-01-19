CFLAGS = -std=c99 -pedantic -Wall -Ofast
LDFLAGS = -shared -luuid -s
CC := x86_64-w64-mingw32-gcc
DIR := build/
SRC = dllmain.c
OBJ = ${SRC:.c=.o}

all: options dinput8

options:
	if [ ! -d ${DIR} ]; then \
		mkdir ${DIR}; \
	fi
	@echo dinput8 build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $< -o ${DIR}$@

dinput8: ${OBJ}
	${CC} -o ${DIR}$@.dll ${DIR}${OBJ} ${LDFLAGS}

clean:
	rm -rf ${DIR}
