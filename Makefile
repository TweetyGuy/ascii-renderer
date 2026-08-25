all:
	gcc main.c -o donut -lm -lncurses

test: all
	./donut

clean:
	rm donut
	clear