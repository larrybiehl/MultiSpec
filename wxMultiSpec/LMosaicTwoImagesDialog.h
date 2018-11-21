// LMosaicTwoImagesDialog.h : header file
//
// Revised by Larry Biehl on 06/20/2017
//
#if !defined __LMOSAICTWOIMAGESDIALOG_H__
	#define	__LMOSAICTWOIMAGESDIALOG_H__ 

#include "SMultiSpec.h"

#include "LDialog.h"
#include "LOneColumnDialog.h"
#include "wx/wx.h"

typedef bool BOOL;

// CMMosaicTwoImagesDialog dialog

class CMMosaicTwoImagesDialog : public CMDialog
{
public:
    CMMosaicTwoImagesDialog(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, 
    const wxString& title = wxT("Set Mosaic Images Specifications"));   // standard constructor
    
    ~CMMosaicTwoImagesDialog();
    
    Boolean 
    DoDialog(FileInfoPtr     fileInfoPtr,
	     FileInfoPtr	outFileInfoPtr,
             ReformatOptionsPtr reformatOptionsPtr);
    
    // Dialog Data
    enum {IDD = IDD_Mosaic};
    
protected:
    
	void CreateControls();
	//void OnButtonPress(wxKeyEvent& event);
	void OnInitDialog(wxInitDialogEvent& event);
	bool TransferDataToWindow();
	bool TransferDataFromWindow();
    
	void CheckRightImageLines(UInt16  itemSelected);
	void CheckBottomImageColumns(UInt16  itemSelected);
    
    DECLARE_EVENT_TABLE();
    
    FileInfoPtr         m_fileInfoPtr;
    FileInfoPtr         m_outputFileInfoPtr;
    ReformatOptionsPtr	m_reformatOptionsPtr;
    SInt16		m_headerOptionsSelection;
    Boolean		m_initializedFlag;
    Handle              m_rightBottomWindowInfoHandle;

    /*Dialog items*/
    wxBoxSizer* bSizer119;
    wxStaticText* m_staticText248;
    wxComboBox* m_comboBox45;
    wxStaticText* m_staticText249;
    wxStaticText* m_staticText251;
    wxStaticText* m_staticText250;
    wxComboBox* m_comboBox46;
    wxBitmapButton* m_bpButton5;
    wxBitmapButton* m_bpButton4;
    wxStaticText* m_staticText307;
    wxStaticText* m_staticText308;
    wxStaticText* m_staticText309;
    wxTextCtrl* m_textCtrl141;
    wxTextCtrl* m_textCtrl142;
    wxTextCtrl* m_textCtrl143;
    wxStaticText* m_staticText310;
    wxTextCtrl* m_textCtrl144;
    wxTextCtrl* m_textCtrl145;
    wxTextCtrl* m_textCtrl146;
    wxBitmapButton* m_bpButton52;
    wxStaticText* m_staticText311;
    wxStaticText* m_staticText312;
    wxStaticText* m_staticText313;
    wxTextCtrl* m_textCtrl147;
    wxStaticText* m_staticText148;
    wxStaticText* m_staticText314;
    wxTextCtrl* m_textCtrl149;
    wxTextCtrl* m_textCtrl150;
    wxStaticText* m_staticText291;
    wxComboBox* m_comboBox48;
    wxBitmapButton* m_bpButton51;
    wxStaticText* m_staticText303;
    wxStaticText* m_staticText304;
    wxStaticText* m_staticText305;
    wxTextCtrl* m_textCtrl137;
    wxTextCtrl* m_textCtrl138;
    wxStaticText* m_staticText306;
    wxTextCtrl* m_textCtrl139;
    wxStaticText* m_textCtrl140;
    wxCheckBox* m_checkBox21;
    wxTextCtrl* m_textCtrl136;
    wxCheckBox* m_checkBox22;
    wxStaticText* m_staticText290;
    wxComboBox* m_comboBox47;
    wxStdDialogButtonSizer* m_sdbSizer6;
    wxButton* m_sdbSizer6OK;
    wxButton* m_sdbSizer6Cancel;
    

public:
    DialogSelectArea	m_rightBottomDialogSelectArea;
    
    SInt32              m_backgroundValue;
    SInt32              m_maxDataValue;

    int			m_headerListSelection;
    int			m_mosaicDirectionCode;
    int                 m_rightFileNameSelection;
    int                 m_bottomFileNameSelection;

    BOOL		m_ignoreBackgroundValueFlag;
    BOOL		m_writeChannelDescriptionsFlag;
    
    SInt32 m_maxNumberColumns;
    SInt32 m_maxNumberLines;
    
    void OnBnClickedIgnoreBackgroundValue(wxCommandEvent& event);
    void OnCbnSelChangeMosaicDirection(wxCommandEvent& event);
    void OnCbnSelChangeHeaderFormatList(wxCommandEvent& event);
    void OnCbnSelChangeRightImageFileList(wxCommandEvent& event);
    void OnCbnSelChangeBottomImageFileList(wxCommandEvent& event);
    void OnEnChangeLeftTopLineStart(wxCommandEvent& event);
    void OnEnChangeLeftTopLineEnd(wxCommandEvent& event);
    void OnEnChangeLeftTopColumnStart(wxCommandEvent& event);
    void OnEnChangeLeftTopColumnEnd(wxCommandEvent& event);
    void OnEnChangeRightLineStart2(wxCommandEvent& event);
    void OnEnChangeBottomColumnStart3(wxCommandEvent& event);
    void ToEntireImage(wxCommandEvent& event);
    void ToSelectedImage(wxCommandEvent& event);
};
#endif