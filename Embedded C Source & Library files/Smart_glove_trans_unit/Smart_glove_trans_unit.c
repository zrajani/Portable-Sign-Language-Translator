
#include<avr/io.h>
#include<compat/deprecated.h>
#include<stdio.h>
#define F_CPU 16000000UL
#include<util/delay.h>
#include<functx.h>
#include<cc2500lib.h>
#include<stdlib.h>
#include<inttypes.h>
#include<math.h>
#include<samp.h>
#include<avr/pgmspace.h>



// data packet is divided into                      EXTRA DATA USED FOR MOUSE MODE
//                                                  --------|----------|----------
// |--flag--|--packet_length--|--command--|--data--|--data(only for mouse mode)---|--CRC--|

//txBuffer[0]=packet length
//txBuffer[1]=command
//txBuffer[2]=data (X-axis data for mouse)
//txBuffer[3]=data (Y-axis data for mouse)
//txBuffer[4]=data (for right click)
//txBuffer[5]=data (for left click)




#define keypress   300   //DELAY IN MILI-SECONDS FOR THE KEY PRESS

#define   total_samples  10 
#define   avg_total      10



#define mouse_rc    420
#define mouse_lc    420


#define alp_puts    470   //threshold for accelerometer value (to put character to screen)



//char patt_match(uint8_t);//the old pattern matching algorithm
char patt_match();//new improved implementation of cornering algorithm
void menu();
void submenu();
void take_samples();
void init_array();
void char_disp();
void num_disp();
void sp_char_disp();
void mousedata_tx();
//int map(int,int,int,int,int);
//void eeprom_map(int);
//void load_val();
//void init_eeprom();
void read_xyz();
void cpy_samp(uint8_t);
void user_ch();


uint16_t samp[total_samples][5],scnt,acnt=0,dist[total_samples][26],min_dist_index[26],min_cnt=0;;
uint16_t robo_x,robo_y;

uint16_t alph[26][5];

uint8_t samp_cnt=0;

//uint8_t temp0,temp1,temp2,temp3,temp4;

uint8_t prob[26],menu_select=0,submenu_select=0,bksp_cnt=0;

char key,matched_char,dev=0;

uint16_t xa,ya,za;
uint8_t tempx,tempy;
uint16_t mouse_x=0,mouse_y=0,lclick=0,rclick=0;
uint8_t rc=0,lc=0,i;

uint8_t mode=0x1B;




