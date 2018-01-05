// WReformatTransformDialog.h : header file
//               
#if !defined __WRTRADLG_H__
	#define	__WRTRADLG_H__  
	
	#include "WDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMReformatTransform dialog

class CMReformatTransformDlg : public CMDialog
{
// Construction
public:
						CMReformatTransformDlg(CWnd* pParent = NULL);	// standard constructor
						
						~CMReformatTransformDlg(); 
	
	Boolean			DoDialog(
							UInt16*			recommendNumberOfBitsPtr,
							SInt16			bandInterleaveSelection); 

// Dialog Data
	//{{AFX_DATA(CMReformatTransformDlg)
	enum { IDD = IDD_ReformatTransform };
	double		m_adjustDivisor;
	double		m_adjustFactor;
	double		m_adjustOffset;
	double		m_adjustSelectedChannelsFactor;
	double		m_functionFactor;
	double		m_transformFactor;
	double		m_transformOffset;
	CString		m_denominatorString;
	CString		m_numeratorString;
	double		m_scaleFactor;
//	int			m_channelSelection;
	UINT			m_adjustSelectedChannel;
	UINT			m_kthSmallestElement;
	UINT			m_minimumNumberBits;
	UINT			m_minSelectedNumberBits;
	int			m_transformCode;
	int			m_functionCode;
	//}}AFX_DATA

// Implementation
protected:
	virtual void 	DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 
							
	void				ShowHideAdjustChannelItems(
							DialogPtr         	dialogPtr,
							Boolean					showFlag); 

	void				ShowHideAdjustSelectedChannelsByChannelItems(
							DialogPtr				dialogPtr,
							Boolean					showFlag);
							
	void				ShowHideAlgebraicTransformItems(
							DialogPtr         	dialogPtr,
							Boolean					showFlag);
	
	void				ShowHidePCTransformItems(
							DialogPtr         	dialogPtr,
							Boolean					showFlag,
							Boolean					pcButtonFlag);
	
	void				ShowHideFunctionChannelsItems(
							DialogPtr				dialogPtr,
							Boolean					showFlag,
							UInt16					functionChannelCode);
			
	// Generated message map functions
	//{{AFX_MSG(CMReformatTransformDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdjustSelectedChannels();
	afx_msg void OnAdjustSelectedChannelsByChannel();
	afx_msg void OnRTAlgebraicTransformation();
	afx_msg void OnRTEigenvectors();
	afx_msg void OnRTFunctionOfChannels();
	afx_msg void OnRTNoTransformation();
	afx_msg void OnSelendokEVEigenvectors();
	afx_msg void OnSelendokReformatFunctions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()  
	
	double			m_maxValue;
	double			m_minValue;
	
	TBYTE*			m_denominatorStringPtr;	
	TBYTE*			m_numeratorStringPtr;	
	
	SInt16			m_bandInterleaveSelection; 
	SInt16			m_eigenSource;
	SInt16			m_maxAdjustOffset;
	UInt16			m_maxAdjustDivisor;
	UInt16			m_maxAdjustFactor; 
	UInt16			m_maxChannel;
	UInt16			m_numberEigenvectors;
	
	Boolean			m_initializedFlag;
};

#endif	// !defined __WRTRADLG_H__
