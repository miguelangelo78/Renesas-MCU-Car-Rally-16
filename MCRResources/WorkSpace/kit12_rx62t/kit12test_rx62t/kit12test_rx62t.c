/***********************************************************************/
/*  Supported Microcontroller:RX62T                                    */
/*  File:                   kit12test_rx62t.c                          */
/*  File Contents:          Operation Test Program(RX62T version)      */
/*  Version number:         Ver.1.00                                   */
/*  Date:                   2013.09.01                                 */
/*  Copyright:              Renesas Micom Car Rally Secretariat        */
/***********************************************************************/
/*
This program supports the following boards:
* RMC-RX62T board
* Sensor board Ver. 5
* Motor drive board Ver. 5
*/

/*
operation check Micon car kit sensor board Ver.5 and Motor drive board 
Ver.5

change content of operation check by dip switch of micon board
   DipSW
bit3 2 1 0
   0 0 0 0 check LED           LED on alternately intervals of 0.5 seconds 
   0 0 0 1 check push switch   OFF：LED0 ON  ON：LED1ON
   0 0 1 0 check servo         repeat 0°→right30°→left30°
   0 0 1 1 no operation
   0 1 0 0 check right motor   repeat forward  → brake
   0 1 0 1                     repeat backward → brake
   0 1 1 0 check left motor    repeat forward  → brake
   0 1 1 1                     repeat backward → brake

   1 0 0 0 check sensor        output sensor bit1,0 to LED1,0
   1 0 0 1                     output sensor bit3,2 to LED1,0
   1 0 1 0                     output sensor bit5,4 to LED1,0
   1 0 1 1                     output sensor bit7,6 to LED1,0

   1 1 0 0 check straightness  PWM forward at 50%  stop agter 2 seconds
   1 1 0 1 check straightness  PWM forward at 50%  stop after 5 seconds
   1 1 1 0 check straightness  PWM forward at 100% stop after 2 seconds
   1 1 1 1 check straightness  PW  forward at 100% stop after 5 seconds
*/

/*======================================*/
/* Include                              */
/*======================================*/
#include "iodefine.h"

/*======================================*/
/* Symbol definitions                   */
/*======================================*/

/* Constant settings */
#define PWM_CYCLE       24575           /* Motor PWM period (16ms)     */
#define SERVO_CENTER    2300            /* Servo center value          */
#define HANDLE_STEP     13              /* 1 degree value              */

/*======================================*/
/* Prototype declarations               */
/*======================================*/
void init(void);
unsigned char sensor_inp( unsigned char mask );
unsigned char dipsw_get( void );
unsigned char buttonsw_get( void );
unsigned char pushsw_get( void );
void led_out_m( unsigned char led );
void led_out( unsigned char led );
void motor( int accele_l, int accele_r );
void handle( int angle );

/*======================================*/
/* Global variable declarations         */
/*======================================*/
unsigned long   cnt0;
unsigned long   cnt1;
int             pattern;

