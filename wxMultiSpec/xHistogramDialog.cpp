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
//	File:						xHistogramDialog.cpp : class implementation file
//	Class Definition:		xHistogramDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/02/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the 
//								CMHistogramSpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "SHistogram_class.h"
#include "SImageWindow_class.h"

#include "xHistogramDialog.h"
#include "xImageView.h"

typedef wxString CString;



IMPLEMENT_DYNAMIC_CLASS (CMHistogramSpecsDlg, CMDialog)

BEGIN_EVENT_TABLE (CMHistogramSpecsDlg, CMDialog)
	EVT_BUTTON (IDEntireImage, CMHistogramSpecsDlg::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMHistogramSpecsDlg::ToSelectedImage)

	EVT_CHECKBOX (IDC_DiskFile, CMHistogramSpecsDlg::OnDiskFile)
	EVT_CHECKBOX (IDC_EmptyBins, CMHistogramSpecsDlg::OnEmptyBins)
	EVT_CHECKBOX (IDC_ComputeOnly, CMHistogramSpecsDlg::OnComputeOnly)
	EVT_CHECKBOX (IDC_ListHistogram, CMHistogramSpecsDlg::OnListHistogram)
	EVT_CHECKBOX (IDC_TextWindow, CMHistogramSpecsDlg::OnTextWindow)

	#if defined multispec_wxlin
		EVT_COMBOBOX (IDC_ChannelCombo, CMHistogramSpecsDlg::OnSelendokChannels)
		EVT_COMBOBOX (IDC_Method, CMHistogramSpecsDlg::OnSelendokMethod)
		EVT_COMBOBOX_CLOSEUP (IDC_ChannelCombo, CMHistogramSpecsDlg::OnChannelComboCloseUp)
		EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo, CMHistogramSpecsDlg::OnChannelComboDropDown)
	#endif
	#if defined multispec_wxmac
		EVT_CHOICE (IDC_ChannelCombo, CMHistogramSpecsDlg::OnSelendokChannels)
		EVT_CHOICE (IDC_Method, CMHistogramSpecsDlg::OnSelendokMethod)
	#endif

	EVT_CHOICE (IDC_ColumnLineFormat, CMHistogramSpecsDlg::OnSelendokColumnLineFormat)


	EVT_INIT_DIALOG (CMHistogramSpecsDlg::OnInitDialog)

	EVT_TEXT (IDC_ColumnEnd, CMHistogramSpecsDlg::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMHistogramSpecsDlg::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMHistogramSpecsDlg::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMHistogramSpecsDlg::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMHistogramSpecsDlg::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMHistogramSpecsDlg::CheckColumnInterval)
END_EVENT_TABLE ()



CMHistogramSpecsDlg::CMHistogramSpecsDlg ()

{

}	// end "CMHistogramSpecsDlg"



CMHistogramSpecsDlg::CMHistogramSpecsDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title)
		: CMDialog (CMHistogramSpecsDlg::IDD, pParent, title)

