// cmd: Summary(0); Low Freq: FL(1); High Freq: FH(2); Small Period: PS(3);
//      Large Period: PL(4); Short Interval: IS(5); Long Interval: IL(6);
//      Spectrum: SP(7)
#include <p18f452.h>
#include <adc.h>
#include <usart.h>
#include <timers.h>
#include <stdio.h>
#include <spi.h>

#pragma config WDT=OFF
#pragma config OSC=ECIO
#pragma config LVP=OFF

//#define TESTCHIP
//#define TESTSRAM
//#define TESTSPI

signed int optfft(signed int real[256], signed int real2[256], signed int imag[256], signed int imag2[256]);
void delay(unsigned t);
void delay10(unsigned t);
void high_int(void);
void high_isr(void);
void low_int(void);
void low_isr(void);
void rx_handler(void);
void tm0_handler(void);
void iprintln(int i);
void iprint(int i);
void dprintln(double res);
void dprint(double res);
void xprintln(int i);
void initLCD(void);
void LCDputchar(char c);
void LCDconsprint(const rom char *s);
void LCDprint(char *s);
void LCDiprint(int i);
void LCD1st(void);
void LCD2nd(void);
void int0handler(void);
void ave(void);
void writeSRAM(char addr, char c);
unsigned char readSRAM(char addr);
void slaveselect(int s);

int Measure=0;
int mes[7];
int mesresind=0;
double mesres[10];
int sum=0;

#define incmesresind mesresind=((mesresind)+1)%10

#pragma udata gpr1
signed int noiseRawPtr[128];
#pragma udata gpr2
signed int noiseRawPtr2[128];
#pragma udata gpr3
signed int noiseDataPtr[128];
#pragma udata gpr4
signed int noiseDataPtr2[128];

void main(void){

    int result, index=0, j;
    signed int peakFreq;
    char temp;
    char c;

#ifdef TESTSRAM
    int sqfreq=0, t=1;
#endif
    
#ifdef TESTCHIP
//    TRISB=0;
//    PORTB=0;
//    while(1){
//        PORTBbits.RB5 =0;
//        delay(500);
//        PORTBbits.RB5 =1;
//        delay(500);
//    }
    ADCON1bits.PCFG0=0;
    ADCON1bits.PCFG1=1;
    ADCON1bits.PCFG2=1;
    ADCON1bits.PCFG3=1;
    TRISAbits.TRISA4=0;
    while(1){
        PORTAbits.RA2=1;
        delay(100);
        PORTAbits.RA2=0;
        delay(100);
    }
#endif

#ifdef TESTSPI
    TRISB=0;
    PORTB=0;
    INTCONbits.GIEH = 1;
    /*********************************UART************************************/
    TRISCbits.TRISC6=1; //?
    /*
    * Open the USART configured as 8N1, 2400 baud, in polled mode
    */
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE &
            USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 129);
    /* Display a prompt to the USART */
    putrsUSART ((const far rom char *)"\n\rUART IS READY!\n\r");

    /* Enable interrupt priority */
    RCONbits.IPEN = 1;

    /* Make receive interrupt high priority */
    IPR1bits.RCIP = 1;

    /*********************************UART************************************/

    ADCON1bits.PCFG0=0;
    ADCON1bits.PCFG1=1;
    ADCON1bits.PCFG2=1;
    ADCON1bits.PCFG3=1;
    TRISAbits.TRISA2=0;
    TRISAbits.TRISA3=0;
    TRISAbits.TRISA4=0;
    TRISAbits.TRISA5=0;
    TRISCbits.TRISC3=0; //SCK
    TRISCbits.TRISC4=1; //SDI
    TRISCbits.TRISC5=0; //SDO
    OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);
//    EnableIntSPI;
//    IPR1bits.SSPIP = 1;


    slaveselect(1);

    while(1){
        PORTAbits.SS=1;
        PORTAbits.RA2=0;
        delay(500);
        PORTAbits.RA2=1;
        delay(500);
        c=ReadSPI();
        WriteUSART(c);
//        PORTAbits.SS=0;
//        WriteSPI('A');
//        c=SSPBUF;
//        delay(500);
//        WriteSPI('B');
//        c=SSPBUF;
//        delay(500);
//        WriteSPI('C');
//        c=SSPBUF;
//        delay(500);
//        WriteSPI('D');
//        c=SSPBUF;
    }