int main()   //main code...
{

init_adc();
cc2500_init();
init_array();
user_ch();



while(1)  
{
submenu_select=0;
menu();



//MENU 1: SIGN TO TEXT AND NUMBER MODE

while(menu_select==1)
{
if(submenu_select==0)
{
txBuffer[1]=0x1F;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
submenu();
}

else if(submenu_select==1){char_disp();}
else if(submenu_select==2){num_disp();}
else if(submenu_select==3){sp_char_disp();}
}



//MENU 2: KEYBOARD AND MOUSE MODE

while(menu_select==2)
{

if(submenu_select==0)
{
txBuffer[1]=0x2F;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
submenu();
}

else if(submenu_select==1){char_disp();}
else if(submenu_select==2){num_disp();}
else if(submenu_select==3){sp_char_disp();}
else if(submenu_select==4){mousedata_tx();}

}



//MENU 3: APPLIANCE CONTROL MODE

while(menu_select==3)
{

key=key_read();
while(key=='/'){key=key_read();if(read_adc(7)>alp_puts){key='*';break;}}

if(key=='*')    //normal small characters
{
init_array();
//load_val();
take_samples();
matched_char = patt_match();
if(matched_char!='*')
{
dev=matched_char;
_delay_ms(keypress);
}
else{dev=0;}
}

if(dev=='a')
{
txBuffer[1]=0x1A;
txBuffer[2]=1;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(dev=='b')
{
txBuffer[1]=0x1A;
txBuffer[2]=2;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(dev=='d')
{
txBuffer[1]=0x1A;
txBuffer[2]=3;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(dev=='o')
{
txBuffer[1]=0x1A;
txBuffer[2]=4;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(dev=='y')
{
txBuffer[1]=0x1A;
txBuffer[2]=5;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='9')//EXIT.... go out of sub menu to main menu
{
menu_select=0;
_delay_ms(keypress);
}      

}

//MENU 4: ROBOT CONTROL MODE

while(menu_select==4)
{

robo_y=read_adc(5);
robo_x=read_adc(6);

if((robo_y > 250 )&&(robo_y < 310))//back
{
txBuffer[1]=mode;
txBuffer[2]=0xBB;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
}

else if((robo_y > 350 )&&(robo_y < 400))//forward
{
txBuffer[1]=mode;
txBuffer[2]=0xAB;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
}

else if((robo_x > 250 )&&(robo_x < 310))//right turn
{
txBuffer[1]=mode;
txBuffer[2]=0xDB;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
}

else if((robo_x > 350 )&&(robo_x < 400))//left turn
{
txBuffer[1]=mode;
txBuffer[2]=0xCB;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
}

else //stop
{

txBuffer[1]=mode;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
}


key=key_read();

if(key=='9')//EXIT.... go out of sub menu to main menu
{
menu_select=0;
_delay_ms(keypress);
}      

if(key=='1')//select user mode on robot
{
mode=0x1B;
txBuffer[1]=mode;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}      

if(key=='2')//select training mode on robot
{
mode=0x2B;
txBuffer[1]=mode;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}      

if(key=='3')//select auto mode on robot
{
mode=0x3B;
txBuffer[1]=mode;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}      

if(key=='4')//select clear memory on robot
{
mode=0x4B;
txBuffer[1]=mode;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
mode=0x1B;
}      


}


/*
while(menu_select==5)//learning mode..some problem...still working on it!!
{
key=key_read();
while(key=='/'){key=key_read();}

if(key=='*')
{
for(scnt=0;scnt<avg_total;scnt++)
{
temp0 += read8_adc(0);
temp1 += read8_adc(1);
temp2 += read8_adc(2);
temp3 += read8_adc(3);
temp4 += read8_adc(4);
_delay_ms(100);// wait for samples to settle
}

temp0/=avg_total;
temp1/=avg_total;
temp2/=avg_total;
temp3/=avg_total;
temp4/=avg_total;

eeprom_map(acnt);

acnt+=5;
if(acnt>=135){menu_select=0;load_val();}
}

if(key=='9')//EXIT.... go out of sub menu to main menu
{
menu_select=0;
_delay_ms(keypress);
}      

}
*/





}

return 0;
}



/*************************** FUNCTION DESCRIPTIONS *******************************/

/* 
void init_eeprom()
{
uint8_t i;
for(i=0;i<512;i++)
{e_write(i,0);}
}


void eeprom_map(int al)
{
e_write((al+1),temp0);
e_write((al+2),temp1);
e_write((al+3),temp2);
e_write((al+4),temp3);
e_write((al+5),temp4);

txBuffer[1]=0xAA;
txBuffer[2]=0x11;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
}

void load_val()
{
uint8_t i;
scnt=0;
for(i=0;i<=130;i+=5)
{
if((scnt==9)||(scnt==25)){scnt++;}
else{
alph[scnt][0]=e_read(i+1);
alph[scnt][1]=e_read(i+2);
alph[scnt][2]=e_read(i+3);
alph[scnt][3]=e_read(i+4);
alph[scnt][4]=e_read(i+5);
scnt++;
}
}
}
*/

void read_xyz()
{
xa=read_adc(5);
ya=read_adc(6);
za=read_adc(7);
}

void char_disp()//function for only normal character display
{

key=key_read();
while(key=='/'){key=key_read();if(read_adc(7)>alp_puts){key='*';break;}}

if(key=='*')    //normal small characters
{
init_array();
//load_val();
take_samples();
matched_char = patt_match();
if(matched_char!='*')
{
txBuffer[1]=0x0C;
txBuffer[2]=matched_char;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
//_delay_ms(keypress-50);
}
}

if(key=='7') //for capital characters
{
init_array();
take_samples();
matched_char = patt_match();
if(matched_char!='*')
{
txBuffer[1]=0x0C;
txBuffer[2]=(matched_char-32);
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
//_delay_ms(keypress-50);
}
}

if(key=='0')
{
txBuffer[1]=0x0C;
txBuffer[2]=32;//space
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='1') //backspace and clear screen
{
bksp_cnt=0;
txBuffer[1]=0x0B;
txBuffer[2]=0x00; 
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);

//clear screen if key 1 is pressed for more than 1 second
key=key_read();
while(key=='1'){
bksp_cnt++;
key=key_read();
_delay_ms(100);
if(bksp_cnt>10)
{
txBuffer[1]=0xAC;
txBuffer[2]=0x00; 
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
break;
}
}
}

/*
if(key=='2')
{
txBuffer[1]=0x0C;
txBuffer[2]='j'; //letter j
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='3')
{
txBuffer[1]=0x0C;
txBuffer[2]='z';//letter z
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='4')
{
txBuffer[1]=0x0C;
txBuffer[2]='d';//letter d
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='5')
{
txBuffer[1]=0x0C;
txBuffer[2]='f';//letter f
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='6')
{
txBuffer[1]=0x0C;
txBuffer[2]='k';//letter k
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='8')
{
txBuffer[1]=0x0C;
txBuffer[2]='x'; //letter x
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}
*/

if(key=='9')//EXIT.... go out of sub menu to main menu
{
menu_select=0;
}      

if(key=='#'){submenu_select=0;}

}


void num_disp()
{

key=key_read();
while(key=='/'){key=key_read();}


if(key=='#'){submenu_select=0;}

else{
txBuffer[1]=0x1C;
txBuffer[2]=key; 
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}


}


void sp_char_disp()
{

key=key_read();
while(key=='/'){key=key_read();}


if(key=='0')
{
txBuffer[1]=0x0C;
txBuffer[2]=32;//space
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='1') //backspace and clear screen
{
bksp_cnt=0;
txBuffer[1]=0x0B;
txBuffer[2]=0x00; 
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);

//clear screen if key 1 is pressed for more than 1 second
key=key_read();
while(key=='1')
{
bksp_cnt++;
key=key_read();
_delay_ms(100);
if(bksp_cnt>10)
{
txBuffer[1]=0xAC;
txBuffer[2]=0x00; 
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
break;
}
}
}

if(key=='2')
{
txBuffer[1]=0x2C;
txBuffer[2]='!'; //exclamation mark
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='3')
{
txBuffer[1]=0x2C;
txBuffer[2]='%';//percent symbol
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='4')
{
txBuffer[1]=0x2C;
txBuffer[2]='.';//full stop
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='5')
{
txBuffer[1]=0x2C;
txBuffer[2]=',';//comma
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='6')
{
txBuffer[1]=0x2C;
txBuffer[2]='?';//question mark
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='7')
{
txBuffer[1]=0x2C;
txBuffer[2]='&'; //amphacent
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='8')
{
txBuffer[1]=0x2C;
txBuffer[2]='-'; //dash
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();
_delay_ms(keypress);
}

if(key=='9')//EXIT.... go out of sub menu to main menu
{
menu_select=0;
}      

if(key=='#'){submenu_select=0;}

}


//PATTERN MATCHING AND CORNERING ALGORITHM IMPLEMENTATION//

//using Brute Force algorithm instead of KMP algorithm for minimal memory use...

/*
char patt_match(uint8_t err) // pattern matching code
{
uint8_t  al_cnt=0,val_cnt=0,samp_cnt=0,match_fnd=0,tie=0;

char fnd_char='*';

for(al_cnt=0;al_cnt<26;al_cnt++)
{
samp_cnt=0;

while(samp_cnt<total_samples)
{
match_fnd=0;

for(val_cnt=0;val_cnt<5;val_cnt++)
{ 

if((alph[al_cnt][val_cnt]<(samp[samp_cnt][val_cnt]+err)) && (alph[al_cnt][val_cnt]>(samp[samp_cnt][val_cnt]-err)))
{
match_fnd++;
}

else {break;}
}

if(match_fnd==5){prob[al_cnt]+=1;}
samp_cnt++;
}

}

for(al_cnt=0;al_cnt<26;al_cnt++)
{
if(prob[al_cnt]>=8){tie++;fnd_char=(al_cnt+97);}
}

//if((tie!=1)&&(err>5)){return patt_match((err-1));}
if((tie!=1)&&(err>0)){return patt_match((err-1));}
else 
{

//check for similar character signs whose real match gets confusing...
//use acclerometer reading to get out of this problem...!

if((fnd_char=='p')||(fnd_char=='q')||(fnd_char=='h')||(fnd_char=='k')||(fnd_char=='u'))
{
read_xyz();
if((xa>320)&&(ya<280)){return 'h';}
else if((xa<280)&&(ya>315)) {return 'k';}
else if((xa>310)&&(ya>310)) {return fnd_char;}
}

else if((fnd_char=='i')||(fnd_char=='j'))
{
read_xyz();
if((xa>320)&&(ya<280)){return 'j';}
else return 'i';
}

else if((fnd_char=='d')||(fnd_char=='z'))
{
read_xyz();
if((xa>310)&&(ya>310)){return 'z';}
}

return fnd_char;
}

}
*/

char patt_match()
{
uint8_t  al_cnt=0,samp_cnt=0,match_fnd=0,tie=0,temp_val=0,min_val;
char fnd_char='*';

for(al_cnt=0;al_cnt<26;al_cnt++)//find distance for all characters
{

for(samp_cnt=0;samp_cnt<total_samples;samp_cnt++)
{
temp_val+=abs(alph[al_cnt][0]-samp[samp_cnt][0]);
temp_val+=abs(alph[al_cnt][1]-samp[samp_cnt][1]);
temp_val+=abs(alph[al_cnt][2]-samp[samp_cnt][2]);
temp_val+=abs(alph[al_cnt][3]-samp[samp_cnt][3]);
temp_val+=abs(alph[al_cnt][4]-samp[samp_cnt][4]);

dist[samp_cnt][al_cnt]=temp_val;
}
}

min_val=dist[(total_samples/2)][0];

for(al_cnt=1;al_cnt<26;al_cnt++)//find minimum distance and also check for tie
{
if(dist[(total_samples/2)][al_cnt]<min_val)
{
min_val=dist[(total_samples/2)][al_cnt];
tie=0;
min_cnt=0;
min_dist_index[min_cnt]=al_cnt;
}

if(dist[(total_samples/2)][al_cnt]==min_val)
{min_dist_index[min_cnt]=al_cnt;tie=1;min_cnt++;}
}

//calculate probability of certain match
if(tie==0)
{
for(samp_cnt=0;samp_cnt<total_samples;samp_cnt++)
{
if(dist[samp_cnt][min_dist_index[0]]<=min_val)
{match_fnd++;}
}
if(match_fnd>=8){fnd_char=(min_dist_index[0]+97);}
}

else
{

al_cnt=0;
while(al_cnt<=min_cnt)
{
for(samp_cnt=0;samp_cnt<total_samples;samp_cnt++)
{
if(dist[samp_cnt][min_dist_index[al_cnt]]<=min_val)
{match_fnd++;}
}

if(match_fnd>=8){fnd_char=(min_dist_index[al_cnt]+97);}
al_cnt++;
}
}

//check for similar character signs whose real match gets confusing...
//use acclerometer reading to get out of this problem...!

if((fnd_char=='p')||(fnd_char=='q')||(fnd_char=='h')||(fnd_char=='k')||(fnd_char=='u'))
{
read_xyz();
if((xa>320)&&(ya<280)){return 'h';}
else if((xa<280)&&(ya>315)) {return 'k';}
else if((xa>310)&&(ya>310)) {return fnd_char;}
}

else if((fnd_char=='i')||(fnd_char=='j'))
{
read_xyz();
if((xa>320)&&(ya<280)){return 'j';}
else return 'i';
}

else if((fnd_char=='d')||(fnd_char=='z'))
{
read_xyz();
if((xa>310)&&(ya>310)){return 'z';}
}

return fnd_char;

}

void take_samples()//take multiple samples and average
{
uint8_t samp_cnt,avg;
uint16_t v0=0,v1=0,v2=0,v3=0,v4=0;

for(samp_cnt=0;samp_cnt<total_samples;samp_cnt++)
{
for(avg=0;avg<avg_total;avg++)
{
v0 +=read_adc(0);
v1 +=read_adc(1);
v2 +=read_adc(2);
v3 +=read_adc(3);
v4 +=read_adc(4);
}

samp[samp_cnt][0]=(uint16_t)(v0/avg_total);
samp[samp_cnt][1]=(uint16_t)(v1/avg_total);
samp[samp_cnt][2]=(uint16_t)(v2/avg_total);
samp[samp_cnt][3]=(uint16_t)(v3/avg_total);
samp[samp_cnt][4]=(uint16_t)(v4/avg_total);
}
}


void init_array()
{

uint8_t cnt;

for(cnt=0;cnt<total_samples;cnt++)
{
samp[cnt][0]=0;
samp[cnt][1]=0;
samp[cnt][2]=0;
samp[cnt][3]=0;
samp[cnt][4]=0;
}
/*
for(cnt=0;cnt<26;cnt++)
{
alph[cnt][0]=0;
alph[cnt][1]=0;
alph[cnt][2]=0;
alph[cnt][3]=0;
alph[cnt][4]=0;
}
*/

for(cnt=0;cnt<26;cnt++){prob[cnt]=0;}
}



void mousedata_tx()
{

lclick=read_adc(1);
rclick=read_adc(2);
mouse_y=read_adc(5);
mouse_x=read_adc(6);

tempx=0;
tempy=0;

if(rclick<=mouse_rc) {rc=0xDF;}
else{rc=0x00;}

if(lclick<=mouse_lc) {lc=0xEF;}
else{lc=0x00;}


if((mouse_y > 250 )&&(mouse_y < 290))//fast down
{
tempy=4;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_y > 290 )&&(mouse_y < 310))//slow down
{
tempy=3;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_y > 350 )&&(mouse_y < 370))//slow up
{
tempy=1;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_y > 370 )&&(mouse_y < 400))//fast up
{
tempy=2;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_x > 250 )&&(mouse_x < 290))//fast right 
{
tempx=2;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_x > 290 )&&(mouse_x < 310))//slow right 
{
tempx=1;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_x > 350 )&&(mouse_x < 370))//fast left 
{
tempx=3;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}

