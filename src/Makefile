CC = gcc
CFLAGS = -g -Wall -std=c99 -O0
RM = rm ./bin/*

main:
	${CC} ${CFLAGS} -o ./bin/cgp main.c -lbdd
clean:
	${RM}
	
profile:
	${CC} ${CFLAGS} -pg -o ./bin/cgp_profile main.c -lbdd
gprof:
	gprof ./bin/cgp_profile gmon.out > ./bin/gprof.txt
