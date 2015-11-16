
/******************************************************************************
*
*                ��Ȩ���� (C), 2001-2015, ������Դ���ӿƼ��ɷ����޹�˾
*
*******************************************************************************
* �� �� ��   : WaveformDriver.cpp
* �� �� ��   : V1.0.0
* ��������   : 2015��11��12��
* ����޸�   : 2015��11��12�� 20:19:30
* ��������   : �����źŷ�������API
*
* �޸���ʷ   :
* 1.��    ��   : 2015��11��12�� 20:20:06
*   ��    ��   : QiuWeibo
*   �޸�����   : �׷�
*
******************************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "AioTestFixture.h"
#include "WaveformDriver.h"

#pragma comment(lib, "visa32.lib")


CWaveformDriver *gpWaveformDev = NULL;


/******************************************************************************
* �������ƣ�CWaveformDriver
* ��    �ܣ����캯��
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��12��
******************************************************************************/
CWaveformDriver::CWaveformDriver(void)
    :m_bIsDeviceOpen(false)
{
}


/******************************************************************************
* �������ƣ�~CWaveformDriver
* ��    �ܣ��鹹����
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��12��
******************************************************************************/
CWaveformDriver::~CWaveformDriver(void)
{
    closeDevice();
}


/******************************************************************************
* �������ƣ�initApplication
* ��    �ܣ���ʼ������
* ��    ����void
* �� �� ֵ��int, <0 -- error    >=0 success
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��12��
******************************************************************************/
int CWaveformDriver::initApplication(void)
{
    return 0;
}


/******************************************************************************
* �������ƣ�checkIsDeviceOpen
* ��    �ܣ��ж��豸�Ƿ��
* ��    ����void
* �� �� ֵ��bool, true -- �豸�Ѵ� false -- �豸��δ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��12��
******************************************************************************/
bool CWaveformDriver::checkIsDeviceOpen(void)
{
    return m_bIsDeviceOpen;
}



