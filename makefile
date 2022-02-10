all: sos_game.o sos_game.c
	gcc sos_game.o -lncurses -lm  -o sos_game

sos_game.o: sos_game.c
	gcc sos_game.c -c

clean:
	rm *.o
	rm sos_game
