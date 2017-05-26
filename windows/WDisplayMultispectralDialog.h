// WDispDlg.h : header file
//                     
#if !defined __WDISPDLG_H__
	#define	__WDISPDLG_H__       
	     
	#include "WDialog.h"
	

/////////////////////////////////////////////////////////////////////////////
// CMDisplaySpecsDlg dialog

class CMDisplaySpecsDlg : public CMDialog
{                         
// Construction
public:
						CMDisplaySpecsDlg(CWnd* pParent = NULL);	// standard constructor  
	
						~CMDisplaySpecsDlg();		// standard desctructor
	
	Boolean			DoDialog(
							DisplaySpecsPtr		displaySpecsPtr); 

// Dialog Data
	//{{AFX_DATA(CMDisplaySpecsDlg)
	enum { IDD = IDD_DisplaySpecs };
	UINT		m_BlueChannel;
	UINT		m_GreenChannel;
	double	m_Magnification;
	double	m_thematicLegendFactor;
	BOOL		m_ComputeHistogramFlag;
	UINT		m_NumberLevels;
	UINT		m_RedChannel;
	CString	m_FileName;
	int		m_BitsOfColor;
	int		m_DisplayType;
	int		m_Enhancement;
	UINT		m_GrayChannel; 
	int		m_TreatZeroAs;
	int		m_MinMaxPopupCode;
	BOOL		m_redChannelInvertFlag;
	BOOL		m_greenChannelInvertFlag;
	BOOL		m_blueChannelInvertFlag;
	BOOL		m_vectorOverlaysFlag;
	//}}AFX_DATA

// Implementation
protected:                             
	void				CheckChannel(
							UInt16					itemNumber,
							UINT*						channelValuePtr);
				
	void				CheckDisplayLevels (
							DialogPtr				dialogPtr,          
							UInt16					displayType);
				
	virtual void 	DoDataExchange(
							CDataExchange* 		pDX);	// DDX/DDV support
							
	Boolean			GetComputeHistogramDialogSetting (
							DisplaySpecsPtr		displaySpecsPtr,
							SInt16					lEnhancement,
							SInt16					lMinMaxCode);
	
//	void				GetDefaultPaletteSpecs (
//							SInt16					currentDisplayType,
//							SInt16					currentPixelSize,  
//							SInt16					numberChannels, 
//							UInt16					*newDisplayTypePtr,
//							SInt16					*maxSystemPixelSizePtr);
							
	UInt16			GetMaximumDisplayLevels (
							SInt16					bitsOfColorIndex,          
							SInt16					displayType, 
							SInt16					duplicateChannelCode); 
							
	void				SetComputeHistogramDialogItem (         
							Boolean					computeHistogramFlag,
							DialogPtr				dialogPtr, 
							SInt16					itemNumber); 
							
/*	void				UpdateDialogChannelItems (
							DialogPtr				dialogPtr, 
							SInt16					rgbColors, 
							SInt16					displayType);
*/							
/*	void				UpdateDisplayLevels (
							DialogPtr				dialogPtr,          
							SInt16					displayType, 
							SInt16					duplicateChannelCode);
*/							
	SInt16			Update8_16_24BitsOfColorIndex (
							SInt16					menuBitsOfColorIndex);

	// Generated message map functions
	//{{AFX_MSG(CMDisplaySpecsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelendokDisplayType();
	afx_msg void OnSelendokBitsOfColor();
	afx_msg void OnChangeBlueChannel();
	afx_msg void OnChangeGreenChannel();
	afx_msg void OnChangeRedChannel();
	afx_msg void OnSelendokChannels();
	afx_msg void OnDropdownBitsOfColor();
	afx_msg void OnSelendokMinMaxValues();
	afx_msg void OnChangeGrayChannel();
	afx_msg void OnChannelDescriptions();
	afx_msg void OnSelendokEnhancement();
	afx_msg void OnDropdownEnhancement();
	afx_msg void OnCloseupEnhancement();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	DisplaySpecsPtr		m_displaySpecsPtr;
	
			// This is used as the index into the number of levels array
			// to allow for the Windows version having only 8 and 24 bit
			// color choices.

	double		m_gaussianStretch;
			                                 
	SInt16		mBitsOfColorIndex;         
	SInt16		m_SavedBitsOfColor;
	
	SInt16		mDuplicateChannelCode;
	SInt16		mMaxSystemPixelSize;
	SInt16		m_MinMaxCode;                        
	SInt16		m_PercentClip;
	SInt16		mRGBColors;
	                          
	UInt16		mColorLevelsMax[3][3];
	UInt16		mLocalDisplayType;
	UInt16		mMaxChannelNumber;
	double		mMinMaxValues[6];
	UInt16		mPixelSizeVector[3];
	
	Boolean		m_initializedFlag;
	Boolean		m_optionKeyFlag;
	Boolean		m_thematicDisplayWithUserSettingFlag;
	
public:
//	afx_msg void OnBnClickedRedchannelinvert();
	afx_msg void OnEnChangeLegendfactor();
	afx_msg void OnBnClickedCheck1();
};                         
  
#endif // !defined __WDISPDLG_H__
