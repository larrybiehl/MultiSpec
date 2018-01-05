// WStatisticsDialog.h : header file
//      
#if !defined __WSTATISTICS_H__
	#define	__WSTATISTICS_H__  
	
	#include "WDialog.h"   


/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDialog dialog

class CMStatisticsDialog : public CMDialog
{
// Construction
public:
	CMStatisticsDialog(CWnd* pParent = NULL);	// standard constructor 
	
	SInt16			DoDialog(
							SInt16*								featurePtr,
							SInt16								totalNumberChannels,
							Handle*								trainMaskFileInfoHandle,
							Handle*								testMaskFileInfoHandle);
			
// Dialog Data
	//{{AFX_DATA(CMStatisticsDialog)
	enum { IDD = IDD_StatisticsDialog };
	BOOL	m_showFieldNames;
	BOOL	m_showTestFields;
	BOOL	m_showTrainingFields;
	int		m_projectCommands;
	BOOL	m_showClassNames;
	BOOL	m_showTrainTestText;
	int		m_outlineColorSelection;
	int		m_testMaskCombo;
	int		m_trainMaskCombo;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support  
	
	void SetOutlineAreaOptions (void);

	// Generated message map functions
	//{{AFX_MSG(CMStatisticsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokChannels();
	afx_msg void OnSelendokProjectChanges();
	afx_msg void OnDropdownProjectChanges();
	afx_msg void OnDropdownChannels();
	afx_msg void OnStatisticsOptions();
	afx_msg void OnShowClassNames();
	afx_msg void OnShowFieldNames();
	afx_msg void OnTestFields();
	afx_msg void OnTrainingFields();
	afx_msg void OnShowTrainTestLabel();
	afx_msg void OnDropdownTestMaskCOMBO();
	afx_msg void OnSelendokTestMaskCOMBO();
	afx_msg void OnDropdownTrainMaskCOMBO();
	afx_msg void OnSelendokTrainMaskCOMBO();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	double				m_minLogDeterminant,
							m_variance;  
							
	Handle				m_testMaskFileInfoHandle,
							m_trainMaskFileInfoHandle;
	
	SInt16				*m_featurePtr,
							m_localStatCode,
							m_totalNumberChannels;

	UInt16				m_maxNumberTestLayers,
							m_maxNumberTrainLayers;
	
	Boolean				m_initializedFlag,
							m_keepClassStatsFlag,        
							m_useCommonCovarianceInLOOCFlag;
	
};                                             

#endif // !defined __WSTATISTICS_H__ 
