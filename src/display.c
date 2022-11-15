#include "display.h"
#include "string.h"
#include "stdlib.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"

/*  bool display[5][15];
    [
            |seg|
        0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        0 x 0 x 0 x 0 x 0 x 0 x 0 x 0
        1 1 1 0 0 0 0 0 0 0 0 0 0 0 0
        0 x 0 x 0 x 0 x 0 x 0 x 0 x 0
        0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
        |seg|
    ]
*/

typedef enum _segment { a, b, c, d, e, f, g, h, j, k, m } segment;
SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

// setting pixel value in map
void setPixel(map *map, pixel position, uint8_t value)
{
    if (!map) {
        return;
    }
    map->pixels[position.y][position.x] = value;
}

// checks if pixel is in map's range
__STATIC_INLINE bool isValid(pixel position)
{
    return position.x >= 0 && position.x < WIDTH && position.y >= 0 && position.y < HEIGHT;
}

// draws line between two pixels if they are either in the same column or row
void drawLine(map *map, pixel startPosition, pixel endPosition, uint8_t value)
{
    if (!map || !isValid(startPosition) || !isValid(endPosition)) {
        return;
    }
    bool vertical   = endPosition.x == startPosition.x;
    bool horizontal = endPosition.y == startPosition.y;

    // check if it can be represented as a segment
    if ((abs(endPosition.x - startPosition.x) > 0 || abs(endPosition.y - startPosition.y) > 0) &&
        (vertical || horizontal)) {
        if (vertical) {
            int8_t dir = endPosition.y - startPosition.y > 0 ? 1 : -1;

            // drawing vertical line with the desired direction
            for (int8_t i = startPosition.y; i != endPosition.y + dir && i >= 0 && i < HEIGHT; i += dir) {
                map->pixels[i][startPosition.x] = value;
            }
        } else if (horizontal) {
            int8_t dir = endPosition.x - startPosition.x > 0 ? 1 : -1;

            // drawing horizontal line with the desired direction
            for (int8_t i = startPosition.x; i != endPosition.x + dir && i >= 0 && i < WIDTH; i += dir) {
                map->pixels[startPosition.y][i] = value;
            }
        }
    }
}

void setSegmentByPixel(pixel startPixel, pixel endPixel)
{
    if (startPixel.x < WIDTH - 2) {
        segment seg = -1;
        if (endPixel.x - startPixel.x) {
            // horizontal segments
            switch (startPixel.y) {
                case 0:
                    seg = a;
                    break;
                case 2:
                    seg = g;
                    break;
                case 4:
                    seg = d;
                    break;
            }
        } else {
            // vertical segments
            switch (startPixel.y) {
                case 0:
                    seg = f;
                    break;
                case 2:
                    seg = e;
                    break;
            }
        }
        if (seg == -1) {
            return; // segment not found
        }
        lowerCharSegments[startPixel.x / 2].raw |= 1 << seg;
        if (seg == g) { // the middle horizontal pixels m & g are combined
            lowerCharSegments[startPixel.x / 2].raw |= 1 << m;
        }
    } else if (startPixel.x == WIDTH - 1) {
        // last line 8 instead of E
        segment seg = -1;
        switch (startPixel.y) {
            case 0:
                seg = b;
                break;
            case 2:
                seg = c;
                break;
        }
        if (seg == -1) {
            return;
        }
        lowerCharSegments[startPixel.x / 2 - 1].raw |= 1 << seg;
    }
}

void displayMap(map *map)
{
    if (!map) {
        return;
    }
    memset(lowerCharSegments, 0, sizeof(lowerCharSegments)); // clear previous segment setup
    for (uint8_t i = 0; i < HEIGHT; i += 2) {
        for (uint8_t j = 0; j < WIDTH; j += 2) {
            // checks if three pixels in a column are on
            if (i + 2 < HEIGHT && map->pixels[i][j] && map->pixels[i + 1][j] && map->pixels[i + 2][j]) {
                setSegmentByPixel((pixel){j, i}, (pixel){j, i + 2}); // setting vertical segments
            }
            // checks if three pixels in a row are on
            if (j + 2 < WIDTH && map->pixels[i][j] && map->pixels[i][j + 1] && map->pixels[i][j + 2]) {
                setSegmentByPixel((pixel){j, i}, (pixel){j + 2, i}); // setting horizontal segments
            }
        }
    }
    SegmentLCD_LowerSegments(lowerCharSegments);
}

void clearMap(map *map)
{
    memset(map->pixels, 0, sizeof(map->pixels));
}

void setDecimalPoints(bool value)
{
    LCD_FreezeEnable(true);
    LCD_SegmentSet(7, 12, value); // 1. decimal point
    LCD_SegmentSet(5, 12, value); // 2. decimal point
    LCD_SegmentSet(6, 12, value); // 3. decimal point
    LCD_SegmentSet(7, 29, value); // 4. decimal point
    LCD_SegmentSet(7, 31, value); // 5. decimal point
    LCD_FreezeEnable(false);
}

bool cmpPixel(pixel a, pixel b) // checks whether a & b pixels are the same
{
    return a.x == b.x && a.y == b.y;
}

