#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdlib>
#include <vector>

struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

struct Color
{
    int red;
    int green;
    int blue;
    int alpha;
};

const int cell_width = 30;
const int cell_height = 30;
const int cell_width_num = 10;
const int cell_height_num = 10;

std::vector<std::vector<int>> tile = {
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

void draw_background(SDL_Renderer* renderer) 
{
    Color bg_c {255, 255, 255, 255};
    SDL_SetRenderDrawColor(renderer, bg_c.red, bg_c.green, bg_c.blue, bg_c.alpha);
    SDL_RenderClear(renderer);

    Color line_c {0, 0, 0, 255};
    SDL_Rect r;
    r.x = 0;
    r.y = 0;
    r.w = cell_width * cell_width_num;
    r.h = cell_height * cell_height_num;
    SDL_SetRenderDrawColor(renderer, line_c.red, line_c.green, line_c.blue, line_c.alpha);
    SDL_RenderDrawRect(renderer, &r);
    SDL_RenderPresent(renderer);
}

void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    
    // background
    draw_background(renderer);
    
    // moving blue rectangle
    SDL_Rect r;
    r.x = ctx->iteration % 255;
    r.y = 50;
    r.w = 50;
    r.h = 50;
    Color color {0, 0, 255, 255};
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
    SDL_RenderDrawRect(renderer, &r);

    SDL_RenderPresent(renderer);

    ctx->iteration++;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    const int width = 300;
    const int height = 360;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1;
    const int fps = -1;
    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
