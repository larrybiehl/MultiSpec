//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDisplayThematicDialog.cpp : class implementation file
//	Class Definition:		LDisplayThematicDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/30/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMDisplayThematicDlg class.
//
// Following is template for debugging
/*
		int numberChars = sprintf ((char*)gTextString3,
									 " LDisplayThematicDialog:: (): %s",
									 gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------
//
#include "SMultiSpec.h"

#include "CDisplay.h" 
#include "CImageWindow.h" 

#include "LClassGroupDialog.h"
#include "LDisplayThematicDialog.h"  
#include "LImageView.h"  
#include "LImage_dialog.cpp"
#include "LLegendView.h"

#include "wx/window.h"
#include "wx/valnum.h"



// CMDisplayThematicDlg dialog

CMDisplayThematicDlg::CMDisplayThematicDlg ()

{
}	// end "CMDisplayThematicDlg"



CMDisplayThematicDlg::CMDisplayThematicDlg (
				wxWindow*							pParent, 
				wxWindowID							id, 
				const wxString&					title, 
				const wxPoint&						pos, 
				const wxSize&						size, 
				long									style)
	: CMDialog (CMDisplayThematicDlg::IDD, pParent, title) 
{
	FileStringPtr						fileNamePtr;
	
	
	m_classGroupSelection = 0;
	m_classSelection = 0;
	m_showLegendFlag = FALSE;
	m_paletteSelection = 0;
	m_numberClasses = "";
	m_numberGroups = "";
	m_fileName = "";
	m_magnification = 1.;
	m_useThresholdFileFlag = FALSE;
	m_includeVectorOverlaysFlag = FALSE;

	m_localClassGroupsHandle = NULL;
	m_localClassToGroupPtr = NULL;
	m_localClassGroupsPtr = NULL;

	m_initializedFlag = CMDialog::m_initializedFlag;

	CreateControls();

	if (m_initializedFlag) 
		{
				//	Set the image file name
				
		fileNamePtr = (FileStringPtr)(gActiveImageViewCPtr->GetImageWindowCPtr())->GetImageFileNamePointer();
		if (fileNamePtr != NULL)
			m_fileName = wxString(fileNamePtr, wxConvUTF8);
		wxStaticText * fname = (wxStaticText*) FindWindow(IDC_FileName);
		fname->SetLabel(m_fileName);

		}	// end "if (m_initializedFlag)"

	if (m_initializedFlag) 
		{
				// Get pointer to local class to group list.									

		m_localClassGroupsHandle = MNewHandle (
				 (SInt32) gImageFileInfoPtr->numberClasses * 2 * sizeof (SInt16));

		m_initializedFlag = (m_localClassGroupsHandle != NULL);

		}	// end "if (m_initializedFlag)"

	if (m_initializedFlag) 
		{
		m_localClassGroupsPtr = (SInt16*)GetHandlePointer (
												m_localClassGroupsHandle, kLock, kNoMoveHi);
		}	// end "if (m_initializedFlag)"                   

	Centre();
	//if (!m_initializedFlag)
	//  return;

}	// end "CMDisplayThematicDlg" 



CMDisplayThematicDlg::~CMDisplayThematicDlg (void)

{
	m_localClassGroupsHandle = UnlockAndDispose(m_localClassGroupsHandle);

	m_localClassToGroupPtr = CheckAndDisposePtr(m_localClassToGroupPtr);

}	// end "~CMDisplayThematicDlg"



BEGIN_EVENT_TABLE (CMDisplayThematicDlg, CMDialog)
	EVT_BUTTON (IDC_BackgroundColor, CMDisplayThematicDlg::OnBackgroundColor)
	EVT_BUTTON (IDEntireImage, CMDisplayThematicDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMDisplayThematicDlg::ToSelectedImage)
	//EVT_CHAR_HOOK (CMDisplayThematicDlg::OnCharHook)
	EVT_COMBOBOX (IDC_PaletteCombo, CMDisplayThematicDlg::OnSelendokPaletteCombo)
	EVT_COMBOBOX (IDC_ClassesGroupsCombo, CMDisplayThematicDlg::OnSelendokClassesGroupsCombo)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassesGroupsCombo, CMDisplayThematicDlg::OnSelendokChannelComboDropDown)
	EVT_COMBOBOX_DROPDOWN (IDC_PaletteCombo, CMDisplayThematicDlg::OnDropdownPaletteCombo)
	EVT_INIT_DIALOG (CMDisplayThematicDlg::OnInitDialog)
	EVT_TEXT (ID3C_ColumnEnd, CMDisplayThematicDlg::CheckColumnEnd)
	EVT_TEXT (ID3C_ColumnStart, CMDisplayThematicDlg::CheckColumnStart)
	EVT_TEXT (ID3C_ColumnInterval, CMDisplayThematicDlg::CheckColumnInterval)
	EVT_TEXT (ID3C_LineEnd, CMDisplayThematicDlg::CheckLineEnd)
	EVT_TEXT (ID3C_LineStart, CMDisplayThematicDlg::CheckLineStart)
	EVT_TEXT (ID3C_LineInterval, CMDisplayThematicDlg::CheckLineInterval)
END_EVENT_TABLE()



void CMDisplayThematicDlg::CreateControls ()

{
	wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
	wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
	wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
	wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);

	this->SetSizeHints (wxDefaultSize, wxDefaultSize);

	//wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer(wxVERTICAL);

	m_staticText30 = new wxStaticText(this, IDC_FileName, wxT("file_name"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText30->Wrap(-1);
	//bSizer25->Add (m_staticText30, 0, wxALL, 5);
	bSizer25->Add (m_staticText30, wxSizerFlags(0).Border(wxBOTTOM,5));

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Area to Display")),
												wxHORIZONTAL);
	
	CreateLineColumnControls (sbSizer8);
	/*
	wxBoxSizer* bSizer791;
	bSizer791 = new wxBoxSizer(wxHORIZONTAL);

	//m_bpButton4 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDEntireImage, wxBitmap(wxT("../../NetBeansProjects/CWP/project/trunk/res/entireim.bmp"), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_bpButton4 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_bpButton4->SetBitmapDisabled(toentirei);
	//m_bpButton4->SetToolTip(wxT("Switch between the user selected area and the entire image area"));
   SetUpToolTip(m_bpButton4, IDS_ToolTip40);
	bSizer791->Add(m_bpButton4, 0, wxALIGN_CENTER_VERTICAL | wxLEFT|wxTOP|wxBOTTOM, 5);

	// m_bpButton5 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDSelectedImage, wxBitmap(wxT("../../NetBeansProjects/CWP/project/trunk/res/selected.bmp"), wxBITMAP_TYPE_ANY), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_bpButton5 = new wxBitmapButton(sbSizer8->GetStaticBox(), IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_bpButton5->SetBitmapDisabled(bmp4i);
	bSizer791->Add(m_bpButton5, 0, wxALIGN_CENTER_VERTICAL | wxLEFT|wxTOP|wxBOTTOM, 5);


	sbSizer8->Add(bSizer791, 0, wxEXPAND, 5);

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer(3, 4, 0, 0);

	m_staticText63 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_staticText63->Wrap(-1);
	gSizer1->Add(m_staticText63, 0, wxTOP|wxRIGHT|wxBOTTOM, 5);

	m_staticText64 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText64->Wrap(-1);
	gSizer1->Add(m_staticText64, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText65 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("End"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText65->Wrap(-1);
	gSizer1->Add(m_staticText65, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText66 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Interval"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText66->Wrap(-1);
	gSizer1->Add(m_staticText66, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText60 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Line"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText60->Wrap(-1);
	gSizer1->Add(m_staticText60, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL | wxTOP|wxRIGHT|wxBOTTOM, 5);

	m_linestartctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), ID3C_LineStart, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_linestartctrl, IDS_ToolTip19);
	m_linestartctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_LineStartString));

	gSizer1->Add(m_linestartctrl, 0, wxALIGN_CENTER | wxALL, 5);

	m_lineendctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), ID3C_LineEnd, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_lineendctrl, IDS_ToolTip20);
	m_lineendctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_LineEndString));

	gSizer1->Add(m_lineendctrl, 0, wxALIGN_CENTER | wxALL, 5);

	m_lineintctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), ID3C_LineInterval, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_lineintctrl, IDS_ToolTip21);
	m_lineintctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_LineIntervalString));

	gSizer1->Add(m_lineintctrl, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText62 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Column"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText62->Wrap(-1);
	gSizer1->Add(m_staticText62, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL | wxTOP|wxRIGHT|wxBOTTOM, 5);

	m_colstartctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), ID3C_ColumnStart, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_colstartctrl, IDS_ToolTip22);
	m_colstartctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_ColumnStartString));

	gSizer1->Add(m_colstartctrl, 0, wxALIGN_CENTER | wxALL, 5);

	m_colendctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), ID3C_ColumnEnd, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_colendctrl, IDS_ToolTip23);
	m_colendctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_ColumnEndString));

	gSizer1->Add(m_colendctrl, 0, wxALIGN_CENTER | wxALL, 5);

	m_colintctrl = new wxTextCtrl(sbSizer8->GetStaticBox(), ID3C_ColumnInterval, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_colintctrl, IDS_ToolTip24);
	m_colintctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_ColumnIntervalString));

	gSizer1->Add(m_colintctrl, 0, wxALIGN_CENTER | wxALL, 5);

	sbSizer8->Add(gSizer1, 1, wxEXPAND, 5);
	*/
	bSizer25->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	bSizer19->Add (bSizer25, wxSizerFlags(1).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer(wxVERTICAL);

	m_staticText31 = new wxStaticText(this, wxID_ANY, wxT("Magnification"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText31->Wrap(-1);
   SetUpToolTip(m_staticText31, IDS_ToolTip33);
	bSizer26->Add(m_staticText31, 0, wxALL, 5);

	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText38 = new wxStaticText(this, wxID_ANY, wxT("x"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText38->Wrap(-1);
	bSizer34->Add(m_staticText38, 0, wxALIGN_CENTER | wxALL, 5);

	wxFloatingPointValidator<double> _val(3, &m_magnification);
	_val.SetRange(0.01, 99.);
	m_textCtrl14 = new wxTextCtrl(this, IDC_Magnification, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl14, IDS_ToolTip33);
	m_textCtrl14->SetValidator(_val);
	//bSizer34->Add (m_textCtrl14, 0, wxALL, 5);
	bSizer34->Add (m_textCtrl14, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));


	//bSizer26->Add (bSizer34, 0, wxEXPAND, 5);
	bSizer26->Add (bSizer34, wxSizerFlags(0).Expand());


	//bSizer19->Add (bSizer26, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer19->Add (bSizer26, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));


	//bSizer18->Add (bSizer19, 0, wxALL | wxEXPAND, 5);
	bSizer18->Add (bSizer19, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT,12));

	//wxBoxSizer* bSizer20;
	//bSizer20 = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Options")), wxVERTICAL);

	//wxBoxSizer* bSizer27;
	//bSizer27 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText36 = new wxStaticText(sbSizer9->GetStaticBox(), wxID_ANY, wxT("Palette:"), wxDefaultPosition, wxDefaultSize, 0);	
   m_staticText36->Wrap(-1);
   SetUpToolTip(m_staticText36, IDS_ToolTip41);
	//bSizer31->Add (m_staticText36, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer31->Add (m_staticText36, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	m_paletteCombo = new wxComboBox (sbSizer9->GetStaticBox(), IDC_PaletteCombo, wxT("Default Colors"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_paletteCombo->Append(wxT("Default Colors"));
	m_paletteCombo->Append(wxT("Default Gray Levels"));
	m_paletteCombo->Append(wxT("Blue-Green-Red"));
	m_paletteCombo->Append(wxT("AVHRR NDVI"));
	m_paletteCombo->Append(wxT("MODIS NDVI"));
	m_paletteCombo->Append(wxT("False Color..."));
	m_paletteCombo->Append(wxT("ERDAS .trl file"));
	m_paletteCombo->Append(wxT("User Defined"));
	
	m_paletteCombo->SetClientData (0, (void*)kDefaultColors);
	m_paletteCombo->SetClientData (1, (void*)kDefaultGrays);
	m_paletteCombo->SetClientData (2, (void*)kCorrelationMatrixColors);
	m_paletteCombo->SetClientData (3, (void*)kAVHRR_NDVI_Colors);
	m_paletteCombo->SetClientData (4, (void*)kMODIS_NDVI_Colors);
	m_paletteCombo->SetClientData (5, (void*)kFalseColors);
	m_paletteCombo->SetClientData (6, (void*)kImageDefaultColorTable);
	m_paletteCombo->SetClientData (7, (void*)kUserDefinedColors);
	
	m_paletteCombo->SetSelection (0);
	
   SetUpToolTip (m_paletteCombo, IDS_ToolTip41);
	//bSizer31->Add (m_comboBox2, 0, wxALL, 5);
	bSizer31->Add (m_paletteCombo, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

	//bSizer27->Add(bSizer31, 0, wxALL | wxEXPAND, 5);
	sbSizer9->Add (bSizer31, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,5));

	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText37 = new wxStaticText(sbSizer9->GetStaticBox(), wxID_ANY, wxT("Display:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText37->Wrap(-1);
   SetUpToolTip(m_staticText37, IDS_ToolTip42);
	//bSizer32->Add (m_staticText37, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer32->Add (m_staticText37, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM,5));

	m_comboBox3 = new wxComboBox(sbSizer9->GetStaticBox(), IDC_DisplayCombo, wxT("Classes"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_comboBox3->Append(wxT("Classes"));
	m_comboBox3->Append(wxT("Information groups"));
	//m_comboBox3->SetToolTip(wxT("Designate whether classes or information groups (if defined) will be displayed"));
   SetUpToolTip(m_comboBox3, IDS_ToolTip42);
	//bSizer32->Add (m_comboBox3, 0, wxALL | wxEXPAND, 5);
	bSizer32->Add (m_comboBox3, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));


	//bSizer27->Add(bSizer32, 0, wxALL | wxEXPAND, 5);
	sbSizer9->Add (bSizer32, wxSizerFlags(0).Expand().Border(wxRIGHT,5));


	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText23 = new wxStaticText(sbSizer9->GetStaticBox(), wxID_ANY, wxT("Display classes/groups:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText23->Wrap(-1);
	bSizer22->Add (m_staticText23, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxRIGHT|wxBOTTOM,5));

	m_comboBox1 = new wxComboBox(sbSizer9->GetStaticBox(), IDC_ClassesGroupsCombo, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_comboBox1->Append(wxT("All"));
	m_comboBox1->Append(wxT("Subsets..."));
   SetUpToolTip(m_comboBox1, IDS_ToolTip79);
	m_comboBox1->SetSelection(0);
	bSizer22->Add (m_comboBox1, wxSizerFlags(0).Center().Border(wxTOP|wxBOTTOM,5));

	sbSizer9->Add (bSizer22, wxSizerFlags(0).Expand().Border(wxRIGHT,90));
	

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer(wxHORIZONTAL);

	m_button3 = new wxButton(sbSizer9->GetStaticBox(), IDC_BackgroundColor, wxT("Background color..."), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_button3, IDS_ToolTip43);
	//m_button3->SetToolTip(wxT("Background color to be used in the image window.The default color is white"));
   //bSizer23->Add (m_button3, 0, wxALL, 5);
	bSizer23->Add (m_button3, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM,5));

	m_bitmap1 = new wxStaticBitmap(sbSizer9->GetStaticBox(), IDC_BackgroundColorChip, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);
	//bSizer23->Add (m_bitmap1, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer23->Add (m_bitmap1, wxSizerFlags(0).Center().Border(wxTOP|wxBOTTOM,5));

	//bSizer20->Add (bSizer23, 0, wxEXPAND, 5);
	sbSizer9->Add (bSizer23, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,5));
	

	display_legend = new wxCheckBox(sbSizer9->GetStaticBox(), IDC_DisplayLegend, wxT("Display legend"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(display_legend, IDS_ToolTip44);
	//display_legend->SetToolTip(wxT("Whether the legend should be displayed to the left of the image window."));
   
	//bSizer20->Add (display_legend, 0, wxALL, 5);
	sbSizer9->Add (display_legend, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	wxCheckBox* checkBox17 = new wxCheckBox(sbSizer9->GetStaticBox(), IDC_VectorOverlays, wxT("Include Vector Overlays"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(checkBox17, IDS_ToolTip31);
	//m_checkBox17->SetToolTip(wxT("Request that currently loaded vector sets be overlain onto the resulting image window"));
	sbSizer9->Add(checkBox17, 0, wxALL, 5);

	bSizer21->Add (sbSizer9, wxSizerFlags(0).Expand().Border(wxTOP|wxRIGHT|wxBOTTOM,5));
	
			// Second column
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText32 = new wxStaticText(this, wxID_ANY, wxT("Number of classes:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText32->Wrap(-1);
   SetUpToolTip(m_staticText32, IDS_ToolTip45);
	bSizer29->Add(m_staticText32, 0, wxALL, 5);

	m_staticText33 = new wxStaticText(this, IDC_NumberClasses, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_staticText33, IDS_ToolTip45);
	//m_staticText33->SetToolTip(wxT("Number of classes in the image"));
	m_staticText33->Wrap(-1);
	bSizer29->Add(m_staticText33, 0, wxALL, 5);


	bSizer28->Add(bSizer29, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText34 = new wxStaticText(this, wxID_ANY, wxT("Number of groups:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText34->Wrap(-1);
   SetUpToolTip(m_staticText34, IDS_ToolTip46);
	bSizer30->Add(m_staticText34, 0, wxALL, 5);

	m_staticText35 = new wxStaticText(this, IDC_NumberGroups, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_staticText35, IDS_ToolTip46);
	//m_staticText35->SetToolTip(wxT("Number of groups in the image"));
	m_staticText35->Wrap(-1);
	//bSizer30->Add (m_staticText35, 0, wxALL, 5);
	bSizer30->Add (m_staticText35, wxSizerFlags(0).Border(wxTOP|wxBOTTOM|wxRIGHT,5));

	//bSizer28->Add (bSizer30, 0, wxALL | wxEXPAND, 5);
	bSizer28->Add (bSizer30, wxSizerFlags(0).Expand().Border(wxALL,5));

	//bSizer21->Add (bSizer28, 0, wxALL, 5);
	bSizer21->Add (bSizer28, wxSizerFlags(0).Expand().Border(wxALL,5));


	bSizer18->Add (bSizer21, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));
	/*
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer(wxHORIZONTAL);

	m_button4 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	//m_button4->SetToolTip(wxT("Close the dialog box and cancel the display image request"));
	//bSizer33->Add (m_button4, 0, wxALL, 5);
	bSizer33->Add (m_button4, wxSizerFlags(0).Border(wxTOP|wxRIGHT,6));

	m_button5 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	//m_button5->SetToolTip(wxT("Execute the display request using the requested specifications"));
	//bSizer33->Add (m_button5, 0, wxALL, 5);
	bSizer33->Add (m_button5, wxSizerFlags(0).Expand().Border(wxTOP,6));

	//bSizer18->Add (bSizer33, 0, wxALIGN_RIGHT | wxALL, 15);
	bSizer18->Add (bSizer33, wxSizerFlags(0).Right().Border(wxLEFT|wxBOTTOM|wxRIGHT,12));
	*/
	CreateStandardButtons (bSizer18);

	this->SetSizerAndFit (bSizer18);
	//this->Layout ();
	//this->Centre(wxBOTH);
	
}	// end "CreateControls"



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
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Abdur R. Maud			Date: ??/??/2009
//	Revised By:			Larry L. Biehl			Date: 02/16/2016	

Boolean CMDisplayThematicDlg::DoDialog(
				DisplaySpecsPtr					displaySpecsPtr)

{
	Boolean continueFlag = FALSE;

	SInt16 returnCode;


	// Make sure intialization has been completed.

	if (!m_initializedFlag)
																									return (FALSE);

	m_displaySpecsPtr = displaySpecsPtr;

	returnCode = ShowModal();

	if (returnCode == wxID_OK) 
		{
				// Display area													

		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;

		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		//m_displaySpecsPtr->editedBackgroundColor = m_oldRGB;
		gCurrentSelectedColor = m_oldRGB;

		DisplayThematicDialogOK (m_magnification,
										 displaySpecsPtr,
										 &m_dialogSelectArea,
										 m_localClassGroupsPtr,
										 m_localClassGroupsHandle,
										 m_localDisplayClasses,
										 m_localDisplayGroups,
										 m_localAllSubsetClass,
										 m_localAllSubsetGroup,
										 m_localPaletteUpToDateFlag,
										 m_showLegendFlag,
										 m_includeVectorOverlaysFlag);

				// Get maximum magnification value to use.

		displaySpecsPtr->maxMagnification = CMDisplay::GetMaxZoomValue (displaySpecsPtr);

				// Magnification to use										

		displaySpecsPtr->magnification =
				 MIN(m_magnification, displaySpecsPtr->maxMagnification);
		displaySpecsPtr->magnification =
				 MAX(gMinMagnification, displaySpecsPtr->magnification);

				// Update the combo list in the legend. 

		CMLegendView* legendViewCPtr =  gActiveImageViewCPtr->GetImageLegendViewCPtr ();
		legendViewCPtr->UpdateClassGroupComboList (gClassGroupSelection);

		continueFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

	return (continueFlag);

}	// end "DoDialog"



void CMDisplayThematicDlg::OnBackgroundColor (
				wxCommandEvent& event)

{
	RGBColor newRGB;

	if (SelectColor (3, &m_oldRGB, &newRGB))
		{
		m_oldRGB = newRGB;

		//InvalidateRect(NULL, FALSE);
		Refresh(FALSE, NULL);

		wxColour* wxicolor = new wxColour(*wxWHITE);
		//unsigned redc = (UChar) (m_oldRGB.red >> 8);
		//unsigned greenc = (UChar) (m_oldRGB.green >> 8);
		//unsigned bluec = (UChar) (m_oldRGB.blue >> 8);
		//wxicolor->Set((UChar) (m_oldRGB.red >> 8), (UChar) (m_oldRGB.green >> 8), (UChar) (m_oldRGB.blue >> 8));

		unsigned redc = (UChar)m_oldRGB.red;
		unsigned greenc = (UChar)m_oldRGB.green;
		unsigned bluec = (UChar)m_oldRGB.blue;
		wxicolor->Set((UChar)m_oldRGB.red, (UChar)m_oldRGB.green, (UChar)m_oldRGB.blue);

		m_displaySpecsPtr->backgroundColor = m_oldRGB;

		wxMemoryDC imdc;
		wxBrush cbrush(*wxicolor);
		wxBitmap legrect(16, 16);
		imdc.SelectObject(legrect);
		imdc.SetBrush(cbrush);
		imdc.SetBackground(cbrush);
		//imdc.Clear();
		wxRect rectDraw(0, 0, 16, 16);
		imdc.DrawRectangle(rectDraw);
		m_bitmap1->SetBitmap(legrect);

				// To indicate the color has been changed.
		
		m_localPaletteUpToDateFlag = FALSE;
		
		}	// end "if ( SelectColor ( 3, ..."

}	// end "OnBackgroundColor"



void CMDisplayThematicDlg::OnDropdownPaletteCombo (
				wxCommandEvent& event)

{
	m_paletteSelection = m_paletteCombo->GetSelection ();
	
			// The drop down is forced to be -1 setting with each combo
			// box selection so that the 'user selection' box will be opened if
			// it is the last selection. Otherwise it will not open the dialog window.

	m_paletteCombo->SetSelection (-1);
	
}	// end "OnDropdownPaletteCombo"



void CMDisplayThematicDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	SInt32								numberClasses,
											numberGroups;

	SInt16								classGroupSelection,
											classSelection;

	Boolean								includeVectorOverlaysFlag,
											showLegendFlag,
											useThresholdFileFlag;


	wxDialog::OnInitDialog (event);

	DisplayThematicDialogInitialize (this,
												m_displaySpecsPtr,
												gImageWindowInfoPtr,
												gImageFileInfoPtr,
												&m_dialogSelectArea,
												&m_magnification,
												&classGroupSelection,
												&m_paletteChangedFlag,
												&m_localPaletteUpToDateFlag,
												m_localClassGroupsPtr,
												&m_localDisplayClasses,
												&m_localDisplayGroups,
												&m_localAllSubsetClass,
												&m_localAllSubsetGroup,
												&classSelection,
												&m_oldRGB,
												&showLegendFlag,
												&useThresholdFileFlag,
												&numberClasses,
												&numberGroups,
												&includeVectorOverlaysFlag);

	m_classGroupSelection = classGroupSelection - 1;
	m_classSelection = classSelection - 1;

	m_showLegendFlag = showLegendFlag;
	m_useThresholdFileFlag = useThresholdFileFlag;
	m_includeVectorOverlaysFlag = includeVectorOverlaysFlag;

	m_LineStart = m_displaySpecsPtr->lineStart;
	m_LineEnd = m_displaySpecsPtr->lineEnd;
	m_LineInterval = m_displaySpecsPtr->lineInterval;
	m_ColumnStart = m_displaySpecsPtr->columnStart;
	m_ColumnEnd = m_displaySpecsPtr->columnEnd;
	m_ColumnInterval = m_displaySpecsPtr->columnInterval;

	wxColour* wxicolor = new wxColour (*wxWHITE); //default set as white
	wxMemoryDC imdc;
	wxBrush cbrush (*wxicolor);
	wxBitmap legrect (16, 16);
	imdc.SelectObject(legrect);
	imdc.SetBrush (cbrush);
	imdc.SetBackground (cbrush);
	wxRect rectDraw (0, 0, 16, 16);
	imdc.DrawRectangle (rectDraw);
	m_bitmap1->SetBitmap (legrect);

	m_oldRGB = m_displaySpecsPtr->backgroundColor;
	m_numberClasses = wxString::Format (wxT("%i"),
													(SInt32)m_displaySpecsPtr->numberLevels);
	m_numberGroups =
			wxString::Format (wxT("%i"), (SInt32)gImageFileInfoPtr->numberGroups);
	wxStaticText* num_class = (wxStaticText*)FindWindow (IDC_NumberClasses);
	wxStaticText* num_groups = (wxStaticText*)FindWindow (IDC_NumberGroups);
	num_class->SetLabel (m_numberClasses);
	num_groups->SetLabel (m_numberGroups);

	SetEntireImageButtons (NULL,
									m_LineStart,
									m_LineEnd,
									m_ColumnStart,
									m_ColumnEnd);

			// Update the default palette selection in the combo base on the
			// items actually in the list.

	m_paletteSelection = GetComboListSelection (IDC_PaletteCombo,
																gPaletteSelection);
	
	if (TransferDataToWindow ())
		PositionDialogWindow ();
	
	//TransferDataToWindow ();

	//SetSizerAndFit (bSizer18);
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

	////this->Layout ();
	//this->Fit ();
	
	PositionDialogWindow ();

}	// end "OnInitDialog"



void CMDisplayThematicDlg::OnSelendokChannelComboDropDown (
				wxCommandEvent& event)

{
	wxComboBox* classcombo = (wxComboBox *) FindWindow(IDC_ClassesGroupsCombo);
	classcombo->SetSelection(0);
	m_classSelection = 0;
	m_localPaletteUpToDateFlag = FALSE;

	m_classGroupSelection = ((wxComboBox *) FindWindow(IDC_DisplayCombo))->GetSelection();

	// Adjust for an offset of 1 not 0
	m_classGroupSelection++;

	if (m_classGroupSelection == kClassDisplay)
		m_localAllSubsetClass = m_classSelection + 1;

	else // m_classGroupSelection == kGroupDisplay
		m_localAllSubsetGroup = m_classSelection + 1;
	
}	// end "OnSelendokChannelComboDropDown"



void CMDisplayThematicDlg::OnSelendokClassesGroupsCombo (
				wxCommandEvent& event)

{
	Boolean returnFlag = FALSE;


			// Determine whether all or subset of classes/groups are
			// was selected.

	m_classSelection = ((wxComboBox *) FindWindow(IDC_ClassesGroupsCombo))->GetSelection();

			// Determine whether classes or groups are being used.

	m_classGroupSelection = ((wxComboBox *) FindWindow(IDC_DisplayCombo))->GetSelection();

			// Adjust for an offset of 1 not 0
	
	m_classGroupSelection++;

	if (m_classSelection == kSubsetMenuItem)
		{
				// Subset of classes or groups to be used.

		SetDLogControlHilite(NULL, wxID_OK, 255);

		CMClassGroupDlg* classesGroupsDialogPtr = NULL;

		classesGroupsDialogPtr = new CMClassGroupDlg(this);

		if (m_classGroupSelection == kClassDisplay)
			{
			returnFlag = classesGroupsDialogPtr->DoDialog (
												  &m_localDisplayClasses,
												  m_localClassGroupsPtr,
												  1,
												  (SInt16) gImageFileInfoPtr->numberClasses,
												  kClassDisplay);

			if (m_localDisplayClasses == gImageFileInfoPtr->numberClasses)
				{
				m_classSelection = kAllMenuItem;
				m_classSelection = ((wxComboBox*)FindWindow (IDC_ClassesGroupsCombo))->GetSelection ();

				}	// end "if (m_localDisplayClasses == ..."

			}	// end "if (m_classGroupSelection == kClassDisplay)"

		if (m_classGroupSelection == kGroupDisplay)
			{
			returnFlag = classesGroupsDialogPtr->DoDialog (
													&m_localDisplayGroups,
													&m_localClassGroupsPtr[gImageFileInfoPtr->numberClasses],
													1,
													m_localNumberGroups,
													kGroupDisplay);

			if (m_localDisplayGroups == (UInt16) m_localNumberGroups)
				{
				m_classSelection = kAllMenuItem;
				m_classSelection = ((wxComboBox *) FindWindow(IDC_ClassesGroupsCombo))->GetSelection();

				}	// end "if (m_localDisplayGroups == ..."

			}	// end "if (m_classGroupSelection == kGroupDisplay)"

		if (classesGroupsDialogPtr != NULL)
			delete classesGroupsDialogPtr;
		
		}	// end "if (m_classSelection == kSubsetMenuItem)"

	SetDLogControlHilite (NULL, wxID_OK, 0);

	if (returnFlag)
		m_localPaletteUpToDateFlag = FALSE;

	if (m_classGroupSelection == kClassDisplay)
		m_localAllSubsetClass = m_classSelection + 1;

	else // m_classGroupSelection == kGroupDisplay
		m_localAllSubsetGroup = m_classSelection + 1;

}	// end "OnSelendokClassesGroupsCombo"



void CMDisplayThematicDlg::OnSelendokPaletteCombo (
				wxCommandEvent& event)

{
	int				lastPaletteSelection;


	lastPaletteSelection = m_paletteSelection;

	m_paletteSelection = m_paletteCombo->GetSelection ();

   SInt64 palette64 =
				(SInt64)((int*)m_paletteCombo->GetClientData (m_paletteSelection));
   gPaletteSelection = (SInt16)palette64;

	if (gPaletteSelection == kFalseColors)
		{
		if (FalseColorPaletteDialog ())
			m_localPaletteUpToDateFlag = FALSE;

		else	// !FalseColorPaletteDialog()
			{
			if (lastPaletteSelection != m_paletteSelection)
				{
				m_paletteSelection = lastPaletteSelection;
				m_paletteCombo->SetSelection (m_paletteSelection);
				
				}	// end "if (lastPaletteSelection != m_paletteSelection)"

			//m_paletteSelection = lastPaletteSelection;

			}	// end "else !FalseColorPaletteDialog()"

		}	// end "if (gPaletteSelection == kFalseColors)"

	if (!m_localPaletteUpToDateFlag || lastPaletteSelection != m_paletteSelection)
		m_paletteChangedFlag = TRUE;

}	// end "OnSelendokPaletteCombo"



bool CMDisplayThematicDlg::TransferDataFromWindow ()

{
	SInt16		returnCode;
	
	
	wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
	wxTextCtrl* magnify = (wxTextCtrl*)FindWindow (IDC_Magnification);
	wxComboBox* display = (wxComboBox*)FindWindow (IDC_DisplayCombo);
	wxComboBox* classgroup = (wxComboBox *) FindWindow (IDC_ClassesGroupsCombo);
	wxStaticBitmap* color = (wxStaticBitmap *) FindWindow (IDC_BackgroundColorChip);
	wxCheckBox* legend = (wxCheckBox *) FindWindow (IDC_DisplayLegend);
	wxCheckBox* voverlays = (wxCheckBox*) FindWindow (IDC_VectorOverlays);
	
	m_LineStartString = l_start->GetValue();
	m_LineEndString = l_end->GetValue();
	m_LineIntervalString = l_inter->GetValue();
	m_ColumnStartString = c_start->GetValue();
	m_ColumnEndString = c_end->GetValue();
	m_ColumnIntervalString = c_inter->GetValue();

	m_LineStart = wxAtoi(m_LineStartString);
	m_LineEnd = wxAtoi(m_LineEndString);
	m_LineInterval = wxAtoi(m_LineIntervalString);
	m_ColumnStart = wxAtoi(m_ColumnStartString);
	m_ColumnEnd = wxAtoi(m_ColumnEndString);
	m_ColumnInterval = wxAtoi(m_ColumnIntervalString);
	wxString magnstring = magnify->GetValue();
	magnstring.ToDouble(&m_magnification);

	m_paletteSelection = m_paletteCombo->GetSelection();
	m_paletteSelection = m_paletteSelection + 1;

	m_classGroupSelection = display->GetSelection();
	m_classSelection = classgroup->GetSelection();

	m_showLegendFlag = legend->GetValue();
	m_oldRGB = m_displaySpecsPtr->backgroundColor;
	
	m_includeVectorOverlaysFlag = voverlays->GetValue();

			// Adjust Class/Group selection to be 1 based.
	//if(gClassGroupSelection == 0)                 //Add on 3/29 to display subsets channel
	gClassGroupSelection = m_classGroupSelection + 1;

			// Adjust All or Subset of Classes/Groups selection to be 1 based.

	gClassSelection = m_classSelection + 1;

	returnCode = VerifyLineColumnValues(
					  IDC_LineStart,
					  IDC_ColumnStart,
					  true);
					  
	if (returnCode == 0)
		{
				// Verify that the number of lines to be displayed is less than
				// the maximum possible.

		if (CheckNumberDisplayLines (m_LineStart,
												m_LineEnd,
												m_LineInterval,
												this,
												IDC_LineStart) == 0)
			returnCode = IDC_LineEnd;
			
		}		// end "if (returnCode == 0)"

	if (returnCode == 0)
		{
				// Verify that the number of columns to be displayed is less than
				// the maximum possible.
				// This part needs to be updated.

		if (CheckNumberDisplayColumns (m_ColumnStart,
													m_ColumnEnd,
													m_ColumnInterval,
													24,
													2,
													1,
													this,
													IDC_ColumnStart) == 0)
			returnCode = IDC_ColumnEnd;
			
		}	// end "if (returnCode == 0)"
	
	return (returnCode == 0);
	 
}	// end "TransferDataFromWindow"



bool CMDisplayThematicDlg::TransferDataToWindow ()
	{
	wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
	wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
	wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
	wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
	wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
	wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
	wxTextCtrl* magnify = (wxTextCtrl*) FindWindow(IDC_Magnification);
	wxComboBox* display = (wxComboBox *) FindWindow(IDC_DisplayCombo);
	wxComboBox* classgroup = (wxComboBox *) FindWindow(IDC_ClassesGroupsCombo);
	// wxStaticBitmap* colorchip = (wxStaticBitmap *) FindWindow(IDC_BackgroundColorChip);
	wxCheckBox* legend = (wxCheckBox *) FindWindow(IDC_DisplayLegend);
	wxCheckBox* voverlays = (wxCheckBox*) FindWindow(IDC_VectorOverlays);
	wxStaticText* file_name = (wxStaticText*) FindWindow(IDC_FileName);
	wxStaticText* num_class = (wxStaticText*) FindWindow(IDC_NumberClasses);
	wxStaticText* num_group = (wxStaticText*) FindWindow(IDC_NumberGroups);

	c_end->ChangeValue(wxString::Format(wxT("%i"), m_ColumnEnd));
	c_inter->ChangeValue(wxString::Format(wxT("%i"), m_ColumnInterval));
	c_start->ChangeValue(wxString::Format(wxT("%i"), m_ColumnStart));
	l_end->ChangeValue(wxString::Format(wxT("%i"), m_LineEnd));
	l_inter->ChangeValue(wxString::Format(wxT("%i"), m_LineInterval));
	l_start->ChangeValue(wxString::Format(wxT("%i"), m_LineStart));
	magnify->ChangeValue(wxString::Format(wxT("%.3lf"), m_magnification));
	voverlays->SetValue(m_includeVectorOverlaysFlag);

	m_paletteCombo->SetSelection (m_paletteSelection);
	//display->SetSelection(m_classSelection);
	//classgroup->SetSelection(m_classGroupSelection);
	//color->SetBitmap(wxRect(0,0,10,10));
	wxColour* wxicolor = new wxColour(*wxWHITE);
	//wxicolor->Set((UChar) (m_oldRGB.red >> 8), (UChar) (m_oldRGB.green >> 8), (UChar) (m_oldRGB.blue >> 8));
	wxicolor->Set((UChar)m_oldRGB.red, (UChar)m_oldRGB.green, (UChar)m_oldRGB.blue);
	wxMemoryDC imdc;
	wxBrush cbrush(*wxicolor);
	wxBitmap legrect(16, 16);
	imdc.SelectObject(legrect);
	imdc.SetBrush(cbrush);
	imdc.SetBackground(cbrush);
	wxRect rectDraw(0, 0, 16, 16);
	imdc.DrawRectangle(rectDraw);
	m_bitmap1->SetBitmap(legrect);

	display->SetSelection(m_classGroupSelection);
	classgroup->SetSelection(m_classSelection);

	legend->SetValue(m_showLegendFlag);
	file_name->SetLabel(m_fileName);
	num_class->SetLabel(m_numberClasses);
	num_group->SetLabel(m_numberGroups);
	
	return true;

}	// end "TransferDataToWindow"
