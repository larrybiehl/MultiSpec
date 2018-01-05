// WFieldsToThematicDialog.h : header file
//                  
#if !defined __WFTOTDLG_H__
	#define	__WFTOTDLG_H__    
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFieldsToThematicDialog dialog

class CMFieldsToThematicDialog : public CMDialog
{
// Construction
public:
	CMFieldsToThematicDialog(CWnd* pParent = NULL);	// standard constructor 
	
	Boolean			DoDialog(
							ReformatOptionsPtr				reformatOptionsPtr);

// Dialog Data
	//{{AFX_DATA(CMFieldsToThematicDialog)
	enum { IDD = IDD_AreasToThematicImage };
	BOOL	m_trainingAreasFlag;
	BOOL	m_testingAreasFlag;
	INT	m_outputFormatCode;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMFieldsToThematicDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	ReformatOptionsPtr		m_reformatOptionsPtr;
	
	Boolean						m_initializedFlag;
}; 
   
#endif	// !defined __WFTOTDLG_H__
