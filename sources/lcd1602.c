/////////location//////////
#define RS 1
#define RW 2
#define EN 3
#define BKL 0
#define DAT_DDR (DDRC)
#define DAT_PORT (PORTC)
#define DAT_PIN (PINC)

#define CON_DDR (DDRC)
#define CON_PORT (PORTC)
#define CON_PIN (PINC)

/////edit before use///////
/*
void num2char(uint num,uchar *p,bit adzr);
void printnum(uint num,bit zero);
void print(uchar * printdat);
void locate(uchar lx,bit ly);
void waitbusy();
void cls();
uchar readlcd(bit cmd);
void writelcd(uchar dat,bit cmd,bit tstbusy);
void initlcd();
void delaylcd(uint time);
*/

void initlcd()
{
	DAT_PORT&=~0xf0;
	if(en_bkl) CON_PORT&=~(1<<BKL);
	else CON_PORT|=(1<<BKL);
	CON_DDR|=((1<<RS)|(1<<RW)|(1<<EN)|(1<<BKL));
	delaylcd(10000);
	writelcd(0x28,0,0);//4位数据接口 两行 5*7点阵
	delaylcd(4000);
	writelcd(0x28,0,0);
	delaylcd(4000);
	writelcd(0x28,0,1);
	delaylcd(4000);
	writelcd(0x0c,0,1);//开显示，无光标，不闪烁。0x0c 0x0f
	delaylcd(4000);
	writelcd(0x06,0,1);//输入设置，AC自增，位置不动
	cls();
	
}

void writelcd(uchar dat,bit cmd,bit tstbusy) //0:cmd 1:not cmd
{
	if(tstbusy) waitbusy();
	CON_PORT&=~( (1<<RS) | (1<<RW) | (1<<EN) );
	DAT_DDR|=0xf0;
	DAT_PORT&=~0xf0;
	CON_PORT|=(cmd<<RS);
	//CON_PORT&=~(1<<RW);
	DAT_PORT|=(dat&0xf0);
	CON_PORT|=(1<<EN);
	CON_PORT&=~(1<<EN);
	DAT_PORT&=~0xf0;
	DAT_PORT|=((dat<<4)&0xf0);
	CON_PORT|=(1<<EN);
	CON_PORT&=~(1<<EN);
	DAT_PORT&=~0xf0;
}

uchar readlcd(bit cmd)
{
	 uchar ret;
	 ret=0;
	 CON_PORT&=~( (1<<RS) | (1<<RW) | (1<<EN) );
	 DAT_PORT&=~0xf0;
	 DAT_DDR&=~0xf0;
	 CON_PORT|=(cmd<<RS);
	 CON_PORT|=(1<<RW);
	 CON_PORT|=(1<<EN);
	 ret|=(DAT_PIN&0xf0);
	 CON_PORT&=~(1<<EN);
	 CON_PORT|=(1<<EN);
	 ret|=((DAT_PIN>>4)&0x0f);
	 CON_PORT&=~(1<<EN);
	 return ret;
}

void cls()
{
 	writelcd(1,0,1);
}

void waitbusy()
{
	CON_PORT&=~( (1<<RS) | (1<<RW) | (1<<EN) );
	DAT_PORT&=~0xf0;
	DAT_DDR&=~0xf0;
	CON_PORT|=(1<<RW);
	CON_PORT|=(1<<EN);
	while(DAT_PIN&0x80);
	CON_PORT&=~(1<<EN);
}

void locate(uchar lx,bit ly) //格式(x,y)
{
	 uchar locdat;
	 if(ly) locdat=0x40;
	 else locdat=0;
	 locdat=locdat+lx;
	 locdat|=0x80;
	 writelcd(locdat,0,1);
}

void print(uchar * printdat)
{
	 while(*printdat)
	 {
	  	writelcd(*(printdat++),1,1);
	 }
}

void printnum(uint num,bit zero)
{
	 uchar numdat[6];
	 num2char(num,numdat,zero);
	 print(numdat);
}

void num2char(uint num,uchar *p,bit adzr)//用于10进制数字分解为数组，最大65535 adzr:add zero
{
	 uint mult;
	 mult=10000;
	 while(!(num/mult))
	 {
	  	mult/=10;
	 }
	
	 if(num==0)
	 {
		  *(p++)='0';
		  if(adzr)
		  {
		  	 *(p++)='0';
		  	 *p=0;
		  }
		  else *p=0;
	 }
	
	 else
	 {
		  if(adzr && mult==1){*(p++)=48;}
		  while(1)
		  {
			   *p=(num/mult)+'0';
			   p++;
			   if(mult==1) break;
			   num%=mult;
			   mult/=10;
		  }
		  *p=0;
	 }
}

static void delaylcd(uint time)
{
	while(time--);
}