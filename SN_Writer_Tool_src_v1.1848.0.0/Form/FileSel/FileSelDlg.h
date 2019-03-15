#if !defined(AFX_FILESELDLG_H__90882514_7EAE_4849_AE73_BA114F9CE6CE__INCLUDED_)
#define AFX_FILESELDLG_H__90882514_7EAE_4849_AE73_BA114F9CE6CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileSelDlg dialog

#include "ShadeButtonST.h"

typedef enum
{
    LOAD_SP_AUTH_FILE = 0,
    LOAD_HDCP_BIN_FILE,
    LOAD_DRMKEY_FILE,
    LOAD_HDCP_DATA_FILE,
    LOAD_HDCP_CEK_FILE,
    LOAD_ATTESTATION_KEY_FILE,

    UNKOWN_FILETYPE = 256
}LoadFile_Type_e;

class CFileSelDlg : public CDialog
{
// Construction
public:
	CFileSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileSelDlg)
	enum { IDD = IDD_FILE_LOAD_DIALOG };
	CString	m_strDRMKeyPath;
	CString	m_strHdcpBinPath;
	CString	m_strHdcpCekPath;
	CString	m_strHdcpDataPath;
	CString	m_strSPAuthPath;
	CString m_strAttestationKeyPath;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileSelDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileSelDlg)
	afx_msg void OnBtnSelDrmkeyFile();
	afx_msg void OnBtnSelHdcpBinFile();
	afx_msg void OnBtnSelHdcpCekFile();
	afx_msg void OnBtnSelHdcpDatafile();
	afx_msg void OnBtnSelSpAuthFile();
	afx_msg void OnBtnSelAttestationKey();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CShadeButtonST m_LoadSPAuthBTN;
    CShadeButtonST m_LoadHdcpBinBTN;
    CShadeButtonST m_LoadDRMKeyBTN;
    CShadeButtonST m_LoadHdcpDataBTN;
    CShadeButtonST m_LoadHdcpCekBTN;
    CShadeButtonST m_LoadAttestationKeyBTN;
    CShadeButtonST m_LoadOKBTN;
    CShadeButtonST m_LoadCancelBTN;

    LoadFile_Type_e m_eFileType;

protected:
    void EnableUIItemByFileType(const LoadFile_Type_e eFileType);

public:
    void SetLoadFileType(const LoadFile_Type_e eFileType);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILESELDLG_H__90882514_7EAE_4849_AE73_BA114F9CE6CE__INCLUDED_)