#endif

    
    INTCON=0;

    /*********************************ADC*************************************/
    ADCON1bits.PCFG0=0;
    ADCON1bits.PCFG1=1;
    ADCON1bits.PCFG2=1;
    ADCON1bits.PCFG3=1;
    TRISAbits.TRISA0=1; // pin A0 read
    // Setup A/D converter
    OpenADC(ADC_FOSC_4 & ADC_RIGHT_JUST & ADC_1ANA_0REF, ADC_CH0 & ADC_INT_OFF);
    /*********************************ADC*************************************/

    /*********************************UART************************************/
    TRISCbits.TRISC6=1; //?
    /*
    * Open the USART configured as 8N1, 2400 baud, in polled mode
    */
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE &
            USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 129);
    /* Display a prompt to the USART */
    putrsUSART ((const far rom char *)"\n\rUART IS READY!\n\r");

    /* Enable interrupt priority */
    RCONbits.IPEN = 1;

    /* Make receive interrupt high priority */
    IPR1bits.RCIP = 1;
    /*********************************UART************************************/

    /*********************************TIMER***********************************/
    /*
     * Enable the TMR0 interrupt, setting up the timer as an internal
     * 16-bit clock.
     */
    // period: 0.8388608s
    OpenTimer0 (TIMER_INT_ON & T0_SOURCE_INT & T0_16BIT & T0_PS_1_64);
    OpenTimer3 (TIMER_INT_OFF & T3_SOURCE_INT & T3_16BIT_RW & T3_PS_1_8);
    /*********************************TIMER***********************************/

    /*********************************SPI*************************************/
    TRISAbits.TRISA3=0;
    TRISAbits.TRISA4=0;
    TRISAbits.TRISA5=0;
    TRISAbits.TRISA2=0;
    TRISCbits.TRISC3=0; //SCK
    TRISCbits.TRISC4=1; //SDI
    TRISCbits.TRISC5=0; //SDO
    OpenSPI(SPI_FOSC_4, MODE_11, SMPMID);
    /*********************************SPI*************************************/
    
    TRISB=0;
    PORTB=0;
    
    TRISBbits.RB0=1;

    INTCONbits.GIEH=1;

    INTCON1bits.GIEH=1;
    INTCON1bits.INT0E=1;
//    while(1){
//        PORTAbits.RA2=1;
//        delay(100);
//        PORTAbits.RA2=0;
//        delay(100);
//    }

    TRISAbits.RA6=0;
    TRISD=0;
    TRISDbits.RD0=1;
    TRISDbits.RD1=1;
    PORTDbits.RD5=1; // rst counter
    delay(1);
    PORTDbits.RD5=0;

    initLCD();

    for(index=0; index<7; index++){
        mes[index]=0;
    }

#ifdef TESTSRAM

    TRISAbits.RA1=0;
    TRISCbits.RC0=0;
    TRISCbits.RC1=0;
    TRISCbits.RC2=0;
    TRISDbits.RD3=1;
    TRISDbits.RD4=0;
    TRISE=0;

    PORTDbits.RD4=1; // disable
    PORTCbits.RC1=1; // RNW
    PORTCbits.RC2=0; // NOE

    while(1){
        for(j=0; j<256; j++){
            writeSRAM(j, j);
        }

        for(j=0; j<256; j++){
            iprint(j);
            putrsUSART(": ");
            xprintln(readSRAM(j));
            delay(200);
        }
    }
#endif


//    while(1){
//        PORTAbits.RA2=0;
//        WriteSPI('J');
//        c=SSPBUF;
//        delay(100);
//        PORTAbits.RA2=1;
//        delay(100);
//    }
    
    slaveselect(0);
    
    ///WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE///
    ///WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE///
    ///WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE///
    ///WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE///
    ///WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE///
    ///WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE WHILEHERE///
    while (1) {

    /*********************************ADCTEST*********************************/
//        ConvertADC();
//        while(BusyADC());
//        result=(((ReadADC()/341.0*10-15.0)*21.0/10.0)+0.5);
//        //temp=result>>2;
//        //iprintln(temp);
//        iprintln(result);
    /*********************************ADCTEST*********************************/

    /*********************************FFT*************************************/
        if(Measure==7){
            mes[6]++;
            for(index=0; index<128; index++){
                ConvertADC();
                while(BusyADC());
                noiseRawPtr[index]=(ReadADC()-465)/16;
                delay(1);
            }
            for(index=0; index<128; index++){
                ConvertADC();
                while(BusyADC());
                noiseRawPtr2[index]=(ReadADC()-465)/16;
                delay(1);
            }
            for(index=0; index<128; index++){
                //noiseRawPtr[index]=((noiseRawPtr[index]/341.0*10.0-15.0)*21.0/10.0)+0.5;
                noiseDataPtr[index]=0;
                noiseDataPtr2[index]=0;
            }
    //        for(index=0; index<128; index++){
    //            iprintln(noiseRawPtr[index]);delay(2);
    //        }

            peakFreq = optfft(noiseRawPtr, noiseRawPtr2, noiseDataPtr, noiseDataPtr2);

            if(peakFreq<56){
                mesres[mesresind]=peakFreq*18.5;

                // uart
                iprintln(mesres[mesresind]);
                //lcd
                LCD1st();
                LCDconsprint("SPECTRUM        ");
                LCD2nd();
                LCDiprint(mesres[mesresind]);
                LCDconsprint("                ");

                incmesresind;
                ave();
            }
        }
        if(Measure==8 && sum==1){
            sum=0;
            putrsUSART((const far rom char *)"\n\rSUMMARY:\n\rLOW FREQ: ");
            iprint(mes[0]);
            putrsUSART((const far rom char *)"\n\rHIGH FREQ: ");
            iprint(mes[1]);
            putrsUSART((const far rom char *)"\n\rSMALL PERIOD: ");
            iprint(mes[2]);
            putrsUSART((const far rom char *)"\n\rLARGE PERIOD: ");
            iprint(mes[3]);
            putrsUSART((const far rom char *)"\n\rSHORT INTERVAL: ");
            iprint(mes[4]);
            putrsUSART((const far rom char *)"\n\rLONG INTERVAL: ");
            iprint(mes[5]);
            putrsUSART((const far rom char *)"\n\rSPECTRUM: ");
            iprint(mes[6]);
            putrsUSART((const far rom char *)"\n\r");
        }
    /*********************************FFT************************************/

    /*********************************SPI************************************/
////        PORTAbits.SS=0;
////        WriteSPI('J');
////        c=SSPBUF;
////        delay(500);
//
////        PORTAbits.SS=0;
////        while(1){
////            PORTAbits.SS=0;
////            WriteSPI('J');
////            c=SSPBUF;
////            delay(500);
////        }
//
//        WriteSPI('J');
//        c=SSPBUF;
//        delay(500);
////        WriteSPI('J');
////        c=SSPBUF;
////        delay(500);
////        WriteSPI('J');
////        c=SSPBUF;
////        delay(500);
////        WriteSPI('J');
////        c=SSPBUF;
////        delay(500);
//        while(1){
//            c=ReadSPI();
//            WriteUSART(c);
//            delay(100);
//        }
        PORTAbits.RA4=0;
        
        if(Measure>='A' && Measure<='H'){}

        if(Measure=='I' && sum==1){
            sum=0;
            PORTAbits.SS=0;
            PORTAbits.RA4=1;
            WriteSPI(Measure);
            c=SSPBUF;
            delay(500);
            c=0;
            while(c==0){
                c=ReadSPI();
                //WriteUSART(c);
            }
            for(index=0; index<16; index++){
                for(j=0; j<5; ){
                    c=ReadSPI();
                    WriteUSART(c);
                    if(c!=0)
                        j++;
                    delay(100);
                }
                putrsUSART((const far rom char *)"\t");
            }
        }

        if(Measure=='J' && sum==1){
            sum=0;
            PORTAbits.SS=0;
            PORTAbits.RA4=1;
            WriteSPI(Measure);
            c=SSPBUF;
            delay(500);
            c=0;
            while(c==0){
                c=ReadSPI();
                //WriteUSART(c);
            }
            for(index=0; index<7; index++){
                for(j=0; j<5; ){
                    c=ReadSPI();
                    WriteUSART(c);
                    if(c!=0)
                        j++;
                    delay(100);
                }
                putrsUSART((const far rom char *)"\t");
            }
        }
    /*********************************SPI************************************/
    }

}

