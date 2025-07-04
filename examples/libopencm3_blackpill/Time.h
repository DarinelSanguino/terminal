#ifndef _TIMEXXX_H_
#define _TIMEXXX_H_

#include <stdint.h>

#ifndef	CLK_FREQ_MHZ
#define	CLK_FREQ_MHZ	25U
#endif

void Time_setup(void);
uint64_t Time_get_counter(void);
void Time_reset_counter(void);

#endif