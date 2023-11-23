#include <xc.h>
#include "cabecera.h"
#include <string.h>
#include "LCD.h"
#include "U1SERIAL.h"

#define _XTAL_FREQ 32000000UL

unsigned char horas=17, minutos=03, segundos=12, centesimas=56;
unsigned char horas2=21, minutos2=22, segundos2=48, centesimas2=56;
unsigned char horas3=21, minutos3=22, segundos3=48, centesimas3=56;
unsigned char horas4=21, minutos4=22, segundos4=48, centesimas4=56;
unsigned char centena, decena, unidad,centena2,decena2,unidad2;
unsigned int lectura =0;
char mode=1,selec=0,sw=0,sw3=0,sw2=0,sw4=0;
char comando_anterior = '\0';  // Variable para almacenar el comando anterior
uint16_t debounce_count = 0;  // Contador para el tiempo de rebote

void configuro(void) {
    //configuracion del oscilador
    OSCCON1 = 0x60;
    OSCFRQ = 0x06;
    OSCEN = 0x40;
    //configuracion de los puertos
    TRISC = 0X00;   //RC salidas
    ANSELC = 0XFE;  //RC digitales
    TRISB = 0XFF;   //RB entrada
    ANSELB = 0X00;  //RB digitales
    WPUB =0X1F;
    //configuraciones de la E/S de servo
    TRISEbits.TRISE0=0; //RE0 salida
    ANSELEbits.ANSELE0=0; //RE0 digital
    TRISA = 0xFF;   //RA entrada
    ANSELA = 0xFF;  //RA analogicos
    TRISD = 0X00;     //RD salidas
    ANSELD = 0X00;    //RD digitales
    //configuraciones de bluetooth
    TRISFbits.TRISF0 = 0;   //RF0 como salida TX
    ANSELFbits.ANSELF0 = 0; //RF0 como digital TX
    TRISFbits.TRISF1 = 1;   //RF1 como entrada RX
    ANSELFbits.ANSELF1 = 0; //RF1 como digital RX
     //configuracion del TMR1
    T1CLK = 0X01;
    T1CON = 0X33;
    //configuracion del CCP1 en comparador evento especial de disparo
    CCP1CON = 0X81;
    CCPR1H = 0X27;
    CCPR1L =0X10;
    //configuracion de las interrupciones
    PIE3bits.CCP1IE = 1;
    INTCON0bits.GIE = 1;
    PIE1bits.INT0IE = 1;        //habilitador del INT0
    PIE6bits.INT1IE = 1;        //habilitador del INT1
    PIE10bits.INT2IE = 1;       //habilitador del INT2
    INTCON0bits.INT0EDG = 0;
    INTCON0bits.INT1EDG = 0;
    INTCON0bits.INT2EDG = 0;
    //ADC
    ADCON0bits.ADFM = 1;          //right justify
    ADCON0bits.CS = 1;          //ADCRC Clock
    //ADPCH = 0x02;               //RA2 is Analog channel
    //ADPCH = 0x01;               //RA1 is Analog channel
    ADPCH = 0x00;               //RA0 is Analog channel
    ADCON0bits.ADON = 1;          //Turn ADC On
    
}


void convierte (unsigned char numero){
    centena = numero /100;
    decena = (numero % 100)/10;
    unidad = numero %10;
}

unsigned int tomamuestra_ADC(void){
    ADCON0bits.GO=1;
    while((ADCON0bits.GO)==1);
    return (ADRESH<<8)+ADRESL;    
}
void delay_ms(unsigned int milliseconds) {
    while (milliseconds--) {
        __delay_ms(1);
    }
}

void puertas(void) {
    unsigned int pulseWidth;

    // Mover el servo a 180 grados
    for (pulseWidth = 1000; pulseWidth <= 2000; pulseWidth += 10) {
        LATEbits.LATE0 = 1;
        delay_ms(pulseWidth);
        LATEbits.LATE0 = 0;
        delay_ms(20000 - pulseWidth);
    }

    // Esperar 10 segundos
    delay_ms(10000);

    // Mover el servo a 0 grados
    for (pulseWidth = 2000; pulseWidth >= 1000; pulseWidth -= 10) {
        LATEbits.LATE0 = 1;
        delay_ms(pulseWidth);
        LATEbits.LATE0 = 0;
        delay_ms(20000 - pulseWidth);
    }
}

