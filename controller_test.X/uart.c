/*
 * File:   uart.c
 * Author: JTP
 *
 * Created on February 19, 2023, 6:34 PM
 */


#include <xc.h>
#include "uart.h"


void UART_RX_config (uint16_t baudrate) {
    TXSTAbits.SYNC = 0; //Lo pone en modo asincrono
    
    //control baudrate
    TXSTAbits.BRGH = 1; //baud rate alto
    BAUDCTLbits.BRG16 = 1; //definicion de baud rate es de 16 digitos
    SPBRG = (char)(baudrate & 0xFF);
    SPBRGH = (char)(baudrate >> 8);
    
    RCSTAbits.SPEN = 1; //configura tx y rx como entrada y salida
    RCSTAbits.RX9 = 0; //RX solo es de 8 digitos
    RCSTAbits.CREN = 1; //Activa la funcion de recibir
    RCSTAbits.FERR = 0;     // Disable framing error
    RCSTAbits.OERR = 0;
};

void UART_TX_config (uint16_t baudrate) {
    TXSTAbits.SYNC = 0; //Lo pone en modo asincrono
    
    //control baudrate
    TXSTAbits.BRGH = 1; //baud rate alto
    BAUDCTLbits.BRG16 = 1; //definicion de baud rate es de 16 digitos
    SPBRG = (char)baudrate & 0x00FF;
    SPBRGH = (char)baudrate >> 8;
    
    RCSTAbits.SPEN = 1; //configura tx y rx como entrada y salida
    RCSTAbits.RX9 = 0; //RX solo es de 8 digitos
    RCSTAbits.CREN = 1; //Activa la funcion de recibir
    
    TXSTAbits.TXEN = 1;
};

void UART_write_char (int character){ 
  while (TXSTAbits.TRMT == 0);
  TXREG = character;
};

int UART_read_char() {
    while (PIR1bits.RCIF == 0);
    PIR1bits.RCIF = 0;
    return RCREG;
};

/*void UARTSendString(char* str, char uint8_t max_length) {
    int i = 0;
    for (i=0 ; i<  max_length && str[i]!='\0' ; i++) {
        UART_write_char(str[i]);
    }
}*/

/*uint8_t UARTReadString(char* buf, uint8_t max_length) {
    uint8_t i = 0;
    char tmp = 1;
    for (i=0 ; i<max_length-1 ; i++) {
        tmp = UART_read_char();
        // Stop reading if end of string is read
        if (tmp == '\0' || tmp == '\n' || tmp == '\r') {
            break;
        }
        buf[i] = tmp;
    }
    
    buf[i+1] = '\0';
    
    return i;
}*/
/*
void UARTInit(const uint32_t baud_rate, const uint8_t BRGH) {
    // Calculate BRG
    if (BRGH == 0) {
        SPBRG = __XTAL_FREQ/(64*baud_rate) - 1;
        TXSTAbits.BRGH = 0;
    } else {
        SPBRG = __XTAL_FREQ/(16*baud_rate) - 1;
        TXSTAbits.BRGH = 1;
    }
   
    // TXSTA register
    TXSTAbits.TX9 = 0;      // 8-bit transmission
    TXSTAbits.TXEN = 1;     // Enable transmission
    TXSTAbits.SYNC = 0;     // Asynchronous mode
    
    // RCSTA register
    RCSTAbits.SPEN = 1;     // Enable serial port
    RCSTAbits.RX9 = 0;      // 8-bit reception
    RCSTAbits.CREN = 1;     // Enable continuous reception
    RCSTAbits.FERR = 0;     // Disable framing error
    RCSTAbits.OERR = 0;     // Disable overrun error
    
    // Set up direction of RX/TX pins
    UART_TRIS_RX = 1;
    UART_TRIS_TX = 0;
} */

