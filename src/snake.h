#pragma once

#include <stdbool.h>
#include "display.h"

#define MAX_LENGTH  24
#define FOOD_LENGTH 2

typedef enum _direction { up = 0, right = 1, down = 2, left = 3 } direction;

typedef struct _snake {
    uint8_t            len;
    pixel              pos[MAX_LENGTH];
    volatile direction dir;
} snake;

typedef struct _food {
    pixel pos[FOOD_LENGTH];
    bool  eaten;
} food;

void initSnake(snake *);

void stepSnake(snake *);

void drawSnake(map *, snake *);

bool checkCollision(snake *);

bool generateFood(snake *, food *, volatile uint32_t *time);

void drawFood(map *, food *);

bool isEating(snake *, food *);