// LListResultsOptionsDialog.h : header file
//    
// Revised by Larry Biehl on 06/20/2017
// 
#if !defined __LLIOPDLG_H__
#define	__LLIOPDLG_H__             

#include "LDialog.h"

typedef bool BOOL;

/////////////////////////////////////////////////////////////////////////////
// CMListResultsOptionsDlg dialog

class CMListResultsOptionsDlg : public CMDialog {
    // Construction
public:
    CMListResultsOptionsDlg(wxWindow* pParent = NULL, wxWindowID id = IDD_ClassWeights, const wxString& title = wxT("Classification Results List Options")); // standard constructor  

    void DoDialog(
            SInt16* listResultsTrainingCodePtr,
            SInt16* listResultsTestCodePtr);

    // Dialog Data
    //{{AFX_DATA(CMListResultsOptionsDlg)

    enum {
        IDD = IDD_ResultsListOptions
    };
    BOOL m_trainAreasUsedFlag;
    BOOL m_trainAreaSummariesFlag;
    BOOL m_trainAreaPerformanceFlag;
    BOOL m_trainClassPerformanceFlag;
    BOOL m_testAreasUsedFlag;
    BOOL m_testAreaSummariesFlag;
    BOOL m_testAreaPerformanceFlag;
    BOOL m_testClassPerformanceFlag;
    //}}AFX_DATA

    // Implementation
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    bool TransferDataToWindow();
    bool TransferDataFromWindow();
    void CreateControls();
    // Generated message map functions
    //{{AFX_MSG(CMListResultsOptionsDlg)
    void OnInitDialog(wxInitDialogEvent& event);
    //}}AFX_MSG
    //DECLARE_MESSAGE_MAP()
    DECLARE_EVENT_TABLE();
    

    SInt16 m_listResultsTestCode,
    m_listResultsTrainingCode;
    
    wxBoxSizer* bSizer142;
    wxStaticText* m_staticText169;
    wxStaticText* m_staticText170;
    wxCheckBox* m_checkBox17;
    wxCheckBox* m_checkBox18;
    wxCheckBox* m_checkBox19;
    wxCheckBox* m_checkBox20;
    wxCheckBox* m_checkBox21;
    wxCheckBox* m_checkBox22;
    wxCheckBox* m_checkBox23;
    wxCheckBox* m_checkBox24;
    wxButton* m_button31;
    wxButton* m_button32;
};

#endif // !defined __LLIOPDLG_H__
