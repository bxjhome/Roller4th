--[[
ģ�����ƣ��绰������
ģ�鹦�ܣ����Ե绰����д
ģ������޸�ʱ�䣺2017.05.23
]]

module(...,package.seeall)
require"qrencode"

--[[
��������print
����  ����ӡ�ӿڣ����ļ��е����д�ӡ�������testǰ׺
����  ����
����ֵ����
]]
local function print(...)
	_G.print("ljd test",...)
end

local function test_print()
	print("enter test_main.....")
end

local function test_main_obj()
    local qr_object = qrencode.create("/ldata/test1.jpg")

    print("test_main_obj create",qr_object)

    local wd,ht = qr_object:info()

    print("test_main_obj info",wd,ht)

    local buf = qr_object:buffer()

    print("test_main_obj buffer",buf)

   qr_object:destory()

    print("test_main_obj destory")
end


--ѭ����ʱ��ֻ��Ϊ���ж�PB����ģ���Ƿ�ready
sys.timer_loop_start(test_print,2000)

sys.timer_loop_start(test_main_obj,5000)
