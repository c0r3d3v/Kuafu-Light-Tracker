bool flag_h=0;
bool flag_v=0;

char num;
void init_adc()
{
	DDRA=0x20;
	PORTA=0x20;
	ADMUX=0x40;
	SFIOR&=~0xe0;
	num=0;
	trans_ok=0;
	ADCSRA=0xcd;
}

#pragma vector=ADC_vect
__interrupt void adc_int()
{
	adcdat[num]=ADC;
	if(num >= 4)
	{
		num=0;
		ADMUX&=~0x1f;
		ADMUX|=num;
		trans_ok=1;
	}
	else
	{
		num++;
		ADMUX&=~0x1f;
		ADMUX|=num;
		ADCSRA|=0x40;
	}
}

void turn()
{
	static bool d_trig=0;
	if( (flag_h || compare(L_VAL,R_VAL,valve_a)!=0) && !d_trig)
	{
		flag_h=1;
		switch( compare(L_VAL,R_VAL,valve_b) )
		{
		 case 0:
			sm(STOP);flag_h=0;break;
		 case 1:
			sm(LEFT);break;
		 case 2:
			sm(RIGHT);break;
		 default:
			cls();
			print("err:compare v.");
			while(1);
		}
	}
	else if( (flag_v ^ flag_h) && (elevation==MID_elev) )
	{
		if(flag_v)
		{
			if( abs(azimuth+MAX_dire)< abs(azimuth-MAX_dire) )
			{
				sm(LEFT);
			}
			else
			{
				sm(RIGHT);
			}
		}
	}
	if((d_trig || (abs(azimuth)>MAX_dire)) && en_dir)
	{
		if(!d_trig)
		{
			d_trig=1;
			if(azimuth >0)
			{
				sm(RIGHT);
			}
			else
			{
				sm(LEFT);
			}
		}
		if( (compare(L_VAL,R_VAL,valve_b)==0) ) 
		{
			sm(STOP);
			d_trig=0;
		}
	}
	
	
	if( flag_v || compare(U_VAL,D_VAL,valve_a)!=0 )
	{
		flag_v=1;
		switch( compare(U_VAL,D_VAL,valve_b) )
		{
		 case 0:
			flag_v=0;break;
		 case 1:
			if(elevation < MID_elev)
			{
			 	set_elev(++elevation);
			}
			break;
			
		 case 2:
			if(elevation > MIN_elev)
			{
				set_elev(--elevation);
			}
			break;
			
		 default:
			cls();
			print("err:compare h.");
			while(1);
		}
		delay(v_speed);
	}
	
}

char compare(int a,int b,char los) //bigger 1,smaller 2, equal 0
{
	if(abs(a-b) <= los) return 0;
	else if(a>b) return 1;
	else return 2;
}