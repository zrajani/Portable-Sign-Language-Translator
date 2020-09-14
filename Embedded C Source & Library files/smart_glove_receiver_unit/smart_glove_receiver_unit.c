
#include<stdio.h>
#include<avr/io.h>
#include<compat/deprecated.h>

#define F_CPU 16000000UL

#include<util/delay.h>
#include<glcd.h>
#include<cc2500lib.h>
#include<func.h>
#include<stdlib.h>
#include<inttypes.h>
#include<uart.h>
#include<avr/interrupt.h>
#include<avr/pgmspace.h>
#include<math.h>


#define UART_BAUD_RATE      9600     

#define max_char 112
#define ani_delay  150

char disp[max_char],c;
uint8_t cmd=0,char_cnt=0;
uint8_t inp=0;

int x,y,tempx,tempy;

uint8_t tog[6]={0,0,0,0,0,0},rc,lc;

uint8_t x_cnt=1,y_cnt=1,b=0;

//#define screen_xmax    1366
//#define screen_ymax    768


void welcome();
void menu();
void submenu();
void mouse_menu();
void dev_status();

void init_disp();
void check_char();
//void y_move(uint16_t,uint8_t);
//void x_move(uint16_t,uint8_t);
void x_movep(uint16_t);
void x_moven(uint16_t);
void y_movep(uint16_t);
void y_moven(uint16_t);
void chk_click();
void user_ch();
void refresh_robo_screen();


uint8_t alp_cnt;

