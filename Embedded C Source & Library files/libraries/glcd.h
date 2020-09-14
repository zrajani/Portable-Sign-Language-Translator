/****************************************************************************
 Title :    ATmega Graphic LCD library
 Author:    Shawn D'Souza <shawnelec@gmail.com>
 Software:  AVR-GCC 3.3 
 Target:    any AVR ATmega series device with minimum 3 ports

 DESCRIPTION
       this library for AVR Studio 4 is a simple interface for the Graphic LCD. 
	   The source code is a compilation and modification of many author's 
	   source code done under the GPL. This code is free to modify and use.  
	   Special thanks to Osama for some of his wonderful library functions. 
	   
	   please use 16Mhz crystal in the circuit. 
	   
	   configure the Ports to suit your convenience.

	   important: initialize the control port and data port in the program 
 
 USAGE
       add this header file as #include <glcd.h>
*****************************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "font.h"
#define F_CPU 16000000UL
#include <util/delay.h>

typedef unsigned char byte;

#define E_DELAY 5
//change to suit your design.

#define DATAPORT PORTC
#define CONTROLPORT	PORTD
#define DATADDR DDRC
#define DATAPIN PINC
#define CS1 (1<<3)
#define CS2 (1<<4)
#define DI  (1<<5) //RS
#define RW  (1<<6)
#define EN  (1<<7)

//#define CS_ACTIVE_LOW   1   //Define this if your GLCD CS
                              //is active low 

void trigger()
{
	CONTROLPORT |= EN;	//EN high
   _delay_us(E_DELAY);
   CONTROLPORT &= ~EN;	//EN low
   _delay_us(E_DELAY);
}
//----------------------
void glcd_on()
{
#ifdef CS_ACTIVE_LOW
   CONTROLPORT &= ~CS1;	//Activate both chips
   CONTROLPORT &= ~CS2;
#else
   CONTROLPORT |= CS1;	//Activate both chips
   CONTROLPORT |= CS2;
#endif
   CONTROLPORT &= ~DI;	//DI low --> command
   CONTROLPORT &= ~RW;	//RW low --> write
   DATAPORT = 0x3F; 	//ON command
   trigger();
}           
//----------------------
void glcd_off()
{
#ifdef CS_ACTIVE_LOW
   CONTROLPORT &= ~CS1;	//Activate both chips
   CONTROLPORT &= ~CS2;
#else
   CONTROLPORT |= CS1;	//Activate both chips
   CONTROLPORT |= CS2;
#endif
	CONTROLPORT &= ~DI;	//DI low --> command
	CONTROLPORT &= ~RW;	//RW low --> write
	DATAPORT = 0x3E;    //OFF command
	trigger();
}     
//----------------------
void set_start_line(byte line)
{
	CONTROLPORT &= ~DI;			//RS low --> command
	CONTROLPORT &= ~RW;			//RW low --> write
    //Activate both chips
    #ifdef CS_ACTIVE_LOW
		CONTROLPORT &= ~CS1;
		CONTROLPORT &= ~CS2;
    #else
		CONTROLPORT |= CS1;
		CONTROLPORT |= CS2;
    #endif
    DATAPORT = 0xC0 | line;     //Set Start Line command
    trigger();
}
//----------------------
void goto_col(unsigned int x)
{
   byte pattern;
   CONTROLPORT &= ~DI;	//DI low --> command
   CONTROLPORT &= ~RW;	//RW low --> write
   if(x<64)             //left section
   {
#ifdef CS_ACTIVE_LOW
      CONTROLPORT &= ~CS1;	//select chip 1
      CONTROLPORT |= CS2;	//deselect chip 2
#else
      CONTROLPORT |= CS1;	//select chip 1
      CONTROLPORT &= ~CS2;	//deselect chip 2
#endif
      pattern = x;      	//put column address on data port
   }
   else                 //right section
   {
#ifdef CS_ACTIVE_LOW
	   CONTROLPORT |= CS1;	//deselct chip 1
	   CONTROLPORT &= ~CS2;	//select chip 2
#else
	   CONTROLPORT &= ~CS1;	//deselct chip 1
	   CONTROLPORT |= CS2;	//select chip 2
#endif
	   pattern = x-64;   //put column address on data port
   }    
   pattern = (pattern | 0x40 ) & 0x7F;  //Command format
   DATAPORT = pattern; 
   trigger();     
}   
//----------------------
void goto_row(unsigned int y)
{    
   byte pattern;
   CONTROLPORT &= ~DI;	//DI low --> command
   CONTROLPORT &= ~RW;	//RW low --> write
   pattern = (y | 0xB8) & 0xBF; //put row address on data port set command
   DATAPORT = pattern;   
   trigger();             
}
//----------------------
void goto_xy(unsigned int x,unsigned int y)
{    
    goto_col(x);
    goto_row(y);
}  
//----------------------
void glcd_write(byte b)
{
	CONTROLPORT |= DI;	//DI high --> data
	CONTROLPORT &= ~RW;	//RW low --> write
	DATAPORT = b;    	//put data on data port
   _delay_us(1);
   trigger();
}
//------------------------
void glcd_clrln(byte ln)
{
    int i;
    goto_xy(0,ln);      //At start of line of left side
    goto_xy(64,ln);     //At start of line of right side (Problem)
#ifdef CS_ACTIVE_LOW
    CONTROLPORT &= ~CS1;
#else
    CONTROLPORT |= CS1;
#endif
    for(i=0;i<64;i++)
        glcd_write(0);
}
//-------------------------
void glcd_clear() //clear the screen
{
   int i;
   for(i=0;i<8;i++)
	   glcd_clrln(i);
}
//-----------------------   
byte glcd_read(byte column)
{
    byte read_data = 0; //Read data here
    DATADDR = 0x00;     //Input 
    
    CONTROLPORT |= RW;	//Read
    CONTROLPORT |= DI;	//Data

    if(column<64)
    {
#ifdef CS_ACTIVE_LOW
    	CONTROLPORT &= ~CS1;	//Enable CS1
    	CONTROLPORT |= CS2;		//Disable CS2
#else
    	CONTROLPORT |= CS1;		//Enable CS1
    	CONTROLPORT &= ~CS2;	//Disable CS2
#endif
    }
    else
    {
#ifdef CS_ACTIVE_LOW
    	CONTROLPORT &= ~CS2;	//Enable CS2
    	CONTROLPORT |= CS1;		//Disable CS1
#else
    	CONTROLPORT |= CS2;		//Enable CS2
    	CONTROLPORT &= ~CS1;	//Disable CS1
#endif
    }
    _delay_us(1);        		//tasu
    CONTROLPORT |= EN;			//Latch RAM data into ouput register
    _delay_us(1);        		//twl + tf
    
    //Dummy read
    CONTROLPORT &= ~EN;			//Low Enable
    _delay_us(20);       		//tf + twl + error
    
    CONTROLPORT |= EN;			//latch data from output register to data bus
    _delay_us(1);        		//tr + td(twh)
                                  
    read_data = DATAPIN;    	//Input data
    CONTROLPORT &= ~EN;			//Low Enable to remove data from the bus
    _delay_us(1);        		//tdhr
    DATADDR = 0xFF;             //Output again
    return read_data;      
}
//-----------------------
// set color 0 for light and 1 for dark

void point_at(unsigned int x,unsigned int y,byte color)// set point at (x,y,color)
{
    byte pattern = 0;
    goto_xy(x,(int)(y/8));  
    switch (color)
    {               
        case 0:                                
            pattern = ~(1<<(y%8)) & glcd_read(x);                  
        break;    
        case 1:                     
            pattern = (1<<(y%8)) | glcd_read(x);
        break;
    }           
    goto_xy(x,(int)(y/8));                                
    glcd_write(pattern);
}
//-----------------------
void h_line(unsigned int x,unsigned int y,byte l,byte s,byte c)// horizontal line (x,y,length,space between points,color)
{
    int i;
    for(i=x; i<(l+x); i += (byte)(s+1))
        point_at(i,y,c);
}
//-----------------------
void v_line(unsigned int x,unsigned int y,signed int l,byte s,byte c)// vertical line (x,y,length,space between points,color)
{
    unsigned int i;
    for(i=y; i<(y+l); i += (byte)(s+1))
        point_at(x,i,c);
}
//-----------------------
void line(unsigned int x1,unsigned int y1,
          unsigned int x2,unsigned int y2,
          byte s,byte c)//line (x1,y1,x2,y2,space between points,color)
{
    byte i;
    byte y01;
    
    byte temp;
    
    float a;
    float b;
    
    byte y00;
    byte y010;
    
    if(x1==x2) 
    {    
        v_line(x1,fmin(y1,y2),abs(y2-y1)+1,s,c);
    }
    else if(y1==y2)
    {
        h_line(fmin(x1,x2),y1,abs(x2-x1)+1,s,c);
    }
    else    
    {
        if(x1>x2)
        {
            temp = x1;
            x1 = x2;
            x2 = temp;
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        
        a = (float)(signed)(y2-y1)/(x2-x1);
        b = y1 - a*x1;
        y00 = a*x1 + b;
        y010 = y00;    
                   
        for(i=(x1+1); i<=x2; i ++)
        {
            y01 = a*i + b + 0.5; //+ 0.5 to approximate to the nearest integer
                
            if( (signed char)(y01 - y010) > 1 )
            {
                v_line((i-1),(byte)(y010+1),(y01-y010-1),s,c);
            }
            else if( (signed char)(y010 - y01) > 1 )
            {
                v_line((i-1),(byte)(y01+1),(y010-y01-1),s,c);
            } 
                       
            if((i==x2) && (y00 == y01)) h_line(x1,y01,(x2-x1),0,c);                                  
            y010 = y01;       
            if( y00 ==  y01) continue;
            h_line(x1,y00,(i-x1),0,c);
            x1 = i;
            y00 = y01;
        }
        point_at(x2,y2,c); 
    }          
}

void rectangle(unsigned int x1,unsigned int y1,
               unsigned int x2,unsigned int y2,
               byte s,byte c)// rectangle(x1,y1,x2,y2,space between points,color)
{
    h_line(x1,y1,(x2-x1),s,c);
    h_line(x1,y2,(x2-x1),s,c);
    v_line(x1,y1,(y2-y1),s,c);
    v_line(x2,y1,(y2-y1+1),s,c);
}


void cuboid(unsigned int x11,unsigned int y11,
            unsigned int x12,unsigned int y12,
            unsigned int x21,unsigned int y21,
            unsigned int x22,unsigned int y22,
            byte s,byte c)
{
    rectangle(x11,y11,x12,y12,s,c);
    rectangle(x21,y21,x22,y22,s,c);
    line(x11,y11,x21,y21,s,c);
    line(x12,y11,x22,y21,s,c);
    line(x11,y12,x21,y22,s,c);
    line(x12,y12,x22,y22,s,c);
}
//-----------------------
void h_parallelogram(unsigned int x1,unsigned int y1,
                     unsigned int x2,unsigned int y2,
                     byte l,byte s,byte c)// horizontal parallelogram (x,y,length,space between points,color)
{
    h_line(x1,y1,l,s,c);
    h_line((x2-l+1),y2,l,s,c);
    line(x1,y1,(x2-l+1),y2,s,c);
    line((x1+l-1),y1,x2,y2,s,c);
}
//-----------------------
void v_parallelogram(unsigned int x1,unsigned int y1,
                     unsigned int x2,unsigned int y2,
                     byte l,byte s,byte c)
{
    v_line(x1,y1,l,s,c);
    v_line(x2,(y2-l+1),l,s,c);
    line(x1,y1,x2,(y2-l+1),s,c);
    line(x1,(y1+l-1),x2,y2,s,c);
}
//-----------------------
void h_parallelepiped(unsigned int x11,unsigned int y11,
                      unsigned int x12,unsigned int y12,byte l1,
                      unsigned int x21,unsigned int y21,
                      unsigned int x22,unsigned int y22,byte l2,
                      byte s,byte c)
{
    h_parallelogram(x11,y11,x12,y12,l1,s,c);
    h_parallelogram(x21,y21,x22,y22,l2,s,c);
    line(x11,y11,x21,y21,s,c);
    line(x12,y12,x22,y22,s,c);
    line((x11+l1-1),y11,(x21+l2-1),y21,s,c);
    line((x12-l1+1),y12,(x22-l2+1),y22,s,c);
}
//-----------------------
void v_parallelepiped(unsigned int x11,unsigned int y11,
                      unsigned int x12,unsigned int y12,byte l1,
                      unsigned int x21,unsigned int y21,
                      unsigned int x22,unsigned int y22,byte l2,
                      byte s,byte c)
{
    v_parallelogram(x11,y11,x12,y12,l1,s,c);
    v_parallelogram(x21,y21,x22,y22,l2,s,c);
    line(x11,y11,x21,y21,s,c);
    line(x12,y12,x22,y22,s,c);
    line(x11,(y11+l1-1),x21,(y21+l2-1),s,c);
    line(x12,(y12-l1+1),x22,(y22-l2+1),s,c);
}
//-----------------------
void circle(unsigned int x0,unsigned int y0,
            unsigned int r,byte s,byte c)
{
    byte i,y,y00;
    y00 = r;            //Point (0,r) is the 1st point
    for(i=0; i<r; i++)
    {
        y = sqrt((int)r*r - (int)i*i);
        point_at((x0+i),(y0+y),c);
        point_at((x0+i),(y0-y),c);
        point_at((x0-i),(y0+y),c);
        point_at((x0-i),(y0-y),c);        
        if(abs(y00-y)>1)
        {
            v_line(x0+(i-1),y0+fmin(y00,y)+1,abs(y00-y),s,c);
            v_line(x0+(i-1),y0-fmax(y00,y),abs(y00-y),s,c);
            v_line(x0-(i-1),y0+fmin(y00,y)+1,abs(y00-y),s,c);
            v_line(x0-(i-1),y0-fmax(y00,y),abs(y00-y),s,c);
        }
        y00 = y;   
    }
    v_line(x0+(i-1) ,y0,y ,s,c);
    v_line(x0+(i-1) ,y0-y ,y,s,c);
    v_line(x0-(i-1) ,y0,y ,s,c);
    v_line(x0-(i-1) ,y0-y ,y,s,c);    
}

void glcd_putchar(int c,int x,int y) //(character ie x+8 and  line y from 0-8
{
   byte i;
  goto_col(x);
  goto_row(y);
  for(i=0;i<8;i++)
  {
     glcd_write(pgm_read_byte(&(font[(8*(c))+i]))); 
  }
}

/*
void glcd_putchar16(int c,int x,int y) //(character ie x+16 and  line y from 0-4
{
   byte i;
  goto_col(x);
  goto_row(y);
  for(i=0;i<16;i++)
  {
     glcd_write(pgm_read_byte(&(font16[(8*(c))+i])));
  }
  goto_row(y+1);
  for(i=0;i<16;i++)
  {
     glcd_write(pgm_read_byte(&(font16[(8*(c))+i])));
  }
}
*/


