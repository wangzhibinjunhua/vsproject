#include "StdAfx.h"
#include <shlwapi.h>
#include "SPexc.h"
#include "SN Writer.h"
#include "SN WriterDlg.h"
#include "ScanData.h"
#include "HdcpEncryption.h"
#include "C2kAgent_api.h"
//#include "C2kAgent_api_datatype.h"
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include "Encryption.h"
#include "SLA_Challenge.h"

#define s_MAX_PATH 512

#define META_MODEM_SRV_ETS 3
#define META_MODEM_CH_TUNNELING 2


extern CSNWriterDlg *g_pMainDlg;

static const GUID  GUID_PORT_CLASS_USB2SER = {0x4D36E978L, 0xE325, 0x11CE, {0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18}};

void __stdcall CNF_SPReadFromNVRAM(const AP_FT_NVRAM_READ_CNF *cnf, const short token, void *usrData)
{
    if (cnf->status == META_SUCCESS)
    {
        SetEvent(*((HANDLE*)usrData));
    }
}

void __stdcall CNF_SPWriteToNVRAM ( const AP_FT_NVRAM_WRITE_CNF *cnf, const short token, void *usrData)
{
    if (cnf->status == META_SUCCESS)
    {
        SetEvent(*((HANDLE*)usrData));
    }
}

SmartPhoneSN::SmartPhoneSN()
{
    memset(&m_stModeArg, 0, sizeof(SP_BOOT_ARG_S));
    memset(&m_sMdInfo, 0, sizeof(SP_MODEMInfo_s));
    m_nKernelComport = 0;
    m_eMetaMode = SP_NOTIN_META;
    m_hMauiMetaHandle = INVALID_META_HANDLE;
    m_hSPMetaHandle = INVALID_META_HANDLE;
    m_bWorldPhone = false;
    m_bWithoutMD = false;
    m_bDualModem = false;
    hdle_write_cmd = NULL;
    hdle_read_out = NULL;
    r_bufsize = sizeof(read_buf);
    memset(&read_buf, 0, r_bufsize);
}

SmartPhoneSN::~SmartPhoneSN()
{
    MetaHandle_DeInit();
    //dongck add for fastboot oem lock
    Adb_WR_Cmd("exit", strlen("exit"));
    //MTRACE(g_hEBOOT_DEBUG, "ATB_MR::Scan_Devices_FunProc(): close cmd.exe");
    memset(&read_buf, 0, r_bufsize);
    CloseHandle(hdle_write_cmd);
    //MTRACE(g_hEBOOT_DEBUG, "Adb_Manager::~Adb_Manager(): close hdle_write_cmd");
    CloseHandle(hdle_read_out);
    //MTRACE(g_hEBOOT_DEBUG, "Adb_Manager::~Adb_Manager(): close hdle_read_out");
}

void SmartPhoneSN::SPInit()
{
    memset(&m_stModeArg, 0, sizeof(SP_BOOT_ARG_S));
    memset(&m_sMdInfo, 0, sizeof(SP_MODEMInfo_s));
    m_nKernelComport = 0;
    m_eMetaMode = SP_NOTIN_META;
    m_bWorldPhone = false;
    m_bWithoutMD = false;
    m_bDualModem = false;
    m_bWriteProdInfo = false;

    m_iCurMDChanelIndex = 0;
    m_bDSDAProject = false;
    m_iC2kProject = 0;
    m_bInitExtMDdb = false;
    memset(m_iMDChannelIndex, 0, (sizeof(UINT)*MAX_MD_CHANNEL_NUM));

    if (g_sMetaComm.sWriteOption.bWriteBarcode ||
        g_sMetaComm.sWriteOption.bWriteIMEI ||
        g_sMetaComm.sWriteOption.bWriteMeid ||
        g_sMetaComm.sWriteOption.bWriteBT ||
        g_sMetaComm.sWriteOption.bWriteWifi ||
        g_sMetaComm.sWriteOption.bWriteSerialNo)
    {
        m_bWriteProdInfo = true;
    }
    else
    {
        m_bWriteProdInfo = false;
    }
}


//add by wzb
META_RESULT SmartPhoneSN::WriteCountryCode()
{
	MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode() start...");
	MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode() scanSN=%s...",m_sScanData.strBarcode);
	UpdateProgress(0.25);
	META_RESULT MetaResult = META_SUCCESS;
	MetaResult = ReadSN_From_PRODINFO();
	if (MetaResult != META_SUCCESS)
	{
		UpdateStatusProgress(4,1.0,0);
		g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO4, "fail");
		MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode()check SN FAIL!");
	}
	else
	{
		UpdateProgress(0.5);
		
		g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO4, "pass");
		MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode()check SN OK!");
		MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode()write countrycode start");
		MetaResult=REQ_CountryCode_WriteAP_NVRAM_Start(m_sScanData.strSerialNo,1);
		if(MetaResult != META_SUCCESS)
		{
			g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO5, "fail");
			MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode() write countrycode fail");
		}else
		{
			g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO5, "pass");
			MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode() write countrycode ok");
		}
	}
	
	MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteCountryCode() end...");
	return MetaResult;

}
//end

META_RESULT SmartPhoneSN::WriteNvramLoop()
{
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteNvramLoop() start...");

    SMART_PHONE_STAGE_e spStage = SP_START_STAGE;
    SMART_PHONE_STAGE_e spNextStage = SP_END_STAGE;
    META_RESULT MetaResult = META_SUCCESS;
    MULTIIMEI_OPTION_e multiIMEIOption = SINGLE_MD_SINGLE_IMEI;

    int iRet = 0;
    int option = 0;
    double fBeginProcess = 0.25;
    double fStep = 0.04;
    m_bBackupNvramSuccess = false;

    bool bCheckCalFlag = g_sMetaComm.bCheckCalFlag;
    bool bCheckFtFlag = g_sMetaComm.bCheckFtFlag;

    int mdNums = 1;
    int MDIndex = 0;
    int IMEI_index = 0;
    bool bWriteModemFail = false;
    if (g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM || m_bDSDAProject)
    {
        mdNums = 2;
        g_sMetaComm.sIMEIOption.iImeiNums = 2;
    }
    else if (g_sMetaComm.eTargetType == TABLET_WIFI_ONLY)
    {
        m_bWriteModemNvram = false;
        bCheckCalFlag = false;
        bCheckFtFlag = false;
        mdNums = 0;
    }
    else
    {
        mdNums = m_sMdInfo.number_of_md;
    }

    while ((spStage != SP_END_STAGE) && (*m_pMetaStopFlag != BOOT_STOP))
    {
        switch (spStage)
        {
        case SP_START_STAGE:
            if ((m_bWriteModemNvram || (bCheckCalFlag || bCheckFtFlag)) && mdNums >= 1)
            {
                //Target in AP meta mode, need to switch Modem meta
                if (m_eMetaMode == SP_AP_META)
                {
                    spNextStage = SP_AP2MD_STAGE;
                }
                else //Target already in modem meta mode
                {
                    spNextStage = SP_CHECK_CAL_FT_FLAG_STAGE;
                }
            }
            else
            {
                spNextStage = SP_BT_ADDRESS_STAGE;
            }
            break;

        case SP_AP2MD_STAGE:
            fBeginProcess += fStep;
            iRet = EnableModemMeta();
            if (iRet == META_SUCCESS)
            {
                spNextStage = SP_CHECK_CAL_FT_FLAG_STAGE;
            }
            else
            {
                UpdateUIMsg("ERROR!! APSwithToModemMeta_Ex() : MetaResult = %s", ResultToString(iRet));
                spNextStage = SP_END_STAGE;
            }
            break;

        case SP_CHECK_CAL_FT_FLAG_STAGE:
            if (g_sMetaComm.bCheckCalFlag || g_sMetaComm.bCheckFtFlag)
            {
                char tmpBuf[2048] = {0};
                iRet = REQ_ReadModem_NVRAM_Start(WRITE_BARCODE, tmpBuf, 1);
                if (iRet == META_SUCCESS)
                {
                    bool bCheckPass = true;
                    bCheckPass = CheckCalFinalTestStatus(tmpBuf);
                    if (bCheckPass == true)
                    {
                        spNextStage = SP_BARCODE_MD_STAGE;
                    }
                    else
                    {
                        iRet = META_FAILED;
                        spNextStage = SP_END_STAGE;
                    }
                }
                else
                {
                    UpdateUIMsg("ERROR!! Read Barcode: MetaResult = %s", ResultToString(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_BARCODE_MD_STAGE;
            }
            break;

        case SP_BARCODE_MD_STAGE:
            if (g_sMetaComm.sWriteOption.bWriteIMEI)
            {
                if (mdNums == 1 && g_sMetaComm.sIMEIOption.iImeiNums == 1)
                {
                    multiIMEIOption = SINGLE_MD_SINGLE_IMEI;
                }
                else if (mdNums == 1 && g_sMetaComm.sIMEIOption.iImeiNums >= 2)
                {
                    multiIMEIOption = SINGLE_MD_MULTI_IMEI;
                }
                else if (mdNums == 2)
                {
                    if (m_iC2kProject != 0 && g_sMetaComm.sIMEIOption.iImeiNums == 1)
                    {
                        multiIMEIOption = SINGLE_MD_SINGLE_IMEI;
                    }
                    if (m_iC2kProject != 0 && g_sMetaComm.sIMEIOption.iImeiNums >= 2 )
                    {
                        multiIMEIOption = SINGLE_MD_MULTI_IMEI;
                    }
                    if (g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM && g_sMetaComm.sIMEIOption.iImeiNums == 2)
                    {
                        multiIMEIOption = DUAL_MD_DUAL_IMEI;
                    }
                }
                else
                {
                    multiIMEIOption = MULTIIMEI_RESERVE;
                }
            }
            spNextStage = SP_IMEI_MD_STAGE;

            break;

        case SP_IMEI_MD_STAGE:

            bWriteModemFail = false;
            for (MDIndex = 0; MDIndex < mdNums && bWriteModemFail == false; MDIndex++)
            {
                //When MDIndex = 0, database already init by EnterModemMetaToInitModemDB() function
                //So just need to load MDIndex >= 1 MD database
                if (mdNums >= 2 && MDIndex >= 1 && m_iC2kProject == 0)
                {
                    fBeginProcess += fStep;
                    iRet = SwitchMDByIndex(MDIndex);
                    if (iRet != META_SUCCESS)
                    {
                        UpdateUIMsg("ERROR!!SwitchMDByIndex[%d] : MetaResult = %s", MDIndex, ResultToString(iRet));
                        bWriteModemFail = true;
                        break;
                    }
                    else
                    {
                        fBeginProcess += fStep;
                        iRet = LoadModemDatabase(MDIndex);
                        if (iRet != META_SUCCESS)
                        {
                            UpdateUIMsg("ERROR!!LoadModemDatabase[%d] : MetaResult = %s", MDIndex, ResultToString(iRet));
                            bWriteModemFail = true;
                            break;
                        }
                    }
                }

                if (g_sMetaComm.sWriteOption.bWriteBarcode && (MDIndex == 0 || (mdNums >= 2 && MDIndex >= 1 && m_iC2kProject == 0)))
                {
                    fBeginProcess += fStep;
                    MTRACE(g_hEBOOT_DEBUG, "Barcode[%d] = \"%s\"", MDIndex, m_sScanData.strBarcode);
                    iRet = REQ_WriteModem_NVRAM_Start(WRITE_BARCODE, m_sScanData.strBarcode, 1);
                    if (iRet != META_SUCCESS)
                    {
                        UpdateUIMsg("ERROR!! Barcode[%d] : MetaResult = %s", MDIndex, ResultToString(iRet));
                        bWriteModemFail = true;
                        break;
                    }
                }

                if (g_sMetaComm.sWriteOption.bWriteIMEI && (MDIndex == 0 || (mdNums >= 2 && MDIndex >= 1 && m_iC2kProject == 0)))
                {
                    if (multiIMEIOption == SINGLE_MD_SINGLE_IMEI || multiIMEIOption == SINGLE_MD_MULTI_IMEI)
                    {
                        for (int i = 0; i < g_sMetaComm.sIMEIOption.iImeiNums; i++)
                        {
                            fBeginProcess += fStep;
                            IMEI_index = i;
                            MTRACE(g_hEBOOT_DEBUG, "IMEI[%d] = \"%s\"", i, m_sScanData.strIMEI[i]);

                            iRet = REQ_WriteModem_NVRAM_Start(WRITE_IMEI, m_sScanData.strIMEI[i], i + 1);
                            if (iRet != META_SUCCESS)
                            {
                                if (m_bNeedBackupIMEI)
                                {
                                    strcpy_s(g_AutoGenData.sIMEI.Next, m_strBackupIMEI);
                                }

                                UpdateUIMsg("ERROR!! IMEI[%d] : MetaResult = %s", i, ResultToString(iRet));
                                bWriteModemFail = true;
                                break;
                            }
                        }
                    }
                    else if (multiIMEIOption == DUAL_MD_DUAL_IMEI)
                    {
                        fBeginProcess += fStep;
                        int i = 0;
                        i = MDIndex;
                        IMEI_index = MDIndex;
                        if (m_bDSDAProject)
                        {
                            IMEI_index = 0;
                        }

                        MTRACE(g_hEBOOT_DEBUG, "IMEI[%d] = \"%s\"", i, m_sScanData.strIMEI[i]);
                        iRet = REQ_WriteModem_NVRAM_Start(WRITE_IMEI, m_sScanData.strIMEI[i], IMEI_index + 1);
                        if (iRet != META_SUCCESS)
                        {
                            if (m_bNeedBackupIMEI)
                            {
                                strcpy_s(g_AutoGenData.sIMEI.Next, m_strBackupIMEI);
                            }

                            UpdateUIMsg("ERROR!! IMEI[%d] : MetaResult = %s", i, ResultToString(iRet));
                            bWriteModemFail = true;
                            break;
                        }
                    }

                    if (iRet == META_SUCCESS)
                    {
                        if ((IMEI_index + 1) == g_sMetaComm.sIMEIOption.iImeiNums)
                        {
                            if (g_sMetaComm.sIMEIOption.bLockIMEI)
                            {
                                iRet = META_NVRAM_LockDown_r(m_hMauiMetaHandle, 5000);
                                if (iRet != META_SUCCESS)
                                {
                                    bWriteModemFail = true;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        bWriteModemFail = true;
                        break;
                    }
                }//end if(g_sMetaComm.sWriteOption.bWriteIMEI)
            }//end for(MDIndex = 0; MDIndex < mdNums; MDIndex++)

            // Backup imei to SDS
            if (!bWriteModemFail && m_bDSDAProject)
            {
                bool backupResult = DSDA_ExternalModemBackup();
                if (!backupResult)
                {
                    UpdateUIMsg("ERROR!! Backup external modem fail!");
                    spNextStage = SP_C2K_STAGE;
                    break;
                }
            }

            if (bWriteModemFail == true)
            {
                spNextStage = SP_END_STAGE;
            }
            else
            {
                spNextStage = SP_C2K_STAGE;
            }

            break;

        case SP_C2K_STAGE:
            fBeginProcess += fStep;
            if (g_sMetaComm.sWriteOption.bWriteMeid || g_sMetaComm.sWriteOption.bWriteEsn)
            {
                if (m_iC2kProject != 0)
                    iRet = EnterC2KGen90();
                else
                    iRet = EnterC2KGen93();

                if (iRet != META_SUCCESS)
                {
                    spNextStage = SP_END_STAGE;
                    break;
                }
            }
            spNextStage = SP_MD2AP_STAGE;
            break;

        case SP_MD2AP_STAGE:

            if (g_sMetaComm.sIMEIOption.bLockOtp)
            {
                iRet = LockOTP();
                if (iRet != 0)
                {
                    spNextStage = SP_END_STAGE;
                    break;
                }
            }

            fBeginProcess += fStep;
            spNextStage = SP_BT_ADDRESS_STAGE;
            break;

        case SP_BT_ADDRESS_STAGE:
            if (g_sMetaComm.sWriteOption.bWriteBT)
            {
                fBeginProcess += fStep;
                MTRACE(g_hEBOOT_DEBUG, "BT = \"%s\"", m_sScanData.strBTAddr);
                iRet = REQ_WriteAP_NVRAM_Start(WRITE_BT, m_sScanData.strBTAddr, 1);
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_WIFI_ADDRESS_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! BT : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_WIFI_ADDRESS_STAGE;
            }
            break;

        case SP_WIFI_ADDRESS_STAGE:
            if (g_sMetaComm.sWriteOption.bWriteWifi)
            {
                fBeginProcess += fStep;
                MTRACE(g_hEBOOT_DEBUG, "Wifi = \"%s\"", m_sScanData.strWifiAddr);
                iRet = REQ_WriteAP_NVRAM_Start(WRITE_WIFI, m_sScanData.strWifiAddr, 1);
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_ETHERNET_MAC_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! Wifi : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_ETHERNET_MAC_STAGE;
            }
            break;

        case SP_ETHERNET_MAC_STAGE:
            if (g_sMetaComm.sWriteOption.bWriteEthernetMac)
            {
                fBeginProcess += fStep;
                MTRACE(g_hEBOOT_DEBUG, "Ethernet Mac = \"%s\"", m_sScanData.strEthernetMac);
                iRet = REQ_WriteAP_NVRAM_Start(WRITE_ETHERNET_MAC, m_sScanData.strEthernetMac, 1);
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_HDCP_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! Ethernet Mac : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_HDCP_STAGE;
            }
            break;

        case SP_HDCP_STAGE:
            fBeginProcess += fStep;
            if (g_sMetaComm.sWriteOption.bWriteHdcp)
            {
                iRet = REQ_WriteHdcpBinToAPNvram_Start(g_sMetaComm.sLoadFile.strHdcpBinPath);
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_DRMKEY_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! Hdcp : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_DRMKEY_STAGE;
            }
            break;

        case SP_DRMKEY_STAGE:
            fBeginProcess += fStep;
            if (g_sMetaComm.sWriteOption.bWriteDrm)
            {
                iRet = REQ_InstallDRMKey_Start();
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_HDCPDATA_INSTALL_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! DRMKey : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_HDCPDATA_INSTALL_STAGE;
            }
            break;

        case SP_HDCPDATA_INSTALL_STAGE:
            fBeginProcess += fStep;
            if (g_sMetaComm.sWriteOption.bInstallHdcpData)
            {
                iRet = REQ_InstallHdcpData_Start(g_sMetaComm.sLoadFile.strHdcpDataPath, g_sMetaComm.sLoadFile.strHdcpCekPath);
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_DRMKEY_MCID_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! HdcpData : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_DRMKEY_MCID_STAGE;
            }
            break;

        case SP_DRMKEY_MCID_STAGE:
            fBeginProcess += fStep;
            if (g_sMetaComm.sWriteOption.bWriteDrmkeyMCID)
            {
                iRet = REQ_WriteDRMKeyMCID_Start(m_sScanData.strDrmkeyMCID);
                if (iRet == META_SUCCESS)
                {
                    spNextStage = SP_ATTESTATIONKEY_STAGE;
                }
                else
                {
                    UpdateUIMsg("ERROR!! DRMKeyMCID : MetaResult = %s", ResultToString_SP(iRet));
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_ATTESTATIONKEY_STAGE;
            }
            break;

        case SP_ATTESTATIONKEY_STAGE:
            fBeginProcess += fStep;
            if (!g_sMetaComm.sWriteOption.bWriteAttestationKey)
            {
                spNextStage = SP_BACKUPNV_TO_PC_STAGE;
                break;
            }

            iRet = REQ_InstallAttestationKey_Start(g_sMetaComm.sLoadFile.strAttestationKeyPath);
            if (iRet != 0)
            {
                spNextStage = SP_END_STAGE;
                break;
            }

            spNextStage = SP_BACKUPNV_TO_PC_STAGE;
            break;

        case SP_BACKUPNV_TO_PC_STAGE:
            if (g_sMetaComm.bCheckBackNVtoPC)
            {
                fBeginProcess += fStep;
                char NumFile[128] = {0};
                iRet = GetNvramFileName(NumFile, 128);

                if (iRet == META_SUCCESS)
                {
                    iRet = REQ_BackupNvram2PC_Start(NumFile);
                    if (iRet != META_SUCCESS)
                    {
                        UpdateUIMsg("ERROR!! REQ_BackupNvram2PC_Start : MetaResult = %s", ResultToString_SP(iRet));
                        spNextStage = SP_END_STAGE;
                    }
                    else
                    {
                        spNextStage = SP_BACKUP_NVRAM_STAGE;
                    }
                }
                else
                {
                    spNextStage = SP_END_STAGE;
                }
            }
            else
            {
                spNextStage = SP_BACKUP_NVRAM_STAGE;
            }
            break;

        case SP_BACKUP_NVRAM_STAGE:
        {
            fBeginProcess += fStep;

            iRet = REQ_BackupNvram2BinRegion_Start();
            if (iRet != META_SUCCESS)
            {
                m_bBackupNvramSuccess = false;
                UpdateUIMsg("ERROR!! BackUP : MetaResult = %s", ResultToString_SP(iRet));
                spNextStage = SP_END_STAGE;
            }
            else
            {
                m_bBackupNvramSuccess = true;
                spNextStage = SP_PRODINFO_STAGE;
            }

            break;
        }

        case SP_PRODINFO_STAGE:
        {
            if (m_bWriteProdInfo && g_sMetaComm.bSkipWriteProdInfo == false)
            {
                MTRACE(g_hEBOOT_DEBUG, "Prod_Info = \"...\"");
                fBeginProcess += fStep;
                iRet = REQ_WriteAP_PRODINFO_Start();
                if (iRet != META_SUCCESS)
                {
                    UpdateUIMsg("ERROR!! Prod_Info : MetaResult = %s", ResultToString_SP(iRet));
                }
            }
            spNextStage = SP_END_STAGE;

            break;
        }

        default:
            // for maintain use
            MTRACE_ERR(g_hEBOOT_DEBUG, "Unknown stage %d, end the flow.", spStage);
            UpdateUIMsg("ERROR!! Unsupported write item.");
            iRet = META_NOT_SUPPORT;
            spNextStage = SP_END_STAGE;
        }

        UpdateProgress(fBeginProcess);
        spStage = spNextStage;
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::WriteNvramLoop() end...");
    return (META_RESULT)iRet;
}

int SmartPhoneSN::GetSPModemInfo_Ex()
{
    int iRet = 0;
    char *pFuncName = NULL;

    //Init parameters for without world phone feature
    m_sMdInfo.number_of_md = 0;
    m_sMdInfo.active_md_idx = 1;
    m_sMdInfo.number_of_mdSwImg = 1;
    m_sMdInfo.activeMdTypeIdx = 0;
    m_bDSDAProject = false;

    MODEM_QUERY_INFO_REQ pReq;
    MODEM_QUERY_INFO_CNF pCnf;
    memset(&pReq, 0, sizeof(MODEM_QUERY_INFO_REQ));
    memset(&pCnf, 0, sizeof(MODEM_QUERY_INFO_CNF));

    if (SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 5000, "SP_META_MODEM_Query_Info_r") == META_SUCCESS)
    {
        iRet = SP_META_MODEM_Query_Info_r(m_hSPMetaHandle, 10000, &pReq, &pCnf);
        if (iRet != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Query_Info_r(): fail, %s.", ResultToString_SP(iRet));
            return iRet;
        }
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Query_Info_r(): ok. md_num = %d, active_md_idx = %d.",
               pCnf.modem_number, pCnf.modem_id);
        m_sMdInfo.number_of_md = pCnf.modem_number;
        m_sMdInfo.active_md_idx = pCnf.modem_id;
        m_bWithoutMD = (m_sMdInfo.number_of_md == 0) ? true : false;
        m_bDualModem = (m_sMdInfo.number_of_md == 2) ? true : false;
    }

    if (m_sMdInfo.number_of_md <= 0)
        return META_SUCCESS;

    MODEM_CAPABILITY_LIST_REQ pCapabilitiesReq;
    MODEM_CAPABILITY_LIST_CNF pCapabilitiesCnf;
    memset(&pCapabilitiesReq, 0, sizeof(pCapabilitiesReq));
    memset(&pCapabilitiesCnf, 0, sizeof(pCapabilitiesCnf));

    if (SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 5000, "SP_META_MODEM_Capability_r") == META_SUCCESS)
    {
        iRet = SP_META_MODEM_Capability_r(m_hSPMetaHandle, 10000, &pCapabilitiesReq, &pCapabilitiesCnf);
        if (iRet != META_SUCCESS)
        {
            m_sMdInfo.multi_md_capability_support = 0;
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Capability_r(): Get modem capability from target Fail!!");

            m_tMetaReq_Ex.protocol = 1;
            m_tMetaReq_Ex.channel_type = 1;
            iRet = META_SUCCESS;
        }
        else
        {
            m_sMdInfo.multi_md_capability_support = 1;
            memcpy(&m_SpMdCapList, &pCapabilitiesCnf, sizeof(pCapabilitiesCnf));
            m_tMetaReq_Ex.protocol = m_SpMdCapList.modem_cap[m_sMdInfo.active_md_idx - 1].md_service;
            m_tMetaReq_Ex.channel_type =  m_SpMdCapList.modem_cap[m_sMdInfo.active_md_idx - 1].ch_type;

            if (m_sMdInfo.number_of_md >= 2)
            {
                m_bDSDAProject = true;
                m_iC2kProject = 0;
                int mdIndex = 0;

                for (int i = 0; i < MAX_MD_CHANNEL_NUM; i++)
                {
                    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Modem capability, protocol = %d, channel_type = %d!",
                           m_SpMdCapList.modem_cap[i].md_service, m_SpMdCapList.modem_cap[i].ch_type);

                    if (m_SpMdCapList.modem_cap[i].md_service > 0 )
                    {
                        m_iMDChannelIndex[mdIndex] = i;
                        mdIndex += 1;
                    }

                    if (m_sMdInfo.number_of_md == 2 && m_SpMdCapList.modem_cap[i].md_service == 3)
                    {
                        m_iC2kProject = 1;
                        m_bDSDAProject = false;
                    }
                }
            }
        }
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Capability_r(): Get modem capability, protocol = %d, channel_type = %d!",
               m_tMetaReq_Ex.protocol, m_tMetaReq_Ex.channel_type);
    }


    MODEM_GET_CURRENTMODEMTYPE_REQ pCurMDTypeReq;
    MODEM_GET_CURENTMODEMTYPE_CNF pCurMDTypeCnf;
    memset(&pCurMDTypeReq, 0, sizeof(pCurMDTypeReq));
    memset(&pCurMDTypeCnf, 0, sizeof(pCurMDTypeCnf));

    if (SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 5000, "SP_META_MODEM_Get_CurrentModemType_r") == META_SUCCESS)
    {
        iRet = SP_META_MODEM_Get_CurrentModemType_r(m_hSPMetaHandle, 10000, &pCurMDTypeReq, &pCurMDTypeCnf);
        if (iRet != META_SUCCESS)
        {
            iRet = META_SUCCESS;
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Get_CurrentModemType_r(): Get modem type from target Fail, mean that handset not support worldphone feature!!");
        }
        else
        {
            m_sMdInfo.current_mdtype = pCurMDTypeCnf.current_modem_type;
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Get_CurrentModemType_r(): Get MD Image info from target success, MdType = %d.", m_sMdInfo.current_mdtype);
        }
    }


    MODEM_QUERY_MDIMGTYPE_REQ pMDImgTypeReq;
    MODEM_QUERY_MDIMGTYPE_CNF pMDImgTypeCnf;
    memset(&pMDImgTypeReq, 0, sizeof(pMDImgTypeReq));
    memset(&pMDImgTypeCnf, 0, sizeof(pMDImgTypeCnf));

    if (SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 5000, "SP_META_MODEM_Query_MDIMGType_r") == META_SUCCESS)
    {
        iRet = SP_META_MODEM_Query_MDIMGType_r(m_hSPMetaHandle, 10000, &pMDImgTypeReq, &pMDImgTypeCnf);
        if (iRet != META_SUCCESS)
        {
            iRet = META_SUCCESS;
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Query_MDIMGType_r(): Get MD Image info from target fail, mean that handset not support worldphone feature!!");
        }
        else
        {
            memcpy(m_sMdInfo.mdimg_type, pMDImgTypeCnf.mdimg_type, 16);
            bool bAllZero = true;
            m_bWorldPhone = true;
            m_sMdInfo.number_of_mdSwImg = 0;
            //Get current active mdtype index
            for (int i = 0; i < 16; i++)
            {
                if (m_sMdInfo.mdimg_type[i] != 0 )
                {
                    m_sMdInfo.number_of_mdSwImg += 1;
                    bAllZero = false;
                }

                if (m_sMdInfo.current_mdtype != 0 && m_sMdInfo.mdimg_type[i] == m_sMdInfo.current_mdtype)
                {
                    m_sMdInfo.activeMdTypeIdx = i;
                }
            }

            //For before MT6577(include MT6577)old chip project
            //Old chip project call this api will return success, but mdimg_type array data all zero
            if (bAllZero)
            {
                m_sMdInfo.number_of_mdSwImg = 1;
                m_sMdInfo.activeMdTypeIdx = 0;
                m_bWorldPhone = false;
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Query_MDIMGType_r(): Get MD Image info from target success but all zero, mean that handset not support worldphone feature!!");
            }
            else
            {
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Query_MDIMGType_r(): Get MD Image info from target success, activeMdTypeIdx = %d, mean that handset support worldphone feature!!", m_sMdInfo.activeMdTypeIdx);
            }

            if (m_sMdInfo.number_of_md >= 2)
            {
                //extern modem have one SwImg
                m_sMdInfo.number_of_mdSwImg += (m_sMdInfo.number_of_md - 1);
            }
        }
    }

    //if(SPMETA_DLL:P_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 5000, "SP_META_MODEM_Query_Download_Status_r ") == META_SUCCESS)
    if (m_bDSDAProject)
    {
        MODEM_QUERY_DOWNLOAD_STATUS_REQ pDLReq;
        MODEM_QUERY_DOWNLOAD_STATUS_CNF pDLCnf;

        memset(&pDLReq, 0, sizeof(MODEM_QUERY_DOWNLOAD_STATUS_REQ));
        memset(&pDLCnf, 0, sizeof(MODEM_QUERY_DOWNLOAD_STATUS_CNF));

        iRet = SP_META_MODEM_Query_Download_Status_r (m_hSPMetaHandle, 80000, &pDLReq, &pDLCnf);
        if (iRet != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_MODEM_Query_Download_Status_r(): Query extern modem download process fail, MetaResult = %s", ResultToString_SP(iRet));
            iRet = 0;
        }
    }

    return iRet;
}

//Fix connect with kernel comport probabilistic fail bug
int SmartPhoneSN::TryToOpenSPKernelComport(int KernelCom_Num)
{
    int ret_i = 1;
    DWORD dwError_win = 0u;
    char * sz_error = NULL;
    HANDLE Hcom;
    char tmp_com[30] = {0};
    int retryTime = 0;

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel comport until it ready, KernelCom_Num = %d...", KernelCom_Num);

    sz_error = new char[1024];
    memset(sz_error, 0, 1024);
    sprintf_s(tmp_com, "\\\\.\\COM%d", KernelCom_Num);

    //try to open kernel comport until it ready
    do
    {
        retryTime += 1;
        Hcom = CreateFile(tmp_com, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (INVALID_HANDLE_VALUE != Hcom)
        {
            CloseHandle(Hcom);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): success, retryTime = %d", retryTime);
            ret_i = 0;
            break;
        }
        dwError_win = GetLastError();
        if (sz_error != NULL)
        {
            ResultToString_Win(dwError_win, sz_error, 1024u);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): fail(%u: %s), retryTime = %d",
                   dwError_win, sz_error, retryTime);
        }
        else
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): fail(%u), retryTime = %d",
                   dwError_win, retryTime);

        Sleep(1000);
    } while (retryTime < 50);

    if (sz_error != NULL)
        delete [] sz_error;
    return ret_i;
}

