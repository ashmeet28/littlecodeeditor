all:
	gcc -std=c11 littlecodeeditor.c -o littlecodeeditor `sdl2-config --cflags --libs` -lSDL2_ttf