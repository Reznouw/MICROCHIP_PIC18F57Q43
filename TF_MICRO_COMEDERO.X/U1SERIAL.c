#include <xc.h>
#include <string.h>
#include "U1SERIAL.h"

void U1_INIT(unsigned char baudios){
    //configuracion del modulo UART
    TRISFbits.TRISF0 = 0;   //RF0 como salida TX
    ANSELFbits.ANSELF0 = 0; //RF0 como digital TX
    TRISFbits.TRISF1 = 1;   //RF1 como entrada RX
    ANSELFbits.ANSELF1 = 0; //RF1 como digital RX
    U1BRGH = 0;
    U1BRGL = baudios;           //U1 trabaje a 9600 baudios y BRGS=0
    U1CON0 = 0x30;              //BRGS=0, TX and RX enabled, async 8 bit mode
    U1CON1 = 0x80;              //U1 enabled
    U1CON2 = 0x00;              //valores por defecto
    RF0PPS = 0x20;              //RF0 conectado al TX del UART1
    U1RXPPS = 0x29;   //RF1 conectado al RX del UART1   
}

char U1_DATA_RECIEVE(){
    return U1RXB;
    
}


void U1_DATA_SEND(unsigned char dato){
    U1TXB = dato;
    while(U1ERRIRbits.TXMTIF == 0);
}

void U1_STRING_SEND(const char *cadena)
{
	unsigned char tam;
    unsigned char i = 0;
    tam = strlen(cadena);
	for(i = 0; i<tam; i++)
	{
        U1TXB = cadena[i];
        while(U1ERRIRbits.TXMTIF == 0);
	}
}

void U1_NEWLINE(void){
    U1_DATA_SEND(0x0A);
    U1_DATA_SEND(0x0D);
}

void U1_ESCRIBE_VAR_CHAR(unsigned char numero, unsigned char n_digitos){
    unsigned char centena, decena, unidad;  //variables de funcion convierte
    centena = (numero % 1000) / 100;
    decena =  (numero % 100) / 10;
    unidad = numero % 10;
    switch(n_digitos){
        case 1:
            U1_DATA_SEND(unidad+0x30);
            break;
        case 2:
            U1_DATA_SEND(decena+0x30);
            U1_DATA_SEND(unidad+0x30);
            break;
        case 3:
            U1_DATA_SEND(centena+0x30);
            U1_DATA_SEND(decena+0x30);
            U1_DATA_SEND(unidad+0x30);
            break;
    }
}

void U1_ESCRIBE_VAR_INT(unsigned int numero, unsigned char n_digitos){
    unsigned char d_millar, millar, centena, decena, unidad;  //variables de funcion convierte
    d_millar = numero / 10000;
    millar = (numero % 10000) / 1000;
    centena = (numero % 1000) / 100;
    decena =  (numero % 100) / 10;
    unidad = numero % 10;
    switch(n_digitos){
        case 1:
            U1_DATA_SEND(unidad+0x30);
            break;
        case 2:
            U1_DATA_SEND(decena+0x30);
            U1_DATA_SEND(unidad+0x30);
            break;
        case 3:
            U1_DATA_SEND(centena+0x30);
            U1_DATA_SEND(decena+0x30);
            U1_DATA_SEND(unidad+0x30);
            break;
        case 4:
            U1_DATA_SEND(millar+0x30);
            U1_DATA_SEND(centena+0x30);
            U1_DATA_SEND(decena+0x30);
            U1_DATA_SEND(unidad+0x30);
            break;
        case 5:
            U1_DATA_SEND(d_millar+0x30);
            U1_DATA_SEND(millar+0x30);
            U1_DATA_SEND(centena+0x30);
            U1_DATA_SEND(decena+0x30);
            U1_DATA_SEND(unidad+0x30);
            break;
    }
}