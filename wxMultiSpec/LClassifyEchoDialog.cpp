//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyEchoDialog.cpp : class implementation file
//	Class Definition:		LClassifyEchoDialog.h
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
//								CMEchoClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"                      
#include "LClassifyEchoDialog.h"

CMEchoClassifyDialog::CMEchoClassifyDialog (
				wxWindow*							parent, 
				wxWindowID							id, 
				const wxString&					title)
		: CMDialog (CMEchoClassifyDialog::IDD, parent, title) 

{
   m_cellWidth = 2;
   m_annexationThreshold = 0;
   m_homogeneityThreshold = 0;
   m_combineLikeFieldsFlag = FALSE;
   m_createHomogeneousFilesFlag = FALSE;
   m_mixCellsFlag = FALSE;
   m_homogeneousThresholdType = 0;
   m_echoAlgorithmProcedure = -1;
   
   m_minPhase1 = 0.;
   m_maxPhase1 = 100.;

   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls ();
   SetSizerAndFit (bSizer148);
   
}	// end "CMEchoClassifyDialog"



CMEchoClassifyDialog::~CMEchoClassifyDialog (void) 

{

}	// end "~CMEchoClassifyDialog"



BEGIN_EVENT_TABLE (CMEchoClassifyDialog, CMDialog)
	EVT_INIT_DIALOG (CMEchoClassifyDialog::OnInitDialog)
	EVT_RADIOBUTTON (IDC_Percent, CMEchoClassifyDialog::OnPercent)
	EVT_RADIOBUTTON (IDC_LogLike, CMEchoClassifyDialog::OnLogLike)
	//EVT_CHAR_HOOK(CMEchoClassifyDialog::OnButtonPress)
END_EVENT_TABLE ()



