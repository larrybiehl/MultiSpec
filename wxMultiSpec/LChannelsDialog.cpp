//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LChannelsDialog.cpp : class implementation file
//	Class Definition:		LChannelsDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/12/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the CMChannelsDlg
//								class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#include "LChannelsDialog.h"
#include "CImageWindow.h" 



CMChannelsDlg::CMChannelsDlg (
				wxWindow*							pParent /*=NULL*/) : CMOneColDlg (pParent) 

{
    m_layerInfoPtr = NULL;
    m_fileInfoPtr = NULL;
    m_useTransformFlag = FALSE;
    m_availableFeaturePtr = NULL;

    m_initializedFlag = CMOneColDlg::m_initializedFlag;
	 
}	// end "CMChannelsDlg"



BEGIN_EVENT_TABLE (CMChannelsDlg, CMOneColDlg)
	EVT_INIT_DIALOG (CMChannelsDlg::OnInitDialog)
	//EVT_CHAR_HOOK (CMChannelsDlg::OnButtonPress)
END_EVENT_TABLE ()



//------------------------------------------------------------------------------------
//								 Copyright (2009-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:			
// 
//	Called By:			
//
//	Coded By:			Abdur Rahman Maud		Date: 10/10/2009
//	Revised By:			Larry L. Biehl			Date: 01/12/2018	

bool CMChannelsDlg::DoDialog (
				SInt16*								numberOutputFeaturesPtr,
				SInt16*								selectedFeaturePtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				SInt16								listType,
				Boolean								useTransformFlag,
				SInt16*								availableFeaturePtr,
				SInt16								numberInputVecItems,
				SInt16								currentSelection)
				
{
	bool									OKFlag = false;

	SInt16								returnCode;

			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (false);

			// This is a 0 base list.

	m_indexStart = 0;

	m_numberOutputItems = *numberOutputFeaturesPtr;
	m_selectedItemsPtr = selectedFeaturePtr;
	m_layerInfoPtr = layerInfoPtr;
	m_fileInfoPtr = fileInfoPtr;
	m_listType = listType;
	m_useTransformFlag = useTransformFlag;
	m_availableFeaturePtr = availableFeaturePtr;
	m_numberInputVecItems = numberInputVecItems;
	m_currentSelection = currentSelection;

	returnCode = ShowModal ();

	if (returnCode == wxID_OK) 
		{
				// Get items to use.

		if (m_listType == kSelectItemsList || m_listType == kSelectPCList) 
			{
			*numberOutputFeaturesPtr = m_numberSelections;

			SaveSubsetList ((SInt16)m_numberSelections,
									m_selectedItemsPtr,
									m_availableFeaturePtr,
									(SInt16)m_numberInputVecItems);

			}	// end "if (m_listType == kSelectChannelsList || ..."

		OKFlag = true;

		}	// end "if (returnCode == IDOK)"

	CheckAndUnlockHandle (gTransformationMatrix.eigenFeatureHandle);

	return (OKFlag);

}	// end "DoDialog"


/*
		//Press keyboard "Enter"  -> wxID_OK
		
void CMChannelsDlg::OnButtonPress (
				wxKeyEvent&							event) 

{

	if (event.GetKeyCode () == WXK_RETURN) 
		{
		if (Validate () && TransferDataFromWindow ())
			this->EndModal (wxID_OK);
		} 
		
	else
		event.Skip ();

}	// end "OnButtonPress"
*/


void CMChannelsDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	ChannelDescriptionPtr			channelDescriptionPtr;
	char*									spacer = ": ";
	DialogPtr							dialogPtr = this;
	FileInfoPtr							localFileInfoPtr;
	wxListBox*							listBoxPtr = NULL;

	SInt32								estimatedLengthListDescription;

	SInt16								fileInfoIndex;
	
	Boolean								showChannelListFlag = true;


	CMOneColDlg::OnInitDialog ();

			// Initialize local variables.

	channelDescriptionPtr = NULL;
	fileInfoIndex = -1;
	localFileInfoPtr = NULL;

	if (m_useTransformFlag) 
		{
		m_numberInputVecItems = gTransformationMatrix.numberFeatures;
		MHLock (gTransformationMatrix.eigenFeatureHandle);
		m_availableFeaturePtr = NULL;
		m_fileInfoPtr = NULL;

		m_listEnd = (UInt16)m_numberInputVecItems;

		} // end "if (m_useTransformFlag)"

	LoadSubsetList (&m_numberOutputItems,
							m_selectedItemsPtr,
							m_availableFeaturePtr,
							(SInt16) m_numberInputVecItems,
							&m_currentSelection);

			// Get estimate of the length of the description in the list.
			// Use 22 for the description list = 4 for channel number and 18
			// for the channel description.

	estimatedLengthListDescription = (SInt32) m_numberInputVecItems * 22;

	SetLabel (wxT("Select Channels"));
	LoadDItemString (IDC_ListTitle, (Str255*)"\0Channel List:");

			// If this is just a display list, hide all but OK button and list.
			// Put "Channel Descriptions" or "Selected Channels" as title of
			// the list.

	if (m_listType == kItemsListOnly || m_listType == kSelectedItemsListOnly) 
		{
		MHideDialogItem (dialogPtr, wxID_CANCEL);
		MHideDialogItem (dialogPtr, IDC_AllSelected);
		MHideDialogItem (dialogPtr, IDC_NoneSelected);

		MHideDialogItem (dialogPtr, IDC_ListTitle);
		MHideDialogItem (dialogPtr, IDC_AllSelected);
		MHideDialogItem (dialogPtr, IDC_FirstLabel);
		MHideDialogItem (dialogPtr, IDC_First);
		MHideDialogItem (dialogPtr, IDC_LastLabel);
		MHideDialogItem (dialogPtr, IDC_Last);
		MHideDialogItem (dialogPtr, IDC_IntervalLabel);
		MHideDialogItem (dialogPtr, IDC_Interval);
		MHideDialogItem (dialogPtr, IDC_EnterNewRange);
		MHideDialogItem (dialogPtr, IDC_RangeOutline);

		if (m_listType == kItemsListOnly) 
			SetLabel (wxT("Channel Descriptions"));
			
		if (m_listType == kSelectedItemsListOnly)
			SetLabel (wxT("Selected Channels"));

		}	// end "if (listType == kItemsListOnly || ...)"

	if (m_listType == kItemsListOnly) 
		{
		MHideDialogItem (dialogPtr, IDC_SelectionCountLabel);
		MHideDialogItem (dialogPtr, IDC_SelectionCount);

		}	// end "if (m_listType == kItemsListOnly)"

	if (m_useTransformFlag) 
		{
		SetLabel (wxT("Select Transformation Features"));
		LoadDItemString (IDC_ListTitle, (Str255*)"\0Feature List:");

		}	// end "if (m_useTransformFlag)"

			// Hide the channel list if the number of channels is more than 2000.
			// It makes handling the list very slow.
		
	if (m_numberInputVecItems > 2000) 
		{
		MHideDialogItem (dialogPtr, IDC_List1);
		showChannelListFlag = false;

		}	// end "if (m_numberInputVecItems > 2000)"

	if (TransferDataToWindow ()) 
		{
		PositionDialogWindow ();

		listBoxPtr = (wxListBox*)FindWindow (IDC_List1);

		}	// end "if (TransferDataToWindow ())"

	if (showChannelListFlag && listBoxPtr != NULL) 
		{
		SInt32 numberSelectedItems = AddChannelsToDialogList (
																	 listBoxPtr,
																	 &m_numberOutputItems,
																	 m_selectedItemsPtr,
																	 m_layerInfoPtr,
																	 m_fileInfoPtr,
																	 m_listType,
																	 m_availableFeaturePtr,
																	 (SInt32) m_numberInputVecItems,
																	 m_currentSelection);
			 
		listBoxPtr->SetFirstItem (0);

		//listBoxPtr->Fit ();

				// Set the number of selected items.

		::LoadDItemValue (dialogPtr, IDC_SelectionCount, numberSelectedItems);

		}	// end "if (listBoxPtr != NULL)"

			// Set default text selection to first edit text item

	SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);
	//this->Fit ();

}	// end "OnInitDialog"



CMChannelCombinationsDlg::CMChannelCombinationsDlg (
				wxWindow*							pParent /*=NULL*/) : CMOneColDlg (pParent) 

{
	m_useTransformFlag = false;
	m_availableFeaturePtr = NULL;

	m_initializedFlag = CMOneColDlg::m_initializedFlag;

}	// end "CMChannelCombinationsDlg"



BEGIN_EVENT_TABLE (CMChannelCombinationsDlg, CMOneColDlg)
   EVT_INIT_DIALOG (CMChannelCombinationsDlg::OnInitDialog)
END_EVENT_TABLE ()



//------------------------------------------------------------------------------------
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the channel
//							combinations dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 01/12/2018	

bool CMChannelCombinationsDlg::DoDialog (
				SInt16								channelCombinationSelection,
				UInt16*								numberOutputChannelCombinationsPtr,
				UInt16*								channelCombinationsPtr,
				UInt32								numberInputChannelCombinations,
				UInt16*								allChanCombinationsPtr,
				UInt32								contiguousChannelsPerGroup,
				Boolean								useTransformFlag)
				
{
	Boolean OKFlag = false;

	SInt16 returnCode;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (false);

			// This is a 1 base list.

	m_indexStart = 1;
	m_listType = kSelectItemsList;

	m_channelCombinationSelection = channelCombinationSelection;
	m_numberOutputItems = *numberOutputChannelCombinationsPtr;
	m_selectedItemsPtr = (SInt16*)channelCombinationsPtr;
	m_numberInputVecItems = numberInputChannelCombinations;
	m_availableFeaturePtr = (SInt16*)allChanCombinationsPtr;
	m_contiguousChannelsPerGroup = (SInt16) contiguousChannelsPerGroup;
	m_useTransformFlag = useTransformFlag;

	returnCode = ShowModal ();

	if (returnCode == wxID_OK) 
		{
				// Get items to use.

		ChannelCombinationsDialogOK (m_numberSelections,
												 numberOutputChannelCombinationsPtr,
												 channelCombinationsPtr,
												 allChanCombinationsPtr);

		OKFlag = true;

		}	// end "if (returnCode == IDOK)"

	return (OKFlag);

}	// end "DoDialog"



void CMChannelCombinationsDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	wxListBox*							listBoxPtr = NULL;


	CMOneColDlg::OnInitDialog ();

			// Initialize local variables.

	ChannelCombinationsDialogInitialize (this,
														m_contiguousChannelsPerGroup,
														m_useTransformFlag,
														m_numberInputVecItems);

	if (TransferDataToWindow ()) 
		{
		PositionDialogWindow ();

		listBoxPtr = (wxListBox*)FindWindow (IDC_List1);

		}	// end "if (UpdateData (FALSE))"

	ChannelCombinationsDialogLoadList (this,
													listBoxPtr,
													m_numberOutputItems,
													(UInt16*)m_selectedItemsPtr,
													m_numberInputVecItems,
													(UInt16*)m_availableFeaturePtr,
													m_channelCombinationSelection);

			// Set default text selection to first edit text item

	SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);

}	// end "OnInitDialog"
