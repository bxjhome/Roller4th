/*
  Copyright 2014-2015 juma.io

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "juma_sdk_api.h"

#define BEEPER_CTL 9
#define SERIAL_RX 29
#define SERIAL_TX 28

void on_ready()
{
    SEGGER_RTT_WriteString(0, "on_ready\n");	
    serial_setup(SERIAL_RX, SERIAL_TX, UART_BAUDRATE_Baud115200);
    timer_init(9, TIMER_PERIODIC);
    timer_start(2000);//2000ms is the max value
    ble_device_set_name("ROLLER_ECHO_DEMO");
    ble_device_start_advertising();	 
}

void timer_on_fired(void)
{	
    SEGGER_RTT_WriteString(0, "timer_on_fired\n");	
	
 //   if(0 != ble_device_is_connected()){
 //       sprintf(serial_data,"%d %d",v_actual_press_combined_u32,v_actual_temp_combined_s32);  
 //       ble_device_send(1, strlen(serial_data),(uint8_t *)serial_data);
 //   }
  serial_send( "hello\r\n", strlen("hello\r\n") );  

 
    play_sound(BEEPER_CTL);
}
void serial_on_data(uint8_t data)
{
}

void ble_device_on_connect(void)
{

}

void ble_device_on_disconnect(uint8_t reason)
{
    ble_device_start_advertising();
}

void ble_device_on_message(uint8_t type, uint16_t length, uint8_t* value)
{
    ble_device_send(type, length, value);
}



