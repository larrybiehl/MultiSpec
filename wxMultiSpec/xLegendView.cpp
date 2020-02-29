//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						xLegendView.cpp : class implementation file
//	Class Definition:		xLegendView.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/28/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMLegendView class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " xLegendView:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SPalette_class.h"

#include "xMultiSpec.h"
#include "xImageView.h"
#include "xImageDoc.h"  
#include "xImageFrame.h"
#include "xMainFrame.h"    
#include "xLegendView.h"

bool CMLegendView::s_inSetLegendWidthFlag = FALSE;


IMPLEMENT_DYNAMIC_CLASS (CMLegendView, wxPanel)



BEGIN_EVENT_TABLE (CMLegendView, wxPanel)
	EVT_CHOICE (IDC_COMBO1, CMLegendView::OnSelendokClassGroup)

	EVT_COMBOBOX (IDC_PaletteCombo, CMLegendView::OnPaletteCtrlSelendok)
	EVT_COMBOBOX_CLOSEUP (IDC_PaletteCombo, CMLegendView::OnPaletteCtrlCloseUp)
	EVT_COMBOBOX_DROPDOWN (IDC_PaletteCombo, CMLegendView::OnPaletteCtrlDropDown)

	EVT_INIT_DIALOG (CMLegendView::OnInitialUpdate)
END_EVENT_TABLE ()



CMLegendView::CMLegendView ()

{

}	// end "CMLegendView"



CMLegendView::CMLegendView (
				wxWindow*							parent,
				wxWindowID							id,
				wxDocument*							doc,
				CMImageView*						view)
		: wxPanel (parent, CMLegendView::IDD)

{
   m_classGroupCode = kClassDisplay - 1;
   m_paletteSelection = 0;

   m_initializedFlag = FALSE;
   m_legendWidthSetFlag = FALSE;

   m_imageViewCPtr = view;
   m_numberDisplayedListItems = 0;

   m_pDocument = (CMImageDoc*)doc;
   m_initializedFlag = TRUE;
   m_localPaletteUpToDateFlag = FALSE;

   SetupView ();
   InitDialog ();

}	// end "CMLegendView"



CMLegendView::~CMLegendView ()

{

}	// end "~CMLegendView"



CMImageFrame* CMLegendView::GetImageFrameCPtr (void)

{
   return (GetDocument()->GetImageFrameCPtr ());

}	// end "GetImageFrameCPtr"



CMLegendList* CMLegendView::GetLegendListCPtr (void)

{
   return (m_legendListBox);

}	// end "GetLegendListCPtr"



void CMLegendView::OnDropdownClassGroupCombo ()

{
	if (((wxChoice*)FindWindow (IDC_COMBO1))->GetCount () < 3)
		{
				// Check if group string needs to be added.

      if (m_imageViewCPtr->GetNumberGroups () > 0)
			{
         ((wxChoice*)FindWindow (IDC_COMBO1))->Insert (wxT("Groups"),
																			kGroupDisplay - 1);
         if (m_classGroupCode > 0) 
				{
            m_classGroupCode++;
            ((wxChoice*)FindWindow (IDC_COMBO1))->SetSelection (m_classGroupCode);

				}	// end "if (m_classGroupCode > 0)"

			}	// end "if (m_imageViewCPtr->GetNumberGroups () > 0)"

		}	// end "if (((wxChoice*)GetDlgItem (IDC_COMBO1))->Count () < 3)"

}	// end "OnDropdownClassGroupCombo"



void CMLegendView::OnInitialUpdate (
				wxInitDialogEvent&				event)

{
   Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
   WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

   if (windowInfoPtr != NULL)
      windowInfoPtr->legendListHandle = m_legendListBox;

   UpdateLegendComboBox ();

}	// end "OnInitialUpdate"



void CMLegendView::OnPaletteCtrlCloseUp (
				wxCommandEvent& 					event)

{
	int paletteSelection = m_legendPaletteCtrl->GetSelection ();
	
	if (paletteSelection == -1)
		m_legendPaletteCtrl->SetSelection (m_paletteSelection);
	
	event.Skip ();
	
}	// end "OnPaletteCtrlCloseUp"



void CMLegendView::OnPaletteCtrlDropDown (
				wxCommandEvent&					event)

{
   UpdateLegendComboBox ();
	
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.
	
	m_legendPaletteCtrl->SetSelection (-1);

}	// end "OnPaletteCtrlDropDown"



void CMLegendView::OnPaletteCtrlSelendok (
				wxCommandEvent&					event)

