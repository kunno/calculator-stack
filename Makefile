# This is the Makefile to use for calculator-stack
# To use, at the prompt, type:
#
#       make Calc               # This will make executable calc
# or
#       make Driver             # This will make executable driver
# or
#       make clean              # This will safely remove old stuff

all:	Driver Calc

Calc:	Calc.o Decin.o Main.o Namein.o
	g++ -Wall -pedantic -o Calc -g Calc.o Decin.o Main.o Namein.o
	touch Calc.datafile

Calc.o:	Calc.c Calc.h Decin.h Tree.h List.h Stack.h SymTab.h Tree.c
	g++ -Wall -pedantic -g -c Calc.c

Driver:	Driver.o 
	g++ -Wall -pedantic -o Driver Driver.o 
	touch Driver.datafile

Driver.o:	Driver.c Driver.h Tree.h Tree.c SymTab.h
	g++ -Wall -pedantic -g -c Driver.c

Decin.o:	Decin.c Decin.h
	g++ -Wall -pedantic -g -c Decin.c

Main.o:	Main.c Calc.h Tree.h List.h Stack.h SymTab.h
	g++ -Wall -pedantic -g -c Main.c

Namein.o:	Namein.h Namein.c
	g++ -Wall -pedantic -g -c Namein.c

valgrind_Calc: Calc
	valgrind --leak-check=full --read-var-info=yes \
	    --show-reachable=yes ./Calc $(DEBUG)

valgrind_Driver: Driver
	valgrind --leak-check=full --read-var-info=yes \
	    --show-reachable=yes ./Driver $(DEBUG)

clean:
	rm -f *.o Calc Driver
	make fresh

new:
	make clean
	make

backup:	Makefile *.[ch]
	cp Makefile *.[ch] backup

fresh:
	rm -f Calc.datafile Driver.datafile
	touch Calc.datafile Driver.datafile
