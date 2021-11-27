
map1: map1.c ass2func.o
	gcc map1.c ass2func.o -o map1 -lm

map2: map2.c ass2func.o
	gcc map2.c ass2func.o -o map2 -lm

ass2func.o: ass2func.c
	gcc -c ass2func.c -lm

clean:
	rm *.o map1 map2