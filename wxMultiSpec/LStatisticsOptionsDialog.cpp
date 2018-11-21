// LStatisticsOptionsDialog.cpp : implementation file
//
// Revised by Larry Biehl on 11/07/2018
//               
/* Template for debugging for MultiSpec Online on mygeohub.org.
	int numberChars = sprintf ((char*)&gTextString3,
								" LStatisticsOptionsDialog::xxx (countBytes, errCode) %d, %d%s", 
								countBytes,
								errCode,
								gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/
#include "SMultiSpec.h"

#include	"LChannelsDialog.h"
#include "LImageView.h"
#include "LStatisticsOptionsDialog.h"


extern Boolean StatisticsOptionsDialog (
				SInt16*								statCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				Boolean*								useModifiedStatsFlagPtr);

extern void StatisticsOptionsDialogOK (
				SInt16								localStatCode,
				Boolean								localKeepStatsFlag,
				Boolean								localZeroVarianceFlag,
				double								localVariance,
				double								localMinLogDeterminant,
				Boolean								localCommonCovarianceInLOOCFlag,
				SInt16*								statCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				Boolean*								useCommonCovarianceInLOOCFlagPtr);



CMStatOptionsDlg::CMStatOptionsDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMStatOptionsDlg::IDD, pParent, title) 
{
   m_classStatsOnlyFlag = FALSE;
   m_setZeroVarianceFlag = FALSE;
   m_statCode = -1;
   m_minLogDetValue = 0;
   m_varianceValue = 0;
   m_useCommonCovarianceInLOOCFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls();
}


BEGIN_EVENT_TABLE(CMStatOptionsDlg, CMDialog)
EVT_INIT_DIALOG(CMStatOptionsDlg::OnInitDialog)
EVT_RADIOBUTTON(IDC_MeanStd, CMStatOptionsDlg::OnMeanStd)
EVT_RADIOBUTTON(IDC_meanStdCov, CMStatOptionsDlg::OnMeanStdCov)
EVT_CHECKBOX(IDC_SetZeroVariance, CMStatOptionsDlg::OnSetZeroVariance)
END_EVENT_TABLE()


void CMStatOptionsDlg::CreateControls() 
{
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);

   bSizer181 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer182;
   bSizer182 = new wxBoxSizer(wxVERTICAL);

   m_staticText193 = new wxStaticText (this,
   												wxID_ANY,
   												wxT("Compute statistics for:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText193->Wrap(-1);
   bSizer182->Add(m_staticText193, 0, wxALL, 5);

   m_radioBtn17 = new wxRadioButton (this,
   												IDC_MeanStd,
   												wxT("Mean and standard deviation"),
   												wxDefaultPosition,
   												//wxDefaultSize,
   												wxSize (-1, 24),
   												0);
   SetUpToolTip (m_radioBtn17, IDS_ToolTip115);
   bSizer182->Add (m_radioBtn17, 0, wxLEFT, 20);

   m_radioBtn18 = new wxRadioButton (this,
   												IDC_meanStdCov,
   												wxT("Mean, standard deviation and covariance"),
   												wxDefaultPosition,
   												//wxDefaultSize,
   												wxSize (-1, 24),
   												0);
   SetUpToolTip (m_radioBtn18, IDS_ToolTip116);
   bSizer182->Add (m_radioBtn18, 0, wxLEFT, 20);


   bSizer181->Add (bSizer182, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 12);

   wxBoxSizer* bSizer183;
   bSizer183 = new wxBoxSizer (wxVERTICAL);

   m_checkBox33 = new wxCheckBox (this,
   											IDC_ClassStatsOnly,
   											wxT("Keep only class statistics in memory"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox33, IDS_ToolTip117);
   bSizer183->Add (m_checkBox33, 0, wxALL, 5);


   bSizer181->Add (bSizer183, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

   wxBoxSizer* bSizer184;
   bSizer184 = new wxBoxSizer (wxHORIZONTAL);

   m_checkBox35 = new wxCheckBox (this,
   											IDC_SetZeroVariance,
   											wxT("Set zero variance to:"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox35, IDS_ToolTip118);
   bSizer184->Add (m_checkBox35, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl92 = new wxTextCtrl (this, IDC_Variance, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_textCtrl92->SetValidator (wxTextValidator(wxFILTER_NUMERIC, &m_varienceString));
   bSizer184->Add (m_textCtrl92, 0, wxALL, 5);

   bSizer181->Add(bSizer184, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

   wxBoxSizer* bSizer185;
   bSizer185 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText195 = new wxStaticText(this, IDC_MinLogPrompt, wxT("Minimum log determinant \noffset allowed for valid \nmatrix inversion:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText195->Wrap(-1);
   bSizer185->Add(m_staticText195, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl93 = new wxTextCtrl(this, IDC_minLogDetValue, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_textCtrl93->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_minLogString));
   SetUpToolTip(m_textCtrl93, IDS_ToolTip119);
   bSizer185->Add(m_textCtrl93, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer181->Add(bSizer185, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

   wxBoxSizer* bSizer186;
   bSizer186 = new wxBoxSizer(wxVERTICAL);

   m_checkBox34 = new wxCheckBox(this, IDC_UseCommonCov, wxT("Use common covariance in leave-one-out \ncovariance estimations"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox34, IDS_ToolTip120);
   bSizer186->Add(m_checkBox34, 0, wxALL, 5);

   bSizer181->Add(bSizer186, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);
	/*
   wxBoxSizer* bSizer187;
   bSizer187 = new wxBoxSizer(wxHORIZONTAL);

   m_button42 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer187->Add(m_button42, 0, wxALL, 5);

   m_button43 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer187->Add(m_button43, 0, wxALL, 5);

   bSizer181->Add(bSizer187, 0, wxALIGN_RIGHT | wxALL, 12);
	*/
	//wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	//bSizer181->Add (standardButtonSizer, wxSizerFlags(0).Right());
	CreateStandardButtons (bSizer181);

   this->SetSizer(bSizer181);
	
   SetSizerAndFit (bSizer181);
   //this->Layout();
	
   this->Centre(wxBOTH);
	
}	// end "CreateControls"


