symnmf: symnmf.o
	gcc -o symnmf symnmf.o -lm

symnmf.o: symnmf.c symnmf.h
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors -c symnmf.c 

clean:
	rm -f *.o