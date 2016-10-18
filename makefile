snakeG: snakeG.c  
	cc snakeG.c `pkg-config --libs gtk+-3.0` `pkg-config --cflags gtk+-3.0`  -o snakeG
	./snakeG
