#include "snake.h"
#include "display.h"

void initSnake(snake *snk)
{
    snk->len    = 1 + 1;
    snk->dir    = right;
    snk->pos[0] = (pixel){0, 2};
    snk->pos[1] = (pixel){2, 2};
}

void stepSnake(snake *snk)
{
    for (int i = snk->len - 1; i > 0; i--) {
        snk->pos[i] = snk->pos[i - 1];
    }
}

void drawSnake(map *map, snake *snk)
{
    clearMap(map); // TODO: only clear the last segment
    for (int i = 0; i < snk->len - 1; i++) {
        drawLine(map, snk->pos[i], snk->pos[i + 1], 1);
    }
    displayMap(map);
}

