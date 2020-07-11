//--------------------------------------------------------------
// File     : stm32_ub_ws2812.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_WS2812_H
#define __STM32F4_UB_WS2812_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_Hal_dma.h"
//#include "misc.h"


//--------------------------------------------------------------
// Anzahl der WS2812-LEDs (1...n) in den einzelnen Ketten 
// CPU -> LED_1 -> LED_2 -> LED_3 -> LED_n
//
// falls ein Channel nicht benutzt wird => laenge auf 0 setzen
//--------------------------------------------------------------
#define  WS2812_LED_CH1_ANZ    5  // [CH1 an PC6] LED-Kette mit 5 LEDs
#define  WS2812_LED_CH2_ANZ    0  // [CH2 an PB5] CH2 wird nicht benutzt
#define  WS2812_LED_CH3_ANZ    0  // [CH3 an PB0] CH3 wird nicht benutzt
#define  WS2812_LED_CH4_ANZ    0  // [CH4 an PB1] CH4 wird nicht benutzt

#define  WS2812_LED_MAX_ANZ    5  // anzahl der LEDs in der laengsten Kette



//--------------------------------------------------------------
// check der laengenangaben
//--------------------------------------------------------------
#if WS2812_LED_CH1_ANZ>WS2812_LED_MAX_ANZ
  #error wrong len
#endif
#if WS2812_LED_CH2_ANZ>WS2812_LED_MAX_ANZ
  #error wrong len
#endif
#if WS2812_LED_CH3_ANZ>WS2812_LED_MAX_ANZ
  #error wrong len
#endif
#if WS2812_LED_CH4_ANZ>WS2812_LED_MAX_ANZ
  #error wrong len
#endif

#if (WS2812_LED_MAX_ANZ>WS2812_LED_CH1_ANZ) && (WS2812_LED_MAX_ANZ>WS2812_LED_CH2_ANZ)
  #if (WS2812_LED_MAX_ANZ>WS2812_LED_CH3_ANZ) && (WS2812_LED_MAX_ANZ>WS2812_LED_CH4_ANZ)
    #error wrong len
  #endif
#endif




//--------------------------------------------------------------
// benutzer Timer fuer das Daten-Signal => TIM3
//--------------------------------------------------------------
#define  WS2812_TIM_CLOCK     RCC_APB1Periph_TIM3
#define  WS2812_TIM           TIM3
#define  WS2812_TIM_AF        GPIO_AF2_TIM3
#define  WS2812_TIM_CH1       1 
#define  WS2812_TIM_CCR_REG1  TIM3->CCR1
#define  WS2812_TIM_DMA_TRG1  TIM_DMA_CC1
#define  WS2812_TIM_CH2       2 
#define  WS2812_TIM_CCR_REG2  TIM3->CCR2
#define  WS2812_TIM_DMA_TRG2  TIM_DMA_CC2
#define  WS2812_TIM_CH3       3 
#define  WS2812_TIM_CCR_REG3  TIM3->CCR3
#define  WS2812_TIM_DMA_TRG3  TIM_DMA_CC3
#define  WS2812_TIM_CH4       4 
#define  WS2812_TIM_CCR_REG4  TIM3->CCR4
#define  WS2812_TIM_DMA_TRG4  TIM_DMA_CC4



//--------------------------------------------------------------
// GPIO-Pins (CH1...CH4) fuer Data-OUT 
//
// moegliche Pinbelegungen (bei TIM3)
//   CH1 : [PA6, PB4, PC6]
//   CH2 : [PA7, PB5, PC7]
//   CH3 : [PB0, PC8]
//   CH4 : [PB1, PC9] 
//--------------------------------------------------------------
#define  WS2812_CH1_CLOCK    RCC_AHB1Periph_GPIOC
#define  WS2812_CH1_PORT     GPIOC
#define  WS2812_CH1_PIN      GPIO_Pin_6 
#define  WS2812_CH1_SOURCE   GPIO_PinSource6 

#define  WS2812_CH2_CLOCK    RCC_AHB1Periph_GPIOB
#define  WS2812_CH2_PORT     GPIOB
#define  WS2812_CH2_PIN      GPIO_Pin_5 
#define  WS2812_CH2_SOURCE   GPIO_PinSource5

#define  WS2812_CH3_CLOCK    RCC_AHB1Periph_GPIOB
#define  WS2812_CH3_PORT     GPIOB
#define  WS2812_CH3_PIN      GPIO_Pin_0 
#define  WS2812_CH3_SOURCE   GPIO_PinSource0

#define  WS2812_CH4_CLOCK    RCC_AHB1Periph_GPIOB
#define  WS2812_CH4_PORT     GPIOB
#define  WS2812_CH4_PIN      GPIO_Pin_1 
#define  WS2812_CH4_SOURCE   GPIO_PinSource1



