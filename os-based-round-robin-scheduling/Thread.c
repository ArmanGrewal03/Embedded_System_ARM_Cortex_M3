#include "cmsis_os.h"
#include "LPC17xx.h"
#include <string.h>
#include "Board_LED.h"
#include "GLCD.h"

#define __FI 1

/* globals */
size_t have;
int r1, r2, r3;
volatile unsigned long * bit;
volatile unsigned mm_access_cnt = 0;
volatile unsigned cpu_access_cnt = 0;
volatile unsigned app_cnt = 0;
volatile unsigned dev_cnt = 0;
volatile unsigned ui_users = 0;
char logger[128];
int current_led=0;

/* Bit Band Macros */
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) ADDRESS(((unsigned long)(x) & 0xF0000000UL) | 0x02000000UL | (((unsigned long)(x) & 0x000FFFFFUL) << 5) | ((y) << 2))

/* signals */
#define MEM_CPU  0x01
#define CPU_MEM  0x02
#define APP_DEV  0x04  
#define DEV_APP  0x08

/*----------------------------------------------------------------------------
 *      Threads
 *---------------------------------------------------------------------------*/
void Memory_Management (void const *argument);
void CPU_Management    (void const *argument);
void App_Interface     (void const *argument);
void Device_Management (void const *argument);
void User_Interface    (void const *argument);

osThreadId tid_Thread;
osThreadDef (Memory_Management, osPriorityNormal, 1, 0);

osThreadId tid2_Thread;
osThreadDef (CPU_Management, osPriorityNormal, 1, 0);

osThreadId tid3_Thread;
osThreadDef (App_Interface, osPriorityNormal, 1, 0);

osThreadId tid4_Thread;
osThreadDef (Device_Management, osPriorityNormal, 1, 0);

osThreadId tid5_Thread;
osThreadDef (User_Interface, osPriorityNormal, 1, 0);

/* sync primitives */
osMutexId   log_lock;
osMutexDef  (log_lock);


int Init_Thread (void) {

  log_lock    = osMutexCreate(osMutex(log_lock));

  tid_Thread  = osThreadCreate(osThread(Memory_Management), NULL);
  tid2_Thread = osThreadCreate(osThread(CPU_Management), NULL);
  tid3_Thread = osThreadCreate(osThread(App_Interface), NULL);
  tid4_Thread = osThreadCreate(osThread(Device_Management), NULL);
  tid5_Thread = osThreadCreate(osThread(User_Interface), NULL);

  if (!tid_Thread) return -1;
  return 0;
}

void Memory_Management (void const *argument) {
	LED_Off(current_led);
	current_led=0;
	LED_On(current_led);
  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(2, 8, __FI, (unsigned char*)" MEM       ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"Mem mgmt running  ");

  mm_access_cnt += 1;

  bit = &BitBand(0x2009c034, 28);

  osSignalSet(tid2_Thread, MEM_CPU);
  osSignalWait(CPU_MEM, osWaitForever);

  LED_Off(current_led);
	current_led=0;
	LED_On(current_led);
	  GLCD_SetTextColor(Blue);
	GLCD_DisplayString(2, 8, __FI, (unsigned char*)"Thread: MEM       ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"Mem mgmt running  ");

  osDelay(1);
  osThreadTerminate(NULL);
}

void CPU_Management (void const *argument) {
	osSignalWait(MEM_CPU, osWaitForever);
	LED_Off(current_led);
	current_led=1;
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(2, 8, __FI, (unsigned char*)"Thread: CPU       ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"CPU mgmt running  ");
  LED_On(current_led);
  cpu_access_cnt += 1;

  r1 = 1; r2 = 0; r3 = 5;
  while (r2 <= 0x18) {
    if ((r1 - r2) > 0) {
      r1 = r1 + 2;
      r2 = r1 + (r3 * 4);
      r3 = r3 / 2;
    } else {
      r2 = r2 + 1;
    }
  }
  (void)r1; (void)r2; (void)r3;

  osSignalSet(tid_Thread, CPU_MEM);

  osThreadTerminate(NULL);
}

void App_Interface (void const *argument) {
	osMutexWait(log_lock, osWaitForever);
	LED_Off(current_led);
	current_led=2;
	LED_On(current_led);
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(2, 8, __FI, (unsigned char*)"Thread: APP       ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"App iface running ");
  strncpy(logger, "APP: start ", sizeof(logger) - 1);
  osMutexRelease(log_lock);
	osSignalSet(tid4_Thread,APP_DEV);
  osSignalWait(DEV_APP, osWaitForever);
	LED_Off(current_led);
	  GLCD_SetTextColor(Blue);

	GLCD_DisplayString(2, 8, __FI, (unsigned char*)"Thread: APP       ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"App iface running ");
	
	current_led=2;
	LED_On(current_led);
	
	
  app_cnt += 1;


  osDelay(1);
  osThreadTerminate(NULL);
}

void Device_Management (void const *argument) {
	osSignalWait(APP_DEV, osWaitForever);
	osMutexWait(log_lock, osWaitForever);

	LED_Off(current_led);
	current_led=3;
	LED_On(current_led);
    GLCD_SetTextColor(Blue);

  GLCD_DisplayString(2, 8, __FI, (unsigned char*)"Thread: DEV       ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"Device running    ");

  have = strlen(logger);
  strncat(logger, " DEVICE:done", (sizeof(logger) - 1) - have);
  logger[sizeof(logger) - 1] = '\0';
  osMutexRelease(log_lock);

  osSignalSet(tid3_Thread, DEV_APP);

  dev_cnt += 1;
  osDelay(1);
  osThreadTerminate(NULL);
}

void User_Interface (void const *argument) {
	LED_Off(current_led);
	current_led=4;
	LED_On(current_led);
  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(2, 8, __FI, (unsigned char*)"Thread: UI        ");
	  GLCD_SetTextColor(Blue);

  GLCD_DisplayString(3, 0, __FI, (unsigned char*)"User iface run    ");

  ui_users += 1U;

  LED_Off(4);
  osDelay(1);
  osThreadTerminate(NULL);
}