if((mouse_x > 370 )&&(mouse_x < 400))//slow left 
{
tempx=4;
txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();
}


txBuffer[1]=0xCF;
txBuffer[2]=tempx; 
txBuffer[3]=tempy;
txBuffer[4]=lc;
txBuffer[5]=rc;
data_tx();

key=key_read();
if(key=='#'){submenu_select=0;_delay_ms(keypress);}

}




void menu() //main menu
{

txBuffer[1]=0xFF;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();

key=key_read();
while(key=='/'){key=key_read();}
_delay_ms(keypress);

switch(key)
{
case'1':menu_select=1;
        txBuffer[1]=0xAF;
        txBuffer[2]=1;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'2':menu_select=2; 
        txBuffer[1]=0xAF;
        txBuffer[2]=2;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'3':menu_select=3;
        txBuffer[1]=0xAF;
        txBuffer[2]=3;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'4':menu_select=4;
        txBuffer[1]=0xAF;
        txBuffer[2]=4;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;
/*
case'5':menu_select=5;
        txBuffer[1]=0xAF;
        txBuffer[2]=5;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
		init_eeprom();
		init_array();
        break;
*/

}
_delay_ms(1500);
}



void submenu()//menu for mouse and normal text mode
{

key=key_read();
while(key=='/'){key=key_read();}
_delay_ms(keypress);

switch(key)
{
case'1':submenu_select=1;
        txBuffer[1]=0xBF;
        txBuffer[2]=1;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'2':submenu_select=2; 
        txBuffer[1]=0xBF;
        txBuffer[2]=2;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'3':submenu_select=3;
        txBuffer[1]=0xBF;
        txBuffer[2]=3;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'4':if(menu_select==2)
        {
        submenu_select=4;
        txBuffer[1]=0xBF;
        txBuffer[2]=4;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
		}
		else {submenu_select=0;}
        break;

case'9':submenu_select=0;
        menu_select=0;
        break; 

}
}

