// LProjectDialogs.cpp : implementation file
//
// Revised by Larry Biehl on 07/06/2017
//               
/* Template for debugging for MultiSpec Online on mygeohub.org.
	int numberChars = sprintf ((char*)&gTextString3,
								" LProjectDialogs::xxx (countBytes, errCode) %d, %d%s", 
								countBytes,
								errCode,
								gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/
#include "SMultiSpec.h"

#include "LImageView.h"
#include "LProjectDialogs.h"
#include	"LChannelsDialog.h"


extern void ChangeFieldType (
				SInt16								classStorage,
				SInt16								currentField,
				SInt16								fieldType);

extern void ComputeMapCoordinates (
				Handle								windowInfoHandle,
				SInt16								viewUnits,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr);

extern void EditClassFieldDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								classFieldFlag,
				SInt16								classStorage,
				SInt16								currentField,
				UInt16*								selectedItemPtr);
	
extern SInt16 EditLineColumnDialogCheckCoordinates (
				DialogPtr							dialogPtr,
				Handle								inputWindowInfoHandle,
				SInt16								selectionDisplayUnits,
				LongRect*							selectionRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				double								newColumnStart,
				double								newColumnEnd,
				double								newLineStart,
				double								newLineEnd);

extern void EditLineColumnDialogInitialize (
				DialogPtr							dialogPtr,
				WindowPtr							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				SInt16								pointType,
				SInt16								unitsDisplayCode,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				DoubleRect*							inputCoordinateRectanglePtr,
				Boolean*								applyToAllWindowsFlagPtr,
				Boolean*								useStartLineColumnFlagPtr,
				SInt16*								selectionDisplayUnitsPtr,
				SInt16								stringID);

extern void EditLineColumnDialogOK (
				Handle								windowInfoHandle,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								unitsDisplayCode,
				Boolean*								changedFlagPtr);

extern void EditLineColumnDialogSetStartLC (
				DialogPtr							dialogPtr,
				SInt16								unitsDisplayCode,
				Boolean								applyToAllWindowsFlag);

extern void EditSelectionDialogSetCoordinates (
				DialogPtr							dialogPtr,
				Handle								windowInfoHandle,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							inputCoordinateRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				LongRect*							minMaxSelectionRectanglePtr,
				DoubleRect*							minMaxCoordinateRectanglePtr,
				SInt16								requestedSelectionUnitsCode,
				SInt16								currentSelectionUnitsCode);

extern void EditSelectionDialogShowSelection (
				WindowPtr							windowPtr,
				Handle								windowInfoHandle,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				Boolean								applyToAllWindowsFlag,
				Boolean								useStartLineColumnFlag,
				SInt16								unitsToUseCode,
				double								factor);

extern SInt16 GetFileGridCoordinateCode (
				Handle								windowInfoHandle);

extern void StatisticsDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								totalNumberChannels,
				SInt16*								localStatCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				Boolean*								useCommonCovarianceInLOOCFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				SInt16*								channelSelectionPtr,
				SInt16*								channelListTypePtr,
				UInt16*								featurePtr,
				UInt16*								localNumberChannelsPtr,
				SInt16*								localOutlineFieldTypePtr,
				SInt16*								localLabelFieldCodePtr,
				SInt16*								outlineColorSelectionPtr,
				SInt16*								maskTrainImageSelectionPtr,
				SInt16*								maskTestImageSelectionPtr,
				UInt16*								maxNumberTrainLayersPtr,
				UInt16*								maxNumberTestLayersPtr);

extern void StatisticsDialogOK (
				SInt16								channelSelection,
				SInt16								totalNumberChannels,
				UInt16*								featurePtr,
				UInt32								localNumberChannels,
				Boolean								showTrainingFieldsFlag,
				Boolean								showTestFieldsFlag,
				Boolean								showClassNamesFlag,
				Boolean								showFieldNamesFlag,
				Boolean								showTrainTestTextFlag,
				SInt16								outlineColorSelection,
				SInt16								localStatCode,
				Boolean								keepClassStatsFlag,
				double								variance,
				double								minLogDeterminant,
				Boolean								useCommonCovarianceInLOOCFlag);

extern SInt16 StatisticsDialogSelectMaskItem (
				Handle*								maskFileInfoHandlePtr,
				DialogPtr							dialogPtr,
				MenuHandle							popUpSelectMaskImageMenu,
				SInt16								itemHit,
				SInt16								maskImageSelection,
				SInt16								selectStringNumber,
				SInt16								maskPopupItemNumber,
				SInt16								layerItemNumber,
				UInt16*								maxNumberLayersPtr);

extern SInt16 StatisticsDialogMaskCheck (
				Handle								trainMaskFileInfoHandle,
				Handle								testMaskFileInfoHandle,
				SInt16								maskTrainImageSelection,
				SInt16								maskTestImageSelection,
				UInt16								trainLayerNumber,
				UInt16								testLayerNumber);

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


/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg dialog

CMEditClassFieldDlg::CMEditClassFieldDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMEditClassFieldDlg::IDD, pParent, title) {
   //{{AFX_DATA_INIT(CMEditClassFieldDlg)
   m_className = "Class 1";
   m_fieldName = "Field 1";
   m_fieldType = 0;
   //}}AFX_DATA_INIT 

   m_classNameCStringPtr = NULL;
   m_fieldNameCStringPtr = NULL;

   try {
      //	Get pointers to the string buffers.	
//      m_classNameCStringPtr = m_className.GetBufferSetLength(64);
//      m_fieldNameCStringPtr = m_fieldName.GetBufferSetLength(64);
		m_classNameCStringPtr = m_classNamebuf;
		m_fieldNameCStringPtr = m_fieldNamebuf;
      
   }   catch (int e) {
      m_initializedFlag = FALSE;
   }

   CreateControls();
   SetSizerAndFit(bSizer285);
   m_initializedFlag = TRUE;
}

//void CMEditClassFieldDlg::DoDataExchange(CDataExchange* pDX) {
//   CMDialog::DoDataExchange(pDX);
//   //{{AFX_DATA_MAP(CMEditClassFieldDlg)        
//   DDX_Text(pDX, IDC_ClassName, m_className);
//   DDV_MaxChars(pDX, m_className, 31);
//   DDX_Text(pDX, IDC_FieldName, m_fieldName);
//   DDV_MaxChars(pDX, m_fieldName, 31);
//   DDX_Radio(pDX, IDC_Training, m_fieldType);
//   //}}AFX_DATA_MAP
//}

bool CMEditClassFieldDlg::TransferDataFromWindow(){
   wxTextCtrl* classname = (wxTextCtrl*) FindWindow(IDC_ClassName);
   wxTextCtrl* fieldname = (wxTextCtrl*) FindWindow(IDC_FieldName);
   wxRadioButton* training = (wxRadioButton*) FindWindow(IDC_Training);
   
   m_className = classname->GetValue();
   m_fieldName = fieldname->GetValue();
   
   strcpy( m_classNamebuf, (const char*)m_className.mb_str(wxConvUTF8) );
   strcpy( m_fieldNamebuf, (const char*)m_fieldName.mb_str(wxConvUTF8) ); 
   m_classNameCStringPtr = m_classNamebuf;
   m_fieldNameCStringPtr = m_fieldNamebuf;
   
   if(training->GetValue() == true)
      m_fieldType = 0;
   else
      m_fieldType = 1;
   
   return true;
}
// This actually not effective. Data transfer is done in OnInitDialog
bool CMEditClassFieldDlg::TransferDataToWindow(){
   wxTextCtrl* classname = (wxTextCtrl*) FindWindow(IDC_ClassName);
   wxTextCtrl* fieldname = (wxTextCtrl*) FindWindow(IDC_FieldName);
   wxRadioButton* training = (wxRadioButton*) FindWindow(IDC_Training);
   wxRadioButton* testfield = (wxRadioButton*) FindWindow(IDC_TestField);   

   classname->SetValue(m_className);
   fieldname->SetValue(m_fieldName);
   
   if (m_fieldType == 0)
      training->SetValue(true);
   else
      testfield->SetValue(true);
   
   return true;
}

BEGIN_EVENT_TABLE(CMEditClassFieldDlg, CMDialog)
EVT_INIT_DIALOG(CMEditClassFieldDlg::OnInitDialog)
EVT_TEXT_MAXLEN(IDC_ClassName, CMEditClassFieldDlg::CheckLength)
EVT_TEXT_MAXLEN(IDC_FieldName, CMEditClassFieldDlg::CheckLength)
//{{AFX_MSG_MAP(CMEditClassFieldDlg)               
//}}AFX_MSG_MAP
END_EVENT_TABLE()



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/19/96
//	Revised By:			Larry L. Biehl			Date: 06/16/2017	

Boolean CMEditClassFieldDlg::DoDialog(
				SInt16								classFieldCode,
				SInt16								currentClass,
				SInt16								currentField,
				char*									titleStringPtr) 
{
   Boolean OKFlag = FALSE;

   SInt16 returnCode;
   UInt16 selectedItem;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
																							return (FALSE);

   m_classFieldCode = classFieldCode;
   m_currentField = currentField;

   m_classStorage = gProjectInfoPtr->storageClass[currentClass];

   m_titleStringPtr = titleStringPtr;

   returnCode = ShowModal();

   if (returnCode == wxID_OK) 
		{
      OKFlag = TRUE;

      if (classFieldCode == 2) 
			{
					// Class Name
         CtoPstring ((UCharPtr)m_classNameCStringPtr,
							gProjectInfoPtr->classNamesPtr[m_classStorage].name);
			CtoPstring ((UCharPtr)m_classNameCStringPtr, gTextString);
			returnCode = CheckForDuplicateClassName (m_classStorage, (Str255*) gTextString);
         if (returnCode > 0) 
				{
            if (returnCode == 1)
               DisplayAlert(kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);

            else // returnCode == 2
               DupClassFieldNameAlert (1, gTextString);

            selectedItem = IDC_ClassName;
            OKFlag = FALSE;
				}	// end "if ( returnCode > 0 )"
				 
         else
				{ // returnCode == 0
						// change name in class list
            wxComboBox* classList = (wxComboBox*)gProjectWindow->GetFrame()->FindWindow(IDC_ClassList);
            wxString classname = wxString::FromUTF8 ((char*)m_classNameCStringPtr);
            classList->SetString (currentClass+1, classname);
				
				}
				
			}	// end "if (classFieldCode == 2)" 
      
      if (OKFlag && classFieldCode == 3) 
			{
         HPFieldIdentifiersPtr fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
			
					// Field Identifier
         CtoPstring ((UCharPtr)m_fieldNameCStringPtr,
							gProjectInfoPtr->fieldIdentPtr[m_currentField].name);
         CtoPstring ((UCharPtr)m_fieldNameCStringPtr, gTextString);
			
					// Training field or test field						
         SInt16 fieldType = kTrainingType;
         if (m_fieldType == 1)
            fieldType = kTestingType;
         fieldIdentPtr[currentField].fieldType = fieldType;
         returnCode = CheckForDuplicateFieldName (m_currentField, (Str255*) gTextString);
         if (returnCode > 0) 
				{
            if (returnCode == 1)
               DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);

            else // returnCode == 2
               DupClassFieldNameAlert (2, gTextString);

            selectedItem = IDC_FieldName;
            OKFlag = FALSE;
				
				}	// end "if ( returnCode > 0 )"      
				      
			}	// end "if (classFieldCode == 3)" 
			
		}	// end "if (returnCode == IDOK)"
   
	return OKFlag;
		  
}	// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg message handlers

void 
CMEditClassFieldDlg::OnInitDialog(wxInitDialogEvent& event) {
   UInt16 selectedItem;


   CMDialog::OnInitDialog(event);

   EditClassFieldDialogInitialize(this,
      m_classFieldCode,
      m_classStorage,
      m_currentField,
      &selectedItem);

//   SetWindowText(m_titleStringPtr);
   
      PositionDialogWindow();

   // Set default text selection 

   SelectDialogItemText(this, selectedItem, 0, SInt16_MAX);

//   return FALSE; // return TRUE  unless you set the focus to a control 

} // end "OnInitDialog"

void CMEditClassFieldDlg::CheckLength(wxCommandEvent& event){
   DisplayAlert(kErrorAlertID,
         kStopAlert,
         kAlertStrID,
         IDS_Alert116,
         0,
         NULL);
}

void 
CMEditClassFieldDlg::CreateControls(){
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
//	wxBoxSizer* bSizer285;
	bSizer285 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer286;
	bSizer286 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText265 = new wxStaticText( this, IDC_ClassListPrompt, wxT("Class:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText265->Wrap( -1 );
   SetUpToolTip(m_staticText265, IDS_ToolTip301);
	bSizer286->Add( m_staticText265, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox56 = new wxComboBox( this, IDC_ClassList, wxT("Combo!"), wxDefaultPosition, wxSize( 200,-1 ), 0, NULL, 0 ); 
   SetUpToolTip(m_comboBox56, IDS_ToolTip301);
	bSizer286->Add( m_comboBox56, 0, wxALL, 5 );
	
	
	bSizer285->Add( bSizer286, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer287;
	bSizer287 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText266 = new wxStaticText( this, IDC_ClassNamePrompt, wxT("Enter Class Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText266->Wrap( -1 );
   SetUpToolTip(m_staticText266, IDS_ToolTip302);
	bSizer287->Add( m_staticText266, 0, wxALL, 5 );
	
	m_textCtrl102 = new wxTextCtrl( this, IDC_ClassName, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), 0 );
   m_textCtrl102->SetMaxLength(31);
   SetUpToolTip(m_textCtrl102, IDS_ToolTip302);
	bSizer287->Add( m_textCtrl102, 0, wxALL, 5 );
	
	m_staticText267 = new wxStaticText( this, IDC_ClassNumberPixels, wxT("Number train pixels in class: 1000000"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText267->Wrap( -1 );
   SetUpToolTip(m_staticText267, IDS_ToolTip303);
	bSizer287->Add( m_staticText267, 0, wxLEFT, 25 );
	
	
	bSizer285->Add( bSizer287, 0, wxEXPAND|wxLEFT|wxRIGHT, 12 );
	
	wxBoxSizer* bSizer288;
	bSizer288 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText268 = new wxStaticText( this, IDC_FieldNamePrompt, wxT("Enter Field Identifier:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText268->Wrap( -1 );
   SetUpToolTip(m_staticText268, IDS_ToolTip304);
	bSizer288->Add( m_staticText268, 0, wxALL, 5 );
	
	m_textCtrl103 = new wxTextCtrl( this, IDC_FieldName, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), 0 );
   m_textCtrl103->SetMaxLength(31);
   SetUpToolTip(m_textCtrl103, IDS_ToolTip304);
	bSizer288->Add( m_textCtrl103, 0, wxALL, 5 );
	
	m_staticText269 = new wxStaticText( this, IDC_FieldNumberPixels, wxT("Number pixels: 1000000"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText269->Wrap( -1 );
   SetUpToolTip(m_staticText269, IDS_ToolTip305);
	bSizer288->Add( m_staticText269, 0, wxLEFT, 25 );
	
	
	bSizer285->Add( bSizer288, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 12 );
	
	wxBoxSizer* bSizer289;
	bSizer289 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer292;
	bSizer292 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText270 = new wxStaticText( this, IDC_AreaTypeOutline, wxT("AreaType:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText270->Wrap( -1 );
   SetUpToolTip(m_staticText270, IDS_ToolTip306);
	bSizer292->Add( m_staticText270, 0, wxALL, 5 );
	
	m_radioBtn27 = new wxRadioButton( this, IDC_Training, wxT("Training Field"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn27, IDS_ToolTip306);
	bSizer292->Add( m_radioBtn27, 0, wxALL, 5 );
	
	m_radioBtn28 = new wxRadioButton( this, IDC_TestField, wxT("Test Field"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn28, IDS_ToolTip306);
	bSizer292->Add( m_radioBtn28, 0, wxALL, 5 );
	
	
	bSizer289->Add( bSizer292, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer293;
	bSizer293 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button84 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer293->Add( m_button84, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 5 );
	
	m_button85 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer293->Add( m_button85, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 5 );
	
	
	bSizer289->Add( bSizer293, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxEXPAND, 5 );
	
	
	bSizer285->Add( bSizer289, 0, wxALL, 12 );
	
	
	this->SetSizer( bSizer285 );
	this->Layout();
	
	this->Centre( wxBOTH );
}
   

//void
//CMEditClassFieldDlg::OnOK(void) {
//   Boolean OKFlag = TRUE;
//   SInt16 returnCode;
//   UInt16 selectedItem;
//
//
//   UpdateData(TRUE);
//
//   if (m_classFieldCode == 2) {
//      CtoPstring(m_classNameCStringPtr, (char*) gTextString);
//
//      // Check if proposed class name is a duplicate.	 If it is, 	
//      // allow user to change it again.									
//
//      returnCode = CheckForDuplicateClassName(m_classStorage, (Str255*) gTextString);
//      if (returnCode > 0) {
//         if (returnCode == 1)
//            DisplayAlert(kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
//
//         else // returnCode == 2
//            DupClassFieldNameAlert(1, (char*) gTextString);
//
//         selectedItem = IDC_ClassName;
//         OKFlag = FALSE;
//
//      } // end "if ( returnCode > 0 )"                                     
//
//   } // end "if (m_classFieldCode == 2)" 
//
//   if (OKFlag && m_classFieldCode == 3) {
//      CtoPstring(m_fieldNameCStringPtr, (char*) gTextString);
//
//      // Check if proposed field name is a duplicate.	 If it  	
//      // is, allow user to change it again.							
//
//      returnCode = CheckForDuplicateFieldName(m_currentField, (Str255*) gTextString);
//      if (returnCode > 0) {
//         if (returnCode == 1)
//            DisplayAlert(kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
//
//         else // returnCode == 2
//            DupClassFieldNameAlert(2, (char*) gTextString);
//
//         selectedItem = IDC_FieldName;
//         OKFlag = FALSE;
//
//      } // end "if ( returnCode > 0 )"                                        
//
//   } // end "if (OKFlag && ...)"
//
//   if (OKFlag)
//      CMDialog::OnOK();
//
//   else // !OKFlag                      
//      SelectDialogItemText(this, selectedItem, 0, SInt16_MAX);
//
//} // end "OnOK"


/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDialog dialog

CMStatisticsDialog::CMStatisticsDialog(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMStatisticsDialog::IDD, pParent, title) 
{
   m_showFieldNames = FALSE;
   m_showTestFields = FALSE;
   m_showTrainingFields = FALSE;
   m_projectCommands = 0;
   m_showClassNames = FALSE;
   m_showTrainTestText = FALSE;
   m_outlineColorSelection = -1;
   m_testMaskCombo = -1;
   m_trainMaskCombo = -1;

   m_totalNumberChannels = 0;

   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls();
   SetSizerAndFit(bSizer170);
}


BEGIN_EVENT_TABLE(CMStatisticsDialog, CMDialog)
EVT_INIT_DIALOG(CMStatisticsDialog::OnInitDialog)
EVT_COMBOBOX(IDC_Channels, CMStatisticsDialog::OnSelendokChannels)
EVT_COMBOBOX(IDC_ProjectChanges, CMStatisticsDialog::OnSelendokProjectChanges)
EVT_COMBOBOX_DROPDOWN(IDC_ProjectChanges, CMStatisticsDialog::OnDropdownProjectChanges)
EVT_COMBOBOX_DROPDOWN(IDC_Channels, CMStatisticsDialog::OnDropdownChannels)
EVT_BUTTON(IDC_StatisticsOptions, CMStatisticsDialog::OnStatisticsOptions)
EVT_CHECKBOX(IDC_ShowClassNames, CMStatisticsDialog::OnShowClassNames)
EVT_CHECKBOX(IDC_ShowFieldNames, CMStatisticsDialog::OnShowFieldNames)
EVT_CHECKBOX(IDC_TestFields, CMStatisticsDialog::OnTestFields)
EVT_CHECKBOX(IDC_TrainingFields, CMStatisticsDialog::OnTrainingFields)
EVT_CHECKBOX(IDC_ShowTrainTestLabel, CMStatisticsDialog::OnShowTrainTestLabel)
EVT_COMBOBOX_DROPDOWN(IDC_TestMaskPopUp, CMStatisticsDialog::OnDropdownTestMaskCOMBO)
EVT_COMBOBOX(IDC_TestMaskPopUp, CMStatisticsDialog::OnSelendokTestMaskCOMBO)
EVT_COMBOBOX_DROPDOWN(IDC_TrainMaskPopUp, CMStatisticsDialog::OnDropdownTrainMaskCOMBO)
EVT_COMBOBOX(IDC_TrainMaskPopUp, CMStatisticsDialog::OnSelendokTrainMaskCOMBO)
//EVT_BUTTON(wxID_OK, CMStatisticsDialog::OnOK)
END_EVENT_TABLE()


void CMStatisticsDialog::CreateControls() 
{
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);

   //wxBoxSizer* bSizer170;
   bSizer170 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer171;
   bSizer171 = new wxBoxSizer(wxHORIZONTAL);

   wxBoxSizer* bSizer173;
   bSizer173 = new wxBoxSizer(wxVERTICAL);

   m_comboBox28 = new wxComboBox(this, IDC_ProjectChanges, wxT("Combo!"), wxDefaultPosition, wxSize(160, -1), 0, NULL, wxCB_READONLY);
   m_comboBox28->Append(wxT("Project Changes"));
   m_comboBox28->Append(wxT("Clear Statistics..."));
   SetUpToolTip(m_comboBox28, IDS_ToolTip105);  
   bSizer173->Add(m_comboBox28, 0, wxALL, 5);

   wxBoxSizer* bSizer177;
   bSizer177 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText184 = new wxStaticText(this, wxID_ANY, wxT("Channels to Use:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText184->Wrap(-1);
   SetUpToolTip(m_staticText184, IDS_ToolTip50);
   bSizer177->Add(m_staticText184, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox29 = new wxComboBox(this, IDC_Channels, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
   m_comboBox29->Append(wxT("All"));
   m_comboBox29->Append(wxT("Subset..."));
   SetUpToolTip(m_comboBox29, IDS_ToolTip50);
   bSizer177->Add(m_comboBox29, 0, wxALL, 5);


   bSizer173->Add(bSizer177, 0, wxEXPAND, 5);

   m_button39 = new wxButton(this, IDC_StatisticsOptions, wxT("Project Statistics Options..."), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_button39, IDS_ToolTip106);
   bSizer173->Add(m_button39, 0, wxALIGN_CENTER | wxALL, 5);


   bSizer171->Add(bSizer173, 0, wxEXPAND, 5);

   wxStaticBoxSizer* sbSizer7;
   sbSizer7 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Outline selected areas:")), wxVERTICAL);

   m_checkBox28 = new wxCheckBox(sbSizer7->GetStaticBox(), IDC_TrainingFields, wxT("Training fields"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox28, IDS_ToolTip109);
   sbSizer7->Add(m_checkBox28, 0, wxLEFT, 15);

   m_checkBox29 = new wxCheckBox(sbSizer7->GetStaticBox(), IDC_TestFields, wxT("Test Fields"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox29, IDS_ToolTip110);
   sbSizer7->Add(m_checkBox29, 0, wxLEFT, 15);

   m_checkBox30 = new wxCheckBox(sbSizer7->GetStaticBox(), IDC_ShowClassNames, wxT("Show class names"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox30, IDS_ToolTip111);
   sbSizer7->Add(m_checkBox30, 0, wxLEFT, 15);

   m_checkBox31 = new wxCheckBox(sbSizer7->GetStaticBox(), IDC_ShowFieldNames, wxT("Show field names"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox31, IDS_ToolTip112);
   sbSizer7->Add(m_checkBox31, 0, wxLEFT, 15);

   m_checkBox32 = new wxCheckBox(sbSizer7->GetStaticBox(), IDC_ShowTrainTestLabel, wxT("Show Train/Test Label"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox32, IDS_ToolTip113);
   sbSizer7->Add(m_checkBox32, 0, wxLEFT, 15);

   wxBoxSizer* bSizer178;
   bSizer178 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText186 = new wxStaticText(sbSizer7->GetStaticBox(), IDC_ColorPrompt, wxT("Color:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText186->Wrap(-1);
   bSizer178->Add(m_staticText186, 0, wxALIGN_CENTER | wxLEFT, 20);

   m_comboBox30 = new wxComboBox(sbSizer7->GetStaticBox(), IDC_ColorCombo, wxT("Combo!"), wxDefaultPosition, wxSize(100, -1), 0, NULL, wxCB_READONLY);
   m_comboBox30->Append(wxT("Black"));
   m_comboBox30->Append(wxT("White"));
   SetUpToolTip(m_comboBox30, IDS_ToolTip114);
   bSizer178->Add(m_comboBox30, 0, wxALL, 5);

   sbSizer7->Add(bSizer178, 0, wxEXPAND, 5);
   bSizer171->Add(sbSizer7, 0, wxEXPAND | wxLEFT, 25);

   bSizer170->Add(bSizer171, 0, wxALL | wxEXPAND, 12);

   wxBoxSizer* bSizer172;
   bSizer172 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer175;
   bSizer175 = new wxBoxSizer(wxVERTICAL);

   //wxBoxSizer* bSizer175_1;
   //bSizer175_1 = new wxBoxSizer(wxHORIZONTAL);
	
   wxStaticText* m_staticText175_1 = new wxStaticText(this, IDC_LayerPrompt, wxT("Layer"), wxDefaultPosition, wxDefaultSize, 0);
   //bSizer175_1->Add (m_staticText175_1, 0, wxEXPAND | wxALIGN_RIGHT, 0);
   bSizer175->Add (m_staticText175_1, wxSizerFlags(0).Right());

   wxBoxSizer* bSizer179;
   bSizer179 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText187 = new wxStaticText(this, IDC_TrainMaskPrompt, wxT("Training mask file: "), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText187->Wrap(-1);
   SetUpToolTip(m_staticText187, IDS_ToolTip107);
   bSizer179->Add(m_staticText187, 0, wxALIGN_CENTER | wxALL, 5);

   m_comboBox31 = new wxComboBox(this, IDC_TrainMaskPopUp, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
   m_comboBox31->Append(wxT("None"));
   m_comboBox31->Append(wxT("Select mask image..."));
   SetUpToolTip(m_comboBox31, IDS_ToolTip107);
   bSizer179->Add(m_comboBox31, 0, wxALL, 5);

   m_textCtrl90 = new wxTextCtrl (this, IDC_TrainMaskLayer, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   bSizer179->Add(m_textCtrl90, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer175->Add(bSizer179, 1, wxEXPAND, 5);

   wxBoxSizer* bSizer180;
   bSizer180 = new wxBoxSizer(wxHORIZONTAL);

   m_staticText188 = new wxStaticText(this, IDC_TestMaskPrompt, wxT("Test mask file:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText188->Wrap(-1);
   SetUpToolTip(m_staticText188, IDS_ToolTip108);
   bSizer180->Add(m_staticText188, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer180->Add(0, 0, 0, wxLEFT, 25);

   m_comboBox32 = new wxComboBox(this, IDC_TestMaskPopUp, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
   m_comboBox32->Append(wxT("None"));
   m_comboBox32->Append(wxT("Select mask image..."));
   SetUpToolTip(m_comboBox32, IDS_ToolTip108);
   bSizer180->Add(m_comboBox32, 0, wxALL, 5);

   m_textCtrl91 = new wxTextCtrl(this, IDC_TestMaskLayer, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   bSizer180->Add(m_textCtrl91, 0, wxALIGN_CENTER | wxALL, 5);

   bSizer175->Add(bSizer180, 1, wxEXPAND, 5);
   bSizer172->Add(bSizer175, 0, wxEXPAND, 5);

   wxBoxSizer* bSizer176;
   bSizer176 = new wxBoxSizer (wxHORIZONTAL);
   m_button40 = new wxButton (this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
   //bSizer176->Add (m_button40, 0, wxALL, 5);
	bSizer176->Add (m_button40, wxSizerFlags(0).Border(wxRIGHT,6));

   m_button41 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
   //bSizer176->Add(m_button41, 0, wxALL, 5);
	bSizer176->Add (m_button41, wxSizerFlags(0));

   //bSizer172->Add(bSizer176, 0, wxALIGN_RIGHT, 5);
	bSizer172->Add (bSizer176, wxSizerFlags(0).Right().Border(wxRIGHT,0).Border(wxTOP,6));
	
   bSizer170->Add (bSizer172, 0, wxALL | wxEXPAND, 12);

   this->SetSizer(bSizer170);
   this->Layout();

   this->Centre(wxBOTH);

}	// end "CreateControls"
/*
   rightColumnBoxSizer->Add(sbSizer18, 0, wxEXPAND, 5);
	
	wxBoxSizer* bSizer77 = new wxBoxSizer(wxHORIZONTAL);
	wxButton* cancelbtn = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer77->Add (cancelbtn, wxSizerFlags(0).Border(wxRIGHT,6));
	wxButton* okbtn = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer77->Add (okbtn, wxSizerFlags(0));
	rightColumnBoxSizer->Add (bSizer77, wxSizerFlags(0).Right().Border(wxRIGHT,0).Border(wxTOP,42));
*/

