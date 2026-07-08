#include <xc.h>
#include <string.h>

#define _XTAL_FREQ 20000000  // 20MHz Crystal
#define RS PORTDbits.RD0
#define RW PORTDbits.RD1
#define EN PORTDbits.RD2
// Keypad Pins
#define c1 PORTBbits.RB0
#define c2 PORTBbits.RB1
#define c3 PORTBbits.RB2
#define R1 PORTBbits.RB4
#define R2 PORTBbits.RB5
#define R3 PORTBbits.RB6
#define R4 PORTBbits.RB7

// Function Prototypes
void lcd_init();
void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_string(const unsigned char*);
void keypad();
void check_password();

// Predefined Password
char password[] = "1234";
char input[5];
int pos = 0;

void main() {
    TRISC = 0X00;
    TRISD = 0x00;
    TRISB = 0XF0;
    lcd_init();
    lcd_cmd(0x80);
    lcd_string((const unsigned char*)"Enter Password:");  
    lcd_cmd(0xC0);
    while (1) {
        keypad();
    }
}

// Initialize LCD
void lcd_init() {
    lcd_cmd(0x38);
    lcd_cmd(0x06);
    lcd_cmd(0x0C);
    lcd_cmd(0x01);
}

// Send command to LCD
void lcd_cmd(unsigned char cmd) {
    PORTC = cmd;
    RS = 0;
    EN = 1;
    RW = 0;
    __delay_ms(5);
    EN = 0;
}

// Send data to LCD
void lcd_data(unsigned char data) {
    PORTC = data;
    RS = 1;
    EN = 1;
    RW = 0;
    __delay_ms(5);
    EN = 0;
}

// Display string on LCD
void lcd_string(const unsigned char* str) {
    while (*str) {
        lcd_data(*str++);
    }
}

// Scan 4x3 Keypad
void keypad() {
    if (pos >= 4) return; // Prevent entering more than 4 digits

    c1 = 1; c2 = 0; c3 = 0;
    if (R1 == 1) { input[pos++] = '1'; lcd_data('*'); while (R1 == 1); }
    if (R2 == 1) { input[pos++] = '4'; lcd_data('*'); while (R2 == 1); }
    if (R3 == 1) { input[pos++] = '7'; lcd_data('*'); while (R3 == 1); }
    if (R4 == 1) { input[pos++] = '*'; lcd_data('*'); while (R4 == 1); }

    c1 = 0; c2 = 1; c3 = 0;
    if (R1 == 1) { input[pos++] = '2'; lcd_data('*'); while (R1 == 1); }
    if (R2 == 1) { input[pos++] = '5'; lcd_data('*'); while (R2 == 1); }
    if (R3 == 1) { input[pos++] = '8'; lcd_data('*'); while (R3 == 1); }
    if (R4 == 1) { input[pos++] = '0'; lcd_data('*'); while (R4 == 1); }

    c1 = 0; c2 = 0; c3 = 1;
    if (R1 == 1) { input[pos++] = '3'; lcd_data('*'); while (R1 == 1); }
    if (R2 == 1) { input[pos++] = '6'; lcd_data('*'); while (R2 == 1); }
    if (R3 == 1) { input[pos++] = '9'; lcd_data('*'); while (R3 == 1); }
    if (R4 == 1) { input[pos++] = '#'; lcd_data('*'); while (R4 == 1); }

    if (pos == 4) {
        input[4] = '\0'; // Null terminate the string
        __delay_ms(500);
        check_password();
    }
}

// Check password
void check_password() {
    lcd_cmd(0x01); // Clear screen
    lcd_cmd(0x80);
    if (strcmp(input, password) == 0) {
        lcd_string((const unsigned char*)"Access Granted");
    } else {
        lcd_string((const unsigned char*)"Access Denied");
    }
    __delay_ms(2000);
    lcd_cmd(0x01); // Clear screen
    lcd_cmd(0x80);
    lcd_string((const unsigned char*)"Enter Password:");
    lcd_cmd(0xC0);
    pos = 0; // Reset position for new input
}
