#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "em_lcd.h"
#define HEIGHT 5
#define WIDTH 15

//grid of pixels
typedef struct map
{
    uint8_t pixels[HEIGHT][WIDTH];
} map;

//represents a pixel position in map
typedef struct pixel
{
    int8_t x;
    int8_t y;
} pixel;

void drawLine(map* map,pixel startPosition,pixel endPosition,uint8_t value);

void setPixel(map* map,pixel position,uint8_t value);

void displayMap(map* map);

void clearMap(map* map);

void setDecimalPoints(bool value);

bool cmpPixel(pixel a,pixel b);

