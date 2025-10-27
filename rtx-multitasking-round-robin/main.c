/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "GLCD.h"
#include "Board_LED.h"

#define __FI        1                      /* Font index 16x24               */
 
extern int Init_Thread (void);
 

/*
 * main: initialize and start the system
 */
int main (void) {
	
  /* -------- LCD init + static header -------- */
  GLCD_Init();
  GLCD_Clear(White);
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, (unsigned char*)"   COE718 Lab 3   ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  GLCD_DisplayString(2, 0, __FI, (unsigned char*)"Thread:             ");
	
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS
 	if (Init_Thread() != 0) {
    for(;;) { /* creation failed */ }
  }
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}
