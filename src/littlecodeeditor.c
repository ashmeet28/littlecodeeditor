#include "SDL.h"
#include "SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

#define ASCII_PRINTABLE_CHARS_COUNT 95

#define L_EDITOR_CHAR_GRID_CAP 1048576

typedef struct {
    int w;
    int h;
    int fw;
    int fh;
    int cursor;
    int offset_x;
    int offset_y;
    uint8_t *char_buf;
    uint8_t *char_grid;
    SDL_Renderer *main_renderer;
    SDL_Surface *regular_font_surface;
    SDL_Texture *regular_font_texture;
} L_EDITOR;

SDL_Surface *create_regular_ascii_font_surface(int ptsize)
{
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

L_EDITOR *create_assci_renderer(SDL_Window *window, int ptsize)
{
    L_EDITOR *l_ed = (L_EDITOR *)malloc(sizeof(L_EDITOR));

    SDL_GetWindowSize(window, &(l_ed->w), &(l_ed->h));

    l_ed->main_renderer = SDL_CreateRenderer(window, -1, 0);
    l_ed->regular_font_surface = create_regular_ascii_font_surface(ptsize);
    l_ed->fw = (l_ed->regular_font_surface->w) / ASCII_PRINTABLE_CHARS_COUNT;
    l_ed->fh = (l_ed->regular_font_surface->h);
    l_ed->cursor = 0;
    l_ed->regular_font_texture = SDL_CreateTextureFromSurface(l_ed->main_renderer,
                                                                    l_ed->regular_font_surface);

    l_ed->char_grid = (uint8_t *)malloc(L_EDITOR_CHAR_GRID_CAP);
    for (int i = 0; i < L_EDITOR_CHAR_GRID_CAP; i++) {
        l_ed->char_grid[i] = 0x20;
    }
    l_ed->char_buf = (uint8_t *)malloc(8000000);

    return l_ed;
}

void l_editor_print_char(L_EDITOR *l_ed, uint8_t c, int x, int y)
{
    SDL_Rect r1;
    SDL_Rect r2;

    int fw = l_ed->fw;
    int fh = l_ed->fh;

    r1.x = fw * (int)(c - 0x20);
    r1.y = 0;
    r1.w = fw;
    r1.h = fh;

    r2.x = fw * x;
    r2.y = fh * y;
    r2.w = fw;
    r2.h = fh;

    SDL_RenderCopy(l_ed->main_renderer, l_ed->regular_font_texture, &r1, &r2);
}

int l_editor_cols(L_EDITOR *l_ed)
{
    return l_ed->w / l_ed->fw;
}

int l_editor_rows(L_EDITOR *l_ed)
{
    return l_ed->h / l_ed->fh;
}

void l_editor_set_char(L_EDITOR *l_ed, uint8_t c, int x, int y)
{
    l_ed->char_grid[(l_editor_cols(l_ed) * y) + x] = c;
}

uint8_t l_editor_get_char(L_EDITOR *l_ed, int x, int y)
{
   return l_ed->char_grid[(l_editor_cols(l_ed) * y) + x];
}

void l_editor_put_char_buf_on_grid(L_EDITOR *l_ed)
{
    int x = 0;
    int y = 0;
    for (int i = 0; l_ed->char_buf[i] != 0; i++) {
        if (l_ed->char_buf[i] == 0x0a){
            x = 0;
            y++;
        } else {
            if (x < l_editor_cols(l_ed) && y < l_editor_rows(l_ed)) {
                l_editor_set_char(l_ed, l_ed->char_buf[i], x, y);
                x++;
            }
        }
    }
}

void l_editor_present(L_EDITOR *l_ed)
{
    SDL_RenderClear(l_ed->main_renderer);

    int rows = l_editor_rows(l_ed);
    int cols = l_editor_cols(l_ed);

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            l_editor_print_char(l_ed, l_editor_get_char(l_ed, x, y), x, y);
        }
    }

    SDL_RenderPresent(l_ed->main_renderer);
}

void start_handling_keyboard_events(L_EDITOR *l_ed)
{
    int quit_loop = 0;
    int i = 0;
    while(!quit_loop) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_QUIT:
                quit_loop = 1;
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    strcat(l_ed->char_buf, "\x0a");
                }
                break;
            case SDL_TEXTINPUT:
                if (strlen(ev.text.text) == 1 && (ev.text.text)[0] >= 0x20 && (ev.text.text)[0] <= 0x7e) {
                    strcat(l_ed->char_buf, ev.text.text);
                    l_editor_put_char_buf_on_grid(l_ed);
                }
                break;
            }
            l_editor_present(l_ed);
            SDL_Delay(100);
        }
    }
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    SDL_Window *window = SDL_CreateWindow("Text Editor",
                                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, 0);

    TTF_Init();

    L_EDITOR *l_ed = create_assci_renderer(window, 40);

    start_handling_keyboard_events(l_ed);

    TTF_Quit();
    SDL_Quit();
    return 0;
}