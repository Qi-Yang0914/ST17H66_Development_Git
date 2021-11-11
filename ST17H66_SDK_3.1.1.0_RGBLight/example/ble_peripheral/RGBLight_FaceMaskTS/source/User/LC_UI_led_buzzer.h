/**
 *	@file		LC_UI_led_buzzer.h
 *	@author		YQ
 *	@date		09/16/2020
 *	@version	1.2.0
 *
 */

/*!
 * 	@defgroup	LC_UI_led_buzzer
 *	@brief
 *	@{*/

#ifndef	LC_UI_LED_BUZZER_H_
#define	LC_UI_LED_BUZZER_H_
/*------------------------------------------------------------------*/
/*						C++ guard macro								*/
/*------------------------------------------------------------------*/
#ifdef	__cplusplus
extern	"C"	{
#endif
/*------------------------------------------------------------------*/
/*						head files include 							*/
/*------------------------------------------------------------------*/
#include "LC_Common.h"
/*------------------------------------------------------------------*/
/*						Pins definitions							*/
/*------------------------------------------------------------------*/
#define		MY_GPIO_LED_R		P34
#define		MY_GPIO_LED_G		P2
#define		MY_GPIO_LED_B		P3

#if(LC_RGBLight_Module == RGBWLight)
#define		MY_GPIO_LED_WW		P11
#endif

#define		MY_GPIO_LED_NO1		P11
#define		MY_GPIO_PWM_NO1		P7

/*------------------------------------------------------------------*/
/*						MACROS										*/
/*------------------------------------------------------------------*/
#define		RGB_COLOR_MAX		(255)
#define		RGB_PWM_MAX			(255)
#define		RGB_WHITE_MAX		(180)
#define		RGB_LEVEL_PECENT	(100)
#define		RGB_PWM_COLOR_MUL	(RGB_PWM_MAX/RGB_COLOR_MAX)

#define		BUZZER_FREQ		740
#define		BUZZER_DUTY		370

#define		LED_NO1_ON()		{hal_gpio_write(MY_GPIO_LED_NO1, 0);}
#define		LED_NO1_OFF()		{hal_gpio_write(MY_GPIO_LED_NO1, 1);}

#define		BUZZER_ON()			{hal_gpio_write(MY_GPIO_PWM_NO1, 1);}
#define		BUZZER_OFF()		{hal_gpio_write(MY_GPIO_PWM_NO1, 0);}
/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/
#define		UI_EVENT_LEVEL1		0x0001
#define		UI_EVENT_LEVEL2		0x0002
#define		UI_EVENT_LEVEL3		0x0004
#define		UI_EVENT_LEVEL4		0x0008
#define		UI_EVENT_LEVEL5		0x0010
/*------------------------------------------------------------------*/
/*						Data structures								*/
/*------------------------------------------------------------------*/
typedef struct
{
	uint16			offOn_Ms[2];
	uint8			offOn_cnt;
	uint8			next_mode;
}lc_ui_mode_para;

typedef struct
{
	lc_ui_mode_para *ui_type;
	uint8			cur_mode;
	uint8			cur_cnt;
	uint8			cur_state;
	uint32			next_wakeup_tick;
}lc_ui_run_para;
/*------------------------------------------------------------------*/
/*						external variables							*/
/*------------------------------------------------------------------*/
extern uint8	LC_Ui_Led_Buzzer_TaskID;
/*------------------------------------------------------------------*/
/*						User function prototypes					*/
/*------------------------------------------------------------------*/
void	LC_Dev_Poweroff					(uint8 ledenable);
void	LC_Led_No1_Enter_Mode			(uint8 mode);
void	LC_Buzzer_Enter_Mode			(uint8 mode);
void 	LC_UI_Led_Buzzer_Task_Init		(uint8 task_id	);
uint16	LC_UI_Led_Buzzer_ProcessEvent	(uint8 task_id, uint16 events);


#ifdef	__cplusplus
}
#endif

#endif	/*	LC_UI_LED_BUZZER_H_	*/
/**	@}*/
