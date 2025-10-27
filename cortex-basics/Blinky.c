/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 * Note(s): __USE_LCD   - enable Output on LCD, uncomment #define in code to use
 *  				for demo (NOT for analysis purposes)
 *----------------------------------------------------------------------------
 * Copyright (c) 2008-2011 Keil - An ARM Company.
 * Name: Anita Tino
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "Blinky.h"
#include "LPC17xx.h"                       
#include "GLCD.h"
#include "LED.h"
#include "Board_ADC.h" 
#include "KBD.h"


#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   0										/* Uncomment to use the LCD */

//ITM Stimulus Port definitions for printf //////////////////
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000


//FILE __stdout;
//FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}
/////////////////////////////////////////////////////////

char text[10];
char text_l[10];

static volatile uint16_t AD_dbg;

uint16_t ADC_last;                      // Last converted value
/* Import external variables from IRQ.c file                                  */
extern uint8_t  clock_ms;



/*////////// Joystick Methoods ///////////////////*/

void handleJoystick(uint32_t val) {
    if (val == KBD_MASK) {
        LED_Out(0x00);
        printf("Joystick none\n");
        return;
    }

    LED_Out(val);   
}

const char* joystickName(uint32_t val) {
    if (val == KBD_UP)     return "UP";
    if (val == KBD_DOWN)   return "DOWN";
    if (val == KBD_LEFT)   return "LEFT";
    if (val == KBD_RIGHT)  return "RIGHT";
    if (val == KBD_SELECT) return "SELECT";
    return "NONE";
}



/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  int32_t  res;
  uint32_t AD_sum   = 0U;
  uint32_t AD_cnt   = 0U;
  uint32_t AD_value = 0U;
  uint32_t AD_print = 0U;

  LED_Init();                                /* LED Initialization            */
  ADC_Initialize();                                /* ADC Initialization            */
	KBD_Init();																			/* Joysticck Initialization*/

#ifdef __USE_LCD
  GLCD_Init();
  GLCD_Clear(White);
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, (unsigned char *)"   Arman's COE718 Joystick Demo ");
  GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, (unsigned char *)"        LAB 1           ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  GLCD_DisplayString(3, 0, __FI, (unsigned char *)"Last Dir:");
  GLCD_SetTextColor(Green);
  GLCD_DisplayString(4, 0, __FI, (unsigned char *)"NONE            ");
#endif

  //SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock/100);       /* Generate interrupt each 10 ms */
	
	uint32_t last_buttons = 0U;   						/* Last joystick control */

  while (1) {                                /* Loop forever                  */

    /* AD converter input                                                     */
    // AD converter input
    res = ADC_GetValue();
    if (res != -1) {                     // If conversion has finished
      ADC_last = (uint16_t)res;
      
      AD_sum += ADC_last;                // Add AD value to sum
      if (++AD_cnt == 16U) {             // average over 16 values
        AD_cnt = 0U;
        AD_value = AD_sum >> 4;          // average devided by 16
        AD_sum = 0U;
      }
    }

    if (AD_value != AD_print) {
      AD_print = AD_value;               // Get unscaled value for printout
      AD_dbg   = (uint16_t)AD_value;

      sprintf(text, "0x%04X", AD_value); // format text for print out
		}	
		
			
		//Joystick input
		uint32_t joystick_val = get_button();
		handleJoystick(joystick_val);

			
    
			
#ifdef __USE_LCD
			GLCD_SetTextColor(Blue);
      GLCD_DisplayString(4, 0, __FI, (unsigned char *)"                "); /* clear line */
      GLCD_SetTextColor(Green);
      GLCD_DisplayString(4, 0, __FI, (unsigned char *)joystickName(joystick_val));
			
	
#endif
    
			/*
      GLCD_SetTextColor(Red);
      GLCD_DisplayString(6,  9, __FI,  (unsigned char *)text);
			GLCD_SetTextColor(Green);
      GLCD_Bargraph (144, 7*24, 176, 20, (AD_value >> 2));
			*/
		

    /* Print message with AD value every 10 ms                               */
    if (clock_ms) {
      clock_ms = 0;
			
			printf("Joystick: %s\n", joystickName(joystick_val));

      printf("AD value: %s\r\n", text);
    }
  }
}
