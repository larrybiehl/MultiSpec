// LFileFormatDialog.h : header file
//        
//	Revised by Larry Biehl on 03/12/2019
//
#if !defined __LFORMDLG_H__
#define	__LFORMDLG_H__   

#include "LDialog.h"

typedef bool BOOL;
typedef wxString CString;

#define	IDC_FillDataValueExists			29
#define	IDC_FillDataValue					30

/////////////////////////////////////////////////////////////////////////////
// CMFileFormatSpecsDlg dialog

class CMFileFormatSpecsDlg : public CMDialog {
    // Construction
public:
//    CMFileFormatSpecsDlg();
    CMFileFormatSpecsDlg(wxWindow* pParent = NULL,  wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Multispectral File Format Specifications")); // standard constructor
    ~CMFileFormatSpecsDlg();
    
    Boolean DoDialog(
            Handle fileInfoHandle,
            Handle windowInfoHandle,
            Handle* mapInformationHandlePtr,
            Handle* hfaHandlePtr,
            Handle* newChannelToHdfDataSetHandlePtr,
            Boolean* parameterChangedFlagPtr);

    // Dialog Data
    //{{AFX_DATA(CMFileFormatSpecsDlg)

    enum {
        IDD = IDD_FileFormatSpecs
    };
    UInt32 m_blockHeight;
    UInt32 m_blockWidth;
    UInt32 m_numberLines;
    UInt32 m_numberColumns;
    UInt32 m_postChannelBytes;
    UInt32 m_headerBytes;
    UInt32 m_postLineBytes;
    UInt32 m_preChannelBytes;
    UInt32 m_preLineBytes;
    UInt32 m_numberChannels;
    SInt32 m_startColumnNumber;
    SInt32 m_startLineNumber;
    BOOL m_swapBytesFlag;
    BOOL m_linesBottomToTopFlag;
    BOOL m_fillDataValueExistsFlag;
    double m_fillDataValue;
    UInt32 m_trailerBytes;
    CString m_imageName;
    BOOL m_computeNumClasses;
    int m_dataValueType;
    int m_bandInterleave;
    int m_hdfDataSetSelection;
    int m_collapseClassSelection;
	
    int m_eightBitsPerDataSelection;
    void* m_menuclientdata;
	 
	 wxString	m_stringCheck;

    // Implementation
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	void					CreateControls();
	void					OnInitDialog(wxInitDialogEvent& event);
	bool					TransferDataFromWindow();
	bool					TransferDataToWindow();

	Handle				m_fileInfoHandle,
							m_hfaHandle,
							m_mapInformationHandle,
							m_newChannelToHdfDataSetHandle,
							m_windowInfoHandle;

	UInt32				m_maxNumberChannelsClasses;

	FileInfoPtr			m_fileInfoPtr;
	WindowInfoPtr		m_windowInfoPtr;

	SInt16				m_dataSetIndex,
							m_gdalDataTypeCode;

	UInt16				m_dataCompressionCode;

	Boolean				m_callGetHDFLineFlag,
							m_forceGroupTableUpdateFlag;
					
	wxBoxSizer* bSizer47;
	wxTextCtrl* m_textCtrl35;
	wxTextCtrl* m_textCtrl41;
	wxTextCtrl* m_textCtrl42;
	wxTextCtrl* m_textCtrl43;
	wxTextCtrl* m_textCtrl44;
	wxTextCtrl* m_textCtrl45;
	wxTextCtrl* m_textCtrl46;
	wxTextCtrl* m_textCtrl47;
	wxTextCtrl* m_textCtrl48;
	wxTextCtrl* m_textCtrl49;
	wxTextCtrl* m_textCtrl50;
	wxTextCtrl* m_textCtrl51;

	wxStaticText* m_staticText60;
	wxStaticText* m_staticText80;
	wxStaticText* m_staticText81;
	wxStaticText* m_staticText82;
	wxStaticText* m_staticText83;
	wxStaticText* m_staticText89;
	wxStaticText* m_staticText90;
	wxStaticText* m_staticText91;
	wxStaticText* m_staticText92;
	wxStaticText* m_staticText93;
	wxStaticText* m_staticText94;
	wxStaticText* m_staticText95;
	wxStaticText* m_staticText96;
	wxStaticText* m_staticText97;
	wxStaticText* m_staticText98;
	wxStaticText* m_staticText99;
	wxStaticText* m_staticText100;
	wxStaticText* m_staticText101;
	wxStaticText* m_staticText102;
	wxStaticText* m_staticText103;

	wxComboBox* m_comboBox5;
	wxComboBox* m_comboBox4;
	wxComboBox* m_comboBox6;
	wxComboBox* m_comboBox7;

	wxCheckBox* m_checkBox2;
	wxCheckBox* m_checkBox3;
	wxCheckBox* m_checkBox4;
	wxCheckBox* m_checkBox5;

	wxButton* m_button7;
	wxButton* m_button8;
	wxButton* m_button9;
	wxButton* m_button10;
	
	void OnActivate (wxActivateEvent& event);
	void OnSelendokBandInterleave(wxCommandEvent& event);
	void OnSelendokDataValueType(wxCommandEvent& event);
	void OnDetermineNumClasses(wxCommandEvent& event);
	void OnSelendokHDFDataSet(wxCommandEvent& event);
	void OnPaint(wxPaintEvent& event);
	void ShowMultipleDataSetMessage(void);
	void OnShow(wxShowEvent& event);

	DECLARE_EVENT_TABLE()

	Boolean			m_initializedFlag,
						m_onShowCalledFlag;
public:
    void OnBnClickedHdfdatasethelp(wxCommandEvent& event);
    void OnBnClickedLinebottomtotop(wxCommandEvent& event);
    void OnBnClickedFillDataValueExists(wxCommandEvent& event);
    void OnStnClickedHdfdatasetprompt(wxCommandEvent& event);
};
#endif	// !defined __LFORMDLG_H__
