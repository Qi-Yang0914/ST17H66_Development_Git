/**
 *	@file		LC_ADC.h
 *	@date		10/19/2020
 *	@version	1.0.0
 */

/*!
 *	@defgroup	LC_ADC
 *	@brief
 *	@{*/

#ifndef		LC_ADC_H_
#define		LC_ADC_H_
/*------------------------------------------------------------------*/
/*						C++ guard macro								*/
/*------------------------------------------------------------------*/
#ifdef	__cplusplus
	extern "C"	{
#endif
/*------------------------------------------------------------------*/
/*						head files include 							*/
/*------------------------------------------------------------------*/
#include "LC_Common.h"
/*------------------------------------------------------------------*/
/*						Pins definitions							*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*						MACROS										*/
/*------------------------------------------------------------------*/
//	battery pecent
#define		MY_BATT_PECENT_MAX		100
#define		MY_BATT_PECENT_75		75
#define		MY_BATT_PECENT_50		50
#define		MY_BATT_PECENT_25		25
#define		MY_BATT_PECENT_MIN		0

#define		LC_DEV_BAT_CHECK_EN()	{hal_gpio_write(GPIO_BAT_CHECK_EN, 1);}
#define		LC_DEV_BAT_CHECK_DIS()	{hal_gpio_write(GPIO_BAT_CHECK_EN, 0);}
/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/
#define		ADC_EVENT_LEVEL1		0x0001
#define		ADC_EVENT_LEVEL2		0x0002
#define		ADC_EVENT_LEVEL3		0x0004
/*------------------------------------------------------------------*/
/*						Data structures								*/
/*------------------------------------------------------------------*/
typedef		struct{
	uint32	adc_simp_value;	//	adc value
}lc_adc_struct_data;
/*------------------------------------------------------------------*/
/*						external variables							*/
/*------------------------------------------------------------------*/
extern		uint8					LC_ADC_TaskID;
extern		lc_adc_struct_data		LC_ADC_Param;
extern		uint8					battLevel;

/*------------------------------------------------------------------*/
/*						User function prototypes					*/
/*------------------------------------------------------------------*/
void 	LC_ADC_Gpio_Init		(void);
int 	LC_ADC_Channel_Init		(void);
void 	LC_ADC_Handler_Evt		(adc_Evt_t* pev					);
void 	LC_ADC_Task_Init		(uint8 task_id					);
uint16	LC_ADC_ProcessEvent		(uint8 task_id, uint16 events	);

#ifdef	__cplusplus
	}
#endif

#endif	/**	LC_ADC.h **/
/**	@}*/
