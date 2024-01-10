all:
	gcc -o littlecodeeditor littlecodeeditor.c `sdl2-config --cflags --libs` -lSDL2_ttf