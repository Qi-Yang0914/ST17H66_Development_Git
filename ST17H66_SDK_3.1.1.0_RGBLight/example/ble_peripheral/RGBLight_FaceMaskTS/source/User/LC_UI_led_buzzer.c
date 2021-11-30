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

/*------------------------------------------------------------------*/
/*						head files include 							*/
/*------------------------------------------------------------------*/
#include "LC_UI_Led_Buzzer.h"
#include "LC_Key.h"
#include "LC_RGBLight_Mode.h"
/*------------------------------------------------------------------*/
/* 					 	public variables		 					*/
/*------------------------------------------------------------------*/
uint8	LC_Ui_Led_Buzzer_TaskID;
lc_ui_mode_para	Led_No1_Mode_Ms[]	=	{
	//off			on		cnt 	next mode
	{{0,			0}, 	0,		0},
	{{0, 			4500},	0x01,	2},
	{{1000,			1300},	0xff,	2}, 
	{{0,			200},	0x01,	0},
	{{0,			6000},	0x01,	0},
	{{1000,			60},	0x28,	0},
	{{180,			60},	0xff,	0},
};
lc_ui_mode_para	Buzzer_Mode_Ms[]	=	{
	//off			on		cnt 	next mode
	{{0,			0}, 	0,		0},    
	{{100,			100},	0x02,	0}, 
	{{0,			100},	0x01,	0}, 	
	{{300,			100},	0xff,	3}, 
};
lc_ui_run_para	Led_No1_Param	=	{Led_No1_Mode_Ms};
lc_ui_run_para	Buzzer_Param	=	{Buzzer_Mode_Ms};
/*------------------------------------------------------------------*/
/* 					 	local functions			 					*/
/*------------------------------------------------------------------*/
/**
 * @brief 	Read RGB data from flash.
 * 
 */
static	void	LC_ReadReservedData(void)
{
	uint8	Flash_Read_Buffer[32];
	osal_snv_read(0x88, 16, Flash_Read_Buffer);

	LOG("read flash mode\n");
	LOG_DUMP_BYTE(Flash_Read_Buffer,16);

	if(Flash_Read_Buffer[0] == 0x55){
		LC_RGBLight_Param.RGB_Light_Mode			=	Flash_Read_Buffer[1];
		if((LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode) || \
			((LC_RGBLight_Param.RGB_Light_Mode >= RGB_Static_Red) && (LC_RGBLight_Param.RGB_Light_Mode <= RGB_Static_White))){
			LC_RGBLight_Param.RGB_rValue	=	(uint16)((Flash_Read_Buffer[2] << 8)&0xff00) + Flash_Read_Buffer[3];
			LC_RGBLight_Param.RGB_gValue	=	(uint16)((Flash_Read_Buffer[4] << 8)&0xff00) + Flash_Read_Buffer[5];
			LC_RGBLight_Param.RGB_bValue	=	(uint16)((Flash_Read_Buffer[6] << 8)&0xff00) + Flash_Read_Buffer[7];
		#if(LC_RGBLight_Module == RGBWLight)
			LC_RGBLight_Param.RGB_wValue	=	(uint16)((Flash_Read_Buffer[8] << 8)&0xff00) + Flash_Read_Buffer[9];
		#endif
		}else if((LC_RGBLight_Param.RGB_Light_Mode >= RGB_Fade_ThreeColors) && (LC_RGBLight_Param.RGB_Light_Mode <= RGB_Smooth)){
			LC_RGBLight_Param.RGB_Mode_Change_Speed	=	(uint16)((Flash_Read_Buffer[12] << 8)&0xff00) + Flash_Read_Buffer[13];
		}
	}
}
/**
 * @brief 	Initlize PWM channel.No output before start PWM.
 * 
 */
