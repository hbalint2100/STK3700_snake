#include <stdlib.h>
#include <string.h>
#include "snake.h"

void initSnake(snake *snk)
{
    memset(snk->pos, 0, sizeof(snk->pos));
    snk->len    = 1 + 1;
    snk->dir    = right;
    snk->pos[0] = (pixel){2, 2};
    snk->pos[1] = (pixel){0, 2};
}

void stepSnake(snake *snk)
{
    for (int i = snk->len - 1; i > 0; i--) {
        snk->pos[i] = snk->pos[i - 1];
    }

    switch (snk->dir) {
        case up:
            snk->pos[0].y -= 2;
            if (snk->pos[0].y < 0) {
                snk->pos[0].y = HEIGHT - 1 - 2;
                snk->pos[1].y = HEIGHT - 1;
            }
            break;
        case down:
            snk->pos[0].y += 2;
            if (snk->pos[0].y > HEIGHT - 1) {
                snk->pos[0].y = 0 + 2;
                snk->pos[1].y = 0;
            }
            break;
        case left:
            snk->pos[0].x -= 2;
            if (snk->pos[0].x < 0) {
                snk->pos[0].x = WIDTH - 1 - 2;
                snk->pos[1].x = WIDTH - 1;
            }
            break;
        case right:
            snk->pos[0].x += 2;
            if (snk->pos[0].x > WIDTH - 1) {
                snk->pos[0].x = 0 + 2;
                snk->pos[1].x = 0;
            }
            break;
    }
}

void drawSnake(map *map, snake *snk)
{
    clearMap(map); // TODO: only clear the last segment
    for (int i = 0; i < snk->len - 1; i++) {
        if (abs(snk->pos[i].x - snk->pos[i + 1].x) > 2)
            continue;
        drawLine(map, snk->pos[i], snk->pos[i + 1], 1);
    }
    displayMap(map);
}

bool checkCollision(snake *snk)
{
    bool retval = false;
    for (int i = 1; i < snk->len - 1; i++) {
        if (snk->pos[0].x == snk->pos[i].x && snk->pos[0].y == snk->pos[i].y) {
            retval = true;
            break;
        }
    }
    return retval;
}

