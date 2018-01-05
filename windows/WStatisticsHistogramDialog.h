#if !defined(AFX_WSTATHISTOGRAMSPECSDLG_H__789FC6F3_D18F_11D6_8F2A_00105AA88EE3__INCLUDED_)
#define AFX_WSTATHISTOGRAMSPECSDLG_H__789FC6F3_D18F_11D6_8F2A_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WStatisticsHistogramDialog.h : header file
//
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMStatHistogramSpecsDlg dialog

class CMStatHistogramSpecsDlg : public CMDialog
{
// Construction
public:
	CMStatHistogramSpecsDlg(CWnd* pParent = NULL);   // standard constructor

	~CMStatHistogramSpecsDlg(void);
	
	Boolean			DoDialog(
							StatHistogramSpecsPtr			statHistogramSpecsPtr,
							FileInfoPtr							fileInfoPtr); 

// Dialog Data
	//{{AFX_DATA(CMStatHistogramSpecsDlg)
	enum { IDD = IDD_StatHistogram };
	BOOL	m_featureTransformationFlag;
	BOOL	m_includeEmptyBinsFlag;
	BOOL	m_blankValuesFlag;
	BOOL	m_overlayDensityFunctionFlag;
	long	m_lineInterval;
	long	m_columnInterval;
	int		m_histogramClassFieldCode;
	int		m_listPlotCode;
	int		m_channelsClassesCode;
	int		m_matrixColumnCode;
//	int		m_channelSelection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMStatHistogramSpecsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMStatHistogramSpecsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClassesRadio();
	afx_msg void OnFieldsRadio();
	afx_msg void OnListRadio();
	afx_msg void OnPlotRadio();
	afx_msg void OnFeatureTransformation();
	afx_msg void OnSelendokChannelCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	StatHistogramSpecsPtr 	m_statHistogramSpecsPtr;
	FileInfoPtr					m_fileInfoPtr;

	SInt16						m_savedMatrixColumnCode;

	Boolean						m_featureTransformAllowedFlag,
									m_overlayDFAllowedFlag;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WSTATHISTOGRAMSPECSDLG_H__789FC6F3_D18F_11D6_8F2A_00105AA88EE3__INCLUDED_)