int SmartPhoneSN::ConnectWithKernelPort_Ex()
{
    int iRet = 0;

    PortEnumHelper cPortHelper;
    SP_COM_PROPERTY_S * psCOMProperty = NULL;
    META_ConnectByUSB_Req spUSBConnReq;
    META_ConnectByUSB_Report spUSBConnReport;

    // kernel comport filter white list
    cPortHelper.SetFilter(SP_WHITE_LIST, g_sMetaComm.sPortFilter.strKernelFilter);
    psCOMProperty = cPortHelper.GetPorts(true);
    memset(&spUSBConnReq, 0, sizeof(spUSBConnReq));
    memset(&spUSBConnReport, 0, sizeof(spUSBConnReport));

    if (g_sMetaComm.bUsbEnable)
    {
        // phase out SP_META_GetDynamicUSBComPort_r API, cause it have probabilistic can`t fetch kernel comport issue
        if (!g_sMetaComm.bAlreadyInMeata)
        {
            // timeout unit: s, but m_tMetaReq_Ex.ms_connect_timeout unit is ms
            int iTimeout = m_tMetaReq_Ex.ms_connect_timeout / 1000;
            MTRACE(g_hEBOOT_DEBUG, "SP_BROM::SP_GetUSBCOMPortWithFilter(): enum kernel comport...");
            iRet = SP_GetUSBCOMPortWithFilter(cPortHelper.GetFilter(), psCOMProperty, false, m_pMetaStopFlag, iTimeout);
            if (iRet != 0)
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "SP_BROM::SP_GetUSBCOMPortWithFilter(): enum kernel comport fail(%d).", iRet);
                iRet = 1;
                goto Err;
            }
            MTRACE(g_hEBOOT_DEBUG, "SP_BROM::SP_GetUSBCOMPortWithFilter(): kernel comport %u.", psCOMProperty->m_uNumber);
        }
        else
        {
            int nPortNum = 1;
            MTRACE(g_hEBOOT_DEBUG, "SP_BROM::SP_GetCurrentCOMPortInfoWithFilter(): enum kernel comport...");
            for (unsigned int time = 0; time <= m_tMetaReq_Ex.ms_connect_timeout; time += 500)
            {
                iRet = SP_GetCurrentCOMPortInfoWithFilter(cPortHelper.GetFilter(), &GUID_PORT_CLASS_USB2SER, false, psCOMProperty, &nPortNum);
                if (iRet == 0 && nPortNum > 0)
                    break;

                MTRACE_ERR(g_hEBOOT_DEBUG, "SP_BROM::SP_GetCurrentCOMPortInfoWithFilter(): enum fail(%d)", iRet);
                Sleep(500);
            }
            if (nPortNum > 1)
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "SP_BROM::SP_GetCurrentCOMPortInfoWithFilter(): enum %d kernel comports, please ensure only 1 valid port.", nPortNum);
                iRet = 1;
                goto Err;
            }
            if (iRet != 0 || nPortNum <= 0)
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "SP_BROM::SP_GetCurrentCOMPortInfoWithFilter(): enum kernel comport fail.");
                iRet = 1;
                goto Err;
            }
            MTRACE(g_hEBOOT_DEBUG, "SP_BROM::SP_GetCurrentCOMPortInfoWithFilter(): kernel comport %u.", psCOMProperty->m_uNumber);
        }
        m_nKernelComport = psCOMProperty->m_uNumber;

        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel comport...");
        spUSBConnReq.com_port = m_nKernelComport;
        spUSBConnReq.ms_connect_timeout = m_tMetaReq_Ex.ms_connect_timeout;
        iRet = TryToOpenSPKernelComport(m_nKernelComport);
        if (iRet != 0)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel comport fail.");
            iRet = 2;
            goto Err;
        }
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel com port ok.");

        MTRACE(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaModeByUSB_r(): Enter AP meta mode by comport %u...", m_nKernelComport);
        iRet = SP_META_ConnectInMetaModeByUSB_r (m_hSPMetaHandle, &spUSBConnReq, m_pMetaStopFlag, &spUSBConnReport);
        if (iRet == META_SUCCESS)
        {
            MTRACE(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaModeByUSB_r(): ok.");
            m_eMetaMode = SP_AP_META;
            m_bTargetInMetaMode = true;
            m_bStopBeforeUSBInsert = false;
            iRet = 0;
        }
        else if (iRet == META_MAUI_DB_INCONSISTENT)
        {
            m_eMetaMode = SP_AP_META;
            m_bTargetInMetaMode = true;
            m_bStopBeforeUSBInsert = false;
            if (!g_sMetaComm.sDBFileOption.bDBInitAP)
            {
                MTRACE(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaModeByUSB_r(): ap db inconsistent, directly ignore.");
                iRet = 0;
            }
            else if (g_sMetaComm.IgnoreDBInconsistent)
            {
                MTRACE_WARN(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaModeByUSB_r(): ap db inconsistent, but ignore.");
                iRet = 0;
            }
            else
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaModeByUSB_r(): fail, ap db inconsistent.");
                iRet = 3;
            }
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaModeByUSB_r(): Enter meta fail, %s", ResultToString_SP(iRet));
            m_eMetaMode = SP_NOTIN_META;
            m_bTargetInMetaMode = false;
            m_bStopBeforeUSBInsert = true;
            iRet = 3;
        }
    } // if(g_sMetaComm.bUsbEnable)
    else
    {
        m_nKernelComport = g_sMetaComm.iCOMPort;

        WM_META_ConnectInMETA_Req uartMETA_Req;
        WM_META_ConnectInMETA_Report uartMETA_Report;
        memset(&uartMETA_Req, 0 , sizeof(WM_META_ConnectInMETA_Req));
        memset(&uartMETA_Report, 0 , sizeof(WM_META_ConnectInMETA_Report));

        uartMETA_Req.com_port = m_nKernelComport;
        uartMETA_Req.baudrate[0] = META_BAUD921600;
        uartMETA_Req.baudrate[1] = META_BAUD_END;
        uartMETA_Req.flowctrl    = META_NO_FLOWCTRL;//META_SW_FLOWCTRL;
        uartMETA_Req.ms_connect_timeout = m_tMetaReq_Ex.ms_connect_timeout;

        MTRACE(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaMode_r(): Enter AP meta mode by comport %d...", m_nKernelComport);
        iRet = SP_META_ConnectInMetaMode_r(m_hSPMetaHandle, &uartMETA_Req, m_pMetaStopFlag, &uartMETA_Report);
        if (iRet == META_SUCCESS)
        {
            MTRACE(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaMode_r(): ok.");
            m_eMetaMode = SP_AP_META;
            m_bTargetInMetaMode = true;
            m_bStopBeforeUSBInsert = false;
            iRet = 0;
        }
        else if (iRet == META_MAUI_DB_INCONSISTENT)
        {
            m_eMetaMode = SP_AP_META;
            m_bTargetInMetaMode = true;
            m_bStopBeforeUSBInsert = false;
            if (!g_sMetaComm.sDBFileOption.bDBInitAP)
            {
                iRet = 0;
                MTRACE(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaMode_r(): ap db inconsistent, directly ignore.");
            }
            else if (g_sMetaComm.IgnoreDBInconsistent)
            {
                iRet = 0;
                MTRACE_WARN(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaMode_r(): ap db inconsistent, but ignore.");
            }
            else
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaMode_r(): fail, ap db inconsistent.");
                iRet = 3;
            }
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_ConnectInMetaMode_r(): Enter meta fail, %s", ResultToString_SP(iRet));
            m_eMetaMode = SP_NOTIN_META;
            m_bTargetInMetaMode = false;
            m_bStopBeforeUSBInsert = true;
            iRet = 3;
        }
    }

Err:
	
    return iRet;
}

int SmartPhoneSN::ConnectWithPreloader()
{
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ConnectWithPreloader(): Start to get dynamic preloader comport...");

    PortEnumHelper cPortHelper;
    SP_COM_PROPERTY_S sCOMProperty = {0};

    int iRet = 0;
    unsigned int eType;   //0:BoorROMUSB,1:PreloaderUSB

    cPortHelper.SetFilter(SP_WHITE_LIST, g_sMetaComm.sPortFilter.strBromFilter, BootROMUSB);
    cPortHelper.SetFilter(SP_WHITE_LIST, g_sMetaComm.sPortFilter.strPreloaderFilter, PreloaderUSB, true);

    if (g_sMetaComm.bUsbEnable == false)
    {
        m_stModeArg.m_uPortNumber = g_sMetaComm.iCOMPort;
        eType = PreloaderUSB;
    }
    else
    {
        int iTimeout = m_stModeArg.m_uTimeout / 1000;  //timeout unit: s, but m_stModeArg.m_uTimeout unit is ms
        iRet = SP_GetIncrementCOMPortWithFilter(cPortHelper.GetFilter(), &sCOMProperty, NULL, true, m_pMetaStopFlag, iTimeout);
        if (0 == iRet)
        {
            switch (cPortHelper.GetFlag(sCOMProperty.m_iFilterIndex))
            {
            case PreloaderUSB:
                eType = PreloaderUSB;
                break;
            case BootROMUSB:
                eType = BootROMUSB;
                break;
            default:
                //MessageBox (NULL, "Filter index error!", "fail", MB_OK);
                return META_FAILED;
            }

            m_stModeArg.m_uPortNumber = sCOMProperty.m_uNumber;
        }
        else
        {
            if (iRet == SP_S_TIMEOUT)
            {
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ConnectWithPreloader(): Get dynamic preloader comport timeout...");
                PopupMsgBox("Error", MB_OK | MB_ICONERROR, "SmartPhoneSN::ConnectWithPreloader(): Get dynamic preloader comport timeout...");
            }

            return iRet;
        }
    }

    ::Sleep(10);
    if (BootROMUSB == eType)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ConnectWithPreloader(): Get preloader comport successfully, comport = %d", m_stModeArg.m_uPortNumber);
        iRet = SP_BootROM_BootMode(&m_stModeArg);
        if (iRet == 0)
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_BootROM_BootMode(): Preloader boot meta mode success!!");
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_BootROM_BootMode(): Preloader boot meta mode Fail, Err = %d", iRet);
        }
    }
    else if (PreloaderUSB == eType)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ConnectWithPreloader(): Get preloader comport successfully, comport = %d", m_stModeArg.m_uPortNumber);
        iRet = SP_Preloader_BootMode(&m_stModeArg);
        if (iRet == 0)
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_Preloader_BootMode(): Preloader boot meta mode success!!");
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_Preloader_BootMode(): Preloader boot meta mode Fail, Err = %d", iRet);
        }
    }

    return iRet;
}

void SmartPhoneSN::ModemMetaHandle_DeInit()
{
    META_Deinit_r(&m_hMauiMetaHandle);
}

void SmartPhoneSN::APMetaHandle_DeInit()
{
    SP_META_Deinit_r(&m_hSPMetaHandle);
}

META_RESULT SmartPhoneSN::APMetaHandle_Init()
{
    META_RESULT spMetaResult = META_SUCCESS;

    m_hSPMetaHandle = m_hMauiMetaHandle;

    spMetaResult = SP_META_Init_r (m_hSPMetaHandle , NULL);
    if ( spMetaResult != META_SUCCESS)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_Init_r(): Init AP handle fail, MetaResult = %s", ResultToString_SP(spMetaResult));
        return spMetaResult;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_Init_r(): Init AP handle success");

    return META_SUCCESS;
}

META_MD_Query_Result_T __stdcall MdQueryHandler(void* MdQuery_CB_Arg)
{
    SP_MODEMInfo_s *sMdInfo = (SP_MODEMInfo_s*)MdQuery_CB_Arg;
    META_MD_Query_Result_T result;

    result.number_of_md = sMdInfo->number_of_md;
    result.active_md_idx = sMdInfo->active_md_idx - 1;
    result.number_of_mdSwImg = sMdInfo->number_of_mdSwImg;
    result.active_mdtype_idx = sMdInfo->activeMdTypeIdx;
    result.multi_talk = (result.active_md_idx != 0 || result.number_of_md >= 2) ? TRUE : FALSE;
    result.multi_frame_type = 1;

    //result.multi_mdtype = (sMdInfo->current_mdtype != 0 && result.number_of_mdSwImg >= 2)?TRUE:FALSE;
    result.multi_mdtype = (result.number_of_mdSwImg > result.number_of_md) ? true : false;//zishuo modify
    result.multi_md_capability_support = sMdInfo->multi_md_capability_support;
    result.reserved = 5;

    return result;
}

int __stdcall MdTypeSwitchHandler(META_MDTYPE_Switch_Param_T mdtype_switch_param, void* MdTypeSwitch_CB_Arg)
{
    return 1;
}

