// SN Writer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Common.h"
#include "SN Writer.h"
#include "SN WriterDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSNWriterApp

BEGIN_MESSAGE_MAP(CSNWriterApp, CWinApp)
    //{{AFX_MSG_MAP(CSNWriterApp)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSNWriterApp construction

CSNWriterApp::CSNWriterApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSNWriterApp object

CSNWriterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSNWriterApp initialization

BOOL CSNWriterApp::InitInstance()
{
    // 如果一个运行在 Windows XP 上的应用程序清单指定要
    // 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
    //则需要 InitCommonControlsEx()。否则，将无法创建窗口。
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 将它设置为包括所有要在应用程序中使用的
    // 公共控件类。
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    // Get SN Write current work path & Login Identify
    if(GetSNSetupFilePath() != OK)
    {
        CString strInfo;
        strInfo.Format(_T("Get setup files fail, please confirm these files path:\r\n")
			
			_T("SN Setup, %s\r\n")
			_T("CustomConfig, %s\r\n")//add by wzb for customconfig 20190321
#ifdef _AUTO_GEN_FEATURE_
            _T("SN AutoGen, %s\r\n")
#endif
            _T("SNDATA, %s\r\n"),
           
            g_pSetupFilepath,
            g_pCustomConfigFilepath,//add by wzb for customconfig 20190321
#ifdef _AUTO_GEN_FEATURE_
            g_pAutoGenSetupFilepath,
#endif
            g_pPasswdFilepath);
        ::AfxMessageBox((const TCHAR *)strInfo, MB_ICONERROR);
        return FALSE;
    }
    g_LoginIdentify = GetLoginIdentify();
    if (g_LoginIdentify == UNKNOW_IDENTIFY)
    {
        ::AfxMessageBox("SNDATA file maybe destory.", MB_ICONERROR);
        return FALSE;
    }

    CSNWriterDlg dlg;
    m_pMainWnd = &dlg;
	
    int nResponse = dlg.DoModal();
	
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
       
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
        
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
