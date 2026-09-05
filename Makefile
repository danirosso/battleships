main:main.cpp
	g++ main.cpp -Wall -Iinclude lib/libraylib.a -lX11 -lm -o main
	./main
shubs:shubs.c
	gcc shubs.c  -Iinclude lib/libraylib.a -lX11 -lm -o main
	./main
