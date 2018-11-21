//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDisplayMinMaxDialog.cpp : class implementation file
//	Class Definition:		LDisplayMinMaxDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			11/13/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMDisplayMinMaxDialog class.
/* Template for debugging
		int numberChars = sprintf (
			(char*)&gTextString3,
			" LDisplayMinMaxDialog.cpp: (): %s",
			gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "LDialog.h"	
#include "wx/valgen.h"
#include "wx/valnum.h"

#include "LDisplayMinMaxDialog.h"

extern void UpdateEnhancementMinMaxes (
				HistogramSpecsPtr					histogramSpecsPtr,
				DialogPtr							dialogPtr,
				SInt16								localMinMaxCode,
				SInt16								percentTailsClipped,
				SInt16								numberLevels,
				double*								minMaxValuesPtr,
				SInt16*								channels,
				Boolean								updateUserDefinedFlag);



CMDisplayMinMaxDialog::CMDisplayMinMaxDialog (
				wxWindow*							pParent,
				wxWindowID							id,
				const wxString&					title /*=NULL*/)
		: CMDialog (CMDisplayMinMaxDialog::IDD,
						pParent,
						title)
{
	m_newBlueChannelMax = 0;
	m_newBlueChannelMin = 0;
	m_newGreenChannelMax = 0;
	m_newGreenChannelMin = 0;
	m_newRedChannelMax = 0;
	m_newRedChannelMin = 0;
	m_percentClipped = 0;
	m_localMinMaxCode = 0;
	
	m_histogramAvailableFlag = true;

	CreateControls ();

	m_initializedFlag = CMDialog::m_initializedFlag;

	m_maxValue = 255;
	m_minValue = 0;
	m_updatingMinMaxFlag = FALSE;

}	// end "CMDisplayMinMaxDialog"



BEGIN_EVENT_TABLE (CMDisplayMinMaxDialog, CMDialog)
	EVT_INIT_DIALOG (CMDisplayMinMaxDialog::OnInitDialog)
	EVT_RADIOBUTTON (IDC_ChannelMinMax, CMDisplayMinMaxDialog::OnChannelMinMax)
	EVT_RADIOBUTTON (IDC_EntireRange, CMDisplayMinMaxDialog::OnChannelMinMax)
	EVT_RADIOBUTTON (IDC_UserSpecified, CMDisplayMinMaxDialog::OnChannelMinMax)

	EVT_TEXT (IDC_PercentClipped, CMDisplayMinMaxDialog::OnChangePercentClipped)
	EVT_TEXT (IDC_RedChannelMinE, CMDisplayMinMaxDialog::OnChangeMinMaxRed)
	EVT_TEXT (IDC_RedChannelMaxE, CMDisplayMinMaxDialog::OnChangeMinMaxRed)
	EVT_TEXT (IDC_GreenChannelMinE, CMDisplayMinMaxDialog::OnChangeMinMaxGreen)
	EVT_TEXT (IDC_GreenChannelMaxE, CMDisplayMinMaxDialog::OnChangeMinMaxGreen)
	EVT_TEXT (IDC_BlueChannelMinE, CMDisplayMinMaxDialog::OnChangeMinMaxBlue)
	EVT_TEXT (IDC_BlueChannelMaxE, CMDisplayMinMaxDialog::OnChangeMinMaxBlue)

	//EVT_BUTTON (IDC_RedChannelMin, OnStnClickedRedchannelmin)
	//EVT_CHAR_HOOK (CMDisplayMinMaxDialog::OnButtonPress)
END_EVENT_TABLE ()



CMDisplayMinMaxDialog::~CMDisplayMinMaxDialog ()

{

}	// end "~CMDisplayMinMaxDialog"



void CMDisplayMinMaxDialog::CheckNewMinMaxValue (
				int									id,
				SInt16								channelIndex)

{
	double								doubleValue = 0;
	
		
	UpdateMinMaxOptions ();
	
	SInt16 numberFDecimalDigits = GetNumberDecimalDigits (channelIndex);
	
	wxTextCtrl* itemTextCtrl = (wxTextCtrl*)FindWindow (id);
	wxString stringValue = itemTextCtrl->GetValue ();
	stringValue.ToDouble (&doubleValue);
	if (doubleValue < m_minValue || doubleValue > m_maxValue)
		{
		doubleValue = MIN (doubleValue, m_maxValue);
		doubleValue = MAX (doubleValue, m_minValue);
		itemTextCtrl->ChangeValue (wxString::Format (wxT("%.*g"),
																	numberFDecimalDigits,
																	doubleValue));
		
		SelectDialogItemText (this, id, 0, SInt16_MAX);
		
		}	// end "if (doubleValue < m_minValue || doubleValue > m_maxValue)"
		
}	// end "CheckNewMinMaxValue"



