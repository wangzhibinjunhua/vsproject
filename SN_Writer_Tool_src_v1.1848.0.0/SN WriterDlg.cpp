// SN WriterDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "SN Writer.h"
#include "SN WriterDlg.h"
#include "SystemConfig.h"
#include "ScanData.h"
#include "BtnST.h"
#include "AboutSNDlg.h"
#include "ChgPasswdDlg.h"
#include "VerifyPasswdDlg.h"
#include "FileSelDlg.h"
#include "AutoGenDlg.h"

#include "Common.h"
#include "SNBase.h"
#include "HdcpEncryption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
#define  MAX_COMPORT_COUNT 255
unsigned short g_iTotalComPort = MAX_COMPORT_COUNT;
unsigned short g_iComportArray[MAX_COMPORT_COUNT] = {0};

const char *g_strToolVersion = "写国家码工具 V1.0.0";

extern CSNWriterDlg *g_pMainDlg;

/////////////////////////////////////////////////////////////////////////////
// CSNWriterDlg dialog

CSNWriterDlg::CSNWriterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSNWriterDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSNWriterDlg)
    m_strTotal = _T("");
    m_strPass = _T("");
    m_strFail = _T("");
	//add by wzb
	m_strTestItem = _T("#测试项");
	m_strTestItem1 = _T("1.查询工单号");
	m_strTestItem2 = _T("2.扫描SN码");
	m_strTestItem3 = _T("3.连接手机");
	m_strTestItem4 = _T("4.SN号比对");
	m_strTestItem5 = _T("5.核对手机状态");
	m_strTestItem6 = _T("6.写国家码");
	m_strTestItem7 = _T("7.断开手机连接");
	m_strTestItem8 = _T("8.上传数据库");
	m_strTestItemTime = _T("耗时");
	m_strTestItemInfo = _T("详细信息");
	m_strTestItemStatus = _T("状态");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSNWriterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSNWriterDlg)

    DDX_Control(pDX, IDC_COMBO_TARGET_TYPE, m_cTargetTypeBox);
    DDX_Control(pDX, IDC_COMBO_COMPORT, m_cComPortBox);
    DDX_Control(pDX, IDC_PROGRESS_SN, m_ProgressSN);
    DDX_Text(pDX, IDC_COUNT_TOTAL, m_strTotal);
    DDX_Text(pDX, IDC_COUNT_PASS, m_strPass);
    DDX_Text(pDX, IDC_COUNT_FAIL, m_strFail);
	
    //}}AFX_DATA_MAP

	//add by wzb
	DDX_Control(pDX, IDC_PROGRESS_STATUS1, m_ProgressStatus1);
	DDX_Control(pDX, IDC_PROGRESS_STATUS2, m_ProgressStatus2);
	DDX_Control(pDX, IDC_PROGRESS_STATUS3, m_ProgressStatus3);
	DDX_Control(pDX, IDC_PROGRESS_STATUS4, m_ProgressStatus4);
	DDX_Control(pDX, IDC_PROGRESS_STATUS5, m_ProgressStatus5);
	DDX_Control(pDX, IDC_PROGRESS_STATUS6, m_ProgressStatus6);
	DDX_Control(pDX, IDC_PROGRESS_STATUS7, m_ProgressStatus7);
	DDX_Control(pDX, IDC_PROGRESS_STATUS8, m_ProgressStatus8);
	DDX_Text(pDX, IDC_TV_TESTITEM, m_strTestItem);
	DDX_Text(pDX, IDC_TV_TESTITEM1, m_strTestItem1);
	DDX_Text(pDX, IDC_TV_TESTITEM2, m_strTestItem2);
	DDX_Text(pDX, IDC_TV_TESTITEM3, m_strTestItem3);
	DDX_Text(pDX, IDC_TV_TESTITEM4, m_strTestItem4);
	DDX_Text(pDX, IDC_TV_TESTITEM5, m_strTestItem5);
	DDX_Text(pDX, IDC_TV_TESTITEM6, m_strTestItem6);
	DDX_Text(pDX, IDC_TV_TESTITEM7, m_strTestItem7);
	DDX_Text(pDX, IDC_TV_TESTITEM8, m_strTestItem8);
	DDX_Text(pDX, IDC_TV_TESTITEM_STATUS, m_strTestItemStatus);
	DDX_Text(pDX, IDC_TV_TESTITEM_TIME, m_strTestItemTime);
	DDX_Text(pDX, IDC_TV_TESTITEM_INFO, m_strTestItemInfo);

    DDX_Control(pDX, IDC_BTN_INDITE, m_InditeBTN);

    DDX_Control(pDX, IDC_BTN_START, m_StartBTN);
    DDX_Control(pDX, IDC_BTN_QUIT, m_QuitBTN);
    DDX_Control(pDX, IDC_BTN_SYSCONFIG, m_SystemCfgBTN);
}