void ave(){
//    double res;
//    int i;
//    if(mesresind==9){
//        for(i=0; i<10; i++)
//            res+=mesres[i];
//        res/=10.0;
//        putrsUSART((const far rom char *)"AVE OF RECENT 10 MES: ");
//        iprintln(res);
//    }
}

/***********************************SPI***************************************/
void slaveselect(int s){
    switch(s){
        case 0:
            PORTAbits.RA2=0;
            PORTAbits.RA3=0;
            PORTBbits.RB1=0;
            break;
        case 1:
            PORTAbits.RA2=1;
            PORTAbits.RA3=0;
            PORTBbits.RB1=0;
            break;
        case 2:
            PORTAbits.RA2=0;
            PORTAbits.RA3=1;
            PORTBbits.RB1=0;
            break;
        case 3:
            PORTAbits.RA2=1;
            PORTAbits.RA3=1;
            PORTBbits.RB1=0;
            break;
        case 4:
            PORTAbits.RA2=0;
            PORTAbits.RA3=0;
            PORTBbits.RB1=1;
            break;
        case 5:
            PORTAbits.RA2=1;
            PORTAbits.RA3=0;
            PORTBbits.RB1=1;
            break;
        case 6:
            PORTAbits.RA2=0;
            PORTAbits.RA3=1;
            PORTBbits.RB1=1;
            break;
        case 7:
            PORTAbits.RA2=1;
            PORTAbits.RA3=1;
            PORTBbits.RB1=1;
            break;
        default:
            putrsUSART((const far rom char *)"Error: \n\rIllegal Slave!\n\r");
            break;
    }
}
/***********************************SPI***************************************/

/***********************************SRAM**************************************/
void writeSRAM(char addr, char c){
    int index;
    // addr
    PORTCbits.RC2=1;
    PORTDbits.RD4=0; // enable
    for(index=0; index<8; index++){
        PORTAbits.RA1=(addr>>index)&1;
        PORTAbits.RA6=1;
        PORTAbits.RA6=0;
    }
    // data
    for(index=0; index<8; index++){
        PORTCbits.RC0=(c>>index)&1;
        PORTEbits.RE0=1;
        PORTEbits.RE0=0;
    }
    PORTCbits.RC1=0;
    PORTCbits.RC1=1;
}
unsigned char readSRAM(char addr){
    int index;
    unsigned char res;
    PORTDbits.RD4=1; // disable
    PORTCbits.RC2=0;
    PORTCbits.RC1=1;

    PORTEbits.RE0=0;
    // addr
    for(index=0; index<8; index++){
        PORTAbits.RA1=(addr>>index)&1;
        PORTAbits.RA6=1;
        PORTAbits.RA6=0;
    }
    // load
    PORTEbits.RE2=0;
    PORTEbits.RE1=1;
    PORTEbits.RE2=1;
    PORTEbits.RE1=0;
    // data
    res=PORTDbits.RD3;
    for(index=0; index<7; index++){
        PORTEbits.RE0=1;
        res+=(PORTDbits.RD3<<(index+1));
        PORTEbits.RE0=0;
    }
    return res;
}
/***********************************SRAM**************************************/

