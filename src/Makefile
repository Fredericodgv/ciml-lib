CC = gcc
CFLAGS = -g -Wall -std=c99 -O0
RM = rm ./bin/*
FILE?=profile

main:
	${CC} ${CFLAGS} -o ./bin/cgp main.c -lbdd

clean:
	${RM}
	
profile:
	${CC} ${CFLAGS} -pg -o ./bin/${FILE} main.c -lbdd

gprof:
	gprof ./bin/profile gmon.out > ./gprof/${FILE}.txt
