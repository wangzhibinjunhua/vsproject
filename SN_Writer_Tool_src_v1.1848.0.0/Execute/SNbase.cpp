#include "StdAfx.h"
#include "Common.h"
#include "SNbase.h"
#include "SN Writer.h"
#include "SN WriterDlg.h"

#include <shlwapi.h>

CSNWriterDlg *g_pMainDlg;

////////////////////////////////////////////////////

const unsigned int PortEnumHelper::ms_iFilterLen = 1024;
const unsigned int PortEnumHelper::ms_iFilterNum = PortEnumHelper::ms_iFilterLen/20u;
const unsigned int PortEnumHelper::ms_iPortNum = 20;

PortEnumHelper::PortEnumHelper()
    : m_szFilter(NULL)
    , m_aiFlag(NULL)
    , m_astPort(NULL)
{
    memset(&m_stFiler, 0, sizeof(SP_COM_FILTER_LIST_S));
}
PortEnumHelper::~PortEnumHelper()
{
    int i = 0;

    delete [] m_szFilter;
    delete [] m_aiFlag;
    delete [] m_stFiler.m_ppFilterID;
    delete [] m_astPort;
}

void PortEnumHelper::SetFilter(SP_FILTER_TYPE_E type, const char * filter, int flag /*= 0*/, bool bAppend /*= false*/)
{
    char * szFilter = NULL;
    size_t nLen = 0u, nPos = 0u;

    if (m_szFilter == NULL)
        m_szFilter = new char[ms_iFilterLen];
    if (m_stFiler.m_ppFilterID == NULL)
    {
        m_stFiler.m_ppFilterID = new char * [ms_iFilterNum];
        m_aiFlag = new int [ms_iFilterNum];
    }

    if (filter == NULL || filter[0] == '\0')
        return;

    if (!bAppend)
    {
        memset(m_szFilter, 0, ms_iFilterLen);
        memset(m_stFiler.m_ppFilterID, 0, sizeof(char *)*ms_iFilterNum);
        m_stFiler.m_uCount = 0;
        szFilter = m_szFilter;
    }
    else if (m_stFiler.m_uCount == 0)
        szFilter = m_szFilter;
    else
    {
        nLen = strlen(m_stFiler.m_ppFilterID[m_stFiler.m_uCount - 1]);
        szFilter = m_stFiler.m_ppFilterID[m_stFiler.m_uCount - 1] + nLen + 1;
    }

    strcpy_s(szFilter, ms_iFilterLen - (szFilter - m_szFilter), filter);
    m_stFiler.m_eType = type;

    nLen = strlen(szFilter);
    while (nPos < nLen)
    {
        nPos += strspn(szFilter+nPos, " \t,;");
        if (nPos >= nLen)
            break;
        m_stFiler.m_ppFilterID[m_stFiler.m_uCount] = szFilter+nPos;
        m_aiFlag[m_stFiler.m_uCount] = flag;
        m_stFiler.m_uCount ++;

        nPos += strcspn(szFilter+nPos, " \t,;");
        if (nPos >= nLen)
            break;
        szFilter[nPos++] = '\0';
    }
}

SP_COM_FILTER_LIST_S * PortEnumHelper::GetFilter()
{
    return &m_stFiler;
}

int PortEnumHelper::GetFlag(int index)
{
    if (index >= (int)m_stFiler.m_uCount)
        return -1;
    return m_aiFlag[index];
}

SP_COM_PROPERTY_S * PortEnumHelper::GetPorts(bool bClear /*= false*/)
{
    if (m_astPort == NULL)
    {
        m_astPort = new SP_COM_PROPERTY_S[ms_iPortNum];
        memset(m_astPort, 0, sizeof(SP_COM_PROPERTY_S)*ms_iPortNum);
    }
    else if (bClear)
        memset(m_astPort, 0, sizeof(SP_COM_PROPERTY_S)*ms_iPortNum);

    return m_astPort;
}

////////////////////////////////////////////////////

SNBase::SNBase()
{
    memset(&m_tMetaReq_Ex, 0, sizeof(m_tMetaReq_Ex));
    memset(&m_tMetaConnReport_Ex, 0, sizeof(m_tMetaConnReport_Ex));

    m_hThread = NULL;
    m_iThreadID = 0;
    m_hMauiMetaHandle = 0;
    m_hSPMetaHandle = 0;
    m_sNVRAM_OPID = 0;
    g_iMetaStopFlag = 0;
    m_pMetaStopFlag = &g_iMetaStopFlag;
    m_bWriteNvram = false;
    m_bWriteModemNvram = false;
    m_bTargetInMetaMode = false;
    m_bNeedBackupIMEI = false;
    m_hReadFromNVRAMEvent = NULL;
    m_hWriteToNVRAMEvent = NULL;

    memset(m_strLogDir_Sub, 0, MAX_PATH);

    memset(m_sIMEISV, 0, sizeof(IMEISV_struct_T)*4);
    memset(&m_sScanData, 0, sizeof(ScanData_struct));
}

SNBase::~SNBase()
{
    if (g_sMetaComm.bSwithTool)
    {
        DisableUSBSwitch();
    }
}

UINT SNBase::ThreadStaticEntryPoint(LPVOID pThis)
{
    SNBase * pthX = (SNBase*)pThis;   // the tricky cast
    pthX->ThreadMainEntryPoint();           // now call the true entry-point-function

    pthX->EnableStartBTN(true);
    return 1;
}

//add by wzb 20190320
void SNBase::SetStatusProgress()
{
    
    //(&(g_pMainDlg->m_ProgressStatus1))->SetRange(0, 1000);
	//(&(g_pMainDlg->m_ProgressStatus1))->SendMessage(PBM_SETBARCOLOR, 0, RGB(0, 0, 255));
    UpdateStatusProgress(1,0.01,0);
	UpdateStatusProgress(2,0.01,0);
	UpdateStatusProgress(3,0.01,0);
	UpdateStatusProgress(4,0.01,0);
	UpdateStatusProgress(5,0.01,0);
	UpdateStatusProgress(6,0.01,0);
	UpdateStatusProgress(7,0.01,0);
	UpdateStatusProgress(8,0.01,0);
	
}


