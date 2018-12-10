// WListDataDialog.h : header file
// 
// Revised by Larry Biehl on 07/03/2018

#if !defined __WLSTDDLG_H__
	#define	__WLSTDDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMListDataDialog dialog

class CMListDataDialog : public CMDialog
{
// Construction
public:
	CMListDataDialog(CWnd* pParent = NULL);	// standard constructor      
	
							~CMListDataDialog();		// standard desctructor 
	
	SInt16				DoDialog(void);

// Dialog Data
	//{{AFX_DATA(CMListDataDialog)
	enum { IDD = IDD_ListData };
	BOOL	m_areaFlag;
	BOOL	m_classFlag;
	BOOL	m_diskFileFlag;
	BOOL	m_graphDataFlag;
	BOOL	m_localGraphDataFlag;
	BOOL	m_includeClassFieldFlag;
	BOOL	m_includeLineColumnFlag;
	BOOL	m_includeLatLongFlag;
	BOOL	m_textWindowFlag;
	BOOL	m_trainingFlag;
	BOOL	m_testFlag;
	int	m_listDataFormatCode;
	long	m_numberDecimalPlaces;
	//}}AFX_DATA

// Implementation
protected:
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	void				CheckClassItems(
							Boolean			listClassDataFlag);
							
	void				CheckOKButton (void);   
	
	Boolean			m_initializedFlag,
						m_latLongPossibleFlag;

	// Generated message map functions
	//{{AFX_MSG(CMListDataDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnSelendokClassCombo();
	afx_msg void OnClasses();
	afx_msg void OnArea();
	afx_msg void OnGraphData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnCbnSelendokListchannelsformatcombo();
	afx_msg void OnEnChangeEdit1();
};            
  
#endif // !defined __WLSTDDLG_H__