void CMEchoClassifyDialog::CreateControls () 

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	//wxBoxSizer* bSizer148;
	bSizer148 = new wxBoxSizer (wxVERTICAL);
	
	wxBoxSizer* bSizer149;
	bSizer149 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText171 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Spectral-spatial Algorithm:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText171->Wrap (-1);
   SetUpToolTip (m_staticText171, IDS_ToolTip307);
	bSizer149->Add (m_staticText171, 0, wxALIGN_CENTER, 5);
	
	m_comboBox25 = new wxComboBox (this, 
												IDC_EchoAlgorithm, 
												wxT("Combo!"), 
												wxDefaultPosition, 
												wxSize (180,-1), 
												0, 
												NULL, 
												0);
	m_comboBox25->Append (wxT("Quadratic Likelihood"));
	m_comboBox25->Append (wxT("Mahalanobis"));
	m_comboBox25->Append (wxT("Fisher Linear"));
   SetUpToolTip (m_comboBox25, IDS_ToolTip307);
	bSizer149->Add (m_comboBox25, 0, wxALIGN_CENTER|wxLEFT, 5);
	
	
	bSizer148->Add (bSizer149, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer150;
	bSizer150 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText172 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Cell size (pixels square):"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText172->Wrap (-1);
   SetUpToolTip (m_staticText172, IDS_ToolTip308);
	bSizer150->Add (m_staticText172, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl72 = new wxTextCtrl (this, 
												IDC_CellWidth, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxSize (50,-1), 
												0);
   m_textCtrl72->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_cellWidthString));
   SetUpToolTip (m_textCtrl72, IDS_ToolTip308);
	bSizer150->Add (m_textCtrl72, 0, wxALIGN_CENTER|wxLEFT, 5);
	
	
	bSizer148->Add (bSizer150, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText173 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Homogeneous Cell Threshold:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText173->Wrap (-1);
   SetUpToolTip (m_staticText173, IDS_ToolTip309);
	bSizer151->Add (m_staticText173, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl73 = new wxTextCtrl (this, 
												IDC_HomogeneityThreshold, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   m_textCtrl73->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_homoThresString));
   SetUpToolTip (m_textCtrl73, IDS_ToolTip309);
	bSizer151->Add (m_textCtrl73, 0, wxALIGN_CENTER|wxLEFT, 5);
		
	bSizer148->Add (bSizer151, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer152;
	bSizer152 = new wxBoxSizer (wxVERTICAL);
	
	m_radioBtn13 = new wxRadioButton (this, 
													IDC_Percent, 
													wxT("Percent"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	SetUpToolTip (m_radioBtn13, IDS_ToolTip310);
   bSizer152->Add (m_radioBtn13, 0, wxTOP, 5);
	
	m_radioBtn14 = new wxRadioButton (this, 
													IDC_LogLike, 
													wxT("-Cell Log Likelihood"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
   SetUpToolTip (m_radioBtn14, IDS_ToolTip311);
	bSizer152->Add (m_radioBtn14, 0, wxTOP, 5);
	
	
	bSizer148->Add (bSizer152, 0, wxEXPAND|wxLEFT, 25);
	
	wxBoxSizer* bSizer153;
	bSizer153 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText174 = new wxStaticText (this, 
													wxID_ANY, 
													wxT("Annexation Threshold:"), 
													wxDefaultPosition, 
													wxDefaultSize, 
													0);
	m_staticText174->Wrap (-1);
   SetUpToolTip (m_staticText174, IDS_ToolTip312);
	bSizer153->Add (m_staticText174, 0, wxALIGN_CENTER, 5);
	
	m_textCtrl74 = new wxTextCtrl (this, 
												IDC_AnnexationThreshold, 
												wxEmptyString, 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   m_textCtrl74->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_annexThresString));
   SetUpToolTip (m_textCtrl74, IDS_ToolTip312);
	bSizer153->Add (m_textCtrl74, 0, wxALIGN_CENTER|wxLEFT, 5);
	
	
	bSizer148->Add (bSizer153, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12);
	
	wxBoxSizer* bSizer154;
	bSizer154 = new wxBoxSizer (wxVERTICAL);
	
	m_checkBox25 = new wxCheckBox (this, 
												IDC_CombineLikeFields, 
												wxT("Merge Like Fields"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox25, IDS_ToolTip313);
	bSizer154->Add (m_checkBox25, 0, wxTOP, 5);
	
	m_checkBox26 = new wxCheckBox (
										this, 
										IDC_CreateHomogeneousFiles, 
										wxT("Create Files for Homogeneous Fields and Classes"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
   SetUpToolTip (m_checkBox26, IDS_ToolTip314);
	bSizer154->Add (m_checkBox26, 0, wxTOP, 5);
	
	m_checkBox27 = new wxCheckBox (this, 
												IDC_MixCells, 
												wxT("Allow Merging of Unlike Cells and Fields"), 
												wxDefaultPosition, 
												wxDefaultSize, 
												0);
   SetUpToolTip (m_checkBox27, IDS_ToolTip315);
	bSizer154->Add (m_checkBox27, 0, wxTOP, 5);
	
	bSizer148->Add (bSizer154, 0, wxEXPAND|wxLEFT|wxRIGHT, 12);
	/*
	wxBoxSizer* bSizer155;
	bSizer155 = new wxBoxSizer (wxHORIZONTAL);
	
	m_button33 = new wxButton (this, 
										wxID_CANCEL, 
										wxT("Cancel"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer155->Add (m_button33, 0, wxALL, 5);
	
	m_button34 = new wxButton (this, 
										wxID_OK, 
										wxT("OK"), 
										wxDefaultPosition, 
										wxDefaultSize, 
										0);
	bSizer155->Add (m_button34, 0, wxALL, 5);
	
	bSizer148->Add (bSizer155, 0, wxALIGN_RIGHT|wxBOTTOM|wxLEFT|wxRIGHT, 12);
	*/
	CreateStandardButtons (bSizer148);
	
	this->SetSizer (bSizer148);
	this->Layout ();
	bSizer148->Fit (this);
	
	this->Centre (wxBOTH);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (2009-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the echo classify
//							specification dialog box to the user and copy the
//							revised back to the echo specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 04/18/2009
//	Revised By:			Larry L. Biehl			Date: 04/08/2009	

SInt16 CMEchoClassifyDialog::DoDialog (
				EchoClassifierVarPtr				echoClassifierVarPtr)

{
   Boolean continueFlag = FALSE;
   SInt16 returnCode;

			// Make sure intialization has been completed.
			
   if (!m_initializedFlag)
																							return (FALSE);

   m_echoClassifierVarPtr = echoClassifierVarPtr;

   returnCode = ShowModal ();

   if (returnCode == wxID_OK) 
		{
      continueFlag = TRUE;

				// Cell Width.													

      m_echoClassifierVarPtr->cell_width = (SInt16) m_cellWidth;

      m_echoClassifierVarPtr->cell_size =
         m_echoClassifierVarPtr->cell_width * m_echoClassifierVarPtr->cell_width;

				// Homogeneity (phase 1) threshold.						

      m_echoClassifierVarPtr->homogeneityThreshold = m_homogeneityThreshold;

				// Homogeniety (phase 1) Threshold specified in percent probability 
				// correct.										

      m_echoClassifierVarPtr->fixed_homogeneityThreshold_option =
																			m_homogeneousThresholdType;

				// Annexation threshold.									

      m_echoClassifierVarPtr->annexationThreshold = m_annexationThreshold;

				// Flag indicating whether to merge like fields.	

      m_echoClassifierVarPtr->combineLikeFieldsFlag =
         m_combineLikeFieldsFlag;

				// Flag indicating whether to create image files with the homogeneous 
				// fields and classes.									

      m_echoClassifierVarPtr->createHomogeneousFilesFlag =
																			m_createHomogeneousFilesFlag;

				// Flag indicating whether to allow merging of cells and fields which 
				// are of different classes.			

      m_echoClassifierVarPtr->mixCellsFlag = m_mixCellsFlag;

				// Set classification algorithm to use for echo. 

      m_echoClassifierVarPtr->algorithmCode = m_echoAlgorithmProcedure + 1;

		}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"


/*
void CMEchoClassifyDialog::OnButtonPress (
				wxKeyEvent&							event) 

{
	if (event.GetKeyCode () == WXK_RETURN) 
		{
		if (TransferDataFromWindow ())
			EndModal (wxID_OK);
		} 
		
	else 
		{
		event.Skip ();
		}
}	// end "OnButtonPress"
*/


void CMEchoClassifyDialog::OnInitDialog (
				wxInitDialogEvent&				event) 

{
   CMDialog::OnInitDialog (event);	
	
			// Cell Width.							

   m_cellWidth = m_echoClassifierVarPtr->cell_width;
	
			// Homogeniety Threshold (phase 1).
			// Threshold specified in percent probability correct. (> 0.0 && < 100.0)
			// or in -Log Likelihood. (> 0.0 && < 1000000.0)													

   m_homogeneityThreshold = m_echoClassifierVarPtr->homogeneityThreshold;

   m_homogeneousThresholdType = 0;
   if (m_echoClassifierVarPtr->fixed_homogeneityThreshold_option)
      m_homogeneousThresholdType = 1;

   m_minPhase1 = 0;
   m_savedPhase1PercentThreshold = 2.;
   m_savedPhase1LogLikelihoodThreshold = 95.;

   if (m_homogeneousThresholdType == 1) 
		{
      m_maxPhase1 = 1000000.;
      m_savedPhase1LogLikelihoodThreshold = m_homogeneityThreshold;

		}	// end "if (m_homogeneousThresholdType == 1)" 

   else	// m_homogeneousThresholdType == 0
		{
      m_maxPhase1 = 100.;
      m_savedPhase1PercentThreshold = m_homogeneityThreshold;

		}	// end "else m_homogeneousThresholdType == 0"

			// Annexation Threshold. (>= 0.0 && < 10.0)						

   m_annexationThreshold = m_echoClassifierVarPtr->annexationThreshold;

			// Flag indicating whether to merge like fields.				

   m_combineLikeFieldsFlag = m_echoClassifierVarPtr->combineLikeFieldsFlag;

			// Flag indicating whether to create image files with the homogeneous 
			// fields and classes.												

   m_createHomogeneousFilesFlag =
											m_echoClassifierVarPtr->createHomogeneousFilesFlag;

			// Flag indicating whether to allow merging of cells and fields which are of 
			// different classes.										

   m_mixCellsFlag = m_echoClassifierVarPtr->mixCellsFlag;
   MHideDialogItem (this, IDC_MixCells);

			// Classification procedure

   m_echoAlgorithmProcedure = m_echoClassifierVarPtr->algorithmCode - 1;

   if (TransferDataToWindow ())
      PositionDialogWindow ();

   SelectDialogItemText (this, IDC_CellWidth, 0, SInt16_MAX);

}	// end "OnInitDialog"



void CMEchoClassifyDialog::OnLogLike (
				wxCommandEvent&					event) 

{
   if (m_homogeneousThresholdType == 0) 
		{
      wxRadioButton* homogenRad = (wxRadioButton*)FindWindow (IDC_Percent);
      if (homogenRad->GetValue () == true)
         m_homogeneousThresholdType = 0;
      else
         m_homogeneousThresholdType = 1;
      
      wxTextCtrl* homogenthres = (wxTextCtrl*)FindWindow (IDC_HomogeneityThreshold);
      wxString homogenthresString = homogenthres->GetValue ();
      homogenthresString.ToDouble (&m_homogeneityThreshold);

      m_maxPhase1 = 1000000.;
      m_savedPhase1PercentThreshold = m_homogeneityThreshold;
      m_homogeneityThreshold = m_savedPhase1LogLikelihoodThreshold;

      homogenthres->ChangeValue (wxString::Format (wxT("%.1f"), 
											m_homogeneityThreshold));
		
		}	// end "if (m_homogeneousThresholdType == 0)"                                                         

}	// end "OnLogLike"



void CMEchoClassifyDialog::OnPercent (
				wxCommandEvent&					event)
				
{
   if (m_homogeneousThresholdType == 1) 
		{
      wxRadioButton* homogenRad = (wxRadioButton*)FindWindow (IDC_Percent);
      if (homogenRad->GetValue () == true)
         m_homogeneousThresholdType = 0;
       else
         m_homogeneousThresholdType = 1;

      wxTextCtrl* homogenthres = (wxTextCtrl*)FindWindow (IDC_HomogeneityThreshold);
      wxString homogenthresString = homogenthres->GetValue ();
      homogenthresString.ToDouble (&m_homogeneityThreshold);

      m_maxPhase1 = 100.;
      m_savedPhase1LogLikelihoodThreshold = m_homogeneityThreshold;
      m_homogeneityThreshold = m_savedPhase1PercentThreshold;

      homogenthres->ChangeValue (wxString::Format (wxT("%.1f"), 
											m_homogeneityThreshold));

   }	// end "if (m_homogeneousThresholdType == 1)"                                                         

}	// end "OnPercent"



bool CMEchoClassifyDialog::TransferDataFromWindow () 

{
   SInt16 returnCode = 0;
	
	
   wxTextCtrl* cellwidth = (wxTextCtrl*)FindWindow (IDC_CellWidth);
   wxTextCtrl* annexthres = (wxTextCtrl*)FindWindow (IDC_AnnexationThreshold);
   wxTextCtrl* homogenthres = (wxTextCtrl*)FindWindow (IDC_HomogeneityThreshold);
   wxCheckBox* combinefield = (wxCheckBox*)FindWindow (IDC_CombineLikeFields);
   wxCheckBox* createfile = (wxCheckBox*)FindWindow (IDC_CreateHomogeneousFiles);
   wxCheckBox* mixcell = (wxCheckBox*)FindWindow (IDC_MixCells);
   wxComboBox* echoalg = (wxComboBox*)FindWindow (IDC_EchoAlgorithm);
   wxRadioButton* percent = (wxRadioButton*)FindWindow (IDC_Percent);

   wxString cellWidthString = cellwidth->GetValue ();
   cellWidthString.ToLong (&m_cellWidth);
   wxString annexthresString = annexthres->GetValue ();
   annexthresString.ToDouble (&m_annexationThreshold);
   wxString homogenthresString = homogenthres->GetValue ();
   homogenthresString.ToDouble (&m_homogeneityThreshold);
   
   m_combineLikeFieldsFlag = combinefield->GetValue ();
   m_createHomogeneousFilesFlag = createfile->GetValue ();
   m_mixCellsFlag = mixcell->GetValue ();
   m_echoAlgorithmProcedure = echoalg->GetSelection ();
	
   if (percent->GetValue () == true)
      m_homogeneousThresholdType = 0;
   else
      m_homogeneousThresholdType = 1;
   
   returnCode = CheckDialogRealValue (this,
													IDC_CellWidth,
													1,
													100,
													0,
													true,	// minValue  allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   if (returnCode == 0)
      returnCode = CheckDialogRealValue (this,
														IDC_AnnexationThreshold,
														0.,
														100.,
														0,
														false,	// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
   
   
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMEchoClassifyDialog::TransferDataToWindow () 

{
   wxTextCtrl* cellwidth = (wxTextCtrl*)FindWindow (IDC_CellWidth);
   wxTextCtrl* annexthres = (wxTextCtrl*)FindWindow (IDC_AnnexationThreshold);
   wxTextCtrl* homogenthres = (wxTextCtrl*)FindWindow (IDC_HomogeneityThreshold);
   wxCheckBox* combinefield = (wxCheckBox*)FindWindow (IDC_CombineLikeFields);
   wxCheckBox* createfile = (wxCheckBox*)FindWindow (IDC_CreateHomogeneousFiles);
   wxCheckBox* mixcell = (wxCheckBox*)FindWindow (IDC_MixCells);
   wxComboBox* echoalg = (wxComboBox*)FindWindow (IDC_EchoAlgorithm);
   wxRadioButton* percent = (wxRadioButton*)FindWindow (IDC_Percent);
   wxRadioButton* loglike = (wxRadioButton*)FindWindow (IDC_LogLike);

   cellwidth->ChangeValue (wxString::Format (wxT("%ld"), m_cellWidth));
   annexthres->ChangeValue (wxString::Format (wxT("%.1f"), m_annexationThreshold));
   homogenthres->ChangeValue (wxString::Format (wxT("%.1f"), m_homogeneityThreshold));

   combinefield->SetValue (m_combineLikeFieldsFlag);
   createfile->SetValue (m_createHomogeneousFilesFlag);
   mixcell->SetValue (m_mixCellsFlag);

   echoalg->SetSelection (m_echoAlgorithmProcedure);
	
   if (m_homogeneousThresholdType == 0)
      percent->SetValue (true);
   else
      loglike->SetValue (true);

   return true;
	
}	// end "TransferDataToWindow"
