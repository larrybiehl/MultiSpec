//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LLegendView.cpp : class implementation file
//	Class Definition:		LLegendView.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/08/2019
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMLegendView class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " LLegendView:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------
//

#include "SMultiSpec.h"

#include "CPalette.h" 

#include "LMultiSpec.h"
#include "LImageView.h"
#include "LImageDoc.h"  
#include "LImageFrame.h"
#include "LMainFrame.h"    
#include "LLegendView.h"

bool CMLegendView::s_inSetLegendWidthFlag = FALSE;



// CMLegendView

IMPLEMENT_DYNAMIC_CLASS (CMLegendView, wxPanel)

CMLegendView::CMLegendView ()
{
}



CMLegendView::CMLegendView (
				wxWindow*							parent,
				wxWindowID							id,
				wxDocument*							doc,
				CMImageView*						view)
		: wxPanel(parent, CMLegendView::IDD)

{
   m_classGroupCode = kClassDisplay - 1;
   m_paletteSelection = 0;

   m_initializedFlag = FALSE;
   m_legendWidthSetFlag = FALSE;

   m_imageViewCPtr = view;
   m_numberDisplayedListItems = 0;

   m_pDocument = (CMImageDoc*)doc;
   m_initializedFlag = TRUE;

   SetupView ();
   InitDialog ();

}	// end "CMLegendView"



CMLegendView::~CMLegendView ()
{

}	// end "~CMLegendView"



BEGIN_EVENT_TABLE (CMLegendView, wxPanel)
	EVT_CHOICE (IDC_COMBO1, CMLegendView::OnSelendokClassGroup)
	EVT_COMBOBOX (IDC_PaletteCombo, CMLegendView::OnSelendokPalette)
	EVT_COMBOBOX_DROPDOWN (IDC_PaletteCombo, CMLegendView::OnDropdownLegendCombo)
	EVT_INIT_DIALOG (CMLegendView::OnInitialUpdate)
END_EVENT_TABLE()



CMImageFrame* CMLegendView::GetImageFrameCPtr (void)

{
   CMImageFrame* imageFrameCPtr = NULL;

   if (this != NULL)
      imageFrameCPtr = GetDocument()->GetImageFrameCPtr ();

   return (imageFrameCPtr);

}	// end "GetImageFrameCPtr"



CMLegendList* CMLegendView::GetLegendListCPtr (void)

{
   CMLegendList* legendListCPtr = NULL;

   if (this != NULL)
      legendListCPtr = m_legendListBox;

   return (legendListCPtr);

}	// end "GetLegendListCPtr"



void CMLegendView::OnDblclkList1 ()

{
}	// end "OnDblclkList1"



void CMLegendView::OnDropdownClassGroupCombo ()

{
	if (((wxChoice*)FindWindow (IDC_COMBO1))->GetCount () < 3)
		{
				// Check if group string needs to be added.

      if (m_imageViewCPtr->GetNumberGroups() > 0) 
			{
         ((wxChoice*)FindWindow (IDC_COMBO1))->Insert (wxT("Groups"),
																			kGroupDisplay - 1);
         if (m_classGroupCode > 0) 
				{
            m_classGroupCode++;
            ((wxChoice*)FindWindow (IDC_COMBO1))->SetSelection (m_classGroupCode);

				}	// end "if (m_classGroupCode > 0)"

			}	// end "if (m_imageViewCPtr->GetNumberGroups() > 0)"

		}	// end "if( ((CComboBox*)GetDlgItem(IDC_COMBO1))->Count() < 3 )"

}	// end "OnDropdownClassGroupCombo"



void CMLegendView::OnDropdownLegendCombo (
				wxCommandEvent&					event)

{
   UpdateLegendComboBox ();
	
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.
			
	m_legendPaletteCombo->SetSelection (-1);

}	// end "OnDropdownLegendCombo"



void CMLegendView::OnInitialUpdate (
				wxInitDialogEvent&				event)

{
   Handle windowInfoHandle = GetWindowInfoHandle (m_imageViewCPtr);
   WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);

   if (windowInfoPtr != NULL)
      windowInfoPtr->legendListHandle = m_legendListBox;

   UpdateLegendComboBox ();

			// If the number of groups is 0, then do not allow the Groups option.

   if (m_imageViewCPtr->GetNumberGroups() <= 0)
      ((wxChoice*)FindWindow (IDC_COMBO1))->Delete (kGroupDisplay - 1);

   ((wxChoice*)FindWindow (IDC_COMBO1))->SetSelection (m_classGroupCode);

}	// end "OnInitialUpdate"



void CMLegendView::OnSelendokClassGroup (
				wxCommandEvent&					event)

