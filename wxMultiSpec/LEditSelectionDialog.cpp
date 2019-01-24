// LEditSelectionDialog.cpp
//
// Revised by Larry Biehl on 01/17/2019
//
// CMEditCoordinatesDlg dialog
//
#include "SMultiSpec.h"
#include "wx/valgen.h"
#include "wx/valnum.h"
#include "LEditSelectionDialog.h"

extern void EditLineColumnDialogOK (
        Handle windowInfoHandle,
        LongRect* inputSelectionRectanglePtr,
        LongRect* selectionRectanglePtr,
        DoubleRect* coordinateRectanglePtr,
        SInt16 unitsDisplayCode,
        Boolean* changedFlagPtr);
		  

CMEditCoordinatesDlg::CMEditCoordinatesDlg (wxWindow* pParent, wxWindowID id, const wxString& title /*=NULL*/)
		: CMDialog(CMEditCoordinatesDlg::IDD, pParent, title)
{
    m_applyToAllWindowsFlag = FALSE;
    m_listSelectionUnits = -1;    
    m_newColumnStart = 0.0;
    m_newColumnEnd = 0.0;
    m_newLineEnd = 0.0;
    m_newLineStart = 0.0;
    m_useStartLineColumnFlag = FALSE;

    m_pointType = 0;
    m_windowInfoPtr = NULL;
    m_previewWasUsedFlag = FALSE;
    m_valueChangedFlag = FALSE;

    m_initializedFlag = CMDialog::m_initializedFlag;

    CreateControls();
	
}	// end "CMEditCoordinatesDlg"


BEGIN_EVENT_TABLE(CMEditCoordinatesDlg, CMDialog)
	EVT_INIT_DIALOG(CMEditCoordinatesDlg::OnInitDialog)
	EVT_TEXT(IDC_NewColumnEnd, CMEditCoordinatesDlg::OnChangeNewColumnEnd)
	EVT_TEXT(IDC_NewColumnStart, CMEditCoordinatesDlg::OnChangeNewColumnStart)
	EVT_TEXT(IDC_NewLineEnd, CMEditCoordinatesDlg::OnChangeNewLineEnd)
	EVT_TEXT(IDC_NewLineStart, CMEditCoordinatesDlg::OnChangeNewLineStart)
	EVT_BUTTON(IDC_Preview, CMEditCoordinatesDlg::OnPreview)
	EVT_CHOICE(IDC_CoordinateUnits, CMEditCoordinatesDlg::OnSelendokCoordinateUnits)
	EVT_CHECKBOX(IDC_ApplyToAllCheckbox, CMEditCoordinatesDlg::OnApplyToAllCheckbox)
	//EVT_CHAR_HOOK(CMEditCoordinatesDlg::OnButtonPress)
END_EVENT_TABLE()



