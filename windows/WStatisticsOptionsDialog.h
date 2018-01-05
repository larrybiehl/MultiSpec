// WStatisticsOptionsDialog.h : header file
//      
#if !defined __WSTATISTICS_OPTIONS_H__
	#define	__WSTATISTICS_OPTIONS_H__  
	
	#include "WDialog.h"   


/////////////////////////////////////////////////////////////////////////////
// CMStatOptionsDlg dialog

class CMStatOptionsDlg : public CMDialog
{
// Construction
public:
	CMStatOptionsDlg(CWnd* pParent = NULL);	// standard constructor  
	
	SInt16			DoDialog(
							SInt16*			statCodePtr, 
							Boolean*			keepClassStatsFlagPtr, 
							double*			variancePtr,
							double*			minLogDeterminantPtr,
							Boolean*			useCommonCovarianceInLOOCFlagPtr);
			
// Dialog Data
	//{{AFX_DATA(CMStatOptionsDlg)
	enum { IDD = IDD_StatisticsOptionsDialog };
	BOOL	m_classStatsOnlyFlag;
	BOOL	m_setZeroVarianceFlag;
	int		m_statCode;
	double	m_minLogDetValue;
	double	m_varianceValue;
	BOOL	m_useCommonCovarianceInLOOCFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMStatOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMeanStd();
	afx_msg void OnSetZeroVariance();
	afx_msg void OnMeanStdCov();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	void			SetZeroVariance(void);
	
};

#endif // !defined __WSTATISTICS_OPTIONS_H__ 
