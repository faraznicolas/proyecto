GETS= multihilo
FLAGS= -Wall -g -pthread -lrt
CC= gcc
TARGETS= multihilo

multihilo: main.c hilo.c enviar.c responder.c	
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm -fv $(TARGETS) *.o *~