int main()
{


DDRC=0xFF;
DDRD=0xF8;
DDRA=0b01111111;

uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
sei();

glcd_on();
glcd_clear();
welcome();

init_disp();
cc2500_init();
_delay_ms(100);
wake();



while(1)
{
while(data_rx()==0);
cmd=rxBuffer[0];

if(cmd==0x0C) //character display
{
glcd_clear();
read_batt();
glcd_puts("CHARACTER  ",0,0);
glcd_puts(disp,0,1);
disp[char_cnt]=rxBuffer[1];
c=rxBuffer[1];
uart_putc(c);
char_cnt++;
check_char();
}

if(cmd==0x1C) //number display
{
glcd_clear();
read_batt();
glcd_puts("NUMBER     ",0,0);
glcd_puts(disp,0,1);
if(rxBuffer[1]=='*'){disp[char_cnt]='.';c='.';}
else{disp[char_cnt]=rxBuffer[1];c=rxBuffer[1];}
uart_putc(c);
char_cnt++;
check_char();
}

if(cmd==0x2C) //special symbols
{
glcd_clear();
read_batt();
glcd_puts("SP SYMBOL  ",0,0);
glcd_puts(disp,0,1);
disp[char_cnt]=rxBuffer[1];
c=rxBuffer[1];
uart_putc(c);
char_cnt++;
check_char();
}

if(cmd==0x0B) //backspace
{
char_cnt--;
disp[char_cnt]=32;
uart_putc(0x08);    // backspace control character
glcd_puts(disp,0,1);
}

if(cmd==0xAC) //clear screen
{
read_batt();
init_disp();
glcd_puts(disp,0,1);
char_cnt=0;
}

if(cmd==0xFF)//display main menu
{
glcd_clear();
read_batt();
menu();
}

if(cmd==0x1F)//display main menu
{
glcd_clear();
read_batt();
submenu();
}

if(cmd==0x2F)//display mouse menu
{
glcd_clear();
read_batt();
mouse_menu();
}

if(cmd==0xAA)//display USER CHOICE
{
glcd_clear();
read_batt();
user_ch();
}

if(cmd==0xAF)// MAIN MENU response
{
read_batt();
inp=rxBuffer[1];
switch(inp)
{
case 1: glcd_clear();
        glcd_puts("Selected...      TEXT MODE",0,3);
		_delay_ms(1000);
		glcd_clear();
        break;

case 2:glcd_clear();
        glcd_puts("Selected...      MOUSE/KEYBOARD   MODE",0,3);
		_delay_ms(1000);
		glcd_clear();
       break;

case 3: glcd_clear();
        glcd_puts("Selected...      DEVICE CONTROL   MODE",0,3);
		_delay_ms(1000);
		glcd_clear();
		glcd_puts("DEVICE CTRL",0,0);
		dev_status();
        break;

case 4:glcd_clear();
        glcd_puts("Selected...      ROBOT CONTROL    MODE",0,3);
		_delay_ms(1000);
		glcd_clear();
		refresh_robo_screen();
		read_batt();
       break;
/*
case 5: glcd_clear();
        glcd_puts("Prepare to train unit...",0,3);
		_delay_ms(500);
		glcd_clear();
		glcd_puts("TRAINING...",0,0);
		alp_cnt=0;
		init_disp();
		read_batt();
       break;
	   */
}


}


if(cmd==0xAE)// USER SETTINGS response
{
read_batt();
inp=rxBuffer[1];

switch(inp)
{
case 1: glcd_clear();
        glcd_puts("WELCOME ANIRUDDH",0,3);
        glcd_puts("LOADING YOUR    SETTINGS....",0,5);
		_delay_ms(2000);
		glcd_clear();
        break;

case 2:glcd_clear();
        glcd_puts("WELCOME SIDDESH",0,3);
        glcd_puts("LOADING YOUR    SETTINGS....",0,5);
		_delay_ms(2000);
		glcd_clear();
       break;

case 3: glcd_clear();
        glcd_puts("WELCOME SHAWN",0,3);
        glcd_puts("LOADING YOUR    SETTINGS....",0,5);
		_delay_ms(2000);
		glcd_clear();
       break;


}
}


if(cmd==0xBF)// SUBMENU response 
{
read_batt();
inp=rxBuffer[1];
switch(inp)
{
case 1:glcd_clear();
       read_batt();
       glcd_puts("CHARACTER  ",0,0);
	   glcd_puts(disp,0,1);
       break;

case 2:glcd_clear();
       read_batt();
       glcd_puts("NUMBER     ",0,0);
	   glcd_puts(disp,0,1);
       break;

case 3:glcd_clear();
       read_batt();
       glcd_puts("SP SYMBOL  ",0,0);
	   glcd_puts(disp,0,1);
       break;

case 4:glcd_clear();
       read_batt();
       glcd_puts("MOUSE      ",0,0);
	   glcd_puts(disp,0,1);
	   uart_puts("<m>");
       break;
}


}



if(cmd==0x1A)//control devices
{
read_batt();
dev_status();
cbi(PORTA,0);

switch(rxBuffer[1])
{
case 1: glcd_puts("1",17,3);
        if(tog[0]==0){sbi(PORTA,1);tog[0]=1;glcd_puts("ON ",89,3);}
        else{cbi(PORTA,1);tog[0]=0;glcd_puts("OFF",89,3);}
		break;

case 2: glcd_puts("2",17,4);
        if(tog[1]==0){sbi(PORTA,2);tog[1]=1;glcd_puts("ON ",89,4);}
        else{cbi(PORTA,2);tog[1]=0;glcd_puts("OFF",89,4);}
		break;

case 3: glcd_puts("3",17,5);
        if(tog[2]==0){sbi(PORTA,3);tog[2]=1;glcd_puts("ON ",89,5);}
        else{cbi(PORTA,3);tog[2]=0;glcd_puts("OFF",89,5);}
		break;

case 4: glcd_puts("4",17,6);
        if(tog[3]==0){sbi(PORTA,4);tog[3]=1;glcd_puts("ON ",89,6);}
        else{cbi(PORTA,4);tog[3]=0;glcd_puts("OFF",89,6);}
		break;

case 5: glcd_puts("5",17,7);
        if(tog[4]==0){sbi(PORTA,5);tog[4]=1;glcd_puts("ON ",89,7);}
        else{cbi(PORTA,5);tog[4]=0;glcd_puts("OFF",89,7);}
		break;

/*case 6: if(tog[5]==0){sbi(PORTA,6);tog[5]=1;}
        else{cbi(PORTA,6);tog[5]=0;}
		break;*/

}
}

if(cmd==0xCF)//mouse control
{

while(cmd==0xCF)
{

if(data_rx()==1)
{
cmd=rxBuffer[0];
x=rxBuffer[1];
y=rxBuffer[2];
lc=rxBuffer[3];
rc=rxBuffer[4];
}

chk_click();

//right
if(x==1){x_movep(6000);}
if(x==2){x_movep(1000);}

//left
if(x==3){x_moven(6000);}
if(x==4){x_moven(1000);}

//down
if(y==1){y_moven(6000);}
if(y==2){y_moven(1000);}

//up
if(y==3){y_movep(6000);}
if(y==4){y_movep(1000);}

}

}


if((cmd==0x1B)||(cmd==0x2B))//robot control
{


switch(rxBuffer[1])
{

//cbi(PORTA,0);

case 0xAB: glcd_puts(" FORWARD",0,7);
           //sbi(PORTA,4);
           break;

case 0xCB: glcd_puts(" LEFT   ",0,7);
           //sbi(PORTA,3);
           break;

case 0xDB: glcd_puts(" RIGHT  ",0,7);
           //sbi(PORTA,2);
           break;

case 0xBB: glcd_puts(" REVERSE",0,7);
           //sbi(PORTA,1);
           break;

default:   glcd_puts(" STOP   ",0,7);
           break;
		    

}

} 

if(cmd==0x9B)//response from robot
{
switch(rxBuffer[1])
{

case 0x1B: glcd_puts(" U.Ctrl ",65,7);
           
           break;

case 0x2B: glcd_puts("  Train ",65,7);
           
           break;

case 0x3B: glcd_puts("  Auto  ",65,7);
           
           break;

case 0x4B: glcd_puts("MEMORY CLEARED!!",0,7);
           _delay_ms(2000);
           break;

default:   glcd_puts("PLEASE WAIT.....",0,7);
           _delay_ms(1000);
		   glcd_puts("                ",0,7);
           break;
		    
refresh_robo_screen();
}
}
    
/*
if(cmd==0xAA)//train unit
{
glcd_puts("TRAINING...",0,0);
read_batt();

if(rxBuffer[1]==0x11){disp[alp_cnt]=(alp_cnt+65);alp_cnt++;}
if((alp_cnt==9)||(alp_cnt==25)){alp_cnt++;}
glcd_puts(disp,0,2);
if(alp_cnt>=25){glcd_puts("training done...",0,7);_delay_ms(500);}
}
*/

	
}
return 0;
}


