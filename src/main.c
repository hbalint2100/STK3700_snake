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

volatile uint32_t msTicks; // counts 1ms timeTicks
volatile bool     reset    = true;
volatile bool     gameOver = false;
snake             snk;
food              fd;
map               mp;

void SysTick_Handler(void)
{
    msTicks++; // increment counter necessary in Delay()
}

void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks);
}

void UART0_RX_IRQHandler(void)
{
    switch (USART_RxDataGet(UART0)) {
        case 'd':
        case 67: // right arrow
            snk.newDir = right;
            break;
        case 'a':
        case 68: // left arrow
            snk.newDir = left;
            break;
        case 'w':
        case 65: // up arrow
            snk.newDir = up;
            break;
        case 's':
        case 66: // down arrow
            snk.newDir = down;
            break;
        case 'j':
            snk.newDir = snk.dir == left ? up : snk.dir + 1;
            break;
        case 'b':
            snk.newDir = snk.dir == up ? left : snk.dir - 1;
            break;
        case 'r':
            reset    = true;
            gameOver = false;
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

    USART_IntEnable(UART0, UART_IF_RXDATAV); // enable UART interrupt
    NVIC_EnableIRQ(UART0_RX_IRQn);
}

__STATIC_INLINE void initGame()
{
    reset    = false;
    gameOver = false;
    setDecimalPoints(false);
    initSnake(&snk);
    generateFood(&snk, &fd, &msTicks);
}

int main()
{
    UART_Init();
    // Setup SysTick Timer for 1 msec interrupts
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1);
    }
    SegmentLCD_Init(false); // Enable LCD without voltage boost

    while (1) {
        if (reset) {
            initGame();
        }

        if (snk.lenChanged) {
            SegmentLCD_Number(snk.len - 1);
            snk.lenChanged = false;
        }

        if (isEating(&snk, &fd) && !generateFood(&snk, &fd, &msTicks)) {
            gameOver = true;
        }
        if (checkCollision(&snk)) {
            gameOver = true;
        }

        clearMap(&mp);
        drawSnake(&mp, &snk);
        drawFood(&mp, &fd);
        displayMap(&mp);

        Delay(TICK);
        if (gameOver) {
            clearMap(&mp);
            displayMap(&mp);
            while (gameOver) {
                toggleDecimalPoints();
                Delay(500);
            }
        }

        setDir(&snk);
        stepSnake(&snk);
    }
}

