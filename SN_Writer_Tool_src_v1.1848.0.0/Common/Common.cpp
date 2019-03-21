#include "StdAfx.h"
#include <shlwapi.h>

#include "Common.h"
#include "SNFstream.h"
#include "C2kAgent_api.h"

#include "USBSwitchDLLibrary.h"
#include "des.h"
#include "AutoGen.h"
#include "SNbase.h"


void *g_hSwitchTool = NULL;
//DWORD g_MainDlgHandle;
META_Common_struct g_sMetaComm;
int  g_iMetaStopFlag;
char g_pSetupFilepath[MAX_PATH];
char g_pPasswdFilepath[MAX_PATH];
Login_Identify_e g_LoginIdentify;

#ifdef _AUTO_GEN_FEATURE_

char g_pAutoGenSetupFilepath[MAX_PATH];
AutoGenData_struct  g_AutoGenData;

IniData_struct g_AutoGenSetup[] =
{
    {"Enable AutoGen", (bool*)&g_AutoGenData.bEnableAutoGen, INI_BOOLEAN},
    {"Barcode Increase Radix", (int*)&g_AutoGenData.eIncreaseRadix, INI_INTEGER},
    {"AutoGen Barcode", (bool*)&g_AutoGenData.bAutoGenBarcode, INI_BOOLEAN},
    {"AutoGen IMEI", (bool*)&g_AutoGenData.bAutoGenIMEI, INI_BOOLEAN},
    {"AutoGen BT", (bool*)&g_AutoGenData.bAutoGenBT, INI_BOOLEAN},
    {"AutoGen Wifi", (bool*)&g_AutoGenData.bAutoGenWifi, INI_BOOLEAN},
    {"AutoGen Ethernet", (bool*)&g_AutoGenData.bAutoGenEthernet, INI_BOOLEAN},
    {"AutoGen DRMKey MCID", (bool*)&g_AutoGenData.bAutoGenDRMKeyMCID, INI_BOOLEAN},

    {"Barcode Step", (int*)&g_AutoGenData.sBarcode.iStep, INI_INTEGER},
    {"Barcode Start", (char*)&g_AutoGenData.sBarcode.Start, INI_STRING},
    {"Barcode End", (char*)&g_AutoGenData.sBarcode.End, INI_STRING},
    {"Barcode Next", (char*)&g_AutoGenData.sBarcode.Next, INI_STRING},

    {"IMEI Step", (int*)&g_AutoGenData.sIMEI.iStep, INI_INTEGER},
    {"IMEI Start", (char*)&g_AutoGenData.sIMEI.Start, INI_STRING},
    {"IMEI End", (char*)&g_AutoGenData.sIMEI.End, INI_STRING},
    {"IMEI Next", (char*)&g_AutoGenData.sIMEI.Next, INI_STRING},

    {"BT Step", (int*)&g_AutoGenData.sBtAddress.iStep, INI_INTEGER},
    {"BT Start", (char*)&g_AutoGenData.sBtAddress.Start, INI_STRING},
    {"BT End", (char*)&g_AutoGenData.sBtAddress.End, INI_STRING},
    {"BT Next", (char*)&g_AutoGenData.sBtAddress.Next, INI_STRING},

    {"Wifi Step", (int*)&g_AutoGenData.sWifiAddress.iStep, INI_INTEGER},
    {"Wifi Start", (char*)&g_AutoGenData.sWifiAddress.Start, INI_STRING},
    {"Wifi End", (char*)&g_AutoGenData.sWifiAddress.End, INI_STRING},
    {"Wifi Next", (char*)&g_AutoGenData.sWifiAddress.Next, INI_STRING},

    {"Ethernet Step", (int*)&g_AutoGenData.sEthernetAddress.iStep, INI_INTEGER},
    {"Ethernet Start", (char*)&g_AutoGenData.sEthernetAddress.Start, INI_STRING},
    {"Ethernet End", (char*)&g_AutoGenData.sEthernetAddress.End, INI_STRING},
    {"Ethernet Next", (char*)&g_AutoGenData.sEthernetAddress.Next, INI_STRING},

    {"DRMKey MCID Step", (int*)&g_AutoGenData.sDRMKeyMCID.iStep, INI_INTEGER},
    {"DRMKey MCID Start", (char*)&g_AutoGenData.sDRMKeyMCID.Start, INI_STRING},
    {"DRMKey MCID End", (char*)&g_AutoGenData.sDRMKeyMCID.End, INI_STRING},
    {"DRMKey MCID Next", (char*)&g_AutoGenData.sDRMKeyMCID.Next, INI_STRING},
};

