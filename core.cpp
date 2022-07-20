#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdlib>
#include <vector>
#include <unistd.h>

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

struct Direction
{
    int x;
    int y;
};

const int cell_width = 30;
const int cell_height = 30;
const int cell_width_num = 10;
const int cell_height_num = 10;

std::vector<std::vector<int>> tile = {
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

std::vector<Direction> directions = {
    Direction {-1, -1},
    Direction {0, -1},
    Direction {1, -1},
    Direction {-1, 0},
    Direction {1, 0},
    Direction {-1, 1},
    Direction {0, 1},
    Direction {1, 1}
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

void draw_cell(SDL_Renderer* renderer, const int i, const int j) {
    if(tile[i][j] == 1) {
        Color cell_c {0, 0, 0, 255};
        SDL_Rect r;
        r.x = j * cell_width;
        r.y = i * cell_height;
        r.w = cell_width;
        r.h = cell_height;
        SDL_SetRenderDrawColor(renderer, cell_c.red, cell_c.green, cell_c.blue, cell_c.alpha);
        SDL_RenderFillRect(renderer, &r);
        SDL_RenderPresent(renderer);
    } 
}

void draw_cells(SDL_Renderer* renderer)
{
    for(int i=0; i!=cell_width_num; ++i) {
        for(int j=0; j!=cell_height_num; ++j) {
            draw_cell(renderer, i, j);
        }
    }
}

int at(const int i, const int j) {
    if(i<0 || j < 0 || i >= cell_height_num || j >= cell_width_num) {
        return 0;
    }
    return tile[i][j];
}

bool is_rule1(const int i, const int j) {
    int alive_count = 0;
    for(auto direction : directions) {
        alive_count += at(i + direction.x, j + direction.y);
    }

    if(alive_count == 3 && tile[i][j] == 0) {
        return true;
    } else {
        return false;
    }
}

bool is_rule2(const int i, const int j) {
    int alive_count = 0;
    for(auto direction : directions) {
        alive_count += at(i + direction.x, j + direction.y);
    }

    if(alive_count == 2 || alive_count == 3) {
        if(tile[i][j] == 1) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

int dead_or_alive(const int i, const int j)
{
    /**
     * Rule 1: New cells will be birth if surrounding three cells are alive
     * Rule 2: Cells touching more than two cells (>=2) will be alive
     * Rule 3: Cells touching less than two cells (<2) will be dead
     * Rule 4: Cells touching more than three cells (>=4) will be dead
    **/

    /**
    if(is_rule1(i, j) || is_rule2(i, j)) {
        return 1;
    }

    if(is_rule3(i, j) || is_rule4(i, j)) {
        return 0;
    }

    return 0;
    **/
    if(is_rule1(i, j) || is_rule2(i, j)) {
        return 1;
    } else {
        return 0;
    }
}

void update_cells()
{
    // check status of cells in the next generation
    std::vector<std::vector<int>> is_alive = tile;
    for(int i=0; i!=cell_width_num; ++i) {
        for(int j=0; j!=cell_height_num; ++j) {
            is_alive[i][j] = dead_or_alive(i, j);
        }
    }
    
    // update cells
    for(int i=0; i!=cell_width_num; ++i) {
        for(int j=0; j!=cell_height_num; ++j) {
            if(is_alive[i][j]) {
                tile[i][j] = 1;
            } else {
                tile[i][j] = 0;
            }
        }
    }
}

void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    
    // background
    draw_background(renderer);
    
    // draw pixels
    draw_cells(renderer);
    
    // update cell states 
    update_cells();

    ctx->iteration++;
    sleep(1);
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
