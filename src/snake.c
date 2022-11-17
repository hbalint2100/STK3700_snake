#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "display.h"
#include "snake.h"

// set starting position and length
void initSnake(snake *snake)
{
    if (!snake) {
        return;
    }
    memset(snake->pos, -1, sizeof(snake->pos));
    snake->len        = 1 + 1; // head has length 2 because it's a vector
    snake->lenChanged = true;
    snake->dir        = right;
    snake->pos[0]     = (pixel){2, 2};
    snake->pos[1]     = (pixel){0, 2};
}

// move
void stepSnake(snake *snake)
{
    if (!snake) {
        return;
    }
    // shift snake except its head
    for (int8_t i = snake->len - 1; i > 0; i--) {
        snake->pos[i] = snake->pos[i - 1];
    }

    // New position for head according to direction.
    // When on overflow happens we would need a new position because the head takes
    // one away, but instead of dynamically increasing and decreasing the number of
    // positions we deal with these edge cases in drawSnake().
    // for example:
    //   before stepSnake(): snake.pos = {{14, 2}, {12, 2}, {10, 2}} and dir = right
    //   after stepSnake():  snake.pos = {{2, 2}, {0, 2}, {12, 2}}
    //   we lost {14, 2} which would be needed to draw a segment between {12, 2}, {14, 2}
    switch (snake->dir) {
        case up:
            snake->pos[0].y -= 2; // set head's direction to up
            if (snake->pos[0].y < 0) {
                // move head to the bottom of the screen
                snake->pos[0].y = HEIGHT - 1 - 2;
                snake->pos[1].y = HEIGHT - 1;
            }
            break;
        case down:
            snake->pos[0].y += 2; // set head's direction to down
            if (snake->pos[0].y > HEIGHT - 1) {
                // move head to the top of the screen
                snake->pos[0].y = 0 + 2;
                snake->pos[1].y = 0;
            }
            break;
        case left:
            snake->pos[0].x -= 2; // set head's direction to left
            if (snake->pos[0].x < 0) {
                // move head to the right of the screen
                snake->pos[0].x = WIDTH - 1 - 2;
                snake->pos[1].x = WIDTH - 1;
            }
            break;
        case right:
            snake->pos[0].x += 2; // set head's direction to right
            if (snake->pos[0].x > WIDTH - 1) {
                // move head to the left of the screen
                snake->pos[0].x = 0 + 2;
                snake->pos[1].x = 0;
            }
            break;
    }
}

// draws snake on map
void drawSnake(map *map, snake *snake)
{
    if (!map || !snake) {
        return;
    }

    for (uint8_t i = 0; i < snake->len - 1 - (snake->lenChanged == true); i++) {
        // All the edge cases below occur because in stepSnake() an overflow happened
        // and we would need an extra position to draw a segment. Instead of dynamically
        // increasing and decreasing the number of positions we deal with them here.

        // overflow on the left or right
        if (abs(snake->pos[i].x - snake->pos[i + 1].x) > 2) {
            // the most right position is missing -> needs to be patched
            // for example:
            //   before stepSnake(): snake.pos = {{14, 2}, {12, 2}, {10, 2}} and dir = right
            //   after stepSnake():  snake.pos = {{2, 2}, {0, 2}, {12, 2}}
            //   we lost {14, 2} which would be needed to draw a segment between {12, 2}, {14, 2}
            if (snake->pos[i + 1].x == WIDTH - 3) {
                drawLine(map, snake->pos[i + 1], (pixel){WIDTH - 1, snake->pos[i + 1].y}, 1);
            }
            // the most left position is missing -> needs to be patched
            else if (snake->pos[i + 1].x == 2) {
                drawLine(map, snake->pos[i + 1], (pixel){0, snake->pos[i + 1].y}, 1);
            }
        }
        // when snake goes sideways on the top or bottom and turns so it overflows, the
        // position where it rotated goes missing because of the head
        // for example:
        //   before stepSnake(): snake.pos = {{6, 0}, {4, 0}, {2, 0}} and dir = up
        //   after stepSnake():  snake.pos = {{6, 2}, {6, 4}, {4, 0}}
        //   we lost {6, 0} which would be needed to draw a segment between {4, 0}, {6, 0}
        else if (abs(snake->pos[i].y - snake->pos[i + 1].y) > 2 && snake->pos[i].x != snake->pos[i + 1].x) {
            // patch the missing position
            drawLine(map, snake->pos[i + 1], (pixel){snake->pos[i].x, snake->pos[i + 1].y}, 1);
        }
        // snake goes vertically and constantly overflows
        // (only makes sense when length is 2 + 1, otherwise it dies)
        else if (!(snake->dir % 2) && snake->len > 2 && cmpPixel(snake->pos[i], snake->pos[i + 2])) {
            drawLine(map, (pixel){snake->pos[i + 2].x, snake->pos[i + 2].y - 2},
                     (pixel){snake->pos[i + 2].x, snake->pos[i + 2].y + 2}, 1);
        }
        // no overflow
        else {
            drawLine(map, snake->pos[i], snake->pos[i + 1], 1);
        }
    }
}

