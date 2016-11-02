project: snakeG.c  
	cc -w  snakeG.c `pkg-config --libs --cflags gtk+-3.0`  -o project