/***********************************LCD***************************************/
void LCD1st(){
    PORTBbits.RB4=0;
    PORTBbits.RB7=0; //RS
    PORTBbits.RB6=0; //RW

    //L: 0100 1100
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;

    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;

    PORTBbits.RB7=0; //RS
    delay(10);
}

void LCD2nd(){
    PORTBbits.RB4=0;
    PORTBbits.RB7=0; //RS
    PORTBbits.RB6=0; //RW

    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=1;
    PORTBbits.RB3=1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;

    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;

    PORTBbits.RB7=0; //RS
    delay(10);
}

void LCDputchar(char c){
    PORTBbits.RB6=0;
    PORTBbits.RB7=1; //RS

    //L: 0100 1100
//    PORTDbits.RD2=0;
//    PORTBbits.RB5=0;
//    PORTBbits.RB2=1;
//    PORTBbits.RB3=0;

    PORTDbits.RD2=(c>>4)&1;
    PORTBbits.RB5=(c>>5)&1;
    PORTBbits.RB2=(c>>6)&1;
    PORTBbits.RB3=(c>>7)&1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;

//    PORTDbits.RD2=0;
//    PORTBbits.RB5=0;
//    PORTBbits.RB2=1;
//    PORTBbits.RB3=1;

    PORTDbits.RD2=c&1;
    PORTBbits.RB5=(c>>1)&1;
    PORTBbits.RB2=(c>>2)&1;
    PORTBbits.RB3=(c>>3)&1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;

    PORTBbits.RB7=0; //RS
    delay(10);
}

void LCDconsprint(const rom char *s){
    int i;
    for(i=0; s[i] != '\0'; i++)
        LCDputchar(s[i]);
}

void LCDprint(char *s){
    int i;
    for(i=0; s[i] != '\0'; i++)
        LCDputchar(s[i]);
}

void LCDiprint(int i){
    char buf[16];
    sprintf(buf, "%d", i);
    LCDprint(buf);
}

void initLCD(){
    PORTBbits.RB4=0; //enable
    PORTBbits.RB6=0; //RW
    PORTBbits.RB7=0; //RS

    //send1
    PORTDbits.RD2=1;
    PORTBbits.RB5=1;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send2
    PORTDbits.RD2=1;
    PORTBbits.RB5=1;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send3
    PORTDbits.RD2=1;
    PORTBbits.RB5=1;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send4
    PORTDbits.RD2=0;
    PORTBbits.RB5=1;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send5
    PORTDbits.RD2=0;
    PORTBbits.RB5=1;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send6
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send7
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send8
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send9
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send10
    PORTDbits.RD2=1;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send11
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send12
    PORTDbits.RD2=0;
    PORTBbits.RB5=1;
    PORTBbits.RB2=1;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send13
    PORTDbits.RD2=0;
    PORTBbits.RB5=0;
    PORTBbits.RB2=0;
    PORTBbits.RB3=0;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);

    //send14
    PORTDbits.RD2=1;
    PORTBbits.RB5=1;
    PORTBbits.RB2=1;
    PORTBbits.RB3=1;

    PORTBbits.RB4=1;
    PORTBbits.RB4=0;
    delay(50);
}
/***********************************LCD***************************************/

void delay10(unsigned t){
    volatile unsigned counter, i;
    for (i=0; i<t; i++)
        for (counter=0; counter<10; counter++);
}

void delay(unsigned t){
    volatile unsigned counter, i;
    for (i=0; i<t; i++)
        for (counter=0; counter<50; counter++);
}

/***********************************INT***************************************/
//
#pragma code high_interrupt=0x8
void high_int (void)
{
    _asm GOTO high_isr _endasm
    //_asm goto rx_handler _endasm
}
#pragma code
#pragma interrupt high_isr
void high_isr (void){
    if(INTCONbits.TMR0IF){
        tm0_handler();
        INTCONbits.TMR0IF = 0;
    }
    if(PIR1bits.RCIF)
        rx_handler();
    if(INTCON1bits.INT0F){
        int0handler();
        INTCON1bits.INT0F=0;
    }
}

#pragma code low_interrupt=0x18
void low_int (void)
{
}
#pragma code
/***********************************INT***************************************/

