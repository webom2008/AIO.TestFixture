#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "AioTestFixture.h"
#include "WaveformDriver.h"

#pragma comment(lib, "visa32.lib")


CWaveformDriver *gpWaveformDev = NULL;


const char noErrString[256] = {"+0,\"No error\"\n"};




ViStatus __stdcall myExceptionHandler (
    ViSession vi,
    ViEventType eventType,
    ViEvent context,
    ViAddr usrHandle
    ) 
{
        ViStatus exceptionErrNbr; char     nameBuffer[256];
        ViString functionName = nameBuffer; char     errStrBuffer[256];
        /* Get the error value from the exception context */
        viGetAttribute( context, VI_ATTR_STATUS, &exceptionErrNbr );
        /* Get the function name from the exception context */
        viGetAttribute( context, VI_ATTR_OPER_NAME, functionName );
        errStrBuffer[0] = 0;
        viStatusDesc( vi, exceptionErrNbr, errStrBuffer );

        printf("ERROR: Exception Handler reports\n"
            "(%s)\n","VISA function '%s' failed witherror 0x%lx\n", 
            functionName,
            exceptionErrNbr, 
            errStrBuffer );
        return VI_SUCCESS;
}



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



void CWaveformDriver::BinaryArb(void)
{
    const char noErrString[] = "+0,\"No error\"\n";

    const int NUM_DATA_POINTS = 100000;
    float z[NUM_DATA_POINTS];

    char tBuffer[100];
    tBuffer[0] = '\0';

    ViRsrc instAddress = "TCPIP0::156.140.113.206::inst0::INSTR";
    ViSession rm = 0,
        vi = 0;

    //Open session for instrument.
    viOpenDefaultRM(&rm);
    viOpen(rm, instAddress, 0, 0, &vi);
    viSetAttribute(vi, VI_ATTR_TMO_VALUE, 10000);

    //Query Identity string and report.
    viPrintf(vi, "*IDN?\n");
    viScanf(vi, "%t", tBuffer);

    printf("Instrument Identity String: %s\n", tBuffer);

    //Clear and reset instrument
    viPrintf(vi, "*CLS;*RST\n");
    viPrintf(vi, "*OPC?\n");
    viScanf(vi, "%t", tBuffer);

    //Clear volatile memory
    viPrintf(vi, "SOURce1:DATA:VOLatile:CLEar\n");

    //Create simple ramp waveform
    for (int i = 0; i < NUM_DATA_POINTS; i++)
        z[i] = (i-1)/(float)NUM_DATA_POINTS;

    // swap the endian format
    viPrintf(vi, "FORM:BORD NORM\n");

    //Downloading 
    printf("Downloading Waveform...\n");

    viPrintf(vi, "SOURce1:DATA:ARBitrary testarb, %*zb\n", NUM_DATA_POINTS, z);

    // wait for the operation to complete before moving on
    viPrintf(vi, "*WAI\n");

    printf("Download Complete\n");

    //Set desired configuration
    viPrintf(vi, "SOURce1:FUNCtion:ARBitrary testarb\n"); // set current arb waveform to defined arb pulse
    viPrintf(vi, "SOURce1:FUNCtion ARB\n"); // turn on arb function
    viPrintf(vi, "SOURCE1:FUNCtion:ARB:SRATe 400000\n"); // set sample rate
    viPrintf(vi, "SOURCE1:VOLT 2\n"); // set max waveform amplitude to 2 Vpp
    viPrintf(vi, "SOURCE1:VOLT:OFFSET 0\n"); // set offset to 0 V
    viPrintf(vi, "OUTPUT1:LOAD 50\n"); // set output load to 50 ohms

    //Enable Output
    viPrintf(vi, "OUTPUT1 ON\n"); // turn on channel 1 output

    //Read Error/s
    viPrintf(vi, "SYSTEM:ERROR?\n");
    viScanf(vi, "%t", tBuffer);

    if (strcmp(tBuffer, noErrString) == 0)
    {
        printf("Output set without any error\n");
    }
    else
    {
        printf("Error reported: %s\n", tBuffer);
    }

    //OPTIONAL: Save Arb to USB stick, and/or internal memory titled TEST ARB.barb

    //viPrintf(vi, "MMEM:STOR:DATA \"USB:\TEST ARB.barb\"\n");
    //viPrintf(vi, "MMEM:STOR:DATA \"INT:\TEST ARB.barb\"\n");

    return;
}

