// WListResultsOptionsDialog.h : header file
//     
#if !defined __WLIOPDLG_H__
	#define	__WLIOPDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMListResultsOptionsDlg dialog

class CMListResultsOptionsDlg : public CMDialog
{
// Construction
public:
	CMListResultsOptionsDlg(CWnd* pParent = NULL);	// standard constructor  
	
	void				DoDialog(
							SInt16*			listResultsTrainingCodePtr, 
							SInt16*			listResultsTestCodePtr); 

// Dialog Data
	//{{AFX_DATA(CMListResultsOptionsDlg)
	enum { IDD = IDD_ResultsListOptions };
	BOOL	m_trainAreasUsedFlag;
	BOOL	m_trainAreaSummariesFlag;
	BOOL	m_trainAreaPerformanceFlag;
	BOOL	m_trainClassPerformanceFlag;
	BOOL	m_testAreasUsedFlag;
	BOOL	m_testAreaSummariesFlag;
	BOOL	m_testAreaPerformanceFlag;
	BOOL	m_testClassPerformanceFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMListResultsOptionsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()  
							
	SInt16				m_listResultsTestCode,
							m_listResultsTrainingCode;
};                        
  
#endif // !defined __WLIOPDLG_H__