/***********************************UART**************************************/
// cmd: Summary(0); Low Freq: FL(1); High Freq: FH(2); Small Period: PS(3);
//      Large Period: PL(4); Short Interval: IS(5); Long Interval: IL(6);
//      Spectrum: SP(7)
#pragma interrupt rx_handler
void rx_handler (void)
{
    char s[3];

    //recv
    //c = ReadUSART();//trans
    getsUSART (s, 2);
    s[3]='\0';
    //putsUSART (s);

    INTCON1bits.INT0E=0;

    switch(s[0]){
        case 'F':
            if(s[1]=='L'){
                Measure=1;
                putrsUSART((const far rom char *)"\n\rLOW FREQ\n\r");
            }
            else if(s[1]=='H'){
                Measure=2;
                putrsUSART((const far rom char *)"\n\rHIGH FREQ\n\r");
            }
            else putrsUSART((const far rom char *)"\n\rERROR: NO CMD\n\r");
            break;
        case 'P':
            if(s[1]=='L'){
                INTCON1bits.INT0E=1;
                Measure=3;
                putrsUSART((const far rom char *)"\n\rLARGE PERIOD\n\r");
            }
            else if(s[1]=='S'){
                Measure=4;
                putrsUSART((const far rom char *)"\n\rSMALL PERIOD\n\r");
            }
            else putrsUSART((const far rom char *)"\n\rERROR: NO CMD\n\r");
            break;
        case 'I':
            INTCON1bits.INT0E=1;
            if(s[1]=='L'){
                Measure=5;
                putrsUSART((const far rom char *)"\n\rLONG INTERVAL\n\r");
            }
            else if(s[1]=='S'){
                Measure=6;
                putrsUSART((const far rom char *)"\n\rSHORT INTERVAL\n\r");
            }
            else putrsUSART((const far rom char *)"\n\rERROR: NO CMD\n\r");
            break;
        case 'S':
            if(s[1]=='P'){
                Measure=7;
                putrsUSART((const far rom char *)"\n\rSPECTRUM\n\r");
            }
            else if(s[1]=='U'){
                Measure=8;
                sum=1;

            }
            else if(s[1]>='0' && s[1]<='7'){
                Measure=9;
                slaveselect(s[1]-'0');
            }
            else putrsUSART((const far rom char *)"\n\rERROR: NO CMD\n\r");
            break;
        case 'R':
            sum=1;
            Measure=s[1];
            if(Measure<'A' || Measure>'J')
                putrsUSART((const far rom char *)"\n\rERROR: NO CMD\n\r");
            break;
        default: putrsUSART((const far rom char *)"\n\rERROR: NO CMD\n\r");
    }

    //trans
    //WriteUSART(c);

//    if(c!=Measure)
//        Measure=c;

    /* Clear the interrupt flag */
    PIR1bits.RCIF = 0;
}
/***********************************UART**************************************/

/***********************************TIMER*************************************/
void tm0_handler(){
    int sqfreq, index, t=1;
    static int state=0;
    PORTAbits.RA6=0;

    ////////////////////LOW FREQ MODE///////////////////
    if(Measure==1){
        mes[0]++;
        // load
        state++;
        if(state==3){
            state=0;
            PORTDbits.RD6=0;
            delay(t);
            PORTDbits.RD7=1;
            delay(t);
            PORTDbits.RD6=1;
            delay(t);
            PORTDbits.RD7=0;
            delay(t);

            sqfreq=PORTDbits.RD0;
            for(index=0; index<7; index++){
                PORTAbits.RA6=1;
                delay(t);
                sqfreq+=(PORTDbits.RD0<<(index+1));
                PORTAbits.RA6=0;
                delay(t);
            }
            //xprintln(sqfreq);
            mesres[mesresind]=sqfreq*11.9;
            // uart
            iprintln(mesres[mesresind]);
            //lcd
            LCD1st();
            LCDconsprint("LOW FREQ        ");
            LCD2nd();
            LCDiprint(mesres[mesresind]);
            LCDconsprint("                ");
            incmesresind;
            ave();
        }
    }

    ////////////////////HIGH FREQ MODE///////////////////
    else if(Measure==2){
        mes[1]++;
        // load
        PORTDbits.RD6=0;
        delay(t);
        PORTDbits.RD7=1;
        delay(t);
        PORTDbits.RD6=1;
        delay(t);
        PORTDbits.RD7=0;
        delay(t);

        sqfreq=PORTDbits.RD1;
        for(index=0; index<7; index++){
            PORTAbits.RA6=1;
            delay(t);
            sqfreq+=(PORTDbits.RD1<<(index+1));
            PORTAbits.RA6=0;
            delay(t);
        }
        //xprintln(sqfreq);
        mesres[mesresind]=sqfreq*5.3;
        // uart
        iprintln(mesres[mesresind]);
        //lcd
        LCD1st();
        LCDconsprint("HIGH FREQ       ");
        LCD2nd();
        LCDiprint(mesres[mesresind]);
        LCDconsprint("                ");
        incmesresind;
        ave();
    }

    else if(Measure==4){
        mes[3]++;
        // load
        PORTDbits.RD6=0;
        delay(t);
        PORTDbits.RD7=1;
        delay(t);
        PORTDbits.RD6=1;
        delay(t);
        PORTDbits.RD7=0;
        delay(t);

        sqfreq=PORTDbits.RD1;
        for(index=0; index<7; index++){
            PORTAbits.RA6=1;
            delay(t);
            sqfreq+=(PORTDbits.RD0<<(index+1));
            PORTAbits.RA6=0;
            delay(t);
        }
        //xprintln(sqfreq);
        mesres[mesresind]=10000.0/sqfreq*7.7;
        // uart
        iprintln(mesres[mesresind]);
        //lcd
        LCD1st();
        LCDconsprint("SHORT PERIOD    ");
        LCD2nd();
        LCDiprint(mesres[mesresind]);
        LCDconsprint("                ");
        incmesresind;
        ave();
    }

    PORTDbits.RD5=1; // rst counter
    delay(t);
    PORTDbits.RD5=0;
}
/***********************************TIMER*************************************/