#endif //#ifdef _AUTO_GEN_FEATURE_

IniData_struct g_IniData[] =
{
    {"Stress Test", &g_sMetaComm.iStressTest, INI_INTEGER},
    {"Already in Meta", &g_sMetaComm.bAlreadyInMeata, INI_BOOLEAN},
    {"Keep in Meta", &g_sMetaComm.bKeepInMeta, INI_BOOLEAN},
    {"Clear Meta Boot Flag", &g_sMetaComm.bClearMetaBootFlag, INI_BOOLEAN},
    {"Operator Mode", (int*)&g_sMetaComm.eOperatorMode, INI_INTEGER},
    {"Preloader Connect Timeout", (int*)&g_sMetaComm.iPre_Connect_Timeout, INI_INTEGER},
    {"Kernel Connect Timeout", (int*)&g_sMetaComm.iKernel_Connect_Timeout, INI_INTEGER},
    {"Target type", (int*)&g_sMetaComm.eTargetType, INI_INTEGER},
    {"Brom Port Filter", (char*)g_sMetaComm.sPortFilter.strBromFilter, INI_STRING},
    {"Preloader Port Filter", (char*)g_sMetaComm.sPortFilter.strPreloaderFilter, INI_STRING},
    {"Kernel Port Filter", (char*)g_sMetaComm.sPortFilter.strKernelFilter, INI_STRING},
    {"Composite Device Enable", (bool*)&g_sMetaComm.bCompositeDeviceEnable, INI_BOOLEAN},
    {"Usb Enable", (bool*)&g_sMetaComm.bUsbEnable, INI_BOOLEAN},
    {"USBSwitchTool Enable", (bool*)&g_sMetaComm.bSwithTool, INI_BOOLEAN},
    {"DualTalk Enable", (bool*)&g_sMetaComm.bDualTalk, INI_BOOLEAN},
    {"Security USB Enable", (bool*)&g_sMetaComm.bSecurityUSB, INI_BOOLEAN},
    {"Skip Write Prod_Info Enable", (bool*)&g_sMetaComm.bSkipWriteProdInfo, INI_BOOLEAN},
    {"Check BackupNv to PC Enable", (bool*)&g_sMetaComm.bCheckBackNVtoPC, INI_BOOLEAN},
    {"Check Fastboot OEM Lock Enable", (bool*)&g_sMetaComm.bCheckFastbootLock, INI_BOOLEAN},
    {"Check Calibration flag Enable", (bool*)&g_sMetaComm.bCheckCalFlag, INI_BOOLEAN},
    {"Check FinalTest flag Enable", (bool*)&g_sMetaComm.bCheckFtFlag, INI_BOOLEAN},
    // C2K Modem
    {"Write Meid", (bool*)&g_sMetaComm.sWriteOption.bWriteMeid, INI_BOOLEAN},
    {"Write Esn", (bool*)&g_sMetaComm.sWriteOption.bWriteEsn, INI_BOOLEAN},
    {"Write Barcode", (bool*)&g_sMetaComm.sWriteOption.bWriteBarcode, INI_BOOLEAN},
    {"Write IMEI", (bool*)&g_sMetaComm.sWriteOption.bWriteIMEI, INI_BOOLEAN},
    {"Write Serial No.", (bool*)&g_sMetaComm.sWriteOption.bWriteSerialNo, INI_BOOLEAN},
    {"Write BT", (bool*)&g_sMetaComm.sWriteOption.bWriteBT, INI_BOOLEAN},
    {"Write Wifi", (bool*)&g_sMetaComm.sWriteOption.bWriteWifi, INI_BOOLEAN},
    {"Write Hdcp", (bool*)&g_sMetaComm.sWriteOption.bWriteHdcp, INI_BOOLEAN},
    {"Write DRMKey", (bool*)&g_sMetaComm.sWriteOption.bWriteDrm, INI_BOOLEAN},
    {"Install Hdcp Data", (bool*)&g_sMetaComm.sWriteOption.bInstallHdcpData, INI_BOOLEAN},
    {"Write Attestation Key", (bool*)&g_sMetaComm.sWriteOption.bWriteAttestationKey, INI_BOOLEAN},
    {"Write Ethernet Mac Address", (bool*)&g_sMetaComm.sWriteOption.bWriteEthernetMac, INI_BOOLEAN},
    {"Write DRMkey MCID", (bool*)&g_sMetaComm.sWriteOption.bWriteDrmkeyMCID, INI_BOOLEAN},
    {"IMEI Nums", (int*)&g_sMetaComm.sIMEIOption.iImeiNums, INI_INTEGER},
    {"IMEI CheckSum", (bool*)&g_sMetaComm.sIMEIOption.bCheckSum, INI_BOOLEAN},
    {"IMEI Lock", (bool*)&g_sMetaComm.sIMEIOption.bLockIMEI, INI_BOOLEAN},
    {"Lock OTP", (bool*)&g_sMetaComm.sIMEIOption.bLockOtp, INI_BOOLEAN},
    {"DualIMEI same", (bool*)&g_sMetaComm.sIMEIOption.bDualIMEISame, INI_BOOLEAN},
    {"IMEI_1 Check header", (bool*)&g_sMetaComm.sIMEIHeader_Option[0].bCheckHeader, INI_BOOLEAN},
    {"IMEI_2 Check header", (bool*)&g_sMetaComm.sIMEIHeader_Option[1].bCheckHeader, INI_BOOLEAN},
    {"IMEI_3 Check header", (bool*)&g_sMetaComm.sIMEIHeader_Option[2].bCheckHeader, INI_BOOLEAN},
    {"IMEI_4 Check header", (bool*)&g_sMetaComm.sIMEIHeader_Option[3].bCheckHeader, INI_BOOLEAN},
    //C2K modem
    {"Meid Check header", (bool*)&g_sMetaComm.sMeidHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"Esn Check header", (bool*)&g_sMetaComm.sEsnHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"Serial No. Check header", (bool*)&g_sMetaComm.sSerialNoHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"Barcode Check header", (bool*)&g_sMetaComm.sBarcHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"BT Check header", (bool*)&g_sMetaComm.sBTHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"Wifi Check header", (bool*)&g_sMetaComm.sWifiHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"Ethernet Check header", (bool*)&g_sMetaComm.sEthernetMacHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"DRMKey MCID Check header", (bool*)&g_sMetaComm.sDrmkeyMCIDHeader_Option.bCheckHeader, INI_BOOLEAN},
    {"Enable Check BTWifi", (bool*)&g_sMetaComm.bCheckBTWifi, INI_BOOLEAN},
    {"IMEI_1 header string", (char*)g_sMetaComm.sIMEIHeader_Option[0].strHeader, INI_STRING},
    {"IMEI_2 header string", (char*)g_sMetaComm.sIMEIHeader_Option[1].strHeader, INI_STRING},
    {"IMEI_3 header string", (char*)g_sMetaComm.sIMEIHeader_Option[2].strHeader, INI_STRING},
    {"IMEI_4 header string", (char*)g_sMetaComm.sIMEIHeader_Option[3].strHeader, INI_STRING},
    //C2K Modem
    {"Meid header string", (char*)g_sMetaComm.sMeidHeader_Option.strHeader, INI_STRING},
    {"Esn header string", (char*)g_sMetaComm.sEsnHeader_Option.strHeader, INI_STRING},
    {"Serial No. header string", (char*)g_sMetaComm.sSerialNoHeader_Option.strHeader, INI_STRING},
    {"Barcode header string", (char*)g_sMetaComm.sBarcHeader_Option.strHeader, INI_STRING},
    {"BT header string", (char*)g_sMetaComm.sBTHeader_Option.strHeader, INI_STRING},
    {"Wifi header string", (char*)g_sMetaComm.sWifiHeader_Option.strHeader, INI_STRING},
    {"Ethernet header string", (char*)g_sMetaComm.sEthernetMacHeader_Option.strHeader, INI_STRING},
    {"DRMKey MCID header string", (char*)g_sMetaComm.sDrmkeyMCIDHeader_Option.strHeader, INI_STRING},
    {"AP DB from DUT", (bool*)&g_sMetaComm.sDBFileOption.bAPDBFromDUT, INI_BOOLEAN},
    {"MD DB from DUT", (bool*)&g_sMetaComm.sDBFileOption.bMDDBFromDUT, INI_BOOLEAN},
    {"Modem_1 database path", (char*)g_sMetaComm.sDBFileOption.strMD1Dbpath, INI_STRING},
    {"Modem_2 database path", (char*)g_sMetaComm.sDBFileOption.strMD2Dbpath, INI_STRING},
    {"AP database path", (char*)g_sMetaComm.sDBFileOption.strAPDbpath, INI_STRING},
    {"SP Auth path", (char*)g_sMetaComm.sLoadFile.strSPAuthPath, INI_STRING},
    {"Hdcp Bin path", (char*)g_sMetaComm.sLoadFile.strHdcpBinPath, INI_STRING},
    {"DrmKey path", (char*)g_sMetaComm.sLoadFile.strDrmKeyPath, INI_STRING},
    {"Hdcp data path", (char*)g_sMetaComm.sLoadFile.strHdcpDataPath, INI_STRING},
    {"Hdcp cek path", (char*)g_sMetaComm.sLoadFile.strHdcpCekPath, INI_STRING},
    {"Attestation Key Path", (char*)g_sMetaComm.sLoadFile.strAttestationKeyPath, INI_STRING},

    {"Log Dir", (char*)g_sMetaComm.strLogDir, INI_STRING},
    {"ModemLog", (bool *)&g_sMetaComm.bMDLog, INI_BOOLEAN},

    //for C2K
    {"Enable Serial No", (bool*)&g_sMetaComm.EnableSerialNo, INI_BOOLEAN},
    {"Ignore Database Inconsistent", (bool*)&g_sMetaComm.IgnoreDBInconsistent, INI_BOOLEAN},
    {"Serial Num From IMEI", (bool*)&g_sMetaComm.SerialNumFromIMEI, INI_BOOLEAN},
    {"IMEI Lock Hidden", (bool*)&g_sMetaComm.ImeiLockHidden, INI_BOOLEAN},
    {"Enable Lock OTP", (bool*)&g_sMetaComm.bEnableLockOtp, INI_BOOLEAN},
};

