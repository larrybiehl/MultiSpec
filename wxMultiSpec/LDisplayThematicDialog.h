// LDisplayThematicDialog.h : header file
// 
// Revised by Larry Biehl on 04/26/2018
//
#if !defined __LDISTDLG_H__
#define	__LDISTDLG_H__

#include "LDialog.h"
#include "SMultiSpec.h"
#include "wx/wx.h"


class CMDisplayThematicDlg : public CMDialog {
    // Construction
public:
    CMDisplayThematicDlg();
    CMDisplayThematicDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Thematic Display Specifications"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = /*wxDefaultSize*/wxSize( 200,200 ), long style = wxDEFAULT_DIALOG_STYLE); // standard constructor   

    ~CMDisplayThematicDlg(); // standard desctructor


    Boolean DoDialog(
            DisplaySpecsPtr displaySpecsPtr);

    // Dialog Data
    //{{AFX_DATA(CMDisplayThematicDlg)

    enum {
        IDD = IDD_DisplayThematic
    };
    int m_classGroupSelection;
    int m_classSelection;
    bool m_showLegendFlag;
    int m_paletteSelection;
    wxString m_numberClasses;
    wxString m_numberGroups;
    wxString m_fileName;
    double m_magnification;
    bool m_useThresholdFileFlag;
    bool m_includeVectorOverlaysFlag;

    // Implementation
protected:

    void CreateControls();
    void OnInitDialog(wxInitDialogEvent& event);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    
    void OnSelendokPaletteCombo(wxCommandEvent& event);
    void OnDropdownPaletteCombo(wxCommandEvent& event);
    void OnSelendokClassesGroupsCombo(wxCommandEvent& event);
    void OnSelendokChannelComboDropDown(wxCommandEvent& event);    
    void OnBackgroundColor(wxCommandEvent& event);
    void OnPaint();
	 //void OnCharHook (wxKeyEvent& event);
    DECLARE_EVENT_TABLE()
            
    Boolean m_initializedFlag,
    m_localPaletteUpToDateFlag,
    m_paletteChangedFlag;

    RGBColor m_oldRGB;

    DisplaySpecsPtr m_displaySpecsPtr;

    Handle m_localClassGroupsHandle;

    SInt16 *m_localClassGroupsPtr,
    *m_localClassToGroupPtr;

    UInt32 m_localDisplayClasses,
    m_localDisplayGroups;

    SInt16 m_localAllSubsetClass,
    m_localAllSubsetGroup,
    m_localNumberGroups;
    
    wxBoxSizer* bSizer18;
    //wxString m_LineStartString;
    //wxString m_LineEndString;
    //wxString m_LineIntervalString;
    //wxString m_ColumnStartString;
    //wxString m_ColumnEndString;
    //wxString m_ColumnIntervalString;
  
    wxBitmapButton* m_bpButton4;
    wxBitmapButton* m_bpButton5;
    wxStaticBitmap* m_bitmap1;
            
    wxStaticText* m_staticText30;
    wxStaticText* m_staticText31;
    wxStaticText* m_staticText32;
    wxStaticText* m_staticText33;
    wxStaticText* m_staticText34;
    wxStaticText* m_staticText35;
    wxStaticText* m_staticText38;
    wxStaticText* m_staticText37;
    wxStaticText* m_staticText36;
    wxStaticText* m_staticText63;
    wxStaticText* m_staticText64;
    wxStaticText* m_staticText65;
    wxStaticText* m_staticText66;
    wxStaticText* m_staticText60;
    wxStaticText* m_staticText62;
    wxStaticText* m_staticText23;
    
    wxTextCtrl* m_textCtrl14;
    wxTextCtrl* m_linestartctrl;
    wxTextCtrl* m_lineendctrl;
    wxTextCtrl* m_lineintctrl;
    wxTextCtrl* m_colstartctrl;
    wxTextCtrl* m_colendctrl;
    wxTextCtrl* m_colintctrl;
    
    wxComboBox* m_comboBox1;
    wxComboBox* m_comboBox3;
	 wxComboBox* m_paletteCombo;
    
    wxCheckBox* display_legend;
    
    wxButton* m_button3;
    wxButton* m_button4;
    wxButton* m_button5;
    
public:
    void OnBnClickedCheck1();
};

#endif // !defined __LDISTDLG_H__