/***********************************TIMER*************************************/
void int0handler(){
    WriteTimer3(0);
    WriteTimer0(0);
    // Short
    if(Measure==6){
        mes[5]++;
        while(PORTBbits.RB0);
        mesres[mesresind]=ReadTimer3()/6;
        // uart
        iprintln(mesres[mesresind]);
        //lcd
        LCD1st();
        LCDconsprint("SHORT INTERVAL  ");
        LCD2nd();
        LCDiprint(mesres[mesresind]);
        LCDconsprint("                ");
        incmesresind;
        ave();
    }
    // Long
    else if(Measure==5){
        mes[4]++;
        while(PORTBbits.RB0);
        mesres[mesresind]=ReadTimer0()/78;
        // uart
        iprintln(mesres[mesresind]);
        //lcd
        LCD1st();
        LCDconsprint("LONG INTERVAL   ");
        LCD2nd();
        LCDiprint(mesres[mesresind]);
        LCDconsprint("                ");
        incmesresind;
        ave();
    }

    else if(Measure==3){
        mes[2]++;
        while(PORTBbits.RB0);
        mesres[mesresind]=ReadTimer0()/39;
        // uart
        iprintln(mesres[mesresind]);
        //lcd
        LCD1st();
        LCDconsprint("LONG PERIOD     ");
        LCD2nd();
        LCDiprint(mesres[mesresind]);
        LCDconsprint("                ");
        incmesresind;
        ave();
    }
}
/***********************************TIMER*************************************/

/***********************************PRINT*************************************/
void iprintln(int i){
    char buf[16];
    sprintf(buf, "%d\n\r", i);
    putsUSART(buf);
}

void iprint(int i){
    char buf[16];
    sprintf(buf, "%d", i);
    putsUSART(buf);
}

void dprint(double res){
    char buf[16];
    sprintf(buf, "%d.%d", (int)res, 100.0*(res-(int)res));
    putsUSART(buf);
}

void dprintln(double res){
    dprint(res);
    putrsUSART((const far rom char *)"\n\r");
}

void xprintln(int i){
    char buf[16];
    sprintf(buf, "%x\n\r", i);
    putsUSART(buf);
}
/***********************************PRINT*************************************/


#pragma romdata bigvector
/* 256 bytes */
rom const signed char u1[256] = {
 32,
 32,
  0,
 32,
 22,
  0,
-22,
 32,
 29,
 22,
 12,
  0,
-12,
-22,
-29,
 32,
 31,
 29,
 26,
 22,
 17,
 12,
  6,
  0,
 -6,
-12,
-17,
-22,
-26,
-29,
-31,
 32,
 31,
 31,
 30,
 29,
 28,
 26,
 24,
 22,
 20,
 17,
 15,
 12,
  9,
  6,
  3,
  0,
 -3,
 -6,
 -9,
-12,
-15,
-17,
-20,
-22,
-24,
-26,
-28,
-29,
-30,
-31,
-31,
 32,
 31,
 31,
 31,
 31,
 31,
 30,
 30,
 29,
 28,
 28,
 27,
 26,
 25,
 24,
 23,
 22,
 21,
 20,
 19,
 17,
 16,
 15,
 13,
 12,
 10,
  9,
  7,
  6,
  4,
  3,
  1,
  0,
 -1,
 -3,
 -4,
 -6,
 -7,
 -9,
-10,
-12,
-13,
-15,
-16,
-17,
-19,
-20,
-21,
-22,
-23,
-24,
-25,
-26,
-27,
-28,
-28,
-29,
-30,
-30,
-31,
-31,
-31,
-31,
-31,
 32,
 31,
 31,
 31,
 31,
 31,
 31,
 31,
 31,
 31,
 31,
 30,
 30,
 30,
 30,
 29,
 29,
 29,
 28,
 28,
 28,
 27,
 27,
 27,
 26,
 26,
 25,
 25,
 24,
 24,
 23,
 23,
 22,
 22,
 21,
 20,
 20,
 19,
 19,
 18,
 17,
 17,
 16,
 15,
 15,
 14,
 13,
 12,
 12,
 11,
 10,
 10,
  9,
  8,
  7,
  7,
  6,
  5,
  4,
  3,
  3,
  2,
  1,
  0,
  0,
  0,
 -1,
 -2,
 -3,
 -3,
 -4,
 -5,
 -6,
 -7,
 -7,
 -8,
 -9,
-10,
-10,
-11,
-12,
-12,
-13,
-14,
-15,
-15,
-16,
-17,
-17,
-18,
-19,
-19,
-20,
-20,
-21,
-22,
-22,
-23,
-23,
-24,
-24,
-25,
-25,
-26,
-26,
-27,
-27,
-27,
-28,
-28,
-28,
-29,
-29,
-29,
-30,
-30,
-30,
-30,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31
};