//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
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
//	Called By:			StatisticsDialog in SStatist.cpp
//
//	Coded By:			Abdur Rachman Maud	Date: ??/??/2015
//	Revised By:			Larry L. Biehl			Date: 06/21/2017	

SInt16 CMStatisticsDialog::DoDialog (
				SInt16*								featurePtr,
				SInt16								totalNumberChannels,
				Handle*								trainMaskFileInfoHandlePtr,
				Handle*								testMaskFileInfoHandlePtr) 
{
	Boolean								continueFlag = FALSE;

   SInt16								returnCode,
											statisticsRequest;


			// Make sure intialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   m_localFeaturesPtr = (UInt16*) featurePtr;
   m_totalNumberChannels = totalNumberChannels;
   m_trainMaskFileInfoHandle = *trainMaskFileInfoHandlePtr;
   m_testMaskFileInfoHandle = *testMaskFileInfoHandlePtr;

   returnCode = ShowModal();

   if (returnCode == wxID_OK) 
		{
      statisticsRequest = 1;

      StatisticsDialogOK (m_channelSelection,
									m_totalNumberChannels,
									m_localActiveFeaturesPtr,
									m_localActiveNumberFeatures,
									m_showTrainingFields,
									m_showTestFields,
									m_showClassNames,
									m_showFieldNames,
									m_showTrainTestText,
									m_outlineColorSelection + 1,
									m_localStatCode,
									m_keepClassStatsFlag,
									m_variance,
									m_minLogDeterminant,
									m_useCommonCovarianceInLOOCFlag);

      continueFlag = TRUE;

		}	// end "if (returnCode == IDOK)"

   CheckAndUnlockHandle(gProjectInfoPtr->windowInfoHandle);

   *trainMaskFileInfoHandlePtr = m_trainMaskFileInfoHandle;
   *testMaskFileInfoHandlePtr = m_testMaskFileInfoHandle;

   return (continueFlag);

}	// end "DoDialog"


