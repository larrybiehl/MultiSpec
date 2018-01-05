// WEvaluateCovarianceDialog.h : header file
//      
#if !defined __WCOVEDLG_H__
	#define	__WCOVEDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMEvalCovarianceDialog dialog

class CMEvalCovarianceDialog : public CMDialog
{
// Construction
public:
			CMEvalCovarianceDialog(CWnd* pParent = NULL);	// standard constructor      
	
						~CMEvalCovarianceDialog();		// standard desctructor 
						
	Boolean			DoDialog(
							EvaluateCovarianceSpecsPtr		evaluateCovarianceSpecsPtr);

// Dialog Data
	//{{AFX_DATA(CMEvalCovarianceDialog)
	enum { IDD = IDD_EvalCovariances };
	BOOL	m_listOriginalMatrixFlag;
	BOOL	m_listInvertedMatrixFlag;
	BOOL	m_listInvertedInvertedMatrixFlag;
	BOOL	m_listOriginalXInvertedMatrixFlag;
	BOOL	m_featureTransformationFlag; 
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMEvalCovarianceDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTransformation();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnSelendokClassCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	                       
	EvaluateCovarianceSpecsPtr		m_evaluateCovarianceSpecsPtr;
	Boolean		  						m_featureTransformAllowedFlag,
											m_initializedFlag;
	UInt16								m_numberEigenvectors;
	
};  
  
#endif // !defined __WCOVEDLG_H__
