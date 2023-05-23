// Includes and definitions

#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

// Function declaration
int init_LEDs(void);
int set_LED(int position, int value);

int AB=3;
uint8_t duty;

int main(void) {
	
	sei();    // set global interrupt flag -> enable ALL interrupts (but those that are masked won't do anything)
	//set_LED(1,1);
	setup_INTs();
	setup_PWM();
	//set_LED(2,1);
	duty=100;
	update_PWM();
	while(1) {
	
	_delay_ms(200);

		}
	
	return 0;
}

	
int setup_PWM(void) {
	
	DDRD |= ((1<<DDD5)|(1<<DDD6));	//Set PIND5 and PIND6 as outputs
	TCCR0A |= 0b11100011;			//Configure fast PWM mode, non-inverted output on OCB 4 and inverted output on OCA 3 amplifier conected to oca3
	TCCR0B |= 0x01;					//Internal clock selector, no prescaler
	return 1;
}


int setup_INTs(void) {

	PCMSK0 = (1<<PCINT1)|(1<<PCINT2); //PB1 and PB2
	PCICR = (1 << PCIE0);
	return 1;
}


int encoder_read(void) {
    
	//encoder read: function that reads wether the encoder is moving one way or the other: 1 or -1
	int res; //result, 1 or -1 depending on direction
	int An; //Channel A new value fo the encoder
	int Bn; //Channel B new value of the encoder
	int AB_new;
	
	//Read pins and assign values to vbles A2 and B2 (new value after interruption)
	//Chanel A connected to PD2 and chanel B connected to PD3
	An = PINB & (1<<PINB1);
	Bn = PINB & (1<<PINB2);
	
	AB_new=(An|Bn)>>PINB1;
	
	//Make the computations to guess the direction in which the encoder is moving
	switch (AB){
		case 3 : /*AB new up*/  if (AB_new == 2) res=1; /*B moves*/ else  res = 2; break; /* A moves */		
		case 2 : /*A up B down*/if (AB_new == 0) res=1; /*B moves*/ else  res = 2; break; /* A moves */
		case 1 : /*A down B up*/if (AB_new == 3) res=1; /*B moves*/ else  res = 2; break; /* A moves */
		case 0 : /*AB new down*/if (AB_new == 1) res=1; /*B moves*/ else  res = 2; break; /* A moves */
	}
	
	//Set led 1 on if res=1 and led 2 on if res=2, or led 3 on in other case (for debuging)
	
	//Update global variables with new encoder value
	AB = AB_new;
	
	return res;
}

ISR(PCINT0_vect)
{

	cli(); 
	int dir = encoder_read();
	if (dir == 1 && duty < 255 ) {
    duty++;
    }
	else if (dir == 2 && duty > 0) {
    duty--;
    }
	update_PWM();
	sei();
}

int update_PWM(void)
{
	/* Making sure the duty is within the range */
	if (duty < 0) {
         OCR0A = 0;
    }
	else if (duty > 255) {
        OCR0A = 255;
    } else {
        OCR0A = duty;
    }
	return 1;
}