void SmartPhoneSN::MetaHandle_DeInit()
{
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::MetaHandle_DeInit() : DeInit meta handle start...");
    if (m_hMauiMetaHandle != INVALID_META_HANDLE)
    {
        ModemMetaHandle_DeInit();
        m_hMauiMetaHandle = INVALID_META_HANDLE;
    }

    if (m_hSPMetaHandle != INVALID_META_HANDLE)
    {
        APMetaHandle_DeInit();
        m_hSPMetaHandle = INVALID_META_HANDLE;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::MetaHandle_DeInit() : DeInit meta handle end...");
}

META_RESULT SmartPhoneSN::ModemMetaHandle_Init()
{
    META_RESULT meta_result = META_SUCCESS;
    meta_result = META_GetAvailableHandle_Ex(&m_hMauiMetaHandle, DATA_LIBRARY_MODE_EDB);
    if (meta_result != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::META_GetAvailableHandle(): Get available modem meta handle fail, MetaResult = %s", ResultToString(meta_result));
        return meta_result;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_GetAvailableHandle(): Get available modem meta handle success!");

    /*
    if (g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM)
    {
    meta_result =  META_Init_Ex_r( m_hMauiMetaHandle, NULL, MdQueryHandler, NULL, NULL, NULL);
    }
    else
    {
    meta_result = META_Init_Ex_2_r( m_hMauiMetaHandle, NULL, MdQueryHandler, NULL, NULL, NULL, MdTypeSwitchHandler, NULL);
    }
    */
    meta_result = META_Init_Ex_2_r( m_hMauiMetaHandle, NULL, MdQueryHandler, (void*)&m_sMdInfo, NULL, NULL, MdTypeSwitchHandler, NULL);
    if (meta_result != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::META_Init_Ex_2_r(): Init modem meta handle fail, MetaResult = %s", ResultToString(meta_result));
        return meta_result;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::META_Init_Ex_2_r(): Init modem meta handle success!");

    return meta_result;
}

META_RESULT SmartPhoneSN::MetaHandle_Init()
{
    META_RESULT result;

    if (m_hMauiMetaHandle == INVALID_META_HANDLE)
    {
        result = ModemMetaHandle_Init();
        if (result != META_SUCCESS)
            return result;

        result = APMetaHandle_Init();
        if (result != META_SUCCESS)
            return result;
    }

    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::REQ_ReadFromAPNVRAM(AP_FT_NVRAM_READ_REQ *psNVRAM_ReadReq, AP_FT_NVRAM_READ_CNF *psNVRAM_ReadCnf)
{
    DWORD wait_result;
    short iNVRAM_OPID;
    META_RESULT MetaResult;

    m_hReadFromNVRAMEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
    ::ResetEvent(m_hReadFromNVRAMEvent);

    MetaResult = SP_META_NVRAM_Read_r(m_hSPMetaHandle,
        psNVRAM_ReadReq,
        psNVRAM_ReadCnf,
        CNF_SPReadFromNVRAM,
        &iNVRAM_OPID,
        (void*)&m_hReadFromNVRAMEvent);

    wait_result = ::WaitForSingleObject(m_hReadFromNVRAMEvent, 15000);
    ::CloseHandle (m_hReadFromNVRAMEvent);
    if (wait_result == WAIT_TIMEOUT)
        return META_TIMEOUT;
    if (wait_result != WAIT_OBJECT_0)
        return META_FAILED;

    return MetaResult;
}

META_RESULT SmartPhoneSN::REQ_BackupNvram2BinRegion_Start()
{
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_BackupNvram2BinRegion_Start() : Backup nvram to BinRegion start...");
    UpdateUIMsg("Start backup nvram to BinRegion...");

    META_RESULT spMetaResult = META_SUCCESS;
    SetCleanBootFlag_REQ req;
    SetCleanBootFlag_CNF cnf;

    req.Notused = 0;

    spMetaResult =  SP_META_SetCleanBootFlag_r ( m_hSPMetaHandle, 15000, &req, &cnf );
    if (spMetaResult == META_SUCCESS && cnf.drv_status != 0)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_SetCleanBootFlag_r() : Backup nvram successfully!!");
    }
    else
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_SetCleanBootFlag_r() : Backup nvram Fail!! MetaResult %s, status %d",
            ResultToString_SP(spMetaResult), cnf.drv_status);
    }

    return spMetaResult;
}

int SmartPhoneSN::LockOTP()
{
    int ret_i;

    UpdateUIMsg("Lock OTP partition start...");
    MTRACE(g_hEBOOT_DEBUG, "Lock OTP partition start...");
    ret_i = META_NVRAM_OTP_LockDown_r(m_hMauiMetaHandle, 30000);
    if (ret_i != META_SUCCESS)
    {
        UpdateUIMsg("Lock OTP partition fail.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "Lock OTP partition fail(%d:%s).", ret_i, ResultToString(ret_i));
        return 1;
    }
    MTRACE(g_hEBOOT_DEBUG, "Lock OTP partition ok.");

    return 0;
}

META_RESULT SmartPhoneSN::ConductProdInfoData(unsigned char *pBuf, int nBufLen)
{
    typedef struct _sn_write_flag
    {
        unsigned char     total_pass: 1;
        unsigned char     barcode: 1;
        unsigned char     IMEI: 1;
        unsigned char     bt: 1;
        unsigned char     wifi: 1;
        unsigned char     MEID: 1;
        unsigned int      reserved: 10;
        //unsigned char       sn_write_flag[2];
    } sn_write_flag;

    PRODUCT_INFO * pstProInfo = (PRODUCT_INFO *)pBuf;
    sn_write_flag * pstSnFlag = (sn_write_flag *) & (pstProInfo->mtk_test_flag.sn_write_flag);
    memset(pstSnFlag, 0, sizeof(sn_write_flag));

    if (g_sMetaComm.sWriteOption.bWriteBarcode)
    {
        MTRACE(g_hEBOOT_DEBUG, "Prod_Info->Barcode = [%s]", m_sScanData.strBarcode);
        memcpy(&pstProInfo->barcode, m_sScanData.strBarcode, BARCODE_MAX_LENGTH);
        pstSnFlag->barcode = 1;
    }

    if (g_sMetaComm.sWriteOption.bWriteIMEI)
    {
        int idIMEI = 0, idByte = 0;
        unsigned char tmpCh;

        for (idIMEI = 0; idIMEI < g_sMetaComm.sIMEIOption.iImeiNums; idIMEI++)
        {
            MTRACE(g_hEBOOT_DEBUG, "Prod_Info->IMEI[%d] = [%s]", idIMEI, m_sScanData.strIMEI[idIMEI]);
            for (int idByte = 0; idByte < 7; idByte++)
            {
                // High 4Bit
                tmpCh = m_sScanData.strIMEI[idIMEI][idByte * 2 + 1] - '0';
                tmpCh = ((tmpCh << 4) & 0xf0u);

                // Low 4Bit
                tmpCh += m_sScanData.strIMEI[idIMEI][idByte * 2] - '0';
                pstProInfo->IMEI[idIMEI].imei[idByte] = tmpCh;
            }

            // Checksum Byte : 0xf-
            tmpCh = m_sScanData.strIMEI[idIMEI][14] - '0';
            tmpCh |= 0xf0u;
            pstProInfo->IMEI[idIMEI].imei[7] = tmpCh;
        }

        pstSnFlag->IMEI = 1;
    }

    if (g_sMetaComm.sWriteOption.bWriteBT)
    {
        char * pBtAddr = m_sScanData.strBTAddr;
        unsigned char tmpCh;

        MTRACE(g_hEBOOT_DEBUG, "Prod_Info->BtAddress = [%s]", m_sScanData.strBTAddr);
        for (int i = 0; i < 6; i++)
        {
            // High 4Bit
            if ('0' <= pBtAddr[i * 2] && pBtAddr[i * 2] <= '9')
            {
                tmpCh = pBtAddr[i * 2] - '0';
                tmpCh = ((tmpCh << 4) & 0xf0u);
            }
            else if ('A' <= pBtAddr[i * 2] && pBtAddr[i * 2] <= 'F')
            {
                tmpCh = pBtAddr[i * 2] - 'A' + 10;
                tmpCh = ((tmpCh << 4) & 0xf0u);
            }

            // Low 4Bit
            if ('0' <= pBtAddr[(i * 2 + 1)] && pBtAddr[(i * 2 + 1)] <= '9')
            {
                tmpCh += pBtAddr[(i * 2 + 1)] - '0';
            }
            else if ('A' <= pBtAddr[(i * 2 + 1)] && pBtAddr[(i * 2 + 1)] <= 'F')
            {
                tmpCh += pBtAddr[(i * 2 + 1)] - 'A' + 10;
            }

            pstProInfo->target_info.BTAddr[i] = tmpCh;
        }

        pstSnFlag->bt = 1;
    }

    if (g_sMetaComm.sWriteOption.bWriteWifi)
    {
        char * pWiFiAddr = m_sScanData.strWifiAddr;
        unsigned char tmpCh;

        MTRACE(g_hEBOOT_DEBUG, "Prod_Info->WiFiAddress = [%s]", m_sScanData.strWifiAddr);
        for (int i = 0; i < 6; i++)
        {
            // High 4Bit
            if ('0' <= pWiFiAddr[i * 2] && pWiFiAddr[i * 2] <= '9')
            {
                tmpCh = pWiFiAddr[i * 2] - '0';
                tmpCh = ((tmpCh << 4) & 0xf0u);
            }
            else if ('A' <= pWiFiAddr[i * 2] && pWiFiAddr[i * 2] <= 'F')
            {
                tmpCh = pWiFiAddr[i * 2] - 'A' + 10;
                tmpCh = ((tmpCh << 4) & 0xf0u);
            }

            // Low 4Bit
            if ('0' <= pWiFiAddr[(i * 2 + 1)] && pWiFiAddr[(i * 2 + 1)] <= '9')
            {
                tmpCh += pWiFiAddr[(i * 2 + 1)] - '0';
            }
            else if ('A' <= pWiFiAddr[(i * 2 + 1)] && pWiFiAddr[(i * 2 + 1)] <= 'F')
            {
                tmpCh += pWiFiAddr[(i * 2 + 1)] - 'A' + 10;
            }

            pstProInfo->target_info.WifiAddr[i] = tmpCh;
        }

        pstSnFlag->wifi = 1;
    }

    if (g_sMetaComm.sWriteOption.bWriteSerialNo)
    {
        MTRACE(g_hEBOOT_DEBUG, "Prod_Info->ADBSeriaNo = [%s]", m_sScanData.strSerialNo);
        strcpy_s((char *)pstProInfo->target_info.ADBSeriaNo, SERIAL_NO_BUF_LEN, m_sScanData.strSerialNo);
        pstProInfo->target_info.ADBSeriaNo[SERIAL_NO_LEN] = '\0';
    }

    if (g_sMetaComm.sWriteOption.bWriteMeid)
        pstSnFlag->MEID = 1;

    pstSnFlag->total_pass = 1;
    MTRACE(g_hEBOOT_DEBUG, "Prod_Info->sn_flag = [0x%02x%02x] Bitmap: (15~0) Reserve|MEID|Wifi|Bt|IMEI|Barcode|Pass",
           pstProInfo->mtk_test_flag.sn_write_flag.sn_write_flag[1],
           pstProInfo->mtk_test_flag.sn_write_flag.sn_write_flag[0]);

    if (g_sMetaComm.bClearMetaBootFlag)
    {
        MTRACE(g_hEBOOT_DEBUG, "Clear MetaBootFlag: mode %u, type %u, id %u",
               pstProInfo->mtk_boot_mode_flag.boot_mode,
               pstProInfo->mtk_boot_mode_flag.com_type,
               pstProInfo->mtk_boot_mode_flag.com_id);
        pstProInfo->mtk_boot_mode_flag.boot_mode = 0;
        pstProInfo->mtk_boot_mode_flag.com_type = 0;
        pstProInfo->mtk_boot_mode_flag.com_id = 0;
    }

    return META_SUCCESS;
}

//add by wzb 20190318

META_RESULT SmartPhoneSN::ReadSN_From_PRODINFO()
{
    META_RESULT meta_result = META_SUCCESS;
    
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ReadSN_From_PRODINFO(): start...");

    m_bWriteNvram = false;
    int iMetaTimeout = 5000;
    int iWriteBufSize = 0;
    char *pLID = "AP_CFG_REEB_PRODUCT_INFO_LID";
    char *pFuncName = NULL;

	pConductDataFunc pDataFunc = &SmartPhoneSN::ConductBarcodeDataNoSyncProdinfo;
    
    AP_FT_NVRAM_READ_REQ sNVRAM_ReadReq;
    AP_FT_NVRAM_READ_CNF sNVRAM_ReadCnf;

    memset(&sNVRAM_ReadReq, 0, sizeof(AP_FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(AP_FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    meta_result = SP_META_NVRAM_GetRecLen(pLID, &iWriteBufSize);
    if (  META_SUCCESS != meta_result)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString_SP(meta_result));
        return meta_result;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iWriteBufSize);
    if ( NULL != sNVRAM_ReadCnf.buf )
    {
        free(sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    sNVRAM_ReadReq.LID = pLID;
    sNVRAM_ReadReq.RID = 1;
    sNVRAM_ReadCnf.len = iWriteBufSize;
    sNVRAM_ReadCnf.buf = (unsigned char*) malloc(iWriteBufSize * sizeof(unsigned char));
    
    if (NULL == sNVRAM_ReadCnf.buf)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
        return  META_FAILED;
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromAPNVRAM(): Start to read nvram data...");
    
    memset(sNVRAM_ReadCnf.buf, 0 , sNVRAM_ReadCnf.len);
    meta_result = REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }
    else
    {
		char tmpReadData[64] = {0};
		char pReadData[64] = {0};
		memcpy(pReadData, sNVRAM_ReadCnf.buf, 64);
		(this->*pDataFunc)(tmpReadData, 1, pReadData, 64);

		if (strncmp(m_sScanData.strBarcode, tmpReadData, 64) != 0){
			MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: ScanSN[%s]", m_sScanData.strBarcode);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: ReadSN[%s]", tmpReadData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check SN data FAIL!!");
			meta_result = META_FAILED;
		}
		else
		{
		 	MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: ScanSN[%s]", m_sScanData.strBarcode);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: ReadSN[%s]", tmpReadData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check SN data PASS!!");

		}

    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data end...");

    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }


    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ReadSN_From_PRODINFO():  end...");
    return meta_result;

Err:
    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

   
    MTRACE_ERR (g_hEBOOT_DEBUG, "%s: fail! MetaResult = %s", pFuncName, ResultToString_SP(meta_result));
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ReadSN_From_PRODINFO():  end...");
    return meta_result;
}
//end

META_RESULT SmartPhoneSN::REQ_WriteAP_PRODINFO_Start()
{
    META_RESULT meta_result = META_SUCCESS;
    m_bWriteNvram = true;
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteAP_PRODINFO_Start(): Write Barcode/Imei/Bt/Wifi/Serial.No to prod_info start...");

    int iIMEINums = g_sMetaComm.sIMEIOption.iImeiNums;
    int iMetaTimeout = 5000;
    int iWriteBufSize = 0;
    char *pLID = "AP_CFG_REEB_PRODUCT_INFO_LID";
    char *pFuncName = NULL;
    unsigned char *pWriteData = NULL;

    AP_FT_NVRAM_WRITE_REQ sNVRAM_WriteReq;
    AP_FT_NVRAM_READ_REQ sNVRAM_ReadReq;
    AP_FT_NVRAM_READ_CNF sNVRAM_ReadCnf;

    // Check
    if (g_sMetaComm.sWriteOption.bWriteBarcode)
    {
        if (m_sScanData.strBarcode == NULL || strlen(m_sScanData.strBarcode) == 0)
        {
            return META_INVALID_ARGUMENTS;
        }
    }
    if (g_sMetaComm.sWriteOption.bWriteIMEI)
    {
        for (int i = 0; i < g_sMetaComm.sIMEIOption.iImeiNums; i++)
        {
            if (m_sScanData.strIMEI[i] == NULL || strlen(m_sScanData.strIMEI[i]) == 0)
            {
                MTRACE(g_hEBOOT_DEBUG, "i = %d", i);
                return META_INVALID_ARGUMENTS;
            }
        }
    }
    if (g_sMetaComm.sWriteOption.bWriteBT)
    {
        if (m_sScanData.strBTAddr == NULL || strlen(m_sScanData.strBTAddr) == 0)
        {
            return META_INVALID_ARGUMENTS;
        }
    }
    if (g_sMetaComm.sWriteOption.bWriteWifi)
    {
        if (m_sScanData.strWifiAddr == NULL || strlen(m_sScanData.strWifiAddr) == 0)
        {
            return META_INVALID_ARGUMENTS;
        }
    }
    if (g_sMetaComm.sWriteOption.bWriteSerialNo)
    {
        if (m_sScanData.strSerialNo == NULL || strlen(m_sScanData.strSerialNo) == 0)
        {
            return META_INVALID_ARGUMENTS;
        }
    }

    memset(&sNVRAM_WriteReq, 0, sizeof(AP_FT_NVRAM_WRITE_REQ));
    memset(&sNVRAM_ReadReq, 0, sizeof(AP_FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(AP_FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    meta_result = SP_META_NVRAM_GetRecLen(pLID, &iWriteBufSize);
    if (  META_SUCCESS != meta_result)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString_SP(meta_result));
        return meta_result;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iWriteBufSize);
    if ( NULL != sNVRAM_ReadCnf.buf )
    {
        free(sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    if (NULL != pWriteData)
    {
        free(pWriteData);
        pWriteData = NULL;
    }

    sNVRAM_ReadReq.LID = pLID;
    sNVRAM_ReadReq.RID = 1;
    sNVRAM_ReadCnf.len = iWriteBufSize;
    sNVRAM_ReadCnf.buf = (unsigned char*) malloc(iWriteBufSize * sizeof(unsigned char));
    pWriteData = (unsigned char*) malloc(iWriteBufSize * sizeof(unsigned char));
    if (NULL == sNVRAM_ReadCnf.buf || NULL == pWriteData)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
        return  META_FAILED;
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromAPNVRAM(): Start to read nvram data...");
    meta_result =  REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ConductProdInfoData(): Conduct Prod_Info nvram data start...");
    meta_result = ConductProdInfoData(sNVRAM_ReadCnf.buf, iWriteBufSize);
    if (meta_result != META_SUCCESS)
    {
        pFuncName = "SmartPhoneSN::ConductProdInfoData()";
        goto Err;
    }
    else
    {
        memcpy(pWriteData, sNVRAM_ReadCnf.buf, iWriteBufSize);
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ConductProdInfoData(): Conduct Prod_Info nvram data successfully!!");
    }

    sNVRAM_WriteReq.LID = pLID;
    sNVRAM_WriteReq.RID = 1;
    sNVRAM_WriteReq.len = iWriteBufSize;
    sNVRAM_WriteReq.buf = pWriteData;
    m_sNVRAM_OPID = 1;

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Start to write nvram data...");
    meta_result =  REQ_WriteToAPNVRAM(sNVRAM_WriteReq);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_WriteToAPNVRAM()";
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Write nvram data successfully!");

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data for check start...");
    memset(sNVRAM_ReadCnf.buf, 0 , sNVRAM_ReadCnf.len);
    meta_result = REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }
    else
    {
        if (memcmp(sNVRAM_ReadCnf.buf, pWriteData, sNVRAM_ReadCnf.len) != 0)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN: Check prod_info data FAIL!!");
            meta_result = META_FAILED;
        }
        else
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check prod_info data PASS!!");
        }
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data for check end...");

    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    if (pWriteData != NULL)
    {
        free (pWriteData);
        pWriteData = NULL;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteAP_PRODINFO_Start(): Write Barcode/Imei/Bt/Wifi/Serial.No to prod_info end...");
    return meta_result;

Err:
    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    if (pWriteData != NULL)
    {
        free (pWriteData);
        pWriteData = NULL;
    }

    MTRACE_ERR (g_hEBOOT_DEBUG, "%s: fail! MetaResult = %s", pFuncName, ResultToString_SP(meta_result));
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteAP_PRODINFO_Start(): Write Barcode/Imei/Bt/Wifi/Serial.No to prod_info end...");
    return meta_result;
}

int SmartPhoneSN::LoadHDCPBinFile(const char *strHdcpBinPath)
{
    int iRet = 0;
    int file_size = 0;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    FILE *fp = NULL;

    EN_HDCP_RET retHdcp = R_OK;
    LPWIN32_FIND_DATA file_data;

    char tempfile[1024] = {0};
    unsigned char bufNoEncrypt[HDCP_KEY_ARRAY_NUMBER_FILE] = {0};
    unsigned char bufEncrypt[HDCP_KEY_ARRAY_NUMBER] = {0};

    file_data = (WIN32_FIND_DATA*)malloc(sizeof(WIN32_FIND_DATA));
    sprintf_s(tempfile, "%s\\*.bin", strHdcpBinPath);
    hFile = FindFirstFile(tempfile, file_data);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        UpdateUIMsg("FAIL: Can`t found Hdcp Key File!!");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadHDCPBinFile(): Can`t found Hdcp Key File!!");
        iRet = -1;
        goto End;
    }

    sprintf_s(m_strHdcpFileFullName, "%s\\%s", strHdcpBinPath, file_data->cFileName);
    if (fopen_s(&fp, m_strHdcpFileFullName, "rb") != 0 || fp == NULL)
    {
        UpdateUIMsg("FAIL: Can`t Open Hdcp Key File!!");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadHDCPBinFile(): Can`t Open Hdcp Key File!!");
        iRet = -1;
        goto End;
    }
    fseek(fp, 0, SEEK_SET);
    fread(bufNoEncrypt, sizeof(char), 308, fp);
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    /*
    for( i = 0; i < HDCP_KEY_ARRAY_NUMBER_FILE; i++)
    {
    sprintf(strLog,"HDCP: HdcpProcess input bufNoEncrypt[%d]: 0x%2.2X.\n", i, bufNoEncrypt[i]&0xFF);
    }
    */

    retHdcp = HdcpProcess(bufNoEncrypt, bufEncrypt);

End:
    fclose(fp);
    FindClose( hFile );
    memcpy((char*)&m_sHDCPNvramStruct, bufEncrypt, sizeof(bufEncrypt));
    free (file_data);
    return iRet;
}

META_RESULT SmartPhoneSN::REQ_WriteHdcpBinToAPNvram_Start(const char *strHdcpBinPath)
{
    if (!strHdcpBinPath || !strlen(strHdcpBinPath))
    {
        return META_INVALID_ARGUMENTS;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteHdcpBinToAPNvram_Start(): Write Hdcp Bin start...");
    int i = 0;
    int iWriteBufSize = 0;
    int iReadBufSize = 0;
    char *pLID = "AP_CFG_RDCL_FILE_HDCP_KEY_LID";
    char *pFuncName = NULL;
    char* m_pcHDCPBuf = NULL;

    META_RESULT MetaResult = META_SUCCESS;
    AP_FT_NVRAM_WRITE_REQ sNVRAM_WriteReq;
    AP_FT_NVRAM_READ_REQ sNVRAM_ReadReq;
    AP_FT_NVRAM_READ_CNF sNVRAM_ReadCnf;
    memset(&sNVRAM_WriteReq, 0, sizeof(AP_FT_NVRAM_WRITE_REQ));
    memset(&m_sHDCPNvramStruct, 0, sizeof(m_sHDCPNvramStruct));
    memset(&sNVRAM_ReadReq, 0, sizeof(AP_FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(AP_FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);

    MetaResult = SP_META_NVRAM_GetRecLen ( pLID, &iWriteBufSize );
    if ( MetaResult != META_SUCCESS )
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString_SP(MetaResult));
        return MetaResult;
    }
    else
    {
        m_pcHDCPBuf = (char*)malloc(iWriteBufSize * sizeof(char));
        if (!m_pcHDCPBuf)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
            return META_FAILED;
        }
    }


    //load hdcp file
    if (LoadHDCPBinFile(strHdcpBinPath) == -1)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteHdcpBinToAPNvram_Start(): Write Hdcp Bin end...");
        goto End;
    }

    MetaResult = SP_META_NVRAM_Compose_HDCP(&m_sHDCPNvramStruct, m_pcHDCPBuf, iWriteBufSize);
    if (MetaResult != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_Compose_HDCP(): Compose hdcp data fail, MetaResult = %s", ResultToString_SP(MetaResult));
        goto End;
    }

    sNVRAM_WriteReq.LID = pLID;
    sNVRAM_WriteReq.RID = 1;
    sNVRAM_WriteReq.len = iWriteBufSize;
    sNVRAM_WriteReq.buf = (unsigned char *)m_pcHDCPBuf;

    MetaResult = REQ_WriteToAPNVRAM(sNVRAM_WriteReq);
    if (MetaResult != META_SUCCESS )
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Write hdcp key to nvram data Fail, MetaResult = %s", ResultToString_SP(MetaResult));
        goto End;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Write hdcp key to nvram data successfully!");

    //read hdcp 12.11 add by mtk71596
    iReadBufSize = iWriteBufSize;
    sNVRAM_ReadReq.LID = pLID;
    sNVRAM_ReadReq.RID = 1;
    sNVRAM_ReadCnf.len = iReadBufSize;
    sNVRAM_ReadCnf.buf = (unsigned char *)m_pcHDCPBuf;
    if (NULL == sNVRAM_ReadCnf.buf)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
        goto End;
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromAPNVRAM(): Start to read nvram data...");
    MetaResult =  REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (MetaResult != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto End;
    }
    MetaResult = SP_META_NVRAM_Decompose_HDCP(&m_sReadBackHdcp, m_pcHDCPBuf, iReadBufSize);

    for (i = 0; i < 287; i++)
    {
        if (m_sHDCPNvramStruct.HdcpKeyArray[i] != m_sReadBackHdcp.HdcpKeyArray[i])
        {
            MTRACE (g_hEBOOT_DEBUG, "Fail:The Key Read back is not the same as the write \n");
            goto End;
        }
    }

    remove(m_strHdcpFileFullName);
    MTRACE (g_hEBOOT_DEBUG, "OK: Write HDCP Key File\n");

End:
    if (m_pcHDCPBuf != NULL)
    {
        free (m_pcHDCPBuf);
        m_pcHDCPBuf = NULL;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteHdcpBinToAPNvram_Start(): Write Hdcp Bin end...");
    return MetaResult;
}

int SmartPhoneSN::LoadDRMKeyFile(char *szDrmKeyFile)
{
    HANDLE hFileEnum = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA stFileData = {0};
    BOOL bFindContinue = TRUE;

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): enum drmkey file start...");

    if (g_sMetaComm.sLoadFile.strDrmKeyPath == '\0')
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): invalide drmkey folder.");
        return -1;
    }

    ::PathCombine(szDrmKeyFile, g_sMetaComm.sLoadFile.strDrmKeyPath, "*");
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): pattern %s.", szDrmKeyFile);

    hFileEnum = ::FindFirstFile(szDrmKeyFile, &stFileData);
    bFindContinue = (hFileEnum != INVALID_HANDLE_VALUE);
    for (; bFindContinue == TRUE; bFindContinue = ::FindNextFile(hFileEnum, &stFileData))
    {
        if ((stFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            continue;

        ::PathCombine(szDrmKeyFile, g_sMetaComm.sLoadFile.strDrmKeyPath, stFileData.cFileName);
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): enum file %s.", szDrmKeyFile);

        // only get the first one
        break;
    }
    ::FindClose(hFileEnum);
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): enum drmkey file end.");

    return (bFindContinue == TRUE) ? 0 : -2;
}