{
	int						classGroupCode,
								savedClassGroupCode;
	
	
	savedClassGroupCode = m_classGroupCode;
   CMImageFrame* imageFrameCPtr = m_imageViewCPtr->GetImageFrameCPtr ();
   m_imageViewCPtr->GetImageFrameCPtr ()->UpdateActiveImageWindowInfo ();

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



void CMLegendView::OnSelendokPalette (
				wxCommandEvent&					event)

{
	SInt16								lastComboPaletteSelection,
											lastPaletteCode,
											paletteCode;
	

   m_imageViewCPtr->GetImageFrameCPtr()->UpdateActiveImageWindowInfo ();

   lastComboPaletteSelection = m_paletteSelection;
   m_paletteSelection = m_legendPaletteCombo->GetSelection ();

   SInt64 palette64 =
				(SInt64)((int*)m_legendPaletteCombo->GetClientData (m_paletteSelection));
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
            m_legendPaletteCombo->SetSelection (m_paletteSelection);

				}	// end "if (lastPaletteSelection != m_paletteSelection)"

         palette64 =
					(SInt64)((int*)m_legendPaletteCombo->GetClientData (m_paletteSelection));
         paletteCode = (SInt16)palette64;

			}	// end "else !FalseColorPaletteDialog()"

		}	// end "if (paletteSelection == kFalseColors)"

   SInt64 lastpalette64 =
			(SInt64)((int*)m_legendPaletteCombo->GetClientData (lastComboPaletteSelection));
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
	
}	// end "OnSelendokPalette"



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
	wxComboBox*			comboBoxPtr17;
	
   this->SetBackgroundColour (wxColour (255, 255, 255));

   wxBoxSizer* bSizer133;
   bSizer133 = new wxBoxSizer (wxVERTICAL);

   wxString m_choice16Choices[] = {wxT("Classes"),
												wxT("Groups"),
												wxT("Groups/ Classes")};
   int m_choice16NChoices = sizeof (m_choice16Choices) / sizeof (wxString);
   m_choice16 = new wxChoice (this,
										IDC_COMBO1,
										wxDefaultPosition,
										wxDefaultSize,
										m_choice16NChoices,
										m_choice16Choices,
										0);
   m_choice16->SetSelection (0);
   //bSizer133->Add (m_choice16, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
   bSizer133->Add (m_choice16,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxTOP, 4));

   m_legendListBox = new CMLegendList (this);
    bSizer133->Add (m_legendListBox, 2, wxALL | wxEXPAND, 5);

   comboBoxPtr17 = new wxComboBox (this, IDC_PaletteCombo, wxT("Default Colors"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
   comboBoxPtr17->Append (wxT("Default Colors"));
	comboBoxPtr17->Append (wxT("Default Gray Levels"));
	comboBoxPtr17->Append (wxT("Blue-Green-Red"));
	comboBoxPtr17->Append (wxT("AVHRR NDVI"));
	comboBoxPtr17->Append (wxT("MODIS NDVI"));
	comboBoxPtr17->Append (wxT("False Color..."));
	comboBoxPtr17->Append (wxT("ERDAS .trl file"));
	comboBoxPtr17->Append (wxT("User Defined"));
	
	comboBoxPtr17->SetClientData (0, (void*)kDefaultColors);
	comboBoxPtr17->SetClientData (1, (void*)kDefaultGrays);
	comboBoxPtr17->SetClientData (2, (void*)kCorrelationMatrixColors);
	comboBoxPtr17->SetClientData (3, (void*)kAVHRR_NDVI_Colors);
	comboBoxPtr17->SetClientData (4, (void*)kMODIS_NDVI_Colors);
	comboBoxPtr17->SetClientData (5, (void*)kFalseColors);
	comboBoxPtr17->SetClientData (6, (void*)kImageDefaultColorTable);
	comboBoxPtr17->SetClientData (7, (void*)kUserDefinedColors);
	
	comboBoxPtr17->SetSelection (0);
   comboBoxPtr17->SetToolTip (wxT("Palette to use"));

   bSizer133->Add (comboBoxPtr17, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

   this->SetSizer (bSizer133);
   this->Layout ();
   bSizer133->Fit (this);
   m_legendTitleCombo = m_choice16;
   m_legendPaletteCombo = comboBoxPtr17;
	
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
													GetLegendListCPtr ()->m_imageFileInfoHandle);

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
		/*
      m_paletteSelection = ::GetComboListSelection (this,
																		IDC_PaletteCombo,
																		paletteSelection);
		
		if (m_paletteSelection < 0)
					// This implies that the False Color option is not available now.
					// Set paletteSelection to the last option available which is the
					// the original
			m_paletteSelection = m_legendPaletteCombo->GetCount () - 1;

      m_legendPaletteCombo->SetSelection (m_paletteSelection);
		*/
		m_paletteSelection = m_legendPaletteCombo->GetSelection ();

		}	// end "if (fileInfoPtr != NULL)"

}	// end "UpdateLegendComboBox"



void CMLegendView::UpdateThematicLegendControls ()

{
   SInt16								itemsInList,
											topIndex;

			// Legend Control

   DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														m_imageViewCPtr->GetDisplaySpecsHandle ());

   UpdateLegendComboBox ();
   if (displaySpecsPtr->imageDimensions[kVertical] > 0)
      m_legendPaletteCombo->Enable (TRUE);

   else // displaySpecsPtr->imageDimensions[kVertical] <= 0 
      m_legendPaletteCombo->Enable (FALSE);

} // end "UpdateThematicLegendButtons"

