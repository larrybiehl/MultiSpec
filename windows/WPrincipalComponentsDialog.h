// WPrincipalComponentsDialog.h : header file
//       
#if !defined __WPRINDLG_H__
	#define	__WPRINDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMPrincipalCompDialog dialog

class CMPrincipalCompDialog : public CMDialog
{
// Construction
public:
	CMPrincipalCompDialog(CWnd* pParent = NULL);		// standard constructor      
	
							~CMPrincipalCompDialog();		// standard desctructor 
	
	SInt16				DoDialog(void);

// Dialog Data
	//{{AFX_DATA(CMPrincipalCompDialog)
	enum { IDD = IDD_PrincipalComponent };
	int		m_dataCode;
	BOOL		m_trainingFlag;
	BOOL		m_testFlag;
	BOOL		m_listEigenvectorFlag;
	BOOL		m_equalVariancesFlag;
	BOOL		m_saveEigenvectorFlag;
	BOOL		m_textWindowFlag;
	BOOL		m_diskFileFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	void				HideFieldTypeItems();
	
	void				ShowFieldTypeItems();
	                                       
	
	Boolean			m_initializedFlag;

	// Generated message map functions
	//{{AFX_MSG(CMPrincipalCompDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClasses();
	afx_msg void OnImageArea();
	afx_msg void OnSelendokClassCombo();
	afx_msg void OnSelendokChannelCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};          
  
#endif // !defined __WPRINDLG_H__
