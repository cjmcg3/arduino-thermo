/*Connor MCGarry --------- 03/20/23 --------- ECE231 lab2

The following code recieves data from a TMP36 temperature sensor on ADC5 of an arduino, and displays the temp in F on a four digit, 7 segment display.
The value changes from F to C when a button is being pressed, and the output is contantly output to a serial moniter via USART*/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#define MYDELAY 2
 

void uart_init(void);
void uart_send(unsigned char ch);
void send_string(char *stringAddress);
void adc_init(void);
unsigned int get_adc(void);

int main(void){
    
    unsigned int digitalValue;
    int tempC, tempF;
    adc_init();
    uart_init();
    PORTD = 1<<PORTD2;
    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D,
    0x07, 0x7F, 0x67};  // NUmber values
    unsigned char deci, ones, tens, unit;
    unsigned char test[6];
    unsigned char test2[6];
    DDRC = 0x0F;  // PC0 - PC3
    DDRD = 0xF0;  //PD4 - PD7
    DDRB = 0xFF;   // Digit enable pins (9,10,11,12)

        
        
    while(1){
        digitalValue = get_adc();  
        tempC = (digitalValue * 1.1/10.24 - 50.0)*10;
        tempF = (tempC * 9.0/5.0/10 + 32.0)*10;

        if ((PIND&(1<<PIND2))==0){ // If the button is being pressed
            tens = (tempC/100)%10;
            ones = tempC/10%10;
            // decimal
            deci = tempC%10;  // NEEDS TO BE THE REMAINDER
            unit = 0x39;
            uart_send(tens+'0');
            uart_send(ones+'0');
            uart_send('.');
            uart_send(deci+'0');
            uart_send('C');
            }

        else{
            tens = (tempF/100)%10;
            ones = tempF/10%10;
            // decimal
            deci = tempF%10; // NEEDS TO BE THE REMAINDER
            unit = 0x71;
            uart_send(tens+'0');
            uart_send(ones+'0');
            uart_send('.');
            uart_send(deci+'0');
            uart_send('F');
        }

        PORTC = (PORTC&0xF0)|(unit&0x0F);  // Chooses either the C or F value 
        PORTD = (unit&0xF0)|(PORTD&0x0F);
        PORTB = ~ (1<<1);   // Enables digit
        _delay_ms(MYDELAY);

        PORTC = (PORTC&0xF0)|(ledDigits[deci]&0x0F);  
        PORTD = (ledDigits[deci]&0xF0)|(PORTD&0x0F);
        PORTB = ~ (1<<2);
        _delay_ms(MYDELAY);

        PORTC = (PORTC&0xF0)|(ledDigits[ones]&0x0F)|0b10000000;  
        PORTD = (ledDigits[ones]&0xF0)|(PORTD&0x0F)|0b10000000;  // OR with the binary needed for a decimal point 
        PORTB = ~ (1<<3);
        _delay_ms(MYDELAY);

        PORTC = (PORTC&0xF0)|(ledDigits[tens]&0x0F);  
        PORTD = (ledDigits[tens]&0xF0)|(PORTD&0x0F);
        PORTB = ~ (1<<4);
        _delay_ms(MYDELAY);

        PORTB = 0xFF;

        uart_send(13);
        uart_send(10);
        _delay_ms(2);  
    }
    return 0;
}

void uart_init(void){
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
    UBRR0L = 103;
}

void uart_send(unsigned char ch){
    while(!(UCSR0A&(1<<UDRE0)));
    UDR0=ch;
}

void send_string(char *stringAddress){
    unsigned char i;
    for (i=0; i < strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
    
}

void adc_init(void){
    ADMUX = 0xC5;  // 1.1 Vref
    ADCSRA = 0x87;
}

unsigned int get_adc(){
    ADCSRA |= (1<<ADSC);
    while ((ADCSRA &(1<<ADIF))==0);
    return ADCL | (ADCH<<8);
}
