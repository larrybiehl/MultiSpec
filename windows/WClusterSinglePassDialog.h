// WClusterSinglePassDialog.h : header file
//                          
#if !defined __WCLSPDLG_H__
	#define	__WCLSPDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMSinglePassClusterDialog dialog

class CMSinglePassClusterDialog : public CMDialog
{
// Construction
public:
						CMSinglePassClusterDialog(CWnd* pParent = NULL);	// standard constructor       
	
						~CMSinglePassClusterDialog();		// standard desctructor   
						
	Boolean			DoDialog(void); 

// Dialog Data
	//{{AFX_DATA(CMSinglePassClusterDialog)
	enum { IDD = IDD_SinglePassCluster };
	double	m_criticalDistance1;
	double	m_criticalDistance2;
	long	m_minClusterSize;
	int		m_clustersFrom;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMSinglePassClusterDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokClassCombo();
	afx_msg void OnClusterTrainingAreas();
	afx_msg void OnClusterImageArea();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	void		ClustersFromSetting(
					CDataExchange* 	pDX, 
					int 					nIDC, 
					int& 					value);      
	
	Boolean				m_initializedFlag;

	UInt16				m_distanceDecimalDigits;
	
};    
  
#endif // !defined __WCLSPDLG_H__
