all: shell
shell: shell.c
    cc -Wall -std=c99 -o shell shell.c
.PHONY: clean
clean: ; rm -rf shell