{
   m_computeOnlyMinMaxFlag = TRUE;
   m_includeEmptyBinsFlag = TRUE;
   m_listHistogramValuesFlag = FALSE;
   m_listHistogramSummaryFlag = FALSE;
   m_histogramMethod = 1;
   m_textWindowFlag = TRUE;
   m_diskFileFlag = FALSE;
   m_fileName = "";
   m_columnsLinesFormat = 0;
   m_supportFileName = "";

   m_ColumnEnd = 1;
   m_ColumnInterval = 1;
   m_ColumnStart = 1;
   m_LineEnd = 1;
   m_LineInterval = 1;
   m_LineStart = 1;

   m_histogramMethodCode = kComputeFromImage;
   m_histogramSpecsPtr = NULL;
   m_defaultStatFileChangedFlag = FALSE;
   m_updateListHistogramItemsFlag = FALSE;
   CreateControls ();

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
				// Get pointer to memory for temporary storage of channel list.

      m_localFeaturesPtr = (UInt16*)MNewPointer (
              (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

      m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

}	// end "CMHistogramSpecsDlg"



CMHistogramSpecsDlg::~CMHistogramSpecsDlg (void)

{
   m_localFeaturesPtr = CheckAndDisposePtr (m_localFeaturesPtr);

}	// end "~CMHistogramSpecsDlg"



void CMHistogramSpecsDlg::CreateControls ()

{
   SetSizeHints (wxDefaultSize, wxDefaultSize);

   wxBoxSizer* bSizer80;
   bSizer80 = new wxBoxSizer (wxVERTICAL);

   wxBoxSizer* bSizer81;
   bSizer81 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText77 = new wxStaticText (this,
													wxID_ANY,
													wxT("Image File:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText77->Wrap (-1);
	bSizer81->Add (m_staticText77, wxSizerFlags(0).Border(wxRIGHT|wxBOTTOM, 5));

   m_staticText78 = new wxStaticText (this,
													IDC_FileName,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText78->Wrap (-1);
	bSizer81->Add (m_staticText78, wxSizerFlags(0).Border(wxBOTTOM,5));

	bSizer80->Add (bSizer81, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	wxBoxSizer* bSizer82;
   bSizer82 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText79 = new wxStaticText (this,
													wxID_ANY,
													wxT("Default Statistics File:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText79->Wrap (-1);
   SetUpToolTip (m_staticText79, IDS_ToolTip60);
	bSizer82->Add (m_staticText79, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

   m_staticText80 = new wxStaticText (this,
													IDC_DefaultFile,
													wxT("'None'"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   SetUpToolTip (m_staticText80, IDS_ToolTip60);
   m_staticText80->Wrap (-1);
	bSizer82->Add (m_staticText80, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));

	bSizer80->Add (bSizer82, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer83;
   bSizer83 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText81 = new wxStaticText (this,
													wxID_ANY,
													wxT("Method:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText81->Wrap (-1);
   SetUpToolTip (m_staticText81, IDS_ToolTip61);
   bSizer83->Add (m_staticText81,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	#if defined multispec_wxlin
		m_methodCtrl = new wxComboBox (this,
													IDC_Method,
													wxT("From default statistics file"),
													wxDefaultPosition,
													wxSize (200, -1),
													0,
													NULL,
													wxCB_READONLY);
	#endif	// multispec_wxlin
	#if defined multispec_wxmac
		m_methodCtrl = new wxChoice (this,
												IDC_Method,
												wxDefaultPosition,
												wxSize (200, -1));
	#endif	// multispec_wxmac

	m_methodCtrl->Append (wxT("From default statistics file"));
   m_methodCtrl->Append (wxT("Compute new histogram"));
   m_methodCtrl->Append (wxT("Select default statistics..."));
   SetUpToolTip (m_methodCtrl, IDS_ToolTip61);
   bSizer83->Add (m_methodCtrl, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));

	bSizer80->Add (bSizer83, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	bSizer80->Add (6,6);

   wxStaticBoxSizer* sbSizer8;
	wxStaticBox* areaToHistogramBox = new wxStaticBox (this,
																		IDC_LineColFrame,
																		wxT("Area to Histogram"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL);
   sbSizer8 = new wxStaticBoxSizer (areaToHistogramBox, wxHORIZONTAL);

	CreateLineColumnControls (sbSizer8);
	
	bSizer80->Add (sbSizer8, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	bSizer80->Add (6,6);

   wxBoxSizer* bSizer86;
   bSizer86 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText94 = new wxStaticText (this,
													wxID_ANY,
													wxT("Channels:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText94->Wrap (-1);
   SetUpToolTip (m_staticText94, IDS_ToolTip52);
	bSizer86->Add (m_staticText94,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_staticText95 = new wxStaticText (this,
													IDC_AllChannels,
													wxT("All"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText95->Wrap (-1);
	bSizer86->Add (m_staticText95,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   GetAllSubsetMenuControl (kChannelsMenu,
										this,
										IDC_ChannelCombo,
										120,
										IDS_ToolTip52);
   bSizer86->Add (m_channelsCtrl, wxSizerFlags(0).Border(wxALL, 5));

 	bSizer80->Add (bSizer86, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	bSizer80->Add (3,3);
	
			// Currently the following variable is not used. Hide for now.
	
   m_checkBox23 = new wxCheckBox (
									this,
									IDC_ComputeOnly,
									wxT("Compute only min., max., mean and standard deviation"),
									wxDefaultPosition,
									wxDefaultSize,
									0);
   bSizer80->Add (m_checkBox23, wxSizerFlags(0).Border(wxALL, 5));
	m_checkBox23->Hide ();

   wxBoxSizer* bSizer87;
   bSizer87 = new wxBoxSizer (wxHORIZONTAL);

   wxStaticBoxSizer* sbSizer14;
   sbSizer14 = new wxStaticBoxSizer (new wxStaticBox (this,
																		wxID_ANY,
																		wxT("List Options")),
													wxVERTICAL);

   m_checkBox18 = new wxCheckBox (sbSizer14->GetStaticBox (),
												IDC_ListSummary,
												wxT("List Histogram summary"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox18, IDS_ToolTip62);
   sbSizer14->Add (m_checkBox18, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox19 = new wxCheckBox (sbSizer14->GetStaticBox (),
												IDC_ListHistogram,
												wxT("List Histogram"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox19, IDS_ToolTip63);
   sbSizer14->Add (m_checkBox19, wxSizerFlags(0).Border(wxALL, 5));

   wxBoxSizer* bSizer88;
   bSizer88 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText96 = new wxStaticText (sbSizer14->GetStaticBox (),
													IDC_ColumnLinePrompt,
													wxT("Format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText96->Wrap (-1);
   bSizer88->Add (m_staticText96,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

	wxChoice*		comboBox19;
   comboBox19 = new wxChoice (sbSizer14->GetStaticBox (),
										IDC_ColumnLineFormat,
										wxDefaultPosition,
										wxDefaultSize);
   comboBox19->Append (wxT("Columns"));
   comboBox19->Append (wxT("Lines"));
   SetUpToolTip (comboBox19, IDS_ToolTip64);
   bSizer88->Add (comboBox19,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   sbSizer14->Add (
   				bSizer88,
					wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   m_checkBox20 = new wxCheckBox (sbSizer14->GetStaticBox (),
												IDC_EmptyBins,
												wxT("Include empty bins"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox20, IDS_ToolTip65);
   sbSizer14->Add (m_checkBox20,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	bSizer87->Add (sbSizer14, wxSizerFlags(0).Expand().Border(wxRIGHT, 6));

   wxBoxSizer* bSizer89;
   bSizer89 = new wxBoxSizer (wxVERTICAL);

   wxStaticBoxSizer* sbSizer15;
   sbSizer15 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_WriteResultsFrame,
																		wxT("Write results to")),
													wxVERTICAL);

   m_checkBox21 = new wxCheckBox (sbSizer15->GetStaticBox (),
												IDC_TextWindow,
												wxT("Text window"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox21, IDS_ToolTip66);
   sbSizer15->Add (m_checkBox21,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   m_checkBox22 = new wxCheckBox (sbSizer15->GetStaticBox (),
												IDC_DiskFile,
												wxT("Disk file"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox22, IDS_ToolTip67);
	sbSizer15->Add (m_checkBox22,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	bSizer89->Add (sbSizer15, wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden());

	bSizer87->Add (bSizer89, wxSizerFlags(1).Expand().Border(wxLEFT, 6));

	bSizer80->Add (bSizer87, wxSizerFlags(1).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bSizer80->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bSizer80->Add (standardButtonSizer,
							wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif

   SetSizer (bSizer80);
   Layout ();
   bSizer80->Fit (this);
	
}	// end "CreateControls"



void CMHistogramSpecsDlg::CheckWriteResultsToSettings (void)

{
   if (m_listHistogramValuesFlag)
		{
      		// Verify that an output destination has been selected. If
      		// not, then deselect the OK button.

		m_textWindowFlag = ((wxCheckBox*)FindWindow (IDC_TextWindow))->GetValue ();
      m_diskFileFlag = ((wxCheckBox*)FindWindow (IDC_DiskFile))->GetValue ();

      if (!m_textWindowFlag && !m_diskFileFlag)
         SetDLogControlHilite (NULL, wxID_OK, 255);

      else	// m_textWindowFlag || m_diskFileFlag
         SetDLogControlHilite (NULL, wxID_OK, 0);

   	}	// end "if (m_listHistogramValuesFlag)"

   else	// !m_listHistogramValuesFlag
      SetDLogControlHilite (NULL, wxID_OK, 0);

}	// end "CheckWriteResultsToSettings"



Boolean CMHistogramSpecsDlg::DoDialog (
				HistogramSpecsPtr 				histogramSpecsPtr,
				WindowInfoPtr 						windowInfoPtr,
				FileInfoPtr 						fileInfoPtr)

{
	SInt16 								returnCode;

   Boolean continueFlag = FALSE;


   		// Make sure intialization has been completed.

   if (!m_initializedFlag)
      																				return (FALSE);

   m_histogramSpecsPtr = histogramSpecsPtr;
   m_windowInfoPtr = windowInfoPtr;
   m_fileInfoPtr = fileInfoPtr;

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
   	{
      		// Histogram area

      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = m_LineInterval;

      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = m_ColumnInterval;

      HistogramDialogOK (this,
								  histogramSpecsPtr,
								  windowInfoPtr,
								  m_histogramMethodCode,
								  &m_dialogSelectArea,
								  m_localFeaturesPtr,
								  m_localActiveNumberFeatures,
								  m_channelSelection,
								  m_computeOnlyMinMaxFlag,
								  m_listHistogramSummaryFlag,
								  m_listHistogramValuesFlag,
								  (m_columnsLinesFormat == 1),
								  m_includeEmptyBinsFlag,
								  m_textWindowFlag,
								  m_diskFileFlag,
								  m_defaultStatFileChangedFlag,
								  m_allChannelsAtOnceFlag);

      continueFlag = TRUE;

   	}	// end "if (returnCode == IDOK)"

   else	// returnCode != IDOK
      		// Set computeFlag so that new histogram is not computed
      histogramSpecsPtr->computeFlag = FALSE;

   return (continueFlag);

}	// end "DoDialog"



SInt16 CMHistogramSpecsDlg::GetItemCode (
				wxString 							selection)

{
	SInt16 								returnCode;
	
	
   if (selection.Cmp (wxT("From default statistics file")) == 0)
      returnCode = kStoredInMemory;
	
   else if (selection.Cmp (wxT("Compute new histogram")) == 0)
      returnCode = kComputeFromImage;
	
   else if (selection.Cmp (wxT("Select default statistics ...")) == 0)
      returnCode = kReadFromSupportFile;
	
   else
      returnCode = 0;
	
   return returnCode;
	
}	// end "GetItemCode"



void CMHistogramSpecsDlg::OnComputeOnly (
				wxCommandEvent& 					event)

{
   UpdateListAndEmptyBinsDialogBoxes ();

}	// end "OnComputeOnly"



void CMHistogramSpecsDlg::OnDiskFile (
				wxCommandEvent& 					event)

{
   CheckWriteResultsToSettings ();

}	// end "OnDiskFile"


void CMHistogramSpecsDlg::OnEmptyBins (
				wxCommandEvent& 					event)

{
  m_includeEmptyBinsFlag = ((wxCheckBox*)FindWindow (IDC_EmptyBins))->GetValue ();

}	// end "OnEmptyBins"



void CMHistogramSpecsDlg::OnInitDialog (
				wxInitDialogEvent& 				event)

{
   DialogPtr 							dialogPtr = this;

   SInt16 								channelSelection;

   Boolean 								histogramInMemoryAvailableFlag,
           								lineFormatHistFlag;

			// Initialize the histogram dialog variables.
			// Note that the boolean variables are defined as Boolean in
			// HistogramDialogInitialize and bool in this linux routine. Boolean and
			// bool are not the same size. One needs to make an intermediate
			// variable to handle this difference.
	
	Boolean 								computeOnlyMinMaxFlag,
											listHistogramSummaryFlag,
											listHistogramValuesFlag,
											includeEmptyBinsFlag,
											textWindowFlag,
											diskFileFlag;
	
	
	CMDialog::OnInitDialog (event);

	HistogramDialogInitialize (dialogPtr,
										  m_histogramSpecsPtr,
										  m_windowInfoPtr,
										  m_fileInfoPtr,
										  &m_fileName,
										  &m_supportFileName,
										  &m_histogramMethodCode,
										  &histogramInMemoryAvailableFlag,
										  &m_dialogSelectArea,
										  m_localFeaturesPtr,
										  &m_localNumberFeatures,
										  &channelSelection,
										  &computeOnlyMinMaxFlag,
										  &listHistogramSummaryFlag,
										  &listHistogramValuesFlag,
										  &lineFormatHistFlag,
										  &includeEmptyBinsFlag,
										  &textWindowFlag,
										  &diskFileFlag,
										  &m_defaultStatFileChangedFlag,
										  &m_updateListHistogramItemsFlag,
										  &m_allChannelsAtOnceFlag);
			  
	m_computeOnlyMinMaxFlag = computeOnlyMinMaxFlag;
	m_listHistogramSummaryFlag = listHistogramSummaryFlag;
	m_listHistogramValuesFlag = listHistogramValuesFlag;
	m_includeEmptyBinsFlag = includeEmptyBinsFlag;
	m_textWindowFlag = textWindowFlag;
	m_diskFileFlag = diskFileFlag;

   		// Set the method code for the histogram method drop down list items.

   m_histogramMethod = m_histogramMethodCode - 1;

  			// Only allow for computation for thematic type images.

   if (m_fileInfoPtr->thematicType)
   	{
      m_methodCtrl->Delete (2);
      m_methodCtrl->Delete (0);

      m_histogramMethod = 0;

   	}	// end "if (m_fileInfoPtr->thematicType)"

   else	// !fileInfoPtr->thematicType
   	{
      if (!histogramInMemoryAvailableFlag)
      	{
         m_methodCtrl->Delete (0);
         m_histogramMethod--;

      	}	// end "if (!histogramInMemoryAvailableFlag)"

   	}	// end "else !fileInfoPtr->thematicType"

   		// Selected area to histogram.

   m_LineStart = m_dialogSelectArea.lineStart;
   m_LineEnd = m_dialogSelectArea.lineEnd;
   m_LineInterval = m_dialogSelectArea.lineInterval;
   m_ColumnStart = m_dialogSelectArea.columnStart;
   m_ColumnEnd = m_dialogSelectArea.columnEnd;
   m_ColumnInterval = m_dialogSelectArea.columnInterval;

   		// Set feature parameters

   m_localActiveNumberFeatures = m_localNumberFeatures;
   m_localActiveFeaturesPtr = m_localFeaturesPtr;
   m_channelSelection = channelSelection;

   m_columnsLinesFormat = 0;
   if (lineFormatHistFlag)
      m_columnsLinesFormat = 1;

   		// Set default text selection to first edit text item

   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);

   if (CMHistogramSpecsDlg::TransferDataToWindow ())
      PositionDialogWindow ();

}	// end "OnInitDialog"



void CMHistogramSpecsDlg::OnListHistogram (
				wxCommandEvent& 					event)

{
   UpdateListAndEmptyBinsDialogBoxes ();

}	// end "OnListHistogram"



void CMHistogramSpecsDlg::OnSelendokColumnLineFormat (
				wxCommandEvent& 					event)

{
   UpdateListAndEmptyBinsDialogBoxes ();

}	// end "OnSelendokColumnLineFormat"



void CMHistogramSpecsDlg::OnSelendokChannels (
				wxCommandEvent& 					event)

{
	Boolean 								lineFormatHistFlag;


   HandleChannelsMenu (IDC_ChannelCombo,
							  kNoTransformation,
							  (SInt16) gImageWindowInfoPtr->totalNumberChannels,
							  kImageChannelType,
							  TRUE);

   m_allChannelsAtOnceFlag = HistogramDialogUpdateAllChannelsAtOnceFlag (
																		  this,
																		  m_localActiveNumberFeatures,
																		  &lineFormatHistFlag);

   m_columnsLinesFormat = 0;
   if (lineFormatHistFlag)
      m_columnsLinesFormat = 1;

}	// end "OnSelendokChannels"



void CMHistogramSpecsDlg::OnTextWindow (
				wxCommandEvent& 					event)

{
   CheckWriteResultsToSettings ();

}	// end "OnTextWindow"



void CMHistogramSpecsDlg::OnSelendokMethod (
				wxCommandEvent& 					event)

{
   wxString 							prvsel,
   										cursel;
	
   int 									localMethod;

   SInt16 								previousMethodCode;


   prvsel = m_methodCtrl->GetString (m_histogramMethod);
   previousMethodCode = GetItemCode (prvsel);
   
   localMethod = m_methodCtrl->GetSelection ();
   m_histogramMethodCode = (SInt16)localMethod + 1;

   m_histogramMethodCode = HistogramDialogHandleMethod (
																  this,
																  m_histogramSpecsPtr,
																  m_windowInfoPtr,
																  m_fileInfoPtr,
																  &m_supportFileName,
																  m_histogramMethodCode,
																  wxID_OK,
																  previousMethodCode,
																  &m_updateListHistogramItemsFlag,
																  &m_defaultStatFileChangedFlag);

   if (m_histogramMethodCode == kComputeFromImage)
		{
      SetEntireImageButtons (NULL,
									  m_LineStart,
									  m_LineEnd,
									  m_ColumnStart,
									  m_ColumnEnd);
		
		}	// end "if (m_histogramMethodCode == kComputeFromImage)"

   		// Set the proper method item.

   if ((m_methodCtrl->GetString(0)).Cmp (wxT("From default statistics file")) == 0)
      m_histogramMethod = m_histogramMethodCode - 1;

   else	// !methodCtrl->GetItemData (0) == kStoredInMemory
      m_histogramMethod = m_histogramMethodCode - 2;

   m_methodCtrl->SetSelection (m_histogramMethod);

   if (m_updateListHistogramItemsFlag)
		{
      HistogramDialogSetListAndEmptyBins (this,
														  m_histogramMethodCode,
														  m_computeOnlyMinMaxFlag,
														  m_listHistogramValuesFlag,
														  m_includeEmptyBinsFlag,
														  (m_columnsLinesFormat == 1));

		}	// end "if (m_updateListHistogramItemsFlag)"

}	// end "OnSelendokMethod"



void CMHistogramSpecsDlg::ShowAreaHistogramItems (void)

{
   HistogramDialogShowAreaItems (this);

   SetEntireImageButtons (NULL,
								  m_LineStart,
								  m_LineEnd,
								  m_ColumnStart,
								  m_ColumnEnd);

}	// end "ShowAreaHistogramItems"



bool CMHistogramSpecsDlg::TransferDataFromWindow ()

{
	SInt16								returnCode = 0;
					
	
   m_channelSelection = m_channelsCtrl->GetSelection ();
   m_histogramMethod = m_methodCtrl->GetSelection ();
	
   wxChoice* linecolcb = (wxChoice*)FindWindow (IDC_ColumnLineFormat);
	m_columnsLinesFormat = linecolcb->GetSelection ();
	
	TransferLinesColumnsFromWindow ();

   wxCheckBox* cponly = (wxCheckBox*)FindWindow (IDC_ComputeOnly);
   m_computeOnlyMinMaxFlag = cponly->GetValue ();
	
   wxCheckBox* emptybins = (wxCheckBox*)FindWindow (IDC_EmptyBins);
   m_includeEmptyBinsFlag = emptybins->GetValue ();
	
   wxCheckBox* listh = (wxCheckBox*)FindWindow (IDC_ListHistogram);
   m_listHistogramValuesFlag = listh->GetValue ();
	
   wxCheckBox* listsumm = (wxCheckBox*)FindWindow (IDC_ListSummary);
   m_listHistogramSummaryFlag = listsumm->GetValue ();
	
   wxCheckBox* textw = (wxCheckBox*)FindWindow (IDC_TextWindow);
   m_textWindowFlag = textw->GetValue ();
	
   wxCheckBox* diskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	m_diskFileFlag = diskf->GetValue ();
   
	if (m_histogramMethodCode == kComputeFromImage)
		returnCode = VerifyLineColumnValues (IDC_LineStart,
														  IDC_ColumnStart,
														  true);
						  
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"



bool CMHistogramSpecsDlg::TransferDataToWindow ()

{
   m_channelsCtrl->SetSelection (m_channelSelection);
   m_methodCtrl->SetSelection (m_histogramMethod);
	
   wxChoice* linecolcb = (wxChoice*)FindWindow (IDC_ColumnLineFormat);
	linecolcb->SetSelection (m_columnsLinesFormat);

   wxCheckBox* cponly = (wxCheckBox*)FindWindow (IDC_ComputeOnly);
   cponly->SetValue (m_computeOnlyMinMaxFlag);
	
   wxCheckBox* emptybins = (wxCheckBox*)FindWindow (IDC_EmptyBins);
   emptybins->SetValue (m_includeEmptyBinsFlag);
	
   wxCheckBox* listh = (wxCheckBox*)FindWindow (IDC_ListHistogram);
   listh->SetValue (m_listHistogramValuesFlag);
	
   wxCheckBox* listsumm = (wxCheckBox*)FindWindow (IDC_ListSummary);
   listsumm->SetValue (m_listHistogramSummaryFlag);
	
   wxCheckBox* textw = (wxCheckBox*)FindWindow (IDC_TextWindow);
   textw->SetValue (m_textWindowFlag);
	
   wxCheckBox* diskf = (wxCheckBox*)FindWindow (IDC_DiskFile);
	diskf->SetValue (m_diskFileFlag);
	
	TransferLinesColumnsToWindow ();

   wxStaticText* fname = (wxStaticText*)FindWindow (IDC_FileName);
   fname->SetLabel (m_fileName);
	
   wxStaticText* dfname = (wxStaticText*)FindWindow (IDC_DefaultFile);
	dfname->SetLabel (m_supportFileName);
	
   return true;
	
}	// end "TransferDataToWindow"


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateListAndEmptyBinsDialogBoxes                
//
//	Software purpose:	The purpose of this routine is to update the list and
//							empty bin dialog check boxes in the histogram
//							dialog window. This routine is called after a dialog
//							item has been changed
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:			HistogramDialog in SHistogram.cpp
//
//	Coded By:			Larry L. Biehl					Date:	06/28/1995
//	Revised By:			Larry L. Biehl					Date: 06/28/1995

void CMHistogramSpecsDlg::UpdateListAndEmptyBinsDialogBoxes (void)

{
			// Get current settings for:
			//		compute only min, max, mean, etc
			//		list histogram values
			//		include empty bins
			//		columns or lines format

   m_computeOnlyMinMaxFlag =
   						((wxCheckBox*)FindWindow (IDC_ComputeOnly))->GetValue ();

   m_listHistogramValuesFlag =
   						((wxCheckBox*)FindWindow (IDC_ListHistogram))->GetValue ();

   m_columnsLinesFormat =
   						((wxChoice*)FindWindow (IDC_ColumnLineFormat))->GetSelection ();

   HistogramDialogSetListAndEmptyBins (this,
													  m_histogramMethodCode,
													  m_computeOnlyMinMaxFlag,
													  m_listHistogramValuesFlag,
													  m_includeEmptyBinsFlag,
													  (m_columnsLinesFormat == 1));

   CheckWriteResultsToSettings ();

}	// end "UpdateListAndEmptyBinsDialogBoxes"

