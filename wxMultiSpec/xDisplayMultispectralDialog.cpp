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
//	File:						xDisplayMultispectralDialog.cpp : class implementation file
//	Class Definition:		xDisplayMultispectralDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			05/05/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMDisplaySpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "wx/cshelp.h"		// This needs to be at this location to compile correctly
									// on mygeohub (MultiSpec Online)

#include "SMultiSpec.h"
#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "xChannelsDialog.h"
#include "xDisplayMultispectralDialog.h"
#include "xImageFrame.h"
#include "xImageView.h"

#include "wx/valgen.h"
#include "wx/valnum.h"
#include "wx/window.h"



IMPLEMENT_DYNAMIC_CLASS (CMDisplaySpecsDlg, CMDialog)

BEGIN_EVENT_TABLE (CMDisplaySpecsDlg, CMDialog)
	EVT_BUTTON (ID3C_ChannelDescriptions, CMDisplaySpecsDlg::OnChannelDescriptions)
	EVT_BUTTON (IDEntireImage, CMDisplaySpecsDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMDisplaySpecsDlg::ToSelectedImage)

	EVT_INIT_DIALOG (CMDisplaySpecsDlg::OnInitDialog)

	EVT_CHOICE (ID3C_DisplayType, CMDisplaySpecsDlg::OnSelendokDisplayType)
	
	#if defined multispec_wxmac
		EVT_CHOICE (ID3C_Channels, CMDisplaySpecsDlg::OnChannelsSelendok)
		EVT_CHOICE (ID3C_Enhancement, CMDisplaySpecsDlg::OnEnhancementComboSelendok)
		EVT_CHOICE (ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValues)
	#endif

	#if defined multispec_wxlin
		EVT_COMBOBOX (ID3C_Channels, CMDisplaySpecsDlg::OnChannelsSelendok)
		EVT_COMBOBOX (ID3C_Enhancement, CMDisplaySpecsDlg::OnEnhancementComboSelendok)
		EVT_COMBOBOX (ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValues)
	#endif	// multispec_wxlin

	EVT_COMBOBOX_CLOSEUP (ID3C_Channels, CMDisplaySpecsDlg::OnChannelsComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (ID3C_Enhancement,CMDisplaySpecsDlg::OnEnhancementComboCloseUp)
	EVT_COMBOBOX_CLOSEUP (ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValuesCloseUp)

	EVT_COMBOBOX_DROPDOWN (ID3C_Channels, CMDisplaySpecsDlg::OnChannelsComboDropDown)
	EVT_COMBOBOX_DROPDOWN (ID3C_Enhancement, CMDisplaySpecsDlg::OnEnhancementComboDropDown)
	EVT_COMBOBOX_DROPDOWN (ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValuesDropDown)

	EVT_TEXT (ID3C_BlueChannel, CMDisplaySpecsDlg::OnChangeBlueChannel)
	EVT_TEXT (ID3C_GreenChannel, CMDisplaySpecsDlg::OnChangeGreenChannel)
	EVT_TEXT (ID3C_RedChannel, CMDisplaySpecsDlg::OnChangeRedChannel)
	EVT_TEXT (ID3C_GrayChannel, CMDisplaySpecsDlg::OnChangeGrayChannel)
	EVT_TEXT (IDC_ColumnEnd, CMDisplaySpecsDlg::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMDisplaySpecsDlg::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMDisplaySpecsDlg::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMDisplaySpecsDlg::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMDisplaySpecsDlg::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMDisplaySpecsDlg::CheckColumnInterval)
END_EVENT_TABLE ()



CMDisplaySpecsDlg::CMDisplaySpecsDlg ()

{

}	// end "CMDisplaySpecsDlg"



CMDisplaySpecsDlg::CMDisplaySpecsDlg (
				wxWindow*							pParent,
				wxWindowID							id,
				const wxString&					title,
				const wxPoint&						pos,
				const wxSize&						size,
				long									style)
		: CMDialog (CMDisplaySpecsDlg::IDD, pParent, title)

{
	FileStringPtr fileNamePtr;


	m_BlueChannel = 1;
	m_ColumnEnd = 1;
	m_ColumnInterval = 1;
	m_ColumnStart = 1;
	m_GreenChannel = 1;
	m_LineEnd = 1;
	m_LineInterval = 1;
	m_LineStart = 1;
	m_Magnification = 1;
	m_ComputeHistogramFlag = FALSE;
	m_NumberLevels = 254;
	m_RedChannel = 1;
	m_FileName = "";
	m_BitsOfColor = 0;
	m_DisplayType = 0;
	m_Enhancement = 0;
	m_GrayChannel = 1;
	m_TreatZeroAs = 0;
	m_MinMaxPopupCode = 0;
	m_redChannelInvertFlag = FALSE;
	m_greenChannelInvertFlag = FALSE;
	m_blueChannelInvertFlag = FALSE;
	m_thematicLegendFactor = 1.0;
	m_vectorOverlaysFlag = FALSE;
	m_localFeaturesPtr = NULL;
	mLocalDisplayType = k1_ChannelGrayLevelDisplayType;

	m_gaussianStretch = 2;

	m_initializedFlag = CMDialog::m_initializedFlag;

	wxHelpProvider::Set (new wxSimpleHelpProvider);

	if (m_initializedFlag) 
		{
				// Get pointer to memory for temporary storage of channel list.

		m_localFeaturesPtr = (UInt16*)MNewPointer (
				 (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

		m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

	CreateControls ();
	if (m_initializedFlag) 
		{
				//	Set the image file name.

		fileNamePtr = (FileStringPtr)(gActiveImageViewCPtr->GetImageWindowCPtr())->
																				GetImageFileNamePointer ();
		if (fileNamePtr != NULL) 
			{
			m_FileName = wxString (fileNamePtr, wxConvUTF8);
			wxStaticText * fname = (wxStaticText*)FindWindow (ID3C_FileName);
			fname->SetLabel (m_FileName);
			
			}	// end "if (fileNamePtr != NULL)"
		
		else	// fileNamePtr == NULL
			m_initializedFlag = FALSE;

		}	// end "if (m_initializedFlag)"

	Centre ();
	if (!m_initializedFlag)
		return;

}	// end "CMDisplaySpecsDlg"



CMDisplaySpecsDlg::~CMDisplaySpecsDlg ()

{
	m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);
 
}	// end "~CMDisplaySpecsDlg"



void CMDisplaySpecsDlg::CreateControls ()

{
	wxWindow* parentWindow = this;

	SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer67 = new wxBoxSizer (wxVERTICAL);

	m_staticimagename = new wxStaticText (parentWindow,
														ID3C_FileName,
														wxT("image name"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_staticimagename->Wrap (-1);
	bSizer67->Add (m_staticimagename, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT,12));
	bSizer67->Add (20,6);

	wxBoxSizer* bSizer78;
	bSizer78 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer731;
	bSizer731 = new wxBoxSizer (wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (parentWindow,
																		wxID_ANY,
																		wxT("Area to Display"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxHORIZONTAL);

	CreateLineColumnControls (sbSizer8);
	
	bSizer731->Add (sbSizer8, wxSizerFlags(1).Expand());

	bSizer78->Add (bSizer731, 0, wxEXPAND);

	bSizer67->Add (bSizer78, wxSizerFlags(0).Expand().Border(wxLEFT,12));
	bSizer67->Add (20,6);

	wxBoxSizer* bSizer79;
	bSizer79 = new wxBoxSizer (wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer (new wxStaticBox (parentWindow,
																		wxID_ANY,
																		wxT("Display"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
													wxVERTICAL);
	wxStaticBox* sbSizerStaticBox9 = sbSizer9->GetStaticBox ();

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer (0, 1, 0, 0);
	fgSizer3->SetFlexibleDirection (wxBOTH);
	fgSizer3->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* bSizer732;
	bSizer732 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText701 = new wxStaticText (sbSizerStaticBox9,
													wxID_ANY, wxT("Type:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText701->Wrap (-1);
   SetUpToolTip (m_staticText701, IDS_ToolTip25);
	bSizer732->Add (
					m_staticText701, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	wxChoice*	displaytypectrl;
	displaytypectrl = new wxChoice (sbSizerStaticBox9,
													ID3C_DisplayType,
													wxDefaultPosition,
													wxSize (190, -1));
	displaytypectrl->Append (wxT("1-Channel Thematic"));
	displaytypectrl->Append (wxT("1-Channel Grayscale"));
	//displaytypectrl->Append (wxT("2-Channel Color (R-G)"));
	//displaytypectrl->Append (wxT("2-Channel Color (G-B)"));
	//displaytypectrl->Append (wxT("2-Channel Color (R-B)"));
	displaytypectrl->Append (wxT("3-Channel Color"));
	displaytypectrl->Append (wxT("Side by Side Channels"));
   SetUpToolTip (displaytypectrl, IDS_ToolTip25);
	bSizer732->Add (displaytypectrl, 0, wxALL, 5);

	fgSizer3->Add (bSizer732, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer741;
	bSizer741 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText72 = new wxStaticText (sbSizerStaticBox9,
													wxID_ANY,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_staticText72, IDS_ToolTip26);
	m_staticText72->Wrap (-1);
	bSizer741->Add (m_staticText72, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

   GetAllSubsetMenuControl (kChannelsMenu,
										sbSizerStaticBox9,
										ID3C_Channels,
										120,
										0);
	m_channelsCtrl->Hide ();
   bSizer741->Add (
   		m_channelsCtrl,
			wxSizerFlags(0).ReserveSpaceEvenIfHidden().CenterVertical().Border(wxALL, 5));

	fgSizer3->Add (bSizer741, 1, wxEXPAND, 5);

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer (3, 3, 0, 0);
	wxBoxSizer* rgsizer1 = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer* rgsizer2 = new wxBoxSizer (wxHORIZONTAL);
	m_redpromptctrl = new wxStaticText (sbSizerStaticBox9,
													ID3C_RedPrompt,
													wxT("Red:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_redpromptctrl->Wrap (-1);
   SetUpToolTip (m_redpromptctrl, IDS_ToolTip27);
	rgsizer1->Add (
					m_redpromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_redchannelctrl = new wxTextCtrl (sbSizerStaticBox9,
													ID3C_RedChannel,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_redchannelctrl, IDS_ToolTip27);
	m_redchannelctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																		&m_RedChannelString));
	rgsizer2->Add (m_redchannelctrl, 0, wxALL, 5);

	m_grayctrl = new wxTextCtrl (sbSizerStaticBox9,
											ID3C_GrayChannel,
											wxEmptyString,
											wxDefaultPosition,
											wxDefaultSize,
											0);
	m_grayctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_GrayChannelString));
	m_grayctrl->Hide ();

	rgsizer2->Add (m_grayctrl, 0, wxALL, 5);

	m_graypromptctrl = new wxStaticText (sbSizerStaticBox9,
														ID3C_GrayPrompt,
														wxT("Grey"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_graypromptctrl->Wrap (-1);
	m_graypromptctrl->Hide ();
	rgsizer1->Add (m_graypromptctrl, 0, wxALIGN_CENTER | wxALL, 5);

	gSizer3->Add (rgsizer1);
	gSizer3->Add (rgsizer2);

	m_redchaninvctrl = new wxCheckBox (sbSizerStaticBox9,
													IDC_RedChannelInvert,
													wxT("Invert"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_redchaninvctrl, IDS_ToolTip28);
	m_redchaninvctrl->SetValidator (wxGenericValidator (&m_redChannelInvertFlag));
	gSizer3->Add (m_redchaninvctrl, 0, wxALL, 5);

	m_greenpromptctrl = new wxStaticText (sbSizerStaticBox9,
														ID3C_GreenPrompt,
														wxT("Green:"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
   SetUpToolTip (m_greenpromptctrl, IDS_ToolTip29);
	m_greenpromptctrl->Wrap (-1);
	gSizer3->Add (
				m_greenpromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_greenchannelctrl = new wxTextCtrl (sbSizerStaticBox9,
														ID3C_GreenChannel,
														wxEmptyString,
														wxDefaultPosition,
														wxDefaultSize,
														0);
   SetUpToolTip (m_greenchannelctrl, IDS_ToolTip29);
	m_greenchannelctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																		&m_GreenChannelString));
	gSizer3->Add(m_greenchannelctrl, 0, wxALL, 5);

	m_greenchaninvctrl = new wxCheckBox (sbSizerStaticBox9,
														IDC_GreenChannelInvert,
														wxT("Invert"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
   SetUpToolTip (m_greenchaninvctrl, IDS_ToolTip28);
	m_greenchaninvctrl->SetValidator (wxGenericValidator (&m_greenChannelInvertFlag));

	gSizer3->Add(m_greenchaninvctrl, 0, wxALL, 5);

	m_bluepromptctrl = new wxStaticText (sbSizerStaticBox9,
														ID3C_BluePrompt,
														wxT("Blue:"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_bluepromptctrl->Wrap (-1);
   SetUpToolTip (m_bluepromptctrl, IDS_ToolTip30);
	gSizer3->Add(m_bluepromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_bluechannelctrl = new wxTextCtrl (sbSizerStaticBox9,
													ID3C_BlueChannel,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_bluechannelctrl, IDS_ToolTip30);
	m_bluechannelctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																		&m_BlueChannelString));
	gSizer3->Add (m_bluechannelctrl, 0, wxALL, 5);

	m_bluechaninvctrl = new wxCheckBox (sbSizerStaticBox9,
													IDC_BlueChannelInvert,
													wxT("Invert"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_bluechaninvctrl, IDS_ToolTip28);
	m_bluechaninvctrl->SetValidator (wxGenericValidator (&m_blueChannelInvertFlag));
	gSizer3->Add (m_bluechaninvctrl, 0, wxALL, 5);

	fgSizer3->Add (gSizer3, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer76;
	bSizer76 = new wxBoxSizer (wxHORIZONTAL);

	fgSizer3->Add(bSizer76, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer (wxHORIZONTAL);

	m_legendpromptctrl = new wxStaticText (sbSizerStaticBox9,
														IDC_LegendFactorPrompt,
														wxT("Legend Factor"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_legendpromptctrl->Wrap (-1);
	m_legendpromptctrl->Hide ();

	bSizer81->Add (
				m_legendpromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_legendfactorctrl = new wxTextCtrl (sbSizerStaticBox9,
														IDC_LegendFactor,
														wxT("1"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	wxFloatingPointValidator<double> doubleValue8Digits (8, &m_double8DigitValueCheck);
   m_legendfactorctrl->SetValidator (doubleValue8Digits);
	m_legendfactorctrl->Hide ();

	bSizer81->Add (m_legendfactorctrl, 0, wxALL, 5);

	fgSizer3->Add (bSizer81, 1, wxEXPAND, 5);

	m_chandescctrl = new wxButton (sbSizerStaticBox9,
												ID3C_ChannelDescriptions,
												wxT("Channel Descriptions..."),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_chandescctrl, IDS_ToolTip32);

	fgSizer3->Add(m_chandescctrl, 0, wxALIGN_CENTER | wxALL, 5);

	wxBoxSizer* bSizer75;
	bSizer75 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText76 = new wxStaticText (sbSizerStaticBox9,
													wxID_ANY,
													wxT("Magnification:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText76->Wrap (-1);
   SetUpToolTip (m_staticText76, IDS_ToolTip33);
	bSizer75->Add (m_staticText76, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxFloatingPointValidator<double> _val (3, &m_Magnification);
	_val.SetMin (0);
	m_magnificationctrl = new wxTextCtrl (sbSizerStaticBox9,
														ID3C_Magnification,
														wxEmptyString,
														wxDefaultPosition,
														wxDefaultSize,
														0);
   SetUpToolTip (m_magnificationctrl, IDS_ToolTip33);
	m_magnificationctrl->SetValidator (_val);
	bSizer75->Add (m_magnificationctrl, 0, wxALL, 5);
	
	fgSizer3->Add (bSizer75, 1, wxEXPAND, 5);

	m_checkBox17 = new wxCheckBox (sbSizerStaticBox9,
												IDC_VectorOverlays,
												wxT("Include Vector Overlays"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox17, IDS_ToolTip31);
	fgSizer3->Add (m_checkBox17, 0, wxALL, 5);

	sbSizer9->Add (fgSizer3, 1, wxEXPAND, 5);

	bSizer79->Add (sbSizer9, wxSizerFlags(0).Expand().Border(wxRIGHT,6));

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer73;
	bSizer73 = new wxBoxSizer (wxVERTICAL);

	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer (new wxStaticBox (parentWindow,
																		wxID_ANY,
																		wxT("Enhancement")),
													wxVERTICAL);

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer (0, 2, 0, 0);

	wxStaticText* staticText671 = new wxStaticText (sbSizer10->GetStaticBox (),
																	wxID_ANY,
																	wxT("Bits of color:"),
																	wxDefaultPosition,
																	wxDefaultSize,
																	0);
	staticText671->Wrap (-1);
	SetUpToolTip (staticText671, IDS_ToolTip34);
	gSizer2->Add (staticText671, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);
	
	m_staticText67 = new wxStaticText (sbSizer10->GetStaticBox (),
													ID3C_BitsOfColor,
													wxT("24"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText67->Wrap (-1);
	gSizer2->Add (m_staticText67, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);
	/*
	wxString m_bitsofcolctrlChoices[] = {wxT("8"), wxT("24")};
	int m_bitsofcolctrlNChoices = sizeof (m_bitsofcolctrlChoices) / sizeof (wxString);
	m_bitsofcolctrl = new wxChoice (sbSizer10->GetStaticBox (),
												ID3C_BitsOfColor,
												wxDefaultPosition,
												wxDefaultSize,
												m_bitsofcolctrlNChoices,
												m_bitsofcolctrlChoices,
												0);
	m_bitsofcolctrl->SetSelection (0);
   SetUpToolTip (m_bitsofcolctrl, IDS_ToolTip34);
	m_bitsofcolctrl->SetValidator (wxGenericValidator (&m_BitsOfColor));
	
	gSizer2->Add(m_bitsofcolctrl, 0, wxALL, 5);
	*/
	m_staticText68 = new wxStaticText (sbSizer10->GetStaticBox (),
													wxID_ANY,
													wxT("Stretch:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText68->Wrap (-1);
   SetUpToolTip (m_staticText68, IDS_ToolTip35);
	gSizer2->Add (m_staticText68, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);
	
	#if defined multispec_wxlin
		m_enhancementCtrl = new wxComboBox (sbSizer10->GetStaticBox (),
														ID3C_Enhancement,
														wxT("Linear"),
														wxDefaultPosition,
														wxDefaultSize,
														0,
														NULL,
														wxCB_READONLY);
	#endif	// multispec_wxlin
	#if defined multispec_wxmac
		m_enhancementCtrl = new wxChoice (sbSizer10->GetStaticBox (),
														ID3C_Enhancement,
														wxDefaultPosition,
														wxDefaultSize);
	#endif	// multispec_wxmac
	
   m_enhancementCtrl->Append (wxT("Linear"));
   m_enhancementCtrl->Append (wxT("Equal Area"));
   m_enhancementCtrl->Append (wxT("Gaussian  +\u21E7..."));
   
   SetUpToolTip (m_enhancementCtrl, IDS_ToolTip35);
	m_enhancementCtrl->SetSelection (0);
	m_enhancementCtrl->SetValidator (wxGenericValidator (&m_Enhancement));
	gSizer2->Add (m_enhancementCtrl, 0, wxALL, 5);

	m_staticText69 = new wxStaticText (sbSizer10->GetStaticBox (),
													wxID_ANY,
													wxT("Min-max:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText69->Wrap (-1);
   SetUpToolTip (m_staticText69, IDS_ToolTip36);
	gSizer2->Add (m_staticText69, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	#if defined multispec_wxlin
  		m_minMaxCtrl = new wxComboBox (sbSizer10->GetStaticBox (),
													ID3C_MinMaxValues,
													wxT("Clip 0% of tails"),
													wxDefaultPosition,
													wxDefaultSize,
													0,
													NULL,
													wxCB_READONLY);
	#endif	// multispec_wxlin
	#if defined multispec_wxmac
  		m_minMaxCtrl = new wxChoice (sbSizer10->GetStaticBox (),
   											ID3C_MinMaxValues,
   											wxDefaultPosition,
   											wxDefaultSize);
	#endif	// multispec_wxmac
	m_minMaxCtrl->Append (wxT("Clip 0% of tails"));
	m_minMaxCtrl->Append (wxT("Clip 2% of tails"));
	m_minMaxCtrl->Append (wxT("Entire Range"));
	m_minMaxCtrl->Append (wxT("User Specified..."));
	m_minMaxCtrl->SetSelection (0);
   SetUpToolTip (m_minMaxCtrl, IDS_ToolTip36);
	gSizer2->Add (m_minMaxCtrl, 0, wxALL, 5);

	m_staticText70 = new wxStaticText (sbSizer10->GetStaticBox (),
													wxID_ANY,
													wxT("Treat '0' as:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText70->Wrap (-1);
   SetUpToolTip (m_staticText70, IDS_ToolTip37);
	gSizer2->Add (m_staticText70, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	wxString m_treatzeroctrlChoices[] = {wxT("Background"), wxT("Data")};
	int m_treatzeroctrlNChoices = sizeof (m_treatzeroctrlChoices) / sizeof (wxString);
	m_treatzeroctrl = new wxChoice (sbSizer10->GetStaticBox (),
												ID3C_TreatZeroAs,
												wxDefaultPosition,
												wxDefaultSize,
												m_treatzeroctrlNChoices,
												m_treatzeroctrlChoices,
												0);
	m_treatzeroctrl->SetSelection (0);
   SetUpToolTip (m_treatzeroctrl, IDS_ToolTip37);
	m_treatzeroctrl->SetValidator (wxGenericValidator (&m_TreatZeroAs));

	gSizer2->Add(m_treatzeroctrl, 0, wxALL, 5);

	m_staticText681 = new wxStaticText (sbSizer10->GetStaticBox (),
													wxID_ANY,
													wxT("Number of display levels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText681->Wrap (-1);
   SetUpToolTip (m_staticText681, IDS_ToolTip38);
	gSizer2->Add (m_staticText681, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

   UInt16 value;
   wxIntegerValidator<UInt16> val (&value, wxNUM_VAL_DEFAULT);
   val.SetRange (1, 256);
	m_noofdisplevelsctrl = new wxComboBox (sbSizer10->GetStaticBox (),
														ID3C_NumberOfLevels, wxT("256"),
														wxDefaultPosition,
														wxDefaultSize,
														0,
														NULL,
														0,
														val);	//,wxTE_PROCESS_ENTER);
	m_noofdisplevelsctrl->Append (wxT("6"));
	m_noofdisplevelsctrl->Append (wxT("15"));
	m_noofdisplevelsctrl->Append (wxT("32"));
	m_noofdisplevelsctrl->Append (wxT("254"));
	m_noofdisplevelsctrl->Append (wxT("256"));
   SetUpToolTip (m_noofdisplevelsctrl, IDS_ToolTip38);
	gSizer2->Add (m_noofdisplevelsctrl, 0, wxALL, 5);

	m_newhistctrl = new wxCheckBox (sbSizer10->GetStaticBox (),
												ID3C_NewHistogram,
												wxT("Load New Histogram"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_newhistctrl, IDS_ToolTip39);
	m_newhistctrl->SetValidator (wxGenericValidator (&m_ComputeHistogramFlag));

	gSizer2->Add (m_newhistctrl, wxSizerFlags(0).Border(wxALL, 5));

	sbSizer10->Add (gSizer2, wxSizerFlags(1).Expand());

	bSizer73->Add (sbSizer10, wxSizerFlags(1).Expand());
	
	bSizer72->Add (bSizer73, wxSizerFlags(1).Expand());

	bSizer79->Add (bSizer72, wxSizerFlags(0).Expand().Border(wxLEFT,6));

	bSizer67->Add (bSizer79, wxSizerFlags(1).Expand().Border(wxLEFT|wxRIGHT,12));

	CreateStandardButtons (bSizer67);

	SetSizerAndFit (bSizer67);
	
			// Set focus and tab order
	
   SelectDialogItemText ((DialogPtr)sbSizerStaticBox9,
   								ID3C_RedChannel,
   								0,
   								SInt16_MAX);
	
	m_greenchannelctrl->MoveAfterInTabOrder (m_redchannelctrl);
	m_bluechannelctrl->MoveAfterInTabOrder (m_greenchannelctrl);
	
	//m_redchaninvctrl->MoveAfterInTabOrder (m_bluechannelctrl);
	//m_greenchaninvctrl->MoveAfterInTabOrder (m_redchaninvctrl);
	//m_bluechaninvctrl->MoveAfterInTabOrder (m_greenchaninvctrl);
	
}	// end "CreateControls"



void CMDisplaySpecsDlg::CheckChannel (
				int									itemID,
				UInt32*								channelValuePtr)

{
	SInt32								newValue;

	SInt16								numberLevels;


	newValue = (UInt32)CheckValue (itemID, 1, gImageWindowInfoPtr->totalNumberChannels);
	
	if (newValue > 0 && newValue <= gImageWindowInfoPtr->totalNumberChannels) {
		*channelValuePtr = newValue;

				// Verify number of levels setting

		DisplayMultispectralDialogCheckDisplayLevels (m_displaySpecsPtr,
																	  this,
																	  mLocalDisplayType,
																	  m_RedChannel,
																	  m_GreenChannel,
																	  m_BlueChannel,
																	  (SInt16*)&mColorLevelsMax[0][0],
																	  &mDuplicateChannelCode,
																	  (SInt16)m_BitsOfColor + 1,
																	  &numberLevels);

		m_NumberLevels = numberLevels;

		}	// end "if (newValue > 0 &&"

}	// end "CheckChannel"



Boolean CMDisplaySpecsDlg::DoDialog (
				DisplaySpecsPtr 					displaySpecsPtr)

{
	Boolean 								continueFlag = FALSE;
	SInt16 								returnCode;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
        																				return (FALSE);

	m_displaySpecsPtr = displaySpecsPtr;
	returnCode = ShowModal ();

	if (returnCode == wxID_OK)
	 	{
				// Display area

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		DisplayMultispectralDialogOK (this,
												 displaySpecsPtr,
												 gImageWindowInfoPtr,
												 gImageFileInfoPtr,
												 &m_dialogSelectArea,
												 m_localFeaturesPtr,
												 m_localActiveNumberFeatures,
												 m_PercentClip,
												 &mMinMaxValues[0],
												 mDuplicateChannelCode,
												 m_GrayChannel,
												 m_RedChannel,
												 m_GreenChannel,
												 m_BlueChannel,
												 mRGBColors,
												 mLocalDisplayType,
												 m_DisplayType + 1,
												 mBitsOfColorIndex + 1,
												 m_Magnification,
												 (SInt16*)&mPixelSizeVector[0],
												 m_gaussianStretch,
												 m_Enhancement + 1,
												 m_MinMaxCode,
												 m_TreatZeroAs, // + 1,
												 //m_NumberLevels,
												 m_numberLevelsval,
												 (SInt16*)&mColorLevelsMax[0][0],
												 m_ComputeHistogramFlag,
												 m_channelSelection,
												 m_redChannelInvertFlag,
												 m_greenChannelInvertFlag,
												 m_blueChannelInvertFlag,
												 m_thematicLegendFactor,
												 m_vectorOverlaysFlag);

		if (gActiveImageWindow->m_frame != NULL)
         gActiveImageWindow->m_frame->m_frameMaximized = false;
        
        continueFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetComputeHistogramDialogSetting
//
//	Software purpose:	The purpose of this routine is to determine the setting to
//							use for the compute histogram dialog item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/2012
//	Revised By:			Larry L. Biehl			Date: 06/06/2019

Boolean CMDisplaySpecsDlg::GetComputeHistogramDialogSetting (
				DisplaySpecsPtr 					displaySpecsPtr,
				SInt16 								lEnhancement,
				SInt16 								lMinMaxCode)

{
	Boolean 								setComputeItemFlag;


			// Initialize local variables.

	setComputeItemFlag = FALSE;

	if (!displaySpecsPtr->defaultHistogramFileFlag)
		{
		if (lEnhancement == kEqualAreaStretch)
			setComputeItemFlag = TRUE;

		if (lEnhancement == kLinearStretch &&
				 displaySpecsPtr->histogramCompute == 1 &&
						lMinMaxCode != kUserSpecified)
			setComputeItemFlag = TRUE;

		}	// end "if (!displaySpecsPtr->defaultHistogramFileFlag)"

	return (setComputeItemFlag);

}	// end "GetComputeHistogramDialogSetting"



UInt16 CMDisplaySpecsDlg::GetMaximumDisplayLevels (
				SInt16 								bitsOfColorIndex,
				SInt16 								displayType,
				SInt16 								duplicateChannelCode)

{
	UInt16 								numberChannelsIndex = 0;


	switch (displayType)
	 	{
		case k1_ChannelThematicDisplayType:
		case k1_ChannelGrayLevelDisplayType:
			numberChannelsIndex = 0;
			break;

		case k3_ChannelDisplayType:
			if (duplicateChannelCode)
				numberChannelsIndex = 1;

			else
				numberChannelsIndex = 2;
			break;

		case kSideSideChannelDisplayType:
			numberChannelsIndex = 0;
			break;

		}	// end "switch (displayType)"

	return (mColorLevelsMax[bitsOfColorIndex][numberChannelsIndex]);

}	// end "GetMaximumDisplayLevels"



void CMDisplaySpecsDlg::OnChangeBlueChannel (
				wxCommandEvent& 					event)

{
		// Get current blue channel

	CheckChannel (ID3C_BlueChannel, &m_BlueChannel);

}	// end "OnChangeBlueChannel"



void CMDisplaySpecsDlg::OnChangeGrayChannel (
				wxCommandEvent& 					event)

{
	CheckChannel (ID3C_GrayChannel, &m_GrayChannel);

}	// end "OnChangeGrayChannel"



void CMDisplaySpecsDlg::OnChangeGreenChannel (
				wxCommandEvent& 					event)

{
			// Get current green channel

	CheckChannel (ID3C_GreenChannel, &m_GreenChannel);

}	// end "OnChangeGreenChannel"


void CMDisplaySpecsDlg::OnChangeRedChannel (
				wxCommandEvent& 					event)

{
			// Get current red channel

	CheckChannel (ID3C_RedChannel, &m_RedChannel);

}	// end "OnChangeRedChannel"



void CMDisplaySpecsDlg::OnChannelDescriptions (
				wxCommandEvent& 					event)

{
	CMChannelsDlg* 					channelsDialogPtr = NULL;

	SInt16 								tempInt;
			// Show channel list.

	SetDLogControlHilite (NULL, wxID_OK, 255);

	channelsDialogPtr = new CMChannelsDlg (this);

	channelsDialogPtr->DoDialog (&tempInt,
											NULL,
											gImageLayerInfoPtr,
											gImageFileInfoPtr,
											2,
											kNoTransformation,
											NULL,
											(SInt16)gImageWindowInfoPtr->totalNumberChannels,
											kSubsetMenuItem);

	if (channelsDialogPtr != NULL)
		delete channelsDialogPtr;

	SetDLogControlHilite (NULL, wxID_OK, 0);

}	// end "OnChannelDescriptions"


 
void CMDisplaySpecsDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16 								bitsOfColor,
											blueChannel,
											channelSelection,
											displayType,
											enhancement,
											grayChannel,
											greenChannel,
											localNumberFeatures,
											minMaxPopupCode,
											numberLevels,
											redChannel,
											treatZeroAs;
	
	Boolean								blueChannelInvertFlag,
											computeHistogramFlag,
											greenChannelInvertFlag,
											redChannelInvertFlag,
											vectorOverlaysFlag;
	
    
	DisplayMultispectralDialogInitialize (this,
														m_displaySpecsPtr,
														gImageWindowInfoPtr,
														&m_dialogSelectArea,
														m_localFeaturesPtr,
														&localNumberFeatures,
														&m_PercentClip,
														&mMinMaxValues[0],
														&mDuplicateChannelCode,
														&grayChannel,
														&redChannel,
														&greenChannel,
														&blueChannel,
														&mRGBColors,
														&mLocalDisplayType,
														&mMaxSystemPixelSize,
														&displayType,
														&bitsOfColor,
														&m_SavedBitsOfColor,
														&m_Magnification,
														(SInt16*)&mPixelSizeVector[0],
														&m_gaussianStretch,
														&enhancement,
														&m_MinMaxCode,
														&minMaxPopupCode,
														&m_thematicDisplayWithUserSettingFlag,
														&treatZeroAs,
														(SInt16*)&mColorLevelsMax[0][0],
														&numberLevels,
														&computeHistogramFlag,
														&channelSelection,
														&redChannelInvertFlag,
														&greenChannelInvertFlag,
														&blueChannelInvertFlag,
														&m_thematicLegendFactor,
														&vectorOverlaysFlag);

    if (m_Magnification > 1 && m_Magnification != gActiveImageViewCPtr->m_Scale)
        m_Magnification = 1;   

    m_localNumberFeatures = localNumberFeatures;
    m_GrayChannel = grayChannel;
    m_RedChannel = redChannel;
    m_GreenChannel = greenChannel;
    m_BlueChannel = blueChannel;
    m_DisplayType = displayType - 1;
    m_BitsOfColor = bitsOfColor - 1;
    m_Enhancement = enhancement - 1;
	
	m_MinMaxPopupCode = minMaxPopupCode - 1;
	m_TreatZeroAs = treatZeroAs;
	m_channelSelection = channelSelection;
	m_NumberLevels = numberLevels;

	m_LineStart = m_displaySpecsPtr->lineStart;
	m_LineEnd = m_displaySpecsPtr->lineEnd;
	m_LineInterval = m_displaySpecsPtr->lineInterval;
	m_ColumnStart = m_displaySpecsPtr->columnStart;
	m_ColumnEnd = m_displaySpecsPtr->columnEnd;
	m_ColumnInterval = m_displaySpecsPtr->columnInterval;

	m_ComputeHistogramFlag = computeHistogramFlag;
	
	m_redChannelInvertFlag = redChannelInvertFlag;
	m_greenChannelInvertFlag = greenChannelInvertFlag;
	m_blueChannelInvertFlag = blueChannelInvertFlag;
	
	m_vectorOverlaysFlag = vectorOverlaysFlag;
    
			// Change the value of mLocalDisplayType
	
   if (m_DisplayType == 2)
   	{
		m_staticText67->SetLabel (wxT("24"));
		m_BitsOfColor = 2; // color image
		
    	}	// end "if (mLocalDisplayType > 2 || mLocalDisplayType < 7)"
	
	else	// m_DisplayType != 2
		{
		m_staticText67->SetLabel (wxT("8"));
		m_BitsOfColor = 0; // grey  image
		
    	}	// end "else mLocalDisplayType <= 2 && ..."

    		// Set limit values

    mMaxChannelNumber = gImageWindowInfoPtr->totalNumberChannels;

    		// Make sure that we have the bitmaps for the entire image buttons.

    SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);

    		// Set feature parameters

    m_localActiveNumberFeatures = m_localNumberFeatures;
    m_localActiveFeaturesPtr = m_localFeaturesPtr;

    		// Update the display specification dialog with the information
    		// given in the image specification

	TransferDataToWindow ();

			// Set up edit text fields for channels to be used for diplay
	
	SetSizerAndFit (bSizer67);
	UpdateDialogChannelItems (this, mRGBColors, mLocalDisplayType);
   
	Layout();
	Fit();
	PositionDialogWindow ();

}	// end "OnInitDialog"



void CMDisplaySpecsDlg::OnChannelsComboCloseUp (
				wxCommandEvent& 					event)

{
	wxComboBox* channelsComboPtr = (wxComboBox*)FindWindow (ID3C_Channels);
	int channelSelection = channelsComboPtr->GetSelection ();
	
	if (channelSelection == -1)
		channelsComboPtr->SetSelection (m_channelSelection);
	
	event.Skip ();
	
}	// end "OnChannelsComboCloseUp"



void CMDisplaySpecsDlg::OnChannelsComboDropDown (
				wxCommandEvent& 					event)

{
	wxComboBox* channelsComboPtr = (wxComboBox*)FindWindow (ID3C_Channels);
	channelsComboPtr->SetSelection (-1);

}	// end "OnChannelsComboDropDown"



void CMDisplaySpecsDlg::OnChannelsSelendok (wxCommandEvent& event)

{
	HandleChannelsMenu (ID3C_Channels,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);

}	// end "OnChannelsSelendok"



void CMDisplaySpecsDlg::OnSelendokDisplayType (
				wxCommandEvent& 					event)

{
	SInt16* 								colorLevelsMaxPtr;

	SInt32 								blueChannel,
            							greenChannel,
            							redChannel;

	SInt16 								bitsOfColor,
            							currentDisplayTypeMenuSelection,
            							minMaxPopupCode,
            							numberDisplayLevels;

	Boolean 								checkMinMaxSettingFlag;


	currentDisplayTypeMenuSelection = m_DisplayType + 1;
	
	m_DisplayType = ((wxChoice*)FindWindow (ID3C_DisplayType))->GetSelection ();

	blueChannel = m_BlueChannel;
	greenChannel = m_GreenChannel;
	redChannel = m_RedChannel;
	bitsOfColor = m_BitsOfColor + 1; // Make index from 1 not 0

	colorLevelsMaxPtr = (SInt16*)&mColorLevelsMax[0][0];

	DisplayMultispectralDialogUpdateDisplayType (m_displaySpecsPtr,
																	this,
																	currentDisplayTypeMenuSelection,
																	m_DisplayType + 1,
																	m_SavedBitsOfColor,
																	&mLocalDisplayType,
																	&mRGBColors,
																	&bitsOfColor,
																	mMaxSystemPixelSize,
																	&numberDisplayLevels,
																	redChannel,
																	greenChannel,
																	blueChannel,
																	colorLevelsMaxPtr,
																	&mDuplicateChannelCode,
																	&checkMinMaxSettingFlag);

    		// Check min-max menu settings.

	minMaxPopupCode = m_MinMaxPopupCode + 1;
	DisplayMultispectralDialogCheckMinMaxSettings (
															m_DisplayType + 1,
															m_MinMaxPopupCode + 1,
															&minMaxPopupCode,
															&m_MinMaxCode,
															&m_thematicDisplayWithUserSettingFlag);

	m_MinMaxPopupCode = minMaxPopupCode - 1;
	m_minMaxCtrl->SetSelection (m_MinMaxPopupCode);

	DisplayMultispectralDialogUpdateComputeHistogram (m_displaySpecsPtr,
																		m_ComputeHistogramFlag,
																		m_DisplayType + 1,
																		m_Enhancement + 1,
																		m_MinMaxPopupCode + 1,
																		this,
																		ID3C_NewHistogram);

    		// This function changes the value of mLocalDisplayType
    		// Handling OnDropDownBitsofcolor here

	if (m_DisplayType == 2)
    	{
    			// m_DisplayType count from 0
				// Remove the '8' item from the bits of color list.
		
		m_staticText67->SetLabel (wxT("24"));
		m_BitsOfColor = 2; // color image
            
    	}	// end "if (m_DisplayType == 2)"
	
	else // m_DisplayType < 2 || m_DisplayType > 2
		{
		m_staticText67->SetLabel (wxT("8"));
		m_BitsOfColor = 0; // grey  image
		
		}	// end "else m_DisplayType < 2 || m_DisplayType > 2"
	
	m_NumberLevels = numberDisplayLevels;

			// Get index into 8, 16, 24 bits of color vector.

	mBitsOfColorIndex = Update8_16_24BitsOfColorIndex (m_BitsOfColor);
    
	Layout ();

}	// end "OnSelendokDisplayType"



void CMDisplaySpecsDlg::OnEnhancementComboSelendok (
				wxCommandEvent& 					event)

{
	wxMouseState 						mousestate;
	
	
   m_Enhancement = event.GetSelection ();

	if (m_Enhancement == 2 && (wxGetKeyState (WXK_SHIFT) || mousestate.RightIsDown ()))
    	{
		SetDLogControlHilite (NULL, wxID_OK, 255);

		GaussianParameterDialog (&m_gaussianStretch);

		SetDLogControlHilite (NULL, wxID_OK, 0);

		}	// end "if (m_localMinMaxCode == 2)"

    DisplayMultispectralDialogUpdateComputeHistogram (m_displaySpecsPtr,
																		m_ComputeHistogramFlag,
																		m_DisplayType + 1,
																		m_Enhancement + 1,
																		m_MinMaxPopupCode + 1,
																		this,
																		ID3C_NewHistogram);
	
}	// end "OnEnhancementComboSelendok"



void CMDisplaySpecsDlg::OnEnhancementComboCloseUp (
				wxCommandEvent& event)

{
   //int enhancement = event.GetSelection ();


			// set to -1 so that it can be active again when user selects it second
			// time directly
	
	//m_enhancementctrl->SetSelection (m_Enhancement);
	//m_enhancementctrl->SetSelection (-1);
	
}	// end "OnEnhancementComboCloseUp"



void CMDisplaySpecsDlg::OnEnhancementComboDropDown (
				wxCommandEvent& 					event)

{

	m_optionKeyFlag = FALSE;
    
	wxMouseState mousestate;
	
	if (mousestate.RightIsDown () || wxGetKeyState (WXK_SHIFT))
		m_optionKeyFlag = TRUE;
	
			// Set to -1 so that enhancement option can be active again when user selects it.
	
	if (m_Enhancement == 2)
		m_enhancementCtrl->SetSelection (-1);
	
	else	// enhancement != 2
		m_enhancementCtrl->SetSelection (m_Enhancement);
	
}	// end "OnEnhancementComboDropDown"



void CMDisplaySpecsDlg::OnSelendokMinMaxValues (
				wxCommandEvent& 					event)

{
	SInt16 								channels[3];


	m_MinMaxPopupCode = m_minMaxCtrl->GetSelection ();
  
	if (m_MinMaxPopupCode == 0)
		{
		m_MinMaxCode = kPerChannelMinMax;
		m_PercentClip = 0;

		}	// end "if (m_MinMaxPopupCode == 0)"

	else if (m_MinMaxPopupCode == 1)
		{
		m_MinMaxCode = kPerChannelMinMax;
		m_PercentClip = 2;

		}	// end "if (m_MinMaxPopupCode == 1)"

	else if (m_MinMaxPopupCode == 2)
		m_MinMaxCode = kEntireDataRange;

	else if (m_MinMaxPopupCode == 3)
		{
		SetDLogControlHilite (NULL, wxID_OK, 255);
      
		channels[0] = (SInt16)m_RedChannel;
		channels[1] = (SInt16)m_GreenChannel;
		channels[2] = (SInt16)m_BlueChannel;

		if (mLocalDisplayType <= 2)
			channels[0] = (SInt16)m_GrayChannel;

		MinMaxEnhancementDialog (channels,
										 mRGBColors,
										 mLocalDisplayType,
										 m_NumberLevels,
										 &m_PercentClip,
										 &m_MinMaxCode,
										 mMinMaxValues);

		m_MinMaxPopupCode = GetMinMaxPopupCode (m_MinMaxCode, m_PercentClip);
		m_minMaxCtrl->SetSelection (m_MinMaxPopupCode);

		SetDLogControlHilite (NULL, wxID_OK, 0);

		}	// end "if (m_localMinMaxCode == 3)"

	else if (m_MinMaxPopupCode == 4)
		m_MinMaxCode = kThematicDefault;

	DisplayMultispectralDialogUpdateComputeHistogram (m_displaySpecsPtr,
																		m_ComputeHistogramFlag,
																		m_DisplayType + 1,
																		m_Enhancement + 1,
																		m_MinMaxCode,
																		this,
																		ID3C_NewHistogram);

}	// end "OnSelendokMinMaxValues"



void CMDisplaySpecsDlg::OnSelendokMinMaxValuesCloseUp (
				wxCommandEvent& 					event)

{
	event.Skip ();

			// set to -1 so that it can be active again when user selects it second
			// time directly
	
	//m_enhancementctrl->SetSelection (m_Enhancement);
	//m_enhancementctrl->SetSelection (-1);
	
}	// end "OnSelendokMinMaxValuesCloseUp"



void CMDisplaySpecsDlg::OnSelendokMinMaxValuesDropDown (
				wxCommandEvent& 					event)

{
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.
	
	if (m_MinMaxPopupCode == 3)
		m_minMaxCtrl->SetSelection (-1);
	
}	// end "OnSelendokMinMaxValuesDropDown"



bool CMDisplaySpecsDlg::TransferDataFromWindow ()

{
	SInt16								returnCode;
	
	

	wxCheckBox* rinv = (wxCheckBox*)FindWindow (IDC_RedChannelInvert);
	m_redChannelInvertFlag = rinv->GetValue ();
	
	wxCheckBox* ginv = (wxCheckBox*)FindWindow (IDC_GreenChannelInvert);
	m_greenChannelInvertFlag = ginv->GetValue ();
	
	wxCheckBox* binv = (wxCheckBox*)FindWindow (IDC_BlueChannelInvert);
	m_blueChannelInvertFlag = binv->GetValue ();
	
	wxCheckBox* voverlays = (wxCheckBox*)FindWindow (IDC_VectorOverlays);
	m_vectorOverlaysFlag = voverlays->GetValue ();
	
	wxCheckBox* newhist = (wxCheckBox*)FindWindow (ID3C_NewHistogram);
	m_ComputeHistogramFlag = newhist->GetValue ();
	
   if (m_enhancementCtrl->GetSelection () > -1)
      m_Enhancement = m_enhancementCtrl->GetSelection ();
	
	wxChoice* tzeroas = (wxChoice*)FindWindow (ID3C_TreatZeroAs);
	m_TreatZeroAs = tzeroas->GetSelection ();
	
	m_MinMaxPopupCode = m_minMaxCtrl->GetSelection ();
	
	m_channelSelection = m_channelsCtrl->GetSelection ();

	wxChoice* disptype = (wxChoice*)FindWindow (ID3C_DisplayType);
	m_DisplayType = disptype->GetSelection ();
	
	wxComboBox* nofdispl = (wxComboBox*)FindWindow (ID3C_NumberOfLevels);
	m_NumberLevels = (UInt32)(nofdispl->GetSelection ());
	m_numberLevelsval = wxAtoi (nofdispl->GetValue ());
	
	TransferLinesColumnsFromWindow ();
	
	unsigned long int 	tempULong;
	wxTextCtrl* rchannel = (wxTextCtrl*)FindWindow (ID3C_RedChannel);
	m_RedChannelString = rchannel->GetValue ();
	m_RedChannelString.ToULong (&tempULong);
	m_RedChannel = (UInt32)tempULong;
	
	wxTextCtrl* gchannel = (wxTextCtrl*)FindWindow (ID3C_GreenChannel);
	m_GreenChannelString = gchannel->GetValue ();
	m_GreenChannelString.ToULong (&tempULong);
	m_GreenChannel = (UInt32)tempULong;
	
	wxTextCtrl* bchannel = (wxTextCtrl*)FindWindow (ID3C_BlueChannel);
	m_BlueChannelString = bchannel->GetValue ();
	m_BlueChannelString.ToULong (&tempULong);
	m_BlueChannel = (UInt32)tempULong;
	
	wxTextCtrl* grchannel = (wxTextCtrl*)FindWindow (ID3C_GrayChannel);
	m_GrayChannelString = grchannel->GetValue ();
	m_GrayChannelString.ToULong (&tempULong);
	m_GrayChannel = (UInt32)tempULong;
	
	//wxTextCtrl* legendfactor = (wxTextCtrl*)FindWindow (IDC_LegendFactor);
	m_thematicLegendFactorString = m_legendfactorctrl->GetValue ();
	m_thematicLegendFactorString.ToDouble (&m_thematicLegendFactor);
	
	m_Magnificationstring = m_magnificationctrl->GetValue ();
	m_Magnificationstring.ToDouble (&m_Magnification);

			// Get bits of color index for 8(0), 16(1),24(2) bit vector.

   if (m_DisplayType == 2)
      mBitsOfColorIndex = 2; // color image, 24 bits
   else
      mBitsOfColorIndex = 0; // 8 bits

	returnCode = VerifyLineColumnValues (IDC_LineStart,
													  IDC_ColumnStart,
													  true);

	if (returnCode == 0)
		{
				// Verify that the number of lines to be displayed is less than
				// the maximum possible.

		if (CheckNumberDisplayLines (m_LineStart,
												m_LineEnd,
												m_LineInterval,
												NULL,
												IDC_LineStart) == 0)
			returnCode = IDC_LineEnd;
			
		}	// end "if (returnCode == 0)"

	if (returnCode == 0)
		{
				// Verify that the number of columns to be displayed is less than
				// the maximum possible.
				// This part needs to be updated.

		if (CheckNumberDisplayColumns (m_ColumnStart,
													m_ColumnEnd,
													m_ColumnInterval,
													24,
													m_DisplayType+1,
													m_localActiveNumberFeatures,
													NULL,
													IDC_ColumnStart) == 0)
			returnCode = IDC_ColumnEnd;
			
		}	// end "if (returnCode == 0)"
		
			// Verify that channels entered for red, green, blue, gray are not blank.
			
	if (returnCode == 0)
		{
		if (mRGBColors <= k1_ChannelGrayLevelDisplayType && m_GrayChannelString.empty ())
			returnCode = ID3C_GrayChannel;
		
		else if (mRGBColors & 0x0004 && m_RedChannelString.empty ())
			returnCode = ID3C_RedChannel;
		
		else if (mRGBColors & 0x0002 && m_GreenChannelString.empty ())
			returnCode = ID3C_GreenChannel;
		
		else if (mRGBColors & 0x0001 && m_BlueChannelString.empty ())
			returnCode = ID3C_BlueChannel;
			
		if (returnCode != 0)
			{
			if (LoadSpecifiedStringNumberLongP (
												kAlertStrID, 
												IDS_Alert91, 
												(char*)gTextString,
												(char*)gTextString2,
												TRUE,
												1,
												gImageWindowInfoPtr->totalNumberChannels))
				DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
				
			SelectDialogItemText (this, returnCode, 0, SHRT_MAX);
				
			}	// end "if (returnCode != 0)"
	
		}	// end "if (returnCode == 0)"
			
   if (returnCode == 0)
		returnCode = CheckDialogRealValue (this,
														ID3C_NumberOfLevels,
														1,
														256,
														0,
														true,		// minValue is allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
	
	if (returnCode == 0)
		{
				// Check magnification setting.
		
		if (m_Magnification <= 0 || m_Magnification > 99)
			{
			DisplayAlert (kErrorAlertID,
								kStopAlert,
								kAlertStrID,
								IDS_Alert152,
								0,
								NULL);
			
			returnCode = ID3C_Magnification;
			
			SelectDialogItemText (this, ID3C_Magnification, 0, SInt16_MAX);
			
			}	// end "if (m_Magnification <= 0 || m_Magnification > 99)"
			
		}	// end "if (returnCode == 0)"
	
	return (returnCode == 0);
	 
}	// end "TransferDataFromWindow"



bool CMDisplaySpecsDlg::TransferDataToWindow ()

{
	m_channelsCtrl->SetSelection (m_channelSelection);
	
	m_enhancementCtrl->SetSelection (m_Enhancement);
	
	m_minMaxCtrl->SetSelection (m_MinMaxPopupCode);
	
	wxChoice* tzeroas = (wxChoice*)FindWindow (ID3C_TreatZeroAs);
	tzeroas->SetSelection (m_TreatZeroAs);

	wxCheckBox* rinv = (wxCheckBox*)FindWindow (IDC_RedChannelInvert);
	rinv->SetValue (m_redChannelInvertFlag);
	
	wxCheckBox* ginv = (wxCheckBox*)FindWindow (IDC_GreenChannelInvert);
	ginv->SetValue (m_greenChannelInvertFlag);
	
	wxCheckBox* binv = (wxCheckBox*)FindWindow (IDC_BlueChannelInvert);
	binv->SetValue (m_blueChannelInvertFlag);
	
	wxCheckBox* voverlays = (wxCheckBox*)FindWindow (IDC_VectorOverlays);
	voverlays->SetValue (m_vectorOverlaysFlag);
	
	wxCheckBox* newhist = (wxCheckBox*)FindWindow (ID3C_NewHistogram);
	newhist->SetValue (m_ComputeHistogramFlag);

	wxChoice* disptype = (wxChoice*)FindWindow (ID3C_DisplayType);
	disptype->SetSelection (m_DisplayType);
	
	wxComboBox* nofdispl = (wxComboBox*)FindWindow (ID3C_NumberOfLevels);
	nofdispl->SetStringSelection (wxString::Format (wxT("%i"), (int)m_NumberLevels));

	wxTextCtrl* bchannel = (wxTextCtrl*)FindWindow (ID3C_BlueChannel);
	bchannel->Clear ();
	*bchannel << (int)m_BlueChannel;
	
	wxTextCtrl* gchannel = (wxTextCtrl*)FindWindow (ID3C_GreenChannel);
	gchannel->Clear ();
	*gchannel << (int)m_GreenChannel;
	
	wxTextCtrl* rchannel = (wxTextCtrl*)FindWindow (ID3C_RedChannel);
	rchannel->Clear ();
	*rchannel << (int)m_RedChannel;
	
	wxTextCtrl* grchannel = (wxTextCtrl*)FindWindow (ID3C_GrayChannel);
	grchannel->Clear ();
	*grchannel << (int)m_GrayChannel;
	
	m_magnificationctrl->Clear ();
	m_magnificationctrl->ChangeValue (wxString::Format(wxT("%.3f"), m_Magnification));

	//wxTextCtrl* legendfactor = (wxTextCtrl*)FindWindow (IDC_LegendFactor);
	m_legendfactorctrl->Clear ();
	//*legendfactor << m_thematicLegendFactor;
	m_legendfactorctrl->ChangeValue (
									wxString::Format (wxT("%.8f"), m_thematicLegendFactor));
	
	TransferLinesColumnsToWindow ();

	SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);

	return true;
	
}	// end "TransferDataToWindow"