//add by wzb for customconfig ini 20190321
char g_pCustomConfigFilepath[MAX_PATH];
IniData_struct g_CustomConfigIniData[] =
{
	{"DB_Machine", (char*)g_sMetaComm.strDBMachine, INI_STRING},
	{"DB_SQLUser", (char*)g_sMetaComm.strDBSQLUser, INI_STRING},
	{"DB_SQLPassword", (char*)g_sMetaComm.strDBSQLPassword, INI_STRING},
	{"DB_Port", (char*)g_sMetaComm.strDBPort, INI_STRING},
	{"DB_Name", (char*)g_sMetaComm.strDBName, INI_STRING},
	{"SoftwarePN", (char*)g_sMetaComm.strSoftwarePN, INI_STRING},
};
//end

bool ResultToString_Win(DWORD ED, char* lpBuffer, DWORD nSize)
{
    DWORD ret_dw = 0u;
    DWORD i_dw = 0u, j_dw = 0u;

    if (lpBuffer == NULL || nSize < 64u)
        return false;

    ret_dw = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        ED,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        lpBuffer,
        nSize,
        NULL);
    if (ret_dw == 0u)
        return false;

    // remove '\r' '\n'
    j_dw = 0u;
    for (i_dw = 0u; i_dw < ret_dw && lpBuffer[i_dw] != '\0'; i_dw++)
    {
        if (lpBuffer[i_dw] != '\r' && lpBuffer[i_dw] != '\n')
        {
            if (j_dw == i_dw)
                j_dw++;
            else
                lpBuffer[j_dw++] = lpBuffer[i_dw];
        }
    }
    lpBuffer[j_dw] = '\0';

    return true;
}

