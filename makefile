mainpthread: main.c
	gcc -g -Wall -o main main.c -lpthread -lm
mainopenmp: openmp.c
	gcc -fopenmp openmp.c -o mainopenmp -lm
