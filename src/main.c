#include <stdint.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_lcd.h"
#include "segmentlcd.h"
#include "display.h"

#define SLEEP 200

typedef enum _direction { up, down, left, right } direction;

volatile uint32_t  msTicks; /* counts 1ms timeTicks */
volatile direction dir      = right;
volatile direction prev_dir = right;

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
    prev_dir = dir;
    switch (USART_RxDataGet(UART0)) {
        case 'd':
        case 'l':
        case 67: // right arrow
            dir = right;
            break;
        case 'a':
        case 'h':
        case 68: // left arrow
            dir = left;
            break;
        case 'w':
        case 'k':
        case 65: // up arrow
            dir = up;
            break;
        case 's':
        case 'j':
        case 66: // down arrow
            dir = down;
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
        while (1)
            ;
    }
    /* Enable LCD without voltage boost */
    SegmentLCD_Init(false);
    //map map={{0}};
    map map = {{{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
               ,{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
               ,{0,0,1,1,1,1,1,1,1,1,1,1,1,1,1}
               ,{0,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
               ,{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1}}};
    
    /*drawLine(&map,(pixel){0,0},(pixel){0,2},1);
    drawLine(&map,(pixel){2,2},(pixel){2,4},1);
    drawLine(&map,(pixel){0,2},(pixel){2,2},1);
    drawLine(&map,(pixel){0,0},(pixel){2,0},1);
    drawLine(&map,(pixel){0,4},(pixel){2,4},1);

    drawLine(&map,(pixel){4,0},(pixel){4,4},1);
    drawLine(&map,(pixel){4,0},(pixel){6,0},1);
    drawLine(&map,(pixel){4,2},(pixel){6,2},1);
    drawLine(&map,(pixel){4,4},(pixel){6,4},1);

    drawLine(&map,(pixel){6,0},(pixel){6,4},1);
    drawLine(&map,(pixel){8,0},(pixel){8,4},1);
    drawLine(&map,(pixel){6,2},(pixel){8,2},1);

    drawLine(&map,(pixel){10,0},(pixel){10,2},1);
    drawLine(&map,(pixel){12,0},(pixel){12,4},1);
    drawLine(&map,(pixel){10,2},(pixel){12,2},1);*/
    
    displayMap(&map);
    // test
    int num = 0;
    while (1) {
        SegmentLCD_Number(num);
        num++;
        Delay(500);
    }
}