const char * ResultToString_SP(int errCode)
{
    // content is not right, so instead use modem meta API
    //return SP_META_GetErrorString((META_RESULT)errCode);
    return META_GetErrorString((META_RESULT)errCode);
}

const char * ResultToString(int errCode)
{
    return META_GetErrorString((META_RESULT)errCode);
}

int InitSNIni()
{
    int initStatus = 0;

    InitGlobalValue();

    RW_SetupFile_Status status = INI_RW_SUCCESS;
    status = GetParaFromSetupFile();
    if (status == INI_RW_SUCCESS)
    {
        initStatus = 1;
    }
    else
    {
        initStatus = 0;
    }

    AuthHandle_Init();

    return initStatus;

}

bool AuthHandle_Init()
{
    bool bAuthHandleInitDone = true;

    if(SPATE_AUTH_Create() == false)
        bAuthHandleInitDone = false;
    else if (g_sMetaComm.sLoadFile.strSPAuthPath[0] != '\0')
        SPATE_AUTH_Load(g_sMetaComm.sLoadFile.strSPAuthPath);

    if (SPATE_SCERT_Create() == false)
        bAuthHandleInitDone = false;

    return bAuthHandleInitDone;
}

void AuthHandle_DeInit()
{
    if (g_sMetaComm.sLoadFile.strSPAuthPath[0] != '\0')
        SPATE_AUTH_Unload();
    SPATE_AUTH_Destroy();

    if (false)
        SPATE_SCERT_Unload();
    SPATE_SCERT_Destroy();
}

