// WStatisticsView.h : header file
//                       

#if !defined __WSTATVEW_H__
	#define __WSTATVEW_H__   

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif         

#include "WStatisticsDoc.h"
#include "WListBox.h"

class CMStatisticsForm : public CFormView
{
	DECLARE_DYNCREATE(CMStatisticsForm)
protected:
	CMStatisticsForm();			// protected constructor used by dynamic creation  
			
			virtual void 		OnInitialUpdate();	// first time after construct 

// Form Data
public:
	//{{AFX_DATA(CMStatisticsForm)
	enum { IDD = IDD_StatisticsWindow };
	CMListBox			m_listBox;
	int		m_classList;  
	int		m_histogramStatsCode;  
	int		m_listStatsCode;
	int		m_statsTypeCode;
//new
	BOOL     m_polygonFlag;
//end new
	//}}AFX_DATA
			
	virtual void 		OnDraw(CDC* pDC);  // overridden to draw this view 
		
	CMStatisticsDoc* 		GetDocument(void);
	
	void UpdateHistogramStatsCombo(void);
	
	void UpdateListStatsCombo(void); 
	
	void UpdateStatsTypeCombo(
				SInt16								statsWindowMode);
	       
protected:       
	virtual ~CMStatisticsForm();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support  
	
	// Generated message map functions
	//{{AFX_MSG(CMStatisticsForm)
	afx_msg void OnDestroy();
	afx_msg void OnAddToList();
	afx_msg void OnSelendokClassList();
	afx_msg void OnProject();
	afx_msg void OnClass();
	afx_msg void OnField();
	afx_msg void OnUpdate();
	afx_msg void OnPolygon();
	afx_msg void OnSelect();
	afx_msg void OnList();
	afx_msg void OnHistogram();
	afx_msg void OnSelchangeListBox();
	afx_msg void OnDblclkListBox();
	afx_msg void OnEditName();
	afx_msg void OnSelendokHistogramStatsCombo();
	afx_msg void OnSelendokListStatsCombo();
	afx_msg void OnSelendokStatsCombo();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropdownStatsTypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	Boolean				m_initializedFlag,
							m_optionKeyFlag;
	
	CDataExchange		*m_dialogFromPtr,
							*m_dialogToPtr;  
	
};

///////////////////////////////////////////////////////////////////////////// 
          
#endif	// !defined __WSTATVEW_H__
