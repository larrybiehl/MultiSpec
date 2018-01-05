// WDialog.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
                    
#include "SMultiSpec.h"

#include "CImageWindow.h"
#include "CProcessor.h"

#include "WChannelsDialog.h"
#include "WClassesDialog.h" 
#include "WClassWeightsDialog.h" 
#include "WDialog.h"
#include "WImageView.h"

//#include	"SExtGlob.h"	

extern Boolean 		GetSelectionRectangle (
								WindowPtr							windowPtr,
								LongRect*							selectionRectanglePtr, 
								Boolean								clearSelectionFlag, 
								Boolean								useThresholdFlag, 
								Boolean								adjustToBaseImageFlag);

extern SInt16			GetComboListSelection(
								CWnd*									cWindowPtr,
								UInt16								dialogItemNumber,
								SInt32								selectedListItemData);  

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMDialog dialog


CMDialog::CMDialog(UINT IDD, CWnd* pParent)
	: CDialog(IDD, pParent)

{
	m_ColumnEnd = 1;
	m_ColumnInterval = 1;
	m_ColumnStart = 1;
	m_LineEnd = 1;
	m_LineInterval = 1;
	m_LineStart = 1; 
	m_channelSelection = 0; 
	m_channelListType = 1; 
	m_classSelection = 0;
	m_featureTransformationFlag = FALSE; 
	m_symbolSelection = 0;
	
	m_dialogFromPtr = NULL;
	m_dialogToPtr = NULL;
	
	m_valuePtr = NULL;
	
	m_availableFeaturePtr = NULL;
	m_localActiveFeaturesPtr = NULL;
	m_localFeaturesPtr = NULL; 
	m_localTransformFeaturesPtr = NULL;
	
	m_classListPtr = NULL; 
	
	m_localSymbolsPtr = NULL;  
	
			// Get the default image window info pointer. It may
			// be changed later by super classes of this.
			
	m_dialogSelectArea.imageWindowInfoPtr = gImageWindowInfoPtr;
	m_dialogSelectArea.windowPtr = NULL;
	
	m_localActiveNumberFeatures = 0;
	m_localNumberFeatures = 0;
	m_localNumberTransformFeatures = 0;
	
	m_localNumberClasses = 0;  
	
	m_outputStorageType = 0;

			// m_settingSelectedEntireButton is used to indicate when the selected/
			// entire button is being set so that no checks are made of the line
			// and column values being used.

	m_settingSelectedEntireButton = FALSE;
	
	//m_imageViewCPtr = gActiveImageViewCPtr;
	
	m_initializedFlag = FALSE; 
	
			// Set limit values   
	
	if (m_dialogSelectArea.imageWindowInfoPtr != NULL)
		{
		m_maxNumberLines = m_dialogSelectArea.imageWindowInfoPtr->maxNumberLines;
		m_maxNumberColumns = m_dialogSelectArea.imageWindowInfoPtr->maxNumberColumns;
		
		}		// end "m_dialogSelectArea.imageWindowInfoPtr != NULL"
		
	else		// m_dialogSelectArea.imageWindowInfoPtr == NULL
		{
		m_maxNumberLines = 1;
		m_maxNumberColumns = 1;
		
		}		// end "else m_dialogSelectArea.imageWindowInfoPtr == NULL"
		
			// Get memory for needed objects.
			
	TRY
		{                          
		m_dialogFromPtr = new CDataExchange(this, TRUE);
	
		m_dialogToPtr = new CDataExchange(this, FALSE);
	                         
		m_valuePtr = (TBYTE*)m_value.GetBuffer(255); 
		
		m_initializedFlag = TRUE;
		
		}
		
	CATCH_ALL(e)
		{                         
		MemoryMessage(0, kObjectMessage);
		
		}
		
	END_CATCH_ALL   

}		// end "CMDialog"


// ---------------------------------------------------------------------------
//		~CMDialog
// ---------------------------------------------------------------------------
//	Destructor
//

CMDialog::~CMDialog()
{                          
	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr; 
			
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;
			
	m_value.Empty();                 

}		// end "~CMDialog"   
 

void CMDialog::CheckColumnEnd (void)