int CWaveformDriver::openDevice(const char *pUsbDevName)
{
    ViUInt16 intfType;  
    ViString intfName[512]; 
    ViBoolean viFlag = VI_FALSE;  
    ViStatus ret = -1;
    ViAddr     myUserHandle = 0;

    //打开总的资源管理器，初始化资源管理器  
    ret = viOpenDefaultRM(&m_ViSessionRM); 
    if (ret < VI_SUCCESS )
    {
        TRACE("viOpenDefaultRM Error!!!\r\n");
        return -1;
    }
    else
    {
        TRACE("viOpenDefaultRM successfully!!!\r\n");
    }
  
//================================
    ///* Install the exception handler and enable events for it */
    //ret = viInstallHandler(m_ViSessionRM,VI_EVENT_EXCEPTION, myExceptionHandler,myUserHandle);
    //if ( ret < VI_SUCCESS ) 
    //{
    //    printf( "ERROR: viInstallHandler failed with error 0x%lx\n", ret );
    //}

    //ret = viEnableEvent(m_ViSessionRM, VI_EVENT_EXCEPTION, VI_HNDLR, VI_NULL);
    //if ( ret < VI_SUCCESS ) {
    //    printf( "ERROR: viEnableEvent failed with error 0x%lx\n", ret );
    //}

    ///* Generate an error to demonstrate that the
    //handler will be called */
    //ret = viOpen( m_ViSessionRM, "badVisaName", NULL,
    //    NULL, &vi );
    //if ( ret < VI_SUCCESS ) {

    //    printf("ERROR: viOpen failed with error 0x%lx\n"
    //        "Exception Handler should have beencalled\n"
    //        "before this message was printed.\n",ret
    //        );
    //}

//================================




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
	//char tBuffer[100];
	//Clear and reset instrument
	viPrintf(m_ViSession33522B, "*CLS;*RST\n");
	//viPrintf(m_ViSession33522B, "*OPC?\n");
 //   viScanf(m_ViSession33522B, "%t", tBuffer);
    WaitOperComplete(m_ViSession33522B);
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

void CWaveformDriver::system_err(void)
{
    ViStatus err;
    char buf[1024]={0};
    int err_no;

    err = viPrintf(m_ViSession33522B, "SYSTEM:ERR?\n");
    if (err < VI_SUCCESS) err_handler (m_ViSession33522B, err);

    err = viScanf (m_ViSession33522B, "%d%t", &err_no, &buf);
    if (err < VI_SUCCESS) err_handler (m_ViSession33522B, err);

    while (err_no >0){
        printf ("Error Found: %d,%s\n", err_no, buf);
        err = viScanf (m_ViSession33522B, "%d%t", &err_no, &buf);
    }
    err = viFlush(m_ViSession33522B, VI_READ_BUF);
    if (err < VI_SUCCESS) err_handler (m_ViSession33522B, err);

    err = viFlush(m_ViSession33522B, VI_WRITE_BUF);
    if (err < VI_SUCCESS) err_handler (m_ViSession33522B, err);

}

void CWaveformDriver::err_handler(ViSession vi, ViStatus err)
{
    CString str;
    char err_msg[1024]={0};
    viStatusDesc (vi, err, err_msg);
    str.Format("%s",err_msg);
    MessageBox(NULL, str,"Agilent",MB_OK);
    return;
}

void CWaveformDriver::WaitOperComplete(ViSession oIo)
{
	char strResult[128] = "";
	while(1)
	{
		viQueryf(oIo,"*OPC?\n","%t",&strResult);
		if(strResult)
			break;
	}
}

int CWaveformDriver::exampleARBFuncCh2USBDeviceFile(void)
{
    ViStatus status;
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

    //==================================
    //viWriteFromFile();
    //viWrite();
    status = viPrintf(m_ViSession33522B, "*CLS\n");
    status = viPrintf(m_ViSession33522B, "*RST\n");
    status = viPrintf(m_ViSession33522B, ":SOURce1:DATA:VOLatile:CLEar\n");
    status = viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA1 \"%s\"\n", PATH_PACE_A);
    if (status < VI_SUCCESS)
    {
        err_handler(m_ViSession33522B, status);
    }
    status = viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary \"%s\"\n", PATH_PACE_A);
    if (status < VI_SUCCESS)
    {
        err_handler(m_ViSession33522B, status);
    }
    status = viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "ARB");
    if (status < VI_SUCCESS)
    {
        err_handler(m_ViSession33522B, status);
    }
    status = viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);
    if (status < VI_SUCCESS)
    {
        err_handler(m_ViSession33522B, status);
    }
    //==================================
    
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 0);                                      //关闭输出  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "ARB");
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:HIGH %@3lf\n", 0.002);               //最大输出电压  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:LOW %@3lf\n", 0);                   //最小输出电压  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary:SRATe %@3lf\n", 1000000.0);    //采样率 1MSa/s
    //viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary:FREQuency %@3lf\n", 1000000.0);//频率 1MSa/s
    
    //MMEM:COPY "INT:\MySetup.sta","INT:\Backup"
    viPrintf(m_ViSession33522B, ":MMEMory:COPY \"USB:\\PACE_A.BARB\",\"INT:\\MYARB.BARB\"\n");
    viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA1 \"INT:\\MYARB.BARB\"\n");
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary \"INT:\\MYARB.BARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);                                   //开启输出  
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
        high_V = amp_V / 2;
        low_V =  - amp_V / 2;
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
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");                           //高阻抗
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

