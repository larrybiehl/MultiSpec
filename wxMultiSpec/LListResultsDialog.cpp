// LListResultsDialog.cpp : implementation file
//                
// Revised by Larry Biehl on 10/19/2018
//
#include "SMultiSpec.h"  
                   
#include "LImage_dialog.cpp"
#include "LListResultsDialog.h"
#include "LMultiSpec.h"

//#include	"SExternalGlobals.h" 

extern ListResultsSpecsPtr gListResultsSpecsPtr;

extern void ListResultsDialogSetThresholdItems(
   DialogPtr dialogPtr,
   Boolean thresholdResultsFlag,
   SInt16 thresholdTypeCode);

/////////////////////////////////////////////////////////////////////////////
// CMListResultsDialog dialog

CMListResultsDialog::CMListResultsDialog(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMListResultsDialog::IDD, pParent, title)
{
   //{{AFX_DATA_INIT(CMListResultsDialog)
   m_trainingFlag = FALSE;
   m_testFlag = FALSE;
   m_imageFlag = FALSE;
   m_fieldSummaryFlag = FALSE;
   m_classSummaryFlag = FALSE;
   m_groupSummaryFlag = FALSE;
   m_conversionFactor = 0;
   m_thresholdResultsFlag = FALSE;
   m_thresholdPercent = 0;
   m_textWindowFlag = FALSE;
   m_diskFileFlag = FALSE;
   m_tableType = -1;
   m_tableUnits = -1;
   //}}AFX_DATA_INIT

   m_projectWindowFlag = (gProjectInfoPtr != NULL);
   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag) {
      // Get memory for the local class list vector.	

      m_classListPtr = NULL;
      if (m_projectWindowFlag) {
         m_classListPtr = (UInt16*) MNewPointer(
            (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

         m_initializedFlag = (m_classListPtr != NULL);

      } // end "if (m_projectWindowFlag)"

   } // end "if (m_initializedFlag)"

   CreateControls();
   SetSizerAndFit(bSizer188);
} // end "CMListResultsDialog"


CMListResultsDialog::~CMListResultsDialog(void)
{
	m_classListPtr = CheckAndDisposePtr(m_classListPtr);

} // end "~CMListResultsDialog"


void CMListResultsDialog::CheckListAreaItems()
{
   m_localAreaCode = 0;

   if (m_trainingFlag)
      m_localAreaCode += 1;

   if (m_testFlag)
      m_localAreaCode += 2;

   if (m_imageFlag)
      m_localAreaCode += 4;

   SetSummarizeByItems();

} // end "CheckListAreaItems" 


void CMListResultsDialog::CheckSummaryItems()
{
   m_localSummaryCode = 0;

   if (m_fieldSummaryFlag)
      m_localSummaryCode += kFieldSummary;

   if (m_classSummaryFlag)
      m_localSummaryCode += kClassSummary;

   if (m_groupSummaryFlag)
      m_localSummaryCode += kGroupSummary;

   SetSummarizeByItems();

} // end "CheckSummaryItems"


void CMListResultsDialog::CreateControls()
{
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);

   this->SetSizeHints(wxDefaultSize, wxDefaultSize);

   //wxBoxSizer* bSizer188;
  	bSizer188 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer189;
	bSizer189 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer18;
	sbSizer18 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("List results for following areas") ), wxVERTICAL );
	
	wxBoxSizer* bSizer193;
	bSizer193 = new wxBoxSizer( wxHORIZONTAL );
	
	m_checkBox36 = new wxCheckBox( sbSizer18->GetStaticBox(), IDC_Training, wxT("Training"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox36, IDS_ToolTip263);
	bSizer193->Add( m_checkBox36, 0, wxLEFT, 15 );
	
	m_comboBox33 = new wxComboBox( sbSizer18->GetStaticBox(), IDC_ClassCombo, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox33->Append( wxT("All") );
	m_comboBox33->Append( wxT("Subset...") );
   SetUpToolTip(m_comboBox33, IDS_ToolTip103);
	bSizer193->Add( m_comboBox33, 0, wxLEFT|wxRIGHT|wxTOP, 15 );
	
	
	sbSizer18->Add( bSizer193, 0, wxEXPAND, 5 );
	
	m_checkBox37 = new wxCheckBox( sbSizer18->GetStaticBox(), IDC_Test, wxT("Test"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox37, IDS_ToolTip264);
	sbSizer18->Add( m_checkBox37, 0, wxLEFT, 15 );
	
	wxBoxSizer* bSizer117;
	bSizer117 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox38 = new wxCheckBox( sbSizer18->GetStaticBox(), IDC_Image, wxT("Image selection"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox38, IDS_ToolTip265);
	bSizer117->Add( m_checkBox38, 0, wxLEFT, 15 );
	
	wxBoxSizer* bSizer209;
	bSizer209 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer791;
	bSizer791 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bpButton4 = new wxBitmapButton( sbSizer18->GetStaticBox(), IDEntireImage, toentirei, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	
	m_bpButton4->SetBitmapDisabled( toentirei );
	//bSizer791->Add( m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	bSizer791->Add( m_bpButton4, 0, wxALIGN_CENTER|wxALL, 5 );
	m_bpButton5 = new wxBitmapButton( sbSizer18->GetStaticBox(), IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );	
	m_bpButton5->SetBitmapDisabled( selectedi );
   m_bpButton5->Hide();
	//bSizer791->Add( m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );
	bSizer791->Add( m_bpButton5, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer209->Add( bSizer791, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer210;
	bSizer210 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText177 = new wxStaticText( sbSizer18->GetStaticBox(), IDC_StartEndInterval, wxT("                      Start       \tEnd          Interval"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText177->Wrap( -1 );
	bSizer210->Add( m_staticText177, 0, wxALL, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 4, 0, 0 );
	
	m_staticText60 = new wxStaticText( sbSizer18->GetStaticBox(), IDC_LinePrompt, wxT("Lines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText60->Wrap( -1 );
	gSizer1->Add( m_staticText60, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_linestartctrl = new wxTextCtrl( sbSizer18->GetStaticBox(), IDC_LineStart, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_linestartctrl->SetMaxLength( 0 ); 
	m_linestartctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_LineStartString ) );
	
	gSizer1->Add( m_linestartctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_lineendctrl = new wxTextCtrl( sbSizer18->GetStaticBox(), IDC_LineEnd, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_lineendctrl->SetMaxLength( 0 ); 
	m_lineendctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_LineEndString ) );
	
	gSizer1->Add( m_lineendctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_lineintctrl = new wxTextCtrl( sbSizer18->GetStaticBox(), IDC_LineInterval, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_lineintctrl->SetMaxLength( 0 ); 
	m_lineintctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_LineIntervalString ) );
	
	gSizer1->Add( m_lineintctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_staticText62 = new wxStaticText( sbSizer18->GetStaticBox(), IDC_ColumnPrompt, wxT("Columns"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText62->Wrap( -1 );
	gSizer1->Add( m_staticText62, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_colstartctrl = new wxTextCtrl( sbSizer18->GetStaticBox(), IDC_ColumnStart, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_colstartctrl->SetMaxLength( 0 ); 
	m_colstartctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_ColumnStartString ) );
	
	gSizer1->Add( m_colstartctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_colendctrl = new wxTextCtrl( sbSizer18->GetStaticBox(), IDC_ColumnEnd, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_colendctrl->SetMaxLength( 0 ); 
	m_colendctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_ColumnEndString ) );
	
	gSizer1->Add( m_colendctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_colintctrl = new wxTextCtrl( sbSizer18->GetStaticBox(), IDC_ColumnInterval, wxEmptyString, wxDefaultPosition, wxSize( 70,-1 ), 0 );
	m_colintctrl->SetMaxLength( 0 ); 
	m_colintctrl->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_ColumnIntervalString ) );
	
   SetUpToolTip(m_bpButton4, IDS_ToolTip40);  
   SetUpToolTip(m_linestartctrl, IDS_ToolTip19);
   SetUpToolTip(m_lineendctrl, IDS_ToolTip20);
   SetUpToolTip(m_lineintctrl, IDS_ToolTip21);
   SetUpToolTip(m_colstartctrl, IDS_ToolTip22);
   SetUpToolTip(m_colendctrl, IDS_ToolTip23);
   SetUpToolTip(m_colintctrl, IDS_ToolTip24);
   
	gSizer1->Add( m_colintctrl, 0, wxALIGN_CENTER|wxALL, 2 );
	
	
	bSizer210->Add( gSizer1, 1, wxEXPAND, 5 );
	
	
	bSizer209->Add( bSizer210, 1, wxEXPAND, 5 );
	
	
	bSizer117->Add( bSizer209, 0, wxEXPAND|wxLEFT, 15 );
	
	
	sbSizer18->Add( bSizer117, 1, wxEXPAND, 5 );
	
	
	bSizer191->Add( sbSizer18, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( this, IDC_SummarizeOutline, wxT("Summarize by (train/test only):") ), wxVERTICAL );
	
	m_checkBox39 = new wxCheckBox( sbSizer9->GetStaticBox(), IDC_Field, wxT("Field"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox39->SetValue(true); 
   SetUpToolTip(m_checkBox39, IDS_ToolTip266);
	sbSizer9->Add( m_checkBox39, 0, wxLEFT, 15 );
	
	m_checkBox40 = new wxCheckBox( sbSizer9->GetStaticBox(), IDC_Class, wxT("Class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox40->SetValue(true); 
   SetUpToolTip(m_checkBox40, IDS_ToolTip267);
	sbSizer9->Add( m_checkBox40, 0, wxLEFT, 15 );
	
	m_checkBox41 = new wxCheckBox( sbSizer9->GetStaticBox(), IDC_Group, wxT("Group"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox41->SetValue(true); 
	SetUpToolTip(m_checkBox41, IDS_ToolTip268);
   sbSizer9->Add( m_checkBox41, 0, wxLEFT, 15 );
	
	
	bSizer191->Add( sbSizer9, 0, wxEXPAND, 5 );
	
	
	bSizer189->Add( bSizer191, 0, wxALL|wxEXPAND, 12 );
	
	wxBoxSizer* bSizer192;
	bSizer192 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Table type") ), wxVERTICAL );
	
	m_radioBtn19 = new wxRadioButton( sbSizer10->GetStaticBox(), IDC_ClassTable, wxT("Class"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn19, IDS_ToolTip269);
	sbSizer10->Add( m_radioBtn19, 0, wxLEFT|wxRIGHT, 15 );
	
	m_radioBtn20 = new wxRadioButton( sbSizer10->GetStaticBox(), IDC_GroupTable, wxT("Group"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn20, IDS_ToolTip269);
	sbSizer10->Add( m_radioBtn20, 0, wxLEFT|wxRIGHT, 15 );
	
	
	bSizer192->Add( sbSizer10, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Table units") ), wxVERTICAL );
	
	m_radioBtn21 = new wxRadioButton( sbSizer11->GetStaticBox(), IDC_Samples, wxT("Samples"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn21, IDS_ToolTip270);
	sbSizer11->Add( m_radioBtn21, 0, wxLEFT|wxRIGHT, 15 );
	
	m_radioBtn22 = new wxRadioButton( sbSizer11->GetStaticBox(), IDC_Percent, wxT("Percent"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn22, IDS_ToolTip270);
	sbSizer11->Add( m_radioBtn22, 0, wxLEFT|wxRIGHT, 15 );
	
	wxBoxSizer* bSizer198;
	bSizer198 = new wxBoxSizer( wxHORIZONTAL );
	
	m_radioBtn23 = new wxRadioButton( sbSizer11->GetStaticBox(), IDC_Area, wxT("Area"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn23, IDS_ToolTip270);
	bSizer198->Add( m_radioBtn23, 0, wxALIGN_CENTER|wxLEFT, 15 );
	
	m_staticText203 = new wxStaticText( sbSizer11->GetStaticBox(), IDC_ConversionText, wxT("conversion factor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText203->Wrap( -1 );
   SetUpToolTip(m_staticText203, IDS_ToolTip271);
	bSizer198->Add( m_staticText203, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrl106 = new wxTextCtrl( sbSizer11->GetStaticBox(), IDC_ConversionFactor, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl106, IDS_ToolTip271);
	bSizer198->Add( m_textCtrl106, 0, wxALIGN_CENTER|wxBOTTOM|wxLEFT|wxRIGHT, 5 );
	
	
	sbSizer11->Add( bSizer198, 0, wxEXPAND, 5 );
	
	
	bSizer192->Add( sbSizer11, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer199;
	bSizer199 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox42 = new wxCheckBox( this, IDC_ThresholdResults, wxT("Threshold results at"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox42, IDS_ToolTip272);
	bSizer199->Add( m_checkBox42, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer200;
	bSizer200 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText204 = new wxStaticText( this, IDC_ProbabilityPrompt, wxT("Probabilities <"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText204->Wrap( -1 );
	bSizer200->Add( m_staticText204, 0, wxALIGN_CENTER|wxLEFT, 45 );
	
	m_textCtrl107 = new wxTextCtrl( this, IDC_Probability, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0 );
	bSizer200->Add( m_textCtrl107, 0, wxALIGN_CENTER|wxLEFT, 5 );
	
	m_staticText205 = new wxStaticText( this, IDC_LRdegrees, wxT("o"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText205->Wrap( -1 );
	bSizer200->Add( m_staticText205, 0, wxLEFT, 5 );
	
	m_staticText206 = new wxStaticText( this, IDC_LRPercent, wxT("%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText206->Wrap( -1 );
	bSizer200->Add( m_staticText206, 0, wxALIGN_CENTER|wxALL|wxLEFT, 5 );
	
	
	bSizer199->Add( bSizer200, 1, wxBOTTOM|wxEXPAND, 5 );
	
	
	bSizer192->Add( bSizer199, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer16;
	sbSizer16 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Write results to:") ), wxVERTICAL );
	
	m_checkBox43 = new wxCheckBox( sbSizer16->GetStaticBox(), IDC_TextWindow, wxT("Text window"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_checkBox43, IDS_ToolTip66);
   sbSizer16->Add( m_checkBox43, 0, wxLEFT, 15 );
	
	m_checkBox44 = new wxCheckBox( sbSizer16->GetStaticBox(), IDC_DiskFile, wxT("Disk file"), wxDefaultPosition, wxDefaultSize, 0 );
	SetUpToolTip(m_checkBox44, IDS_ToolTip67);
   sbSizer16->Add( m_checkBox44, 0, wxLEFT, 15 );
	
	
	bSizer192->Add( sbSizer16, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizer189->Add( bSizer192, 0, wxALL|wxEXPAND, 12 );
	
	
	bSizer188->Add( bSizer189, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer190;
	bSizer190 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button44 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer190->Add( m_button44, 0, wxALL, 5 );
	
	m_button45 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer190->Add( m_button45, 0, wxALL, 5 );
	
	
	bSizer188->Add( bSizer190, 0, wxALIGN_RIGHT|wxALL, 12 );
	
	
	this->SetSizer( bSizer188 );
	this->Layout();
	
	this->Centre( wxBOTH );
}


BEGIN_EVENT_TABLE(CMListResultsDialog, CMDialog)
EVT_INIT_DIALOG(CMListResultsDialog::OnInitDialog)
EVT_CHECKBOX(IDC_Image, CMListResultsDialog::OnImage)
EVT_CHECKBOX(IDC_Training, CMListResultsDialog::OnTraining)
EVT_CHECKBOX(IDC_Test, CMListResultsDialog::OnTest)
EVT_CHECKBOX(IDC_Field, CMListResultsDialog::OnFieldSummary)
EVT_CHECKBOX(IDC_Class, CMListResultsDialog::OnClassSummary)
EVT_CHECKBOX(IDC_Group, CMListResultsDialog::OnGroupSummary)
EVT_CHECKBOX(IDC_ThresholdResults, CMListResultsDialog::OnThresholdResults)
EVT_COMBOBOX(IDC_ClassCombo, CMListResultsDialog::OnSelendokClassCombo)
EVT_TEXT(IDC_ColumnEnd, CMListResultsDialog::CheckColumnEnd)
EVT_TEXT(IDC_ColumnStart, CMListResultsDialog::CheckColumnStart)
EVT_TEXT(IDC_LineEnd, CMListResultsDialog::CheckLineEnd)
EVT_TEXT(IDC_LineStart, CMListResultsDialog::CheckLineStart)
EVT_BUTTON(IDEntireImage, CMListResultsDialog::ToEntireImage)
EVT_BUTTON(IDSelectedImage, CMListResultsDialog::ToSelectedImage)
EVT_RADIOBUTTON(IDC_Samples, CMListResultsDialog::OnSampleUnits)
EVT_RADIOBUTTON(IDC_Percent, CMListResultsDialog::OnPercentUnits)
EVT_RADIOBUTTON(IDC_Area, CMListResultsDialog::OnAreaUnits)
//EVT_CHAR_HOOK(CMListResultsDialog::OnButtonPress)
END_EVENT_TABLE()


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DoDialog
//
//	Software purpose:	The purpose of this routine is to present the List
//							Results specification dialog box to the user and copy the
//							revised info to the list results specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 05/20/97
//	Revised By:			Larry L. Biehl			Date: 05/20/97	

Boolean
CMListResultsDialog::DoDialog(void) {
   Boolean continueFlag = FALSE;

   SInt16 returnCode;



   // Make sure intialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   returnCode = ShowModal();

   if (returnCode == wxID_OK) {
      continueFlag = TRUE;

      // Class areas																

      if (m_groupSummaryFlag)
         m_classSelection = 0;

      gListResultsSpecsPtr->classAreaSet = m_classSelection;

      if (m_projectWindowFlag) {
         SInt16* classAreaPtr = (SInt16*) GetHandlePointer(
            gListResultsSpecsPtr->classAreaHandle,
            kNoLock,
            kNoMoveHi);

         if (m_classSelection == kAllMenuItem)
            LoadClassAreaVector(
            &gListResultsSpecsPtr->numberClassAreas,
            classAreaPtr);

         else // m_classSelection == kSubsetMenuItem
         {
            UInt32 index;

            gListResultsSpecsPtr->numberClassAreas = m_localNumberClasses;
            for (index = 0; index < m_localNumberClasses; index++)
               classAreaPtr[index] = m_classListPtr[index];

         } // end "else m_classSelection == kSubsetMenuItem"

      } // end "if (m_projectWindowFlag)"

      // Areas to use to list results.										

      gListResultsSpecsPtr->areaCode = 0;
      if (m_trainingFlag)
         gListResultsSpecsPtr->areaCode += kTrainingType;

      if (m_testFlag)
         gListResultsSpecsPtr->areaCode += kTestingType;

      if (m_imageFlag)
         gListResultsSpecsPtr->areaCode += kAreaType;

      // Selected list results area.										

      gListResultsSpecsPtr->lineStart = m_LineStart;
      gListResultsSpecsPtr->lineEnd = m_LineEnd;
      gListResultsSpecsPtr->lineInterval = m_LineInterval;
      gListResultsSpecsPtr->columnStart = m_ColumnStart;
      gListResultsSpecsPtr->columnEnd = m_ColumnEnd;
      gListResultsSpecsPtr->columnInterval = m_ColumnInterval;

      // Summarize by.															

      if (m_nonClusterFieldTypeCode) {
         gListResultsSpecsPtr->summaryCode = 0;
         if (m_fieldSummaryFlag)
            gListResultsSpecsPtr->summaryCode += kFieldSummary;

         if (m_classSummaryFlag)
            gListResultsSpecsPtr->summaryCode += kClassSummary;

         if (m_groupSummaryFlag)
            gListResultsSpecsPtr->summaryCode += kGroupSummary;

      } // end "if (nonClusterFieldTypeCode)" 

      // Table type.																

      gListResultsSpecsPtr->tableType = 0;
      if (m_tableType == 0)
         gListResultsSpecsPtr->tableType = 1;

      else if (m_tableType == 1)
         gListResultsSpecsPtr->tableType = 2;

      // Table units.															

      gListResultsSpecsPtr->unitsCode = 0;
      if (m_tableUnits == 0)
         gListResultsSpecsPtr->unitsCode += kSampleUnits;

      if (m_tableUnits == 1)
         gListResultsSpecsPtr->unitsCode += kPercentUnits;

      if (m_tableUnits == 2)
         gListResultsSpecsPtr->unitsCode += kAreaUnits;

      if (gListResultsSpecsPtr->unitsCode == kAreaUnits)
         gListResultsSpecsPtr->summaryUnitsCode = kAreaUnits;

      else // ...->unitsCode != kAreaUnits
      {
         if (GetDefaultAreaConversionFactor(
            gImageFileInfoPtr->mapProjectionHandle,
            gImageWindowInfoPtr->windowInfoHandle) == 0)
            gListResultsSpecsPtr->summaryUnitsCode = kPercentUnits;

      } // end "else		// ...->unitsCode != kAreaUnits"

      // Area conversion factor.												

      if (m_conversionFactor > 0)
         gListResultsSpecsPtr->areaConversionFactor = m_conversionFactor;

      // Threshold results flag.												

      gListResultsSpecsPtr->thresholdFlag = m_thresholdResultsFlag;

      if (gListResultsSpecsPtr->thresholdFlag) {
         // Get probability threshold.										

         gListResultsSpecsPtr->probabilityThreshold = m_thresholdPercent;

         // Get the image data value which corresponds to the given
         // threshold value.

         Handle fileInfoHandle = GetFileInfoHandle(
            gListResultsSpecsPtr->probabilityWindowInfoHandle);

         gListResultsSpecsPtr->probabilityThresholdCode =
            GetThresholdCode(m_thresholdPercent,
            fileInfoHandle,
            gListResultsSpecsPtr->thresholdTypeCode);

      } // end "if (gListResultsSpecsPtr->thresholdFlag)" 

      // Write list results to output window.							
      // Write list results to disk file.									

      gListResultsSpecsPtr->outputStorageType = 0;

      if (m_textWindowFlag)
         gListResultsSpecsPtr->outputStorageType += 1;

      if (m_diskFileFlag)
         gListResultsSpecsPtr->outputStorageType += 2;

   } // end "if (returnCode == IDOK)"

   return (continueFlag);

} // end "DoDialog" 

/*
void CMListResultsDialog::HideShowProbabilityItems(
            Boolean				thresholdResultsFlag)
				
{  
   if (thresholdResultsFlag)
      {                                          
      MShowDialogItem (this, IDC_ProbabilityPrompt);
      MShowDialogItem (this, IDC_Probability);
	
      }		// end "if (thresholdResultsFlag)"
	
   else		// !thresholdResultsFlag
      {	                                        
      MHideDialogItem (this, IDC_ProbabilityPrompt);
      MHideDialogItem (this, IDC_Probability);
		
      }		// end "else		// !thresholdResultsFlag"
	
}		// end "HideShowProbabilityItems"
*/

void CMListResultsDialog::HideShowUnitsItems(
   Boolean areaUnitsFlag) {
   if (areaUnitsFlag) {
      MShowDialogItem(this, IDC_ConversionText);
      MShowDialogItem(this, IDC_ConversionFactor);

   }// end "if (areaUnitsFlag)"

   else // !areaUnitsFlag
   {
      MHideDialogItem(this, IDC_ConversionText);
      MHideDialogItem(this, IDC_ConversionFactor);

   } // end "else		// !areaUnitsFlag"

} // end "HideShowUnitsItems"


void CMListResultsDialog::OnAreaUnits(wxCommandEvent& event)
{
   HideShowUnitsItems(TRUE);

} // end "OnAreaUnits"

/*
void CMListResultsDialog::OnButtonPress(wxKeyEvent& event) 
{
   if (event.GetKeyCode() == WXK_RETURN) {
      if (TransferDataFromWindow())
         EndModal(wxID_OK);
   } else {
      event.Skip();
   }
}
*/

void CMListResultsDialog::OnClassSummary(wxCommandEvent& event)
{
   wxCheckBox * classcb = (wxCheckBox*) FindWindow(IDC_Class);
   m_classSummaryFlag = classcb->GetValue();
   CheckSummaryItems();

}


void CMListResultsDialog::OnFieldSummary(wxCommandEvent& event)
{
   wxCheckBox* field = (wxCheckBox*) FindWindow(IDC_Field);
   m_fieldSummaryFlag = field->GetValue();
   CheckSummaryItems();

}


void CMListResultsDialog::OnGroupSummary(wxCommandEvent& event)
{
   wxCheckBox* group = (wxCheckBox*) FindWindow(IDC_Group);
   m_groupSummaryFlag = group->GetValue();
   CheckSummaryItems();

}


void CMListResultsDialog::OnImage(wxCommandEvent& event)
{
   //   DDX_Check(m_dialogFromPtr, IDC_Image, m_imageFlag);
   wxCheckBox* image = (wxCheckBox*) FindWindow(IDC_Image);
   m_imageFlag = image->GetValue();
   CheckListAreaItems();

   HideShowAreaItems(m_imageFlag);

   if (m_imageFlag) {
      SetEntireImageButtons(
         NULL,
         m_LineStart,
         m_LineEnd,
         m_ColumnStart,
         m_ColumnEnd);

      // Set default text selection to first edit text item	

      //		GetDlgItem(IDC_LineStart)->SetFocus();
      //		SendDlgItemMessage( IDC_LineStart, EM_SETSEL, 0, MAKELPARAM(0, -1) );
      SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);

   } // end "if (m_imageFlag)"

} // end "OnImage"


void CMListResultsDialog::OnInitDialog(wxInitDialogEvent& event)
{
   m_nonClusterFieldTypeCode = 0;

   if (m_projectWindowFlag)
      m_nonClusterFieldTypeCode = DetermineFieldTypes();

   m_nonClusterFieldTypeCode &= 0x0003;

   // List areas to be used - training, test, and/or image selection.

   m_localAreaCode = gListResultsSpecsPtr->areaCode;

   if (m_projectWindowFlag) {
      // List results for training areas. Unhilite this control if there 	
      // are no training areas for the project. 									

      if (!(m_nonClusterFieldTypeCode & kTrainingType))
         SetDLogControlHilite(this, IDC_Training, 255);

      else // m_nonClusterFieldTypeCode & kTrainingType 
         m_trainingFlag = (m_localAreaCode & kTrainingType);

      // List results for test areas. Unhilite this control if there 	
      // are no test areas for the project.										

      if (!(m_nonClusterFieldTypeCode & kTestingType))
         SetDLogControlHilite(this, IDC_Test, 255);

      else // m_nonClusterFieldTypeCode & kTestingType
         m_testFlag = (m_localAreaCode & kTestingType);

      if (!m_trainingFlag && !m_testFlag)
         MHideDialogItem(this, IDC_ClassCombo);

   }// end "if (m_projectWindowFlag)" 

   else // !m_projectWindowFlag
   {
      SetDLogControlHilite(this, IDC_Training, 255);
      SetDLogControlHilite(this, IDC_Test, 255);
//      MHideDialogItem(this, IDC_Training);
//      MHideDialogItem(this, IDC_Test);
      MHideDialogItem(this, IDC_ClassCombo);

   } // end "else !m_projectWindowFlag" 

   // Class areas to use.		

   m_classSelection = gListResultsSpecsPtr->classAreaSet;
   m_localNumberClasses = gListResultsSpecsPtr->numberClassAreas;

   if (m_projectWindowFlag) {
      UInt32 index;

      UInt16* classPtr = (UInt16*) GetHandlePointer(
         gListResultsSpecsPtr->classAreaHandle,
         kNoLock,
         kNoMoveHi);

      for (index = 0; index < m_localNumberClasses; index++)
         m_classListPtr[index] = classPtr[index];

   } // end "if (m_projectWindowFlag)"

   // If the train/test areas are to be summarized by groups then all	
   // classes will need to be used.													

   if (gListResultsSpecsPtr->summaryCode & kGroupSummary)
      MHideDialogItem(this, IDC_ClassCombo);
   //		m_classSelection = -1; 

   // List results for selected area.												

   m_imageFlag = (m_localAreaCode & kAreaType) ||
      (m_nonClusterFieldTypeCode == 0);

   //	Selected area for list results
   // 	Initialize selected area structure.		

   InitializeDialogSelectArea(&m_dialogSelectArea,
      gImageWindowInfoPtr,
      gActiveImageWindow,
      gListResultsSpecsPtr->columnStart,
      gListResultsSpecsPtr->columnEnd,
      gListResultsSpecsPtr->columnInterval,
      gListResultsSpecsPtr->lineStart,
      gListResultsSpecsPtr->lineEnd,
      gListResultsSpecsPtr->lineInterval,
      12,
      10,
      kAdjustToBaseImage);

   m_LineStart = gListResultsSpecsPtr->lineStart;
   m_LineEnd = gListResultsSpecsPtr->lineEnd;
   m_LineInterval = gListResultsSpecsPtr->lineInterval;
   m_ColumnStart = gListResultsSpecsPtr->columnStart;
   m_ColumnEnd = gListResultsSpecsPtr->columnEnd;
   m_ColumnInterval = gListResultsSpecsPtr->columnInterval;

   HideShowAreaItems(m_imageFlag);

   if (m_imageFlag)
      SetEntireImageButtons(
      NULL,
      m_LineStart,
      m_LineEnd,
      m_ColumnStart,
      m_ColumnEnd);

   // Summarize by field.																
   // Summarize by class. 																
   // Summarize by group.

   m_localSummaryCode = gListResultsSpecsPtr->summaryCode;

   if (m_projectWindowFlag) {
      // Verify that the group summary option is not available if there
      // are not groups defined.

      if (gImageFileInfoPtr->numberGroups == 0)
         m_localSummaryCode &= (kFieldSummary + kClassSummary);

      if (m_nonClusterFieldTypeCode &&
         (m_localAreaCode & (kTrainingType + kTestingType))) {
         m_fieldSummaryFlag = ((m_localSummaryCode & kFieldSummary) != 0);
         m_classSummaryFlag = ((m_localSummaryCode & kClassSummary) != 0);
         m_groupSummaryFlag = ((m_localSummaryCode & kGroupSummary) != 0);

      } // end "if (nonClusterFieldTypeCode && ...)" 

      if (gImageFileInfoPtr->numberGroups == 0)
         SetDLogControlHilite(this, IDC_Group, 255);

      if (m_localAreaCode == kAreaType || m_nonClusterFieldTypeCode == 0) {
         SetDLogControlHilite(this, IDC_Field, 255);
         SetDLogControlHilite(this, IDC_Class, 255);
         SetDLogControlHilite(this, IDC_Group, 255);

      } // end "if (m_localAreaCode == ...)" 

   }// end "if (projectWindowFlag)" 

   else // !projectWindowFlag 
   {
      MHideDialogItem(this, IDC_SummarizeOutline);
      MHideDialogItem(this, IDC_Field);
      MHideDialogItem(this, IDC_Class);
      MHideDialogItem(this, IDC_Group);

   } // end "else !projectWindowFlag"  

   // Class type table. 																
   // Group type table. 																

   if (gListResultsSpecsPtr->tableType == kClassTable)
      m_tableType = 0;

   else if (gListResultsSpecsPtr->tableType == kGroupTable)
      m_tableType = 1;

   if (gImageFileInfoPtr->numberGroups == 0)
      SetDLogControlHilite(this, IDC_GroupTable, 255);

   // Sample units. 																		
   // Percent units. 																	
   // Area units. 																		

   if (gListResultsSpecsPtr->unitsCode & kSampleUnits)
      m_tableUnits = 0;

   else if (gListResultsSpecsPtr->unitsCode & kPercentUnits)
      m_tableUnits = 1;

   else if (gListResultsSpecsPtr->unitsCode & kAreaUnits)
      m_tableUnits = 2;

   HideShowUnitsItems(m_tableUnits == 2);

   // Area conversion factor.															

   m_saveAreaConversionFactor = gListResultsSpecsPtr->areaConversionFactor;
   m_conversionFactor = m_saveAreaConversionFactor;

   // Threshold the results.															
   // Threshold percent.																

   m_thresholdResultsFlag = gListResultsSpecsPtr->thresholdFlag;
   if (gListResultsSpecsPtr->probabilityWindowInfoHandle == NULL)
      SetDLogControlHilite(this, IDC_ThresholdResults, 255);

   m_saveThresholdPercent = gListResultsSpecsPtr->probabilityThreshold;
   m_thresholdPercent = m_saveThresholdPercent;

   ListResultsDialogSetThresholdItems(this,
      m_thresholdResultsFlag,
      gListResultsSpecsPtr->thresholdTypeCode);

   // List results to output text window.											
   // List results to disk file.														

   m_textWindowFlag = (gListResultsSpecsPtr->outputStorageType & 0x0001);
   m_diskFileFlag = (gListResultsSpecsPtr->outputStorageType & 0x0002);

   if (TransferDataToWindow())
      PositionDialogWindow();

   // Set default text selection to first edit text item	

   //	GetDlgItem(IDC_LineStart)->SetFocus();
   //	SendDlgItemMessage( IDC_LineStart, EM_SETSEL, 0, MAKELPARAM(0, -1) );
   SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);

   //return FALSE; // return TRUE  unless you set the focus to a control 

} // end "OnInitDialog"


void CMListResultsDialog::OnPercentUnits(wxCommandEvent& event)
{
   HideShowUnitsItems(FALSE);

} // end "OnPercentUnits"


void CMListResultsDialog::OnSampleUnits(wxCommandEvent& event)
{
   HideShowUnitsItems(FALSE);

} // end "OnSampleUnits"


void CMListResultsDialog::OnSelendokClassCombo(wxCommandEvent& event)
{
   HandleClassesMenu(&m_localNumberClasses,
      (SInt16*) m_classListPtr,
      1,
      (SInt16) gProjectInfoPtr->numberStatisticsClasses,
      IDC_ClassCombo,
      &m_classSelection);

} // end "OnSelendokClassCombo"


void CMListResultsDialog::OnTest(wxCommandEvent& event)
{
   wxCheckBox* test = (wxCheckBox*) FindWindow(IDC_Test);
   m_testFlag = test->GetValue();
   CheckListAreaItems();

} // end "OnTest"


void CMListResultsDialog::OnThresholdResults(wxCommandEvent& event)
{
   wxCheckBox* thres = (wxCheckBox*) FindWindow(IDC_ThresholdResults);
   m_thresholdResultsFlag = thres->GetValue();

   ListResultsDialogSetThresholdItems(this,
      m_thresholdResultsFlag,
      gListResultsSpecsPtr->thresholdTypeCode);

   //	HideShowProbabilityItems (m_thresholdResultsFlag);

} // end "OnThresholdResults"


void CMListResultsDialog::OnTraining(wxCommandEvent& event)
{
   wxCheckBox* train = (wxCheckBox*) FindWindow(IDC_Training);
   m_trainingFlag = train->GetValue();

   CheckListAreaItems();

} // end "OnTraining"


void CMListResultsDialog::SetSummarizeByItems()
{
   Boolean tempFlag = TRUE;


   if (m_projectWindowFlag) {
      tempFlag = (m_localSummaryCode != 0);

      if (m_localAreaCode == kAreaType || m_localAreaCode == 0)
			{
         m_fieldSummaryFlag = FALSE;
         m_classSummaryFlag = FALSE;
         m_groupSummaryFlag = FALSE;

         wxCheckBox* field = (wxCheckBox*) FindWindow(IDC_Field);
         field->SetValue(m_fieldSummaryFlag);
         wxCheckBox* classcb = (wxCheckBox*) FindWindow(IDC_Class);
         classcb->SetValue(m_classSummaryFlag);
         wxCheckBox* group = (wxCheckBox*) FindWindow(IDC_Group);
         group->SetValue(m_groupSummaryFlag);

         SetDLogControlHilite(this, IDC_Field, 255);
         SetDLogControlHilite(this, IDC_Class, 255);
         SetDLogControlHilite(this, IDC_Group, 255);

         MHideDialogItem(this, IDC_ClassCombo);

      }// end "if (m_localAreaCode == kAreaType || ...)" 

      else // localAreaCode != kAreaType && ... 
      {
         m_fieldSummaryFlag = ((m_localSummaryCode & kFieldSummary) != 0);
         m_classSummaryFlag = ((m_localSummaryCode & kClassSummary) != 0);
         m_groupSummaryFlag = ((m_localSummaryCode & kGroupSummary) != 0);

         //         DDX_Check(m_dialogToPtr, IDC_Field, m_fieldSummaryFlag);
         //         DDX_Check(m_dialogToPtr, IDC_Class, m_classSummaryFlag);
         //         DDX_Check(m_dialogToPtr, IDC_Group, m_groupSummaryFlag);

         wxCheckBox* field = (wxCheckBox*) FindWindow(IDC_Field);
         field->SetValue(m_fieldSummaryFlag);
         wxCheckBox* classcb = (wxCheckBox*) FindWindow(IDC_Class);
         classcb->SetValue(m_classSummaryFlag);
         wxCheckBox* group = (wxCheckBox*) FindWindow(IDC_Group);
         group->SetValue(m_groupSummaryFlag);

         SetDLogControlHilite(this, IDC_Field, 0);
         SetDLogControlHilite(this, IDC_Class, 0);

         if (gImageFileInfoPtr->numberGroups > 0)
            SetDLogControlHilite(this, IDC_Group, 0);

         if (m_groupSummaryFlag)
            MHideDialogItem(this, IDC_ClassCombo);

         else // !m_groupSummaryFlag
            MShowDialogItem(this, IDC_ClassCombo);

      } // end "else m_localAreaCode != kAreaType && ..." 

   } // end "if (m_projectWindowFlag)" 

   //   GetDlgItem(IDOK)->EnableWindow((m_localAreaCode != 0) && tempFlag);
   wxButton* okbutton = (wxButton*) FindWindow(wxID_OK);
   okbutton->Enable((m_localAreaCode != 0) && tempFlag);

} // end "SetSummarizeByItems"


bool CMListResultsDialog::TransferDataFromWindow()
{
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
   wxTextCtrl* conversion = (wxTextCtrl*) FindWindow(IDC_ConversionFactor);
   wxTextCtrl* prob = (wxTextCtrl*) FindWindow(IDC_Probability);

   wxCheckBox* train = (wxCheckBox*) FindWindow(IDC_Training);
   wxCheckBox* test = (wxCheckBox*) FindWindow(IDC_Test);
   wxCheckBox* image = (wxCheckBox*) FindWindow(IDC_Image);
   wxCheckBox* field = (wxCheckBox*) FindWindow(IDC_Field);
   wxCheckBox* classcb = (wxCheckBox*) FindWindow(IDC_Class);
   wxCheckBox* group = (wxCheckBox*) FindWindow(IDC_Group);
   wxCheckBox* thres = (wxCheckBox*) FindWindow(IDC_ThresholdResults);
   wxCheckBox* text = (wxCheckBox*) FindWindow(IDC_TextWindow);
   wxCheckBox* disk = (wxCheckBox*) FindWindow(IDC_DiskFile);

   wxComboBox* classcombo = (wxComboBox*) FindWindow(IDC_ClassCombo);
   wxRadioButton* classtable = (wxRadioButton*) FindWindow(IDC_ClassTable);
   wxRadioButton* grouptable = (wxRadioButton*) FindWindow(IDC_GroupTable);
   wxRadioButton* sample = (wxRadioButton*) FindWindow(IDC_Samples);
   wxRadioButton* percent = (wxRadioButton*) FindWindow(IDC_Percent);
   wxRadioButton* area = (wxRadioButton*) FindWindow(IDC_Area);   
   
   wxString conversion_double = conversion->GetValue();
   conversion_double.ToDouble(&m_conversionFactor);
   wxString prob_double = prob->GetValue();
   prob_double.ToDouble(&m_thresholdPercent);
   
    m_trainingFlag = train->GetValue();
     m_testFlag = test->GetValue();
     m_imageFlag = image->GetValue();
     m_fieldSummaryFlag = field->GetValue();
     m_classSummaryFlag = classcb->GetValue();
     m_groupSummaryFlag = group->GetValue();
     m_thresholdResultsFlag = thres->GetValue();  
     m_textWindowFlag  = text->GetValue();
     m_diskFileFlag = disk->GetValue();
     
     if(sample->GetValue() == true){
        m_tableUnits = 0;
     }else if (percent->GetValue() == true){
        m_tableUnits = 1;  
     }else{
        m_tableUnits = 2;
     }
   
     if(classtable->GetValue() == true){
        m_tableType = 0;
     }else{
        m_tableType = 1;
     }
   m_LineStartString = l_start->GetValue();
   m_LineEndString = l_end->GetValue();
   m_LineIntervalString = l_inter->GetValue();
   m_ColumnStartString = c_start->GetValue();
   m_ColumnEndString = c_end->GetValue();
   m_ColumnIntervalString = c_inter->GetValue();

   return true;
}


bool CMListResultsDialog::TransferDataToWindow()
{
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*) FindWindow(IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*) FindWindow(IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
   wxTextCtrl* conversion = (wxTextCtrl*) FindWindow(IDC_ConversionFactor);
   wxTextCtrl* prob = (wxTextCtrl*) FindWindow(IDC_Probability);

   wxCheckBox* train = (wxCheckBox*) FindWindow(IDC_Training);
   wxCheckBox* test = (wxCheckBox*) FindWindow(IDC_Test);
   wxCheckBox* image = (wxCheckBox*) FindWindow(IDC_Image);
   wxCheckBox* field = (wxCheckBox*) FindWindow(IDC_Field);
   wxCheckBox* classcb = (wxCheckBox*) FindWindow(IDC_Class);
   wxCheckBox* group = (wxCheckBox*) FindWindow(IDC_Group);
   wxCheckBox* thres = (wxCheckBox*) FindWindow(IDC_ThresholdResults);
   wxCheckBox* text = (wxCheckBox*) FindWindow(IDC_TextWindow);
   wxCheckBox* disk = (wxCheckBox*) FindWindow(IDC_DiskFile);

   wxComboBox* classcombo = (wxComboBox*) FindWindow(IDC_ClassCombo);
   wxRadioButton* classtable = (wxRadioButton*) FindWindow(IDC_ClassTable);
   wxRadioButton* grouptable = (wxRadioButton*) FindWindow(IDC_GroupTable);
   
   wxRadioButton* sample = (wxRadioButton*) FindWindow(IDC_Samples);
   wxRadioButton* percent = (wxRadioButton*) FindWindow(IDC_Percent);
   wxRadioButton* area = (wxRadioButton*) FindWindow(IDC_Area);   

   if (m_tableType == 0) {
      classtable->SetValue(true);
      grouptable->SetValue(false);      
   }else{
       classtable->SetValue(false);
      grouptable->SetValue(true);     
   } 
   
   if (m_tableUnits == 0) {
      sample->SetValue(true);
      percent->SetValue(false);
      area->SetValue(false);
   } else if (m_tableUnits == 1){
      sample->SetValue(false);
      percent->SetValue(true);
      area->SetValue(false);
   }else{
      sample->SetValue(false);
      percent->SetValue(false);
      area->SetValue(true);
   }

   train->SetValue(m_trainingFlag);
   test->SetValue(m_testFlag);
   image->SetValue(m_imageFlag);
   field->SetValue(m_fieldSummaryFlag);
   classcb->SetValue(m_classSummaryFlag);
   group->SetValue(m_groupSummaryFlag);
   thres->SetValue(m_thresholdResultsFlag);
   text->SetValue(m_textWindowFlag);
   disk->SetValue(m_diskFileFlag);

   classcombo->SetSelection(m_classSelection);


   conversion->ChangeValue(wxString::Format(wxT("%.9f"), m_conversionFactor));
   prob->ChangeValue(wxString::Format(wxT("%.1f"), m_thresholdPercent));
   c_end->ChangeValue(wxString::Format(wxT("%i"), m_ColumnEnd));
   c_inter->ChangeValue(wxString::Format(wxT("%i"), m_ColumnInterval));
   c_start->ChangeValue(wxString::Format(wxT("%i"), m_ColumnStart));
   l_end->ChangeValue(wxString::Format(wxT("%i"), m_LineEnd));
   l_inter->ChangeValue(wxString::Format(wxT("%i"), m_LineInterval));
   l_start->ChangeValue(wxString::Format(wxT("%i"), m_LineStart));
   return true;
}