static	void	LC_PWMChannelInit(void)
{
	hal_pwm_init(PWM_CH0, PWM_CLK_DIV_8, PWM_CNT_UP, PWM_POLARITY_FALLING);
	hal_pwm_open_channel(PWM_CH0, MY_GPIO_LED_R);
	
	hal_pwm_init(PWM_CH1, PWM_CLK_DIV_8, PWM_CNT_UP, PWM_POLARITY_FALLING);
	hal_pwm_open_channel(PWM_CH1, MY_GPIO_LED_G);
	
	hal_pwm_init(PWM_CH2, PWM_CLK_DIV_8, PWM_CNT_UP, PWM_POLARITY_FALLING);
	hal_pwm_open_channel(PWM_CH2, MY_GPIO_LED_B);
#if(LC_RGBLight_Module == RGBWLight)
	hal_pwm_init(PWM_CH3, PWM_CLK_DIV_8, PWM_CNT_UP, PWM_POLARITY_FALLING);
	hal_pwm_open_channel(PWM_CH3, MY_GPIO_LED_WW);
#endif

	hal_pwm_init(PWM_CH4, PWM_CLK_DIV_8, PWM_CNT_UP, PWM_POLARITY_FALLING);
	hal_pwm_open_channel(PWM_CH4, MY_GPIO_PWM_NO1);
	hal_pwm_set_count_val(PWM_CH4, 0, BUZZER_FREQ);

}
/**
 * @brief	timer working
 * 
 */
void	LC_Dev_WorkingTimer(void)
{
	if(LC_RGBLight_Param.RGB_Light_TimerEn == State_On)
	{
		LC_RGBLight_Param.RGB_Light_TimerSeconds--;
		LOG("working time = %d [s]\n",LC_RGBLight_Param.RGB_Light_TimerSeconds);
		if(LC_RGBLight_Param.RGB_Light_TimerSeconds == 0)
		{
			LC_RGBLight_Param.RGB_Light_TimerEn	=	State_Off;
			if(LC_RGBLight_Param.RGB_Light_State == State_On)
			{
				LC_RGBLight_Turn_Onoff(State_Off);
			}
		}
	}
	else
	{
		LC_Dev_System_Param.dev_timeout_poweroff_cnt--;
		if(LC_Dev_System_Param.dev_timeout_poweroff_cnt == 0)
		{
			LOG("power timer to sleep\n");
			LC_Dev_System_Param.dev_power_flag		=	SYSTEM_STANDBY;
		}
	}
}
/*!
 *	@fn			LC_Led_No1_Onoff
 *	@brief		led and buzzer on-off state control.
 *	@param[in]	Onoff		:the state of led or buzzer
 *	@return		none.
 */
void LC_Led_No1_Onoff(uint8 Onoff)
{
	if(Onoff){
		LED_NO1_ON();
	}else{
		LED_NO1_OFF();
	}
}
void LC_Buzzer_Onoff(uint8 Onoff)
{
	if(Onoff)
	{
		hal_pwm_set_count_val(PWM_CH4, BUZZER_DUTY, BUZZER_FREQ);
		// BUZZER_ON();
	}
	else
	{
		hal_pwm_set_count_val(PWM_CH4, 0, BUZZER_FREQ);
		// BUZZER_OFF();
	}
}
/*!
 *	@fn			LC_UI_Cacl
 *	@brief		Caculate the ui wakeup time.
 *	@param[in]	ui_bl		:current running parameter of led or buzzer.
 *	@return		running time of led.
 */