/*
void cpy_samp(uint16_t * samp)
{
for(samp_cnt=0;samp_cnt<26;samp_cnt++)
{
alph[samp_cnt][0]=pgm_read_word(& samp[samp_cnt][0]);
alph[samp_cnt][1]=pgm_read_word(& samp[samp_cnt][1]);
alph[samp_cnt][2]=pgm_read_word(& samp[samp_cnt][2]);
alph[samp_cnt][3]=pgm_read_word(& samp[samp_cnt][3]);
alph[samp_cnt][4]=pgm_read_word(& samp[samp_cnt][4]);
}

}
*/


void cpy_samp(uint8_t ch)
{

switch(ch)
{

case 1:
for(samp_cnt=0;samp_cnt<26;samp_cnt++)
{
alph[samp_cnt][0]=aniruddh[samp_cnt][0];
alph[samp_cnt][1]=aniruddh[samp_cnt][1];
alph[samp_cnt][2]=aniruddh[samp_cnt][2];
alph[samp_cnt][3]=aniruddh[samp_cnt][3];
alph[samp_cnt][4]=aniruddh[samp_cnt][4];
}

break;


case 2:
for(samp_cnt=0;samp_cnt<26;samp_cnt++)
{
alph[samp_cnt][0]=siddesh[samp_cnt][0];
alph[samp_cnt][1]=siddesh[samp_cnt][1];
alph[samp_cnt][2]=siddesh[samp_cnt][2];
alph[samp_cnt][3]=siddesh[samp_cnt][3];
alph[samp_cnt][4]=siddesh[samp_cnt][4];
}

break;

case 3:
for(samp_cnt=0;samp_cnt<26;samp_cnt++)
{
alph[samp_cnt][0]=shawn[samp_cnt][0];
alph[samp_cnt][1]=shawn[samp_cnt][1];
alph[samp_cnt][2]=shawn[samp_cnt][2];
alph[samp_cnt][3]=shawn[samp_cnt][3];
alph[samp_cnt][4]=shawn[samp_cnt][4];
}
break;

//add more users here...
}

}


void user_ch()
{
txBuffer[1]=0xAA;
txBuffer[2]=0x00;
txBuffer[3]=0x00;
txBuffer[4]=0x00;
txBuffer[5]=0x00;
data_tx();

key=key_read();
while(key=='/'){key=key_read();}
_delay_ms(keypress);


switch(key)
{
case'1':cpy_samp(1);
        txBuffer[1]=0xAE;
        txBuffer[2]=1;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'2':cpy_samp(2); 
        txBuffer[1]=0xAE;
        txBuffer[2]=2;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;

case'3':cpy_samp(3);
        txBuffer[1]=0xAE;
        txBuffer[2]=3;
        txBuffer[3]=0x00;
        txBuffer[4]=0x00;
        txBuffer[5]=0x00;
        data_tx();
        break;


}
_delay_ms(2800);
}



/*
int map(int x, int in_min, int in_max, int out_min, int out_max)//mapping function
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
*/