void CMStatisticsDialog::OnDropdownChannels (
				wxCommandEvent&					event) 
{
   UpdateAllSubsetList(IDC_Channels);

}	// end "OnDropdownChannels" 


void CMStatisticsDialog::OnDropdownProjectChanges (
				wxCommandEvent&					event) 
{
   if (gProjectInfoPtr->statsLoaded) 
		{
				// Make sure that the clear statistics command is in the list.

      wxComboBox* projchange = (wxComboBox*) FindWindow(IDC_ProjectChanges);
      if (projchange->GetCount() == 1)
         projchange->Append(wxT("Clear Statistics..."));

		}	// end "if (gProjectInfoPtr->statsLoaded)" 

   else // !gProjectInfoPtr->statsLoaded
		{
				// Make sure that the clear statistics command is not in the list.

      wxComboBox* projchange = (wxComboBox*) FindWindow(IDC_ProjectChanges);
      if (projchange->GetCount() == 2)
         projchange->Delete(1);

		}	// end "else !gProjectInfoPtr->statsLoaded"

}	// end "OnDropdownProjectChanges"
 

void CMStatisticsDialog::OnDropdownTestMaskCOMBO (
				wxCommandEvent&					event) 
{

}	// end "OnDropdownTestMaskCOMBO" 


void CMStatisticsDialog::OnDropdownTrainMaskCOMBO (
				wxCommandEvent&					event) 
{

}	// end "OnDropdownTrainMaskCOMBO"


