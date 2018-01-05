// WClusterIsodataDialog.h : header file
//                     
#if !defined __WCLIDDLG_H__
	#define	__WCLIDDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMISODATAClusterDialog dialog

class CMISODATAClusterDialog : public CMDialog
{
// Construction
public:
						CMISODATAClusterDialog(CWnd* pParent = NULL);	// standard constructor       
	
						~CMISODATAClusterDialog();		// standard desctructor   
						
	Boolean			DoDialog(void);   

// Dialog Data
	//{{AFX_DATA(CMISODATAClusterDialog)
	enum { IDD = IDD_ISODATACluster };
	int		m_initializationOption; 
	double	m_criticalDistance1;
	double	m_criticalDistance2;
	long	m_numberClusters;
	long	m_minClusterSize;
	double	m_convergence; 
	int		m_clustersFrom; 
	BOOL	m_projectClassMeansFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMISODATAClusterDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokClassCombo();
	afx_msg void OnClusterTrainingAreas();
	afx_msg void OnClusterImageArea();
	afx_msg void On1stCovEigenvector();
	afx_msg void OnEigenvectorVolume();
	afx_msg void OnOnePassCluster();
	afx_msg void OnProjectClassMeans();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	void		ClustersFromSetting(
					CDataExchange* 	pDX, 
					int 					nIDC, 
					int& 					value);
					
	void 		UpdateOptionSettings(void);      
	
	Boolean				m_initializedFlag;

	UInt16				m_distanceDecimalDigits;
	
};   
  
#endif // !defined __WCLIDDLG_H__
