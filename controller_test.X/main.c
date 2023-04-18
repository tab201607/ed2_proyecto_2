/*
 * File:   main.c
 * Author: kirby
 *
 * Created on April 14, 2023, 11:23 AM
 */


#pragma config FOSC = INTRC_NOCLKOUT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)



#include <xc.h>
#include "adclib.h"

char xchannel = 6;
char ychannel = 7;
char up = 0, right = 0, down = 0, left = 0, button = 0;
char x, y;
char datastream = 0;

void main(void) {
    
    TRISE = 0x07; //configurando e0, e1, y e2 como inputs
    PORTE = 0;
    TRISD = 0;
    PORTD = 0;
    
    //Configurar adc
    adc_setup(0);  
    adc_channel(xchannel); //RE1 como X
    adc_channel(ychannel); //RE2 como Y
    
    
    while(1)
    {
    x = adc_RO(xchannel);  
    y = adc_RO(ychannel);
    if (x < 70) PORTDbits.RD7 = 1;  else PORTDbits.RD7 = 0;
    if (x > 200) PORTDbits.RD6 = 1; else PORTDbits.RD6 = 0;
    if (y <70) PORTDbits.RD5 = 1;  else PORTDbits.RD5 = 0;
    if (y > 200) PORTDbits.RD4 = 1; else PORTDbits.RD4 = 0;
    if (PORTEbits.RE0 == 0) PORTDbits.RD3 = 1; else PORTDbits.RD3 = 0;
    }
    
}