bool SPAuthInit(const char *strSPAuthFilePath)
{
    if (g_sMetaComm.sLoadFile.strSPAuthPath[0] != '\0')
        SPATE_AUTH_Unload();

    if (!strSPAuthFilePath || strlen(strSPAuthFilePath) == 0)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "Invalid SP auth file path!!");
        return false;
    }

    return SPATE_AUTH_Load(strSPAuthFilePath);
}

bool SPScertInit(const char *strSPScertFilePath)
{
    if (false)
        SPATE_SCERT_Unload();

    if (!strSPScertFilePath || strlen(strSPScertFilePath) == 0)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "Invalid SP Scert file path!!");
        return false;
    }

    return SPATE_SCERT_Load(strSPScertFilePath);
}

E_TARGET_TYPE GetTargetType()
{
    return g_sMetaComm.eTargetType;
}

bool CheckFileSize(const char *strFilePath, const long fileSize)
{
    long size = 0;
    FILE * pFile = NULL;

    if (fopen_s(&pFile, strFilePath, "rb") != 0 || pFile == NULL)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "Execution Failure: Open data file error!!");
        return false;
    }

    size = GetFilesize(pFile);
    if (size != fileSize)
    {
        MTRACE_ERR(g_hEBOOT_DEBUG, "Execution Failure: data file size error!!");
        return false;
    }

    return true;
}

long GetFilesize( FILE *fp )
{
    long int save_pos = 0;
    long size_of_file = 0;

    if(fp == NULL)
    {
        return 0;
    }


    /* Save the current position. */
    save_pos = ftell( fp );

    /* Jump to the end of the file. */
    fseek( fp, 0L, SEEK_END );

    /* Get the end position. */
    size_of_file = ftell( fp );

    /* Jump back to the original position. */
    fseek( fp, save_pos, SEEK_SET );

    return( size_of_file );
}


Login_Identify_e GetLoginIdentify()
{
    BYTE key[10]="03055030";
    BYTE strPassword[LOGIN_PASSWORD_MAX] = {0};
    Login_Identify_e Identify = UNKNOW_IDENTIFY;

    if(ReadDataFromFile(strPassword, LOGIN_PASSWORD_MAX, key, g_pPasswdFilepath) == S_DES_SUCCESS)
    {
        if(strPassword[0] == '0' && strPassword[1] == '0')
        {
            Identify = OPERATOR;
        }
        else if(strPassword[0] == '0' && strPassword[1] == '1')
        {
            Identify = ENGINEER;
        }
        else
        {
            Identify = UNKNOW_IDENTIFY;
        }
    }

    return Identify;
}

UINT SaveLoginPasswd(const char *strPasswd)
{
    bool bRet = false;
    BYTE key[10]="03055030";
    int iDatalen=0;
    char tmpStr[LOGIN_PASSWORD_MAX+2] = {0};

    tmpStr[0] = '0';
    tmpStr[1] = '1';
    strcat_s(tmpStr, strPasswd);
    if(WriteDataToFile((unsigned char*)tmpStr, LOGIN_PASSWORD_MAX, key, g_pPasswdFilepath)  == S_DES_SUCCESS)
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}

