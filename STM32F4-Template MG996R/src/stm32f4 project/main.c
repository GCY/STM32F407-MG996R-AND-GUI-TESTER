#include <stdio.h>
#include <stdarg.h>
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_exti.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_adc.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_dma.h>
#include <stm32f4xx_rtc.h>


#include "./usb_cdc_device/usbd_usr.h"
#include "./usb_cdc_device/usbd_cdc_core.h"
#include "./usb_cdc_device/usb_conf.h"
#include "./usb_cdc_device/usbd_desc.h"
#include "./usb_cdc_device/usbd_cdc_vcp.h"

#include "tiny_printf.h"

volatile uint32_t TimingDelay;

volatile uint32_t micros = 0;

void Delay(__IO uint32_t nTime)
{
   TimingDelay = nTime;
   while(TimingDelay){
   }
}

void SysTick_Handler(void)
{
   if(TimingDelay){
      --TimingDelay;
   }
   ++micros;
}

/* Private */
#define USB_VCP_RECEIVE_BUFFER_LENGTH		128
uint8_t INT_USB_VCP_ReceiveBuffer[USB_VCP_RECEIVE_BUFFER_LENGTH];
uint32_t int_usb_vcp_buf_in, int_usb_vcp_buf_out, int_usb_vcp_buf_num;
USB_VCP_Result USB_VCP_INT_Status;
//extern LINE_CODING linecoding;
uint8_t USB_VCP_INT_Init = 0;
USB_OTG_CORE_HANDLE	USB_OTG_dev;

extern uint8_t INT_USB_VCP_ReceiveBuffer[USB_VCP_RECEIVE_BUFFER_LENGTH];

USB_VCP_Result USBVCPInit(void)
{
   USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_FS
	 USB_OTG_FS_CORE_ID,
#else
	 USB_OTG_HS_CORE_ID,
#endif
	 &USR_desc, 
	 &USBD_CDC_cb, 
	 &USR_cb);   
   
   /* Reset buffer counters */
   int_usb_vcp_buf_in = 0;
   int_usb_vcp_buf_out = 0;
   int_usb_vcp_buf_num = 0;
	
   /* Initialized */
   USB_VCP_INT_Init = 1;

   return USB_VCP_OK;
}

USB_VCP_Result USB_VCP_GetStatus(void) {
   if (USB_VCP_INT_Init) {
      return USB_VCP_INT_Status;
   }
   return USB_VCP_ERROR;
}

USB_VCP_Result USB_VCP_Getc(uint8_t* c) {
   /* Any data in buffer */
   if (int_usb_vcp_buf_num > 0) {
      /* Check overflow */
      if (int_usb_vcp_buf_out >= USB_VCP_RECEIVE_BUFFER_LENGTH) {
	 int_usb_vcp_buf_out = 0;
      }
      *c = INT_USB_VCP_ReceiveBuffer[int_usb_vcp_buf_out];
      INT_USB_VCP_ReceiveBuffer[int_usb_vcp_buf_out] = 0;
		
      /* Set counters */
      int_usb_vcp_buf_out++;
      int_usb_vcp_buf_num--;
		
      /* Data OK */
      return USB_VCP_DATA_OK;
   }
   *c = 0;
   /* Data not ready */
   return USB_VCP_DATA_EMPTY;
}

USB_VCP_Result USB_VCP_Putc(volatile char c) {
	uint8_t ce = (uint8_t)c;
	
	/* Send data over USB */
	VCP_DataTx(&ce, 1);
	
	/* Return OK */
	return USB_VCP_OK;
}

USB_VCP_Result USB_VCP_Puts(char* str) {
	while (*str) {
		USB_VCP_Putc(*str++);
	}
	
	/* Return OK */
	return USB_VCP_OK;
}

USB_VCP_Result INT_USB_VCP_AddReceived(uint8_t c) {
	/* Still available data in buffer */
	if (int_usb_vcp_buf_num < USB_VCP_RECEIVE_BUFFER_LENGTH) {
		/* Check for overflow */
		if (int_usb_vcp_buf_in >= USB_VCP_RECEIVE_BUFFER_LENGTH) {
			int_usb_vcp_buf_in = 0;
		}
		/* Add character to buffer */
		INT_USB_VCP_ReceiveBuffer[int_usb_vcp_buf_in] = c;
		/* Increase counters */
		int_usb_vcp_buf_in++;
		int_usb_vcp_buf_num++;
		
		/* Return OK */
		return USB_VCP_OK;
	}
	
	/* Return Buffer full */
	return USB_VCP_RECEIVE_BUFFER_FULL;
}

void PWMInit(void){

   GPIO_InitTypeDef            GPIO_InitStructure;
   TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
   TIM_OCInitTypeDef           TIM_OCInitStructure;
    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
   GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
    
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
   GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);

   /*
    * 84M / TIM_Prescaler = 1M
    * 1M / TIM_ClockDivision = 1M
    * 1M / (TIM_Period + 1) = 50Hz (20ms)
    * TIM_Pulse = 500(us) ~ 2500(us)
    */
   uint16_t PrescalerValue = (uint16_t) 84;
   
   TIM_TimeBaseStructure.TIM_Period        = 19999;
   TIM_TimeBaseStructure.TIM_Prescaler     = PrescalerValue;
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//0;
   TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    
   TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
   TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse       = 0; 
   TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    
   TIM_OC1Init(TIM4, &TIM_OCInitStructure);
   TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
   TIM_OC2Init(TIM4, &TIM_OCInitStructure);
   TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
   TIM_ARRPreloadConfig(TIM4, ENABLE);
    
   TIM_Cmd(TIM4, ENABLE);
}

int main(void)
{
   if(SysTick_Config(SystemCoreClock / 1000 / 1000)){
      while(1){}
   }

  USBVCPInit();

  PWMInit();

  TIM4->CCR1 = 1500;      // 1500 == 1.5 ms -> Medium
  TIM4->CCR2 = 1500;
  Delay(1500000);
/*
  while(1){
     for(int i = 500;i < 2500;++i){
	TIM4->CCR1 = i;
	TIM4->CCR2 = i;	
	Delay(2000/2);
     }
     for(int i = 2500;i > 500;--i){
	TIM4->CCR1 = i;
	TIM4->CCR2 = i;	
	Delay(2000/2);
     }     
     
  }
*/

  int id_state = 0;

  char str[255] = {""};  
  int index = 0;

  while(1){

  if(USB_VCP_GetStatus() == USB_VCP_CONNECTED) {

	uint8_t c;
	if (USB_VCP_Getc(&c) == USB_VCP_DATA_OK) {
	   USB_VCP_Putc(c);
	   if(c == '[' && id_state == 0){
	      ++id_state;
	   }
	   else if(c != ']' && id_state == 1){
	      str[index++] = c;
	   }	   
	   else if(c == ']' && id_state == 1){
	      ++id_state;
	   }

	   if(id_state == 2){
	      //USB_VCP_Puts(str);
	      int servos = 0;
	      char *p = NULL;
	      p = strtok(str,",");
	      //USB_VCP_Puts(p);
	      TIM4->CCR1 = atoi(p);
	      ++servos;
	      while((p = strtok(NULL,","))){
		 USB_VCP_Puts(p);
		 int pulse = atoi(p);
		 if(servos == 1){
		    TIM4->CCR2 = pulse;
		 }
		 else if(servos == 2){
		    //TIM4->CCR3 = pulse;
		 }
		 ++servos;
	      }
	      index = 0;
	      id_state = 0;
	      memset(str,0,sizeof(str));
	   }
	}

     }
     Delay(1000);
  }
 
  return(0); // System will implode
}    