//end

void SNBase::SetProgress()
{
    m_iProcessRange = 1000;
    m_Process->SetRange(0, m_iProcessRange);
    m_Process->SendMessage(PBM_SETBARCOLOR, 0, RGB(0, 0, 255));
	UpdateProgress(0.01);
}

int SNBase::UpdateUIMsg(const char * strMsg, ...)
{
    int iRet = 0;
    char szBuf[256] = "";
    va_list varg;

    va_start(varg, strMsg);
    iRet = vsprintf_s(szBuf, strMsg, varg);
    va_end(varg);

    g_pMainDlg->SetDlgItemText(IDC_OPERATE_INFO, szBuf);

    return iRet;
}

void SNBase::UpdateUICountResult(WriteNvram_Status_e eWriteStatus)
{
    static UINT totalNums = 0;
    static UINT passNums = 0;
    static UINT failNums = 0;

    // for the user click 'STOP' button before USB still not insert
    if (m_bStopBeforeUSBInsert == true)
        return;

    totalNums += 1;
    if (eWriteStatus == PASS)
        passNums += 1;
    else
        failNums += 1;

    g_pMainDlg->SetDlgItemInt(IDC_COUNT_TOTAL, totalNums, FALSE);
    g_pMainDlg->SetDlgItemInt(IDC_COUNT_PASS, passNums, FALSE);
    g_pMainDlg->SetDlgItemInt(IDC_COUNT_FAIL, failNums, FALSE);

    UpdateMainDlgUI(true, eWriteStatus);
}

void SNBase::EnableStartBTN(bool bEnable)
{
    g_pMainDlg->EnableStartBTN(bEnable);

}

void SNBase::UpdateMainDlgUI(bool bEnable, WriteNvram_Status_e eWriteStatus)
{
    if (bEnable == false)
    {
        g_pMainDlg->DisableUIItem();
    }
    else
    {
        g_pMainDlg->EnableUIItem();
    }
    g_pMainDlg->UpdateOperateResult(eWriteStatus);

}

//add by wzb 20190320
void SNBase::UpdateStatusProgress(int iType,double dPercent,int iColor)
{
    if (dPercent >= 1)
    {
        dPercent = 1;
    }

    int curPos = (int)(dPercent * 100);
	CProgressCtrl *pStatusProcess=NULL;

	switch(iType)
	{
		case 1:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus1);
			break;
		case 2:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus2);
			break;
		case 3:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus3);
			break;
		case 4:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus4);
			break;
		case 5:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus5);
			break;
		case 6:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus6);
			break;
		case 7:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus7);
			break;
		case 8:
			pStatusProcess=&(g_pMainDlg->m_ProgressStatus8);
			break;
		default:
			break;
	}
	if(pStatusProcess!=NULL)
	{
		pStatusProcess->SetRange(0, 100);
		if(iColor==1)
		{
			pStatusProcess->SendMessage(PBM_SETBARCOLOR, 0, RGB(0, 255,0));
		}
		else
		{
			pStatusProcess->SendMessage(PBM_SETBARCOLOR, 0, RGB(255, 0, 0));
		}
		pStatusProcess->SetPos(curPos);
	}
}



//end

void SNBase::UpdateProgress(double dPercent)
{
    if (dPercent >= 1)
    {
        dPercent = 1;
    }

    int curPos = (int)(dPercent * m_iProcessRange);
    m_Process->SetPos(curPos);
}

void SNBase::PopupMsgBox(const char* strTilte, UINT uType, const char* strMsg, ...)
{
    char szBuf[256] = "";

    va_list varg;
    va_start(varg, strMsg);
    vsprintf_s(szBuf, strMsg, varg);
    va_end(varg);
    g_pMainDlg->PopupMsgBox(strTilte, uType, strMsg);
}

bool SNBase::NeedScanData()
{
    return (g_sMetaComm.sWriteOption.bWriteBarcode ||
        g_sMetaComm.sWriteOption.bWriteIMEI ||
        g_sMetaComm.sWriteOption.bWriteBT ||
        g_sMetaComm.sWriteOption.bWriteWifi ||
        g_sMetaComm.sWriteOption.bWriteEthernetMac ||
        g_sMetaComm.sWriteOption.bWriteDrmkeyMCID ||
        g_sMetaComm.sWriteOption.bWriteMeid ||
        g_sMetaComm.sWriteOption.bWriteEsn ||
        g_sMetaComm.sWriteOption.bWriteSerialNo);
}

CheckCalFlag_Status_e SNBase::CheckCalibrationStatus(const char* strBarcode)
{
    CheckCalFlag_Status_e eCheckCalFlag = CAL_FLAG_PASS;
    if (strBarcode[60] == '1' && strBarcode[61] == '0')  //Calibration Pass
    {
        eCheckCalFlag = CAL_FLAG_PASS;
    }
    else if (strBarcode[60] == '0' && strBarcode[61] == '1')//Calibration Fail
    {
        eCheckCalFlag = CAL_FLAG_FAILED;
    }
    else
    {
        eCheckCalFlag = CAL_FLAG_NOT_EXIST;
    }

    return eCheckCalFlag;

}