void CMStatisticsDialog::OnInitDialog (
				wxInitDialogEvent&				event) 
{
	SInt16								localChannelSelection,
											localLabelFieldCode,
											localOutlineColorSelection,
											localOutlineFieldType,
											localTestMaskCombo,
											localTrainMaskCombo;


   //CMDialog::OnInitDialog();

			// Initialize dialog variables.

	StatisticsDialogInitialize (this,
											m_totalNumberChannels,
											&m_localStatCode,
											&m_keepClassStatsFlag,
											&m_useCommonCovarianceInLOOCFlag,
											&m_variance,
											&m_minLogDeterminant,
											&localChannelSelection,
											&m_channelListType,
											m_localFeaturesPtr,
											&m_localNumberFeatures,
											&localOutlineFieldType,
											&localLabelFieldCode,
											&localOutlineColorSelection,
											&localTrainMaskCombo,
											&localTestMaskCombo,
											&m_maxNumberTrainLayers,
											&m_maxNumberTestLayers);

			// Set feature parameters 

   m_localActiveNumberFeatures = m_localNumberFeatures;
   m_localActiveFeaturesPtr = m_localFeaturesPtr;
   m_channelSelection = localChannelSelection;

			// Outline training areas.															

   m_showTrainingFields = (localOutlineFieldType & 0x0001);

			// Outline test areas.																

   m_showTestFields = (localOutlineFieldType & 0x0002);

			// Put labels with the area outlines.              

   m_showClassNames = (localLabelFieldCode & 0x0001);

   m_showFieldNames = FALSE;
   if (localLabelFieldCode & 0x0002)
      m_showFieldNames = TRUE;

   m_showTrainTestText = FALSE;
   if (localLabelFieldCode & 0x0004)
      m_showTrainTestText = TRUE;

   m_outlineColorSelection = localOutlineColorSelection;

			// Make the train and test mask combo variables to be 0 based.

   m_trainMaskCombo = localTrainMaskCombo - 1;
   m_testMaskCombo = localTestMaskCombo - 1;

			// Make the outline color combo variable to be 0 based.

   m_outlineColorSelection--;

			// Update the dialog box parameters and then center the dialog.

   if (TransferDataToWindow())
      PositionDialogWindow();

   SetOutlineAreaOptions();

   //return TRUE; // return TRUE  unless you set the focus to a control

}	// end "OnInitDialog" 


void CMStatisticsDialog::OnSelendokChannels (
				wxCommandEvent&					event) 
{
   HandleChannelsMenu (IDC_Channels,
								kNoTransformation,
								m_totalNumberChannels,
								kImageChannelType,
								TRUE);

}	// end "OnSelendokChannels"


void CMStatisticsDialog::OnSelendokProjectChanges (
				wxCommandEvent&					event) 
{
   wxComboBox* projchange = (wxComboBox*) FindWindow(IDC_ProjectChanges);
   m_projectCommands = projchange->GetSelection();

   if (m_projectCommands == 1) 
		{
      SInt16 returnCode = m_channelListType;
      if (ProjectMenuClearStatistics())
         returnCode = 1;

				// If project stats were cleared, then reload the local	
				// feature vector to include all channels.					

      if (m_channelListType == kSelectedItemsListOnly && returnCode == 1) 
			{
         SInt16 index;

         m_channelListType = 1;
         m_channelSelection = kAllMenuItem;
         for (index = 0; index < gProjectInfoPtr->numberStatisticsChannels; index++)
            m_localFeaturesPtr[index] = index;
         m_localActiveNumberFeatures = gProjectInfoPtr->numberStatisticsChannels;

         UpdateAllSubsetList(IDC_Channels);

			wxComboBox* channsel = (wxComboBox*) FindWindow(IDC_Channels);
         channsel->SetSelection(m_projectCommands);
			
			}	// end "if (m_channelListType == ..."

      m_projectCommands = 0;
      projchange->SetSelection(m_projectCommands);
		
		}	// end "if (m_projectCommands == 1)"

}	// end "OnSelendokProjectChanges"


void CMStatisticsDialog::OnSelendokTestMaskCOMBO (
				wxCommandEvent&					event) 
{
	SInt16								itemHit,
											savedTestMaskCombo;


   savedTestMaskCombo = m_testMaskCombo;

   wxComboBox* testmask = (wxComboBox*)FindWindow (IDC_TestMaskPopUp);
   m_testMaskCombo = testmask->GetSelection();

   if (m_testMaskCombo >= 0) 
		{
      itemHit = StatisticsDialogSelectMaskItem (
								&m_testMaskFileInfoHandle,
								this,
								NULL,
								m_testMaskCombo + 1,
								savedTestMaskCombo + 1,
								IDS_FileIO96,
								IDC_TestMaskPopUp,
								IDC_TestMaskLayer,
								&m_maxNumberTestLayers);

      m_testMaskCombo = itemHit - 1;

      testmask->SetSelection(m_testMaskCombo);
		
		if (m_trainMaskCombo != 0 ||  m_testMaskCombo != 0)
			ShowDialogItem (this, IDC_LayerPrompt);
		else
			HideDialogItem (this, IDC_LayerPrompt);

		}	// end "if (m_testMaskCombo >= 0)"

}	// end "OnSelendokTestMaskCOMBO"


void CMStatisticsDialog::OnSelendokTrainMaskCOMBO (
				wxCommandEvent&					event)  
{
	SInt16								itemHit,
											savedTrainMaskCombo;


   savedTrainMaskCombo = m_trainMaskCombo;

   wxComboBox* trainmask = (wxComboBox*)FindWindow (IDC_TrainMaskPopUp);
   m_trainMaskCombo = trainmask->GetSelection();

   if (m_trainMaskCombo >= 0) 
		{
      itemHit = StatisticsDialogSelectMaskItem (
							&m_trainMaskFileInfoHandle,
							this,
							NULL,
							m_trainMaskCombo + 1,
							savedTrainMaskCombo + 1,
							IDS_FileIO95,
							IDC_TrainMaskPopUp,
							IDC_TrainMaskLayer,
							&m_maxNumberTrainLayers);

      m_trainMaskCombo = itemHit - 1;

      trainmask->SetSelection (m_trainMaskCombo);
		
		if (m_trainMaskCombo != 0 ||  m_testMaskCombo != 0)
			ShowDialogItem (this, IDC_LayerPrompt);
		else
			HideDialogItem (this, IDC_LayerPrompt);
		
		}	// end "if (m_trainMaskCombo >= 0)"

}	// end "OnSelendokTrainMaskCOMBO" 


