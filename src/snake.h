#pragma once

#include "display.h"

#define MAX_LENGTH 50

typedef enum _direction { up, down, left, right } direction;

typedef struct _snake {
    uint8_t            len;
    pixel              pos[MAX_LENGTH];
    volatile direction dir;
} snake;

void initSnake(snake *);

void stepSnake(snake *);

void drawSnake(map *, snake *);

