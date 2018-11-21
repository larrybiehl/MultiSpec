// LOverlayParametersDialog.h : implementation file
//
//	Revised by Larry Biehl on 06/20/2017
//  
#if !defined(AFX_WOVERLAYPARAMETERDLG_H)
#define AFX_WOVERLAYPARAMETERDLG_H

#include "LDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMOverlayParameterDlg dialog

class CMOverlayParameterDlg : public CMDialog {
    // Construction
public:
    CMOverlayParameterDlg(wxWindow* pParent = NULL,  wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Overlay Parameters")); // standard constructor


    Boolean DoDialog(
            WindowInfoPtr windowInfoPtr,
            SInt16 overlayCode,
            SInt16 selection);

    // Dialog Data
    //{{AFX_DATA(CMOverlayParameterDlg)

    enum {
        IDD = IDD_OverlayParameters
    };
    int m_overlayValue;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMOverlayParameterDlg)
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    void CreateControls();
    void OnInitDialog(wxInitDialogEvent& event);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CMOverlayParameterDlg)
    //virtual BOOL OnInitDialog();
    void OnOverlayColor(wxCommandEvent& event);
    void OnPaint();
    //}}AFX_MSG
    //DECLARE_MESSAGE_MAP()
    DECLARE_EVENT_TABLE();

    WindowInfoPtr m_windowInfoPtr;

    Boolean m_initializedFlag;

    SInt16 m_overlayCode,
    m_overlayIndex;

    int m_lineThickness,
    m_maxValue,
    m_minValue,
    m_transparency;
	 
	 RGBColor	m_currentSelectedColor;
    
    wxBoxSizer* bSizer107;
    wxButton* m_button23;
    wxStaticBitmap* m_bitmap3;
    wxTextCtrl* m_textCtrl55;
    wxStaticText* m_staticText139;
    wxButton* m_button20;
    wxButton* m_button21;        
};

#endif // !defined(AFX_WOVERLAYPARAMETERDLG_H)