uint32 LC_UI_Cacl(lc_ui_run_para *ui_bl)
{
	if(ui_bl->cur_mode == 0) return 0;

	if(ui_bl->next_wakeup_tick - ((hal_systick()|1) + 2) < BIT(30)){
		return ui_bl->next_wakeup_tick;
	}
	if(ui_bl->cur_state && ui_bl->cur_cnt && ui_bl->cur_cnt != 0xff){//!=0 !=0xff ==on
		ui_bl->cur_cnt --;
	}
	if(ui_bl->cur_cnt == 0 ){
		ui_bl->cur_mode = ui_bl->ui_type[ui_bl->cur_mode].next_mode;
		ui_bl->cur_cnt = ui_bl->ui_type[ui_bl->cur_mode].offOn_cnt;
	}
	if(ui_bl->cur_cnt && ui_bl->ui_type[ui_bl->cur_mode].offOn_Ms[0] == 0){
		ui_bl->cur_state = 1;
	}else {
		ui_bl->cur_state = ui_bl->cur_state ? 0: 1;
	}
	//ui_bl->cur_state = ui_bl->cur_state ? 0: 1;
	ui_bl->next_wakeup_tick = hal_systick() + ui_bl->ui_type[ui_bl->cur_mode].offOn_Ms[ui_bl->cur_state] ;
	return ui_bl->next_wakeup_tick;
}
/*!
 *	@fn				LC_UI_Tick_Process
 *	@brief			deal the mode of led or buzzer,and manage power.
 *	@param[in]		none.
 *	@retrurn		none.
 */
uint32 LC_UI_Tick_Process(void)
{
	static	uint32	next_led_wakeup_timerout	=	0;
	uint32	next_led_wakeup_tick_1	=	LC_UI_Cacl(&Led_No1_Param);
	uint32	next_buzzer_wakeup_tick	=	LC_UI_Cacl(&Buzzer_Param);

	if(Led_No1_Param.cur_mode || Buzzer_Param.cur_mode)
	{
		if(Led_No1_Param.cur_state || Buzzer_Param.cur_state){
			LC_Dev_System_Param.dev_lowpower_flag	=	1;
			next_led_wakeup_timerout	=	hal_systick()|1;
		}else{
			LC_Dev_System_Param.dev_lowpower_flag	=	0;
		}
	}else{
		if(next_led_wakeup_timerout && clock_time_exceed_func(next_led_wakeup_timerout, 100)){
			LC_Dev_System_Param.dev_lowpower_flag	=	0;
			next_led_wakeup_timerout	=	0;
		}
	}
	if(LC_Dev_System_Param.dev_lowpower_flag	!=	2){
		hal_pwrmgr_unlock(MOD_USR8);	//	low power mode
	}else{
		hal_pwrmgr_lock(MOD_USR8);
	}

	LC_Led_No1_Onoff(Led_No1_Param.cur_state);
	LC_Buzzer_Onoff(Buzzer_Param.cur_state);

	return	next_led_wakeup_tick_1;
}
void LC_UI_Enter_Mode(lc_ui_run_para *ui_param, uint8 mode)
{
	ui_param->cur_cnt = ui_param->ui_type[mode].offOn_cnt;
	ui_param->cur_mode = mode;
	ui_param->cur_state = 0;
	ui_param->next_wakeup_tick = hal_systick()|1;
}
/*------------------------------------------------------------------*/
/* 					 	public functions		 					*/
/*------------------------------------------------------------------*/
/**
 * @brief 	Initlize of Pins.
 * 
 */
void	LC_GPIO_RGBPinInit(void)
{

	hal_gpio_pin_init(MY_GPIO_LED_R, OEN);
	hal_gpio_write(MY_GPIO_LED_R, 0);

	hal_gpio_pin_init(MY_GPIO_LED_G, OEN);
	hal_gpio_write(MY_GPIO_LED_G, 0);

	hal_gpio_pin_init(MY_GPIO_LED_B, OEN);
	hal_gpio_write(MY_GPIO_LED_B, 0);

#if(LC_RGBLight_Module == RGBWLight)	
	hal_gpio_pin_init(MY_GPIO_LED_WW, OEN);
	hal_gpio_write(MY_GPIO_LED_WW, 0);
#endif

	hal_gpio_pin_init(MY_GPIO_LED_NO1, OEN);
	LED_NO1_OFF();

	hal_gpio_pin_init(MY_GPIO_PWM_NO1, OEN);
	BUZZER_OFF();
}
/**
 * @brief 	Set RGB value to PWM register.
 * 
 */