//--------------------------------------------------------------
// benutzer DMA
//   => TIM3-CC1 => DMA1, Channel5, Stream4
//   => TIM3-CC2 => DMA1, Channel5, Stream5
//   => TIM3-CC3 => DMA1, Channel5, Stream7
//   => TIM3-CC4 => DMA1, Channel5, Stream2
// (siehe Seite 216+217 vom Referenz Manual)
//--------------------------------------------------------------
#define  WS2812_DMA_CLOCK         RCC_APB1Periph_TIM3

#define  WS2812_DMA_CH1_STREAM    DMA1_Stream4
#define  WS2812_DMA_CH1_CHANNEL   DMA_CHANNEL_5

#define  WS2812_DMA_CH2_STREAM    DMA1_Stream5
#define  WS2812_DMA_CH2_CHANNEL   DMA_CHANNEL_5

#define  WS2812_DMA_CH3_STREAM    DMA1_Stream7
#define  WS2812_DMA_CH3_CHANNEL   DMA_CHANNEL_5

#define  WS2812_DMA_CH4_STREAM    DMA1_Stream2
#define  WS2812_DMA_CH4_CHANNEL   DMA_CHANNEL_5



//--------------------------------------------------------------
// Transfer-Complete Interrupt
//   CC1 => DMA1, Stream4
//   CC2 => DMA1, Stream5
//   CC3 => DMA1, Stream7
//   CC4 => DMA1, Stream2
//--------------------------------------------------------------
#define  WS2812_DMA_CH1_IRQn      DMA1_Stream4_IRQn
#define  WS2812_DMA_CH1_ISR       DMA1_Stream4_IRQHandler
#define  WS2812_DMA_CH1_IRQ_FLAG  DMA_HISR_TCIF4

#define  WS2812_DMA_CH2_IRQn      DMA1_Stream5_IRQn
#define  WS2812_DMA_CH2_ISR       DMA1_Stream5_IRQHandler
#define  WS2812_DMA_CH2_IRQ_FLAG  DMA_HISR_TCIF5

#define  WS2812_DMA_CH3_IRQn      DMA1_Stream7_IRQn
#define  WS2812_DMA_CH3_ISR       DMA1_Stream7_IRQHandler
#define  WS2812_DMA_CH3_IRQ_FLAG  DMA_HISR_TCIF7

#define  WS2812_DMA_CH4_IRQn      DMA1_Stream2_IRQn
#define  WS2812_DMA_CH4_ISR       DMA1_Stream2_IRQHandler
#define  WS2812_DMA_CH4_IRQ_FLAG  DMA_HISR_TCIF4



//--------------------------------------------------------------
// RGB LED Farbdefinition (3 x 8bit)
//--------------------------------------------------------------
typedef struct {
  uint8_t red;    // 0...255 (als PWM-Wert)
  uint8_t green;  // 0...255 (als PWM-Wert)
  uint8_t blue;   // 0...255 (als PWM-Wert)
}WS2812_RGB_t;


//--------------------------------------------------------------
// HSV LED Farbdefinition
//--------------------------------------------------------------
typedef struct {
  uint16_t h;     // 0...359 (in Grad, 0=R, 120=G, 240=B)
  uint8_t s;      // 0...100 (in Prozent)
  uint8_t v;      // 0...100 (in Prozent)
}WS2812_HSV_t;



//--------------------------------------------------------------
// Globale Buffer fuer die Farben (als RGB-Wert)
//--------------------------------------------------------------
#if WS2812_LED_CH1_ANZ>0
  WS2812_RGB_t WS2812_LED_BUF_CH1[WS2812_LED_CH1_ANZ];
#endif

#if WS2812_LED_CH2_ANZ>0
  WS2812_RGB_t WS2812_LED_BUF_CH2[WS2812_LED_CH2_ANZ];
#endif

#if WS2812_LED_CH3_ANZ>0
  WS2812_RGB_t WS2812_LED_BUF_CH3[WS2812_LED_CH3_ANZ];
#endif

#if WS2812_LED_CH4_ANZ>0
  WS2812_RGB_t WS2812_LED_BUF_CH4[WS2812_LED_CH4_ANZ];
#endif



//--------------------------------------------------------------
// standard Farben (R,G,B)
//--------------------------------------------------------------
#define  WS2812_RGB_COL_OFF      (WS2812_RGB_t) {0x00,0x00,0x00}

#define  WS2812_RGB_COL_BLUE     (WS2812_RGB_t) {0x00,0x00,0xFF}
#define  WS2812_RGB_COL_GREEN    (WS2812_RGB_t) {0x00,0xFF,0x00}
#define  WS2812_RGB_COL_RED      (WS2812_RGB_t) {0xFF,0x00,0x00}
#define  WS2812_RGB_COL_WHITE    (WS2812_RGB_t) {0xFF,0xFF,0xFF}

