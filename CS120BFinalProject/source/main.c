/*	Author: sliu153
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "scheduler.h"
#include <avr/eeprom.h>	
#include "keypad.h"
#include "user.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define F_CPU 8000000UL		/* Define CPU Frequency e.g. here its 8MHz */
#include <util/delay.h>		/* Include inbuilt defined Delay header file */
#include <avr/interrupt.h>


#define LCD_Data_Dir DDRC	/* Define LCD data port direction */
#define LCD_Command_Dir DDRD	/* Define LCD command port direction register */
#define LCD_Data_Port PORTC	/* Define LCD data port */
#define LCD_Command_Port PORTD	/* Define LCD data port */
#define RS PD6			/* Define Register Select signal pin */
#define RW PD5			/* Define Read/Write signal pin */
#define EN PD7			/* Define Enable signal pin */


void LCD_Command(unsigned char cmnd)
{
	LCD_Data_Port= cmnd;
	LCD_Command_Port &= ~(1<<RS);	/* RS=0 command reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 Write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(2);
}

void LCD_Char (unsigned char char_data)  /* LCD data write function */
{
	LCD_Data_Port= char_data;
	LCD_Command_Port |= (1<<RS);	/* RS=1 Data reg. */
	LCD_Command_Port &= ~(1<<RW);	/* RW=0 write operation */
	LCD_Command_Port |= (1<<EN);	/* Enable Pulse */
	_delay_us(1);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(2);			/* Data write delay */
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF;		/* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;		/* Make LCD data port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command (0x38);		/* Initialization of 16X2 LCD in 8bit mode */
	LCD_Command (0x0C);		/* Display ON Cursor OFF */
	LCD_Command (0x06);		/* Auto Increment cursor */
	LCD_Command (0x01);		/* clear display */
	_delay_ms(2);			/* Clear display command delay> 1.63 ms */
	LCD_Command (0x80);		/* Cursor at home position */
}


void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)  /* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* clear display */
	LCD_Command (0x80);		/* cursor at home position */
}


void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	unsigned char i;
	if(loc<8)
	{
		LCD_Command (0x40 + (loc*8));	/* Command 0x40 and onwards forces the device to point CGRAM address */
		for(i=0;i<8;i++)	/* Write 8 byte for generation of 1 character */
		LCD_Char(msg[i]);
	}
}
void LCD_Cursor(unsigned char column) {
	if ( column < 17 ) { // 16x1 LCD: column < 9
		// 16x2 LCD: column < 17
		LCD_Command(0x80 + column - 1);
		} else {
		LCD_Command(0xB8 + column - 9);	// 16x1 LCD: column - 1
		// 16x2 LCD: column - 9
	}
}
//****************************************
	unsigned char level = 2;
	unsigned char dowm[8] = { 0x4,0xa,0x11,0x11,0x11,0x11,0xa,0x4}; 
	unsigned char up[8] = {0x4,0xa,0x4,0x15,0xe,0x4,0xa,0x11};
	unsigned char jump[8] = {0x4,0xa,0x4,0x4,0x1f,0x4,0x1b,0x0};
	unsigned char x=17;
	unsigned char lose=0;
	unsigned char direbut;
	unsigned char direbut_temp;
	unsigned char hold_up=1;
enum direction_state{direinit,direup,diredowm,direleft,direright}direstate;
void snake_dire(){
	
	switch(direstate){
	case direinit:
		LCD_Custom_Char(0,dowm);
		direstate = direright;
		break;
	case direright:
		if(direbut == 0x01){
			direstate = direup;
		}
		else if(direbut == 0x02){
			direstate = diredowm;
		}
		else{
			direstate = direright;
			x++;
		}
		break;
	case direleft:
	if(direbut == 0x01){
		direstate = direup;
	}
	else if(direbut == 0x02){
		direstate = diredowm;
	}
	else{
		direstate = direleft;
		x--;
	}
	break;
	case direup:
		if(direbut == 0x04){
			direstate = direright;
		}
		else if(direbut == 0x08){
			direstate = direleft;
		}
		else{
			direstate = direup;}
		break;		
	case diredowm:
	if(direbut == 0x04){
		direstate = direright;
	}
	else if(direbut == 0x08){
		direstate = direleft;
	}
	else{
	direstate = diredowm;}
	break;
	}
	
	
	switch(direstate){
		case direinit:
			break;
		case direright:
			LCD_Cursor(x);
			LCD_Char(0);
			break;
		case direleft:
			LCD_Cursor(x);
			LCD_Char(0);
			break;
		case direup:
			if(hold_up==1){				
				LCD_Custom_Char(0, up);
				hold_up =2;
			}
			else if(hold_up == 2){
				x = x-16;
				level = 1;
				LCD_Custom_Char(0, dowm);
				hold_up = 1;
			}
			LCD_Cursor(x);
			LCD_Char(0);
			break;
		case diredowm:
		if(hold_up==2){
			LCD_Custom_Char(0, dowm);
			hold_up=1;
		}
		else if(hold_up == 1){
			x = x+16;
			level = 2;
			LCD_Custom_Char(0, up);
			hold_up = 2;
		}
		LCD_Cursor(x);
		LCD_Char(0);
		break;
	}
}

