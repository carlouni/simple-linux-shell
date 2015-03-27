all: hello cat shell
hello: hello.c
    gcc -Wall -std=c99 -o hello hello.c
cat: cat.c
    gcc -Wall -std=c99 -o cat cat.c
shell: shell.c
    cc -Wall -std=c99 -o shell shell.c
.PHONY: clean
clean: ; rm -rf hello cat shell
