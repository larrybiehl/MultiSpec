// WFColDlg.h : header file
//      
#if !defined __WFCOLDLG_H__
	#define	__WFCOLDLG_H__       
	     
	#include "LDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFalseColorDlg dialog

class CMFalseColorDlg : public CMDialog
{
// Construction
public:
	CMFalseColorDlg(wxWindow* pParent = NULL, 
                wxWindowID id = IDS_ProcessorBiPlot, 
		const wxString& title = wxT("Assign Channels for False Color Palette"));	// standard constructor  
	
	Boolean					DoDialog(void);

// Dialog Data
	//{{AFX_DATA(CMFalseColorDlg)
	enum { IDD = IDD_FalseColor };
	long	m_redChannel;
	long	m_greenChannel;
	long	m_blueChannel;
	//}}AFX_DATA

// Implementation
protected:
//	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	bool TransferDataFromWindow();
	bool TransferDataToWindow();
	// Generated message map functions
	//{{AFX_MSG(CMFalseColorDlg)
//	virtual BOOL OnInitDialog();
        void OnInitDialog(wxInitDialogEvent& event);
        
//	virtual void OnOK();
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP() 
        DECLARE_EVENT_TABLE()
        void CreateControls();

        LayerInfoPtr m_layerInfoPtr;
        FileInfoPtr m_fileInfoPtr;

        Boolean m_initializedFlag;

        SInt16 m_blueChannelIndex;
        SInt16 m_greenChannelIndex;
        SInt16 m_redChannelIndex;

        SInt32 m_maxChannelFeatureNum;  
	
        wxBoxSizer* bSizer322;
        wxStaticText* m_staticText293;
        wxStaticText* m_staticText294;
        wxStaticText* m_staticText295;
        wxStaticText* m_staticText296;
        wxListBox* m_listBox5;
        wxTextCtrl* m_textCtrl132;
        wxTextCtrl* m_textCtrl133;
        wxTextCtrl* m_textCtrl134;
        wxButton* m_button78;
        wxButton* m_button79;
};      
  
#endif // !defined __WFCOLDLG_H__