/* 256 bytes */
rom const signed char u2[256] = {
  0,
  0,
-32,
  0,
-22,
-31,
-22,
  0,
-12,
-22,
-29,
-31,
-29,
-22,
-12,
  0,
 -6,
-12,
-17,
-22,
-26,
-29,
-31,
-31,
-31,
-29,
-26,
-22,
-17,
-12,
 -6,
  0,
 -3,
 -6,
 -9,
-12,
-15,
-17,
-20,
-22,
-24,
-26,
-28,
-29,
-30,
-31,
-31,
-31,
-31,
-31,
-30,
-29,
-28,
-26,
-24,
-22,
-20,
-17,
-15,
-12,
 -9,
 -6,
 -3,
  0,
 -1,
 -3,
 -4,
 -6,
 -7,
 -9,
-10,
-12,
-13,
-15,
-16,
-17,
-19,
-20,
-21,
-22,
-23,
-24,
-25,
-26,
-27,
-28,
-28,
-29,
-30,
-30,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-30,
-30,
-29,
-28,
-28,
-27,
-26,
-25,
-24,
-23,
-22,
-21,
-20,
-19,
-17,
-16,
-15,
-13,
-12,
-10,
 -9,
 -7,
 -6,
 -4,
 -3,
 -1,
  0,
  0,
 -1,
 -2,
 -3,
 -3,
 -4,
 -5,
 -6,
 -7,
 -7,
 -8,
 -9,
-10,
-10,
-11,
-12,
-12,
-13,
-14,
-15,
-15,
-16,
-17,
-17,
-18,
-19,
-19,
-20,
-20,
-21,
-22,
-22,
-23,
-23,
-24,
-24,
-25,
-25,
-26,
-26,
-27,
-27,
-27,
-28,
-28,
-28,
-29,
-29,
-29,
-30,
-30,
-30,
-30,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-31,
-30,
-30,
-30,
-30,
-29,
-29,
-29,
-28,
-28,
-28,
-27,
-27,
-27,
-26,
-26,
-25,
-25,
-24,
-24,
-23,
-23,
-22,
-22,
-21,
-20,
-20,
-19,
-19,
-18,
-17,
-17,
-16,
-15,
-15,
-14,
-13,
-12,
-12,
-11,
-10,
-10,
 -9,
 -8,
 -7,
 -7,
 -6,
 -5,
 -4,
 -3,
 -3,
 -2,
 -1,
  0
};

/* 512 bytes */
rom const unsigned char square[512] = {
	    0,	    0,	    0,	    0,	    0,	    0,	    0,	    0,
	    0,	    0,	    0,	    0,	    0,	    0,	    0,	    0,
	    0,	    0,	    0,	    0,	    0,	    0,	    0,	    0,
	    0,	    0,	    0,	    0,	    0,	    0,	    0,	    0,
	    1,	    1,	    1,	    1,	    1,	    1,	    1,	    1,
	    1,	    1,	    1,	    1,	    1,	    1,	    2,	    2,
	    2,	    2,	    2,	    2,	    2,	    2,	    2,	    2,
	    3,	    3,	    3,	    3,	    3,	    3,	    3,	    3,
	    4,	    4,	    4,	    4,	    4,	    4,	    4,	    4,
	    5,	    5,	    5,	    5,	    5,	    5,	    5,	    6,
	    6,	    6,	    6,	    6,	    6,	    7,	    7,	    7,
	    7,	    7,	    7,	    8,	    8,	    8,	    8,	    8,
	    9,	    9,	    9,	    9,	    9,	    9,	   10,	   10,
	   10,	   10,	   10,	   11,	   11,	   11,	   11,	   12,
	   12,	   12,	   12,	   12,	   13,	   13,	   13,	   13,
	   14,	   14,	   14,	   14,	   15,	   15,	   15,	   15,
	   16,	   16,	   16,	   16,	   17,	   17,	   17,	   17,
	   18,	   18,	   18,	   18,	   19,	   19,	   19,	   19,
	   20,	   20,	   20,	   21,	   21,	   21,	   21,	   22,
	   22,	   22,	   23,	   23,	   23,	   24,	   24,	   24,
	   25,	   25,	   25,	   25,	   26,	   26,	   26,	   27,
	   27,	   27,	   28,	   28,	   28,	   29,	   29,	   29,
	   30,	   30,	   30,	   31,	   31,	   31,	   32,	   32,
	   33,	   33,	   33,	   34,	   34,	   34,	   35,	   35,
	   36,	   36,	   36,	   37,	   37,	   37,	   38,	   38,
	   39,	   39,	   39,	   40,	   40,	   41,	   41,	   41,
	   42,	   42,	   43,	   43,	   43,	   44,	   44,	   45,
	   45,	   45,	   46,	   46,	   47,	   47,	   48,	   48,
	   49,	   49,	   49,	   50,	   50,	   51,	   51,	   52,
	   52,	   53,	   53,	   53,	   54,	   54,	   55,	   55,
	   56,	   56,	   57,	   57,	   58,	   58,	   59,	   59,
	   60,	   60,	   61,	   61,	   62,	   62,	   63,	   63,
	   64,	   64,	   65,	   65,	   66,	   66,	   67,	   67,
	   68,	   68,	   69,	   69,	   70,	   70,	   71,	   71,
	   72,	   72,	   73,	   73,	   74,	   74,	   75,	   76,
	   76,	   77,	   77,	   78,	   78,	   79,	   79,	   80,
	   81,	   81,	   82,	   82,	   83,	   83,	   84,	   84,
	   85,	   86,	   86,	   87,	   87,	   88,	   89,	   89,
	   90,	   90,	   91,	   92,	   92,	   93,	   93,	   94,
	   95,	   95,	   96,	   96,	   97,	   98,	   98,	   99,
	  100,	  100,	  101,	  101,	  102,	  103,	  103,	  104,
	  105,	  105,	  106,	  106,	  107,	  108,	  108,	  109,
	  110,	  110,	  111,	  112,	  112,	  113,	  114,	  114,
	  115,	  116,	  116,	  117,	  118,	  118,	  119,	  120,
	  121,	  121,	  122,	  123,	  123,	  124,	  125,	  125,
	  126,	  127,	  127,	  128,	  129,	  130,	  130,	  131,
	  132,	  132,	  133,	  134,	  135,	  135,	  136,	  137,
	  138,	  138,	  139,	  140,	  141,	  141,	  142,	  143,
	  144,	  144,	  145,	  146,	  147,	  147,	  148,	  149,
	  150,	  150,	  151,	  152,	  153,	  153,	  154,	  155,
	  156,	  157,	  157,	  158,	  159,	  160,	  160,	  161,
	  162,	  163,	  164,	  164,	  165,	  166,	  167,	  168,
	  169,	  169,	  170,	  171,	  172,	  173,	  173,	  174,
	  175,	  176,	  177,	  178,	  178,	  179,	  180,	  181,
	  182,	  183,	  183,	  184,	  185,	  186,	  187,	  188,
	  189,	  189,	  190,	  191,	  192,	  193,	  194,	  195,
	  196,	  196,	  197,	  198,	  199,	  200,	  201,	  202,
	  203,	  203,	  204,	  205,	  206,	  207,	  208,	  209,
	  210,	  211,	  212,	  212,	  213,	  214,	  215,	  216,
	  217,	  218,	  219,	  220,	  221,	  222,	  223,	  224,
	  225,	  225,	  226,	  227,	  228,	  229,	  230,	  231,
	  232,	  233,	  234,	  235,	  236,	  237,	  238,	  239,
	  240,	  241,	  242,	  243,	  244,	  245,	  246,	  247,
	  248,	  249,	  250,	  251,	  252,	  253,	  254,	  255
};