Boolean CMEditCoordinatesDlg::CheckLineColumnValues(
        UInt16* controlIDPtr) 
{
	double	maxDoubleValue,
				minDoubleValue;
	SInt32	maxValue;
	SInt16	valueItemHit = 0;
	Boolean	returnFlag = TRUE;

			// Verify that no fields are blank.
			
	if (m_LineStartString.empty())
		valueItemHit = IDC_NewLineStart;
	
	else if (m_LineEndString.empty())
		valueItemHit = IDC_NewLineEnd;
		
	if (valueItemHit != 0)
		{
		if (m_selectionUnits == kLineColumnUnits)
			maxValue = m_minMaxSelectionRectangle.bottom;
			
		else	// m_selectionUnits != kLineColumnUnits
			{
			minDoubleValue = m_minMaxCoordinateRectangle.bottom;
			maxDoubleValue = m_minMaxCoordinateRectangle.top;
			
			}	// else m_selectionUnits != kLineColumnUnits
			
		}	// end "if (valueItemHit != 0)"
		
	if (valueItemHit == 0)
		{
		if (m_ColumnStartString.empty ())
			valueItemHit = IDC_NewColumnStart;
		
		else if (m_ColumnEndString.empty ())
			valueItemHit = IDC_NewColumnStart;
			
		if (valueItemHit != 0)
			{
			if (m_selectionUnits == kLineColumnUnits)
				maxValue = m_minMaxSelectionRectangle.right;
			
			else
				{
				minDoubleValue = m_minMaxCoordinateRectangle.left;
				maxDoubleValue = m_minMaxCoordinateRectangle.right;
				
				}
				
			}
			
		}	// end "if (valueItemHit == 0)"
		
	if (valueItemHit != 0)
		{
		Boolean stringLoadedFlag = TRUE;
		if (m_selectionUnits == kLineColumnUnits)
			stringLoadedFlag = LoadSpecifiedStringNumberLongP ( 
													kAlertStrID, 
													IDS_Alert91, 
													(char*)gTextString,
													(char*)gTextString2,
													TRUE,
													1,
													maxValue);
			
		else	// m_selectionUnits != kLineColumnUnits
			{
			int numberDecimals = 4;
			if (m_selectionUnits == kLatLongUnits)
            numberDecimals = 8;
			SInt16 numberChars = sprintf ((char*)&gTextString[1],
												"Enter value between %.*f and %.*f.",
												numberDecimals, 
												minDoubleValue,
												numberDecimals,
												maxDoubleValue);	
													
			gTextString[0] = numberChars;
			
			}
		
		if (stringLoadedFlag)
			DisplayAlert (kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
			
		}	// end "if (valueItemHit != 0)"

	if (valueItemHit == 0)
		{
		if (m_valueChangedFlag)
			{
			valueItemHit = EditLineColumnDialogCheckCoordinates (
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

			if (valueItemHit == 0)
				m_valueChangedFlag = FALSE;

			}	// end "if (m_valueChangedFlag)"

		if (controlIDPtr != NULL)
			*controlIDPtr = valueItemHit;
			
		}	// end "if (valueItemHit == 0)"

	if (valueItemHit != 0)
		{
		SelectDialogItemText(this, valueItemHit, 0, SInt16_MAX);
		returnFlag = FALSE;

		}	// end "if (valueItemHit != 0)"

	return (returnFlag);

} // end "CheckLineColumnValues"



void CMEditCoordinatesDlg::CheckOKFlag()

{
    		// Make certain the line-column values make sense.

    if ((m_newLineStart > m_newLineEnd) ||
            (m_newColumnStart > m_newColumnEnd))
        MHiliteControl((CMImageView*)this, (ControlHandle) wxID_OK, 255);

    else // (m_newLineStart <= m_newLineEnd) &&
        //				(m_newColumnStart <= m_newColumnEnd) 
        MHiliteControl((CMImageView*)this, (ControlHandle) wxID_OK, 0);

}	// end "CheckOKFlag"


void CMEditCoordinatesDlg::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);

	bSizer200 = new wxBoxSizer (wxVERTICAL);

	wxBoxSizer* bSizer201 = new wxBoxSizer (wxHORIZONTAL);

	m_staticText313 = new wxStaticText(this, wxID_ANY, wxT("Units:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText313->Wrap(-1);
   SetUpToolTip(m_staticText313, IDS_ToolTip316);
	//bSizer201->Add (m_staticText313, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer201->Add (m_staticText313, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT|wxBOTTOM, 5));

	wxString m_choice1Choices[] = {wxT("Lines-Columns"), wxT("Transverse Mercator-Meters"), wxT("Latitude-Longitude")};
	int m_choice1NChoices = sizeof ( m_choice1Choices) / sizeof ( wxString);
	m_choice1 = new wxChoice(this, IDC_CoordinateUnits, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0);
	m_choice1->SetSelection(0);
   SetUpToolTip(m_choice1, IDS_ToolTip316);
	//bSizer201->Add (m_choice1, 0, wxALIGN_CENTER | wxALL, 5);
	bSizer201->Add (m_choice1, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxBOTTOM, 5));


	//bSizer200->Add (bSizer201, 0, wxALL | wxEXPAND, 5);
	bSizer200->Add (bSizer201, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer(2, 4, 0, 0);
	fgSizer20->SetFlexibleDirection(wxBOTH);
	fgSizer20->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText314 = new wxStaticText(this, IDC_CurrentLine, wxT("Current Line(s): "), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_staticText314->Wrap(-1);
   SetUpToolTip(m_staticText314, IDS_ToolTip324);
	//fgSizer20->Add (m_staticText314, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	fgSizer20->Add (m_staticText314, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	m_staticText315 = new wxStaticText(this, IDC_CurrentLineStart, wxT("1"), wxDefaultPosition, wxSize(120,-1), 0);
	m_staticText315->Wrap(-1);
	fgSizer20->Add (m_staticText315, 0, wxALL|wxEXPAND, 5);

	m_staticText316 = new wxStaticText(this, IDC_CurrentLineDash, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	m_staticText316->Wrap(-1);
	fgSizer20->Add(m_staticText316, 0, wxALL, 5);

	m_staticText317 = new wxStaticText(this, IDC_CurrentLineEnd, wxT("512"), wxDefaultPosition, wxSize(120,-1), 0);
	m_staticText317->Wrap(-1);
	fgSizer20->Add(m_staticText317, 0, wxALL|wxEXPAND, 5);

	m_staticText318 = new wxStaticText(this, IDC_NewLine, wxT("New Line(s):"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_staticText318->Wrap(-1);
   SetUpToolTip(m_staticText318, IDS_ToolTip328);
	//fgSizer20->Add (m_staticText318, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	fgSizer20->Add (m_staticText318, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	m_textCtrl182 = new wxTextCtrl(this, IDC_NewLineStart);//, wxT("1"), wxDefaultPosition, wxSize( 120,-1 ), 0);
	m_textCtrl182->SetSize (wxSize(120,-1));
   SetUpToolTip(m_textCtrl182, IDS_ToolTip317);
   //m_textCtrl182->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	fgSizer20->Add(m_textCtrl182, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText319 = new wxStaticText(this, IDC_NewLineDash, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	m_staticText319->Wrap(-1);
	fgSizer20->Add(m_staticText319, 0, wxALIGN_CENTER | wxALL, 5);

	m_textCtrl183 = new wxTextCtrl(this, IDC_NewLineEnd, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), 0);
   SetUpToolTip(m_textCtrl183, IDS_ToolTip318);
   //m_textCtrl183->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	fgSizer20->Add(m_textCtrl183, 0, wxALIGN_CENTER | wxALL, 5);


	//bSizer200->Add (fgSizer20, 0, wxALL | wxEXPAND, 5);
	bSizer200->Add (fgSizer20, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

	wxFlexGridSizer* fgSizer201;
	fgSizer201 = new wxFlexGridSizer (2, 4, 0, 0);
	fgSizer201->SetFlexibleDirection (wxBOTH);
	fgSizer201->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);

	m_staticText3141 = new wxStaticText(this, IDC_CurrentColumn, wxT("Current Column(s): "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3141->Wrap(-1);
   SetUpToolTip(m_staticText3141, IDS_ToolTip324);
	//fgSizer201->Add (m_staticText3141, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	fgSizer201->Add (m_staticText3141, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	m_staticText3151 = new wxStaticText(this, IDC_CurrentColumnStart, wxT("1"), wxDefaultPosition, wxSize(120,-1), 0);
	m_staticText3151->Wrap(-1);
	fgSizer201->Add(m_staticText3151, 0, wxALL|wxEXPAND, 5);

	m_staticText3161 = new wxStaticText(this, IDC_CurrentColumnDash, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	m_staticText3161->Wrap(-1);
	fgSizer201->Add(m_staticText3161, 0, wxALL, 5);

	m_staticText3171 = new wxStaticText(this, IDC_CurrentColumnEnd, wxT("512"), wxDefaultPosition, wxSize(120,-1), 0);
	m_staticText3171->Wrap(-1);
	fgSizer201->Add(m_staticText3171, 0, wxALL|wxEXPAND, 5);

	m_staticText3181 = new wxStaticText(this, IDC_NewColumn, wxT("New Column(s):"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3181->Wrap(-1);
   SetUpToolTip(m_staticText3181, IDS_ToolTip328);
	//fgSizer201->Add (m_staticText3181, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	fgSizer201->Add (m_staticText3181, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxTOP|wxRIGHT|wxBOTTOM, 5));

	m_textCtrl1821 = new wxTextCtrl(this, IDC_NewColumnStart);//, wxT("1"), wxDefaultPosition, wxSize( 120,-1 ), 0);
	m_textCtrl1821->SetSize (wxSize(120,-1));
   SetUpToolTip(m_textCtrl1821, IDS_ToolTip319);
   //m_textCtrl1821->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	fgSizer201->Add(m_textCtrl1821, 0, wxALIGN_CENTER | wxALL, 5);

	m_staticText3191 = new wxStaticText(this, IDC_NewColumnDash, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	m_staticText3191->Wrap(-1);
	fgSizer201->Add(m_staticText3191, 0, wxALIGN_CENTER | wxALL, 5);

	m_textCtrl1831 = new wxTextCtrl(this, IDC_NewColumnEnd, wxEmptyString, wxDefaultPosition, wxSize( 120,-1 ), 0);
   SetUpToolTip(m_textCtrl1831, IDS_ToolTip320);
   //m_textCtrl1831->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	fgSizer201->Add(m_textCtrl1831, 0, wxALIGN_CENTER | wxALL, 5);
	
	//wxIntegerValidator<unsigned long int> uLongIntValue(&m_uInt32ValueCheck);
	/*
	uLongIntValue.SetRange(1, kMaxNumberLines);
	m_textCtrl182->SetValidator(uLongIntValue);
	m_textCtrl183->SetValidator(uLongIntValue);
	m_textCtrl1821->SetValidator(uLongIntValue);
	m_textCtrl1831->SetValidator(uLongIntValue);
	*/
	wxFloatingPointValidator<double> doubleValue(0, &m_doubleValueCheck);
	doubleValue.SetRange(1, kMaxNumberLines);
	m_textCtrl182->SetValidator(doubleValue);
	m_textCtrl183->SetValidator(doubleValue);
	m_textCtrl1821->SetValidator(doubleValue);
	m_textCtrl1831->SetValidator(doubleValue);

	//bSizer200->Add (fgSizer201, 0, wxALL | wxEXPAND, 5);
	bSizer200->Add (fgSizer201, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

	wxBoxSizer* bSizer203;
	bSizer203 = new wxBoxSizer(wxVERTICAL);

	m_checkBox15 = new wxCheckBox(this, IDC_ApplyToAllCheckbox, wxT("Apply to all image windows"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox15, IDS_ToolTip321);
	bSizer203->Add(m_checkBox15, 0, wxLEFT, 30);

	//bSizer200->Add (bSizer203, 0, wxALL | wxEXPAND, 5);
	bSizer200->Add (bSizer203, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

	wxBoxSizer* bSizer205;
	bSizer205 = new wxBoxSizer(wxVERTICAL);

	m_checkBox16 = new wxCheckBox(this, IDC_StartLCCheckBox, wxT("Use start line/column"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_checkBox16, IDS_ToolTip322);
	bSizer205->Add(m_checkBox16, 0, wxLEFT, 60);

	//bSizer200->Add(bSizer205, 0, wxALL | wxEXPAND, 5);
	bSizer200->Add (bSizer205, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

	wxBoxSizer* bSizer204;
	bSizer204 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer207;
	bSizer207 = new wxBoxSizer(wxVERTICAL);
	m_button29 = new wxButton(this, IDC_Preview, wxT("Preview"), wxDefaultPosition, wxDefaultSize, 0);
   SetUpToolTip(m_button29, IDS_ToolTip322);
	//bSizer207->Add (m_button29, 0, wxALIGN_LEFT | wxALL, 5);
	bSizer207->Add (m_button29, wxSizerFlags(0).Left());
	
	//bSizer204->Add (bSizer207, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 5);
	bSizer204->Add (bSizer207, wxSizerFlags(1).Left());

	//bSizer200->Add (bSizer204, 0, wxALL | wxEXPAND, 5);
	bSizer200->Add (bSizer204, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
	
	CreateStandardButtons (bSizer200);

	this->SetSizer (bSizer200);
	this->Layout ();

	//this->Centre(wxBOTH);
	
}	// end "CreateControls"



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

    m_newLineStart = (double) inputSelectionRectanglePtr->top;
    m_newColumnStart = (double) inputSelectionRectanglePtr->left;
    m_newLineEnd = (double) inputSelectionRectanglePtr->bottom;
    m_newColumnEnd = (double) inputSelectionRectanglePtr->right;

    m_applyToAllWindowsFlag = *applyToAllWindowsFlagPtr;
    m_useStartLineColumnFlag = *useStartLineColumnFlagPtr;

    m_coordinateUnitsChangingFlag = FALSE;

    returnCode = ShowModal();
    
    if (returnCode == wxID_OK) {     
    
        continueFlag = TRUE;
        *unitsDisplayCodePtr = m_selectionUnits;
        *useStartLineColumnFlagPtr = m_useStartLineColumnFlag;

    } // end "if (returnCode == IDOK)" 

    *previewWasUsedFlagPtr = m_previewWasUsedFlag;
    *applyToAllWindowsFlagPtr = m_applyToAllWindowsFlag;

    return (continueFlag);    
} // end "DoDialog"


void CMEditCoordinatesDlg::OnApplyToAllCheckbox(wxCommandEvent& event) {
    wxCheckBox* applyToAllWindowsFlag = (wxCheckBox*) FindWindow(IDC_ApplyToAllCheckbox);
    m_applyToAllWindowsFlag = applyToAllWindowsFlag->GetValue();

    EditLineColumnDialogSetStartLC(this,
            m_selectionUnits,
            m_applyToAllWindowsFlag);
    this->Layout();
    this->Fit();

} // end "OnApplyToAllCheckbox"

/*
//Press keyboard "Enter"  -> wxID_OK
void CMEditCoordinatesDlg::OnButtonPress(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		if(Validate() && TransferDataFromWindow())
			this->EndModal(wxID_OK);
	} else {
		event.Skip();
	}
}
*/

void CMEditCoordinatesDlg::OnChangeNewColumnStart(wxCommandEvent& event) {
    double previousValue;

    Boolean //errorAlertFlag = FALSE,
            updateValueFlag = FALSE;


    previousValue = m_newColumnStart;
    //DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_value);
    wxTextCtrl* newColumnStart = (wxTextCtrl*) FindWindow(IDC_NewColumnStart);
    m_value = newColumnStart->GetValue();

    if (m_selectionUnits == kLineColumnUnits) {
        //if (m_valuePtr[0] != 0) {
//        if (!((wxDynamicCast(FindWindow(IDC_NewColumnStart), wxTextCtrl)))->IsEmpty()){           
//            //DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);            
//            m_value.ToDouble(&m_newColumnStart);
//        } else // m_valuePtr[0] == 0
//            m_newColumnStart = 0;
        if(m_value.length() != 0)
            m_value.ToDouble(&m_newColumnStart);
        else
            m_newColumnStart = 0;
        
        if (m_newColumnStart < m_minMaxSelectionRectangle.left && (m_value.length()!= 0)) {
            //errorAlertFlag = TRUE;
            updateValueFlag = TRUE;
            m_newColumnStart = m_minMaxSelectionRectangle.left;

        }// end "if (m_newColumnStart < m_minMaxSelectionRectangle.left)"

        else if (m_newColumnStart > m_minMaxSelectionRectangle.right && (m_value.length()!= 0)) {
            //errorAlertFlag = TRUE;
            updateValueFlag = TRUE;
            m_newColumnStart = m_minMaxSelectionRectangle.right;

        } // end "else if (m_newColumnStart > m_minMaxSelectionRectangle.right)"

        m_selectionRectanglePtr->left = (SInt32) m_newColumnStart;

    }// end "if (m_selectionUnits == kLineColumnUnits)"

    else // m_selectionUnits != kLineColumnUnits
    {
        //if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0)) {
        //if (!(wxDynamicCast(FindWindow(IDC_NewColumnStart), wxTextCtrl))->IsEmpty()){           
        if(m_value.length()!= 0 && !(m_value == '-' && m_value.length()==1)){
            //DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);            
            m_value.ToDouble(&m_newColumnStart);
        } else // m_valuePtr[0] == 0
            m_newColumnStart = 0;

        m_coordinateRectanglePtr->left = m_newColumnStart;

    } // end "else m_selectionUnits != kLineColumnUnits"

    if (previousValue != m_newColumnStart && !m_coordinateUnitsChangingFlag)
        m_valueChangedFlag = TRUE;

    if (updateValueFlag) {
        //DDX_Text(m_dialogToPtr, IDC_NewColumnStart, m_newColumnStart);
        wxTextCtrl* newColumnStart = (wxTextCtrl*) FindWindow(IDC_NewColumnStart);
        //newColumnStart->ChangeValue(wxString::Format(wxT("%ld"), m_newColumnStart));
        if (m_selectionUnits == kLineColumnUnits)
            newColumnStart->ChangeValue(wxString::Format(wxT("%.0lf"), m_newColumnStart));
        else
            newColumnStart->ChangeValue(wxString::Format(wxT("%lf"), m_newColumnStart));
    }

    //if (errorAlertFlag)
    //    NumberErrorAlert((SInt32) m_newColumnStart, this, IDC_NewColumnStart);

    //	CheckOKFlag ();

} // end "OnChangeNewColumnStart"


void CMEditCoordinatesDlg::OnChangeNewColumnEnd(wxCommandEvent& event) {
    double previousValue;

    Boolean //errorAlertFlag = FALSE,
            updateValueFlag = FALSE;


    previousValue = m_newColumnEnd;
    //DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_value);
    wxTextCtrl* newColumnEnd = (wxTextCtrl*) FindWindow(IDC_NewColumnEnd);
    m_value = newColumnEnd->GetValue();

    if (m_selectionUnits == kLineColumnUnits) {
//        if (m_valuePtr[0] != 0) {
//        if (!((wxDynamicCast(FindWindow(IDC_NewColumnEnd), wxTextCtrl)))->IsEmpty()){           
//            //DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);
//            m_value.ToDouble(&m_newColumnEnd);
//        } else // m_valuePtr[0] == 0
//            m_newColumnEnd = 0;
            
        if(m_value.length() != 0)
            m_value.ToDouble(&m_newColumnEnd);
        else
            m_newColumnEnd = 0;
        
        if (m_newColumnEnd < m_minMaxSelectionRectangle.left && (m_value.length()!= 0)) {
            //errorAlertFlag = TRUE;
            updateValueFlag = TRUE;
            m_newColumnEnd = m_minMaxSelectionRectangle.left;

        }// end "if (m_newColumnEnd < m_minMaxSelectionRectangle.left)"

        else if (m_newColumnEnd > m_minMaxSelectionRectangle.right && (m_value.length()!= 0)) {
            //errorAlertFlag = TRUE;
            updateValueFlag = TRUE;
            m_newColumnEnd = m_minMaxSelectionRectangle.right;

        } // end "else if (m_newColumnEnd > m_minMaxSelectionRectangle.right)"

        m_selectionRectanglePtr->right = (SInt32) m_newColumnEnd;

    }// end "if (m_selectionUnits == kLineColumnUnits)"

    else // m_selectionUnits != kLineColumnUnits
    {
        // (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0)) {
        //if (!(wxDynamicCast(FindWindow(IDC_NewColumnEnd), wxTextCtrl))->IsEmpty()){           
        if(m_value.length()!= 0 && !(m_value == '-' && m_value.length()==1)){
            //DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);
            m_value.ToDouble(&m_newColumnEnd);
        } else // m_valuePtr[0] == 0
            m_newColumnEnd = 0;

        m_coordinateRectanglePtr->right = m_newColumnEnd;

    } // end "else m_selectionUnits != kLineColumnUnits"

    if (previousValue != m_newColumnEnd && !m_coordinateUnitsChangingFlag)
        m_valueChangedFlag = TRUE;

    if (updateValueFlag) {
        //DDX_Text(m_dialogToPtr, IDC_NewColumnEnd, m_newColumnEnd);
        //newColumnEnd->ChangeValue(wxString::Format(wxT("%ld"), m_newColumnEnd));
        if (m_selectionUnits == kLineColumnUnits)
            newColumnEnd->ChangeValue(wxString::Format(wxT("%.0lf"), m_newColumnEnd));
        else
            newColumnEnd->ChangeValue(wxString::Format(wxT("%lf"), m_newColumnEnd));
    }

    //if (errorAlertFlag)
    //    NumberErrorAlert((SInt32) m_newColumnEnd, this, IDC_NewColumnEnd);

    //	CheckOKFlag ();

} // end "OnChangeNewColumnEnd"


void CMEditCoordinatesDlg::OnChangeNewLineStart(wxCommandEvent& event) {
	double previousValue;

	Boolean		//errorAlertFlag = FALSE,
					//selectValueFlag = FALSE,
					updateValueFlag = FALSE;


	previousValue = m_newLineStart;
	wxTextCtrl* newLineStart = (wxTextCtrl*) FindWindow(IDC_NewLineStart);
	m_value = newLineStart->GetValue();

	if (m_selectionUnits == kLineColumnUnits) {
		//if (m_valuePtr[0] != 0) {
		//        if (!((wxDynamicCast(FindWindow(IDC_NewLineStart), wxTextCtrl))->IsEmpty())){ 
		//        } else // m_valuePtr[0] == 0
		//            m_newLineStart = 0;        
		if(m_value.length() != 0)
			m_value.ToDouble(&m_newLineStart);
		else	{
			m_newLineStart = 0;
			//selectValueFlag = TRUE;
			}

		if (m_newLineStart < m_minMaxSelectionRectangle.top && (m_value.length()!= 0)) {
		//if (m_newLineStart < m_minMaxSelectionRectangle.top) {
			//if (m_value.length() > 0)
			//	errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineStart = m_minMaxSelectionRectangle.top;

			}	// end "if (m_newLineStart < m_minMaxSelectionRectangle.top)"

		else if (m_newLineStart > m_minMaxSelectionRectangle.bottom && (m_value.length()!= 0)) {
			//errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineStart = m_minMaxSelectionRectangle.bottom;

			} // end "else if (m_newLineStart > m_minMaxSelectionRectangle.bottom)"

		m_selectionRectanglePtr->top = (SInt32) m_newLineStart;

		}	// end "if (m_selectionUnits == kLineColumnUnits)"

	else // m_selectionUnits != kLineColumnUnits
		{
		//if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0)) {
		//if (!(wxDynamicCast(FindWindow(IDC_NewLineStart), wxTextCtrl))->IsEmpty()){ 
		if(m_value.length()!= 0 && !(m_value == '-' && m_value.length()==1)){
			m_value.ToDouble(&m_newLineStart);
			} 
		else // m_valuePtr[0] == 0
			m_newLineStart = 0;

		m_coordinateRectanglePtr->bottom = m_newLineStart;

		} // end "else if (m_selectionUnits != kLineColumnUnits)"

	if (previousValue != m_newLineStart && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;

	if (updateValueFlag) {
		if (m_selectionUnits == kLineColumnUnits)
			newLineStart->ChangeValue(wxString::Format(wxT("%.0lf"), m_newLineStart));
		else
			newLineStart->ChangeValue(wxString::Format(wxT("%lf"), m_newLineStart));
		}
	
	//if (errorAlertFlag)
	//	NumberErrorAlert((SInt32) m_newLineStart, this, IDC_NewLineStart);
	/*
	if (selectValueFlag)  {
		newLineStart->Refresh();
		m_value = newLineStart->GetValue();
		int numberChars = sprintf ((char*)&gTextString3,
												" LEditSelRec:OnChangeNewLineStart (m_newLineStart, m_value length): %lf, %d%s", 
												m_newLineStart,	
												m_value.length(),
												//m_value,
												gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);	

		SelectDialogItemText(this, IDC_NewLineStart, 0, SInt16_MAX);
		newLineStart->SelectAll();
		
		m_value = newLineStart->GetValue();
		
		}
	*/
} // end "OnChangeNewLineStart" 


void CMEditCoordinatesDlg::OnChangeNewLineEnd(wxCommandEvent& event) {
    double previousValue;

    Boolean //errorAlertFlag = FALSE,
            updateValueFlag = FALSE;


    previousValue = m_newLineEnd;
    wxTextCtrl* newLineEnd = (wxTextCtrl*) FindWindow(IDC_NewLineEnd);
    m_value = newLineEnd->GetValue();
			
    if (m_selectionUnits == kLineColumnUnits) {
        //if (m_valuePtr[0] != 0) {
//        if (!((wxDynamicCast(FindWindow(IDC_NewLineEnd), wxTextCtrl))->IsEmpty())){
//            m_value.ToDouble(&m_newLineEnd);
//        } else // m_valuePtr[0] == 0
//            m_newLineEnd = 0;
        if(m_value.length() != 0)
            m_value.ToDouble(&m_newLineEnd);
        else
            m_newLineEnd = 0;
        
        if (m_newLineEnd < m_minMaxSelectionRectangle.top && (m_value.length()!= 0)) {
            //errorAlertFlag = TRUE;
            updateValueFlag = TRUE;
            m_newLineEnd = m_minMaxSelectionRectangle.top;

        }// end "if (m_newLineEnd < m_minMaxSelectionRectangle.top)"

        else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom && (m_value.length()!= 0)) {
            //errorAlertFlag = TRUE;
            updateValueFlag = TRUE;
            m_newLineEnd = m_minMaxSelectionRectangle.bottom;

        } // end "else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom)"

        m_selectionRectanglePtr->bottom = (SInt32) m_newLineEnd;

    }// end "if (m_selectionUnits == kLineColumnUnits)"

    else // m_selectionUnits != kLineColumnUnits
    {
        //if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0)) {        
        if(m_value.length()!= 0 && !(m_value == '-' && m_value.length()==1)){
            m_value.ToDouble(&m_newLineEnd);
        } else // m_valuePtr[0] == 0
            m_newLineEnd = 0;

        m_coordinateRectanglePtr->top = m_newLineEnd;

    } // end "if (m_selectionUnits == kLineColumnUnits)"

    if (previousValue != m_newLineEnd && !m_coordinateUnitsChangingFlag)
        m_valueChangedFlag = TRUE;

	if (updateValueFlag) {
		if (m_selectionUnits == kLineColumnUnits) {		
			//UInt32 newLineEndValue = (UInt32)m_newLineEnd;	
			//newLineEnd->ChangeValue(wxString::Format(wxT("%d"), newLineEndValue));
			newLineEnd->ChangeValue(wxString::Format(wxT("%.0lf"), m_newLineEnd));
			}
		else
			newLineEnd->ChangeValue(wxString::Format(wxT("%lf"), m_newLineEnd));
		}

    //if (errorAlertFlag)
    //    NumberErrorAlert((SInt32) m_newLineStart, this, IDC_NewLineEnd);

    //	CheckOKFlag ();

} // end "OnChangeNewLineEnd" 


/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg message handlers

void CMEditCoordinatesDlg::OnInitDialog(wxInitDialogEvent& event) {
    Boolean applyToAllWindowsFlag,
            useStartLineColumnFlag;


    // Get maximum number of lines and columns allowed.

    CMDialog::OnInitDialog(event);

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

//    m_listSelectionUnits = GetComboListSelection(IDC_CoordinateUnits,
//            m_selectionUnits);
    m_listSelectionUnits = (int)m_selectionUnits;
    
    if (m_listSelectionUnits < 0)
        m_listSelectionUnits = 0;

    // Update the dialog box parameters and then center the dialog.

    //if (UpdateData(FALSE))
    if (CMEditCoordinatesDlg::TransferDataToWindow())
        PositionDialogWindow();

    // Set default text selection to first edit text item 

    SelectDialogItemText(this, IDC_NewLineStart, 0, SInt16_MAX);
    SetSizerAndFit(bSizer200);
    //this->Layout();
    this->Fit();
    //return FALSE; // return TRUE  unless you set the focus to a control 

} // end "OnInitDialog"


void CMEditCoordinatesDlg::OnPreview(wxCommandEvent& event) {	

	wxTextCtrl* newColumnStart = (wxTextCtrl*) FindWindow(IDC_NewColumnStart);
	wxTextCtrl* newColumnEnd = (wxTextCtrl*) FindWindow(IDC_NewColumnEnd);
	wxTextCtrl* newLineStart = (wxTextCtrl*) FindWindow(IDC_NewLineStart);
	wxTextCtrl* newLineEnd = (wxTextCtrl*) FindWindow(IDC_NewLineEnd);
	
	m_LineStartString = newLineStart->GetValue();
	m_LineEndString = newLineEnd->GetValue();
	m_ColumnStartString = newColumnStart->GetValue();
	m_ColumnEndString = newColumnEnd->GetValue();

	if (CheckLineColumnValues(NULL))
		{
		wxCheckBox* applyToAllWindowsFlag = (wxCheckBox*) FindWindow(IDC_ApplyToAllCheckbox);
		m_applyToAllWindowsFlag = applyToAllWindowsFlag->GetValue();

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


void CMEditCoordinatesDlg::OnSelendokCoordinateUnits(wxCommandEvent& event) 
{
	wxChoice* comboBoxPtr;
	SInt16 previousSelectionUnits;


	//	if (CheckLineColumnValues (NULL))
	//		{
	comboBoxPtr = (wxChoice*) FindWindow(IDC_CoordinateUnits);

	previousSelectionUnits = m_selectionUnits;

	m_listSelectionUnits = comboBoxPtr->GetSelection();

	m_selectionUnits = (SInt16) m_listSelectionUnits;
	//(SInt16) comboBoxPtr->GetItemData(m_listSelectionUnits);


	if (m_selectionUnits >= 0 && previousSelectionUnits != m_selectionUnits) {
		//DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);
		wxCheckBox* applyToAllWindowsFlag = (wxCheckBox*) FindWindow(IDC_ApplyToAllCheckbox);
		m_applyToAllWindowsFlag = applyToAllWindowsFlag->GetValue();

				// Change the validators.
		
		if (m_selectionUnits == kLineColumnUnits) {
			//wxIntegerValidator<unsigned long int> uLongIntValue(&m_uInt32ValueCheck);
			//uLongIntValue.SetRange(1, kMaxNumberLines);
			//m_textCtrl182->SetValidator(uLongIntValue);
			//m_textCtrl183->SetValidator(uLongIntValue);
			//m_textCtrl1821->SetValidator(uLongIntValue);
			//m_textCtrl1831->SetValidator(uLongIntValue);
			wxFloatingPointValidator<double> doubleValue(0, &m_doubleValueCheck);
			doubleValue.SetRange(1, kMaxNumberLines);
			m_textCtrl182->SetValidator(doubleValue);
			m_textCtrl183->SetValidator(doubleValue);
			m_textCtrl1821->SetValidator(doubleValue);
			m_textCtrl1831->SetValidator(doubleValue);
			
			}	// end "if (m_selectionUnits == kLineColumnUnits)"
			
		else {	// m_selectionUnits != kLineColumnUnits
			int numberDecimals = 4;
			if (m_selectionUnits == kLatLongUnits)
            numberDecimals = 8;
				
			wxFloatingPointValidator<double> doubleValue(numberDecimals, &m_doubleValueCheck);
			m_textCtrl182->SetValidator(doubleValue);
			m_textCtrl183->SetValidator(doubleValue);
			m_textCtrl1821->SetValidator(doubleValue);
			m_textCtrl1831->SetValidator(doubleValue);
			
			}	// end "if (m_selectionUnits != kLineColumnUnits)"

		m_coordinateUnitsChangingFlag = TRUE;

		EditLineColumnDialogSetStartLC(this,
				 m_selectionUnits,
				 m_applyToAllWindowsFlag);

		EditSelectionDialogSetCoordinates(
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

		} // end "m_listSelectionUnits >= 0  && ..."

	//		}		// end "if (CheckLineColumnValues ())"
	this->Layout();
	this->Fit();
	
} // end "OnSelendokCoordinateUnits"


bool CMEditCoordinatesDlg::TransferDataFromWindow ()

{
	wxCheckBox* applyToAllWindowsFlag = (wxCheckBox*) FindWindow(IDC_ApplyToAllCheckbox);
	wxCheckBox* useStartLineColumnFlag = (wxCheckBox*) FindWindow(IDC_StartLCCheckBox);
	wxChoice* listSelectionUnits = (wxChoice*) FindWindow(IDC_CoordinateUnits);
	wxTextCtrl* newColumnStart = (wxTextCtrl*) FindWindow(IDC_NewColumnStart);
	wxTextCtrl* newColumnEnd = (wxTextCtrl*) FindWindow(IDC_NewColumnEnd);
	wxTextCtrl* newLineStart = (wxTextCtrl*) FindWindow(IDC_NewLineStart);
	wxTextCtrl* newLineEnd = (wxTextCtrl*) FindWindow(IDC_NewLineEnd);

	m_applyToAllWindowsFlag = applyToAllWindowsFlag->GetValue();
	m_useStartLineColumnFlag = useStartLineColumnFlag->GetValue();
	m_listSelectionUnits = listSelectionUnits->GetSelection();
	
	m_LineStartString = newLineStart->GetValue();
	m_LineEndString = newLineEnd->GetValue();
	m_ColumnStartString = newColumnStart->GetValue();
	m_ColumnEndString = newColumnEnd->GetValue();

			// The way double value is passed 
	m_ColumnStartString.ToDouble(&m_newColumnStart);
	m_ColumnEndString.ToDouble(&m_newColumnEnd);
	m_LineStartString.ToDouble(&m_newLineStart);
	m_LineEndString.ToDouble(&m_newLineEnd);
	
	if (m_pointType == kPolygonType)
		{
		m_ColumnEndString = m_ColumnStartString;
		m_LineEndString = m_LineStartString;
		
		m_newColumnEnd = m_newColumnStart;
		m_newLineEnd = m_newLineStart;
		
		}	// end "if (m_pointType == kPolygonType)"

	if (!CheckLineColumnValues (NULL))
		return false;

	return true;
	
}	// end "TransferDataFromWindow"


bool CMEditCoordinatesDlg::TransferDataToWindow() {
    wxCheckBox* applyToAllWindowsFlag = (wxCheckBox*) FindWindow(IDC_ApplyToAllCheckbox);
    wxCheckBox* useStartLineColumnFlag = (wxCheckBox*) FindWindow(IDC_StartLCCheckBox);
    wxChoice* listSelectionUnits = (wxChoice*) FindWindow(IDC_CoordinateUnits);
    //wxTextCtrl* newColumnStart = (wxTextCtrl*) FindWindow(IDC_NewColumnStart);
    //wxTextCtrl* newColumnEnd = (wxTextCtrl*) FindWindow(IDC_NewColumnEnd);
    //wxTextCtrl* newLineStart = (wxTextCtrl*) FindWindow(IDC_NewLineStart);
    //wxTextCtrl* newLineEnd = (wxTextCtrl*) FindWindow(IDC_NewLineEnd);


    applyToAllWindowsFlag->SetValue(m_applyToAllWindowsFlag);
    useStartLineColumnFlag->SetValue(m_useStartLineColumnFlag);
    listSelectionUnits->SetSelection(m_listSelectionUnits);
	 
			// The values have already been set.
    //newColumnStart->ChangeValue(wxString::Format(wxT("%.0lf"), m_newColumnStart));
    //newColumnEnd->ChangeValue(wxString::Format(wxT("%.0lf"), m_newColumnEnd));
    //newLineStart->ChangeValue(wxString::Format(wxT("%.0lf"), m_newLineStart));
    //newLineEnd->ChangeValue(wxString::Format(wxT("%.0lf"), m_newLineEnd));

    return true;
	 
}		// end "TransferDataToWindow"
