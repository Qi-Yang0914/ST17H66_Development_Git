/**
 *	@file		LC_Key.h
 *	@author		YQ
 *	@date		09/17/2020
 *	@version	1.0.0
 *
 */

/*!
 * 	@defgroup	LC_Key
 *	@brief
 *	@{*/
/*------------------------------------------------------------------*/
/* 					 head files include 						 	*/
/*------------------------------------------------------------------*/
#include "LC_Key.h"
#include "LC_UI_Led_Buzzer.h"
#include "LC_RGBLight_Mode.h"
/*------------------------------------------------------------------*/
/* 					 	public variables		 					*/
/*------------------------------------------------------------------*/
uint8	LC_Key_TaskID;
lc_key_struct_data	LC_Key_Param	=	{
	.key_down_sys_tick		=	0,
	.key_down_flag			=	0,
	.key_repeated_num		=	0,

};
/*------------------------------------------------------------------*/
/* 					 	local functions			 					*/
/*------------------------------------------------------------------*/

static void LC_KeyScanf(void)
{
    uint8 lc_keynewvalue = 0;
    static uint8 lc_keyoldvalue = 0;

    if (hal_gpio_read(MY_KEY_NO1_GPIO) == 0)
    {
        lc_keynewvalue = 1;
    }

    if (lc_keynewvalue)
    {
        if ((lc_keynewvalue == 0) || (lc_keyoldvalue != lc_keynewvalue))
        {
            lc_keyoldvalue = lc_keynewvalue;
            LC_Key_Param.key_down_flag = lc_keynewvalue;
            osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 10);
            osal_stop_timerEx(LC_Key_TaskID, KEY_STOPSCANF_EVT);
        }
    }
    else
    {
        if (lc_keyoldvalue != 0)
        {
            lc_keyoldvalue = 0;
            LC_Key_Param.key_down_flag = 0;
            osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 10);
            osal_start_timerEx(LC_Key_TaskID, KEY_STOPSCANF_EVT, 500);
        }
    }
}
/*------------------------------------------------------------------*/
/* 					 	public functions		 					*/
/*------------------------------------------------------------------*/
/*!
 *	@fn			LC_Gpio_Key_Init
 *	@brief		Initialize the key pins. 
 *	@param[in]	none.
 *	@return		none.
 */
void LC_Gpio_Key_Init(void)
{
	hal_gpio_pin_init(MY_KEY_NO1_GPIO, IE) ;                  		//set gpio input
	hal_gpio_pull_set(MY_KEY_NO1_GPIO, STRONG_PULL_UP) ;      		//pull up 150k
	hal_gpioin_register(MY_KEY_NO1_GPIO, NULL, LC_Key_Pin_IntHandler);

	hal_gpio_pin_init(GPIO_USB_CHECK, IE);
	hal_gpio_pull_set(GPIO_USB_CHECK, PULL_DOWN);
	hal_gpioin_register(GPIO_USB_CHECK, LC_Key_Pin_IntHandler, NULL);

	hal_gpio_pin_init(GPIO_CHGR_CHECK, IE);
	hal_gpio_pull_set(GPIO_CHGR_CHECK, GPIO_PULL_UP_S);

	hal_pwrmgr_register(MOD_USR8, NULL, NULL);	
	hal_pwrmgr_lock(MOD_USR8);
}
/*!
 *	@fn			LC_Key_Task_Init 
 *	@brief		Initialize function for the KEY Task. 
 *	@param[in]	task_id		: 	the ID assigned by OSAL,
 *								used to send message and set timer.
 *	@retrurn	none.
 */
void LC_Key_Task_Init(uint8 task_id)
{
	LC_Key_TaskID	=	task_id;
	LOG("LC_Gpio_Key_Init:\n");
	osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 100);
}
/*!
 *	@fn			LC_Key_ProcessEvent
 *	@brief		KEY Task event processor.This function
 *				is called to processs all events for the task.Events
 *				include timers,messages and any other user defined events.
 *	@param[in]	task_id			:The OSAL assigned task ID.
 *	@param[in]	events			:events to process.This is a bit map and can
 *									contain more than one event.
 */