bool SNBase::CheckCalFinalTestStatus(const char* strBarcode)
{
    MTRACE(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): Check calibration & nsft flag start...");

    if (g_sMetaComm.bCheckCalFlag)
    {
        CheckCalFlag_Status_e eCheckCalFlag = CAL_FLAG_PASS;
        eCheckCalFlag = CheckCalibrationStatus(strBarcode);
        if (eCheckCalFlag != CAL_FLAG_PASS)
        {
            if (eCheckCalFlag == CAL_FLAG_FAILED)
            {
                UpdateUIMsg("Calibration Flag : Calibration Fail!");
                MTRACE_ERR(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): Calibration Flag : Calibration Fail!");
            }
            else if (eCheckCalFlag == CAL_FLAG_NOT_EXIST)
            {
                UpdateUIMsg("Calibration Flag : No exist calibration flag!!");
                MTRACE_ERR(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): Calibration Flag : No exist calibration flag!!");
            }

            return false;
        }

    }

    if (g_sMetaComm.bCheckFtFlag)
    {
        CheckFinalTestFlag_Status_e eCheckFTFlag = FT_FLAG_PASS;
        eCheckFTFlag = CheckFinalTestStatus(strBarcode);
        if (eCheckFTFlag != FT_FLAG_PASS)
        {
            if (eCheckFTFlag == FT_FLAG_FAILED)
            {
                UpdateUIMsg("FinalTest Flag : Final test Fail!");
                MTRACE_ERR(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): FinalTest Flag : Final test Fail!");
            }
            else if (eCheckFTFlag == FT_FLAG_NOT_EXIST)
            {
                UpdateUIMsg("FinalTest Flag : No exist final test flag!!");
                MTRACE_ERR(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): FinalTest Flag : No exist final test flag!!");
            }
            return false;
        }
    }

    MTRACE(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): Check calibration & nsft flag Pass!!");

    MTRACE(g_hEBOOT_DEBUG, "SNBase::CheckCalFinalTestStatus(): Check calibration & nsft flag end...");
    return true;
}

CheckFinalTestFlag_Status_e SNBase::CheckFinalTestStatus(const char* strBarcode)
{
    CheckFinalTestFlag_Status_e eCheckFTFlag = FT_FLAG_PASS;
    if (strBarcode[62] == 'P' )  //Final Test Pass
    {
        eCheckFTFlag = FT_FLAG_PASS;
    }
    else if (strBarcode[62] == 'F')//Final Test Fail
    {
        eCheckFTFlag = FT_FLAG_FAILED;
    }
    else
    {
        eCheckFTFlag = FT_FLAG_NOT_EXIST;
    }

    return eCheckFTFlag;
}

//add by wzb 20190318
META_RESULT SNBase::ConductBarcodeDataNoSyncProdinfo(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    char flag_wire;     // wireless flag
    char flag_cal[2];   // calibration flag
    char flag_nsft;     // nsft flag

    flag_wire = pOutData[59];
    flag_cal[0] = pOutData[60];
    flag_cal[1] = pOutData[61];
    flag_nsft = pOutData[62];

    strncpy_s(pOutData, outBufSize, pInDatabuf, BARCODE_MAX_LENGTH);

    if (flag_wire != '\0' && flag_cal[0] != '\0' && flag_cal[1] != '\0' && flag_nsft != '\0')
    {
        int barcode_len;
        barcode_len = strlen(pOutData);
        if (barcode_len < 59)
            memset(pOutData + barcode_len, 0x20, 59 - barcode_len);

        pOutData[59] = flag_wire;
        pOutData[60] = flag_cal[0];
        pOutData[61] = flag_cal[1];
        pOutData[62] = flag_nsft;
        pOutData[63] = '\0';
    }

    // Sync to AP Prod_Info
    //strncpy_s(m_sScanData.strBarcode, pOutData, BARCODE_MAX_LENGTH);

    return META_SUCCESS;
}


//end

META_RESULT SNBase::ConductBarcodeData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    char flag_wire;     // wireless flag
    char flag_cal[2];   // calibration flag
    char flag_nsft;     // nsft flag

    flag_wire = pOutData[59];
    flag_cal[0] = pOutData[60];
    flag_cal[1] = pOutData[61];
    flag_nsft = pOutData[62];

    strncpy_s(pOutData, outBufSize, pInDatabuf, BARCODE_MAX_LENGTH);

    if (flag_wire != '\0' && flag_cal[0] != '\0' && flag_cal[1] != '\0' && flag_nsft != '\0')
    {
        int barcode_len;
        barcode_len = strlen(pOutData);
        if (barcode_len < 59)
            memset(pOutData + barcode_len, 0x20, 59 - barcode_len);

        pOutData[59] = flag_wire;
        pOutData[60] = flag_cal[0];
        pOutData[61] = flag_cal[1];
        pOutData[62] = flag_nsft;
        pOutData[63] = '\0';
    }

    // Sync to AP Prod_Info
    strncpy_s(m_sScanData.strBarcode, pOutData, BARCODE_MAX_LENGTH);

    return META_SUCCESS;
}

META_RESULT SNBase::ConductIMEIData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    if (pOutData == NULL || pInDatabuf == NULL || outBufSize <= 0)
        return META_INVALID_ARGUMENTS;

    /*
    //UI input = "123459876543210" storage in nvram will be:
    //imei[0] = 0x21
    //imei[1] = 0x43
    //imei[2] = 0x95
    //imei[3] = 0x78
    //imei[4] = 0x56
    //imei[5] = 0x34
    //imei[6] = 0x12
    //imei[7] = 0xf0
    */
    META_RESULT MetaResult;
	char *pFuncName;

    if (m_bWriteNvram == false)//read from nvram
    {
        MTRACE (g_hEBOOT_DEBUG, "SNBase::Decompose_IMEI(): Start to Decompose IMEI[%d]...", RID_para-1);
		pFuncName = "SNBase::Decompose_IMEI(): Decompose IMEI";
        MetaResult = Decompose_IMEI(pOutData, RID_para, pInDatabuf, outBufSize);
    }
    else //write to nvram
    {
        MTRACE (g_hEBOOT_DEBUG, "SNBase::Compose_IMEI(): Start to Compose IMEI[%d]...", RID_para-1);
		pFuncName = "SNBase::Compose_IMEI(): Compose IMEI";
        MetaResult = Compose_IMEI( pOutData, RID_para, pInDatabuf, outBufSize, g_sMetaComm.sIMEIOption.bCheckSum);
    }

    if(MetaResult == META_SUCCESS)
    {
        MTRACE (g_hEBOOT_DEBUG, "%s successfully!", pFuncName);
    }
	else
	{
	    MTRACE_ERR (g_hEBOOT_DEBUG, "%s Fail! MetaResult = %s",  pFuncName, ResultToString(MetaResult));
	}
	return MetaResult;
}