/******************************************************************************
* �������ƣ�BinaryArb
* ��    �ܣ�һ��ʾ����1.����ͨ�Ź��ܣ�2.�Լ��������ݹ���
* ��    ����void
* �� �� ֵ��void
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��12��
******************************************************************************/
void CWaveformDriver::BinaryArb(void)
{
    const char noErrString[] = "+0,\"No error\"\n";

    const int NUM_DATA_POINTS = 100000;
    float *z = new float[NUM_DATA_POINTS];

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

    delete[] z;

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

/******************************************************************************
* �������ƣ�openDevice
* ��    �ܣ����źŷ����������Ҹ�λ����
* ��    ����const char *, ����VASI��ʽ���豸���ƣ�����
*           1��USB�豸��"USB0::0x0957::0x2C07::MY52802529::0::INSTR"
*           2�������豸��"TCPIP0::156.140.113.206::inst0::INSTR"
* �� �� ֵ��int,ʧ�ܷ��ظ�ֵ���ɹ�����ֵ0
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::openDevice(const char *pUsbDevName)
{
    ViUInt16 intfType;  
    ViString intfName[512]; 
    ViBoolean viFlag = VI_FALSE;  
    ViStatus ret = -1;
    ViAddr     myUserHandle = 0;

    //���ܵ���Դ����������ʼ����Դ������  
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
  
    //��ָ����USB�ӿڿ��Ƶĺ���������
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
        AfxMessageBox("�޷��������ӣ���ѡ����ȷ��USB�ڣ�");
        viClose(m_ViSessionRM);
        return -2;
    }
  
    //ȷ��Ĭ�ϵĺ����������������\n���������ﶨ���SCPI�����Ǳ�����\n��β��  
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

    rset2Default(); //reset

    return 0;
}


/******************************************************************************
* �������ƣ�closeDevice
* ��    �ܣ��ر��豸
* ��    ����void
* �� �� ֵ��int,0 -- �����ɹ���<0 -- ����ʧ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::closeDevice(void)
{
    if (m_bIsDeviceOpen)
    {
        //�رյ�ָ����USB�ӿڿ��Ƶĺ���������������  
        viClose(m_ViSession33522B);  
        //�ر��ܵ���Դ������  
        viClose(m_ViSessionRM);
        m_bIsDeviceOpen = false;
    }
    return 0;
}


/******************************************************************************
* �������ƣ�rset2Default
* ��    �ܣ���λ�豸
* ��    ����void
* �� �� ֵ��int,0 -- �����ɹ���<0 -- ����ʧ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::rset2Default(void)
{
    if (!m_bIsDeviceOpen) return -1;
	viPrintf(m_ViSession33522B, "*CLS;*RST\n");
    WaitOperComplete(m_ViSession33522B);
    clearDISPlay();
    return 0;
}

/******************************************************************************
* �������ƣ�getDeviceIDN
* ��    �ܣ���ȡ�豸IDN��Ϣ
* ��    ����void    ,
* �� �� ֵ��int     ,0 -- �����ɹ���<0 -- ����ʧ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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


/******************************************************************************
* �������ƣ�testSampleCh1
* ��    �ܣ���������1��ʵ��ͨ��1�������
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::testSampleCh1(void)
{
    if (!m_bIsDeviceOpen) return -1;

    //��������������䣬ע��SCPI��д����\n��β  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "SQUare");                     //����  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:HIGH %@3lf\n", 5.0);             //��������ѹ  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:LOW %@3lf\n", -5.0);             //��С�����ѹ  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:STATe %@1d\n", 1);                 
    viPrintf(m_ViSession33522B, ":SOURce1:FREQuency %@3lf\n", 100.0);                    //Ƶ��(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage %@3lf\n", 4.0);                        //��ֵ(V)  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:OFFSet %@3lf\n", 1.0);                 //ƫ��ֵ(V)  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:SQUare:DCYCle %@3lf\n", 20.0);        //ռ�ձ�(%)  
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);                                   //�������  

    return 0;
}

/******************************************************************************
* �������ƣ�testSampleCh2
* ��    �ܣ��������ӣ�ʹͨ��2�������
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::testSampleCh2(void)
{
    if (!m_bIsDeviceOpen) return -1;
    //��������������䣬ע��SCPI��д����\n��β  
    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion %s\n", "SQUare");                     //����  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:HIGH %@3lf\n", 5.0);             //��������ѹ  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:LOW %@3lf\n", -5.0);             //��С�����ѹ  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:STATe %@1d\n", 1);                 
    viPrintf(m_ViSession33522B, ":SOURce2:FREQuency %@3lf\n", 500.0);                    //Ƶ��(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage %@3lf\n", 4.0);                        //��ֵ(V)  
    viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:OFFSet %@3lf\n", 1.0);                 //ƫ��ֵ(V)  
    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:SQUare:DCYCle %@3lf\n", 20.0);        //ռ�ձ�(%)  
    viPrintf(m_ViSession33522B, ":OUTPut2 %@1d\n", 1);                                   //�������  

    return 0;
}

/******************************************************************************
* �������ƣ�exampleARBFuncCh1InternalFile
* ��    �ܣ�
*           ����Ĵ���ɼ��غ��޸��������Ⲩ�Ρ�
*           FUNCtion ARB
*           VOLTage +3
*           VOLTage:OFFSet +1
*           FUNC:ARB:SRAT 1E5
*           FUNCtion:ARBitrary "INT:\BUILTIN\EXP_RISE.ARB"
*           OUTPut 1
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::exampleARBFuncCh1InternalFile(void)
{
    if (!m_bIsDeviceOpen) return -1;

    //��������������䣬ע��SCPI��д����\n��β  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "ARB");
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage %@3lf\n", 3.0);
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:OFFSet %@3lf\n", 1.0);
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary:SRATe %@3lf\n", 100000.0); //������ 100KSa/s
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary \"INT:\\BUILTIN\\EXP_RISE.ARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);                                   //�������  

    return 0;
}

/******************************************************************************
* �������ƣ�system_err
* ��    �ܣ��ж��źŷ������Ƿ�������
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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


/******************************************************************************
* �������ƣ�err_handler
* ��    �ܣ�����error number ���������Ϣ
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
void CWaveformDriver::err_handler(ViSession vi, ViStatus err)
{
    //CString str;
    char err_msg[1024]={0};
    viStatusDesc (vi, err, err_msg);
    //str.Format("%s",err_msg);
    MessageBox(NULL, err_msg,"Agilent",MB_OK);
    return;
}

/******************************************************************************
* �������ƣ�WaitOperComplete
* ��    �ܣ��ȴ��������
* ��    ����ViSession ,VI����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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

/******************************************************************************
* �������ƣ�exampleARBFuncCh2USBDeviceFile
* ��    �ܣ�һ�������źŷ���������USB�豸�Ĳ����ļ�
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::exampleARBFuncCh2USBDeviceFile(void)
{
    ViStatus status;
    if (!m_bIsDeviceOpen) return -1;

    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion %s\n", "ARB");
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:HIGH %@3lf\n", 0.88);             //��������ѹ  
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:LOW %@3lf\n", -0.13);             //��С�����ѹ  
    //viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary:SRATe %@3lf\n", 10000.0);    //������ 10KSa/s
    
    viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA2 \"USB:\\C01_001.BARB\"\n");
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary \"USB:\\C01_001.BARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut2 %@1d\n", 1);                                   //�������  
    
    viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion %s\n", "ARB");
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:HIGH %@3lf\n", 0.88);             //��������ѹ  
    //viPrintf(m_ViSession33522B, ":SOURce2:VOLTage:LIMit:LOW %@3lf\n", -0.13);             //��С�����ѹ  
    //viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary:SRATe %@3lf\n", 10000.0);    //������ 10KSa/s
    
    viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA2 \"USB:\\C01_002.BARB\"\n");
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce2:FUNCtion:ARBitrary \"USB:\\C01_002.BARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut2 %@1d\n", 1);                                   //�������  

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
    
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 0);                                      //�ر����  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion %s\n", "ARB");
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:HIGH %@3lf\n", 0.002);               //��������ѹ  
    viPrintf(m_ViSession33522B, ":SOURce1:VOLTage:LIMit:LOW %@3lf\n", 0);                   //��С�����ѹ  
    viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary:SRATe %@3lf\n", 1000000.0);    //������ 1MSa/s
    //viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary:FREQuency %@3lf\n", 1000000.0);//Ƶ�� 1MSa/s
    
    //MMEM:COPY "INT:\MySetup.sta","INT:\Backup"
    viPrintf(m_ViSession33522B, ":MMEMory:COPY \"USB:\\PACE_A.BARB\",\"INT:\\MYARB.BARB\"\n");
    viPrintf(m_ViSession33522B, ":MMEMory:LOAD:DATA1 \"INT:\\MYARB.BARB\"\n");
    if (VI_SUCCESS != viPrintf(m_ViSession33522B, ":SOURce1:FUNCtion:ARBitrary \"INT:\\MYARB.BARB\"\n"))
    {
        TRACE("=================FUNCtion:ARBitrary error \r\n");
    }
    viPrintf(m_ViSession33522B, ":OUTPut1 %@1d\n", 1);                                   //�������  
    return 0;
}


/******************************************************************************
* �������ƣ�setFuncSin
* ��    �ܣ��������ָ�����Ҳ�
* ��    ����
*           UINT8 channel   ,���룬ָ��������ͨ��
*           float freq_hz   ,���룬Ƶ��ֵ����λHz
*           float amp_V     ,���룬����ֵ����λV
*           float high_V    ,���룬�ߵ�ѹ��ֵ����λV
*           float low_V     ,���룬�͵�ѹ��ֵ����λV
*           float offset_V  ,���룬ƫ�Ƶ�ѹֵ����λV
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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
    //��������������䣬ע��SCPI��д����\n��β  
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");                           //���迹
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"SINusoid");      
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FREQuency %@3lf\n", channel,freq_hz);                  //Ƶ��(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage %@3lf\n", channel,amp_V);                      //��ֵ(V)  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:LIMit:HIGH %@3lf\n",channel,high_V);           //��������ѹ  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:LIMit:LOW %@3lf\n", channel,low_V);            //��С�����ѹ  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:LIMit:STATe %@1d\n", channel,0);               //���û������������ѹ����,Ĭ��OFF(0)        
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:OFFSet %@3lf\n", channel,offset_V);            //ƫ��ֵ(V)
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);                                  //�������  
    return 0;
}

/******************************************************************************
* �������ƣ�setFuncPULSe
* ��    �ܣ��������ָ�����岨
* ��    ����
*           UINT8 channel   ,���룬ָ��������ͨ��
*           float freq_hz   ,���룬Ƶ��ֵ����λHz
*           float volt_V    ,���룬����ֵ����λV
*           float lead_s    ,���룬������ʱ�䣬��λs
*           float tra_s     ,���룬�½���ʱ�䣬��λs
*           float widt_s    ,���룬������ʱ�䣬��λs
*           float offset_V  ,���룬ƫ�Ƶ�ѹֵ����λV
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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

    //��������������䣬ע��SCPI��д����\n��β  
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");                           //���迹
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"PULSe");      
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FREQuency %@3lf\n", channel,freq_hz);                  //Ƶ��(kHz)  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage %@3lf\n", channel,volt_V);                     //��ֵ(V)        
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:OFFSet %@3lf\n", channel,offset_V);            //ƫ��ֵ(V)
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:PULSe:WIDTh %@3lf\n", channel,widt_s);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:PULSe:TRANsition:LEADing %@3lf\n",channel,lead_s);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:PULSe:TRANsition:TRAiling %@3lf\n", channel,tra_s);     
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);                                           //�������  
    return 0;
}

/******************************************************************************
* �������ƣ�setFuncARB
* ��    �ܣ����Ⲩ�μ��ؽӿڡ������ļ������ڲ��洢��Ҳ������USB�洢
* ��    ����
*           UINT8 channel           ,���룬ָ�����ͨ��
*           const char *pathName    �����룬�����ļ�·��
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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
    
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");   //���迹
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"ARB");
    //viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");   //���迹
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);          //�������  
    
    return 0;
}

/******************************************************************************
* �������ƣ�myWriteIEEEBlock
* ��    �ܣ�ͨ��IEEE��ģʽ��������
* ��    ����
*           const char *head        ,�������,������ͷ
*           const INT16 *pDACVal    ,�������,���ݻ�����
*           const UINT32 nDataCount ,�������,�����ܸ���
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::myWriteIEEEBlock(const char *head, const INT16 *pDACVal, const UINT32 nDataCount)
{
    UINT32 i = 0;
    UINT32 newLen = 0;
    UINT32 dataLenOfBytes = nDataCount*sizeof(INT16);
	char *pDataFrame = NULL;
    char pBuf[32];
    
    newLen = dataLenOfBytes + 100;
    pDataFrame = new char[newLen];
    if (NULL == pDataFrame)
    {
        return -1;
    }
    
    //NORMal��ÿ�����ݵ�������Ч�ֽ� (MSB) �ǵ�һ���ֽڡ� 
    //SWAPped��ÿ�����ݵ�������Ч�ֽ� (LSB) �ǵ�һ���ֽڡ�����������ʹ�ô��ֽ�˳��
	viPrintf(m_ViSession33522B, "FORMat:BORDer %s\n", "SWAPped"); //NORMal|SWAPped,default NORMal

    memset(pDataFrame, 0x00, newLen);
    strcat(pDataFrame, head);

    memset(pBuf, 0x00, sizeof(pBuf));
    sprintf(pBuf, "%d", dataLenOfBytes);
    i =0;
    while(0x00 != pBuf[i++]);
    sprintf(pBuf, ",#%01d%d", i-1, dataLenOfBytes);
    strcat(pDataFrame,pBuf);
    viPrintf(m_ViSession33522B, "%s%*hb\n",pDataFrame, nDataCount, pDACVal);
    delete[] pDataFrame;
    pDataFrame = NULL;
    return 0;
}

/******************************************************************************
* �������ƣ�myWriteARBitraryDAC
* ��    �ܣ�ͨ��ACSIIģʽ��������
* ��    ����
*           const char *head        ,�������,������ͷ
*           const INT16 *pDACVal    ,�������,���ݻ�����
*           const UINT32 nDataCount ,�������,�����ܸ���
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
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
    delete[] pDataFrame;
    pDataFrame = NULL;
    return 0;
}

/******************************************************************************
* �������ƣ�setFuncARBByRemoteFile
* ��    �ܣ���PC�˶�ȡ�ļ��������Ҽ��ص������źŷ�����
* ��    ����
*           UINT8 channel       ,���������ָ��������ͨ��
*           const char *fileName,���������ָ��PC�˵��ļ�·��������
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::setFuncARBByRemoteFile (UINT8 channel, const char *fileName)
{
    CStdioFile file;
    CString str;
    UINT32 i, u32DataPoints;
    char DataFrameHead[100];
    int channel_cnt = 0;
    int sample_rate = 0;
    float high_level = 0.0;
    float low_level = 0.0;
    INT16 data = 0;
    INT16 *pDataArray = NULL;

    if ((!m_bIsDeviceOpen) || (FALSE == PathFileExists(fileName)))
    {
        MSG("��ѡ����ȷ���ļ�·��\r\n");
		return -1;
    }

    file.Open(fileName, CFile::modeRead);
    file.SeekToBegin();         //�Ƶ��ļ�ͷ

    //���ָ��ͨ���Ĳ��δ洢��Ȼ�����¼���Ĭ�ϲ���
	viPrintf(m_ViSession33522B, "SOURce%@1d:DATA:VOLatile:CLEar\n", channel);
    
    if(!file.ReadString(str))   //Line001: File Format:1.10
    {
        file.Close();
        MSG("��001:��ȡ�ļ��汾ʧ��!!!\r\n");
		return -1;
    }
    if(!file.ReadString(str))   //Line002: Channel Count:1
    {
        file.Close();
        MSG("��002:��ȡͨ������ʧ��!!!\r\n");
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        channel_cnt = atoi((LPSTR)(LPCTSTR)str);
    }

    if(!file.ReadString(str))   //Line003: Sample Rate:1000000
    {
        file.Close();
        MSG("��003:��ȡ������ʧ��!!!\r\n");
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        sample_rate = atoi((LPSTR)(LPCTSTR)str);
    }

    if(!file.ReadString(str))   //Line004: High Level:0.002
    {
        file.Close();
        MSG("��004:��ȡ�ߵ�ƽʧ��!!!\r\n");
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        high_level = (float)atof((LPSTR)(LPCTSTR)str);
    }

    if(!file.ReadString(str))   //Line005: Low Level:0
    {
        file.Close();
        MSG("��005:��ȡ�͵�ƽʧ��!!!\r\n");
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        low_level = (float)atof((LPSTR)(LPCTSTR)str);
    }

    if(!file.ReadString(str))   //Line006: Filter:"NORMAL"
    {
        file.Close();
        MSG("��006:��ȡ�˲���ģʽʧ��!!!\r\n");
        return -1;
    }

    if(!file.ReadString(str))   //Line007: Data Points:1000000
    {
        file.Close();
        MSG("��007:��ȡ���ݲ��������ʧ��!!!\r\n");
        return -1;
    }
    else
    {
        str = str.Mid(str.Find(':')+1);
        u32DataPoints = (UINT32)atoi((LPSTR)(LPCTSTR)str);
    }
    if(!file.ReadString(str))   //Line008: Data:
    {
        file.Close();
        MSG("��008:��ȡ���ݿ�ʼ��עʧ��!!!\r\n");
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
        MSG("��������ڴ�ʧ��!!!\r\n");
        return -1;
    }
    memset(pDataArray, 0x00,sizeof(INT16)*u32DataPoints);
    for (i = 0; i < u32DataPoints; i++)
    {
            if(file.ReadString(str))   //Line009: Data[0],���ļ�û�ж���ʱ������TRUE�������ļ�β������FALSE
            {
                pDataArray[i] = (INT16)atoi((LPSTR)(LPCTSTR)str);
            }
            else
            {
                break;
            }
    }

    //if (u32DataPoints > 65536)
    //{
        myWriteIEEEBlock(DataFrameHead, pDataArray, u32DataPoints);
    //}
    //else
    //{
    //    myWriteARBitraryDAC(DataFrameHead, pDataArray, u32DataPoints);
    //}
    delete[] pDataArray;
    pDataArray = NULL;

    viPrintf(m_ViSession33522B,"*WAI\n");//Make sure no other commands are exectued until arb is done downloading
	WaitOperComplete(m_ViSession33522B);
	printf("Download Complete\n\n");
    //�ر��ļ�
    file.Close();
    
    //Set desired configuration.
    viPrintf(m_ViSession33522B, ":OUTPut%@1d:LOAD %s\n", channel, "INF");   //���迹
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:ARBitrary TestArb\n", channel);
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion:ARBitrary:FILTer NORM\n", channel); //default
    viPrintf(m_ViSession33522B, ":SOURce%@1d:FUNCtion %s\n", channel,"ARB");// turn on arb function
    viPrintf(m_ViSession33522B, ":SOURCE%@1d:VOLTage:HIGH %@3lf\n", channel,high_level);
    viPrintf(m_ViSession33522B, ":SOURCE%@1d:VOLTage:LOW %@3lf\n", channel, low_level);  
    viPrintf(m_ViSession33522B, ":SOURce%@1d:VOLTage:OFFSet %@3lf\n", channel,0.0);            //ƫ��ֵ(V)
    viPrintf(m_ViSession33522B, ":SOURCE%@1d:FUNCtion:ARB:SRATe %@1d\n", channel,sample_rate); //sample_rate
    viPrintf(m_ViSession33522B, ":OUTPut%@1d %@1d\n", channel, 1);          //�������  

    if (IsErrorOutput())
    {
        ERROR_INFO("setFuncARBByRemoteFile �����д���!!!\r\n");
        return -1;
    }
    INFO("setFuncARBByRemoteFile ���óɹ�!!!\r\n");
    return 0;
}

/******************************************************************************
* �������ƣ�IsErrorOutput
* ��    �ܣ��ж��Ƿ����ʧ��
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
bool CWaveformDriver::IsErrorOutput(void)
{
    bool flag = false;
    char oneLineBuf[256];
    const char noErrString[32] = {"+0,\"No error\"\n"};

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
        flag = true;
    }
    return flag;
}


/******************************************************************************
* �������ƣ�setPaceByEnum
* ��    �ܣ����ָ����PACE�ź�
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::setPaceByEnum(int type)
{
    int ret = -1;
    switch (type)
    {
    case PACE_A:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_A.arb");
        //ret = gpWaveformDev->setFuncARB(1,PATH_PACE_A);
        break;
    case PACE_B:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_B.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_B);
        break;
    case PACE_C:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_C.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_C);
        break;
    case PACE_D:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_D.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_D);
        break;
    case PACE_E:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_E.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_E);
        break;
    case PACE_F:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_F.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_F);
        break;
    case PACE_G:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_G.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_G);
        break;
    case PACE_H:
        ret = gpWaveformDev->setFuncARBByRemoteFile(1, "AIOTEST/PACE_H.arb");
        //ret = gpWaveformDev->setFuncARB(1, PATH_PACE_H);
        break;
    default:
        break;
    }
    return ret;
}


/******************************************************************************
* �������ƣ�setDISPlay
* ��    �ܣ������źŷ�����Ļ��ʾ����
* ��    ����const char *pText����Ҫ��ʾ�����ݣ����32�ֽ�
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::setDISPlay(const char *pText)
{
#if 0
    if (!m_bIsDeviceOpen) return -1;
	viPrintf(m_ViSession33522B, "DISPlay OFF\n");
	viPrintf(m_ViSession33522B, "DISPlay:TEXT \"%s\"\n",pText);
#endif
    return 0;
}

/******************************************************************************
* �������ƣ�clearDISPlay
* ��    �ܣ���������źŷ�������Ļ����
* ��    ����
* �� �� ֵ��
* ��    �ߣ�QiuWeibo
* �������䣺qiuweibo@cvte.com
* ��    �ڣ�2015��11��13��
******************************************************************************/
int CWaveformDriver::clearDISPlay(void)
{
    if (!m_bIsDeviceOpen) return -1;
	viPrintf(m_ViSession33522B, "DISPlay ON\n");
	viPrintf(m_ViSession33522B, "DISPlay:TEXT:CLEar\n");
    return 0;
}
