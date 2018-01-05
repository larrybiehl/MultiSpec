// WClusterDialog.h : header file
//     
#if !defined __WCLUSDLG_H__
	#define	__WCLUSDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMClusterDialog dialog

class CMClusterDialog : public CMDialog
{
// Construction
public:
	CMClusterDialog(CWnd* pParent = NULL);	// standard constructor     
	
						~CMClusterDialog();		// standard desctructor  
						
	Boolean			DoDialog(
							Boolean						newProjectFlag); 

// Dialog Data
	//{{AFX_DATA(CMClusterDialog)
	enum { IDD = IDD_Cluster };
	BOOL		m_thresholdFlag;
	BOOL		m_textWindowFlag;
	int		m_symbolSelection;
	int		m_clusterProcedure;
	double	m_probabilityThreshold;
	double	m_classifyThreshold;
	BOOL		m_diskFileFlag;
	int		m_classificationArea;
	int		m_saveStatisticsSelection;
	BOOL		m_createClusterMaskFlag;
	int		m_maskFileFormatCode;
	BOOL		m_createImageOverlayFlag;
	int		m_selectImageOverlaySelection;
	//}}AFX_DATA

// Implementation
protected:    
	void 				ClassifyAreaSetting(
							CDataExchange* 		pDX, 
							int 						nIDC, 
							int& 						value);
				
	virtual void 	DoDataExchange(
							CDataExchange* 		pDX);	// DDX/DDV support 
							
	void 				OnAlgorithm(
							Boolean					returnFlag);
	
	void 				UpdateClassificationItems(
							UInt16					classificationArea);

	// Generated message map functions
	//{{AFX_MSG(CMClusterDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnClassifySelectedArea();
	afx_msg void OnSinglePassAlgorithm();     
	afx_msg void OnISODATAAlgorithm();
	afx_msg void OnImageOverlay();
	afx_msg void OnSelendokImageOverlayCombo();
	afx_msg void OnCreateMaskFile();
	afx_msg void OnSelendokMaskFileDiskCombo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	double				mMaxClassifyThreshold;

	SInt32				m_transparencyValue;
	
	SInt16				m_channelListType;  
	
	Boolean				m_initializedFlag,
							m_characterLimitDialogDisplayedFlag,					
							m_clusterProcedureSetFlag,
							m_newProjectFlag;
	
};    
  
#endif // !defined __WCLUSDLG_H__
