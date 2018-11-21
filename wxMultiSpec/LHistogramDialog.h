// LHistogramDialog.h : header file
//   
// Revised by Larry Biehl on 06/20/2017
//
#if !defined __LHISTDLG_H__
#	define	__LHISTDLG_H__

#include "SMultiSpec.h"

#include "LDialog.h"
#include "wx/wx.h"

/////////////////////////////////////////////////////////////////////////////
// CMHistogramSpecsDlg dialog

class CMHistogramSpecsDlg : public CMDialog {
    DECLARE_DYNAMIC_CLASS( CMHistogramSpecsDlg )
    // Construction
public:
    //CMHistogramSpecsDlg(wxWindow* pParent = NULL); // standard constructor
    CMHistogramSpecsDlg(); // standard constructor
    CMHistogramSpecsDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Histogram Specifications for:"));

    ~CMHistogramSpecsDlg(void); // standard desctructor

    Boolean DoDialog(
            HistogramSpecsPtr histogramSpecsPtr,
            WindowInfoPtr windowInfoPtr,
            FileInfoPtr fileInfoPtr);

    SInt16 GetItemCode(wxString selection);

    // Dialog Data
    //{{AFX_DATA(CMHistogramSpecsDlg)

    enum {
        IDD = IDD_HistogramSpecs
    };
    bool m_computeOnlyMinMaxFlag;
    bool m_includeEmptyBinsFlag;
    bool m_listHistogramValuesFlag;
    bool m_listHistogramSummaryFlag;
    int m_histogramMethod;
    bool m_textWindowFlag;
    bool m_diskFileFlag;
    wxString m_fileName;
    int m_columnsLinesFormat;
    wxString m_supportFileName;
    //wxString m_LineStartString;
    //wxString m_LineEndString;
    //wxString m_LineIntervalString;
    //wxString m_ColumnStartString;
    //wxString m_ColumnEndString;
    //wxString m_ColumnIntervalString;
    //}}AFX_DATA

    // Implementation
protected:
    wxStaticText* m_staticText77;
    wxStaticText* m_staticText78;
    wxStaticText* m_staticText79;
    wxStaticText* m_staticText80;
    wxStaticText* m_staticText81;
    wxComboBox* m_comboBox17;
    wxBitmapButton* m_bpButton4;
    wxBitmapButton* m_bpButton5;
    wxStaticText* m_staticText63;
    wxStaticText* m_staticText64;
    wxStaticText* m_staticText65;
    wxStaticText* m_staticText66;
    wxStaticText* m_staticText60;
    wxTextCtrl* m_linestartctrl;
    wxTextCtrl* m_lineendctrl;
    wxTextCtrl* m_lineintctrl;
    wxStaticText* m_staticText62;
    wxTextCtrl* m_colstartctrl;
    wxTextCtrl* m_colendctrl;
    wxTextCtrl* m_colintctrl;
    wxStaticText* m_staticText94;
    wxStaticText* m_staticText95;
    wxComboBox* m_comboBox18;
    wxCheckBox* m_checkBox23;
    wxCheckBox* m_checkBox18;
    wxCheckBox* m_checkBox19;
    wxStaticText* m_staticText96;
    wxComboBox* m_comboBox19;
    wxCheckBox* m_checkBox20;
    wxCheckBox* m_checkBox21;
    wxCheckBox* m_checkBox22;
    wxButton* m_button26;
    wxButton* m_button27;

    void CheckWriteResultsToSettings(void);
    void CreateControls(void);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();

    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

    //	void				HideAreaHistogramItems (void);

    //	void				HideListHistogramItems (void);

    //	void				HistogramDialogStatisticsFile (
    //							HistogramSpecsPtr		histogramSpecsPtr);

    //	Boolean			Initialize(void);

    void SetListAndEmptyBinsDialogBoxes(
            DialogPtr dialogPtr,
            Boolean minMaxetcOnlyFlag,
            Boolean listHistogramFlag,
            Boolean includeEmptyBinsFlag,
            Boolean lineFormatHistFlag);

    void ShowAreaHistogramItems(void);

    //	void				ShowListHistogramItems (void);

    Boolean UpdateAllChannelsAtOnceFlag(
            DialogPtr dialogPtr,
            SInt16 numberChannels);

    void UpdateListAndEmptyBinsDialogBoxes(void);

    // Generated message map functions
    //{{AFX_MSG(CMHistogramSpecsDlg)
    //virtual BOOL OnInitDialog();
    void OnInitDialog(wxInitDialogEvent& event);
//    afx_msg void OnListHistogram();
//    afx_msg void OnComputeOnly();
//    afx_msg void OnSelendokColumnLineFormat();
//    afx_msg void OnEmptyBins();
//    afx_msg void OnDiskFile();
//    afx_msg void OnTextWindow();
//    afx_msg void OnSelendokMethod();
//    afx_msg void OnSelendokChannels();
    //}}AFX_MSG

    void OnListHistogram(wxCommandEvent& event);
    void OnComputeOnly(wxCommandEvent& event);
    void OnSelendokColumnLineFormat(wxCommandEvent& event);
    void OnEmptyBins(wxCommandEvent& event);
    void OnDiskFile(wxCommandEvent& event);
    void OnTextWindow(wxCommandEvent& event);
    void OnSelendokMethod(wxCommandEvent& event);
    void OnSelendokChannels(wxCommandEvent& event);
//    void OnSelendokChannelComboDropDown(wxCommandEvent& event);
    //void OnButtonPress(wxKeyEvent& event);
    DECLARE_EVENT_TABLE()

    HistogramSpecsPtr m_histogramSpecsPtr;
    FileInfoPtr m_fileInfoPtr;
    WindowInfoPtr m_windowInfoPtr;

    Boolean m_allChannelsAtOnceFlag;
    Boolean m_defaultStatFileChangedFlag;
    Boolean m_initializedFlag;
    Boolean m_lineFormatOnlyFlag;
    Boolean m_updateListHistogramItemsFlag;
    //	SInt16					m_methodOffset;
    SInt16 m_histogramMethodCode;

};

#endif // !defined __LHISTDLG_H__