/***********************************************************************/
/* Main program                                                        */
/***********************************************************************/
void main(void)
{
    unsigned char   now_sw;             /* memorize dip switch now      */
    unsigned char   before_sw;          /* memorize dip switch last time*/
    unsigned char   c;                  /* for work                     */

    /* Initialize MCU functions */
    init();

    /* Initialize micom car state */
    handle( 0 );
    motor( 0, 0 );
    led_out( 0x0 );

    /* variable initialization */
    before_sw = dipsw_get();
    cnt1 = 0;

    while( 1 ) {
    /* read dip switch */
    now_sw = dipsw_get();

    /* comparing with switch at last time */
    if( before_sw != now_sw ) {
        /* mismatch →　update value at last time, clear timer value */
        before_sw = now_sw;
        cnt1 = 0;
    }

    /* choose operation check mode by value of dip switch */
    switch( now_sw ) {

        /* check LED LED on alternately intervals of 0.5 seconds */
        case 0:
            if( cnt1 < 500 ) {
                led_out( 0x1 );
            } else if( cnt1 < 1000 ) {
                led_out( 0x2 );
            } else {
                cnt1 = 0;
            }
            break;

        /* check push switch   OFF：LED0 ON  ON：LED1ON */
        case 1:
            led_out( pushsw_get() + 1 );
            break;

        /* check servo   repeat 0°→right30°→left30° */
        case 2:
            if( cnt1 < 1000 ) {
                handle( 0 );
            } else if( cnt1 < 2000 ) {
                handle( 30 );
            } else if( cnt1 < 3000 ) {
                handle( -30 );
            } else {
                cnt1 = 0;
            }
            break;

        /* not do anything */
        case 3:
            break;

        /* check right motor   repeat forward  → brake */
        case 4:
            if( cnt1 < 1000 ) {
                motor( 0, 100 );
            } else if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else {
                cnt1 = 0;
            }
            break;

        /* check right motor   repeat backward  → brake */
        case 5:
            if( cnt1 < 1000 ) {
                motor( 0, -100 );
            } else if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else {
                cnt1 = 0;
            }
            break;

        /* check left motor    repeat forward  → brake */
        case 6:
            if( cnt1 < 1000 ) {
                motor( 100, 0 );
            } else if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else {
                cnt1 = 0;
            }
            break;

        /* check left motor    repeat backward  → brake */
        case 7:
            if( cnt1 < 1000 ) {
                motor( -100, 0 );
            } else if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else {
                cnt1 = 0;
            }
            break;

        /* check sensor   output sensor bit1,0 to LED1,0 */
        case 8:
            c = sensor_inp( 0x03 );
            led_out( c );
            break;

        /* check sensor   output sensor bit3,2 to LED1,0 */
        case 9:
            c = sensor_inp( 0x0c );
            c = c >> 2;
            led_out( c );
            break;

        /* check sensor   output sensor bit5,4 to LED1,0 */
        case 10:
            c = sensor_inp( 0x30 );
            c = c >> 4;
            led_out( c );
            break;

        /* check sensor   output sensor bit7,6 to LED1,0 */
        case 11:
            c = sensor_inp( 0xc0 );
            c = c >> 6;
            led_out( c );
            break;

        /* check straightness  PWM  forward at 50%  stop agter 2 seconds */
        case 12:
            if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else if( cnt1 < 4000 ) {
                motor( 50, 50 );
            } else {
                motor( 0, 0 );
            }
            break;

        /* check straightness  PWM  forward at 50%  stop agter 5 seconds */
        case 13:
            if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else if( cnt1 < 7000 ) {
                motor( 50, 50 );
            } else {
                motor( 0, 0 );
            }
            break;

        /* check straightness  PWM  forward at 100%  stop agter 2 seconds */
        case 14:
            if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else if( cnt1 < 4000 ) {
                motor( 100, 100 );
            } else {
                motor( 0, 0 );
            }
            break;

        /* check straightness  PWM  forward at 100%  stop agter 5 seconds */
        case 15:
            if( cnt1 < 2000 ) {
                motor( 0, 0 );
            } else if( cnt1 < 7000 ) {
                motor( 100, 100 );
            } else {
                motor( 0, 0 );
            }
            break;

        /* if none */
        default:
            break;
    }
    }
}

