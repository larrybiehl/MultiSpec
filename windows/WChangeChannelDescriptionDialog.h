#if !defined(AFX_WCCHNDLG_H__293B4E23_BDF3_11D3_8D70_00105AA88EE3__INCLUDED_)
#define AFX_WCCHNDLG_H__293B4E23_BDF3_11D3_8D70_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WCChnDlg.h : header file
//
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMChangeChannelDescriptionDlg dialog

class CMChangeChannelDescriptionDlg : public CMDialog
{
// Construction
public:
	CMChangeChannelDescriptionDlg(CWnd* pParent = NULL);   // standard constructor

	SInt16			DoDialog(
							FileInfoPtr							fileInfoPtr, 
							ChannelDescriptionPtr			channelDescriptionPtr, 
							float*								channelValuesPtr);

// Dialog Data
	//{{AFX_DATA(CMChangeChannelDescriptionDlg)
	enum { IDD = IDD_ChangeChannelDescriptions };
	CString	m_description;
	float	m_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMChangeChannelDescriptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void	Update(void);

	// Generated message map functions
	//{{AFX_MSG(CMChangeChannelDescriptionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNextChannelButton();
	afx_msg void OnPreviousChannelButton();
	virtual void OnOK();
	afx_msg void OnSetfocusDescription();
	afx_msg void OnSetfocusValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	ChannelDescriptionPtr			m_channelDescriptionPtr; 
	FileInfoPtr							m_fileInfoPtr;
	float*								m_channelValuesPtr;

	SInt16								m_channelIndex,
											m_lastSelectedTextItem;
		
	Boolean								m_changeFlag,
											m_initializedFlag;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WCCHNDLG_H__293B4E23_BDF3_11D3_8D70_00105AA88EE3__INCLUDED_)
