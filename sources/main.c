#define ENABLE_BIT_DEFINITIONS
#define MASTER
#include <iom16.h>
#include <intrinsics.h>
#include <stdbool.h>
#include <stdlib.h>
#include "def.c"
#include "lcd1602.c"
#include "motor.c"
#include "light.c"


void main()
{
	unsigned char refresh=0,ret_stdby=0,key;
	
	__disable_interrupt();
	initlcd();
	print("Initializing...");  
	locate(0,1);
	init_tc1();
	init_s_motor();
	init_adc();
	DDRD&=~0x0f;//initialize key
	PORTD|=0x0f;
	DDRB|=(1<<PB4);
	PORTB&=~(1<<PB4);
	ENABLE_KEY;
	locate(0,1);
	print("by Wang C.X.");
	delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);
	delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);
	delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);
	locate(0,1);
	print("Jining,Shandong");
	delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);
	delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);
	delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);delay(65000);
	__enable_interrupt();
	while(1)
	{
		key=getkey(0,0);
		
		if(!stdby)
		{
			turn();
		}
		
		if(trans_ok)
		{
			trans_ok=0;
			ADCSRA|=0x40;
			refresh++;
			if(refresh>=REFRESH)
			{
				refresh=0;
				disp_dat();
			}
			if(F_VAL<stdby_val && en_stdby)
			{
				sm(STOP);
				stdby=1;
			}
			else if(stdby && compare(F_VAL,stdby_val,START_VAL)==1 )
			{
				if(++ret_stdby>200)
				{
					stdby=0;
					ret_stdby=0;
				}
			}
			else
			{
				ret_stdby=0;
			}
		}
		
		if(key==1)
		{
			sm(STOP);
			__disable_interrupt();
			getkey(1,0);
			key=0;
			settings();
			getkey(1,0);
			init_tc1();
			init_s_motor();
			init_adc();
			__enable_interrupt();
		}
		else if(key==2)
		{
			cls();
			print("please wait...");
			if(azimuth>0)
			{
				sm(RIGHT);
			}
			else if(azimuth<0)
			{
				sm(LEFT);
			}
			while( !(compare(azimuth,0,1)==0) );
			sm(STOP);
			set_elev(MIN_elev);
			__disable_interrupt();
			break;
		}
		
			delay(sys_speed);
	}
	cls();
	print("Now,turn it off.");
	while(1);
}

void disp_dat()
{
	cls();
	print("L");
	printnum(L_VAL,0);
	print("R");
	printnum(R_VAL,0);
	if(stdby)
		print(" STD");
	if(uarterr)
		print("UE");
	else
	{
		print(" ");
		printnum(abs(azimuth),1);
		print("\xdf");
	}
	locate(0,1);
	print("U");
	printnum(U_VAL,0);
	print("D");
	printnum(D_VAL,0);
	print(" F");
	printnum(F_VAL,0);
}

void settings()
{
	signed char num;
	char key;
	signed char t;
	uint pushtime;
	bool enset;
	num=0;
	pushtime=0;
	enset=0;
	while(1)
	{
		cls();
		print(menu[num]);
		locate(0,1);
		if(num<OPTIONS)
		{
			if(*ep_o[num])
			{
				print("enable");
			}
			else
			{
				print("disable");
			}
		}
		else if(num<OPTIONS+PARAMETERS)
		{
			printnum(*ep_p[num-OPTIONS],0);
		}
		else
		{
			print("Press key3");
		}
		if(enset)
		{
			locate(13,1);
			print("Set");
		}
		else
		{
			locate(11,1);
			printnum(num,1);
			print("/");
			printnum(OPTIONS+PARAMETERS,1);
		}
		key=getkey(0,1);
		
		t=0;
		switch(key)
		{
		 case 1:
			t=-1;break;
		 case 2:
			t=1;break;
		 case 3:
			enset=!enset;break;
		 case 4:
			return;
		}
		
		if(enset)
		{
			if(num<OPTIONS)
			{
				*ep_o[num]=!(*ep_o[num]);
				enset=0;
				getkey(1,0);
				delay(50000);delay(50000);
				
				if(en_bkl==1) CON_PORT&=~(1<<BKL);
				else CON_PORT|=(1<<BKL);/*backlight control*/
			}
			else if(num<OPTIONS+PARAMETERS)
			{
				
				if(pushtime<10){delay(40000);delay(40000);delay(40000);delay(40000);}
				else if(pushtime<20){delay(40000);delay(40000);delay(40000);}
				else if(pushtime<30){delay(40000);delay(40000);}
				else if(pushtime<50){delay(40000);}
				else if(pushtime<200){delay(10000);}
				else if(pushtime<2000){t*=5;delay(10000);}
				else {t*=50;delay(10000);}
				
				*ep_p[num-OPTIONS]+=t;
			}
			else
			{
				allreset();
				getkey(1,0);
				delay(60000);
				while(1);
			}
		}
		else
		{
			num+=t;
			if(num>OPTIONS+PARAMETERS) num=0;
			else if(num<0) num=OPTIONS+PARAMETERS;
			delay(65000);
			delay(65000);
			delay(65000);
		}
		if((~PIND)&0x0f)
		{
			if(pushtime<65000) pushtime++;
		}
		else
		{
			pushtime=0;
		}
	}
				
}

unsigned char getkey(bool waitrelease,bool waitpush)
{
	unsigned char key;
	key=0;
	do
	{
		key=(~PIND)&0x0f;
	}
	while(waitpush && (key==0));
	
	while(waitrelease && ((~PIND)&0x0f));
	if(key&1) return 4;
	else if(key&2) return 3;
	else if(key&4) return 2;
	else if(key&8) return 1;
	else return 0;
}

void allreset()
{
	cls();
	en_stdby=1;
    en_dir=1;
    en_bkl=1;
    en_motor_pwrsv=0;
	
    stdby_val=100;
    h_speed=100;
    v_speed=1000;
	sys_speed=2000;
    valve_a=70;
    valve_b=10;
	print("Done. press RST.");
}
