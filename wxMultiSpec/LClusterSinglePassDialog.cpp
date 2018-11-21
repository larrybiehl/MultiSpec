//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClusterSinglePassDialog.cpp : class implementation file
//	Class Definition:		LClusterSinglePassDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/06/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMSinglePassClusterDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "LClusterSinglePassDialog.h"
#include	"SExternalGlobals.h"  
#include "LImage_dialog.cpp"



IMPLEMENT_DYNAMIC_CLASS (CMSinglePassClusterDialog, CMDialog)

CMSinglePassClusterDialog::CMSinglePassClusterDialog ()
{
   
}



CMSinglePassClusterDialog::CMSinglePassClusterDialog (
				wxWindow*							parent,
				wxWindowID							id,
				const wxString&					title)
		: CMDialog (CMSinglePassClusterDialog::IDD, parent, title)

{
   m_criticalDistance1 = 1;
   m_criticalDistance2 = 1;
   m_minClusterSize = 1;
   m_clustersFrom = -1;
   m_classSelection = -1;
	m_classListPtr = NULL; 

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
				// Get memory for the local class list vector if it may be needed.

      if (gProjectInfoPtr->numberStatisticsClasses > 0)
			{
         m_classListPtr = (UInt16*)MNewPointer (
                 (UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

         m_initializedFlag = (m_classListPtr != NULL);

			}	// end "if (...->numberStatisticsClasses > 0)"

		}	// end "if (m_initializedFlag)"
   
   CreateControls ();

}	// end "CMSinglePassClusterDialog"



CMSinglePassClusterDialog::~CMSinglePassClusterDialog (void)

{
   m_classListPtr = CheckAndDisposePtr (m_classListPtr);

}	// end "~CMSinglePassClusterDialog"



BEGIN_EVENT_TABLE (CMSinglePassClusterDialog, CMDialog)
	EVT_INIT_DIALOG (CMSinglePassClusterDialog::OnInitDialog)
	EVT_COMBOBOX (IDC_ClassCombo, CMSinglePassClusterDialog::OnSelendokClassCombo)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMSinglePassClusterDialog::OnSelendokClassComboDropDown)
	EVT_RADIOBUTTON (IDC_ClusterTrainingAreas, CMSinglePassClusterDialog::OnClusterTrainingAreas)
	EVT_RADIOBUTTON (IDC_ClusterImageArea, CMSinglePassClusterDialog::OnClusterImageArea)
	EVT_BUTTON (IDEntireImage, CMSinglePassClusterDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMSinglePassClusterDialog::ToSelectedImage)
	EVT_TEXT (IDC_ColumnEnd, CMSinglePassClusterDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMSinglePassClusterDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMSinglePassClusterDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMSinglePassClusterDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMSinglePassClusterDialog::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMSinglePassClusterDialog::CheckColumnInterval)
	//EVT_CHAR_HOOK (CMSinglePassClusterDialog::OnButtonPress)
END_EVENT_TABLE ()



void CMSinglePassClusterDialog::ClustersFromSetting (
				bool									from,
				int									nIDC,
				int&									value)

{
	wxRadioButton						*imgarea,
											*trainingarea;
	
   bool									settingFlag;
	

   trainingarea = (wxRadioButton*)FindWindow (IDC_ClusterTrainingAreas);
   imgarea = (wxRadioButton*)FindWindow (IDC_ClusterImageArea);

   if (from)
		{
				// Reading from radio group. Get index ignoring the disabled items.

      value = -1;

		settingFlag = trainingarea->GetValue ();
      if (settingFlag)
         value = 0;

		settingFlag = imgarea->GetValue ();
      if (settingFlag)
         value = 1;

		}	// end "if (from)"

   else	// !from
		{
      int localValue = value;

      bool savedTrainingAreaEnabledFlag;


      savedTrainingAreaEnabledFlag = trainingarea->IsEnabled ();

				// Setting specific radio group item. Take into account any disabled items.

      if (!savedTrainingAreaEnabledFlag)
         trainingarea->Enable (true);

      if (localValue == 0)
         trainingarea->SetValue (true);
      else
         imgarea->SetValue (true);
		
      if (!savedTrainingAreaEnabledFlag)
         trainingarea->Enable (false);

		}	// end "else !from"

}	// end "ClustersFromSetting"
 


