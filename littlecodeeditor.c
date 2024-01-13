#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

#define ASCII_PRINTABLE_CHARS_COUNT 95

#define ASCII_RENDERER_CHAR_GRID_CAPACITY 1048576

typedef struct {
    int w;
    int h;
    int fw;
    int fh;
    uint8_t *char_grid;
    SDL_Renderer *renderer;
    SDL_Surface *regular_font_surface;
    SDL_Texture *regular_font_texture;
} ASCII_RENDERER;

SDL_Surface *create_regular_ascii_font_surface(int ptsize) {
    TTF_Font *font = TTF_OpenFont("fonts/Hack-Regular.ttf", ptsize);

    uint8_t *printable_chars = (uint8_t*)malloc(ASCII_PRINTABLE_CHARS_COUNT + 1);
    uint8_t c = 0x20;
    while (c != 0x7f) {
        printable_chars[c - 0x20] = c;
        c++;
    }
    printable_chars[c - 0x20] = 0;

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

    SDL_GetWindowSize(window, &(ascii_rd->w), &(ascii_rd->h));

    ascii_rd->renderer = SDL_CreateRenderer(window, -1, 0);
    ascii_rd->regular_font_surface = create_regular_ascii_font_surface(ptsize);
    ascii_rd->fw = (ascii_rd->regular_font_surface->w) / ASCII_PRINTABLE_CHARS_COUNT;
    ascii_rd->fh = (ascii_rd->regular_font_surface->h);
    ascii_rd->regular_font_texture = SDL_CreateTextureFromSurface(
                                        ascii_rd->renderer, ascii_rd->regular_font_surface);

    ascii_rd->char_grid = (uint8_t *)malloc(ASCII_RENDERER_CHAR_GRID_CAPACITY);
    for(int i = 0; i < ASCII_RENDERER_CHAR_GRID_CAPACITY; i++) {
        ascii_rd->char_grid[i] = 0x20;
    }

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

int ascii_renderer_cols(ASCII_RENDERER *ascii_rd) {
    return ascii_rd->w / ascii_rd->fw;
}

int ascii_renderer_rows(ASCII_RENDERER *ascii_rd) {
    return ascii_rd->h / ascii_rd->fh;
}

void ascii_renderer_set_char(ASCII_RENDERER *ascii_rd, uint8_t c, int x, int y) {
    ascii_rd->char_grid[ascii_renderer_cols(ascii_rd) * y + x] = c;
}

uint8_t ascii_renderer_get_char(ASCII_RENDERER *ascii_rd, int x, int y) {
   return ascii_rd->char_grid[(ascii_renderer_cols(ascii_rd) * y) + x];
}

void ascii_renderer_present(ASCII_RENDERER *ascii_rd) {
    SDL_RenderClear(ascii_rd->renderer);

    int rows = ascii_renderer_rows(ascii_rd);
    int cols = ascii_renderer_cols(ascii_rd);

    for(int y = 0; y < rows; y++) {
        for(int x = 0; x < cols; x++) {
            ascii_renderer_print_char(ascii_rd,
                ascii_renderer_get_char(ascii_rd, x, y), x, y);
        }
    }

    SDL_RenderPresent(ascii_rd->renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Text Editor",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, 0);

    TTF_Init();

    ASCII_RENDERER *ascii_rd = create_assci_renderer(window, 40);
    
    char c[] = "Hello World";

    for (int i = 0; i < ARRAY_SIZE(c) - 1; i++) {
        ascii_renderer_set_char(ascii_rd, c[i], i, 0);
        ascii_renderer_present(ascii_rd);
        SDL_Delay(300);
    }

    TTF_Quit();
    SDL_Quit();
    return 0;
}