#pragma romdata

/*******************************************************************************/
/* optfft.c                                                                    */
/*                                                                             */
/* An optimized version of the fft function using only 16-bit integer math.    */
/*                                                                             */
/* Optimized by Brent Plump                                                    */
/* Based heavily on code by Jinhun Joung                                       */
/*                                                                             */
/* - Works only for input arrays of 256 length.                                */
/* - Requires two arrays of 16-bit ints.  The first contains the samples, the  */
/*   second contains all zeros.  The samples range from -31 to 32              */
/* - Returns the index of the peak frequency                                   */
/*******************************************************************************/

#define ABS(x)  (((x)<0)?(-(x)):(x))
#define CEILING(x) (((x)>511)?511:(x))

signed int optfft(signed int real[256], signed int real2[256], signed int imag[256], signed int imag2[256]) {

signed int i, i1, j, l, l1, l2, t1, t2, u;

	/* Bit reversal. */
	/*Do the bit reversal */
	l2 = 128;
	i=0;
	for(l=0;l<255;l++) {
		if(l < i) {
                    if(l<128)
			j=real[l];
                    else
                        j=real2[l-127];

                    if(l<128 && i<128)
                        real[l]=real[i];
                    else if(l>128 && i<128)
                        real2[l-127]=real[i];
                    else if(l<128 && i>128)
                        real[l]=real2[i-127];
                    else
                        real2[l-127]=real2[i-127];

                    if(i<128)
                        real[i]=j;
                    else
                        real2[i-127]=j;
		}
		l1 = l2;
		while (l1 <= i){
			i -= l1;
			l1 >>= 1;
		}
		i += l1;
	}

	/* Compute the FFT */
	u = 0;
	l2 = 1;
	for(l=0;l<8;l++){
		l1 = l2;
		l2 <<= 1;
		for(j=0;j<l1;j++){
			for(i=j;i<256;i+=l2){
				i1 = i + l1;
                                if(i1<128){
                                    t1 = (u1[u]*real[i1] - u2[u]*imag[i1])/32;
                                    t2 = (u1[u]*imag[i1] + u2[u]*real[i1])/32;
                                }else{
                                    t1 = (u1[u]*real2[i1-127] - u2[u]*imag2[i1-127])/32;
                                    t2 = (u1[u]*imag2[i1-127] + u2[u]*real2[i1-127])/32;
                                }

                                if(i1<128 && i<128){
                                    real[i1] = real[i]-t1;
                                    imag[i1] = imag[i]-t2;
                                }else if(i1>128 && i<128){
                                    real2[i1-127] = real[i]-t1;
                                    imag2[i1-127] = imag[i]-t2;
                                }else if(i1<128 && i>128){
                                    real[i1] = real2[i-127]-t1;
                                    imag[i1] = imag2[i-127]-t2;
                                }else{
                                    real2[i1-127] = real2[i-127]-t1;
                                    imag2[i1-127] = imag2[i-127]-t2;
                                }

                                if(i<128){
                                    real[i] += t1;
                                    imag[i] += t2;
                                }else{
                                    real2[i-127] += t1;
                                    imag2[i-127] += t2;
                                }
			}
			u++;
		}
	}

	/* Find the highest amplitude value */
	/* start at index 1 because 0 can hold high values */
	j=1;
	l=0;
	for ( i=1; i<(128); i++ ) {
		l1 = square[CEILING(ABS(real[i]))]+square[CEILING(ABS(imag[i]))];
		if (l1 > l) {
			j = i;
			l = l1;
		}
	}
	return (j);
}