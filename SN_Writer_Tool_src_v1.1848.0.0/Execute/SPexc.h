#ifndef _SP_EXC_H_
#define _SP_EXC_H_

#include "SNbase.h"

#define  PreloaderUSB  1
#define  BootROMUSB   0
#define  MAX_MD_CHANNEL_NUM 8


typedef enum
{
    SP_MODEM_META = 0,
    SP_AP_META,
    SP_NOTIN_META = 256
}SP_METAMODE_e;

typedef struct
{
    unsigned int number_of_md;
    unsigned int active_md_idx;
    unsigned int current_mdtype;
    unsigned int mdimg_type[16];
    unsigned int number_of_mdSwImg;
    int          activeMdTypeIdx;
    unsigned int multi_md_capability_support;
}SP_MODEMInfo_s;

typedef struct r_buf
{
    char rd_buf[1000];

}ADB_READ_BUF;

class SmartPhoneSN : public SNBase
{
public:
    SmartPhoneSN();
    virtual ~SmartPhoneSN();

protected:
    virtual void ThreadMainEntryPoint();
    virtual void SPInit();
    virtual META_RESULT MetaHandle_Init();
    virtual META_RESULT ModemMetaHandle_Init();
    virtual META_RESULT APMetaHandle_Init();

    virtual void MetaHandle_DeInit();
    virtual void ModemMetaHandle_DeInit();
    virtual void APMetaHandle_DeInit();

    virtual void SetupMetaModeParameters();
    virtual int TryToOpenSPKernelComport(int KernelCom_Num);
    virtual int ConnectWithPreloader();
    virtual int ConnectWithKernelPort_Ex();

    virtual META_RESULT EnterAPMetaMode();
    virtual META_RESULT ExitAPMetaMode();

    virtual META_RESULT EnableModemMeta();
    virtual META_RESULT SwitchMDByIndex(UINT MDIndex);

    virtual int GetSPModemInfo_Ex();
    virtual META_RESULT QueryDBFromDUT();
    virtual META_RESULT GetAPDBFromDUT();
    virtual META_RESULT GetMDDBFromDUT();
    virtual META_RESULT LoadModemDatabase(int MDindex);
    virtual META_RESULT LoadAPDatabase();

    virtual META_RESULT REQ_ReadAP_NVRAM_Start(WriteData_Type_e dataType, char *pOutData, unsigned short iRID);
    virtual META_RESULT REQ_WriteAP_NVRAM_Start(WriteData_Type_e dataType, char *pInData, unsigned short iRID);
	virtual META_RESULT REQ_CountryCode_WriteAP_NVRAM_Start(char *pInData, unsigned short iRID);//add by wzb
    virtual META_RESULT REQ_ReadModem_NVRAM_Start(WriteData_Type_e dataType, char *pOutData, unsigned short iRID);
    virtual META_RESULT REQ_WriteModem_NVRAM_Start(WriteData_Type_e dataType, char *pInData, unsigned short iRID);

    virtual META_RESULT ConductProdInfoData(unsigned char *pBuf, int nBufLen);
    virtual META_RESULT REQ_WriteAP_PRODINFO_Start();
	virtual META_RESULT ReadSN_From_PRODINFO();//add by wzb

    virtual META_RESULT REQ_ReadFromAPNVRAM(AP_FT_NVRAM_READ_REQ *psNVRAM_ReadReq, AP_FT_NVRAM_READ_CNF *psNVRAM_ReadCnf);
    virtual META_RESULT REQ_WriteToAPNVRAM(AP_FT_NVRAM_WRITE_REQ sNVRAM_WriteReq);

    virtual int LoadHDCPBinFile(const char *strHdcpBinPath);
    int LoadDRMKeyFile(char * szDrmKeyFile);
    int RecycleDRMKeyFile(const char *szDrmKeyFile);
    virtual META_RESULT REQ_WriteHdcpBinToAPNvram_Start(const char *strHdcpBinPath);
    META_RESULT REQ_InstallDRMKey_Start();
    virtual META_RESULT REQ_InstallHdcpData_Start(const char *strHdcpDataFilePath, const char *strHdcpCekFilePath);

    virtual META_RESULT REQ_WriteDRMKeyMCID_Start(const char *strDrmKeyMCIDData);
    virtual META_RESULT REQ_ReadDRMKeyMCID_Start(char *strDrmKeyMCIDData);

    virtual META_RESULT REQ_InstallAttestationKey_Start(const char * strKeyFile);

    virtual META_RESULT GetNvramFileName(char * file_name, int buf_len);
    virtual META_RESULT REQ_BackupNvram2PC_Start(char *NumFile);

    virtual META_RESULT REQ_BackupNvram2BinRegion_Start();

    virtual int LockOTP();

    // For ROME DSDA Native Download and Backup
    //virtual int DSDA_QueryNativeDownload();
    //virtual META_RESULT DSDA_StartNativeDownload();
    virtual bool DSDA_ExternalModemBackup();

    virtual META_RESULT WriteNvramLoop();
	virtual META_RESULT WriteCountryCode();//add by wzb
    virtual int REQ_FastbootLockOem(void);
    virtual int REQ_FastbootUnLockOem(void);
    int Fastboot_Lock_Oem();
    bool Adb_Manager_Init();
    bool Adb_WR_Cmd(char *cmd_line,int cmd_length);
    int Adb_Detect_Devact();
    int Adb_Manager_Deinit();
    int Set_Fastboot_EnvironmentVariable(void);

    bool QueryEncryptionSupport(int * MetaHandle);
    bool VeritifiEncryption(int *MetaHandle);

    bool SetDutClock(int hApMeta);

    META_RESULT EnterC2KGen90();
    int EnterC2KGen93();
    int WriteMEID90();
    int WriteESN90();
    int WriteMEID93();

private:
    int Adb_Manager_Write(char *cmd_line,int cmd_length);
    int Adb_Manager_Read(ADB_READ_BUF *adb_read_buf,int buf_size);

private:
    MODEM_CAPABILITY_LIST_CNF m_SpMdCapList;
    SP_BOOT_ARG_S m_stModeArg;
    UINT m_nKernelComport;
    SP_METAMODE_e m_eMetaMode;
    SP_MODEMInfo_s m_sMdInfo;
    bool m_bWorldPhone;
    bool m_bDualModem;
    bool m_bWithoutMD;
    bool m_bWriteProdInfo;
    //For MT6595 DSDA project
    bool m_bDSDAProject;
    int m_iC2kProject;
    bool m_bInitExtMDdb;

    char m_strHdcpFileFullName[1024];
    UINT m_iMDChannelIndex[MAX_MD_CHANNEL_NUM];
    UINT m_iCurMDChanelIndex;
    UINT m_iLastPercentage;

    FILE_CUSTOM_HDCP_KEY_STRUCT m_sHDCPNvramStruct;
    FILE_CUSTOM_HDCP_KEY_STRUCT m_sReadBackHdcp;

    //dongck add for fastboot
    HANDLE hdle_write_cmd;
    HANDLE hdle_read_out;
    ADB_READ_BUF read_buf;
    int r_bufsize;
    //DEVICES_INFOS devices_infos;
};

META_MD_Query_Result_T __stdcall MdQueryHandler(void* MdQuery_CB_Arg);
int __stdcall MdTypeSwitchHandler(META_MDTYPE_Switch_Param_T mdtype_switch_param, void* MdTypeSwitch_CB_Arg);

#endif
