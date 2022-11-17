#pragma once

#include <stdbool.h>
#include "display.h"

#define MAX_LENGTH  24 // ~ screen size
#define FOOD_LENGTH 2  // vector

typedef enum _direction { up = 0, right = 1, down = 2, left = 3 } direction;

typedef struct _snake {
    uint8_t            len;
    pixel              pos[MAX_LENGTH];
    direction          dir;
    volatile direction newDir;
    bool               lenChanged;
} snake;

typedef struct _food {
    pixel pos[FOOD_LENGTH];
} food;

void initSnake(snake *snake);

void setDir(snake *snake);

void stepSnake(snake *snake);

void drawSnake(map *map, snake *snake);

bool checkCollision(snake *snake);

bool generateFood(snake *snake, food *food, volatile uint32_t *time);

void drawFood(map *map, food *food);

bool isEating(snake *snake, food *food);