META_RESULT SNBase::ConductBTAddrData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    if (pOutData == NULL || pInDatabuf == NULL || outBufSize <= 0)
        return META_INVALID_ARGUMENTS;

    /*
    *** Feature phone ***
    *  UI input = "1234567890AC" storage in Modem nvram will be:
    * btAddr[0] = 0xAC
    * btAddr[1] = 0x90
    * btAddr[2] = 0x78
    * btAddr[3] = 0x56
    * btAddr[4] = 0x34
    * btAddr[5] = 0x12
    ---------------------------------------------------------
    *** Smart Phone ***
    * UI input = "1234567890AC" storage in AP nvram will be:
    * btAddr[0] = 0x12
    * btAddr[1] = 0x34
    * btAddr[2] = 0x56
    * btAddr[3] = 0x78
    * btAddr[4] = 0x90
    * btAddr[5] = 0xAC
    */

    int tmpCh;
    if (!m_bWriteNvram)
    {
        for (int i = 0; i < outBufSize; i++)
        {
            tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
            if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
            {
                pOutData[(i*2 + 1)] = tmpCh + '0';
            }
            else if (10 <= tmpCh && tmpCh <= 15) //A - F
            {
                pOutData[(i*2 + 1)] = (tmpCh - 10) + 'A';
            }

            //tmpCh = ((pInDatabuf[i] >> 4)  & 15);
            tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
            if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
            {
                pOutData[i*2] = tmpCh + '0';
            }
            else if (10 <= tmpCh && tmpCh <= 15) //A - F
            {
                pOutData[i*2] = (tmpCh - 10) + 'A';
            }
        }
    }
    else if(m_bWriteNvram)
    {
        _strupr_s(pInDatabuf, strlen(pInDatabuf)+1);
        for (int i = 0; i < outBufSize; i++)
        {
            if ('0'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= '9')
            {
                tmpCh = pInDatabuf[i*2] - '0';
                tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
            }
            else if('A'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= 'F')
            {
                tmpCh = pInDatabuf[i*2] - 'A' + 10;
                tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
            }

            if ('0'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= '9')
            {
                tmpCh += pInDatabuf[(i*2 + 1)] - '0'; //get Low 4bits
            }
            else if('A'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= 'F')
            {
                tmpCh += pInDatabuf[(i*2 + 1)] - 'A' + 10; //get Low 4bits
            }

            pOutData[i] = tmpCh;
        }
    }

    return META_SUCCESS;
}

//add by wzb 20190319
META_RESULT SNBase::ConductCountryCodeData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    if (pOutData == NULL || pInDatabuf == NULL || outBufSize <= 0)
        return META_INVALID_ARGUMENTS;

    //strupr(pInDatabuf);       //转换字符串中的小写字母为大写
    //strlwr(pInDatabuf);       //转换字符串的大写字母为小写
    /*
    * UI input = "1234567890AC" storage in AP nvram will be:
    * wifiAddr[0] = 0x12
    * wifiAddr[1] = 0x34
    * wifiAddr[2] = 0x56
    * wifiAddr[3] = 0x78
    * wifiAddr[4] = 0x90
    * wifiAddr[5] = 0xAC
    */

    int tmpCh=0;
	if (m_bWriteNvram == false)
		{
			for (int i = 0; i < outBufSize; i++)
			{
				tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
				if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
				{
					pOutData[(i*2 + 1)] = tmpCh + '0';
				}
				else if (10 <= tmpCh && tmpCh <= 15) //a - f
				{
					pOutData[(i*2 + 1)] = (tmpCh - 10) + 'A';
				}
	
				//tmpCh = ((pInDatabuf[i] >> 4)  & 15);
				tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
				if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
				{
					pOutData[i*2] = tmpCh + '0';
				}
				else if (10 <= tmpCh && tmpCh <= 15) //A - F
				{
					pOutData[i*2] = (tmpCh - 10) + 'A';
				}
			}
		}
		else if(m_bWriteNvram == true)
		{
			_strupr_s(pInDatabuf, strlen(pInDatabuf)+1);
			for (int i = 0; i < outBufSize; i++)
			{
				if ('0'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= '9')
				{
					tmpCh = pInDatabuf[i*2] - '0';
					tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
				}
				else if('A'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= 'F')
				{
					tmpCh = pInDatabuf[i*2] - 'A' + 10;
					tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
				}
	
				if ('0'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= '9')
				{
					tmpCh += pInDatabuf[(i*2 + 1)] - '0'; //get Low 4bits
				}
				else if('A'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= 'F')
				{
					tmpCh += pInDatabuf[(i*2 + 1)] - 'A' + 10; //get Low 4bits
				}
	
				pOutData[i] = tmpCh;
			}
		}
	
		return META_SUCCESS;

}



//end

