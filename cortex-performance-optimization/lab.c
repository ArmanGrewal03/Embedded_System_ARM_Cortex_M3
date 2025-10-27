/***** COE718 Lab 2 — Bit-banding (no joystick, auto-rotate via SysTick) *****/
#include "LPC17xx.h"
#include <stdio.h>
#include "Board_LED.h"
#include "GLCD.h"          /* + added for LCD */

/* ---------------- Config ---------------- */
#define __FI        1                 /* 16x24 font for GLCD */
#define __USE_LCD   1                 /* set to 1 to enable LCD */
#define ROTATE_INTERVAL_MS  3000u     /* change mode every 3 seconds    */

/* ------- ITM (printf over SWO) ------- */
#define ITM_Port8(n)   (*((volatile unsigned char  *)(0xE0000000 + 4*(n))))
#define ITM_Port32(n)  (*((volatile unsigned long  *)(0xE0000000 + 4*(n))))
#define DEMCR          (*((volatile unsigned long  *)(0xE000EDFC)))
#define TRCENA         0x01000000

struct __FILE { int handle; };
FILE __stdout, __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) { while (ITM_Port32(0) == 0); ITM_Port8(0) = (unsigned char)ch; }
  return ch;
}

/* -------- Bit-band helpers -------- */
#define ADDRESS(x)      (*((volatile unsigned long *)(x)))
#define BitBand(x, y)   ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 | \
                                (((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))

/* Precomputed alias addresses (same as your working code) */
#define LED1_ALIAS (*(volatile unsigned long *)0x233806F0)   /* maps to P1.28 bit */
#define LED2_ALIAS (*(volatile unsigned long *)0x23380A88)   /* maps to P2.2  bit */

/* ---- Simple delay (visible LED toggles) ---- */
static void delay_ms(int ms) {
  if (ms <= 0) return;
  for (volatile int i = 0; i < ms * 8000; ++i) { __NOP(); } /* ~1ms-ish, depends on clock/opt */
}

/* ======================= Auto-rotate via SysTick (10 ms) ======================= */

typedef enum { M_MASK, M_FUNC, M_DIRECT, M_BARREL, M_MAX } Mode;

static volatile Mode     g_mode = M_MASK;
static volatile uint8_t  g_tick10ms_flag = 0;

#define ROTATE_TICKS        (ROTATE_INTERVAL_MS / 10u)   /* 10 ms tick -> N ticks per mode */

void SysTick_Handler(void){
  static uint32_t ms10 = 0;
  g_tick10ms_flag = 1;
  if (++ms10 >= ROTATE_TICKS) {
    ms10 = 0;
    g_mode = (Mode)((g_mode + 1) % M_MAX);
    ITM_Port8(0) = '.';               /* heartbeat on SWO each rotation */
  }
}

/* ---------------- LED methods (use simple if/else) ----------------
   NOTE: At -O3 the compiler typically emits an IT/ITE block for these if/else,
         so this satisfies the lab's "conditional execution" requirement.       */

static void led_Mask_mode(void) {
  printf("\nMask mode (if/else ? IT at -O3)\n");
  static int t = 0; t ^= 1;                  /* flip 0/1 each call */

  /* Using Board LED indices (mapping defined in Board_LED.c) */
  if (t == 0) LED_On(0);  else LED_Off(0);   /* LED index 0 */
  delay_ms(500);
  if (t == 0) LED_Off(3); else LED_On(3);    /* LED index 3 (opposite sense) */
  delay_ms(500);
}

static void led_function_mode(void) {
  printf("\nFunction bit-band (if/else ? IT at -O3)\n");
  static int t = 0; t ^= 1;

  volatile unsigned long *a1 = &BitBand(&LPC_GPIO1->FIOPIN, 28);  /* P1.28 alias */
  volatile unsigned long *a2 = &BitBand(&LPC_GPIO2->FIOPIN,  2);  /* P2.2  alias */

  if (t == 0) *a1 = 1; else *a1 = 0;         /* write 1/0 to alias */
  delay_ms(500);
  if (t == 0) *a2 = 0; else *a2 = 1;         /* opposite sense */
  delay_ms(500);
}

static void led_direct_bitband_mode(void) {
  printf("\nDirect bit-band (if/else ? IT at -O3)\n");
  static int t = 0; t ^= 1;

  if (t == 0) LED1_ALIAS = 1; else LED1_ALIAS = 0;  /* P1.28 alias */
  delay_ms(500);
  if (t == 0) LED2_ALIAS = 0; else LED2_ALIAS = 1;  /* P2.2  alias (opposite) */
  delay_ms(500);
}

/* ------------- Barrel shifter demo (LCD optional) ------------- */
static void method_barrel_shifter(void) {
  printf("\nBarrel shifter demo\n");
  uint8_t pattern = 0x01;
  int leds = (int)LED_GetCount();
  for (int i = 0; i < leds*2; ++i) {
    LED_SetOut((uint32_t)(pattern & ((1<<leds)-1)));
    printf("Barrel: %u(0x%02X) << 1 = %u(0x%02X)\n",
           pattern, pattern, (uint8_t)(pattern<<1), (uint8_t)(pattern<<1));
    delay_ms(200);
    pattern = (uint8_t)(pattern << 1);
    if (pattern == 0) pattern = 0x01;
  }
  LED_SetOut(0);
}

/* =============================== main =============================== */

int main (void){
  __enable_irq();                 /* ensure interrupts are on */

  LED_Initialize();
  printf("Init done\n");

  /* Ensure directions for raw GPIO pins used by function/direct modes */
  LPC_GPIO1->FIODIR |= (1U<<28);    /* P1.28 output */
  LPC_GPIO2->FIODIR |= (1U<<2);     /* P2.2  output */

  /* SysTick @ 10 ms via CMSIS system clock (requires system_LPC17xx.c + startup) */
  SystemCoreClockUpdate();
  if (SysTick_Config(SystemCoreClock / 100)) {   /* 100 Hz -> 10 ms */
    printf("SysTick config ERROR\n");
    while (1) { /* trap */ }
  }

#if __USE_LCD
  /* -------- LCD init + static header -------- */
  GLCD_Init();
  GLCD_Clear(White);
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, (unsigned char*)"   COE718 Lab 2   ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
  GLCD_DisplayString(2, 0, __FI, (unsigned char*)"Mode:             ");
#endif

  Mode last = (Mode)0xFF;

  while (1) {
    /* optional: check 10 ms flag for housekeeping/prints */
    if (g_tick10ms_flag) {
      g_tick10ms_flag = 0;
      if (g_mode != last) {
        last = g_mode;
        /* Console (SWO) */
        switch (g_mode) {
          case M_MASK:   printf("Mode: Masking\n");          break;
          case M_FUNC:   printf("Mode: Function BitBand\n"); break;
          case M_DIRECT: printf("Mode: Direct BitBand\n");   break;
          case M_BARREL: printf("Mode: Barrel Shifter\n");   break;
          default: break;
        }
#if __USE_LCD
        /* LCD update */
        GLCD_SetTextColor(Red);
        switch (g_mode) {
          case M_MASK:   GLCD_DisplayString(2, 6, __FI, (unsigned char*)"Masking        "); break;
          case M_FUNC:   GLCD_DisplayString(2, 6, __FI, (unsigned char*)"Function BB    "); break;
          case M_DIRECT: GLCD_DisplayString(2, 6, __FI, (unsigned char*)"Direct BB      "); break;
          case M_BARREL: GLCD_DisplayString(2, 6, __FI, (unsigned char*)"Barrel Shifter "); break;
        }
        GLCD_SetTextColor(Blue);
#endif
      }
    }

    /* Run the selected demo once. Each routine has visible delays. */
    switch (g_mode) {
      case M_MASK:   led_Mask_mode();            break;
      case M_FUNC:   led_function_mode();        break;
      case M_DIRECT: led_direct_bitband_mode();  break;
      case M_BARREL: method_barrel_shifter();    break;
      default: break;
    }

    /* Small idle to avoid hammering methods back-to-back */
    for (volatile uint32_t i = 0; i < 50000; ++i) __NOP();
  }

  /* not reached */
  // return 0;
}