void CMDisplayMinMaxDialog::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
			// Create validator based on image data value type.
			
	wxFloatingPointValidator<double> validatorValue (
						gImageFileInfoPtr->maxNumberFDecimalDigits, &m_doubleValueCheck);
	if (gImageFileInfoPtr->dataTypeCode == kIntegerType &&
																!gImageFileInfoPtr->signedDataFlag)
		validatorValue.SetMin (0);

	bSizer1 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer (wxVERTICAL);

	m_staticText34 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText34->Wrap (-1);
	bSizer16->Add (m_staticText34, wxSizerFlags(0).Border(wxTOP, 15));

	bSizer15->Add (bSizer16, 0, wxEXPAND);

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer (wxVERTICAL);

	m_staticText35 = new wxStaticText (this,
													wxID_ANY,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText35->Wrap (-1);
	bSizer17->Add (m_staticText35, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 7));

	bSizer15->Add (bSizer17, 0, wxEXPAND);

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer (wxHORIZONTAL);

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer (3, 1, 0, 0);

	m_staticText5 = new wxStaticText (this,
													IDC_RedChannelPrompt,
													wxT("Red channel:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText5->Wrap (-1);
	gSizer3->Add (m_staticText5, 0, wxALL, 5);

	m_staticText7 = new wxStaticText (this,
													IDC_GreenChannelPrompt,
													wxT("Green channel:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText7->Wrap (-1);
	gSizer3->Add (m_staticText7, 0, wxALL, 5);

	m_staticText9 = new wxStaticText (this,
													IDC_BlueChannelPrompt,
													wxT("Blue channel:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText9->Wrap (-1);
	gSizer3->Add (m_staticText9, 0, wxALL, 5);
	bSizer18->Add (gSizer3, 1, wxEXPAND);

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer (3, 1, 0, 0);

	m_staticText36 = new wxStaticText (this,
													IDC_RedChannel,
													wxT("1"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText36->Wrap (-1);
	gSizer4->Add (m_staticText36, 0, wxALL, 5);

	m_staticText37 = new wxStaticText (this,
													IDC_GreenChannel,
													wxT("2"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText37->Wrap (-1);
	gSizer4->Add (m_staticText37, 0, wxALL, 5);

	m_staticText38 = new wxStaticText (this,
													IDC_BlueChannel,
													wxT("3"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText38->Wrap (-1);
	gSizer4->Add (m_staticText38, 0, wxALL, 5);
	bSizer18->Add (gSizer4, 0, wxEXPAND);
	bSizer15->Add (bSizer18, 1, 0, 5);
	bSizer2->Add (bSizer15, 0, wxEXPAND);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer (wxVERTICAL);

	m_staticText12 = new wxStaticText (this,
													IDC_TableTitle,
													wxT("Data"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText12->Wrap (-1);
	bSizer6->Add (m_staticText12, 0, wxALIGN_CENTER|wxALL, 5);


	bSizer4->Add (bSizer6, 0, wxALIGN_CENTER, 5);

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText24 = new wxStaticText (this,
													IDC_TableTitle,
													wxT("Min"),
													wxDefaultPosition,
													wxSize (120,-1),
													wxALIGN_CENTER_HORIZONTAL);
	m_staticText24->Wrap (-1);
	bSizer9->Add (m_staticText24,
						wxSizerFlags (0).Align (wxALIGN_CENTER).Border (wxALL,5)); //wxSHAPED

	m_staticText25 = new wxStaticText (this,
													IDC_TableTitle,
													wxT("Max"),
													wxDefaultPosition,
													wxSize (120,-1),
													wxALIGN_CENTER_HORIZONTAL);
	m_staticText25->Wrap (-1);
	bSizer9->Add (m_staticText25,
						wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL,5));

	bSizer4->Add (bSizer9,  wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer (wxHORIZONTAL);

	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer (3, 2, 0, 0);

	m_staticText23 = new wxStaticText (this,
													IDC_RedChannelMin,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (120,-1),
													0);
	m_staticText23->Wrap (-1);
	gSizer5->Add (m_staticText23,
						wxSizerFlags (0).Align (wxALIGN_CENTRE_VERTICAL).Border (wxALL,5));

	m_staticText241 = new wxStaticText (this,
													IDC_RedChannelMax,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (120,-1),
													0);
	m_staticText241->Wrap (-1);
	gSizer5->Add (m_staticText241,
						wxSizerFlags (0).Align (wxALIGN_CENTRE_VERTICAL).Border (wxALL,5));

	m_staticText251 = new wxStaticText (this,
													IDC_GreenChannelMin,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (120,-1),
													0);
	m_staticText251->Wrap (-1);
	gSizer5->Add (m_staticText251,
						wxSizerFlags (0).Align (wxALIGN_CENTRE_VERTICAL).Border (wxALL,5));

	m_staticText261 = new wxStaticText (this,
													IDC_GreenChannelMax,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (120,-1),
													0);
	m_staticText261->Wrap (-1);
	gSizer5->Add (m_staticText261,
						wxSizerFlags (0).Align (wxALIGN_CENTRE_VERTICAL).Border (wxALL,5));

	m_staticText271 = new wxStaticText (this,
													IDC_BlueChannelMin,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (120,-1),
													0);
	m_staticText271->Wrap (-1);
	gSizer5->Add (m_staticText271,
						wxSizerFlags (0).Align (wxALIGN_CENTRE_VERTICAL).Border (wxALL,5));

	m_staticText281 = new wxStaticText (this,
													IDC_BlueChannelMax,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxSize (120,-1),
													0);
	m_staticText281->Wrap (-1);
	gSizer5->Add (m_staticText281,
						wxSizerFlags (0).Align (wxALIGN_CENTRE_VERTICAL).Border (wxALL,5));


	//bSizer11->Add (gSizer5, 1, wxEXPAND);
	bSizer11->Add (gSizer5, wxSizerFlags(1).Expand());

	m_staticText26 = new wxStaticText (
									this,
									IDC_NotCalculatedMessage,
									wxT("Not known.\rNeed to\rread or\rcalculate\rhistogram."),
									wxDefaultPosition,
									wxDefaultSize,
									0);
	m_staticText26->Wrap (-1);
	m_staticText26->Hide ();

	//bSizer11->Add (m_staticText26, 0, wxALL, 5);
	bSizer11->Add (m_staticText26, wxSizerFlags(0).Border(wxALL, 5));
	//bSizer4->Add (bSizer11, 1, wxALL | wxALIGN_CENTER, 5);
	bSizer4->Add (bSizer11, wxSizerFlags(1).Align(wxALIGN_CENTER).Border(wxALL, 5));
	//bSizer2->Add (bSizer4, 0, wxLEFT | wxRIGHT | wxEXPAND, 30);
	bSizer2->Add (bSizer4, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 30));

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer (wxVERTICAL);

	m_staticText21 = new wxStaticText (this,
													IDC_TableTitle,
													wxT("Enhancement"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText21->Wrap (-1);
	//bSizer7->Add (m_staticText21, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer7->Add (m_staticText21, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));

	bSizer5->Add (bSizer7, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText27 = new wxStaticText (this,
													IDC_TableTitle,
													wxT("Min"),
													wxDefaultPosition,
													wxSize (120,-1),
													wxALIGN_CENTRE_HORIZONTAL);
	m_staticText27->Wrap (-1);
	//bSizer12->Add (m_staticText27, 1, wxALIGN_CENTER | wxALL, 5); //wxSHAPED
	bSizer12->Add (m_staticText27, wxSizerFlags(1).Align(wxALIGN_CENTER).Border(wxALL, 5));

	m_staticText28 = new wxStaticText (this,
													IDC_TableTitle,
													wxT("Max"),
													wxDefaultPosition,
													wxSize (120,-1),
													wxALIGN_CENTRE_HORIZONTAL);
	m_staticText28->Wrap (-1);
	//bSizer12->Add (m_staticText28, 1, wxALIGN_CENTER | wxALL, 5); //wxSHAPED
	bSizer12->Add (m_staticText28, wxSizerFlags(1).Align(wxALIGN_CENTER).Border(wxALL, 5));

	//bSizer5->Add (bSizer12, 0, wxALIGN_CENTER | wxEXPAND, 5);
	bSizer5->Add (bSizer12, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer (wxHORIZONTAL);

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer (3, 2, 0, 0);

	m_textCtrl2 = new wxTextCtrl (this,
											IDC_RedChannelMinE,
											wxEmptyString,
											wxDefaultPosition,
											wxSize (120,-1),
											0);
	gSizer2->Add (m_textCtrl2, 0, wxALL, 5);
	m_textCtrl2->SetValidator (validatorValue);

	m_textCtrl3 = new wxTextCtrl (this,
											IDC_RedChannelMaxE,
											wxEmptyString,
											wxDefaultPosition,
											wxSize (120,-1),
											0);
	gSizer2->Add (m_textCtrl3, 0, wxALL, 5);
	m_textCtrl3->SetValidator (validatorValue);

	m_textCtrl4 = new wxTextCtrl (this,
											IDC_GreenChannelMinE,
											wxEmptyString,
											wxDefaultPosition,
											wxSize (120,-1),
											0);
	gSizer2->Add (m_textCtrl4, 0, wxALL, 5);
	m_textCtrl4->SetValidator (validatorValue);
	  
	m_textCtrl5 = new wxTextCtrl (this,
											IDC_GreenChannelMaxE,
											wxEmptyString,
											wxDefaultPosition,
											wxSize (120,-1),
											0);
	gSizer2->Add (m_textCtrl5, 0, wxALL, 5);
	m_textCtrl5->SetValidator (validatorValue);

	m_textCtrl6 = new wxTextCtrl (this,
											IDC_BlueChannelMinE,
											wxEmptyString,
											wxDefaultPosition,
											wxSize (120,-1),
											0);
	gSizer2->Add (m_textCtrl6, 0, wxALL, 5);
	m_textCtrl6->SetValidator (validatorValue);

	m_textCtrl7 = new wxTextCtrl (this,
											IDC_BlueChannelMaxE,
											wxEmptyString,
											wxDefaultPosition,
											wxSize (120,-1),
											0);
	gSizer2->Add (m_textCtrl7, 0, wxALL, 5);
	m_textCtrl7->SetValidator (validatorValue);

	bSizer13->Add (gSizer2, 0, 0, 5);

	m_staticText29 = new wxStaticText (
									this,
									IDC_NotCalculatedMessage2,
									wxT("Not known.\rNeed to\rread or\rcalculate\rhistogram."),
									wxDefaultPosition,
									wxDefaultSize,
									0);
	m_staticText29->Wrap (-1);
	m_staticText29->Hide ();

	bSizer13->Add (m_staticText29, 0, wxALL, 5);
	bSizer5->Add (bSizer13, 1, wxALL | wxALIGN_CENTER, 5);
	bSizer2->Add (bSizer5, 0, 0, 5);
	bSizer1->Add (
					bSizer2,
					wxSizerFlags (0).Expand ().Border (wxLEFT|wxTOP|wxRIGHT|wxBOTTOM,12));

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer (wxHORIZONTAL);
   
	m_radioBtn3 = new wxRadioButton (
										this,
										IDC_ChannelMinMax,
										wxT("Channel min/max with percent of tails clipped:"),
										wxDefaultPosition,
										wxDefaultSize,
										wxRB_GROUP);
   m_radioBtn3->SetToolTip (
			wxT("Ignore the specified percentage of the tails of the histogram, usually results in a display of the data that has better contrast"));
	bSizer3->Add (m_radioBtn3, 0, wxALL, 5);
	m_textCtrl1 = new wxTextCtrl (this,
											IDC_PercentClipped,
											wxEmptyString,
											wxDefaultPosition,
											wxDefaultSize,
											0);
	bSizer3->Add (m_textCtrl1, 0, wxALL, 5);
	m_textCtrl1->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																&m_percentClippedString));

	bSizer20->Add (bSizer3, 0, wxRIGHT | wxEXPAND, 15);
	m_radioBtn1 = new wxRadioButton (this,
												IDC_EntireRange,
												wxT("Entire data range"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_radioBtn1->SetToolTip (
			wxT("Have the entire data range of the histogram to be used in establishing the image gray scale"));
	bSizer20->Add (m_radioBtn1, 0, wxALL, 5);
	m_radioBtn2 = new wxRadioButton (this,
												IDC_UserSpecified,
												wxT("User specified"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_radioBtn2->SetToolTip (
						wxT("Use ranges specified by the edit boxes below 'Enhancement'"));
	m_radioBtn2->Enable (false);
	bSizer20->Add (m_radioBtn2, 0, wxALL, 5);

	m_radioBtn4 = new wxRadioButton (this,
												IDC_ThematicDefault,
												wxT("Thematic default"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
	m_radioBtn4->Hide ();

	bSizer20->Add (m_radioBtn4, 0, wxALL, 5);
	bSizer1->Add (bSizer20, wxSizerFlags (1).Expand ().Border (wxLEFT|wxRIGHT,12));
	
	/*
	   m_button1->SetToolTip (
			wxT("Close the dialog box; no changes are made in the display enhancement specifications"));
	bSizer8->Add (m_button1, wxSizerFlags (0).Border (wxTOP|wxRIGHT,6));
	m_button2 = new wxButton (this,
										wxID_OK,
										wxT("OK"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
   m_button2->SetToolTip (wxT("Set the display enhancement parameters"));
	*/
	/*
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bSizer1->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bSizer1->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif
	*/
	CreateStandardButtons (bSizer1);
	
	this->SetSizer (bSizer1);
	this->Layout ();
	//SetSizerAndFit (bSizer1);
	//this->Centre (wxBOTH);
	this->Fit ();
}



//------------------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							min-max dialog box to the user and copy the
//							revised back to the display specification dialog variables
//							if the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Abdur Rahman Maud		Date: 06/01/2011
//	Revised By:			Larry L. Biehl			Date: 06/19/2018

Boolean CMDisplayMinMaxDialog::DoDialog (
				SInt16*								channelsPtr,
				SInt16								rgbColors,
				SInt16								displayType,
				SInt16								numberLevels,
				SInt16*								percentTailsClippedPtr,
				SInt16*								minMaxSelectionPtr,
				double*								minMaxValuesPtr)

{
	SInt16								returnCode;

	Boolean								continueFlag = FALSE;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

	m_channelsPtr = channelsPtr;
	m_rgbColors = rgbColors;
	m_displayType = displayType;
	m_numberLevels = numberLevels;
	m_percentClipped = *percentTailsClippedPtr;
	m_minMaxOptionCode = *minMaxSelectionPtr;
	m_minMaxValuesPtr = minMaxValuesPtr;

	returnCode = ShowModal ();

	if (returnCode == wxID_OK)
		{
		MinMaxEnhancementDialogOK (m_histogramSummaryPtr,
											 channelsPtr,
											 minMaxValuesPtr,
											 minMaxSelectionPtr,
											 percentTailsClippedPtr,
											 m_newRedChannelMin,
											 m_newRedChannelMax,
											 m_newGreenChannelMin,
											 m_newGreenChannelMax,
											 m_newBlueChannelMin,
											 m_newBlueChannelMax,
											 m_localMinMaxCode + 2,
											 (SInt16)m_percentClipped);

	  continueFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog" 



SInt16 CMDisplayMinMaxDialog::GetNumberDecimalDigits (
				SInt16								channelIndex)

{
	SInt16								numberFDecimalDigits;
	
	
	if (m_histogramSummaryPtr != NULL)
		{
		SInt16 summaryIndex = m_channelsPtr[channelIndex] - 1;
		numberFDecimalDigits = m_histogramSummaryPtr[summaryIndex].numberFDecimalDigits;

		}	// end "if (m_histogramSummaryPtr != NULL)"

	else	// m_histogramSummaryPtr == NULL
		{
		numberFDecimalDigits = gImageFileInfoPtr->maxNumberFDecimalDigits;

		}	// end "else histogramSummaryPtr == NULL"
		
	return (numberFDecimalDigits);

}	// end "GetNumberDecimalDigits" 



void CMDisplayMinMaxDialog::OnChangePercentClipped (
				wxCommandEvent&					event)

{
	int									savedPercentClipped;

	Boolean								updateMinMaxFlag = FALSE;


	savedPercentClipped = m_percentClipped;
	m_percentClipped = CheckValue (IDC_PercentClipped, 0, 100);

	if (m_percentClipped <= 100)
		{
		if (savedPercentClipped != m_percentClipped && m_histogramAvailableFlag)
			updateMinMaxFlag = TRUE;

		if (m_percentClipped == 0 && !m_histogramAvailableFlag)
			updateMinMaxFlag = TRUE;

		}	// end "if (m_percentClipped <= 100)"

	if (updateMinMaxFlag)
		{
				// Indicate that the min maxes are being updated by the
				// per-channel or entire range settings.

		m_updatingMinMaxFlag = TRUE;

		UpdateEnhancementMinMaxes (m_histogramSpecsPtr,
											 this,
											 (SInt16)(m_localMinMaxCode + 2),
											 m_percentClipped,
											 m_numberLevels,
											 m_minMaxValuesPtr,
											 m_channelsPtr,
											 FALSE);

				// Now indicate that any change in the min maxes are being done
				// by the user.   

		m_updatingMinMaxFlag = FALSE;

		}	// end "if (updateMinMaxFlag)"

}	// end "OnChangePercentClipped"



void CMDisplayMinMaxDialog::OnChangeMinMaxBlue (
				wxCommandEvent&					event)

{
	CheckNewMinMaxValue (event.GetId (), 2);
		
}	// end "OnChangeMinMaxBlue" 



void CMDisplayMinMaxDialog::OnChangeMinMaxGreen (
				wxCommandEvent&					event)

{
	CheckNewMinMaxValue (event.GetId (), 1);

}	// end "OnChangeMinMaxGreen" 



void CMDisplayMinMaxDialog::OnChangeMinMaxRed (
				wxCommandEvent&					event)

{
	CheckNewMinMaxValue (event.GetId (), 0);
		
}	// end "OnChangeMinMaxRed"



void CMDisplayMinMaxDialog::OnChannelMinMax (
				wxCommandEvent&					event)

{
	SInt16								currentMinMaxCode,
											savedMinMaxCode;


	currentMinMaxCode = m_localMinMaxCode + 2;
	savedMinMaxCode = currentMinMaxCode;

	if (((wxRadioButton*)FindWindow (IDC_ChannelMinMax))->GetValue ())
		m_localMinMaxCode = 0;

	else if (((wxRadioButton*)FindWindow (IDC_EntireRange))->GetValue ())
		m_localMinMaxCode = 1;
	
	else if (((wxRadioButton*)FindWindow (IDC_UserSpecified))->GetValue ())
		m_localMinMaxCode = 2;
	
	else
		m_localMinMaxCode = 3;
	
	if (MinMaxEnhancementDialogOnMinMaxCode (this,
															&currentMinMaxCode,
															(SInt16)(m_localMinMaxCode + 2),
															m_percentClipped,
															m_histogramSummaryPtr,
															m_histogramAvailableFlag))
	
		{
				// Indicate that the min maxes are being updated by the
				// per-channel or entire range settings.
				
		m_updatingMinMaxFlag = TRUE;
		
		UpdateEnhancementMinMaxes (m_histogramSpecsPtr,
											 this,
											 (SInt16)(m_localMinMaxCode + 2),
											 m_percentClipped,
											 m_numberLevels,
											 m_minMaxValuesPtr,
											 m_channelsPtr,
											 FALSE);

				// Now indicate that any change in the min maxes are being done
				// by the user.

		m_updatingMinMaxFlag = FALSE;

		}	// end "if ( MinMaxEnhancementDialogOnMinMaxCode (..."

	MinMaxEnhancementDialogSetSelection (this,
														savedMinMaxCode,
														(SInt16)(m_localMinMaxCode + 2));
	
	this->Fit ();
	
}	// end "OnChannelMinMax"



void CMDisplayMinMaxDialog::OnInitDialog (
				wxInitDialogEvent&				event)

{
	SInt16								localMinMaxCode,
											percentTailsClipped;


	CMDialog::OnInitDialog (event);
    
	percentTailsClipped = (SInt16)m_percentClipped;
	localMinMaxCode = (SInt16)m_localMinMaxCode;
	
	MinMaxEnhancementDialogInitialize (this,
													m_channelsPtr,
													m_displayType,
													m_numberLevels,
													m_rgbColors,
													m_minMaxOptionCode,
													m_minMaxValuesPtr,
													&m_histogramSpecsPtr,
													&m_histogramSummaryPtr,
													&m_minValue,
													&m_maxValue,
													&localMinMaxCode,
													&percentTailsClipped,
													&m_histogramAvailableFlag);

			// Get the current min-max settings.
			
	wxTextCtrl* redmin = (wxTextCtrl*)FindWindow (IDC_RedChannelMinE);
	wxTextCtrl* redmax = (wxTextCtrl*)FindWindow (IDC_RedChannelMaxE);
	wxTextCtrl* greenmin = (wxTextCtrl*)FindWindow (IDC_GreenChannelMinE);
	wxTextCtrl* greenmax = (wxTextCtrl*)FindWindow (IDC_GreenChannelMaxE);
	wxTextCtrl* bluemin = (wxTextCtrl*)FindWindow (IDC_BlueChannelMinE);
	wxTextCtrl* bluemax = (wxTextCtrl*)FindWindow (IDC_BlueChannelMaxE);
	wxTextCtrl* percentclip = (wxTextCtrl*)FindWindow (IDC_PercentClipped);

	m_newBlueChannelMax = wxAtof (bluemax->GetValue ());
	m_newBlueChannelMin = wxAtof (bluemin->GetValue ());
	m_newGreenChannelMax = wxAtof (greenmax->GetValue ());
	m_newGreenChannelMin = wxAtof (greenmin->GetValue ());
	m_newRedChannelMax = wxAtof (redmax->GetValue ());
	m_newRedChannelMin = wxAtof (redmin->GetValue ());

	m_percentClipped = percentTailsClipped;
	m_localMinMaxCode = localMinMaxCode - 2;

	if (CMDisplayMinMaxDialog::TransferDataToWindow ())
		PositionDialogWindow ();

			// Set default text selection to first edit text item

	int selectedItem = IDC_RedChannelMinE;    
	if (localMinMaxCode == kPerChannelMinMax)
		selectedItem = IDC_PercentClipped;

	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);

	SetSizerAndFit (bSizer1);
	this->Layout ();
	this->Fit ();

}	// end "OnInitDialog" 



bool CMDisplayMinMaxDialog::TransferDataFromWindow ()

{
	int									idErr;
	bool									returnFlag;
	
	
	wxTextCtrl* redmin = (wxTextCtrl*)FindWindow (IDC_RedChannelMinE);
	wxTextCtrl* redmax = (wxTextCtrl*)FindWindow (IDC_RedChannelMaxE);
	wxTextCtrl* greenmin = (wxTextCtrl*)FindWindow (IDC_GreenChannelMinE);
	wxTextCtrl* greenmax = (wxTextCtrl*)FindWindow (IDC_GreenChannelMaxE);
	wxTextCtrl* bluemin = (wxTextCtrl*)FindWindow (IDC_BlueChannelMinE);
	wxTextCtrl* bluemax = (wxTextCtrl*)FindWindow (IDC_BlueChannelMaxE);
	wxTextCtrl* percentclip = (wxTextCtrl*)FindWindow (IDC_PercentClipped);

	m_newBlueChannelMax = wxAtof (bluemax->GetValue ());
	m_newBlueChannelMin = wxAtof (bluemin->GetValue ());
	m_newGreenChannelMax = wxAtof (greenmax->GetValue ());
	m_newGreenChannelMin = wxAtof (greenmin->GetValue ());
	m_newRedChannelMax = wxAtof (redmax->GetValue ());
	m_newRedChannelMin = wxAtof (redmin->GetValue ());

	m_percentClippedString = percentclip->GetValue ();
	m_percentClipped = wxAtoi (m_percentClippedString);

	if (((wxRadioButton*)FindWindow (IDC_ChannelMinMax))->GetValue ())
		m_localMinMaxCode = 0;
	
	else if (((wxRadioButton*)FindWindow (IDC_EntireRange))->GetValue ())
		m_localMinMaxCode = 1;
	
	else if (((wxRadioButton*)FindWindow (IDC_UserSpecified))->GetValue ())
		m_localMinMaxCode = 2;
	
	else
		m_localMinMaxCode = 3;

	idErr = VerifyDataRangeStartEndValues (m_newRedChannelMin,
														m_newRedChannelMax,
														IDC_RedChannelMinE);
	
	if (idErr == 0)
		idErr = VerifyDataRangeStartEndValues (m_newGreenChannelMin,
															m_newGreenChannelMax,
															IDC_GreenChannelMinE);
	
	if (idErr == 0)
		idErr = VerifyDataRangeStartEndValues (m_newBlueChannelMin,
															m_newBlueChannelMax,
															IDC_BlueChannelMinE);
		
	if (idErr != 0)
		SelectDialogItemText (this, idErr, 0, SInt16_MAX);
		
	return (idErr == 0);
	
}	// end "TransferDataFromWindow"


bool CMDisplayMinMaxDialog::TransferDataToWindow ()

{    
	wxTextCtrl* redmin = (wxTextCtrl*)FindWindow (IDC_RedChannelMinE);
	wxTextCtrl* redmax = (wxTextCtrl*)FindWindow (IDC_RedChannelMaxE);
	wxTextCtrl* greenmin = (wxTextCtrl*)FindWindow (IDC_GreenChannelMinE);
	wxTextCtrl* greenmax = (wxTextCtrl*)FindWindow (IDC_GreenChannelMaxE);
	wxTextCtrl* bluemin = (wxTextCtrl*)FindWindow (IDC_BlueChannelMinE);
	wxTextCtrl* bluemax = (wxTextCtrl*)FindWindow (IDC_BlueChannelMaxE);

	wxTextCtrl* percentclip = (wxTextCtrl*)FindWindow (IDC_PercentClipped);
	percentclip->ChangeValue (wxString::Format (wxT("%i"), m_percentClipped));

	((wxRadioButton*)FindWindow (IDC_ThematicDefault))->SetValue (false);

	if (m_localMinMaxCode == 0)
		{
		((wxRadioButton*)FindWindow (IDC_ChannelMinMax))-> SetValue (true);
		((wxRadioButton*)FindWindow (IDC_EntireRange))->SetValue (false);
		((wxRadioButton*)FindWindow (IDC_UserSpecified))->SetValue (false);
		redmin->ChangeValue (wxString::Format (wxT("%g"), m_newRedChannelMin));
		redmax->ChangeValue (wxString::Format (wxT("%g"), m_newRedChannelMax));
		greenmin->ChangeValue (wxString::Format (wxT("%g"), m_newGreenChannelMin));
		greenmax->ChangeValue (wxString::Format (wxT("%g"), m_newGreenChannelMax));
		bluemin->ChangeValue (wxString::Format (wxT("%g"), m_newBlueChannelMin));
		bluemax->ChangeValue (wxString::Format (wxT("%g"), m_newBlueChannelMax));
		
		}	// end "if (m_localMinMaxCode == 0)"
		
	else if (m_localMinMaxCode == 1)
		{
		((wxRadioButton*)FindWindow (IDC_ChannelMinMax))-> SetValue (false);
		((wxRadioButton*)FindWindow (IDC_EntireRange))->SetValue (true);
		((wxRadioButton*)FindWindow (IDC_UserSpecified))->SetValue (false);
		redmin->ChangeValue (wxString::Format (wxT("%g"), m_minValue));
		redmax->ChangeValue (wxString::Format (wxT("%g"), m_maxValue));
		greenmin->ChangeValue (wxString::Format (wxT("%g"), m_minValue));
		greenmax->ChangeValue (wxString::Format (wxT("%g"), m_maxValue));
		bluemin->ChangeValue (wxString::Format (wxT("%g"), m_minValue));
		bluemax->ChangeValue (wxString::Format (wxT("%g"), m_maxValue));
		
		}	// end "else if (m_localMinMaxCode == 1)"
	
	else if (m_localMinMaxCode == 2)
		{
		((wxRadioButton*)FindWindow (IDC_ChannelMinMax))->SetValue (false);
		((wxRadioButton*)FindWindow (IDC_EntireRange))->SetValue (false);
		((wxRadioButton*)FindWindow (IDC_UserSpecified))->SetValue (true);
		SetDLogControlHilite (this, IDC_UserSpecified, 0);
		
		}	// end "else if (m_localMinMaxCode == 2)"
		
	else	// m_localMinMaxCode != 0, 1, or 2
		{
		((wxRadioButton*)FindWindow (IDC_ChannelMinMax))->SetValue (false);
		((wxRadioButton*)FindWindow (IDC_EntireRange))->SetValue (false);
		((wxRadioButton*)FindWindow (IDC_UserSpecified))->SetValue (false);
		((wxRadioButton*)FindWindow (IDC_ThematicDefault))->SetValue (true);
		
		}	// end "else m_localMinMaxCode != 0, 1, or 2"

	return true;
	 
}	// end "TransferDataToWindow"



void CMDisplayMinMaxDialog::UpdateMinMaxOptions ()

{
	if (!m_updatingMinMaxFlag && (m_localMinMaxCode != (kUserSpecified - 2)))
		{
		HideDialogItem (this, IDC_PercentClipped);

				// Hilite the user specified control so that it can
				// be set, then unhilite the control. This control is
				// just for user feedback. If the control is not hilited then
				// it cannot be set.
               
		SetDLogControlHilite (this, IDC_UserSpecified, 0);
        
		m_localMinMaxCode = kUserSpecified - 2;

		if (m_localMinMaxCode == 0)
			((wxRadioButton*)FindWindow (IDC_ChannelMinMax))-> SetValue (true);
		
		else if (m_localMinMaxCode == 1)
			((wxRadioButton*)FindWindow (IDC_EntireRange))-> SetValue (true);
		
		else if (m_localMinMaxCode == 2)
			((wxRadioButton*)FindWindow (IDC_UserSpecified))-> SetValue (true);
		
		else
			((wxRadioButton*)FindWindow (IDC_ThematicDefault))-> SetValue (true);
			
		}	// end "if (!m_updatingMinMaxFlag&& ..."
		
}	// end "UpdateMinMaxOptions"



int CMDisplayMinMaxDialog::VerifyDataRangeStartEndValues (
				double								minValue,
				double								maxValue,
				int									minID)
{
	int									returnCode = 0;
	
	
			// Verify that the data start and end range values make sense

	if (minValue > maxValue)
		{
		wxMessageBox (wxT("Data Range Min value is larger than Max value"),
							wxT("Error"),
							wxOK | wxICON_ERROR | wxSTAY_ON_TOP,
							this);
		/*
		DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							kAlertStrID, 
							IDS_Alert68,
							0,
							NULL);
		*/	
		returnCode = minID;

		}	// end "if (minValue > maxValue)"

	return returnCode;

}	// end "VerifyDataRangeStartEndValues"