int SmartPhoneSN::RecycleDRMKeyFile(const char *szDrmKeyFile)
{
    char szRecylceFolder[MAX_PATH] = {0};

    ::PathCombine(szRecylceFolder, g_sMetaComm.sLoadFile.strDrmKeyPath, "Recycle\\");
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): recycle folder %s.", szRecylceFolder);
    if (::PathFileExists(szRecylceFolder) == FALSE)
    {
        MTRACE_WARN(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): recycle folder not exist, don't recycle key file.");
        return 0;
        // if (::CreateDirectory(szRecylceFolder, NULL) == FALSE)
        // {
        //     DWORD dwErrCode = ::GetLastError();
        //     ResultToString_Win(dwErrCode, szRecylceFolder, MAX_PATH);
        //     MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): create recycle folder fail(%s).", szRecylceFolder);
        //     return -1;
        // }
        // MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): create recycle folder ok.");
    }

    ::PathAppend(szRecylceFolder, ::PathFindFileName(szDrmKeyFile));
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): new file path %s.", szRecylceFolder);
    if (::MoveFile(szDrmKeyFile, szRecylceFolder) == FALSE)
    {
        DWORD dwErrCode = ::GetLastError();
        ResultToString_Win(dwErrCode, szRecylceFolder, MAX_PATH);
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): move keyfile to recycle folder fail(%s).", szRecylceFolder);
        return -2;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): move keyfile to recycle folder ok.");

    return 0;
}

META_RESULT SmartPhoneSN::REQ_WriteDRMKeyMCID_Start(const char *strDrmKeyMCIDData)
{
    if (!strDrmKeyMCIDData || strlen(strDrmKeyMCIDData) != DRMKEY_MCID_LENGTH)
    {
        return META_INVALID_ARGUMENTS;
    }

    META_RESULT MetaResult = META_SUCCESS;
    DRMKEY_WRITE_MCID_REQ pWriteReq;
    DRMKEY_WRITE_MCID_CNF pWriteCnf;
    DRMKEY_READ_MCID_REQ pReadReq;
    DRMKEY_READ_MCID_CNF pReadCnf;

    memset(&pWriteReq, 0, sizeof(DRMKEY_WRITE_MCID_REQ));
    memset(&pWriteCnf, 0, sizeof(DRMKEY_WRITE_MCID_CNF));
    memset(&pReadReq, 0, sizeof(DRMKEY_READ_MCID_REQ));
    memset(&pReadCnf, 0, sizeof(DRMKEY_READ_MCID_CNF));

    pWriteReq.keylength = DRMKEY_MCID_LENGTH;
    memcpy(pWriteReq.content, strDrmKeyMCIDData, DRMKEY_MCID_LENGTH);
    pWriteReq.content[DRMKEY_MCID_LENGTH] = '\0';
    MTRACE(g_hEBOOT_DEBUG, "DRMKeyMCID = [%s]", pWriteReq.content);

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Write_MCID_r(): Write DRMKeyMCID = [%s] to target start...", pWriteReq.content);
    MetaResult = SP_META_DRMKey_Write_MCID_r(m_hSPMetaHandle, 5000, &pWriteReq, &pWriteCnf);
    if (MetaResult != META_SUCCESS)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Write_MCID_r(): Write DRMKeyMCID Fail, MetaResult = %s", ResultToString_SP(MetaResult));
        return MetaResult;
    }
    else
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Write_MCID_r(): Write DRMKeyMCID successfully!!");

        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Read_MCID_r(): Read DRMKeyMCID from target for check start...");
        MetaResult = SP_META_DRMKey_Read_MCID_r(m_hSPMetaHandle, 5000, &pReadReq, &pReadCnf);
        if (MetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Read_MCID_r(): Read DRMKeyMCID Fail, MetaResult = %s", ResultToString_SP(MetaResult));
            return MetaResult;
        }
        else
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_DRMKeyMCID[%s]", pWriteReq.content);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_DRMKeyMCID[%s]", pReadCnf.content);

            if (memcmp(pReadCnf.content, pWriteReq.content, DRMKEY_MCID_LENGTH) != 0)
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN: Check DRMKeyMCID fail!! ");
                return META_FAILED;
            }
            else
            {
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check DRMKeyMCID Pass!!");
            }
        }
    }

    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::REQ_ReadDRMKeyMCID_Start(char *strDrmKeyMCIDData)
{
    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::REQ_InstallDRMKey_Start()
{
    int iRet = 0;

    char szKeyFile[MAX_PATH] = "";
    META_RESULT MetaResult = META_SUCCESS;
    DRMKEY_INSTALL_QUERY_REQ Req;
    DRMKEY_INSTALL_SET_CNF pCnf;
    DRMKEY_INSTALL_QUERY_CNF QueCnf;

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallDRMKey_Start(): start...");

    memset(&Req, 0, sizeof(DRMKEY_INSTALL_QUERY_REQ));
    memset(&QueCnf, 0, sizeof(DRMKEY_INSTALL_QUERY_CNF));
    memset(&pCnf, 0, sizeof(DRMKEY_INSTALL_SET_CNF));

    iRet = LoadDRMKeyFile(szKeyFile);
    if (iRet != 0)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): load drmkey file fail.");
        return META_FAILED;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadDRMKeyFile(): load drmkey file %s.", szKeyFile);

    MetaResult =  SP_META_DRMKey_Install_Set_r(m_hSPMetaHandle, 10000, szKeyFile, &pCnf);
    if (MetaResult != META_SUCCESS || pCnf.result != 0)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Install_Set_r(): drmkey install fail, result %u, %s.",
                   pCnf.result, ResultToString_SP(MetaResult));
        return MetaResult;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Install_Set_r(): drmkey install ok.");

    MetaResult = SP_META_DRMKey_Install_Query_r(m_hSPMetaHandle, 5000, &Req, &QueCnf);
    if (MetaResult != META_SUCCESS)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Install_Query_r(): fail %s.", ResultToString_SP(MetaResult));
        return MetaResult;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_DRMKey_Install_Query_r(): keycount %d.", QueCnf.keycount);
    if (QueCnf.keycount == 0)
        return META_FAILED;

    iRet = RecycleDRMKeyFile(szKeyFile);
    if (iRet != 0)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): recycle drmkey fail.");
        return META_FAILED;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::RecycleDRMKeyFile(): recycle drmkey ok.");

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallDRMKey_Start(): end.");
    return MetaResult;
}

META_RESULT SmartPhoneSN::REQ_InstallHdcpData_Start(const char *strHdcpDataFilePath, const char *strHdcpCekFilePath)
{
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallHdcpData_Start(): Install Hdcp data start...");
    META_RESULT MetaResult = META_SUCCESS;
    HDCP_INSTALL_CNF Cnf;
    memset(&Cnf, 0, sizeof(HDCP_INSTALL_CNF));

    if (!strlen(strHdcpDataFilePath) || !strlen(strHdcpCekFilePath))
    {
        UpdateUIMsg("Error: Invalid HDCP file path");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallHdcpData_Start(): Invalid HDCP file path");

        MetaResult = META_INVALID_ARGUMENTS;
        goto End;
    }
    Sleep(2000);
    MetaResult  =  SP_META_HDCP_Install_r(m_hSPMetaHandle, 5000, strHdcpDataFilePath, strHdcpCekFilePath, &Cnf);
    if ((MetaResult == META_SUCCESS) &&  (Cnf.install_result == 0))
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallHdcpData_Start(): Install HDCP data successfully!!");
    }
    else
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallHdcpData_Start(): Install HDCP data Fail, MetaResult = %s", ResultToString_SP(MetaResult));
    }
    Sleep(2000);
End:
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallHdcpData_Start(): Install Hdcp data end...");
    return MetaResult;
}

META_RESULT SmartPhoneSN::REQ_InstallAttestationKey_Start(const char * strKeyFile)
{
    META_RESULT MetaResult = META_SUCCESS;
    ATTESTATIONKEY_INSTALL_SET_CNF Cnf;
    memset(&Cnf, 0, sizeof(ATTESTATIONKEY_INSTALL_SET_CNF));

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallAttestationKey_Start(): Install attestation key start...");
    if (strKeyFile == NULL || strKeyFile[0] == '\0')
    {
        UpdateUIMsg("Error: Invalid attestation key path.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallAttestationKey_Start(): Invalid attestation key path");

        MetaResult = META_INVALID_ARGUMENTS;
        goto End;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallAttestationKey_Start(): key file \"%s\".", strKeyFile);

    MetaResult = SP_META_AttestationKey_Install_Set_r(m_hSPMetaHandle, 5000, strKeyFile, &Cnf);
    if (MetaResult != META_SUCCESS || Cnf.result != 0)
    {
        UpdateUIMsg("ERROR!! Install attestation key fail, %s", ResultToString_SP(MetaResult));
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallAttestationKey_Start(): Install attestation key fail, MetaResult = %s", ResultToString_SP(MetaResult));
    }

End:
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_InstallAttestationKey_Start(): Install attestation key end.");
    return MetaResult;
}

