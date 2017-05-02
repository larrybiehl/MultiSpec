// wrfrmdlg.h : header file
//                
//                     
#if !defined __WRFRMDLG_H__
	#define	__WRFRMDLG_H__ 
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMReformatDlg dialog

class CMReformatDlg : public CMDialog
{
// Construction
public:
					CMReformatDlg(CWnd* pParent = NULL);	// standard constructor SInt16 
	
	SInt16		DoDialog(
						SInt16*					channelDescriptionRequestPtr);

// Dialog Data
	//{{AFX_DATA(CMReformatDlg)
	enum { IDD = IDD_Reformat };
	int		m_chanDescriptionSelection;
	int		m_reformatRequest;
	//}}AFX_DATA

// Implementation
protected:
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
						
//	Boolean			Initialize(void);

	// Generated message map functions
	//{{AFX_MSG(CMReformatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnModifyChanDescriptions();
	afx_msg void OnChangeImageFileFormat();
	afx_msg void OnConvertFieldsToThematic();
	afx_msg void OnENVIROIToThematic();
	afx_msg void OnHeader();
	afx_msg void OnMosaicImage();
	afx_msg void OnMultispectralToThematic();
	afx_msg void OnRecodeThematic();
	afx_msg void OnRectifyImage();
	afx_msg void OnSelendokChanDescriptionChoice();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	SInt16			m_actionCodeVector[9];
	
	SInt16			m_fileNamesSelection;
	
	Boolean			m_initializedFlag;
};                          
  
#endif // !defined __WRFRMDLG_H__
