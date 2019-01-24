//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDisplayMultispectralDialog.cpp : class implementation file
//	Class Definition:		LDisplayMultispectralDialog.h
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
//								CMDisplaySpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "wx/cshelp.h"
#include "SMultiSpec.h"

#include "LDisplayMultispectralDialog.h"
#include "LImageView.h"
#include "CImageWindow.h"
#include "CDisplay.h"
#include "wx/valgen.h"
#include "wx/valnum.h"
#include "wx/window.h"
#include "LChannelsDialog.h"

#include "LImage_dialog.cpp"
#include "LImageFrame.h"

extern Boolean MinMaxEnhancementDialog (
				SInt16*								channelsPtr,
				SInt16								rgbColors,
				SInt16								displayType,
				SInt16								numberLevels,
				SInt16*								percentTailsClippedPtr,
				SInt16*								minMaxSelectionPtr,
				double*								minMaxValuesPtr);

extern void UpdateDialogChannelItems (
				DialogPtr							dialogPtr,
				SInt16								rgbColors,
				SInt16								displayType);



IMPLEMENT_DYNAMIC_CLASS (CMDisplaySpecsDlg, CMDialog)

CMDisplaySpecsDlg::CMDisplaySpecsDlg ()

{
}



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

		m_localFeaturesPtr = (UInt16*) MNewPointer(
				 (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

		m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

	CreateControls();
	if (m_initializedFlag) 
		{
				//	Set the image file name.

		fileNamePtr = (FileStringPtr)(gActiveImageViewCPtr->GetImageWindowCPtr())->
																				GetImageFileNamePointer();
		if (fileNamePtr != NULL) 
			{
			m_FileName = wxString (fileNamePtr, wxConvUTF8);
			wxStaticText * fname = (wxStaticText*)FindWindow (ID3C_FileName);
			fname->SetLabel (m_FileName);
			
			}	// end "if (fileNamePtr != NULL)"
		
		else	// fileNamePtr == NULL
			m_initializedFlag = FALSE;

		}	// end "if (m_initializedFlag)"

	Centre();
	if (!m_initializedFlag)
		return;

}	// end "CMDisplaySpecsDlg"



CMDisplaySpecsDlg::~CMDisplaySpecsDlg ()

{
	m_localFeaturesPtr = CheckAndDisposePtr(m_localFeaturesPtr);
 
}	// end "~CMDisplaySpecsDlg"



BEGIN_EVENT_TABLE(CMDisplaySpecsDlg, CMDialog)
	EVT_CHOICE(ID3C_BitsOfColor, CMDisplaySpecsDlg::OnSelendokBitsOfColor)
	EVT_INIT_DIALOG(CMDisplaySpecsDlg::OnInitDialog)
	EVT_TEXT(ID3C_BlueChannel, CMDisplaySpecsDlg::OnChangeBlueChannel)
	EVT_TEXT(ID3C_GreenChannel, CMDisplaySpecsDlg::OnChangeGreenChannel)
	EVT_TEXT(ID3C_RedChannel, CMDisplaySpecsDlg::OnChangeRedChannel)
	EVT_TEXT(ID3C_GrayChannel, CMDisplaySpecsDlg::OnChangeGrayChannel)
	EVT_TEXT(IDC_ColumnEnd, CMDisplaySpecsDlg::CheckColumnEnd)
	EVT_TEXT(IDC_ColumnStart, CMDisplaySpecsDlg::CheckColumnStart)
	EVT_TEXT(IDC_LineEnd, CMDisplaySpecsDlg::CheckLineEnd)
	EVT_TEXT(IDC_LineStart, CMDisplaySpecsDlg::CheckLineStart)
	EVT_TEXT(IDC_LineInterval, CMDisplaySpecsDlg::CheckLineInterval)
	EVT_TEXT(IDC_ColumnInterval, CMDisplaySpecsDlg::CheckColumnInterval)
	EVT_BUTTON(ID3C_ChannelDescriptions, CMDisplaySpecsDlg::OnChannelDescriptions)
	EVT_COMBOBOX(ID3C_DisplayType, CMDisplaySpecsDlg::OnSelendokDisplayType)
	EVT_COMBOBOX(ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValues)
	EVT_COMBOBOX(ID3C_Channels, CMDisplaySpecsDlg::OnSelendokChannels)
	EVT_COMBOBOX_DROPDOWN(ID3C_Channels, CMDisplaySpecsDlg::OnSelendokChannelsDropDown)
	EVT_COMBOBOX(ID3C_Enhancement, CMDisplaySpecsDlg::OnSelendokEnhancement)
	EVT_COMBOBOX_CLOSEUP (ID3C_Enhancement,CMDisplaySpecsDlg::OnSelendokEnhancementCloseUp)
	EVT_COMBOBOX_DROPDOWN (ID3C_Enhancement, CMDisplaySpecsDlg::OnSelendokEnhancementDropDown)
	EVT_BUTTON (IDEntireImage, CMDisplaySpecsDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMDisplaySpecsDlg::ToSelectedImage)
	//EVT_CHAR_HOOK(CMDisplaySpecsDlg::OnButtonPress)
	EVT_COMBOBOX_CLOSEUP (ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValuesCloseUp)
	EVT_COMBOBOX_DROPDOWN (ID3C_MinMaxValues, CMDisplaySpecsDlg::OnSelendokMinMaxValuesDropDown)
	//EVT_PAINT(CMDisplaySpecsDlg::OnPaint)
END_EVENT_TABLE()



void CMDisplaySpecsDlg::CreateControls ()

{
	/*
	wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
	wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
	wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
	wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);
	*/
	//m_dialogPanel = new wxPanel (this, wxID_ANY);
	
	wxWindow* parentWindow = this;
	//wxWindow* parentWindow = m_dialogPanel;

	SetSizeHints (wxDefaultSize, wxDefaultSize);
	//m_dialogPanel->SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer67 = new wxBoxSizer (wxVERTICAL);

	m_staticimagename = new wxStaticText (parentWindow,
														ID3C_FileName,
														wxT("image name"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_staticimagename->Wrap(-1);
	bSizer67->Add (m_staticimagename, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT,12));
	bSizer67->Add (20,6);

	wxBoxSizer* bSizer78;
	bSizer78 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer731;
	bSizer731 = new wxBoxSizer (wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (parentWindow,
																		wxID_ANY,
																		wxT("Area to Display")),
												wxHORIZONTAL);
	/*
	wxBoxSizer* bSizer791;
	bSizer791 = new wxBoxSizer (wxHORIZONTAL);

	m_bpButton4 = new wxBitmapButton (sbSizer8->GetStaticBox(),
													IDEntireImage,
													entireimi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
   SetUpToolTip(m_bpButton4, IDS_ToolTip40);
	m_bpButton4->SetBitmapDisabled(toentirei);
   bSizer791->Add (m_bpButton4,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	m_bpButton5 = new wxBitmapButton (sbSizer8->GetStaticBox(),
													IDSelectedImage,
													selectedi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);

	m_bpButton5->SetBitmapDisabled (bmp4i);
   bSizer791->Add (m_bpButton5, 0, wxALIGN_CENTER_VERTICAL);

   sbSizer8->Add (bSizer791, 0, wxEXPAND);
	*/
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
																		wxT("Display")),
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
	bSizer732->Add (m_staticText701, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_displaytypectrl = new wxComboBox (sbSizerStaticBox9,
													ID3C_DisplayType,
													/*wxT("3-Channel Color")*/wxEmptyString,
													wxDefaultPosition,
													wxSize (180, -1),
													0,
													NULL,
													wxCB_READONLY);
	m_displaytypectrl->Append (wxT("1-Channel Thematic"));
	m_displaytypectrl->Append (wxT("1-Channel Grayscale"));
	m_displaytypectrl->Append (wxT("2-Channel Color (R-G)"));
	m_displaytypectrl->Append (wxT("2-Channel Color (G-B)"));
	m_displaytypectrl->Append (wxT("2-Channel Color (R-B)"));
	m_displaytypectrl->Append (wxT("3-Channel Color"));
	m_displaytypectrl->Append (wxT("Side by Side Channels"));
   SetUpToolTip (m_displaytypectrl, IDS_ToolTip25);
	bSizer732->Add (m_displaytypectrl, 0, wxALL, 5);

	fgSizer3->Add (bSizer732, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer741;
	bSizer741 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText72 = new wxStaticText (sbSizerStaticBox9,
													wxID_ANY,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip(m_staticText72, IDS_ToolTip26);
	m_staticText72->Wrap(-1);
	bSizer741->Add (m_staticText72, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_comboBox29 = new wxComboBox (sbSizerStaticBox9,
												ID3C_Channels,
												wxT("All"),
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												0);
	m_comboBox29->Append (wxT("All"));
	m_comboBox29->Append (wxT("Subset..."));
	m_comboBox29->Enable (true);
	m_comboBox29->Hide ();
   bSizer741->Add (m_comboBox29, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

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
	rgsizer1->Add (m_redpromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_redchannelctrl = new wxTextCtrl (sbSizerStaticBox9,
													ID3C_RedChannel,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_redchannelctrl, IDS_ToolTip27);
	m_redchannelctrl->SetValidator (wxTextValidator(wxFILTER_DIGITS, &m_RedChannelString));
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
	gSizer3->Add (m_greenpromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_greenchannelctrl = new wxTextCtrl (sbSizerStaticBox9,
														ID3C_GreenChannel,
														wxEmptyString,
														wxDefaultPosition,
														wxDefaultSize,
														0);
   SetUpToolTip(m_greenchannelctrl, IDS_ToolTip29);
	m_greenchannelctrl->SetValidator (wxTextValidator(wxFILTER_DIGITS, &m_GreenChannelString));
	gSizer3->Add(m_greenchannelctrl, 0, wxALL, 5);

	m_greenchaninvctrl = new wxCheckBox (sbSizerStaticBox9,
														IDC_GreenChannelInvert,
														wxT("Invert"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
   SetUpToolTip(m_greenchaninvctrl, IDS_ToolTip28);
	m_greenchaninvctrl->SetValidator (wxGenericValidator (&m_greenChannelInvertFlag));

	gSizer3->Add(m_greenchaninvctrl, 0, wxALL, 5);

	m_bluepromptctrl = new wxStaticText (sbSizerStaticBox9,
														ID3C_BluePrompt,
														wxT("Blue:"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_bluepromptctrl->Wrap(-1);
   SetUpToolTip(m_bluepromptctrl, IDS_ToolTip30);
	gSizer3->Add(m_bluepromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_bluechannelctrl = new wxTextCtrl (sbSizerStaticBox9,
													ID3C_BlueChannel,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip(m_bluechannelctrl, IDS_ToolTip30);
	m_bluechannelctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_BlueChannelString));
	gSizer3->Add (m_bluechannelctrl, 0, wxALL, 5);

	m_bluechaninvctrl = new wxCheckBox (sbSizerStaticBox9,
													IDC_BlueChannelInvert,
													wxT("Invert"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_bluechaninvctrl, IDS_ToolTip28);
	m_bluechaninvctrl->SetValidator (wxGenericValidator (&m_blueChannelInvertFlag));
	gSizer3->Add(m_bluechaninvctrl, 0, wxALL, 5);

	fgSizer3->Add(gSizer3, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer76;
	bSizer76 = new wxBoxSizer(wxHORIZONTAL);

	fgSizer3->Add(bSizer76, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5);

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer(wxHORIZONTAL);

	m_legendpromptctrl = new wxStaticText (sbSizerStaticBox9,
														IDC_LegendFactorPrompt,
														wxT("Legend Factor"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_legendpromptctrl->Wrap(-1);
	m_legendpromptctrl->Hide();

	bSizer81->Add (m_legendpromptctrl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	m_legendfactorctrl = new wxTextCtrl (sbSizerStaticBox9,
														IDC_LegendFactor,
														wxT("1"),
														wxDefaultPosition,
														wxDefaultSize,
														0);
	m_legendfactorctrl->SetValidator (wxTextValidator (wxFILTER_NUMERIC, &m_thematicLegendFactorString));
	m_legendfactorctrl->Hide ();

	bSizer81->Add (m_legendfactorctrl, 0, wxALL, 5);

	fgSizer3->Add (bSizer81, 1, wxEXPAND, 5);

	m_chandescctrl = new wxButton (sbSizerStaticBox9, ID3C_ChannelDescriptions, wxT("Channel Descriptions..."), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_chandescctrl, IDS_ToolTip32);

	fgSizer3->Add(m_chandescctrl, 0, wxALIGN_CENTER | wxALL, 5);

	wxBoxSizer* bSizer75;
	bSizer75 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText76 = new wxStaticText (sbSizerStaticBox9, wxID_ANY, wxT("Magnification:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText76->Wrap (-1);
   SetUpToolTip(m_staticText76, IDS_ToolTip33);
	bSizer75->Add(m_staticText76, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxFloatingPointValidator<double> _val(3, &m_Magnification);
	_val.SetRange(0.01, 99.);
	m_magnificationctrl = new wxTextCtrl (sbSizerStaticBox9, ID3C_Magnification, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_magnificationctrl, IDS_ToolTip33);
	m_magnificationctrl->SetValidator(_val);

	bSizer75->Add(m_magnificationctrl, 0, wxALL, 5);
	fgSizer3->Add(bSizer75, 1, wxEXPAND, 5);

	m_checkBox17 = new wxCheckBox (sbSizerStaticBox9, IDC_VectorOverlays, wxT("Include Vector Overlays"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox17, IDS_ToolTip31);
	fgSizer3->Add (m_checkBox17, 0, wxALL, 5);

	sbSizer9->Add (fgSizer3, 1, wxEXPAND, 5);

	bSizer79->Add (sbSizer9, wxSizerFlags(0).Expand().Border(wxRIGHT,6));

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer (wxHORIZONTAL);

	wxBoxSizer* bSizer73;
	bSizer73 = new wxBoxSizer (wxVERTICAL);

	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer (new wxStaticBox (parentWindow, wxID_ANY, wxT("Enhancement")), wxVERTICAL);

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer(0, 2, 0, 0);

	m_staticText67 = new wxStaticText(sbSizer10->GetStaticBox(), wxID_ANY, wxT("Bits of color:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText67->Wrap(-1);
	SetUpToolTip(m_staticText67, IDS_ToolTip34);
	gSizer2->Add(m_staticText67, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	wxString m_bitsofcolctrlChoices[] = {wxT("8"), wxT("24")};
	int m_bitsofcolctrlNChoices = sizeof (m_bitsofcolctrlChoices) / sizeof (wxString);
	m_bitsofcolctrl = new wxChoice (sbSizer10->GetStaticBox(),
												ID3C_BitsOfColor,
												wxDefaultPosition,
												wxDefaultSize,
												m_bitsofcolctrlNChoices,
												m_bitsofcolctrlChoices,
												0);
	m_bitsofcolctrl->SetSelection (0);
   SetUpToolTip (m_bitsofcolctrl, IDS_ToolTip34);
	m_bitsofcolctrl->SetValidator (wxGenericValidator(&m_BitsOfColor));

	gSizer2->Add(m_bitsofcolctrl, 0, wxALL, 5);

	m_staticText68 = new wxStaticText (sbSizer10->GetStaticBox (),
													wxID_ANY,
													wxT("Stretch:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText68->Wrap (-1);
   SetUpToolTip (m_staticText68, IDS_ToolTip35);
	gSizer2->Add (m_staticText68, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);
	/*
	//wxString m_enhancementChoices[] = {wxT("Linear"), wxT("Equal Area"), wxT("Gaussian")};
	//int m_enhancementNChoices = sizeof (m_enhancementChoices) / sizeof (wxString);
   m_enhancementctrl = new wxChoice (sbSizer10->GetStaticBox(),
													ID3C_Enhancement,
													wxDefaultPosition,
													wxDefaultSize,
													//m_enhancementNChoices,
													//m_enhancementChoices,
													0,
													NULL,
													0);
	*/
   m_enhancementctrl = new wxComboBox (sbSizer10->GetStaticBox(),
   												ID3C_Enhancement,
   												wxT("Linear"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0,
   												NULL,
   												wxCB_READONLY|wxTE_PROCESS_ENTER);
	
   m_enhancementctrl->Append (wxT("Linear"));
   m_enhancementctrl->Append (wxT("Equal Area"));
   m_enhancementctrl->Append (wxT("Gaussian"));
   
   SetUpToolTip (m_enhancementctrl, IDS_ToolTip35);
	m_enhancementctrl->SetSelection (0);
	m_enhancementctrl->SetValidator (wxGenericValidator (&m_Enhancement));
	gSizer2->Add (m_enhancementctrl, 0, wxALL, 5);

	m_staticText69 = new wxStaticText(sbSizer10->GetStaticBox(), wxID_ANY, wxT("Min-max:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText69->Wrap(-1);   
   SetUpToolTip(m_staticText69, IDS_ToolTip36);
	gSizer2->Add(m_staticText69, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

   m_minmaxctrl = new wxComboBox (sbSizer10->GetStaticBox(),
   											ID3C_MinMaxValues,
   											wxT("Clip 0% of tails"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0,
   											NULL,
   											wxCB_READONLY|wxTE_PROCESS_ENTER);
	m_minmaxctrl->Append (wxT("Clip 0% of tails"));
	m_minmaxctrl->Append (wxT("Clip 2% of tails"));
	m_minmaxctrl->Append (wxT("Entire Range"));
	m_minmaxctrl->Append (wxT("User Specified..."));
	m_minmaxctrl->SetSelection (0);
   SetUpToolTip (m_minmaxctrl, IDS_ToolTip36);
	gSizer2->Add (m_minmaxctrl, 0, wxALL, 5);

	m_staticText70 = new wxStaticText(sbSizer10->GetStaticBox(), wxID_ANY, wxT("Treat '0' as:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText70->Wrap(-1);
   SetUpToolTip(m_staticText70, IDS_ToolTip37);
	gSizer2->Add(m_staticText70, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

	wxString m_treatzeroctrlChoices[] = {wxT("Background"), wxT("Data")};
	int m_treatzeroctrlNChoices = sizeof (m_treatzeroctrlChoices) / sizeof (wxString);
	m_treatzeroctrl = new wxChoice (sbSizer10->GetStaticBox(),
												ID3C_TreatZeroAs,
												wxDefaultPosition,
												wxDefaultSize,
												m_treatzeroctrlNChoices,
												m_treatzeroctrlChoices,
												0);
	m_treatzeroctrl->SetSelection (0);
   SetUpToolTip(m_treatzeroctrl, IDS_ToolTip37);
	m_treatzeroctrl->SetValidator(wxGenericValidator(&m_TreatZeroAs));

	gSizer2->Add(m_treatzeroctrl, 0, wxALL, 5);

	m_staticText681 = new wxStaticText (sbSizer10->GetStaticBox(),
													wxID_ANY,
													wxT("Number of display levels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText681->Wrap (-1);
   SetUpToolTip (m_staticText681, IDS_ToolTip38);
	gSizer2->Add (m_staticText681, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT | wxALL, 5);

   UInt16 value;
   wxIntegerValidator<UInt16> val(&value, wxNUM_VAL_DEFAULT);
   val.SetRange(1,256);
	m_noofdisplevelsctrl = new wxComboBox(sbSizer10->GetStaticBox(), ID3C_NumberOfLevels, wxT("256"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0, val);//,wxTE_PROCESS_ENTER);
	m_noofdisplevelsctrl->Append (wxT("6"));
	m_noofdisplevelsctrl->Append (wxT("15"));
	m_noofdisplevelsctrl->Append (wxT("32"));
	m_noofdisplevelsctrl->Append (wxT("254"));
	m_noofdisplevelsctrl->Append (wxT("256"));
   SetUpToolTip (m_noofdisplevelsctrl, IDS_ToolTip38);
	m_noofdisplevelsctrl->SetValidator (wxTextValidator(wxFILTER_NUMERIC, &m_NumberLevelsString));
	gSizer2->Add (m_noofdisplevelsctrl, 0, wxALL, 5);

	m_newhistctrl = new wxCheckBox(sbSizer10->GetStaticBox(), ID3C_NewHistogram, wxT("Load New Histogram"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_newhistctrl, IDS_ToolTip39);
	m_newhistctrl->SetValidator(wxGenericValidator(&m_ComputeHistogramFlag));

	gSizer2->Add (m_newhistctrl, wxSizerFlags(0).Border(wxALL, 5));

	sbSizer10->Add (gSizer2, wxSizerFlags(1).Expand());

	bSizer73->Add (sbSizer10, wxSizerFlags(1).Expand());
	/*
	wxBoxSizer* bSizer77;
	bSizer77 = new wxBoxSizer (wxHORIZONTAL);

	m_cancelbtn = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);   
	bSizer77->Add (m_cancelbtn, wxSizerFlags(0).Border(wxRIGHT,6));

	m_okbtn = new wxButton (this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
   m_okbtn->SetDefault ();
	bSizer77->Add (m_okbtn, wxSizerFlags(0));
    
	bSizer73->Add (bSizer77, wxSizerFlags(0).Right().Border(wxTOP,6));
   */
	//bSizer73->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP,6));
	
	bSizer72->Add (bSizer73, wxSizerFlags(1).Expand());

	bSizer79->Add (bSizer72, wxSizerFlags(0).Expand().Border(wxLEFT,6));

	bSizer67->Add (bSizer79, wxSizerFlags(1).Expand().Border(wxLEFT|wxRIGHT,12));
	/*
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	//wxSizer* standardButtonSizer = CreateSeparatedButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bSizer67->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bSizer67->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif
	*/
	CreateStandardButtons (bSizer67);

	//SetSizer (bSizer67);
	SetSizerAndFit (bSizer67);
	
			// Set focus and tab order
	
	//wxWindow* selectedItemWindowPtr = FindWindow (ID3C_RedChannel);
   SelectDialogItemText ((DialogPtr)sbSizerStaticBox9, ID3C_RedChannel, 0, SInt16_MAX);
	
	m_greenchannelctrl->MoveAfterInTabOrder (m_redchannelctrl);
	m_bluechannelctrl->MoveAfterInTabOrder (m_greenchannelctrl);
	
	//m_redchaninvctrl->MoveAfterInTabOrder(m_bluechannelctrl);
	//m_greenchaninvctrl->MoveAfterInTabOrder(m_redchaninvctrl);
	//m_bluechaninvctrl->MoveAfterInTabOrder(m_greenchaninvctrl);
	
   //m_linestartctrl->MoveAfterInTabOrder (m_bluechannelctrl);
   //FindWindow (ID3C_Magnification)->MoveAfterInTabOrder (FindWindow (IDC_ColumnInterval));

	//bSizer67->Layout();
	
	//this->SetClientSize (this->GetSize ());
	
}	// end "CreateControls"


/*
//Press keyboard "Enter"  -> wxID_OK
void CMDisplaySpecsDlg::OnButtonPress(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_RETURN) {
        if(Validate() && TransferDataFromWindow())
            this->EndModal(wxID_OK);
    } else {
        event.Skip();
    }
}
*/


void CMDisplaySpecsDlg::CheckChannel (
        int							itemID,
        UINT*						channelValuePtr) 
{
	SInt32					newValue;

	SInt16					numberLevels;


	newValue = (UINT)CheckValue (itemID, 1, gImageWindowInfoPtr->totalNumberChannels);
	
	if (newValue > 0 && newValue <= gImageWindowInfoPtr->totalNumberChannels) {
		*channelValuePtr = newValue;

				// Verify number of levels setting

		DisplayMultispectralDialogCheckDisplayLevels (
											  m_displaySpecsPtr,
											  this,
											  mLocalDisplayType,
											  m_RedChannel,
											  m_GreenChannel,
											  m_BlueChannel,
											  (SInt16*) & mColorLevelsMax[0][0],
											  &mDuplicateChannelCode,
											  (SInt16) m_BitsOfColor + 1,
											  &numberLevels);

		m_NumberLevels = numberLevels;

		}	// end "if (newValue > 0 &&"

} // end "CheckChannel"


//-----------------------------------------------------------------------------
//								 Copyright (1988-2015)
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
//	Coded By:			Larry L. Biehl			Date: 06/01/1995
//	Revised By:			Larry L. Biehl			Date: 11/30/2006	

Boolean CMDisplaySpecsDlg::DoDialog(
        DisplaySpecsPtr displaySpecsPtr) {
    Boolean continueFlag = FALSE;
    SInt16 returnCode;


    // Make sure intialization has been completed.

    if (!m_initializedFlag)
        return (FALSE);

    m_displaySpecsPtr = displaySpecsPtr;
    returnCode = ShowModal();

    if (returnCode == wxID_OK) {
        // Display area

        m_dialogSelectArea.lineStart = m_LineStart;
        m_dialogSelectArea.lineEnd = m_LineEnd;
        m_dialogSelectArea.lineInterval = m_LineInterval;

        m_dialogSelectArea.columnStart = m_ColumnStart;
        m_dialogSelectArea.columnEnd = m_ColumnEnd;
        m_dialogSelectArea.columnInterval = m_ColumnInterval;

        DisplayMultispectralDialogOK(this,
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
                (SInt16*) & mPixelSizeVector[0],
                m_gaussianStretch,
                m_Enhancement + 1,
                m_MinMaxCode,
                m_TreatZeroAs, // + 1,
                //m_NumberLevels,
                m_numberLevelsval,
                (SInt16*) & mColorLevelsMax[0][0],
                m_ComputeHistogramFlag,
                m_channelSelection,
                m_redChannelInvertFlag,
                m_greenChannelInvertFlag,
                m_blueChannelInvertFlag,
                m_thematicLegendFactor,
                m_vectorOverlaysFlag);

        if(gActiveImageWindow->m_frame != NULL)
         gActiveImageWindow->m_frame->m_frameMaximized = false;
        
        continueFlag = TRUE;

    } // end "if (returnCode == IDOK)"

    return (continueFlag);

} // end "DoDialog"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								c Purdue Research Foundation
//									All rights reserved.
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
//	Coded By:			Larry L. Biehl			Date: 12/05/91
//	Revised By:			Larry L. Biehl			Date: 06/06/95	

Boolean
CMDisplaySpecsDlg::GetComputeHistogramDialogSetting(
        DisplaySpecsPtr displaySpecsPtr,
        SInt16 lEnhancement,
        SInt16 lMinMaxCode) {
    Boolean setComputeItemFlag;


    // Initialize local variables.

    setComputeItemFlag = FALSE;

    if (!displaySpecsPtr->defaultHistogramFileFlag) {
        if (lEnhancement == kEqualAreaStretch)
            setComputeItemFlag = TRUE;

        if (lEnhancement == kLinearStretch &&
                displaySpecsPtr->histogramCompute == 1 &&
                lMinMaxCode != kUserSpecified)
            setComputeItemFlag = TRUE;

    } // end "if ( !displaySpecsPtr->defaultHistogramFileFlag )"

    return (setComputeItemFlag);

} // end "GetComputeHistogramDialogSetting"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMaximumDisplayLevels
//
//	Software purpose:	The purpose of this routine is to determine the maximum
//							number of display levels for the input specifications.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/06/95
//	Revised By:			Larry L. Biehl			Date: 06/06/95	

UInt16
CMDisplaySpecsDlg::GetMaximumDisplayLevels(
        SInt16 bitsOfColorIndex,
        SInt16 displayType,
        SInt16 duplicateChannelCode) {
    UInt16 numberChannelsIndex = 0;


    switch (displayType) {
        case 1:
        case 2:
        case 7:
            numberChannelsIndex = 0;

            break;

        case 3:

            if (duplicateChannelCode)
                numberChannelsIndex = 1;

            else
                numberChannelsIndex = 2;

            break;

        case 4:
            numberChannelsIndex = 1;

            break;

    } // end "switch (displayType)"

    return (mColorLevelsMax[bitsOfColorIndex][numberChannelsIndex]);

} // end "GetMaximumDisplayLevels"


void CMDisplaySpecsDlg::OnChangeBlueChannel(wxCommandEvent& event) {

		// Get current blue channel

	CheckChannel (ID3C_BlueChannel, &m_BlueChannel);

} // end "OnChangeBlueChannel"


void CMDisplaySpecsDlg::OnChangeGrayChannel(wxCommandEvent& event) {
	
	CheckChannel (ID3C_GrayChannel, &m_GrayChannel);

} // end "OnChangeGrayChannel"


void CMDisplaySpecsDlg::OnChangeGreenChannel(wxCommandEvent& event) {
		
		// Get current green channel

	CheckChannel (ID3C_GreenChannel, &m_GreenChannel);

} // end "OnChangeGreenChannel"


void CMDisplaySpecsDlg::OnChangeRedChannel(wxCommandEvent& event) {

			// Get current red channel

	CheckChannel (ID3C_RedChannel, &m_RedChannel);

} // end "OnChangeRedChannel"


void CMDisplaySpecsDlg::OnChannelDescriptions(wxCommandEvent& event) {

    // Show channel list.

    SetDLogControlHilite(NULL, wxID_OK, 255);

    CMChannelsDlg* channelsDialogPtr = NULL;

    SInt16 tempInt;

    channelsDialogPtr = new CMChannelsDlg(this);

    channelsDialogPtr->DoDialog(
            &tempInt,
            NULL,
            gImageLayerInfoPtr,
            gImageFileInfoPtr,
            2,
            kNoTransformation,
            NULL,
            (SInt16) gImageWindowInfoPtr->totalNumberChannels,
            kSubsetMenuItem);

    if (channelsDialogPtr != NULL)
        delete channelsDialogPtr;

    SetDLogControlHilite(NULL, wxID_OK, 0);

} // end "OnChannelDescriptions"

/* Moving this inside OnSelendokEnhancement
void CMDisplaySpecsDlg::OnCloseupEnhancement() {
    if (m_optionKeyFlag) {
        SetComboItemText(ID3C_Enhancement,
                2,
                (char*) "Gaussian");

        DDX_CBIndex(m_dialogToPtr, ID3C_Enhancement, m_Enhancement);

        m_optionKeyFlag = FALSE;

    } // end "if (m_optionKeyFlag)"

} // end "OnCloseupEnhancement"
 */

// Dont think this is needed now as this has been added to OnSelndDisplayType
/*
void CMDisplaySpecsDlg::OnDropdownBitsOfColor(void)
 {
    CComboBox* comboBoxPtr = (CComboBox*) GetDlgItem(ID3C_BitsOfColor);

    if (mLocalDisplayType == 2 || mLocalDisplayType == 7) {
        if (comboBoxPtr->GetCount() == 2)
            // Remove the '24' item from the bits of color list.

            comboBoxPtr->DeleteString(1);

    }// end "if (mLocalDisplayType == 2 || mLocalDisplayType == 7)"

    else // mLocalDisplayType != 2 && mLocalDisplayType != 7
    {
        if (comboBoxPtr->GetCount() == 1)
            comboBoxPtr->AddString("24");

    } // end "else mLocalDisplayType != 2 && ..."

} // end "OnDropdownBitsOfColor"
 */

/* Moving this inside OnSelendokEnhancement
void CMDisplaySpecsDlg::OnDropdownEnhancement() {
    m_optionKeyFlag = FALSE;

    if (GetKeyState(VK_RBUTTON) < 0) {
        SetComboItemText(ID3C_Enhancement,
                2,
                (char*) "Gaussian...");

        m_optionKeyFlag = TRUE;

        DDX_CBIndex(m_dialogToPtr, ID3C_Enhancement, m_Enhancement);

    } // end "if (GetKeyState (VK_RBUTTON) < 0)"

} // end "OnDropdownEnhancement"
 */
 
 
void CMDisplaySpecsDlg::OnInitDialog (
				wxInitDialogEvent& event)

{
    SInt16 bitsOfColor,
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

    
    DisplayMultispectralDialogInitialize(this,
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
            (SInt16*) & mPixelSizeVector[0],
            &m_gaussianStretch,
            &enhancement,
            &m_MinMaxCode,
            &minMaxPopupCode,
            &m_thematicDisplayWithUserSettingFlag,
            &treatZeroAs,
            (SInt16*) & mColorLevelsMax[0][0],
            &numberLevels,
            (Boolean*) & m_ComputeHistogramFlag,
            &channelSelection,
            (Boolean*) & m_redChannelInvertFlag,
            (Boolean*) & m_greenChannelInvertFlag,
            (Boolean*) & m_blueChannelInvertFlag,
            &m_thematicLegendFactor,
            (Boolean*) & m_vectorOverlaysFlag);

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
    //m_TreatZeroAs = treatZeroAs - 1;
    m_TreatZeroAs = treatZeroAs;
    m_channelSelection = channelSelection;
    m_NumberLevels = numberLevels;

    m_LineStart = m_displaySpecsPtr->lineStart;
    m_LineEnd = m_displaySpecsPtr->lineEnd;
    m_LineInterval = m_displaySpecsPtr->lineInterval;
    m_ColumnStart = m_displaySpecsPtr->columnStart;
    m_ColumnEnd = m_displaySpecsPtr->columnEnd;
    m_ColumnInterval = m_displaySpecsPtr->columnInterval;
    
    /*This function changes the value of mLocalDisplayType*/
    /*Handling OnDropDownBitsofcolor here*/
    /////////////////////////////////////////////////////////////////
    wxChoice* comboBoxPtr = (wxChoice*) FindWindow(ID3C_BitsOfColor);
/*
    if (mLocalDisplayType == 2 || mLocalDisplayType == 7) {
        if (comboBoxPtr->GetCount() == 2)
            // Remove the '24' item from the bits of color list.

            comboBoxPtr->Delete(1);

    }// end "if (mLocalDisplayType == 2 || mLocalDisplayType == 7)"

    else // mLocalDisplayType != 2 && mLocalDisplayType != 7
    {
        if (comboBoxPtr->GetCount() == 1)
            comboBoxPtr->Append(wxT("24"));

  } // end "else mLocalDisplayType != 2 && ..."
  */

   if (m_DisplayType > 1 && m_DisplayType < 6) {
         // Remove the '8' item from the bits of color list.
      if (comboBoxPtr->GetCount() == 2){
            comboBoxPtr->Delete(0);
      }
    }// end "if (mLocalDisplayType > 2 || mLocalDisplayType < 7)"
    else // mLocalDisplayType <= 2 && mLocalDisplayType == 7
    {
      if (comboBoxPtr->GetCount() == 2){
            comboBoxPtr->Delete(1);
      }            
    } // end "else mLocalDisplayType <= 2 && ..."
    int numb = comboBoxPtr->GetCount();
    comboBoxPtr->SetSelection (0);

    // Set limit values

    mMaxChannelNumber = gImageWindowInfoPtr->totalNumberChannels;

    // Make sure that we have the bitmaps for the entire image buttons.

    SetEntireImageButtons(
            NULL,
            m_LineStart,
            m_LineEnd,
            m_ColumnStart,
            m_ColumnEnd);

    // Get pointer to file information.

    //	fileInfoPtr = gImageFileInfoPtr;

    // Get pointer to window information.

    //	windowInfoPtr = gImageWindowInfoPtr;

    // Load current feature pointer and number channels.

    //	m_localNumberFeatures = m_displaySpecsPtr->numberChannels;

    //	SInt16* channelsPtr = (SInt16*)GetHandlePointer (
    //										m_displaySpecsPtr->channelsHandle, kNoLock, kNoMoveHi);
    //	UInt16	index;
    //	for (index=0; index<m_localNumberFeatures; index++)
    //		m_localFeaturesPtr[index] = channelsPtr[index];

    // Set feature parameters

    m_localActiveNumberFeatures = m_localNumberFeatures;
    m_localActiveFeaturesPtr = m_localFeaturesPtr;

    // Set requests of inverting data value with respect to the display color.

    //	m_redChannelInvertFlag = m_displaySpecsPtr->invertValuesFlag[2];
    //	m_greenChannelInvertFlag = m_displaySpecsPtr->invertValuesFlag[1];
    //	m_blueChannelInvertFlag = m_displaySpecsPtr->invertValuesFlag[0];

    // Update the display specification dialog with the information
    // given in the image specification

    // To entire image icon.

    //	LoadLineColumnItems ( &dialogSelectArea, dialogPtr);

    // Remove the 'User defined...' item from the min-maxes list.

    //	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(ID3C_MinMaxValues);
    //	comboBoxPtr->DeleteString(2);

    // Compute histogram dialog item

    //	SetComputeHistogramDialogItem (
    //								m_ComputeHistogramFlag,
    //								NULL,
    //								ID3C_NewHistogram);

    // Display channel descriptions.

	//GetDlgItem(ID3C_ChannelDescriptions)->EnableWindow(fileInfoPtr->descriptionsFlag);

	TransferDataToWindow ();

			// Set up edit text fields for channels to be used for diplay
	
	SetSizerAndFit (bSizer67);
	UpdateDialogChannelItems (this, mRGBColors, mLocalDisplayType);
   
	this->Layout();
	this->Fit();
	PositionDialogWindow();

}	// end "OnInitDialog"


void CMDisplaySpecsDlg::OnSelendokBitsOfColor (
				wxCommandEvent&									event) 
{
	SInt16 numberLevels;


			// Get current bits of color setting.
			
	m_BitsOfColor = ((wxChoice *) FindWindow(ID3C_BitsOfColor))->GetSelection(); 
	if(m_DisplayType > 1 && m_DisplayType < 6)
		m_BitsOfColor = 2; // color image
	else
		m_BitsOfColor = 0; // grey  image
    
			// Get index into 8, 16, 24 bits of color vector.

	//mBitsOfColorIndex = Update8_16_24BitsOfColorIndex(m_BitsOfColor);

			// Save the bits of color index.

	m_SavedBitsOfColor = m_BitsOfColor;

			// Verify number of levels setting

	//CheckDisplayLevels (NULL, mLocalDisplayType);
	DisplayMultispectralDialogCheckDisplayLevels (
												m_displaySpecsPtr,
												this,
												mLocalDisplayType,
												m_RedChannel,
												m_GreenChannel,
												m_BlueChannel,
												(SInt16*) & mColorLevelsMax[0][0],
												&mDuplicateChannelCode,
												(SInt16) m_BitsOfColor + 1,
												&numberLevels);

    m_NumberLevels = numberLevels;

} // end "OnSelendokBitsOfColor"


void CMDisplaySpecsDlg::OnSelendokChannels(wxCommandEvent& event) 
{
	HandleChannelsMenu (ID3C_Channels,
								kNoTransformation,
								(SInt16)gImageWindowInfoPtr->totalNumberChannels,
								kImageChannelType,
								TRUE);

}	// end "OnSelendokChannels"


void CMDisplaySpecsDlg::OnSelendokChannelsDropDown(wxCommandEvent& event) 
{
	wxComboBox* classcombo = (wxComboBox*)FindWindow (ID3C_Channels);
	classcombo->SetSelection (0);

}	// end "OnSelendokChannels"


void CMDisplaySpecsDlg::OnSelendokDisplayType(wxCommandEvent& event)

{
    SInt16* colorLevelsMaxPtr;

    SInt32 blueChannel,
            greenChannel,
            redChannel;

    SInt16 bitsOfColor,
            currentDisplayTypeMenuSelection,
            minMaxPopupCode,
            numberDisplayLevels;

    Boolean checkMinMaxSettingFlag;


    currentDisplayTypeMenuSelection = m_DisplayType + 1;
    
    m_DisplayType = ((wxComboBox*)FindWindow(ID3C_DisplayType))->GetSelection();

    blueChannel = m_BlueChannel;
    greenChannel = m_GreenChannel;
    redChannel = m_RedChannel;
    bitsOfColor = m_BitsOfColor + 1; // Make index from 1 not 0

    colorLevelsMaxPtr = (SInt16*)&mColorLevelsMax[0][0];

    DisplayMultispectralDialogUpdateDisplayType(m_displaySpecsPtr,
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

//    m_BitsOfColor = bitsOfColor - 1;
//    
//    m_NumberLevels = numberDisplayLevels;
//
//    //DDX_CBIndex(m_dialogToPtr, ID3C_BitsOfColor, m_BitsOfColor);
//    ((wxChoice *) FindWindow(ID3C_BitsOfColor))->SetSelection(m_BitsOfColor);
//
//    // Get index into 8, 16, 24 bits of color vector.
//
//    mBitsOfColorIndex = Update8_16_24BitsOfColorIndex(m_BitsOfColor);

    // Check min-max menu settings.

    minMaxPopupCode = m_MinMaxPopupCode + 1;
    DisplayMultispectralDialogCheckMinMaxSettings (m_DisplayType + 1,
            m_MinMaxPopupCode + 1,
            &minMaxPopupCode,
            &m_MinMaxCode,
            &m_thematicDisplayWithUserSettingFlag);

    m_MinMaxPopupCode = minMaxPopupCode - 1;
    m_minmaxctrl->SetSelection (m_MinMaxPopupCode);

    DisplayMultispectralDialogUpdateComputeHistogram(
            m_displaySpecsPtr,
            m_ComputeHistogramFlag,
            m_DisplayType + 1,
            m_Enhancement + 1,
            m_MinMaxPopupCode + 1,
            this,
            ID3C_NewHistogram);

    /*This function changes the value of mLocalDisplayType*/
    /*Handling OnDropDownBitsofcolor here*/

    wxChoice* comboBoxPtr = (wxChoice*) FindWindow(ID3C_BitsOfColor);
/*
    if (mLocalDisplayType == 2 || mLocalDisplayType == 7) {
        if (comboBoxPtr->GetCount() == 2)
            // Remove the '24' item from the bits of color list.

            comboBoxPtr->Delete(1);

    }// end "if (mLocalDisplayType == 2 || mLocalDisplayType == 7)"

    else // mLocalDisplayType != 2 && mLocalDisplayType != 7
    {
        if (comboBoxPtr->GetCount() == 1)
            comboBoxPtr->Append(wxT("24"));

    } // end "else mLocalDisplayType != 2 && ..."
    */
    if (m_DisplayType > 1 && m_DisplayType < 6) { // m_DisplayType count from 0
         // Remove the '8' item from the bits of color list.
            comboBoxPtr->Delete(0);
            comboBoxPtr->Append(wxT("24"));
            
    }else // m_DisplayType <= 1 && m_DisplayType == 6
    {
            comboBoxPtr->Delete(0);
            comboBoxPtr->Append(wxT("8"));
    } 
    comboBoxPtr->SetSelection (0);
    
    if(m_DisplayType > 1 && m_DisplayType < 6)
       m_BitsOfColor = 2; // color image
    else
       m_BitsOfColor = 0; // grey  image
    
//    m_BitsOfColor = bitsOfColor - 1;
    
    m_NumberLevels = numberDisplayLevels;

	//((wxChoice*)FindWindow (ID3C_BitsOfColor))->SetSelection (m_BitsOfColor);

    // Get index into 8, 16, 24 bits of color vector.

    mBitsOfColorIndex = Update8_16_24BitsOfColorIndex(m_BitsOfColor);
    
    this->Layout();

} // end "OnSelendokDisplayType"



void CMDisplaySpecsDlg::OnSelendokEnhancement (
				wxCommandEvent& event)

{
   m_Enhancement = event.GetSelection();

	if (m_Enhancement == 2 && m_optionKeyFlag)
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
	
}	// end "OnSelendokEnhancement"



void CMDisplaySpecsDlg::OnSelendokEnhancementCloseUp (
				wxCommandEvent& event)

{
   int enhancement = event.GetSelection ();


			// set to -1 so that it can be active again when user selects it second time directly
	
	//m_enhancementctrl->SetSelection (m_Enhancement);
	//m_enhancementctrl->SetSelection (-1);
	
}	// end "OnSelendokEnhancementDropDown"



void CMDisplaySpecsDlg::OnSelendokEnhancementDropDown (
				wxCommandEvent& event)

{
	m_optionKeyFlag = FALSE;
    
	wxMouseState mousestate;
	
	if (mousestate.RightIsDown () || wxGetKeyState (WXK_SHIFT) || m_gaussianStretch != 2.3)
		{
		SetComboItemText (ID3C_Enhancement,
								2,
								(char*)"Gaussian...");
         
		m_optionKeyFlag = TRUE;

		}	// end "if (GetKeyState (VK_RBUTTON) < 0)"
	
	else	// !mousestate.RightIsDown () && !wxGetKeyState (WXK_SHIFT))
		{
		SetComboItemText (ID3C_Enhancement,
								2,
								(char*)"Gaussian");
		
    	}	// end "else !mousestate.RightIsDown () && .."
	
			// Set to -1 so that it can be active again when user selects it second time directly
			// when the current selection is Gaussian. We need to force the Gaussian dialog
			// to be opened.
	
	if (m_Enhancement == 2 && (m_gaussianStretch != 2.3 || m_optionKeyFlag))
		m_enhancementctrl->SetSelection (-1);
	else	// enhancement != 2
		m_enhancementctrl->SetSelection (m_Enhancement);
	
}	// end "OnSelendokEnhancementDropDown"



void CMDisplaySpecsDlg::OnSelendokMinMaxValues (
				wxCommandEvent& 					event)

{
	SInt16 channels[3];


	m_MinMaxPopupCode = m_minmaxctrl->GetSelection();
  
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
      
		channels[0] = (SInt16) m_RedChannel;
		channels[1] = (SInt16) m_GreenChannel;
		channels[2] = (SInt16) m_BlueChannel;

		if (mLocalDisplayType <= 2)
			channels[0] = (SInt16) m_GrayChannel;

		MinMaxEnhancementDialog(channels,
				 mRGBColors,
				 mLocalDisplayType,
				 m_NumberLevels,
				 &m_PercentClip,
				 &m_MinMaxCode,
				 mMinMaxValues);

		m_MinMaxPopupCode = GetMinMaxPopupCode(m_MinMaxCode, m_PercentClip);
		m_minmaxctrl->SetSelection(m_MinMaxPopupCode);

		SetDLogControlHilite(NULL, wxID_OK, 0);

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
				wxCommandEvent& event)

{
   int minMaxPopupCode = event.GetSelection ();
	
	event.Skip ();

			// set to -1 so that it can be active again when user selects it second time directly
	
	//m_enhancementctrl->SetSelection (m_Enhancement);
	//m_enhancementctrl->SetSelection (-1);
	
}	// end "OnSelendokMinMaxValuesCloseUp"



void CMDisplaySpecsDlg::OnSelendokMinMaxValuesDropDown (
				wxCommandEvent& event)

{
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.
	
	if (m_MinMaxPopupCode == 3)
		m_minmaxctrl->SetSelection (-1);
	//m_MinMaxPopupCode = 1;
	//m_minmaxctrl->SetSelection (-1);
}


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetComputeHistogramDialogItem
//
//	Software purpose:	The purpose of this routine is to draw the enhancement
//							prompt and enhancement selection for the enhancement
//							pop up menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/05/91
//	Revised By:			Larry L. Biehl			Date: 06/06/95	

void 
CMDisplaySpecsDlg::SetComputeHistogramDialogItem ( 
                        Boolean						computeItemFlag,
                        DialogPtr					dialogPtr,
                        SInt16						itemNumber)

{                                         
		
        if (computeItemFlag)
                SetDLogControlHilite (dialogPtr, itemNumber, 255);
			
        else		// !computeItemFlag
                SetDLogControlHilite (dialogPtr, itemNumber, 0);
	
}		// end "SetComputeHistogramDialogItem" 
 */


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDialogChannelItems
//
//	Software purpose:	The purpose of this routine is to update the dialog 
//							channel items in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/93
//	Revised By:			Larry L. Biehl			Date: 06/09/95	

void 
CMDisplaySpecsDlg::UpdateDialogChannelItems (
                                DialogPtr				dialogPtr,
                                SInt16					rgbColors,
                                SInt16					displayType)

{
        SInt16				hiliteItem;
	
	
                        // text fields for channels to be displayed
			
                        // Side by side channel display.
								
        ShowHideDialogItem ( NULL, ID3C_Channels, (displayType == 7) );
//	GetDlgItem(ID3C_Channels)->EnableWindow(FALSE);
			
                        // 1-channel display channel number
	
        ShowHideDialogItem ( NULL, ID3C_GrayChannel, (rgbColors & 8) );
        ShowHideDialogItem ( NULL, ID3C_STATIC35, (rgbColors & 8) );
        if (displayType == 7)
                MHideDialogItem (NULL, ID3C_GrayChannel);
		
                        // red channel number
				
        ShowHideDialogItem ( NULL, ID3C_STATIC28, (rgbColors & 4) );
        ShowHideDialogItem ( NULL, ID3C_RedChannel, (rgbColors & 4) );
		
                        // green channel number
				
        ShowHideDialogItem ( NULL, ID3C_STATIC29, (rgbColors & 2) );
        ShowHideDialogItem ( NULL, ID3C_GreenChannel, (rgbColors & 2) );
		
                        // blue channel number
				
        ShowHideDialogItem ( NULL, ID3C_STATIC30, (rgbColors & 1) );
        ShowHideDialogItem ( NULL, ID3C_BlueChannel, (rgbColors & 1) );
	
                        // Set intial focus (hilite) to the appropriate edit text item
	
        if (m_DisplayType+2 == 7)
                hiliteItem = IDC_LineStart;
		
        else		// m_DisplayType+2 != 7
                {
                hiliteItem = ID3C_GrayChannel;
		
                if (m_DisplayType+2 > 2)
                        hiliteItem = ID3C_RedChannel;
		
                if (m_DisplayType+2 == 4)
                        hiliteItem = ID3C_GreenChannel;
		
                }		// end "else m_DisplayType+2 != 7"
		                                       
//	GetDlgItem(hiliteItem)->SetFocus();
//	SendDlgItemMessage( hiliteItem, EM_SETSEL, 0, MAKELPARAM(0, -1) );
        SelectDialogItemText (this, hiliteItem, 0, SInt16_MAX);
	
}		// end "UpdateDialogChannelItems"
 */

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateDisplayLevels
//
//	Software purpose:	The purpose of this routine is to update the display
//							levels dialog item in the multispectral display dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/24/93
//	Revised By:			Larry L. Biehl			Date: 06/01/95	

void 
CMDisplaySpecsDlg::UpdateDisplayLevels (
                                DialogPtr					dialogPtr,
                                SInt16						displayType,
                                SInt16						duplicateChannelCode)

{                                         
        m_NumberLevels = GetMaximumDisplayLevels (
                                                        mBitsOfColorIndex,
                                                        displayType,
                                                        duplicateChannelCode);
							
        DDX_Text(m_dialogToPtr, ID3C_NumberOfLevels, m_NumberLevels);
	
}		// end "UpdateDisplayLevels"
 */


SInt16
CMDisplaySpecsDlg::Update8_16_24BitsOfColorIndex(
        SInt16 menuBitsOfColorIndex) {
    // Get index into 8, 16, 24 bits of color vector.

    SInt16 bitsOfColorIndex = menuBitsOfColorIndex;
    if (bitsOfColorIndex == 1)
        bitsOfColorIndex = 2;

    return (bitsOfColorIndex);

} // end "Update8_16_24BitsOfColorIndex"

//bool CMDisplaySpecsDlg::Create(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style) {
//    if (!wxDialog::Create(parent, id, caption, pos, size, style))
//        return false;
//    CreateControls();
//    GetSizer()->Fit(this);
//    GetSizer()->SetSizeHints(this);
//    Centre();
//    return true;
//}

/*
void CMDisplaySpecsDlg::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    wxDialog::Update();
}
*/


bool CMDisplaySpecsDlg::TransferDataFromWindow() 
{
	SInt16		returnCode;
	
	
	wxComboBox* c_channels = (wxComboBox *) FindWindow(ID3C_Channels);
	wxChoice* bitsofcol = (wxChoice *) FindWindow(ID3C_BitsOfColor);
   wxComboBox* enhance = (wxComboBox *) FindWindow(ID3C_Enhancement);
	wxChoice* tzeroas = (wxChoice *) FindWindow(ID3C_TreatZeroAs);

	wxComboBox* disptype = (wxComboBox*) FindWindow(ID3C_DisplayType);
	wxComboBox* nofdispl = (wxComboBox*) FindWindow(ID3C_NumberOfLevels);

	wxCheckBox* rinv = (wxCheckBox*) FindWindow(IDC_RedChannelInvert);
	wxCheckBox* ginv = (wxCheckBox*) FindWindow(IDC_GreenChannelInvert);
	wxCheckBox* binv = (wxCheckBox*) FindWindow(IDC_BlueChannelInvert);
	wxCheckBox* voverlays = (wxCheckBox*) FindWindow(IDC_VectorOverlays);
	wxCheckBox* newhist = (wxCheckBox*) FindWindow(ID3C_NewHistogram);

	wxTextCtrl* bchannel = (wxTextCtrl*) FindWindow(ID3C_BlueChannel);
	wxTextCtrl* gchannel = (wxTextCtrl*) FindWindow(ID3C_GreenChannel);
	wxTextCtrl* rchannel = (wxTextCtrl*) FindWindow(ID3C_RedChannel);
	wxTextCtrl* grchannel = (wxTextCtrl*) FindWindow(ID3C_GrayChannel);
	wxTextCtrl* magnctrl = (wxTextCtrl*) FindWindow(ID3C_Magnification);
	wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
	wxTextCtrl* legendfactor = (wxTextCtrl*) FindWindow(IDC_LegendFactor);

	m_redChannelInvertFlag = rinv->GetValue();
	m_greenChannelInvertFlag = ginv->GetValue();
	m_blueChannelInvertFlag = binv->GetValue();
	m_vectorOverlaysFlag = voverlays->GetValue();
	m_ComputeHistogramFlag = newhist->GetValue();

	m_BitsOfColor = bitsofcol->GetSelection();
   if(enhance->GetSelection() > -1)
      m_Enhancement = enhance->GetSelection();
	m_TreatZeroAs = tzeroas->GetSelection();
	m_MinMaxPopupCode = m_minmaxctrl->GetSelection();
	m_channelSelection = c_channels->GetSelection();

	m_DisplayType = disptype->GetSelection();
	m_NumberLevels = (UINT) (nofdispl->GetSelection());
	//m_numberLevelsval = (UINT)wxAtoi(nofdispl->GetStringSelection());
	m_numberLevelsval = wxAtoi(nofdispl->GetValue());

	m_LineStartString = l_start->GetValue();
	m_LineEndString = l_end->GetValue();
	m_LineIntervalString = l_inter->GetValue();
	m_ColumnStartString = c_start->GetValue();
	m_ColumnEndString = c_end->GetValue();
	m_ColumnIntervalString = c_inter->GetValue();
	m_RedChannelString = rchannel->GetValue();
	m_GreenChannelString = gchannel->GetValue();
	m_BlueChannelString = bchannel->GetValue();
	m_GrayChannelString = grchannel->GetValue();
	m_thematicLegendFactorString = legendfactor->GetValue();
	m_Magnificationstring = magnctrl->GetValue();
	//m_Magnification = magnctrl->GetValue();
	m_LineStart = wxAtoi(m_LineStartString);
	m_LineEnd = wxAtoi(m_LineEndString);
	m_LineInterval = wxAtoi(m_LineIntervalString);
	m_ColumnStart = wxAtoi(m_ColumnStartString);
	m_ColumnEnd = wxAtoi(m_ColumnEndString);
	m_ColumnInterval = wxAtoi(m_ColumnIntervalString);
	//    m_LineStartString.ToLong(&m_LineStart);
	//    m_LineEndString.ToLong(&m_LineEnd);
	//    m_LineIntervalString.ToLong(&m_LineInterval);
	//    m_ColumnStartString.ToLong(&m_ColumnStart);
	//    m_ColumnEndString.ToLong(&m_ColumnEnd);
	//    m_ColumnIntervalString.ToLong(&m_ColumnInterval);
	m_RedChannelString.ToULong(&m_RedChannel);
	m_GreenChannelString.ToULong(&m_GreenChannel);
	m_BlueChannelString.ToULong(&m_BlueChannel);
	m_GrayChannelString.ToULong(&m_GrayChannel);
	m_thematicLegendFactorString.ToDouble(&m_thematicLegendFactor);
	m_Magnificationstring.ToDouble(&m_Magnification);

		// Get bits of color index for 8(0), 16(1),24(2) bit vector.

//	mBitsOfColorIndex = m_BitsOfColor;
//	if (mBitsOfColorIndex == 1)
//	  mBitsOfColorIndex = 2;
   
   if(m_DisplayType > 1 && m_DisplayType < 6)
      mBitsOfColorIndex = 2; // color image, 24 bits
   else
      mBitsOfColorIndex = 0; // 8 bits

	returnCode = VerifyLineColumnValues(
						  IDC_LineStart,
						  IDC_ColumnStart,
						  true);
	/*					  
	int numberChars = sprintf ((char*)&gTextString3,
											" CMDisplaySpecsDlg::TransferDataFromWindow() returnCode: %d%s", 
											returnCode,
											gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);
	*/
	if (returnCode == 0) {
				// Verify that the number of lines to be displayed is less than
				// the maximum possible.

		if (CheckNumberDisplayLines(
				m_LineStart,
				m_LineEnd,
				m_LineInterval,
				NULL,
				IDC_LineStart) == 0)
			returnCode = IDC_LineEnd;
			
		}		// end "if (returnCode == 0)"

	if (returnCode == 0) {
				// Verify that the number of columns to be displayed is less than
				// the maximum possible.
				// This part needs to be updated.

		if (CheckNumberDisplayColumns (
				m_ColumnStart,
				m_ColumnEnd,
				m_ColumnInterval,
				24,
				m_DisplayType+1,
				m_localActiveNumberFeatures,
				NULL,
				IDC_ColumnStart) == 0)
			returnCode = IDC_ColumnEnd;
			
		}		// end "if (returnCode == 0)"	
		
			// Verify that channels entered for red, green, blue, gray are not blank.
			
	if (returnCode == 0) {
		if (mRGBColors <= k1_ChannelGrayLevelDisplayType && m_GrayChannelString.empty())
			returnCode = ID3C_GrayChannel;
		
		else if (mRGBColors & 0x0004 && m_RedChannelString.empty())
			returnCode = ID3C_RedChannel;
		
		else if (mRGBColors & 0x0002 && m_GreenChannelString.empty())
			returnCode = ID3C_GreenChannel;
		
		else if (mRGBColors & 0x0001 && m_BlueChannelString.empty())
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
				
			}		// end "if (returnCode != 0)"
	
		}		// end "if (returnCode == 0)"
			
   if(returnCode == 0)
         returnCode = CheckDialogRealValue (this,
													ID3C_NumberOfLevels,
													1, 
                                       256,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);  
	return (returnCode == 0);
	 
}		// end "TransferDataFromWindow"



bool CMDisplaySpecsDlg::TransferDataToWindow ()

{
	wxComboBox* c_channels = (wxComboBox *)FindWindow(ID3C_Channels);
	wxChoice* bitsofcol = (wxChoice *) FindWindow(ID3C_BitsOfColor);
	wxComboBox* enhance = (wxComboBox *) FindWindow(ID3C_Enhancement);
	wxChoice* tzeroas = (wxChoice *) FindWindow(ID3C_TreatZeroAs);

	wxComboBox* disptype = (wxComboBox*)FindWindow(ID3C_DisplayType);
	wxComboBox* nofdispl = (wxComboBox*) FindWindow(ID3C_NumberOfLevels);

	wxCheckBox* rinv = (wxCheckBox*) FindWindow(IDC_RedChannelInvert);
	wxCheckBox* ginv = (wxCheckBox*) FindWindow(IDC_GreenChannelInvert);
	wxCheckBox* binv = (wxCheckBox*) FindWindow(IDC_BlueChannelInvert);
	wxCheckBox* voverlays = (wxCheckBox*) FindWindow(IDC_VectorOverlays);
	wxCheckBox* newhist = (wxCheckBox*) FindWindow(ID3C_NewHistogram);

	wxTextCtrl* bchannel = (wxTextCtrl*) FindWindow(ID3C_BlueChannel);
	wxTextCtrl* gchannel = (wxTextCtrl*) FindWindow(ID3C_GreenChannel);
	wxTextCtrl* rchannel = (wxTextCtrl*) FindWindow(ID3C_RedChannel);
	wxTextCtrl* grchannel = (wxTextCtrl*) FindWindow(ID3C_GrayChannel);
	wxTextCtrl* magnctrl = (wxTextCtrl*) FindWindow(ID3C_Magnification);
	wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
	wxTextCtrl* legendfactor = (wxTextCtrl*) FindWindow(IDC_LegendFactor);

	c_channels->SetSelection (m_channelSelection);
	enhance->SetSelection (m_Enhancement);
	m_minmaxctrl->SetSelection (m_MinMaxPopupCode);
	tzeroas->SetSelection (m_TreatZeroAs);

	rinv->SetValue (m_redChannelInvertFlag);
	ginv->SetValue (m_greenChannelInvertFlag);
	binv->SetValue (m_blueChannelInvertFlag);
	voverlays->SetValue (m_vectorOverlaysFlag);
	newhist->SetValue (m_ComputeHistogramFlag);

	disptype->SetSelection (m_DisplayType);
	nofdispl->SetStringSelection (wxString::Format (wxT("%i"), (int)m_NumberLevels));

	bchannel->Clear();
	gchannel->Clear();
	rchannel->Clear();
	grchannel->Clear();
	magnctrl->Clear();

	legendfactor->Clear();

	*bchannel << (int) m_BlueChannel;
	*gchannel << (int) m_GreenChannel;
	*rchannel << (int) m_RedChannel;
	*grchannel << (int) m_GrayChannel;

	magnctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Magnification));

	c_end->ChangeValue(wxString::Format(wxT("%i"), m_ColumnEnd));
	c_inter->ChangeValue(wxString::Format(wxT("%i"), m_ColumnInterval));
	c_start->ChangeValue(wxString::Format(wxT("%i"), m_ColumnStart));
	l_end->ChangeValue(wxString::Format(wxT("%i"), m_LineEnd));
	l_inter->ChangeValue(wxString::Format(wxT("%i"), m_LineInterval));
	l_start->ChangeValue(wxString::Format(wxT("%i"), m_LineStart));
	*legendfactor << m_thematicLegendFactor;

	SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);

	return true;
	
}
