a.out: main.o read.o edit.o display.o
	gcc main.o read.o edit.o display.o

main.o: main.c main.h
	gcc -c main.c

read.o: read.c main.h
	gcc -c read.c

edit.o: edit.c main.h
	gcc -c edit.c

display.o: display.c main.h
	gcc -c display.c

clean:
	rm -f *.o a.out