//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LHistogramDialog.cpp : class implementation file
//	Class Definition:		LHistogramDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/16/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMHistogramSpecsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "CHistogram.h"
#include "CImageWindow.h"

#include "LHistogramDialog.h"
#include "LImageView.h"
#include "LImage_dialog.cpp"

typedef wxString CString;

extern SInt16 HistogramDialogHandleMethod (
				DialogPtr							dialogPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				CString*								supportFileNamePtr,
				SInt16								itemHit,
				SInt16								okItemNumber,
				SInt16								previousMethodCode,
				Boolean*								updateListHistogramItemsFlagPtr,
				Boolean*								defaultStatFileChangedFlagPtr);

extern void HistogramDialogHideAreaItems (
				DialogPtr							dialogPtr);

extern void HistogramDialogHideListItems (
				DialogPtr							dialogPtr);

extern void HistogramDialogInitialize (
				DialogPtr							dialogPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr windowInfoPtr,
				FileInfoPtr fileInfoPtr,
				CString* imageFileNamePtr,
				CString* supportFileNamePtr,
				SInt16* histogramMethodCodePtr,
				Boolean* histogramInMemoryAvailableFlagPtr,
				DialogSelectArea* dialogSelectAreaPtr,
				UInt16* localChannelsPtr,
				UInt16* localNumberChannelsPtr,
				SInt16* channelSelectionPtr,
				Boolean* minMaxetcOnlyFlagPtr,
				Boolean* listHistogramSummaryFlagPtr,
				Boolean* listHistogramFlagPtr,
				Boolean* lineFormatHistFlagPtr,
				Boolean* includeEmptyBinsFlagPtr,
				Boolean* textWindowFlagPtr,
				Boolean* diskFileFlagPtr,
				Boolean* defaultStatFileChangedFlagPtr,
				Boolean* updateListHistogramItemsFlagPtr,
				Boolean* allChannelsAtOnceFlagPtr);

extern void HistogramDialogOK (
        DialogPtr dialogPtr,
        HistogramSpecsPtr histogramSpecsPtr,
        WindowInfoPtr windowInfoPtr,
        SInt16 histogramMethodCode,
        DialogSelectArea* dialogSelectAreaPtr,
        UInt16* localChannelsPtr,
        UInt16 localNumberChannels,
        SInt16 channelSelection,
        Boolean minMaxetcOnlyFlag,
        Boolean listHistogramSummaryFlag,
        Boolean listHistogramFlag,
        Boolean lineFormatHistFlag,
        Boolean includeEmptyBinsFlag,
        Boolean textWindowFlag,
        Boolean diskFileFlag,
        Boolean defaultStatFileChangedFlag,
        Boolean allChannelsAtOnceFlag);

extern void HistogramDialogSetListAndEmptyBins(
        DialogPtr dialogPtr,
        SInt16 histogramMethodCode,
        Boolean minMaxetcOnlyFlag,
        Boolean listHistogramFlag,
        Boolean includeEmptyBinsFlag,
        Boolean lineFormatHistFlag);

extern void HistogramDialogShowAreaItems(
        DialogPtr dialogPtr);

extern void HistogramDialogShowListItems(
        DialogPtr dialogPtr);

extern Boolean HistogramDialogUpdateAllChannelsAtOnceFlag(
        DialogPtr dialogPtr,
        UInt16 numberChannels,
        Boolean* lineFormatHistFlagPtr);



IMPLEMENT_DYNAMIC_CLASS(CMHistogramSpecsDlg, CMDialog)

CMHistogramSpecsDlg::CMHistogramSpecsDlg()

{

}



CMHistogramSpecsDlg::CMHistogramSpecsDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMHistogramSpecsDlg::IDD, pParent, title)

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
   CreateControls();

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag)
		{
				// Get pointer to memory for temporary storage of channel list.

      m_localFeaturesPtr = (UInt16*) MNewPointer(
              (UInt32) gImageWindowInfoPtr->totalNumberChannels * sizeof (UInt16));

      m_initializedFlag = (m_localFeaturesPtr != NULL);

		}	// end "if (m_initializedFlag)"

}	// end "CMHistogramSpecsDlg"



CMHistogramSpecsDlg::~CMHistogramSpecsDlg (void)

