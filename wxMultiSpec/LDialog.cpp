

//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDialog.cpp : class implementation file
//	Class Definition:		LDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMDialog class.
/* Template for debugging
		int numberChars = sprintf (
			(char*)&gTextString3,
			" LDialog: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "LChannelsDialog.h"
#include "LDialog.h"
#include "LClassesDialog.h"
#include "LImage_dialog.cpp"
#include "wx/wx.h"
#include "wx/textctrl.h"
#include "wx/msgdlg.h"
#include "wx/radiobut.h"
#include "wx/checkbox.h"


CMDialog::CMDialog ()

{

};



CMDialog::CMDialog (
				wxWindowID							IDD,
				wxWindow*							pParent,
				const wxString&					title)

		: wxDialog (pParent,
						IDD,
						title,
						wxDefaultPosition,
						wxDefaultSize,
						wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP)

{
	/*
	m_dialogPanel = new wxPanel (this,
											wxID_ANY,
											wxDefaultPosition,
											wxDefaultSize,
											wxTAB_TRAVERSAL);
	*/
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

   m_classSelection_Saved = 0;
   m_channelSelection_Saved = 0;
   m_weightSelection_Saved = 0;
   m_areaSelection_Saved = 0;
   m_interClassWeightsSelection_Saved = 0;
    
   m_valuePtr = NULL;

   m_availableFeaturePtr = NULL;
   m_localActiveFeaturesPtr = NULL;
   m_localFeaturesPtr = NULL;
   m_localTransformFeaturesPtr = NULL;

   m_classListPtr = NULL;

   m_localSymbolsPtr = NULL;
	
   m_dialogPanel = NULL;

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

   m_initializedFlag = FALSE;

			// Set limit values

   if (m_dialogSelectArea.imageWindowInfoPtr != NULL) 
		{
      m_maxNumberLines = m_dialogSelectArea.imageWindowInfoPtr->maxNumberLines;
      m_maxNumberColumns = m_dialogSelectArea.imageWindowInfoPtr->maxNumberColumns;

		}	// end "m_dialogSelectArea.imageWindowInfoPtr != NULL"

   else	// m_dialogSelectArea.imageWindowInfoPtr == NULL
		{
      m_maxNumberLines = 1;
      m_maxNumberColumns = 1;

		}	// end "else m_dialogSelectArea.imageWindowInfoPtr == NULL"

   m_initializedFlag = TRUE;

}	// end "CMDialog"



CMDialog::~CMDialog ()

{
   m_value.Clear ();
	
   if (m_dialogPanel != NULL)
   	delete m_dialogPanel;
	
	m_dialogPanel = NULL;
	
}	// end "~CMDialog"



BEGIN_EVENT_TABLE (CMDialog, wxDialog)
	EVT_CHAR_HOOK (CMDialog::OnCharHook)
END_EVENT_TABLE ()



void CMDialog::CheckColumnEnd (
				wxCommandEvent&					event)