UINT SaveLoginIdentify(const Login_Identify_e identify)
{
    bool bRet = false;
    BYTE key[10]="03055030";
    int iDatalen=0;
    BYTE tmpStr[LOGIN_PASSWORD_MAX+2] = {0};

    if(ReadDataFromFile(tmpStr, LOGIN_PASSWORD_MAX, key, g_pPasswdFilepath) == S_DES_SUCCESS)
    {
        switch(identify)
        {
        case ENGINEER:
            tmpStr[0] = '0';
            tmpStr[1] = '1';
            break;

        case OPERATOR:
            tmpStr[0] = '0';
            tmpStr[1] = '0';
            break;

        default:
            break;
        }
    }
    else
    {
        return ERROR;
    }

    if(WriteDataToFile((unsigned char*)tmpStr, LOGIN_PASSWORD_MAX, key, g_pPasswdFilepath)  == S_DES_SUCCESS)
    {
        return OK;
    }
    else
    {
        return ERROR;
    }
}

void DisableUSBSwitch()
{
    if (g_hSwitchTool != NULL)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DisableUSBSwitch(): Quit UsbSwitchtool start...");
        DisableUSBSwitch(g_hSwitchTool);
        g_hSwitchTool = NULL;
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DisableUSBSwitch(): Quit UsbSwitchtool end...");
    }
    else
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::DisableUSBSwitch(): Had quit UsbSwitchtool.");
}

void EnableUSBSwitch()
{
    if(g_hSwitchTool == NULL)
    {
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::EnableUSBSwitch(): Enable UsbSwitchtool for dongle project start...");
        g_hSwitchTool = EnableUSBSwitch("-m");
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::EnableUSBSwitch(): Enable UsbSwitchtool for dongle project end...");
    }
    else
        MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::EnableUSBSwitch(): Had enabled UsbSwitchtool.");
}

void InitGlobalValue()
{
    memset(&g_sMetaComm, 0, sizeof(META_Common_struct));
    g_sMetaComm.sAuthOption.bAuthenEnable = false;
}

int GetSNSetupFilePath()
{
    char FolderPath[MAX_PATH];

    memset(&g_pSetupFilepath, 0, MAX_PATH);
    memset(&g_pAutoGenSetupFilepath, 0, MAX_PATH);
    memset(&g_pPasswdFilepath, 0, MAX_PATH);
	//add by wzb for customconfig 20190321
	memset(&g_pCustomConfigFilepath, 0, MAX_PATH);
	//end

    if(::GetModuleFileName(NULL, FolderPath, MAX_PATH) > 0)
        ::PathRemoveFileSpec(FolderPath);
    else
        ::GetCurrentDirectory(MAX_PATH, FolderPath);

	//add by wzb for customconfig 20190321
	sprintf_s(g_pCustomConfigFilepath, "%s\\%s", FolderPath, "CustomConfig.ini");
    if (strlen(g_pCustomConfigFilepath) == 0 || ::PathFileExists(g_pCustomConfigFilepath) == FALSE)
        return ERROR;
	//end
    sprintf_s(g_pSetupFilepath, "%s\\%s", FolderPath, "SN_Setup.ini");
    if (strlen(g_pSetupFilepath) == 0 || ::PathFileExists(g_pSetupFilepath) == FALSE)
        return ERROR;

#ifdef _AUTO_GEN_FEATURE_
    sprintf_s(g_pAutoGenSetupFilepath, "%s\\%s", FolderPath, "SN_AutoGen.ini");
    if (strlen(g_pAutoGenSetupFilepath) == 0 || ::PathFileExists(g_pAutoGenSetupFilepath) == FALSE)
        return ERROR;
#endif

    sprintf_s(g_pPasswdFilepath, "%s\\%s", FolderPath, "SNDATA");
    if (strlen(g_pPasswdFilepath) == 0 || ::PathFileExists(g_pPasswdFilepath) == FALSE)
        return ERROR;

    return OK;
}

RW_SetupFile_Status GetItemParaFromSetupFile(const char *strFilePath, LPCTSTR lpKeyName, void* lpValue, E_INI_DATA_TYPE dataType)
{
    RW_SetupFile_Status readIniStatus;
    IniData_struct iniData;
    iniData.dataType = dataType;
    iniData.pItemKeyWord = lpKeyName;
    iniData.pItemData = lpValue;

    readIniStatus = ReadIniItemData(strFilePath, iniData);
    return readIniStatus;
}

RW_SetupFile_Status SaveItemParaToSetupFile(const char *strFilePath, LPCTSTR lpKeyName, void* lpValue, E_INI_DATA_TYPE dataType)
{
    RW_SetupFile_Status writeIniStatus;
    IniData_struct iniData;
    iniData.dataType = dataType;
    iniData.pItemKeyWord = lpKeyName;
    iniData.pItemData = lpValue;

    writeIniStatus = WriteIniItemData(strFilePath, iniData);
    return writeIniStatus;
}

