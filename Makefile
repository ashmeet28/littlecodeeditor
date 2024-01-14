all:
	gcc -std=c11 src/littlecodeeditor.c -o littlecodeeditor `sdl2-config --cflags --libs` -lSDL2_ttf