#include <xc.h>

#define _XTAL_FREQ 20000000  // 20MHz Crystal Oscillator

// PIC16F877A Configuration Bits
#pragma config FOSC = XT        // Oscillator Selection (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer (Disabled)
#pragma config BOREN = OFF      // Brown-out Reset (Disabled)
#pragma config LVP = OFF        // Low-Voltage Programming (Disabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection (Disabled)
#pragma config WRT = OFF        // Flash Program Memory Write Protection (Disabled)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Disabled)

// LCD Control Pins
#define EN PORTDbits.RD2
#define RW PORTDbits.RD1
#define RS PORTDbits.RD0

// ADC Variables
unsigned int adc_value = 0;
float temperature = 0.0;

// Function Prototypes
void delay(unsigned int i);
void lcd_cmd(unsigned char cmd);
void lcd_chr(unsigned char data);
void lcd_str(const char *str);
void lcd_init();
void adc_init();
unsigned int adc_read();
void display_temp();

void delay(unsigned int i) {
    while(i--);
}

// Send Command to LCD
void lcd_cmd(unsigned char cmd) {
    PORTC = cmd;  // Send command to PORTC (LCD Data)
    RS = 0;       // Command Mode
    RW = 0;       // Write Mode
    EN = 1;       // Enable LCD
    __delay_ms(2);
    EN = 0;
}

// Send Character to LCD
void lcd_chr(unsigned char data) {
    PORTC = data; // Send character to PORTC
    RS = 1;       // Data Mode
    RW = 0;       // Write Mode
    EN = 1;       // Enable LCD
    __delay_ms(2);
    EN = 0;
}

// Send String to LCD
void lcd_str(const char *str) {
    while(*str) {
        lcd_chr(*str++);
    }
}

// Initialize LCD
void lcd_init() {
    __delay_ms(20);
    lcd_cmd(0x38);  // 8-bit mode, 2 lines, 5x8 font
    lcd_cmd(0x06);  // Entry mode
    lcd_cmd(0x0C);  // Display ON, Cursor OFF
    lcd_cmd(0x01);  // Clear Display
    __delay_ms(5);  // Extra delay
}

// Initialize ADC
void adc_init() {
    ADCON0 = 0x41;  // Select Channel 0 (RA0), Enable ADC
    ADCON1 = 0x80;  // Configure RA0 as analog input
}

// Read ADC Value from RA0
unsigned int adc_read() {
    ADCON0 |= 0x04;   // Start ADC Conversion
    while (ADCON0 & 0x04); // Wait for conversion to complete
    return ((unsigned int)ADRESH << 8) | ADRESL; // Read ADC result (Fixed signedness warning)
}

// Display Temperature on LCD
void display_temp() {
    adc_value = adc_read();  // Read ADC value
    float adc_voltage = (float)adc_value * (5.0f / 1023.0f); // Convert ADC to Voltage
    temperature = adc_voltage * 100.0f; // Convert Voltage to Temperature (LM35)

    // Convert to integer values for display
    int temp_int = (int)temperature;  // Whole number part
    int temp_frac = (int)((temperature - temp_int) * 100); // Two decimal places

    // Move Cursor to Position
    lcd_cmd(0x86);

    // Display Temperature (XX.XX°C)
    lcd_chr((temp_int / 10) + '0'); // Tens
    lcd_chr((temp_int % 10) + '0'); // Ones
    lcd_chr('.');  // Decimal point
    lcd_chr((temp_frac / 10) + '0'); // Tenths place
    lcd_chr((temp_frac % 10) + '0'); // Hundredths place
    lcd_chr('C'); // Degree Celsius
}

void main(void) {
    TRISD = 0x00;  // PORTD as Output (LCD Control)
    TRISC = 0x00;  // PORTC as Output (LCD Data)
    PORTC = PORTD = 0x00; // Clear LCD
    
    lcd_init();  // Initialize LCD
    lcd_cmd(0x80);
    lcd_str("Temp: ");

    adc_init(); // Initialize ADC

    while(1) {
        display_temp(); // Update Temperature Display
        __delay_ms(500);
    }
}