BEGIN_MESSAGE_MAP(CSNWriterDlg, CDialog)
    //{{AFX_MSG_MAP(CSNWriterDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
    ON_BN_CLICKED(IDC_BTN_QUIT, OnBtnQuit)
    ON_BN_CLICKED(IDC_BTN_SYSCONFIG, OnBtnSysconfig)
    ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, OnSelchangeComboComport)
    ON_CBN_SELCHANGE(IDC_COMBO_TARGET_TYPE, OnSelchangeComboTargetType)
    ON_COMMAND(IDR_ABOUT_SN, OnAboutSn)
    ON_COMMAND(IDR_ATCMD_MODE, OnAtcmdMode)
    ON_COMMAND(IDR_CHANGE_PASSWD, OnChangePasswd)
    ON_COMMAND(IDR_ENG_TO_OPERATER, OnEngToOperater)
    ON_COMMAND(IDR_OPTION_ALREADYINMETA, OnOptionAlreadyInMeta)
    ON_COMMAND(IDR_META_MODE, OnMetaMode)
    ON_COMMAND(IDR_OPERATOR_TO_ENG, OnOperatorToEng)
    ON_COMMAND(IDR_OPTION_DRMKEY, OnOptionDrmkey)
    ON_COMMAND(IDR_OPTION_EXTMODEM, OnOptionExtmodem)
    ON_COMMAND(IDR_OPTION_HDCP, OnOptionHdcp)
    ON_COMMAND(IDR_OPTION_SECURITY_USB, OnOptionSecurityUsb)
    ON_COMMAND(IDR_OPTION_SWITCHTOOL, OnOptionSwitchtool)
    ON_COMMAND(IDR_SP_AUTH_FILE, OnSpAuthFile)
    ON_COMMAND(IDR_OPTION_HDCPDATA, OnOptionHdcpdata)
    ON_COMMAND(IDR_OPTION_ATTESTATION, OnOptionAttestationKey)
    ON_COMMAND(IDR_ENABLE_AUTOGEN, OnEnableAutogen)
    ON_COMMAND(IDR_OPTION_CHECK_CAL_FLAG, OnOptionCheckCalFlag)
    ON_COMMAND(IDR_OPTION_CHECK_FT_FLAG, OnOptionCheckFtFlag)
    ON_COMMAND(IDR_OPTION_SKIP_WRITE_PRODINFO, OnOptionSkipWriteProdinfo)
    ON_COMMAND(IDR_CHECK_BTWIFI, OnCheckBtwifi)
    ON_COMMAND(IDR_OPTION_COMPOSITE_DEVICE, OnOptionCompositeDevice)
    ON_COMMAND(IDR_BACKNV_TOPC, OnBacknvTopc)
    ON_COMMAND(IDR_OEM_LOCK, OnFastbootOemLock)
    //}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_INDITE, &CSNWriterDlg::OnBnClickedBtnIndite)
	ON_EN_CHANGE(IDC_COUNT_TOTAL, &CSNWriterDlg::OnEnChangeCountTotal)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSNWriterDlg message handlers

BOOL CSNWriterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    //ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    // ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            //pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    SetWindowText(g_strToolVersion);

    // TODO: Add extra initialization here
    //g_MainDlgHandle = (DWORD)this;

    m_SNMenu.LoadMenu(IDR_SN_MENU);
    m_hSNMenu = m_SNMenu.GetSafeHmenu();

#ifdef _LOAD_SKIN_
    skinppSetDrawMenu(m_hSNMenu, TRUE);
#endif

    //m_hSNMenu = LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_SN_MENU));
    ::SetMenu(this->GetSafeHwnd(),m_hSNMenu);

    m_hBitmapFail = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_FAIL));
    m_hBitmapPass = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_PASS));
    ((CWnd*)GetDlgItem(IDC_BTN_INDITE))->ShowWindow(SW_HIDE);

