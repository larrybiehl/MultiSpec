// LStatisticsListDialog.h : header file
// CMListStatsDialog dialog
//
// Revised by Larry Biehl on 06/21/2017
//

#include "SMultiSpec.h"

#include "LDialog.h"
#include "LImageView.h"
#include "wx/wx.h"

typedef long unsigned int UINT;

class CMListStatsDialog : public CMDialog {
    // Construction
public:
    
    CMListStatsDialog(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, 
    const wxString& title = wxT("List Stats"));   // standard constructor
    Boolean DoDialog(
            SInt16 statsWindowMode);

    void CheckListFieldClassSettings(void);
    bool TransferDataToWindow();
    bool TransferDataFromWindow();

    // Dialog Data
    //{{AFX_DATA(CMListStatsDialog)

    enum {
        IDD = IDD_ListStats
    };
    Boolean m_listClassFlag;
    Boolean m_listFieldFlag;
    Boolean m_listMeansStdDevFlag;
    Boolean m_listCovarianceFlag;
    Boolean m_listCorrelationFlag;
    Boolean m_featureTransformationFlag;
    UINT m_listMeanStdPrecision;
    UINT m_listCovCorPrecision;
    wxString m_meanPrecisionString;
    wxString m_covPrecisionString;
    //}}AFX_DATA

    // Implementation
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

    // Generated message map functions
    void OnInitDialog(wxInitDialogEvent& event);
    void OnClasses(wxCommandEvent& event);
    void OnFields(wxCommandEvent& event);
    
    void CreateControls();

    DECLARE_EVENT_TABLE()

    SInt16 m_statsWindowMode;
    
    wxStaticText* m_staticText258;
    wxStaticText* m_staticText259;
    wxStaticText* m_staticText264;
    wxStaticText* m_staticText265;
    wxStaticText* m_staticText266;
    wxCheckBox* m_checkBox44;
    wxCheckBox* m_checkBox45;
    wxCheckBox* m_checkBox46;
    wxCheckBox* m_checkBox47;
    wxCheckBox* m_checkBox48;
    wxTextCtrl* m_textCtrl125;
    wxTextCtrl* m_textCtrl126;
    wxCheckBox* m_checkBox49;
    wxButton* m_button20;
    wxButton* m_button21;
    
};
