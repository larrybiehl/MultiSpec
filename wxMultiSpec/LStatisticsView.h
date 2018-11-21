// LStatisticsView.h : header file
//       
// Revised by Larry Biehl on 06/20/2017                
//
#ifndef __LSTATVEW_H__
	#define __LSTATVEW_H__   

#include "LStatisticsDoc.h"

typedef bool BOOL;
typedef long unsigned int UINT;

class CMStatisticsView : public wxView {
   DECLARE_DYNAMIC_CLASS(CMStatisticsView);
//protected:
//    C
//    // Form Data
public:
    CMStatisticsView(); // protected constructor used by dynamic creation  
    ~CMStatisticsView();
    virtual void OnInitialUpdate(); // first time after construct 

    bool TransferDataToWindow();
    bool TransferDataFromWindow();
    //{{AFX_DATA(CMStatisticsView)
    CMStatisticsFrame *m_frame;
    
    enum {
        IDD = IDD_StatisticsWindow
    };
//    CMListBox m_listBox;
    wxBoxSizer* bSizer233;
    wxBoxSizer* bSizer234;
    wxBoxSizer* bSizer235;
    wxBoxSizer* bSizer236;
    wxBoxSizer* bSizer237;
    wxBoxSizer* bSizer238;
    wxBoxSizer* bSizer239;
    wxBoxSizer* bSizer240;
    wxBoxSizer* bSizer241;
    wxBoxSizer* bSizer242;
    wxBoxSizer* bSizer243;
    wxBoxSizer* bSizer244;
    
    wxListBox* m_listBox;
    int m_classList;
    int m_histogramStatsCode;
    int m_listStatsCode;
    int m_statsTypeCode;
    //new
    BOOL m_polygonFlag;
    //end new
    //}}AFX_DATA

    virtual  void OnDraw(CDC* pDC); // overridden to draw this view 

    CMStatisticsDoc* GetDocument(void);

    void UpdateHistogramStatsCombo(void);

    void UpdateListStatsCombo(void);

    void UpdateStatsTypeCombo(
            SInt16 statsWindowMode);

protected:
   
//    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support  

    // Generated message map functions
    //{{AFX_MSG(CMStatisticsView)
    //void OnDestroy();
    //void OnClose(wxCloseEvent& event);
	bool OnClose(bool deleteWindow = true);
	 void virtual OnActivateView(
						  bool				bActivate,
						  wxView*			pActivateView,
						  wxView*			pDeactiveView);
    void OnAddToList(wxCommandEvent& event);
    void OnSelendokClassList(wxCommandEvent& event);
    void OnProject(wxCommandEvent& event);
    void OnClass(wxCommandEvent& event);
    void OnField(wxCommandEvent& event);
    void OnUpdate(wxCommandEvent& event);
    void OnPolygon(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnList();
    void OnHistogram(wxCommandEvent& event);
    void OnSelchangeListBox(wxCommandEvent& event);
    void OnDblclkListBox(wxCommandEvent& event);
    void OnEditName(wxCommandEvent& event);
    void OnSelendokHistogramStatsCombo(wxCommandEvent& event);
    void OnHistogramStatsComboDropDown(wxCommandEvent& event);
    void OnHistogramStatsComboCloseUp(wxCommandEvent& event);
    void OnSelendokListStatsCombo(wxCommandEvent& event);
    void OnListStatsComboDropDown(wxCommandEvent& event);
    void OnListStatsComboCloseUp(wxCommandEvent& event);
    void OnSelendokStatsCombo(wxCommandEvent& event);
    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnDropdownStatsTypeCombo(wxCommandEvent& event);
    void CreateControls();
    void CreateControls2();
    bool OnCreate(wxDocument *doc, long WXUNUSED(flags));
    
    wxStaticText* m_staticText223;
    wxStaticText* m_staticText224;
    wxStaticText* m_staticText225;
    wxStaticText* m_staticText226;
    wxStaticText* m_staticText227;
    wxStaticText* m_staticText228;
    wxStaticText* m_staticText229;
    wxStaticText* m_staticText230;
    wxStaticText* m_staticText231;
    wxStaticText* m_staticText259;
    wxStaticText* m_staticText260;
    wxStaticText* m_staticText261;
    wxStaticText* m_staticText262;
    wxStaticText* m_staticText263;
    wxStaticText* m_staticText264;
    wxComboBox* m_comboBox40;
    wxComboBox* m_comboBox41;
    wxComboBox* m_comboBox42;
    wxComboBox* m_comboBox43;
    wxCheckBox* m_checkBox53;
    wxButton* m_button56;
    wxButton* m_button60;
    wxButton* m_button61;
    wxButton* m_button62;
    wxButton* m_button63;
    wxButton* m_button64;
    wxButton* m_button65;
    wxListBox* m_listBox4;
    
    wxChoice* m_choice2;
    
    //}}AFX_MSG
    //DECLARE_MESSAGE_MAP()
        Boolean m_initializedFlag,
    m_optionKeyFlag;
private:
    DECLARE_EVENT_TABLE()
//    DECLARE_DYNAMIC_CLASS(CMStatisticsView);


//    CDataExchange *m_dialogFromPtr,
//    *m_dialogToPtr;

};

///////////////////////////////////////////////////////////////////////////// 

#endif	// !defined __LSTATVEW_H__
