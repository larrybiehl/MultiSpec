// WFileFormatDialog.h : header file
//        
#if !defined __MFORMDLG_H__
	#define	__MFORMDLG_H__   
	     
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFileFormatSpecsDlg dialog

class CMFileFormatSpecsDlg : public CMDialog
{
// Construction
public:
									CMFileFormatSpecsDlg(CWnd* pParent = NULL);	// standard constructor
		
	Boolean						DoDialog(
										Handle								fileInfoHandle, 
										Handle								windowInfoHandle,
										Handle*								mapInformationHandlePtr,
										Handle*								hfaHandlePtr,
										Handle*								newChannelToHdfDataSetHandlePtr,
										Boolean*								parameterChangedFlagPtr);

// Dialog Data
	//{{AFX_DATA(CMFileFormatSpecsDlg)
	enum { IDD = IDD_FileFormatSpecs };
	UInt32		m_blockHeight;
	UInt32		m_blockWidth;
	UInt32		m_numberLines;
	UInt32		m_numberColumns;
	UInt32		m_postChannelBytes;
	UInt32		m_headerBytes;
	UInt32		m_postLineBytes;
	UInt32		m_preChannelBytes;
	UInt32		m_preLineBytes;
	UInt32		m_numberChannels;
	SInt32		m_startColumnNumber;
	SInt32		m_startLineNumber;
	BOOL			m_swapBytesFlag;
	BOOL			m_linesBottomToTopFlag;
	BOOL			m_fillDataValueExistsFlag;
	double		m_fillDataValue;
	UInt32		m_trailerBytes;
	CString		m_imageName;
	BOOL			m_computeNumClasses;
	int			m_dataValueType;
	int			m_bandInterleave;
	int			m_hdfDataSetSelection;
	int			m_collapseClassSelection;
	//}}AFX_DATA
	
	int			m_eightBitsPerDataSelection;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
	Handle					m_fileInfoHandle,
								m_hfaHandle,
								m_mapInformationHandle,
								m_newChannelToHdfDataSetHandle,
								m_windowInfoHandle;
	
	UInt32					m_maxNumberChannelsClasses;
										
	FileInfoPtr				m_fileInfoPtr; 
	WindowInfoPtr			m_windowInfoPtr;

	SInt16					m_dataSetIndex,
								m_gdalDataTypeCode;

	UInt16					m_dataCompressionCode;
	
	Boolean					m_callGetHDFLineFlag,
								m_forceGroupTableUpdateFlag,
								m_showMessagesFlag;

	// Generated message map functions
	//{{AFX_MSG(CMFileFormatSpecsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokBandInterleave();
	afx_msg void OnSelendokDataValueType();
	afx_msg void OnDetermineNumClasses();
	afx_msg void OnSelendokHDFDataSet();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	Boolean						m_initializedFlag;
public:
	afx_msg void OnBnClickedHdfdatasethelp();
	afx_msg void OnBnClickedLinebottomtotop();
	afx_msg void OnBnClickedFillDataValueExists();
	afx_msg void OnStnClickedHdfdatasetprompt();
};

#endif	// !defined __MFORMDLG_H__
