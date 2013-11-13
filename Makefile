# Author: Tim Bradt
# Makefile for building mtush
CC = gcc
EXEC = mtush
CCFLAGS = -g -Wall -Wextra -std=c99

${EXEC}: mtush.c mtush.h colors.h
	${CC} ${CCFLAGS} -o ${EXEC} mtush.c -lm -lreadline
    
clean:
	rm -f ${EXEC} *~ *.tgz

rebuild: clean ${EXEC}

cppcheck: mtush.c mtush.h colors.h
	cppcheck *.c

pre:
	${CC} -E *.c

#Makes TAR file
tar: clean ${EXEC}
	./${EXEC}
	make clean
	tar cvfz mtush.tgz *.h *.c Makefile