// LReformatRectifyDialog.h : header file
//
// Revised by Larry Biehl on 06/20/2017
//
#if !defined __LREFORMATRECTIFYDLG_H__
    #define	__LREFORMATRECTIFYDLG_H__ 

#include "LDialog.h"
#include "LOneColumnDialog.h"
#include "SMultiSpec.h"
#include "wx/wx.h"

typedef bool BOOL;

/////////////////////////////////////////////////////////////////////////////
// CMReformatRectifyDlg dialog

class CMReformatRectifyDlg : public CMDialog
{
// ConstructionTransform
public:
	CMReformatRectifyDlg(wxWindow* pParent = NULL, wxWindowID id = wxID_ANY, 
	const wxString& title = wxT("Set Image Rectification Parameters"));   // standard constructor

	~CMReformatRectifyDlg();

	Boolean DoDialog(FileInfoPtr outFileInfoPtr,
						FileInfoPtr fileInfoPtr,
						WindowInfoPtr imageWindowInfoPtr,
						LayerInfoPtr imageLayerInfoPtr,
						ReformatOptionsPtr	reformatOptionsPtr,
						double minBackgroundValue,
						double maxBackgroundValue);

	void OnBnClickedUseMapOrientationAngle(wxCommandEvent& event);
	void OnBnClickedTranslateScaleRotateRadio(wxCommandEvent& event);
	void OnBnClickedReprojectToRadio(wxCommandEvent& event);
	void OnCbnSelendokTargetCombo(wxCommandEvent& event);
	void OnCbnSelendokResampleMethod(wxCommandEvent& event);
	void OnEnChangeRotationClockwise(wxCommandEvent& event);
	void OnSelendokOutChannels(wxCommandEvent& event);

	// Dialog Data
	enum { IDD = IDD_ReformatRectify };
	double  m_backgroundValue;
	long    m_lineShift;
	long    m_columnShift;
	double  m_columnScaleFactor;
	double  m_lineScaleFactor;
	double  m_rotationAngle;
	BOOL    m_blankOutsideSelectedAreaFlag;
	int     m_headerListSelection;
	int     m_channelSelection;
	BOOL    m_useMapOrientationAngleFlag;
	int     m_procedureCode;
	int     m_fileNamesSelection;
	int     m_resampleSelection;

	//wxString m_lineStartString;
	//wxString m_lineEndString;
	//wxString m_columnStartString;
	//wxString m_columnEndString;

	wxString					m_backgroundValueString;
	wxString					m_lineOffsetString;
	wxString					m_columnOffsetString;
	wxString					m_lineScaleString;
	wxString					m_columnScaleString;
	wxString					m_rotationAngleString;

	const wxString title;

	int m_kNoneMenuItem;
	int m_kArcViewMenuItem;
	int m_kERDAS74MenuItem;
	int m_kGAIAMenuItem;
	int m_kTIFFGeoTIFFMenuItem;
	int m_kMatlabMenuItem;
    
// Implementation
protected:
    
	void CreateControls();
	//void OnButtonPress(wxKeyEvent& event);
	void OnInitDialog(wxInitDialogEvent& event);
	bool TransferDataToWindow();
	bool TransferDataFromWindow();

	DECLARE_EVENT_TABLE();

	void UpdateProcedureItems(int   selectItemNumber,
									Boolean	blankOutsideSelectedAreaFlag);
	double							m_mapOrientationAngle;
	FileInfoPtr					m_fileInfoPtr;
	FileInfoPtr					m_outputFileInfoPtr;
	LayerInfoPtr					m_imageLayerInfoPtr;
	RectifyImageOptionsPtr		m_rectifyImageOptionsPtr;
	ReformatOptionsPtr			m_reformatOptionsPtr;
	WindowInfoPtr					m_imageWindowInfoPtr;

	SInt16					m_headerOptionsSelection;
	SInt16					m_resampleMethodCode;
					
	Boolean					m_initializedFlag;
	double					m_maxBackgroundValue,
								m_minBackgroundValue;
	Handle					m_referenceWindowInfoHandle;


		// wxWidget objects
	wxBoxSizer*         bSizer119;
	wxBitmapButton*     m_bpButton4;
	wxBitmapButton*     m_bpButton5;   
	wxStaticText*       m_staticText410;
	wxStaticText*       m_staticText411;    
	wxStaticText*       m_staticText413;
	wxTextCtrl*         m_textCtrl156;
	wxTextCtrl*         m_textCtrl157;
	wxStaticText*       m_staticText414;
	wxTextCtrl*         m_textCtrl158;
	wxTextCtrl*         m_textCtrl159;
	wxTextCtrl*         m_textCtrl160;
	wxTextCtrl*         m_textCtrl161;
	wxStaticText*       m_staticText402;
	wxComboBox*         m_comboBox27;
	wxCheckBox*         m_checkBox20;
	wxStaticText*       m_staticText403;
	wxTextCtrl*         m_textCtrl154;
	wxStaticText*       m_staticText404;
	wxComboBox*         m_comboBox28;
	wxRadioButton*      m_radioBtn18;
	wxStaticText*       m_staticText361;
	wxStaticText*       m_staticText362;
	wxStaticText*       m_staticText363;
	wxStaticText*       m_staticText364;
	wxTextCtrl*         m_textCtrl147;
	wxTextCtrl*         m_textCtrl144;
	wxStaticText*       m_staticText366;
	wxTextCtrl*         m_textCtrl145;
	wxTextCtrl*         m_textCtrl146;
	wxStaticText*       m_staticText378;
	wxStaticText*       m_staticText380;
	wxTextCtrl*         m_textCtrl149;
	wxCheckBox*         m_checkBox18;
	wxRadioButton*      m_radioBtn19;
	wxStaticText*       m_staticText381;
	wxComboBox*         m_comboBox29;
	wxStaticText*       m_staticText382;
	wxComboBox*         m_comboBox30;
	wxStdDialogButtonSizer* m_sdbSizer5;
	wxButton*           m_sdbSizer5OK;
	wxButton*           m_sdbSizer5Cancel;
    
};

#endif // !defined __LREFORMATRECTIFYDLG_H__