{
	SInt16								lastComboPaletteSelection,
											lastPaletteCode,
											paletteCode;
	

   m_imageViewCPtr->GetImageFrameCPtr()->UpdateActiveImageWindowInfo ();

   lastComboPaletteSelection = m_paletteSelection;
   m_paletteSelection = m_legendPaletteCtrl->GetSelection ();

   SInt64 palette64 =
				(SInt64)((int*)m_legendPaletteCtrl->GetClientData (m_paletteSelection));
   paletteCode = (SInt16)palette64;
	
   if (paletteCode == kFalseColors)
		{
      if (FalseColorPaletteDialog ())
         m_localPaletteUpToDateFlag = FALSE;

      else	// !FalseColorPaletteDialog ()
			{
         if (lastComboPaletteSelection != m_paletteSelection)
				{
            m_paletteSelection = lastComboPaletteSelection;
            m_legendPaletteCtrl->SetSelection (m_paletteSelection);

				}	// end "if (lastPaletteSelection != m_paletteSelection)"

         palette64 =
					(SInt64)((int*)m_legendPaletteCtrl->GetClientData (m_paletteSelection));
         paletteCode = (SInt16)palette64;

			}	// end "else !FalseColorPaletteDialog ()"

		}	// end "if (paletteSelection == kFalseColors)"

   SInt64 lastpalette64 =
			(SInt64)((int*)m_legendPaletteCtrl->GetClientData (lastComboPaletteSelection));
   lastPaletteCode = (SInt16)lastpalette64;
	
   if (!m_localPaletteUpToDateFlag || paletteCode != lastPaletteCode)
		{
      Handle displaySpecsHandle = m_imageViewCPtr->GetDisplaySpecsHandle ();
      DisplaySpecsPtr displaySpecsPtr =
										(DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);

      SetPaletteSpecification (displaySpecsPtr,
											displaySpecsPtr->classGroupCode,
											paletteCode,
											m_localPaletteUpToDateFlag);

      UpdateActiveImageLegend (displaySpecsPtr->classGroupCode, kCallCreatePalette);
		
		displaySpecsPtr->paletteObject->SetPaletteLoadedFlag (FALSE);

		}	// end "if (paletteCode != lastPaletteSelection)"

   (m_imageViewCPtr->m_Canvas)->Refresh ();
	
}	// end "OnPaletteCtrlSelendok"



void CMLegendView::OnSelendokClassGroup (
				wxCommandEvent&					event)

{
	int						classGroupCode,
								savedClassGroupCode;
	
	
	savedClassGroupCode = m_classGroupCode;
   CMImageFrame* imageFrameCPtr = m_imageViewCPtr->GetImageFrameCPtr ();
   m_imageViewCPtr->GetImageFrameCPtr()->UpdateActiveImageWindowInfo ();

	wxChoice* clgroup = (wxChoice*)FindWindow (IDC_COMBO1);
	m_classGroupCode = clgroup->GetSelection ();

	if (m_classGroupCode != savedClassGroupCode)
		{
		classGroupCode = m_classGroupCode + 1;
		if (m_imageViewCPtr->GetNumberGroups () <= 0 && m_classGroupCode == 1)
			classGroupCode = kGroupClassDisplay;

		//imageFrameCPtr->ChangeClassGroupDisplay (classGroupCode, FALSE);
		imageFrameCPtr->ChangeClassGroupDisplay (classGroupCode);
		
		}	// end "if (m_classGroupCode != savedClassGroupCode)"
	
	OnDropdownClassGroupCombo ();
	m_legendListBox->DrawLegendList ();
	(m_imageViewCPtr->m_Canvas)->Refresh ();
	
			// Reset the maximization flag
	
	m_imageViewCPtr->m_frame->m_frameMaximized = false;

}	// end "OnSelendokClassGroup"



void CMLegendView::SetLegendWidthSetFlag (void)

{
   m_legendWidthSetFlag = TRUE;

}	// end "SetLegendWidthSetFlag"



void CMLegendView::SetImageView (
				CMImageView*						imageViewCPtr)

{
   m_imageViewCPtr = imageViewCPtr;

}	// end "SetImageView"



void CMLegendView::SetupView ()

