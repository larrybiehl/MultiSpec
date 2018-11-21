// CMGaussianParameterDlg.h : header file
//       
#if !defined(AFX_WGAUSSIANPARAMETERDLG_H__12DCBA43_80E8_11D7_8F8E_00105AA88EE3__INCLUDED_)
#define AFX_WGAUSSIANPARAMETERDLG_H__12DCBA43_80E8_11D7_8F8E_00105AA88EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WGaussianParameterDlg.h : header file
//      

#include "LDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMGaussianParameterDlg dialog

class CMGaussianParameterDlg : public CMDialog {
    // Construction
public:
    CMGaussianParameterDlg(wxWindow* pParent = NULL,  wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Gaussian Stretch Parameter")); // standard constructor

    Boolean DoDialog(
            double* gaussianStretchPtr);

    // Dialog Data
    //{{AFX_DATA(CMGaussianParameterDlg)

    enum {
        IDD = IDD_GaussianParameter
    };
    double m_gaussianStretch;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMGaussianParameterDlg)
protected:
    void CreateControls();
    void OnInitDialog(wxInitDialogEvent& event);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    
    wxBoxSizer* bSizer308;
    wxStaticText* m_staticText289;
    wxStaticText* m_staticText290;
    wxTextCtrl* m_textCtrl130;
    wxButton* m_button75;
    wxButton* m_button76;
    
    DECLARE_EVENT_TABLE()
    //    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
    //protected:

    // Generated message map functions
    //{{AFX_MSG(CMGaussianParameterDlg)
    //    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    //    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WGAUSSIANPARAMETERDLG_H__12DCBA43_80E8_11D7_8F8E_00105AA88EE3__INCLUDED_)
