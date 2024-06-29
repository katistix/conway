#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdio.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CELL_SIZE 10
#define GRID_WIDTH WINDOW_WIDTH / CELL_SIZE
#define GRID_HEIGHT WINDOW_HEIGHT / CELL_SIZE
#define FPS 60


typedef enum { EMPTY, ALIVE } CellType;

// Global variables
CellType grid[GRID_WIDTH][GRID_HEIGHT];
CellType next_grid[GRID_WIDTH][GRID_HEIGHT];
bool running = false;


void init_grid()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            grid[x][y] = EMPTY;
        }
    }
}

void update_grid()
{
    // Empty the next grid
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            next_grid[x][y] = EMPTY;
        }
    }

    // Update the next grid based on the current grid
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            int alive_neighbors = 0;

            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0)
                    {
                        continue;
                    }

                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0 || nx >= GRID_WIDTH || ny < 0 || ny >= GRID_HEIGHT)
                    {
                        continue;
                    }

                    if (grid[nx][ny] == ALIVE)
                    {
                        alive_neighbors++;
                    }
                }
            }

            if (grid[x][y] == ALIVE)
            {
                if (alive_neighbors == 2 || alive_neighbors == 3)
                {
                    next_grid[x][y] = ALIVE;
                }
            }
            else
            {
                if (alive_neighbors == 3)
                {
                    next_grid[x][y] = ALIVE;
                }
            }
        }
    }

    // Copy the next grid to the current grid
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            grid[x][y] = next_grid[x][y];
        }
    }
}

void render_grid(SDL_Renderer* renderer, bool running)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for empty cells
    SDL_RenderClear(renderer);

    // Render a thin line grid
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        SDL_RenderDrawLine(renderer, x * CELL_SIZE, 0, x * CELL_SIZE, WINDOW_HEIGHT);
    }
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        SDL_RenderDrawLine(renderer, 0, y * CELL_SIZE, WINDOW_WIDTH, y * CELL_SIZE);
    }


    // Render the cells
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            if (grid[x][y] == ALIVE)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for alive cells
                SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }

    // Render a thick red line around the grid if paused
    if (!running)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }
    // Render a thick green line around the grid if running
    else
    {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    }
    // draw the rectangle
    SDL_Rect rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderDrawRect(renderer, &rect);

    // Present the renderer
    SDL_RenderPresent(renderer);
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Conway's Game of Life in C and SDL2",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    bool quit = false;
    SDL_Event e;

    bool running = false;
    init_grid();

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (running) // ignore mouse clicks when running
                {
                    continue;
                }

                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                int gridX = mouseX / CELL_SIZE;
                int gridY = mouseY / CELL_SIZE;
                if (gridX < GRID_WIDTH && gridY < GRID_HEIGHT)
                {
                    if (grid[gridX][gridY] == ALIVE)
                    {
                        grid[gridX][gridY] = EMPTY;
                    }
                    else
                    {
                        grid[gridX][gridY] = ALIVE;
                    }
                }
            }

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_SPACE)
                {
                    printf("Space key pressed\n");
                    running = !running;
                }
            }
        }

        if (running)
        {
            update_grid();
            SDL_Delay(1000 / FPS);
        }
        render_grid(renderer, running);
    }


    return 0;
}