int CWaveformDriver::setFuncARB (UINT8 channel, const char *pathName)
{
    if (!m_bIsDeviceOpen) return -1;
    
	//Clear volatile memory
	viPrintf(m_ViSession33522B, "SOURce%@1d:DATA:VOLatile:CLEar\n", channel);
    
    if (viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA%@1d \"%s\"\n",channel,pathName) < VI_SUCCESS)
    {
        TRACE("=================:MMEMory:LOAD error \r\n");
        return -2;
    }
	// wait for the operation to complete before moving on
	viPrintf(m_ViSession33522B, "*WAI\n");

    if (viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:ARBitrary \"%s\"\n", channel, pathName) < VI_SUCCESS)
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
        return -3;
    }
	// wait for the operation to complete before moving on
	viPrintf(m_ViSession33522B, "*WAI\n");
    
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");   //高阻抗
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"ARB");
    //viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");   //高阻抗
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);          //开启输出  
    
    return 0;
}
#if 0

#include <visa.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static void wait(clock_t wait)
{
    clock_t goal;
    goal = wait + clock();
    while( goal > clock() )
        ;
}
static void WaitOperComplete(ViSession oIo)
{
    char strResult[128] = "";
    while(1)
    {
        viQueryf(oIo,"*OPC?\n","%t",&strResult);
        if(strResult)
            break;
    }
}

