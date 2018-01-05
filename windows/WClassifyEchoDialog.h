// WClassifyEchoDialog.h : header file
//  
#if !defined __WECHODLG_H__
	#define	__WECHODLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMEchoClassifyDialog dialog

class CMEchoClassifyDialog : public CMDialog
{
// Construction
public:
	CMEchoClassifyDialog (CWnd* pParent = NULL);	// standard constructor    
	
						~CMEchoClassifyDialog();		// standard desctructor 
  
	
	Boolean			DoDialog (
							EchoClassifierVarPtr				echoClassifierVarPtr); 

// Dialog Data
	//{{AFX_DATA(CMEchoClassifyDialog)
	enum { IDD = IDD_EchoClassify };
	long		m_cellWidth;
	double	m_annexationThreshold;
	double	m_homogeneityThreshold;
	BOOL		m_combineLikeFieldsFlag;
	BOOL		m_createHomogeneousFilesFlag;
	BOOL		m_mixCellsFlag;
	int		m_homogeneousThresholdType;
	int		m_echoAlgorithmProcedure;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMEchoClassifyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPercent();
	afx_msg void OnLogLike();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	double						m_maxPhase1,
									m_minPhase1,
									m_savedPhase1LogLikelihoodThreshold,
									m_savedPhase1PercentThreshold; 
	
	EchoClassifierVarPtr		m_echoClassifierVarPtr;
	
	Boolean						m_initializedFlag;
	
};      
  
#endif // !defined __WECHODLG_H__
