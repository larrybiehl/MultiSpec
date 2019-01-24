// LReformatTransformDialog.h : header file
//
// Revised by Larry Biehl on 01/18/2019
//
#if !defined __LRTRADLG_H__
#define	__LRTRADLG_H__  

#include "LDialog.h"
#include "LOneColumnDialog.h"

typedef bool BOOL;
typedef wxString CString;
typedef long unsigned int UINT;

/////////////////////////////////////////////////////////////////////////////
// CMReformatTransform dialog

class CMReformatTransformDlg : public CMDialog {
    // Construction
public:
    //CMReformatTransformDlg(wxWindow* pParent = NULL); // standard constructor
    CMReformatTransformDlg(wxWindow* pParent = NULL,  wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Reformat Transform Parameters")); // standard constructor
    
    ~CMReformatTransformDlg();

    Boolean DoDialog(
            UInt16* recommendNumberOfBitsPtr,
            SInt16 bandInterleaveSelection);

    // Dialog Data
    //{{AFX_DATA(CMReformatTransformDlg)

    enum {
        IDD = IDD_ReformatTransform
    };
    double m_adjustDivisor;
    double m_adjustFactor;
    double m_adjustOffset;
    double m_adjustSelectedChannelsFactor;
    double m_functionFactor;
    double m_transformFactor;
    double m_transformOffset;
    CString m_denominatorString;
    CString m_numeratorString;
    double m_scaleFactor;
    //	int			m_channelSelection;
    UINT m_adjustSelectedChannel;
    UINT m_kthSmallestElement;
    UINT m_minimumNumberBits;
    UINT m_minSelectedNumberBits;
    int m_transformCode;
    int m_functionCode;
    //}}AFX_DATA

    // Implementation
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support 

    void ShowHideAdjustChannelItems(
            DialogPtr dialogPtr,
            Boolean showFlag);

    void ShowHideAdjustSelectedChannelsByChannelItems(
            DialogPtr dialogPtr,
            Boolean showFlag);

    void ShowHideAlgebraicTransformItems(
            DialogPtr dialogPtr,
            Boolean showFlag);

    void ShowHidePCTransformItems(
            DialogPtr 										dialogPtr,
            Boolean 											showFlag,
				SInt16											eigenSource);

    void ShowHideFunctionChannelsItems(
            DialogPtr dialogPtr,
            Boolean showFlag,
            UInt16 functionChannelCode);

    // Generated message map functions
    //{{AFX_MSG(CMReformatTransformDlg)
    //virtual BOOL OnInitDialog();
    void CreateControls();
    void OnInitDialog(wxInitDialogEvent& event);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    
    void OnAdjustSelectedChannels(wxCommandEvent& event);
	void OnAdjustSelectedChannelsByChannel(wxCommandEvent& event);
	//void OnButtonPress(wxKeyEvent& event);
    void OnRTAlgebraicTransformation(wxCommandEvent& event);
    void OnRTEigenvectors(wxCommandEvent& event);
    void OnRTFunctionOfChannels(wxCommandEvent& event);
    void OnRTNoTransformation(wxCommandEvent& event);
    void OnSelendokEVEigenvectors(wxCommandEvent& event);
    void OnSelendokReformatFunctions(wxCommandEvent& event);
    //}}AFX_MSG
    //DECLARE_MESSAGE_MAP()
    DECLARE_EVENT_TABLE();

    double m_maxValue;
    double m_minValue;

    char* m_denominatorStringPtr;
    char* m_numeratorStringPtr;
    
    
    SInt16 m_bandInterleaveSelection;
    SInt16 m_eigenSource;
    SInt16 m_maxAdjustOffset;
    UInt16 m_maxAdjustDivisor;
    UInt16 m_maxAdjustFactor;
    UInt16 m_maxChannel;
    UInt16 m_numberEigenvectors;

    Boolean m_initializedFlag;
    
    wxBoxSizer* bSizer84;
    wxBoxSizer* bSizer103;
    wxRadioButton* m_radioBtn5;
    wxRadioButton* m_radioBtn6;
    wxRadioButton* m_radioBtn7;
    wxRadioButton* m_radioBtn8;
    wxRadioButton* m_radioBtn9;
    wxRadioButton* m_radioBtn10;
    wxComboBox* m_comboBox13;
    wxComboBox* m_comboBox14;
    wxStaticText* m_staticText127;
    wxStaticText* m_staticText128;
    wxStaticText* m_staticText129;
    wxStaticText* m_staticText130;
    wxStaticText* m_staticText131;
    wxStaticText* m_staticText132;
    wxStaticText* m_staticText133;
    wxStaticText* m_staticText134;
    wxStaticText* m_staticText135;
    wxStaticText* m_staticText136;
    wxStaticText* m_staticText137;
    wxStaticText* m_staticText140;
    wxStaticText* m_staticText141;
    wxStaticText* m_staticText142;
    wxStaticText* m_staticText143;
    wxStaticText* m_staticText144;
    wxStaticText* m_staticText145;
    wxTextCtrl* m_textCtrl48;
    wxTextCtrl* m_textCtrl49;
    wxTextCtrl* m_textCtrl50;
    wxTextCtrl* m_textCtrl51;
    wxTextCtrl* m_textCtrl52;
    wxTextCtrl* m_textCtrl53;
    wxTextCtrl* m_textCtrl54;
    wxTextCtrl* m_textCtrl56;
    wxTextCtrl* m_textCtrl57;
    wxTextCtrl* m_textCtrl58;
    wxTextCtrl* m_textCtrl59;
    wxTextCtrl* m_textCtrl60;
    wxButton* m_button18;
    wxButton* m_button19;
    
};

#endif	// !defined __LRTRADLG_H__
