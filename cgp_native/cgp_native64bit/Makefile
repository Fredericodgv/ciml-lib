all:	cgp

cgp:	cgp_native.cpp cgp.h compiler.h circ.h
	g++ cgp_native.cpp -std=c++11 -o cgp_native -DXDONOTEVALUATEUNUSEDNODES -m64

avx:
	g++ cgp_native.cpp -std=c++11 -o cgp_native -DXDONOTEVALUATEUNUSEDNODES -m64 -mavx2

profile:
	g++ cgp_native.cpp -pg -o cgp_native -DXDONOTEVALUATEUNUSEDNODES -m64

gprof:
	gprof cgp_native gmon.out > test.txt

debug:
	g++ cgp_native.cpp -g -O0 -std=c++11 -o cgp_native -DXDONOTEVALUATEUNUSEDNODES -m64
