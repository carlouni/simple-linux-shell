all: hello cat
hello: hello.c
    gcc -Wall -std=c99 -o hello hello.c
cat: cat.c
    gcc -Wall -std=c99 -o cat cat.c
.PHONY: clean
clean: ; rm -rf hello cat
