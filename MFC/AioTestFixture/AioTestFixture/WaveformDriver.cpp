#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "WaveformDriver.h"
#include "visa.h"

#pragma comment(lib, "visa32.lib")

CWaveformDriver::CWaveformDriver(void)
{
}


CWaveformDriver::~CWaveformDriver(void)
{
}


int CWaveformDriver::initApplication(void)
{
    return 0;
}


//const char DEVICE_NAME[] = "USB0::2391::11015::MY52700871::0::INSTR";
const char DEVICE_NAME[] = "USB0::0x0957::0x2C07::MY52802529::0::INSTR";

int CWaveformDriver::openDevice(void)
{
    //控制函数发生器产生矩形波 
    ViSession rm;  
    ViSession Function_Generator_33522B;
    ViUInt16 intfType;  
    ViString intfName[512];  
  
    //打开总的资源管理器，初始化资源管理器  
    viOpenDefaultRM(&rm);  
  
    //打开指定的USB接口控制的函数发生器  
    viOpen(rm, (ViRsrc)DEVICE_NAME, VI_NULL, VI_NULL, &Function_Generator_33522B);  
  
    //确认默认的函数发生器命令否以\n结束，这里定义的SCPI语言是必须以\n结尾的  
    ViBoolean termDefaultFunction_Generator_33500B = VI_FALSE;  
    if((VI_SUCCESS == viGetAttribute(Function_Generator_33522B, VI_ATTR_RSRC_CLASS, intfName)) &&   
       (0 == strcmp("SOCKET", (ViString)intfName)))  
    {  
        termDefaultFunction_Generator_33500B = VI_TRUE;  
    }  
    else if((VI_SUCCESS == viGetAttribute(Function_Generator_33522B, VI_ATTR_INTF_TYPE, &intfType)) &&   
            (intfType == VI_INTF_ASRL))  
    {  
        termDefaultFunction_Generator_33500B = VI_TRUE;  
    }  
    viSetAttribute(Function_Generator_33522B, VI_ATTR_TERMCHAR_EN, termDefaultFunction_Generator_33500B);  
  
    //具体的命令操作语句，注意SCPI的写法和\n结尾  
    viPrintf(Function_Generator_33522B, ":SOURce:FUNCtion %s\n", "SQUare");                     //方波  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:LIMit:HIGH %@3lf\n", 5.0);             //最大输出电压  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:LIMit:LOW %@3lf\n", -5.0);             //最小输出电压  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:LIMit:STATe %@1d\n", 1);                 
    viPrintf(Function_Generator_33522B, ":SOURce:FREQuency %@3lf\n", 100.0);                    //频率(kHz)  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage %@3lf\n", 4.0);                        //幅值(V)  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:OFFSet %@3lf\n", 1.0);                 //偏移值(V)  
    viPrintf(Function_Generator_33522B, ":SOURce:FUNCtion:SQUare:DCYCle %@3lf\n", 20.0);        //占空比(%)  
    viPrintf(Function_Generator_33522B, ":OUTPut %@1d\n", 1);                                   //开启输出  
  
    //关闭到指定的USB接口控制的函数发生器的连接  
    viClose(Function_Generator_33522B);  
  
    //关闭总的资源管理器  
    viClose(rm);  
    return 0;
}

int CWaveformDriver::closeDevice(void)
{
    return 0;
}