//add by wzb for customconfig 20190321
RW_SetupFile_Status GetCustomConfigParaFromSetupFile()
{
    RW_SetupFile_Status readIniStatus;
    int genSize = (sizeof(g_CustomConfigIniData) / sizeof(IniData_struct));
    readIniStatus = ReadIniData(g_pCustomConfigFilepath, g_CustomConfigIniData, genSize);
    return readIniStatus;
}

//end

RW_SetupFile_Status GetParaFromSetupFile()
{
    RW_SetupFile_Status readIniStatus;

#ifdef _AUTO_GEN_FEATURE_
    readIniStatus = GetAutoGenParaFromSetupFile();
    if (readIniStatus != INI_RW_SUCCESS)
    {
        return readIniStatus;
    }
#endif

//add by wzb for customconfig 20190321
	readIniStatus = GetCustomConfigParaFromSetupFile();
    if (readIniStatus != INI_RW_SUCCESS)
    {
        return readIniStatus;
    }

//end

    int size = (sizeof(g_IniData) / sizeof(IniData_struct));

    readIniStatus = ReadIniData(g_pSetupFilepath, g_IniData, size);
    if (readIniStatus != INI_RW_SUCCESS)
    {
        return readIniStatus;
    }

    switch (g_sMetaComm.sIMEIOption.iImeiNums)
    {
    case ONE_IMEI:
        g_sMetaComm.sIMEIOption.bDualIMEI = false;
        g_sMetaComm.sIMEIOption.bThreeIMEI = false;
        g_sMetaComm.sIMEIOption.bFourIMEI = false;
        break;

    case DUAL_IMEI:
        g_sMetaComm.sIMEIOption.bDualIMEI = true;
        g_sMetaComm.sIMEIOption.bThreeIMEI = false;
        g_sMetaComm.sIMEIOption.bFourIMEI = false;
        break;

    case THREE_IMEI:
        g_sMetaComm.sIMEIOption.bDualIMEI = false;
        g_sMetaComm.sIMEIOption.bThreeIMEI = true;
        g_sMetaComm.sIMEIOption.bFourIMEI = false;
        break;

    case FOUR_IMEI:
        g_sMetaComm.sIMEIOption.bDualIMEI = false;
        g_sMetaComm.sIMEIOption.bThreeIMEI = false;
        g_sMetaComm.sIMEIOption.bFourIMEI = true;
        break;

    default:
        g_sMetaComm.sIMEIOption.bDualIMEI = false;
        g_sMetaComm.sIMEIOption.bThreeIMEI = false;
        g_sMetaComm.sIMEIOption.bFourIMEI = false;
        break;
    }

    if (!g_sMetaComm.bEnableLockOtp)
        g_sMetaComm.sIMEIOption.bLockOtp = false;

    return INI_RW_SUCCESS;
}

RW_SetupFile_Status SaveParaToSetupFile()
{
    RW_SetupFile_Status writeIniStatus;
#ifdef _AUTO_GEN_FEATURE_
    writeIniStatus = SaveAutoGenParaToSetupFile();
    if (writeIniStatus != INI_RW_SUCCESS)
    {
        return writeIniStatus;
    }
#endif

    int size = (sizeof(g_IniData) / sizeof(IniData_struct));

    writeIniStatus = WriteIniData(g_pSetupFilepath, g_IniData, size);

    return writeIniStatus;
}

#ifdef _AUTO_GEN_FEATURE_
RW_SetupFile_Status GetAutoGenParaFromSetupFile()
{
    RW_SetupFile_Status readIniStatus;
    int genSize = (sizeof(g_AutoGenSetup) / sizeof(IniData_struct));
    readIniStatus = ReadIniData(g_pAutoGenSetupFilepath, g_AutoGenSetup, genSize);
    return readIniStatus;
}

RW_SetupFile_Status SaveAutoGenParaToSetupFile()
{
    RW_SetupFile_Status writeIniStatus;
    int genSize = (sizeof(g_AutoGenSetup) / sizeof(IniData_struct));
    writeIniStatus = WriteIniData(g_pAutoGenSetupFilepath, g_AutoGenSetup, genSize);
    return writeIniStatus;
}

