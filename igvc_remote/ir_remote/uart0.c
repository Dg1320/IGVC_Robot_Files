// UART0 Library

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    -

// Hardware configuration:
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"

// PortA masks
#define UART_TX_MASK 2
#define UART_RX_MASK 1

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize UART0
void initUart0()
{
    // Enable clocks
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;            // enable uart clock 0
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;            // enable gpio port A
    _delay_cycles(3);

    // Configure UART0 pins
    GPIO_PORTA_DEN_R |= UART_TX_MASK | UART_RX_MASK;    // enable digital on UART0 pins
    GPIO_PORTA_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;  // use peripheral to drive PA0, PA1
    GPIO_PORTA_PCTL_R &= ~(GPIO_PCTL_PA1_M | GPIO_PCTL_PA0_M); // clear bits 0-7
    GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;
                                                        // select UART0 to drive pins PA0 and PA1: default, added for clarity

    // Configure UART0 to 19200 baud (assuming fcyc = 40 MHz), 8N1 format
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_CC_R = UART_CC_CS_SYSCLK;                     // use system clock (40 MHz)
    UART0_IBRD_R = 21;                                  // Integer Baud Rate = 40 MHz / (Nx115.2kHz), where N=16
    UART0_FBRD_R = 45;                                  // Fractional Baud Rate = decimal portion converted to a 6-bit integer = round(fract(r)*64)= 13
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
                                                        // enable TX, RX, and module
}

// Set baud rate as function of instruction cycle frequency
void setUart0BaudRate(uint32_t baudRate, uint32_t fcyc)
{
    uint32_t divisorTimes128 = (fcyc * 8) / baudRate;   // calculate divisor (r) in units of 1/128,
                                                        // where r = fcyc / 16 * baudRate
    divisorTimes128 += 1;                               // add 1/128 to allow rounding
    UART0_CTL_R = 0;                                    // turn-off UART0 to allow safe programming
    UART0_IBRD_R = divisorTimes128 >> 7;                // set integer value to floor(r)
    UART0_FBRD_R = ((divisorTimes128) >> 1) & 63;       // set fractional value to round(fract(r)*64)
    UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // configure for 8N1 w/ 16-level FIFO   // using system clock/8 [HEN]
    UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;
                                                        // turn-on UART0
}

// Blocking function that writes a serial character when the UART buffer is not full
void putcUart0(char c)
{
    while (UART0_FR_R & UART_FR_TXFF);               // wait if uart0 tx fifo full
    UART0_DR_R = c;                                  // write character to fifo
}

// Blocking function that writes a string when the UART buffer is not full
void putsUart0(char* str)
{
    uint8_t i = 0;
    while (str[i] != '\0')
        putcUart0(str[i++]);
}

// Blocking function that returns with serial data once the buffer is not empty
char getcUart0()
{

    while (UART0_FR_R & UART_FR_RXFE);               // wait if uart0 rx fifo empty
    return UART0_DR_R & 0xFF;                        // get character from fifo, masking off the flags
}

// Function gets string from the UART
void getsUart0(USER_DATA *structdata)
{
    int count = 0;
    char k;

    while (k = getcUart0())
            {


                switch(k)
                {

                    case 8:
                    case 127:    if(count>0)                                    // if character is backspace(8) or delete(127)
                                     {
                                         count--;
                                     }
                                 break;

                    case 10:
                    case 13:      structdata->buffer[count] =  0;           // if character is a carriage return(13) or linefeed(10)

                                     return;                                   // add NULL bite to the string



                    default: if(k >= 32)                                        // if character is a space or any other character
                                {

                                    structdata->buffer[count] = k;              // letter stored inside the string
                                    count++;
                                }

                             if(count == MAX_CHARS)                             // or reached the max# of characters
                                 {

                                         structdata->buffer[count] =  0;        // add NULL bite to the end of string
                                         return;
                                 }


                }

            }

}

int isita_character(char x)
{

       switch(x)
       {

                  case 97 ... 122:                                            // if character is  a - z
                  case 65 ... 90:                                             // if character is  A - Z
                  case 46:                                                    // if character is a period (.)

                                    return 1;

                  default:          return 0;                                  // if not a character return 0

       }

}

int isita_number(char x)
{

       switch(x)
      {

                 case 48 ... 57:                                               // if number is 0 - 9
                                  return 1;

                 default:         return 0;                                  // if not a number return 0

      }


}