META_RESULT SmartPhoneSN::REQ_BackupNvram2PC_Start(char* NumFile)
{
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_BackupNvram2PC_Start() : Backup nvram to PC start....");
    UpdateUIMsg("Start backup nvram to PC...");
    META_RESULT spMetaResult = META_SUCCESS;
    char FolderPath[MAX_PATH];
    char FileFullPath[MAX_PATH];

    char MapFile[MAX_PATH];
    char DatFile[MAX_PATH];

    memset(MapFile, 0, sizeof(MapFile));
    memset(DatFile, 0, sizeof(DatFile));

    GetModuleFileName(NULL, FolderPath, MAX_PATH);
    (_tcsrchr(FolderPath, _T('\\')))[1] = 0;
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_BackupNvram2PC_Start() : GetModuleFileName folder path = %s ....", FolderPath);

    sprintf_s(FileFullPath, "%s\\%s", FolderPath, "BackupNvData");

    if (!PathFileExists(FileFullPath))
    {
        if (!CreateDirectory(FileFullPath, NULL))
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_BackupNvram2PC_Start() : Can`t create  backup nvram files folder path = %s ....", FileFullPath);
            return META_FAILED;
        }
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_BackupNvram2PC_Start() : The backup nvram files folder path = %s ....", FileFullPath);

    char temp1[10] = ".map";
    char temp2[10] = ".dat";


    sprintf_s(MapFile, "%s\\%s%s", FileFullPath, NumFile, temp1);
    MTRACE(g_hEBOOT_DEBUG, "debug!!!SmartPhoneSN::REQ_BackupNvram2PC_Start() : MapFile files folder path = %s ....", MapFile);


    sprintf_s(DatFile, "%s\\%s%s", FileFullPath, NumFile, temp2);
    MTRACE(g_hEBOOT_DEBUG, "debug!!!SmartPhoneSN::REQ_BackupNvram2PC_Start() : DatFile files folder path = %s ....", DatFile);
    spMetaResult = SP_META_Nvram_Backup_r( m_hSPMetaHandle, 15000, MapFile, DatFile);
    if (spMetaResult == META_SUCCESS)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_Nvram_Backup_r() : Backup nvram to PC successfully!!");
    }
    else
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_Nvram_Backup_r() : Backup nvram to PC Fail!! MetaResult = %s", ResultToString_SP(spMetaResult));
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_BackupNvram2PC_Start() : Backup nvram to PC end...");

    return spMetaResult;
}

META_RESULT SmartPhoneSN::REQ_WriteToAPNVRAM(AP_FT_NVRAM_WRITE_REQ sNVRAM_WriteReq)
{
    META_RESULT MetaResult;
    DWORD wait_result;
    short iNVRAM_OPID;

    m_hWriteToNVRAMEvent = CreateEvent(NULL, true, false, NULL);
    ResetEvent(m_hWriteToNVRAMEvent);

    MetaResult = SP_META_NVRAM_Write_r(m_hSPMetaHandle,
                 &sNVRAM_WriteReq,
                 CNF_SPWriteToNVRAM,
                 &iNVRAM_OPID,
                 (void*)&m_hWriteToNVRAMEvent);

    wait_result = WaitForSingleObject(m_hWriteToNVRAMEvent, 15000);

    if (WAIT_TIMEOUT == wait_result)
    {
        return META_TIMEOUT;
    }
    else if (WAIT_OBJECT_0)
    {
        CloseHandle (m_hWriteToNVRAMEvent);
    }

    if ( MetaResult != META_SUCCESS )
    {
        return MetaResult;
    }

    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::REQ_ReadAP_NVRAM_Start(WriteData_Type_e dataType, char *pOutData, unsigned short iRID)
{
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadAP_NVRAM_Start()...");
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Read DataType is = %d", dataType);

    META_RESULT MetaResult = META_SUCCESS;
    int iBufLen = 0;
    char *pLID = NULL;

    switch (dataType)
    {
    case WRITE_BARCODE:
    case WRITE_IMEI:
        pLID = "AP_CFG_REEB_PRODUCT_INFO_LID";
        break;

    default:
        return META_INVALID_ARGUMENTS;
    }

    AP_FT_NVRAM_READ_REQ sNVRAM_ReadReq;
    AP_FT_NVRAM_READ_CNF sNVRAM_ReadCnf;
    META_RESULT meta_result;
    int iWriteBufSize = 0;
    memset(&sNVRAM_ReadReq, 0, sizeof(AP_FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(AP_FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    meta_result = SP_META_NVRAM_GetRecLen(pLID, &iWriteBufSize);
    if (  META_SUCCESS != meta_result)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString_SP(meta_result));
        return meta_result;
    }
    else
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iWriteBufSize);
        if ( NULL != sNVRAM_ReadCnf.buf )
        {
            free ( sNVRAM_ReadCnf.buf );
            sNVRAM_ReadCnf.buf = NULL;
        }

        sNVRAM_ReadReq.LID = pLID;
        sNVRAM_ReadReq.RID = iRID;
        sNVRAM_ReadCnf.len = iWriteBufSize;
        sNVRAM_ReadCnf.buf = (unsigned char*) malloc (iWriteBufSize * sizeof(unsigned char));
        if (NULL == sNVRAM_ReadCnf.buf)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
            return  META_FAILED;
        }
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromAPNVRAM(): Start to read nvram data...");
    meta_result =  REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);

    if (meta_result != META_SUCCESS )
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "REQ_ReadFromAPNVRAM: fail! MetaResult = %s", ResultToString_SP(meta_result));
        return MetaResult;
    }

    if (dataType == WRITE_BARCODE)
    {
        // if(!strcmp(sNVRAM_ReadCnf.buf,"000000000000"))
        {
            int i = 0;
            for (i = 0; i < 64; i++)
            {
                if (!((sNVRAM_ReadCnf.buf[i] >= 'a' && sNVRAM_ReadCnf.buf[i] <= 'z')
                        || (sNVRAM_ReadCnf.buf[i] > 'A' && sNVRAM_ReadCnf.buf[i] < 'Z')
                        || (sNVRAM_ReadCnf.buf[i] >= '0' && sNVRAM_ReadCnf.buf[i] <= '9')))
                {
                    break;
                }
                else
                {
                    pOutData[i] = sNVRAM_ReadCnf.buf[i];
                }
            }

            pOutData[i] = '\0';
        }
    }

    if (dataType == WRITE_IMEI)
    {
        int i = 0;
        for (; i < 8; i++)
        {
            if ((sNVRAM_ReadCnf.buf[64 + i] & 15) >= '0' &&
                (sNVRAM_ReadCnf.buf[64 + i] & 15) <= '9' &&
                ((sNVRAM_ReadCnf.buf[64 + i] >> 4) & 15) >= '0' &&
                ((sNVRAM_ReadCnf.buf[64 + i] >> 4) & 240) <= '9')
            {
                break;
            }
            else
            {
                pOutData[2 * i] = (sNVRAM_ReadCnf.buf[64 + i] & 15 ) + '0';
            }

            pOutData[2 * i + 1] = (sNVRAM_ReadCnf.buf[64 + i] >> 4 & 15) + '0';
        }

        pOutData[2 * i - 1] = '\0';
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadAP_NVRAM_Start() Success...");
    return MetaResult;
}

//add by wzb 20190319
META_RESULT SmartPhoneSN::REQ_CountryCode_WriteAP_NVRAM_Start(char *pInData, unsigned short iRID)
{
    if (!pInData)
    {
        return META_INVALID_ARGUMENTS;
    }
    m_bWriteNvram = true;

    int iBufLen = 0;
    char *pLID = NULL;
    pConductDataFunc pDataFunc = NULL;


	//for country code
	iBufLen = 2;
    pLID = "AP_CFG_RDEB_FILE_WIFI_LID";
    pDataFunc = &SmartPhoneSN::ConductCountryCodeData;

   
    const int MAX_DATA_LENGTH = 2;
    int iMetaTimeout = 5000;

    AP_FT_NVRAM_WRITE_REQ sNVRAM_WriteReq;
    AP_FT_NVRAM_READ_REQ sNVRAM_ReadReq;
    AP_FT_NVRAM_READ_CNF sNVRAM_ReadCnf;
    META_RESULT meta_result;
    char pWriteData[MAX_DATA_LENGTH] = {0};
    char pReadData[MAX_DATA_LENGTH] = {0};
    int iWriteBufSize = 0;
    unsigned long wifiChipVersion = 0;
    char *pFuncName = NULL;
    int rs = 0;

    memset(&sNVRAM_WriteReq, 0, sizeof(AP_FT_NVRAM_WRITE_REQ));
    memset(&sNVRAM_ReadReq, 0, sizeof(AP_FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(AP_FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    meta_result = SP_META_NVRAM_GetRecLen(pLID, &iWriteBufSize);
    if (  META_SUCCESS != meta_result)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString_SP(meta_result));
        return meta_result;
    }
    else
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iWriteBufSize);
        if ( NULL != sNVRAM_ReadCnf.buf )
        {
            free ( sNVRAM_ReadCnf.buf );
            sNVRAM_ReadCnf.buf = NULL;
        }

        sNVRAM_ReadReq.LID = pLID;
        sNVRAM_ReadReq.RID = iRID;
        sNVRAM_ReadCnf.len = iWriteBufSize;
        sNVRAM_ReadCnf.buf = (unsigned char*) malloc (iWriteBufSize * sizeof(unsigned char));
        if (NULL == sNVRAM_ReadCnf.buf)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
            return  META_FAILED;
        }
    }


    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromAPNVRAM(): Start to read nvram data...");
    meta_result =  REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }


   
    MTRACE (g_hEBOOT_DEBUG, "SNBase::ConductCountryCodeData(): Start Conduct wifi nvram data...!");
    pFuncName = "SNBase::ConductCountryCodeData()";
    rs = (this->*pDataFunc)((char*)sNVRAM_ReadCnf.buf + 0xa, iRID, pInData, iBufLen);
    for (int i = 0; i < iBufLen; i++)
    {
        pWriteData[i] = sNVRAM_ReadCnf.buf[0xa + i];
    }
    
 
    meta_result = (META_RESULT)rs;
    if (meta_result !=  META_SUCCESS)
    {
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "%s: Conduct nvram data successfully!", pFuncName);

    sNVRAM_WriteReq.LID = pLID;
    sNVRAM_WriteReq.RID = iRID;
    sNVRAM_WriteReq.len = iWriteBufSize;
    sNVRAM_WriteReq.buf = sNVRAM_ReadCnf.buf;
    m_sNVRAM_OPID = iRID;

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_CountryCode_WriteAP_NVRAM(): Start to write nvram data...");
    meta_result =  REQ_WriteToAPNVRAM(sNVRAM_WriteReq);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_WriteToAPNVRAM()";
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Write nvram data successfully!");

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data for check start...");
    memset(sNVRAM_ReadCnf.buf, 0 , sNVRAM_ReadCnf.len);
    meta_result = REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }
    else
    {
        char *pDataType = "Country Code";
        memset(pReadData, 0, sizeof(char)*MAX_DATA_LENGTH);
        
        //countrycode address offset is 0xa
        for (int i = 0; i < iBufLen; i++)
        {
           pReadData[i] = sNVRAM_ReadCnf.buf[0xa + i];
        }
        
        
        char tmpReadData[5] = {0};
        m_bWriteNvram = false;
        (this->*pDataFunc)(tmpReadData, iRID, pReadData, iBufLen);

        if (strncmp(pWriteData, pReadData, iBufLen) != 0)
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_%s[%s]", pDataType, pInData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_%s[%s]", pDataType, tmpReadData);
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN: Check nvram data FAIL!!");
            meta_result = META_FAILED;
        }
        else
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_%s[%s]", pDataType, pInData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_%s[%s]", pDataType, tmpReadData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check nvram data PASS!!");
        }
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data for check end...");

    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    return meta_result;

Err:
    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    MTRACE_ERR (g_hEBOOT_DEBUG, "%s: fail! MetaResult = %s", pFuncName, ResultToString_SP(meta_result));
    return meta_result;
}


//end

META_RESULT SmartPhoneSN::REQ_WriteAP_NVRAM_Start(WriteData_Type_e dataType, char *pInData, unsigned short iRID)
{
    if (!pInData)
    {
        return META_INVALID_ARGUMENTS;
    }
    m_bWriteNvram = true;

    int iBufLen = 0;
    char *pLID = NULL;
    pConductDataFunc pDataFunc = NULL;

    switch (dataType)
    {
    case WRITE_BT:
        iBufLen = 6;
        pLID = "AP_CFG_RDEB_FILE_BT_ADDR_LID";
        pDataFunc = &SmartPhoneSN::ConductBTAddrData;
        break;

    case WRITE_WIFI:
        iBufLen = 6;
        pLID = "AP_CFG_RDEB_FILE_WIFI_LID";
        pDataFunc = &SmartPhoneSN::ConductWifiAddrData;
        break;

    case WRITE_ETHERNET_MAC:
        iBufLen = 6;
        pLID = "AP_CFG_RDEB_FILE_ETHERNET_LID";
        pDataFunc = &SmartPhoneSN::ConductEthernetMacAddrData;
        break;

    default:
        return META_INVALID_ARGUMENTS;
    }

    const int MAX_DATA_LENGTH = 6;
    int iMetaTimeout = 5000;

    AP_FT_NVRAM_WRITE_REQ sNVRAM_WriteReq;
    AP_FT_NVRAM_READ_REQ sNVRAM_ReadReq;
    AP_FT_NVRAM_READ_CNF sNVRAM_ReadCnf;
    META_RESULT meta_result;
    char pWriteData[MAX_DATA_LENGTH] = {0};
    char pReadData[MAX_DATA_LENGTH] = {0};
    int iWriteBufSize = 0;
    unsigned long wifiChipVersion = 0;
    char *pFuncName = NULL;
    int rs = 0;

    memset(&sNVRAM_WriteReq, 0, sizeof(AP_FT_NVRAM_WRITE_REQ));
    memset(&sNVRAM_ReadReq, 0, sizeof(AP_FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(AP_FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    meta_result = SP_META_NVRAM_GetRecLen(pLID, &iWriteBufSize);
    if (  META_SUCCESS != meta_result)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString_SP(meta_result));
        return meta_result;
    }
    else
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SP_META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iWriteBufSize);
        if ( NULL != sNVRAM_ReadCnf.buf )
        {
            free ( sNVRAM_ReadCnf.buf );
            sNVRAM_ReadCnf.buf = NULL;
        }

        sNVRAM_ReadReq.LID = pLID;
        sNVRAM_ReadReq.RID = iRID;
        sNVRAM_ReadCnf.len = iWriteBufSize;
        sNVRAM_ReadCnf.buf = (unsigned char*) malloc (iWriteBufSize * sizeof(unsigned char));
        if (NULL == sNVRAM_ReadCnf.buf)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
            return  META_FAILED;
        }
    }


    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromAPNVRAM(): Start to read nvram data...");
    meta_result =  REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }


    if (strcmp(pLID, "AP_CFG_RDEB_FILE_WIFI_LID") == 0)
    {
        MTRACE (g_hEBOOT_DEBUG, "SNBase::ConductWifiAddrData(): Start Conduct wifi nvram data...!");
        pFuncName = "SNBase::ConductWifiAddrData()";
        rs = (this->*pDataFunc)((char*)sNVRAM_ReadCnf.buf + 0x4, iRID, pInData, iBufLen);
        for (int i = 0; i < iBufLen; i++)
        {
            pWriteData[i] = sNVRAM_ReadCnf.buf[0x4 + i];
        }
    }
    else if (strcmp(pLID, "AP_CFG_RDEB_FILE_BT_ADDR_LID") == 0)
    {
        MTRACE (g_hEBOOT_DEBUG, "SNBase::ConductBTAddrData(): Start Conduct BT nvram data...!");
        pFuncName = "SNBase::ConductBTAddrData()";
        rs = (this->*pDataFunc)((char*)sNVRAM_ReadCnf.buf, iRID, pInData, iBufLen);
        for (int i = 0; i < iBufLen; i++)
        {
            pWriteData[i] = sNVRAM_ReadCnf.buf[i];
        }
    }
    else if (strcmp(pLID, "AP_CFG_RDEB_FILE_ETHERNET_LID") == 0)
    {
        MTRACE (g_hEBOOT_DEBUG, "SNBase::ConductEthernetMacAddrData(): Start Conduct Ethernet Mac nvram data...!");
        pFuncName = "SNBase::ConductEthernetMacAddrData()";
        rs = (this->*pDataFunc)((char*)sNVRAM_ReadCnf.buf, iRID, pInData, iBufLen);
        for (int i = 0; i < iBufLen; i++)
        {
            pWriteData[i] = sNVRAM_ReadCnf.buf[i];
        }
    }

    meta_result = (META_RESULT)rs;
    if (meta_result !=  META_SUCCESS)
    {
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "%s: Conduct nvram data successfully!", pFuncName);

    sNVRAM_WriteReq.LID = pLID;
    sNVRAM_WriteReq.RID = iRID;
    sNVRAM_WriteReq.len = iWriteBufSize;
    sNVRAM_WriteReq.buf = sNVRAM_ReadCnf.buf;
    m_sNVRAM_OPID = iRID;

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Start to write nvram data...");
    meta_result =  REQ_WriteToAPNVRAM(sNVRAM_WriteReq);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_WriteToAPNVRAM()";
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Write nvram data successfully!");

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data for check start...");
    memset(sNVRAM_ReadCnf.buf, 0 , sNVRAM_ReadCnf.len);
    meta_result = REQ_ReadFromAPNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromAPNVRAM()";
        goto Err;
    }
    else
    {
        char *pDataType = NULL;
        memset(pReadData, 0, sizeof(char)*MAX_DATA_LENGTH);
        if (strcmp(pLID, "AP_CFG_RDEB_FILE_WIFI_LID") == 0)
        {
            pDataType = "Wifi";
            //Wifi address offset is 0x4
            for (int i = 0; i < iBufLen; i++)
            {
                pReadData[i] = sNVRAM_ReadCnf.buf[0x4 + i];
            }
        }
        else if (strcmp(pLID, "AP_CFG_RDEB_FILE_BT_ADDR_LID") == 0)
        {
            pDataType = "BT";
            for (int i = 0; i < iBufLen; i++)
            {
                pReadData[i] = sNVRAM_ReadCnf.buf[i];
            }
        }
        else if (strcmp(pLID, "AP_CFG_RDEB_FILE_ETHERNET_LID") == 0)
        {
            pDataType = "Ethernet_Mac";
            for (int i = 0; i < iBufLen; i++)
            {
                pReadData[i] = sNVRAM_ReadCnf.buf[i];
            }
        }

        char tmpReadData[13] = {0};
        m_bWriteNvram = false;
        (this->*pDataFunc)(tmpReadData, iRID, pReadData, iBufLen);

        if (strncmp(pWriteData, pReadData, iBufLen) != 0)
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_%s[%s]", pDataType, pInData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_%s[%s]", pDataType, tmpReadData);
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN: Check nvram data FAIL!!");
            meta_result = META_FAILED;
        }
        else
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_%s[%s]", pDataType, pInData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_%s[%s]", pDataType, tmpReadData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check nvram data PASS!!");
        }
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToAPNVRAM(): Read nvram data for check end...");

    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    return meta_result;

Err:
    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    MTRACE_ERR (g_hEBOOT_DEBUG, "%s: fail! MetaResult = %s", pFuncName, ResultToString_SP(meta_result));
    return meta_result;
}

META_RESULT SmartPhoneSN::SwitchMDByIndex(UINT MDIndex)
{
    if (m_bDSDAProject)
    {
        m_bInitExtMDdb = true;
        MDIndex = m_iMDChannelIndex[MDIndex];
        m_iCurMDChanelIndex = MDIndex;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SwitchMDByIndex(): Switch to MD[%d] start...", MDIndex);

    META_RESULT MetaResult;
    char *pFuncName = NULL;

    if (m_bDSDAProject)
    {
        pFuncName = "META_SwitchCurrentModemEx_r()";
        MetaResult = META_SwitchCurrentModemEx_r(m_hMauiMetaHandle, 10000, MDIndex,
                     m_SpMdCapList.modem_cap[MDIndex].md_service, m_SpMdCapList.modem_cap[MDIndex].ch_type, NULL, NULL);
    }
    else
    {
        pFuncName = "META_SwitchCurrentModem_r()";
        MetaResult = META_SwitchCurrentModem_r(m_hMauiMetaHandle, 5000, MDIndex);
    }

    if (MetaResult == META_SUCCESS)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::%s: Switch to MD[%d] successfully!!", pFuncName, MDIndex);
        /*
        if(MDIndex >= 1)
        {
        MetaResult = LoadModemDatabase(MDIndex);
        }
        */
    }
    else
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::%s: Switch to MD[%d] Fail, MetaResult = %s!!",
                   pFuncName, MDIndex, ResultToString(MetaResult));
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SwitchMDByIndex(): Switch to MD[%d] end...", MDIndex);
    return MetaResult;
}

META_RESULT SmartPhoneSN::REQ_ReadModem_NVRAM_Start(WriteData_Type_e dataType, char *pOutData, unsigned short iRID)
{

    if (!pOutData)
    {
        return META_INVALID_ARGUMENTS;
    }
    m_bWriteNvram = false;

    int iBufLen = 0;
    char *pLID = NULL;
    pConductDataFunc pDataFunc = NULL;
    char *pFuncName = NULL;
    char *pDataType = NULL;

    switch (dataType)
    {
    case WRITE_BARCODE:
        iBufLen = 64;
        pDataType = "Barcode";
        pLID = "NVRAM_EF_BARCODE_NUM_LID";
        pDataFunc = &SmartPhoneSN::ConductBarcodeData;
        pFuncName = "SmartPhoneSN::ConductBarcodeData()";
        break;

    case WRITE_IMEI:
        iBufLen = 10;
        pDataType = "IMEI";
        pLID = "NVRAM_EF_IMEI_IMEISV_LID";
        pDataFunc = &SmartPhoneSN::ConductIMEIData;
        pFuncName = "SmartPhoneSN::ConductIMEIData()";
        break;

    default:
        return META_INVALID_ARGUMENTS;
    }

    META_RESULT MetaResult = META_SUCCESS;

    int iMetaTimeout  = 5000;
    int iReadBufSize = 0;
    FT_NVRAM_READ_REQ  sNVRAM_ReadReq;
    FT_NVRAM_READ_CNF  sNVRAM_ReadCnf;


    memset(&sNVRAM_ReadReq, 0, sizeof(FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    MetaResult = META_NVRAM_GetRecLen(pLID, &iReadBufSize);
    if ( META_SUCCESS != MetaResult)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString(MetaResult));
        return MetaResult;
    }
    else
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iReadBufSize);

        if ( NULL != sNVRAM_ReadCnf.buf )
        {
            free ( sNVRAM_ReadCnf.buf );
            sNVRAM_ReadCnf.buf = NULL;
        }

        sNVRAM_ReadReq.LID = pLID;
        sNVRAM_ReadReq.RID = iRID;
        sNVRAM_ReadCnf.len = iReadBufSize;
        sNVRAM_ReadCnf.buf = (unsigned char*) malloc (iReadBufSize * sizeof(unsigned char));
        if (NULL == sNVRAM_ReadCnf.buf)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
            return  META_FAILED;
        }
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromModemNVRAM(): Start to read nvram data...");
    MetaResult =  REQ_ReadFromModemNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (MetaResult != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromModemNVRAM()";
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromModemNVRAM(): Read nvram data successfully!");

    memcpy(pOutData, sNVRAM_ReadCnf.buf, iReadBufSize);

    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    return MetaResult;

Err:
    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    MTRACE_ERR (g_hEBOOT_DEBUG, "%s: fail! MetaResult = %s", pFuncName, ResultToString(MetaResult));

    return MetaResult;

}

META_RESULT SmartPhoneSN::REQ_WriteModem_NVRAM_Start(WriteData_Type_e dataType, char *pInData, unsigned short iRID)
{
    if (!pInData || strlen(pInData) == 0)
    {
        return META_INVALID_ARGUMENTS;
    }
    m_bWriteNvram = true;

    int iBufLen = 0;
    char *pLID = NULL;
    pConductDataFunc pDataFunc = NULL;
    char *pFuncName = NULL;
    char pWriteData[64] = {0};
    char pReadData[64] = {0};
    char *pDataType = NULL;

    switch (dataType)
    {
    case WRITE_BARCODE:
        iBufLen = 64;
        pDataType = "Barcode";
        pLID = "NVRAM_EF_BARCODE_NUM_LID";
        pDataFunc = &SmartPhoneSN::ConductBarcodeData;
        pFuncName = "SmartPhoneSN::ConductBarcodeData()";
        break;

    case WRITE_IMEI:
        iBufLen = 10;
        pDataType = "IMEI";
        pLID = "NVRAM_EF_IMEI_IMEISV_LID";
        pDataFunc = &SmartPhoneSN::ConductIMEIData;
        pFuncName = "SmartPhoneSN::ConductIMEIData()";
        break;

    default:
        return META_INVALID_ARGUMENTS;
    }


    int iMetaTimeout  = 5000;
    int iWriteBufSize = 0;
    FT_NVRAM_WRITE_REQ sNVRAM_WriteReq;
    FT_NVRAM_WRITE_CNF sNVRAM_WriteCnf;
    FT_NVRAM_READ_REQ  sNVRAM_ReadReq;
    FT_NVRAM_READ_CNF  sNVRAM_ReadCnf;
    META_RESULT meta_result = META_SUCCESS;

    memset(&sNVRAM_WriteReq, 0, sizeof(FT_NVRAM_WRITE_REQ));
    memset(&sNVRAM_WriteCnf, 0, sizeof(FT_NVRAM_WRITE_CNF));
    memset(&sNVRAM_ReadReq, 0, sizeof(FT_NVRAM_READ_REQ));
    memset(&sNVRAM_ReadCnf, 0, sizeof(FT_NVRAM_READ_CNF));

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_GetRecLen(): Start to get nvram struct size via LID = \"%s\"...", pLID);
    meta_result = META_NVRAM_GetRecLen(pLID, &iWriteBufSize);
    if ( META_SUCCESS != meta_result)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_GetRecLen(): Get nvram struct size fail, MetaResult = %s", ResultToString(meta_result));
        return meta_result;
    }
    else
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_GetRecLen(): Get nvram struct size = %d successfully!", iWriteBufSize);

        if ( NULL != sNVRAM_ReadCnf.buf )
        {
            free ( sNVRAM_ReadCnf.buf );
            sNVRAM_ReadCnf.buf = NULL;
        }

        sNVRAM_ReadReq.LID = pLID;
        sNVRAM_ReadReq.RID = iRID;
        sNVRAM_ReadCnf.len = iWriteBufSize;
        sNVRAM_ReadCnf.buf = (unsigned char*) malloc (iWriteBufSize * sizeof(unsigned char));
        if (NULL == sNVRAM_ReadCnf.buf)
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "Malloc heap memory cause fail!");
            return  META_FAILED;
        }
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromModemNVRAM(): Start to read nvram data...");
    meta_result =  REQ_ReadFromModemNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_ReadFromModemNVRAM()";
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_ReadFromModemNVRAM(): Read nvram data successfully!");

    meta_result = (this->*pDataFunc)((char*)sNVRAM_ReadCnf.buf, iRID, pInData, iBufLen);
    if (meta_result !=  META_SUCCESS)
    {
        goto Err;
    }
    else
    {
        /*
        for(int i = 0; i < iBufLen; i++)
        {
        writeData[i] = sNVRAM_ReadCnf.buf[i];
        }
        */
        memcpy(pWriteData, sNVRAM_ReadCnf.buf, iBufLen);
        MTRACE (g_hEBOOT_DEBUG, "%s: Conduct nvram data successfully!", pFuncName);
    }

    sNVRAM_WriteReq.LID = pLID;
    sNVRAM_WriteReq.RID = iRID;
    sNVRAM_WriteReq.len = iWriteBufSize;
    sNVRAM_WriteReq.buf = sNVRAM_ReadCnf.buf;
    m_sNVRAM_OPID = iRID;

    //sNVRAM_WriteCnf.status = 0;
    //sNVRAM_WriteCnf.LID = pLID;
    sNVRAM_WriteCnf.RID = iRID;

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToModemNVRAM(): Start to write nvram data...");
    meta_result =  REQ_WriteToModemNVRAM(&sNVRAM_WriteReq, &sNVRAM_WriteCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_WriteToModemNVRAM()";
        goto Err;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToModemNVRAM(): Write nvram data successfully!");

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToModemNVRAM(): Read nvram data for check start...");
    memset(sNVRAM_ReadCnf.buf, 0 , sNVRAM_ReadCnf.len);
    meta_result = REQ_ReadFromModemNVRAM (&sNVRAM_ReadReq, &sNVRAM_ReadCnf);
    if (meta_result != META_SUCCESS )
    {
        pFuncName = "SmartPhoneSN::REQ_WriteToModemNVRAM()";
        goto Err;
    }
    else
    {
        char tmpReadData[64] = {0};
        m_bWriteNvram = false;
        memcpy(pReadData, sNVRAM_ReadCnf.buf, iBufLen);
        (this->*pDataFunc)(tmpReadData, iRID, pReadData, iBufLen);
        if (strncmp(pWriteData, pReadData, iBufLen) != 0)
        {
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_%s[%s]", pDataType, pInData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_%s[%s]", pDataType, tmpReadData);
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN: Check nvram data FAIL!!");
            meta_result = META_FAILED;
        }
        else
        {

            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Write_%s[%s]", pDataType, pInData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Read_%s[%s]", pDataType, tmpReadData);
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN: Check nvram data PASS!!");
        }
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_WriteToModemNVRAM(): Read nvram data for check end...");

    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    return meta_result;

Err:
    if (sNVRAM_ReadCnf.buf != NULL)
    {
        free (sNVRAM_ReadCnf.buf);
        sNVRAM_ReadCnf.buf = NULL;
    }

    MTRACE_ERR (g_hEBOOT_DEBUG, "%s: fail! MetaResult = %s", pFuncName, ResultToString(meta_result));

    return meta_result;
}

META_RESULT SmartPhoneSN::GetNvramFileName(char * file_name, int buf_len)
{
    int offset;
    SYSTEMTIME time;

    if (g_sMetaComm.sWriteOption.bWriteIMEI)
    {
        file_name[0] = 'I';
        strcpy_s(file_name+1, buf_len-1, m_sScanData.strIMEI[0]);
    }
    else if(g_sMetaComm.sWriteOption.bWriteBarcode)
    {
        file_name[0] = 'B';
        strcpy_s(file_name+1, buf_len-1, m_sScanData.strBarcode);
    }
    else if (g_sMetaComm.sWriteOption.bWriteMeid)
    {
        file_name[0] = 'M';
        strcpy_s(file_name+1, buf_len-1, m_sScanData.strMeid);
    }
    else if(g_sMetaComm.sWriteOption.bWriteWifi)
    {
        file_name[0] = 'W';
        strcpy_s(file_name+1, buf_len-1, m_sScanData.strWifiAddr);
    }
    else if(g_sMetaComm.sWriteOption.bWriteBT)
    {
        file_name[0] = 'T';
        strcpy_s(file_name+1, buf_len-1, m_sScanData.strBTAddr);
    }
    else
    {
        file_name[0] = 'N';
    }

    offset = strlen(file_name);
    if (offset > 84) // buf size 100 - time length 14 - null end 1
        offset = 84;
    file_name[offset++] = '_';

    ::GetLocalTime(&time);
    sprintf_s(file_name + offset, buf_len - offset, "%04u%02u%02u%02u%02u%02u",
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::QueryDBFromDUT()
{
    META_RESULT SPMetaResult = META_SUCCESS;

    MTRACE(g_hEBOOT_DEBUG, "SP_META_QueryIfFunctionSupportedByTarget_r(): Query is suppport load db from DUT...");
    SPMetaResult = SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 3000, "SP_META_File_Operation_Parse_r");
    if (SPMetaResult != META_SUCCESS)
        MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_QueryIfFunctionSupportedByTarget_r(): Query fail, %s.", ResultToString_SP(SPMetaResult));
    else
        MTRACE(g_hEBOOT_DEBUG, "SP_META_QueryIfFunctionSupportedByTarget_r(): Suppport load db from DUT.");

    return SPMetaResult;
}

META_RESULT SmartPhoneSN::GetAPDBFromDUT()
{
    META_RESULT SPMetaResult = META_SUCCESS;
    unsigned int tmp_m = 0;

    // init data for Travel
    char db_folder_dut[128] = "/system/etc/apdb";
    static const char * db_pattern = "APDB_";
    static const char * db_pattern_n = "_ENUM";
    char db_fullname_dut[256] = "";
    char db_fullname_pc[256] = "";
    FILE_OPERATION_PARSE_REQ fop_req;
    FILE_OPERATION_PARSE_CNF fop_cnf;
    FILE_OPERATION_GETFILEINFO_REQ fog_req;
    FILE_OPERATION_GETFILEINFO_CNF fog_cnf;

    // query ap db path in dut
    SPMetaResult = SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 3000, "SP_META_Query_APDBPath_r");
    if (SPMetaResult != META_SUCCESS)
    {
        MTRACE(g_hEBOOT_DEBUG, "Don't support SP_META_Query_APDBPath_r(%d: %s), use default path %s. ",
            SPMetaResult, ResultToString_SP(SPMetaResult), db_folder_dut);
    }
    else
    {
        const int dbpathQueryCount = 30;
        QUERY_APDBPATH_REQ dbpath_req;
        QUERY_APDBPATH_CNF dbpath_cnf;

        memset(&dbpath_req, 0, sizeof(dbpath_req));
        memset(&dbpath_cnf, 0, sizeof(dbpath_cnf));
        for (tmp_m = 0; tmp_m < dbpathQueryCount; tmp_m++)
        {
            SPMetaResult = SP_META_Query_APDBPath_r(m_hSPMetaHandle, 3000, &dbpath_req, &dbpath_cnf);
            if (SPMetaResult == META_SUCCESS)
                break;
            MTRACE_WARN(g_hEBOOT_DEBUG, "Query AP DB path fail(%d, %s), wait 1s and retry.", SPMetaResult, ResultToString_SP(SPMetaResult));
            ::Sleep(1000);
        }
        if (tmp_m >= dbpathQueryCount)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "Query AP DB path fail, stop query.");
            return SPMetaResult;
        }
        strcpy_s(db_folder_dut, 128, (char *)dbpath_cnf.apdb_path);
        MTRACE(g_hEBOOT_DEBUG, "Get AP DB path %s.", db_folder_dut);
    }

    // get file count
    memset(&fop_req, 0, sizeof(fop_req));
    strcpy_s((char*)fop_req.path_name, 256, db_folder_dut);
    strcpy_s((char*)fop_req.filename_substr, 256, db_pattern);
    memset(&fop_cnf, 0, sizeof(fop_cnf));
    MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_Parse_r(): Find AP DB files from DUT...");
    SPMetaResult = SP_META_File_Operation_Parse_r(m_hSPMetaHandle, 3000, &fop_req, &fop_cnf);
    if (SPMetaResult != META_SUCCESS)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_File_Operation_Parse_r(): Find fail, %s.", ResultToString_SP(SPMetaResult));
        return SPMetaResult;
    }
    MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_Parse_r(): Find %u AP DB file(s) from DUT.", fop_cnf.file_count);
    if (fop_cnf.file_count <= 0u)
        return META_FAILED;

    memset(&fog_req, 0, sizeof(fog_req));
    memset(&fog_cnf, 0, sizeof(fog_cnf));
    memset(db_fullname_dut, 0, 256);
    memset(db_fullname_pc, 0, 256);
    for (tmp_m = 0; tmp_m < fop_cnf.file_count; tmp_m++)
    {
        // get DB file info: type, size, full name
        fog_req.index = tmp_m;
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_GetFileInfo_r(): Get %dth AP DB file info...", tmp_m, ResultToString_SP(SPMetaResult));
        SPMetaResult = SP_META_File_Operation_GetFileInfo_r(m_hSPMetaHandle, 3000, &fog_req, &fog_cnf);
        if (SPMetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_File_Operation_GetFileInfo_r(): Get %dth AP DB file info fail, %s.", tmp_m, ResultToString_SP(SPMetaResult));
            continue;
        }
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_GetFileInfo_r(): %dth AP DB: type(%u) size(%u) name(%s).",
               tmp_m, fog_cnf.file_info.file_type, fog_cnf.file_info.file_size, (char*)fog_cnf.file_info.file_name);
        if (fog_cnf.file_info.file_size <= 0u)
            continue;

        // ap db file name don't has prefix "_APDB" and postfix "_ENUM"
        if (fog_cnf.file_info.file_name[0] == '_')
            continue;
        if (strstr((char *)fog_cnf.file_info.file_name, db_pattern_n) != NULL)
            continue;

        // trans file to pc
        sprintf_s(db_fullname_dut, "%s/%s", db_folder_dut, (char*)fog_cnf.file_info.file_name);
        sprintf_s(db_fullname_pc, "%s%s", m_strLogDir_Sub, (char*)fog_cnf.file_info.file_name);
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_ReceiveFile_r(): Save %dth AP DB file to %s...", tmp_m, db_fullname_pc, ResultToString_SP(SPMetaResult));
        SPMetaResult = SP_META_File_Operation_ReceiveFile_r(m_hSPMetaHandle, 8000, db_fullname_dut, db_fullname_pc);
        if (SPMetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_File_Operation_ReceiveFile_r(): Save AP DB file fail, %s.", tmp_m, db_fullname_pc, ResultToString_SP(SPMetaResult));
            continue;
        }
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_ReceiveFile_r(): Save %dth AP DB file ok.", tmp_m, db_fullname_pc, ResultToString_SP(SPMetaResult));

        // get the db path on pc
        strcpy_s(g_sMetaComm.sDBFileOption.strAPDbPath_DUT, db_fullname_pc);

        // current: only support ubin, only one modem db
        break;
    }

    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::GetMDDBFromDUT()
{
    META_RESULT SPMetaResult = META_SUCCESS;
    unsigned int tmp_m = 0;

    // init data for Travel
    char db_folder_dut[64] = "/system/etc/mddb";
    static const char * db_pattern = "MDDB.META_";
    char db_fullname_dut[256] = "";
    char db_fullname_pc[256] = "";
    FILE_OPERATION_PARSE_REQ fop_req;
    FILE_OPERATION_PARSE_CNF fop_cnf;
    FILE_OPERATION_GETFILEINFO_REQ fog_req;
    FILE_OPERATION_GETFILEINFO_CNF fog_cnf;

    // query ap db path in dut
    SPMetaResult = SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 3000, "SP_META_MODEM_Query_MDDBPath_r");
    if (SPMetaResult != META_SUCCESS)
    {
        MTRACE(g_hEBOOT_DEBUG, "Don't support SP_META_MODEM_Query_MDDBPath_r(%d: %s), use default path %s. ",
            SPMetaResult, ResultToString_SP(SPMetaResult), db_folder_dut);
    }
    else
    {
        const int dbpathQueryCount = 30;
        MODEM_QUERY_MDDBPATH_REQ dbpath_req;
        MODEM_QUERY_MDDBPATH_CNF dbpath_cnf;

        memset(&dbpath_req, 0, sizeof(dbpath_req));
        memset(&dbpath_cnf, 0, sizeof(dbpath_cnf));
        for (tmp_m = 0; tmp_m < dbpathQueryCount; tmp_m++)
        {
            SPMetaResult = SP_META_MODEM_Query_MDDBPath_r(m_hSPMetaHandle, 3000, &dbpath_req, &dbpath_cnf);
            if (SPMetaResult == META_SUCCESS)
                break;
            MTRACE_WARN(g_hEBOOT_DEBUG, "Query MD DB path fail(%d, %s), wait 1s and retry.", SPMetaResult, ResultToString_SP(SPMetaResult));
            ::Sleep(1000);
        }
        if (tmp_m >= dbpathQueryCount)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "Query MD DB path fail, stop query.");
            return SPMetaResult;
        }
        strcpy_s(db_folder_dut, 64, (char *)dbpath_cnf.mddb_path);
        MTRACE(g_hEBOOT_DEBUG, "Get MD DB path %s.", db_folder_dut);
    }

    // get file count
    memset(&fop_req, 0, sizeof(fop_req));
    strcpy_s((char*)fop_req.path_name, 256, db_folder_dut);
    strcpy_s((char*)fop_req.filename_substr, 256, db_pattern);
    memset(&fop_cnf, 0, sizeof(fop_cnf));
    MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_Parse_r(): Find MD DB files from DUT...");
    SPMetaResult = SP_META_File_Operation_Parse_r(m_hSPMetaHandle, 3000, &fop_req, &fop_cnf);
    if (SPMetaResult != META_SUCCESS)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_File_Operation_Parse_r(): Find fail, %s.", ResultToString_SP(SPMetaResult));
        return SPMetaResult;
    }
    MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_Parse_r(): Find %u MD DB file(s) from DUT.", fop_cnf.file_count);
    if (fop_cnf.file_count <= 0u)
        return META_FAILED;

    memset(&fog_req, 0, sizeof(fog_req));
    memset(&fog_cnf, 0, sizeof(fog_cnf));
    memset(db_fullname_dut, 0, 256);
    memset(db_fullname_pc, 0, 256);
    for (tmp_m = 0; tmp_m < fop_cnf.file_count; tmp_m++)
    {
        // get DB file info: type, size, full name
        fog_req.index = tmp_m;
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_GetFileInfo_r(): Get %dth MD DB file info...", tmp_m, ResultToString_SP(SPMetaResult));
        SPMetaResult = SP_META_File_Operation_GetFileInfo_r(m_hSPMetaHandle, 3000, &fog_req, &fog_cnf);
        if (SPMetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_File_Operation_GetFileInfo_r(): Get %dth MD DB file info fail, %s.", tmp_m, ResultToString_SP(SPMetaResult));
            continue;
        }
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_GetFileInfo_r(): %dth MD DB: type(%u) size(%u) name(%s).",
               tmp_m, fog_cnf.file_info.file_type, fog_cnf.file_info.file_size, (char*)fog_cnf.file_info.file_name);
        if (fog_cnf.file_info.file_size <= 0u)
            continue;

        // trans file to pc
        sprintf_s(db_fullname_dut, "%s/%s", db_folder_dut, (char*)fog_cnf.file_info.file_name);
        sprintf_s(db_fullname_pc, "%s%s", m_strLogDir_Sub, (char*)fog_cnf.file_info.file_name);
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_ReceiveFile_r(): Save %dth MD DB file to %s...", tmp_m, db_fullname_pc, ResultToString_SP(SPMetaResult));
        SPMetaResult = SP_META_File_Operation_ReceiveFile_r(m_hSPMetaHandle, 8000, db_fullname_dut, db_fullname_pc);
        if (SPMetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "SP_META_File_Operation_ReceiveFile_r(): Save MD DB file fail, %s.", tmp_m, db_fullname_pc, ResultToString_SP(SPMetaResult));
            continue;
        }
        MTRACE(g_hEBOOT_DEBUG, "SP_META_File_Operation_ReceiveFile_r(): Save %dth MD DB file ok.", tmp_m, db_fullname_pc, ResultToString_SP(SPMetaResult));

        // get the db path on pc
        strcpy_s(g_sMetaComm.sDBFileOption.strMD1DbPath_DUT, db_fullname_pc);

        // current: only support ubin, only one modem db
        break;
    }

    return META_SUCCESS;
}

META_RESULT SmartPhoneSN::LoadAPDatabase()
{
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase() start...");

    META_RESULT MetaResult = META_SUCCESS;
    const char * dbpath = NULL;
    unsigned long db;

    if (g_sMetaComm.sDBFileOption.bAPDBFromDUT && g_sMetaComm.sDBFileOption.strAPDbPath_DUT[0] != '\0')
        dbpath = g_sMetaComm.sDBFileOption.strAPDbPath_DUT;
    else
        dbpath = g_sMetaComm.sDBFileOption.strAPDbpath;

    if (g_sMetaComm.sDBFileOption.bDBInitAP)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase(): Already init AP database done, DB path = %s", dbpath);
        MetaResult = META_SUCCESS;
        goto _end;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase(): Start to init AP database, DB path = %s", dbpath);
    MetaResult = SP_META_NVRAM_Init_r(m_hSPMetaHandle, dbpath, &db);
    if ( MetaResult == META_SUCCESS)
    {
        g_sMetaComm.sDBFileOption.bDBInitAP = true;
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase(): ok.");
    }
    else if (MetaResult == META_MAUI_DB_INCONSISTENT && g_sMetaComm.IgnoreDBInconsistent)
    {
        g_sMetaComm.sDBFileOption.bDBInitAP = true;
        MTRACE_WARN(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase(): ap db inconsistent, but ignore.");
        MetaResult = META_SUCCESS;
    }
    else
    {
        g_sMetaComm.sDBFileOption.bDBInitAP = false;
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase(): fail, %s", ResultToString_SP(MetaResult));
    }

_end:
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::LoadAPDatabase() end.");
    return MetaResult;
}

META_RESULT SmartPhoneSN::LoadModemDatabase(int MDindex)
{
    META_RESULT meta_result = META_SUCCESS;
    char *pFuncName = NULL;

    bool bInitMD1Done = true;

    bool *pbInitDBDone = NULL;
    const char *pStrMDDbpath = NULL;
    unsigned long db = 0;

    switch (MDindex)
    {
    case 0:
        pbInitDBDone = &g_sMetaComm.sDBFileOption.bDBInitModem_1;
        if (g_sMetaComm.sDBFileOption.bMDDBFromDUT && g_sMetaComm.sDBFileOption.strMD1DbPath_DUT[0] != '\0')
            pStrMDDbpath = g_sMetaComm.sDBFileOption.strMD1DbPath_DUT;
        else
            pStrMDDbpath = g_sMetaComm.sDBFileOption.strMD1Dbpath;
        break;

    case 1:
        pbInitDBDone = &g_sMetaComm.sDBFileOption.bDBInitModem_2;
        pStrMDDbpath = g_sMetaComm.sDBFileOption.strMD2Dbpath;
        break;

    default:
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::LoadModemDatabase(): Incorrect MDIndex = %d!", MDindex);
        return META_INVALID_ARGUMENTS;
        break;
    }

    if (pStrMDDbpath == NULL || pStrMDDbpath[0] == '\0')
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadModemDatabase(): Incorrect database file path!");
        return META_INVALID_ARGUMENTS;
    }

    if (*pbInitDBDone)
    {
        // For MT6595 DSDA project
        if (m_bDSDAProject)
        {
            MDindex = m_iCurMDChanelIndex;
        }
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_Init_r(): MD[%d] database already init done!",  MDindex);
        return META_SUCCESS;
    }

    //if (g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM)
    if ((m_bDualModem == true && m_bWorldPhone == false) /*|| g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM*/)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_Init_Ex_r(): start to init MD[%d] database, DB path = \"%s\"...",
                MDindex, pStrMDDbpath);
        meta_result = META_NVRAM_Init_Ex_r(m_hMauiMetaHandle, MDindex, pStrMDDbpath, &db);
        pFuncName = "META_NVRAM_Init_Ex_r";
    }
    else
    {
        MDindex = m_sMdInfo.active_md_idx - 1;
        //For MT6595 DSDA project
        if (m_bDSDAProject)
        {
            MDindex = m_iCurMDChanelIndex;
            if (m_bInitExtMDdb)
            {
                m_sMdInfo.activeMdTypeIdx = 0;
            }
        }

        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::META_NVRAM_Init_Ex_Mdtype_r(): start to init MD[%d] database, DB path = \"%s\"...",
                MDindex, pStrMDDbpath);
        meta_result = META_NVRAM_Init_Ex_Mdtype_r(m_hMauiMetaHandle, MDindex, m_sMdInfo.activeMdTypeIdx, pStrMDDbpath, &db);
        pFuncName = "META_NVRAM_Init_Ex_Mdtype_r";
    }

    if (meta_result == META_SUCCESS)
    {
        *pbInitDBDone = true;
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::%s(): Init MD[%d] database successfully!", pFuncName, MDindex);
    }
    else if (meta_result == META_MAUI_DB_INCONSISTENT && g_sMetaComm.IgnoreDBInconsistent)
    {
        *pbInitDBDone = true;
        MTRACE_WARN(g_hEBOOT_DEBUG, "SmartPhoneSN::%s(): md db inconsistent, but ignore.", pFuncName);
        meta_result = META_SUCCESS;
    }
    else
    {
        *pbInitDBDone = false;
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::%s(): Init MD[%d] database Fail, MetaResult = %s",
                   pFuncName, MDindex, ResultToString(meta_result));
    }

    return meta_result;
}

META_RESULT SmartPhoneSN::ExitAPMetaMode()
{
    META_RESULT spMetaResult;
    char *pFuncName = NULL;

    if (!m_bTargetInMetaMode)
    {
        MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ExitAPMetaMode(): not in meta mode.");
        return META_SUCCESS;
    }

    if (g_sMetaComm.bKeepInMeta)
    {
        spMetaResult = SP_META_DisconnectInMetaMode_r (m_hSPMetaHandle);
        pFuncName = "SP_META_DisconnectInMetaMode_r()";
    }
    else
    {
        spMetaResult = SP_META_DisconnectWithTarget_r (m_hSPMetaHandle);
        pFuncName = "SP_META_DisconnectWithTarget_r()";
    }
    if (spMetaResult == META_SUCCESS)
        m_bTargetInMetaMode = false;

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::ExitAPMeta(): Exit meta mode by %s successfully! MetaResult = %s", pFuncName, ResultToString_SP(spMetaResult));
    return spMetaResult;
}

META_RESULT SmartPhoneSN::EnableModemMeta()
{
    int iRet = 0;

    MDLogSetParam(m_sMdInfo.active_md_idx-1);

    MTRACE(g_hEBOOT_DEBUG, "META_DLL::META_ConnectWithMultiModeTarget_r(): Connect to MODEM meta mode...");
    m_tMetaReq_Ex.com_port = m_nKernelComport;
    iRet = META_ConnectWithMultiModeTarget_r(m_hMauiMetaHandle, &m_tMetaReq_Ex, sizeof(m_tMetaReq_Ex), m_pMetaStopFlag, &m_tMetaConnReport_Ex);
    if (iRet == META_SUCCESS)
    {
        MTRACE(g_hEBOOT_DEBUG, "META_DLL::META_ConnectWithMultiModeTarget_r(): ok.");
        iRet = META_SUCCESS;
        m_eMetaMode = SP_MODEM_META;
    }
    else if (iRet == META_MAUI_DB_INCONSISTENT)
    {
        m_eMetaMode = SP_MODEM_META;
        if (!g_sMetaComm.sDBFileOption.bDBInitModem_1)
        {
            MTRACE(g_hEBOOT_DEBUG, "META_DLL::META_ConnectWithMultiModeTarget_r(): md db inconsistent, directly ignore.");
            iRet = META_SUCCESS;
        }
        else if (g_sMetaComm.IgnoreDBInconsistent)
        {
            MTRACE_WARN(g_hEBOOT_DEBUG, "META_DLL::META_ConnectWithMultiModeTarget_r(): md db inconsistent, but ignore.");
            iRet = META_SUCCESS;
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "META_DLL::META_ConnectWithMultiModeTarget_r(): fail, md db inconsistent.");
        }
    }
    else
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "META_DLL::META_ConnectWithMultiModeTarget_r(): fail, %s", ResultToString(iRet));
    }

    if (iRet == META_SUCCESS)
        MDLogOn();

    return (META_RESULT)iRet;
}

void SmartPhoneSN::SetupMetaModeParameters()
{
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SetupMetaModeParameters() start...");
    //For preloader handshake parameters
    memset (&m_stModeArg, 0x0, sizeof(SP_BOOT_ARG_S));
    memset(&m_tMetaReq_Ex, 0, sizeof(m_tMetaReq_Ex));
    memset(&m_tMetaConnReport_Ex, 0, sizeof(m_tMetaConnReport_Ex));

    if (g_sMetaComm.iPre_Connect_Timeout < 10000)
    {
        g_sMetaComm.iPre_Connect_Timeout = 10000;
    }

    if (g_sMetaComm.iKernel_Connect_Timeout < 20000)
    {
        g_sMetaComm.iKernel_Connect_Timeout = 20000;
    }

    m_nKernelComport = g_sMetaComm.iCOMPort;

    //Old parameters
    m_stModeArg.m_bbchip_type = SP_AUTO_DETECT_BBCHIP;
    m_stModeArg.m_ext_clock = SP_AUTO_DETECT_EXT_CLOCK;
    m_stModeArg.m_ms_boot_timeout = SP_BOOT_INFINITE;
    //m_stModeArg.m_max_start_cmd_retry_count = SP_DEFAULT_BROM_START_CMD_RETRY_COUNT;
    m_stModeArg.m_max_start_cmd_retry_count = 0;
    //New parameters
    m_stModeArg.m_uTimeout = g_sMetaComm.iPre_Connect_Timeout;
    m_stModeArg.m_uRetryTime = 2000;
    m_stModeArg.m_uInterval = 10;
    m_stModeArg.m_uBaudrate = CBR_115200;
    m_stModeArg.m_pStopFlag  = &g_iMetaStopFlag;

    if (g_sMetaComm.bSecurityUSB)
    {
        m_stModeArg.m_auth_handle = SPATE_Get_AuthHandle();
        m_stModeArg.m_scert_handle = SPATE_Get_ScertHandle();
        m_stModeArg.m_cb_sla_challenge = SLA_Challenge;
        m_stModeArg.m_cb_sla_challenge_arg = NULL;
        m_stModeArg.m_cb_sla_challenge_end = SLA_Challenge_END;
        m_stModeArg.m_cb_sla_challenge_end_arg = NULL;
    }
    else
    {
        m_stModeArg.m_auth_handle = NULL;
        m_stModeArg.m_scert_handle = NULL;
        m_stModeArg.m_cb_sla_challenge = NULL;
        m_stModeArg.m_cb_sla_challenge_arg = NULL;
        m_stModeArg.m_cb_sla_challenge_end = NULL;
        m_stModeArg.m_cb_sla_challenge_end_arg = NULL;
    }
    m_stModeArg.m_bIsUSBEnable = g_sMetaComm.bUsbEnable;
    m_stModeArg.m_bIsSymbolicEnable = false;
    m_stModeArg.m_bIsCompositeDeviceEnable = g_sMetaComm.bCompositeDeviceEnable;
    m_stModeArg.m_euBootMode = SP_META_BOOT;
    m_stModeArg.m_uPortNumber = 0;
    m_stModeArg.m_uMDMode = 0;

    //Common
    m_tMetaReq_Ex.com_port = m_nKernelComport;
    m_tMetaReq_Ex.ms_connect_timeout = g_sMetaComm.iKernel_Connect_Timeout;
    m_tMetaReq_Ex.close_com_port = 1;
    m_tMetaReq_Ex.InMetaMode = 1;
    m_tMetaReq_Ex.protocol = 1;          // DHL
    m_tMetaReq_Ex.channel_type = 1;      // Native channel
    m_tMetaReq_Ex.boot_meta_arg.m_bbchip_type = AUTO_DETECT_BBCHIP;
    m_tMetaReq_Ex.boot_meta_arg.m_ext_clock = AUTO_DETECT_EXT_CLOCK;
    //UART parameters
    m_tMetaReq_Ex.baudrate[0] = META_BAUD921600;
    m_tMetaReq_Ex.baudrate[1] = META_BAUD_END;
    m_tMetaReq_Ex.flowctrl = META_SW_FLOWCTRL;
    //USB
    if (g_sMetaComm.bUsbEnable)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SetupMetaModeParameters(): Target enter meta mode by [USB].");
        m_tMetaReq_Ex.usb_enable = 1;
        m_tMetaReq_Ex.boot_meta_arg.m_usb_enable = 1;
    }
    else
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SetupMetaModeParameters(): Target enter meta mode by [UART].");
        m_tMetaReq_Ex.usb_enable = 0;
        m_tMetaReq_Ex.boot_meta_arg.m_usb_enable = 0;
        m_tMetaReq_Ex.escape = 1;
    }

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::SetupMetaModeParameters() end...");
}

META_RESULT SmartPhoneSN::EnterAPMetaMode()
{
    META_RESULT spMetaResult = META_SUCCESS;
    int bootResult = 0;

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::EnterAPMetaMode() : Enter ap meta start...");

    if (!g_sMetaComm.bAlreadyInMeata)
    {
        UpdateUIMsg("Start searching preloader com port and handshake...");
        UpdateProgress(0.08);

        bootResult = ConnectWithPreloader();
        if (bootResult != 0)
        {
            UpdateUIMsg("Searching preloader com port and handshake Fail.");

            spMetaResult = (META_RESULT)bootResult;
            return spMetaResult;
        }
		//add by wzb
		g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO3, "连接中...");
		UpdateStatusProgress(3,0.4,0);
        // Kernel Port apear at least take 6s, Wait 4s for Preloader Port disapear
        //  when Preloader and Kernel has the same port
        Sleep(4000);
    }
    else
    {
        UpdateUIMsg("Already in Meta Mode, bypass preloader com port handshake.");
        UpdateProgress(0.08);
    }

    UpdateUIMsg("Start searching kernel com port and handshake...");
    UpdateProgress(0.10);

    bootResult = ConnectWithKernelPort_Ex();
    UpdateProgress(0.12);
    if (bootResult == 0)
    {
        UpdateUIMsg("Searching kernel com port and handshake OK.");
		//add by wzb
		UpdateStatusProgress(3,1.0,1);
		g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO3, "已连接");
    }
    else
    {
        UpdateUIMsg("Searching kernel com port and handshake Fail.");
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterAPMetaMode() : Enter ap meta end.");
    spMetaResult = (META_RESULT)bootResult;
    return spMetaResult;
}

//Add for DSDA Extern Modem Native Download
#if 0
int SmartPhoneSN::DSDA_QueryNativeDownload()    //0:query fail   1:need to download   2:do not need to download
{
    int iRet = 0;
    META_RESULT spMetaResult = META_SUCCESS;

    MODEM_QUERY_DOWNLOAD_STATUS_REQ qDownloadReq;
    MODEM_QUERY_DOWNLOAD_STATUS_CNF qDownloadCnf;

    memset(&qDownloadReq, 0, sizeof(MODEM_QUERY_DOWNLOAD_STATUS_REQ));
    memset(&qDownloadCnf, 0, sizeof(MODEM_QUERY_DOWNLOAD_STATUS_CNF));

    for (int i = 0; i < 50; i++)
    {
        spMetaResult = SP_META_MODEM_Query_Download_Status_r(m_hSPMetaHandle, 2000, &qDownloadReq, &qDownloadCnf);
        if (spMetaResult == META_SUCCESS) break;
    }

    if (spMetaResult != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_QueryNativeDownload() : Query extern modem download status fail! MetaResult = %s!", ResultToString_SP(spMetaResult));
        UpdateUIMsg("Query download extern modem status fail!");
        iRet = 1;
    }
    else
    {
        if (qDownloadCnf.percentage != m_iLastPercentage)
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_QueryNativeDownload() : Download status = %d / %d", qDownloadCnf.percentage, qDownloadCnf.status_code);
        }
        m_iLastPercentage = qDownloadCnf.percentage;

        if (qDownloadCnf.percentage == 110 && qDownloadCnf.status_code == 0000)
        {
            //Have downloaded already
            iRet = 0;
        }
        else
        {
            //Need to download
            iRet = 2;
        }
    }
    return iRet;
}
META_RESULT SmartPhoneSN::DSDA_StartNativeDownload()
{
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_StartNativeDownload() : Extern modem native download start!");
    META_RESULT spMetaResult = META_SUCCESS;

    MODEM_TRIGGER_NATIVE_DOWNLOAD_REQ DownloadReq;
    MODEM_TRIGGER_NATIVE_DOWNLOAD_CNF DownloadCnf;

    memset(&DownloadReq, 0, sizeof(MODEM_TRIGGER_NATIVE_DOWNLOAD_REQ));
    memset(&DownloadCnf, 0, sizeof(MODEM_TRIGGER_NATIVE_DOWNLOAD_CNF));

    spMetaResult = SP_META_MODEM_TRIGGER_NATIVE_DOWNLOAD_r(m_hSPMetaHandle, 20000, &DownloadReq, &DownloadCnf);
    if (spMetaResult != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_StartNativeDownload() : Extern modem native download fail! MetaResult = %s!", ResultToString_SP(spMetaResult));
        return spMetaResult;
    }
    else
    {
        //Sleep(10000);
        int QueryResult = -1;
        m_iLastPercentage = 0;
        do
        {
            QueryResult = DSDA_QueryNativeDownload();
        } while (QueryResult == 2);

        if (QueryResult != 0)
        {
            spMetaResult = META_FAILED;
        }
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_StartNativeDownload() : Extern modem native download finish!");
    return spMetaResult;
}
#endif

//Add for DSDA Extern Modem Backup and Restore
bool SmartPhoneSN::DSDA_ExternalModemBackup()
{
    META_RESULT MetaResult;
    // query if bakcup function support
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_ExternalModemBackup() : Start query if target support backup function...");
    MetaResult = META_QueryIfFunctionSupportedByTarget_r(m_hMauiMetaHandle, 5000, "META_MISC_EX_BackupCalibrationToStorage_r");
    if (META_SUCCESS != MetaResult)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_ExternalModemBackup() : Query target backup function fail! MetaResult = %s", ResultToString(MetaResult));
        return false;
    }
    // trigger external MODEM backup
    unsigned int status = 0;
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_ExternalModemBackup() : Start backup data to SDS...");
    MetaResult = META_MISC_EX_BackupCalibrationToStorage_r(m_hMauiMetaHandle, 2000, 0, &status);
    if (META_SUCCESS != MetaResult || status != 0)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_ExternalModemBackup() : Backup fail! MetaResult = %s", ResultToString(MetaResult));
        return false;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DSDA_ExternalModemBackup() : Backup finish!");
    return true;
}


void SmartPhoneSN::ThreadMainEntryPoint()
{
    int iTestCount = 0;
    bool bNeedScanData = false;
    bool bSkipScanData = false;
    bool bNeedLoadAPDB = false;
    int times = 0;
    int Ret = 0;

    /*For dual talk(external modem) project
    * Due to dual talk project can`t switch back and forth between ap and external modem
    * When target type select tablet  and enable external modem option, mean that will write ap nvram, so need disable write external modem nvram
    * so it is mean(ex md + tablet to write bt wifi; ex md + tablet to write barcode imei;)
    */
    bool bWriteBarcode = g_sMetaComm.sWriteOption.bWriteBarcode;
    bool bWriteIMEI = g_sMetaComm.sWriteOption.bWriteIMEI;

    if (g_sMetaComm.bDualTalk)
    {
        g_sMetaComm.sWriteOption.bWriteBarcode = false;
        g_sMetaComm.sWriteOption.bWriteIMEI = false;
    }

    bNeedScanData = NeedScanData();
    if (bNeedScanData == false)
    {
        if (!g_sMetaComm.sWriteOption.bWriteDrm &&
            !g_sMetaComm.sWriteOption.bWriteHdcp &&
            !g_sMetaComm.sWriteOption.bInstallHdcpData &&
            !g_sMetaComm.sWriteOption.bWriteAttestationKey)
        {
            PopupMsgBox("Warning", MB_OK | MB_ICONWARNING, "At least select one write option!!");
            return;
        }

        // Don't need to Popup scan data dialog
        bSkipScanData = true;
        if (g_sMetaComm.sWriteOption.bWriteHdcp)
        {
            // Write DRM Key & Install Hdcp data don't need to load ap database
            bNeedLoadAPDB = true;
        }
    }
    else
    {
        if (g_sMetaComm.sWriteOption.bWriteBarcode ||
            g_sMetaComm.sWriteOption.bWriteIMEI ||
            g_sMetaComm.sWriteOption.bWriteBT ||
            g_sMetaComm.sWriteOption.bWriteWifi ||
            g_sMetaComm.sWriteOption.bWriteEthernetMac ||
            g_sMetaComm.sWriteOption.bWriteDrmkeyMCID ||
            g_sMetaComm.sWriteOption.bWriteSerialNo ||
            g_sMetaComm.sWriteOption.bWriteMeid ||
            g_sMetaComm.bClearMetaBootFlag)
        {
            bNeedLoadAPDB = true;
        }
    }

    bool bAnyOperationFail = false;
    META_RESULT MetaResult = META_SUCCESS;
    META_RESULT SPMetaResult = META_SUCCESS;

    if (g_sMetaComm.bSwithTool)
        EnableUSBSwitch();

    if (g_sMetaComm.bCheckFastbootLock)
    {
        UpdateUIMsg("Start Adb_Manager_Init() and Set_Fastboot_EnvironmentVariable()...");
        if (!Set_Fastboot_EnvironmentVariable())
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable() FAIL...");
            UpdateUIMsg("Set Fastboot Environment Variable Fail!!");
            return;
        }
        UpdateUIMsg("Set Fastboot Environment Variable OK!!");
        if (!Adb_Manager_Init())
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Manager_Init() FAIL...");

            Adb_Manager_Deinit();
            UpdateUIMsg("SmartPhoneSN::Adb_Manager_Init() FAIL...");
            return;
        }
        UpdateUIMsg("SmartPhoneSN::Adb_Manager_Init() OK...");
    }
    m_hMauiMetaHandle = INVALID_META_HANDLE;
    m_hSPMetaHandle = INVALID_META_HANDLE;
    //Init meta handle
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::MetaHandle_Init() start...");
    MetaResult = MetaHandle_Init();
    if (MetaResult != META_SUCCESS)
    {
        bAnyOperationFail = true;
        UpdateUIMsg("MetaHandle_Init() : MetaResult = %s", ResultToString_SP(MetaResult));
        *m_pMetaStopFlag = BOOT_STOP;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::MetaHandle_Init() end...");

    CScanData scanDlg;

    while(true)
    {
        if (*m_pMetaStopFlag == BOOT_STOP)
            break;
        if (g_sMetaComm.iStressTest > 1 && iTestCount > 0)
        {
            if (iTestCount >= g_sMetaComm.iStressTest)
                break;
        }
        else if (!bSkipScanData && scanDlg.DoModal() != IDOK)
            break;
        else if (bSkipScanData && iTestCount > 0 &&
            ::AfxMessageBox(_T("Start write next devices ?"), MB_YESNO|MB_ICONQUESTION) != IDYES)
            break;
        iTestCount++;

        DebugOnOff(true);
        MTRACE(g_hEBOOT_DEBUG, "------------------------------------ START -------------------------------------");
        LogConfig();
        bAnyOperationFail = false;
        UpdateMainDlgUI(false, NORMAL);

        //SPInit() can`t call before SNThread_Init() function
        SNThread_Init();
        //MTRACE (g_hEBOOT_DEBUG, "m_bWriteModemNvram = %d,m_sMdInfo.number_of_md = %d", m_bWriteModemNvram, m_sMdInfo.number_of_md);
        SPInit();

        SetupMetaModeParameters();

        UpdateUIMsg("Start load and init AP database...");

        UpdateProgress(0.06);

        if (g_sMetaComm.bUsbEnable)
        {
            UpdateUIMsg("Wait for usb insert...");
        }

        EnableStartBTN(true);
		
	
        MetaResult = (META_RESULT)EnterAPMetaMode();
		
        if (MetaResult != META_SUCCESS)
        {
            bAnyOperationFail = true;

			//add by wzb
			g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO3, "fail");
			UpdateStatusProgress(3,1.0,0);
			//end
            goto End;
        }

        SetDutClock(m_hSPMetaHandle);

        if (g_sMetaComm.sDBFileOption.bAPDBFromDUT || g_sMetaComm.sDBFileOption.bMDDBFromDUT)
        {
            SPMetaResult = QueryDBFromDUT();
            if (SPMetaResult != 0)
            {
                UpdateUIMsg("Don't support load db from device.");
                bAnyOperationFail = true;
                goto End;
            }
        }

        if (bNeedLoadAPDB == true)
        {
            // load ap db from dut
            if (!g_sMetaComm.sDBFileOption.bDBInitAP && g_sMetaComm.sDBFileOption.bAPDBFromDUT)
            {
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::GetAPDBFromDUT() : Get AP DB from DUT start...");
                SPMetaResult = GetAPDBFromDUT();
                if (SPMetaResult != META_SUCCESS)
                    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::GetMDDBFromDUT() : fail, will try load config DB.");
                else
                    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::GetAPDBFromDUT() : end.");
            }

            MetaResult = (META_RESULT)LoadAPDatabase();
            if (MetaResult != META_SUCCESS)
            {
                bAnyOperationFail = true;
                UpdateUIMsg("Load AP Database fail, %s", ResultToString_SP(MetaResult));
                goto End;
            }
        }

        SPMetaResult = SP_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 500, "SP_META_ENCRYPTED_Support_r");
        if (SPMetaResult == META_SUCCESS)
        {
            if (QueryEncryptionSupport(&m_hSPMetaHandle))
            {
                Encryption Edlg;
                //Edlg.DoModal();
                while ((IDOK == Edlg.DoModal() ) && times < 3)
                {
                    if (!VeritifiEncryption(&m_hSPMetaHandle))
                    {
                        MTRACE(g_hEBOOT_DEBUG, "VeritifiEncryption() retry : %d", times + 1);
                        times = times + 1;
                    }
                    else
                        break;
                }
                if (3 == times)
                {
                    bAnyOperationFail = true;
                    MTRACE(g_hEBOOT_DEBUG, "VeritifiEncryption() retry times more than 3 times!");
                    goto End;
                }
            }
        }

        if (g_sMetaComm.eTargetType != TABLET_WIFI_ONLY)
        {
            // query modem info must after queryEncryption.
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::GetSPModemInfo_Ex(): Query modem information start...");
            Ret = GetSPModemInfo_Ex();
            if (Ret != 0)
            {
                bAnyOperationFail = true;
                MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::GetSPModemInfo_Ex(): Query modem information fail, MetaResult = %s", ResultToString_SP(Ret));
                UpdateUIMsg("Query modem information fail, %s", ResultToString_SP(Ret));
                goto End;
            }
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::GetSPModemInfo_Ex(): Query modem information end.");
        }

        //Special case: m_bWriteModemNvram = true && m_sMdInfo.number_of_md = 0
        //So skip to load modem database
        MTRACE (g_hEBOOT_DEBUG, "m_bWriteModemNvram = %d,m_sMdInfo.number_of_md = %d", m_bWriteModemNvram, m_sMdInfo.number_of_md);
        if ((m_bWriteModemNvram == true || g_sMetaComm.bCheckCalFlag || g_sMetaComm.bCheckFtFlag) && m_sMdInfo.number_of_md >= 1)
        {
            UpdateUIMsg("Start switch to MD meta and init Modem database...");

            UpdateProgress(0.14);

            if (!g_sMetaComm.sDBFileOption.bDBInitModem_1 && g_sMetaComm.sDBFileOption.bMDDBFromDUT)
            {
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::GetMDDBFromDUT() : Get Modem DB from DUT start...");
                SPMetaResult = GetMDDBFromDUT();
                if (SPMetaResult != META_SUCCESS)
                    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::GetMDDBFromDUT() : fail, will try load config DB.");
                else
                    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::GetMDDBFromDUT() : end.");
            }

            UpdateProgress(0.18);

            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnableModemMeta(): Enable MODEM meta from AP meta start...");
            MetaResult = EnableModemMeta();
            if (MetaResult != META_SUCCESS)
            {
                bAnyOperationFail = true;
                MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::ApSwithToModemMeta_Ex() : Enable MODEM meta from AP meta fail.");
                UpdateUIMsg("SmartPhoneSN::ApSwithToModemMeta_Ex() : Enable MODEM meta from AP meta fail.");
                goto End;
            }
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnableModemMeta(): end.");

            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadModemDatabase(): start...");
            MetaResult = LoadModemDatabase(0);
            if (MetaResult != META_SUCCESS)
            {
                bAnyOperationFail = true;
                MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadModemDatabase() : MetaResult = %s", ResultToString(MetaResult));
                UpdateUIMsg("SmartPhoneSN::LoadModemDatabase() : MetaResult = %s", ResultToString(MetaResult));
                goto End;
            }
            MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::LoadModemDatabase(): end.");
        }
        UpdateProgress(0.22);

        EnableStartBTN(false);
        UpdateUIMsg("Start loop write data to nvram...");
		//modify by wzb for write country code 20190318
		//MetaResult = WriteNvramLoop();
		MetaResult =WriteCountryCode();
		//end
        if (MetaResult != META_SUCCESS)
        {
            bAnyOperationFail = true;
            goto End;
        }