{
   SetBackgroundColour (wxColour (255, 255, 255));

   wxBoxSizer* bSizer133;
   bSizer133 = new wxBoxSizer (wxVERTICAL);

   wxString m_choice16Choices[] = {wxT("Classes"),
												//wxT("Groups"),
												wxT("Groups / Classes")};
   int m_choice16NChoices = sizeof (m_choice16Choices) / sizeof (wxString);
   m_legendTitleCtrl = new wxChoice (this,
													IDC_COMBO1,
													wxDefaultPosition,
													wxDefaultSize,
													m_choice16NChoices,
													m_choice16Choices,
													0);
   m_legendTitleCtrl->SetSelection (0);
   bSizer133->Add (m_legendTitleCtrl,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxTOP, 4));

   m_legendListBox = new CMLegendList (this);
    bSizer133->Add (m_legendListBox, 2, wxALL | wxEXPAND, 5);

			// Would like to use wxChoice here but really need the drop down event
			// to make sure the palette menu is up to date relative to changes in
			// what the user is doing. For example a project may have been opened
			// which could affect the choices in the menu like False Color.
	
	m_legendPaletteCtrl = new wxComboBox (this,
														IDC_PaletteCombo,
														wxT("Default Colors"),
														wxDefaultPosition,
														wxDefaultSize,
														0,
														NULL,
														wxCB_READONLY);
   m_legendPaletteCtrl->Append (wxT("Default Colors"));
	m_legendPaletteCtrl->Append (wxT("Default Gray Levels"));
	m_legendPaletteCtrl->Append (wxT("Blue-Green-Red"));
	m_legendPaletteCtrl->Append (wxT("AVHRR NDVI"));
	m_legendPaletteCtrl->Append (wxT("MODIS NDVI"));
	m_legendPaletteCtrl->Append (wxT("False Color..."));
	m_legendPaletteCtrl->Append (wxT("ERDAS .trl file"));
	m_legendPaletteCtrl->Append (wxT("User Defined"));
	
	m_legendPaletteCtrl->SetClientData (0, (void*)kDefaultColors);
	m_legendPaletteCtrl->SetClientData (1, (void*)kDefaultGrays);
	m_legendPaletteCtrl->SetClientData (2, (void*)kCorrelationMatrixColors);
	m_legendPaletteCtrl->SetClientData (3, (void*)kAVHRR_NDVI_Colors);
	m_legendPaletteCtrl->SetClientData (4, (void*)kMODIS_NDVI_Colors);
	m_legendPaletteCtrl->SetClientData (5, (void*)kFalseColors);
	m_legendPaletteCtrl->SetClientData (6, (void*)kImageDefaultColorTable);
	m_legendPaletteCtrl->SetClientData (7, (void*)kUserDefinedColors);
	
	m_legendPaletteCtrl->SetSelection (0);
   m_legendPaletteCtrl->SetToolTip (wxT("Palette to use"));

   bSizer133->Add (m_legendPaletteCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 4);

   SetSizer (bSizer133);
   Layout ();
   bSizer133->Fit (this);
	
}	// end "SetupView"



void CMLegendView::UpdateClassGroupComboList (
				SInt16								newClassGroupCode)

{
			// Make sure that the combo list is correct.

	OnDropdownClassGroupCombo ();

   m_classGroupCode = newClassGroupCode - 1;
   ((wxChoice*)FindWindow (IDC_COMBO1))->SetSelection (m_classGroupCode);

}	// end "UpdateClassGroupComboList"



void CMLegendView::UpdateLegendComboBox (void)

{
	DisplaySpecsPtr					displaySpecsPtr;
	FileInfoPtr							fileInfoPtr;

   Handle								nameHandle;

   UInt32								colorTableOffset,
											numberClasses;

   SInt16								ancillaryInfoFormat,
											format,
											paletteCode;


   fileInfoPtr = (FileInfoPtr)GetHandlePointer (
													GetLegendListCPtr()->m_imageFileInfoHandle);

   if (fileInfoPtr != NULL)
		{
      nameHandle = fileInfoPtr->classDescriptionH;
      format = fileInfoPtr->format;
      ancillaryInfoFormat = fileInfoPtr->ancillaryInfoformat;
      numberClasses = fileInfoPtr->numberClasses;
      colorTableOffset = fileInfoPtr->colorTableOffset;

      displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														m_imageViewCPtr->GetDisplaySpecsHandle (),
														kLock);

      if (displaySpecsPtr->classGroupCode == kClassDisplay)
         paletteCode = displaySpecsPtr->thematicClassPaletteType;

      else // ...classGroupCode != kClassDisplay 
         paletteCode = displaySpecsPtr->thematicGroupPaletteType;

      paletteCode = SetUpPalettePopUpMenu ((CMDialog*)this,
																NULL,
																format,
																ancillaryInfoFormat,
																numberClasses,
																nameHandle,
																colorTableOffset,
																displaySpecsPtr,
																displaySpecsPtr->classGroupCode,
																paletteCode);

      CheckAndUnlockHandle (m_imageViewCPtr->GetDisplaySpecsHandle ());

		m_paletteSelection = m_legendPaletteCtrl->GetSelection ();

		}	// end "if (fileInfoPtr != NULL)"

}	// end "UpdateLegendComboBox"



void CMLegendView::UpdateThematicLegendControls ()

{
			// Legend Control

   DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														m_imageViewCPtr->GetDisplaySpecsHandle ());

   UpdateLegendComboBox ();
   if (displaySpecsPtr->imageDimensions[kVertical] > 0)
      m_legendPaletteCtrl->Enable (TRUE);

   else // displaySpecsPtr->imageDimensions[kVertical] <= 0 
      m_legendPaletteCtrl->Enable (FALSE);

}	// end "UpdateThematicLegendButtons"