void init_disp()
{
uint8_t cntr;
for(cntr=0;cntr<max_char;cntr++){disp[cntr]=0;}
}

void check_char()//checks to see if the end of screen is reached and shifts a line upward
{

uint8_t i;
if(char_cnt==max_char)
{
for(i=0;i<(max_char-16);i++) {disp[i]=disp[i+16];}
for(i=96;i<max_char;i++){disp[i]=0x00;}
char_cnt=(max_char-16);
}
glcd_puts("                ",0,7);
glcd_puts(disp,0,1);
}


void welcome()//welcome screen and animation display
{
_delay_ms(1000);
vertical_change(hand6,1);
_delay_ms(1000);
vertical_change(hand1,1);
for(b=0;b<3;b++)
{
bmp_disp(hand1);
_delay_ms(ani_delay);
bmp_disp(hand2);
_delay_ms(ani_delay);
bmp_disp(hand3);
_delay_ms(ani_delay);
bmp_disp(hand4);
_delay_ms(ani_delay);
bmp_disp(hand5);
_delay_ms(ani_delay);
bmp_disp(hand6);
_delay_ms(ani_delay);
bmp_disp(hand5);
_delay_ms(ani_delay);
bmp_disp(hand4);
_delay_ms(ani_delay);
bmp_disp(hand3);
_delay_ms(ani_delay);
bmp_disp(hand2);
_delay_ms(ani_delay);
}

glcd_vclear(1);
horizontal_change(hand6,1);
_delay_ms(1000);
horizontal_change(hand6,0);
_delay_ms(1000);
vertical_change(author,1);
_delay_ms(2500);
glcd_vclear(1);
_delay_ms(1000);
glcd_clear();
glcd_puts("****WELCOME!****",0,3);
glcd_puts("SWITCH ON THE   TRANSMITTER...",0,5);

}

void user_ch()//user select screen
{
glcd_puts("SELECT USER ",0,0);
glcd_puts("================",0,1);
glcd_puts("1) ANIRUDDH",0,3);
glcd_puts("2) SIDDESH ",0,4);
glcd_puts("3) SHAWN   ",0,5);
}

void menu()//main menu
{
glcd_puts("MAIN MENU  ",0,0);
glcd_puts("================",0,1);
glcd_puts("1) txt/num/spch",0,3);
glcd_puts("2) keybd/mouse",0,4);
glcd_puts("3) ctrl device",0,5);
glcd_puts("4) ctrl robot",0,6);
//glcd_puts("5) train unit",0,7);
}

void submenu()//sub menu
{
glcd_puts("SUB MENU   ",0,0);
glcd_puts("================",0,1);
glcd_puts("1) character",0,3);
glcd_puts("2) number",0,4);
glcd_puts("3) special symb",0,5);
}

void mouse_menu()//menu for mouse
{
glcd_puts("SUB MENU   ",0,0);
glcd_puts("================",0,1);
glcd_puts("1) character",0,3);
glcd_puts("2) number",0,4);
glcd_puts("3) special symb",0,5);
glcd_puts("4) mouse ctrl",0,6);
}

