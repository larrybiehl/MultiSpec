// WProjDlg.h : header file
//      
#if !defined __WPROJDLG_H__
	#define	__WPROJDLG_H__  
	
	#include "WDialog.h"   

/////////////////////////////////////////////////////////////////////////////
// WEditClassFieldDlg dialog

class CMEditClassFieldDlg : public CMDialog
{
// Construction
public:
	CMEditClassFieldDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMEditClassFieldDlg)
	enum { IDD = IDD_ClassField };
	CString	m_className;
	CString	m_fieldName;
	int		m_fieldType;
	//}}AFX_DATA
	
	Boolean			DoDialog(
							SInt16					classFieldCode, 
							SInt16					currentClass,
							SInt16					currentField,
							char*						titleStringPtr);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMEditClassFieldDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();               
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()      
	
	TBYTE*			m_classNameCStringPtr;
	TBYTE*			m_fieldNameCStringPtr;
	char*				m_titleStringPtr;
	
	Boolean			m_initializedFlag;
	SInt16			m_classFieldCode; 
	SInt16			m_currentField;
	SInt16			m_classStorage;
	SInt16*			m_fieldTypePtr; 
	
};  

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
	
	Boolean				m_initializedFlag,
							m_keepClassStatsFlag,        
							m_useCommonCovarianceInLOOCFlag;
	
};                                


/////////////////////////////////////////////////////////////////////////////
// CMStatOptionsDlg dialog

class CMStatOptionsDlg : public CMDialog
{
// Construction
public:
	CMStatOptionsDlg(CWnd* pParent = NULL);	// standard constructor  
	
	SInt16			DoDialog(
							SInt16*			statCodePtr, 
							Boolean*			keepClassStatsFlagPtr, 
							double*			variancePtr,
							double*			minLogDeterminantPtr,
							Boolean*			useCommonCovarianceInLOOCFlagPtr);
			
// Dialog Data
	//{{AFX_DATA(CMStatOptionsDlg)
	enum { IDD = IDD_StatisticsOptionsDialog };
	BOOL	m_classStatsOnlyFlag;
	BOOL	m_setZeroVarianceFlag;
	int		m_statCode;
	double	m_minLogDetValue;
	double	m_varianceValue;
	BOOL	m_useCommonCovarianceInLOOCFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMStatOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMeanStd();
	afx_msg void OnSetZeroVariance();
	afx_msg void OnMeanStdCov();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	void			SetZeroVariance(void);
	
};


                                
/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg dialog

class CMEditCoordinatesDlg : public CMDialog
{
// Construction
public:
	CMEditCoordinatesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMEditCoordinatesDlg)
	enum { IDD = IDD_FieldCoordinates };
	BOOL	m_applyToAllWindowsFlag;
	int		m_listSelectionUnits;
	double	m_newColumnStart;
	double	m_newColumnEnd;
	double	m_newLineEnd;
	double	m_newLineStart;
	BOOL	m_useStartLineColumnFlag;
	//}}AFX_DATA
	
	Boolean					DoDialog(      
									WindowPtr							windowPtr,
									WindowInfoPtr						windowInfoPtr,
									LongRect*							inputSelectionRectanglePtr,
									LongRect*							selectionRectanglePtr,
									DoubleRect*							coordinateRectanglePtr, 
									SInt16								pointType,
									SInt16*								unitsDisplayCodePtr,
									Boolean*								changedFlagPtr,
									Boolean*								applyToAllWindowsPtr,
									Boolean*								useStartLineColumnFlagPtr,
									Boolean*								previewWasUsedFlagPtr,
									SInt16								stringID);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	void			CheckOKFlag (void);

	Boolean		CheckLineColumnValues(
				UInt16*								controlIDPtr); 

	// Generated message map functions
	//{{AFX_MSG(CMEditCoordinatesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNewColumnEnd();
	afx_msg void OnChangeNewColumnStart();
	afx_msg void OnChangeNewLineEnd();
	afx_msg void OnChangeNewLineStart();
	afx_msg void OnPreview();
	afx_msg void OnSelendokCoordinateUnits();
	afx_msg void OnApplyToAllCheckbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	DoubleRect			m_inputCoordinateRectangle,
							m_minMaxCoordinateRectangle;

	LongRect				m_minMaxSelectionRectangle;

	double				m_realValue;
	
	WindowPtr			m_windowPtr;

	LongRect*			m_inputSelectionRectanglePtr;

	SInt16				m_selectionUnits,
							m_unitsDisplayCode;

	Boolean				m_coordinateUnitsChangingFlag,
							m_valueChangedFlag,
							m_initializedFlag,
							m_previewWasUsedFlag,
							m_stringID;   
   
	SInt16				m_pointType;    
	WindowInfoPtr		m_windowInfoPtr; 
	LongRect*			m_selectionRectanglePtr;
	DoubleRect*			m_coordinateRectanglePtr;
	
};                 

#endif // !defined __WPROJDLG_H__ 