#ifdef _BITMAP_BTN_
    m_InditeBTN.SetBitmap(m_hBitmapPass);
#else
    m_InditeBTN.SetShade(CShadeButtonST::SHS_METAL);
#endif

    m_StartBTN.SetShade(CShadeButtonST::SHS_SOFTBUMP);
    m_QuitBTN.SetShade(CShadeButtonST::SHS_SOFTBUMP);
    m_SystemCfgBTN.SetShade(CShadeButtonST::SHS_SOFTBUMP);

    // Get main dialog handle
    g_pMainDlg = (CSNWriterDlg*)this;
    ParameterInit();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSNWriterDlg::MenuItemInit()
{
    CMenu * pSubMenu = NULL;

    //sub menu - option
    pSubMenu = m_SNMenu.GetSubMenu(1);

    if (g_sMetaComm.bAlreadyInMeata)
    {
        pSubMenu->CheckMenuItem(IDR_OPTION_ALREADYINMETA, MF_CHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.eOperatorMode == META_MODE)
    {
        pSubMenu->GetSubMenu(1)->CheckMenuItem(IDR_META_MODE, MF_CHECKED | MF_BYCOMMAND);
        pSubMenu->GetSubMenu(1)->CheckMenuItem(IDR_ATCMD_MODE, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else if (g_sMetaComm.eOperatorMode == AT_MODE)
    {
        pSubMenu->GetSubMenu(1)->CheckMenuItem(IDR_ATCMD_MODE, MF_CHECKED | MF_BYCOMMAND);
        pSubMenu->GetSubMenu(1)->CheckMenuItem(IDR_META_MODE, MF_UNCHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.bSwithTool)
    {
        g_sMetaComm.bDualTalk = false;
        pSubMenu->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_CHECKED | MF_BYCOMMAND);
        pSubMenu->CheckMenuItem(IDR_OPTION_EXTMODEM, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else if (g_sMetaComm.bDualTalk)
    {
        g_sMetaComm.bSwithTool = false;
        pSubMenu->CheckMenuItem(IDR_OPTION_EXTMODEM, MF_CHECKED | MF_BYCOMMAND);
        pSubMenu->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_UNCHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.bSecurityUSB)
    {
        pSubMenu->CheckMenuItem(IDR_OPTION_SECURITY_USB, MF_CHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.bCheckCalFlag)
    {
        pSubMenu->GetSubMenu(6)->CheckMenuItem(IDR_OPTION_CHECK_CAL_FLAG, MF_CHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.bCheckFtFlag)
    {
        pSubMenu->GetSubMenu(6)->CheckMenuItem(IDR_OPTION_CHECK_FT_FLAG, MF_CHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.bSkipWriteProdInfo)
    {
        pSubMenu->CheckMenuItem(IDR_OPTION_SKIP_WRITE_PRODINFO , MF_CHECKED | MF_BYCOMMAND);
    }

    if (g_sMetaComm.bCompositeDeviceEnable)
    {
        pSubMenu->CheckMenuItem(IDR_OPTION_COMPOSITE_DEVICE, MF_CHECKED | MF_BYCOMMAND);
    }
    if (g_sMetaComm.bCheckBackNVtoPC)
    {
        pSubMenu->CheckMenuItem(IDR_BACKNV_TOPC, MF_CHECKED | MF_BYCOMMAND);
    }
    if (g_sMetaComm.bCheckFastbootLock)
    {
        pSubMenu->CheckMenuItem(IDR_OEM_LOCK, MF_CHECKED | MF_BYCOMMAND);
    }

    // sub menu - Hdcp_DrmKey
    pSubMenu = m_SNMenu.GetSubMenu(2);
    if (g_sMetaComm.eTargetType == TABLET_WIFI_ONLY)
    {
        pSubMenu->EnableMenuItem(IDR_OPTION_HDCP, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        pSubMenu->EnableMenuItem(IDR_OPTION_HDCPDATA, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
    }
    else
    {
        if (g_sMetaComm.sWriteOption.bWriteHdcp)
            pSubMenu->CheckMenuItem(IDR_OPTION_HDCP, MF_CHECKED | MF_BYCOMMAND);
        if (g_sMetaComm.sWriteOption.bInstallHdcpData)
            pSubMenu->CheckMenuItem(IDR_OPTION_HDCPDATA, MF_CHECKED | MF_BYCOMMAND);
    }
    if (g_sMetaComm.sWriteOption.bWriteDrm)
        pSubMenu->CheckMenuItem(IDR_OPTION_DRMKEY, MF_CHECKED | MF_BYCOMMAND);
    if (g_sMetaComm.sWriteOption.bWriteAttestationKey)
        pSubMenu->CheckMenuItem(IDR_OPTION_ATTESTATION, MF_CHECKED | MF_BYCOMMAND);

    //sub menu - identify
    pSubMenu = m_SNMenu.GetSubMenu(3);
    if (g_LoginIdentify == ENGINEER)
    {
        pSubMenu->CheckMenuItem(0, MF_CHECKED | MF_BYPOSITION);
        pSubMenu->CheckMenuItem(1, MF_UNCHECKED | MF_BYPOSITION);
        if (g_AutoGenData.bEnableAutoGen)
        {
            pSubMenu->GetSubMenu(0)->CheckMenuItem(IDR_ENABLE_AUTOGEN, MF_CHECKED | MF_BYCOMMAND);
        }
        else
        {
            pSubMenu->GetSubMenu(0)->CheckMenuItem(IDR_ENABLE_AUTOGEN, MF_UNCHECKED | MF_BYCOMMAND);
        }

        if (g_sMetaComm.bCheckBTWifi)
        {
            pSubMenu->GetSubMenu(0)->CheckMenuItem(IDR_CHECK_BTWIFI, MF_CHECKED | MF_BYCOMMAND);
        }
        else
        {
            pSubMenu->GetSubMenu(0)->CheckMenuItem(IDR_CHECK_BTWIFI, MF_UNCHECKED | MF_BYCOMMAND);
        }
    }
    else if (g_LoginIdentify == OPERATOR)
    {
        pSubMenu->CheckMenuItem(1, MF_CHECKED | MF_BYPOSITION);
        pSubMenu->CheckMenuItem(0, MF_UNCHECKED | MF_BYPOSITION);
    }
}

void CSNWriterDlg::ParameterInit()
{
    InitSNIni();

    MenuItemInit();

    EnableUIItem();

    SetUIItem();

}
void CSNWriterDlg::EnableStartBTN(bool bEnable)
{
    (CWnd*)GetDlgItem(IDC_BTN_START)->EnableWindow(bEnable);
}

void CSNWriterDlg::EnableUIItem()
{
    GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(TRUE);

    if (g_LoginIdentify == ENGINEER)
    {
        GetDlgItem(IDC_BTN_SYSCONFIG)->EnableWindow(TRUE);
        GetDlgItem(IDC_COMBO_TARGET_TYPE)->EnableWindow(TRUE);

        m_SNMenu.EnableMenuItem(0, MF_ENABLED | MF_BYPOSITION);
        m_SNMenu.EnableMenuItem(1, MF_ENABLED | MF_BYPOSITION);
        m_SNMenu.EnableMenuItem(2, MF_ENABLED | MF_BYPOSITION);
        m_SNMenu.EnableMenuItem(3, MF_ENABLED | MF_BYPOSITION);
        m_SNMenu.GetSubMenu(3)->EnableMenuItem(0, MF_ENABLED | MF_BYPOSITION);
        m_SNMenu.GetSubMenu(3)->EnableMenuItem(1, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
    }
    else
    {
        GetDlgItem(IDC_BTN_SYSCONFIG)->EnableWindow(FALSE);
        GetDlgItem(IDC_COMBO_TARGET_TYPE)->EnableWindow(FALSE);
        m_SNMenu.EnableMenuItem(0, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
        m_SNMenu.EnableMenuItem(1, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
        m_SNMenu.EnableMenuItem(2, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
        m_SNMenu.EnableMenuItem(3, MF_ENABLED | MF_BYPOSITION);
        m_SNMenu.GetSubMenu(3)->EnableMenuItem(0, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
        m_SNMenu.GetSubMenu(3)->EnableMenuItem(1, MF_ENABLED | MF_BYPOSITION);
    }

    SetDlgItemText(IDC_BTN_START, "Start");
}

void CSNWriterDlg::DisableUIItem()
{
    GetDlgItem(IDC_BTN_SYSCONFIG)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_TARGET_TYPE)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(FALSE);
    m_SNMenu.EnableMenuItem( 0, MF_GRAYED | MF_DISABLED | MF_BYPOSITION );
    m_SNMenu.EnableMenuItem( 1, MF_GRAYED | MF_DISABLED | MF_BYPOSITION );
    m_SNMenu.EnableMenuItem( 2, MF_GRAYED | MF_DISABLED | MF_BYPOSITION );
    m_SNMenu.EnableMenuItem(3, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
    m_SNMenu.GetSubMenu(3)->EnableMenuItem(0, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);
    m_SNMenu.GetSubMenu(3)->EnableMenuItem(1, MF_GRAYED | MF_DISABLED | MF_BYPOSITION);

    SetDlgItemText(IDC_BTN_START, "Stop");
}

void CSNWriterDlg::PopupMsgBox(const char* strTilte, UINT uType, const char* strMsg, ...)
{
    char szBuf[256] = "";

    va_list varg;
    va_start(varg, strMsg);
    vsprintf_s(szBuf, strMsg, varg);
    va_end(varg);
    MessageBox(strMsg, strTilte, uType);
}

void CSNWriterDlg::GetIndicatorPos(CRect &rect)
{
    ((CWnd*)GetDlgItem(IDC_BTN_INDITE))->GetWindowRect(&rect);
}

void CSNWriterDlg::UpdateOperateResult(WriteNvram_Status_e eWriteStatus)
{
#ifdef _BITMAP_BTN_
    switch (eWriteStatus)
    {
    case FAIL:
        ((CWnd*)GetDlgItem(IDC_BTN_INDITE))->ShowWindow(SW_SHOW);
        m_InditeBTN.SetBitmap(m_hBitmapFail);
        break;

    case PASS:
        ((CWnd*)GetDlgItem(IDC_BTN_INDITE))->ShowWindow(SW_SHOW);
        m_InditeBTN.SetBitmap(m_hBitmapPass);
        break;

    case NORMAL:
        ((CWnd*)GetDlgItem(IDC_BTN_INDITE))->ShowWindow(SW_HIDE);
        break;

    case CANCEL:
    default:
        //Do nothing
        break;
    }
#endif
}

void CSNWriterDlg::GetUIItem()
{
    OnSelchangeComboComport();
    OnSelchangeComboTargetType();
}

void CSNWriterDlg::SwitchToEngineer()
{
    g_LoginIdentify = ENGINEER;
    SaveLoginIdentify(ENGINEER);
    EnableUIItem();
}

void CSNWriterDlg::SwitchToOperator()
{
    g_LoginIdentify = OPERATOR;
    SaveLoginIdentify(OPERATOR);
    EnableUIItem();
}

void CSNWriterDlg::GetComPortDynamiclly()
{
    char DestString[10];
    int i = 0, result = 0;

    result = SP_ComPortEnumerate(g_iComportArray, &g_iTotalComPort);
    if(result != SP_S_DONE)
    {
        g_iTotalComPort = 0;
    }

    if(g_iTotalComPort < 1 || g_iTotalComPort >= MAX_COMPORT_COUNT)
    {
        m_cComPortBox.InsertString(0, "USB VCOM");
    }
    else
    {
        for(i = 0; i < g_iTotalComPort; i++)	// show all com port got dynamiclly
        {
            sprintf_s(DestString, "COM%d", g_iComportArray[g_iTotalComPort - i - 1]);
            m_cComPortBox.InsertString(0, DestString);
        }

        m_cComPortBox.InsertString(0, "USB VCOM");
    }

    m_cComPortBox.SetCurSel(0);

}

void CSNWriterDlg::SetUIItem()
{
    GetComPortDynamiclly();

    m_cTargetTypeBox.InsertString(SMART_PHONE, "Smart Phone");
    m_cTargetTypeBox.InsertString(SMART_PHONE_DUALMODEM, "SP DualModem");
    m_cTargetTypeBox.InsertString(TABLET_WIFI_ONLY, "Tablet Wifi Only");
    m_cTargetTypeBox.InsertString(RNDIG_DONGLE, "Rndis Dongle");
    m_cTargetTypeBox.SetCurSel(g_sMetaComm.eTargetType);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSNWriterDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSNWriterDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CSNWriterDlg::OnBtnStart()
{
    // TODO: Add your control notification handler code here
    EnableStartBTN(false);

    OnSelchangeComboTargetType();
    OnSelchangeComboComport();
    E_TARGET_TYPE eTargetType = GetTargetType();

    CString strText;
    ((CWnd*)GetDlgItem(IDC_BTN_START))->GetWindowText(strText);

    if (strText == "Start")
    {
        m_spSN.SNThread_Start();
    }
    else if(strText == "Stop")
    {
        m_spSN.SNThread_Stop();
        EnableUIItem();
    }
}

void CSNWriterDlg::OnBtnQuit()
{
    // TODO: Add your control notification handler code here
    CDialog::OnCancel();
}

BOOL CSNWriterDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
    {
        return TRUE;
    }
    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSNWriterDlg::DestroyWindow()
{
    // TODO: Add your specialized code here and/or call the base class

    SaveParaToSetupFile();
    AuthHandle_DeInit();

    return CDialog::DestroyWindow();
}

void CSNWriterDlg::OnBtnSysconfig()
{
    // TODO: Add your control notification handler code here
    CSystemConfig dlg;
    dlg.DoModal();
}

void CSNWriterDlg::OnSelchangeComboComport()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);

    CString strComport;
    m_cComPortBox.GetWindowText(strComport);
    if(strComport.Find("USB", 0) != -1)
    {
        g_sMetaComm.bUsbEnable = true;
        g_sMetaComm.iCOMPort = 0;
    }
    else
    {
        char *pStr = (LPSTR)(LPCTSTR)strComport;
        g_sMetaComm.bUsbEnable = false;
        pStr += 3;
        g_sMetaComm.iCOMPort = atoi(pStr);
    }
}

void CSNWriterDlg::OnSelchangeComboTargetType()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    g_sMetaComm.eTargetType =(E_TARGET_TYPE)m_cTargetTypeBox.GetCurSel();

    if (g_sMetaComm.eTargetType == RNDIG_DONGLE)
    {
        g_sMetaComm.bSwithTool = true;
        g_sMetaComm.bDualTalk = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_CHECKED | MF_BYCOMMAND);    // USB SwithTool option
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_EXTMODEM, MF_UNCHECKED | MF_BYCOMMAND);    // Extern modem option
    }
    else
    {
        g_sMetaComm.bSwithTool = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_UNCHECKED | MF_BYCOMMAND);  // USB SwithTool
    }

    if(g_sMetaComm.eTargetType == TABLET_WIFI_ONLY)
    {
        g_sMetaComm.sWriteOption.bWriteHdcp = false;
        g_sMetaComm.sWriteOption.bInstallHdcpData = false;
        m_SNMenu.GetSubMenu(2)->EnableMenuItem(IDR_OPTION_HDCP, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        m_SNMenu.GetSubMenu(2)->EnableMenuItem(IDR_OPTION_HDCPDATA, MF_GRAYED | MF_DISABLED | MF_BYCOMMAND);
        m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_HDCP, MF_UNCHECKED);
        m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_HDCPDATA, MF_UNCHECKED);
    }
    else
    {
        m_SNMenu.GetSubMenu(2)->EnableMenuItem(IDR_OPTION_HDCP, MF_ENABLED | MF_BYCOMMAND);
        m_SNMenu.GetSubMenu(2)->EnableMenuItem(IDR_OPTION_HDCPDATA, MF_ENABLED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnAboutSn()
{
    // TODO: Add your command handler code here
    CAboutSNDlg dlg;
    dlg.DoModal();
}

void CSNWriterDlg::OnChangePasswd()
{
    // TODO: Add your command handler code here
    CChgPasswdDlg dlg;
    dlg.DoModal();
}

void CSNWriterDlg::OnEngToOperater()
{
    // TODO: Add your command handler code here
    SwitchToOperator();
}

void CSNWriterDlg::OnOperatorToEng()
{
    // TODO: Add your command handler code here
    CVerifyPasswdDlg dlg;
    if(IDOK == dlg.DoModal())
    {
        SwitchToEngineer();
    }
}

void CSNWriterDlg::OnOptionAlreadyInMeta()
{
    if (g_sMetaComm.bAlreadyInMeata)
    {
        g_sMetaComm.bAlreadyInMeata = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_ALREADYINMETA, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bAlreadyInMeata = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_ALREADYINMETA, MF_CHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnMetaMode()
{
    // TODO: Add your command handler code here
    g_sMetaComm.eOperatorMode = META_MODE;
    m_SNMenu.GetSubMenu(1)->GetSubMenu(1)->CheckMenuItem(IDR_META_MODE, MF_CHECKED | MF_BYCOMMAND);
    m_SNMenu.GetSubMenu(1)->GetSubMenu(1)->CheckMenuItem(IDR_ATCMD_MODE, MF_UNCHECKED | MF_BYCOMMAND);
}

void CSNWriterDlg::OnAtcmdMode()
{
    // TODO: Add your command handler code here

    //Current only support meta mode
    g_sMetaComm.eOperatorMode = META_MODE;
    m_SNMenu.GetSubMenu(1)->GetSubMenu(1)->CheckMenuItem(IDR_META_MODE, MF_CHECKED | MF_BYCOMMAND);
    m_SNMenu.GetSubMenu(1)->GetSubMenu(1)->CheckMenuItem(IDR_ATCMD_MODE, MF_UNCHECKED | MF_BYCOMMAND);

    MessageBox("Current only support meta mode!", "Warning", MB_ICONWARNING);

    /*
    g_sMetaComm.eOperatorMode = AT_MODE;
    m_SNMenu.GetSubMenu(1)->GetSubMenu(1)->CheckMenuItem(0, MF_UNCHECKED | MF_BYPOSITION);
    m_SNMenu.GetSubMenu(1)->GetSubMenu(1)->CheckMenuItem(1, MF_CHECKED | MF_BYPOSITION);
    */
}

void CSNWriterDlg::OnOptionExtmodem()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bDualTalk)
    {
        g_sMetaComm.bDualTalk = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_EXTMODEM, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bSwithTool = false;
        g_sMetaComm.bDualTalk = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_EXTMODEM, MF_CHECKED | MF_BYCOMMAND);
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_UNCHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnOptionSwitchtool()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bSwithTool)
    {
        g_sMetaComm.bSwithTool = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bSwithTool = true;
        g_sMetaComm.bDualTalk = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SWITCHTOOL, MF_CHECKED | MF_BYCOMMAND);
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_EXTMODEM, MF_UNCHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnOptionSecurityUsb()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bSecurityUSB)
    {
        g_sMetaComm.bSecurityUSB = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SECURITY_USB, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bSecurityUSB = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SECURITY_USB, MF_CHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnOptionCheckCalFlag()
{
    // TODO: Add your command handler code here
    if(g_sMetaComm.bCheckCalFlag)
    {
        g_sMetaComm.bCheckCalFlag = false;
        m_SNMenu.GetSubMenu(1)->GetSubMenu(6)->CheckMenuItem(IDR_OPTION_CHECK_CAL_FLAG, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bCheckCalFlag = true;
        m_SNMenu.GetSubMenu(1)->GetSubMenu(6)->CheckMenuItem(IDR_OPTION_CHECK_CAL_FLAG, MF_CHECKED | MF_BYCOMMAND);
    }

}

void CSNWriterDlg::OnOptionCheckFtFlag()
{
    // TODO: Add your command handler code here
    if(g_sMetaComm.bCheckFtFlag)
    {
        g_sMetaComm.bCheckFtFlag = false;
        m_SNMenu.GetSubMenu(1)->GetSubMenu(6)->CheckMenuItem(IDR_OPTION_CHECK_FT_FLAG, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bCheckFtFlag = true;
        m_SNMenu.GetSubMenu(1)->GetSubMenu(6)->CheckMenuItem(IDR_OPTION_CHECK_FT_FLAG, MF_CHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnOptionSkipWriteProdinfo()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bSkipWriteProdInfo)
    {
        g_sMetaComm.bSkipWriteProdInfo = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SKIP_WRITE_PRODINFO, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bSkipWriteProdInfo = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SKIP_WRITE_PRODINFO, MF_CHECKED | MF_BYCOMMAND);
    }
}


void CSNWriterDlg::OnSpAuthFile()
{
    // TODO: Add your command handler code here
    CFileSelDlg dlg;
    dlg.SetLoadFileType(LOAD_SP_AUTH_FILE);
    if (IDOK == dlg.DoModal())
    {
        g_sMetaComm.bSecurityUSB = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_SECURITY_USB, MF_CHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnOptionHdcp()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.sWriteOption.bWriteHdcp)
    {
        g_sMetaComm.sWriteOption.bWriteHdcp = false;
        m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_HDCP, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        CFileSelDlg dlg;
        dlg.SetLoadFileType(LOAD_HDCP_BIN_FILE);
        if (IDOK == dlg.DoModal())
        {
            g_sMetaComm.sWriteOption.bWriteHdcp = true;
            m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_HDCP, MF_CHECKED | MF_BYCOMMAND);
        }
    }
}

void CSNWriterDlg::OnOptionDrmkey()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.sWriteOption.bWriteDrm)
    {
        g_sMetaComm.sWriteOption.bWriteDrm = false;
        m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_DRMKEY, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        CFileSelDlg dlg;
        dlg.SetLoadFileType(LOAD_DRMKEY_FILE);
        if (IDOK == dlg.DoModal())
        {
            g_sMetaComm.sWriteOption.bWriteDrm = true;
            m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_DRMKEY, MF_CHECKED | MF_BYCOMMAND);
        }
    }
}

void CSNWriterDlg::OnOptionHdcpdata()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.sWriteOption.bInstallHdcpData)
    {
        g_sMetaComm.sWriteOption.bInstallHdcpData = false;
        m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_HDCPDATA, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        CFileSelDlg dlg;
        dlg.SetLoadFileType(LOAD_HDCP_DATA_FILE);
        if (IDOK == dlg.DoModal())
        {
            g_sMetaComm.sWriteOption.bInstallHdcpData = true;
            m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_HDCPDATA, MF_CHECKED | MF_BYCOMMAND);
        }
    }
}

void CSNWriterDlg::OnEnableAutogen()
{
    // TODO: Add your command handler code here
    CAutoGenDlg dlg;
    dlg.DoModal();
    if (g_AutoGenData.bEnableAutoGen)
    {
        m_SNMenu.GetSubMenu(3)->GetSubMenu(0)->CheckMenuItem(IDR_ENABLE_AUTOGEN, MF_CHECKED | MF_BYCOMMAND);
    }
    else
    {
        m_SNMenu.GetSubMenu(3)->GetSubMenu(0)->CheckMenuItem(IDR_ENABLE_AUTOGEN, MF_UNCHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnCheckBtwifi()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bCheckBTWifi)
    {
        g_sMetaComm.bCheckBTWifi = false;
        m_SNMenu.GetSubMenu(3)->GetSubMenu(0)->CheckMenuItem(IDR_CHECK_BTWIFI, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bCheckBTWifi = true;
        m_SNMenu.GetSubMenu(3)->GetSubMenu(0)->CheckMenuItem(IDR_CHECK_BTWIFI, MF_CHECKED | MF_BYCOMMAND);
    }

}

void CSNWriterDlg::OnOptionCompositeDevice()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bCompositeDeviceEnable)
    {
        g_sMetaComm.bCompositeDeviceEnable = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_COMPOSITE_DEVICE, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bCompositeDeviceEnable = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OPTION_COMPOSITE_DEVICE, MF_CHECKED | MF_BYCOMMAND);
    }
}

void CSNWriterDlg::OnBacknvTopc()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bCheckBackNVtoPC)
    {
        g_sMetaComm.bCheckBackNVtoPC = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_BACKNV_TOPC, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bCheckBackNVtoPC = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_BACKNV_TOPC, MF_CHECKED | MF_BYCOMMAND);
    }

}

void CSNWriterDlg::OnFastbootOemLock()
{
    // TODO: Add your command handler code here
    if (g_sMetaComm.bCheckFastbootLock)
    {
        g_sMetaComm.bCheckFastbootLock = false;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OEM_LOCK, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        g_sMetaComm.bCheckFastbootLock = true;
        m_SNMenu.GetSubMenu(1)->CheckMenuItem(IDR_OEM_LOCK, MF_CHECKED | MF_BYCOMMAND);
    }

}

void CSNWriterDlg::OnOptionAttestationKey()
{
    if (g_sMetaComm.sWriteOption.bWriteAttestationKey)
    {
        g_sMetaComm.sWriteOption.bWriteAttestationKey = false;
        m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_ATTESTATION, MF_UNCHECKED | MF_BYCOMMAND);
    }
    else
    {
        CFileSelDlg dlg;
        dlg.SetLoadFileType(LOAD_ATTESTATION_KEY_FILE);
        if (dlg.DoModal() == IDOK)
        {
            g_sMetaComm.sWriteOption.bWriteAttestationKey = true;
            m_SNMenu.GetSubMenu(2)->CheckMenuItem(IDR_OPTION_ATTESTATION, MF_CHECKED | MF_BYCOMMAND);
        }
    }
}


void CSNWriterDlg::OnBnClickedBtnIndite()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CSNWriterDlg::OnEnChangeCountTotal()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CSNWriterDlg::OnEnChangeCountTotal2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