void dev_status()
{
uint8_t i;
read_batt();
glcd_puts("DEVICE CTRL",0,0);
glcd_puts("DEVICE",0,1);
glcd_puts("STATUS",73,1);
for(i=0;i<5;i++)
{
glcd_putchar((i+49),17,i+3);
if(tog[i]==1){glcd_puts("ON ",89,i+3);sbi(PORTA,i+1);}
else{glcd_puts("OFF",89,i+3);cbi(PORTA,i+1);}
}

}


void refresh_robo_screen()
{
glcd_puts("ROBOT CTRL",0,0);
glcd_puts(" 1) User Control",0,2);
glcd_puts(" 2) Train Robot",0,3);
glcd_puts(" 3) Auto Mode",0,4);
glcd_puts(" 4) Clear Memory",0,5);
}


void y_movep(uint16_t d)
{
cbi(PORTA,5);
cbi(PORTA,6);
_delay_us(d);
sbi(PORTA,5);
cbi(PORTA,6);
_delay_us(d);
sbi(PORTA,5);
sbi(PORTA,6);
_delay_us(d);
cbi(PORTA,5);
sbi(PORTA,6);
_delay_us(d);
}

void y_moven(uint16_t d)
{
cbi(PORTA,5);
sbi(PORTA,6);
_delay_us(d);
sbi(PORTA,5);
sbi(PORTA,6);
_delay_us(d);
sbi(PORTA,5);
cbi(PORTA,6);
_delay_us(d);
cbi(PORTA,5);
cbi(PORTA,6);
_delay_us(d);
}


void x_movep(uint16_t d)
{
cbi(PORTA,3);
cbi(PORTA,4);
_delay_us(d);
sbi(PORTA,3);
cbi(PORTA,4);
_delay_us(d);
sbi(PORTA,3);
sbi(PORTA,4);
_delay_us(d);
cbi(PORTA,3);
sbi(PORTA,4);
_delay_us(d);

}

void x_moven(uint16_t d)
{
cbi(PORTA,3);
sbi(PORTA,4);
_delay_us(d);
sbi(PORTA,3);
sbi(PORTA,4);
_delay_us(d);
sbi(PORTA,3);
cbi(PORTA,4);
_delay_us(d);
cbi(PORTA,3);
cbi(PORTA,4);
_delay_us(d);

}

void chk_click()
{

if(cmd==0xCF)
{
lc=rxBuffer[3];
rc=rxBuffer[4];

if(rc==0xDF){
sbi(PORTA,1);
//_delay_ms(25);
}
else cbi(PORTA,1);


if(lc==0xEF){
sbi(PORTA,2);
//_delay_ms(25);
}
else cbi(PORTA,2);

}
}



/*
void y_move(uint16_t d, uint8_t dir)//-...0...y...1...+
{

switch(y_cnt)
{

case 1:  cbi(PORTA,5);
         cbi(PORTA,6);
         _delay_us(d);
		 if(dir==1){y_cnt=2;}
		 else{y_cnt=4;}
		 break;
case 2:  sbi(PORTA,5);
         cbi(PORTA,6);
         _delay_us(d);
		 if(dir==1){y_cnt=3;}
		 else{y_cnt=1;}
		 break;

case 3:  sbi(PORTA,5);
         sbi(PORTA,6);
         _delay_us(d);
		 if(dir==1){y_cnt=4;}
		 else{y_cnt=2;}
		 break;

case 4:  cbi(PORTA,5);
         sbi(PORTA,6);
         _delay_us(d);
		 if(dir==1){y_cnt=1;}
		 else{y_cnt=3;}
		 break;

}
}


void x_move(uint16_t d, uint8_t dir) //-...0...x...1...+
{
switch(x_cnt)
{

case 1:  cbi(PORTA,3);
         cbi(PORTA,4);
         _delay_us(d);
		 if(dir==1){x_cnt=2;}
		 else{x_cnt=4;}
		 break;
case 2:  sbi(PORTA,3);
         cbi(PORTA,4);
         _delay_us(d);
		 if(dir==1){x_cnt=3;}
		 else{x_cnt=1;}
		 break;

case 3:  sbi(PORTA,3);
         sbi(PORTA,4);
         _delay_us(d);
		 if(dir==1){x_cnt=4;}
		 else{x_cnt=2;}
		 break;

case 4:  cbi(PORTA,3);
         sbi(PORTA,4);
         _delay_us(d);
		 if(dir==1){x_cnt=1;}
		 else{x_cnt=3;}
		 break;

}
}
*/
