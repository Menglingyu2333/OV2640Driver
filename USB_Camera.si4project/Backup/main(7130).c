/**
 **************************************************************************
 * @file      : EXTI/EXTI_Config/main.c
 * @author    : Artery Technology
 * @version   : V1.0.4
 * @date      : 2018-12-26
 * @brief     : Main program body
 **************************************************************************
 * @brief     : Initializes and turn on LED.Configure external interrupt lines.
 *              Then immediately generation an interrupt on external line 0 by software,it will cause a Toggle on LED3.
 *              And then if monitor a falling trigger on PB9,an interrupt on external line 9 will happen,and it will cause a Toggle on LED4
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, ARTERYTEK SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2018 ArteryTek</center></h2>
 ***************************************************************************
 */

 /* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"
#include "at32_board.h"

#include "sys.h"
#include "lcd.h"
#include "touch.h"
#include "test.h"

#include "ov2640.h"
#include "delay.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup EXTI_Config
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitType GPIO_InitStructure;
EXTI_InitType EXTI_InitStructure;
NVIC_InitType NVIC_InitStructure = {0};

/* Private function prototypes -----------------------------------------------*/
void EXTI0_Config(void);
void EXTI9_5_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
 *	@brief  main function
 *	@param  None
 *	@retval None
 */
u8  ImageBufffer[ImageWidth * ImageHeight];
u8 *			ImageBuf;
u8 *			ImageBufProc;
u8				ImagePtr_W, ImagePtr_H;

#define ImageData(H, W) 		*(u8*)(ImageBuf+H*ImageWidth+W)
void Camera_thread_entry(void * parameter)
{

	LCD_SetWindows(0, 0, ImageWidth - 1, ImageHeight - 1); //LCD窗口设置

  //获取图像缓存
	ImageBuf			= &ImageBufffer[0];
//	if(ImageBuf == NULL) {
////		printf("get memory FAILED!\n");
//		return;
//	}
//	ImageBufProc  = malloc(ImageWidth * ImageHeight);
//	if(ImageBufProc == NULL) {
//	  free(ImageBuf);
//		printf("get memory FAILED!\n");
//		return;
//	}

//  rt_hw_interrupt_disable();//开始接收图像后关闭所有系统中断，否则会导致接收时序错误。

  while(1)
  {
  	ImagePtr_W			= 0;
  	ImagePtr_H			= 0;
  	LCD_SetCursor(0, 0);
  	while(OV2640_VSYNC == 1);//丢弃传输到一半的图像
  	while(OV2640_VSYNC == 0);
  	while(OV2640_VSYNC == 1) //开始采集jpeg数据
  	{
  		ImagePtr_W			= 0;

  		while(OV2640_HREF == 0 && OV2640_VSYNC == 1);

  		while(OV2640_HREF)
      {
  			while(OV2640_PCLK == 0);
  			while(OV2640_PCLK == 1);
  			while(OV2640_PCLK == 0);

  			ImageData(ImagePtr_H, ImagePtr_W) = OV2640_DATA;

  			LCD_RS_SET;
  			LCD_CS_CLR;
  			DATAOUT(yuv_8_to_RGBGray(ImageData(ImagePtr_H, ImagePtr_W)));
  			LCD_WR_CLR;
  			LCD_WR_SET;
  			LCD_CS_SET;

  			while(OV2640_PCLK == 1);

  			++ImagePtr_W;
  		}
  		++ImagePtr_H;
  	}
  }
}
int main(void)
{
  delay_init(240);
	GPIO_StructInit(&GPIO_InitStructure);
	EXTI_StructInit(&EXTI_InitStructure);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	AT32_Board_Init();   ///<Initialize LED and KEY
	AT32_LEDn_ON(LED2);
	AT32_LEDn_ON(LED3);
	AT32_LEDn_ON(LED4);

	EXTI0_Config();    ///<Configure PA0 in interrupt mode
	EXTI9_5_Config();  ///<Configure PB9 in interrupt mode

	EXTI_GenerateSWInt(EXTI_Line0); ///<Generate software interrupt
	LCD_Init();
	delay_ms(1000);
	OV2640_Init();
	Camera_thread_entry(0);
	while (1)
	{
	}
}

/**
  * @brief  EXTI0 Config.Configure PA0 in interrupt mode
  * @param  None
  * @retval None
  */
void EXTI0_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);        ///<Enable GPIOA clock
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                       ///<Configure PA.00 pin as input floating

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);         ///<Enable AFIO clock
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinsSource0); ///<Connect EXTI0 Line to PA0 pin

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineEnable = ENABLE;
	EXTI_Init(&EXTI_InitStructure);                              ///<Configure EXTI0 line

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);                              ///<Enable and set EXTI0 Interrupt to the lowest priority
}

/**
  * @brief  EXTI9_5 Config.Configure PB9 in interrupt mode
  * @param  None
  * @retval None
  */
void EXTI9_5_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);         ///<Enable GPIOB clock
	GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                        ///<Configure PB9 pin as input floating

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);          ///<Enable AFIO clock
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinsSource9);  ///<Connect EXTI9 Line to PB9 pin

	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineEnable = ENABLE;
	EXTI_Init(&EXTI_InitStructure);                               ///<Configure EXTI9 line

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);                               ///<Enable and set EXTI9_5 Interrupt to the lowest priority
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2018 ArteryTek *****END OF FILE****/
