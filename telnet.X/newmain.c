/*
 * File:   newmain.c
 * Author: The Birds
 *
 * Created on July 7, 2017, 11:02 PM
 */
#define _XTAL_FREQ 20000000

//FOR LCD
#define RS PORTDbits.RD2
#define EN PORTDbits.RD3
#define D4 PORTDbits.RD4
#define D5 PORTDbits.RD5
#define D6 PORTDbits.RD6
#define D7 PORTDbits.RD7

//other pins
#define red_led   PORTEbits.RE0
#define green_led PORTEbits.RE1
#define blue_led  PORTEbits.RE2
#define trigger   PORTDbits.RD0
#define echo      PORTDbits.RD1
#define buzzer    PORTAbits.RA0 
#define motion    PORTCbits.RC0

#include <xc.h>
#include <p18f4550.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h";

#pragma	config WDT=OFF      // disable watchdog timer
#pragma	config MCLRE = ON   // MCLEAR Pin on
#pragma	config DEBUG = ON   // Enable Debug Mode
#pragma	config LVP = OFF    // Low-Voltage programming disabled (necessary for debugging)
#pragma config FOSC = HS    // External Oscillator 

char read_char(void) {
    //messages end with \r\n, so we should be able to do somethign with that
    while (PIR1bits.RCIF == 0) {
        // wait for char
        if (RCSTAbits.OERR == 1) {
            RCSTAbits.OERR = 0; // clear overrun if it occurs
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
    }
    return RCREG;
}

void buzz_seconds(int seconds) {
    int time = seconds * 500;
    for (int i = 0; i < time; i++) {
        buzzer = 1; __delay_ms(1);
        buzzer = 0; __delay_ms(1); }
}

void var_delay_us(int us) {
    for (int i = 0; i < us; i++) { __delay_us(1); }
}

void Set_LED(int red, int green, int blue, int count, char * color_name) {
    //if 256 always on, if //0 never on... //128 half on half off
    red /= 8; green /= 8; blue /=  8;

    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Led is: "); 
    Lcd_Write_String(color_name);
    
    for (int i = 0; i < count; i++) {
        if (red){   red_led   = 1; var_delay_us(red);   red_led   = 0;}
        if (green){ green_led = 1; var_delay_us(green); green_led = 0;}
        if (blue){  blue_led  = 1; var_delay_us(blue);  blue_led  = 0;}
    }
}

void read_and_print_distance(int count) {
    
    long distance;
    float centimeters, inches;
    char cent_str[4], inch_str[4];

    for (int i = 0; i < count; i++) {
        TMR1H = 0; TMR1L = 0; //Sets the Initial Value of Timer
        trigger = 0; __delay_us(5);
        trigger = 1; __delay_us(10);
        trigger = 0; 

        while (!echo); TMR1ON = 1; //Timer Starts
        while (echo);  TMR1ON = 0;

        distance = (TMR1L | (TMR1H << 8)); //Reads Timer Value

        centimeters = distance / (5 * 58.82); //Converts Time to Distance
        inches      = centimeters * 0.393701;

        itoa(inch_str, inches, 10);
        itoa(cent_str, centimeters, 10);

        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);

        Lcd_Write_String("Centimeters: "); 
        Lcd_Write_String(cent_str);
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String("Inches:      ");
        Lcd_Write_String(inch_str);
        __delay_ms(500);
    }
}

void standby_message() {
    Lcd_Set_Cursor(1, 1); Lcd_Write_String("   Waiting on   ");
    Lcd_Set_Cursor(2, 1); Lcd_Write_String("    input...    ");
}

void countdown_timer(int seconds) {

    char sec_str[4];

    Lcd_Clear();
    Lcd_Set_Cursor(1, 1); Lcd_Write_String("Counting Down... ");


    for (int i = seconds; i > 0; i--) {
        itoa(sec_str, i, 10);
        Lcd_Set_Cursor(2, 1); Lcd_Write_String("   ");
        Lcd_Set_Cursor(2, 1); Lcd_Write_String(sec_str);
        Lcd_Write_String(" secs left...");
        __delay_ms(1000);
    }

    Lcd_Set_Cursor(1, 1); Lcd_Write_String("   Countdown    ");
    Lcd_Set_Cursor(2, 1); Lcd_Write_String("   Finished!    ");
    buzz_seconds(3);


}

void motion_sensor() {

    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String(" Don't Move.");


    while (motion == 1) {
    };

    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("    Security    ");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("    Activated   ");

    while (motion == 0);


    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("     Motion     ");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("    Detected!   ");

    buzz_seconds(3);


}

void main(void) {

    TRISA = 0b00000000; //RGB LEDs. 
    TRISB = 0b11111100; // ??? what   
    TRISC = 0b00000001; //input for motion sensor.
    TRISD = 0b00000010; // set all of PortD as outputs 
    TRISE = 0b00000000; //3 LEDs as outputs

    //disable tx/rx interrupts here. 
    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 0;

    int led_time = 12000;
    char ours;
    
    // setup UART
    TXSTA = 0;
    TRISCbits.TRISC6 = 0; // TX as output
    TRISCbits.TRISC7 = 1; // RX as input
    TXSTAbits.SYNC   = 0; // Async operation
    TXSTAbits.TX9    = 0; // No tx of 9th bit   

    // This allows us to continuosly receive data.
    RCSTA = 0;
    RCSTAbits.CREN = 1;

    // enable 8 bit baud rate, and set it to 9600.
    // 9600 = 20,000,000 / (64(x+1))
    // x = 31.552.. 
    BAUDCONbits.BRG16 = 0; //enables 8 bit baud rate generator.
    TXSTA = 0;
    TXSTAbits.BRGH = 0; // low speed (default)    
    SPBRG = 32;

    RCSTAbits.RX9 = 0; // No rx of 9th bit    
    RCSTAbits.SPEN = 1; //enable RX/TX as serial pins. 

    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("The Birds! 2017 ");
    __delay_ms(1000);
    
    while (1) {
        standby_message();
        ours = read_char();
        switch (ours) {
            case '1': countdown_timer(10);                         break;
            case '2': countdown_timer(20);                         break;
            case '3': countdown_timer(30);                         break;
            case 'r': Set_LED(256, 000, 000, led_time, "Red");     break;
            case 'g': Set_LED(000, 256, 000, led_time, "Green");   break;
            case 'b': Set_LED(000, 000, 256, led_time, "Blue");    break;
            case 'p': Set_LED(256, 000, 180, led_time, "Purple");  break;
            case 't': Set_LED(000, 256, 256, led_time, "Teal");    break;
            case 'w': Set_LED(256, 128, 128, led_time, "White");   break;
            case 'd': read_and_print_distance(20);                 break;
            case 'm': motion_sensor();                             break;

            default:
                Lcd_Clear();
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("UNEXPECTED: ");
                Lcd_Set_Cursor(2, 1);
                Lcd_Write_Char(ours);
                __delay_ms(2000);
                break;
        }
    }
    return;
}