#define  WS2812_RGB_COL_CYAN     (WS2812_RGB_t) {0x00,0xFF,0xFF}
#define  WS2812_RGB_COL_MAGENTA  (WS2812_RGB_t) {0xFF,0x00,0xFF}
#define  WS2812_RGB_COL_YELLOW   (WS2812_RGB_t) {0xFF,0xFF,0x00}


//--------------------------------------------------------------
// standard Farben (H,S,V)
//--------------------------------------------------------------
#define  WS2812_HSV_COL_OFF      (WS2812_HSV_t) {0,  0,  0}

#define  WS2812_HSV_COL_BLUE     (WS2812_HSV_t) {240,100,100}
#define  WS2812_HSV_COL_GREEN    (WS2812_HSV_t) {120,100,100}
#define  WS2812_HSV_COL_RED      (WS2812_HSV_t) {0,  100,100}

#define  WS2812_HSV_COL_CYAN     (WS2812_HSV_t) {180,100,100}
#define  WS2812_HSV_COL_MAGENTA  (WS2812_HSV_t) {300,100,100}
#define  WS2812_HSV_COL_YELLOW   (WS2812_HSV_t) {60, 100,100}


//--------------------------------------------------------------
// WS2812 Timing : (1.25us = 800 kHz)
//   logische-0 => HI:0.35us , LO:0.90us
//   logische-1 =  HI:0.90us , LO:0.35us
//
// WS23812 Bit-Format : (8G8R8B)
//   24bit pro LED  (30us pro LED)
//    8bit pro Farbe (MSB first)
//    Farbreihenfolge : GRB
//      
//   nach jedem Frame von n-LEDs kommt eine Pause von >= 50us
//
// Grundfrequenz (TIM3) = 2*APB1 (APB1=42MHz) => TIM_CLK=84MHz
// periode   : 0 bis 0xFFFF
// prescale  : 0 bis 0xFFFF
//
// PWM-Frq = TIM_CLK/(periode+1)/(vorteiler+1)
//-------------------------------------------------------------- 
#define  WS2812_TIM_PRESCALE    0  // F_T3  = 84 MHz (11.9ns)
#define  WS2812_TIM_PERIODE   104  // F_PWM = 80 kHz (1.25us)

#define  WS2812_LO_TIME        29  // 29 * 11.9ns = 0.34us
#define  WS2812_HI_TIME        76  // 76 * 11.9ns = 0.90us





//--------------------------------------------------------------
// defines vom WS2812 (nicht abaendern)
//--------------------------------------------------------------
#define  WS2812_BIT_PER_LED    24  // 3*8bit pro LED
#define  WS2812_PAUSE_ANZ       2  // fuer Pause (2*30us)

#define  WS2812_TIMER_BUF_LEN1   (WS2812_LED_CH1_ANZ+WS2812_PAUSE_ANZ)*WS2812_BIT_PER_LED
#define  WS2812_TIMER_BUF_LEN2   (WS2812_LED_CH2_ANZ+WS2812_PAUSE_ANZ)*WS2812_BIT_PER_LED
#define  WS2812_TIMER_BUF_LEN3   (WS2812_LED_CH3_ANZ+WS2812_PAUSE_ANZ)*WS2812_BIT_PER_LED
#define  WS2812_TIMER_BUF_LEN4   (WS2812_LED_CH4_ANZ+WS2812_PAUSE_ANZ)*WS2812_BIT_PER_LED
#define  WS2812_TIMER_BUF_LEN    (WS2812_LED_MAX_ANZ+WS2812_PAUSE_ANZ)*WS2812_BIT_PER_LED





//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void UB_WS2812_Init(void);
void UB_WS2812_SetChannel(uint8_t ch);
void UB_WS2812_Refresh(void);
void UB_WS2812_RGB_2_HSV(WS2812_HSV_t hsv_col, WS2812_RGB_t *rgb_col);
void UB_WS2812_One_Led_RGB(uint32_t nr, WS2812_RGB_t rgb_col, uint8_t refresh);
void UB_WS2812_All_Led_RGB(WS2812_RGB_t rgb_col, uint8_t refresh);
void UB_WS2812_One_Led_HSV(uint32_t nr, WS2812_HSV_t hsv_col, uint8_t refresh);
void UB_WS2812_All_Led_HSV(WS2812_HSV_t hsv_col, uint8_t refresh);
void UB_WS2812_Shift_Left(void);
void UB_WS2812_Shift_Right(void);
void UB_WS2812_Rotate_Left(void);
void UB_WS2812_Rotate_Right(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_WS2812_H