int compare_2words(const char *word1,const char *word2)
{
    int i = 0;

    for(i = 0; word2[i] != '\0'; i++)         // while the first word hasnt reached the end of line
    {

        if ((word1[i] < word2[i]) || (word1[i] > word2[i]))             // if the letter isnt the same go ahead and exit with 0
        {
            return 0;
        }

    }

    return 1;                                                           // make it true if it is equal

}

uint32_t char_to_integer(char* x)
{

    uint32_t answer = 0;
    uint32_t checkingnum = isita_number(*x);
      while(checkingnum)
      {

          answer = (answer*10) + (*x - '0');

          x++;

          checkingnum = isita_number(*x);
      }


  return answer;


}

void int_to_ascii(int32_t number, char *ascii_of_number)
{
    int32_t exponent = 1000000;
    int check[7];
    int i;

    if(number < 0)
    {
        number *= -1;
    }
   // int temp;
    for( i = 0; i <  7; i++)
    {

         check[i] = number / exponent;

         number = number-(check[i]*exponent);

         ascii_of_number[i] = check[i] + '0';

         exponent = exponent/10;
    }

}

void parseFields(USER_DATA *structdata)
{

        int count = 0;
        int transition_counter = 0;
        char k;
        int state = 0;

        while ( k = structdata->buffer[count])
        {



                    switch(k)
                    {


                        case 97 ... 122:                                            // if character is  a - z
                        case 65 ... 90:                                             // if character is  A - Z
                        case 46:                                                    // if character is a period (.)
                                        if(state == 0 || state == 2)
                                        {
                                            structdata->fieldType[transition_counter] =  'a';       // 'a' variable marks that it is a letter or period

                                            structdata->fieldCount++;                     // keeping count of valid inputs

                                            structdata->fieldPosition[transition_counter] = count;   // record field position of transition state

                                            state = 1;

                                            count++;

                                            transition_counter++;
                                        }

                                        else if (state == 1)                        // same character entry
                                        {

                                            count ++;

                                        }
                                        break;


                        case 48 ... 57:                                             // if number is 0 - 9

                                    if(state == 0 || state == 1)
                                    {
                                         structdata->fieldType[transition_counter] =  'n';        // 'n' variable marks that it is a number

                                         structdata->fieldCount++;

                                         structdata->fieldPosition[transition_counter] = count;   // record field position of transition state


                                         state = 2;

                                         count ++;

                                         transition_counter++;

                                    }

                                    else if (state == 2)                            // using the same character
                                    {
                                          count ++;

                                    }

                                     break;

                        default:

                                        structdata->buffer[count] =  0;        //  makes delementer null

                                        count ++;

                                        state = 0;

                                        if(k == 0)                             // if reached the end
                                            {

                                                    return;
                                            }


                                 if(structdata->fieldCount == MAX_FIELDS)                             // or reached the max# of characters
                                     {

                                             structdata->buffer[count] =  0;        // add NULL bite to the end of string
                                             return;
                                     }
                                     break;

                    }

                }

}

char* getFieldString(USER_DATA* data, uint8_t fieldNumber)
{
    int offset = 0;

    if( fieldNumber < data->fieldCount)        // make sure data is in range
    {

        if(data->fieldType[fieldNumber] == 'a' )                        // check if the field type is a character to give back string
        {
            offset =  data->fieldPosition[fieldNumber] ;              // find the offset of the  field position through location in buffer

            return &data->buffer[offset];                               // return address of beginning of character
        }

    }

 return 0;                                                           // return null if not possibl
}

int32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber)
{
    int offset = 0;

    if( fieldNumber < data->fieldCount)        // make sure data is in range
    {

        if(data->fieldType[fieldNumber] == 'n' )                        // check if the field type is a character to give back string
        {
            offset =  data->fieldPosition[fieldNumber] ;              // find the offset of the  field position through location in buffer

            return char_to_integer(&data->buffer[offset]);                               // return number
        }



    }

    return 0;
}

bool isCommand(USER_DATA* data, const char strCommand[],uint8_t minArguments)
{

    int offset = data->fieldPosition[0];


    if ( compare_2words(strCommand, &data->buffer[offset]) && (data->fieldCount >=  minArguments) )
    {
        return true;
    }

    return false;
}
// Returns the status of the receive buffer
bool kbhitUart0()
{
    return !(UART0_FR_R & UART_FR_RXFE);
}

void number_to_uart(int32_t number)
{
    char print_value [8];
    int_to_ascii( number, print_value);
    putsUart0(print_value);

}




