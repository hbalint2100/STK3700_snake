#pragma once

#include<stdint.h>
#include <stdbool.h>
#define HEIGHT 5
#define WIDTH 15


typedef struct map
{
    uint8_t pixels[HEIGHT][WIDTH];
} map;

typedef struct pixel
{
    int8_t x;
    int8_t y;
} pixel;

void drawLine(map* map,pixel startPosition,pixel endPosition,uint8_t value);

void setPixel(map* map,pixel position,uint8_t value);

void displayMap(map* map);

void clearMap(map* map);

void toggleDecimalPoints();

bool cmpPixel(pixel a,pixel b);