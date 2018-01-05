// WFalseColorDialog.h : header file
//      
#if !defined __WFCOLDLG_H__
	#define	__WFCOLDLG_H__       
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFalseColorDlg dialog

class CMFalseColorDlg : public CMDialog
{
// Construction
public:
								CMFalseColorDlg(CWnd* pParent = NULL);	// standard constructor  
	
	Boolean					DoDialog(void);

// Dialog Data
	//{{AFX_DATA(CMFalseColorDlg)
	enum { IDD = IDD_FalseColor };
	long	m_redChannel;
	long	m_greenChannel;
	long	m_blueChannel;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMFalseColorDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	                                       
	LayerInfoPtr		m_layerInfoPtr;
	FileInfoPtr			m_fileInfoPtr;        
	
	Boolean				m_initializedFlag;
	                                          
	SInt16				m_blueChannelIndex; 
	SInt16				m_greenChannelIndex;
	SInt16				m_redChannelIndex;  
	
	SInt32				m_maxChannelFeatureNum;  
	
};      
  
#endif // !defined __WFCOLDLG_H__
