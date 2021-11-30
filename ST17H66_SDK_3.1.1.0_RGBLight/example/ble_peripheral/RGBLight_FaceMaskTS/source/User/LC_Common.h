/**
 *	@file		LC_Common.h
 *	@author		YQ
 *	@date		10/20/2020
 *	@version	1.0.1
 *
 */

/*!
 * 	@defgroup	LC_Common
 *	@brief
 *	@{*/
#ifndef		LC_COMMON_H_
#define		LC_COMMON_H_
/*------------------------------------------------------------------*/
/*						C++ guard macro								*/
/*------------------------------------------------------------------*/
#ifdef	__cplusplus
	 extern  "C" {
#endif
/*------------------------------------------------------------------*/
/* 				 head files include 							 	*/
/*------------------------------------------------------------------*/

#include "att.h"
#include "bcomdef.h"
#include "gapbondmgr.h"
#include "gapgattserver.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "global_config.h"
#include "hci.h"
#include "hci_tl.h"
#include "linkdb.h"
#include "ll.h"
#include "ll_def.h"
#include "ll_hw_drv.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OSAL_Clock.h"
#include "ota_app_service.h"
#include "peripheral.h"
#include "adc.h"
#include "pwm.h"
#include "pwrmgr.h"
#include "rf_phy_driver.h"
#include "simpleBLEPeripheral.h"
#include "sbpProfile_ota.h"

#include "osal_snv.h"
#include "LC_Event_Handler.h"
/*------------------------------------------------------------------*/
/*						Pins definitions							*/
/*------------------------------------------------------------------*/
#define		GPIO_USB_CHECK		P18
#define		GPIO_CHGR_CHECK		P14

/*------------------------------------------------------------------*/
/*						MACROS										*/
/*------------------------------------------------------------------*/
//	Timer To Poweroff
#define		POWEROFF_TIMER_CNT				(60)

//	Light LED Module option
#define		RGBLight						1
#define		RGBWLight						2

// <<< Use Configuration Wizard in Context Menu >>>
//	<h>		Device Module
//	<q>		LC_RGBLight_Key_Enable	-	Key Enable Flag
#ifndef		LC_RGBLight_Key_Enable
#define		LC_RGBLight_Key_Enable			1
#endif
//	<q>		LC_RGBLight_IR_Enable	-	IR Enable Flag
#ifndef		LC_RGBLight_IR_Enable
#define		LC_RGBLight_IR_Enable			0
#endif
//	<q>		LC_RGBLight_Mic_Enable	-	Mic Enable Flag
#ifndef		LC_RGBLight_Mic_Enable
#define		LC_RGBLight_Mic_Enable			0
#endif
//	</h>

//	<h>		RGB LED Type Option
//	<o>		LC_RGBLight_Module	-	Choose LED Type:RGB or RGBW,difference is CCT.
//	<i>		- RGBLight	-	RGB Color,Defaulr Type.
//	<i>		- RGBWLight	-	RGBW Color,
//	<1=>	RGBLight
//	<2=>	RGBWLight
#ifndef		LC_RGBLight_Module
#define		LC_RGBLight_Module				1
#endif
//	</h>
// <<< end of configuration section >>>
/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/* 					 	Data structures							 	*/
/*------------------------------------------------------------------*/
typedef uint8_t				u8;
typedef uint16_t			u16;
typedef uint32_t			u32;
typedef signed   char		int8_t;         //!< Signed 8 bit integer
typedef unsigned char		uint8_t;		//!< Unsigned 8 bit integer
typedef signed   short		int16_t;		//!< Signed 16 bit integer
typedef unsigned short		uint16_t;		//!< Unsigned 16 bit integer
typedef signed   int		int32_t;		//!< Signed 32 bit integer
typedef unsigned int		uint32_t;		//!< Unsigned 32 bit integer
typedef signed   char		int8;			//!< Signed 8 bit integer
typedef unsigned char		uint8;			//!< Unsigned 8 bit integer
typedef signed   short		int16;			//!< Signed 16 bit integer
typedef unsigned short		uint16;         //!< Unsigned 16 bit integer
typedef signed   long		int32;			//!< Signed 32 bit integer
typedef unsigned long		uint32;         //!< Unsigned 32 bit integer

typedef		enum	{
	LC_DEV_BLE_DISCONNECTION	=	0,
	LC_DEV_BLE_CONNECTION,
}lc_dev_ble_state;

typedef	enum{
	State_Off	=	0,
	State_On,
}bit_dev_state_e;

typedef		enum	{
	SYSTEM_STANDBY	=	0,
	SYSTEM_WORKING	=	1,
	SYSTEM_SUSPEND	=	2,
	SYSTEM_CHARGINE	=	3,
}lc_sys_run_t;

typedef struct
{
	uint32		dev_timeout_poweroff_cnt;
	uint8		dev_poweron_switch_flag;
	uint8		dev_power_flag;
	uint8		dev_lowpower_flag;
	uint8		dev_ble_con_state;
	uint8		dev_batt_value;
	uint8		dev_charge_flag;
}lc_dev_sys_param;

typedef struct
{
	uint8		app_write_data[LC_RXD_VALUE_LEN];	//	datas from APP
	uint8		app_notofy_data[LC_RXD_VALUE_LEN];	//	device notify
	uint8		app_write_len;
	uint8		app_notify_len;
}lc_app_set_t;

typedef	struct
{
	uint16		RGB_rValue;					//	value write in register = value_new*light_level/10(0-2550)
	uint16		RGB_gValue;
	uint16		RGB_bValue;
#if(LC_RGBLight_Module ==	RGBWLight)
	uint16		RGB_wValue;
#endif
	uint16		RGB_rValue_New;				//	renew color value(0-255)from app & ir keyboard		
	uint16		RGB_gValue_New;
	uint16		RGB_bValue_New;
#if(LC_RGBLight_Module ==	RGBWLight)
	uint16		RGB_wValue_New;
#endif
	uint16		RGB_Speed_Reserved;			//	original value of speed:0 - 100
	uint16		RGB_Mode_Change_Speed;		//	speed of mode
	uint8		RGB_Mode_Change_Color_Num;	//	number of color in dynamic mode
	uint8		RGB_Mode_Fade_Color_Num;	//	sequence of color in fade mode
	uint8		RGB_Mode_Flash_Time_Num;	//	flash times of one mode
	uint8		RGB_Light_State;			//	on	1,off	0

	uint8		RGB_Light_Level;			//	level of lightness:0--100
	uint8		RGB_Light_Mode;				//	static mode,	dynamic mode
	uint8		RGB_Light_Mode_Reserved;	//	reserved mode before turn off
	uint8		RGB_Light_Mode_Auto;		//	on	1,off	0
	uint8		RGB_Light_TimerEn;			//	on	1,off	0
	uint32		RGB_Light_TimerSeconds;		
}lc_rgblight_t;



/*------------------------------------------------------------------*/
/* 					 external variables							 	*/
/*------------------------------------------------------------------*/
extern		lc_dev_sys_param		LC_Dev_System_Param;
extern		lc_app_set_t			LC_App_Set_Param;
extern		lc_rgblight_t			LC_RGBLight_Param;
/*------------------------------------------------------------------*/
/* 					 User function prototypes					 	*/
/*------------------------------------------------------------------*/
uint32		clock_time_exceed_func 			(uint32 ref, uint32 span_ms		);
uint8		halfbyte_into_str				(uint8 byte						);
void 		Printf_Hex 						(void* data, uint16 len			);
void	 	LC_Common_ProcessOSALMsg	 	(osal_event_hdr_t *pMsg			);
void 		LC_Timer_Start					(void);
void 		LC_Timer_Stop					(void);
uint8		LC_CheckSum						(uint8* data, uint16 len);
#ifdef	__cplusplus
}
#endif

#endif	/* LC_COMMON_H_ */
/** @}*/