void CMStatisticsDialog::OnShowClassNames (
				wxCommandEvent&					event) 
{
   wxCheckBox* classname = (wxCheckBox*) FindWindow(IDC_ShowClassNames);
   m_showClassNames = classname->GetValue();

}	// end "OnShowClassNames"


void CMStatisticsDialog::OnShowFieldNames (
				wxCommandEvent&					event) 
{
   wxCheckBox* fieldname = (wxCheckBox*) FindWindow(IDC_ShowFieldNames);
   m_showFieldNames = fieldname->GetValue();

}	// end "OnShowFieldNames" 


void CMStatisticsDialog::OnShowTrainTestLabel (
				wxCommandEvent&					event) 
{
   wxCheckBox* traintest = (wxCheckBox*) FindWindow(IDC_ShowTrainTestLabel);
   m_showTrainTestText = traintest->GetValue();
	
}	// end "OnShowTrainTestLabel" 


void CMStatisticsDialog::OnStatisticsOptions (
				wxCommandEvent&					event) 
{
   SetDLogControlHilite(NULL, wxID_OK, 255);

   StatisticsOptionsDialog(
      &m_localStatCode,
      &m_keepClassStatsFlag,
      &m_variance,
      &m_minLogDeterminant,
      &m_useCommonCovarianceInLOOCFlag);

   SetDLogControlHilite(NULL, wxID_OK, 0);

}	// end "OnStatisticsOptions" 


void CMStatisticsDialog::OnTestFields (
				wxCommandEvent&					event) 
{
   wxCheckBox* testfield = (wxCheckBox*) FindWindow(IDC_TestFields);
   m_showTestFields = testfield->GetValue();
   SetOutlineAreaOptions();

}	// end "OnTestFields"   
   

void CMStatisticsDialog::OnTrainingFields (
				wxCommandEvent&					event) 
{
   wxCheckBox* trainfield = (wxCheckBox*) FindWindow(IDC_TrainingFields);
   m_showTrainingFields = trainfield->GetValue();

   SetOutlineAreaOptions();

} // end "OnTrainingFields"  


void CMStatisticsDialog::SetOutlineAreaOptions(void) 
{
   if (m_showTrainingFields || m_showTestFields) 
		{
      wxCheckBox* classname = (wxCheckBox*) FindWindow(IDC_ShowClassNames);
      wxCheckBox* showlabel = (wxCheckBox*) FindWindow(IDC_ShowTrainTestLabel);
      wxCheckBox* fieldname = (wxCheckBox*) FindWindow(IDC_ShowFieldNames);
      classname->SetValue(m_showClassNames);
      showlabel->SetValue(m_showTrainTestText);
      fieldname->SetValue(m_showFieldNames);

      SetDLogControlHilite(this, IDC_ShowClassNames, 0);
      SetDLogControlHilite(this, IDC_ShowFieldNames, 0);
      SetDLogControlHilite(this, IDC_ShowTrainTestLabel, 0);

      ShowDialogItem (this, IDC_ColorPrompt);
      ShowDialogItem (this, IDC_ColorCombo);

		}	// end "if (m_showTrainingFields || m_showTestFields)"

   else // !m_showTrainingFields && !m_showTestFields 
		{
      bool falseSetting = FALSE;

      wxCheckBox* classname = (wxCheckBox*) FindWindow(IDC_ShowClassNames);
      wxCheckBox* showlabel = (wxCheckBox*) FindWindow(IDC_ShowTrainTestLabel);
      wxCheckBox* fieldname = (wxCheckBox*) FindWindow(IDC_ShowFieldNames);
      classname->SetValue(false);
      showlabel->SetValue(false);
      fieldname->SetValue(false);

      SetDLogControlHilite(this, IDC_ShowClassNames, 255);
      SetDLogControlHilite(this, IDC_ShowFieldNames, 255);
      SetDLogControlHilite(this, IDC_ShowTrainTestLabel, 255);

      HideDialogItem (this, IDC_ColorPrompt);
      HideDialogItem (this, IDC_ColorCombo);

		}	// end "else !m_showTrainingFields && !m_showTestFields"

}	// end "SetOutlineAreaOptions"


bool CMStatisticsDialog::TransferDataFromWindow() 
{
	SInt16								itemHit = 1;
	
	UInt16								testLayerNumber,
											trainLayerNumber;
											
											
   wxCheckBox* fieldname = (wxCheckBox*)FindWindow(IDC_ShowFieldNames);
   wxCheckBox* testfield = (wxCheckBox*)FindWindow(IDC_TestFields);
   wxCheckBox* trainfield = (wxCheckBox*)FindWindow(IDC_TrainingFields);
   wxCheckBox* classname = (wxCheckBox*)FindWindow(IDC_ShowClassNames);
   wxCheckBox* showlabel = (wxCheckBox*)FindWindow(IDC_ShowTrainTestLabel);

   wxComboBox* projchange = (wxComboBox*)FindWindow(IDC_ProjectChanges);
   wxComboBox* colorcombo = (wxComboBox*)FindWindow(IDC_ColorCombo);
   wxComboBox* channelcb = (wxComboBox*)FindWindow(IDC_Channels);
   wxComboBox* testmask = (wxComboBox*)FindWindow(IDC_TestMaskPopUp);
   wxComboBox* trainmask = (wxComboBox*)FindWindow(IDC_TrainMaskPopUp);

   m_showFieldNames = fieldname->GetValue();
   m_showTestFields = testfield->GetValue();
   m_showTrainingFields = trainfield->GetValue();
   m_showClassNames = classname->GetValue();
   m_showTrainTestText = showlabel->GetValue();

   m_projectCommands = projchange->GetSelection();
   m_outlineColorSelection = colorcombo->GetSelection();
   m_channelSelection = channelcb->GetSelection();
   m_testMaskCombo = testmask->GetSelection();
   m_trainMaskCombo = trainmask->GetSelection();

			// Check channels used for train and test mask

	trainLayerNumber = 1;
	if (m_trainMaskCombo == 2 && m_maxNumberTrainLayers > 1)
		{
		itemHit = CheckMaxValue (this,
										  IDC_TrainMaskLayer,
										  1,
										  m_maxNumberTrainLayers,
										  kDisplayRangeAlert);
											  
		trainLayerNumber =  GetDItemValue (this, IDC_TrainMaskLayer);
											  
		}	// end "if (itemHit == 1 && gMaskTrainImageSelection == 3 && ..."
											  
	testLayerNumber = 1;
	if (itemHit == 1 && m_testMaskCombo == 2 && m_maxNumberTestLayers > 1)
		{
		itemHit = CheckMaxValue (this,
										  IDC_TestMaskLayer,
										  1,
										  m_maxNumberTestLayers,
										  kDisplayRangeAlert);	
											  
		testLayerNumber =  GetDItemValue (this, IDC_TestMaskLayer);
					
		}	// end "if (itemHit == 1 && gMaskTestImageSelection == 3 && ..."

	if (itemHit == 1)
		itemHit = StatisticsDialogMaskCheck (
									m_trainMaskFileInfoHandle,
									m_testMaskFileInfoHandle,
									m_trainMaskCombo + 1,
									m_testMaskCombo + 1,
									trainLayerNumber,
									testLayerNumber);

   if (itemHit == 1)
      return true;
   else
      return false;
		
}	// end "TransferDataFromWindow"


bool CMStatisticsDialog::TransferDataToWindow() 
{
   wxCheckBox* testfield = (wxCheckBox*) FindWindow(IDC_TestFields);
   wxCheckBox* trainfield = (wxCheckBox*) FindWindow(IDC_TrainingFields);
   wxCheckBox* classname = (wxCheckBox*) FindWindow(IDC_ShowClassNames);
   wxCheckBox* showlabel = (wxCheckBox*) FindWindow(IDC_ShowTrainTestLabel);
   wxCheckBox* fieldname = (wxCheckBox*) FindWindow(IDC_ShowFieldNames);

   wxComboBox* projchange = (wxComboBox*) FindWindow(IDC_ProjectChanges);
   wxComboBox* colorcombo = (wxComboBox*) FindWindow(IDC_ColorCombo);
   wxComboBox* channelcb = (wxComboBox*) FindWindow(IDC_Channels);
   wxComboBox* testmask = (wxComboBox*) FindWindow(IDC_TestMaskPopUp);
   wxComboBox* trainmask = (wxComboBox*) FindWindow(IDC_TrainMaskPopUp);

   testfield->SetValue(m_showTestFields);
   trainfield->SetValue(m_showTrainingFields);
   classname->SetValue(m_showClassNames);
   showlabel->SetValue(m_showTrainTestText);
   fieldname->SetValue(m_showFieldNames);

   projchange->SetSelection(m_projectCommands);
   colorcombo->SetSelection(m_outlineColorSelection);
   channelcb->SetSelection(m_channelSelection);
   testmask->SetSelection(m_testMaskCombo);
   trainmask->SetSelection(m_trainMaskCombo);

   return true;
	
}	// end "TransferDataToWindow"



/////////////////////////////////////////////////////////////////////////////
// CMStatOptionsDlg dialog

CMStatOptionsDlg::CMStatOptionsDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMStatOptionsDlg::IDD, pParent, title) 
{
   m_classStatsOnlyFlag = FALSE;
   m_setZeroVarianceFlag = FALSE;
   m_statCode = -1;
   m_minLogDetValue = 0;
   m_varianceValue = 0;
   m_useCommonCovarianceInLOOCFlag = FALSE;
   //}}AFX_DATA_INIT

   m_initializedFlag = CMDialog::m_initializedFlag;
   CreateControls();
   SetSizerAndFit(bSizer181);
}


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


