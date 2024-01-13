#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

#define TOTAL_ASCII_PRINTABLE_CHARS 95

typedef struct {
    int w;
    int h;
    int fw;
    int fh;
    int rows;
    int cols;
    SDL_Renderer *renderer;
    SDL_Surface *regular_font_surface;
    SDL_Texture *regular_font_texture;
} ASCII_RENDERER;

SDL_Surface *create_regular_ascii_font_surface(int ptsize) {
    TTF_Font *font = TTF_OpenFont("fonts/Hack-Regular.ttf", ptsize);

    uint8_t *printable_chars = (uint8_t*)malloc(TOTAL_ASCII_PRINTABLE_CHARS + 1);
    uint8_t c = 0x20;
    while (c != 0x7f) {
        *(printable_chars + (c - 0x20)) = c;
        c++;
    }
    *(printable_chars + (c - 0x20)) = 0;

    SDL_Color fg;
    SDL_Color bg;

    fg.r = 0xff;
    fg.g = 0xff;
    fg.b = 0xff;
    fg.a = 0xff;
    bg.r = 0x27;
    bg.g = 0x27;
    bg.b = 0x27;
    bg.a = 0xff;

    SDL_Surface *font_surface = TTF_RenderUTF8_LCD(font, (const char *)printable_chars, fg, bg);

    TTF_CloseFont(font);
    free((void *)printable_chars);

    return font_surface;
}

ASCII_RENDERER *create_assci_renderer(SDL_Window *window, int ptsize) {
    ASCII_RENDERER *ascii_rd = (ASCII_RENDERER *)malloc(sizeof(ASCII_RENDERER));

    ascii_rd->renderer = SDL_CreateRenderer(window, -1, 0);
    ascii_rd->regular_font_surface = create_regular_ascii_font_surface(ptsize);
    ascii_rd->fw = (ascii_rd->regular_font_surface->w) / TOTAL_ASCII_PRINTABLE_CHARS;
    ascii_rd->fh = (ascii_rd->regular_font_surface->h);
    ascii_rd->regular_font_texture = SDL_CreateTextureFromSurface(ascii_rd->renderer, ascii_rd->regular_font_surface);

    SDL_GetWindowSize(window, &(ascii_rd->w), &(ascii_rd->h));
    return ascii_rd;
}

void ascii_renderer_print_char(ASCII_RENDERER *ascii_rd, uint8_t c, int x, int y) {
    SDL_Rect r1;
    SDL_Rect r2;

    int fw = ascii_rd->fw;
    int fh = ascii_rd->fh;

    r1.x = fw * (int)(c - 0x20);
    r1.y = 0;
    r1.w = fw;
    r1.h = fh;

    r2.x = fw * x;
    r2.y = fh * y;
    r2.w = fw;
    r2.h = fh;

    SDL_RenderCopy(ascii_rd->renderer, ascii_rd->regular_font_texture, &r1, &r2);
}

void ascii_renderer_clear(ASCII_RENDERER *ascii_rd) {
    SDL_RenderClear(ascii_rd->renderer);
}

void ascii_renderer_present(ASCII_RENDERER *ascii_rd) {
    SDL_RenderPresent(ascii_rd->renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Text Editor",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, 0);

    TTF_Init();

    ASCII_RENDERER *ascii_rd = create_assci_renderer(window, 40);
    ascii_renderer_clear(ascii_rd);
    ascii_renderer_print_char(ascii_rd, 'H', 1, 1);
    ascii_renderer_print_char(ascii_rd, 'i', 2, 1);
    ascii_renderer_print_char(ascii_rd, 'i', 3, 1);
    ascii_renderer_present(ascii_rd);

    while (1) {
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch( event.type ){
            case SDL_KEYDOWN:
                printf("Key press detected\n");
                break;

            case SDL_KEYUP:
                printf("Key release detected\n");
                exit(1);
                break;

            default:
                break;
            }
        }
        SDL_Delay(100);
    }

    SDL_Quit();
    return 0;
}