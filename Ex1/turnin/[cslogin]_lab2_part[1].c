/*	Author: sliu153
 *  Partner(s) Name: Shi Liu
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	 unsigned char tempB = 0x00; 
	 unsigned char tempA0 = 0x00;
	 unsigned char tempA1 = 0x00;

	while(1) {
		tempA0 = PINA & 0x01;
		tempA1 = PINA & 0x02;
		if(tempA0 & !tempA1){
			tempB = tempB | 0x01;
		}
		else{
			tempB = tempB & 0x00;
		}
		PORTB = tempB;
	}
	return 1;
}

    /* Insert your solution below */
    
