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

volatile uint32_t msTicks; /* counts 1ms timeTicks */
volatile bool     reset = false;
snake             snk;

void SysTick_Handler(void)
{
    msTicks++; /* increment counter necessary in Delay()*/
}

void Delay(uint32_t dlyTicks)
{
    uint32_t curTicks;

    curTicks = msTicks;
    while ((msTicks - curTicks) < dlyTicks)
        ;
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
            reset = true;
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
    map map = {{{0}}};
    initSnake(&snk);
    UART_Init();
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        while (1)
            ;
    }
    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);

    while (1) {
        drawSnake(&map, &snk);
        SegmentLCD_Number(snk.len - 1);
        Delay(500);
        if(checkCollision(&snk)) {
            reset = 1;
        }
        if (reset) {
            reset = false;
            initSnake(&snk);
            clearMap(&map);
        }
        stepSnake(&snk);
    }
}

