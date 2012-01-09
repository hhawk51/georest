
// GeoRestEasyConfDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CGeoRestEasyConfDlg dialog
class CGeoRestEasyConfDlg : public CDialog
{
// Construction
public:
	CGeoRestEasyConfDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GEORESTEASYCONF_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCreate();
  afx_msg void OnBnClickedBrowse();
  CString GetSourceType();
  CEdit m_CtrlSource;
  CEdit m_Ct_Map;
  CEdit m_Ct_Layer;
  afx_msg void OnBnClickedKml();
  afx_msg void OnEnKillfocusUripart();
  CEdit m_Ct_UriPart;
};