META_RESULT SNBase::ConductWifiAddrData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    if (pOutData == NULL || pInDatabuf == NULL || outBufSize <= 0)
        return META_INVALID_ARGUMENTS;

    //strupr(pInDatabuf);       //转换字符串中的小写字母为大写
    //strlwr(pInDatabuf);       //转换字符串的大写字母为小写
    /*
    * UI input = "1234567890AC" storage in AP nvram will be:
    * wifiAddr[0] = 0x12
    * wifiAddr[1] = 0x34
    * wifiAddr[2] = 0x56
    * wifiAddr[3] = 0x78
    * wifiAddr[4] = 0x90
    * wifiAddr[5] = 0xAC
    */

    int tmpCh;
    if (m_bWriteNvram == false)
    {
        for (int i = 0; i < outBufSize; i++)
        {
            tmpCh = (pInDatabuf[i] & 15);  //15 -->0000 1111, get Low 4Bits
            if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
            {
                pOutData[(i*2 + 1)] = tmpCh + '0';
            }
            else if (10 <= tmpCh && tmpCh <= 15) //a - f
            {
                pOutData[(i*2 + 1)] = (tmpCh - 10) + 'A';
            }

            //tmpCh = ((pInDatabuf[i] >> 4)  & 15);
            tmpCh = ((pInDatabuf[i] & 240) >> 4); //240 -->1111 0000, get High 4Bits
            if (0 <= tmpCh && tmpCh <= 9)  //0 - 9
            {
                pOutData[i*2] = tmpCh + '0';
            }
            else if (10 <= tmpCh && tmpCh <= 15) //A - F
            {
                pOutData[i*2] = (tmpCh - 10) + 'A';
            }
        }
    }
    else if(m_bWriteNvram == true)
    {
        _strupr_s(pInDatabuf, strlen(pInDatabuf)+1);
        for (int i = 0; i < outBufSize; i++)
        {
            if ('0'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= '9')
            {
                tmpCh = pInDatabuf[i*2] - '0';
                tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
            }
            else if('A'<= pInDatabuf[i*2] && pInDatabuf[i*2] <= 'F')
            {
                tmpCh = pInDatabuf[i*2] - 'A' + 10;
                tmpCh = ((tmpCh << 4) & 240);  //240 -->1111 0000, get High 4bits
            }

            if ('0'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= '9')
            {
                tmpCh += pInDatabuf[(i*2 + 1)] - '0'; //get Low 4bits
            }
            else if('A'<= pInDatabuf[(i*2 + 1)] && pInDatabuf[(i*2 + 1)] <= 'F')
            {
                tmpCh += pInDatabuf[(i*2 + 1)] - 'A' + 10; //get Low 4bits
            }

            pOutData[i] = tmpCh;
        }
    }

    return META_SUCCESS;
}

META_RESULT SNBase::ConductEthernetMacAddrData(char *pOutData, unsigned short RID_para, char *pInDatabuf, int outBufSize)
{
    if (pOutData == NULL || pInDatabuf == NULL || outBufSize <= 0)
        return META_INVALID_ARGUMENTS;

    /*
    * UI input = "1234567890AC" storage in AP nvram will be:
    * ethMacAddr[0] = 0x12
    * ethMacAddr[1] = 0x34
    * ethMacAddr[2] = 0x56
    * ethMacAddr[3] = 0x78
    * ethMacAddr[4] = 0x90
    * ethMacAddr[5] = 0xAC
    *
    * The same as wifi address, so just need to call ConductWifiAddrData() function
    */
    return ConductWifiAddrData(pOutData, RID_para, pInDatabuf, outBufSize);
}

META_RESULT SNBase::Decompose_IMEI(char *pOutData, unsigned short RID_para,  char *pInDatabuf, int bufSize)
{
    META_RESULT MetaResult = META_NVRAM_Decompose_IMEISV(&m_sIMEISV[RID_para-1], pInDatabuf, bufSize);
    if (MetaResult == META_SUCCESS)
    {
        for (int i=0; i<IMEI_ARRAY_LEN; i++)
        {
            pOutData[i] = m_sIMEISV[RID_para-1].imei[i];
        }
    }

	return MetaResult;
}

META_RESULT SNBase::Compose_IMEI(char *pOutData, unsigned short RID_para, char *pInDatabuf, int bufSize, bool bChecksum)
{
    META_RESULT MetaResult = META_SUCCESS;
    if (bChecksum)
    {
        int ret_i;
        char checksum_c;
        unsigned short checksum_v;

        MTRACE(g_hEBOOT_DEBUG, "SNBase::Compose_IMEI(): Calculate checksum bit start...");
        MetaResult = META_NVRAM_Calculate_IMEI_CD(pInDatabuf, &checksum_v);
        if (MetaResult == META_SUCCESS)
        {
            checksum_c = checksum_v + '0';

            if (pInDatabuf[IMEI_MAX_LENGTH - 1] != '\0' && pInDatabuf[IMEI_MAX_LENGTH - 1] != checksum_c)
            {
                char szInfo[256];

                MTRACE_WARN (g_hEBOOT_DEBUG, "SNBase::Compose_IMEI(): check IMEI checksum bit fail.");
                sprintf_s(szInfo, "The input IMEI %s checksum bit dismatch:\r\n"
                    "origin checksum %c, right checksum %c\r\n"
                    "Are you sure to continue anyway?",
                    pInDatabuf, pInDatabuf[IMEI_MAX_LENGTH - 1], checksum_c);
                ret_i = ::MessageBox(NULL, szInfo, "Warning",
                    MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2|MB_SYSTEMMODAL|MB_SETFOREGROUND|MB_TOPMOST);
                if (ret_i != IDYES)
                    return META_FAILED;

                pInDatabuf[IMEI_MAX_LENGTH - 1] = checksum_c;
            }
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SNBase::Compose_IMEI(): Calculate checksum bit fail, MetaResult = %s", ResultToString(MetaResult));
            return MetaResult;
        }
        MTRACE(g_hEBOOT_DEBUG, "SNBase::Compose_IMEI(): Calculate checksum bit end...");
    }

    for (int i = 0; i < IMEI_ARRAY_LEN; i++)
    {
        m_sIMEISV[RID_para - 1].imei[i] = pInDatabuf[i];
    }

    MetaResult = META_NVRAM_Compose_IMEISV_ex(&m_sIMEISV[RID_para - 1], pOutData, bufSize, bChecksum);
    return MetaResult;
}

META_RESULT SNBase::REQ_ReadFromModemNVRAM(FT_NVRAM_READ_REQ *psNVRAM_ReadReq, FT_NVRAM_READ_CNF *psNVRAM_ReadCnf)
{
    META_RESULT meta_result;
    meta_result = META_NVRAM_Read_Ex_r(m_hMauiMetaHandle,
        15000,
        psNVRAM_ReadReq,
        psNVRAM_ReadCnf);

    if (meta_result != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SNBase::META_NVRAM_Read_Ex_r(): Read nvram data fail! MetaResult = %s",
            ResultToString(meta_result));
    }

    return meta_result;
}

