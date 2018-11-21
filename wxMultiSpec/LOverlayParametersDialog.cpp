// LOverlayParametersDialog.cpp : implementation file
//
//	Revised by Larry Biehl on 11/16/2018
//  

#include	"SMultiSpec.h" 

#include "LOverlayParametersDialog.h"
#include "wx/valgen.h"
#include "wx/valnum.h"

//#include "SExternalGlobals.h"

extern void OverlayDialogInitialize(
        DialogPtr dialogPtr,
        WindowInfoPtr windowInfoPtr,
        SInt16 overlayCode,
        SInt16 overlayIndex,
        RGBColor* overlayColorPtr,
        UInt16* lineThicknessPtr,
        UInt16* transparencyPtr);

extern void OverlayDialogOK(
        WindowInfoPtr windowInfoPtr,
        SInt16 overlayCode,
        SInt16 overlayIndex,
        RGBColor* overlayColorPtr,
        UInt16 value);



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMOverlayParameterDlg dialog

CMOverlayParameterDlg::CMOverlayParameterDlg(wxWindow* pParent, wxWindowID id, const wxString& title)
: CMDialog(CMOverlayParameterDlg::IDD, pParent, title) {
    //{{AFX_DATA_INIT(CMOverlayParameterDlg)
    m_overlayValue = 1;
    //}}AFX_DATA_INIT

    m_windowInfoPtr = NULL;
    m_overlayIndex = 0;
    m_minValue = 0;
    m_maxValue = 20;

    m_initializedFlag = CMDialog::m_initializedFlag;
    CreateControls();
}

//void CMOverlayParameterDlg::DoDataExchange(CDataExchange* pDX) {
//    CDialog::DoDataExchange(pDX);
//    //{{AFX_DATA_MAP(CMOverlayParameterDlg)
//    DDX_Text(pDX, IDC_LineThickness, m_overlayValue);
//    DDV_MinMaxInt(pDX, m_overlayValue, m_minValue, m_maxValue);
//    //}}AFX_DATA_MAP
//}

bool CMOverlayParameterDlg::TransferDataFromWindow() {
    wxTextCtrl* overlayValue = (wxTextCtrl*)FindWindow(IDC_LineThickness);
    m_overlayValue = wxAtoi(overlayValue->GetValue());
    
    return true;
}

bool CMOverlayParameterDlg::TransferDataToWindow() {
    wxTextCtrl* overlayValue = (wxTextCtrl*)FindWindow(IDC_LineThickness);
    
    overlayValue->ChangeValue(wxString::Format(wxT("%i"), m_overlayValue));
    
    return true;
}

BEGIN_EVENT_TABLE(CMOverlayParameterDlg, CMDialog)
EVT_INIT_DIALOG(CMOverlayParameterDlg::OnInitDialog)
EVT_BUTTON(IDC_LineColorPrompt, CMOverlayParameterDlg::OnOverlayColor)
//ON_WM_PAINT()
END_EVENT_TABLE()


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the overlay
//							specification dialog box to the user and copy the
//							revised info to the overlay specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			OverlayControlDialog in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/25/2001
//	Revised By:			Larry L. Biehl			Date: 07/09/2015	

Boolean
CMOverlayParameterDlg::DoDialog(
        WindowInfoPtr windowInfoPtr,
        SInt16 overlayCode,
        SInt16 overlayIndex) {
    Boolean continueFlag = FALSE;

    SInt16 returnCode;


    // Make sure intialization has been completed.

	if (!m_initializedFlag)
		return (FALSE);

	m_windowInfoPtr = windowInfoPtr;
	m_overlayCode = overlayCode;
	m_overlayIndex = overlayIndex;

	returnCode = ShowModal();

	if (returnCode == wxID_OK) {
		OverlayDialogOK (windowInfoPtr,
                m_overlayCode,
                m_overlayIndex,
                &m_currentSelectedColor,
                m_overlayValue);

        continueFlag = TRUE;

	} // end "if (returnCode == IDOK)"

	return (continueFlag);

} // end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMDisplayThematicDlg message handlers

