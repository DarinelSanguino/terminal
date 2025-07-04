#include "Time.h"
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

volatile uint64_t micros;

void Time_setup(void)
{
	rcc_periph_clock_enable(RCC_TIM2);

	nvic_enable_irq(NVIC_TIM2_IRQ);
	nvic_set_priority(NVIC_TIM2_IRQ, 1);
	rcc_periph_reset_pulse(RST_TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM2, CLK_FREQ_MHZ - 1);

	timer_disable_preload(TIM2);
	timer_continuous_mode(TIM2);
	timer_set_period(TIM2, UINT32_MAX);

	timer_generate_event(TIM2, TIM_EGR_UG);
	timer_enable_irq(TIM2, TIM_DIER_UIE);
	timer_enable_counter(TIM2);
	micros = 0;
}

uint64_t Time_get_counter(void)
{
	return micros + timer_get_counter(TIM2);
}

void Time_reset_counter(void)
{
	timer_set_counter(TIM2, 0);
}

void tim2_isr(void) {
	timer_clear_flag(TIM2, TIM_SR_UIF);
	micros += UINT32_MAX;	
}