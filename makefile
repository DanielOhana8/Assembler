assembler: assembler.o pre_assembler.o help.o table.o help.h first_pass.o second_pass.o
	gcc -g -ansi -Wall -pedantic assembler.o pre_assembler.o help.o table.o first_pass.o second_pass.o -o assembler
assembler.o: assembler.c
	gcc -g -c -ansi -Wall -pedantic assembler.c -o assembler.o
pre_assembler.o: pre_assembler.c help.h
	gcc -g -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o
help.o: help.c help.h
	gcc -g -c -ansi -Wall -pedantic help.c -o help.o
table.o: table.c help.h
	gcc -g -c -ansi -Wall -pedantic table.c -o table.o
first_pass.o: first_pass.c
	gcc -g -c -ansi -Wall -pedantic first_pass.c -o first_pass.o
second_pass.o: second_pass.c
	gcc -g -c -ansi -Wall -pedantic second_pass.c -o second_pass.o
