/**
 *	@file		LC_Common.c
 *	@author		YQ
 *	@date		09/17/2020
 *	@version	1.0.3
 *
 */

/*!
 * 	@defgroup	LC_Common
 *	@brief
 *	@{*/
 
#include "LC_Common.h"
#include "LC_Event_Handler.h"
lc_dev_sys_param	LC_Dev_System_Param	=	
{
	.dev_timeout_poweroff_cnt	=	0xffff,
	.dev_poweron_switch_flag	=	0,
	.dev_power_flag				=	1,
	.dev_lowpower_flag			=	0,
	.dev_ble_con_state			=	0,
	.dev_batt_value				=	0,
	.dev_charge_flag			=	0,
};

lc_app_set_t		LC_App_Set_Param = 
{
	.app_write_data				=	{0},
	.app_notofy_data			=	{0},
	.app_write_len				=	0,
	.app_notify_len				=	0,
	
};
lc_rgblight_t		LC_RGBLight_Param	=
{
	.RGB_rValue					=	0,
	.RGB_gValue					=	0,
	.RGB_bValue					=	0,
#if(LC_RGBLight_Module ==	RGBWLight)
	.RGB_wValue					=	0,
#endif
	.RGB_rValue_New				=	0,
	.RGB_gValue_New				=	0,
	.RGB_bValue_New				=	0,
#if(LC_RGBLight_Module ==	RGBWLight)
	.RGB_wValue_New				=	0,
#endif
	.RGB_Speed_Reserved			=	0,
	.RGB_Mode_Change_Speed		=	2000,
	.RGB_Mode_Change_Color_Num	=	0,
	.RGB_Mode_Fade_Color_Num	=	0,
	.RGB_Mode_Flash_Time_Num	=	0,
	.RGB_Light_State			=	State_On,
	.RGB_Light_Level			=	100,
	.RGB_Light_Mode				=	0x91,
	.RGB_Light_Mode_Reserved	=	0,
	.RGB_Light_Mode_Auto		=	0,
};

/*!
 *	@fn			clock_time_exceed_func
 *	@brief		
 */
uint32 clock_time_exceed_func(uint32 ref, uint32 span_ms)
{
#if 0
	u32 deltTick ,T0 ;
	T0 = hal_read_current_time();
	deltTick =TIME_DELTA(T0,ref);
	if(deltTick>span_ms){
		return 1 ;
	}else {
		return 0 ;
	}
#else 
	uint32 deltTick  = 0 ;
	deltTick = hal_ms_intv(ref) ;
	if(deltTick>span_ms){
		return 1 ;
	}else {
		return 0 ;
	}	
#endif
}
/*!
 *	@fn			halfbyte_into_str
 *	@brief		MAC address transform into device name.
 *	@param[in]	byte:	half byte of MAC.
 *	@return		ASCII of halfbyte.
 */
uint8	halfbyte_into_str(uint8 byte)
{
	int8 temp = 0;
	if(byte < 0x0a)
		temp = byte + '0';
	else
		temp = byte - 0x0a + 'a';

	return temp;
}
/*!
 *	@fn			Printf_Hex
 *	@brief		printf data.
 *	@param[in]	data	:pointer of datas,
 *	@param[in]	len		:length of datas,
 *	@return		none.
 */
void	Printf_Hex(void* data, uint16 len)
{
    uint16 i;
    uint8* p =	data;

    for (i = 0; i < len - 1; i++)
    {
        LOG("%x,",*(p + i));
        LOG(" ");
    }
    LOG("%x\n",*(p + i));

}
/*!
 *	@fn			LC_Common_ProcessOSALMsg
 *	@brief		Process an incoming task message,nothing.
 *	@param[in]	pMsg	:message to process
 *	@return		none.
 */
void LC_Common_ProcessOSALMsg(osal_event_hdr_t *pMsg)
{
	switch(pMsg->event)
	{
		default:
			// do nothing
		break;
	}
}
void LC_Timer_Start(void)
{
	hal_timer_init(LC_RGB_Valeu_Deal);
	hal_timer_set(AP_TIMER_ID_5, 100);
	hal_timer_set(AP_TIMER_ID_6, 5*1000);
//	LOG("Start timer:\n");
}
void LC_Timer_Stop(void)
{
	hal_timer_stop(AP_TIMER_ID_5);
	hal_timer_stop(AP_TIMER_ID_6);
//	LOG("Stop timer\n");
}
/**
 * @brief 			checksum.
 * 
 * @param[in] data 		pointer of data needed to check.
 * @param[in] len 		length.
 * @return uint8 
 */
uint8	LC_CheckSum(uint8* data, uint16 len)
{
	uint16	total_sum	=	0;
	uint16	index		=	0;
	// LOG("CK %x,%x\n",total_sum,len);
	for(index = 0;index < len;index++){
		total_sum += *(data + index);
	}
	// LOG("CK %x,\n",total_sum);

	return	(uint8)total_sum;
}
/** @}*/

