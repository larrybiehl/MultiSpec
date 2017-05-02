// WApdFDlg.h : header file
//                  
#if !defined __WAPDFDLG_H__
	#define	__WAPDFDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMAppendFileDialog dialog

class CMAppendFileDialog : public CMDialog
{
// Construction
public:
	CMAppendFileDialog(CWnd* pParent = NULL);	// standard constructor
	
	SInt16		DoDialog(
						FileInfoPtr					appendFileInfoPtr, 
						FileInfoPtr					newFileInfoPtr, 
						Boolean						modifyFlag,
						SInt64*						bytesToSkipPtr,
						Boolean						lineFlag,
						SInt32						newAfterLineChannel);

// Dialog Data
	//{{AFX_DATA(CMAppendFileDialog)
	enum { IDD = IDD_AppendFile };
	CString	m_appendFileName;
	int		m_lineChannel;
	long	m_lineChannelNumber;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMAppendFileDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnLineChannel();
	afx_msg void OnLastLineChannel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	FileInfoPtr			m_appendFileInfoPtr; 
							
	SInt64*				m_bytesToSkipPtr;
	                                         
	SInt32				m_maximumValue;
	 
	Boolean				m_initializedFlag,
							m_lineFlag,
							m_modifyFlag;
	
};  
  
#endif // !defined __WAPDFDLG_H__