void CMSinglePassClusterDialog::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
   /*
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);
	*/
   bSizer150 = new wxBoxSizer (wxVERTICAL);

   wxStaticBoxSizer* sbSizer30;
   sbSizer30 = new wxStaticBoxSizer (new wxStaticBox (this,
   																	wxID_ANY,
   																	wxT("Options"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxVERTICAL);

   wxFlexGridSizer* fgSizer6;
   fgSizer6 = new wxFlexGridSizer (3, 2, 0, 0);
   fgSizer6->SetFlexibleDirection (wxBOTH);
   fgSizer6->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

   m_staticText189 = new wxStaticText (sbSizer30->GetStaticBox (),
													wxID_ANY,
													wxT("Minimum cluster size:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText189->Wrap (-1);
   SetUpToolTip (m_staticText189, IDS_ToolTip91);  
   //fgSizer6->Add (m_staticText189, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
   fgSizer6->Add (m_staticText189,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_textCtrl98 = new wxTextCtrl (sbSizer30->GetStaticBox (),
												IDC_MinClusterSize,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl98->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																&m_minClusterSizeString));
   SetUpToolTip (m_textCtrl98, IDS_ToolTip91);  
   //fgSizer6->Add (m_textCtrl98, 0, wxALL, 5);
   fgSizer6->Add (m_textCtrl98, wxSizerFlags(0).Border(wxALL, 5));

   m_staticText190 = new wxStaticText (sbSizer30->GetStaticBox (),
													wxID_ANY,
													wxT("Critical distance 1 (first line):"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText190->Wrap (-1);
   SetUpToolTip (m_staticText190, IDS_ToolTip92); 
   //fgSizer6->Add (m_staticText190, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
   fgSizer6->Add (m_staticText190,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_textCtrl99 = new wxTextCtrl (sbSizer30->GetStaticBox (),
												IDC_CriticalDistance1,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_textCtrl99->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_distance1String));
   SetUpToolTip (m_textCtrl99, IDS_ToolTip92);  
   //fgSizer6->Add (m_textCtrl99, 0, wxALL, 5);
   fgSizer6->Add (m_textCtrl99, wxSizerFlags(0).Border(wxALL, 5));

   m_staticText191 = new wxStaticText (sbSizer30->GetStaticBox (),
													wxID_ANY,
													wxT("Critical distance 2 (other lines):"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText191->Wrap (-1);   
   SetUpToolTip (m_staticText191, IDS_ToolTip93);
   //fgSizer6->Add (m_staticText191, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
   fgSizer6->Add (m_staticText191,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_textCtrl100 = new wxTextCtrl (sbSizer30->GetStaticBox (),
												IDC_CriticalDistance2,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl100->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_distance2String));
   SetUpToolTip (m_textCtrl100, IDS_ToolTip93);  
   //fgSizer6->Add (m_textCtrl100, 0, wxALL, 5);
   fgSizer6->Add (m_textCtrl100, wxSizerFlags(0).Border(wxALL, 5));

   //sbSizer30->Add (fgSizer6, 0, wxEXPAND, 5);
   sbSizer30->Add (fgSizer6, wxSizerFlags(0).Expand());

	bSizer150->Add (sbSizer30, wxSizerFlags(0).Border (wxLEFT|wxTOP|wxRIGHT, 12));

   wxBoxSizer* bSizer153;
   bSizer153 = new wxBoxSizer (wxHORIZONTAL);

   wxStaticBoxSizer* sbSizer31;
   sbSizer31 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("Determine clusters from"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxVERTICAL);

   wxBoxSizer* bSizer157;
   bSizer157 = new wxBoxSizer (wxHORIZONTAL);

   m_radioBtn12 = new wxRadioButton (sbSizer31->GetStaticBox (),
													IDC_ClusterTrainingAreas,
													wxT("Training Area(s)"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn12, IDS_ToolTip94);
   //bSizer157->Add (m_radioBtn12, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
   bSizer157->Add (m_radioBtn12,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));


   //bSizer157->Add (40, 0, 0, wxEXPAND, 5);
   bSizer157->Add (40, 0, 0, wxEXPAND);

   m_staticText204 = new wxStaticText (sbSizer31->GetStaticBox (),
													IDC_ClassPrompt,
													wxT("Classes:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText204->Wrap (-1);
   SetUpToolTip (m_staticText204, IDS_ToolTip103); 
   //bSizer157->Add (m_staticText204, 0, wxALIGN_CENTER | wxALL, 5);
   bSizer157->Add (m_staticText204,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_choice22 = new wxComboBox (sbSizer31->GetStaticBox (),
											IDC_ClassCombo,
											wxT("All"),
											wxDefaultPosition,
											wxDefaultSize,
											0,
											NULL,
											wxCB_READONLY );
	m_choice22->Append ("All");
	m_choice22->Append ("Subset...");
   m_choice22->SetSelection (0);
   SetUpToolTip (m_choice22, IDS_ToolTip103);  
   //bSizer157->Add (m_choice22, 0, wxALL, 5);
   bSizer157->Add (m_choice22, wxSizerFlags(0).Border(wxALL, 5));

   //sbSizer31->Add (bSizer157, 0, wxEXPAND, 5);
   sbSizer31->Add (bSizer157, wxSizerFlags(0).Expand());

   m_radioBtn13 = new wxRadioButton (sbSizer31->GetStaticBox (),
													IDC_ClusterImageArea,
													wxT("Image Area"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_radioBtn13, IDS_ToolTip95);
   //sbSizer31->Add (m_radioBtn13, 0, wxALL, 5);
   sbSizer31->Add (m_radioBtn13, wxSizerFlags(0).Border(wxALL, 5));

   wxStaticBoxSizer* sbSizer8;
   sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (sbSizer31->GetStaticBox (),
																		IDC_LineColFrame,
																		wxT("Area to Cluster"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxHORIZONTAL);
	/*
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);

   m_bpButton4 = new wxBitmapButton (sbSizer8->GetStaticBox (),
													IDEntireImage,
													entireimi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   m_bpButton4->SetBitmapDisabled (toentirei);
   bSizer791->Add (m_bpButton4,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_bpButton5 = new wxBitmapButton (sbSizer8->GetStaticBox (),
   												IDSelectedImage,
   												selectedi,
   												wxDefaultPosition,
   												wxDefaultSize,
   												wxBU_AUTODRAW);
   m_bpButton5->Hide ();
   m_bpButton5->SetBitmapDisabled (bmp4i);
	bSizer791->Add (m_bpButton5,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   sbSizer8->Add (bSizer791, wxSizerFlags(0).Expand());
	*/
	CreateLineColumnControls (sbSizer8);
	
 	sbSizer31->Add (sbSizer8, wxSizerFlags(1).Expand().Border (wxALL, 6));

 	bSizer153->Add (sbSizer31, wxSizerFlags(0).Expand().Border (wxTOP|wxRIGHT,10));
	/*
   wxBoxSizer* bSizer54;
   bSizer54 = new wxBoxSizer (wxVERTICAL);

   bSizer54->Add (0, 0, 1, wxEXPAND, 5);

   m_button17 = new wxButton (this,
										wxID_CANCEL,
										wxT("Cancel"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer54->Add (m_button17, wxSizerFlags(0).Right().Border (wxBOTTOM, 6));

   m_button18 = new wxButton (this,
										wxID_OK,
										wxT("OK"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer54->Add (m_button18, wxSizerFlags(0).Right());

 	bSizer153->Add (bSizer54, wxSizerFlags(0).Expand());
	*/
	bSizer150->Add (bSizer153,
						wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	//wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	//bSizer150->Add (standardButtonSizer, wxSizerFlags(0).Right());
	CreateStandardButtons (bSizer150);

   this->SetSizerAndFit (bSizer150);
   this->Layout ();
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the cluster
//							specification dialog box to the user and copy the
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
//	Coded By:			Abdur Rachman Maud	Date: 08/02/2009
//	Revised By:			Larry L. Biehl			Date: 08/02/2016

SInt16 CMSinglePassClusterDialog::DoDialog (void)

{
   SInt16								clustersFrom,
											returnCode;
	
	Boolean								continueFlag = FALSE;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																							return (FALSE);

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
		{
      continueFlag = TRUE;

      clustersFrom = kTrainingType;
      if (m_clustersFrom == 1)
         clustersFrom = kAreaType;

				// Cluster area

      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = m_LineInterval;

      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = m_ColumnInterval;

      OnePassClusterDialogOK ((SInt16)m_minClusterSize,
										  m_criticalDistance1,
										  m_criticalDistance2,
										  clustersFrom,
										  (SInt16)m_classSelection,
										  m_classListPtr,
										  m_localNumberClasses,
										  &m_dialogSelectArea);
 
		}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"


/*
void CMSinglePassClusterDialog::OnButtonPress (
				wxKeyEvent&							event)
{
   if (event.GetKeyCode () == WXK_RETURN)
		{
		if (Validate () && TransferDataFromWindow ())
			EndModal (wxID_OK);
 
		} 
		
	else 
		{
		event.Skip ();
 
		}
 
}	// end "OnButtonPress"
*/


void CMSinglePassClusterDialog::OnClusterImageArea (
				wxCommandEvent&					event)

{
   //HideShowAreaItems (kShow);

   OnePassClusterDialogOnImageArea (this);

   SetEntireImageButtons (NULL,
								  m_LineStart,
								  m_LineEnd,
								  m_ColumnStart,
								  m_ColumnEnd);

}	// end "OnClusterTrainingAreas"



void CMSinglePassClusterDialog::OnClusterTrainingAreas (
				wxCommandEvent&					event)

{
   OnePassClusterDialogOnTrainingAreas (this);
	
	HideDialogItem (this, IDEntireImage);
	HideDialogItem (this, IDSelectedImage);

}	// end "OnClusterTrainingAreas"



void CMSinglePassClusterDialog::OnInitDialog (
				wxInitDialogEvent&				event)

 {
   SInt16								classSelection,
											clustersFrom;
 
   UInt16								distanceDecDigits;
	

   //CMDialog::OnInitDialog ();

   OnePassClusterDialogInitialize (this,
											  m_classListPtr,
											  &m_minClusterSize,
											  &m_criticalDistance1,
											  &m_criticalDistance2,
											  &distanceDecDigits,
											  &clustersFrom,
											  &classSelection,
											  &m_localNumberClasses,
											  &m_dialogSelectArea);

			// Adjust those options that need to be.
			// Cluster areas from training areas or a selected area.

   m_clustersFrom = 0;
   if (clustersFrom == kAreaType)
      m_clustersFrom = 1;

			// All class training areas or a subset.

   m_classSelection = classSelection;

			// Selected area to cluster.

   m_LineStart = m_dialogSelectArea.lineStart;
   m_LineEnd = m_dialogSelectArea.lineEnd;
   m_LineInterval = m_dialogSelectArea.lineInterval;
   m_ColumnStart = m_dialogSelectArea.columnStart;
   m_ColumnEnd = m_dialogSelectArea.columnEnd;
   m_ColumnInterval = m_dialogSelectArea.columnInterval;

   if (TransferDataToWindow ())
      PositionDialogWindow ();

			// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_MinClusterSize, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMSinglePassClusterDialog::OnSelendokClassCombo (
				wxCommandEvent&					event)

{
   HandleClassesMenu (&m_localNumberClasses,
							  (SInt16*)m_classListPtr,
							  1,
							  (SInt16)gProjectInfoPtr->numberStatisticsClasses,
							  IDC_ClassCombo,
							  &m_classSelection);

}	// end "OnSelendokClassCombo"



bool CMSinglePassClusterDialog::TransferDataFromWindow ()

{
	SInt16								continueCode,
											returnCode = 0;
	
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxTextCtrl* dist1 = (wxTextCtrl*)FindWindow (IDC_CriticalDistance1);
   wxTextCtrl* dist2 = (wxTextCtrl*)FindWindow (IDC_CriticalDistance2);
   wxTextCtrl* minclustersz = (wxTextCtrl*)FindWindow (IDC_MinClusterSize);
   
   m_LineStartString = l_start->GetValue ();
   m_LineEndString = l_end->GetValue ();
   m_LineIntervalString = l_inter->GetValue ();
   m_ColumnStartString = c_start->GetValue ();
   m_ColumnEndString = c_end->GetValue ();
   m_ColumnIntervalString = c_inter->GetValue ();
	
   m_LineInterval = wxAtoi (m_LineIntervalString);
   m_ColumnInterval = wxAtoi (m_ColumnIntervalString);
   wxString crdist1 = (dist1->GetValue ());
   crdist1.ToDouble (&m_criticalDistance1);
   wxString crdist2 = (dist2->GetValue ());
   crdist2.ToDouble (&m_criticalDistance2);
   m_minClusterSize = wxAtoi (minclustersz->GetValue ());
   
   //m_convergence = convergence->GetValue ();
      
   ClustersFromSetting (true, IDC_ClusterTrainingAreas, m_clustersFrom);
	
	continueCode = CheckMaxValue (this,
											IDC_MinClusterSize,
											1,
											SInt32_MAX,
											kDisplayRangeAlert);
	
	if (continueCode != 1)
		returnCode = IDC_MinClusterSize;
						
	if (returnCode == 0)						
		returnCode = CheckDialogRealValue (this,
														IDC_CriticalDistance1,
														0,
														UInt32_MAX,
														0,
														false,	// minValue not allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
						
	if (returnCode == 0)						
		returnCode = CheckDialogRealValue (this,
														IDC_CriticalDistance2,
														0,
														UInt32_MAX,
														0,
														false,	// minValue not allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);

	if (returnCode == 0 && m_clustersFrom == 1)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
														  IDC_ColumnStart,
														  true);
	
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMSinglePassClusterDialog::TransferDataToWindow ()

{
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxTextCtrl* dist1 = (wxTextCtrl*)FindWindow (IDC_CriticalDistance1);
   wxTextCtrl* dist2 = (wxTextCtrl*)FindWindow (IDC_CriticalDistance2);
   wxTextCtrl* minclustersz = (wxTextCtrl*)FindWindow (IDC_MinClusterSize);

   c_end->ChangeValue (wxString::Format (wxT("%i"), m_ColumnEnd));
   c_inter->ChangeValue (wxString::Format (wxT("%i"), m_ColumnInterval));
   c_start->ChangeValue (wxString::Format (wxT("%i"), m_ColumnStart));
   l_end->ChangeValue (wxString::Format (wxT("%i"), m_LineEnd));
   l_inter->ChangeValue (wxString::Format (wxT("%i"), m_LineInterval));
   l_start->ChangeValue (wxString::Format (wxT("%i"), m_LineStart));
   dist1->ChangeValue (wxString::Format (wxT("%.1f"), m_criticalDistance1));
   dist2->ChangeValue (wxString::Format (wxT("%.1f"), m_criticalDistance2));
   minclustersz->ChangeValue (wxString::Format (wxT("%i"), m_minClusterSize));
         
   ClustersFromSetting (false, IDC_ClusterTrainingAreas, m_clustersFrom);
	
   if (m_clustersFrom == 1)
		SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);
	
   else	// m_clustersFrom != 1
		{
      HideDialogItem (this, IDEntireImage);
      HideDialogItem (this, IDSelectedImage);
		
		}	// end "else m_clustersFrom != 1"
				
   return true;
	
}	// end "TransferDataToWindow"