int CWaveformDriver::setFuncARBByRemoteFile (UINT8 channel, const char *fileName)
{
    char strResult[256] = {0};
    char noErrString[256] = {"+0,\"No error\"\n"};
    char data[10000];
    int i;

    //Clear volatile memory
    viPrintf(m_ViSession33522B,"DATA:VOLatile:CLEar\n");

    // Create arb waveform named "TestArb" with 4000 points of 0-1 data
    printf("Generating Waveform\n\n");
    strcpy(data, "SOURce1:DATA:ARBitrary TestArb");
    for(i = 1; i <= 5; i++)        /* Set rise time (5 points) */
        sprintf(data, "%s,%3.1f", data, (double)(i - 1)/5);

    for(i = 6; i <= 205; i++)
        strcat(data, ",1");     /* Set pulse width (200 points) */

    for(i = 206; i <= 210; i++)    /* Set fall time (5 points) */
        sprintf(data, "%s,%3.1f", data, (double)(210 - i)/5);

    for(i = 211; i <= 4000; i++)
        strcat(data, ",0");     /* Set remaining points to zero */

    strcat(data,"\n");

    //Send Command to set the desired configuration
    printf("Downloading Waveform...\n\n");
    viPrintf(m_ViSession33522B,data);
    WaitOperComplete(m_ViSession33522B);
    printf("Download Complete\n\n");

    //Set desired configuration.
    viPrintf(m_ViSession33522B,"SOURce1:FUNCtion:ARBitrary TestArb\n"); // set current arb waveform to defined arb pulse
    viPrintf(m_ViSession33522B,"SOURce1:FUNCtion ARB\n"); // turn on arb function
    viPrintf(m_ViSession33522B,"SOURCE1:VOLT 2\n"); // set max waveform amplitude to 2 Vpp
    viPrintf(m_ViSession33522B,"SOURCE1:VOLT:OFFSET 0\n"); // set offset to 0 V
    viPrintf(m_ViSession33522B,"OUTPUT1:LOAD 50\n"); // set output load to 50 ohms
    viPrintf(m_ViSession33522B,"SOURCE1:FUNCtion:ARB:SRATe 40000\n"); // set sample rate


    //Enable Output
    viPrintf(m_ViSession33522B,"OUTPUT1 ON\n"); // turn on channel 1 output

    //Verify error output.
    viQueryf(m_ViSession33522B,"SYST:ERR?\n","%t",&strResult);

    if(!(strcmp(strResult,noErrString)))
    {
        printf("Arbitrary wave generated without any error\n");
    }
    else
    {
        printf("Error reported: %s",strResult);
    }
    return 0;
}

#else

int CWaveformDriver::myWriteIEEEBlock(const char *head, const INT16 *pDACVal, const UINT32 nDataCount)
{
    UINT32 i = 0;
    UINT32 newLen = 0;
    UINT32 dataLenOfBytes = nDataCount*sizeof(INT16);
    UINT32 offset = 0;
	char *pDataFrame = NULL;
    char *pBinValStart = NULL;
    char pBuf[32];
    
    newLen = dataLenOfBytes + 100;
    pDataFrame = new char[newLen];
    if (NULL == pDataFrame)
    {
        return -1;
    }
    
    //NORMal：每个数据点的最高有效字节 (MSB) 是第一个字节。 
    //SWAPped：每个数据点的最低有效字节 (LSB) 是第一个字节。大多数计算机使用此字节顺序。
	viPrintf(m_ViSession33522B, "FORMat:BORDer %s\n", "SWAPped"); //NORMal|SWAPped,default NORMal

    memset(pDataFrame, 0x00, newLen);
    strcat(pDataFrame, head);

    memset(pBuf, 0x00, sizeof(pBuf));
    sprintf(pBuf, "%d", dataLenOfBytes);
    i =0;
    while(0x00 != pBuf[i++]);
    sprintf(pBuf, ",#%01d%d", i-1, dataLenOfBytes);
    strcat(pDataFrame,pBuf);
#if 0
    offset = strlen(pDataFrame);
    for (i=0; i < (offset+10); i++)TRACE("%d:0x%2x\r\n",i,pDataFrame[i]); //test

    pBinValStart = &pDataFrame[offset];
    
    memcpy(pBinValStart, (char *)pDACVal, dataLenOfBytes);
    for (i=0; i < (offset+10); i++)TRACE("%d:0x%2x\r\n",i,pDataFrame[i]);
    offset += dataLenOfBytes;
    memcpy(&pDataFrame[offset], "\n", sizeof("\n"));
    //Send Command to set the desired configuration
    printf("Downloading Waveform...\n\n");

    viPrintf(m_ViSession33522B,pDataFrame);
#else
    //viPrintf(m_ViSession33522B, "%s, %*hb\n",head, nDataCount, pDACVal);
    viPrintf(m_ViSession33522B, "%s%*hb\n",pDataFrame, nDataCount, pDACVal);
    //status = visa32.viPrintf(vi, "DATA:ARB:DAC myWave, %*hb" & vbCrLf, arrData.Length(), arrData)
#endif
    delete pDataFrame;
    pDataFrame = NULL;
    return 0;
}