void check_border(){
	if(x<0 || x>32){
		lose = 1;
	}
	else if(level == 1 && x>16){
		lose =1;
	}
	else if(level == 2 && x<17){
		lose = 1;
	}
	if(lose == 1){
		LCD_Clear();
		LCD_Command(0x80);
		LCD_String("you lose");
	}
}
unsigned char geneposition=0;
unsigned char score;
unsigned check_hit;
void gene_score(){
	unsigned char gene[4]={0x03,0x13,0x0A,0x19};
	
	if(geneposition<4){LCD_Cursor(gene[geneposition]);LCD_Char(1);}
	       
	            //把预定的点打出来
	if(gene[geneposition]==x && hold_up == 2){
		score ++;
		geneposition++;
	}							//判断分数	
}

//***************************
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0 );
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}


void PWM_init() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}
unsigned short adc_x;
 
unsigned char showingpo[5]={1,2,17,25,35};
unsigned char showingpo_temp[5]={1,2,17,25,35};
unsigned char trackingpo[5]={16,16,16,16,16};
unsigned char trackingpo_temp[5]={16,16,16,16,16};
unsigned char number0fblocks=0;
unsigned char showuptime=1;
unsigned char firstblock=1;
//************
unsigned char showingpo_do[5]={5,11,20,22,35};
unsigned char showingpo_temp_do[5]={5,11,20,22,35};
unsigned char trackingpo_do[5]={32,32,32,32,32};
unsigned char trackingpo_temp_do[5]={32,32,32,32,32};
unsigned char number0fblocks_do=0;
unsigned char firstblock_do=1;
unsigned char press;
//**************
unsigned char i;
unsigned char input_hold='D';
unsigned char up_dowm = 2;
unsigned char player_po = 17;
unsigned char score_time;
unsigned char control_flash =0 ;
unsigned char ten;
unsigned char one;
unsigned char higestten;
unsigned char higestone;
//**************music 
unsigned char music_i;
unsigned char music_change;
unsigned char song1[8]={'A','H','B','G','C','F','D','E'};
unsigned char song2[8]="CGEBHECB";
unsigned char song[8];
const double notes[26] = {329.63, 329.63, 329.63, 329.63, 329.63, 329.63, 329.63, 392.00, 261.63, 293.66, 329.63, 349.23, 349.23, 349.23, 349.23, 349.23, 329.63, 329.63, 329.63, 329.63, 329.63, 293.66, 293.66, 329.63, 293.66, 392.00 };
void music_transfer(){
	if(song[music_i] == 'A'){set_PWM(4440.0);}
	else if(song[music_i] == 'B'){set_PWM(2616.3);}
	else if(song[music_i] == 'C'){set_PWM(5232.5);}
	else if(song[music_i] == 'D'){set_PWM(2936.6);}
	else if(song[music_i] == 'E'){set_PWM(3296.3);}
	else if(song[music_i] == 'F'){set_PWM(3492.3);}
	else if(song[music_i] == 'G'){set_PWM(3920.0);}
	else if(song[music_i] == 'H'){set_PWM(5232.5);}
	
}
enum music_state{music_menu,music_play,music_winlose};
int music_tick(int state){
	switch(state){
		case music_menu:
			if(input_hold == 'A'){
				for(music_change=0;music_change<8;music_change++){song[music_change]=song1[music_change];}
				state = music_play;
				music_i = 0;
			}
			break;
		case music_play:
			if(input_hold == 'D'){
				state = music_menu;
				music_i = 0;
			}
			else if(input_hold == 's'){
				for(music_change=0;music_change<8;music_change++){song[music_change]=song2[music_change];}
				state = music_winlose;
				music_i = 0;
			}
			break;
		case music_winlose:
			if(input_hold == 'D'){
				state = music_menu;
				music_i = 0;
			}
			break;
	}
	switch(state){
		case music_menu:
			set_PWM(notes[music_i]*10);
			music_i++;
			if(music_i==26){music_i = 0;}
			
			break;
		case music_play:
			/*music_transfer();
			music_i++;
			if(music_i==8){music_i = 0;}*/
				set_PWM(0);
				if(adc_x >= 950 && input_hold == 'A'){set_PWM(329.63);}
				else if(adc_x <= 50 && input_hold == 'A'){set_PWM(5232.5);}
			break;
		case music_winlose:
			music_transfer();
			music_i++;
			if(music_i==8){music_i = 0;}
			break;
	}
	return state;
}	
//**************
unsigned char highestscore;
enum score_state{scoreinit};
int score_tick(int state){	
	eeprom_write_byte((uint8_t*)1,showuptime);
	score_time = eeprom_read_byte((uint8_t*)1);
	
	highestscore = eeprom_read_byte((uint8_t*)2);
	if(score_time > highestscore){
		eeprom_write_byte((uint8_t*)2,showuptime);
	}
	highestscore = eeprom_read_byte((uint8_t*)2);
	switch (state){
		
		case scoreinit:
		ten = score_time /10;
		one = score_time %10;
		higestten =  highestscore/10;
		higestone = highestscore %10;
	if((player_po == trackingpo[firstblock-1] || player_po == trackingpo_do[firstblock_do-1])&&control_flash == 0){
		input_hold = 's';
		LCD_Clear();
		LCD_Command(0x80);
		LCD_String("lose  highest:");
		LCD_Char(higestten+'0');
		LCD_Char(higestone+'0');
		LCD_Cursor(17);
		LCD_String("score: ");
		control_flash = 1;
		LCD_Char(ten+'0');
		LCD_Char(one+'0');
	}
	if(score_time>48 && control_flash == 0){
		input_hold = 's';
		LCD_Clear();
		LCD_Command(0x80);
		LCD_String("win  highest:");
		LCD_Char(higestten+'0');
		LCD_Char(higestone+'0');
		LCD_Cursor(17);
		LCD_String("score: ");
		control_flash = 1;
		LCD_Char(ten+'0');
		LCD_Char(one+'0');
	}
	state = scoreinit;
	break;}
	return state;
}
enum player_state{playerstart};
int player_tick(int state){
	switch(state){
		case playerstart:
			if(input_hold == 'A'){
				if(up_dowm == 2 && player_po>16){player_po = player_po - 16;up_dowm = 3;}
				if(up_dowm == 1 && player_po<17){player_po = player_po +16;up_dowm = 3;}
				
			LCD_Cursor(player_po);
			if(showuptime%2 == 1){LCD_Char(2);}
			else{LCD_Char(1);}}
			state = playerstart;
			break;
	}
	return state;
}
enum getinput_state{getinit,getpress};
int getinput_tick(int state){
	press = ~PINA & 0x06;
	switch(state){
		case getinit:
			if(press){
				state =getpress;
				if(press == 2 && input_hold != 's'){input_hold = 'A';}
				else if(press == 4){input_hold = 'D';}
				
			}
			adc_x= ADC;
			if(adc_x >= 950){up_dowm = 2;}
			if(adc_x <= 50){up_dowm = 1;}
			else{state = getinit;}break;
		case getpress:
			if(press){state = getpress;}
			else{state = getinit;}
			break;
	}
	return state;
}
enum menuNreset{menu,start,lose_win};
int menu_tick(int state){
	switch(state){
		case menu:
			if(input_hold == 's'){state = lose_win;}
			else if(input_hold == 'A'){
				state = start;
			}
			else{
				state = menu;
			}
			
			break;
		case start:
			if(input_hold == 's'){state = lose_win;}
			else if(input_hold == 'D'){
				state = menu;
				LCD_Clear();
			}
			else{
				state = start;
			}break;
		case lose_win:
			if(input_hold == 's'){state = lose_win;}
			else if(input_hold == 'D'){
				state = menu;
				LCD_Clear();
			}
			else{
				state = start;
			}break;
			break;
	}
	unsigned char z=0; 
	switch(state){
		case menu:
			
//*************************************reset
			
			for(z=0;z<5;z++){showingpo[z]=showingpo_temp[z];}
			for(z=0;z<5;z++){trackingpo[z]=trackingpo_temp[z];}
//*********do
			for(z=0;z<5;z++){showingpo_do[z]=showingpo_temp_do[z];}
			for(z=0;z<5;z++){trackingpo_do[z]=trackingpo_temp_do[z];}
//******
			 number0fblocks=0;
			 number0fblocks_do=0;
			 showuptime=1;
			 firstblock=1;
			firstblock_do=1;
			control_flash = 0;
//************************************reset everything
			 LCD_Command(0x80);
			LCD_String("press A to begin ");
			
			break;
		case start:
			break;
		case lose_win:
			break;
	}
	return state;
}