void	LC_PWMSetRGBValue(void)
{
	hal_pwm_set_count_val(PWM_CH0, LC_RGBLight_Param.RGB_rValue, RGB_PWM_MAX);
	hal_pwm_set_count_val(PWM_CH1, LC_RGBLight_Param.RGB_gValue, RGB_PWM_MAX);
	hal_pwm_set_count_val(PWM_CH2, LC_RGBLight_Param.RGB_bValue, RGB_PWM_MAX);

#if(LC_RGBLight_Module == RGBWLight)
	hal_pwm_set_count_val(PWM_CH3, LC_RGBLight_Param.RGB_wValue, RGB_PWM_MAX);
#endif

}

/*!
 *	@fn			LC_Gpio_UI_Led_Buzzer_Init
 *	@brief		Initialize the LED and Buzzer pins. 
 *	@param[in]	none.
 *	@return		none.
 */
void	LC_Gpio_UI_Led_Buzzer_Init(void)
{
	LC_GPIO_RGBPinInit();
	// LC_ReadReservedData();
	LC_PWMChannelInit();
	LC_PWMSetRGBValue();
	hal_pwm_start();
}
/*!
 *	@fn			LC_Switch_Poweron
 *	@brief		press switch to power on.
 *	@param[in]	cur_state	:
 *	@param[in]	power_start_tick	:set time for long press to poweron,
 *									power_start_tick*25ms
 *	@return		none.
 */
void LC_Switch_Poweron(uint8 cur_state, uint8 power_start_tick)
{
	if(LC_Dev_System_Param.dev_poweron_switch_flag)
	{
		LC_Dev_System_Param.dev_power_flag		=	SYSTEM_WORKING;
		return;
	}
	uint8	poweron_start_num	=	power_start_tick;
	static	uint32	poweron_start_time_100ms;
	if(!cur_state){
		while(poweron_start_num){
			WaitUs(1000);
			if(clock_time_exceed_func(poweron_start_time_100ms, 25)){
				poweron_start_time_100ms	=	hal_systick() | 1;
				if(hal_gpio_read(MY_KEY_NO1_GPIO) == 0){
					poweron_start_num--;
				}else{
					poweron_start_num	=	power_start_tick;
					LC_Dev_System_Param.dev_power_flag		=	SYSTEM_STANDBY;
					LC_Dev_Poweroff(0);
					return ;
				}
			}
		}
		poweron_start_time_100ms	=	hal_systick() | 1;
		while(hal_gpio_read(MY_KEY_NO1_GPIO) == 0){		//	release key after power on if key didn't release
			if(clock_time_exceed_func(poweron_start_time_100ms, 500)){
				poweron_start_time_100ms	=	hal_systick() | 1;
				LC_Dev_System_Param.dev_power_flag		=	SYSTEM_WORKING;
				return;
			}
		}
		LC_Dev_System_Param.dev_power_flag		=	SYSTEM_WORKING;
	}
	LC_Dev_Poweroff(0);
}
/*!
 *	@fn			LC_Dev_Poweroff
 *	@brief		the process of power off,need to disable adv and all events.
 *	@param[in]	none.
 *	@return		none.
 */
void	LC_Dev_Poweroff(uint8 ledenable)
{
	if(ledenable)
	{
		for(uint8 i = 0;i<2;i++)
		{
			LC_Buzzer_Onoff(1);
			WaitMs(100);
			LC_Buzzer_Onoff(0);
			WaitMs(100);
		}
	}
	LC_GPIO_RGBPinInit();

	LOG("system flag[]\n");
	pwroff_cfg_t	User_Set_Wakeup[1];
	User_Set_Wakeup[0].pin	=	MY_KEY_NO1_GPIO;
	User_Set_Wakeup[0].type	=	NEGEDGE;
	User_Set_Wakeup[0].on_time	=	5;


	hal_pwrmgr_unlock(MOD_USR8);

	AP_WDT->CRR	=	0x76;	//	feed watch dog
	while(hal_gpio_read(MY_KEY_NO1_GPIO) == 0){
		WaitUs(10*1000);
		AP_WDT->CRR	=	0x76;	//	feed watch dog
	}
	hal_pwrmgr_poweroff(&User_Set_Wakeup[0], 1);
}

