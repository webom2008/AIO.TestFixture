#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "WaveformDriver.h"

#pragma comment(lib, "visa32.lib")

CWaveformDriver *gpWaveformDev = NULL;

CWaveformDriver::CWaveformDriver(void)
    :m_bIsDeviceOpen(false)
{
}


CWaveformDriver::~CWaveformDriver(void)
{
    closeDevice();
}


int CWaveformDriver::initApplication(void)
{
    return 0;
}


bool CWaveformDriver::checkIsDeviceOpen(void)
{
    return m_bIsDeviceOpen;
}

int CWaveformDriver::openDevice(const char *pUsbDevName)
{
    ViUInt16 intfType;  
    ViString intfName[512]; 
    ViBoolean viFlag = VI_FALSE;  
    ViStatus ret = -1;

    //打开总的资源管理器，初始化资源管理器  
    ret = viOpenDefaultRM(&m_ViSessionRM); 
    if (VI_SUCCESS != ret)
    {
        TRACE("viOpenDefaultRM Error!!!\r\n");
        return -1;
    }
    else
    {
        TRACE("viOpenDefaultRM successfully!!!\r\n");
    }
  
    //打开指定的USB接口控制的函数发生器
    ret = viOpen(m_ViSessionRM, (ViRsrc)pUsbDevName, VI_NULL, VI_NULL, &m_ViSession33522B);  
    if (VI_SUCCESS == ret)
    {
        TRACE("viOpen>>Operation completed successfully!!!\r\n");
    }
    else if (VI_WARN_CONFIG_NLOADED == ret)
    {
        TRACE("viOpen>>The specified configuration either does not exist or "
            "could not be loaded using VISA-specified defaults.\r\n");
    }
    else
    {
        TRACE("viOpenDefaultRM Error!!!\r\n");
        AfxMessageBox("无法建立连接，请选择正确的USB口！");
        viClose(m_ViSessionRM);
        return -2;
    }
  
    //确认默认的函数发生器命令否以\n结束，这里定义的SCPI语言是必须以\n结尾的  
    if((VI_SUCCESS == viGetAttribute(m_ViSession33522B, VI_ATTR_RSRC_CLASS, intfName)) &&   
       (0 == strcmp("SOCKET", (ViString)intfName)))  
    {  
        viFlag = VI_TRUE;  
    }  
    else if((VI_SUCCESS == viGetAttribute(m_ViSession33522B, VI_ATTR_INTF_TYPE, &intfType)) &&   
            (intfType == VI_INTF_ASRL))  
    {  
        viFlag = VI_TRUE;  
    }

    ret = viSetAttribute(m_ViSession33522B, VI_ATTR_TERMCHAR_EN, viFlag);
    if (VI_SUCCESS == ret)
    {
        TRACE("viSetAttribute>> Attribute value set successfully!!!\r\n");
    }
    else if (VI_WARN_NSUP_ATTR_STATE == ret)
    {
        TRACE("viSetAttribute>> Although the specified attribute state is valid, "
            "it is not supported by this resource implementation. \r\n");
    }
    else 
    {
        TRACE("viSetAttribute>> Error!!!\r\n");
    }

    m_bIsDeviceOpen = true;
    return 0;
}

int CWaveformDriver::closeDevice(void)
{
    if (m_bIsDeviceOpen)
    {
        //关闭到指定的USB接口控制的函数发生器的连接  
        viClose(m_ViSession33522B);  
        //关闭总的资源管理器  
        viClose(m_ViSessionRM);
        m_bIsDeviceOpen = false;
    }
    return 0;
}


int CWaveformDriver::rset2Default(void)
{
    if (VI_SUCCESS != viPrintf (m_ViSession33522B, "*RST\n"))
    {
        TRACE(">>getDeviceIDN RST error\r\n");
        return -1;
    }
    return 0;
}

