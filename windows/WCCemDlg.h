// WClsfDlg.h : header file
//               
#if !defined __WCCEMDLG_H__
	#define	__WCCEMDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMCEMClassifyDialog dialog

class CMCEMClassifyDialog : public CMDialog
{
// Construction
public:
	CMCEMClassifyDialog(CWnd* pParent = NULL);	// standard constructor 

	Boolean			DoDialog(
				CEMParametersPtr					cemParametersPtr,
				UInt16*								classifyProcedureEnteredCodePtr);

// Dialog Data
	//{{AFX_DATA(CMCEMClassifyDialog)
	enum { IDD = IDD_CEMParameters };
	int		m_correlationMatrixCode;
	BOOL	m_trainingFieldsFlag;
	BOOL	m_testFieldsFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMCEMClassifyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnUseSelectedArea();
	afx_msg void OnUseClasses();
	afx_msg void OnSelendokClassCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()  
	
	void 		BackgroundCorrelationSettings(
					Boolean										areaCode);
	
	CEMParametersPtr				m_cemParametersPtr; 
	
	SInt16							*m_correlationMatrixClassPtr,
										*m_localClassAreaPtr;
	
	UInt32							m_localNumberClassAreas;
	
	Boolean							m_initializedFlag;
	
};		// end "CMCEMClassifyDialog dialog"
#endif // !defined __WCCEMDLG_H__