bool checkCollision(snake *snake)
{
    if (!snake) {
        return false;
    }
    for (uint8_t i = 1; i < snake->len - 1; i++) {
        if (cmpPixel(snake->pos[0], snake->pos[i])) { // check if the head is in the body
            return true;                              // snake crashed into itself
        }
    }
    return false;
}

// checks if the given coordinate is on snake's body
bool isOnSnake(pixel *pos, snake *snake)
{
    if (!pos || !snake) {
        return false;
    }
    for (uint8_t i = 0; i < snake->len; i++) {
        if (cmpPixel(snake->pos[i], *pos)) {
            return true;
        }
    }
    return false;
}

bool generateFood(snake *snake, food *food, volatile uint32_t *time)
{
    if (!snake || !food || !time) {
        return false;
    }

    srand(*time);
    const uint8_t timeLimit = 200;
    uint32_t      startTime = *time;
    do {
        do { // generate first coordinate of food
            food->pos[0].x = rand() % (WIDTH + 1); // +1 for equal chances
            if (food->pos[0].x % 2 != 0) {
                (food->pos[0].x)--;
            }
            food->pos[0].y = rand() % (HEIGHT + 1); // +1 for equal chances
            if (food->pos[0].y % 2 != 0) {
                (food->pos[0].y)--;
            }
        // try while first coordinate is on snake and if stuck->exit
        } while (isOnSnake(&food->pos[0], snake) && (*time - startTime) < timeLimit);

        uint8_t dice = rand();
        for (uint8_t i = 0; i < 2; i++) {
            if (dice % 2) { // random direction of food if odd -> horizontal if even -> vertical
                // generate second coordinate of food
                food->pos[1].x = food->pos[0].x + 2 < WIDTH ? food->pos[0].x + 2 : food->pos[0].x - 2;
                food->pos[1].y = food->pos[0].y;
            } else {
                // generate second coordinate of food
                food->pos[1].y = food->pos[0].y + 2 < HEIGHT ? food->pos[0].y + 2 : food->pos[0].y - 2;
                food->pos[1].x = food->pos[0].x;
            }
            if (isOnSnake(&food->pos[1], snake)) {
                dice++; // if first direction is on snake try the other one
            } else {
                break;
            }
        }
    // try while coordinates are on snake and if stuck->exit
    } while (isOnSnake(&food->pos[1], snake) && (*time - startTime) < timeLimit);

    if ((*time - startTime) < timeLimit) {
        return true;
    }
    return false; // stuck in loop
}

// draws food on map
void drawFood(map *map, food *food)
{
    if (!map || !food) {
        return;
    }
    drawLine(map, food->pos[0], food->pos[1], 1);
}

// checks if snake is currently eating food -> increases length and sets food as eaten
bool isEating(snake *snake, food *food)
{
    if (!snake || !food || snake->len < 2) {
        return false;
    }
    //checks if snake's head is on food
    if ((cmpPixel(snake->pos[0], food->pos[0]) || cmpPixel(snake->pos[1], food->pos[0])) &&
        (cmpPixel(snake->pos[0], food->pos[1]) || cmpPixel(snake->pos[1], food->pos[1]))) {
        snake->len += 1;
        snake->lenChanged = true;
        return true;
    }
    return false;
}