/***********************************************************************/
/* RX62T Initialization                                                */
/***********************************************************************/
void init(void)
{
    // System Clock
    SYSTEM.SCKCR.BIT.ICK = 0;               //12.288*8=98.304MHz
    SYSTEM.SCKCR.BIT.PCK = 1;               //12.288*4=49.152MHz

    // Port I/O Settings
    PORT1.DDR.BYTE = 0x03;                  //P10:LED2 in motor drive board

    PORT2.DR.BYTE  = 0x08;
    PORT2.DDR.BYTE = 0x1b;                  //P24:SDCARD_CLK(o)
                                            //P23:SDCARD_DI(o)
                                            //P22:SDCARD_DO(i)
                                            //CN:P21-P20
    PORT3.DR.BYTE  = 0x01;
    PORT3.DDR.BYTE = 0x0f;                  //CN:P33-P31
                                            //P30:SDCARD_CS(o)
    //PORT4:input                           //sensor input
    //PORT5:input
    //PORT6:input

    PORT7.DDR.BYTE = 0x7e;                  //P76:LED3 in motor drive board
                                            //P75:forward reverse signal(right motor)
                                            //P74:forward reverse signal(left motor)
                                            //P73:PWM(right motor)
                                            //P72:PWM(left motor)
                                            //P71:PWM(servo motor)
                                            //P70:Push-button in motor drive board
    PORT8.DDR.BYTE = 0x07;                  //CN:P82-P80
    PORT9.DDR.BYTE = 0x7f;                  //CN:P96-P90
    PORTA.DR.BYTE  = 0x0f;                  //CN:PA5-PA4
                                            //PA3:LED3(o)
                                            //PA2:LED2(o)
                                            //PA1:LED1(o)
                                            //PA0:LED0(o)
    PORTA.DDR.BYTE = 0x3f;                  //CN:PA5-PA0
    PORTB.DDR.BYTE = 0xff;                  //CN:PB7-PB0
    PORTD.DDR.BYTE = 0x0f;                  //PD7:TRST#(i)
                                            //PD5:TDI(i)
                                            //PD4:TCK(i)
                                            //PD3:TDO(o)
                                            //CN:PD2-PD0
    PORTE.DDR.BYTE = 0x1b;                  //PE5:SW(i)
                                            //CN:PE4-PE0

    // Compare match timer
    MSTP_CMT0 = 0;                          //CMT Release module stop state
    MSTP_CMT2 = 0;                          //CMT Release module stop state

    ICU.IPR[0x04].BYTE  = 0x0f;             //CMT0_CMI0 Priority of interrupts
    ICU.IER[0x03].BIT.IEN4 = 1;             //CMT0_CMI0 Permission for interrupt
    CMT.CMSTR0.WORD     = 0x0000;           //CMT0,CMT1 Stop counting
    CMT0.CMCR.WORD      = 0x00C3;           //PCLK/512
    CMT0.CMCNT          = 0;
    CMT0.CMCOR          = 96;               //1ms/(1/(49.152MHz/512))
    CMT.CMSTR0.WORD     = 0x0003;           //CMT0,CMT1 Start counting

    // MTU3_3 MTU3_4 PWM mode synchronized by RESET
    MSTP_MTU            = 0;                //Release module stop state
    MTU.TSTRA.BYTE      = 0x00;             //MTU Stop counting

    MTU3.TCR.BYTE   = 0x23;                 //ILCK/64(651.04ns)
    MTU3.TCNT = MTU4.TCNT = 0;              //MTU3,MTU4TCNT clear
    MTU3.TGRA = MTU3.TGRC = PWM_CYCLE;      //cycle(16ms)
    MTU3.TGRB = MTU3.TGRD = SERVO_CENTER;   //PWM(servo motor)
    MTU4.TGRA = MTU4.TGRC = 0;              //PWM(left motor)
    MTU4.TGRB = MTU4.TGRD = 0;              //PWM(right motor)
    MTU.TOCR1A.BYTE = 0x40;                 //Selection of output level
    MTU3.TMDR1.BYTE = 0x38;                 //TGRC,TGRD buffer function
                                            //PWM mode synchronized by RESET
    MTU4.TMDR1.BYTE = 0x00;                 //Set 0 to exclude MTU3 effects
    MTU.TOERA.BYTE  = 0xc7;                 //MTU3TGRB,MTU4TGRA,MTU4TGRB permission for output

    MTU.TSTRA.BYTE  = 0x40;                 //MTU0,MTU3 count function
}

/***********************************************************************/
/* Interrupt                                                           */
/***********************************************************************/
#pragma interrupt Excep_CMT0_CMI0(vect=28)
void Excep_CMT0_CMI0(void)
{
    cnt0++;
    cnt1++;
}

/***********************************************************************/
/* Sensor state detection                                              */
/* Arguments:       masked values                                      */
/* Return values:   sensor value                                       */
/***********************************************************************/
unsigned char sensor_inp( unsigned char mask )
{
    unsigned char sensor;

    sensor  = ~PORT4.PORT.BYTE;

    sensor &= mask;

    return sensor;
}

