#include <stdlib.h>
#include <string.h>
#include "snake.h"
#include <time.h>

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

bool isOnSnake(pixel *pos,snake *snk)
{
    for(int i = 0; i < snk->len;i++)
    {
        if(snk->pos[i].x==pos->x&&snk->pos[i].y==pos->y)
        {
            return true;
        }
    }
    return false;
}

bool generateFood(snake *snk,food *food,volatile uint32_t *time)
{
    if(!snk||!food||!time)
    {
        return false;
    }
    uint32_t startTime = *time;
    do
    {
        do
        {
            srand(*time);
        }
        while((food->pos[0].x = rand()%15)%2!=0); //generate x coordinate
        do
        {
            srand(*time);
        }
        while((food->pos[0].y = rand()%5)%2!=0); //generate y coordinate
    }
    while(isOnSnake(&food->pos[0],snk)&&(*time-startTime)<1000);

    do
    {
        srand(*time);
        if(rand()%2)
        {
            food->pos[1].x = food->pos[0].x + 2 < WIDTH ? food->pos[0].x + 2 : food->pos[0].x - 2;
            food->pos[1].y = food->pos[0].y;
        }
        else
        {
            food->pos[1].y = food->pos[0].y + 2 < HEIGHT ? food->pos[0].y + 2 : food->pos[0].y - 2;
            food->pos[1].x = food->pos[0].x;
        }
    }
    while(isOnSnake(&food->pos[1],snk)&&(*time-startTime)<2000);
    if((*time-startTime)<2000)
    {
        food->eaten = false;
        return true;
    }
    return false;
}

void drawFood(map *map,food *food)
{
    drawLine(map,food->pos[0],food->pos[1],1);
}

bool isEating(snake *snk,food *food)
{
    if(!snk|!food|snk->len<2)
    {
        return false;
    }
    if((cmpPixel(snk->pos[0],food->pos[0])||cmpPixel(snk->pos[1],food->pos[0]))&&
    (cmpPixel(snk->pos[0],food->pos[1])||cmpPixel(snk->pos[1],food->pos[1])))
    {
        food->eaten = true;
        snk->len += 1;
        return true;
    }
    return false;
}