int CWaveformDriver::myWriteARBitraryDAC(const char *head, const INT16 *pDACVal, const UINT32 nDataCount)
{
    UINT32 i;
    UINT32 newLen = 0;
	char *pDataFrame = NULL;
    char pBuf[10];

    if (nDataCount > 65536)
    {
        return -1;
    }

    newLen = nDataCount*10+100;
    pDataFrame = new char[newLen];
    if (NULL == pDataFrame)
    {
        return -1;
    }

    memset(pDataFrame, 0x00, newLen);
    strcat(pDataFrame, head);
    for (i = 0; i < nDataCount; i++)
    {
        memset(pBuf, 0x00, sizeof(pBuf));
        sprintf(pBuf, ",%d", pDACVal[i]);
        strcat(pDataFrame, pBuf);
    }
    strcat(pDataFrame,"\n");
    //Send Command to set the desired configuration
    printf("Downloading Waveform...\n\n");
    viPrintf(m_ViSession33522B,pDataFrame);
    delete pDataFrame;
    pDataFrame = NULL;
    return 0;
}

int CWaveformDriver::setFuncARBByRemoteFile (UINT8 channel, const char *fileName)
{
    CStdioFile file;
    CString str;
    UINT32 i, u32DataPoints;
    char DataFrameHead[100];
    char oneLineBuf[200];
    int channel_cnt = 0;
    int sample_rate = 0;
    float high_level = 0.0;
    float low_level = 0.0;
    INT16 data = 0;
    INT16 *pDataArray = NULL;

    if (!m_bIsDeviceOpen) return -1;
    
    if (FALSE == PathFileExists(fileName))
    {
        MSG("请选择正确的文件路径\r\n");
		return -1;
    }

    file.Open(fileName, CFile::modeRead);
    file.SeekToBegin();//移到文件头
    
	viPrintf(m_ViSession33522B, "SOURce%@1d:DATA:VOLatile:CLEar\n", channel);
    
    if(!file.ReadString(str))   //Line001: File Format:1.10
    {
        file.Close();
		return -1;
    }
    if(!file.ReadString(str))   //Line002: Channel Count:1
    {
        file.Close();
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        memset(oneLineBuf, 0x00, sizeof(oneLineBuf));
        strcpy(oneLineBuf, str);
        channel_cnt = atoi(oneLineBuf);
    }

    if(!file.ReadString(str))   //Line003: Sample Rate:1000000
    {
        file.Close();
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        memset(oneLineBuf, 0x00, sizeof(oneLineBuf));
        strcpy(oneLineBuf, str);
        sample_rate = atoi(oneLineBuf);
    }

    if(!file.ReadString(str))   //Line004: High Level:0.002
    {
        file.Close();
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        memset(oneLineBuf, 0x00, sizeof(oneLineBuf));
        strcpy(oneLineBuf, str);
        high_level = (float)atof(oneLineBuf);
    }

    if(!file.ReadString(str))   //Line005: Low Level:0
    {
        file.Close();
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        memset(oneLineBuf, 0x00, sizeof(oneLineBuf));
        strcpy(oneLineBuf, str);
        low_level = (float)atof(oneLineBuf);
    }

    if(!file.ReadString(str))   //Line006: Filter:"NORMAL"
    {
        file.Close();
        return -1;
    }

    if(!file.ReadString(str))   //Line007: Data Points:1000000
    {
        file.Close();
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        memset(oneLineBuf, 0x00, sizeof(oneLineBuf));
        strcpy(oneLineBuf, str);
        u32DataPoints = (UINT32)atoi(oneLineBuf);
    }
    if(!file.ReadString(str))   //Line008: Data:
    {
        file.Close();
        return -1;
    }
    
	// Create arb waveform named "TestArb" 
	printf("Downloading Waveform...\n\n");
    
    memset(DataFrameHead, 0x00, sizeof(DataFrameHead));
	sprintf(DataFrameHead, ":SOURce%01d:DATA:ARBitrary:DAC TestArb", channel);

    pDataArray = new INT16[u32DataPoints];
    if (NULL == pDataArray)
    {
        file.Close();
        return -1;
    }
    memset(pDataArray, 0x00,sizeof(INT16)*u32DataPoints);
    for (i = 0; i < u32DataPoints; i++)
    {
            if(file.ReadString(str))   //Line009: Data[0],当文件没有读完时，返回TRUE，读到文件尾，返回FALSE
            {
                pDataArray[i] = (INT16)atoi(str);
            }
            else
            {
                break;
            }
    }

    //if (u32DataPoints > 65536)
    {
        myWriteIEEEBlock(DataFrameHead, pDataArray, u32DataPoints);
    }
    //else
    {
    //    myWriteARBitraryDAC(DataFrameHead, pDataArray, u32DataPoints);
    }
    delete pDataArray;
    pDataArray = NULL;
    viPrintf(m_ViSession33522B,"*WAI\n");//Make sure no other commands are exectued until arb is done downloading
	WaitOperComplete(m_ViSession33522B);
	printf("Download Complete\n\n");
    //关闭文件
    file.Close();
    
    //Set desired configuration.
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");   //高阻抗
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:ARBitrary TestArb\n", channel);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:ARBitrary:FILTer NORM\n", channel); //default
	//WaitOperComplete(m_ViSession33522B);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"ARB");// turn on arb function
    viPrintf(m_ViSession33522B, ":SOURCE%@1d:VOLTage:HIGH %@3lf\n", channel,high_level);
    viPrintf(m_ViSession33522B, ":SOURCE%@1d:VOLTage:LOW %@3lf\n", channel, low_level);
    viPrintf(m_ViSession33522B, ":SOURCE%@1d:FUNCtion:ARB:SRATe %@1d\n", channel,sample_rate); // set sample rate
    //Enable Output
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);          //开启输出  

    memset(oneLineBuf, 0x00, sizeof(oneLineBuf));
    //Verify error output.
    viQueryf(m_ViSession33522B,"SYST:ERR?\n","%t",&oneLineBuf);

    if(!(strcmp(oneLineBuf,noErrString)))
    {
        printf("Arbitrary wave generated without any error\n");
    }
    else
    {
        printf("Error reported: %s",oneLineBuf);
    }

    return 0;
}
#endif
int CWaveformDriver::setPaceByEnum(int type)
{
    int ret = -1;
    switch (type)
    {
    case PACE_A:
        ret = gpWaveformDev->setFuncARB(1,PATH_PACE_A);
        break;
    case PACE_B:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_B);
        break;
    case PACE_C:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_C);
        break;
    case PACE_D:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_D);
        break;
    case PACE_E:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_E);
        break;
    case PACE_F:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_F);
        break;
    case PACE_G:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_G);
        break;
    case PACE_H:
        ret = gpWaveformDev->setFuncARB(1, PATH_PACE_H);
        break;
    default:
        break;
    }
    return ret;
}