enum screen_roll{init};
int display1(int state){
	switch(state){
		case init:
		if(input_hold == 'A'){
			if(showuptime == showingpo[number0fblocks]){
				number0fblocks++;
			}
			
			LCD_Clear();
			i=firstblock;
			for(i=firstblock;i<=number0fblocks;i++){
				LCD_Cursor(trackingpo[i-1]);
				LCD_Char(0);
				trackingpo[i-1]=trackingpo[i-1]-1;
				if(trackingpo[i-1]==0){firstblock++;}
			}
		//**********
			if(showuptime == showingpo_do[number0fblocks_do]){
				number0fblocks_do++;
			}
			showuptime++;
			i=firstblock_do;
			for(i=firstblock_do;i<=number0fblocks_do;i++){
				LCD_Cursor(trackingpo_do[i-1]);
				LCD_Char(0);
				trackingpo_do[i-1]=trackingpo_do[i-1]-1;
				if(trackingpo_do[i-1]==16){firstblock_do++;}
			}
		//**********
		}
			state = init;		
			break;
	}
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	ADC_init();
	LCD_Init();
	PWM_init();
	PWM_on();
	LCD_Custom_Char(0,dowm);
	LCD_Custom_Char(1,up);
	LCD_Custom_Char(2,jump);
	if(eeprom_read_byte((uint8_t*)2)<1 || eeprom_read_byte((uint8_t*)2)>70){eeprom_write_byte((uint8_t*)2,1);}
	
	static task task1,task2,task3,task4,task5,task6;
	task *tasks[] = {&task1,&task2,&task3,&task4,&task5,&task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
		
	task1.state = init;//Task initial state
	task1.period = 70;//Task Period
	task1.elapsedTime = task1.period;//Task current elapsed time.
	task1.TickFct = &display1;//Function pointer for the tick

	task2.state = getinit;//Task initial state
	task2.period = 1;//Task Period
	task2.elapsedTime = task2.period;//Task current elapsed time.
	task2.TickFct = &getinput_tick;//Function pointer for the tick

	task3.state = menu;//Task initial state
	task3.period = 70;//Task Period
	task3.elapsedTime = task3.period;//Task current elapsed time.
	task3.TickFct = &menu_tick;//Function pointer for the tick

	task4.state = playerstart;//Task initial state
	task4.period = 70;//Task Period
	task4.elapsedTime = task4.period;//Task current elapsed time.
	task4.TickFct = &player_tick;//Function pointer for the tick	
	
	task5.state = scoreinit;//Task initial state
	task5.period = 20;//Task Period
	task5.elapsedTime = task5.period;//Task current elapsed time.
	task5.TickFct = &score_tick;//Function pointer for the tick

	task6.state = music_menu;//Task initial state
	task6.period = 30;//Task Period
	task6.elapsedTime = task6.period;//Task current elapsed time.
	task6.TickFct = &music_tick;//Function pointer for the tick	
	
	TimerSet(1);
	TimerOn();

	unsigned short i;//Scheduler for-loop iterator
	while (1) {
		unsigned char reset_eeprom;
		reset_eeprom = ~PINA & 0x08;
		if(reset_eeprom ){eeprom_write_byte((uint8_t*)2,1);}
		ADC_init();
		
		for(i = 0; i < numTasks; i++){//Scheduler code
			if(tasks[i]->elapsedTime == tasks[i]->period){//Task is ready to tick
				tasks[i]->state= tasks[i]->TickFct(tasks[i]->state);//set next state
				tasks[i]->elapsedTime = 0;//Reset the elapsed time for next tick;
			}
			tasks[i]->elapsedTime += 1;
			
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}