void	LC_Led_No1_Enter_Mode(uint8 mode)
{
	LC_UI_Enter_Mode(&Led_No1_Param, mode);
	// if(LC_Dev_System_Param.dev_power_flag){
		// LC_Dev_System_Param.dev_lowpower_flag	=	1;
		// osal_start_timerEx(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL1, 5);
		osal_set_event(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL5);
	// }
}
void	LC_Buzzer_Enter_Mode(uint8 mode)
{
	LC_UI_Enter_Mode(&Buzzer_Param, mode);
	osal_set_event(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL5);
}
/*!
 *	@fn			LC_UI_Led_Buzzer_Task_Init 
 *	@brief		Initialize function for the UI_LED_BUZZER Task. 
 *	@param[in]	task_id			:the ID assigned by OSAL,
 *								used to send message and set timer.
 *	@retrurn	none.
 */
void	LC_UI_Led_Buzzer_Task_Init(uint8 task_id)
{
	LC_Ui_Led_Buzzer_TaskID	=	task_id;
	LOG("LC_Gpio_UI_Led_Buzzer_Init:\n");

#if(LC_RGBLight_Key_Enable == 1)
	LC_Gpio_Key_Init();
#endif
	LC_Switch_Poweron(0, 60);
	LC_Gpio_UI_Led_Buzzer_Init();
	LC_Timer_Start();
	if(hal_gpio_read(GPIO_USB_CHECK) == 1)
	{
		LC_Dev_System_Param.dev_power_flag	=	SYSTEM_STANDBY;
		osal_start_timerEx(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL5, 100);
	}
	if(LC_Dev_System_Param.dev_power_flag == SYSTEM_WORKING)
	{
		osal_start_timerEx(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL5, 10);
		osal_start_timerEx(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL2, 1000);
		LC_Led_No1_Enter_Mode(2);
		LC_Buzzer_Enter_Mode(1);
	}
}
/*!
 *	@fn			LC_UI_Led_Buzzer_ProcessEvent
 *	@brief		UI_LED_BUZZER Task event processor.This function
 *				is called to processs all events for the task.Events
 *				include timers,messages and any other user defined events.
 *	@param[in]	task_id			:The OSAL assigned task ID.
 *	@param[in]	events			:events to process.This is a bit map and can
 *									contain more than one event.
 */
