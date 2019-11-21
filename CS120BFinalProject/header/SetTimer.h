#ifndef SETTIMER_H
#define SETTIMER_H

#include <avr/interrupt.h>

//Main() set it to 0; TimerISR() sets this to 1.
volatile unsigned char TimerFlag = 0; 

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
// Start count from here, down to 0. Default 1ms
unsigned long avrTimerM = 1; 
// Current internal count of 1ms ticks
unsigned long avrTimerCurrCnt = 0; 

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	avrTimerM = M;
	avrTimerCurrCnt = avrTimerM;
}

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	// bit3 = 1: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: prescaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s
	TCCR1B 	= 0x0B;	
	// AVR output compare register OCR1A.
	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	OCR1A 	= 125;
	//bit1: OCIE1A -- enables compare match interrupt
	TIMSK1 	= 0x02; 

	//Initialize avr counter
	TCNT1 = 0;

	// TimerISR will be called every avrTimerCurrCnt milliseconds
	avrTimerCurrCnt = avrTimerM;

	//Enable global interrupts
	SREG |= 0x80;
}

void TimerOff() {
	// bit3bit2bit1bit0=0000: timer off
	TCCR1B 	= 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	// CPU automatically calls when TCNT0 == OCR0 (every 1 ms per TimerOn settings)
	// Count down to 0 rather than up to TOP
	avrTimerCurrCnt--; 		
	// results in a more efficient compare
	if (avrTimerCurrCnt == 0) { 	
		// Call the ISR that the user uses
		TimerISR(); 				
		avrTimerCurrCnt = avrTimerM;
	}
}

#endif