bool CMStatOptionsDlg::TransferDataFromWindow() 
{
   SInt16 returnCode = 0;
   wxCheckBox* classstateonly = (wxCheckBox*) FindWindow(IDC_ClassStatsOnly);
   wxCheckBox* zerovariance = (wxCheckBox*) FindWindow(IDC_SetZeroVariance);
   wxCheckBox* commoncov = (wxCheckBox*) FindWindow(IDC_UseCommonCov);
   wxRadioButton* meanstd = (wxRadioButton*) FindWindow(IDC_MeanStd);
   wxRadioButton* meanstdcov = (wxRadioButton*) FindWindow(IDC_meanStdCov);
   wxTextCtrl* minlogdet = (wxTextCtrl*) FindWindow(IDC_minLogDetValue);
   wxTextCtrl* variance = (wxTextCtrl*) FindWindow(IDC_Variance);
   
   m_classStatsOnlyFlag = classstateonly->GetValue();
   m_setZeroVarianceFlag = zerovariance->GetValue();
   m_useCommonCovarianceInLOOCFlag = commoncov->GetValue();
   
   wxString minlogdetString = minlogdet->GetValue();
   minlogdetString.ToDouble(&m_minLogDetValue);
   
   wxString varianceString = variance->GetValue();
   varianceString.ToDouble(&m_varianceValue);   
   
   returnCode = CheckDialogRealValue (this,
													IDC_Variance,
													0.,
													10000.,
													0,
													true,		// minValue is allowed.
													true,		// maxValue is allowed,
													kDisplayRangeAlert);
   
   return (returnCode == 0);
}


BEGIN_EVENT_TABLE(CMStatOptionsDlg, CMDialog)
EVT_INIT_DIALOG(CMStatOptionsDlg::OnInitDialog)
EVT_RADIOBUTTON(IDC_MeanStd, CMStatOptionsDlg::OnMeanStd)
EVT_RADIOBUTTON(IDC_meanStdCov, CMStatOptionsDlg::OnMeanStdCov)
EVT_CHECKBOX(IDC_SetZeroVariance, CMStatOptionsDlg::OnSetZeroVariance)
END_EVENT_TABLE()


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
//	Revised By:			Larry L. Biehl			Date: 02/02/1998	

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

   if (returnCode == wxID_OK) {
      StatisticsOptionsDialogOK(m_statCode + 1,
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

   } // end "if (returnCode == IDOK)" 

   return (continueFlag);

}	// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMStatOptionsDlg message handlers

void CMStatOptionsDlg::OnInitDialog(wxInitDialogEvent& event) {
   //CDialog::OnInitDialog();

   if (m_statCode + 1 == kMeanCovariance) {
      SetDLogControl(this, IDC_SetZeroVariance, m_setZeroVarianceFlag);

      ShowHideDialogItem(this, IDC_Variance, m_setZeroVarianceFlag);

      ShowDialogItem(this, IDC_MinLogPrompt);
      ShowDialogItem(this, IDC_minLogDetValue);

   }// end "if (m_statCode == kMeanCovariance)" 

   else // m_statCode+1 == kMeanStdOnly 
   {
      SetDLogControl(this, IDC_SetZeroVariance, 0);
      SetDLogControlHilite(this, IDC_SetZeroVariance, 255);
      HideDialogItem(this, IDC_Variance);
      HideDialogItem(this, IDC_MinLogPrompt);
      HideDialogItem(this, IDC_minLogDetValue);

      SetDLogControl(this, IDC_UseCommonCov, 0);
      SetDLogControlHilite(this, IDC_UseCommonCov, 255);

   } // end "else m_statCode+1 == kMeanStdOnly"

   // Center the dialog and then show it.	

   // Update the dialog box parameters and then center the dialog.

   if (TransferDataToWindow())
      PositionDialogWindow();

   SInt16 selectedItem = IDC_minLogDetValue;
   if (m_statCode + 1 == kMeanCovariance && m_setZeroVarianceFlag)
      selectedItem = IDC_Variance;

   // Set default text selection to first edit text item 

   //	GetDlgItem(selectedItem)->SetFocus();
   //	SendDlgItemMessage( selectedItem, EM_SETSEL, 0, MAKELPARAM(0, -1) );
   SelectDialogItemText(this, selectedItem, 0, SInt16_MAX);

   //return FALSE; // return TRUE  unless you set the focus to a control

} // end "OnInitDialog"

void
CMStatOptionsDlg::OnMeanStd(wxCommandEvent& event) {
   if (ProjectMenuClearStatistics()) {
      if (m_statCode + 1 == kMeanCovariance) {
         SetDLogControl(this, IDC_SetZeroVariance, 0);
         SetDLogControlHilite(this, IDC_SetZeroVariance, 255);
         HideDialogItem(this, IDC_Variance);
         HideDialogItem(this, IDC_MinLogPrompt);
         HideDialogItem(this, IDC_minLogDetValue);

         m_statCode = kMeanStdDevOnly - 1;

      }// end "if ( m_statCode+1 == kMeanCovariance )"

      else // m_statCode+1 == kMeanStdDevOnly
      {
         SetDLogControl(this, IDC_SetZeroVariance, m_setZeroVarianceFlag);
         SetDLogControlHilite(this, IDC_SetZeroVariance, 0);

         SetZeroVariance();

         ShowDialogItem(this, IDC_MinLogPrompt);
         ShowDialogItem(this, IDC_minLogDetValue);

         m_statCode = kMeanCovariance - 1;

      } // end "else m_statCode+1 == kMeanStdDevOnly"

   }// end "if ( ProjectMenuClearStatistics () )"

   else { // !ProjectMenuClearStatistics()
      // User cancelled the operation. Set back to previous value.	 
      //DDX_Radio(m_dialogToPtr, IDC_MeanStd, m_statCode);
      wxRadioButton* meanstd = (wxRadioButton*) FindWindow(IDC_MeanStd);
      wxRadioButton* meanstdcov = (wxRadioButton*) FindWindow(IDC_meanStdCov);
      if (m_statCode == 1) {
         meanstdcov->SetValue(true);
         meanstd->SetValue(false);
      } else {
         meanstdcov->SetValue(false);
         meanstd->SetValue(true);
      }
   }

} // end "OnMeanStd" 

void
CMStatOptionsDlg::OnSetZeroVariance(wxCommandEvent& event) {
   //DDX_Check(m_dialogFromPtr, IDC_SetZeroVariance, m_setZeroVarianceFlag);
   wxCheckBox* setzerovar = (wxCheckBox*) FindWindow(IDC_SetZeroVariance);
   m_setZeroVarianceFlag = setzerovar->GetValue();

   SetZeroVariance();

} // end "OnSetZeroVariance"

void
CMStatOptionsDlg::OnMeanStdCov(wxCommandEvent& event) {
   OnMeanStd(event);

} // end "OnMeanStdCov" 

void
CMStatOptionsDlg::SetZeroVariance() {
   ShowHideDialogItem(this, IDC_Variance, m_setZeroVarianceFlag);
   if (m_setZeroVarianceFlag) {
      //		GetDlgItem(IDC_Variance)->SetFocus();
      //		SendDlgItemMessage( IDC_Variance, EM_SETSEL, 0, MAKELPARAM(0, -1) );
      SelectDialogItemText(this, IDC_Variance, 0, SInt16_MAX);

   } // end "if (m_setZeroVarianceFlag)"

} // end "SetZeroVariance"