uint16	LC_Key_ProcessEvent(uint8 task_id, uint16 events)
{
	VOID task_id;	// OSAL required parameter that isn't used in this function
	if(events & SYS_EVENT_MSG){
		uint8	*pMsg;
		if((pMsg = osal_msg_receive(LC_Key_TaskID)) != NULL){
			LC_Common_ProcessOSALMsg((osal_event_hdr_t *)pMsg);
            // Release the OSAL message
			VOID osal_msg_deallocate(pMsg);
		}
		return(events ^ SYS_EVENT_MSG);
	}
	if(events & KEY_EVENT_LEVEL1){
		static 	uint8 	LC_last_button_pressed		=	0 ; 	//key pressed once time
		static 	uint8 	LC_last_button_numbale		=	0 ; 	//key value now
		
		static	uint32	LC_last_button_press_time		=	0 ; 	//tick of releasing key
		static	uint32	LC_last_button_release_time		=	0 ; 	//tick of pressing key
		static	uint32	LC_key_time_temp				=	0 ; 	//deal key event every 20ms

		static	uint8	Key_Long_Press_3s_Enable		=	0;		//key pressed 3s once
		static	uint8	Key_Press_Once_Enable			=	0;		//key pressed once flag
		static	uint8	Key_Value_Reserved				=	0;
		static	uint8	Key_Mode_Cnt					=	0;
		attHandleValueNoti_t notif;
		LC_key_time_temp = hal_systick()|1;
		if(LC_Key_Param.key_down_flag){
			if(LC_last_button_numbale && clock_time_exceed_func(LC_last_button_press_time,2*1000)){
				LC_Key_Param.key_repeated_num	=	0;
				if(Key_Long_Press_3s_Enable == 0){
					Key_Long_Press_3s_Enable	=	1;
					LOG("Key_Long_Press_3s: \n") ;
					LC_Dev_System_Param.dev_power_flag	=	SYSTEM_STANDBY;
				}
			}			
		}else{ 
			if(Key_Long_Press_3s_Enable == 1){
				Key_Long_Press_3s_Enable	=	0;
				Key_Press_Once_Enable		=	1;
				LOG("Key_Long_Release:\n");
			}
		}
		
		if(LC_Key_Param.key_down_flag){
			if(!LC_last_button_pressed && clock_time_exceed_func(LC_last_button_release_time,20)){
				LC_last_button_pressed		=	1 ;
				LC_last_button_press_time	=	LC_key_time_temp ;
				LC_last_button_numbale		=	LC_Key_Param.key_down_flag ;
				Key_Value_Reserved			=	LC_Key_Param.key_down_flag;

				if((Key_Press_Once_Enable == 0) && (LC_Dev_System_Param.dev_charge_flag == 0))
				{
					Key_Press_Once_Enable	=	1;
					LC_Buzzer_Enter_Mode(2);
					LOG("key pressed once cnt = %d\n",Key_Mode_Cnt);
					if(Key_Mode_Cnt <= 6)
					{
						if(LC_RGBLight_Param.RGB_Light_State == State_Off)
						{
							LC_RGBLight_Param.RGB_Light_State	=	State_On;
						}
						LC_RGBLight_Param.RGB_Light_Mode	=	Key_Mode_Cnt + 0x60;
						LC_RGBLight_Mode_Static_OneColor(LC_RGBLight_Param.RGB_Light_Mode);					
					}
					else if(Key_Mode_Cnt == 7)
					{
						LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Jump_SevenColors;
						LC_RGBLight_Modetick				=	2000;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Speed_Reserved		=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Speed		=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					}
					else if(Key_Mode_Cnt == 8)
					{
						if(LC_RGBLight_Param.RGB_Light_State == State_On)
						{
							LC_RGBLight_Turn_Onoff(State_Off);
						}
						Key_Mode_Cnt	=	0x0f;
					}

					Key_Mode_Cnt	=	0x0f&(Key_Mode_Cnt + 1);
					if(LC_Dev_System_Param.dev_ble_con_state == LC_DEV_BLE_CONNECTION)
					{
						notif.value[0]	=	0xBB;
						notif.value[1]	=	0x06;
						notif.value[2]	=	0x06;
						notif.value[3]	=	LC_RGBLight_Param.RGB_Light_State;
						notif.value[4]	=	Key_Mode_Cnt;
						notif.value[5]	=	LC_RGBLight_Param.RGB_Light_TimerEn;
						notif.value[6]	=	0;
						notif.value[7]	=	(uint8)(LC_RGBLight_Param.RGB_Light_TimerSeconds/60);
						notif.value[8]	=	(uint8)(LC_RGBLight_Param.RGB_Light_TimerSeconds%60);
						notif.value[9]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
						notif.len		=	10;	
						simpleProfile_Notify(&notif);
					}
				}
			}
		}else{
			if(LC_last_button_pressed && clock_time_exceed_func(LC_last_button_press_time,20) ){
				LC_last_button_release_time	=	LC_key_time_temp;
				LC_last_button_pressed 		=	0 ;
				if(Key_Press_Once_Enable == 1)
				{
					Key_Press_Once_Enable	=	0;
					LOG("key once release\n");
				}
			}
		}
		if(LC_Key_Param.key_repeated_num && LC_Key_Param.key_down_sys_tick && clock_time_exceed_func(LC_Key_Param.key_down_sys_tick,300)){
			LOG("Key total Kick num: %d, key is %d\n",LC_Key_Param.key_repeated_num,Key_Value_Reserved) ;

			// if((LC_Key_Param.key_repeated_num == 1) && (Key_Press_Once_Enable == State_Off)){
			// 	Key_Press_Once_Enable	=	State_On;
			// 	if(Key_Value_Reserved == 1){
			// 		if(LC_RGBLight_Param.RGB_Light_State == State_On){
			// 			// LOG("Key_Mode_Cnt %d\n",Key_Mode_Cnt);
			// 			if(Key_Mode_Cnt < 12){
			// 				LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Fade_ThreeColors + Key_Mode_Cnt;
			// 			}else if(Key_Mode_Cnt < 20){
			// 				LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Flash_SevenColors + Key_Mode_Cnt - 12;
			// 			}else if(Key_Mode_Cnt < 22){
			// 				LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Jump_ThreeColors + Key_Mode_Cnt - 20;
			// 			}else{
			// 				Key_Mode_Cnt	=	0;
			// 			}
			// 			LC_RGBLight_Modetick	=	0;
			// 			LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
			// 			LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
			// 			LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
			// 			LC_RGBLight_Reserve_Mode();
			// 			Key_Mode_Cnt++;
			// 		}
			// 	}
			// }


			// if(Key_Press_Once_Enable == State_On){
			// 	Key_Press_Once_Enable	=	State_Off;
			// 	LOG("Key Once Release:\n");
			// }
			LC_Key_Param.key_down_sys_tick		=	0;
			LC_Key_Param.key_repeated_num		=	0;
			LC_last_button_numbale	=	0;
			Key_Value_Reserved		=	0;
		}
		if(LC_last_button_numbale && !LC_Key_Param.key_down_flag && clock_time_exceed_func(LC_last_button_press_time,20)){
			// LC_Key_Param.key_repeated_num++ ;
			LC_Key_Param.key_down_sys_tick = LC_key_time_temp ;
			// LOG("key time num: %d, key is%d\n",LC_Key_Param.key_repeated_num,LC_last_button_numbale);
			LC_last_button_numbale = 0 ;
			
		}
        if(LC_Key_Param.key_down_flag || LC_Key_Param.key_repeated_num){
			osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 20);
        }
		return(events ^ KEY_EVENT_LEVEL1);
	}

	if(events & KEY_SCANF_EVT)
	{
		osal_start_timerEx(LC_Key_TaskID, KEY_SCANF_EVT, 40);
		LC_KeyScanf();
		return(events ^ KEY_SCANF_EVT);
	}

	if(events & KEY_STOPSCANF_EVT)
	{
		LOG("stop key scanf\n");
		hal_gpioin_register(MY_KEY_NO1_GPIO, NULL, LC_Key_Pin_IntHandler);
		osal_stop_timerEx(LC_Key_TaskID, KEY_SCANF_EVT);
		return(events ^ KEY_STOPSCANF_EVT);
	}

	if(events & KEY_CHARG_CHECK_EVT)
	{
		if(LC_Dev_System_Param.dev_charge_flag == 0)
		{
			LC_Dev_System_Param.dev_charge_flag	=	1;
			LOG("start charging\n");
			if(LC_RGBLight_Param.RGB_Light_State == State_On)
			{
				LC_RGBLight_Turn_Onoff(State_Off);
			}
		}
		osal_start_reload_timer(LC_Key_TaskID, KEY_CHARG_CHECK_EVT, 500);
		if(hal_gpio_read(GPIO_CHGR_CHECK) == 1)
		{
			LOG("charge finish\n");
		}

		if(hal_gpio_read(GPIO_USB_CHECK) == 0)
		{
			LOG("charge stop\n");
			LC_Dev_System_Param.dev_charge_flag	=	0;
			hal_gpioin_register(GPIO_USB_CHECK, LC_Key_Pin_IntHandler, NULL);
			osal_stop_timerEx(LC_Key_TaskID, KEY_CHARG_CHECK_EVT);
		}
		return(events ^ KEY_CHARG_CHECK_EVT);
	}
    // Discard unknown events
    return 0;
}
/** @}*/

