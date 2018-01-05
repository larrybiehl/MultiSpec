// WFeatureSelectionDialog.h : header file
//   
#if !defined __WFSELDLG_H__
	#define	__WFSELDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionDialog dialog

class CMFeatureSelectionDialog : public CMDialog
{
// Construction
public:
	CMFeatureSelectionDialog(CWnd* pParent = NULL);	// standard constructor     
	
							~CMFeatureSelectionDialog();		// standard desctructor 
	
	Boolean			DoDialog(
							SeparabilitySpecsPtr				separabilitySpecsPtr,
							UInt16*								allChanCombinationsPtr);

// Dialog Data
	//{{AFX_DATA(CMFeatureSelectionDialog)
	enum { IDD = IDD_FeatureSelection };
	int		m_separabilityListSelection;         
	int		m_channelCombinationSelection;      
	long		m_savedContiguousChannelsPerGroup;
	long		m_localCombinationsToList;
	int		m_interClassWeightsSelection;  
	BOOL		m_textWindowFlag;   
	BOOL		m_diskFileFlag;
	BOOL	m_searchFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	void HandleChannelCombinationsMenu(
							UInt16								channelCombinationsMenuItemNumber);

	// Generated message map functions
	//{{AFX_MSG(CMFeatureSelectionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnSelendokDistanceMeasureCombo(); 
	afx_msg void OnSelendokNumberChannelsCombo();
	afx_msg void OnChangeChannelsPerGroup();
	afx_msg void OnStepProcedure();
	afx_msg void OnChangeNumberBestToList();
	afx_msg void OnSelendokClassCombo();
	afx_msg void OnFeatureTransformation();
	virtual void OnOK();
	afx_msg void OnListOptions();
	afx_msg void OnSelendokClassPairWeightsCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	
	SeparabilitySpecsPtr				m_separabilitySpecsPtr;

	SInt16								*m_localClassPairWeightsListPtr;
	                                           
	UInt16								*m_allChanCombinationsPtr,
											*m_localChannelCombinationsPtr;
	
	SInt32								m_maxContiguousChannelsPerGroup,
											m_numberClassCombinations;
	
	SInt16								m_localDefaultClassPairWeight,
											m_separabilityDistance;
	
	UInt16								m_localNumberChannelGroupCombinations,
											m_savedNumberChannelGroupCombinations;
	
};            

/////////////////////////////////////////////////////////////////////////////
// CMFeatureSelectionListDialog dialog

class CMFeatureSelectionListDialog : public CMDialog
{
// Construction
public:
	CMFeatureSelectionListDialog(CWnd* pParent = NULL);	// standard constructor

	Boolean				DoDialog(
								SeparabilitySpecsPtr				separabilitySpecsPtr,
								SInt32*								combinationsToListPtr);

// Dialog Data
	//{{AFX_DATA(CMFeatureSelectionListDialog)
	enum { IDD = IDD_FeatureSelectionListOptions };
	int		m_sortChannelCombinations;
	BOOL	m_thresholdedGroupTableFlag;
	BOOL	m_thresholdedClassPairTableFlag;
	double	m_localDistancesLessThanToGroup;
	double	m_localDistancesLessThanToList;
	BOOL	m_listClassPairDistancesFlag;
	double	m_localMinDistanceRangeToList;
	double	m_localMaxDistanceRangeToList;
	long	m_localCombinationsToList;
	BOOL	m_separabilityTableFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMFeatureSelectionListDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	
	SeparabilitySpecsPtr				m_separabilitySpecsPtr;
	
	SInt32*								m_combinationsToListPtr;
	
};
  
#endif // !defined __WFSELDLG_H__
