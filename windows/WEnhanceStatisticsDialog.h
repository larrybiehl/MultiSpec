#if !defined(AFX_WESTADLG_H__D64668B6_8D4C_11D3_8D48_00105AA88EE3__INCLUDED_)
#define AFX_WESTADLG_H__D64668B6_8D4C_11D3_8D48_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WEnhanceStatisticsDialog.h : header file
//

	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMEnhanceStatisticsDialog dialog

class CMEnhanceStatisticsDialog : public CMDialog
{
// Construction
public:
				CMEnhanceStatisticsDialog(CWnd* pParent = NULL);   // standard constructor

				~CMEnhanceStatisticsDialog(void);

	SInt16			DoDialog(
							StatEnhanceSpecsPtr				statEnhanceSpecsPtr);

// Dialog Data
	//{{AFX_DATA(CMEnhanceStatisticsDialog)
	enum { IDD = IDD_EnhanceStatistics };
	double	m_logLikeStopPercent;
	long		m_iterationStopLength;
	long		m_iterationMax;
	double	m_labelWeight;
	BOOL	m_useEnhancedStatisticsFlag;
	BOOL	m_weightLabeledFlag;
	int		m_weightsSelection;
	double	m_softChiChiThreshold;
	double	m_hardChiChiThreshold;
	double	m_softPercentThreshold;
	double	m_hardPercentThreshold;
	int		m_softThresholdCode;
	int		m_hardThresholdCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMEnhanceStatisticsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMEnhanceStatisticsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnWeightLabeledSamples();
	afx_msg void OnUseEnhancedStats();
	afx_msg void OnSelchangeHardThresholdCombo();
	afx_msg void OnSelchangeSoftThresholdCombo();
	afx_msg void OnChangeHardPercentThreshold();
	afx_msg void OnChangeSoftPercentThreshold();
	afx_msg void OnChangeHardChiChiThreshold();
	afx_msg void OnChangeSoftChiChiThreshold();
	afx_msg void OnSelendokClassWeightsCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void					CheckColumnEnd(void);

	void					CheckColumnInterval(void);
	 
	void					CheckColumnStart(void);
	
	void					CheckLineEnd(void);

	void					CheckLineInterval(void);
	
	void					CheckLineStart(void);
								
	void 					OnSelendokClassCombo(void); 
							
	void					ToEntireImage(void);
							
	void					ToSelectedImage(void); 

	float*								m_classWeightsPtr;
	StatEnhanceSpecsPtr				m_statEnhanceSpecsPtr;
	
	SInt16								m_classWeightSet;
								
	Boolean								m_initializedFlag,
											m_updatingThresholdItemsFlag;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WESTADLG_H__D64668B6_8D4C_11D3_8D48_00105AA88EE3__INCLUDED_)
