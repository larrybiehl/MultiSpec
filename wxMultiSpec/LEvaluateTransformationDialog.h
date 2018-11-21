// LEvaluateTransformationDialog.h : header file
// 
// Revised by Larry Biehl on 06/21/2017
//
#if !defined __LTRANDLG_H__
#define	__LTRANDLG_H__             
	     
#include "LDialog.h"

//typedef bool BOOL;
/////////////////////////////////////////////////////////////////////////////
// CMEvalTransformDialog dialog

class CMEvalTransformDialog : public CMDialog
{
 // Construction
public:
    CMEvalTransformDialog(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, 
    const wxString& title = wxT("Check Transformation Matrix"));   // standard constructor
    ~CMEvalTransformDialog();		// standard desctructor
    
    bool TransferDataToWindow();
    bool TransferDataFromWindow();
    
    Boolean DoDialog( EvaluateTransformSpecsPtr evaluateTransformSpecsPtr);

    
// Dialog Data    
    enum { IDD = IDD_EvalTransform };
    Boolean	m_checkTransformationFlag;
    Boolean	m_listCheckMatrixFlag;
    Boolean	m_listEigenvaluesFlag;
    Boolean	m_listOriginalMatrixFlag;
    
    

// Implementation
protected:
    void CreateControls();
    void OnCheckTransform(wxCommandEvent& event);  
    void SetListCheckTransform( Boolean checkTransformFlag);
    
    void  OnInitDialog(wxInitDialogEvent& event);
	
    EvaluateTransformSpecsPtr   m_evaluateTransformSpecsPtr;
    Boolean                     m_initializedFlag;
    
     /*Dialog items*/
    wxBoxSizer* bSizer107;
    wxBoxSizer* bSizer108;
    wxBoxSizer* bSizer175;
    wxCheckBox* m_checkBox14;
    wxCheckBox* m_checkBox15;
    wxCheckBox* m_checkBox16;
    wxCheckBox* m_checkBox32;
    wxCheckBox* m_checkBox23;
    wxStaticText* m_staticText196;
    wxButton* m_button20;
    wxButton* m_button21;
    
    DECLARE_EVENT_TABLE();
    
};
#endif // !defined __LTRANDLG_H__