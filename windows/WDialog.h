// WDialog.h : header file
// 
// Revised by Larry Biehl on 03/18/2017
   
#if !defined __MDIALOG_H__ 
	#define __MDIALOG_H__   
	
//new
#include <afxext.h>
//end new
	
/////////////////////////////////////////////////////////////////////////////
// CMDialog dialog

class CMDialog : public CDialog
{
// Construction
public:
							CMDialog(
								UINT 									IDD, 
								CWnd* 								pParent);	// standard constructor
								
	virtual 				~CMDialog();		// destructor stub 
	
	SInt16				GetComboListSelection(                     
								UInt16								dialogItemNumber,
								SInt32								selectedListItemData);
	
	SInt32				GetDialogItemValue(
								SInt16								itemNumber);
								
	void					HideShowAreaItems(
								Boolean								imageAreaFlag); 
                              			
	void 					LoadDItemString ( 
								UInt16								itemNumber, 
								Str255* 								theStringPtr,
								Boolean								wideCharInputStringFlag=false);
                         
	void 					LoadDItemString ( 
								UInt16								itemNumber, 
								CharPtr 								theStringPtr,
								Boolean								wideCharInputStringFlag=false);
                         
	void					LoadDItemValue ( 
								UInt16								itemNumber, 
								SInt32 								value);
							
	void					MHideDialogItem (
								DialogPtr							dialogPtr, 
								SInt16								itemNumber); 
							
	void					MShowDialogItem (
								DialogPtr							dialogPtr, 
								SInt16								itemNumber);
	
	void					SetComboItemText(                     
								UInt16								dialogItemNumber,
								SInt16								comboItem,
								UCharPtr								stringPtr,
								UInt16								stringCharCode=kASCIICharString); 
							
	void					SetEntireImageButtons (               
								DialogPtr							dialogPtr,
								SInt32								lineStart,
								SInt32								lineEnd,
								SInt32								columnStart,
								SInt32								columnEnd);
								
	void					SetMaxNumberLinesAndColumns (
								UInt32								maxNumberLines,
								UInt32								maxNumberColumns);

	void					SetStaticTextOrDropDownList ( 
								SInt16								dropDownListSelection, 
								SInt16								dropDownListID, 
								SInt16								staticTextID,
								Boolean								deactivatedFlag);
	
// Implementation
protected:
	void					CheckColumnEnd(void);
	 
	void					CheckColumnInterval(void);
	 
	void					CheckColumnStart(void);
	
	void					CheckLineEnd(void);
	 
	void					CheckLineInterval(void);
	
	void					CheckLineStart(void);
	
	Boolean				CheckLocalValue(
								SInt16					itemNumber,
								UINT*						numberValuePtr);
								
	void					HandleChannelsMenu(
								UInt16					channelsMenuItemNumber,
								Boolean					tranformationFlag,
								SInt16					totalNumberChannels,
								SInt16					channelMenuType,
								Boolean					okFlagSetting);
								
	void					HandleClassesMenu(                    
								UInt32*					numberOutputClassesPtr,
								SInt16*					classListPtr,
								SInt16					minimumNumberClasses, 
								SInt16					numberInputClasses,
								UInt16					classesMenuItemNumber,
								int*						classMenuSelectionPtr);        



	void					HandleClassPairWeightsMenu( 
								SInt16**	 							weightsListPtrPtr,
								UInt16								classPairWeightsMenuItemNumber,
								int*									classPairWeightsMenuSelectionPtr,
								SInt16*								defaultClassPairWeightPtr);
								
	void					HandleClassWeightsMenu(                    
								UInt16					numberOutputClassesToUse,
								SInt16*					classListPtr,
								float*					weightsListPtr, 
								Boolean					useEnhancedStatFlag,
								UInt16					classWeightsMenuItemNumber,
								int*						classWeightsMenuSelectionPtr);
								
	void					HideSomeAreaSelectionItems();
				
//	void					LoadLineColumnItems (
//								DialogSelectAreaPtr	dialogSelectAreaPtr,
//								DialogPtr				dialogPtr); 
								
	void 					OnSelendokClassCombo(void); 
								
	void 					OnFeatureTransformation(void);
	
	void					PositionDialogWindow (void);
							
	void					SetDLogControl (
								DialogPtr				dialogPtr, 
								SInt16					itemNumber, 
								SInt16					setting);
							
	void					SetDLogControlHilite (
								DialogPtr				dialogPtr, 
								SInt16					itemNumber, 
								SInt16					setting);
							                  	
	void					ShowHideDialogItem (
								DialogPtr				dialogPtr, 
								SInt16					itemNumber, 
								Boolean					showFlag); 
	
	void					ShowSomeAreaSelectionItems();
								
	void					UpdateAllSubsetList(
								UInt16					channelsMenuItemNumber);
								
	UInt16				UpdateOneColListSelections (
								UInt16					numberInputVecItems, 
								UInt16					listStart, 
								UInt16					listEnd, 
								UInt16					listInterval);

	void					VerifyLineColumnStartEndValues(
								CDataExchange* 					pDX);
								
	SInt16				VerifyLineColumnValues(
								CDataExchange* 		pDX, 
								SInt16					startLineItem,
								SInt16					startColumnItem,
								SInt16					pixelSize,
								SInt16					displayType,
								SInt16					numberChannels); 
							
	void					ToEntireImage(void);
							
	void					ToSelectedImage(void); 
	
	CBitmapButton  	toEntireButton; 
	CBitmapButton  	toSelectedButton;
//	CMImageView*		m_imageViewCPtr;
	
	CString	 			m_value;
	
	TBYTE*				m_valuePtr; 
							
	DialogSelectArea	m_dialogSelectArea;
	
	BOOL					m_featureTransformationFlag;                                                                
                                                    
   Boolean				m_entireImageFlag, 
   						m_featureTransformAllowedFlag,
   						m_initializedFlag;
	
	CDataExchange		*m_dialogFromPtr,
							*m_dialogToPtr;   
	
	unsigned char		*m_localSymbolsPtr;
	
	UInt16				*m_availableFeaturePtr,
							*m_classListPtr,
							*m_localActiveFeaturesPtr,
							*m_localFeaturesPtr,
							*m_localTransformFeaturesPtr; 
							
//	WindowInfoPtr		m_imageWindowInfoPtr;
	
	int					m_channelSelection;
	int					m_classSelection;   
	int					m_symbolSelection;
                              
	SInt32				m_ColumnEnd;
	SInt32				m_ColumnInterval;
	SInt32				m_ColumnStart;
	SInt32				m_LineEnd;
	SInt32				m_LineInterval;
	SInt32				m_LineStart;
	 
	SInt32				m_maxNumberColumns;
	SInt32				m_maxNumberLines;
	  
	UInt32				m_localNumberClasses;
	
	SInt16				m_channelListType,
							m_outputStorageType;            
							
	UInt16				m_localActiveNumberFeatures,
							m_localNumberFeatures,
							m_localNumberTransformFeatures; 
	 
	UInt16				m_numberEigenvectors; 

	Boolean				m_settingSelectedEntireButton;
   
};  

#endif // !defined __MDIALOG_H__