{
   m_localFeaturesPtr = CheckAndDisposePtr(m_localFeaturesPtr);

}	// end "~CMHistogramSpecsDlg"



BEGIN_EVENT_TABLE(CMHistogramSpecsDlg, CMDialog)
EVT_INIT_DIALOG(CMHistogramSpecsDlg::OnInitDialog)
EVT_CHECKBOX(IDC_ListHistogram, CMHistogramSpecsDlg::OnListHistogram)
EVT_CHECKBOX(IDC_ComputeOnly, CMHistogramSpecsDlg::OnComputeOnly)
EVT_COMBOBOX(IDC_ColumnLineFormat, CMHistogramSpecsDlg::OnSelendokColumnLineFormat)
EVT_CHECKBOX(IDC_EmptyBins, CMHistogramSpecsDlg::OnEmptyBins)
EVT_CHECKBOX(IDC_DiskFile, CMHistogramSpecsDlg::OnDiskFile)
EVT_CHECKBOX(IDC_TextWindow, CMHistogramSpecsDlg::OnTextWindow)
EVT_COMBOBOX(IDC_Method, CMHistogramSpecsDlg::OnSelendokMethod)
EVT_BUTTON(IDEntireImage, CMHistogramSpecsDlg::ToEntireImage)
EVT_BUTTON(IDSelectedImage, CMHistogramSpecsDlg::ToSelectedImage)
EVT_TEXT(IDC_ColumnEnd, CMHistogramSpecsDlg::CheckColumnEnd)
EVT_TEXT(IDC_ColumnStart, CMHistogramSpecsDlg::CheckColumnStart)
EVT_TEXT(IDC_LineEnd, CMHistogramSpecsDlg::CheckLineEnd)
EVT_TEXT(IDC_LineStart, CMHistogramSpecsDlg::CheckLineStart)
EVT_TEXT(IDC_LineInterval, CMHistogramSpecsDlg::CheckLineInterval)
EVT_TEXT(IDC_ColumnInterval, CMHistogramSpecsDlg::CheckColumnInterval)
EVT_COMBOBOX (IDC_ChannelCombo, CMHistogramSpecsDlg::OnSelendokChannels)
EVT_COMBOBOX_DROPDOWN (IDC_ChannelCombo,CMHistogramSpecsDlg::OnSelendokChannelComboDropDown)
//EVT_CHAR_HOOK(CMHistogramSpecsDlg::OnButtonPress)
END_EVENT_TABLE()



