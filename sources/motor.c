#define MOTOR_PORT PORTB
#define MOTOR_PORT_DDR DDRB

/*note: ORC1B:305-1200*/
__flash const unsigned char rotation1[]=
{0x7,0x3,0xb,0x9,0xd,0xc,0xe,0x6};//a-ab-b-bc-c-cd-d-da
__flash const unsigned char rotation2[]=
{0x7,0xb,0xd,0xe,0x7,0xb,0xd,0xe};
const unsigned char __flash *drive;
signed char direction;

void init_tc1()
{
	ICR1=9999;
	OCR1B=RST_elev;//500-1000 1ms-2ms
	TCNT1=0;
	TIFR|=0x3c;
	TIMSK|=0x0c;
	TCCR1A=0x22;//0b 0010 0010;
	TCCR1B=0x1a;//0b 0001 1010;
	DDRD|=(1<<4);
}

void init_s_motor()
{   
	MOTOR_PORT_DDR|=0xf;
	MOTOR_PORT&=0xf0;
	if(en_motor_pwrsv) drive=rotation1;
	else drive=rotation2;
	TIMSK|=2;
	OCR0=(char)h_speed;
	TCCR0=0x08;
	TCNT0=0;
	direction=1;
	
}

#pragma vector=TIMER0_COMP_vect
__interrupt void step_motor()
{
	static signed char c=0,temp=0;
	MOTOR_PORT&=0xf0;
	MOTOR_PORT|=drive[c];
	c+=direction;
	temp+=direction;
	if(c<0)
	{c=7;}
	else if(c>7)
	{c=0;}
	if(temp<-11)
	{
		temp=0;
		if(en_motor_pwrsv)
		{
			azimuth-=1;
		}
		else
		{
			azimuth-=2;
		}
	}
	else if(temp>11)
	{
		temp=0;
		if(en_motor_pwrsv)
		{
			azimuth+=1;
		}
		else
		{
			azimuth+=2;
		}
	}
}

	
void set_elev(unsigned int elev)
{
	if(elev>=MIN_elev && elev<=MAX_elev)
	{
		OCR1B=elev;
	}
}

void sm(unsigned char dir)
{
	switch(dir)
	{
	 case STOP:
		TCCR0&=~0x07;
		MOTOR_PORT&=~0x0f;
		break;
	 case LEFT:
		TCCR0|=0x04;
		direction=1;
		break;
	 case RIGHT:
		TCCR0|=0x04;
		direction=-1;
		break;
	 default:
		break;
	}
}
	