End:
        EnableStartBTN(false);
        if (m_bStopBeforeUSBInsert == true)
        {
            UpdateProgress(0.0);
            UpdateUIMsg("...");
        }
        else
        {
            if (m_eMetaMode == SP_AP_META || m_eMetaMode == SP_MODEM_META)
            {
                ExitAPMetaMode();
				//add by wzb
				UpdateStatusProgress(7,1.0,1);
				g_pMainDlg->SetDlgItemText(IDC_TV_TESTITEM_INFO7, "pass");
            }

            //The backup nvram api return success, but the operator pull up usb cable immediately before all operation successfully.
            //This case will cause nvram data lose when firmware upgrate, so that put indicator turn green after exit meta.
            if (g_sMetaComm.bCheckFastbootLock && m_bBackupNvramSuccess && !bAnyOperationFail)
            {
                if (REQ_FastbootLockOem() == -1)
                {
                    bAnyOperationFail = true;
                    UpdateUIMsg("ERROR!! REQ_FastbootLockOem()");
                }
            }

            if (bAnyOperationFail)
            {
                m_Process->SendMessage(PBM_SETBARCOLOR, 0, RGB(255, 0, 0));
                UpdateUICountResult(FAIL);
            }
            else
            {
                UpdateUIMsg("All Operate successfully!!");
                MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::All Operate successfully!!");

#ifdef _AUTO_GEN_FEATURE_
                if (g_AutoGenData.bEnableAutoGen)
                {
                    UpdateAutoGenParaToSetupFile();
                }
#endif
                UpdateUICountResult(PASS);
            }

            UpdateProgress(1.0);
        }

        if (g_sMetaComm.sWriteOption.bWriteDrm)
        {
            *m_pMetaStopFlag = BOOT_STOP;
        }
        MTRACE(g_hEBOOT_DEBUG, "------------------------------------ END --------------------------------------");
        DebugOnOff(false);
        Sleep(bSkipScanData ? 3000 : 1000);
    }

    if (g_sMetaComm.bCheckFastbootLock)
    {
        Adb_Manager_Deinit();
    }
    g_sMetaComm.sWriteOption.bWriteBarcode = bWriteBarcode;
    g_sMetaComm.sWriteOption.bWriteIMEI = bWriteIMEI;

    if (g_sMetaComm.bSwithTool)
        DisableUSBSwitch();

    MetaHandle_DeInit();
    UpdateMainDlgUI(true, CANCEL);
}