/***********************************************************************/
/* DIP switch value read                                               */
/* Return values: Switch value, 0 to 15                                */
/***********************************************************************/
unsigned char dipsw_get( void )
{
    unsigned char sw,d0,d1,d2,d3;

    d0 = ( PORT6.PORT.BIT.B3 & 0x01 );  /* P63~P60 read                */
    d1 = ( PORT6.PORT.BIT.B2 & 0x01 ) << 1;
    d2 = ( PORT6.PORT.BIT.B1 & 0x01 ) << 2;
    d3 = ( PORT6.PORT.BIT.B0 & 0x01 ) << 3;
    sw = d0 | d1 | d2 | d3;

    return  sw;
}

/***********************************************************************/
/* Push-button in MCU board value read                                 */
/* Return values: Switch value, ON: 1, OFF: 0                          */
/***********************************************************************/
unsigned char buttonsw_get( void )
{
    unsigned char sw;

    sw = ~PORTE.PORT.BIT.B5 & 0x01;     /* Read ports with switches    */

    return  sw;
}

/***********************************************************************/
/* Push-button in motor drive board value read                         */
/* Return values: Switch value, ON: 1, OFF: 0                          */
/***********************************************************************/
unsigned char pushsw_get( void )
{
    unsigned char sw;

    sw  = ~PORT7.PORT.BIT.B0 & 0x01;    /* Read ports with switches    */

    return  sw;
}

/***********************************************************************/
/* LED control in MCU board                                            */
/* Arguments: Switch value, LED0: bit 0, LED1: bit 1. 0: dark, 1: lit  */
/*                                                                     */
/***********************************************************************/
void led_out_m( unsigned char led )
{
    led = ~led;
    PORTA.DR.BYTE = led & 0x0f;
}

/***********************************************************************/
/* LED control in motor drive board                                    */
/* Arguments: Switch value, LED0: bit 0, LED1: bit 1. 0: dark, 1: lit  */
/* Example: 0x3 -> LED1: ON, LED0: ON, 0x2 -> LED1: ON, LED0: OFF      */
/***********************************************************************/
void led_out( unsigned char led )
{
    led = ~led;
    PORT7.DR.BIT.B6 = led & 0x01;
    PORT1.DR.BIT.B0 = ( led >> 1 ) & 0x01;
}

/***********************************************************************/
/* Motor speed control                                                 */
/* Arguments:   Left motor: -100 to 100, Right motor: -100 to 100      */
/*        Here, 0 is stopped, 100 is forward, and -100 is reverse.     */
/* Return value:    None                                               */
/***********************************************************************/
void motor( int accele_l, int accele_r )
{
    int    sw_data;

    sw_data = dipsw_get() + 5;
    accele_l = accele_l * sw_data / 20;
    accele_r = accele_r * sw_data / 20;

    /* Left Motor Control */
    if( accele_l >= 0 ) {
        PORT7.DR.BYTE &= 0xef;
        MTU4.TGRC = (long)( PWM_CYCLE - 1 ) * accele_l / 100;
    } else {
        PORT7.DR.BYTE |= 0x10;
        MTU4.TGRC = (long)( PWM_CYCLE - 1 ) * ( -accele_l ) / 100;
    }

    /* Right Motor Control */
    if( accele_r >= 0 ) {
        PORT7.DR.BYTE &= 0xdf;
        MTU4.TGRD = (long)( PWM_CYCLE - 1 ) * accele_r / 100;
    } else {
        PORT7.DR.BYTE |= 0x20;
        MTU4.TGRD = (long)( PWM_CYCLE - 1 ) * ( -accele_r ) / 100;
    }
}

/***********************************************************************/
/* Servo steering operation                                            */
/* Arguments:   servo operation angle: -90 to 90                       */
/*              -90: 90-degree turn to left, 0: straight,              */
/*               90: 90-degree turn to right                           */
/***********************************************************************/
void handle( int angle )
{
    /* When the servo move from left to right in reverse, replace "-" with "+". */
    MTU3.TGRD = SERVO_CENTER - angle * HANDLE_STEP;
}

/***********************************************************************/
/* end of file                                                         */
/***********************************************************************/