//---------------------------
void glcd_puts(char *c,int x,int y)
{
   char i = 0;
   while(i<strlen(c))
   {
      glcd_putchar(*(c+i),x,y);
      x += 8;
      if(x>=128)
      {
         x=0;
         y++;
      }
      i++;
   }
}

void glcdwrite (uint8_t b)
{
	CONTROLPORT |= DI;	//DI high --> data
	CONTROLPORT &= ~RW;	//RW low --> write
	DATAPORT = b;    	//put data on data port
   _delay_us(1);
   trigger();
}

//---------------------BITMAP IMAGE DISPLAY----------------------------
void bmp_disp(byte *bmp)
{
int i;
for(i=0;i<1024;i++)
{       

   goto_col(i%128);            
   goto_row(i/128);   
   glcd_write(pgm_read_byte(&bmp[i]));                              
}   
}


//----------------ANIMATION AND TRANSITION EFFECTS---------------------

void horizontal_change(byte *bmp,uint8_t color)
//transition to new image with horizontal wipe-in effect 
{
int i;
for(i=0;i<1024;i++)
{       

   goto_col(i%128);            
   goto_row(i/128); 
   if(color==1)  
   glcd_write(pgm_read_byte(&bmp[i])); 
   else 
   glcd_write(~(pgm_read_byte(&bmp[i]))); 
   _delay_ms(2);                            
}   
}

