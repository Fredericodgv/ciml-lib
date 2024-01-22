CC = gcc
CFLAGS = -g -Wall -std=c99 -O0
RM = rm ./bin/*
FILE?=default_value_if_not_set_in_enviroment

main:
	${CC} ${CFLAGS} -o ./bin/cgp main.c -lbdd

clean:
	${RM}
	
profile:
	${CC} ${CFLAGS} -pg -o ./bin/${FILE} main.c -lbdd

gprof:
	gprof ./bin/${FILE}  gmon.out > ./gprof/${NAME}.txt
