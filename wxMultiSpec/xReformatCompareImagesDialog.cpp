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
//   File:                 xReformatCompareImagesDialog.cpp : class implementation file
//   Class Definition:     xReformatCompareImagesDialog.h
//
//   Authors:              Larry L. Biehl
//
//   Revision date:        02/14/2025
//
//   Language:					C++
//
//   System:               Linux and MacOS Operating Systems
//
//   Brief description:  	This file contains functions that relate to the
//                       	CMReformatRectifyDlg class.
//
/*
  Template for debug code.
 	int numberChars = snprintf ((char*)&gTextString3,
									256,
 			" LReformatRectifyDlg: (): %s",
 			gEndOfLine);
 ListString ((char*)&gTextString3, numberChars, gOutputTextH);
 */
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "xReformatCompareImagesDialog.h"
#include "wx/gbsizer.h"
#include "xChannelsDialog.h"
#include "xImageView.h"
#include "SImageWindow_class.h"
#include "xDialog_images.cpp"



BEGIN_EVENT_TABLE (CMReformatCompareImagesDlg, CMDialog)
	EVT_BUTTON (IDEntireImage, CMReformatCompareImagesDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMReformatCompareImagesDlg::ToSelectedImage)

	EVT_CHECKBOX (IDC_CreateOutputImageFile, CMReformatCompareImagesDlg::OnBnClickedCreateOutputImageFile)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_Channels, CMReformatCompareImagesDlg::OnSelendokOutChannels)
	#endif
	#if defined multispec_wxmac || defined multispec_wxwin
		EVT_CHOICE (IDC_Channels, CMReformatCompareImagesDlg::OnSelendokOutChannels)
	#endif

	EVT_CHOICE (IDC_ReferenceFileList, CMReformatCompareImagesDlg::OnCbnSelendokTargetCombo)
	EVT_CHOICE (IDC_CompareAlgorithm, CMReformatCompareImagesDlg::OnCbnSelendokCompareAlgorithmMethod)

	EVT_COMBOBOX_DROPDOWN (IDC_Channels, CMReformatCompareImagesDlg::OnChannelComboDropDown)

	EVT_INIT_DIALOG (CMReformatCompareImagesDlg::OnInitDialog)

	EVT_TEXT (IDC_ColumnStart, CMReformatCompareImagesDlg::CheckColumnStart)
	EVT_TEXT (IDC_ColumnEnd, CMReformatCompareImagesDlg::CheckColumnEnd)
	EVT_TEXT (IDC_LineStart, CMReformatCompareImagesDlg::CheckLineStart)
	EVT_TEXT (IDC_LineEnd, CMReformatCompareImagesDlg::CheckLineEnd)
END_EVENT_TABLE ()



CMReformatCompareImagesDlg::CMReformatCompareImagesDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title/*=NULL*/)
		: CMDialog (CMReformatCompareImagesDlg::IDD, pParent, title)

{
	wxString 								fileName;
	FileStringPtr 							fileNamePtr;
	
   m_kNoneMenuItem = kNoneMenuItem;
   m_kArcViewMenuItem = kArcViewMenuItem;
   m_kTIFFGeoTIFFMenuItem = kTIFFGeoTIFFMenuItem;
   
   m_channelSelection = -1;
   m_fileNamesSelection = -1;
   m_headerListSelection = -1;
   m_headerOptionsSelection = -1;
   m_compareAlgorithmCode = 0;
   m_createOutputImageFileFlag = FALSE;
   
   m_compareImageWindowInfoHandle = NULL;
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
	if (m_initializedFlag)
		{
				//	Set the image file name.

		fileNamePtr = (FileStringPtr)(gActiveImageViewCPtr->GetImageWindowCPtr())->
																				GetImageFileNamePointer ();
		if (fileNamePtr != NULL)
			{
			fileName = wxString (fileNamePtr, wxConvUTF8);
			wxStaticText * fname = (wxStaticText*)FindWindow (IDC_FileName);
			fname->SetLabel (fileName);
			
			}	// end "if (fileNamePtr != NULL)"
		
		else	// fileNamePtr == NULL
			m_initializedFlag = FALSE;

		}	// end "if (m_initializedFlag)"
		
   SetSizerAndFit (bSizer119);
	
}	// end "CMReformatCompareImagesDlg"



CMReformatCompareImagesDlg::~CMReformatCompareImagesDlg ()

