// WStatisticsListDialog.h : header file
//
	
	#include "WDialog.h"

                        
/////////////////////////////////////////////////////////////////////////////
// CMListStatsDialog dialog

class CMListStatsDialog : public CMDialog
{
// Construction
public:
	CMListStatsDialog(CWnd* pParent = NULL);	// standard constructor 
	
	Boolean					DoDialog(
									SInt16									statsWindowMode);
									
	void						CheckListFieldClassSettings(void);

// Dialog Data
	//{{AFX_DATA(CMListStatsDialog)
	enum { IDD = IDD_ListStats };
	BOOL	m_listClassFlag;
	BOOL	m_listFieldFlag;
	BOOL	m_listMeansStdDevFlag;
	BOOL	m_listCovarianceFlag;
	BOOL	m_listCorrelationFlag;
	BOOL	m_featureTransformationFlag;
	UINT	m_listMeanStdPrecision;
	UINT	m_listCovCorPrecision;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMListStatsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnClasses();
	afx_msg void OnFields();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	SInt16				m_statsWindowMode;
	
};
