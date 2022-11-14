#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "display.h"
#include "snake.h"

#define TICK 500

volatile uint32_t msTicks; /* counts 1ms timeTicks */
volatile bool     reset     = false;
volatile bool     game_over = false;
snake             snk;
food              _food;
map               _map;

void SysTick_Handler(void)
{
    msTicks++; /* increment counter necessary in Delay()*/
}

void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}

void UART0_RX_IRQHandler(void)
{
    switch (USART_RxDataGet(UART0)) {
        case 'd':
        case 67: // right arrow
            snk.dir = right;
            break;
        case 'a':
        case 68: // left arrow
            snk.dir = left;
            break;
        case 'w':
        case 65: // up arrow
            snk.dir = up;
            break;
        case 's':
        case 66: // down arrow
            snk.dir = down;
            break;
        case 'j':
            snk.dir = snk.dir == left ? up : snk.dir + 1;
            break;
        case 'b':
            snk.dir = snk.dir == up ? left : snk.dir - 1;
            break;
        case 'r':
            reset     = true;
            game_over = false;
            break;
    }
}

__STATIC_INLINE void UART_Init()
{
    CMU_ClockEnable(cmuClock_GPIO, true);  // GPIO clk enable
    CMU_ClockEnable(cmuClock_UART0, true); // UART clk enable

    GPIO_PinModeSet(gpioPortF, 7, gpioModePushPull, 1); // PF7 output 1
    GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 1); // PE0 output 1
    GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);    // PE1 input  0

    USART_InitAsync_TypeDef u = USART_INITASYNC_DEFAULT; // UART config
    USART_InitAsync(UART0, &u);                          // 115200 8N1
    UART0->ROUTE |= USART_ROUTE_LOCATION_LOC1;
    UART0->ROUTE |= USART_ROUTE_RXPEN | USART_ROUTE_TXPEN;

    USART_IntEnable(UART0, UART_IF_RXDATAV);
    NVIC_EnableIRQ(UART0_RX_IRQn);
}

int main()
{
    UART_Init();
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1);
    }
    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);

    initSnake(&snk);
    clearMap(&_map);
    _food.eaten = true;

    bool lengthChanged = true;

    while (1) {
        if (reset) {
            reset     = false;
            game_over = false;
            setDecimalPoints(false);
            initSnake(&snk);
            generateFood(&snk, &_food, &msTicks);
            lengthChanged = true;
        }

        if (lengthChanged) {
            SegmentLCD_Number(snk.len - 1);
            lengthChanged = false;
        }
        if (isEating(&snk, &_food)) {
            lengthChanged = true;
        }

        if (checkCollision(&snk)) {
            game_over = true;
        }
        if (_food.eaten && !generateFood(&snk, &_food, &msTicks)) {
            game_over = true;
        }

        drawSnake(&_map, &snk);
        drawFood(&_map, &_food);
        displayMap(&_map);

        Delay(TICK);

        stepSnake(&snk);

        while (game_over) {
            clearMap(&_map);
            displayMap(&_map);
            setDecimalPoints(true);
            Delay(TICK);
            setDecimalPoints(false);
            Delay(TICK);
        }
    }
}