{
   
}	// end "~CMReformatCompareImagesDlg"



void CMReformatCompareImagesDlg::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);
   
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);
   
   bSizer119 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer67 = new wxBoxSizer (wxHORIZONTAL);
   
   wxStaticText* staticText1167 = new wxStaticText (this,
																		IDC_Prompt,
																		wxT("Active:"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		0);
	bSizer67->Add (staticText1167, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT, 5));

	wxStaticText* m_staticimagename = new wxStaticText (this,
																		IDC_FileName,
																		wxT("image name"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		0);
	m_staticimagename->Wrap (-1);
	bSizer67->Add (m_staticimagename, wxSizerFlags(0).Border(wxTOP|wxRIGHT, 5));
	
	bSizer119->Add (bSizer67, wxSizerFlags(0).Border(wxLEFT|wxTOP, 10));
	//bSizer119->Add (20,6);
   
   wxFlexGridSizer* gSizer16;
   gSizer16 = new wxFlexGridSizer (0, 2, 0, 0);
   
   wxBoxSizer* bSizer195;
   bSizer195 = new wxBoxSizer (wxVERTICAL);
   
   wxBoxSizer* bSizer198;
   bSizer198 = new wxBoxSizer (wxVERTICAL);
   
   wxStaticBoxSizer* inputImageSettingsSizer;
   wxStaticBox* inputImageSettingsBox = new wxStaticBox (this,
   																		IDC_LineColFrame,
   																		wxT("Input Image Settings"));
   inputImageSettingsSizer = new wxStaticBoxSizer (
																inputImageSettingsBox, wxVERTICAL);
   
   wxBoxSizer* bSizer271;
   bSizer271 = new wxBoxSizer (wxHORIZONTAL);
   
   
   bSizer271->Add (20, 0, 0, wxEXPAND);
   
   m_staticText381 = new wxStaticText (inputImageSettingsBox,
   												IDC_ReferenceFileListPrompt,
   												wxT("Compare with:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText381->Wrap (-1);
   SetUpToolTip (m_staticText381, IDS_ToolTip367);
   bSizer271->Add (m_staticText381, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
   m_referenceListCtrl = new wxChoice (inputImageSettingsBox,
													IDC_ReferenceFileList,
													wxDefaultPosition,
													wxSize (300, -1));
   SetUpToolTip (m_referenceListCtrl, IDS_ToolTip367);
   m_referenceListCtrl->SetClientSize (wxSize (260, -1));
   
   bSizer271->Add (m_referenceListCtrl, 0, wxALL, 5);
   
   inputImageSettingsSizer->Add (bSizer271, 0, wxEXPAND);
																
	wxBoxSizer* bSizer199;
	bSizer199 = new wxBoxSizer (wxHORIZONTAL);
		
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);
   
   m_bpButton4 = new wxBitmapButton (inputImageSettingsBox,
   												IDEntireImage,
   												entireimi,
   												wxDefaultPosition,
   												wxDefaultSize,
   												wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled (toentirei);
  	bSizer791->Add (m_bpButton4,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_bpButton5 = new wxBitmapButton (inputImageSettingsBox,
   											IDSelectedImage,
   											selectedi,
   											wxDefaultPosition,
   											wxDefaultSize,
   											wxBU_AUTODRAW);
   m_bpButton5->SetBitmapDisabled (bmp4i);
   bSizer791->Add (m_bpButton5,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   bSizer199->Add (bSizer791, 0, wxALL|wxEXPAND, 5);
   m_bpButton4->Hide ();
   
   wxGridSizer* gSizer1;
   gSizer1 = new wxGridSizer (3, 3, 0, 0);
   
   wxStaticText* staticText63 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxEmptyString,
   																wxDefaultPosition,
   																wxDefaultSize,
   																0);
   staticText63->Wrap (-1);
   gSizer1->Add (staticText63, 0, wxALL, 5);
   
   wxStaticText* staticText64 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("Start"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText64->Wrap (-1);
   gSizer1->Add (staticText64, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText65 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("End"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText65->Wrap (-1);
   gSizer1->Add (staticText65, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText60 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("Lines"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText60->Wrap (-1);
   gSizer1->Add (staticText60, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* linestartctrl = new wxTextCtrl (inputImageSettingsBox,
   															IDC_LineStart,
   															wxEmptyString,
   															wxDefaultPosition,
   															wxSize (70, -1),
   															0);
   linestartctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_LineStartString));
   gSizer1->Add (linestartctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* lineendctrl = new wxTextCtrl (inputImageSettingsBox,
   														IDC_LineEnd,
   														wxEmptyString,
   														wxDefaultPosition,
   														wxSize (70, -1),
   														0);
   lineendctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_LineEndString));
   gSizer1->Add (lineendctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText62 = new wxStaticText (inputImageSettingsBox,
   																wxID_ANY,
   																wxT("Columns"),
   																wxDefaultPosition,
   																wxSize (70, -1),
   																0);
   staticText62->Wrap (-1);
   gSizer1->Add (staticText62, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* colstartctrl = new wxTextCtrl (inputImageSettingsBox,
   															IDC_ColumnStart,
   															wxEmptyString,
   															wxDefaultPosition,
   															wxSize (70, -1),
   															0);
   colstartctrl->SetValidator (
   								wxTextValidator (wxFILTER_DIGITS, &m_ColumnStartString));
   gSizer1->Add (colstartctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* colendctrl = new wxTextCtrl (inputImageSettingsBox,
   														IDC_ColumnEnd,
   														wxEmptyString,
   														wxDefaultPosition,
   														wxSize (70, -1),
   														0);
   colendctrl->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_ColumnEndString));
   
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   SetUpToolTip (linestartctrl, IDS_ToolTip19);
   SetUpToolTip (lineendctrl, IDS_ToolTip20);
   SetUpToolTip (colstartctrl, IDS_ToolTip22);
   SetUpToolTip (colendctrl, IDS_ToolTip23);
   
   gSizer1->Add (colendctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   bSizer199->Add (gSizer1, 1, wxEXPAND|wxALL, 5);
   
   inputImageSettingsSizer->Add (bSizer199, 1, wxEXPAND|wxALL, 5);
   
   wxBoxSizer* bSizer281;
   bSizer281 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText404 = new wxStaticText (inputImageSettingsBox,
   												IDC_ChannelsPrompt,
   												wxT("Channels:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText404->Wrap (-1);
   SetUpToolTip (m_staticText404, IDS_ToolTip366);
   bSizer281->Add (m_staticText404, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	GetAllSubsetMenuControl (kChannelsMenu,
										inputImageSettingsBox,
										IDC_Channels,
										120,
										IDS_ToolTip366);
	bSizer281->Add (m_channelsCtrl, 0, wxALL, 5);
   
   inputImageSettingsSizer->Add (bSizer281, 0, wxEXPAND);
   
   bSizer198->Add (inputImageSettingsSizer, 1, wxEXPAND | wxBOTTOM|wxRIGHT, 6);
   
   bSizer195->Add (bSizer198, 1, wxEXPAND);
	
   gSizer16->Add (bSizer195, 1, wxEXPAND);
   
   wxBoxSizer* bSizer197;
   bSizer197 = new wxBoxSizer (wxVERTICAL);
   
   //wxBoxSizer* bSizer200;
   //bSizer200 = new wxBoxSizer (wxVERTICAL);
   
   wxStaticBoxSizer* outputOptionsSizer;
   wxStaticBox* outputOptionsBox = new wxStaticBox (this,
   																	wxID_ANY,
   																	wxT("Output Image Options"));
   outputOptionsSizer = new wxStaticBoxSizer (outputOptionsBox, wxVERTICAL);
   
   m_checkBox18 = new wxCheckBox (outputOptionsBox,
   											IDC_CreateOutputImageFile,
   											wxT("Create output image file"),
   											wxDefaultPosition,
   											wxDefaultSize,
   											0);
   SetUpToolTip (m_checkBox18, IDS_ToolTip370);
   //outputOptionsSizer->Add (m_checkBox18, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5);
	outputOptionsSizer->Add (m_checkBox18,
									wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
   
   wxBoxSizer* bSizer278;
   bSizer278 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText402 = new wxStaticText (outputOptionsBox,
													IDC_HeaderPrompt,
													wxT("Header:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText402->Wrap (-1);
   SetUpToolTip (m_staticText402, IDS_ToolTip203);
   //bSizer278->Add (m_staticText402, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	bSizer278->Add (m_staticText402,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_fileFormatCtrl = new wxChoice (outputOptionsBox,
   											IDC_Header,
   											wxDefaultPosition,
   											wxDefaultSize);
   m_fileFormatCtrl->Append ("'None'");
   m_fileFormatCtrl->Append ("ArcView format");
   SetUpToolTip (m_fileFormatCtrl, IDS_ToolTip203);
   //bSizer278->Add (m_fileFormatCtrl, 0, wxALL, 5);
	bSizer278->Add (m_fileFormatCtrl,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
   
   //outputOptionsSizer->Add (bSizer278, 0, wxALL|wxEXPAND, 5);
	outputOptionsSizer->Add (bSizer278,
										wxSizerFlags(0).Expand().Border(wxALL, 5));
   
   wxBoxSizer* bSizer272;
   bSizer272 = new wxBoxSizer (wxHORIZONTAL);
   
   //bSizer272->Add (20, 0, 0, wxEXPAND);
   
   m_staticText382 = new wxStaticText (outputOptionsBox,
   												IDC_CompareAlgorithmPrompt,
   												wxT("Algorithm:"),
   												wxDefaultPosition,
   												wxDefaultSize,
   												0);
   m_staticText382->Wrap (-1);
   SetUpToolTip (m_staticText382, IDS_ToolTip368);
   //bSizer272->Add (m_staticText382, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	bSizer272->Add (m_staticText382,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
	
   m_algorithmCtrl = new wxChoice (outputOptionsBox,
   											IDC_CompareAlgorithm,
   											wxDefaultPosition,
   											wxDefaultSize);
   SetUpToolTip (m_algorithmCtrl, IDS_ToolTip368);
   m_algorithmCtrl->SetMinSize (wxSize (150, -1));
   //bSizer272->Add (m_algorithmCtrl, 0, wxALL, 5);
	bSizer272->Add (m_algorithmCtrl,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));
   
   outputOptionsSizer->Add (bSizer272, 0, wxALL|wxEXPAND, 5);
   
   bSizer197->Add (outputOptionsSizer, 1, wxEXPAND | wxBOTTOM|wxRIGHT, 6);
   
   gSizer16->Add (bSizer197, 1, wxEXPAND);
	
   bSizer119->Add (gSizer16, 1, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 12);

	CreateStandardButtons (bSizer119);
   
   Layout ();
   
   		// This forces the positions to be used when the toEntire and toSelected
   		// buttons are switched.
	
   m_bpButton4->Show ();
   m_bpButton5->Hide ();
   Layout ();
   
} 	// end "CreateControls"



Boolean CMReformatCompareImagesDlg::DoDialog (
				FileInfoPtr                   outFileInfoPtr,
				FileInfoPtr                   fileInfoPtr,
				WindowInfoPtr                 imageWindowInfoPtr,
				LayerInfoPtr                  imageLayerInfoPtr,
				ReformatOptionsPtr            reformatOptionsPtr)

{
   SInt16                			returnCode;
   Boolean                  		continueFlag = FALSE;
	
   
   		// Make sure intialization has been complete
   
   if (!m_initializedFlag)
      																					return (FALSE);
   
   m_outputFileInfoPtr = outFileInfoPtr;
   m_fileInfoPtr = fileInfoPtr;
   m_imageWindowInfoPtr = imageWindowInfoPtr;
   m_imageLayerInfoPtr = imageLayerInfoPtr;
   m_reformatOptionsPtr = reformatOptionsPtr;
   m_compareImagesOptionsPtr = reformatOptionsPtr->compareImagesOptionsPtr;
   
   		// Selected area for output file.
	
   m_dialogSelectArea.lineStart = m_LineStart;
   m_dialogSelectArea.lineEnd = m_LineEnd;
   m_dialogSelectArea.lineInterval = m_LineInterval;
   
   m_dialogSelectArea.columnStart = m_ColumnStart;
   m_dialogSelectArea.columnEnd = m_ColumnEnd;
   m_dialogSelectArea.columnInterval = m_ColumnInterval;
	
   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
   	{
      continueFlag = TRUE;
      
      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = 1;
      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = 1;
      
      CompareImagesDialogOK (this,
                            m_outputFileInfoPtr,
                            m_fileInfoPtr,
                            m_imageWindowInfoPtr,
                            m_imageLayerInfoPtr,
                            &m_dialogSelectArea,
                            m_reformatOptionsPtr,
                            (SInt16)m_headerOptionsSelection,
                            (SInt16)m_channelSelection,
                            (Boolean)m_createOutputImageFileFlag,
                            (SInt16)m_compareAlgorithmCode,
                            m_compareImageWindowInfoHandle);
      
   	} 	// end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}	// end "DoDialog"



void CMReformatCompareImagesDlg::OnBnClickedCreateOutputImageFile (
				wxCommandEvent& 					event)

{
   wxCheckBox* createOutputImageFile =
   									(wxCheckBox*)FindWindow (IDC_CreateOutputImageFile);
   m_createOutputImageFileFlag = createOutputImageFile->GetValue ();
   
   CompareImagesDialogOnCreateImageFile (this, m_createOutputImageFileFlag);
         
} 	// end "OnBnClickedCreateOutputImageFile"



void CMReformatCompareImagesDlg::OnCbnSelendokTargetCombo (
				wxCommandEvent& 					event)

{
	SInt16   							savedFileNamesSelection;
   
   
   		// Popup box to select the reference image to register against
   
   if (m_fileNamesSelection >= 0)
   	{
      savedFileNamesSelection = m_fileNamesSelection;
      
      m_fileNamesSelection = m_referenceListCtrl->GetSelection ();
      
      if (savedFileNamesSelection != m_fileNamesSelection)
         CompareImagesDialogOnReferenceFile (this,
															  m_compareAlgorithmCode,
															  m_fileNamesSelection+1,
															  &m_compareImageWindowInfoHandle,
															  &m_dialogSelectArea);
			
   	}	// end "if (m_fileNamesSelection >= 0)"
   
} 	// end "OnCbnSelendokTargetCombo"



void CMReformatCompareImagesDlg::OnCbnSelendokCompareAlgorithmMethod (
				wxCommandEvent& 					event)

{
   		// Select resampling method popup box
   
   if (m_compareAlgorithmSelection >= 0)
   	{
      m_compareAlgorithmSelection = m_algorithmCtrl->GetSelection ();
      
      SInt64 windowIndex64 =
					(SInt64)((int*)m_algorithmCtrl->GetClientData (m_compareAlgorithmSelection));
      m_compareAlgorithmCode = (UInt32)windowIndex64;
      
   	}	// end "if (m_compareAlgorithmSelection >= 0)"
   
}	// end "OnCbnSelendokCompareAlgorithmMethod"



void CMReformatCompareImagesDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
	SInt16         					channelSelection,
											fileNamesSelection,
											compareAlgorithmCode,
											headerOptionsSelection;
   
   Boolean         					createOutputImageFileFlag,
											mapInfoExistsFlag;
   
	
   CompareImagesDialogInitialize (this,
                                 m_fileInfoPtr,
                                 &m_dialogSelectArea,
                                 m_reformatOptionsPtr,
                                 &headerOptionsSelection,
                                 &channelSelection,
                                 &createOutputImageFileFlag,
                                 &compareAlgorithmCode,
                                 &fileNamesSelection,
                                 &m_compareImageWindowInfoHandle);
   
   m_LineStart = m_reformatOptionsPtr->lineStart;
   m_LineEnd = m_reformatOptionsPtr->lineEnd;
   m_ColumnStart = m_reformatOptionsPtr->columnStart;
   m_ColumnEnd = m_reformatOptionsPtr->columnEnd;
   
   m_compareAlgorithmCode = compareAlgorithmCode;
   
   		// Get the compare algorithm to be used.
   		
	//m_compareAlgorithmSelection = m_compareAlgorithmCode;
   
   m_compareAlgorithmSelection = GetComboListSelection (IDC_CompareAlgorithm,
																		m_compareAlgorithmCode);
   
   if (m_compareAlgorithmSelection == -1)
      m_compareAlgorithmSelection = 0;
	
   if (fileNamesSelection > 0)
   	m_fileNamesSelection = fileNamesSelection - 1;
   	
	m_createOutputImageFileFlag = createOutputImageFileFlag;
   
   		// Set text indicating whether the output file format could be
   		// GeoTIFF or TIFF.
	
   mapInfoExistsFlag = FindIfMapInformationExists (gImageWindowInfoPtr);
   
   if (mapInfoExistsFlag)
      m_fileFormatCtrl->Append (wxT ("GeoTIFF format"));
   else
      m_fileFormatCtrl->Append (wxT("TIFF format"));
   
   m_fileFormatCtrl->SetClientData (0, (void*)(SInt64)m_kNoneMenuItem);
   m_fileFormatCtrl->SetClientData (1, (void*)(SInt64)m_kArcViewMenuItem);
   m_fileFormatCtrl->SetClientData (2, (void*)(SInt64)m_kTIFFGeoTIFFMenuItem);
   
   m_headerOptionsSelection = headerOptionsSelection;
   m_headerListSelection = GetComboListSelection (IDC_Header,
                                                  m_headerOptionsSelection);
   
   m_localActiveFeaturesPtr = (UInt16*)m_reformatOptionsPtr->channelPtr;
   m_channelSelection = channelSelection;
   
   CompareImagesDialogOnCreateImageFile (this, m_createOutputImageFileFlag);
   
  	if (TransferDataToWindow ())
      PositionDialogWindow ();
   
   		// Set default text selection to first edit text item
	
   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
   
}	// end "OnInitDialog"



void CMReformatCompareImagesDlg::OnSelendokOutChannels (
				wxCommandEvent& 					event)

{
   m_localActiveNumberFeatures = m_reformatOptionsPtr->numberChannels;
   
   HandleChannelsMenu (IDC_Channels,
                       kNoTransformation,
                       (SInt16)gImageWindowInfoPtr->totalNumberChannels,
                       kImageChannelType,
                       TRUE);
   
   m_reformatOptionsPtr->numberChannels = m_localActiveNumberFeatures;
   
}	// end "OnSelendokOutChannels"



void CMReformatCompareImagesDlg::UpdateProcedureItems (
				int   								selectItemNumber,
				Boolean  							blankOutsideSelectedAreaFlag)

{   
   CompareImagesDialogOnReferenceFile (this,
                                      m_compareAlgorithmCode,
                                      m_fileNamesSelection+1,
                                      &m_compareImageWindowInfoHandle,
                                      &m_dialogSelectArea);
   
   		// Set default text selection to first edit text item
   
   SelectDialogItemText (this, selectItemNumber, 0, SInt16_MAX);
   
}	// end "UpdateProcedureItems"



bool CMReformatCompareImagesDlg::TransferDataFromWindow ()

{
   SInt16      						returnCode = 0;
	
	
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   m_LineStartString = l_start->GetValue ();
   m_LineStart = wxAtoi (m_LineStartString);
	
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   m_LineEndString = l_end->GetValue ();
   m_LineEnd = wxAtoi (m_LineEndString);
	
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   m_ColumnStartString = c_start->GetValue ();
   m_ColumnStart = wxAtoi (m_ColumnStartString);
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   m_ColumnEndString = c_end->GetValue ();
   m_ColumnEnd = wxAtoi (m_ColumnEndString);
   
   m_channelSelection = m_channelsCtrl->GetSelection ();
   
   m_headerListSelection = m_fileFormatCtrl->GetSelection ();
   SInt64 headerList =
				(SInt64)((int*)m_fileFormatCtrl->GetClientData (m_headerListSelection));
   m_headerOptionsSelection = (SInt16)headerList;
	
   m_compareAlgorithmSelection = m_algorithmCtrl->GetSelection ();
   
   if (returnCode == 0)
      returnCode = VerifyLineColumnValues (IDC_LineStart,
                                          	IDC_ColumnStart,
                                          	false);
   
   return (returnCode == 0);
   
}	// end "TransferDataFromWindow"



bool CMReformatCompareImagesDlg::TransferDataToWindow ()

{
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   l_start->ChangeValue (wxString::Format (wxT("%d"), m_LineStart));
	
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   l_end->ChangeValue (wxString::Format (wxT("%d"), m_LineEnd));
	
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   c_start->ChangeValue (wxString::Format (wxT("%d"), m_ColumnStart));
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   c_end->ChangeValue (wxString::Format (wxT("%d"), m_ColumnEnd));
	
   m_channelsCtrl->SetSelection (m_channelSelection);
	
   wxCheckBox* createOutputImageFile =
   										(wxCheckBox*)FindWindow (IDC_CreateOutputImageFile);
	createOutputImageFile->SetValue (m_createOutputImageFileFlag);
	
   m_fileFormatCtrl->SetSelection (m_headerListSelection);
	
   m_referenceListCtrl->SetSelection (m_fileNamesSelection);
	
   m_algorithmCtrl->SetSelection (m_compareAlgorithmSelection);
   
   return true;
   
}	// end "TransferDataToWindow"

