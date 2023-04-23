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
#include "uart.h"

char xchannel = 6;
char ychannel = 7;
__bit up = 0, right = 0, down = 0, left = 0, button = 0;
char x, y;
char datastream = 0;

void main(void) {
    
    TRISE = 0x07; //configurando e0, e1, y e2 como inputs
    PORTE = 0;
    TRISD = 0; 
    PORTD = 0;
    TRISB = 0;
    
    OSCCON = 0x75;
    
    //Configurar adc
    adc_setup(0);  
    adc_channel(xchannel); //RE1 como X
    adc_channel(ychannel); //RE2 como Y
    
    //UART config
    UARTInit(9600, 1);
    
    while(1)
    {
    x = adc_RO(xchannel); //Tomar valores de X y Y 
    y = adc_RO(ychannel);
    //Asignar diferentes estados dependiendo de esos valores
    if (x < 70) left = 1;  else left = 0;
    if (x > 200) right = 1; else right = 0;
    if (y < 70) down = 1;  else down = 0;
    if (y > 200) up = 1; else up = 0;
    button = !PORTEbits.RE0;
    
    //para revisar que señales estan funcionando
    PORTDbits.RD7 = up;
    PORTDbits.RD6 = down;
    PORTDbits.RD5 = right;
    PORTDbits.RD4 = left;
    PORTDbits.RD3 = button; 
    
    char datastreamtemp = (right + (up * 2) + (left * 4) + (down * 8) + (button * 16));
    
    
    if (datastreamtemp != datastream && datastreamtemp != 0) {
        UARTSendChar(datastreamtemp);
    }
    datastream = datastreamtemp;
    __delay_ms(10);
   
    }}
    
    void __interrupt() Isr(void) 
    {
        if (RCIF){
            char nothing = UARTReadChar();

            PIR1bits.RCIF = 0;
        }
    
    };
    
/*
   void __interrupt() Isr(void) {
   char uart_return = UART_read_char();
   UART_write_char(datastream);
   __delay_ms(10);}*/