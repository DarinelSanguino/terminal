/*
MIT License

Copyright (c) 2021 Marcin Borowicz <marcinbor85@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "Time.h"
#include "terminal_config.h"
#include "terminal.h"

#include "my_test_commands/my_test_commands.h"

#define GPIO_TX1				GPIO9
#define GPIO_RX1				GPIO10
#define GPIO_LED				GPIO13

#define USART_BITS  			8
#define USART_SPEED 			9600

#define IRQ_PRIORITY_USART1		1

#define TERMINAL_USART			USART1

char dbgbuffer[256];

void Gpio_setup(void);
void Uart_setup(void);
void _InitHW(void);
void _InitSW(void);

int TUSART_GetChar(char *c);

inline void TUSART_PutChar(char c)
{
	usart_send_blocking(TERMINAL_USART, c);
}

void TUSART_Print(const char* str)
{
	while(str != NULL) {
		TUSART_PutChar(*str);

		if (*str == '\0')
			break;

		str++;
	}
}

int TUSART_GetChar(char *c)
{	
	if(usart_get_flag(TERMINAL_USART, USART_SR_RXNE))
	{
		*c = usart_recv(TERMINAL_USART);
	    return 1;
	}
	return 0;	
}

void _reset_fcn(void)
{
	CLI_Printf("System reset\n");
	scb_reset_system();
}

void Gpio_setup(void) 
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_TX1);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO_TX1);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_RX1);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO_RX1);

}

void Uart_setup(void)
{
	nvic_enable_irq(NVIC_USART1_IRQ);	
	nvic_set_priority(NVIC_USART1_IRQ, IRQ_PRIORITY_USART1);

	rcc_periph_clock_enable(RCC_USART1);
	usart_set_databits(TERMINAL_USART, USART_BITS);
	usart_set_baudrate(TERMINAL_USART, USART_SPEED);
	usart_set_stopbits(TERMINAL_USART, USART_STOPBITS_1);
	usart_set_parity(TERMINAL_USART, USART_PARITY_NONE);
	usart_set_flow_control(TERMINAL_USART, USART_FLOWCONTROL_NONE);
	usart_set_mode(TERMINAL_USART, USART_MODE_TX_RX);
	usart_enable_rx_interrupt(TERMINAL_USART);
	usart_enable(TERMINAL_USART);
}

void _InitHW(void)
{
	rcc_osc_on(RCC_HSE);
	while(!rcc_is_osc_ready(RCC_HSE));
	rcc_set_sysclk_source(RCC_CFGR_SW_HSE);
	rcc_wait_for_sysclk_status(RCC_HSE);

	Time_setup();
	Gpio_setup();
	Uart_setup();
}

void _InitSW(void)
{
	CLI_Init(TDC_All);
	MyTestCmds_Init();
}

int main(void) {	
	_InitHW();
	_InitSW();

    while(1)
    {
    	CLI_Execute();
    }
	return 0;
}

void usart1_isr(void) {
	char c;
	if(TUSART_GetChar(&c)) 
	{
		CLI_EnterChar(c);
	}
}