/*-------------------public----------------------*/
typedef unsigned char uchar;
typedef unsigned int uint;
typedef bool bit;
typedef bool status;

#define SET(reg,pos) (reg|= (1 << (pos)))
#define CLR(reg,pos) (reg&=~(1 << (pos)))
#define TST(reg,pos) (reg&( 1 << (pos)))

#define MIN_elev 370
#define MAX_elev 1150
#define MID_elev 1000
#define RST_elev 570
#define STOP 1
#define LEFT 2
#define RIGHT 3
#define STEP_ANGLE 5.625
#define MAX_dire 270

#define L_VAL (adcdat[2])
#define R_VAL (adcdat[0])
#define U_VAL (adcdat[3])
#define D_VAL (adcdat[1])
#define F_VAL (adcdat[4])
#define START_VAL 80
#define LOSS_A 70
#define LOSS_B 10



#define REFRESH 20

#define OPTIONS 4
#define PARAMETERS 6

#define BAUD 20

#define ELEVATION 1
#define AZIMUTH 2
#define ENPWRSV 3
#define POWEROFF 0xfe
#define ULEFT 0x2000
#define URIGHT 0x3000
#define USTOP 0x4000




///////////////////////////master private//////////////////////////////
#ifdef MASTER

#define ENABLE_KEY DDRB|=(1<<PB4)
#define DISABLE_KEY DDRB&=~(1<<PB4)
int elevation=RST_elev;
int azimuth;
unsigned int adcdat[5];
uchar last_dir;
bool trans_ok,
     stdby=0,
	 uarterr=0;
//options list
__root __eeprom uchar en_stdby=1;
__root __eeprom uchar en_dir=1;
__root __eeprom uchar en_bkl=0;
__root __eeprom uchar en_motor_pwrsv=1;
//parameter list
__root __eeprom uint stdby_val=100;
__root __eeprom uint h_speed=100;
__root __eeprom uint v_speed=1000;
__root __eeprom uint sys_speed=2000;
__root __eeprom uint valve_a=70;
__root __eeprom uint valve_b=10;

uint __eeprom *ep_p[]={&stdby_val,&h_speed,&v_speed,&sys_speed,&valve_a,&valve_b};
uchar __eeprom *ep_o[]={&en_stdby,&en_dir,&en_bkl,&en_motor_pwrsv,};
		
__flash uchar *menu[]=
{
	"Set Standby:",
	"Direction Limit:",
	"Backlight:",
	"Less MotorPower:",
	
	"Standby Valve:",
	"HorizontalSpeed:",
	"Vertical Speed:",
	"System Speed",
	"Valve A:",
	"Valve B:",
	
	"ALL RESET?",
};
//light.c
void turn();
char compare(int a,int b,char los);

//lcd1602.c
void num2char(uint num,uchar *p,bit adzr);
void printnum(uint num,bit zero);
void print(uchar * printdat);
void locate(uchar lx,bit ly);
void waitbusy();
void cls();
uchar readlcd(bit cmd);
void writelcd(uchar dat,bit cmd,bit tstbusy);
void initlcd();
void delay(uint time);
static void delaylcd(uint time);

//main.c
void disp_dat();
void settings();
unsigned char getkey(bool waitrelease,bool waitpush);
void allreset();

//uart_m.c
void senduart(int data,uchar data2);
void inituart();

#endif
/////////////////////////////////////////////////////////////

///////////////////////////////slave private////////////////////////////////
#ifdef SLAVE
bool enset=0;
int elevation;
int azimuth;
int el;
uchar udir;

uint stdby_val=100;
uint h_speed=100;
uint v_speed=1000;

uchar en_dir=1;
uchar en_motor_pwrsv=1;

//uart_s.c
void configure(int rtemp,uchar p);

#endif
///////////////////////////////////////////////////////////////

//motor.c
void init_s_motor();
void init_adc();

//all
void delay(uint i)
{
	while(i--);
}