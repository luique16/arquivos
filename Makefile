all:
	gcc -o arquivos src/*.c

run:
	./arquivos

zip:
	zip -r arquivos.zip ./include ./src Makefile
