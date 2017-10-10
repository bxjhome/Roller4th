module(...,package.seeall)
require"misc"
require"http"
require"common"
--[[
���ܽ��ܣ�http�����ӣ�������Ҫ�ṩADDR��PORT�������ݾ��ǿͻ�����Ҫ���ӵĿͻ���
1.��Ҫ���ú�����������url�����ͷ�������ʵ�壬����ע������ײ�Hostʱ����ǰ���ADDR��PORTһ��,���õ���socket�ĳ�����
2.����request�������ú����Ƿ��ͱ�����������Ҫ���õ�
3.rcvcb�����ǽ��ջص��������᷵�ؽ����״̬�룬�ײ���һ������ʵ�壬�ú������Զ��庯�����ͻ����Ը����Լ��������Լ�����
4.�������ݺ����������û���ٴ���������������������
]]
local ssub,schar,smatch,sbyte,slen = string.sub,string.char,string.match,string.byte,string.len
--����ʱ����д��IP��ַ�Ͷ˿ڣ�������д���ײ�Ҫ�������hostһ�£������ֵ����Ĭ�ϵ�ֵ
local ADDR,PORT ="www.linuxhub.org",80
--����POST����ʱ���õ�ַ
--local ADDR,PORT ="www.luam2m.com",80
local httpclient

--[[
��������print
����  ����ӡ�ӿڣ����ļ��е����д�ӡ�������testǰ׺
����  ����
����ֵ����
]]
local function print(...)
	_G.print("test",...)
end


--[[
��������rcvcb
���ܣ����ջص��������û��Զ���Խ��ղ������в���
������result��0����ʾ ����ʵ�峤����ʵ����ͬ����ȷ��� 1����ʾû��ʵ��	2����ʾʵ�峬��ʵ��ʵ�壬���󣬲����ʵ������	3�����ճ�ʱ	4:��ʾ���������е��Ƿֿ鴫��ģʽ
����ֵ��
]]
local function rcvcb(result,statuscode,head,body)
	print("resultrcvcb: ",result)  
	print("statuscodercvcb: ",statuscode)
	if	head==nil	then	print("headrcvcb:	nil")
	else
		print("headrcvcb:")
		--������ӡ������ͷ������Ϊ�ײ����֣�������Ӧ��ֵΪ�ײ����ֶ�ֵ
		for k,v in pairs(head) do		
			print(k..": "..v)
		end
	end
	print("bodyrcvcb:")
	print(body)
	httpclient:disconnect(discb)
end


--[[
��������connectedcb
����  ��SOCKET connected �ɹ��ص�����
����  ��
����ֵ��
]]
local function connectedcb()
	local cmdtyp="GET"
	local url="/"
	--����ע�⣬head��table���ͣ���ֵ������XXXX: XXXX��ʽ
	local head={"Host: 112.29.250.194"}
	local body=""
	--���ô˺����Żᷢ�ͱ���,request(cmdtyp,url,head,body,rcvcb),�ص�����rcvcb(result,statuscode,head,body)
    httpclient:request(cmdtyp,url,head,body,rcvcb)
end 

--[[
��������sckerrcb
����  ��SOCKETʧ�ܻص�����
����  ��
		r��string���ͣ�ʧ��ԭ��ֵ
		CONNECT: socketһֱ����ʧ�ܣ����ٳ����Զ�����
����ֵ����
]]
local function sckerrcb(r)
	print("sckerrcb",r)
end
--[[
��������connect
���ܣ����ӷ�����
������
	 connectedcb:���ӳɹ��ص�����
	 sckerrcb��http lib��socketһֱ����ʧ��ʱ�������Զ�������������ǵ���sckerrcb����
���أ�
]]
local function connect()
	httpclient:connect(connectedcb,sckerrcb)
end
--[[
��������discb
����  ��HTTP���ӶϿ���Ļص�
����  ����		
����ֵ����
]]
function discb()
	print("http discb")
	--20������½���HTTP����
	sys.timer_start(connect,20000)
end

--[[
��������http_run
����  ������http�ͻ��ˣ�����������
����  ����		
����ֵ����
]]
function http_run()
	--��ΪhttpЭ�������ڡ�TCP��Э�飬���Բ��ش���PROT����
	httpclient=http.create(ADDR,PORT)	
	--����http����
	connect()	
end


--���ú�������
http_run()



