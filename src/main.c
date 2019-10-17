/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
#define getbit(n,b)((n&(1<<b))>>b)

USART_TypeDef *u1 = USART1;
GPIO_TypeDef  *ga = GPIOA;

int i;
uint8_t t;
uint8_t a[] = "enter a for flash\r\nenter b for normal start \r\n";
uint8_t b[] = "u'r in flash mode\r\n";
uint8_t c[] = "well come to Linux\r\n";


void flash()
{
	trace_printf("successfully flashed\r\n");
	while(b[i])
	{
		if(getbit(u1->SR,6) == 1)
		{
			u1->DR  |= b[i++];
			u1->SR  &= ~(USART_SR_TC);
		}
	}
	i = 0;
}
void linux()
{
	trace_printf("well come to linux\r\n");
	while(c[i])
	{
		if(getbit(u1->SR,6) == 1)
		{
			u1->DR  |= c[i++];
			u1->SR  &= ~(USART_SR_TC);
		}
	}
	i = 0;
}

int
main(int argc, char* argv[])
{

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	ga->MODER   |= GPIO_MODER_MODER9_1|GPIO_MODER_MODER10_1;
	ga->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_1|GPIO_OSPEEDER_OSPEEDR10_1;
	ga->PUPDR   |= GPIO_PUPDR_PUPDR9_0|GPIO_PUPDR_PUPDR10_0;
	ga->AFR[1]  |= (1<<4)|(1<<5)|(1<<6)|(1<<8)|(1<<9)|(1<<10);


	u1->CR1  |= USART_CR1_RE|USART_CR1_TE|USART_CR1_UE|USART_CR1_RXNEIE;
	u1->BRR   = 84000000/115200;

	while(a[i])
	{
		if(getbit(u1->SR,6) == 1)
		{
			u1->DR  |= a[i++];
			u1->SR  &= ~(USART_SR_TC);
		}
	}
	i = 0;
	//HAL_NVIC_EnableIRQ( USART1_IRQn);

	while(1)
	{
		if(getbit(u1->SR,5) == 1)
		{
			trace_printf("%c\n",u1->DR);
			t = u1->DR;
			u1->SR &= ~(USART_SR_RXNE);
		}
		if(t == 'a')
		{
			flash();
			t = 0;
		}
		if(t == 'b')
		{
			linux();
			t = 0;
		}
	}

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