void
CMStatOptionsDlg::CreateControls() {
   this->SetSizeHints(wxDefaultSize, wxDefaultSize);

   //wxBoxSizer* bSizer181;
   bSizer181 = new wxBoxSizer(wxVERTICAL);

   wxBoxSizer* bSizer182;
   bSizer182 = new wxBoxSizer(wxVERTICAL);

   m_staticText193 = new wxStaticText(this, wxID_ANY, wxT("Compute statistics for:"), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText193->Wrap(-1);
   bSizer182->Add(m_staticText193, 0, wxALL, 5);

   m_radioBtn17 = new wxRadioButton(this, IDC_MeanStd, wxT("Mean and standard deviation"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn17, IDS_ToolTip115);
   bSizer182->Add(m_radioBtn17, 0, wxLEFT, 20);

   m_radioBtn18 = new wxRadioButton(this, IDC_meanStdCov, wxT("Mean, standard deviation and covariance"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_radioBtn18, IDS_ToolTip116);
   bSizer182->Add(m_radioBtn18, 0, wxLEFT, 20);


   bSizer181->Add(bSizer182, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 12);

   wxBoxSizer* bSizer183;
   bSizer183 = new wxBoxSizer(wxVERTICAL);

   m_checkBox33 = new wxCheckBox(this, IDC_ClassStatsOnly, wxT("Keep only class statistics in memory"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox33, IDS_ToolTip117);
   bSizer183->Add(m_checkBox33, 0, wxALL, 5);


   bSizer181->Add(bSizer183, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

   wxBoxSizer* bSizer184;
   bSizer184 = new wxBoxSizer(wxHORIZONTAL);

   m_checkBox35 = new wxCheckBox(this, IDC_SetZeroVariance, wxT("Set zero variance to:"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox35, IDS_ToolTip118);
   bSizer184->Add(m_checkBox35, 0, wxALIGN_CENTER | wxALL, 5);

   m_textCtrl92 = new wxTextCtrl(this, IDC_Variance, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   m_textCtrl92->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_varienceString));
   bSizer184->Add(m_textCtrl92, 0, wxALL, 5);


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

   wxBoxSizer* bSizer187;
   bSizer187 = new wxBoxSizer(wxHORIZONTAL);

   m_button42 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer187->Add(m_button42, 0, wxALL, 5);

   m_button43 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
   bSizer187->Add(m_button43, 0, wxALL, 5);


   bSizer181->Add(bSizer187, 0, wxALIGN_RIGHT | wxALL, 12);


   this->SetSizer(bSizer181);
   this->Layout();

   this->Centre(wxBOTH);
}

#ifndef multispec_lin
/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg dialog

CMEditCoordinatesDlg::CMEditCoordinatesDlg(CWnd* pParent /*=NULL*/)
: CMDialog(CMEditCoordinatesDlg::IDD, pParent) {
   //{{AFX_DATA_INIT(CMEditCoordinatesDlg)
   m_applyToAllWindowsFlag = FALSE;
   m_listSelectionUnits = -1;
   m_newColumnStart = 0.0;
   m_newColumnEnd = 0.0;
   m_newLineEnd = 0.0;
   m_newLineStart = 0.0;
   m_useStartLineColumnFlag = FALSE;
   //}}AFX_DATA_INIT

   m_pointType = 0;
   m_windowInfoPtr = NULL;
   m_previewWasUsedFlag = FALSE;
   m_valueChangedFlag = FALSE;

   m_initializedFlag = CMDialog::m_initializedFlag;

}

void CMEditCoordinatesDlg::DoDataExchange(CDataExchange* pDX) {
   UInt16 controlID;

   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CMEditCoordinatesDlg)
   DDX_Check(pDX, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);
   DDX_CBIndex(pDX, IDC_CoordinateUnits, m_listSelectionUnits);
   DDX_Text(pDX, IDC_NewColumnStart, m_newColumnStart);
   DDX_Text(pDX, IDC_NewColumnEnd, m_newColumnEnd);
   DDX_Text(pDX, IDC_NewLineEnd, m_newLineEnd);
   DDX_Text(pDX, IDC_NewLineStart, m_newLineStart);
   DDX_Check(pDX, IDC_StartLCCheckBox, m_useStartLineColumnFlag);
   //}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate) {
      // Verify that the coordinates make sense.

      if (!CheckLineColumnValues(&controlID)) {
         pDX->PrepareEditCtrl(controlID);
         pDX->Fail();

      } // end "if (!CheckLineColumnValues ())"

   } // end "if (pDX-m_bSaveAndValidate)"

   DDV_MinMaxDouble(pDX, m_newLineEnd, -99999999, 99999999);
} // end "DoDataExchange"     

BEGIN_MESSAGE_MAP(CMEditCoordinatesDlg, CMDialog)
//{{AFX_MSG_MAP(CMEditCoordinatesDlg)
ON_EN_CHANGE(IDC_NewColumnEnd, OnChangeNewColumnEnd)
ON_EN_CHANGE(IDC_NewColumnStart, OnChangeNewColumnStart)
ON_EN_CHANGE(IDC_NewLineEnd, OnChangeNewLineEnd)
ON_EN_CHANGE(IDC_NewLineStart, OnChangeNewLineStart)
ON_BN_CLICKED(IDC_Preview, OnPreview)
ON_CBN_SELENDOK(IDC_CoordinateUnits, OnSelendokCoordinateUnits)
ON_BN_CLICKED(IDC_ApplyToAllCheckbox, OnApplyToAllCheckbox)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
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
//	Coded By:			Larry L. Biehl			Date: 02/20/1996
//	Revised By:			Larry L. Biehl			Date: 03/18/2005	

Boolean CMEditCoordinatesDlg::DoDialog(
   WindowPtr windowPtr,
   WindowInfoPtr windowInfoPtr,
   LongRect* inputSelectionRectanglePtr,
   LongRect* selectionRectanglePtr,
   DoubleRect* coordinateRectanglePtr,
   SInt16 pointType,
   SInt16* unitsDisplayCodePtr,
   Boolean* changedFlagPtr,
   Boolean* applyToAllWindowsFlagPtr,
   Boolean* useStartLineColumnFlagPtr,
   Boolean* previewWasUsedFlagPtr,
   SInt16 stringID) {
   Boolean continueFlag = FALSE;

   SInt16 returnCode;


   // Make sure intialization has been completed.

   if (!m_initializedFlag)
      return (FALSE);

   m_windowPtr = windowPtr;
   m_windowInfoPtr = windowInfoPtr;
   m_inputSelectionRectanglePtr = inputSelectionRectanglePtr;
   m_selectionRectanglePtr = selectionRectanglePtr;
   m_coordinateRectanglePtr = coordinateRectanglePtr;
   m_pointType = pointType;
   m_selectionUnits = *unitsDisplayCodePtr;
   m_stringID = stringID;

   // Initialize new rectangle coordinates. 									

   m_newLineStart = inputSelectionRectanglePtr->top;
   m_newColumnStart = inputSelectionRectanglePtr->left;
   m_newLineEnd = inputSelectionRectanglePtr->bottom;
   m_newColumnEnd = inputSelectionRectanglePtr->right;

   m_applyToAllWindowsFlag = *applyToAllWindowsFlagPtr;
   m_useStartLineColumnFlag = *useStartLineColumnFlagPtr;

   m_coordinateUnitsChangingFlag = FALSE;

   returnCode = DoModal();

   if (returnCode == IDOK) {
      continueFlag = TRUE;
      *unitsDisplayCodePtr = m_selectionUnits;
      *useStartLineColumnFlagPtr = m_useStartLineColumnFlag;

   } // end "if (returnCode == IDOK)" 

   *previewWasUsedFlagPtr = m_previewWasUsedFlag;
   *applyToAllWindowsFlagPtr = m_applyToAllWindowsFlag;

   return (continueFlag);

} // end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg message handlers

bool CMEditCoordinatesDlg::OnInitDialog() 
{
   Boolean applyToAllWindowsFlag,
      useStartLineColumnFlag;


   // Get maximum number of lines and columns allowed.

   CDialog::OnInitDialog();

   //	Set menu options.

   applyToAllWindowsFlag = m_applyToAllWindowsFlag;
   useStartLineColumnFlag = TRUE;
   m_coordinateUnitsChangingFlag = TRUE;

   EditLineColumnDialogInitialize(this,
      m_windowPtr,
      m_windowInfoPtr,
      m_pointType,
      m_selectionUnits,
      m_inputSelectionRectanglePtr,
      m_selectionRectanglePtr,
      m_coordinateRectanglePtr,
      &m_minMaxSelectionRectangle,
      &m_minMaxCoordinateRectangle,
      &m_inputCoordinateRectangle,
      &applyToAllWindowsFlag,
      &useStartLineColumnFlag,
      &m_selectionUnits,
      m_stringID);

   m_applyToAllWindowsFlag = applyToAllWindowsFlag;
   m_coordinateUnitsChangingFlag = FALSE;

   m_listSelectionUnits = GetComboListSelection(IDC_CoordinateUnits,
      m_selectionUnits);
   if (m_listSelectionUnits < 0)
      m_listSelectionUnits = 0;

   // Update the dialog box parameters and then center the dialog.

   if (UpdateData(FALSE))
      PositionDialogWindow();

   // Set default text selection to first edit text item 

   SelectDialogItemText(this, IDC_NewLineStart, 0, SInt16_MAX);

   return FALSE; // return TRUE  unless you set the focus to a control 

} // end "OnInitDialog"

void CMEditCoordinatesDlg::OnChangeNewColumnStart() {
   double previousValue;

   Boolean errorAlertFlag = FALSE,
      updateValueFlag = FALSE;


   previousValue = m_newColumnStart;
   DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_value);

   if (m_selectionUnits == kLineColumnUnits) {
      if (m_valuePtr[0] != 0)
         DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);

      else // m_valuePtr[0] == 0
         m_newColumnStart = 0;

      if (m_newColumnStart < m_minMaxSelectionRectangle.left) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newColumnStart = m_minMaxSelectionRectangle.left;

      }// end "if (m_newColumnStart < m_minMaxSelectionRectangle.left)"

      else if (m_newColumnStart > m_minMaxSelectionRectangle.right) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newColumnStart = m_minMaxSelectionRectangle.right;

      } // end "else if (m_newColumnStart > m_minMaxSelectionRectangle.right)"

      m_selectionRectanglePtr->left = (SInt32) m_newColumnStart;

   }// end "if (m_selectionUnits == kLineColumnUnits)"

   else // m_selectionUnits != kLineColumnUnits
   {
      if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))
         DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);

      else // m_valuePtr[0] == 0
         m_newColumnStart = 0;

      m_coordinateRectanglePtr->left = m_newColumnStart;

   } // end "else m_selectionUnits != kLineColumnUnits"

   if (previousValue != m_newColumnStart && !m_coordinateUnitsChangingFlag)
      m_valueChangedFlag = TRUE;

   if (updateValueFlag)
      DDX_Text(m_dialogToPtr, IDC_NewColumnStart, m_newColumnStart);

   if (errorAlertFlag)
      NumberErrorAlert((SInt32) m_newColumnStart, this, IDC_NewColumnStart);

   //	CheckOKFlag ();

} // end "OnChangeNewColumnStart"

