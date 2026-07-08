#include <xc.h>

#define _XTAL_FREQ 20000000  // 20MHz Crystal Frequency
#define STREETLIGHT PORTCbits.RC0  // Streetlight connected to RC0

// Configuration Bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

void ADC_Init() {
    ADCON1 = 0x80;  // Right justified, Fosc/32
    ADCON0 = 0x41;  // Channel 0 (AN0), ADC ON
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0xC5;  // Clear channel selection bits
    ADCON0 |= channel << 3;  // Select ADC channel
    __delay_ms(2);  // Wait for acquisition time
    GO_nDONE = 1;  // Start conversion
    while (GO_nDONE);  // Wait for conversion to complete
    return ((ADRESH << 8) + ADRESL);  // Return ADC result
}

void main() {
    TRISA = 0xFF;  // Set PORTA as input (for LDR)
    TRISC = 0x00;  // Set PORTC as output (for streetlight)
    ADC_Init();  // Initialize ADC
    
    while (1) {
        unsigned int light = ADC_Read(0);  // Read LDR value
        
        if (light < 500) {  // Low light condition (threshold can be adjusted)
            STREETLIGHT = 0;  // Turn ON streetlight
        } else {
            STREETLIGHT = 1;  // Turn OFF streetlight
        }
        
        __delay_ms(500);  // Small delay
    }
}

