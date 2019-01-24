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
	
	 /*
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
	 */
    int m_classList;
	 /*
    int m_histogramStatsCode;
    int m_listStatsCode;
    int m_statsTypeCode;
	 */
    BOOL m_polygonFlag;

    virtual  void OnDraw(CDC* pDC); // overridden to draw this view 

    CMStatisticsDoc* GetDocument(void);

protected:
	bool OnClose(bool deleteWindow = true);
	 void virtual OnActivateView(
						  bool				bActivate,
						  wxView*			pActivateView,
						  wxView*			pDeactiveView);
    bool OnCreate(wxDocument *doc, long WXUNUSED(flags));

		Boolean m_initializedFlag;
	
private:
    DECLARE_EVENT_TABLE()
	 
};

///////////////////////////////////////////////////////////////////////////// 

#endif	// !defined __LSTATVEW_H__
