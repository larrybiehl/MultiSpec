// LFileFormatDialog.cpp : implementation file
//
//	Revised by Larry Biehl on 11/30/2018
/*		
	int numberChars = sprintf ((char*)&gTextString3,
												" LFormDlg:OnPaint (m_hdfDataSetSelection): %ld%s", 
												m_hdfDataSetSelection,
												gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/	

#include "SMultiSpec.h"
#include "wx/valgen.h"
#include "LFileFormatDialog.h"
#include "LImageView.h"
#include "CImageWindow.h"
#include "wx/window.h"

extern void FileSpecificationDialogInitialize (
        DialogPtr dialogPtr,
        Handle fileInfoHandle,
        Handle windowInfoHandle,
        FileInfoPtr* fileInfoPtrPtr,
        WindowInfoPtr* windowInfoPtrPtr,
        UInt32* numberLinesPtr,
        UInt32* numberColumnsPtr,
        UInt32* numberChannelsPtr,
        SInt32* startLinePtr,
        SInt32* startColumnPtr,
        UInt32* numberHeaderBytesPtr,
        UInt32* numberTrailerBytesPtr,
        UInt32* numberPreLineBytesPtr,
        UInt32* numberPostLineBytesPtr,
        UInt32* numberPreChannelBytesPtr,
        UInt32* numberPostChannelBytesPtr,
        UInt32* blockHeightPtr,
        UInt32* blockWidthPtr,
        SInt16* bandInterleaveSelectionPtr,
        SInt16* dataValueTypeSelectionPtr,
        SInt16* eightBitsPerDataSelectionPtr,
        Boolean* swapBytesFlagPtr,
        Boolean* signedDataFlagPtr,
        Boolean* linesBottomToTopFlagPtr,
        Boolean* fillDataValueExistsFlagPtr,
        double* fillDataValuePtr,
        SInt16* hdfDataSetPtr,
        SInt16* collapseClassesSelecctionPtr,
        Boolean* callGetHDFLineFlagPtr);

extern Boolean FileSpecificationDialogOK(
        DialogPtr dialogPtr,
        FileInfoPtr fileInfoPtr,
        Handle fileInfoHandle,
        Handle* newMapInformationHandlePtr,
        Handle* newHfaHandlePtr,
        Handle* newChannelToHdfDataSetHandlePtr,
        WindowInfoPtr windowInfoPtr,
        Handle windowInfoHandle,
        UInt32 numberLines,
        UInt32 numberColumns,
        UInt32 numberChannels,
        SInt32 startLine,
        SInt32 startColumn,
        UInt32 numberHeaderBytes,
        UInt32 numberTrailerBytes,
        UInt32 numberPreLineBytes,
        UInt32 numberPostLineBytes,
        UInt32 numberPreChannelBytes,
        UInt32 numberPostChannelBytes,
        UInt32 blockHeight,
        UInt32 blockWidth,
        Boolean forceGroupTableUpdateFlag,
        SInt16 bandInterleaveSelection,
        SInt16 dataValueTypeSelection,
        Boolean swapBytesFlag,
        Boolean signedDataFlag,
        Boolean computeNumberClassesFlag,
        Boolean linesBottomToTopFlag,
        Boolean fillDataValueExistsFlag,
        double fillDataValue,
        SInt16 hdfDataSetSelection,
        UInt32 hdfDataSetIndex,
        SInt16 collapseClassesSelecction,
        UInt16 dataCompressionCode,
        SInt16 gdalDataTypeCode,
        Boolean callGetHDFLineFlag);

extern void FileSpecificationDialogSetInterleaveItems(
        DialogPtr dialogPtr,
        SInt16 bandInterleaveSelection,
        Boolean blockedFlag);

extern SInt16 FileSpecificationDialogSetHDFValues(
        DialogPtr dialogPtr,
        FileInfoPtr fileInfoPtr,
        SInt16 hdfDataSetSelection,
        Handle okHandle,
        Boolean shiftKeyDownFlag,
        Handle* mapInformationHandlePtr,
        Handle* hfaHandlePtr,
        Handle* newChannelToHdfDataSetHandlePtr,
        SInt16* bandInterleaveSelectionPtr,
        SInt16* dataValueTypeSelectionPtr,
        UInt16* dataCompressionCodePtr,
        SInt16* gdalDataTypeCodePtr,
        Boolean* callGetHDFLineFlagPtr);

extern SInt16 FileSpecificationDialogGetNumberBytes(
        SInt16 dataTypeSelection);

// === Static Member Variable ===                      

// CMImageWindow*		CMFileFormatSpecsDlg::sImageWindowCPtr = NULL;							
// FileInfoPtr			CMFileFormatSpecsDlg::sFileInfoPtr = NULL;
// WindowInfoPtr		CMFileFormatSpecsDlg::sWindowInfoPtr = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMFileFormatSpecsDlg dialog

//CMFileFormatSpecsDlg::CMFileFormatSpecsDlg(){    
//}

CMFileFormatSpecsDlg::~CMFileFormatSpecsDlg() {
	free(m_menuclientdata);
}

CMFileFormatSpecsDlg::CMFileFormatSpecsDlg(wxWindow* pParent, wxWindowID id, const wxString& title /*=NULL*/)
: CMDialog(CMFileFormatSpecsDlg::IDD, pParent, title) {
    //{{AFX_DATA_INIT(CMFileFormatSpecsDlg)
    m_numberLines = 512;
    m_numberColumns = 614;
    m_postChannelBytes = 0;
    m_headerBytes = 128;
    m_postLineBytes = 0;
    m_preChannelBytes = 0;
    m_preLineBytes = 0;
    m_numberChannels = 0;
    m_startColumnNumber = 1;
    m_startLineNumber = 1;
    m_swapBytesFlag = FALSE;
    m_linesBottomToTopFlag = FALSE;
    m_trailerBytes = 0;
    m_imageName = "";
    m_computeNumClasses = FALSE;
    m_dataValueType = 0;
    m_bandInterleave = 0;
    m_hdfDataSetSelection = -1;
    m_collapseClassSelection = -1;
    //}}AFX_DATA_INIT

    m_blockHeight = 0;
    m_blockWidth = 0;

    m_fileInfoHandle = NULL;
    m_windowInfoHandle = NULL;
    m_newChannelToHdfDataSetHandle = NULL;

    m_fileInfoPtr = NULL;
    m_windowInfoPtr = NULL;
    m_menuclientdata = NULL;

    m_dataCompressionCode = 0;

    m_maxNumberChannelsClasses = 0;

    m_dataSetIndex = 1;

    m_eightBitsPerDataSelection = 0;
    m_forceGroupTableUpdateFlag = FALSE;
    m_initializedFlag = CMDialog::m_initializedFlag;
	 
			// m_onShowCalledFlag is used to account for differences on wxWidgets message passing on mygeohub (debian)
			// and mdw linux box (redhat). OnShow message is called after OnInitDialog on debian but vice versa
			// on redhat. The reason may not be the operating system, but hopefully this accounts for differences
			// in where the code is run.
	 m_onShowCalledFlag = FALSE;

    CreateControls();
}


