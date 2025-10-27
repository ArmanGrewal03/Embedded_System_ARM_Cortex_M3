#include "cmsis_os.h"
#include "Board_LED.h"
#include "LPC17xx.h"
#include "GLCD.h"

#define LED_COUNT     8
#define __FI          1


enum { N = 4096 };
static int g_arr[N];
uint32_t i;
int current=0;

static void Data_Init(void) {
  int i;
  for (i = 0; i < N; ++i) {
    g_arr[i] = i;
  }
}

void Thread1 (void const *);
void Thread2 (void const *);
void Thread3 (void const *);

osThreadId tid_Thread;
osThreadDef (Thread1, osPriorityNormal, 1, 0);

osThreadId tid2_Thread;
osThreadDef (Thread2, osPriorityNormal, 1, 0);

osThreadId tid3_Thread;
osThreadDef (Thread3, osPriorityNormal, 1, 0);

int Init_Thread (void) {
  LED_Initialize();

  Data_Init();

  tid_Thread  = osThreadCreate(osThread(Thread1), NULL);
  tid2_Thread = osThreadCreate(osThread(Thread2), NULL);
  tid3_Thread = osThreadCreate(osThread(Thread3), NULL);

  if (!tid_Thread || !tid2_Thread || !tid3_Thread) {
    return -1;
  }
  return 0;
}

void Thread1 (void const *argument) {
  const int ITERATIONS = 80;
  unsigned rng = 1u;
	uint32_t i;
  int t; (void)argument;

  for (t = 0; t < ITERATIONS; ++t) {
		LED_Off(current);
		current=0;
    LED_On(current);

    rng = rng * 1664525u + 1013904223u;
    GLCD_DisplayString(2, 0, __FI, (unsigned char*)"Thread: T1 (LIN) ");
    GLCD_DisplayString(3, 0, __FI, (unsigned char*)"Linear search...  ");
		for(i=0; i<9000000; i++){
			__NOP();
		}
    {
      int key = (int)(rng % (unsigned)N);
      int i;
      for (i = 0; i < N; ++i) {
        if (g_arr[i] == key) break;
      }
    }

    //LED_Off(0);
    osDelay(1);              /* yield via sleep (no osThreadYield) */
  }

  osThreadTerminate(NULL);
}

void Thread2 (void const *argument) {
  const int ITERATIONS = 60;
  unsigned rng = 2u;
  int t; (void)argument;

  for (t = 0; t < ITERATIONS; ++t) {
		LED_Off(current);
		current=1;
    LED_On(current);

    rng = rng * 1664525u + 1013904223u;
    GLCD_DisplayString(2, 0, __FI, (unsigned char*)"Thread: T2 (BIN) ");
    GLCD_DisplayString(3, 0, __FI, (unsigned char*)"Binary search...  ");
		for(i=0; i<9000000; i++){
			__NOP();
		}
    {
      int key = (int)(rng % (unsigned)N);
      int lo = 0, hi = N - 1;
      while (lo <= hi) {
        int mid = lo + ((hi - lo) >> 1);
        if (g_arr[mid] == key) break;
        if (g_arr[mid] < key) lo = mid + 1; else hi = mid - 1;
      }
    }

    //LED_Off(1);
    osDelay(1);              /* clean handoff point */
  }

  osThreadTerminate(NULL);
}

void Thread3 (void const *argument) {
  const int ITERATIONS = 30;
  const unsigned STEP = 64u;
  unsigned rng = 3u;
  int t; (void)argument;

  for (t = 0; t < ITERATIONS; ++t) {
		LED_Off(current);
		current=2;
    LED_On(current);

    rng = rng * 1664525u + 1013904223u;
    GLCD_DisplayString(2, 0, __FI, (unsigned char*)"Thread: T3 (JMP) ");
    GLCD_DisplayString(3, 0, __FI, (unsigned char*)"Jump search...    ");
		for(i=0; i<9000000; i++){
			__NOP();
		}
    {
      int key = (int)(rng % (unsigned)N);
      unsigned prev = 0u, curr = STEP;

      while (curr <= (unsigned)N) {
        if (g_arr[curr - 1] >= key) break;
        prev = curr; curr += STEP;
      }
      if (curr > (unsigned)N) curr = (unsigned)N;

      {
        unsigned i;
        for (i = prev; i < curr; ++i) {
          if (g_arr[i] == key) break;
        }
      }
    }

    //LED_Off(2);
    osDelay(1);              /* switch after LED off */
  }

  osThreadTerminate(NULL);
}