void CMOverlayParameterDlg::OnInitDialog(wxInitDialogEvent& event) {
	UInt16 lineThickness;

	UInt16 transparency;


	CMDialog::OnInitDialog(event);		

	OverlayDialogInitialize(this,
			m_windowInfoPtr,
			m_overlayCode,
			m_overlayIndex,
			&m_currentSelectedColor,
			&lineThickness,
			&transparency);

			// Overlay color

	//	MHideDialogItem (this, IDC_LineColor);
	m_lineThickness = lineThickness;
	m_transparency = transparency;

	if (m_overlayCode == kImageOverlay) {
		m_overlayValue = m_transparency;
		m_minValue = 0;
		m_maxValue = 100;

		}// end "if (m_overlayCode == kImageOverlay)"

	else // m_overlayCode != kImageOverlay
		{
		m_overlayValue = m_lineThickness;
		m_minValue = 1;
		m_maxValue = 20;

		} // end "else m_overlayCode != kImageOverlay"

	if(TransferDataToWindow())
		PositionDialogWindow();

	SelectDialogItemText(this, IDC_LineThickness, 0, SInt16_MAX);
	SetSizerAndFit(bSizer107);

} // end "OnInitDialog" 



/////////////////////////////////////////////////////////////////////////////
// CMOverlayParameterDlg message handlers

void CMOverlayParameterDlg::OnOverlayColor(wxCommandEvent& event) {
    RGBColor newRGB;

    
    if (SelectColor(3,
            &m_currentSelectedColor,
            &newRGB)) {
        m_currentSelectedColor = newRGB;

        this->Update();

    } // end "if ( SelectColor ( 3, ..."

    SelectDialogItemText(this, IDC_LineThickness, 0, SInt16_MAX);

    
} // end "OnOverlayColor"

//void CMOverlayParameterDlg::OnPaint() {
//    if (m_overlayCode == kVectorOverlay) {
//        CPaintDC dc(this); // device context for painting
//
//        // TODO: Add your message handler code here
//        COLORREF newColor;
//
//        RECT colorChipRect;
//
//        CWnd *cWndPtr;
//
//        SInt32 eightBitColor;
//
//        eightBitColor = m_currentSelectedColor.blue / 256;
//        newColor = eightBitColor << 16;
//
//        eightBitColor = m_currentSelectedColor.green / 256;
//        newColor += eightBitColor << 8;
//
//        eightBitColor = m_currentSelectedColor.red / 256;
//        newColor += eightBitColor;
//
//        cWndPtr = (CWnd*) GetDlgItem(IDC_LineColor);
//        cWndPtr->GetClientRect(&colorChipRect);
//        cWndPtr->MapWindowPoints(this, &colorChipRect);
//
//        dc.Rectangle(&colorChipRect);
//
//        colorChipRect.top++;
//        colorChipRect.bottom--;
//        colorChipRect.left++;
//        colorChipRect.right--;
//        dc.FillSolidRect(&colorChipRect, newColor);
//
//        // Do not call CMDialog::OnPaint() for painting messages
//
//    } // end "if (m_overlayCode == kVectorOverlay)"
//
//} // end "OnPaint"



void CMOverlayParameterDlg::CreateControls ()

{
    this->SetSizeHints (wxDefaultSize, wxDefaultSize);

    //wxBoxSizer* bSizer107;
    bSizer107 = new wxBoxSizer(wxVERTICAL);

    wxFlexGridSizer* fgSizer7;
    fgSizer7 = new wxFlexGridSizer(2, 2, 0, 0);
    fgSizer7->SetFlexibleDirection(wxBOTH);
    fgSizer7->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_button23 = new wxButton(this, IDC_LineColorPrompt, wxT("Line Color..."), wxDefaultPosition, wxDefaultSize, 0);
    fgSizer7->Add(m_button23, 0, wxALL, 5);

    m_bitmap3 = new wxStaticBitmap(this, IDC_LineColor, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);
    fgSizer7->Add(m_bitmap3, 0, wxALL, 5);

    m_staticText139 = new wxStaticText(this, IDC_ValuePrompt, wxT("Line Thickness:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText139->Wrap(-1);
    fgSizer7->Add(m_staticText139, 0, wxALIGN_CENTER | wxALL, 5);

    m_textCtrl55 = new wxTextCtrl(this, IDC_LineThickness, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    fgSizer7->Add(m_textCtrl55, 0, wxALL, 5);
    wxIntegerValidator<int> _val(&m_lineThickness);
    _val.SetRange(1, 20);
    m_textCtrl55->SetValidator(_val);


    bSizer107->Add(fgSizer7, 0, wxALL | wxEXPAND, 5);
	/*
    wxBoxSizer* bSizer110;
    bSizer110 = new wxBoxSizer(wxHORIZONTAL);

    m_button20 = new wxButton(this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer110->Add(m_button20, 0, wxALL, 5);

    m_button21 = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer110->Add(m_button21, 0, wxALL, 5);


    bSizer107->Add(bSizer110, 0, wxALIGN_RIGHT, 5);
	*/
	CreateStandardButtons (bSizer107);

	this->SetSizer (bSizer107);
	this->Layout ();

	this->Centre(wxBOTH);
}