int CWaveformDriver::getDeviceIDN(void)
{
    if (!m_bIsDeviceOpen) return -1;
    char buf [256] = {0};

    /* Initialize device */
    if (VI_SUCCESS != viPrintf (m_ViSession33522B, "*RST\n"))
    {
        TRACE(">>getDeviceIDN RST error\r\n");
        return -2;
    }

    /* Send an *IDN? string to the device */
    if (VI_SUCCESS != viPrintf (m_ViSession33522B, "*IDN?\n"))
    {
        TRACE(">>getDeviceIDN IDN error\r\n");
        return -2;
    }
  
    /* Read results */
    if (VI_SUCCESS != viScanf (m_ViSession33522B, "%t", &buf))
    {
        TRACE(">>getDeviceIDN viScanf error\r\n");
        return -2;
    }

    /* Print results */
    TRACE ("Instrument identification string: %s\n", buf); 
    return 0;
}


//控制函数发生器产生矩形波 
int CWaveformDriver::testSampleCh1(void)
{
    if (!m_bIsDeviceOpen) return -1;

    //具体的命令操作语句，注意SCPI的写法和\n结尾  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "SQUare");                     //方波  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:HIGH %@3lf\n", 5.0);             //最大输出电压  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:LOW %@3lf\n", -5.0);             //最小输出电压  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:STATe %@1d\n", 1);                 
    viPrintf(m_ViSession33522B, ":SOURce1:FREQuency %@3lf\n", 100.0);                    //频率(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage %@3lf\n", 4.0);                        //幅值(V)  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:OFFSet %@3lf\n", 1.0);                 //偏移值(V)  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:SQUare:DCYCle %@3lf\n", 20.0);        //占空比(%)  
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);                                   //开启输出  

    return 0;
}


//控制函数发生器产生矩形波 
int CWaveformDriver::testSampleCh2(void)
{
    if (!m_bIsDeviceOpen) return -1;
    //具体的命令操作语句，注意SCPI的写法和\n结尾  
    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion %s\n", "SQUare");                     //方波  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:HIGH %@3lf\n", 5.0);             //最大输出电压  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:LOW %@3lf\n", -5.0);             //最小输出电压  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:STATe %@1d\n", 1);                 
    viPrintf(m_ViSession33522B, ":SOURce2:FREQuency %@3lf\n", 500.0);                    //频率(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage %@3lf\n", 4.0);                        //幅值(V)  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:OFFSet %@3lf\n", 1.0);                 //偏移值(V)  
    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:SQUare:DCYCle %@3lf\n", 20.0);        //占空比(%)  
    viPrintf(m_ViSession33522B, ":OUTPut2 %@1d\n", 1);                                   //开启输出  

    return 0;
}



/*
下面的代码可加载和修改内置任意波形。
FUNCtion ARB
VOLTage +3
VOLTage:OFFSet +1
FUNC:ARB:SRAT 1E5
FUNCtion:ARBitrary "INT:\BUILTIN\EXP_RISE.ARB"
OUTPut 1
*/
int CWaveformDriver::exampleARBFuncCh1InternalFile(void)
{
    if (!m_bIsDeviceOpen) return -1;

    //具体的命令操作语句，注意SCPI的写法和\n结尾  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "ARB");
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage %@3lf\n", 3.0);
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:OFFSet %@3lf\n", 1.0);
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary:SRATe %@3lf\n", 100000.0); //采样率 100KSa/s
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary \"INT:\\BUILTIN\\EXP_RISE.ARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);                                   //开启输出  

    return 0;
}

int CWaveformDriver::exampleARBFuncCh2USBDeviceFile(void)
{
    if (!m_bIsDeviceOpen) return -1;

    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion %s\n", "ARB");
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:HIGH %@3lf\n", 0.88);             //最大输出电压  
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:LOW %@3lf\n", -0.13);             //最小输出电压  
    //viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary:SRATe %@3lf\n", 10000.0);    //采样率 10KSa/s
    
    viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA2 \"USB:\\C01_001.BARB\"\n");
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary \"USB:\\C01_001.BARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut2 %@1d\n", 1);                                   //开启输出  
    
    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion %s\n", "ARB");
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:HIGH %@3lf\n", 0.88);             //最大输出电压  
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:LOW %@3lf\n", -0.13);             //最小输出电压  
    //viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary:SRATe %@3lf\n", 10000.0);    //采样率 10KSa/s
    
    viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA2 \"USB:\\C01_002.BARB\"\n");
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary \"USB:\\C01_002.BARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut2 %@1d\n", 1);                                   //开启输出  
    return 0;
}