//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/13/1996
//	Revised By:			Larry L. Biehl			Date: 07/07/2017	

SInt16 CMStatOptionsDlg::DoDialog (
				SInt16*								statCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				Boolean*								useCommonCovarianceInLOOCFlagPtr) 
{
   Boolean continueFlag = FALSE;

   SInt16 returnCode;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																					return (FALSE);

			// Statistics to be computed.														

   m_statCode = *statCodePtr - 1;

			// Keep only class statistics in memory.										

   m_classStatsOnlyFlag = *keepClassStatsFlagPtr;

   m_setZeroVarianceFlag = (*variancePtr > 0);

   m_varianceValue = fabs(*variancePtr);

			// Minimum log determinant allowed for valid matrix inversion.

   m_minLogDetValue = *minLogDeterminantPtr;

   m_useCommonCovarianceInLOOCFlag = *useCommonCovarianceInLOOCFlagPtr;

   returnCode = ShowModal();

   if (returnCode == wxID_OK) 
		{
      StatisticsOptionsDialogOK (m_statCode + 1,
											m_classStatsOnlyFlag,
											m_setZeroVarianceFlag,
											m_varianceValue,
											m_minLogDetValue,
											m_useCommonCovarianceInLOOCFlag,
											statCodePtr,
											keepClassStatsFlagPtr,
											variancePtr,
											minLogDeterminantPtr,
											useCommonCovarianceInLOOCFlagPtr);

      continueFlag = TRUE;

   }	// end "if (returnCode == IDOK)" 

   return (continueFlag);

}	// end "DoDialog"


void CMStatOptionsDlg::OnInitDialog(wxInitDialogEvent& event) 
{
   if (m_statCode + 1 == kMeanCovariance) 
		{
      SetDLogControl(this, IDC_SetZeroVariance, m_setZeroVarianceFlag);

      ShowHideDialogItem(this, IDC_Variance, m_setZeroVarianceFlag);

      ShowDialogItem(this, IDC_MinLogPrompt);
      ShowDialogItem(this, IDC_minLogDetValue);

		}	// end "if (m_statCode == kMeanCovariance)" 

   else // m_statCode+1 == kMeanStdOnly 
		{
      SetDLogControl (this, IDC_SetZeroVariance, 0);
      SetDLogControlHilite (this, IDC_SetZeroVariance, 255);
      HideDialogItem (this, IDC_Variance);
      HideDialogItem (this, IDC_MinLogPrompt);
      HideDialogItem (this, IDC_minLogDetValue);

      SetDLogControl (this, IDC_UseCommonCov, 0);
      SetDLogControlHilite (this, IDC_UseCommonCov, 255);

		}	// end "else m_statCode+1 == kMeanStdOnly"

			// Center the dialog and then show it.	

			// Update the dialog box parameters and then center the dialog.

   if (TransferDataToWindow())
      PositionDialogWindow();

   SInt16 selectedItem = IDC_minLogDetValue;
   if (m_statCode + 1 == kMeanCovariance && m_setZeroVarianceFlag)
      selectedItem = IDC_Variance;

			// Set default text selection to first edit text item 

   SelectDialogItemText(this, selectedItem, 0, SInt16_MAX);

} // end "OnInitDialog"


