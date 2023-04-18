/*
 * File:   adc.c
 * Author: jacob t
 *
 * Created on February 10, 2023, 11:25 AM
 */

#define _XTAL_FREQ 4000000
#include "adclib.h"
#include <xc.h>
#include <stdbool.h>

//Inicializa el adc


void adc_setup(bool inter) { 
    if (inter == 1) {
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 0;} 
    else {
    PIE1bits.ADIE = 0;} // Esto decide si se activa el interrupt
    
    ADCON1bits.ADFM = 0; 
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;   
    
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    ADCON0bits.ADCS = 1; 
    ADCON0bits.CHS = 0;  
    __delay_us(100);
    ADCON0bits.ADON = 1;  
    __delay_us(100);
}

//Funcion de seleccion de canal. El interrupt siempre esta prendido


void adc_channel (int channel) {
    switch (channel)
    {case 0: 
        ANSELbits.ANS0 = 1;
        TRISAbits.TRISA0 = 1;
        break;
    case 1:
        ANSELbits.ANS1 = 1;
        TRISAbits.TRISA1 = 1;
        break;
    case 2:
        ANSELbits.ANS2 = 1;
        TRISAbits.TRISA2 = 1;
        break;
    case 3:
        ANSELbits.ANS3 = 1;
        TRISAbits.TRISA3 = 1;
        break; 
    case 4:
        ANSELbits.ANS4 = 1;
        TRISAbits.TRISA5 = 1;
        break;  
    case 5:
        ANSELbits.ANS5 = 1;
        TRISEbits.TRISE0 = 1;
        break;  
    case 6:
        ANSELbits.ANS6 = 1;
        TRISEbits.TRISE1 = 1;
        break;  
    case 7:
        ANSELbits.ANS7 = 1;
        TRISEbits.TRISE2 = 1;
        break;   
    case 8:
        ANSELHbits.ANS8 = 1;
        TRISBbits.TRISB2 = 1;
        break;  
    case 9:
        ANSELHbits.ANS9 = 1;
        TRISBbits.TRISB3 = 1;
        break;  
    case 10:
        ANSELHbits.ANS10 = 1;
        TRISBbits.TRISB1 = 1;
        break; 
    case 11:
        ANSELHbits.ANS11 = 1;
        TRISBbits.TRISB4 = 1;
        break; 
    case 12:
        ANSELHbits.ANS12 = 1;
        TRISBbits.TRISB0 = 1;
        break; 
    case 13:
        ANSELHbits.ANS13 = 1;
        TRISBbits.TRISB5 = 1;
        break; 
    default:
        break;
    } }
    

//Empieza una lectura del adc
void adc_read (char channel) {
    ADCON0bits.CHS = channel;
    ADCON0bits.GO = 1;
}

// Da un valor y resetee el interrupt del adc

char adc_output (void) {
    if (PIR1bits.ADIF == 1) {
        PIR1bits.ADIF = 0;
    }
    return ADRESH;
}

char adc_RO (char channel) {
    adc_read(channel);
    __delay_us(50);
    return adc_output();
}

float adc_volt (char channel) {
    int out;
    float final;
    out = (int)adc_RO(channel);
    final = (float)out * 5 / 256;
    return final;
}

int adc_mv (char channel) {
    int out;
    int final;
    out = adc_RO(channel);
    final = out * 5000 / 255;
    return final;
}