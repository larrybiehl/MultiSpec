// WDisplayThematicDialog.h : header file
//          
#if !defined __WDISTDLG_H__
	#define	__WDISTDLG_H__       
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMDisplayThematicDlg dialog

class CMDisplayThematicDlg : public CMDialog
{
// Construction
public:
						CMDisplayThematicDlg(CWnd* pParent = NULL);	// standard constructor   
	
						~CMDisplayThematicDlg();		// standard desctructor
  
	
	Boolean			DoDialog(
							DisplaySpecsPtr		displaySpecsPtr); 

// Dialog Data
	//{{AFX_DATA(CMDisplayThematicDlg)
	enum { IDD = IDD_DisplayThematic };
	int		m_classGroupSelection;
	int		m_classSelection;
	BOOL		m_showLegendFlag;
	int		m_paletteSelection;
	CString	m_numberClasses;
	CString	m_numberGroups;
	CString	m_fileName;
	double	m_magnification;
	BOOL		m_useThresholdFileFlag;
	BOOL		m_includeVectorOverlaysFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMDisplayThematicDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokPaletteCombo();
	afx_msg void OnDropdownPaletteCombo();
	afx_msg void OnSelendokClassesGroupsCombo();
	afx_msg void OnBackgroundColor();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	Boolean				m_initializedFlag,
							m_localPaletteUpToDateFlag,
							m_paletteChangedFlag;
	
	RGBColor				m_oldRGB;
	
	DisplaySpecsPtr	m_displaySpecsPtr;
	
	Handle				m_localClassGroupsHandle;
	
	SInt16				*m_localClassGroupsPtr,
							*m_localClassToGroupPtr;
	
	UInt32				m_localDisplayClasses,
							m_localDisplayGroups;
							
	SInt16				m_localAllSubsetClass,
							m_localAllSubsetGroup,
							m_localNumberGroups;
	
public:
	afx_msg void OnBnClickedCheck1();
};              
  
#endif // !defined __WDISTDLG_H__
