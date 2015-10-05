/****************************************************************************/
/* Supported Microcontroller:RX62T                                          */
/* File contents:   Searching the maximum steering angle from PC            */
/*                  (RX62T version)                                         */
/* Version:     Ver.1.00                                                    */
/* Date:        2013.09.04                                                  */
/* Copyright:   Renesas Micon Car Rally Secretariat                         */
/****************************************************************************/
/*

This is check program to the maximum steering angle of Micon Car.
push the following key. The next movement.

'Z'key     ÅFservo offset value-1
'X'key     ÅFservo offset valueÅ{1
'A'key     ÅFservo offset valueÅ|3
'S'key     ÅFservo offset valueÅ{3


Connection
ÅEtransmission cableÅ©Å®PC(communication soft:TeraTermPro or Hyper terminal)
ÅEP7 and bit0 of P0Å©Motor drive board Ver.5Å©servo
*/

/*======================================*/
/* Include                              */
/*======================================*/
#include "iodefine.h"
#include <machine.h>
#include <stdio.h>
#include "typedefine.h"
#include "printf_lib.h"                 // printf use library

/*======================================*/
/* Symbol definitions                   */
/*======================================*/

/* Constant settings */
#define PWM_CYCLE       24575           // Motor PWM period (16ms)
#define SERVO_CENTER    2300            // Servo center value
#define HANDLE_STEP     13              // 1 degree value

/*======================================*/
/* Prototype declaration                */
/*======================================*/
void init( void );

/*======================================*/
/* Global variable declarations         */
/*======================================*/
int     servo_angle;                    // servo offset

/************************************************************************/
/* Main program                                                         */
/************************************************************************/
void main( void )
{
    int     i;
    char    c;

    /* Initialize MCU functions */
    init();                             // Initialization
    init_sci1_printf( SPEED_9600 );     // SCI1 and printf initialization
    setpsw_i();                         // Ibit=1(permit interrupt),IPL=0
                                        //(processor interrupt prior level=0)
    servo_angle = 0;
    printf(
        "Servo Angle Check Soft\n"
        "'Z' key   : Angle Value -1\n"
        "'X' key   : Angle Value +1\n"
        "\n"
        "'A' key   : Angle Value -3\n"
        "'S' key   : Angle Value +3\n"
        "\n"
    );
    printf( "%3d\r", servo_angle );

    while( 1 ) {
        MTU3.TGRD = SERVO_CENTER - servo_angle * HANDLE_STEP;

        i = get_sci1( &c );
        if( i == 1 ) {
            switch( c ) {
            case 'Z':
            case 'z':
                servo_angle--;
                if( servo_angle < -90 ) servo_angle = -90;
                printf( "%3d\r", servo_angle );
                break;

            case 'X':
            case 'x':
                servo_angle++;
                if( servo_angle > 90 ) servo_angle = 90;
                printf( "%3d\r", servo_angle );
                break;

            case 'A':
            case 'a':
                servo_angle -= 3;
                if( servo_angle < -90 ) servo_angle = -90;
                printf( "%3d\r", servo_angle );
                break;

            case 'S':
            case 's':
                servo_angle += 3;
                if( servo_angle > 90 ) servo_angle = 90;
                printf( "%3d\r", servo_angle );
                break;

            default:
                break;
            }
        }
    }
}

/************************************************************************/
/* RX62T  Initialization                                                */
/************************************************************************/
void init( void )
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
    PORTD.DDR.BYTE = 0x07;                  //PD7:TRST#(i)
                                            //PD5:TDI(i)
                                            //PD4:TCK(i)
                                            //PD3:TDO(o)
                                            //CN:PD2-PD0
    PORTE.DDR.BYTE = 0x1b;                  //PE5:SW(i)
                                            //CN:PE4-PE0

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
    MTU3.TMDR1.BYTE  = 0x38;                 //TGRC,TGRD buffer function
                                            //PWM mode synchronized by RESET
    MTU4.TMDR1.BYTE  = 0x00;                 //Set 0 to exclude MTU3 effects
    MTU.TOERA.BYTE  = 0xc7;                 //MTU3TGRB,MTU4TGRA,MTU4TGRB permission for output

    MTU.TSTRA.BYTE  = 0x40;                 //MTU0,MTU3 count function
}

/************************************************************************/
/* end of file                                                          */
/************************************************************************/