void main(void){
    configuro();
    LCD_INIT();
    PORTCbits.RC0=1;
      __delay_ms(200);
    PORTCbits.RC0=0;
    POS_CURSOR(1,0);
    ESCRIBE_MENSAJE2("Reloj:");
    U1_INIT(BAUD9600);
    while(1){ 
       lectura = tomamuestra_ADC();
       POS_CURSOR (1,11);
       LCD_ESCRIBE_VAR_INT(lectura,4);
       POS_CURSOR (2,0);
       convierte(horas);
       ENVIA_CHAR(decena+0x30);
       ENVIA_CHAR(unidad+0x30);
       ENVIA_CHAR(':');
       convierte(minutos);
       ENVIA_CHAR(decena+0x30);
       ENVIA_CHAR(unidad+0x30);
       ENVIA_CHAR(':');
       convierte(segundos);
       ENVIA_CHAR(decena+0x30);
       ENVIA_CHAR(unidad+0x30);
    
  
     
    lectura = tomamuestra_ADC();
    char comando = U1_DATA_RECIEVE();
    POS_CURSOR(1, 7);
    ENVIA_CHAR(comando);
    if (comando != comando_anterior) {
            // Actualizamos el comando anterior
            comando_anterior = comando;

            // Procesamos el comando Bluetooth
            switch (comando) {
                case '1':
                    // Incrementar horas
                    if (horas == 23) {
                        horas = 0;
                    } else {
                        horas++;
                    }
                    break;
                case '2':
                    // Incrementar minutos
                    if (minutos == 59) {
                        minutos = 0;
                    } else {
                        minutos++;
                    }
                    break;
                case '3':
                    mode = 1;
                case '4':
                    // Decrementar horas
                    if (horas == 0) {
                        horas = 23;
                    } else {
                        horas--;
                    }
                    break;
                case '5':
                    // Decrementar minutos
                    if (minutos == 0) {
                        minutos = 59;
                    } else {
                        minutos--;
                    }
                    break;
                case '6':
                    mode = 0;
                // Puedes agregar más casos según tus necesidades
            }
        }

        // Manejar el tiempo de rebote
        if (debounce_count > 0) {
            debounce_count--;
        }
    if(horas==horas2 && minutos==minutos2 && sw3==0){ sw3=1; mode=1;
     for(char f=0;f<=150;f++){
        
        PORTCbits.RC0=1;
        __delay_ms(500);
        PORTCbits.RC0=0;
        
    
        unsigned int x;

        for (x=0;x<250;x++){
            LATEbits.LATE0 = 1;
            __delay_us (500);
            LATEbits.LATE0 = 0;
            __delay_us(19500);
        }
        for (x=0;x<250;x++){
            LATEbits.LATE0 = 1;
            __delay_us (2500);
            LATEbits.LATE0 = 0;
            __delay_us(17500);
            
        }

        break;
     }} 
    
    if(horas==horas3 && minutos==minutos3 && sw2==0){ sw2=1; mode=1;
     for(char f=0;f<=150;f++){
        
        PORTCbits.RC0=1;
        __delay_ms(500);
        PORTCbits.RC0=0;
         
        unsigned int x;

        for (x=0;x<500;x++){
            LATEbits.LATE0 = 1;
            __delay_us (500);
            LATEbits.LATE0 = 0;
            __delay_us(19500);
        }
        for (x=0;x<500;x++){
            LATEbits.LATE0 = 1;
            __delay_us (2500);
            LATEbits.LATE0 = 0;
            __delay_us(17500);
        }

        break;
     }} 
    
    if(horas==horas4 && minutos==minutos4 && sw4==0){ sw4=1; mode=1;
     for(char f=0;f<=150;f++){
        
        PORTCbits.RC0=1;
        __delay_ms(500);
        PORTCbits.RC0=0;
         
        unsigned int x;

        for (x=0;x<750;x++){
            LATEbits.LATE0 = 1;
            __delay_us (500);
            LATEbits.LATE0 = 0;
            __delay_us(19500);
        }
        for (x=0;x<750;x++){
            LATEbits.LATE0 = 1;
            __delay_us (2500);
            LATEbits.LATE0 = 0;
            __delay_us(17500);
            
        }

        break;
     }} 
    
    
    if(PORTBbits.RB4==0 && sw==0){mode=1;sw=1;}
    else if(PORTBbits.RB4==1){sw=0;}
    
     while(mode>0){
     
     if(PORTBbits.RB4==0 && sw==0){mode++;sw=1;}
     else if(PORTBbits.RB4==1){sw=0;}
     
        lectura = tomamuestra_ADC();
        char comando = U1_DATA_RECIEVE();
        if (comando == '6') {
            mode = 0;  // Reinicia
        }
       switch(mode){
         case 1:
             
             if(lectura<800){selec=1;  //cancelar
             
             POS_CURSOR(2,9);
             ESCRIBE_MENSAJE2("Cancel:");} 
             
             else if(lectura<1600){selec=2; //hora
             
             POS_CURSOR(2,9);
             ESCRIBE_MENSAJE2("alarm: ");}
             
             else if(lectura<2400){selec=3;
             
             POS_CURSOR(2,9);
             ESCRIBE_MENSAJE2("mode1: ");}
             
             else if(lectura<3200){selec=4;
                
             POS_CURSOR(2,9);
             ESCRIBE_MENSAJE2("mode2: ");}
             
             else {selec=5; //mode
             
             POS_CURSOR(2,9);
           
             ESCRIBE_MENSAJE2("mode3: ");}
             
   
             break; 
         case 2:
             switch(selec){
                 case(1):
                     mode=0;
                     break;  
                 case(2):    
                     mode=5;
                     break;
                 case (3): 
                     mode=3;
                     break;
                 case (4):
                     mode=7;
                     break;
                 case (5):
                     mode=10;
                     break;
                     
             }
             
             break;
         case (3)://hora1
             segundos=0;
             horas2=(lectura*0.76)/128; 
             POS_CURSOR(2,0);
             convierte(horas2);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30);    
             
             
            
             break;
         case (4): //minutos1
             segundos=0;
             minutos2=(lectura*4.04)/256;
             POS_CURSOR(2,3);
             convierte(minutos2);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30);
             break;
       
            case (5):
               POS_CURSOR(2,0);
             convierte(horas2); 
             ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30); 
            ENVIA_CHAR(':');
             convierte(minutos2);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30);
           
               break; 
            case (6):
               mode = 0;
               sw = 0;
               sw3=0;
               break;
               
            case (7):   //horas2
               segundos=0;
             horas3=(lectura*0.76)/128; 
             POS_CURSOR(2,0);
             convierte(horas3);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30); 
               break;
            case (8):   //minutos2
               segundos=0;
             minutos3=(lectura*4.04)/256;
             POS_CURSOR(2,3);
             convierte(minutos3);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30);
               break;
            case (9):
               mode = 0;
               sw2=0;
               sw=0;
               break;
            case (10):  //horas3
               segundos=0;
             horas4=(lectura*0.76)/128; 
             POS_CURSOR(2,0);
             convierte(horas4);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30); 
               break;
            case (11):  //Minutos3
               segundos=0;
             minutos4=(lectura*4.04)/256;
             POS_CURSOR(2,3);
             convierte(minutos4);
            ENVIA_CHAR(decena+0x30);
            ENVIA_CHAR(unidad+0x30);
               break;
            case (12):
               mode = 0;
               sw4=0;
               sw=0;
               break;
            }  
       }   }
}   

void __interrupt(irq(IRQ_CCP1)) CCP1_ISR(void){
    PIR3bits.CCP1IF = 0;
	if(centesimas == 99){ 
        centesimas = 0;
        if(segundos == 59){
            segundos = 0;
            if(minutos == 59){
                minutos = 0;
                if(horas == 23){
                    horas = 0;
                }
                else{
                    horas++;
                }
            }
            else{
                minutos++;
            }
        }
        else{
            segundos++;
        }
    }
    else{
        centesimas++;
    }
}

void __interrupt(irq(IRQ_INT0)) INT0_ISR(void){
    PIR1bits.INT0IF=0;
    if(horas==23){
        horas=0;
        
    }
    else{
        horas++;
    }
    
    
}

void __interrupt(irq(IRQ_INT1)) INT1_ISR(void){
    PIR6bits.INT1IF=0;
    if(minutos==59){
        minutos=0;
        
    }
    else{
        minutos++;
    }
    
}

void __interrupt(irq(default)) DEFAULT_ISR(void)
{
    PIR10bits.INT2IF = 0;
    segundos = 0;
}