void CMStatOptionsDlg::OnMeanStd (wxCommandEvent& event) 
{
   if (ProjectMenuClearStatistics()) 
		{
      if (m_statCode + 1 == kMeanCovariance) 
			{
         SetDLogControl (this, IDC_SetZeroVariance, 0);
         SetDLogControlHilite (this, IDC_SetZeroVariance, 255);
         HideDialogItem (this, IDC_Variance);
         HideDialogItem (this, IDC_MinLogPrompt);
         HideDialogItem (this, IDC_minLogDetValue);

         m_statCode = kMeanStdDevOnly - 1;

			}	// end "if ( m_statCode+1 == kMeanCovariance )"

      else // m_statCode+1 == kMeanStdDevOnly
			{
         SetDLogControl (this, IDC_SetZeroVariance, m_setZeroVarianceFlag);
         SetDLogControlHilite (this, IDC_SetZeroVariance, 0);

         SetZeroVariance();

         ShowDialogItem (this, IDC_MinLogPrompt);
         ShowDialogItem (this, IDC_minLogDetValue);

         m_statCode = kMeanCovariance - 1;

			}	// end "else m_statCode+1 == kMeanStdDevOnly"

		}	// end "if (ProjectMenuClearStatistics())"

   else // !ProjectMenuClearStatistics()
		{ 
				// User cancelled the operation. Set back to previous value.	 
				
      wxRadioButton* meanstd = (wxRadioButton*) FindWindow(IDC_MeanStd);
      wxRadioButton* meanstdcov = (wxRadioButton*) FindWindow(IDC_meanStdCov);
      if (m_statCode == 1) 
			{
         meanstdcov->SetValue(true);
         meanstd->SetValue(false);
			} 
			
		else 
			{
         meanstdcov->SetValue(false);
         meanstd->SetValue(true);
			}
		}

}	// end "OnMeanStd" 


void CMStatOptionsDlg::OnMeanStdCov(wxCommandEvent& event) 
{
   OnMeanStd(event);

}	// end "OnMeanStdCov" 


void CMStatOptionsDlg::OnSetZeroVariance(wxCommandEvent& event) 
{
   wxCheckBox* setzerovar = (wxCheckBox*)FindWindow (IDC_SetZeroVariance);
   m_setZeroVarianceFlag = setzerovar->GetValue();

   SetZeroVariance();

}	// end "OnSetZeroVariance"


void CMStatOptionsDlg::SetZeroVariance() 
{
   ShowHideDialogItem (this, IDC_Variance, m_setZeroVarianceFlag);
   if (m_setZeroVarianceFlag) 
      SelectDialogItemText(this, IDC_Variance, 0, SInt16_MAX);


}	// end "SetZeroVariance"


bool CMStatOptionsDlg::TransferDataFromWindow() 
{
   SInt16 returnCode = 0;
   wxCheckBox* classstateonly = (wxCheckBox*)FindWindow (IDC_ClassStatsOnly);
   wxCheckBox* zerovariance = (wxCheckBox*)FindWindow (IDC_SetZeroVariance);
   wxCheckBox* commoncov = (wxCheckBox*)FindWindow (IDC_UseCommonCov);
   wxRadioButton* meanstd = (wxRadioButton*)FindWindow (IDC_MeanStd);
   wxRadioButton* meanstdcov = (wxRadioButton*)FindWindow (IDC_meanStdCov);
   wxTextCtrl* minlogdet = (wxTextCtrl*)FindWindow (IDC_minLogDetValue);
   wxTextCtrl* variance = (wxTextCtrl*)FindWindow (IDC_Variance);
   
   m_classStatsOnlyFlag = classstateonly->GetValue();
   m_setZeroVarianceFlag = zerovariance->GetValue();
   m_useCommonCovarianceInLOOCFlag = commoncov->GetValue();
   
   wxString minlogdetString = minlogdet->GetValue();
   minlogdetString.ToDouble (&m_minLogDetValue);
   
   wxString varianceString = variance->GetValue();
   varianceString.ToDouble (&m_varianceValue);   
   
   returnCode = CheckDialogRealValue (this,
													IDC_Variance,
													0.,
													10000.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"


bool CMStatOptionsDlg::TransferDataToWindow() 
{
	wxCheckBox* classstateonly = (wxCheckBox*) FindWindow(IDC_ClassStatsOnly);
	wxCheckBox* zerovariance = (wxCheckBox*) FindWindow(IDC_SetZeroVariance);
	wxCheckBox* commoncov = (wxCheckBox*) FindWindow(IDC_UseCommonCov);
	wxRadioButton* meanstd = (wxRadioButton*) FindWindow(IDC_MeanStd);
	wxRadioButton* meanstdcov = (wxRadioButton*) FindWindow(IDC_meanStdCov);
	wxTextCtrl* minlogdet = (wxTextCtrl*) FindWindow(IDC_minLogDetValue);
	wxTextCtrl* variance = (wxTextCtrl*) FindWindow(IDC_Variance);

	classstateonly->SetValue(m_classStatsOnlyFlag);
	zerovariance->SetValue(m_setZeroVarianceFlag);
	commoncov->SetValue(m_useCommonCovarianceInLOOCFlag);

	minlogdet->ChangeValue(wxString::Format(wxT("%.1f"), m_minLogDetValue));
	variance->ChangeValue(wxString::Format(wxT("%.2f"), m_varianceValue));

	if (m_statCode == 1) 
		{
		meanstdcov->SetValue(true);
		meanstd->SetValue(false);
		} 
	else 
		{
		meanstdcov->SetValue(false);
		meanstd->SetValue(true);
		}
		
	return true;
	
}	// end "TransferDataToWindow"
