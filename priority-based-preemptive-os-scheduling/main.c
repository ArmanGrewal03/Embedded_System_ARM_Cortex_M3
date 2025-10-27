/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "GLCD.h"
#define LCD_ON 1
#include "Board_LED.h"

#define __FI        1                      /* Font index 16x24               */
 
extern int Init_Thread (void);
 
/*
 * main: initialize and start the system
 */
int main (void) {	
	  /* -------- LCD init + static header -------- */
	LED_Initialize();
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	GLCD_Init();
  GLCD_Clear(White);
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, (unsigned char*)"   COE718 Lab 3b PT2   ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
	GLCD_DisplayString(2, 0, __FI, (unsigned char*)"Thread:             ");
 	Init_Thread();
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}