void CMHistogramSpecsDlg::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);

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
   SetUpToolTip(m_staticText81, IDS_ToolTip61);
   bSizer83->Add (m_staticText81,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

   m_comboBox17 = new wxComboBox (this,
												IDC_Method,
												//wxEmptyString,
												wxT("From default statistics file"),
												wxDefaultPosition,
												wxSize (200,-1),
												//wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
   m_comboBox17->Append (wxT("From default statistics file"));
   m_comboBox17->Append (wxT("Compute new histogram"));
   m_comboBox17->Append (wxT("Select default statistics..."));
   SetUpToolTip (m_comboBox17, IDS_ToolTip61);
   bSizer83->Add (m_comboBox17, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 5));

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
	/*
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);

   m_bpButton4 = new wxBitmapButton (areaToHistogramBox,
													IDEntireImage,
													entireimi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   m_bpButton4->SetBitmapDisabled (toentirei);
   bSizer791->Add (m_bpButton4,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_bpButton5 = new wxBitmapButton (areaToHistogramBox,
													IDSelectedImage,
													selectedi,
													wxDefaultPosition,
													wxDefaultSize,
													wxBU_AUTODRAW);

   m_bpButton5->SetBitmapDisabled (bmp4i);
   bSizer791->Add (m_bpButton5,
							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   m_bpButton5->Hide ();
   sbSizer8->Add (bSizer791, wxSizerFlags(0).Expand().Border(wxALL, 5));
	*/
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

   m_comboBox18 = new wxComboBox (this,
												IDC_ChannelCombo,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												wxCB_READONLY);
   m_comboBox18->Append (wxT("All"));
   m_comboBox18->Append (wxT("Subset..."));
   SetUpToolTip (m_comboBox18, IDS_ToolTip52);
   bSizer86->Add (m_comboBox18, wxSizerFlags(0).Border(wxALL, 5));

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

   m_checkBox18 = new wxCheckBox (sbSizer14->GetStaticBox(),
												IDC_ListSummary,
												wxT("List Histogram summary"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox18, IDS_ToolTip62);
   sbSizer14->Add (m_checkBox18, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox19 = new wxCheckBox (sbSizer14->GetStaticBox(),
												IDC_ListHistogram,
												wxT("List Histogram"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox19, IDS_ToolTip63);
   sbSizer14->Add (m_checkBox19, wxSizerFlags(0).Border(wxALL, 5));

   wxBoxSizer* bSizer88;
   bSizer88 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText96 = new wxStaticText (sbSizer14->GetStaticBox(),
													IDC_ColumnLinePrompt,
													wxT("Format:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
   m_staticText96->Wrap (-1);
   bSizer88->Add (m_staticText96,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_comboBox19 = new wxComboBox (sbSizer14->GetStaticBox(),
												IDC_ColumnLineFormat,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0,
												NULL,
												0);
   m_comboBox19->Append (wxT("Columns"));
   m_comboBox19->Append (wxT("Lines"));
   SetUpToolTip (m_comboBox19, IDS_ToolTip64);
   bSizer88->Add (m_comboBox19,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   sbSizer14->Add (bSizer88,
						wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   m_checkBox20 = new wxCheckBox (sbSizer14->GetStaticBox(),
												IDC_EmptyBins,
												wxT("Include empty bins"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip(m_checkBox20, IDS_ToolTip65);
   sbSizer14->Add(m_checkBox20,
						wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	bSizer87->Add (sbSizer14, wxSizerFlags(0).Expand().Border(wxRIGHT, 6));

   wxBoxSizer* bSizer89;
   bSizer89 = new wxBoxSizer (wxVERTICAL);

   wxStaticBoxSizer* sbSizer15;
   sbSizer15 = new wxStaticBoxSizer (new wxStaticBox (this,
																		IDC_WriteResultsFrame,
																		wxT("Write results to")),
													wxVERTICAL);

   m_checkBox21 = new wxCheckBox (sbSizer15->GetStaticBox(),
												IDC_TextWindow,
												wxT("Text window"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox21, IDS_ToolTip66);
   sbSizer15->Add (m_checkBox21,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

   m_checkBox22 = new wxCheckBox (sbSizer15->GetStaticBox(),
												IDC_DiskFile,
												wxT("Disk file"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_checkBox22, IDS_ToolTip67);
	sbSizer15->Add (m_checkBox22,
							wxSizerFlags(0).ReserveSpaceEvenIfHidden().Border(wxALL, 5));

	bSizer89->Add (sbSizer15, wxSizerFlags(0).Expand().ReserveSpaceEvenIfHidden());
	/*
   wxBoxSizer* bSizer90;
   bSizer90 = new wxBoxSizer (wxHORIZONTAL);

   m_button26 = new wxButton (this,
										wxID_CANCEL,
										wxT("Cancel"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer90->Add(m_button26, wxSizerFlags(0).Border(wxRIGHT, 6));

   m_button27 = new wxButton (this,
										wxID_OK,
										wxT("OK"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer90->Add (m_button27, wxSizerFlags(0));

	bSizer89->Add (bSizer90, wxSizerFlags(1).Right().Border(wxTOP, 46));
	*/
	//bSizer89->Add (standardButtonSizer, wxSizerFlags(1).Right().Border(wxTOP, 46));

	bSizer87->Add (bSizer89, wxSizerFlags(1).Expand().Border(wxLEFT, 6));

	//bSizer80->Add (bSizer87, wxSizerFlags(1).Expand().Border(wxALL, 12));
	bSizer80->Add (bSizer87, wxSizerFlags(1).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	#if defined multispec_wxmac
		bSizer80->Add (standardButtonSizer, wxSizerFlags(0).Right());
	#else
		bSizer80->Add (standardButtonSizer, wxSizerFlags(0).Right().Border(wxTOP|wxBOTTOM, 12));
	#endif

   this->SetSizer(bSizer80);
   this->Layout();
   bSizer80->Fit(this);
   //this->Centre();
	
}	// end "CreateControls"


//-----------------------------------------------------------------------------
//					 Copyright (1988-1998)
//                                  c Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void CheckWriteResultsToSettings                
//
//	Software purpose:	The purpose of this routine is to verify that an output 
//				destination has been selected. If not, then deselect the 
//				OK button.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
//	Value Returned:  	None
//
// Called By:			HistogramDialog in histogram.c
//
//	Coded By:			Larry L. Biehl					Date:	06/28/1995
//	Revised By:			Larry L. Biehl					Date: 07/21/2015

void CMHistogramSpecsDlg::CheckWriteResultsToSettings(void)

{
   if (m_listHistogramValuesFlag) {
      // Verify that an output destination has been selected. If
      // not, then deselect the OK button.

      //DDX_Check(m_dialogFromPtr, IDC_TextWindow, m_textWindowFlag);
      m_textWindowFlag = ((wxCheckBox *) FindWindow(IDC_TextWindow))->GetValue();
      //DDX_Check(m_dialogFromPtr, IDC_DiskFile, m_diskFileFlag);
      m_diskFileFlag = ((wxCheckBox *) FindWindow(IDC_DiskFile))->GetValue();

      if (!m_textWindowFlag && !m_diskFileFlag)
         SetDLogControlHilite(NULL, wxID_OK, 255);

      else // m_textWindowFlag || m_diskFileFlag
         SetDLogControlHilite(NULL, wxID_OK, 0);

   }// end "if (m_listHistogramValuesFlag)"

   else // !m_listHistogramValuesFlag
      SetDLogControlHilite(NULL, wxID_OK, 0);

} // end "CheckWriteResultsToSettings"


//-----------------------------------------------------------------------------
//					 Copyright (1988-1998)
//                                  c Purdue Research Foundation
//					All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//				specification dialog box to the user and copy the
//				revised back to the display specification structure if
//				the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/27/95
//	Revised By:			Larry L. Biehl			Date: 11/23/99	

Boolean
CMHistogramSpecsDlg::DoDialog(
        HistogramSpecsPtr histogramSpecsPtr,
        WindowInfoPtr windowInfoPtr,
        FileInfoPtr fileInfoPtr) {
   SInt16 returnCode;

   Boolean continueFlag = FALSE,
           defaultStatFileChangedFlag = FALSE;


   // Make sure intialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   m_histogramSpecsPtr = histogramSpecsPtr;
   m_windowInfoPtr = windowInfoPtr;
   m_fileInfoPtr = fileInfoPtr;

   returnCode = ShowModal();

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

   else // returnCode != IDOK
      // Set computeFlag so that new histogram is not computed
      histogramSpecsPtr->computeFlag = FALSE;

   return (continueFlag);

}	// end "DoDialog"


SInt16 CMHistogramSpecsDlg::GetItemCode(wxString selection) {
   SInt16 returnCode;
   if (selection.Cmp(wxT("From default statistics file")) == 0)
      returnCode = kStoredInMemory;
   else if (selection.Cmp(wxT("Compute new histogram")) == 0)
      returnCode = kComputeFromImage;
   else if (selection.Cmp(wxT("Select default statistics ...")) == 0)
      returnCode = kReadFromSupportFile;
   else
      returnCode = 0;
   return returnCode;
}

/*
void CMHistogramSpecsDlg::OnButtonPress(wxKeyEvent& event) {
    if (event.GetKeyCode() == WXK_RETURN) {
        if(TransferDataFromWindow())
            EndModal(wxID_OK);
    } else {
        event.Skip();
    }
}
*/

void CMHistogramSpecsDlg::OnComputeOnly(wxCommandEvent& event) {
   UpdateListAndEmptyBinsDialogBoxes();

} // end "OnComputeOnly"


void CMHistogramSpecsDlg::OnDiskFile(wxCommandEvent& event) {
   CheckWriteResultsToSettings();

} // end "OnDiskFile"


void CMHistogramSpecsDlg::OnEmptyBins (wxCommandEvent& event)

{
   //DDX_Check(m_dialogFromPtr, IDC_EmptyBins, m_includeEmptyBinsFlag);
   m_includeEmptyBinsFlag = ((wxCheckBox *) FindWindow(IDC_EmptyBins))->GetValue();
   //	mIncludeEmptyBinsFlag = m_IncludeEmptyBins;

}	// end "OnEmptyBins"


/////////////////////////////////////////////////////////////////////////////
// CMHistogramSpecsDlg message handlers

void CMHistogramSpecsDlg::OnInitDialog(wxInitDialogEvent& event) {
   wxComboBox* comboBoxPtr;
   DialogPtr dialogPtr = this;

   SInt16 channelSelection;

   Boolean histogramInMemoryAvailableFlag,
           lineFormatHistFlag;


   CMDialog::OnInitDialog(event);

   // Make sure that we have the bitmaps for the entire image buttons.

   //VERIFY(toEntireButton.AutoLoad(IDEntireImage, this));
   //VERIFY(toSelectedButton.AutoLoad(IDSelectedImage, this));

   // Initialize the histogram dialog variables.
	// Note that the boolean variables are defined as Boolean in HistogramDialogInitialize and bool
	// in this linux routine. Boolean and bool are not the same size. One needs to make an intermediate
	// variable to handle this difference.
	
	Boolean computeOnlyMinMaxFlag,
				listHistogramSummaryFlag,
				listHistogramValuesFlag,
				includeEmptyBinsFlag,
				textWindowFlag,
				diskFileFlag;

   HistogramDialogInitialize(dialogPtr,
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

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_Method);
   //comboBoxPtr->SetItemData(0, kStoredInMemory);
   //comboBoxPtr->SetItemData(1, kComputeFromImage);
   //comboBoxPtr->SetItemData(2, kReadFromSupportFile);

   // Only allow for computation for thematic type images.

   if (m_fileInfoPtr->thematicType) {
      comboBoxPtr->Delete(2);
      comboBoxPtr->Delete(0);

      m_histogramMethod = 0;

   }// end "if (m_fileInfoPtr->thematicType)"

   else // !fileInfoPtr->thematicType
   {
      if (!histogramInMemoryAvailableFlag) {
         comboBoxPtr->Delete(0);
         m_histogramMethod--;

      } // end "if (!histogramInMemoryAvailableFlag)"

   } // end "else		// !fileInfoPtr->thematicType"

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

   SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);

   if (CMHistogramSpecsDlg::TransferDataToWindow())
      // if (UpdateData(FALSE))
      PositionDialogWindow();

   //    return FALSE; // return TRUE  unless you set the focus to a control

} // end "OnInitDialog"


void CMHistogramSpecsDlg::OnListHistogram(wxCommandEvent& event) {
   UpdateListAndEmptyBinsDialogBoxes();

} // end "OnListHistogram"


void CMHistogramSpecsDlg::OnSelendokColumnLineFormat(wxCommandEvent& event) {
   UpdateListAndEmptyBinsDialogBoxes();

} // end "OnSelendokColumnLineFormat"


void CMHistogramSpecsDlg::OnSelendokChannels(wxCommandEvent& event) {
   Boolean lineFormatHistFlag;


   HandleChannelsMenu(IDC_ChannelCombo,
           kNoTransformation,
           (SInt16) gImageWindowInfoPtr->totalNumberChannels,
           kImageChannelType,
           TRUE);

   m_allChannelsAtOnceFlag = HistogramDialogUpdateAllChannelsAtOnceFlag(
           this,
           m_localActiveNumberFeatures,
           &lineFormatHistFlag);

   m_columnsLinesFormat = 0;
   if (lineFormatHistFlag)
      m_columnsLinesFormat = 1;

} // end "OnSelendokChannels"


//void CMHistogramSpecsDlg::OnSelendokChannelComboDropDown(wxCommandEvent& event){
//    wxComboBox* classcombo = (wxComboBox *) FindWindow(IDC_ChannelCombo);
//    classcombo->SetSelection(0);    
//}


void CMHistogramSpecsDlg::OnTextWindow(wxCommandEvent& event) {
   CheckWriteResultsToSettings();

} // end "OnTextWindow"


void CMHistogramSpecsDlg::OnSelendokMethod(wxCommandEvent& event)
{
   wxComboBox* comboBoxPtr;
   wxString prvsel, cursel;
   int localMethod;

   SInt16 previousMethodCode;

   comboBoxPtr = (wxComboBox*) FindWindow(IDC_Method);

   prvsel = comboBoxPtr->GetString(m_histogramMethod);
   previousMethodCode = this->GetItemCode(prvsel);
   
   localMethod = comboBoxPtr->GetSelection();
   //SInt64 localmethod = (SInt64)((int*)comboBoxPtr->GetClientData(localMethod));
   m_histogramMethodCode = (SInt16)localMethod +1 ;
//   cursel = comboBoxPtr->GetString(localMethod);
//   m_histogramMethodCode = this->GetItemCode(cursel);

   m_histogramMethodCode = HistogramDialogHandleMethod(
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
      SetEntireImageButtons(
           NULL,
           m_LineStart,
           m_LineEnd,
           m_ColumnStart,
           m_ColumnEnd);
		}

   // Set the proper method item.

   if ((comboBoxPtr->GetString(0)).Cmp(wxT("From default statistics file")) == 0)
      m_histogramMethod = m_histogramMethodCode - 1;

   else // !comboBoxPtr->GetItemData(0) == kStoredInMemory
      m_histogramMethod = m_histogramMethodCode - 2;

   comboBoxPtr->SetSelection(m_histogramMethod);

   if (m_updateListHistogramItemsFlag)
		{
      HistogramDialogSetListAndEmptyBins(this,
              m_histogramMethodCode,
              m_computeOnlyMinMaxFlag,
              m_listHistogramValuesFlag,
              m_includeEmptyBinsFlag,
              (m_columnsLinesFormat == 1));

		} // end "if (m_updateListHistogramItemsFlag)"

} // end "OnSelendokMethod"


//-----------------------------------------------------------------------------
//										Copyright (1988-2015)
//                        c Purdue Research Foundation
//										All rights reserved.
//
//	Function name:		void ShowAreaHistogramItems
//
//	Software purpose:	The purpose of this routine is to show area selected
//							dialog items.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 11/23/1999	

void CMHistogramSpecsDlg::ShowAreaHistogramItems(void)
{
   HistogramDialogShowAreaItems (this);

   SetEntireImageButtons(
           NULL,
           m_LineStart,
           m_LineEnd,
           m_ColumnStart,
           m_ColumnEnd);

} // end "ShowAreaHistogramItems"


bool CMHistogramSpecsDlg::TransferDataFromWindow()
{
	SInt16		returnCode = 0;
					
					
   wxComboBox* channelcb = (wxComboBox *) FindWindow(IDC_ChannelCombo);
   wxComboBox* methodcb = (wxComboBox *) FindWindow(IDC_Method);
   wxComboBox* linecolcb = (wxComboBox *) FindWindow(IDC_ColumnLineFormat);

   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);

   wxCheckBox* cponly = (wxCheckBox*) FindWindow(IDC_ComputeOnly);
   wxCheckBox* emptybins = (wxCheckBox*) FindWindow(IDC_EmptyBins);
   wxCheckBox* listh = (wxCheckBox*) FindWindow(IDC_ListHistogram);
   wxCheckBox* listsumm = (wxCheckBox*) FindWindow(IDC_ListSummary);
   wxCheckBox* textw = (wxCheckBox*) FindWindow(IDC_TextWindow);
   wxCheckBox* diskf = (wxCheckBox*) FindWindow(IDC_DiskFile);

   m_channelSelection = channelcb->GetSelection();
   m_histogramMethod = methodcb->GetSelection();
   m_columnsLinesFormat = linecolcb->GetSelection();

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

   m_computeOnlyMinMaxFlag = cponly->GetValue();
   m_includeEmptyBinsFlag = emptybins->GetValue();
   m_listHistogramValuesFlag = listh->GetValue();
   m_listHistogramSummaryFlag = listsumm->GetValue();
   m_textWindowFlag = textw->GetValue();
   m_diskFileFlag = diskf->GetValue();

   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
	if (m_histogramMethodCode == kComputeFromImage)
		returnCode = VerifyLineColumnValues(
							  IDC_LineStart,
							  IDC_ColumnStart,
							  true);
						  
   return (returnCode == 0);
	
}	// end "TransferDataFromWindow"


bool CMHistogramSpecsDlg::TransferDataToWindow ()

{
   wxComboBox* channelcb = (wxComboBox *) FindWindow(IDC_ChannelCombo);
   wxComboBox* methodcb = (wxComboBox *) FindWindow(IDC_Method);
   wxComboBox* linecolcb = (wxComboBox *) FindWindow(IDC_ColumnLineFormat);

   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);

   wxCheckBox* cponly = (wxCheckBox*) FindWindow(IDC_ComputeOnly);
   wxCheckBox* emptybins = (wxCheckBox*) FindWindow(IDC_EmptyBins);
   wxCheckBox* listh = (wxCheckBox*) FindWindow(IDC_ListHistogram);
   wxCheckBox* listsumm = (wxCheckBox*) FindWindow(IDC_ListSummary);
   wxCheckBox* textw = (wxCheckBox*) FindWindow(IDC_TextWindow);
   wxCheckBox* diskf = (wxCheckBox*) FindWindow(IDC_DiskFile);

   wxStaticText* fname = (wxStaticText *) FindWindow(IDC_FileName);
   wxStaticText* dfname = (wxStaticText *) FindWindow(IDC_DefaultFile);

   channelcb->SetSelection(m_channelSelection);
   methodcb->SetSelection(m_histogramMethod);
   linecolcb->SetSelection(m_columnsLinesFormat);

   cponly->SetValue(m_computeOnlyMinMaxFlag);
   emptybins->SetValue(m_includeEmptyBinsFlag);
   listh->SetValue(m_listHistogramValuesFlag);
   listsumm->SetValue(m_listHistogramSummaryFlag);
   textw->SetValue(m_textWindowFlag);
   diskf->SetValue(m_diskFileFlag);
/*
   c_end->Clear();
   c_inter->Clear();
   c_start->Clear();
   l_end->Clear();
   l_inter->Clear();
   l_start->Clear();

   *c_end << m_ColumnEnd;
   *c_inter << m_ColumnInterval;
   *c_start << m_ColumnStart;
   *l_end << m_LineEnd;
   *l_inter << m_LineInterval;
   *l_start << m_LineStart;
*/
   c_end->ChangeValue(wxString::Format(wxT("%i"), m_ColumnEnd));
   c_inter->ChangeValue(wxString::Format(wxT("%i"), m_ColumnInterval));
   c_start->ChangeValue(wxString::Format(wxT("%i"), m_ColumnStart));
   l_end->ChangeValue(wxString::Format(wxT("%i"), m_LineEnd));
   l_inter->ChangeValue(wxString::Format(wxT("%i"), m_LineInterval));
   l_start->ChangeValue(wxString::Format(wxT("%i"), m_LineStart));
   
   fname->SetLabel(m_fileName);
   dfname->SetLabel(m_supportFileName);
   return true;
}


//-----------------------------------------------------------------------------
//				 Copyright (1988-2015)
//                c Purdue Research Foundation
//				All rights reserved.
//
//	Function name:		void UpdateListAndEmptyBinsDialogBoxes                
//
//	Software purpose:	The purpose of this routine is to update the list and
//				empty bin dialog check boxes in the histogram
//				dialog window. This routine is called after a dialog
//				item has been changed
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:			HistogramDialog in histogram.c
//
//	Coded By:			Larry L. Biehl					Date:	06/28/1995
//	Revised By:			Larry L. Biehl					Date: 06/28/1995

void CMHistogramSpecsDlg::UpdateListAndEmptyBinsDialogBoxes(void) {
   // Get current settings for:
   //		compute only min, max, mean, etc
   //		list histogram values
   //		include empty bins
   //		columns or lines format

   m_computeOnlyMinMaxFlag = ((wxCheckBox *) FindWindow(IDC_ComputeOnly))->GetValue();

   m_listHistogramValuesFlag = ((wxCheckBox *) FindWindow(IDC_ListHistogram))->GetValue();

   m_columnsLinesFormat = ((wxComboBox *) FindWindow(IDC_ColumnLineFormat))->GetSelection();

   HistogramDialogSetListAndEmptyBins(this,
           m_histogramMethodCode,
           m_computeOnlyMinMaxFlag,
           m_listHistogramValuesFlag,
           m_includeEmptyBinsFlag,
           (m_columnsLinesFormat == 1));

   CheckWriteResultsToSettings();

}	// end "UpdateListAndEmptyBinsDialogBoxes"

