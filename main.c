#pragma config PLLSEL = PLL4X   // PLL Selection (4x clock multiplier)
#pragma config CFGPLLEN = OFF   // PLL Enable Configuration bit (PLL Disabled (firmware controlled))
#pragma config CPUDIV = NOCLKDIV// CPU System Clock Postscaler (CPU uses system clock (no divide))
#pragma config LS48MHZ = SYS24X4// Low Speed USB mode with 48 MHz system clock (System clock at 24 MHz, USB clock divider is set to 4)

// CONFIG1H
#pragma config FOSC = HSM       // Oscillator Selection (HS oscillator, medium power 4MHz to 16MHz)
#pragma config PCLKEN = ON      // Primary Oscillator Shutdown (Primary oscillator enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config nPWRTEN = OFF    // Power-up Timer Enable (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable (BOR enabled in hardware (SBOREN is ignored))
#pragma config BORV = 190       // Brown-out Reset Voltage (BOR set to 1.9V nominal)
#pragma config nLPBOR = OFF     // Low-Power Brown-out Reset (Low-Power Brown-out Reset disabled)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (WDT enabled in hardware (SWDTEN ignored))
#pragma config WDTPS = 32768    // Watchdog Timer Postscaler (1:32768)

// CONFIG3H
#pragma config CCP2MX = RC1     // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF      // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
#pragma config T3CMX = RC0      // Timer3 Clock Input MUX bit (T3CKI function is on RC0)
#pragma config SDOMX = RC7      // SDO Output MUX bit (SDO function is on RB3)
#pragma config MCLRE = ON       // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled)

// CONFIG4L
#pragma config STVREN = OFF      // Stack Full/Underflow Reset (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled)

// CONFIG5L
#pragma config CP0 = OFF        // Block 0 Code Protect (Block 0 is not code-protected)
#pragma config CP1 = OFF        // Block 1 Code Protect (Block 1 is not code-protected)
#pragma config CP2 = OFF        // Block 2 Code Protect (Block 2 is not code-protected)
#pragma config CP3 = OFF        // Block 3 Code Protect (Block 3 is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protect (Boot block is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protect (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Block 0 Write Protect (Block 0 (0800-1FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Block 1 Write Protect (Block 1 (2000-3FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Block 2 Write Protect (Block 2 (04000-5FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Block 3 Write Protect (Block 3 (06000-7FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Registers Write Protect (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protect (Boot block (0000-7FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protect (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Block 0 Table Read Protect (Block 0 is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Block 1 Table Read Protect (Block 1 is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Block 2 Table Read Protect (Block 2 is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Block 3 Table Read Protect (Block 3 is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protect (Boot block is not protected from table reads executed in other blocks)

#define _XTAL_FREQ 48000000

/*
 I'll plan to upload this project to my github so:
 * 
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lcd.h"
#include "kbd4x4.h"
#include "diy_expr.h"

void addCharacter(char **str, char c);
void removeCharacter(char **str);
void clearString(char **str);

char rbuffer[32]; //float max digits is 32 at any given time
char *eval_buffer = NULL; //this is where the expr is saved
short eval_len = 1; //the size (number of characters) of eval_buffer

void main()
{
    /*Setup*/
    INTCON2bits.RBPU=0; //enable all weak pull-up resistors at PORTB
    
    Keypad_Init();
    Lcd_Init();                 
    Lcd_Clear(); 
    Lcd_Blink();
    Lcd_Set_Cursor(1,1);
    
    /* Main loop */
    while(1)
    {
        __delay_ms(50);
        char c = Keypad_Get_Char();
        switch(c){
            case 0:
                continue;
            case 'R':
                if (eval_len == 1)
                    continue;
            
                eval_len--;
                if(eval_len>15){
                    Lcd_Shift_Right();
                }
                Lcd_Set_Cursor(1,eval_len%16);
                removeCharacter(&eval_buffer);
                
                break;
            case '=':
            {                
                eval_len=1;
                float result = evaluateExpression(eval_buffer);
                sprintf(rbuffer, "%f", result);
                Lcd_Clear();
                Lcd_Set_Cursor(2,1);
                Lcd_Write_String(rbuffer);
                Lcd_Set_Cursor(1,1);
                clearString(&eval_buffer);
            }
                break;
            default:
                if(eval_len>15){
                    Lcd_Shift_Left();
                }
                Lcd_Write_Char(c);
                addCharacter(&eval_buffer,c);
                eval_len++;
                break;
        }
    }
}

void addCharacter(char **str, char c) {
    // Check if the input string is NULL
    if (*str == NULL) {
        // Allocate memory for the first character and null terminator
        *str = (char *)malloc(2 * sizeof(char));
        if (*str == NULL) {
            return;
        }
        (*str)[0] = c;
        (*str)[1] = '\0'; // Null terminator
    } else {
        // Calculate the new size of the string
        size_t newSize = strlen(*str) + 2; // Current length + new character + null terminator

        // Reallocate memory for the updated string
        *str = (char *)realloc(*str, newSize * sizeof(char));
        if (*str == NULL) {
            return;
        }

        // Add the new character to the end of the string
        (*str)[newSize - 2] = c;
        (*str)[newSize - 1] = '\0'; // Null terminator
    }
}

void removeCharacter(char **str) {
    // Check if the input string is NULL or empty
    if (*str == NULL || (*str)[0] == '\0') {
        return;
    }

    // Calculate the new size of the string after removing one character
    size_t newSize = strlen(*str);

    // Reallocate memory for the updated string
    *str = (char *)realloc(*str, newSize * sizeof(char));
    if (*str == NULL) {
        return;
    }
}

void clearString(char **str) {
    // Check if the input string is NULL
    if (*str == NULL) {
        return;
    }

    // Free the memory allocated for the string
    free(*str);

    // Set the string pointer to NULL to indicate an empty string
    *str = NULL;
}