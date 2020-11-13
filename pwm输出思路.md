pwm输出思路

设置定时器0，100微秒，16位自动重载

开中断

设置pwm_num,tt两个参数

设置中断复位为

if(tt == pwm_num)

{

​	响应

}

else if(tt==100)

{

​	关闭，复位tt

}