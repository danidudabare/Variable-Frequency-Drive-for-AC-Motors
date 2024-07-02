/* MIT License

Original code by Matan Pazi
Copyright (c) 2023 Matan Pazi

Modifications by Lahiru Cooray
Copyright (c) 2024 Lahiru Cooray

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

#define F_CPU 16000000UL
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "TM1637Display.h"

#define CLK1 12
#define DIO1 2
#define CLK2 7
#define DIO2 4
#define CURR_INPUT A0
#define POT_INPUT A2
//
#define THREE_PH 0
#define PWM_RUNNING 2
#define PWM_NOT_RUNNING 1
#define PWM_NOT_SET 0
#define DEADTIME_ADD  5
#define DEADTIME_SUB  2
#define SHORT_CLICK     10
#define LONG_CLICK      500
#define POT_SWITCH_SAMPLES  2
#define BOOT_CAP_CHARGE_TIME   160         
#define RELAY_CHARGE_WAIT      2000000     
#define DISPLAY_BLINK   100
#define MIN_PWM_VAL   6                   
#define BUTTON_IS_PRESSED      (!((PINC >> PINC4) & 1))      
#define POT_SWITCH_IS_ON       (!((PINC >> PINC3) & 1))      


const uint8_t THREE_PHASE[] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,            
    SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,            
    SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,            
    0,                                                
    };
const uint8_t SPACE[] = {
    0,                                                
    }; 

const float Sine[] = {125.0,179.0,223.0,246.0,246.0,223.0,179.0,125.0,71.0,27.0,6.0,6.0,27.0,71.0,125.0};
const uint8_t Sine_Len = 15;              
const uint8_t Min_Freq = 20;              
const uint8_t Max_Freq = 120;             
const uint16_t Base_Freq = 1041;          
const float Min_Amp = 0.1;                
const float Max_Amp = 1.0;                
const float V_f = 1.66667;                
const float VBus = 230.0;                 
bool  Phase_Config = 0;                   
bool  Config_Editable = 0;               
int16_t Sine_Used[] = {125,179,223,246,246,223,179,125,71,27,6,6,27,71,125};
uint8_t  Click_Type = 0;                  
uint8_t  PWM_Running = PWM_NOT_SET;       
uint32_t Timer = 0;                      
uint32_t Click_Timer = 0;                 
uint32_t Pot_Switch_Off_Timer = 0;        
uint32_t Pot_Switch_On_Timer = 0;         
uint32_t Display_Timer = 0;               
uint32_t Timer_Temp = 0;                  
uint32_t Timer_Temp1 = 0;                 
uint32_t Init_PWM_Counter = 0;            
uint16_t Curr_Value = 0;                  
uint8_t Sine_Index = 0;                   
uint8_t Sine_Index_120 = Sine_Len / 3;
uint8_t Sine_Index_240 = (Sine_Len * 2) / 3;       
uint8_t OVF_Counter = 0;                                                   
uint8_t OVF_Counter_Compare = 0;          


TM1637Display Display1(CLK1, DIO1);
TM1637Display Display2(CLK2, DIO2);


void Button_Click();
void Display(uint8_t PWM_Running, bool Blink, uint8_t Desired_Freq);
void Pot_Switch_State_Check();
void Reverse_3_Phase();
void Wait_A_Bit(uint32_t Executions_To_Wait);
void Pwm_Disable();
void Pwm_Config();
void ADC_init();
uint16_t analogRead(uint8_t channel) ;



void setup()
{
  cli();                                      
  CLKPR = (1 << CLKPCE);                      
  CLKPR = (1 << CLKPS0);                      
  sei();
  Display1.setBrightness(0x02);
  Display2.setBrightness(0x02);
  Display1.clear();
  Display2.clear();
  PORTC = (1 << PORTC3) | (1 << PORTC4);         
  DDRD = (1 << DDD6) | (1 << DDD5) | (1 << DDD3); 
  DDRB = (1 << DDB3) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0); 
                                                                
  Wait_A_Bit(RELAY_CHARGE_WAIT);              
                                              
  PORTB = (1 << PORTB0);                      
}


void loop()
{   
   uint8_t Desired_Freq;                                 
   uint8_t OVF_Counter_Compare_Temp;                     
   float Amp; 
   ADC_init();
   Curr_Value = analogRead(0) << 3;                 
   Desired_Freq = ((uint8_t)(analogRead(2) >> 3));   
   if (Desired_Freq < Min_Freq) Desired_Freq = Min_Freq;
   else if (Desired_Freq > Max_Freq) Desired_Freq = Max_Freq;
   OVF_Counter_Compare_Temp = (uint8_t)(Base_Freq / Desired_Freq);                     
   {
      OVF_Counter_Compare = OVF_Counter_Compare_Temp;       
                                                                                                  
   }
   Amp = ((float)(Desired_Freq) * V_f) / VBus;                      
   if (Amp < Min_Amp) Amp = Min_Amp;      
   else if (Amp > Max_Amp) Amp = Max_Amp;                 
   {
      for (int i = 0; i < Sine_Len; i++)
      {
         Sine_Used[i] = (int16_t)(Amp * Sine[i]); 
      }
   }

   Pot_Switch_State_Check();
   if (BUTTON_IS_PRESSED) Button_Click();
   if (PWM_Running != PWM_NOT_SET) Display(PWM_Running, Config_Editable, Desired_Freq);
   Timer++;    
}


int main()
{
    setup();
    while (1)
    {
        loop();
    }
    return 0;
}


void Button_Click()
{
  if (BUTTON_IS_PRESSED)
  {
    if (Timer - Timer_Temp1  > 1) Click_Timer = 0;    
    else Click_Timer++;
    Timer_Temp1 = Timer;
    if (Click_Timer > SHORT_CLICK)
    {
      Reverse_3_Phase();    
    }
    Click_Timer = 0;
  } 

  else Click_Timer = 0;
}


void Display(uint8_t PWM_Running, bool Blink, uint8_t Desired_Freq)
{
  if (PWM_Running == PWM_RUNNING)
  {
    if (Desired_Freq > 99) Display1.showNumberDec(Desired_Freq, false, 3, 1);
    else 
    {
      Display1.setSegments(SPACE, 1, 1);
      Display1.showNumberDec(Desired_Freq, false, 2, 2);         
    }
    if (Curr_Value > 999) Display2.showNumberDec(Curr_Value, false, 4, 0);
    else 
    {
      Display2.setSegments(SPACE, 1, 0);
      Display2.showNumberDec(Curr_Value, false, 3, 1);         
    }
  }
  else
  {
    Display_Timer++;
    if (Blink && (Display_Timer == DISPLAY_BLINK))
    {
      Display1.clear();
      Display2.clear();
    }
    else if (Display_Timer > (2*DISPLAY_BLINK))
    { 
      Display1.setSegments(THREE_PHASE); 
      Display_Timer = 0;   
    }
  }
}


void Wait_A_Bit(uint32_t Executions_To_Wait)
{
  volatile uint32_t Timer_Temp2 = 0;
  while (Timer_Temp2 < Executions_To_Wait)
  {
    Timer_Temp2++;
  }
}


void Pwm_Disable()
{
   PWM_Running = PWM_NOT_RUNNING;
   cli();
   Init_PWM_Counter = 0;
   TCCR0A = 0;
   TCCR0B = 0;
   TCCR1A = 0;
   TCCR1B = 0;
   TCCR2A = 0;
   TCCR2B = 0;
   sei();    
}

void Pwm_Config()
{
   if (Phase_Config == THREE_PH)  
   {
       cli();                      
       PWM_Running = PWM_RUNNING;
       GTCCR = (1<<TSM)|(1<<PSRASY)|(1<<PSRSYNC); 
       TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << COM0B0) | (1 << WGM00); 
       TCCR0B = (1 << CS00);      
       TIMSK0 = (1 << TOIE0);     
       OCR0A = 0;     
       OCR0B = 127;   
       TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM10); 
       TCCR1B = (1 << CS10);      
       OCR1A = 0;     
       OCR1B = 127;   
       TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << COM2B0) | (1 << WGM20); 
       TCCR2B = (1 << CS20);       
       OCR2A = 0;    
       OCR2B = 127;  
       TCNT0 = 0;     
       TCNT1H = 0;    
       TCNT1L = 0;    
       TCNT2 = 0;         
       GTCCR = 0;     
       Init_PWM_Counter = 0;
       Wait_A_Bit(BOOT_CAP_CHARGE_TIME);
       sei();      
   }

}


void Pot_Switch_State_Check()    
{
   if (POT_SWITCH_IS_ON)
   {      
      if (PWM_Running != PWM_RUNNING)   
      {
        if (Timer - Timer_Temp  > 1) Pot_Switch_On_Timer = 0;   
        else  Pot_Switch_On_Timer++;      
        Timer_Temp = Timer;
        Pot_Switch_Off_Timer = 0;
        if (Pot_Switch_On_Timer > POT_SWITCH_SAMPLES)  
        {
          Pwm_Config();      
          Pot_Switch_On_Timer = 0;
          Pot_Switch_Off_Timer = 0;      
          Display1.clear();
          Display2.clear();
        }         
      }
   }
   else          
   {
      if (PWM_Running != PWM_NOT_RUNNING)     
      {
        if (Timer - Timer_Temp  > 1) Pot_Switch_Off_Timer = 0;  
        else  Pot_Switch_Off_Timer++;      
        Timer_Temp = Timer;
        Pot_Switch_On_Timer = 0;
        if (Pot_Switch_Off_Timer > POT_SWITCH_SAMPLES)  
        {
          Pwm_Disable();
          Pot_Switch_On_Timer = 0;
          Pot_Switch_Off_Timer = 0;
          Display1.clear();
          Display2.clear();
        } 
      }
   }    
}


ISR (TIMER0_OVF_vect)
{   
   OVF_Counter++;   
   if (OVF_Counter >= OVF_Counter_Compare)
   {
      if (Sine_Index == Sine_Len) Sine_Index = 0;
      if (Sine_Index_120 == Sine_Len) Sine_Index_120 = 0;
      if (Sine_Index_240 == Sine_Len) Sine_Index_240 = 0;    
      //  
      if ((Sine_Used[Sine_Index] - DEADTIME_SUB) < MIN_PWM_VAL)        
      {
         OCR0A = 0;
      }
      else
      {
         OCR0A = uint8_t(Sine_Used[Sine_Index] - DEADTIME_SUB);
      }
      OCR0B = OCR0A + DEADTIME_ADD;                            

      if ((Sine_Used[Sine_Index_120] - DEADTIME_SUB) < MIN_PWM_VAL)    
      {
         OCR1A = 0;
      }
      else
      {
         OCR1A = uint8_t(Sine_Used[Sine_Index_120] - DEADTIME_SUB);    
      }
      OCR1B = OCR1A + DEADTIME_ADD;

      if ((Sine_Used[Sine_Index_240] - DEADTIME_SUB) < MIN_PWM_VAL)
      {
          OCR2A = 0;
      }            
      else
      {
          OCR2A = uint8_t(Sine_Used[Sine_Index_240] - DEADTIME_SUB);
      }
      OCR2B = OCR2A + DEADTIME_ADD;

      OVF_Counter = 0;
      Sine_Index++;
      Sine_Index_120++;
      Sine_Index_240++;
   }
}


void Reverse_3_Phase()
{ 
    if (Phase_Config == 0)
    {
        Pwm_Disable();
        uint8_t tempIndex = Sine_Index_120;
        Sine_Index_120 = Sine_Index_240;
        Sine_Index_240 = tempIndex;
        PORTB &= ~((1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2));
        PORTD &= ~((1 << PORTD3) | (1 << PORTD5) | (1 << PORTD6));
    }
}


void ADC_init()
{
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << REFS1);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADEN);
}


uint16_t analogRead(uint8_t channel)
{
    channel &= 0b00000111;
    ADMUX = (ADMUX & 0xF8) | channel;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