RW_SetupFile_Status UpdateAutoGenParaToSetupFile()
{
    AutoGenResult_e genResult = AutoGen_Success;

    if (g_sMetaComm.sWriteOption.bWriteBarcode && g_AutoGenData.bAutoGenBarcode)
    {
        if (g_AutoGenData.eIncreaseRadix == HEX_RADIX)
        {
            genResult = AutoIncrease(AutoGen_Barcode, g_AutoGenData.sBarcode.iStep, g_AutoGenData.sBarcode.Next, BARCODE_ARRAY_LEN);
        }
        else if (g_AutoGenData.eIncreaseRadix == DENARY_RADIX)
        {
            genResult = AutoIncrease_Ex(AutoGen_Barcode, g_AutoGenData.sBarcode.iStep, g_AutoGenData.sBarcode.Next, BARCODE_ARRAY_LEN, AutoGen_BARC_DENARY_OPTION);
        }

        if (genResult == AutoGen_Success)
        {
            SaveItemParaToSetupFile(g_pAutoGenSetupFilepath, "Barcode Next", g_AutoGenData.sBarcode.Next, INI_STRING);
        }
    }

    if (g_sMetaComm.sWriteOption.bWriteIMEI && g_AutoGenData.bAutoGenIMEI)
    {
        if (g_sMetaComm.sIMEIOption.bCheckSum)
        {
            genResult = AutoIncrease_Ex(AutoGen_IMEI, g_AutoGenData.sIMEI.iStep, g_AutoGenData.sIMEI.Next, IMEI_ARRAY_LEN, AutoGen_IMEI_CHECKSUM_OPTION);
        }
        else
        {
            genResult = AutoIncrease(AutoGen_IMEI, g_AutoGenData.sIMEI.iStep, g_AutoGenData.sIMEI.Next, IMEI_ARRAY_LEN);
        }

        if (genResult == AutoGen_Success)
        {
            SaveItemParaToSetupFile(g_pAutoGenSetupFilepath, "IMEI Next", g_AutoGenData.sIMEI.Next, INI_STRING);
        }
    }

    if (g_sMetaComm.sWriteOption.bWriteBT && g_AutoGenData.bAutoGenBT)
    {
        genResult = AutoIncrease(AutoGen_BT, g_AutoGenData.sBtAddress.iStep, g_AutoGenData.sBtAddress.Next, BT_ARRAY_LEN);
        if (genResult == AutoGen_Success)
        {
            SaveItemParaToSetupFile(g_pAutoGenSetupFilepath, "BT Next", g_AutoGenData.sBtAddress.Next, INI_STRING);
        }
    }

    if (g_sMetaComm.sWriteOption.bWriteWifi && g_AutoGenData.bAutoGenWifi)
    {
        genResult = AutoIncrease(AutoGen_Wifi, g_AutoGenData.sWifiAddress.iStep, g_AutoGenData.sWifiAddress.Next, WIFI_ARRAY_LEN);
        if (genResult == AutoGen_Success)
        {
            SaveItemParaToSetupFile(g_pAutoGenSetupFilepath, "Wifi Next", g_AutoGenData.sWifiAddress.Next, INI_STRING);
        }
    }

    if (g_sMetaComm.sWriteOption.bWriteEthernetMac && g_AutoGenData.bAutoGenEthernet)
    {
        genResult = AutoIncrease(AutoGen_Ethernet, g_AutoGenData.sEthernetAddress.iStep, g_AutoGenData.sEthernetAddress.Next, ETHERNET_MAC_ARRAY_LEN);
        if (genResult == AutoGen_Success)
        {
            SaveItemParaToSetupFile(g_pAutoGenSetupFilepath, "Ethernet Next", g_AutoGenData.sEthernetAddress.Next, INI_STRING);
        }
    }

    if (g_sMetaComm.sWriteOption.bWriteDrmkeyMCID && g_AutoGenData.bAutoGenDRMKeyMCID)
    {
        genResult = AutoIncrease(AutoGen_DRMKeyMCID, g_AutoGenData.sDRMKeyMCID.iStep, g_AutoGenData.sDRMKeyMCID.Next, DRMKEY_MCID_ARRAY_LEN);
        if (genResult == AutoGen_Success)
        {
            SaveItemParaToSetupFile(g_pAutoGenSetupFilepath, "DRMKey MCID Next", g_AutoGenData.sDRMKeyMCID.Next, INI_STRING);
        }
    }

    return INI_RW_SUCCESS;
}
#endif