{                   
	if (!m_settingSelectedEntireButton)
		{                                        
		DDX_Text(m_dialogFromPtr, IDC_ColumnEnd, m_value);
															
		if (m_valuePtr[0] != 0)                      
			DDX_Text(m_dialogFromPtr, IDC_ColumnEnd, m_ColumnEnd);
				
		else		// m_valuePtr[0] == 0
			m_ColumnEnd = 0;
			
		if ((UInt32)m_ColumnEnd > (UInt32)m_maxNumberColumns)
			{
			m_ColumnEnd = m_maxNumberColumns;
															
			DDX_Text(m_dialogToPtr, IDC_ColumnEnd, m_ColumnEnd);
								
			}		// end "if (m_ColumnEnd > m_maxNumberColumns)"
			
		SetEntireImageButtons (
							NULL, 
							m_LineStart, 
							m_LineEnd, 
							m_ColumnStart, 
							m_ColumnEnd); 

		}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckColumnEnd"   


void CMDialog::CheckColumnInterval (void)

{                   
	DDX_Text(m_dialogFromPtr, IDC_ColumnInterval, m_value);
														
	if (m_valuePtr[0] != 0)                      
		DDX_Text (m_dialogFromPtr, IDC_ColumnInterval, m_ColumnInterval);
			
	else		// m_valuePtr[0] == 0
		m_ColumnInterval = 0;
		
	if ((UInt32)m_ColumnInterval > (UInt32)m_maxNumberColumns)
		{
		m_ColumnInterval = m_maxNumberColumns;
														
		DDX_Text (m_dialogToPtr, IDC_ColumnInterval, m_ColumnInterval);
							
		}		// end "if (m_ColumnEnd > m_maxNumberColumns)"
	
}		// end "CheckColumnInterval" 
  

void CMDialog::CheckColumnStart()

{        
	if (!m_settingSelectedEntireButton)
		{                                                                                    
		DDX_Text (m_dialogFromPtr, IDC_ColumnStart, m_value);
															
		if (m_valuePtr[0] != 0)                      
			DDX_Text (m_dialogFromPtr, IDC_ColumnStart, m_ColumnStart);
				
		else		// m_valuePtr[0] == 0
			m_ColumnStart = 0;  
		
		SetEntireImageButtons (
							NULL, 
							m_LineStart, 
							m_LineEnd, 
							m_ColumnStart, 
							m_ColumnEnd);

		}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckColumnStart"
 

void CMDialog::CheckLineEnd()
{  
	if (!m_settingSelectedEntireButton)
		{                                                        
		DDX_Text (m_dialogFromPtr, IDC_LineEnd, m_value);
															
		if (m_valuePtr[0] != 0)                      
			DDX_Text (m_dialogFromPtr, IDC_LineEnd, m_LineEnd);
				
		else		// m_valuePtr[0] == 0
			m_LineEnd = 0; 
		
		if ((UInt32)m_LineEnd > (UInt32)m_maxNumberLines)
			{
			m_LineEnd = m_maxNumberLines;
														
			DDX_Text (m_dialogToPtr, IDC_LineEnd, m_LineEnd);
							
			}		// end "if (m_LineEnd > m_maxNumberLines)"
		
		SetEntireImageButtons (
							NULL, 
							m_LineStart, 
							m_LineEnd, 
							m_ColumnStart, 
							m_ColumnEnd);

	}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckLineEnd"
  

void CMDialog::CheckLineInterval(void)

{                   
	DDX_Text (m_dialogFromPtr, IDC_LineInterval, m_value);
														
	if (m_valuePtr[0] != 0)                      
		DDX_Text (m_dialogFromPtr, IDC_LineInterval, m_LineInterval);
			
	else		// m_valuePtr[0] == 0
		m_LineInterval = 0;
		
	if ((UInt32)m_LineInterval > (UInt32)m_maxNumberLines)
		{
		m_LineInterval = m_maxNumberLines;
														
		DDX_Text (m_dialogToPtr, IDC_LineInterval, m_LineInterval);
							
		}		// end "if (m_ColumnEnd > m_maxNumberColumns)"
	
}		// end "CheckLineInterval" 
  

void CMDialog::CheckLineStart()
{  
	if (!m_settingSelectedEntireButton)
		{
		DDX_Text (m_dialogFromPtr, IDC_LineStart, m_value);
															
		if (m_valuePtr[0] != 0)                      
			DDX_Text (m_dialogFromPtr, IDC_LineStart, m_LineStart);
				
		else		// m_valuePtr[0] == 0
			m_LineStart = 0;
		
		SetEntireImageButtons (
							NULL, 
							m_LineStart, 
							m_LineEnd, 
							m_ColumnStart, 
							m_ColumnEnd);

		}		// end "if (!m_settingSelectedEntireButton)"
	
}		// end "CheckLineStart" 
  
/*
void 
CMDialog::CheckLocalValue(
				SInt16		itemNumber,
				UINT*			numberValuePtr)

{                                                               
	DDX_Text(m_dialogFromPtr, itemNumber, m_value);
		                                    
	if (m_valuePtr[0] != 0)                      
		DDX_Text(m_dialogFromPtr, itemNumber, *numberValuePtr);
			
	else		// m_valuePtr[0] == 0
		*numberValuePtr = 0;
	
}		// end "CheckLocalValue" 
*/  

Boolean CMDialog::CheckLocalValue(
				SInt16			itemNumber,
				UINT*				numberValuePtr)

{  
	Boolean		returnFlag;
	
	                                                             
	DDX_Text (m_dialogFromPtr, itemNumber, m_value);
		                                    
	if (m_valuePtr[0] != 0)
		{                      
		DDX_Text(m_dialogFromPtr, itemNumber, *numberValuePtr);
		returnFlag = TRUE;
		
		}		// end "if (m_valuePtr[0] != 0)"
			
	else		// m_valuePtr[0] == 0
		returnFlag = FALSE;
		
	return (returnFlag);
	
}		// end "CheckLocalValue"  


SInt32 CMDialog::GetDialogItemValue(
				SInt16						itemNumber)
{  
	SInt32							value = 0;
	                                                           
	DDX_Text (m_dialogFromPtr, itemNumber, m_value);
		                                    
	if (m_valuePtr[0] != 0)
		DDX_Text (m_dialogFromPtr, itemNumber, value);
		
	return (value);															
	
}		// end "GetDialogItemValue"   


SInt16 CMDialog::GetComboListSelection(
				UInt16								dialogItemNumber,
				SInt32								selectedListItemData)

{     
	return (::GetComboListSelection(this,                     
										dialogItemNumber,
										selectedListItemData) );
				
/*
	CComboBox* 						comboBoxPtr;
	
	UInt32							index,
										numberComboItems;
										
	SInt16							comboListSelection;
	 
	                                
	comboListSelection = -1;
			
	comboBoxPtr = (CComboBox*)GetDlgItem(dialogItemNumber);
	
	if (comboBoxPtr != NULL)
		{                         		
		numberComboItems = (UInt32)comboBoxPtr->GetCount();
		for (index=0; index<numberComboItems; index++)
			{
			if ( (SInt32)comboBoxPtr->GetItemData( (SInt16)index ) == selectedListItemData )
				comboListSelection = (SInt16)index;
			
			}		// end "for (index=0; index<numberComboItems; index++)" 
			
		}		// end "if (comboBoxPtr != NULL)"
		
	return (comboListSelection); 
*/	
}		// end "GetComboListSelection"     



void CMDialog::HandleChannelsMenu(
				UInt16								channelsMenuItemNumber,
				Boolean								transformationFlag,
				SInt16								totalNumberChannels,
				SInt16								channelMenuType,
				Boolean								okFlagSetting)

{        
	int									channelSelection;
	
	                                                   
	DDX_CBIndex(m_dialogFromPtr, channelsMenuItemNumber, channelSelection);
	                                           
	if (channelSelection == kSubsetMenuItem || 
			(channelSelection == kAllMenuItem && 
							m_channelListType == kSelectedItemsListOnly))
		{
				// Subset of channels to be used.							
								                            
		SetDLogControlHilite (NULL, IDOK, 255);  
		
		CMChannelsDlg*		channelsDialogPtr = NULL;
		
		TRY
			{                                
			channelsDialogPtr = new CMChannelsDlg();
			                                                                     
			channelsDialogPtr->DoDialog (
								(SInt16*)&m_localActiveNumberFeatures,
								(SInt16*)m_localActiveFeaturesPtr,
								gImageLayerInfoPtr,
								gImageFileInfoPtr,
								m_channelListType,
								transformationFlag,
								(SInt16*)m_availableFeaturePtr,
								totalNumberChannels,
								m_channelSelection); 
				
		   if (channelsDialogPtr != NULL)                           
				delete channelsDialogPtr;
			}
			
		CATCH_ALL(e)
			{
			MemoryMessage(0, kObjectMessage);
			}
		END_CATCH_ALL 
		
		if (channelMenuType == kImageChannelType)
			{ 											
			if (m_localActiveNumberFeatures == (UInt16)totalNumberChannels)
				channelSelection = kAllMenuItem;          
				
			}		// end "if (channelMenuType == kImageChannelType)"  
			
		if (okFlagSetting)						                                
			SetDLogControlHilite (NULL, IDOK, 0);
						
		}		// end "if (m_channelSelection == kSubsetMenuItem)"
		
	else		// channelSelection == kAllMenuItem
		{                                
		if (channelMenuType == kImageChannelType)			
			m_localActiveNumberFeatures = totalNumberChannels; 
			
		}		// end "else channelSelection == kAllMenuItem" 
		               
	if (channelMenuType == kProjectChannelType)
		{     
		channelSelection = UpdateDialogNumberFeatureParameters (
															transformationFlag, 
															&m_localActiveNumberFeatures,
															&m_localNumberFeatures,
															totalNumberChannels, 
															&m_localNumberTransformFeatures,
															gTransformationMatrix.numberFeatures,
															channelSelection);
															
		}		// end "if (channelMenuType == kProjectChannelType)"
		 
	m_channelSelection = channelSelection;		                                  
	DDX_CBIndex (m_dialogToPtr, channelsMenuItemNumber, m_channelSelection);
	
}		// end "HandleChannelsMenu"      


void CMDialog::HandleClassesMenu(
				UInt32*									numberOutputClassesPtr,
				SInt16*									classListPtr,
				SInt16									minimumNumberClasses,
				SInt16									numberInputClasses,
				UInt16									classesMenuItemNumber,
				int*										classMenuSelectionPtr)

{  
	int									classMenuSelection;
	
	Boolean								returnFlag;
	
	                                                                                                 
	DDX_CBIndex(m_dialogFromPtr, classesMenuItemNumber, classMenuSelection);
	                                           
	if (classMenuSelection == kSubsetMenuItem)
		{
				// Subset of classes to be used.							
								                             
		SetDLogControlHilite (NULL, IDOK, 255); 
							
				// If all classes is the current setting, then assume that
				// the subset is the single active project class.
		
		if (*classMenuSelectionPtr == kAllMenuItem && 
														gProjectInfoPtr->currentClass >= 0)
			{
			*numberOutputClassesPtr = 1;
			classListPtr[0] = gProjectInfoPtr->currentClass + 1;
			
			}		// end "if (currentSelection = kAllMenuItem && ..."
		
		CMClassesDlg*		classesDialogPtr = NULL;
		
		TRY
			{                                
			classesDialogPtr = new CMClassesDlg();
			                                                                     
			returnFlag = classesDialogPtr->DoDialog (numberOutputClassesPtr,
																	classListPtr,
																	minimumNumberClasses,          
																	(UInt16)numberInputClasses); 
		   
		   if (classesDialogPtr != NULL)                           
				delete classesDialogPtr;
			}
			
		CATCH_ALL(e)
			{
			MemoryMessage(0, kObjectMessage);
			}
		END_CATCH_ALL 
									
		if ( !returnFlag && *classMenuSelectionPtr == kAllMenuItem )  
			*numberOutputClassesPtr = gProjectInfoPtr->numberStatisticsClasses;
									
		if ( *numberOutputClassesPtr == gProjectInfoPtr->numberStatisticsClasses )
			{                                     
			classMenuSelection = kAllMenuItem;
			DDX_CBIndex(m_dialogToPtr, classesMenuItemNumber, classMenuSelection);
							
			}		// end "if (*numberOutputClassesPtr == ..." 
									                                
		SetDLogControlHilite (NULL, IDOK, 0);
						
		}		// end "if (classMenuSelection == kSubsetMenuItem)"
		
	else		// classMenuSelection == kAllMenuItem
		{                                
		*numberOutputClassesPtr = gProjectInfoPtr->numberStatisticsClasses;
		
		}		// end "else classMenuSelection == kAllMenuItem"
		
	*classMenuSelectionPtr = classMenuSelection;             
	
}		// end "HandleClassesMenu"         


void CMDialog::HandleClassPairWeightsMenu(
				SInt16**									weightsListPtrPtr,
				UInt16									classPairWeightsMenuItemNumber,
				int*										classPairWeightsMenuSelectionPtr,
				SInt16*									defaultClassPairWeightPtr)

{  
	int						classPairWeightsMenuSelection;
	
	                                                                                                 
	DDX_CBIndex (m_dialogFromPtr,
						classPairWeightsMenuItemNumber, 
						classPairWeightsMenuSelection);
	                                           
	if (classPairWeightsMenuSelection == kUnequalWeightMenuItem)
		{
				// Subset of classes to be used.							
								                             
		SetDLogControlHilite (NULL, IDOK, 255); 
		
		classPairWeightsMenuSelection = ClassPairWeightsDialog (
														gProjectInfoPtr->numberStatisticsClasses,
							 							weightsListPtrPtr,
														*classPairWeightsMenuSelectionPtr,
														defaultClassPairWeightPtr);

		SetDLogControlHilite (NULL, IDOK, 0);
	   
		if (classPairWeightsMenuSelection == kEqualWeightMenuItem)
			DDX_CBIndex (m_dialogToPtr,
								classPairWeightsMenuItemNumber, 
								classPairWeightsMenuSelection); 
						
		}		// end "if (classWeightsMenuSelection == kUnequalWeightMenuItem)"
		
	*classPairWeightsMenuSelectionPtr = classPairWeightsMenuSelection;             
	
}		// end "HandleClassPairWeightsMenu"            


void CMDialog::HandleClassWeightsMenu(
								UInt16					numberOutputClassesToUse,
								SInt16*					classListPtr,
								float*					weightsListPtr, 
								Boolean					useEnhancedStatFlag,
								UInt16					classWeightsMenuItemNumber,
								int*						classWeightsMenuSelectionPtr)

{  
	int						classWeightsMenuSelection;
	
	                                                                                                 
	DDX_CBIndex (m_dialogFromPtr,
						classWeightsMenuItemNumber, 
						classWeightsMenuSelection);
	                                           
	if (classWeightsMenuSelection == kUnequalWeightMenuItem)
		{
				// Subset of classes to be used.							
								                             
		SetDLogControlHilite (NULL, IDOK, 255); 
		
		classWeightsMenuSelection = ClassWeightsDialog (
														numberOutputClassesToUse, 
							 							classListPtr,
														weightsListPtr,
														*classWeightsMenuSelectionPtr, 
														useEnhancedStatFlag);

		SetDLogControlHilite (NULL, IDOK, 0);
	   
		if (classWeightsMenuSelection == kEqualWeightMenuItem)
			DDX_CBIndex (m_dialogToPtr,
								classWeightsMenuItemNumber, 
								classWeightsMenuSelection); 
						
		}		// end "if (classWeightsMenuSelection == kUnequalWeightMenuItem)"
		
	*classWeightsMenuSelectionPtr = classWeightsMenuSelection;             
	
}		// end "HandleClassWeightsMenu"    


void CMDialog::HideShowAreaItems(
				Boolean									imageAreaFlag)
				
{  
	if (imageAreaFlag)
		{ 
		MShowDialogItem (this, IDC_LineColFrame);
		 
//		MShowDialogItem (this, IDEntireImage); 
//		MShowDialogItem (this, IDSelectedImage);
		
		MShowDialogItem (this, IDC_StartEndInterval);
		MShowDialogItem (this, IDC_LinePrompt);
		MShowDialogItem (this, IDC_ColumnPrompt);
		MShowDialogItem (this, IDC_LineStart);
		MShowDialogItem (this, IDC_LineEnd);
		MShowDialogItem (this, IDC_LineInterval);
		MShowDialogItem (this, IDC_ColumnStart);
		MShowDialogItem (this, IDC_ColumnEnd);  
		MShowDialogItem (this, IDC_ColumnInterval);
	
		}		// end "if (imageAreaFlag)"
	
	else		// !imageAreaFlag
		{	                           
		MHideDialogItem (this, IDC_LineColFrame); 
		MHideDialogItem (this, IDEntireImage);
		MHideDialogItem (this, IDSelectedImage);
		MHideDialogItem (this, IDC_StartEndInterval);
		MHideDialogItem (this, IDC_LinePrompt);
		MHideDialogItem (this, IDC_ColumnPrompt);
		MHideDialogItem (this, IDC_LineStart);
		MHideDialogItem (this, IDC_LineEnd);
		MHideDialogItem (this, IDC_LineInterval);
		MHideDialogItem (this, IDC_ColumnStart);
		MHideDialogItem (this, IDC_ColumnEnd);  
		MHideDialogItem (this, IDC_ColumnInterval); 
		
		}		// end "else		// !imageAreaFlag"
	
}		// end "HideShowAreaItems"                           


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadDItemString
//
//	Software purpose:	The purpose of this routine is to hide the set of
//							area selection items such as line start, etc
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 06/01/1995

void CMDialog::HideSomeAreaSelectionItems()

{                                                          
	MHideDialogItem (this, IDEntireImage);
	MHideDialogItem (this, IDSelectedImage);                 
	MHideDialogItem (this, IDC_LineStart);          
	MHideDialogItem (this, IDC_LineEnd);          
	MHideDialogItem (this, IDC_ColumnStart);          
	MHideDialogItem (this, IDC_ColumnEnd);   
	
}		// end "HideSomeAreaSelectionItems"  


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadDItemString
//
//	Software purpose:	The purpose of these routines are to load the input
//							string into the specifed dialog item.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 03/17/2017

void CMDialog::LoadDItemString (
				UInt16								itemNumber,
				Str255*								theStringPtr,
				Boolean								wideCharInputStringFlag)

{              
	char*						tempCharPtr;

	tempCharPtr = (char*)theStringPtr;
	CharPtr stringPtr = (CharPtr)&tempCharPtr[1];
	
	LoadDItemString (itemNumber, stringPtr, wideCharInputStringFlag);
						  
}		// end "LoadDItemString"  


void CMDialog::LoadDItemString (
				UInt16								itemNumber,
				CharPtr								theStringPtr,
				Boolean								wideCharInputStringFlag)

{
	CString				string;


	USES_CONVERSION;

	if (wideCharInputStringFlag)
		string = CString ((wchar_t*)theStringPtr);

	else		// !wideCharInputStringFlag
		{			
		//TBYTE		wideFileName[256];
		TBYTE*					tempUnicodeCharacterStringPtr;

		tempUnicodeCharacterStringPtr = 
							ConvertMultibyteStringToUnicodeString ((UCharPtr)theStringPtr);
		/*
		int sizeNeeded = MultiByteToWideChar (
									CP_UTF8, 0, (LPCSTR)theStringPtr, -1, NULL, 0);
		sizeNeeded = MIN(sizeNeeded, 255);
		MultiByteToWideChar (CP_UTF8, 0, (LPCSTR)theStringPtr, -1, (LPWSTR)wideFileName, sizeNeeded);
		*/
		string = CString (&tempUnicodeCharacterStringPtr[1]);

		}		// else !wideCharInputStringFlag
	                      
	DDX_Text (m_dialogToPtr, itemNumber, string);
						  
}		// end "LoadDItemString"  


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadDItemValue
//
//	Software purpose:	The purpose of this routine is to load the input value
//							into the specified dialog item number.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 06/01/1995
                         
void CMDialog::LoadDItemValue (
				UInt16									itemNumber,
				SInt32									value)

{                        
	DDX_Text (m_dialogToPtr, itemNumber, value);
						  
}		// end "LoadDItemValue" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MHideDialogItem
//
//	Software purpose:	The purpose of this routine is to hide the specifed dialog 
//							item.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 06/01/1995

void CMDialog::MHideDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)
				
{
// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN

#	if !defined _X86_ && !defined _AMD64_
		::HideDialogItem (dialogPtr, itemNumber);
#	endif // !defined _X86_ && !defined _AMD64_

#	if defined _X86_ || defined _AMD64_
		CWnd* dialogItemWindowPtr = GetDlgItem(itemNumber);
   
		if (dialogItemWindowPtr != NULL)
			dialogItemWindowPtr->ShowWindow(SW_HIDE);
#	endif // defined _X86_ || defined _AMD64_
	
}		// end "MHideDialogItem"  


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MShowDialogItem
//
//	Software purpose:	The purpose of this routine is to show the specifed dialog 
//							item.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 06/01/1995

void CMDialog::MShowDialogItem (
				DialogPtr			dialogPtr, 
				SInt16				itemNumber)
				
{    
// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if !defined _X86_ && !defined _AMD64_
		ShowDItem (dialogPtr, itemNumber);
#	endif // !defined _X86_ && !defined _AMD64_

// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if defined _X86_ || defined _AMD64_
		CWnd* dialogItemWindowPtr = GetDlgItem(itemNumber);
   
		if (dialogItemWindowPtr != NULL)
			dialogItemWindowPtr->ShowWindow(SW_SHOW);
#	endif // defined _X86_ || defined _AMD64_
	
}		// end "MShowDialogItem"


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PositionDialogWindow
//
//	Software purpose:	The purpose of this routine is to set the hilite
//							of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/29/1995
//	Revised By:			Larry L. Biehl			Date: 06/29/1995

void CMDialog::PositionDialogWindow (void)

{  
// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if !defined _X86_ && !defined _AMD64_

#	endif	// !defined _X86_

#	if defined _X86_ || defined _AMD64_
		::PositionDialogWindow ((CDialog*)this);
/*   
	Rect			dialogRect,
					screenRect; 

	SInt16		newLeft,
					newTop;
				

			// Get rectangle for dialog box
		
	GetWindowRect( (tagRECT*)&dialogRect );
	
			// Get rectangle for screen
			
	GetScreenRect(&screenRect);
	
			// Calculate upper left corner that will leave the box centered.
			
	newLeft = dialogRect.left + (screenRect.right + screenRect.left)/2 -
					(dialogRect.right + dialogRect.left)/2;
			
	newTop = dialogRect.top + (screenRect.top + screenRect.bottom)/2 -
					(dialogRect.top + dialogRect.bottom)/2;
					
			// Adjust so that 1/5 of space is above and 4/5's of space is below
			
	newTop -= (newTop - screenRect.top) * 6/10;
	
	Boolean returnFlag = SetWindowPos (
										NULL, 
										newLeft, 
										newTop, 
										0, 
										0, 
										SWP_NOSIZE | SWP_NOZORDER);
*/
#	endif	// defined _X86_ || defined _AMD64_

}		// end "PositionDialogWindow"  


void CMDialog::SetComboItemText (
				UInt16								dialogItemNumber,
				SInt16								comboItem,
				UCharPtr								stringPtr,
				UInt16								stringCharCode)

{                           
	//TBYTE								tempWideCharacterString[256];
	TBYTE*							tempWideCharacterStringPtr;
	CComboBox* 						comboBoxPtr;
	UInt32							numberComboItems;
	
	
#	if defined multispec_win
		USES_CONVERSION;
#	endif
	                                
	comboBoxPtr = (CComboBox*)GetDlgItem (dialogItemNumber);
	
	if (comboBoxPtr != NULL)
		{ 
		numberComboItems = comboBoxPtr->GetCount();

		if (stringCharCode == kUTF8CharString)
			{
			tempWideCharacterStringPtr = ConvertMultibyteStringToUnicodeString (stringPtr);
			/*
			int sizeNeeded = MultiByteToWideChar (
									CP_UTF8, 0, (LPCSTR)stringPtr, -1, NULL, 0);

			sizeNeeded = MIN(sizeNeeded, 254);
			MultiByteToWideChar (CP_UTF8, 0, (LPCSTR)stringPtr, -1, (LPWSTR)tempWideCharacterString, sizeNeeded);
			*/
			}	// end "if (stringCharCode == kUTF8CharString)"
			
		if (numberComboItems > (UInt32)comboItem)
			{                          
			comboBoxPtr->DeleteString (comboItem);

			if (stringCharCode == kUTF8CharString)
				comboBoxPtr->InsertString (comboItem, (LPCTSTR)&tempWideCharacterStringPtr[1]);

			else	// stringCharCode != kUTF8CharString
				comboBoxPtr->InsertString (comboItem, (LPCTSTR)A2T((LPSTR)stringPtr));
				//comboBoxPtr->InsertString (comboItem, stringPtr);
			
			}		// end "if (numberComboItems > 0 && ..." 
			
		else		// numberComboItems == 0 || ... < (UInt32)comboItem  
			{
 			if (stringCharCode == kUTF8CharString)
				comboBoxPtr->AddString ((LPCTSTR)&tempWideCharacterStringPtr[1]);

			else	// stringCharCode != kUTF8CharString
				comboBoxPtr->AddString ((LPCTSTR)A2T((LPSTR)stringPtr));           
				//comboBoxPtr->AddString (stringPtr);
			}	// end "else numberComboItems == 0 || ... < (UInt32)comboItem"
			
		}		// end "if (comboBoxPtr != NULL)"
	
}		// end "SetComboItemText" 


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDLogControl
//
//	Software purpose:	The purpose of this routine is to set the control
//							value of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			StatisticsDialog   in statistics.c
//
//	Coded By:			Larry L. Biehl			Date: 01/30/1989
//	Revised By:			Larry L. Biehl			Date: 06/02/1995

void CMDialog::SetDLogControl (
				DialogPtr			dialogPtr, 
				SInt16				itemNumber, 
				SInt16				setting)

{      
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if !defined _X86_ && !defined _AMD64_
		Handle				theHandle;
		Rect					theBox;
		
		SInt16				theType;
		
		
		if (dialogPtr != NULL)
			{
			GetDItem (dialogPtr, itemNumber,  &theType, &theHandle, &theBox);
			SetCtlValue ((ControlHandle)theHandle, setting);
			
			}		// end "if (dialogPtr != NULL)"
#	endif // !defined _X86_ && !defined _AMD64_
			
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if defined _X86_ || defined _AMD64_
		CButton* buttonPtr = (CButton*)GetDlgItem(itemNumber);
		
		if (buttonPtr != NULL)
			buttonPtr->SetCheck(setting);
		
		//CheckDlgButton	(itemNumber, setting);
#	endif	// defined _X86_ || _AMD64_
				
}		// end "SetDLogControl" 


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDLogControlHilite
//
//	Software purpose:	The purpose of this routine is to set the hilite
//							of the control in the dialog window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ControlDialog   in control.c
//
//	Coded By:			Larry L. Biehl			Date: 01/30/1989
//	Revised By:			Larry L. Biehl			Date: 06/02/1995

void CMDialog::SetDLogControlHilite (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt16								setting)

{  
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if !defined _X86_ && !defined _AMD64_
		Handle				theHandle;
		Rect					theBox;
		
		SInt16				theType;
		
				
		GetDItem (dialogPtr, itemNumber,  &theType, &theHandle, &theBox);
		HiliteControl ((ControlHandle)theHandle, setting);
#	endif // !defined _X86_ && !defined _AMD64_
	
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
#	if defined _X86_ || defined _AMD64_
	   CWnd* dialogItemWindowPtr = GetDlgItem(itemNumber);
	   
	   if (dialogItemWindowPtr != NULL)
	   	{ 
	   	if (setting == 0)
				dialogItemWindowPtr->EnableWindow(TRUE);
		
			else if (setting == 255)
				dialogItemWindowPtr->EnableWindow(FALSE);
				
			}		// end "if (dialogItemWindowPtr != NULL)"
#	endif	// defined _X86_ || defined _AMD64_
			
}		// end "SetDLogControlHilite"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetEntireImageButtons
//
//	Software purpose:	The purpose of this routine is to set the 'to entire image/
//							to selected image' buttons.
//		
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None.
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/16/1989
//	Revised By:			Larry L. Biehl			Date: 03/08/1996

void CMDialog::SetEntireImageButtons (
				DialogPtr			dialogPtr,
				SInt32				lineStart,
				SInt32				lineEnd,
				SInt32				columnStart,
				SInt32				columnEnd)

{  	
			// Determine if this is the entire area.
							                           
	m_entireImageFlag = CheckIfEntireImage (
						m_dialogSelectArea.imageWindowInfoPtr,
						lineStart, 
						lineEnd, 
						columnStart, 
						columnEnd);
						
	if (!m_entireImageFlag || !gSelectionRectangleFlag)
		{
		MHideDialogItem (dialogPtr, IDSelectedImage);
		MShowDialogItem (dialogPtr, IDEntireImage); 
		
		if (m_entireImageFlag) 
				// Unhilite the button until the option is needed.
				
			SetDLogControlHilite (dialogPtr, IDEntireImage, 255);
			
		else  
			SetDLogControlHilite (dialogPtr, IDEntireImage, 0);
		
		}		// end "if (!m_entireImageFlag || !gSelectionRectangleFlag)"
		
	else		// m_entireImageFlag && gSelectionRectangleFlag
		{
		MHideDialogItem (dialogPtr, IDEntireImage);
		MShowDialogItem (dialogPtr, IDSelectedImage);
		
		}		// end "else m_entireImageFlag && gSelectionRectangleFlag"
/*	
	if ( !gSelectionRectangleFlag )
		{
		if (m_entireImageFlag) 
				// Unhilite the button until the option is needed.
				
			SetDLogControlHilite (dialogPtr, IDEntireImage, 255);
			
		else  
			SetDLogControlHilite (dialogPtr, IDEntireImage, 0);
			
		}		// end "if ( !gSelectionRectangleFlag )"
*/				
}		// end "SetEntireImageButtons" 


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetMaxNumberLinesAndColumns
//
//	Software purpose:	The purpose of this routine is to set the maximum number
//							of lines and columns allowed.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1999
//	Revised By:			Larry L. Biehl			Date: 03/18/1999

void CMDialog::SetMaxNumberLinesAndColumns (
				UInt32								maxNumberLines,
				UInt32								maxNumberColumns)

{  
	m_maxNumberLines = maxNumberLines;
	m_maxNumberColumns = maxNumberColumns;
			
}		// end "SetMaxNumberLinesAndColumns"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetStaticTextOrDropDownList
//
//	Software purpose:	The purpose of this routine is to show
//							or hide the specifed dialog item dependant upon
//							the input flag.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/26/2012
//	Revised By:			Larry L. Biehl			Date: 03/01/2017	

void  CMDialog::SetStaticTextOrDropDownList ( 
				SInt16								dropDownListSelection, 
				SInt16								dropDownListID, 
				SInt16								staticTextID,
				Boolean								deactivatedFlag)
{
	TBYTE									listString[64];
	char									charString[64];
	CComboBox*							comboBoxPtr;
	int									stringLength;


	USES_CONVERSION;

	if (deactivatedFlag)
		{
		HideDialogItem (this, dropDownListID);
		ShowDialogItem (this, staticTextID);

		comboBoxPtr = (CComboBox*)GetDlgItem(dropDownListID);
		stringLength = comboBoxPtr->GetLBText (dropDownListSelection, (LPTSTR)listString);
		strncpy (charString, T2A((LPTSTR)listString), stringLength+1);
			
		LoadDItemString (staticTextID, charString); 

		}		// end "if (projectionDeactivatedFlag)"

	else		// "!deactivatedFlag"
		{
		ShowDialogItem (this, dropDownListID);
		HideDialogItem (this, staticTextID);

		}		// end "if (deactivatedFlag)"

}		// end "SetStaticTextOrDropDownList"


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowHideDialogItem
//
//	Software purpose:	The purpose of this routine is to show
//							or hide the specifed dialog item dependant upon
//							the input flag.
//
//	Parameters in:		Status Dialog item set
//
//	Parameters out:	None
//
// Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/1993
//	Revised By:			Larry L. Biehl			Date: 06/01/1995

void CMDialog::ShowHideDialogItem (
				DialogPtr			dialogPtr, 
				SInt16				itemNumber, 
				Boolean				showFlag)
				
{                      
	if (showFlag)	
		MShowDialogItem (dialogPtr, itemNumber);
		
	else		// !showFlag 		
		MHideDialogItem (dialogPtr, itemNumber);
	
}		// end "ShowHideDialogItem" 


void CMDialog::ShowSomeAreaSelectionItems()

{                                
	MShowDialogItem (this, IDEntireImage);
	MShowDialogItem (this, IDSelectedImage);
	
		// Determine if this is the entire area and set the 
		// to entire image icon.
							
	SetEntireImageButtons (NULL,
									m_LineStart, 
									m_LineEnd, 
									m_ColumnStart, 
									m_ColumnEnd); 
	
	MShowDialogItem (this, IDC_LineStart);          
	MShowDialogItem (this, IDC_LineEnd);          
	MShowDialogItem (this, IDC_ColumnStart);          
	MShowDialogItem (this, IDC_ColumnEnd);
	
}		// end "ShowSomeAreaSelectionItems"


void CMDialog::ToEntireImage(void)

{                                    
			//	Change to entire image
                  
	m_LineStart = 1;
	m_LineEnd = m_maxNumberLines;
	m_ColumnStart = 1;
	m_ColumnEnd = m_maxNumberColumns;
	
	m_settingSelectedEntireButton = TRUE;
	
	DDX_Text(m_dialogToPtr, IDC_LineStart, m_LineStart);   
	DDX_Text(m_dialogToPtr, IDC_LineEnd, m_LineEnd); 
	DDX_Text(m_dialogToPtr, IDC_ColumnStart, m_ColumnStart); 
	DDX_Text(m_dialogToPtr, IDC_ColumnEnd, m_ColumnEnd);
	
	m_settingSelectedEntireButton = FALSE; 

	GetDlgItem(IDC_LineStart)->SetFocus();
	
	SetEntireImageButtons (
							NULL,
							m_LineStart,
							m_LineEnd,
							m_ColumnStart,
							m_ColumnEnd); 
		                                   
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	
}		// end "ToEntireImage"


void CMDialog::ToSelectedImage()

{                                    
	LongRect			selectionRect;
	
				 
			//	Change to selected area
			
	if (GetSelectionRectangle (
//					gActiveImageViewCPtr, 
					m_dialogSelectArea.windowPtr, 
					&selectionRect, 
					kDontClearSelectionArea, 
					kUseThreshold, 
					kDontAdjustToBaseImage) )
		{           
		m_LineStart = selectionRect.top;
		m_LineEnd = selectionRect.bottom;
		m_ColumnStart = selectionRect.left;
		m_ColumnEnd = selectionRect.right;
	
		m_settingSelectedEntireButton = TRUE;
	
		DDX_Text (m_dialogToPtr, IDC_LineStart, m_LineStart);
		DDX_Text (m_dialogToPtr, IDC_LineEnd, m_LineEnd);
		DDX_Text (m_dialogToPtr, IDC_ColumnStart, m_ColumnStart);
		DDX_Text (m_dialogToPtr, IDC_ColumnEnd, m_ColumnEnd);
	
		m_settingSelectedEntireButton = FALSE; 

		GetDlgItem(IDC_LineStart)->SetFocus();
	
		SetEntireImageButtons (
							NULL,
							m_LineStart,
							m_LineEnd,
							m_ColumnStart,
							m_ColumnEnd); 
		                                      
		SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
		
		}		// end "if (GetLineColumnSelection(..."
	
}		// end "ToSelectedImage" 


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int UpdateOneColListSelections
//
//	Software purpose:	The purpose of this routine is to update the selections
//							in the one column list based on the start, end
//							and interval values.
//
//	Parameters in:		dialog list handle
//									
//
//	Parameters out:	None
//
// Value Returned:	the number of selections
//
// Called By:			ClassDialog   in projectUtilities.c
// 						ClassGroupDialog   in displayThematic.c
// 						ChannelsDialog   in multiSpecUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 05/16/1991
//	Revised By:			Larry L. Biehl			Date: 10/11/1995

UInt16 CMDialog::UpdateOneColListSelections (
				UInt16								numberInputVecItems,
				UInt16								listStart,
				UInt16								listEnd,
				UInt16								listInterval)

{
	Boolean					selectionFlag;
	
	UInt16					index,
								item,
								nextTrueSelection,
								numberSelections;
								
	
	index = 0;
	
	for (item=1; item<listStart; item++)
		{
		((CListBox*)GetDlgItem(IDC_List1))->SetSel(index, FALSE);
		index++;
		
		}		// end "for ( item=1; item<..." 
	
	nextTrueSelection = listStart;
	for (item=listStart; item<=listEnd; item++)
		{
		selectionFlag = (item == nextTrueSelection);
		
		if (selectionFlag)
			nextTrueSelection += listInterval;
			
		((CListBox*)GetDlgItem(IDC_List1))->SetSel(index, selectionFlag);
			
		index++;
		
		}		// end "for ( item=listStart; item..." 
	
	for (item=listEnd+1; item<=numberInputVecItems; item++)
		{
		((CListBox*)GetDlgItem(IDC_List1))->SetSel(index, FALSE);
		index++;
		
		}		// end "for ( item=listEnd+1; item<..." 
			
	numberSelections = (listEnd - listStart + listInterval)/listInterval; 
	
	return (numberSelections);
						
}		// end "UpdateOneColListSelections"               


void CMDialog::UpdateAllSubsetList(
				UInt16						channelsMenuItemNumber)
				
{                                                           
	if (m_channelListType == kSelectedItemsListOnly)
		{  	
		if ( ((CComboBox*)GetDlgItem(channelsMenuItemNumber))->GetCount() == 2 )
			{
			if (m_channelSelection == kAllMenuItem)
				((CComboBox*)GetDlgItem(channelsMenuItemNumber))->DeleteString(1);
				
			else		// m_channelSelection == kSubsetMenuItem
				((CComboBox*)GetDlgItem(channelsMenuItemNumber))->DeleteString(0);
				
			}		// end "if (...->GetCount() == 2 )"
		
		}		// end "if (m_channelListType == kSelectedChannelsListOnly)"
		
	else		// m_channelListType != kSelectedChannelsListOnly
		{ 
		if ( ((CComboBox*)GetDlgItem(channelsMenuItemNumber))->GetCount() == 1 )
			{                            
			((CComboBox*)GetDlgItem(channelsMenuItemNumber))->ResetContent();
				                                 
			((CComboBox*)GetDlgItem(channelsMenuItemNumber))->AddString((LPCTSTR)_T("All"));
			((CComboBox*)GetDlgItem(channelsMenuItemNumber))->AddString((LPCTSTR)_T("Subset..."));
				
			}		// end "if (...->GetCount() == 1 )"
			
		}		// end "else m_channelListType != kSelectedChannelsListOnly"
	
}		// end "UpdateAllSubsetList"  



void CMDialog::VerifyLineColumnStartEndValues(
				CDataExchange* 					pDX)
				
{  
	if (pDX->m_bSaveAndValidate)
		{
				// Verify that the line and column values make sense
					
		if (m_LineStart > m_LineEnd)
			{
			DDX_Text(pDX, IDC_LineStart, m_LineStart);              
			DDV_MinMaxLong(pDX, m_LineStart, 1, m_LineEnd);
				
			}		// end "m_LineStart > m_LineEnd"
					
		if (m_ColumnStart > m_ColumnEnd)
			{ 
			DDX_Text(pDX, IDC_ColumnStart, m_ColumnStart);
			DDV_MinMaxLong(pDX, m_ColumnStart, 1, m_ColumnEnd);
				
			}		// end "if (m_ColumnStart > m_ColumnEnd)"

		}		// end "if (pDX->m_bSaveAndValidate)"
	
}		// end "VerifyLineColumnStartEndValues" 


SInt16 CMDialog::VerifyLineColumnValues(
				CDataExchange* 					pDX, 
				SInt16								startLineItem,
				SInt16								startColumnItem,
				SInt16								pixelSize,
				SInt16								displayType,
				SInt16								numberChannels)
				
{  
	SInt16 returnCode = 0;
	
			// Verify that the line and column values make sense
	
	VerifyLineColumnStartEndValues (pDX);
	/*
	if (m_LineStart > m_LineEnd)
		{
		DDX_Text(pDX, IDC_LineStart, m_LineStart);              
		DDV_MinMaxLong(pDX, m_LineStart, 1, m_LineEnd);
			
		}		// end "m_LineStart > m_LineEnd"
				
	if (m_ColumnStart > m_ColumnEnd)
		{ 
		DDX_Text(pDX, IDC_ColumnStart, m_ColumnStart);
		DDV_MinMaxLong(pDX, m_ColumnStart, 1, m_ColumnEnd);
			
		}		// end "if (m_ColumnStart > m_ColumnEnd)"
	*/
			// Verify that the number of lines to be displayed is less than
			// the maximum possible. 
										
	returnCode = CheckNumberDisplayLines (
									m_LineStart, 
									m_LineEnd, 
									m_LineInterval,
									NULL, 
									startLineItem);
										
	if (returnCode == 0)
		{
		pDX->PrepareEditCtrl(IDC_LineEnd);
		pDX->Fail();
			
		}		// end "if (returnCode == 0)"	
			
			// Verify that the number of columns to be displayed is less than
			// the maximum possible.
			// This part needs to be updated. 
										
	returnCode = CheckNumberDisplayColumns (
									m_ColumnStart, 
									m_ColumnEnd, 
									m_ColumnInterval,
									pixelSize,
									displayType,
									numberChannels, 
									NULL, 
									startColumnItem);
										
	if (returnCode == 0)
		{
		pDX->PrepareEditCtrl(IDC_ColumnEnd);
		pDX->Fail();
			
		}		// end "if (returnCode == 0)"
		
	return (returnCode);  
	
}		// end "VerifyLineColumnValues"  


void CMDialog::OnFeatureTransformation()

{                                
	DDX_Check (m_dialogFromPtr,
					IDC_FeatureTransformation, 
					m_featureTransformationFlag);
	
	CheckFeatureTransformationDialog (this,
													m_featureTransformAllowedFlag, 
													IDC_FeatureTransformation, 
													IDC_ChannelPrompt, 
													(SInt16*)&m_featureTransformationFlag);  
	
}		// end "OnFeatureTransformation" 



void CMDialog::OnSelendokClassCombo()
{                                                                                
	HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,      
								(SInt16)gProjectInfoPtr->numberStatisticsClasses, 
								IDC_ClassCombo,
								&m_classSelection);
	
}		// end "OnSelendokClassCombo"  
