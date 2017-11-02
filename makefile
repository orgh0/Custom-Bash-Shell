makefil: parse.c buitin.c exec.c
	gcc -o shell buitin.c parse.c exec.c -I. 