META_RESULT SNBase::REQ_WriteToModemNVRAM(FT_NVRAM_WRITE_REQ *psNVRAM_WriteReq, FT_NVRAM_WRITE_CNF *psNVRAM_WriteCnf)
{
    META_RESULT meta_result;
    meta_result = META_NVRAM_Write_Ex_r(m_hMauiMetaHandle,
        15000,
        psNVRAM_WriteReq,
        psNVRAM_WriteCnf);

    if (meta_result != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SNBase::META_NVRAM_Write_Ex_r(): Write nvram data fail! MetaResult = %s", ResultToString(meta_result));
        return meta_result;
    }

    return meta_result;
}



void SNBase::GetScanData(ScanData_struct &pScanData)
{
    memset(&m_sScanData, 0, sizeof(ScanData_struct));

    if (g_sMetaComm.sWriteOption.bWriteBarcode)
        strncpy_s(m_sScanData.strBarcode, pScanData.strBarcode, strlen(pScanData.strBarcode));

    if (g_sMetaComm.sWriteOption.bWriteBT)
        strncpy_s(m_sScanData.strBTAddr, pScanData.strBTAddr, strlen(pScanData.strBTAddr));

    if (g_sMetaComm.sWriteOption.bWriteWifi)
        strncpy_s(m_sScanData.strWifiAddr, pScanData.strWifiAddr, strlen(pScanData.strWifiAddr));

    if (g_sMetaComm.sWriteOption.bWriteEthernetMac)
        strncpy_s(m_sScanData.strEthernetMac, pScanData.strEthernetMac, strlen(pScanData.strEthernetMac));

    if (g_sMetaComm.sWriteOption.bWriteDrmkeyMCID)
        strncpy_s(m_sScanData.strDrmkeyMCID, pScanData.strDrmkeyMCID, strlen(pScanData.strDrmkeyMCID));

    if (g_sMetaComm.sWriteOption.bWriteIMEI)
    {
        for (int i = 0; i < g_sMetaComm.sIMEIOption.iImeiNums; i++)
            strncpy_s(m_sScanData.strIMEI[i], pScanData.strIMEI[i], strlen(pScanData.strIMEI[i]));

        if (g_AutoGenData.bAutoGenIMEI && g_sMetaComm.sIMEIOption.iImeiNums >= 2)
        {
            m_bNeedBackupIMEI = true;
            strncpy_s(m_strBackupIMEI, pScanData.strIMEI[0], strlen(pScanData.strIMEI[0]));
        }
    }

    if (g_sMetaComm.sWriteOption.bWriteSerialNo)
        strncpy_s(m_sScanData.strSerialNo, pScanData.strSerialNo, strlen(pScanData.strSerialNo));

    if (g_sMetaComm.sIMEIOption.bDualIMEISame)
        strncpy_s(m_sScanData.strIMEI[1], pScanData.strIMEI[0], strlen(pScanData.strIMEI[0]));

    if (g_sMetaComm.sWriteOption.bWriteMeid)
        strncpy_s(m_sScanData.strMeid, pScanData.strMeid, strlen(pScanData.strMeid));

    if (g_sMetaComm.sWriteOption.bWriteEsn)
        strncpy_s(m_sScanData.strEsn, pScanData.strEsn, strlen(pScanData.strEsn));
}

