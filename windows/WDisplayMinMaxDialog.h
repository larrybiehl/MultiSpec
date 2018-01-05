// WDisplayMinMaxDialog.h : header file
//       
#if !defined __WDMINDLG_H__
	#define	__WDMINDLG_H__       
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMDisplayMinMaxDialog dialog

class CMDisplayMinMaxDialog : public CMDialog
{
// Construction
public:
	CMDisplayMinMaxDialog(CWnd* pParent = NULL);	// standard constructor 
	
	Boolean			DoDialog(
							SInt16*								channelsPtr, 
							SInt16								rgbColors, 
							SInt16								displayType,
							SInt16								numberLevels,
							SInt16*								percentTailsClippedPtr, 
							SInt16*								minMaxSelectionPtr, 
							double*								minMaxValuesPtr); 

// Dialog Data
	//{{AFX_DATA(CMDisplayMinMaxDialog)
	enum { IDD = IDD_DisplayEnhancement };
	double	m_newBlueChannelMax;
	double	m_newBlueChannelMin;
	double	m_newGreenChannelMax;
	double	m_newGreenChannelMin;
	double	m_newRedChannelMax;
	double	m_newRedChannelMin;
	int		m_percentClipped;
	int		m_localMinMaxCode;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMDisplayMinMaxDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChannelMinMax();
	afx_msg void OnChangePercentClipped();
	afx_msg void OnChangeMinMax();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP() 
	
	HistogramSpecsPtr					m_histogramSpecsPtr;
	HistogramSummaryPtr				m_histogramSummaryPtr; 
	
	double*								m_minMaxValuesPtr; 
	
	SInt16								*m_channelsPtr;
											
	double								m_maxValue,
											m_minValue; 
	                                           
	SInt16								m_displayType,
											m_minMaxOptionCode,
											m_numberLevels,
											m_rgbColors;
											
	Boolean								m_histogramAvailableFlag,
											m_updatingMinMaxFlag;
											
public:
};                        
  
#endif // !defined __WDISPDLG_H__