uint16	LC_UI_Led_Buzzer_ProcessEvent(uint8 task_id, uint16 events)
{
	VOID task_id;	// OSAL required parameter that isn't used in this function
	if(events & SYS_EVENT_MSG){
		uint8	*pMsg;
		if((pMsg = osal_msg_receive(LC_Ui_Led_Buzzer_TaskID)) != NULL){
			LC_Common_ProcessOSALMsg((osal_event_hdr_t *)pMsg);
            // Release the OSAL message
			VOID osal_msg_deallocate(pMsg);
		}
		return(events ^ SYS_EVENT_MSG);
	}
	if(events & UI_EVENT_LEVEL1)
	{
		LC_PWMSetRGBValue();
		return(events ^ UI_EVENT_LEVEL1);
	}

	if(events & UI_EVENT_LEVEL2)
	{
		LC_Dev_WorkingTimer();
		osal_start_timerEx(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL2, 1000);
		return(events ^ UI_EVENT_LEVEL2);
	}
	// deal with datas from APP
	if(events & UI_EVENT_LEVEL3)
	{
		attHandleValueNoti_t notif;

		osal_memcpy(notif.value, LC_App_Set_Param.app_write_data, LC_App_Set_Param.app_write_len);
		notif.len		=	LC_App_Set_Param.app_write_len;
		notif.value[0]	=	0xBB;
		// LOG("Decrype data: ");
		// LOG_DUMP_BYTE(LC_App_Set_Param.app_write_data,LC_RXD_VALUE_LEN);
		uint8	checksum_buf	=	0;
		checksum_buf	=	LC_CheckSum(LC_App_Set_Param.app_write_data + 1, LC_App_Set_Param.app_write_data[2] + 2);
		LOG("check sum = %x\n",checksum_buf);
		if((LC_App_Set_Param.app_write_data[LC_App_Set_Param.app_write_len - 1] == checksum_buf) && (LC_App_Set_Param.app_write_data[0] == 0xAA) && (LC_Dev_System_Param.dev_charge_flag == 0))
		{
			//	turn on
			if(LC_App_Set_Param.app_write_data[1] == 0x01)
			{
				if(LC_RGBLight_Param.RGB_Light_State == State_Off)
				{
					LC_RGBLight_Turn_Onoff(State_On);
				}
				notif.value[notif.len - 1]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
			}
			//	turn off
			else if(LC_App_Set_Param.app_write_data[1] == 0x02)
			{
				if(LC_RGBLight_Param.RGB_Light_State == State_On)
				{
					LC_RGBLight_Turn_Onoff(State_Off);
				}
				notif.value[notif.len - 1]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
			}
			//	set mode
			else if(LC_App_Set_Param.app_write_data[1] == 0x03)
			{
				if(LC_RGBLight_Param.RGB_Light_State == State_On)
				{
					if((LC_App_Set_Param.app_write_data[3] <= 7) && (LC_App_Set_Param.app_write_data[3] >= 1))
					{
						LC_RGBLight_Param.RGB_Light_Mode	=	LC_App_Set_Param.app_write_data[3] + 0x5F;
						LC_RGBLight_Mode_Static_OneColor(LC_RGBLight_Param.RGB_Light_Mode);
					}
					else if(LC_App_Set_Param.app_write_data[3] == 8)
					{
						LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Jump_SevenColors;
						LC_RGBLight_Modetick				=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Speed_Reserved		=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Speed		=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					}
					else
					{
						LOG("mode err\n");
					}
				}
				notif.value[notif.len - 1]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
			}
			//	set light level
			else if(LC_App_Set_Param.app_write_data[1] == 0x04)
			{
				LC_RGBLight_Param.RGB_Light_Level	=	LC_App_Set_Param.app_write_data[3];
				if(LC_RGBLight_Param.RGB_Light_State == State_On)
				{
					if((LC_RGBLight_Param.RGB_Light_Mode	>= RGB_Static_Red) && (LC_RGBLight_Param.RGB_Light_Mode <= RGB_Static_White))
					{
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					}
				}
				notif.value[notif.len - 1]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
			}
			//	set timer
			else if(LC_App_Set_Param.app_write_data[1] == 0x05)
			{
				if(LC_App_Set_Param.app_write_data[3] == 0x01)
				{
					LC_RGBLight_Param.RGB_Light_TimerEn			=	State_On;
					LC_RGBLight_Param.RGB_Light_TimerSeconds	=	LC_App_Set_Param.app_write_data[5]*60 + LC_App_Set_Param.app_write_data[6] + 1;
					LC_Dev_System_Param.dev_timeout_poweroff_cnt	=	POWEROFF_TIMER_CNT;
				}
				else if(LC_App_Set_Param.app_write_data[3] == 0x00)
				{
					LC_RGBLight_Param.RGB_Light_TimerEn			=	State_Off;
					LC_Dev_System_Param.dev_timeout_poweroff_cnt	=	POWEROFF_TIMER_CNT;
				}
				notif.value[notif.len - 1]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
			}
			//	request information
			else if(LC_App_Set_Param.app_write_data[1] == 0x06)
			{
				notif.value[2]	=	0x06;
				notif.value[3]	=	LC_RGBLight_Param.RGB_Light_State;
				if((LC_RGBLight_Param.RGB_Light_Mode >= RGB_Static_Red) && (LC_RGBLight_Param.RGB_Light_Mode <= RGB_Static_White))
				{
					notif.value[4]	=	LC_RGBLight_Param.RGB_Light_Mode - 0x5F;
				}
				else if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Jump_SevenColors)
				{
					notif.value[4]	=	8;
				}
				notif.value[5]	=	LC_RGBLight_Param.RGB_Light_TimerEn;
				notif.value[6]	=	0;
				notif.value[7]	=	(uint8)(LC_RGBLight_Param.RGB_Light_TimerSeconds/60);
				notif.value[8]	=	(uint8)(LC_RGBLight_Param.RGB_Light_TimerSeconds%60);
				notif.value[9]	=	LC_CheckSum(notif.value + 1, notif.value[2] + 2);
				notif.len		=	10;	
			}
			LC_Buzzer_Enter_Mode(2);
			simpleProfile_Notify(&notif);
		}
		return(events ^ UI_EVENT_LEVEL3);
	}

	if(events & UI_EVENT_LEVEL4){
		uint8	Flash_Reserved_Mode[16]	=	{0x55,};

		Flash_Reserved_Mode[1]	=	LC_RGBLight_Param.RGB_Light_Mode;
		if((LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode) || ((LC_RGBLight_Param.RGB_Light_Mode >= RGB_Static_Red) && (LC_RGBLight_Param.RGB_Light_Mode <= RGB_Static_White))){
			Flash_Reserved_Mode[2]	=	(uint8)((LC_RGBLight_Param.RGB_rValue >> 8) & 0xff);
			Flash_Reserved_Mode[3]	=	(uint8)(LC_RGBLight_Param.RGB_rValue & 0xff);
			Flash_Reserved_Mode[4]	=	(uint8)((LC_RGBLight_Param.RGB_gValue >> 8) & 0xff);
			Flash_Reserved_Mode[5]	=	(uint8)(LC_RGBLight_Param.RGB_gValue & 0xff);
			Flash_Reserved_Mode[6]	=	(uint8)((LC_RGBLight_Param.RGB_bValue >> 8) & 0xff);
			Flash_Reserved_Mode[7]	=	(uint8)(LC_RGBLight_Param.RGB_bValue & 0xff);
		#if(LC_RGBLight_Module == RGBWLight)
			Flash_Reserved_Mode[8]	=	(uint8)((LC_RGBLight_Param.RGB_wValue >> 8) & 0xff);
			Flash_Reserved_Mode[9]	=	(uint8)(LC_RGBLight_Param.RGB_wValue & 0xff);
		#endif
		}

		Flash_Reserved_Mode[12]	=	(uint8)((LC_RGBLight_Param.RGB_Mode_Change_Speed >> 8) & 0xff);
		Flash_Reserved_Mode[13]	=	(uint8)(LC_RGBLight_Param.RGB_Mode_Change_Speed  & 0xff);

		osal_snv_write(0x88, 16, Flash_Reserved_Mode);
		LOG("write flash mode\n");
		return(events ^ UI_EVENT_LEVEL4);
	}
	
	if(events & UI_EVENT_LEVEL5){
		if(LC_Dev_System_Param.dev_power_flag == SYSTEM_WORKING)
		{
			LC_UI_Tick_Process();
			osal_start_timerEx(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL5, 100);
		}
		else if(LC_Dev_System_Param.dev_power_flag == SYSTEM_STANDBY)
		{
			LC_Dev_Poweroff(1);
		}
		return(events ^ UI_EVENT_LEVEL5);
	}

    // Discard unknown events

    return 0;
}
/** @}*/

