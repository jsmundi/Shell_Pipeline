# Makefile 
# Program 4
#
# Author: JT Mundi
#
# Compile: make
# Run    : make run
# Clean  : make clean

CPP = gcc
FLAGS = -Wall -g -std=c99 -pedantic 

EXEC = connect
OBJS = connect.o

PROJECT_NAME = PROGRAM_4
FILES = connect.c Makefile README.md

default:${EXEC}

all:${EXEC}

clean:
	rm -f ${EXEC}
	rm -f *.o

run: ${EXEC}
	./${EXEC}

${EXEC}:${OBJS}
	${CPP} ${FLAGS} -o ${EXEC} ${OBJS}

tar:
	tar -cvf ${PROJECT_NAME}.tgz ${FILES}


.c.o:
	${CPP} ${FLAGS} -c $<

connect.o: connect.c
