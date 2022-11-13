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
       | seg |
    ]
*/

typedef enum _segment { a, b, c, d, e, f, g, h, j, k, m } segment;
SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];
bool decimalPointState = false;

void setPixel(map* map,pixel position,uint8_t value)
{
    if(!map)
    {
        return;
    }
    map->pixels[position.y][position.x] = value;
}

__STATIC_INLINE bool isValid(pixel position)
{
    return position.x>=0&&position.x<WIDTH&&position.y>=0&&position.y<HEIGHT;
}

void drawLine(map* map,pixel startPosition,pixel endPosition,uint8_t value)
{
    if(!map||!isValid(startPosition)||!isValid(endPosition))
    {
        return;
    }
    bool vertical = endPosition.x==startPosition.x;
    bool horizontal = endPosition.y==startPosition.y;
    if((abs(endPosition.x-startPosition.x)>0||abs(endPosition.y-startPosition.y)>0)&&(vertical||(horizontal))) //check if it can be represented as a segment
    {
        
        if(vertical)
        {
            int8_t dir = endPosition.y-startPosition.y>0? 1:-1;
            for(int8_t i = startPosition.y;i!=endPosition.y+dir&&i>=0&&i<HEIGHT;i+=dir)
            {
                map->pixels[i][startPosition.x] = value;
            }
        }
        else if(horizontal)
        {
            int8_t dir = endPosition.x-startPosition.x>0? 1:-1;
            for(int8_t i = startPosition.x;i!=endPosition.x+dir&&i>=0&&i<WIDTH;i+=dir)
            {
                map->pixels[startPosition.y][i] = value;
            }
        }
    }
}

void setSegmentByPixel(pixel startPixel,pixel endPixel)
{
    if(startPixel.x<WIDTH-2)
    {
        segment seg = -1;
        if(endPixel.x-startPixel.x)
        {
            //horizontal segments
            switch(startPixel.y)
            {
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
        }
        else
        {
            //vertical segments
            switch(startPixel.y)
            {
                case 0:
                    seg = f;
                    break;
                case 2:
                    seg = e;
                    break;
            }
        }
        if(seg == -1)
        {
            return;
        }
        lowerCharSegments[startPixel.x/2].raw |= 1 << seg;
        if(seg==g)
        {
            lowerCharSegments[startPixel.x/2].raw |= 1 << m;
        }
    }
    else if(startPixel.x==WIDTH-1)
    {
        //last line 8 instead of E
        segment seg = -1;
        switch(startPixel.y)
        {
            case 0:
                seg = b;
                break;
            case 2:
                seg = c;
                break;
        }
        if(seg == -1)
        {
            return;
        }
        lowerCharSegments[startPixel.x/2-1].raw |= 1 << seg;
    }
}

void displayMap(map* map)
{
    if(!map)
    {
        return;
    }
    memset(lowerCharSegments,0,sizeof(lowerCharSegments));//clear previous segment setup
    for(uint8_t i = 0; i < HEIGHT; i+=2)
    {
        for(uint8_t j = 0; j < WIDTH; j+=2)
        {
            if(i+2<HEIGHT&&map->pixels[i][j]&&map->pixels[i+1][j]&&map->pixels[i+2][j])
            {
                setSegmentByPixel((pixel){j,i},(pixel){j,i+2});
            }
            if(j+2<WIDTH&&map->pixels[i][j]&&map->pixels[i][j+1]&&map->pixels[i][j+2])
            {
                setSegmentByPixel((pixel){j,i},(pixel){j+2,i});
            }
        }
    }
    SegmentLCD_LowerSegments(lowerCharSegments);
}

void clearMap(map* map)
{
    memset(map->pixels,0,sizeof(map->pixels));
}

void toggleDecimalPoints()
{
    LCD_FreezeEnable(true);
    LCD_SegmentSet(7,12,!decimalPointState); //first decimal point
    LCD_SegmentSet(6,12,!decimalPointState); //third decimal point
    LCD_SegmentSet(5,12,!decimalPointState); //second decimal point
    LCD_SegmentSet(7,29,!decimalPointState); //fourth decimal point
    LCD_SegmentSet(7,31,!decimalPointState); //fifth decimal point
    decimalPointState=!decimalPointState;
    LCD_FreezeEnable(false);
}

bool cmpPixel(pixel a,pixel b)
{
    return a.x == b.x && a.y == b.y;
}