{
   if (!m_settingSelectedEntireButton) 
		{
      SInt32 returnValue;


      returnValue = CheckValue (IDC_ColumnEnd, 1, m_maxNumberColumns);

      if (returnValue > 0) 
			{
         m_ColumnEnd = returnValue;

         SetEntireImageButtons (NULL,
											m_LineStart,
											m_LineEnd,
											m_ColumnStart,
											m_ColumnEnd);

			}	// end "if (returnValue > 0)"

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckColumnEnd"



void CMDialog::CheckColumnInterval (
				wxCommandEvent&					event)

{
   SInt32 returnValue;


   returnValue = CheckValue (IDC_ColumnInterval, 1, m_maxNumberColumns);

   if (returnValue > 0)
      m_ColumnInterval = returnValue;

}	// end "CheckColumnInterval"



void CMDialog::CheckColumnStart (
				wxCommandEvent&					event)

{
   if (!m_settingSelectedEntireButton)
		{
      SInt32		returnValue;


      returnValue = CheckValue (IDC_ColumnStart, 1, m_maxNumberColumns);

      if (returnValue > 0)
			{
         m_ColumnStart = returnValue;

         SetEntireImageButtons (NULL,
											m_LineStart,
											m_LineEnd,
											m_ColumnStart,
											m_ColumnEnd);

			}	// end "if (returnValue > 0)"

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckColumnStart"



void CMDialog::CheckLineEnd (
				wxCommandEvent&					event)

{
   if (!m_settingSelectedEntireButton)
		{
      SInt32			returnValue;


      returnValue = CheckValue (IDC_LineEnd, 1, m_maxNumberLines);

      if (returnValue > 0)
			{
         m_LineEnd = returnValue;

         SetEntireImageButtons (NULL,
											m_LineStart,
											m_LineEnd,
											m_ColumnStart,
											m_ColumnEnd);

			}	// end "if (returnValue > 0)"

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckLineEnd"



void CMDialog::CheckLineInterval (
				wxCommandEvent&					event)

{
	SInt32 returnValue;


   returnValue = CheckValue (IDC_LineInterval, 1, m_maxNumberLines);

   if (returnValue > 0)
      m_LineInterval = returnValue;

}	// end "CheckLineInterval"



void CMDialog::CheckLineStart (
				wxCommandEvent&					event)

{
   if (!m_settingSelectedEntireButton)
		{
      SInt32 returnValue;


      returnValue = CheckValue (IDC_LineStart, 1, m_maxNumberLines);

      if (returnValue > 0)
			{
         m_LineStart = returnValue;

         SetEntireImageButtons (NULL,
											m_LineStart,
											m_LineEnd,
											m_ColumnStart,
											m_ColumnEnd);

			}	// end "if (returnValue > 0)"

		}	// end "if (!m_settingSelectedEntireButton)"

}	// end "CheckLineStart"



SInt32 CMDialog::CheckValue (
				int									controlID,
				SInt32								minValue,
				SInt32								maxValue)

{
	wxTextCtrl*							textControlPtr;
   SInt32								outputValue = 0;


   textControlPtr = wxDynamicCast (FindWindow (controlID), wxTextCtrl);

   if (textControlPtr != NULL)
		{
      m_value = textControlPtr->GetValue ();

      if (m_value.Length () > 0)
			{
         outputValue = wxAtoi (m_value);

         if (outputValue < minValue || outputValue > maxValue)
				{
            if (outputValue < minValue)
               outputValue = minValue;
					
            else
               outputValue = maxValue;
 
				textControlPtr->ChangeValue (wxString::Format (wxT("%i"), outputValue));

				}	// end "if (outputValue < minValue || outputValue > maxValue)"
	
			}	// end "if (m_value.Length () > 0)"

		}	// end "if (textControlPtr != NULL)"

   return (outputValue);

}	// end "CheckValue"



void CMDialog::CreateLineColumnControls (
				wxStaticBoxSizer*					sbSizer)

{
	wxStaticText						*staticLineText,
											*staticColumnText,
											*staticEmptyText,
											*staticStartText,
											*staticEndText,
											*staticIntervalText;
	/*
	wxTextCtrl							*colendctrl,
											*colintctrl,
											*colstartctrl,
											*lineendctrl,
											*lineintctrl,
											*linestartctrl;
	*/
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);

   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);

   m_bpButton4 = new wxBitmapButton (sbSizer->GetStaticBox (),
													IDEntireImage,
													entireimi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   m_bpButton4->SetBitmapDisabled (toentirei);
   bSizer791->Add (m_bpButton4,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_bpButton5 = new wxBitmapButton (sbSizer->GetStaticBox (),
   												IDSelectedImage,
   												selectedi,
   												wxDefaultPosition,
   												wxDefaultSize,
   												wxBU_AUTODRAW);
   m_bpButton5->Hide ();
   m_bpButton5->SetBitmapDisabled (bmp4i);
	bSizer791->Add (m_bpButton5,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   sbSizer->Add (bSizer791, wxSizerFlags(0).Expand());

   wxGridSizer* gSizer1;
   gSizer1 = new wxGridSizer (3, 4, 0, 0);

   staticEmptyText = new wxStaticText (sbSizer->GetStaticBox (),
												  wxID_ANY,
												  wxEmptyString,
												  wxDefaultPosition,
												  wxDefaultSize,
												  0);
   staticEmptyText->Wrap (-1);
   gSizer1->Add (staticEmptyText, wxSizerFlags(0).Border(wxALL, 5));

   staticStartText = new wxStaticText (sbSizer->GetStaticBox (),
												  wxID_ANY,
												  wxT("Start"),
												  wxDefaultPosition,
												  wxDefaultSize,
												  0);
   staticStartText->Wrap (-1);
   gSizer1->Add (staticStartText,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   staticEndText = new wxStaticText (sbSizer->GetStaticBox (),
												  wxID_ANY,
												  wxT("End"),
												  wxDefaultPosition,
												  wxDefaultSize,
												  0);
   staticEndText->Wrap (-1);
   gSizer1->Add (staticEndText,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   staticIntervalText = new wxStaticText (sbSizer->GetStaticBox (),
												  wxID_ANY,
												  wxT("Interval"),
												  wxDefaultPosition,
												  wxDefaultSize,
												  0);
   staticIntervalText->Wrap (-1);
   gSizer1->Add (staticIntervalText,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   staticLineText = new wxStaticText (sbSizer->GetStaticBox (),
												  wxID_ANY,
												  wxT("Line"),
												  wxDefaultPosition,
												  wxDefaultSize,
												  0);
   staticLineText->Wrap (-1);
   gSizer1->Add (staticLineText,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   m_linestartctrl = new wxTextCtrl (sbSizer->GetStaticBox (),
												 IDC_LineStart,
												 wxEmptyString,
												 wxDefaultPosition,
												 wxSize(75, -1),
												 0);
   m_linestartctrl->SetValidator (
									wxTextValidator (wxFILTER_DIGITS, &m_LineStartString));

   gSizer1->Add (m_linestartctrl,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   m_lineendctrl = new wxTextCtrl (sbSizer->GetStaticBox (),
											  IDC_LineEnd,
											  wxEmptyString,
											  wxDefaultPosition,
											  wxSize(75, -1),
											  0);
   m_lineendctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_LineEndString));
   gSizer1->Add (m_lineendctrl,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   m_lineintctrl = new wxTextCtrl (sbSizer->GetStaticBox (),
											  IDC_LineInterval,
											  wxEmptyString,
											  wxDefaultPosition,
											  wxSize(75, -1),
											  0);
   m_lineintctrl->SetValidator (
									wxTextValidator (wxFILTER_DIGITS, &m_LineIntervalString));
   gSizer1->Add (m_lineintctrl,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   staticColumnText = new wxStaticText (sbSizer->GetStaticBox (),
												  wxID_ANY,
												  wxT("Column"),
												  wxDefaultPosition,
												  wxSize(75, -1),
												  0);
   staticColumnText->Wrap (-1);
   gSizer1->Add (staticColumnText, wxSizerFlags(0).Align(wxALIGN_CENTER));

   m_colstartctrl = new wxTextCtrl (sbSizer->GetStaticBox (),
												IDC_ColumnStart,
												wxEmptyString,
												wxDefaultPosition,
												wxSize(75, -1),
												0);
   m_colstartctrl->SetValidator (
									wxTextValidator (wxFILTER_DIGITS, &m_ColumnStartString));
   gSizer1->Add (m_colstartctrl,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   m_colendctrl = new wxTextCtrl (sbSizer->GetStaticBox (),
											 IDC_ColumnEnd,
											 wxEmptyString,
											 wxDefaultPosition,
											 wxSize(75, -1),
											 0);
   m_colendctrl->SetValidator (
										wxTextValidator (wxFILTER_DIGITS, &m_ColumnEndString));
   gSizer1->Add (m_colendctrl,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   m_colintctrl = new wxTextCtrl (sbSizer->GetStaticBox (),
											 IDC_ColumnInterval,
											 wxEmptyString,
											 wxDefaultPosition,
											 wxSize(75, -1),
											 0);
   m_colintctrl->SetValidator (
								wxTextValidator (wxFILTER_DIGITS, &m_ColumnIntervalString));
   gSizer1->Add (m_colintctrl,
   					wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

   SetUpToolTip (m_linestartctrl, IDS_ToolTip19);
   SetUpToolTip (m_lineendctrl, IDS_ToolTip20);
   SetUpToolTip (m_lineintctrl, IDS_ToolTip21);
   SetUpToolTip (m_colstartctrl, IDS_ToolTip22);
   SetUpToolTip (m_colendctrl, IDS_ToolTip23);
   SetUpToolTip (m_colintctrl, IDS_ToolTip24);

	sbSizer->Add (gSizer1, wxSizerFlags(1).Expand());

}	// end "CreateLineColumnControls"



 void CMDialog::CreateStandardButtons (
 				wxBoxSizer* 								bVMainSizer)

{
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bVMainSizer->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bVMainSizer->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif
	
}	// end "CreateStandardButtons"



SInt16 CMDialog::GetComboListSelection (
				UInt16								dialogItemNumber,
				SInt32								selectedListItemData)

{
   return (::GetComboListSelection (this,
												dialogItemNumber,
												selectedListItemData));

}	// end "GetComboListSelection"



SInt16 CMDialog::GetComboListSelection2 (
				wxWindow*							cWindowPtr,
				UInt16								dialogItemNumber,
				SInt32								selectedListItemData)

{
	wxComboBox*							comboBoxPtr;
   wxWindow*							wptr;
   wxChoice*							choiceptr;
	
   UInt32								index,
											numberComboItems;
	
   SInt16								comboListSelection;
	
	
   comboListSelection = -1;
   wptr = cWindowPtr->FindWindow (dialogItemNumber);
   if (wxDynamicCast (wptr, wxComboBox) != NULL)
		{
				// If it's wxComboBox
		
      comboBoxPtr = (wxComboBox*)wptr;
      if (comboBoxPtr != NULL)
			{
         numberComboItems = (UInt32)comboBoxPtr->GetCount ();

         for (index=0; index<numberComboItems; index++)
				{
            if (comboBoxPtr->GetClientData (index))
					{
               SInt64 selectindex64 = (SInt64)((int*)comboBoxPtr->GetClientData (index));
               SInt32 selectindex = (SInt32)selectindex64;
               if (selectindex == selectedListItemData)
                  comboListSelection = (SInt16)index;
					
					}	// end "if (comboBoxPtr->GetClientData (index))"
				
				}	// end "for (index=0; index<numberComboItems; index++)"
			
			}	// end "if (comboBoxPtr != NULL)"
			
		}	// end "if (wxDynamicCast (wptr, wxComboBox) != NULL)"
	
	else if (wxDynamicCast (wptr, wxChoice) != NULL)
		{
				// If it's wxChoice
		
      choiceptr = (wxChoice*)wptr;
      if (choiceptr != NULL)
			{
         numberComboItems = (UInt32)choiceptr->GetCount ();
         for (index = 0; index < numberComboItems; index++)
				{
            if (choiceptr->GetClientData (index))
					{
               SInt64 selectindex64 = (SInt64)((int*)choiceptr->GetClientData (index));
               SInt32 selectindex = (SInt32)selectindex64;
               if (selectindex == selectedListItemData)
                  comboListSelection = (SInt16)index;
					
					}	// end "if (choiceptr->GetClientData (index))"
				
				}	// end "for (index=0; index<numberComboItems; index++)"

			}	// end "if (comboBoxPtr != NULL)"
			
		}	// end "else if (wxDynamicCast (wptr, wxChoice) != NULL)"
	
   return (comboListSelection);
	
}	// end "GetComboListSelection2"



void CMDialog::HandleChannelsMenu (
				UInt16								channelsMenuItemNumber,
				Boolean								transformationFlag,
				SInt16								totalNumberChannels,
				SInt16								channelMenuType,
				Boolean								okFlagSetting)

{
	int									channelSelection;

   channelSelection = (wxDynamicCast (FindWindow (channelsMenuItemNumber),
													wxComboBox))->GetCurrentSelection ();
	
   if (channelSelection == kSubsetMenuItem ||
				(channelSelection == kAllMenuItem &&
							m_channelListType == kSelectedItemsListOnly))
		
		{
				// Subset of channels to be used.

      SetDLogControlHilite (NULL, wxID_OK, 255);

      CMChannelsDlg* channelsDialogPtr = NULL;

      channelsDialogPtr = new CMChannelsDlg (this);

      channelsDialogPtr->DoDialog ((SInt16*)&m_localActiveNumberFeatures,
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

      if (channelMenuType == kImageChannelType)
			{
         if (m_localActiveNumberFeatures == (UInt16)totalNumberChannels)
            channelSelection = kAllMenuItem;

			}	// end "if (channelMenuType == kImageChannelType)"

      if (okFlagSetting)
         SetDLogControlHilite (NULL, wxID_OK, 0);

		}	// end "if (m_channelSelection == kSubsetMenuItem)"

   else	// channelSelection == kAllMenuItem
		{
      if (channelMenuType == kImageChannelType)
         m_localActiveNumberFeatures = totalNumberChannels;

		}	// end "else channelSelection == kAllMenuItem"

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

		}	// end "if (channelMenuType == kProjectChannelType)"

   m_channelSelection = channelSelection;
   (wxDynamicCast (FindWindow (channelsMenuItemNumber), wxComboBox))->
																	SetSelection (m_channelSelection);

}	// end "HandleChannelsMenu"



void CMDialog::HandleClassesMenu (
				UInt32*								numberOutputClassesPtr,
				SInt16*								classListPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses,
				UInt16								classesMenuItemNumber,
				int*									classMenuSelectionPtr)

{
	int									classMenuSelection;
	Boolean								returnFlag;
	
	
	wxComboBox* classcombo = (wxComboBox*)FindWindow (classesMenuItemNumber);
	classMenuSelection = classcombo->GetSelection ();
	if (classMenuSelection == kSubsetMenuItem) 
		{
				// Subset of classes to be used.
		
      SetDLogControlHilite (NULL, wxID_OK, 255);
		
				// If all classes is the current setting, then assume that
				// the subset is the single active project class.
		
      if (*classMenuSelectionPtr == kAllMenuItem && gProjectInfoPtr->currentClass >= 0) 
			{
         *numberOutputClassesPtr = 1;
         classListPtr[0] = gProjectInfoPtr->currentClass + 1;

			}	// end "if (currentSelection = kAllMenuItem && ..."

      CMClassesDlg* classesDialogPtr = NULL;
      try 
			{
         classesDialogPtr = new CMClassesDlg ();
         returnFlag = classesDialogPtr->DoDialog (
									numberOutputClassesPtr,
									classListPtr,
									minimumNumberClasses,
									(UInt16) numberInputClasses);

         if (classesDialogPtr != NULL)
            delete classesDialogPtr;
			
			}	// end "try"
		
		catch (int e) 
			{
			MemoryMessage (0, kObjectMessage);
			
			}		// catch

      if (!returnFlag && *classMenuSelectionPtr == kAllMenuItem)
         *numberOutputClassesPtr = gProjectInfoPtr->numberStatisticsClasses;

      if (*numberOutputClassesPtr == gProjectInfoPtr->numberStatisticsClasses) 
			{
         classMenuSelection = kAllMenuItem;
         (wxDynamicCast (FindWindow (classesMenuItemNumber), wxComboBox))->
																	SetSelection (classMenuSelection);

			}	// end "if (*numberOutputClassesPtr == ..."

      SetDLogControlHilite (NULL, wxID_OK, 0);

		}	// end "if (classMenuSelection == kSubsetMenuItem)"

   else	// classMenuSelection == kAllMenuItem
		{
      *numberOutputClassesPtr = gProjectInfoPtr->numberStatisticsClasses;

		}	// end "else classMenuSelection == kAllMenuItem"

   *classMenuSelectionPtr = classMenuSelection;

}	// end "HandleClassesMenu"



void CMDialog::HandleClassPairWeightsMenu (
				SInt16**								weightsListPtrPtr,
				UInt16								classPairWeightsMenuItemNumber,
				int*									classPairWeightsMenuSelectionPtr,
				SInt16*								defaultClassPairWeightPtr)

{

   int									classPairWeightsMenuSelection;


   classPairWeightsMenuSelection = (wxDynamicCast (
														FindWindow (classPairWeightsMenuItemNumber),
														wxComboBox))->GetCurrentSelection ();

   if (classPairWeightsMenuSelection == kUnequalWeightMenuItem)
		{
				// Subset of classes to be used.

      SetDLogControlHilite (NULL, wxID_OK, 255);

      classPairWeightsMenuSelection = ClassPairWeightsDialog (
															gProjectInfoPtr->numberStatisticsClasses,
															weightsListPtrPtr,
															*classPairWeightsMenuSelectionPtr,
															defaultClassPairWeightPtr);

      SetDLogControlHilite (NULL, wxID_OK, 0);

      if (classPairWeightsMenuSelection == kEqualWeightMenuItem)
			{
         (wxDynamicCast (FindWindow (classPairWeightsMenuItemNumber), wxComboBox))->
														SetSelection (classPairWeightsMenuSelection);

			}	// end "if (classPairWeightsMenuSelection == kEqualWeightMenuItem)"
		
		}	// end "if (classPairWeightsMenuSelection == kUnequalWeightMenuItem)"
	
   *classPairWeightsMenuSelectionPtr = classPairWeightsMenuSelection;

}	// end "HandleClassPairWeightsMenu"



void CMDialog::HandleClassWeightsMenu (
				UInt16								numberOutputClassesToUse,
				SInt16*								classListPtr,
				float*								weightsListPtr,
				Boolean								useEnhancedStatFlag,
				UInt16								classWeightsMenuItemNumber,
				int*									classWeightsMenuSelectionPtr)

{
	int									classWeightsMenuSelection;

	
   classWeightsMenuSelection = (wxDynamicCast (FindWindow (
							classWeightsMenuItemNumber), wxComboBox))->GetCurrentSelection ();
	
	if (classWeightsMenuSelection == kUnequalWeightMenuItem)
		{
				// Subset of classes to be used.

      SetDLogControlHilite (NULL, wxID_OK, 255);

      classWeightsMenuSelection = ClassWeightsDialog (numberOutputClassesToUse,
																		classListPtr,
																		weightsListPtr,
																		*classWeightsMenuSelectionPtr,
																		useEnhancedStatFlag);

      SetDLogControlHilite (NULL, wxID_OK, 0);

      if (classWeightsMenuSelection == kEqualWeightMenuItem)
			{
         (wxDynamicCast (FindWindow (classWeightsMenuItemNumber), wxComboBox))->
															SetSelection (classWeightsMenuSelection);
			
			}	// end "if (classWeightsMenuSelection == kEqualWeightMenuItem)"

		}	// end "if (classWeightsMenuSelection == kUnequalWeightMenuItem)"

   *classWeightsMenuSelectionPtr = classWeightsMenuSelection;

}	// end "HandleClassWeightsMenu"



void CMDialog::HideShowAreaItems (
				Boolean								imageAreaFlag)

{
   if (imageAreaFlag)
		{
      MShowDialogItem (this, IDC_LineColFrame);

      MShowDialogItem (this, IDC_StartEndInterval);
      MShowDialogItem (this, IDC_LinePrompt);
      MShowDialogItem (this, IDC_ColumnPrompt);
      MShowDialogItem (this, IDC_LineStart);
      MShowDialogItem (this, IDC_LineEnd);
      MShowDialogItem (this, IDC_LineInterval);
      MShowDialogItem (this, IDC_ColumnStart);
      MShowDialogItem (this, IDC_ColumnEnd);
      MShowDialogItem (this, IDC_ColumnInterval);
      MShowDialogItem (this, IDC_StartPrompt);
      MShowDialogItem (this, IDC_EndPrompt);
      MShowDialogItem (this, IDC_IntervalPrompt);

		}	// end "if (imageAreaFlag)"

   else	// !imageAreaFlag
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
      MHideDialogItem (this, IDC_StartPrompt);
      MHideDialogItem (this, IDC_EndPrompt);
      MHideDialogItem (this, IDC_IntervalPrompt);
            
		}	// end "else !imageAreaFlag"

}	// end "HideShowAreaItems"



void CMDialog::HideSomeAreaSelectionItems ()

{
	 MHideDialogItem (this, IDEntireImage);
	 MHideDialogItem (this, IDSelectedImage);
	 MHideDialogItem (this, IDC_LineStart);
	 MHideDialogItem (this, IDC_LineEnd);
	 MHideDialogItem (this, IDC_ColumnStart);
	 MHideDialogItem (this, IDC_ColumnEnd);
	 MHideDialogItem (this, IDC_StartPrompt);
	 MHideDialogItem (this, IDC_EndPrompt);
	 MHideDialogItem (this, IDC_StartEnd);
	
}	// end "HideSomeAreaSelectionItems"



void CMDialog::LoadDItemString (
				UInt16								itemNumber,
				Str255*								theStringPtr)

{
   CharPtr stringPtr = (CharPtr)theStringPtr;
   stringPtr = (CharPtr)&stringPtr[1];

   LoadDItemString (itemNumber, stringPtr);

}	// end "LoadDItemString"



void CMDialog::LoadDItemString (
				UInt16								itemNumber,
				CharPtr								theStringPtr)

{
   const wxString string = wxString::FromAscii (theStringPtr);

   wxWindow* lcontrol = this->FindWindow ((long)itemNumber);
   wxTextCtrl* tctrl = wxDynamicCast (lcontrol, wxTextCtrl);

   if (tctrl != NULL)
      tctrl->ChangeValue (string);
	
   else	// tctrl == NULL
		{
      wxStaticText* tctrl = wxDynamicCast (lcontrol, wxStaticText);
      if (tctrl != NULL)
         tctrl->SetLabel (string);
		
		}	// end "else tctrl == NULL"

}	// end "LoadDItemString"



void CMDialog::LoadDItemValue (
				UInt16								itemNumber,
				SInt32								value)

{
   wxString strval = wxString::Format (wxT("%i"), value);
   wxTextCtrl* ctrl = wxDynamicCast (FindWindow (itemNumber), wxTextCtrl);
	
   if (ctrl != NULL)
      ctrl->ChangeValue (strval);
	
	else	// ctrl == NULL
		{
      wxStaticText* tctrl = wxDynamicCast (FindWindow (itemNumber), wxStaticText);
      if (tctrl != NULL)
         tctrl->SetLabel (strval);
		
      else	// tctrl == NULL
			{
         wxComboBox* cctrl = wxDynamicCast (FindWindow (itemNumber), wxComboBox);
         if (cctrl != NULL)
            cctrl->SetValue (strval);
			
			}	// end "else tctrl == NULL"
		
		}	// end "else ctrl == NULL"

}	// end "LoadDItemString"




//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
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
//	Coded By:			Abdur Rahman Maud		Date: 06/01/2011
//	Revised By:			Larry L. Biehl			Date: 05/04/2018

void CMDialog::MHideDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
   wxWindow* dialogItemWindowPtr = FindWindow (itemNumber);

   if (dialogItemWindowPtr != NULL)
      dialogItemWindowPtr->Show (false);
	
}	// end "MHideDialogItem"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
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
//	Coded By:			Abdur Rahman Maud		Date: 06/01/2011
//	Revised By:			Larry L. Biehl			Date: 05/04/2018

void CMDialog::MEnableDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
   wxWindow* dialogItemWindowPtr = FindWindow (itemNumber);

   if (dialogItemWindowPtr != NULL)
      dialogItemWindowPtr->Enable (true);

}	// end "MEnableDialogItem"



void CMDialog::MDisableDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
   wxWindow* dialogItemWindowPtr = FindWindow (itemNumber);

   if (dialogItemWindowPtr != NULL)
      dialogItemWindowPtr->Enable (false);
	
}	// end "MDisableDialogItem"



void CMDialog::MShowDialogItem (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
   wxWindow* dialogItemWindowPtr = FindWindow (itemNumber);

   if (dialogItemWindowPtr != NULL)
      dialogItemWindowPtr->Show (true);

}	// end "MShowDialogItem"



void CMDialog::OnCharHook (
				wxKeyEvent&							event)
{
	if (event.GetKeyCode () == WXK_RETURN || event.GetKeyCode () == WXK_NUMPAD_ENTER)
		{
		if (Validate () && TransferDataFromWindow ())
			this->EndModal (wxID_OK);
		
		}	// end "if (event.GetKeyCode () == WXK_RETURN || ..."
	
	else
		event.Skip();
	
}	// end "OnCharHook"	



void CMDialog::OnFeatureTransformation ()

{
   wxCheckBox* checkptr =
					(wxDynamicCast (FindWindow (IDC_FeatureTransformation), wxCheckBox));
   wxRadioButton* radioptr =
					(wxDynamicCast (FindWindow (IDC_FeatureTransformation), wxRadioButton));

   if (checkptr != NULL)
      m_featureTransformationFlag = checkptr->GetValue ();
	
	else if (radioptr != NULL)
      m_featureTransformationFlag = radioptr->GetValue ();

   CheckFeatureTransformationDialog (this,
													m_featureTransformAllowedFlag,
													IDC_FeatureTransformation,
													IDC_ChannelPrompt,
													(SInt16*)&m_featureTransformationFlag);

}	// end "OnFeatureTransformation"


/*
void wxWidgetsDialog::OnKey(wxKeyEvent &event)
{
  wxWindow *win = FindFocus();
  if(win == NULL){
     event.Skip ();
     return;
  }

  if(event.GetKeyCode()==WXK_RETURN){
      switch(win->GetId()){
     case ID_BUTTON1:
           wxMessageBox("Button1");
           break;
      case ID_BUTTON2:
           wxMessageBox("Button2");
           break;
      case ID_BUTTON3:
           wxMessageBox("Button3");
          Destroy();
           break;
   }
   event.Skip ();
  }

  if(event.GetKeyCode() == WXK_TAB){
    bool backward = event.ShiftDown();
   switch (win->GetId()){
     case ID_BUTTON1:
          if(backward)
            FindWindow(ID_BUTTON3)->SetFocus();
          else
            FindWindow(ID_BUTTON2)->SetFocus();
          return;

      case ID_BUTTON2:
          if(backward)
            FindWindow(ID_BUTTON1)->SetFocus();
          else
            FindWindow(ID_BUTTON3)->SetFocus();
          return;

      case ID_BUTTON3:
          if(backward)
            FindWindow(ID_BUTTON2)->SetFocus();
          else
            FindWindow(ID_BUTTON1)->SetFocus();
          return;
   }
  }
  event.Skip ();
}
*/


void CMDialog::OnSelendokClassCombo (
				wxCommandEvent&					event)

{
   HandleClassesMenu (&m_localNumberClasses,
								(SInt16*)m_classListPtr,
								1,
								(SInt16)gProjectInfoPtr->numberStatisticsClasses,
								IDC_ClassCombo,
								&m_classSelection);

}	// end "OnSelendokClassCombo"



void CMDialog::OnSelendokClassComboDropDown (
				wxCommandEvent&					event)

{
   ResetComboBox (IDC_ClassCombo);
	
}	// end "OnSelendokClassComboDropDown"



void CMDialog::OnSelendokAreasComboDropDown (
				wxCommandEvent&					event)

{
   ResetComboBox (IDC_AreasCombo);
	
}	// end "OnSelendokAreasCombo"



void CMDialog::OnSelendokClassWeightsComboDropDown (
				wxCommandEvent&					event)

{
   wxComboBox* combo = wxDynamicCast (FindWindow (IDC_WeightCombo), wxComboBox);
   if (combo != NULL)
      ResetComboBox (IDC_WeightCombo);
	
   else	// combo == NULL
      ResetComboBox (IDC_WeightsCombo);
	
}		// end "OnSelendokClassWeightsComboDropDown"



void CMDialog::OnSelendokChannelComboDropDown (
				wxCommandEvent&					event)

{
   wxComboBox* combo = wxDynamicCast (FindWindow (IDC_ChannelCombo), wxComboBox);
   if (combo != NULL)
		ResetComboBox (IDC_ChannelCombo);
	
   else	// combo == NULL
		ResetComboBox (IDC_Channels);
	
}		// end "OnSelendokChannelComboDropDown"



void CMDialog::OnSelendokClassPairWeightsComboDropDown (
				wxCommandEvent&					event)

{
   ResetComboBox (IDC_ClassPairWeightsCombo);
	
}	// end "OnSelendokClassPairWeightsComboDropDown"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 06/01/2015

void CMDialog::PositionDialogWindow (void)

{

   CenterOnParent ();

}	// end "PositionDialogWindow"



void CMDialog::ResetComboBox (
				int									controlID)

{
	Boolean								setNoSelectionFlag = true;
	
	
   wxComboBox* combo = wxDynamicCast (FindWindow (controlID), wxComboBox);
   switch (controlID)
		{
      case IDC_ClassCombo: 
			m_classSelection_Saved = combo->GetSelection ();
			if (m_classSelection_Saved != 1)
				setNoSelectionFlag = false;
			break;
			
      case IDC_ChannelCombo:
			m_channelSelection_Saved = combo->GetSelection ();
			if (m_channelSelection_Saved != 1)
				setNoSelectionFlag = false;
			break;
			
      case IDC_Channels:
			m_channelSelection_Saved = combo->GetSelection ();
			if (m_channelSelection_Saved != 1)
				setNoSelectionFlag = false;
			break;
			
      case IDC_WeightCombo:
			m_weightSelection_Saved = combo->GetSelection ();
			if (m_weightSelection_Saved != 1)
				setNoSelectionFlag = false;
			break;
			
      case IDC_AreasCombo:
			m_areaSelection_Saved = combo->GetSelection ();
			if (m_areaSelection_Saved != 1)
				setNoSelectionFlag = false;
			break;
			
      case IDC_ClassPairWeightsCombo:
			m_interClassWeightsSelection_Saved = combo->GetSelection ();
			if (m_interClassWeightsSelection_Saved != 1)
				setNoSelectionFlag = false;
			break;
         
      default:
			m_classSelection_Saved = 0;
			m_channelSelection_Saved = 0;
			m_weightSelection_Saved = 0;
			m_areaSelection_Saved = 0;
			m_interClassWeightsSelection_Saved = 0;
			break;
				
		}	// end "switch (controlID)"

	if (setNoSelectionFlag)
		combo->SetSelection (-1);
	
}	// end "ResetComboBox"



void CMDialog::SetComboItemText (
				UInt16								dialogItemNumber,
				SInt16								comboItem,
				CharPtr								stringPtr)

{
   const wxString						str = wxString::FromAscii (stringPtr);
	wxComboBox*							comboBoxPtr;

   UInt32								numberComboItems;
	

   comboBoxPtr = (wxComboBox*)FindWindow (dialogItemNumber);

   if (comboBoxPtr != NULL)
		{
      numberComboItems = comboBoxPtr->GetCount ();

      if (numberComboItems > (UInt32)comboItem)
         comboBoxPtr->SetString (comboItem, str);

      else	// numberComboItems <= comboItem
         comboBoxPtr->Append (str);
		
		}	// end "if (comboBoxPtr != NULL)"

}	// end "SetComboItemText"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
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
// Called By:
//
//	Coded By:			Abdur Rahman Maud		Date: ??/??/2011
//	Revised By:			Larry L. Biehl			Date: 07/20/2018

void CMDialog::SetDLogControl (
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				SInt16								setting)

{
	::SetDLogControl (dialogPtr, itemNumber, setting);

}	// end "SetDLogControl"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDLogControlHilite
//
//	Software purpose:	The purpose of this routine is to set the hilite
//				of the control in the dialog window.
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
   wxWindow* dialogItemWindowPtr = FindWindow (itemNumber);

   if (dialogItemWindowPtr != NULL)
		{
      if (setting == 0)
         dialogItemWindowPtr->Enable (true);

      else if (setting == 255)
         dialogItemWindowPtr->Enable (false);

		}	// end "if (dialogItemWindowPtr != NULL)"

}	// end "SetDLogControlHilite"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetEntireImageButtons
//
//	Software purpose:	The purpose of this routine is to set the 'to entire image/
//				to selected image' buttons.
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
				DialogPtr							dialogPtr,
				SInt32								lineStart,
				SInt32								lineEnd,
				SInt32								columnStart,
				SInt32								columnEnd)

{
			// Determine if this is the entire area.

   m_entireImageFlag = CheckIfEntireImage (m_dialogSelectArea.imageWindowInfoPtr,
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

		}	// end "if (!m_entireImageFlag || !gSelectionRectangleFlag)"

   else	// m_entireImageFlag && gSelectionRectangleFlag
		{
      wxBitmapButton* bitmapbutton = (wxBitmapButton*)FindWindow (IDEntireImage);
		
				// Save the original image position
      wxPoint pos = bitmapbutton->GetPosition ();
      MHideDialogItem (dialogPtr, IDEntireImage);
      MShowDialogItem (dialogPtr, IDSelectedImage);
      bitmapbutton = (wxBitmapButton*)FindWindow (IDSelectedImage);
		
				// Set to the original image position
      bitmapbutton->SetPosition (pos);
		
		}	// end "else m_entireImageFlag && gSelectionRectangleFlag"
   
}	// end "SetEntireImageButtons"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
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

}	// end "SetMaxNumberLinesAndColumns"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 03/26/2012	

void CMDialog::SetStaticTextOrDropDownList (
				SInt16								dropDownListSelection,
				SInt16								dropDownListID,
				SInt16								staticTextID,
				Boolean								deactivatedFlag)

{
	char									listString[64];
   wxComboBox*							comboBoxPtr;


   if (deactivatedFlag)
		{
      HideDialogItem (this, dropDownListID);
      ShowDialogItem (this, staticTextID);

      comboBoxPtr = (wxComboBox*)FindWindow (dropDownListID);
      wxString wxlistString = comboBoxPtr->GetString (dropDownListSelection);
      strcpy (listString, (const char*)wxlistString.mb_str (wxConvUTF8));

      LoadDItemString (staticTextID, listString);

		}	// end "if (projectionDeactivatedFlag)"

   else	// "!deactivatedFlag"
		{
      ShowDialogItem (this, dropDownListID);
      HideDialogItem (this, staticTextID);

		}	// end "if (deactivatedFlag)"

}	// end "SetStaticTextOrDropDownList"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ShowHideDialogItem
//
//	Software purpose:	The purpose of this routine is to show
//						or hide the specifed dialog item dependant upon
//						the input flag.
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
				DialogPtr							dialogPtr,
				SInt16								itemNumber,
				Boolean								showFlag)

{
   if (showFlag)
      MShowDialogItem (dialogPtr, itemNumber);

   else	// !showFlag
      MHideDialogItem (dialogPtr, itemNumber);

   dialogPtr->Fit ();
	
}	// end "ShowHideDialogItem"



void CMDialog::ShowSomeAreaSelectionItems ()

{
   MShowDialogItem (this, IDEntireImage);
   MShowDialogItem (this, IDSelectedImage);

			// Determine if this is the entire area and set the to entire image icon.

   SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);
	
	 MShowDialogItem (this, IDC_LineStart);
	 MShowDialogItem (this, IDC_LineEnd);
	 MShowDialogItem (this, IDC_ColumnStart);
	 MShowDialogItem (this, IDC_ColumnEnd);
	 MShowDialogItem (this, IDC_StartPrompt);
	 MShowDialogItem (this, IDC_EndPrompt);
	 MShowDialogItem (this, IDC_StartEnd);
	
}	// end "ShowSomeAreaSelectionItems"



void CMDialog::ToEntireImage (
				wxCommandEvent&					event)

{
			//	Change to entire image

   m_LineStart = 1;
   m_LineEnd = m_maxNumberLines;
   m_ColumnStart = 1;
   m_ColumnEnd = m_maxNumberColumns;

   m_settingSelectedEntireButton = TRUE;
   LoadDItemValue (IDC_LineStart, m_LineStart);
   LoadDItemValue (IDC_LineEnd, m_LineEnd);
   LoadDItemValue (IDC_ColumnStart, m_ColumnStart);
   LoadDItemValue (IDC_ColumnEnd, m_ColumnEnd);
   m_settingSelectedEntireButton = FALSE;

			// Update m_dialogSelectArea since it will not get updated with the
			// LoadDItemValue calls above.

   m_dialogSelectArea.lineStart = m_LineStart;
   m_dialogSelectArea.lineEnd = m_LineEnd;
   m_dialogSelectArea.columnStart = m_ColumnStart;
   m_dialogSelectArea.columnEnd = m_ColumnEnd;

   FindWindow (IDC_LineStart)->SetFocus ();

   SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);
	
   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

}	// end "ToEntireImage"



void CMDialog::ToSelectedImage (
				wxCommandEvent&					event)

{
   LongRect selectionRect;


			// Change to selected area

   if (GetSelectionRectangle (m_dialogSelectArea.windowPtr,
										&selectionRect,
										kDontClearSelectionArea,
										kUseThreshold,
										kDontAdjustToBaseImage))
		{
      m_LineStart = selectionRect.top;
      m_LineEnd = selectionRect.bottom;
      m_ColumnStart = selectionRect.left;
      m_ColumnEnd = selectionRect.right;

      m_settingSelectedEntireButton = TRUE;
      LoadDItemValue (IDC_LineStart, m_LineStart);
      LoadDItemValue (IDC_LineEnd, m_LineEnd);
      LoadDItemValue (IDC_ColumnStart, m_ColumnStart);
      LoadDItemValue (IDC_ColumnEnd, m_ColumnEnd);
      m_settingSelectedEntireButton = FALSE;

				// Update m_dialogSelectArea since it will not get updated with the
				// LoadDItemValue calls above.

      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;

      FindWindow (IDC_LineStart)->SetFocus ();

      SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);

      SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

		}	// end "if (GetLineColumnSelection (..."

}	// end "ToSelectedImage"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int UpdateOneColListSelections
//
//	Software purpose:	The purpose of this routine is to update the selections
//						in the one column list based on the start, end
//						and interval values.
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
//	Revised By:			Larry L. Biehl			Date: 05/01/2015

UInt16 CMDialog::UpdateOneColListSelections (
				UInt16								numberInputVecItems,
				UInt16								listStart,
				UInt16								listEnd,
				UInt16								listInterval)

{

	wxListBox*							wxListBoxPtr;

   UInt16								index,
											item,
											nextTrueSelection,
											numberSelections;
	
   bool									selectionFlag;
	

   index = 0;
   wxListBoxPtr = (wxListBox*)FindWindow (IDC_List1);
   if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount () > 0)
		{
      for (item=1; item<listStart; item++)
			{
         wxListBoxPtr->Deselect (index);
         index++;

			}	// end "for ( item=1; item<..."

      nextTrueSelection = listStart;
      for (item=listStart; item<=listEnd; item++)
			{
         selectionFlag = (item == nextTrueSelection);

         if (selectionFlag)
				{
            nextTrueSelection += listInterval;
            wxListBoxPtr->Select (index);
				
				}	// end "if (selectionFlag)"
			
         else
            wxListBoxPtr->Deselect (index);

         index++;

      }	// end "for ( item=listStart; item..."

      for (item=listEnd+1; item<=numberInputVecItems; item++)
			{
         wxListBoxPtr->Deselect (index);
         index++;

			}	// end "for ( item=listEnd+1; item<..."

		}	// end "if (wxListBoxPtr != NULL && wxListBoxPtr->GetCount () > 0)"

   numberSelections = (listEnd - listStart + listInterval) / listInterval;

   return (numberSelections);

}	// end "UpdateOneColListSelections"



void CMDialog::UpdateAllSubsetList (
				UInt16								channelsMenuItemNumber)

{
   if (m_channelListType == kSelectedItemsListOnly)
		{
      if (((wxComboBox*)FindWindow (channelsMenuItemNumber))->GetCount () == 2)
			{
         if (m_channelSelection == kAllMenuItem)
            ((wxComboBox*)FindWindow (channelsMenuItemNumber))->Delete (1);

         else	// m_channelSelection == kSubsetMenuItem
            ((wxComboBox*)FindWindow (channelsMenuItemNumber))->Delete (0);

			}	// end "if (...->GetCount () == 2 )"

		}	// end "if (m_channelListType == kSelectedChannelsListOnly)"

   else	// m_channelListType != kSelectedChannelsListOnly
		{
      if (((wxComboBox*)FindWindow (channelsMenuItemNumber))->GetCount () == 1)
			{
         ((wxComboBox*)FindWindow (channelsMenuItemNumber))->Delete (0);

         ((wxComboBox*)FindWindow (channelsMenuItemNumber))->Append (wxT("All"));
         ((wxComboBox*)FindWindow (channelsMenuItemNumber))->Append (wxT("Subset..."));

			}	// end "if (...->GetCount () == 1 )"

   }	// end "else m_channelListType != kSelectedChannelsListOnly"

}	// end "UpdateAllSubsetList"



SInt16 CMDialog::VerifyLineColumnStartEndValues ()

{
			// Verify that the line and column values make sense

   SInt16 returnCode = 0;

   if (m_LineStart > m_LineEnd)
		{
      DisplayAlert (kErrorAlertID,
							kStopAlert,
							kAlertStrID,
							IDS_Alert68,
							0,
							NULL);

      returnCode = IDC_LineStart;

		}	// end "m_LineStart > m_LineEnd"

   else if (m_ColumnStart > m_ColumnEnd)
		{
      DisplayAlert (kErrorAlertID,
							kStopAlert,
							kAlertStrID,
							IDS_Alert69,
							0,
							NULL);

      returnCode = IDC_ColumnStart;

		}	// end "else if (m_ColumnStart > m_ColumnEnd)"

   return returnCode;

}	// end "VerifyLineColumnStartEndValues"


SInt16 CMDialog::VerifyLineColumnValues (
				SInt16								startLineItem,
				SInt16								startColumnItem,
				Boolean								checkIntervalFlag)

{
	SInt32								maxValue;

   SInt16								code,
											returnCode = 0;


			// Verify that none of the entries are blank.

   if (m_LineStartString.empty ())
      returnCode = IDC_LineStart;

   else if (m_LineEndString.empty ())
      returnCode = IDC_LineEnd;

   else if (checkIntervalFlag && m_LineIntervalString.empty ())
      returnCode = IDC_LineInterval;

   if (returnCode != 0)
      maxValue = m_maxNumberLines;

   if (returnCode == 0)
		{
      if (m_ColumnStartString.empty ())
         returnCode = IDC_ColumnStart;

      else if (m_ColumnEndString.empty ())
         returnCode = IDC_ColumnEnd;

      else if (checkIntervalFlag && m_ColumnIntervalString.empty ())
         returnCode = IDC_ColumnInterval;

      if (returnCode != 0)
         maxValue = m_maxNumberColumns;

		}	// end "if (returnCode == 0)"

   if (returnCode != 0) 
		{
      if (LoadSpecifiedStringNumberLongP (kAlertStrID,
														IDS_Alert91,
														(char*)gTextString,
														(char*)gTextString2,
														TRUE,
														1,
														maxValue))
         DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);

		}	// end "if (returnCode != 0)"

			// Verify that the line and column values make sense

   if (returnCode == 0)
      returnCode = VerifyLineColumnStartEndValues ();

   if (returnCode != 0)
      SelectDialogItemText (this, returnCode, 0, SHRT_MAX);

   return (returnCode);

}	// end "VerifyLineColumnValues"