bool SmartPhoneSN::Adb_Manager_Init()
{
    HANDLE hread_cmd = NULL, hwrite_out = NULL;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&hread_cmd, &hdle_write_cmd, &sa, 0))
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Manager_Init() : CreatePipe() write fail...");
        return FALSE;
    }
    //MTRACE (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Manager_Init(): createpipe1 success!");
    if (!CreatePipe(&hdle_read_out, &hwrite_out, &sa, 0))
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Manager_Init() : CreatePipe() read fail...");
        return FALSE;
    }
    //MTRACE (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Manager_Init(): createpipe2 success!");

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = hwrite_out;
    si.hStdOutput = hwrite_out;
    si.hStdInput = hread_cmd;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    if (!CreateProcess("c:\\windows\\system32\\cmd.exe", NULL, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
    {
        //MTRACE_ERR (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Manager_Init(): create cmd.exe process fail!");
        return FALSE;
    }
    //MTRACE (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Manager_Init(): create cmd.exe process success!");

    CloseHandle(hread_cmd);
    CloseHandle(hwrite_out);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    //MTRACE (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Manager_Init(): hdle_write_cmd = %08X, hdle_read_out = %08X",hdle_write_cmd,hdle_read_out);
    return TRUE;
}

int SmartPhoneSN::Adb_Manager_Write(char *cmd_line, int cmd_length)
{
    char cmd_buf[100];
    memset(cmd_buf, 0, sizeof(cmd_buf));
    memcpy(cmd_buf, cmd_line, cmd_length);

    cmd_buf[cmd_length] = '\r';
    cmd_buf[cmd_length + 1] = '\n';
    DWORD bytesWritten = 0;

    if (!WriteFile(hdle_write_cmd, cmd_buf, cmd_length + 2, &bytesWritten, NULL))
    {
        return 0;
    }

    return (int)(bytesWritten);
}

int SmartPhoneSN::Adb_Manager_Read(ADB_READ_BUF *adb_read_buf, int buf_size)
{
    DWORD bytesRead = 0;

    if (!ReadFile(hdle_read_out, adb_read_buf->rd_buf, buf_size, &bytesRead, NULL))
    {
        return 0;
    }
    adb_read_buf->rd_buf[bytesRead] = '\0';

    return (int)bytesRead;
}



bool SmartPhoneSN::Adb_WR_Cmd(char *cmd_line, int cmd_length)
{

    if (!(Adb_Manager_Write(cmd_line, cmd_length)))
    {
        return FALSE;
    }
    Sleep(800);
    memset(&read_buf, 0, r_bufsize);
    if (!(Adb_Manager_Read(&read_buf, r_bufsize)))
    {
        return FALSE;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_WR_Cmd(): read_buf is = %s", read_buf.rd_buf);
    return TRUE;
}

int SmartPhoneSN::Adb_Manager_Deinit()
{

    //Adb_WR_Cmd("exit",strlen("exit"));
    Adb_WR_Cmd("adb kill-server", strlen("adb kill-server"));
    //  MTRACE(g_hEBOOT_DEBUG, "ATB_MR::Scan_Devices_FunProc(): close cmd.exe");
    memset(&read_buf, 0, r_bufsize);
    CloseHandle(hdle_write_cmd);
    //  MTRACE(g_hEBOOT_DEBUG, "Adb_Manager::~Adb_Manager(): close hdle_write_cmd");
    CloseHandle(hdle_read_out);
    //  MTRACE(g_hEBOOT_DEBUG, "Adb_Manager::~Adb_Manager(): close hdle_read_out");
    return 0;
}
int SmartPhoneSN::REQ_FastbootLockOem()
{
    int timer = 0;
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Fastboot Lock OEM start....");
    UpdateUIMsg("Start Lock OEM...");

    int bootResult = 0;
    SetupMetaModeParameters();
    m_stModeArg.m_euBootMode = SP_FAST_BOOT;
    UpdateUIMsg("Start searching preloader com port and handshake...");

    bootResult = ConnectWithPreloader();
    if (bootResult != 0)
    {
        UpdateUIMsg("Searching preloader com port and handshake Fail!!");
        return -1;
    }
    else
    {
        UpdateUIMsg("Searching preloader com port and handshake OK!!");

        UpdateUIMsg("Start searching fastboot devices...");

        // MTRACE (g_hEBOOT_DEBUG, "ATB_MR::ATB_MR_Init(): Adb_Manager_Init success!");
        if (!Adb_WR_Cmd("adb start-server", strlen("adb start-server")))
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : adb start-server FAIL...");
            Adb_Manager_Deinit();
            return -1;
        }

        if (Adb_Detect_Devact() > 0)
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Adb_Detect_Devact() OK...");
            UpdateUIMsg("Find a fastboot devices OK...");
        }
        else
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Adb_Detect_Devact() timeout...");
            Adb_Manager_Deinit();
            return -1;
        }

        //if(!Adb_WR_Cmd("fastboot oem lock",strlen("fastboot oem lock")))
        if (!Fastboot_Lock_Oem())
        {
            MTRACE_ERR (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : fastboot oem lock fail...");
            Adb_Manager_Deinit();
            return -1;
        }
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : fastboot oem lock ok...");
        UpdateUIMsg("fastboot oem lock OK...");
        return 0;
    }
}

int SmartPhoneSN::REQ_FastbootUnLockOem()
{
    int timer = 0;
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Fastboot Lock OEM start....");
    UpdateUIMsg("Start Lock OEM...");

    int bootResult = 0;
    SetupMetaModeParameters();
    m_stModeArg.m_euBootMode = SP_FAST_BOOT;
    UpdateUIMsg("Start searching preloader com port and handshake...");
    bootResult = ConnectWithPreloader();
    if (bootResult != 0)
    {
        UpdateUIMsg("Searching preloader com port and handshake Fail!!");
        return -1;
    }
    else
    {
        UpdateUIMsg("Searching preloader com port and handshake OK!!");

        UpdateUIMsg("Start searching ADB devices...");
        if (!Adb_Manager_Init())
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Adb_Manager_Init() OK...");
            Adb_Manager_Deinit();
            return -1;
        }
        //  MTRACE (g_hEBOOT_DEBUG, "ATB_MR::ATB_MR_Init(): Adb_Manager_Init success!");
        if (!Adb_WR_Cmd("adb start-server", strlen("adb start-server")))
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Adb_Manager_Init() OK...");
            Adb_Manager_Deinit();
            return -1;
        }
        while (1)
        {
            timer++;
            if (Adb_Detect_Devact() > 0)
            {
                MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Adb_Detect_Devact() OK...");
                break;
            }
            else if (timer > 60)
            {
                MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : Adb_Detect_Devact() timeout...");
                Adb_Manager_Deinit();
                return -1;
            }
        }
        if (!Adb_WR_Cmd("fastboot oem unlock", strlen("fastboot oem unlock")))
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : fastboot oem lock fail...");
            Adb_Manager_Deinit();
            return -1;
        }
        if (!Adb_WR_Cmd("fastboot reboot", strlen("fastboot reboot")))
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::REQ_FastbootLockOem() : fastboot oem lock fail...");
            Adb_Manager_Deinit();
            return -1;
        }
        Adb_Manager_Deinit();
        return 0;
    }
}
#if 0
int SmartPhoneSN::Adb_Detect_Devact()
{
    if (!Adb_WR_Cmd("fastboot devices", strlen("fastboot devices")))
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Detect_Devact(): Adb_WR_Cmd() fastboot devices fail!");
        return 0;
    }

    // MTRACE (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Detect_Devact(): send adb devices success!");
    // MTRACE (g_hEBOOT_DEBUG, "Adb_Manager::Adb_Detect_Devact(): %s",read_buf.rd_buf);

    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Detect_Devact(): read_buf is = %s", read_buf.rd_buf);
    int r_leg = strlen(read_buf.rd_buf);
    char *f_str2 = "fastboot";
    int str2_leg = strlen("fastboot");
    if (strstr(read_buf.rd_buf, f_str2) != NULL)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Detect_Devact(): find device ok....");
        return 1;
    }
    return 0;
}

int SmartPhoneSN::Fastboot_Lock_Oem()
{
    if (!Adb_WR_Cmd("fastboot oem lock", strlen("fastboot oem lock")))
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Detect_Devact(): Adb_WR_Cmd() fastboot devices fail!");
        return 0;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Fastboot_Lock_Oem(): read_buf is = %s", read_buf.rd_buf);
    int r_leg = strlen(read_buf.rd_buf);
    char *f_str2 = "OKAY";
    int str2_leg = strlen("OKAY");
    if (strstr(read_buf.rd_buf, f_str2) != NULL)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Fastboot_Lock_Oem(): find OKAY");
        return 1;
    }
    return 0;
}

#else
int SmartPhoneSN::Adb_Detect_Devact()
{
    for (int i = 0; i < 10; i++)
    {
        if (!(Adb_Manager_Write("fastboot devices", strlen("fastboot devices"))))
        {
            return 0;
        }
        Sleep(800);
        memset(&read_buf, 0, r_bufsize);
        if (!(Adb_Manager_Read(&read_buf, r_bufsize)))
        {
            return 0;
        }
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Detect_Devact(): read_buf is = %s", read_buf.rd_buf);
        int r_leg = strlen(read_buf.rd_buf);
        char *f_str1 = strstr(read_buf.rd_buf, "devices");
        if (f_str1 != NULL)
        {
            if (strstr(f_str1, "fastboot") != NULL)
            {
                MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Adb_Detect_Devact(): find device ok....");
                return 1;
            }
        }
    }
    return 0;

}
int SmartPhoneSN::Fastboot_Lock_Oem()
{
    //if(!Adb_WR_Cmd("fastboot oem lock",strlen("fastboot oem lock")))
    if (!(Adb_Manager_Write("fastboot oem lock", strlen("fastboot oem lock"))))
    {
        return 0;
    }
    for (int i = 0; i < 3; i++)
    {
        memset(&read_buf, 0, r_bufsize);
        if (!(Adb_Manager_Read(&read_buf, r_bufsize)))
        {
            return 0;
        }
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Fastboot_Lock_Oem(): read_buf is = %s", read_buf.rd_buf);
        int r_leg = strlen(read_buf.rd_buf);
        char *f_str2 = "OKAY";
        int str2_leg = strlen("OKAY");
        if (strstr(read_buf.rd_buf, f_str2) != NULL)
        {
            MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Fastboot_Lock_Oem(): fastboot oem lock ok....");
            return 1;
        }
        Sleep(1500);
    }
    return 0;
}

