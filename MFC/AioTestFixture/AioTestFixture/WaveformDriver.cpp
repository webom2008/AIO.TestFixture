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
    //���ƺ����������������β� 
    ViSession rm;  
    ViSession Function_Generator_33522B;
    ViUInt16 intfType;  
    ViString intfName[512];  
  
    //���ܵ���Դ����������ʼ����Դ������  
    viOpenDefaultRM(&rm);  
  
    //��ָ����USB�ӿڿ��Ƶĺ���������  
    viOpen(rm, (ViRsrc)DEVICE_NAME, VI_NULL, VI_NULL, &Function_Generator_33522B);  
  
    //ȷ��Ĭ�ϵĺ����������������\n���������ﶨ���SCPI�����Ǳ�����\n��β��  
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
  
    //��������������䣬ע��SCPI��д����\n��β  
    viPrintf(Function_Generator_33522B, ":SOURce:FUNCtion %s\n", "SQUare");                     //����  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:LIMit:HIGH %@3lf\n", 5.0);             //��������ѹ  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:LIMit:LOW %@3lf\n", -5.0);             //��С�����ѹ  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:LIMit:STATe %@1d\n", 1);                 
    viPrintf(Function_Generator_33522B, ":SOURce:FREQuency %@3lf\n", 100.0);                    //Ƶ��(kHz)  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage %@3lf\n", 4.0);                        //��ֵ(V)  
    viPrintf(Function_Generator_33522B, ":SOURce:VOLTage:OFFSet %@3lf\n", 1.0);                 //ƫ��ֵ(V)  
    viPrintf(Function_Generator_33522B, ":SOURce:FUNCtion:SQUare:DCYCle %@3lf\n", 20.0);        //ռ�ձ�(%)  
    viPrintf(Function_Generator_33522B, ":OUTPut %@1d\n", 1);                                   //�������  
  
    //�رյ�ָ����USB�ӿڿ��Ƶĺ���������������  
    viClose(Function_Generator_33522B);  
  
    //�ر��ܵ���Դ������  
    viClose(rm);  
    return 0;
}

int CWaveformDriver::closeDevice(void)
{
    return 0;
}