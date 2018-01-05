// WHistogramDialog.h : header file
//                 
#if !defined __MHISTDLG_H__
	#define	__MHISTDLG_H__  
	
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
	#if defined _X86_ || defined _AMD64_
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMHistogramSpecsDlg dialog

class CMHistogramSpecsDlg : public CMDialog
{
// Construction
public:
						CMHistogramSpecsDlg(CWnd* pParent = NULL);	// standard constructor   
	
						~CMHistogramSpecsDlg(void);		// standard desctructor
	
	Boolean			DoDialog(
							HistogramSpecsPtr					histogramSpecsPtr,
							WindowInfoPtr						windowInfoPtr,
							FileInfoPtr							fileInfoPtr); 

// Dialog Data
	//{{AFX_DATA(CMHistogramSpecsDlg)
	enum { IDD = IDD_HistogramSpecs };
	BOOL		m_computeOnlyMinMaxFlag;
	BOOL		m_includeEmptyBinsFlag;
	BOOL		m_listHistogramValuesFlag;
	BOOL		m_listHistogramSummaryFlag;
	int		m_histogramMethod;
	BOOL		m_textWindowFlag;
	BOOL		m_diskFileFlag;
	CString	m_fileName;
	int		m_columnsLinesFormat;
	CString	m_supportFileName;
	//}}AFX_DATA

// Implementation
protected:
	void				CheckWriteResultsToSettings(void);
	
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
	
//	void				HideAreaHistogramItems (void); 
							
//	void				HideListHistogramItems (void);
	
//	void				HistogramDialogStatisticsFile (
//							HistogramSpecsPtr		histogramSpecsPtr); 
							
//	Boolean			Initialize(void);
	
	void 				SetListAndEmptyBinsDialogBoxes (
							DialogPtr				dialogPtr, 
							Boolean					minMaxetcOnlyFlag, 
							Boolean					listHistogramFlag, 
							Boolean					includeEmptyBinsFlag, 
							Boolean					lineFormatHistFlag);
	
	void				ShowAreaHistogramItems (void);  
							
//	void				ShowListHistogramItems (void);
	
	Boolean 			UpdateAllChannelsAtOnceFlag (
							DialogPtr				dialogPtr, 
							SInt16					numberChannels);

	void				UpdateListAndEmptyBinsDialogBoxes(void);
			
	// Generated message map functions
	//{{AFX_MSG(CMHistogramSpecsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnListHistogram();
	afx_msg void OnComputeOnly();
	afx_msg void OnSelendokColumnLineFormat();
	afx_msg void OnEmptyBins();
	afx_msg void OnDiskFile();
	afx_msg void OnTextWindow();
	afx_msg void OnSelendokMethod();
	afx_msg void OnSelendokChannels();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	HistogramSpecsPtr 	m_histogramSpecsPtr;
	FileInfoPtr				m_fileInfoPtr;
	WindowInfoPtr			m_windowInfoPtr;
	                                                                 
	Boolean					m_allChannelsAtOnceFlag;
	Boolean					m_defaultStatFileChangedFlag;
	Boolean					m_initializedFlag;
	Boolean					m_lineFormatOnlyFlag;  
	Boolean					m_updateListHistogramItemsFlag; 
//	SInt16					m_methodOffset;
	SInt16					m_histogramMethodCode;
	
public:
};  
	
	#endif // defined _X86_ || defined _AMD64_ 
  
#endif // !defined __MHISTDLG_H__
