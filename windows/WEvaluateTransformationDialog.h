// WEvaluateTransformationDialog.h : header file
// 
#if !defined __WTRANDLG_H__
	#define	__WTRANDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMEvalTransformDialog dialog

class CMEvalTransformDialog : public CMDialog
{
// Construction
public:
						CMEvalTransformDialog(CWnd* pParent = NULL);	// standard constructor   		// standard desctructor 
						
	Boolean			DoDialog(
							EvaluateTransformSpecsPtr		evaluateTransformSpecsPtr);

// Dialog Data
	//{{AFX_DATA(CMEvalTransformDialog)
	enum { IDD = IDD_EvalTransform };
	BOOL	m_checkTransformationFlag;
	BOOL	m_listCheckMatrixFlag;
	BOOL	m_listEigenvaluesFlag;
	BOOL	m_listOriginalMatrixFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMEvalTransformDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckTransform();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void 				SetListCheckTransform(
							Boolean							checkTransformFlag);
	
	EvaluateTransformSpecsPtr		m_evaluateTransformSpecsPtr;
	Boolean		  						m_initializedFlag;
	                     		
};
  
#endif // !defined __WTRANDLG_H__