int CWaveformDriver::setFuncSin(UINT8 channel, float freq_hz, float amp_V, float high_V, float low_V, float offset_V)
{
/*
SOURce2:FUNCtion SIN
SOURce2:FREQuency +1.0E+06
SOURce2:VOLTage +2.0
SOURce2:VOLTage:OFFSet +0.0
SOURce2:VOLTage:LIMit:LOW -1.0
SOURce2:VOLTage:LIMit:HIGH +1.0
SOURce2:VOLTage:LIMit:STATe 1
OUTP2 ON
*/
    if (!m_bIsDeviceOpen) return -1;

    if ((0 == high_V)&&(0 == low_V))
    {
        high_V = amp_V;
    }
    //具体的命令操作语句，注意SCPI的写法和\n结尾  
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");                           //高阻抗
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"SINusoid");      
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FREQuency %@3lf\n", channel,freq_hz);                  //频率(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage %@3lf\n", channel,amp_V);                      //幅值(V)  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:LIMit:HIGH %@3lf\n",channel,high_V);           //最大输出电压  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:LIMit:LOW %@3lf\n", channel,low_V);            //最小输出电压  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:LIMit:STATe %@1d\n", channel,0);               //启用或禁用输出振幅电压限制,默认OFF(0)        
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:OFFSet %@3lf\n", channel,offset_V);            //偏移值(V)
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);                                  //开启输出  
    return 0;
}

int CWaveformDriver::setFuncPULSe (UINT8 channel, float freq_hz, float volt_V, float lead_s, float tra_s, float widt_s, float offset_V)
{
/*
FUNC PULS
FUNC:PULS:TRAN:LEAD 4E-8
FUNC:PULS:TRAN:TRA 1E-6
FUNC:PULS:WIDT 3E-6
FREQ 2E5
VOLT 3
OUTP ON
*/
    if (!m_bIsDeviceOpen) return -1;

    //具体的命令操作语句，注意SCPI的写法和\n结尾  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"PULSe");      
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FREQuency %@3lf\n", channel,freq_hz);                  //频率(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage %@3lf\n", channel,volt_V);                     //幅值(V)        
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:OFFSet %@3lf\n", channel,offset_V);            //偏移值(V)
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:PULSe:WIDTh %@3lf\n", channel,widt_s);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:PULSe:TRANsition:LEADing %@3lf\n",channel,lead_s);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:PULSe:TRANsition:TRAiling %@3lf\n", channel,tra_s);     
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);                                           //开启输出  
    return 0;
}

int CWaveformDriver::setPaceByEnum(int type)
{
    switch (type)
    {
    case PACE_A:
        gpWaveformDev->setFuncPULSe(1, 1.0f, 0.002f, 0.0000001f, 0.0000001f, 0.002f, 0.0f);
        break;
    case PACE_B:
        gpWaveformDev->setFuncPULSe(1, 1.0f, -0.002f, 0.0000001f, 0.0000001f, 0.002f, 0.0f);
        break;
    case PACE_C:
        gpWaveformDev->setFuncPULSe(1, 1.0f, 0.7f, 0.0000001f, 0.0000001f, 0.002f, 0.0f);
        break;
    case PACE_D:
        gpWaveformDev->setFuncPULSe(1, 1.0f, -0.7f, 0.0000001f, 0.0000001f, 0.002f, 0.0f);
        break;
    case PACE_E:
        gpWaveformDev->setFuncPULSe(1, 1.0f, 0.002f, 0.0000001f, 0.0000001f, 0.0001f, 0.0f);
        break;
    case PACE_F:
        gpWaveformDev->setFuncPULSe(1, 1.0f, -0.002f, 0.0000001f, 0.0000001f, 0.0001f, 0.0f);
        break;
    case PACE_G:
        gpWaveformDev->setFuncPULSe(1, 1.0f, 0.7f, 0.0000001f, 0.0000001f, 0.0001f, 0.0f);
        break;
    case PACE_H:
        gpWaveformDev->setFuncPULSe(1, 1.0f, -0.7f, 0.0000001f, 0.0000001f, 0.0001f, 0.0f);
        break;
    default:
        break;
    }
    return 0;
}