void vertical_change(byte *bmp,uint8_t color)
//transition to new image with vertical wipe-in effect  
{
int i,cnt=0,cnt_prev=0;

for(i=0;i<1024;i++)
{       

   goto_col(i%128);            
   goto_row(i/128); 
   cnt=(int)(i/128);
   if(color==1)  
   glcd_write(pgm_read_byte(&bmp[i])); 
   else 
   glcd_write(~(pgm_read_byte(&bmp[i]))); 
   if(cnt>cnt_prev)
   {_delay_ms(200);cnt_prev=cnt;}  
} 

}

void glcd_hclear(uint8_t color)
//clears screen with horizontal wipe-in effect 
{
int i;
for(i=0;i<1024;i++)
{       

   goto_col(i%128);            
   goto_row(i/128);   
   if(color==1)  
   glcd_write(0xff); 
   else 
   glcd_write(0x00); 
   _delay_ms(2);                            
}                                
}   

void glcd_vclear(uint8_t color)
//clears screen with vertical wipe-in effect 
{
int i,cnt=0,cnt_prev=0;

for(i=0;i<1024;i++)
{       

   goto_col(i%128);            
   goto_row(i/128); 
   cnt=(int)(i/128);  
   if(color==1)  
   glcd_write(0xff); 
   else 
   glcd_write(0x00); 
   if(cnt>cnt_prev)
   {_delay_ms(200);cnt_prev=cnt;}                              
}   
}