int SmartPhoneSN::Set_Fastboot_EnvironmentVariable(void)
{
    char chBuf[0x8000] = {0};
    DWORD dwSize = GetEnvironmentVariable("path", chBuf, 0x10000);
    if (dwSize == 0)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): GetEnvironmentVariable() fail....");
        return 0;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): GetEnvironmentVariable() chBuf is: %s....", chBuf);

    CString strEnvPaths(chBuf);

    // find current path
    if (!GetModuleFileName(NULL, chBuf, MAX_PATH))
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): GetModuleFileName() fail....");
        return 0;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): GetModuleFileName() chBuf is: %s...", chBuf);

    CString strAppPath(chBuf);
    const int nPos = strAppPath.ReverseFind(_T('\\'));
    if (nPos > 0)
    {
        // add path backslash
        strAppPath = strAppPath.Mid(0, nPos + 1);
    }
    //add fastboot path
    strEnvPaths.TrimRight(";");
    strEnvPaths += ";" + strAppPath + "Android;";
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): SetEnvironmentVariable() strEnvPaths is: %s....", strEnvPaths);

    dwSize = SetEnvironmentVariable("path", strEnvPaths);
    if (dwSize == 0)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): SetEnvironmentVariable() fail....");
        return 0;
    }
    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::Set_Fastboot_EnvironmentVariable(): SetEnvironmentVariable() strEnvPaths is: %s....", strEnvPaths);
    return 1;
}

#endif

// C2K
META_RESULT SmartPhoneSN::EnterC2KGen90()
{
    META_RESULT ret_last = META_SUCCESS;
    META_RESULT MetaResult = META_SUCCESS;

    bool bInited = false;
    bool bSwitchMd = false;
    bool bConnected = false;
    char szLogFilePath[MAX_PATH] = "";
    C2K_LIBCONFIGPARMS c2kConfig;
    PHONEATTRIBUTE_PARMS phoneAttr;
    C2K_CONNECT_PARAMS connectReq;
    char szPath[s_MAX_PATH] = {0};

    int preMdChannelIdx = m_sMdInfo.active_md_idx;
    int c2kMdChannelIdx = -1;
    unsigned int c2k_protocol = META_MODEM_SRV_ETS;
    unsigned int c2k_ch_type = META_MODEM_CH_TUNNELING;

    UpdateUIMsg("Start init C2K Meta...");


    // get c2k modem index and channel protocol + type
    for (int i = 0; i < (int)m_sMdInfo.number_of_md + 2; i++)
    {
        MTRACE(g_hEBOOT_DEBUG, "[C2K] i= %d, protocol = %d, channel_type=%d",
            i, m_SpMdCapList.modem_cap[i].md_service, m_SpMdCapList.modem_cap[i].ch_type);

        if (m_SpMdCapList.modem_cap[i].md_service == META_MODEM_SRV_ETS)
        {
            c2kMdChannelIdx = i;
            c2k_protocol = m_SpMdCapList.modem_cap[i].md_service;
            c2k_ch_type = m_SpMdCapList.modem_cap[i].ch_type;
            break;
        }
    }
    if (c2kMdChannelIdx == -1)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] can not find C2K modem.");
        return META_FAILED;
    }
    UpdateProgress(0.14);


    // init c2k
    sprintf_s(szLogFilePath, "%sETS_LOG.txt", m_strLogDir_Sub);
    c2kConfig.bLog = true;
    c2kConfig.bScript = false;
    c2kConfig.szLogPath = szLogFilePath;
    c2kConfig.szScriptPath = "";
    memset(&phoneAttr, 0, sizeof(PHONEATTRIBUTE_PARMS));
    phoneAttr.eAfcControlMode = C2K_AFC_TCXO;
    phoneAttr.eRfMode = C2K_RF_MODE_EVDO;
    phoneAttr.eRxCalibrationType = C2K_RX_CAL_MAIN;
    phoneAttr.eSidbAccessMode = C2K_SIDB_FSI;
    phoneAttr.bWriteSIDBFlag = false;       //if write calibration flag?
    phoneAttr.uPllSettleTime = 100;
    phoneAttr.uRxAGCGainTableSelectSettleTime = 200;
    phoneAttr.uRxCtrlSettleTime = 200;
    phoneAttr.uRxGainStateSettleTime = 200;
    phoneAttr.uTxAGCConfigSettleTime = 50;
    phoneAttr.uTxCtrlSettleTime = 50;
    phoneAttr.uTxTestRateSettleTime = 100;
    phoneAttr.uEnableRxTxSpySettleTime = 0;
    phoneAttr.bAFCSlopeStepPerPpmWithQ6 = 0;

    MTRACE (g_hEBOOT_DEBUG, "[C2K] META_C2K_Init() start...");
    MetaResult = META_C2K_Init(m_hMauiMetaHandle, &c2kConfig, &phoneAttr);
    if (MetaResult != META_SUCCESS)
    {
        MTRACE_ERR (g_hEBOOT_DEBUG, "[C2K] META_C2K_Init() fail");
        UpdateUIMsg("META_C2K_Init() fail");
        return MetaResult;
    }
    bInited = true; // inited
    MTRACE (g_hEBOOT_DEBUG, "[C2K] META_C2K_Init() end...");
    UpdateProgress(0.18);


    // connect to c2k
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Connect C2K USB port.");
    if (c2k_ch_type == META_MODEM_CH_TUNNELING)
    {
        MTRACE(g_hEBOOT_DEBUG, "[C2K] Switch to C2K modem start...");
        if (c2kMdChannelIdx != preMdChannelIdx)
        {
            MTRACE(g_hEBOOT_DEBUG, "[C2K] META_DLL::META_SwitchCurrentModemEx_r() : Switch to C2K modem start...");
            MetaResult = META_SwitchCurrentModemEx_r(m_hMauiMetaHandle, 10000, c2kMdChannelIdx, c2k_protocol, c2k_ch_type, NULL, NULL);
            if (MetaResult != META_SUCCESS)
            {
                MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_DLL::META_SwitchCurrentModemEx_r() : fail, %s", ResultToString(MetaResult));
                goto end;
            }
            bSwitchMd = true;
            MTRACE(g_hEBOOT_DEBUG, "[C2K] META_DLL::META_SwitchCurrentModemEx_r() : ok.");
        }
        MTRACE(g_hEBOOT_DEBUG, "[C2K] Switch to C2K modem end.");

        MTRACE(g_hEBOOT_DEBUG, "[C2K] us_com_port = %d", c2kMdChannelIdx);
        connectReq.eComType = C2K_COM_VC;
        connectReq.uComNum = c2kMdChannelIdx;
    }
    else
    {
        connectReq.eComType = C2K_COM_USB;
        connectReq.uComNum = 0;

        GetModuleFileName(NULL, szPath, s_MAX_PATH);
        (_tcsrchr(szPath, _T('\\')))[1] = 0;
        strcat_s(szPath, "EtsMsg.txt");

        MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_LoadMessage_r() : start...");
        MetaResult = META_C2K_LoadMessage_r(m_hMauiMetaHandle, szPath);
        if (MetaResult != META_SUCCESS)
        {
            UpdateUIMsg("META_C2K_LoadMessage_r() : fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
            MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_LoadMessage_r() : fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
            UpdateProgress(1.00);
            goto end;
        }
        UpdateUIMsg("META_C2K_LoadMessage_r() : successful!");
        MTRACE (g_hEBOOT_DEBUG, "[C2K] META_C2K_LoadMessage_r() : successful!");
        UpdateProgress(0.28);
    }

    UpdateUIMsg("Start connect with C2K Meta...");
    UpdateProgress(0.34);

    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_ConnectWithTarget_r() start...");
    for (int i = 0; i < 20; i++)
    {
        MetaResult = META_C2K_ConnectWithTarget_r(m_hMauiMetaHandle, 2000, &connectReq);
        if (MetaResult == META_SUCCESS)
            break;
    }
    if (MetaResult != META_SUCCESS)
    {
        UpdateUIMsg("META_C2K_ConnectWithTarget_r() : fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_ConnectWithTarget_r() : fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        UpdateProgress(1.00);
        goto end;
    }
    bConnected = true; // connected
    UpdateUIMsg("META_C2K_ConnectWithTarget_r() : successful!");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_ConnectWithTarget_r() : successful!");
    UpdateProgress(0.48);


    // handshake with c2k
    for (int i = 0; i < 10; i++)
    {
        MetaResult = META_C2K_LoopbackTest_r(m_hMauiMetaHandle, 2000, i + 1);
        if (MetaResult == META_SUCCESS)
            break;
    }
    if (MetaResult != META_SUCCESS )
    {
        UpdateUIMsg("META_C2K_LoopbackTest_r() : fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_LoopbackTest_r() : fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        UpdateProgress(1.00);
        goto end;
    }
    UpdateProgress(0.60);


    // task
    if (g_sMetaComm.sWriteOption.bWriteMeid)
        ret_last = (META_RESULT)WriteMEID90();
    else if (g_sMetaComm.sWriteOption.bWriteEsn)
        ret_last = (META_RESULT)WriteESN90();

    UpdateProgress(0.85);


end:
    // disconnect
    if (bConnected)
    {
        MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_DisconnectWithTarget_r start...");
        MetaResult = META_C2K_DisconnectWithTarget_r(m_hMauiMetaHandle);
        if (MetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_DisconnectWithTarget_r : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
            UpdateUIMsg("META_C2K_DisconnectWithTarget_r : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
            return MetaResult;
        }
        MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_DisconnectWithTarget_r end.");
    }

    // deinit
    if (bInited)
    {
        MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_DeInit start...");
        MetaResult = META_C2K_DeInit(m_hMauiMetaHandle);
        if (MetaResult != META_SUCCESS)
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_DeInit : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
            UpdateUIMsg("META_C2K_DeInit : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
            return MetaResult;
        }
        MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_DeInit end!");
    }

    if (bSwitchMd)
    {
        MTRACE(g_hEBOOT_DEBUG, "[C2K] switch to 234G modem start!");
        MetaResult = META_SwitchCurrentModemEx_r(m_hMauiMetaHandle, 10000, 0, m_SpMdCapList.modem_cap[0].md_service,
        m_SpMdCapList.modem_cap[0].ch_type, NULL, NULL);
        if (MetaResult == META_SUCCESS)
        {
            MTRACE(g_hEBOOT_DEBUG, "[C2K] META_SwitchCurrentModemEx_r(): [C2K] switch to 234G modem successful!");
            UpdateUIMsg("META_SwitchCurrentModemEx_r(): [C2K] switch to 234G modem successful!");
        }
        else if (MetaResult == META_MAUI_DB_INCONSISTENT)
        {
            MetaResult = META_SUCCESS;
            MTRACE(g_hEBOOT_DEBUG, "[C2K] META_SwitchCurrentModemEx_r(): [C2K] switch successful, md db inconsistent, directly ignore.");
        }
        else
        {
            MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_SwitchCurrentModemEx_r(): [C2K] switch to 234G modem fail!");
            UpdateUIMsg("META_SwitchCurrentModemEx_r(): [C2K] switch to 234G modem fail!");
            return MetaResult;
        }
        MTRACE(g_hEBOOT_DEBUG, "[C2K] switch to 234G modem end!");
    }

    if (ret_last != META_SUCCESS)
    {
        if (g_sMetaComm.sWriteOption.bWriteMeid)
            UpdateUIMsg("[C2K] Write MEID fail.");
        else if (g_sMetaComm.sWriteOption.bWriteEsn)
            UpdateUIMsg("[C2K] Write ESN fail.");
        UpdateProgress(1.00);
    }

    return ret_last;
}

int SmartPhoneSN::EnterC2KGen93()
{
    int ret_i = 0;
    META_RESULT ret_meta = META_SUCCESS;

    MMRfTestCmdRfCapabilityReq mreq_rfcap;
    MMRfTestCmdRfCapabilityCnf mcnf_rfcap;

    UpdateUIMsg("Start enter C2K...");

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] Query if support query c2k...");
    ret_meta = META_QueryIfFunctionSupportedByTarget_r(m_hMauiMetaHandle, 30000, "META_MMRf_GetRfCapability_r");
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("Don't support query C2K, may be not support C2K.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] Query if support api META_MMRf_GetRfCapability_r fail. %s.", ResultToString(ret_meta));
        return 1;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] Query if support c2k...");
    memset(&mreq_rfcap, 0, sizeof(mreq_rfcap));
    mreq_rfcap.capabilityItemsSize = sizeof(MMRfCapabilityItemSet);
    mreq_rfcap.calibrationItemsSize = sizeof(MMRfCalibrationItemSet);
    memset(&mcnf_rfcap, 0, sizeof(mcnf_rfcap));
    ret_meta = META_MMRf_GetRfCapability_r(m_hMauiMetaHandle, 30000, &mreq_rfcap, sizeof(mreq_rfcap), &mcnf_rfcap, sizeof(mcnf_rfcap));
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("Query C2K info fail, may be not support C2K.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] META_MMRf_GetRfCapability_r fail. %s.", ResultToString(ret_meta));
        return 2;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] calibrationItems.ratmap_support_WM: is_capable(%d), parameter(%d).",
        mcnf_rfcap.calibrationItems.ratmap_support_WM.is_capable,
        mcnf_rfcap.calibrationItems.ratmap_support_WM.parameter);

    if (!mcnf_rfcap.calibrationItems.ratmap_support_WM.is_capable ||
        (mcnf_rfcap.calibrationItems.ratmap_support_WM.parameter & 0x04) == 0)
    {
        UpdateUIMsg("Don't suport C2K.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] Don't support c2k.", ResultToString(ret_meta));
        return -1;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] META_MMC2K_RegisterHandler_r start...");
    ret_meta = META_MMC2K_RegisterHandler_r(m_hMauiMetaHandle);
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("Register C2K fail.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] META_MMC2K_RegisterHandler_r fail. %s.", ResultToString(ret_meta));
        return 3;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] META_MMC2K_SetTargetGenType_r start...");
    ret_meta = META_MMC2K_SetTargetGenType_r(m_hMauiMetaHandle, 1);
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("Set C2K info fail.");
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] META_MMC2K_SetTargetGenType_r fail. %s.", ResultToString(ret_meta));
        return 4;
    }
    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] META_MMC2K_SetTargetGenType_r end.");

    UpdateUIMsg("Enter C2K success, start to write mied/esn...");

    // task
    if (g_sMetaComm.sWriteOption.bWriteMeid)
        ret_i = WriteMEID93();
    else if (g_sMetaComm.sWriteOption.bWriteEsn)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::EnterC2KGen93(): [C2K] MT6293 don't support ESN writing.");
        UpdateUIMsg("[C2K] MT6293 don't support ESN writing!");
        ret_i = META_FAILED;
    }
    if (ret_i != 0)
        return 5;

    return 0;
}

int SmartPhoneSN::WriteMEID90()
{
    META_RESULT ret_meta = META_SUCCESS;
    MEID_TABLE data_meid;

    UpdateUIMsg("Start to Write MEID (>=MT6290)...");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_WriteNvram_r() : Write MEID start...");
    memset(&data_meid, 0, sizeof(data_meid));
    data_meid.idType = 0;
    strcpy_s(data_meid.meid, g_sMetaComm.sScanData.strMeid);
    for (int i = 0; i < 20; i++)
    {
        ret_meta = META_C2K_WriteNvram_r(m_hMauiMetaHandle, 3000, 0, C2K_DB_MEID, &data_meid, sizeof(data_meid));
        if (ret_meta == META_SUCCESS)
            break;
    }
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("META_C2K_WriteNvram_r() : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_WriteNvram_r() : Write MEID fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        return ret_meta;
    }
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_WriteNvram_r() : Write MEID end.");
    UpdateProgress(0.70);


    UpdateUIMsg("Read MEID for check start...");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Read MEID for check start...");
    memset(&data_meid, 0, sizeof(data_meid));
    data_meid.idType = 0;
    ret_meta = META_C2K_ReadNvram_r(m_hMauiMetaHandle, 3000, 0, C2K_DB_MEID, &data_meid, sizeof(data_meid));
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("META_C2K_ReadNvram_r() : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_ReadNvram_r(): Read MEID fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        return ret_meta;
    }
    UpdateProgress(0.75);

    MTRACE(g_hEBOOT_DEBUG, "[C2K] Write MEID :%s", g_sMetaComm.sScanData.strMeid);
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Read MEID :%s", data_meid.meid);
    if (_stricmp(data_meid.meid, g_sMetaComm.sScanData.strMeid) != 0)
    {
        UpdateUIMsg("[C2K] Checking MEID fail!");
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] Checking MEID fail!");
        return META_FAILED;
    }
    UpdateUIMsg("Checking MEID succesfully!");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Checking MEID succesfully!");
    UpdateProgress(0.80);

    return META_SUCCESS;
}

int SmartPhoneSN::WriteESN90()
{
    META_RESULT ret_meta = META_SUCCESS;
    MEID_TABLE data_meid;

    UpdateUIMsg("Start to Write ESN (>=MT6290)...");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_WriteNvram_r() : Write ESN start...");
    memset(&data_meid, 0, sizeof(data_meid));
    data_meid.idType = 1;
    strcpy_s(data_meid.esn, g_sMetaComm.sScanData.strEsn);
    for (int i = 0; i < 20; i++)
    {
        ret_meta = META_C2K_WriteNvram_r(m_hMauiMetaHandle, 3000, 0, C2K_DB_MEID, &data_meid, sizeof(data_meid));
        if (ret_meta == META_SUCCESS)
            break;
    }
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("META_C2K_WriteNvram_r() : Write ESN fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_WriteNvram_r() : Write ESN fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        return ret_meta;
    }
    UpdateUIMsg("META_C2K_WriteNvram_r() : Write ESN succesful!");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_C2K_WriteNvram_r() : Write ESN succesful!");
    UpdateProgress(0.70);

    UpdateUIMsg("Read ESN for check start...");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Read ESN for check start...");
    memset(&data_meid, 0, sizeof(data_meid));
    data_meid.idType = 1;
    ret_meta = META_C2K_ReadNvram_r(m_hMauiMetaHandle, 3000, 0, C2K_DB_MEID, &data_meid, sizeof(data_meid));
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("META_C2K_ReadNvram_r() : Read ESN fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_C2K_ReadNvram_r() : Read ESN fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        return ret_meta;
    }

    MTRACE(g_hEBOOT_DEBUG, "[C2K] Write ESN :%s", g_sMetaComm.sScanData.strMeid);
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Read ESN :%s", data_meid.esn);
    if (_stricmp(data_meid.esn, g_sMetaComm.sScanData.strEsn) != 0)
    {
        UpdateUIMsg("Checking ESN Fail!");
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] Checking ESN Fail!");
        return ret_meta;
    }
    UpdateUIMsg("Checking Esn Successfully!");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Checking ESN successfully!");
    UpdateProgress(0.80);

    return META_SUCCESS;
}

int SmartPhoneSN::WriteMEID93()
{
    META_RESULT ret_meta = META_SUCCESS;
    C2kTestCmdGetSetMeid data_meid;

    UpdateUIMsg("Start to Write MEID (>=MT6293)...");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_MMC2K_SetMeid_r() : Write MEID start...");
    memset(&data_meid, 0, sizeof(data_meid));
    strcpy_s(data_meid.meid, g_sMetaComm.sScanData.strMeid);
    for (int i = 0; i < 20; i++)
    {
        ret_meta = META_MMC2K_SetMeid_r(m_hMauiMetaHandle, 3000, &data_meid);
        if (ret_meta == META_SUCCESS)
            break;
    }
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("META_MMC2K_SetMeid_r() : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_MMC2K_SetMeid_r() : Write MEID fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        return ret_meta;
    }
    MTRACE(g_hEBOOT_DEBUG, "[C2K] META_MMC2K_SetMeid_r() : Write MEID end.");
    UpdateProgress(0.70);


    UpdateUIMsg("Read MEID for check start...");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Read MEID for check start...");
    memset(&data_meid, 0, sizeof(data_meid));
    ret_meta = META_MMC2K_GetMeid_r(m_hMauiMetaHandle, 3000, &data_meid);
    if (ret_meta != META_SUCCESS)
    {
        UpdateUIMsg("META_MMC2K_GetMeid_r() : MetaResult = %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] META_MMC2K_GetMeid_r(): Read MEID fail, %s", META_C2K_GetErrorString(m_hMauiMetaHandle));
        return ret_meta;
    }
    UpdateProgress(0.75);

    MTRACE(g_hEBOOT_DEBUG, "[C2K] Write MEID :%s", g_sMetaComm.sScanData.strMeid);
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Read MEID :%s", data_meid.meid);
    if (_stricmp(data_meid.meid, g_sMetaComm.sScanData.strMeid) != 0)
    {
        UpdateUIMsg("[C2K] Checking MEID fail!");
        MTRACE_ERR(g_hEBOOT_DEBUG, "[C2K] Checking MEID fail!");
        return META_FAILED;
    }
    UpdateUIMsg("Checking MEID succesfully!");
    MTRACE(g_hEBOOT_DEBUG, "[C2K] Checking MEID succesfully!");
    UpdateProgress(0.80);

    return META_SUCCESS;
}

bool SmartPhoneSN::QueryEncryptionSupport(int * MetaHandle )
{
    CRYPTFS_QUERYSUPPORT_CNF cryptfs_Support_cnf;
    META_RESULT MetaResult;
    MetaResult = SP_META_ENCRYPTED_Support_r(*MetaHandle, 60000, &cryptfs_Support_cnf);

    if ((MetaResult == META_SUCCESS) && (cryptfs_Support_cnf.support == 1))
    {
        MTRACE(g_hEBOOT_DEBUG, "Do support Encryption!");
        return true;
    }
    MTRACE(g_hEBOOT_DEBUG, "Do not support Encryption : MetaResult = %s!", ResultToString_SP(MetaResult));
    return false;
}

bool SmartPhoneSN::VeritifiEncryption(int *MetaHandle)
{
    META_RESULT MetaResult;
    CRYPTFS_VERITIF_REQ cryptfs_Veritif_Req;
    CRYPTFS_VERITIF_CNF cryptfs_Veritif_Cnf;

    memset(&cryptfs_Veritif_Req, 0, sizeof(CRYPTFS_VERITIF_REQ));
    strcpy_s((char *)cryptfs_Veritif_Req.pwd, 32, g_cPwd);
    cryptfs_Veritif_Req.length = strlen(g_cPwd);
    memset(&cryptfs_Veritif_Cnf, 0, sizeof(CRYPTFS_VERITIF_CNF));

    MetaResult = SP_META_ENCRYPTED_VertifyPwd_r(*MetaHandle, 60000, &cryptfs_Veritif_Req, &cryptfs_Veritif_Cnf);
    if ((MetaResult == META_SUCCESS) && cryptfs_Veritif_Cnf.match_result == 1)
    {
        MTRACE(g_hEBOOT_DEBUG, "Do match the password!");
        return true;
    }
    else
    {
        UpdateUIMsg("Do not match: MetaResult = %s!", ResultToString_SP(MetaResult));
        return false;
    }
}

bool SmartPhoneSN::SetDutClock(int hApMeta)
{
    META_RESULT ret_mt;
    SET_TARGET_CLOCK_REQ req;
    SET_TARGET_CLOCK_CNF cnf;
    SYSTEMTIME time;

    ret_mt = SP_META_QueryIfFunctionSupportedByTarget_r(hApMeta, 2000, "SP_META_Set_TargetClock_r");
    if (ret_mt != META_SUCCESS)
    {
        MTRACE_WARN(g_hEBOOT_DEBUG, "SmartPhoneSN::SetDutClock(): don't support(%d): %s.",
            ret_mt, ResultToString_SP(ret_mt));
        return false;
    }

    memset(&req, 0, sizeof(req));
    memset(&cnf, 0, sizeof(cnf));
    ::GetLocalTime(&time);
    req.year = time.wYear;
    req.mon = time.wMonth;
    req.day = time.wDay;
    req.hour = time.wHour;
    req.min = time.wMinute;
    req.sec = time.wSecond;
    req.ms = time.wMilliseconds;

    ret_mt = SP_META_Set_TargetClock_r(hApMeta, 3000, &req, &cnf);
    if (ret_mt != META_SUCCESS)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "SmartPhoneSN::SetDutClock(): set fail(%d): %s.",
            ret_mt, ResultToString_SP(ret_mt));
        return false;
    }

    MTRACE(g_hEBOOT_DEBUG, "SmartPhoneSN::SetDutClock(): set ok.");
    return true;
}
