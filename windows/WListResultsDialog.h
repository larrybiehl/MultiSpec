// WListResultsDialog.h : header file
//      
#if !defined __WLSTRDLG_H__
	#define	__WLSTRDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMListResultsDialog dialog

class CMListResultsDialog : public CMDialog
{
// Construction
public:
							CMListResultsDialog(CWnd* pParent = NULL);	// standard constructor      
	
							~CMListResultsDialog();		// standard desctructor  
	
	Boolean				DoDialog(void);

// Dialog Data
	//{{AFX_DATA(CMListResultsDialog)
	enum { IDD = IDD_ListResults };
	BOOL	m_trainingFlag;
	BOOL	m_testFlag;
	BOOL	m_imageFlag;
	BOOL	m_fieldSummaryFlag;
	BOOL	m_classSummaryFlag;
	BOOL	m_groupSummaryFlag;
	double	m_conversionFactor;
	BOOL	m_thresholdResultsFlag;
	double	m_thresholdPercent;
	BOOL	m_textWindowFlag;
	BOOL	m_diskFileFlag;
	int		m_tableType;
	int		m_tableUnits;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
	
	void					CheckListAreaItems(); 
	
	void					CheckSummaryItems(); 
	
//	void					HideShowProbabilityItems(
//								Boolean				thresholdResultsFlag);
	
	void					HideShowUnitsItems(
								Boolean				areaUnitsFlag);
	
	void					SetListAreaItems();   
	
	void					SetSummarizeByItems();

	// Generated message map functions
	//{{AFX_MSG(CMListResultsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnImage();
	afx_msg void OnSelendokClassCombo();
	afx_msg void OnTraining();
	afx_msg void OnTest();
	afx_msg void OnFieldSummary();
	afx_msg void OnClassSummary();
	afx_msg void OnGroupSummary();
	afx_msg void OnThresholdResults();
	afx_msg void OnSampleUnits();
	afx_msg void OnPercentUnits();
	afx_msg void OnAreaUnits();    
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	double		m_saveAreaConversionFactor,
					m_saveThresholdPercent;
	
	Boolean		m_initializedFlag,
					m_projectWindowFlag;
	
	SInt16		m_localAreaCode,
					m_localSummaryCode,
					m_nonClusterFieldTypeCode;
	
public:
};
  
#endif // !defined __WLSTRDLG_H__