void SNBase::DebugOnOff(bool bOn)
{
    static bool bDebugOn = false;

    if (bOn && !bDebugOn)
    {
        int ret_i = 0;
        char log_file[MAX_PATH] = "";
        SYSTEMTIME time;

        // make sure log folder exist
        ret_i = ::PathFileExists(g_sMetaComm.strLogDir);
        if (ret_i == FALSE)
        {
            ret_i = ::CreateDirectory(g_sMetaComm.strLogDir, NULL);
            if (ret_i == FALSE)
            {
                if (strcmp(g_sMetaComm.strLogDir, "C:\\SNWriter_LOG\\") == 0)
                {
                    g_pMainDlg->MessageBox("Log Dir not exist and can not be created.",
                    "Warning",
                    MB_ICONWARNING | MB_OK);
                    return;
                }

                g_pMainDlg->MessageBox("Log Dir not exist and can not be created.\r\nUse default C:\\SNWriter_LOG\\.",
                                       "Warning",
                                       MB_ICONWARNING | MB_OK);

                strcpy_s(g_sMetaComm.strLogDir, "C:\\SNWriter_LOG\\");
                ret_i = ::CreateDirectory(g_sMetaComm.strLogDir, NULL);
                if (ret_i == FALSE)
                {
                    g_pMainDlg->MessageBox("Can not create default log dir C:\\SNWriter_LOG\\",
                                           "Warning",
                                           MB_ICONWARNING | MB_OK);
                    return;
                }
            }
        }

        ::GetLocalTime(&time);
        sprintf_s(m_strLogDir_Sub, "%s%04u-%02u-%02u-%02u-%02u-%02u\\",
            g_sMetaComm.strLogDir, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

        ret_i = ::CreateDirectory(m_strLogDir_Sub, NULL);
        if (ret_i == FALSE)
        {
            g_pMainDlg->MessageBox("Can not create default sub log dir.",
            "Warning",
            MB_ICONWARNING | MB_OK);
            m_strLogDir_Sub[0] = '\0';
            return;
        }

        /*ret_i = ::SHCreateDirectoryEx(g_pMainDlg != NULL ? g_pMainDlg->GetSafeHwnd() : NULL, m_strLogDir_Sub, NULL);
        if (ret_i != ERROR_SUCCESS)
        {
            sprintf(m_strLogDir_Sub, "C:\\SNWriter_LOG\\%04u-%02u-%02u-%02u-%02u-%02u\\",
                g_sMetaComm.strLogDir, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

            ::SHCreateDirectoryEx(g_pMainDlg != NULL ? g_pMainDlg->GetSafeHwnd() : NULL, m_strLogDir_Sub, NULL);
        }*/

        sprintf_s(log_file, "%sSN Writer.log", m_strLogDir_Sub);
        _Xboot_DebugOn_With_FilePath(log_file);
        sprintf_s(log_file, "%sMetaCore.log", m_strLogDir_Sub);
        META_DebugOn_With_FilePath(log_file);
        sprintf_s(log_file, "%sSP_BROM_DLL.log", m_strLogDir_Sub);
        SP_Brom_Debug_SetLogFilename(log_file);
        SP_Brom_DebugOn();

        MTRACE(g_hEBOOT_DEBUG, "%s", g_strToolVersion);
        bDebugOn = true;
    }
    else if (!bOn && bDebugOn)
    {
        META_DebugOff();
        SP_Brom_DebugOff();
        _Xboot_DebugOff();

        m_strLogDir_Sub[0] = '\0';

        bDebugOn = false;
    }
}

void SNBase::LogConfig()
{
    char szInfo[256] = "";

    // connect type
    if (g_sMetaComm.bUsbEnable)
        strcpy_s(szInfo, " Connect by USB");
    else
        sprintf_s(szInfo, " Connect by COM%d", g_sMetaComm.iCOMPort);
    MTRACE(g_hEBOOT_DEBUG, szInfo);

    // Target Type
    switch(g_sMetaComm.eTargetType)
    {
    case SMART_PHONE: strcpy_s(szInfo, " Target Type: Samrt Phone"); break;
    case SMART_PHONE_DUALMODEM: strcpy_s(szInfo, " Target Type: Smart DualModem Phone"); break;
    case TABLET_WIFI_ONLY: strcpy_s(szInfo, " Target Type: Tablet"); break;
    case RNDIG_DONGLE: strcpy_s(szInfo, " Target Type: Rndig Dongle"); break;
    default: strcpy_s(szInfo, " Target Type: Unknown"); break;
    }
    MTRACE(g_hEBOOT_DEBUG, szInfo);

    // Write Option
    sprintf_s(szInfo, " WO>"
        " Barcode(%d)"
        " IMEI(%d,%s,%s),"
        " BT Addr(%d),"
        " WIFI Addr(%d),"
        " Eth MAC(%d),"
        " Hdcp(%d)"
        ,
        g_sMetaComm.sWriteOption.bWriteBarcode?1:0,
        g_sMetaComm.sWriteOption.bWriteIMEI?g_sMetaComm.sIMEIOption.iImeiNums:0, g_sMetaComm.sIMEIOption.bCheckSum?"Check":"NoCheck", g_sMetaComm.sIMEIOption.bDualIMEISame?"Same":"Indep",
        g_sMetaComm.sWriteOption.bWriteBT?1:0,
        g_sMetaComm.sWriteOption.bWriteWifi?1:0,
        g_sMetaComm.sWriteOption.bWriteEthernetMac?1:0,
        g_sMetaComm.sWriteOption.bWriteHdcp?1:0
        );
    MTRACE(g_hEBOOT_DEBUG, szInfo);
    sprintf_s(szInfo, "    "
        " HdcpData(%d),"
        " Drm(%d),"
        " MCID(%d),"
        " Attest(%d),"
        " MEID(%d),"
        " ESN(%d),"
        " Serial No(%d)"
        ,
        g_sMetaComm.sWriteOption.bInstallHdcpData?1:0,
        g_sMetaComm.sWriteOption.bWriteDrm?1:0,
        g_sMetaComm.sWriteOption.bWriteDrmkeyMCID?1:0,
        g_sMetaComm.sWriteOption.bWriteAttestationKey?1:0,
        g_sMetaComm.sWriteOption.bWriteMeid?1:0,
        g_sMetaComm.sWriteOption.bWriteEsn?1:0,
        g_sMetaComm.sWriteOption.bWriteSerialNo?1:0
        );
    MTRACE(g_hEBOOT_DEBUG, szInfo);

    // Special Option
    sprintf_s(szInfo, " SO>"
        "AlreadyInMeta(%d),"
        " Security(%d),"
        " Bypass proinfo(%d),"
        " Ignore DB Inconsistent(%d),"
        " Check flag K(%d) NSFT(%d),"
        " Serial No from IMEI(%d)"
        ,
        g_sMetaComm.bAlreadyInMeata,
        g_sMetaComm.bSecurityUSB?1:0,
        g_sMetaComm.bSkipWriteProdInfo?1:0,
        g_sMetaComm.IgnoreDBInconsistent?1:0,
        g_sMetaComm.bCheckCalFlag?1:0, g_sMetaComm.bCheckFtFlag?1:0,
        g_sMetaComm.SerialNumFromIMEI?1:0
        );
    MTRACE(g_hEBOOT_DEBUG, szInfo);

    // DB Option
    sprintf_s(szInfo, " DO>"
        " APDB From DUT(%d),"
        " MDDB From DUT(%d),"
        " APDB Init(%d),"
        " MDDB1 Init(%d),"
        " MDDB2 Init(%d)"
        ,
        g_sMetaComm.sDBFileOption.bAPDBFromDUT?1:0,
        g_sMetaComm.sDBFileOption.bMDDBFromDUT?1:0,
        g_sMetaComm.sDBFileOption.bDBInitAP?1:0,
        g_sMetaComm.sDBFileOption.bDBInitModem_1?1:0,
        g_sMetaComm.sDBFileOption.bDBInitModem_2?1:0
        );
    MTRACE(g_hEBOOT_DEBUG, szInfo);
    MTRACE(g_hEBOOT_DEBUG, "  APDB: %s", g_sMetaComm.sDBFileOption.strAPDbpath);
    MTRACE(g_hEBOOT_DEBUG, " MDDB1: %s", g_sMetaComm.sDBFileOption.strMD1Dbpath);
    MTRACE(g_hEBOOT_DEBUG, " MDDB2: %s", g_sMetaComm.sDBFileOption.strMD2Dbpath);
}

void SNBase::MDLogSetParam(int mdindex)
{
    if (g_sMetaComm.bMDLog && m_strLogDir_Sub[0] != '\0')
    {
        m_tMetaReq_Ex.trace_para.mode = FILE_DUMP_MODE;
        m_tMetaReq_Ex.trace_para.modem_index = mdindex;
        m_tMetaReq_Ex.trace_para.dump_file_path = m_strLogDir_Sub;
        MTRACE(g_hEBOOT_DEBUG, "SNBase::MDLogSetParam(): set modem%d log path \"%s\".", mdindex, m_strLogDir_Sub);
    }
}

void SNBase::MDLogOn()
{
    DWORD dwErr = 0u;
    META_RESULT mrRet;
    HANDLE hFind = NULL;
    WIN32_FIND_DATA stFindData = {0};
    char szPath[MAX_PATH] = {0};

    if (!g_sMetaComm.bMDLog || m_strLogDir_Sub[0] == '\0')
    {
        // ensure modem log be off
        MDLogOff();
        return;
    }

    MTRACE(g_hEBOOT_DEBUG, "SNBase::MDLogOn(): find filter file (*.bin) in exe path...");
    if(::GetModuleFileName(NULL, szPath, MAX_PATH) > 0)
    {
        ::PathRemoveFileSpec(szPath);
        strcat_s(szPath, "\\*.bin");
    }
    else
    {
        dwErr = ::GetLastError();
        ResultToString_Win(dwErr, szPath, MAX_PATH);
        MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOn(): get exe file path fail(%u, %s), set to \"C:\\\".", dwErr, szPath);
        strcpy_s(szPath, "C:\\*.bin");
    }

    hFind = ::FindFirstFile(szPath, &stFindData);
    if (hFind == INVALID_HANDLE_VALUE)
        MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOn(): find filter file fail.");
    else
    {
        char * pTmp = NULL;

        pTmp = strrchr(szPath, '*');
        if (pTmp != NULL)
            *pTmp = '\0';
        strcat_s(szPath, stFindData.cFileName);
        MTRACE(g_hEBOOT_DEBUG, "SNBase::MDLogOn(): find filter file \"%s\".", szPath);
        mrRet = META_MDLogging_SetFilterToTarget_r(m_hMauiMetaHandle, szPath);
        if (mrRet != META_SUCCESS)
            MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOn(): set filter fail, %s.", ResultToString(mrRet));
        ::FindClose(hFind);
    }

    mrRet = META_Util_EnableTstMetaTraceMode_r(m_hMauiMetaHandle, 10 * 1000, (const unsigned char)FILE_DUMP_MODE);
    if (mrRet != META_SUCCESS)
        MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOn(): open modem log fail, %s.", ResultToString(mrRet));
}

