#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define BLOCKED -1
#define INSIDE(i, j) ((i>=0 && j>=0 && i<24 && j<32)?1:0)

#define ROWS 24
#define COLS 32


typedef struct
{
    int x, y, visited;
}punct;

int maze[ROWS][COLS];
punct q[ROWS * COLS];
punct where_from[ROWS][COLS];

void generate_maze(int maze[ROWS][COLS], punct where_from[ROWS][COLS])
{
    srand(time(0));

    for (int i=0; i<ROWS; i++)
        for (int j=0; j<COLS; j++)
            if (!(i == 0 && j == 0) && !(i == ROWS - 1 && j == COLS - 1))
                if (rand() % 3 == 0)
                    maze[i][j] = BLOCKED;

}

void push(punct q[ROWS * COLS], int* len, punct node)
{
    q[*len] = node;
    (*len)++;
}

punct pop(punct q[ROWS * COLS], int* len)
{
    punct ret = q[0];

    for (int i=0; i<*len; i++)
        q[i] = q[i+1];

    (*len)--;

    return ret;
}


void bfs_path(int maze[ROWS][COLS], punct q[ROWS * COLS], int* len, punct where_from[ROWS][COLS])
{
    where_from[0][0].visited = 1;
    push(q, len, where_from[0][0]);

    while (*len != 0)
    {
        punct aux = pop(q, len);

        if (aux.x == ROWS - 1 && aux.y == COLS - 1)
            break;

        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++)
                if (INSIDE(aux.x + i, aux.y + j) && maze[aux.x + i][aux.y + j] != BLOCKED && where_from[aux.x + i][aux.y + j].visited != 1)
                {
                    punct p;
                    where_from[aux.x + i][aux.y + j].visited = 1;
                    where_from[aux.x + i][aux.y + j].x = aux.x;
                    where_from[aux.x + i][aux.y + j].y = aux.y;
                    p.x = aux.x + i;
                    p.y = aux.y + j;
                    push(q, len, p);
                }
    }
}


void flood_fill(int maze[ROWS][COLS],punct where_from[ROWS][COLS], int i, int j)
{
    where_from[i][j].visited = 1;
    if (INSIDE(i, j+1) && maze[i][j+1] != BLOCKED && where_from[i][j+1].visited != 1)
    {
        maze[i][j+1] = maze[i][j] + 1;
        where_from[i][j+1].x = i;
        where_from[i][j+1].y = j;
        flood_fill(maze, where_from, i, j+1);
    }
    if (INSIDE(i, j-1) && maze[i][j-1] != BLOCKED && where_from[i][j-1].visited != 1)
    {
        maze[i][j-1] = maze[i][j] + 1;
        where_from[i][j-1].x = i;
        where_from[i][j-1].y = j;
        flood_fill(maze, where_from, i, j-1);
    }
    if (INSIDE(i+1, j) && maze[i+1][j] != BLOCKED && where_from[i+1][j].visited != 1)
    {
        maze[i+1][j] = maze[i][j] + 1;
        where_from[i+1][j].x = i;
        where_from[i+1][j].y = j;
        flood_fill(maze, where_from, i+1, j);
    }
    if (INSIDE(i-1, j) && maze[i-1][j] != BLOCKED && where_from[i-1][j].visited != 1)
    {
        maze[i-1][j] = maze[i][j] + 1;
        where_from[i-1][j].x = i;
        where_from[i-1][j].y = j;
        flood_fill(maze, where_from, i-1, j);
    }
}

punct* ConstructPath(punct where_from[ROWS][COLS], int* len)
{
    punct* path = (punct*)malloc(ROWS * COLS * sizeof(punct));
    int c = 0, i = ROWS - 1, j = COLS - 1, auxi, auxj;

    while (1)
    {
        path[c].x = i;
        path[c].y = j;
        c++;

        auxi = where_from[i][j].x;
        auxj = where_from[i][j].y;

        i = auxi;
        j = auxj;

        if (i == 0 && j == 0)
            break;
    }

    *len = c;
    return path;
}

int find(int x, int y, punct path[ROWS * COLS], int len)
{
    for (int i=0; i<len; i++)
        if (path[i].x == x && path[i].y == y)
            return 1;

    return 0;
}

void BlitPath(SDL_Renderer* renderer, int maze[ROWS][COLS], punct path[ROWS * COLS], int len)
{
    SDL_Rect rect = {0, 0, 20, 20};

    for (int i = 0; i<ROWS; i++)
        for (int j =0; j<COLS; j++)
        {
            if (maze[i][j] == BLOCKED)
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

            else if (find(i, j, path, len) == 1)
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0x00);

            else
                SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0x00);

            rect.x = 20 * i;
            rect.y = 20 * j;

            SDL_RenderDrawRect(renderer, &rect);
            SDL_RenderFillRect(renderer, &rect);
        }

     SDL_SetRenderDrawColor(renderer, 0xf, 0xf, 0xf, 0x00);
     for (int i=0; i<480; i += 20)
            SDL_RenderDrawLine(renderer, i, 0, i, 640);

     for (int i=0; i<640; i += 20)
            SDL_RenderDrawLine(renderer, 0, i, 480, i);

    SDL_RenderPresent(renderer);
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Pathfinder", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 640, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_PRESENTVSYNC);


    generate_maze(maze, where_from);

    for (int i=0; i<ROWS; i++)
    {
        for (int j=0; j<COLS; j++)
            printf("%d ", maze[i][j]);

        printf("\n");
    }

    printf("\n");
    //flood_fill(maze, where_from, 0, 0);
    int qlen = 0;
    bfs_path(maze, q, &qlen, where_from);

    /*for (int i=0; i<ROWS; i++)
    {
        for (int j=0; j<COLS; j++)
            printf("%d ", maze[i][j]);

        printf("\n");
    }*/

    printf("\n");

    int plen;
    punct* p = ConstructPath(where_from, &plen);
    for (int i=0; i<plen; i++)
        {
            printf("{%d , %d}", p[i].x, p[i].y);
        }

    for (;;)
    {
        SDL_Event e;
        if (SDL_PollEvent(&e))
        {

            if (e.type == SDL_QUIT)
                return 0;

        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        BlitPath(renderer, maze, p, plen);
    }




    return 0;
}
