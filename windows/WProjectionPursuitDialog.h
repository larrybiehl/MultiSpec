// WReformatRectifyDlg.h : header file
//
#if !defined __WPROJECTIONPURSUITDLG_H__
	#define	__WPROJECTIONPURSUITDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMProjectionPursuitDialog dialog

class CMProjectionPursuitDialog : public CMDialog
{
public:
	CMProjectionPursuitDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMProjectionPursuitDialog();  // standard destructor

	Boolean			DoDialog (
				ProjectionPursuitSpecsPtr	projectionPursuitSpecsPtr); 

// Dialog Data
	//{{AFX_DATA(CMProjectionPursuitDialog)
	enum { IDD = IDD_ProjectionPursuit };
	BOOL		m_numericalOptimizationFlag;
	double	m_optimizeThreshold;
	double	m_topDownThreshold;
	double	m_bottomUpThreshold;
	long		m_bothOddChoicesNumberFeatures;
	long		m_maximumNumberFeatures;
	short		m_initialNumberFeatures;
	short		m_finalNumberFeatures;
	int		m_algorithmCode;
	int		m_initialGroupingCode;
	int		m_firstStageMethod;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMProjectionPursuitDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	ProjectionPursuitSpecsPtr		m_projectionPursuitSpecsPtr;

public:
	afx_msg void OnBnClickedAlgorithm();
	afx_msg void OnBnClickedNumericaloptimization();
	afx_msg void OnBnClickedInitialBandGrouping();
	afx_msg void OnBnClickedBandGroupingMethod();
};

#endif // !defined __WPROJECTIONPURSUITDLG_H__
