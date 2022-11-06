#pragma once

#include <stdbool.h>
#include "display.h"

#define MAX_LENGTH 37 // 6*5 + 7

typedef enum _direction { up = 0, right = 1, down = 2, left = 3 } direction;

typedef struct _snake {
    uint8_t            len;
    pixel              pos[MAX_LENGTH];
    volatile direction dir;
} snake;

void initSnake(snake *);

void stepSnake(snake *);

void drawSnake(map *, snake *);

bool checkCollision(snake *);

