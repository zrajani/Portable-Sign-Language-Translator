//Author: Shawn D'Souza <shawnelec@gmail.com>
//Compiler: GCC
//Target: AVR ATmega microcontroller series
//Description:
//->specifically designed for the Smart Glove Project
//->function library for some I/O
//->contains interface for the keypad, ADC and eeprom read/write //  and battery level indicator 


#include<avr/eeprom.h>

#define keypad_DDR  DDRD
#define keypad_PIN  PIND
#define keypad_PORT PORTD


void init_adc()                 //initialize the ADC of the microcontroller
                                //call this function first at the beginning
{
ADMUX=(1<<REFS0);              //Aref joined to AVcc
ADCSRA = _BV(ADEN) | _BV(ADPS0);
}

void disab_adc()
{
ADCSRA=0x00;
ADMUX=0x00;
}

int read_adc(uint8_t ch)
{
   // give the channel number when calling the function
   // this function will return an 8 bit char/uint8_t (0-255) which can be directly
   // stored in the EEPROM  

   //Select ADC Channel ch must be 0-7 (corresponds to the flex sensors...)
   //flex sensors from channel 0-4
   //acccelerometer from channel 5-7
   
   ch=ch&0b00000111;
   ADMUX|=ch;

   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   ADCSRA|=(1<<ADIF);

   return(ADC);
}

//IMPORTANT: please start the storage table from address 50 onwards
//I'll be using the initial 0-49 for flags and other data
//26 characters x 5 values = 130 address spaces used in total

void read_batt()
{

float vout = 0.000;
int value = 0;
float R1 = 100000.0;    // !! resistance of R1 !!
float R2 = 4700.0;     // !! resistance of R2 !!
float vin = 0.0;

init_adc();
value = read_adc(7);
vout= (value * 5.0)/1024.0;
vin = vout / (R2/(R1+R2)); //voltage to be displayed

if(vin < 9.3){glcd_putchar(1,112,0);}
if(vin > 9.9){glcd_putchar(2,112,0);}
if(vin > 10.3){glcd_putchar(3,112,0);}
if(vin > 11.1){glcd_putchar(4,112,0);}
if(vin > 12.4){glcd_putchar(5,112,0);}
if(vin > 13.0){glcd_putchar(6,112,0);}

disab_adc();
}

uint8_t e_read(int address)
{   
       //returns value(0-255) from the address given
       //address value from 0-1023 for ATmega 32

	return eeprom_read_byte((unsigned char *) address);
}

void e_write(int address, uint8_t value)  //value from 0-255
{   
       //writes a value(0-255) to the given address
       //address value from 0-1023 for ATmega 32
 
	eeprom_write_byte((unsigned char *) address, value);
}


char key_read()
{ 
  //reads the value of the key pressed and returns the same
  //if no key is pressed, it returns '/'
  //constant polling is required to read the keys
  //use a character variable to store the key value returned by the function

uint8_t row=0,cntr,col=0;

keypad_DDR=0x00;
keypad_PORT=0x0F;

for(cntr=4;cntr<8;cntr++)
{
keypad_DDR=(1 << cntr);
row=keypad_PIN;

if(row==0b00001110) {row=1;break;}
else if(row==0b00001101) {row=2;break;}
else if(row==0b00001011) {row=3;break;}
else if(row==0b00000111) {row=4;break;}
++col;
}

switch(row)
{
case 1: if(col==1){return'*';}
        if(col==2){return'0';}
        if(col==3){return'#';}
	    if(col==0){return'D';}
	    break;

case 2: if(col==1){return'7';}
        if(col==2){return'8';}
	    if(col==3){return'9';}
	    if(col==0){return'C';}
	    break;

case 3: if(col==1){return'4';}
        if(col==2){return'5';}
	    if(col==3){return'6';}
	    if(col==0){return'B';}
	    break;

case 4: if(col==1){return'1';}
        if(col==2){return'2';}
	    if(col==3){return'3';}
	    if(col==0){return'A';}
	    break;

default: return '/';
         break;

}
}
