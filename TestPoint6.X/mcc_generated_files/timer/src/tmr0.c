/**
 * TMR0 Generated Driver File
 * 
 * @file tmr0.c
 * 
 * @ingroup tmr0
 * 
 * @brief This is the generated driver implementation file for the TMR0 driver.
 *
 * @version TMR0 Driver Version 2.0.0
*/
/*
� [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <xc.h>
#include "../tmr0.h"

volatile uint8_t timer0ReloadVal;

const struct TMR_INTERFACE Timer0 = {
    .Initialize = Timer0_Initialize, 
    .Start = NULL,
    .Stop = NULL,
    .PeriodCountSet = Timer0_PeriodCountSet,
    .TimeoutCallbackRegister = Timer0_OverflowCallbackRegister,
    .Tasks = NULL
};

static void (*Timer0_OverflowCallback)(void);
static void Timer0_DefaultOverflowCallback(void);

void Timer0_Initialize(void)
{
  // PS 1:256; PSA assigned; TMRSE Increment_hi_lo; TMRCS FOSC/4; mask the nWPUEN and INTEDG bits
  OPTION_REG = (uint8_t)((OPTION_REG & 0xC0) | (0xD7 & 0x3F));

  // TMR 6; 
  TMR0 = 0x6;

  // Load the TMR value to reload variable
  timer0ReloadVal = 6;

  //Set default callback for TMR0 overflow interrupt
  Timer0_OverflowCallbackRegister(Timer0_DefaultOverflowCallback);

  //Clear Interrupt flag before enabling the interrupt
  INTCONbits.TMR0IF = 0;
	
  //Enable TMR0 interrupt.
  INTCONbits.TMR0IE = 1;
	
}

uint8_t Timer0_Read(void)
{
  uint8_t readVal;

  readVal = TMR0;

  return readVal;
}

void Timer0_Write(size_t timerVal)
{
  // Write to the Timer0 register
  TMR0 = (uint8_t) timerVal;
}

void Timer0_Reload(void)
{
  // Write to the Timer0 register
  TMR0 = timer0ReloadVal;
}

void Timer0_PeriodCountSet(size_t periodVal)
{
  timer0ReloadVal = (uint8_t) periodVal;
}

void Timer0_OverflowISR(void)
{
  static volatile uint16_t CountCallBack = 0;

  //Clear the TMR0 interrupt flag
  INTCONbits.TMR0IF = 0;

  TMR0 = timer0ReloadVal;

  //Callback function - called every 5th pass
  if (++CountCallBack >= Timer0_INTERRUPT_TICKER_FACTOR)
  {
    //Ticker function call
    if(Timer0_OverflowCallback)
    {
      Timer0_OverflowCallback();
    }
      //Reset ticker counter
      CountCallBack = 0;
    }
}

void Timer0_OverflowCallbackRegister(void (* CallbackHandler)(void))
{
  Timer0_OverflowCallback = CallbackHandler;
}

static void Timer0_DefaultOverflowCallback(void)
{
  //Add your interrupt code here or
  //Use Timer0_OverflowCallbackRegister function to use Custom ISR
}
