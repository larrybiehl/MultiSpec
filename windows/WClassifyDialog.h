// WClassifyDialog.h : header file
//               
#if !defined __WCLSFDLG_H__
	#define	__WCLSFDLG_H__             
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMClassifyDialog dialog

class CMClassifyDialog : public CMDialog
{
// Construction
public:
							CMClassifyDialog(CWnd* pParent = NULL);	// standard constructor     
	
							~CMClassifyDialog();		// standard desctructor 
  
	
	Boolean				DoDialog(void); 
	
	void					HideShowClassAreasItem(void);

// Dialog Data
	//{{AFX_DATA(CMClassifyDialog)
	enum { IDD = IDD_ClassifyDialog };
	BOOL		m_trainingAreaFlag;
	BOOL		m_imageAreaFlag;
//	BOOL		m_textWindowFlag;
	BOOL		m_thresholdResultsFlag;
	int		m_classWeightsSelection;
	float		m_thresholdPercent;
	BOOL		m_diskFileFlag;
	int		m_classifyListSelection; 
	int		m_classAreaSelection;
	BOOL		m_createProbabilityFileFlag;
	BOOL		m_testAreaFlag;
	int		m_fileNamesSelection;
	int		m_outputFormatCode;
	int		m_paletteSelection;
	BOOL		m_trainingAreaLOOFlag;
	float		m_cemThreshold;
	float		m_angleThreshold;
	float		m_correlationThreshold;
	BOOL		m_createImageOverlayFlag;
	int		m_selectImageOverlaySelection;
	UInt16			m_classifyProcedureEnteredCode;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMClassifyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokChannelCombo();
	afx_msg void OnSelendokClassWeightsCombo();
	afx_msg void OnSelendokClassificationProcedure();
	afx_msg void OnDiskFile();
	afx_msg void OnThresholdResults();
	afx_msg void OnSelendokDiskCombo();
	afx_msg void OnTextWindow();
	afx_msg void OnTraining();
	afx_msg void OnTestAreas();
	afx_msg void OnImageArea();
	afx_msg void OnFeatureTransformation();
	afx_msg void OnCreateProbabilityFile();
	afx_msg void OnSelendokAreasCombo();
	afx_msg void OnSelendokPaletteCombo();
	afx_msg void OnDropdownPaletteCombo();
	afx_msg void OnListOptions();
	afx_msg void OnChangeCorrelationCoefficient();
	afx_msg void OnChangeCorrelationThresold();
	afx_msg void OnDropdownClassificationProcedure();
	afx_msg void OnTrainingLOO();
	afx_msg void OnSelendokTargetCombo();
	afx_msg void OnImageOverlay();
	afx_msg void OnSelendokImageOverlayCombo();
	afx_msg void OnCloseupClassificationProcedure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	void					CheckColumnEnd(void);
	 
	void					CheckColumnStart(void);
	
	void					CheckLineEnd(void);
	
	void					CheckLineStart(void);
	
	void 					CheckAreaSettings(void);
	
	void 					CheckOutputFormatItems(void);
							
	void					ToEntireImage(void);
							
	void					ToSelectedImage(void); 
	
	double				m_saveAngleThreshold,
							m_saveCEMThreshold,
							m_saveCorrelationThreshold,
							m_saveThresholdPercent;
	
	float*				m_classWeightsPtr;
	UInt16*				m_classAreaListPtr;
	
	Handle				m_targetWindowInfoHandle; 
	     
	UInt32				m_localNumberClassAreas;
							
	SInt16				m_classificationProcedure,
							m_correlationComboListItem,
							m_covarianceEstimate, 
							m_listResultsTestCode,
							m_listResultsTrainingCode, 
							m_outputAsciiCode,
							m_parallelPipedCode; 
								
	Boolean				m_initializedFlag,
							m_optionKeyFlag,
							m_savedLeaveOneOutFlag,
							m_saveThresholdResultsFlag,
							m_thresholdAllowedFlag,
							m_trainingFieldsExistFlag;
							
public:
	afx_msg void OnStnClickedStartendinterval();
};                                 
 
#endif // !defined __WCLSFDLG_H__
