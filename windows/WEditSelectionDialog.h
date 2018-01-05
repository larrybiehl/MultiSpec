// WEditSelectionDialog.h : header file
//      
#if !defined __WEDITSELECTION_H__
	#define	__WEDITSELECTION_H__  
	
	#include "WDialog.h"   


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

#endif // !defined __WEDITSELECTION_H__ 
