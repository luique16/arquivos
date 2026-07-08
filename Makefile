all:
	gcc -o arquivos src/*.c src/funcionalidades/*.c

run:
	./arquivos

clean:
	rm -f arquivos arquivos.zip

zip:
	zip -r arquivos.zip ./include ./src Makefile
