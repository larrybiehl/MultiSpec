// WFeatureExtractionDialog.h : header file
//      
#if !defined __WFEXTDLG_H__
	#define	__WFEXTDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFeatureExtractionDialog dialog

class CMFeatureExtractionDialog : public CMDialog
{
// Construction
public:
	CMFeatureExtractionDialog(CWnd* pParent = NULL);	// standard constructor     
	
							~CMFeatureExtractionDialog();		// standard desctructor 
	
	Boolean			DoDialog (          
							FeatureExtractionSpecsPtr		featureExtractionSpecsPtr);

// Dialog Data
	//{{AFX_DATA(CMFeatureExtractionDialog)
	enum { IDD = IDD_FeatureExtraction };
	float	m_interclassThrehold;
	float	m_withinClassThreshold;
	BOOL	m_optimizeClassesFlag;
	long	m_minThresholdNumber;
	long	m_maxPixelsPerClass;
	BOOL	m_specialOptionsFlag;
	BOOL	m_listTransformationFlag;
	int		m_interClassWeightsSelection; 
	int		m_weightsSelection;
	int		m_preprocessSelection;
	int		m_algorithmCode;
	float	m_optimizeThreshold;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMFeatureExtractionDialog)
	virtual BOOL OnInitDialog();        
	afx_msg void OnOptimizeClasses();
	afx_msg void OnSpecialOptions();
	afx_msg void OnDecisionBoundary();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnSelendokClassWeightsCombo();
	afx_msg void OnSelendokClassPairWeightsCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()      
	
	
	FeatureExtractionSpecsPtr		m_featureExtractionSpecsPtr;
	
	float*								m_classWeightsPtr;
	
	SInt16*								m_localClassPairWeightsListPtr;
	
	SInt16								m_localDefaultClassPairWeight;
											
public:
	afx_msg void OnCbnSelendokPreprocessingcombo();
};                 
  
#endif // !defined __WFEXTDLG_H__
