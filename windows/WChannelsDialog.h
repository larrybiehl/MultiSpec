// WChannelsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMChannelsDlg dialog   

#if !defined __WCHANDLG_H__
	#define	__WCHANDLG_H__ 
	
	#include "WOneColumnDialog.h"

class CMChannelsDlg : public CMOneColDlg
{
// Construction
public:
							CMChannelsDlg(CWnd* pParent = NULL);	// standard constructor
	
	Boolean				DoDialog(
								SInt16*								numberOutputFeaturesPtr, 
								SInt16* 								selectedFeaturePtr, 
								LayerInfoPtr						layerInfoPtr,
								FileInfoPtr							fileInfoPtr, 
								SInt16								listType, 
								Boolean								useTransformFlag, 
								SInt16* 								availableFeaturePtr, 
								SInt16								numberInputChannels,
								SInt16								currentSelection);

// Dialog Data
	//{{AFX_DATA(CMChannelsDlg)
	//}}AFX_DATA

// Implementation
protected:                                               
				
	virtual void 		DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 

	// Generated message map functions
	//{{AFX_MSG(CMChannelsDlg)
	virtual BOOL OnInitDialog(); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	                                       
	LayerInfoPtr		m_layerInfoPtr;
	FileInfoPtr			m_fileInfoPtr; 
	Boolean				m_useTransformFlag; 
	SInt16* 				m_availableFeaturePtr; 
	
	Boolean				m_initializedFlag; 
	
};		// end "CMChannelsDlg" 

                     

class CMChannelCombinationsDlg : public CMOneColDlg
{
// Construction
public:
							CMChannelCombinationsDlg(CWnd* pParent = NULL);	// standard constructor
	
	Boolean				DoDialog(
								SInt16								channelCombinationSelection,
								UInt16*								numberOutputChannelCombinationsPtr, 
								UInt16* 								channelCombinationsPtr, 
								UInt32								numberInputChannelCombinations, 
								UInt16*								allChanCombinationsPtr,
								UInt32								contiguousChannelsPerGroup,
								Boolean								useTransformFlag);

// Dialog Data
	//{{AFX_DATA(CMChannelCombinationsDlg)
	//}}AFX_DATA

// Implementation
protected:                                               
				
	virtual void 		DoDataExchange(CDataExchange* pDX);	// DDX/DDV support 

	// Generated message map functions
	//{{AFX_MSG(CMChannelCombinationsDlg)
	virtual BOOL OnInitDialog(); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	                                      
	SInt16* 					m_availableFeaturePtr;
	
	SInt16					m_channelCombinationSelection,
								m_contiguousChannelsPerGroup; 
	
	Boolean					m_initializedFlag, 
								m_useTransformFlag; 
	
};		// end "CMChannelCombinationsDlg"                     
   
#endif	// !defined __WCHANDLG_H__