void SNBase::MDLogOff()
{
    META_RESULT mrRet;
    unsigned char mode = (const unsigned char)FILE_DUMP_MODE;

    mrRet = META_QueryIfFunctionSupportedByTarget_r(m_hMauiMetaHandle, 10 *1000, "META_Util_QueryTstMetaTraceMode_r");
    if (mrRet != META_SUCCESS)
        MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOff(): query function META_Util_QueryTstMetaTraceMode_r fail, %s.", ResultToString(mrRet));
    else
    {
        mrRet = META_Util_QueryTstMetaTraceMode_r(m_hMauiMetaHandle, 10 * 1000, &mode);
        if (mrRet != META_SUCCESS)
            MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOff(): META_Util_QueryTstMetaTraceMode_r fail, %s.", ResultToString(mrRet));
    }

    if (mode == (const unsigned char)FILE_DUMP_MODE)
    {
        for (int i = 0; i < 3; i++)
        {
            mrRet = META_Util_EnableTstMetaTraceMode_r(m_hMauiMetaHandle, 10 * 1000, (const unsigned char)GENERAL_MODE);
            if (mrRet == META_SUCCESS)
                break;
            MTRACE_WARN(g_hEBOOT_DEBUG, "SNBase::MDLogOff(): META_Util_EnableTstMetaTraceMode_r fail, %s.", ResultToString(mrRet));
        }
    }
}

//add by wzb 20190319
void SNBase::CustomInfo_Init()
{

	SetStatusProgress();
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO1, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO2, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO3, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO4, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO5, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO6, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO7, "");
	g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO8, "");

}

void SNBase::SNThread_Init()
{
    //Get main dialog handle
    //g_pMainDlg = (CSNWriterDlg*)g_MainDlgHandle;

    m_Process = &(g_pMainDlg->m_ProgressSN);
    SetProgress();

    m_bNeedBackupIMEI = false;
    memset(m_strBackupIMEI, 0, IMEI_ARRAY_LEN);
    memset(m_sIMEISV, 0, sizeof(IMEISV_struct_T)*MAX_IMEI_NUMS);
    memset(&m_sScanData, 0, sizeof(ScanData_struct));
	//add by wzb 
	CustomInfo_Init();
	//end
    GetScanData(g_sMetaComm.sScanData);
	//add by wzb 20190319
	if(m_sScanData.strBarcode!=NULL && strlen(m_sScanData.strBarcode)>0)
	{
		
		g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO2, m_sScanData.strBarcode);
		UpdateStatusProgress(2,1.0,1);
	}

	//end
    g_iMetaStopFlag = 0;
    m_hThread = NULL;
    m_iThreadID = 0;
    m_sNVRAM_OPID = 0;
    m_pMetaStopFlag = &g_iMetaStopFlag;
    m_bWriteNvram = false;
    m_bTargetInMetaMode = false;
    m_bStopBeforeUSBInsert = true;
    m_hReadFromNVRAMEvent = NULL;
    m_hWriteToNVRAMEvent = NULL;
    m_bBackupNvramSuccess = false;

    m_bWriteModemNvram = g_sMetaComm.sWriteOption.bWriteMeid || g_sMetaComm.sWriteOption.bWriteEsn ||
        ((g_sMetaComm.sWriteOption.bWriteIMEI || g_sMetaComm.sWriteOption.bWriteBarcode) && g_sMetaComm.eTargetType != TABLET_WIFI_ONLY);

    UpdateUIMsg("");
}

void SNBase::SNThread_Start()
{
    g_iMetaStopFlag = 0;
    m_hThread =  AfxBeginThread(SNBase::ThreadStaticEntryPoint,
        (LPVOID)this,
        THREAD_PRIORITY_NORMAL,
        0,
        0,
        NULL);
}

void SNBase::SNThread_Stop()
{
    g_iMetaStopFlag = BOOT_STOP;
}
