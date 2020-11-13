EEPROM编写思路

利用IIC驱动，调用时没有初始化，直接使用读或写函数即可。

补充WriteEEPROM(),ReadEEPROM();三个函数

WriteEEPROM()函数，接收一个地址，一个值

首先启动IIC

然后发送0xa0指令

等待

发送地址

等待

发送值

等待

结束IIC

延时



ReadEEPROM()函数，接收一个地址，一个值

首先定义一个返回值

启动IIC

发送0xa0指令

等待

发送地址

等待

启动IIC

发送0xa1指令

等待

返回值 = IIC读取函数()

发送应答(参数为1)

IIC结束

将值返回