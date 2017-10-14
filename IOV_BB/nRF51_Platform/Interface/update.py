copyright = '''/*
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

'''


callbacks = [
    ['void', 'on_ready',[]],
    #['void', 'on_lock_state_changed', [('uint8_t', 'is_locked')]],
    ['void', 'gpio_on_change', [('uint32_t', 'pins_state')]],
    ['void', 'timer_on_fired', []],

    ['void', 'ble_device_on_connect', []],
    ['void', 'ble_device_on_disconnect', [('uint8_t', 'reason')]],
    ['void', 'ble_device_on_message', [('uint8_t', 'type'), ('uint16_t', 'length'), ('uint8_t*', 'value')]],

    #['void', 'hif_on_message', [('uint8_t*', 'data'), ('uint32_t', 'size')]],
    ['void', 'serial_on_data', [('uint8_t', 'data')]],
    ['void', 'data_storage_on_finish', [('uint8_t', 'op_code')]],
    ['void', 'watchDog_on_timerout', [('uint32_t', 'juma_wdt_statue')]]
]

syscalls = [
    ['void', 'run_when_idle', [('function_t', 'func'), ('void*', 'args')]],
    ['void', 'run_at_time', [('function_t', 'func'), ('void*', 'args'), ('uint32_t', 'time')]],
    ['uint32_t', 'current_time', []],

    ['void', 'timer_init', [('uint8_t', 'prescalar'), ('uint8_t', 'timer_type')]],
    ['uint32_t', 'timer_start', [('uint16_t', 'value')]],
    ['void', 'timer_stop', []],

    ['void', 'gpio_setup', [('uint8_t', 'pin'), ('uint8_t', 'mode')]],
    ['void', 'gpio_write', [('uint8_t', 'pin'), ('uint8_t', 'state')]],
    ['uint8_t', 'gpio_read', [('uint8_t',  'pin')]],
    ['void', 'gpio_watch', [('uint8_t', 'pin'), ('uint8_t', 'change_direction')]],
    ['void', 'gpio_unwatch', [('uint8_t', 'pin')]],

    ['void', 'adc_measure', [('uint8_t', 'pin'), ('uint8_t', 'resolution'), ('function_t', 'on_complete')]],
    ['void', 'vcc_measure', [('function_t', 'on_complete')]],
    ['int8_t', 'get_temperature', []],

    ['uint8_t', 'ble_device_is_connected', []],
    ['void', 'ble_device_get_id', [('uint8_t*', 'id'), ('uint8_t', 'len')]],
    ['void', 'ble_device_select_address', [('uint8_t', 'id')]],
    ['void', 'ble_device_set_name', [('const char*', 'device_name')]],
    ['void', 'ble_device_set_advertising_interval', [('uint16_t', 'interval')]],
    ['void', 'ble_device_set_tx_power', [('int8_t', 'tx_power')]],
    ['void', 'ble_device_start_advertising', []],
    ['void', 'ble_device_stop_advertising', []],
    ['void', 'ble_device_disconnect', []],
    ['void', 'ble_device_send', [('uint8_t', 'type'), ('uint32_t', 'length'), ('uint8_t*', 'value')]],
    ['uint8_t', 'random_generate',[]],
    #['void', 'hif_setup', [('uint8_t', 'rx_pin'), ('uint8_t', 'tx_pin')]],
    #['void', 'hif_send', [('uint8_t*', 'data'), ('uint32_t', 'size')]],
    ['void', 'serial_send', [('uint8_t*', 'data'), ('uint32_t', 'length')]],
    ['void', 'serial_setup', [('uint8_t', 'rx_pin'), ('uint8_t', 'tx_pin'), ('uint32_t', 'baudrate')]],
	
    ['void', 'spi_setup', [('spi_init_struct_t *', 'spi_struct')]],
    ['void', 'spi_transmit_receive', [('uint8_t *', 'tx_buff'), ('uint8_t *', 'rx_buff'), ('uint32_t', 'buff_len')]],
                
    ['void', 'play_sound', [('uint8_t', 'pin')]],

    ['void', 'light_setup', [('uint8_t*', 'pins'), ('uint8_t', 'is_active_high')]],
    ['uint32_t', 'light_on', []],
    ['void', 'light_off', []],
    ['void', 'light_set_color', [('const uint8_t*', 'rgb_values')]],

    ['uint32_t', 'data_storage_write', [('uint8_t', 'data_id'), ('uint8_t', 'data_len'), ('uint8_t *', 'data')]],
    ['uint32_t', 'data_storage_read', [('uint8_t', 'data_id'), ('uint8_t *', 'data_len'), ('uint8_t *', 'data')]],
    #['void', 'beacon_setup', [('uint8_t*', 'uuid'), ('uint16_t', 'major'), ('uint16_t', 'minor'), ('uint8_t', 'rssi')]],
    
    ['uint32_t', 'ble_infrared_config', [('ble_infrared_init_struct *', 'Init_struct')]],
    ['uint32_t', 'ble_infrared_send', [('const uint8_t *', 'signal_data'), ('uint8_t', 'lenth')]],
    ['uint32_t', 'ble_infrared_get_statue', []],

    ['void', 'gfx_init_screen', [('uint8_t*', 'buffer'), ('uint16_t', 'width'), ('uint16_t', 'height'), ('uint8_t', 'flags')]],
    ['void', 'gfx_clear', []],
    ['void', 'gfx_draw_pixel', [('uint16_t', 'x'), ('uint16_t', 'y'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_draw_line', [('uint16_t', 'x0'), ('uint16_t', 'y0'), ('uint16_t', 'x1'), ('uint16_t', 'y1'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_draw_rect', [('uint16_t', 'x'), ('uint16_t', 'y'), ('uint16_t', 'w'), ('uint16_t', 'h'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_fill_rect', [('uint16_t', 'x'), ('uint16_t', 'y'), ('uint16_t', 'w'), ('uint16_t', 'h'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_draw_circle', [('uint16_t', 'x'), ('uint16_t', 'y'), ('uint16_t', 'r'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_fill_circle', [('uint16_t', 'x'), ('uint16_t', 'y'), ('uint16_t', 'r'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_set_font', [('const gfx_font_t*', 'font')]],
    ['uint16_t', 'gfx_draw_char', [('uint16_t', 'x'), ('uint16_t', 'y'), ('char', 'c'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_draw_string', [('uint16_t', 'x'), ('uint16_t', 'y'), ('char*', 'str'), ('uint8_t', 'ppo')]],
    ['void', 'gfx_draw_image', [('uint16_t', 'x'), ('uint16_t', 'y'), ('const uint8_t*', 'image'), ('uint16_t', 'width'), ('uint16_t', 'height'), ('uint8_t', 'ppo')]],

    ['uint32_t', 'watchDog_Config', [('uint32_t', 'juma_wdt_en')]],
    ['uint32_t', 'watchDog_Start', [('uint32_t', 'juma_wdt_timer_out_value')]],
    ['void', 'watchDog_user_dog1_RR', []],
    ['void', 'watchDog_user_dog2_RR', []],
    ['void', 'watchDog_user_dog3_RR', []],
    ['void', 'watchDog_user_dog4_RR', []],
    ['int8_t', 'I2C_bus_write', [('uint8_t', 'dev_addr'), ('uint8_t', 'reg_addr'), ('uint8_t *', 'reg_data'), ('uint8_t', 'cnt')]],
    ['int8_t', 'I2C_bus_write_byte', [('uint8_t', 'dev_addr'), ('uint8_t', 'reg_addr'), ('uint8_t', 'value')]],
    ['int8_t', 'I2C_bus_read', [('uint8_t', 'dev_addr'), ('uint8_t', 'reg_addr'), ('uint8_t *', 'reg_data'), ('uint8_t', 'cnt')]],
    ['uint8_t', 'I2C_bus_read_byte', [('uint8_t', 'dev_addr'), ('uint8_t', 'reg_addr')]],
    ['void', 'nrf_delay_ms', [('uint32_t', 'number_of_ms')]],
]


def make_func_decl(func):
    if len(func[2]) == 0:
        params = 'void'
    else:
        plist = []
        for p in func[2]:
            plist.append('%s %s' %(p[0], p[1]))
        params = ', '.join(plist)
    return '%s %s(%s);\n' %(func[0], func[1], params)

def make_funcptr_decl(func):
    if len(func[2]) == 0:
        params = 'void'
    else:
        plist = []
        for p in func[2]:
            plist.append('%s %s' %(p[0], p[1]))
        params = ', '.join(plist)
    return '\t%s (*%s)(%s);\n' %(func[0], func[1], params)

# make app interface header

f = open('Include/juma_sdk_app_interface.h', 'w')
f.write('''
#ifndef _JUMA_SDK_APP_INTERFACE_H_
#define _JUMA_SDK_APP_INTERFACE_H_
#include "juma_sdk_vectors.h"

extern app_descriptor_t* app;

''')

for func in callbacks:
    if len(func[2]) == 0:
        params = ''
    else:
        plist = []
        for p in func[2]:
            plist.append(p[1])
        params = ', '.join(plist)
    f.write('#define %s(%s) \\\n' %(func[1], params))
    f.write('\tif(app) app->%s(%s)\n' %(func[1], params))
 
f.write('#endif\n')
f.close()

# make api header

f = open('Include/juma_sdk_api.h', 'w')
f.write(copyright)

f.write('''
#ifndef _JUMA_SDK_API_H_
#define _JUMA_SDK_API_H_
#include "juma_sdk_types.h"

#define run_after_delay(func, args, delay)      \
    run_at_time((func), (args), current_time() + (delay))
''')

for cb in callbacks:
    f.write(make_func_decl(cb))

for sc in syscalls:
    f.write(make_func_decl(sc))
 
f.write('#endif\n')
f.close()

# make vector table header

f = open('Include/juma_sdk_vectors.h', 'w')

f.write('''
#ifndef _JUMA_SDK_VECTORS_H_
#define _JUMA_SDK_VECTORS_H_
#include "juma_sdk_api.h"

typedef struct _app_descriptor_t app_descriptor_t;
typedef struct _system_descriptor_t system_descriptor_t;

''')

f.write('struct _app_descriptor_t  {\n')
f.write('\tvoid (*on_init)(const system_descriptor_t* sys);\n')
for cb in callbacks:
    f.write(make_funcptr_decl(cb))
f.write('};\n')

f.write('struct _system_descriptor_t  {\n')
for sc in syscalls:
    f.write(make_funcptr_decl(sc))
f.write('};\n')
 
f.write('#endif\n')
f.close()

# make vector table file

f = open('Source/juma_sdk_sys_vectors.c', 'w')
f.write('''
#include "juma_sdk_vectors.h"

''')

f.write('const system_descriptor_t sys = {\n')
for sc in syscalls:
    f.write('\t%s,\n' %(sc[1]))
f.write('};\n')
f.close()

f = open('Source/juma_sdk_lib.c', 'w')
f.write('''
#include "juma_sdk_vectors.h"

const system_descriptor_t* sys;

void on_init(const system_descriptor_t* system)
{
    sys = system;
}

''')

for cb in callbacks:
    if len(cb[2]) == 0:
        params = 'void'
    else:
        plist = []
        for p in cb[2]:
            plist.append('%s %s' %(p[0], p[1]))
        params = ', '.join(plist)
    f.write('__weak %s %s(%s) {}\n' %(cb[0], cb[1], params))
f.write('void on_init(const system_descriptor_t* sys);\n')
f.write('const app_descriptor_t app_descriptor __attribute__((section("descriptor"))) = {\n')
f.write('	on_init,\n')
for cb in callbacks:
    f.write('\t%s,\n' %(cb[1]))
f.write('};\n')

for func in syscalls:
    if len(func[2]) == 0:
        params = 'void'
        names = ''
    else:
        plist = []
        name_list = []
        for p in func[2]:
            plist.append('%s %s' %(p[0], p[1]))
            name_list.append(p[1])
        params = ', '.join(plist)
        names = ', '.join(name_list)
    f.write('inline %s %s(%s)\n' %(func[0], func[1], params))
    f.write('{\n\t')
    if (func[0] != 'void'):
        f.write('return ')
    f.write('sys->%s(%s);\n' %(func[1], names))
    f.write('}\n')
f.close()
