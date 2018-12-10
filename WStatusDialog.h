// WStatusDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShortStatusDlg dialog 
	
	#include "WDialog.h"

class CShortStatusDlg : public CMDialog
{
// Construction
public:
						CShortStatusDlg(UInt16 identifier, CWnd* pParent = NULL);	// standard constructor
	
//	Boolean			Initialize(void); 
	
	Boolean			m_initializedFlag;

// Dialog Data
	//{{AFX_DATA(CShortStatusDlg)
	enum { IDD = IDD_GraphicsStatus };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void 	DoDataExchange (CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CShortStatusDlg)
	virtual void OnCancel ();
	virtual BOOL OnInitDialog ();
	afx_msg void OnMouseMove (UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove (UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Overrides
	void PostNcDestroy() ;

// Implementation 
private:

};


                        
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
