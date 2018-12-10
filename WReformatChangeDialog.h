// WReformatChangeDialog.h : header file
//   
// Revised by Larry Biehl on 07/03/2018
//
#if !defined __WRCHANGE_H__
	#define	__WRCHANGE_H__  
	
	#include "WDialog.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CMChangeFormatDlg dialog

class CMChangeFormatDlg : public CMDialog
{
// Construction
public:
						CMChangeFormatDlg(CWnd* pParent = NULL);	// standard constructor
	
	Boolean			DoDialog( 
							FileInfoPtr							outFileInfoPtr, 
							ReformatOptionsPtr				reformatOptionsPtr);

// Dialog Data
	//{{AFX_DATA(CMChangeFormatDlg)
	enum { IDD = IDD_FileFormatChange };
	BOOL		m_swapBytesFlag;
	BOOL		m_transformDataFlag;
	int		m_channelSelection;
	int		m_headerListSelection;
	int		m_dataValueListSelection;
	int		m_outputFileSelection;
	int		m_bandInterleaveSelection;
	BOOL		m_invertBottomToTopFlag;
	BOOL		m_invertLeftToRightFlag;
	BOOL		m_outputInWavelengthOrderFlag;
	BOOL		m_writeChanDescriptionFlag;
	//}}AFX_DATA

// Implementation
protected:
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
							
//	Boolean			Initialize(void);
	
	void				SetChannelDescriptionFlag(void);

	// Generated message map functions
	//{{AFX_MSG(CMChangeFormatDlg)
	virtual BOOL OnInitDialog ();
	afx_msg void OnOutputInWavelengthOrder ();
	afx_msg void OnTransformData ();
	afx_msg void OnSelendokBandInterleave ();
	afx_msg void OnSelendokOutChannels ();
	afx_msg void OnSelendokHeader ();
	afx_msg void OnWriteChanDescriptions ();
	afx_msg void OnSelendokDataValueType ();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	UInt8									m_inputBandInterleaveString[64],
											m_inputDataValueTypeString[64],
											m_tiffMenuNameString[16];
	 
	static ReformatOptionsPtr		s_reformatOptionsPtr;
	
	Boolean								m_channelDescriptionAllowedFlag,
											m_channelThematicDisplayFlag,
											m_dataValueTypeSelectionFlag,
											m_GAIAFormatAllowedFlag,
											m_initializedFlag,
											m_outputInWavelengthOrderAllowedFlag,
											m_savedChannelDescriptionFlag,
											m_sessionUserSetDataValueTypeSelectionFlag;

	int									m_headerOptionsSelection;
	
	SInt16								m_dataValueTypeSelection,
											m_eightBitsPerDataSelection,
											m_noTransformDataValueTypeSelection,
											m_savedDataValueTypeSelection;
	
};
   
#endif	// !defined __WRCHANGE_H__
