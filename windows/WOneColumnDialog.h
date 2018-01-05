// W1ColDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMOneColDlg dialog   

#if !defined __W1COLDLG_H__
	#define	__W1COLDLG_H__    
	
	#include "WDialog.h"

class CMOneColDlg : public CMDialog
{
// Construction
public:
							CMOneColDlg(CWnd* pParent = NULL);	// standard constructor 

// Dialog Data
	//{{AFX_DATA(CMOneColDlg)
	enum { IDD = IDD_OneColumn };
	UINT					m_listStart;
	UINT					m_listInterval;
	UINT					m_listEnd;
	CString				m_selectionCount;
	//}}AFX_DATA

// Implementation
protected: 

	void 					CheckValue(
								UInt16					itemNumber,
								UINT						lastValue,
								UINT*						lastValuePtr);
				
	virtual void 		DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	void					UpdateNumberOfSelections(void);

	// Generated message map functions
	//{{AFX_MSG(CMOneColDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAllSelected();
	afx_msg void OnNoneSelected();
	afx_msg void OnChangeFirst();
	afx_msg void OnChangeInterval();
	afx_msg void OnChangeLast();
	afx_msg void OnSelchangeList1();
	virtual void OnOK();
	afx_msg void OnEnterNewRange();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()                                  
	 
	SInt16* 								m_selectedItemsPtr; 
	
	UInt32								m_numberInputVecItems;
	
	SInt16								m_currentSelection,                                    
											m_indexStart,         
											m_listType, 
											m_numberOutputItems;
	
	UInt16								m_minimumItemsRequired,
											m_numberSelections;
	
	Boolean								m_initializedFlag; 
	
};                      
   
#endif	// !defined __W1COLDLG_H__
