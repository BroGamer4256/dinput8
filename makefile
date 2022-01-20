CC := clang
TARGET := x86_64-pc-windows-gnu
SRC = dllmain.c
OBJ = ${SRC:.c=.o}
CFLAGS = -std=c99 -pedantic -Wall -Ofast -target ${TARGET} -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WIN7
LDFLAGS = -shared -luuid -s -static

all: options dinput8

options:
	if [ ! -d ${TARGET} ]; then \
		mkdir ${TARGET}; \
	fi
	@echo dinput8 build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $< -o ${TARGET}/$@

dinput8: ${OBJ}
	${CC} ${CFLAGS} -o ${TARGET}/$@.dll ${TARGET}/${OBJ} ${LDFLAGS}

clean:
	rm -rf ${TARGET}