void CMFileFormatSpecsDlg::CreateControls ()

{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	//wxBoxSizer* bSizer47;
	bSizer47 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer48;
	bSizer48 = new wxBoxSizer(wxHORIZONTAL);

	//bSizer48->Add(20, 5, 0, wxEXPAND, 5);

	wxStaticText* staticText59 = new wxStaticText(this, wxID_ANY, wxT("For: "), wxDefaultPosition, wxDefaultSize, 0);
	staticText59->Wrap(-1);
	bSizer48->Add(staticText59, 0);

	m_staticText60 = new wxStaticText(this, IDC_ImageName, wxT("Image File Name"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText60->Wrap(-1);
   SetUpToolTip(m_staticText60, IDS_ToolTip277);
	//bSizer48->Add (m_staticText60, 0, wxALL, 5);
	bSizer48->Add (m_staticText60, wxSizerFlags(0).Border(wxBOTTOM,5));


	//bSizer47->Add (bSizer48, 0, wxEXPAND, 5);
	bSizer47->Add (bSizer48, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT,12));

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Size Specifications")), wxHORIZONTAL);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer(5, 2, 0, 0);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText89 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Number of Lines"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText89->Wrap(-1);
   SetUpToolTip(m_staticText89, IDS_ToolTip274);
	//fgSizer1->Add (m_staticText89, 0, wxALL, 5);
	fgSizer1->Add (m_staticText89, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl41 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_NumberLines, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	//m_textCtrl41->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_numberLinesString));
   SetUpToolTip(m_textCtrl41, IDS_ToolTip274);
	m_textCtrl41->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
	fgSizer1->Add(m_textCtrl41, 0, wxALL, 5);

	m_staticText90 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Number of Columns"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText90->Wrap(-1);
   SetUpToolTip(m_staticText90, IDS_ToolTip275);
	//fgSizer1->Add (m_staticText90, 0, wxALL, 5);
	fgSizer1->Add (m_staticText90, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl42 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_NumberColumns, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl42, IDS_ToolTip275);
	m_textCtrl42->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
	fgSizer1->Add(m_textCtrl42, 0, wxALL, 5);

	m_staticText91 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_NumberChannelsClassesPrompt, wxT("Number of Channels"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText91->Wrap(-1);
   SetUpToolTip(m_staticText91, IDS_ToolTip276);
	//fgSizer1->Add (m_staticText91, 0, wxALL, 5);
	fgSizer1->Add (m_staticText91, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl43 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_NumberChannels, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl43, IDS_ToolTip276);
	m_textCtrl43->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
	fgSizer1->Add(m_textCtrl43, 0, wxALL, 5);

	m_staticText92 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Start Line Number"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText92->Wrap(-1);
   SetUpToolTip(m_staticText92, IDS_ToolTip278);
	//fgSizer1->Add(m_staticText92, 0, wxALL, 5);
	fgSizer1->Add (m_staticText92, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl44 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_StartLineNumber, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl44->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl44, IDS_ToolTip278);
	fgSizer1->Add(m_textCtrl44, 0, wxALL, 5);

	m_staticText93 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Start Column Number"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText93->Wrap(-1);
   SetUpToolTip(m_staticText93, IDS_ToolTip279);
	//fgSizer1->Add(m_staticText93, 0, wxALL, 5);
	fgSizer1->Add (m_staticText93, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl45 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_StartColumnNumber, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl45->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl45, IDS_ToolTip279);
	fgSizer1->Add(m_textCtrl45, 0, wxALL, 5);

	//sbSizer3->Add(fgSizer1, 0, wxALIGN_CENTER | wxEXPAND, 5);
   sbSizer3->Add(fgSizer1, 0, wxEXPAND, 5);
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer(8, 2, 0, 0);
	fgSizer2->SetFlexibleDirection(wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText94 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_HeaderPrompt, wxT("File Header Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText94->Wrap(-1);
   SetUpToolTip(m_staticText94, IDS_ToolTip280);
	//fgSizer2->Add(m_staticText94, 0, wxALL, 5);
	fgSizer2->Add (m_staticText94, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl46 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_HeaderBytes, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl46->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl46, IDS_ToolTip280);
	fgSizer2->Add(m_textCtrl46, 0, wxALL, 5);

	m_staticText95 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_TrailerPrompt, wxT("File Trailer Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText95->Wrap(-1);
   SetUpToolTip(m_staticText95, IDS_ToolTip281);
	//fgSizer2->Add(m_staticText95, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText95, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl47 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_TrailerBytes, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl47->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl47, IDS_ToolTip281);
	fgSizer2->Add(m_textCtrl47, 0, wxALL, 5);

	m_staticText100 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_BlockHeightPrompt, wxT("\"Block Height (lines)"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText100->Wrap(-1);
	//fgSizer2->Add(m_staticText100, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText100, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_staticText101 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_BlockHeight, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText101->Wrap(-1);
	fgSizer2->Add(m_staticText101, 0, wxALL, 5);

	m_staticText102 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_BlockWidthPrompt, wxT("Block Width (cols.)"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText102->Wrap(-1);
	//fgSizer2->Add(m_staticText102, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText102, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_staticText103 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_BlockWidth, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText103->Wrap(-1);
	fgSizer2->Add(m_staticText103, 0, wxALL, 5);

	m_staticText96 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_PreLinePrompt, wxT("Preline Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText96->Wrap(-1);
   SetUpToolTip(m_staticText96, IDS_ToolTip282);
	//fgSizer2->Add(m_staticText96, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText96, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl48 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_PreLineBytes, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl48->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl48, IDS_ToolTip282);
	fgSizer2->Add(m_textCtrl48, 0, wxALL, 5);

	m_staticText97 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_PostLinePrompt, wxT("Postline Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText97->Wrap(-1);
   SetUpToolTip(m_staticText97, IDS_ToolTip283);
	//fgSizer2->Add(m_staticText97, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText97, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl49 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_PostLineBytes, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl49->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl49, IDS_ToolTip283);
	fgSizer2->Add(m_textCtrl49, 0, wxALL, 5);

	m_staticText98 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_PreChannelPrompt, wxT("Prechannel Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText98->Wrap(-1);
   SetUpToolTip(m_staticText98, IDS_ToolTip284);
	//fgSizer2->Add(m_staticText98, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText98, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl50 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_PreChannelBytes, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl50->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl50, IDS_ToolTip284);
	fgSizer2->Add(m_textCtrl50, 0, wxALL, 5);

	m_staticText99 = new wxStaticText(sbSizer3->GetStaticBox(), IDC_PostChannelPrompt, wxT("Postchannel Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText99->Wrap(-1);
   SetUpToolTip(m_staticText99, IDS_ToolTip285);
	//fgSizer2->Add(m_staticText99, 0, wxALIGN_CENTER | wxALL, 5);
	fgSizer2->Add (m_staticText99, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL,5));

	m_textCtrl51 = new wxTextCtrl(sbSizer3->GetStaticBox(), IDC_PostChannelBytes, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_textCtrl51->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
   SetUpToolTip(m_textCtrl51, IDS_ToolTip285);
	fgSizer2->Add(m_textCtrl51, 0, wxALL, 5);

	//Set TAB Order
	m_textCtrl42->MoveAfterInTabOrder(m_textCtrl41);
	m_textCtrl43->MoveAfterInTabOrder(m_textCtrl42);
	m_textCtrl44->MoveAfterInTabOrder(m_textCtrl43);
	m_textCtrl45->MoveAfterInTabOrder(m_textCtrl44);
	m_textCtrl46->MoveAfterInTabOrder(m_textCtrl45);
	m_textCtrl47->MoveAfterInTabOrder(m_textCtrl46);
	m_textCtrl48->MoveAfterInTabOrder(m_textCtrl47);
	m_textCtrl49->MoveAfterInTabOrder(m_textCtrl48);
	m_textCtrl50->MoveAfterInTabOrder(m_textCtrl49);
	m_textCtrl51->MoveAfterInTabOrder(m_textCtrl50);

	//sbSizer3->Add(fgSizer2, 0, wxALIGN_CENTER | wxEXPAND, 5);
   sbSizer3->Add(fgSizer2, 0, wxEXPAND, 5);

	//bSizer47->Add (sbSizer3, 0, wxALL | wxEXPAND, 5);
	bSizer47->Add (sbSizer3, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));

	wxBoxSizer* bSizer79;
	bSizer79 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer80;
	bSizer80 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText80 = new wxStaticText(this, wxID_ANY, wxT("Band Interleave Format:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText80->Wrap(-1);
   SetUpToolTip(m_staticText80, IDS_ToolTip286);
	//bSizer80->Add (m_staticText80, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer80->Add (m_staticText80, wxSizerFlags(0).Center().Border(wxALL,5));

	m_comboBox5 = new wxComboBox (this,
											IDC_BandInterleavePopUp,
											wxT("BIL-Band Interleaved by Line"),
      									wxDefaultPosition,
      									wxSize (230, -1),
      									0,
      									NULL,
      									wxTAB_TRAVERSAL | wxCB_READONLY);  // wxTAB_TRAVERSAL | wxVSCROLL | wxCB_READONLY
	m_comboBox5->Append(wxT("BIL-Band Interleaved by Line"));
	m_comboBox5->Append(wxT("BSQ-Band Sequential"));
	m_comboBox5->Append(wxT("BIS-Band Interleaved by Sample"));
	m_comboBox5->Append(wxT("Band Non-sequential"));
	m_comboBox5->Append(wxT("Band Non-sequential Blocked"));
	m_comboBox5->Append(wxT("Band Interleave by Block"));
   SetUpToolTip(m_comboBox5, IDS_ToolTip286);
	//bSizer80->Add (m_comboBox5, 0, wxALL, 5);
	bSizer80->Add (m_comboBox5, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

		
	//bSizer79->Add (bSizer80, 0, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer80, wxSizerFlags(0).Expand().Border(wxTOP,5));

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText81 = new wxStaticText(this, wxID_ANY, wxT("Data Value Type:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText81->Wrap(-1);
   SetUpToolTip(m_staticText81, IDS_ToolTip287);
	//bSizer81->Add(m_staticText81, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer81->Add (m_staticText81, wxSizerFlags(0).Center().Border(wxALL,5));

	m_comboBox4 = new wxComboBox (this,
											IDC_DataValueTypePopUp,
											wxT("4-bit Unsigned Integer"),
      									wxDefaultPosition,
      									wxSize (175, -1),
      									0,
      									NULL,
      									wxTAB_TRAVERSAL |wxCB_READONLY);	// 0 | wxTAB_TRAVERSAL | wxVSCROLL|wxCB_READONLY
	m_comboBox4->Append(wxT("4-bit Unsigned Integer"));
	m_comboBox4->Append(wxT("8-bit Signed Integer"));
	m_comboBox4->Append(wxT("8-bit Unsigned Integer"));
	m_comboBox4->Append(wxT("16-bit Signed Integer"));
	m_comboBox4->Append(wxT("16-bit Unsigned Integer"));
	m_comboBox4->Append(wxT("32-bit Signed Integer"));
	m_comboBox4->Append(wxT("32-bit Unsigned Integer"));
	m_comboBox4->Append(wxT("32-bit Real"));
	m_comboBox4->Append(wxT("64-bit Real"));
   SetUpToolTip(m_comboBox4, IDS_ToolTip287);
	//bSizer81->Add(m_comboBox4, 0, wxALL, 5);
	bSizer81->Add (m_comboBox4, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,5));

	m_checkBox2 = new wxCheckBox(this, IDC_SwapBytes, wxT("Swap Bytes"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkBox2->SetValue(true);
   SetUpToolTip(m_checkBox2, IDS_ToolTip288);
	//bSizer81->Add(m_checkBox2, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer81->Add (m_checkBox2, wxSizerFlags(0).Center().Border(wxALL,5));


	//bSizer79->Add(bSizer81, 0, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer81, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer(wxHORIZONTAL);

	m_checkBox3 = new wxCheckBox(this, IDC_LinesBottomToTop, wxT("Treat Lines from Bottom to Top"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkBox3->SetValue(true);
   SetUpToolTip(m_checkBox3, IDS_ToolTip70);
	bSizer82->Add(m_checkBox3, 0, wxALL, 5);


	//bSizer79->Add(bSizer82, 0, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer82, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer83;
	bSizer83 = new wxBoxSizer(wxHORIZONTAL);

	m_checkBox4 = new wxCheckBox(this, IDC_FillDataValueExists, wxT("Data Fill Value:"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox4, IDS_ToolTip273);
	bSizer83->Add(m_checkBox4, 0, wxALIGN_CENTER | wxALL, 5);

	m_textCtrl35 = new wxTextCtrl(this, IDC_FillDataValue, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_textCtrl35, IDS_ToolTip273);
	bSizer83->Add(m_textCtrl35, 0, wxALL, 5);


	//bSizer79->Add(bSizer83, 0, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer83, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer84;
	bSizer84 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText82 = new wxStaticText(this, IDC_CollapseClassPrompt, wxT("Collapse Classes:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText82->Wrap(-1);
	bSizer84->Add(m_staticText82, 0, wxALIGN_CENTER | wxALL, 5);

	m_comboBox6 = new wxComboBox(this, IDC_CollapseClass, wxT("Yes"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0| wxCB_READONLY);
	m_comboBox6->Append(wxT("Yes"));
	m_comboBox6->Append(wxT("No"));
	m_comboBox6->Append(wxT("To Highest Value"));
	bSizer84->Add(m_comboBox6, 0, wxALL, 5);


	//bSizer79->Add(bSizer84, 0, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer84, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer(wxHORIZONTAL);


	bSizer85->Add(50, 0, 0, wxEXPAND, 5);

	m_checkBox5 = new wxCheckBox(this, IDC_ComputeNumClasses, wxT("Compute Number of Classes"), wxDefaultPosition, wxDefaultSize, 0);
	//wxButton* buttonBox5 = new wxButton(this, IDC_ComputeNumClasses, wxT("Determine Number of Classes"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer85->Add(m_checkBox5, 0, wxALL, 5);


	//bSizer79->Add(bSizer85, 0, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer85, wxSizerFlags(0).Expand());

	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText83 = new wxStaticText(this, IDC_HDFDataSetPrompt, wxT("HDF Data Set:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText83->Wrap(-1);
   SetUpToolTip(m_staticText83, IDS_ToolTip289);
	bSizer86->Add(m_staticText83, 0, wxALIGN_CENTER | wxALL, 5);

	m_comboBox7 = new wxComboBox (this,
											IDC_HDFDataSet,
											wxT(""),
											wxDefaultPosition,
											wxSize (350, -1),
											0,
											NULL,
											wxCB_READONLY);
   SetUpToolTip(m_comboBox7, IDS_ToolTip289);
	bSizer86->Add(m_comboBox7, 0, wxALL, 5);

	m_button7 = new wxButton(this, IDC_HDFDataSetHelp, wxT("?"), wxDefaultPosition, wxSize (25, 25), 0);
	bSizer86->Add(m_button7, 0, wxALL, 5);


	//bSizer79->Add(bSizer86, 1, wxALL | wxEXPAND, 5);
	bSizer79->Add (bSizer86, wxSizerFlags(0).Expand());
	/*
	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer(wxHORIZONTAL);

	m_button8 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer87->Add (m_button8, 0, wxALL, 5);
	bSizer87->Add (m_button8, wxSizerFlags(0).Border(wxRIGHT,6));

	m_button9 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	//bSizer87->Add (m_button9, 0, wxALL, 5);
	bSizer87->Add (m_button9, wxSizerFlags(0));

	bSizer79->Add(bSizer87, 0, wxALIGN_RIGHT);
	*/
	bSizer47->Add (bSizer79, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer47);

	this->SetSizer (bSizer47);
	this->Layout ();
	//this->Centre(wxBOTH);
	
}		// end "CreateControls"


BEGIN_EVENT_TABLE(CMFileFormatSpecsDlg, CMDialog)

EVT_INIT_DIALOG(CMFileFormatSpecsDlg::OnInitDialog)
EVT_COMBOBOX(IDC_BandInterleavePopUp, CMFileFormatSpecsDlg::OnSelendokBandInterleave)
EVT_COMBOBOX(IDC_DataValueTypePopUp, CMFileFormatSpecsDlg::OnSelendokDataValueType)
//EVT_CHECKBOX(IDC_DetermineNumClasses, CMFileFormatSpecsDlg::OnDetermineNumClasses)
EVT_COMBOBOX(IDC_HDFDataSet, CMFileFormatSpecsDlg::OnSelendokHDFDataSet)
//EVT_PAINT(CMFileFormatSpecsDlg::OnPaint)
//EVT_ACTIVATE(CMFileFormatSpecsDlg::OnActivate)
EVT_BUTTON(IDC_HDFDataSetHelp, CMFileFormatSpecsDlg::OnBnClickedHdfdatasethelp)
EVT_BUTTON(IDC_LinesBottomToTop, CMFileFormatSpecsDlg::OnBnClickedLinebottomtotop)
EVT_CHECKBOX(IDC_FillDataValueExists, CMFileFormatSpecsDlg::OnBnClickedFillDataValueExists)
//EVT_CHAR_HOOK(CMFileFormatSpecsDlg::OnButtonPress)

EVT_SHOW(CMFileFormatSpecsDlg::OnShow)

END_EVENT_TABLE()


//-----------------------------------------------------------------------------
//								 Copyright (1988-2016)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the file
//							specification dialog box to the user and copy the
//							revised back to the file information structure if
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
//	Coded By:			Larry L. Biehl			Date: 05/23/1995
//	Revised By:			Larry L. Biehl			Date: 02/22/2012	

Boolean
CMFileFormatSpecsDlg::DoDialog(
        Handle fileInfoHandle,
        Handle windowInfoHandle,
        Handle* mapInformationHandlePtr,
        Handle* hfaHandlePtr,
        Handle* newChannelToHdfDataSetHandlePtr,
        Boolean* parameterChangedFlagPtr) {
    SInt16 returnCode;

    Boolean continueFlag = FALSE,
            changedFlag,
            classesChanged = FALSE,
            parameterChangedFlag = FALSE;


    // Make sure intialization has been completed.

    if (!m_initializedFlag)
        return (FALSE);

    m_fileInfoHandle = fileInfoHandle;
    m_windowInfoHandle = windowInfoHandle;

    m_mapInformationHandle = *mapInformationHandlePtr;
    m_hfaHandle = *hfaHandlePtr;
    m_newChannelToHdfDataSetHandle = *newChannelToHdfDataSetHandlePtr;

    returnCode = ShowModal();

    if (returnCode == wxID_OK) {
        changedFlag = FALSE;
        continueFlag = TRUE;

        parameterChangedFlag = FileSpecificationDialogOK(
                this,
                m_fileInfoPtr,
                fileInfoHandle,
                &m_mapInformationHandle,
                &m_hfaHandle,
                &m_newChannelToHdfDataSetHandle,
                m_windowInfoPtr,
                windowInfoHandle,
                m_numberLines,
                m_numberColumns,
                m_numberChannels,
                m_startLineNumber,
                m_startColumnNumber,
                m_headerBytes,
                m_trailerBytes,
                m_preLineBytes,
                m_postLineBytes,
                m_preChannelBytes,
                m_postChannelBytes,
                m_blockHeight,
                m_blockWidth,
                m_forceGroupTableUpdateFlag,
                m_bandInterleave + 1,
                m_dataValueType,
                m_swapBytesFlag,
                FALSE,
                m_linesBottomToTopFlag,
                m_computeNumClasses,
                m_fillDataValueExistsFlag,
                m_fillDataValue,
                m_hdfDataSetSelection + 1,
                m_dataSetIndex,
                m_collapseClassSelection + 1,
                m_dataCompressionCode,
                m_gdalDataTypeCode,
                m_callGetHDFLineFlag);

    } // end "if (returnCode == IDOK)"

    if (parameterChangedFlagPtr != NULL)
        *parameterChangedFlagPtr = parameterChangedFlag;

    *mapInformationHandlePtr = m_mapInformationHandle;
    *hfaHandlePtr = m_hfaHandle;
    *newChannelToHdfDataSetHandlePtr = m_newChannelToHdfDataSetHandle;

    return (continueFlag);

} // end "DoDialog"


void CMFileFormatSpecsDlg::OnBnClickedHdfdatasethelp(wxCommandEvent& event) 
{

    DisplayAlert(kErrorAlertID,
            kNoteAlert,
            kAlertStrID,
            IDS_Alert103,
            IDS_Alert104,
            NULL);
}


void CMFileFormatSpecsDlg::OnBnClickedLinebottomtotop(wxCommandEvent& event) 
{

}


void CMFileFormatSpecsDlg::OnBnClickedFillDataValueExists(wxCommandEvent& event) 
{
    wxCheckBox* fillDataValueExistsFlag = (wxCheckBox *) FindWindow(IDC_FillDataValueExists);
    m_fillDataValueExistsFlag = fillDataValueExistsFlag->GetValue();

    ShowHideDialogItem(this, IDC_FillDataValue, m_fillDataValueExistsFlag);
    this->Fit();
	 
} // end "OnBnClickedFillDataValueExists"

/*
void CMFileFormatSpecsDlg::OnButtonPress(wxKeyEvent& event) 
{
    if (event.GetKeyCode() == WXK_RETURN) {
        if(Validate() && TransferDataFromWindow())
            this->EndModal(wxID_OK);
    } else {
        event.Skip();
    }
	 
}		// end "OnButtonPress"
*/

void CMFileFormatSpecsDlg::OnDetermineNumClasses (
				wxCommandEvent&					event) 
{
		// originally argument set to void
		
	if (GetClassesFromHistogram(m_fileInfoPtr, m_collapseClassSelection + 1)) 
		{
		m_numberChannels = m_fileInfoPtr->numberClasses;
		wxTextCtrl* numberChannels = (wxTextCtrl*) FindWindow(IDC_NumberChannels);
		numberChannels->ChangeValue(wxString::Format(wxT("%i"), m_numberChannels));
		m_forceGroupTableUpdateFlag = TRUE;

		} // end "if ( GetClassesFromHistogram (gFileInfoPtr) )"

	if (m_fileInfoPtr->classesComputedFlag)
		FindWindow(IDC_DetermineNumClasses)->Enable(FALSE);

}	// end "OnDetermineNumClasses"


void CMFileFormatSpecsDlg::OnInitDialog (
				wxInitDialogEvent&				event) 
{
	FileStringPtr							fileNamePtr;

	SInt16								bandInterleave,
											collapseClassesSelection,
											dataSetSelection,
											dataValueType;

	Boolean								continueFlag,
											signedDataFlag;


	wxDialog::OnInitDialog(event);

			// Initialize dialog variables.

	FileSpecificationDialogInitialize (this,
													m_fileInfoHandle,
													m_windowInfoHandle,
													&m_fileInfoPtr,
													&m_windowInfoPtr,
													&m_numberLines,
													&m_numberColumns,
													&m_numberChannels,
													&m_startLineNumber,
													&m_startColumnNumber,
													&m_headerBytes,
													&m_trailerBytes,
													&m_preLineBytes,
													&m_postLineBytes,
													&m_preChannelBytes,
													&m_postChannelBytes,
													&m_blockHeight,
													&m_blockWidth,
													&bandInterleave,
													&dataValueType,
													(Boolean*) & m_eightBitsPerDataSelection,
													(Boolean*) & m_swapBytesFlag,
													&signedDataFlag,
													(Boolean*) & m_linesBottomToTopFlag,
													(Boolean*) & m_fillDataValueExistsFlag,
													&m_fillDataValue,
													&dataSetSelection,
													&collapseClassesSelection,
													&m_callGetHDFLineFlag);
					
	m_bandInterleave = bandInterleave - 1;
	m_dataValueType = dataValueType;
	m_hdfDataSetSelection = dataSetSelection - 1;
	m_collapseClassSelection = collapseClassesSelection - 1;
   
	m_maxNumberChannelsClasses = kMaxNumberChannels;
	if (m_fileInfoPtr->thematicType)
		m_maxNumberChannelsClasses = kMaxNumberClasses;

			//	Set the image file name.										 

	fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (m_fileInfoPtr);
	if (fileNamePtr != NULL)
		m_imageName = CString (fileNamePtr);

	if (TransferDataToWindow())
		PositionDialogWindow();
		
	if (m_fileInfoPtr->thematicType) 
		{
		wxStaticText* channelsClassesLabel = (wxStaticText*)FindWindow(IDC_NumberChannelsClassesPrompt);
		channelsClassesLabel->SetLabel(wxT("Number of Classes"));
		
		}		// end "if (m_fileInfoPtr->thematicType)"
		
	SetSizerAndFit(bSizer47);
	this->Layout();
	this->Fit();
		
	SelectDialogItemText (this, IDC_NumberLines, 0, (SInt16)INT_MAX);
	
	if (m_onShowCalledFlag)
		ShowMultipleDataSetMessage();

}	// end "OnInitDialog" 
 

void CMFileFormatSpecsDlg::OnSelendokBandInterleave(wxCommandEvent& event) 
{
			// prevent control notifications from being dispatched during 
			// UpdateData

    wxComboBox* bandInterleave = (wxComboBox *) FindWindow(IDC_BandInterleavePopUp);
    m_bandInterleave = bandInterleave->GetSelection();

    FileSpecificationDialogSetInterleaveItems(this,
            m_bandInterleave + 1,
            m_fileInfoPtr->blockedFlag);

} // end "OnSelendokBandInterleave"


void CMFileFormatSpecsDlg::OnSelendokDataValueType(wxCommandEvent& event) 
	{
	wxComboBox* dataValueType = (wxComboBox *) FindWindow(IDC_DataValueTypePopUp);
	m_dataValueType = dataValueType->GetSelection();
	m_dataValueType = m_dataValueType +1;
    //	if (m_bitsPerDataValue > m_eightBitsPerDataSelection)
	if (m_dataValueType >= k2ByteSignedIntegerMenuItem) 
		{
		FindWindow(IDC_SwapBytes)->Enable(TRUE);

		}		// end "if (m_BitsPerDataValue > m_EightBitsPerDataSelection)"

	else // m_bitsPerDataValue < k2ByteSignedIntegerMenuItem
		{
		FindWindow(IDC_SwapBytes)->Enable(FALSE);

		}		// end "if (m_BitsPerDataValue > m_EightBitsPerDataSelection)"

}		// end "OnSelendokDataValueType"


void CMFileFormatSpecsDlg::OnSelendokHDFDataSet (wxCommandEvent& event) 
	{
	int 					newDataSetSelection;

	SInt16 				bandInterleaveSelection,
							dataValueTypeSelection;

	Boolean 				shiftKeyDownFlag;
	

	wxComboBox* DataSetSelection = (wxComboBox*)FindWindow (IDC_HDFDataSet);
	newDataSetSelection = DataSetSelection->GetSelection ();

	//shiftKeyDownFlag = FALSE;
	//if (wxGetKeyState(WXK_SHIFT) & 0x8000)
	//    shiftKeyDownFlag = TRUE;
	shiftKeyDownFlag = wxGetKeyState (WXK_SHIFT);

   if (newDataSetSelection != m_hdfDataSetSelection || shiftKeyDownFlag) 
    	{
		if (FileSpecificationDialogSetHDFValues (this,
																 m_fileInfoPtr,
																 newDataSetSelection,
																 NULL,
																 shiftKeyDownFlag,
																 &m_mapInformationHandle,
																 &m_hfaHandle,
																 &m_newChannelToHdfDataSetHandle,
																 &bandInterleaveSelection,
																 &dataValueTypeSelection,
																 &m_dataCompressionCode,
																 &m_gdalDataTypeCode,
																 &m_callGetHDFLineFlag) == noErr) 
			{
			m_hdfDataSetSelection = newDataSetSelection;

			m_bandInterleave = bandInterleaveSelection - 1;
			wxComboBox* bandInterleave = (wxComboBox*)FindWindow (IDC_BandInterleavePopUp);
			bandInterleave->SetSelection (m_bandInterleave);

			m_dataValueType = dataValueTypeSelection;
			wxComboBox* dataValueType = (wxComboBox*)FindWindow (IDC_DataValueTypePopUp);
			dataValueType->SetSelection (m_dataValueType);
			
			}	// end "if (FileSpecificationDialogSetHDFValues (..."

		else	// FileSpecificationDialogSetHDFValues != noErr
			{ 
			wxComboBox* hdfDataSetSelection = (wxComboBox*)FindWindow (IDC_HDFDataSet);
			hdfDataSetSelection->SetSelection (m_hdfDataSetSelection);
			
			}

   	}	// end "if (savedDataSetSelection != m_hdfDataSetSelection)"

}	// end "OnSelendokHDFDataSet"



void CMFileFormatSpecsDlg::OnShow (wxShowEvent& event)

{
	m_onShowCalledFlag = TRUE;
	
	if (m_windowInfoPtr != NULL)
		ShowMultipleDataSetMessage();
		
}	// end "OnShow"



void CMFileFormatSpecsDlg::ShowMultipleDataSetMessage (void)

{
	SInt16				bandInterleaveSelection,
							dataValueTypeSelection;

		  // Do not allow specifications to be changed if the active				
		  // window represents the base image for an open project.					

	if (gProcessorCode == kChangeImageDescriptionProcessor &&
																m_windowInfoPtr->projectWindowFlag) 
		{
		DisplayAlert(kErrorAlertID, kCautionAlert, kAlertStrID, IDS_Alert25, 0, NULL);

		} // end "if (gProcessorCode == kChangeImageDescriptionProcessor && ..."
	  
		  // Display message about selecting a data set for the first pass through this section.
	
	if ((gProcessorCode == kOpenImageFileProcessor || 
			gProcessorCode == kRefChangeFileFormatProcessor) &&
				(m_fileInfoPtr->format == kHDF4Type || m_fileInfoPtr->format == kNETCDFType ||
					m_fileInfoPtr->format == kHDF5Type || m_fileInfoPtr->format == kNETCDF2Type || 
						m_fileInfoPtr->format == kHDF4Type2 || m_fileInfoPtr->format == kNITFType) &&
							m_fileInfoPtr->numberHdfDataSets > 1 &&
								!gHDFDataSetSelectionAlertDisplayedFlag) 
		{
		
		DisplayAlert(kErrorAlertID,
					  kNoteAlert,
					  kAlertStrID,
					  IDS_Alert103,
					  IDS_Alert104,
					  NULL);
		
		gHDFDataSetSelectionAlertDisplayedFlag = TRUE;

		} // end "if (gProcessorCode == kOpenImageFileProcessor && fileInfoPtr->...)"
	
			// Force any alert messages concerning the default hdf data set to
			// be shown.
	
	if (gProcessorCode == kOpenImageFileProcessor &&
			(m_fileInfoPtr->format == kHDF4Type || m_fileInfoPtr->format == kNETCDFType ||
				m_fileInfoPtr->format == kHDF5Type || m_fileInfoPtr->format == kNETCDF2Type || 
					m_fileInfoPtr->format == kHDF4Type2 || m_fileInfoPtr->format == kNITFType)) 
		{
	
		FileSpecificationDialogSetHDFValues(this,
					  m_fileInfoPtr,
					  m_hdfDataSetSelection,
					  NULL,
					  FALSE,
					  &m_mapInformationHandle,
					  &m_hfaHandle,
					  &m_newChannelToHdfDataSetHandle,
					  &bandInterleaveSelection,
					  &dataValueTypeSelection,
					  &m_dataCompressionCode,
					  &m_gdalDataTypeCode,
					  &m_callGetHDFLineFlag);
		
		m_bandInterleave = bandInterleaveSelection - 1;
		wxComboBox* bandInterleave = (wxComboBox *) FindWindow(IDC_BandInterleavePopUp);
		bandInterleave->SetSelection(m_bandInterleave);

		m_dataValueType = dataValueTypeSelection;
		wxComboBox* dataValueType = (wxComboBox *) FindWindow(IDC_DataValueTypePopUp);
		dataValueType->SetSelection(m_dataValueType);
		
		} // end "if (gProcessorCode == kOpenImageFileProcessor && ..."	
	 
}		// end "ShowMultipleDataSetMessage"


bool CMFileFormatSpecsDlg::TransferDataFromWindow() 
{
	SInt16		returnCode = 0;
	
	
	wxStaticText* blockHeight = (wxStaticText*) FindWindow(IDC_BlockHeight);
	wxStaticText* blockWidth = (wxStaticText*) FindWindow(IDC_BlockWidth);
	wxStaticText* imageName = (wxStaticText*) FindWindow(IDC_ImageName);

	wxTextCtrl* numberLines = (wxTextCtrl*) FindWindow(IDC_NumberLines);
	wxTextCtrl* numberColumns = (wxTextCtrl*) FindWindow(IDC_NumberColumns);
	wxTextCtrl* postChannelBytes = (wxTextCtrl*) FindWindow(IDC_PostChannelBytes);
	wxTextCtrl* headerBytes = (wxTextCtrl*) FindWindow(IDC_HeaderBytes);
	wxTextCtrl* postLineBytes = (wxTextCtrl*) FindWindow(IDC_PostLineBytes);
	wxTextCtrl* preChannelBytes = (wxTextCtrl*) FindWindow(IDC_PreChannelBytes);
	wxTextCtrl* preLineBytes = (wxTextCtrl*) FindWindow(IDC_PreLineBytes);
	wxTextCtrl* numberChannels = (wxTextCtrl*) FindWindow(IDC_NumberChannels);
	wxTextCtrl* startColumnNumber = (wxTextCtrl*) FindWindow(IDC_StartColumnNumber);
	wxTextCtrl* startLineNumber = (wxTextCtrl*) FindWindow(IDC_StartLineNumber);
	wxTextCtrl* trailerBytes = (wxTextCtrl*) FindWindow(IDC_TrailerBytes);
	wxTextCtrl* fillDataValue = (wxTextCtrl*) FindWindow(IDC_FillDataValue);

	wxCheckBox* swapBytesFlag = (wxCheckBox *) FindWindow(IDC_SwapBytes);
	wxCheckBox* linesBottomToTopFlag = (wxCheckBox *) FindWindow(IDC_LinesBottomToTop);
	wxCheckBox* fillDataValueExistsFlag = (wxCheckBox *) FindWindow(IDC_FillDataValueExists);

	wxCheckBox* computeNumClasses = (wxCheckBox *) FindWindow(IDC_ComputeNumClasses);

	wxComboBox* dataValueType = (wxComboBox *) FindWindow(IDC_DataValueTypePopUp);
	wxComboBox* bandInterleave = (wxComboBox *) FindWindow(IDC_BandInterleavePopUp);
	wxComboBox* hdfDataSetSelection = (wxComboBox *) FindWindow(IDC_HDFDataSet);
	wxComboBox* collapseClassSelection = (wxComboBox *) FindWindow(IDC_CollapseClass);

	m_numberLines = wxAtoi(numberLines->GetValue());
	m_numberColumns = wxAtoi(numberColumns->GetValue());
	m_postChannelBytes = wxAtoi(postChannelBytes->GetValue());
	m_headerBytes = wxAtoi(headerBytes->GetValue());
	m_postLineBytes = wxAtoi(postLineBytes->GetValue());
	m_preChannelBytes = wxAtoi(preChannelBytes->GetValue());
	m_numberChannels = wxAtoi(numberChannels->GetValue());
	m_startColumnNumber = wxAtoi(startColumnNumber->GetValue());
	m_startLineNumber = wxAtoi(startLineNumber->GetValue());
	m_trailerBytes = wxAtoi(trailerBytes->GetValue());

	m_fillDataValue = wxAtoi(fillDataValue->GetValue());
	wxString fillDataValue_string = fillDataValue->GetValue();
	fillDataValue_string.ToDouble(&m_fillDataValue);

	m_dataValueType = dataValueType->GetSelection();    
	m_bandInterleave = bandInterleave->GetSelection();
	m_hdfDataSetSelection = hdfDataSetSelection->GetSelection();
	m_collapseClassSelection = collapseClassSelection->GetSelection();

	m_swapBytesFlag = swapBytesFlag->GetValue();
	m_computeNumClasses = computeNumClasses->GetValue();
	m_linesBottomToTopFlag = linesBottomToTopFlag->GetValue();
	m_fillDataValueExistsFlag = fillDataValueExistsFlag->GetValue();
	
			// Verify the settings

	double sizeDifference;
	UInt32 theNum;
	SInt16 numberBytes;
	
	if (CheckMaxValue (this,
								IDC_NumberLines,
								1,
								kMaxNumberLines,
								kDisplayRangeAlert) != 1)
		returnCode = IDC_NumberLines;
		
	if (returnCode == 0) {
		if (CheckMaxValue (this,
									IDC_NumberColumns,
									1,
									kMaxNumberColumns,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_NumberColumns;
		}
		
	if (returnCode == 0) {
		if (CheckMaxValue (this,
									IDC_NumberChannels,
									1,
									m_maxNumberChannelsClasses,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_NumberChannels;
		}
		
	if (returnCode == 0) {
		if (CheckMaxValue (this,
									IDC_StartLineNumber,
									1,
									kMaxNumberLines,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_StartLineNumber;
		}
		
	if (returnCode == 0) {
		if (CheckMaxValue (this,
									IDC_StartColumnNumber,
									1,
									kMaxNumberColumns,
									kDisplayRangeAlert) != 1)
			returnCode = IDC_StartColumnNumber;
		}

	if (m_fileInfoPtr->thematicType &&
			(m_dataValueType < k4BitUnsignedIntegerMenuItem ||
			m_dataValueType > k2ByteUnsignedIntegerMenuItem))
		{
		DisplayAlert(kErrorAlertID,
				 kStopAlert,
				 kAlertStrID,
				 IDS_Alert135,
				 0,
				 NULL);
				 
		returnCode = 1;

		}	// end "if (m_fileInfoPtr->thematicType &&"

	sizeDifference = 0;
	if ((m_fileInfoPtr->format != kHDF4Type && m_fileInfoPtr->format != kNETCDFType &&
			m_fileInfoPtr->format != kHDF5Type && m_fileInfoPtr->format != kNETCDF2Type && 
			m_fileInfoPtr->format != kHDF4Type2 && m_fileInfoPtr->format != kNITFType) ||
				m_fileInfoPtr->numberHdfDataSets <= 1) 
		{
		theNum = 1;
		if (!m_fileInfoPtr->thematicType)
			theNum = m_numberChannels;

	  numberBytes = FileSpecificationDialogGetNumberBytes (m_dataValueType);

	  sizeDifference = CompareFileInfoAndFileSize (m_fileInfoPtr,
																	 m_numberLines,
																	 m_numberColumns,
																	 theNum,
																	 numberBytes,
																	 m_preLineBytes,
																	 m_postLineBytes,
																	 m_preChannelBytes,
																	 m_postChannelBytes,
																	 m_headerBytes,
																	 m_trailerBytes,
																	 m_bandInterleave + 1,
																	 (m_dataValueType == 0));

		}	// end "if (fileInfoPtr->format != kHDFType || ..."

	if (sizeDifference > 0 && SizeOfImageFileCanBeCalculated (m_fileInfoPtr))
		{
				// Display an alert.

		FileStringPtr							fileNamePtr;
		SInt16									alertReturnCode;
		
		fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileInfo (m_fileInfoPtr);
		
		MGetString (gTextString2, kAlertStrID, IDS_FileSizeLimit);
		
		sprintf ((char*)gTextString,
						 (char*)gTextString2,
						 fileNamePtr,
						 sizeDifference);
		
		alertReturnCode = DisplayAlert (kRedoDontCancelAlertID,
													 kCautionAlert,
													 kAlertStrID,
													 0,
													 0,
													 gTextString);

		if (alertReturnCode == 1)
			{
			FindWindow(IDC_NumberLines)->SetFocus();
			returnCode = IDC_NumberLines;
			
			}	// end "if (alertReturnCode == 1)"

		}	// end "if (sizeDifference > 0 && ..."

	else // sizeDifference <= 0
		{
		if (m_fileInfoPtr->hdfHandle != NULL) {
					// Get the hdf data set selection in case it is needed.

			wxComboBox* comboBoxPtr = (wxComboBox*) FindWindow(IDC_HDFDataSet);
			//m_dataSetIndex = (SInt16) comboBoxPtr->GetItemData(m_hdfDataSetSelection);
			comboBoxPtr->SetSelection(m_hdfDataSetSelection);
			m_dataSetIndex = comboBoxPtr->GetSelection();

			}	// end "if (m_fileInfoPtr->hdfHandle != NULL)"

		}	// end "else sizeDifference <= 0"
		
	return (returnCode == 0);
	 
}	// end "TransferDataFromWindow"


bool CMFileFormatSpecsDlg::TransferDataToWindow() {
    wxStaticText* blockHeight = (wxStaticText*) FindWindow(IDC_BlockHeight);
    wxStaticText* blockWidth = (wxStaticText*) FindWindow(IDC_BlockWidth);
    wxStaticText* imageName = (wxStaticText*) FindWindow(IDC_ImageName);

    wxTextCtrl* numberLines = (wxTextCtrl*) FindWindow(IDC_NumberLines);
    wxTextCtrl* numberColumns = (wxTextCtrl*) FindWindow(IDC_NumberColumns);
    wxTextCtrl* postChannelBytes = (wxTextCtrl*) FindWindow(IDC_PostChannelBytes);
    wxTextCtrl* headerBytes = (wxTextCtrl*) FindWindow(IDC_HeaderBytes);
    wxTextCtrl* postLineBytes = (wxTextCtrl*) FindWindow(IDC_PostLineBytes);
    wxTextCtrl* preChannelBytes = (wxTextCtrl*) FindWindow(IDC_PreChannelBytes);
    wxTextCtrl* preLineBytes = (wxTextCtrl*) FindWindow(IDC_PreLineBytes);
    wxTextCtrl* numberChannels = (wxTextCtrl*) FindWindow(IDC_NumberChannels);
    wxTextCtrl* startColumnNumber = (wxTextCtrl*) FindWindow(IDC_StartColumnNumber);
    wxTextCtrl* startLineNumber = (wxTextCtrl*) FindWindow(IDC_StartLineNumber);
    wxTextCtrl* trailerBytes = (wxTextCtrl*) FindWindow(IDC_TrailerBytes);
    wxTextCtrl* fillDataValue = (wxTextCtrl*) FindWindow(IDC_FillDataValue);

    wxCheckBox* swapBytesFlag = (wxCheckBox *) FindWindow(IDC_SwapBytes);
    wxCheckBox* computeNumClasses = (wxCheckBox *) FindWindow(IDC_ComputeNumClasses);
    wxCheckBox* linesBottomToTopFlag = (wxCheckBox *) FindWindow(IDC_LinesBottomToTop);
    wxCheckBox* fillDataValueExistsFlag = (wxCheckBox *) FindWindow(IDC_FillDataValueExists);

    wxComboBox* dataValueType = (wxComboBox *) FindWindow(IDC_DataValueTypePopUp);
    wxComboBox* bandInterleave = (wxComboBox *) FindWindow(IDC_BandInterleavePopUp);
    wxComboBox* hdfDataSetSelection = (wxComboBox *) FindWindow(IDC_HDFDataSet);
    wxComboBox* collapseClassSelection = (wxComboBox *) FindWindow(IDC_CollapseClass);

    blockHeight->SetLabel(wxString::Format(wxT("%i"), m_blockHeight));
    blockWidth->SetLabel(wxString::Format(wxT("%i"), m_blockWidth));
    imageName->SetLabel(m_imageName);

    numberLines->ChangeValue(wxString::Format(wxT("%i"), m_numberLines));
    numberColumns->ChangeValue(wxString::Format(wxT("%i"), m_numberColumns));
    postChannelBytes->ChangeValue(wxString::Format(wxT("%i"), m_postChannelBytes));
    headerBytes->ChangeValue(wxString::Format(wxT("%i"), m_headerBytes));
    postLineBytes->ChangeValue(wxString::Format(wxT("%i"), m_postLineBytes));
    preChannelBytes->ChangeValue(wxString::Format(wxT("%i"), m_preChannelBytes));
    preLineBytes->ChangeValue(wxString::Format(wxT("%i"), m_preLineBytes));
    numberChannels->ChangeValue(wxString::Format(wxT("%i"), m_numberChannels));
    startColumnNumber->ChangeValue(wxString::Format(wxT("%i"), m_startColumnNumber));
    startLineNumber->ChangeValue(wxString::Format(wxT("%i"), m_startLineNumber));
    trailerBytes->ChangeValue(wxString::Format(wxT("%i"), m_trailerBytes));
    fillDataValue->ChangeValue(wxString::Format(wxT("%.2lf"), m_fillDataValue));

    swapBytesFlag->SetValue(m_swapBytesFlag);
    computeNumClasses->SetValue(m_computeNumClasses);
    linesBottomToTopFlag->SetValue(m_linesBottomToTopFlag);
    fillDataValueExistsFlag->SetValue(m_fillDataValueExistsFlag);

    dataValueType->SetSelection(m_dataValueType);
    bandInterleave->SetSelection(m_bandInterleave);
    hdfDataSetSelection->SetSelection(m_hdfDataSetSelection);
    collapseClassSelection->SetSelection(m_collapseClassSelection);

    return true;
	 
}		// end "TransferDataToWindow"
