OUT := dinput8 
CC := clang
TARGET := x86_64-pc-windows-gnu
SRC = dllmain.c
OBJ = ${TARGET}/${SRC:.c=.o}
CFLAGS = -std=c99 -pedantic -Wall -Ofast -target ${TARGET} -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=_WIN32_WINNT_WIN7
LDFLAGS := -shared -luuid -s -static

all: options ${OUT}

options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

${TARGET}:
	@mkdir ${TARGET}

${TARGET}/%.o: %.c
	@echo BUILD $@
	@${CC} -c ${CFLAGS} $< -o $@

${OUT}: | ${TARGET} ${OBJ}
	@echo LINK $@
	@${CC} ${CFLAGS} -o ${TARGET}/$@.dll ${OBJ} ${LDFLAGS}

clean:
	rm -rf ${TARGET}