void CMEditCoordinatesDlg::OnChangeNewColumnEnd() {
   double previousValue;

   Boolean errorAlertFlag = FALSE,
      updateValueFlag = FALSE;


   previousValue = m_newColumnEnd;
   DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_value);

   if (m_selectionUnits == kLineColumnUnits) {
      if (m_valuePtr[0] != 0)
         DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);

      else // m_valuePtr[0] == 0
         m_newColumnEnd = 0;

      if (m_newColumnEnd < m_minMaxSelectionRectangle.left) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newColumnEnd = m_minMaxSelectionRectangle.left;

      }// end "if (m_newColumnEnd < m_minMaxSelectionRectangle.left)"

      else if (m_newColumnEnd > m_minMaxSelectionRectangle.right) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newColumnEnd = m_minMaxSelectionRectangle.right;

      } // end "else if (m_newColumnEnd > m_minMaxSelectionRectangle.right)"

      m_selectionRectanglePtr->right = (SInt32) m_newColumnEnd;

   }// end "if (m_selectionUnits == kLineColumnUnits)"

   else // m_selectionUnits != kLineColumnUnits
   {
      if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))
         DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);

      else // m_valuePtr[0] == 0
         m_newColumnEnd = 0;

      m_coordinateRectanglePtr->right = m_newColumnEnd;

   } // end "else m_selectionUnits != kLineColumnUnits"

   if (previousValue != m_newColumnEnd && !m_coordinateUnitsChangingFlag)
      m_valueChangedFlag = TRUE;

   if (updateValueFlag)
      DDX_Text(m_dialogToPtr, IDC_NewColumnEnd, m_newColumnEnd);

   if (errorAlertFlag)
      NumberErrorAlert((SInt32) m_newColumnEnd, this, IDC_NewColumnEnd);

   //	CheckOKFlag ();

} // end "OnChangeNewColumnEnd"

void CMEditCoordinatesDlg::OnChangeNewLineStart() {
   double previousValue;

   Boolean errorAlertFlag = FALSE,
      updateValueFlag = FALSE;


   previousValue = m_newLineStart;
   DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_value);

   if (m_selectionUnits == kLineColumnUnits) {
      if (m_valuePtr[0] != 0)
         DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_newLineStart);

      else // m_valuePtr[0] == 0
         m_newLineStart = 0;

      if (m_newLineStart < m_minMaxSelectionRectangle.top) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newLineStart = m_minMaxSelectionRectangle.top;

      }// end "if (m_newLineStart < m_minMaxSelectionRectangle.top)"

      else if (m_newLineStart > m_minMaxSelectionRectangle.bottom) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newLineStart = m_minMaxSelectionRectangle.bottom;

      } // end "else if (m_newLineStart > m_minMaxSelectionRectangle.bottom)"

      m_selectionRectanglePtr->top = (SInt32) m_newLineStart;

   }// end "if (m_selectionUnits == kLineColumnUnits)"

   else // m_selectionUnits != kLineColumnUnits
   {
      if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))
         DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_newLineStart);

      else // m_valuePtr[0] == 0
         m_newLineStart = 0;

      m_coordinateRectanglePtr->bottom = m_newLineStart;

   } // end "else if (m_selectionUnits != kLineColumnUnits)"

   if (previousValue != m_newLineStart && !m_coordinateUnitsChangingFlag)
      m_valueChangedFlag = TRUE;

   if (updateValueFlag)
      DDX_Text(m_dialogToPtr, IDC_NewLineStart, m_newLineStart);

   if (errorAlertFlag)
      NumberErrorAlert((SInt32) m_newLineStart, this, IDC_NewLineStart);

   //	CheckOKFlag ();

} // end "OnChangeNewLineStart" 

void CMEditCoordinatesDlg::OnChangeNewLineEnd(void) {
   double previousValue;

   Boolean errorAlertFlag = FALSE,
      updateValueFlag = FALSE;


   previousValue = m_newLineEnd;
   DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_value);

   if (m_selectionUnits == kLineColumnUnits) {
      if (m_valuePtr[0] != 0)
         DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_newLineEnd);

      else // m_valuePtr[0] == 0
         m_newLineEnd = 0;

      if (m_newLineEnd < m_minMaxSelectionRectangle.top) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newLineEnd = m_minMaxSelectionRectangle.top;

      }// end "if (m_newLineEnd < m_minMaxSelectionRectangle.top)"

      else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom) {
         errorAlertFlag = TRUE;
         updateValueFlag = TRUE;
         m_newLineEnd = m_minMaxSelectionRectangle.bottom;

      } // end "else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom)"

      m_selectionRectanglePtr->bottom = (SInt32) m_newLineEnd;

   }// end "if (m_selectionUnits == kLineColumnUnits)"

   else // m_selectionUnits == kLineColumnUnits
   {
      if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))
         DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_newLineEnd);

      else // m_valuePtr[0] == 0
         m_newLineEnd = 0;

      m_coordinateRectanglePtr->top = m_newLineEnd;

   } // end "if (m_selectionUnits == kLineColumnUnits)"

   if (previousValue != m_newLineEnd && !m_coordinateUnitsChangingFlag)
      m_valueChangedFlag = TRUE;

   if (updateValueFlag)
      DDX_Text(m_dialogToPtr, IDC_NewLineEnd, m_newLineEnd);

   if (errorAlertFlag)
      NumberErrorAlert((SInt32) m_newLineStart, this, IDC_NewLineStart);

   //	CheckOKFlag ();

} // end "OnChangeNewLineEnd" 


void CMEditCoordinatesDlg::CheckOKFlag() 
{
			// Make certain the line-column values make sense.					

   if ((m_newLineStart > m_newLineEnd) ||
      (m_newColumnStart > m_newColumnEnd))
      MHiliteControl((CMImageView*)this, (ControlHandle) IDOK, 255);

   else // (m_newLineStart <= m_newLineEnd) &&
      //				(m_newColumnStart <= m_newColumnEnd) 
      MHiliteControl((CMImageView*)this, (ControlHandle) IDOK, 0);

} // end "CheckOKFlag" 


void CMEditCoordinatesDlg::OnPreview()
{
   if (CheckLineColumnValues(NULL))
      /*
         SInt16							valueItemHit;


         valueItemHit = 0;
         if (m_valueChangedFlag)
            {
            valueItemHit = EditLineColumnDialogCheckCoordinates (
                                       this,
                                       m_selectionUnits,
                                       m_selectionRectanglePtr,
                                       m_coordinateRectanglePtr,
                                       &m_minMaxCoordinateRectangle,
                                       m_coordinateRectanglePtr->left,
                                       m_coordinateRectanglePtr->right,
                                       m_coordinateRectanglePtr->bottom,
                                       m_coordinateRectanglePtr->top);

            if (valueItemHit != 0)
               {
               SelectDialogItemText (this, valueItemHit, 0, SInt16_MAX);				

               }		// end "if (valueItemHit != 0)"

            else		// valueItemHit == 0
               m_valueChangedFlag = FALSE;

            }		// end "if (m_valueChangedFlag)"
       */

      //	if (valueItemHit == 0)
   {
      DDX_Check (m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

      EditSelectionDialogShowSelection(
         m_windowPtr,
         m_windowInfoPtr->windowInfoHandle,
         m_selectionRectanglePtr,
         m_coordinateRectanglePtr,
         m_applyToAllWindowsFlag,
         m_useStartLineColumnFlag,
         m_selectionUnits,
         1);

      m_previewWasUsedFlag = TRUE;

   } // end "if (valueItemHit == 0)"

} // end "OnPreview"


void CMEditCoordinatesDlg::OnSelendokCoordinateUnits() 
{
   CComboBox* comboBoxPtr;

   SInt16 previousSelectionUnits;


   comboBoxPtr = (CComboBox*) GetDlgItem(IDC_CoordinateUnits);

   previousSelectionUnits = m_selectionUnits;

   DDX_CBIndex(m_dialogFromPtr, IDC_CoordinateUnits, m_listSelectionUnits);
   m_selectionUnits =
      (SInt16)comboBoxPtr->GetItemData(m_listSelectionUnits);

   if (m_selectionUnits >= 0 && previousSelectionUnits != m_selectionUnits) 
		{
      DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

      m_coordinateUnitsChangingFlag = TRUE;

      EditLineColumnDialogSetStartLC (this,
													m_selectionUnits,
													m_applyToAllWindowsFlag);

      EditSelectionDialogSetCoordinates (
													this,
													m_windowInfoPtr->windowInfoHandle,
													m_inputSelectionRectanglePtr,
													m_selectionRectanglePtr,
													&m_inputCoordinateRectangle,
													m_coordinateRectanglePtr,
													&m_minMaxSelectionRectangle,
													&m_minMaxCoordinateRectangle,
													m_selectionUnits,
													previousSelectionUnits);

      m_coordinateUnitsChangingFlag = FALSE;

		}	// end "m_listSelectionUnits >= 0  && ..."

}	// end "OnSelendokCoordinateUnits"


void CMEditCoordinatesDlg::OnApplyToAllCheckbox() 
{
   DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

   EditLineColumnDialogSetStartLC(this,
      m_selectionUnits,
      m_applyToAllWindowsFlag);

}	// end "OnApplyToAllCheckbox"


Boolean CMEditCoordinatesDlg::CheckLineColumnValues (
				UInt16*									controlIDPtr) 
{
   SInt16 valueItemHit;

   Boolean returnFlag = TRUE;


   valueItemHit = 0;
   if (m_valueChangedFlag) {
      valueItemHit = EditLineColumnDialogCheckCoordinates(
         this,
         m_windowInfoPtr->windowInfoHandle,
         m_selectionUnits,
         m_selectionRectanglePtr,
         &m_minMaxSelectionRectangle,
         m_coordinateRectanglePtr,
         &m_minMaxCoordinateRectangle,
         m_coordinateRectanglePtr->left,
         m_coordinateRectanglePtr->right,
         m_coordinateRectanglePtr->bottom,
         m_coordinateRectanglePtr->top);

      if (valueItemHit != 0) {
         SelectDialogItemText(this, valueItemHit, 0, SInt16_MAX);
         returnFlag = FALSE;

      }// end "if (valueItemHit != 0)"

      else // valueItemHit == 0
         m_valueChangedFlag = FALSE;

   } // end "if (m_valueChangedFlag)"

   if (controlIDPtr != NULL)
      *controlIDPtr = valueItemHit;

   return (returnFlag);

} // end "CheckLineColumnValues"

#endif