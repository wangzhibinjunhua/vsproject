// AboutSNDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SN Writer.h"
#include "AboutSNDlg.h"
#include "SNBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutSNDlg dialog

CAboutSNDlg::CAboutSNDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutSNDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutSNDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAboutSNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutSNDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutSNDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutSNDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutSNDlg message handlers

BOOL CAboutSNDlg::OnInitDialog() 
{
    CString strVersion;

    CDialog::OnInitDialog();

    // SN Writer Version
    GetDlgItem(IDC_SN_VERSION)->SetWindowText(g_strToolVersion);

    // MetaCore.dll Version
    {
        unsigned int major, minor, build, patch;

        SP_META_GetDLLVer(&major, &minor, &build, &patch);
        strVersion.Format(_T("MetaCore Dll Ver: %d.%d.%d.%d"),
            major, minor, build, patch);
        GetDlgItem(IDC_METACOREDLL_VERSION)->SetWindowText(strVersion);
    }

    // MetaApp.dll Version, not used
    GetDlgItem(IDC_METAAPPDLL_VERSION)->SetWindowText(_T(""));

    // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
    return TRUE;
}
