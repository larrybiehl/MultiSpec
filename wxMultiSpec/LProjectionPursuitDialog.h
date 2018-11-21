// LProjectionPursuitDialog.h : header file
//
// Revised by Larry Biehl on 06/21/2017
//
#if !defined __LPROJECTIONPURSUITDLG_H__
	#define	__LPROJECTIONPURSUITDLG_H__             
	     
	#include "LDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMProjectionPursuitDialog dialog

class CMProjectionPursuitDialog : public CMDialog
{
public:
	CMProjectionPursuitDialog(wxWindow* pParent = NULL, wxWindowID id = IDD_ProjectionPursuit, 
                const wxString& title = wxT("Set Preprocessing Specification"));   // standard constructor
	virtual ~CMProjectionPursuitDialog();  // standard destructor

	Boolean			DoDialog (
				ProjectionPursuitSpecsPtr	projectionPursuitSpecsPtr); 

// Dialog Data
	//{{AFX_DATA(CMProjectionPursuitDialog)
	enum { IDD = IDD_ProjectionPursuit };
	bool		m_numericalOptimizationFlag;
	double	m_optimizeThreshold;
	double	m_topDownThreshold;
	double	m_bottomUpThreshold;
	int		m_bothOddChoicesNumberFeatures;
	int		m_maximumNumberFeatures;
	short		m_initialNumberFeatures;
	short		m_finalNumberFeatures;
	int		m_algorithmCode;
	int		m_initialGroupingCode;
	int		m_firstStageMethod;
	//}}AFX_DATA

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        bool TransferDataFromWindow();
        bool TransferDataToWindow();
        void CreateControls();
	// Generated message map functions
	//{{AFX_MSG(CMProjectionPursuitDialog)
	void OnInitDialog(wxInitDialogEvent& event);
        void CheckValues(int controlID, SInt16 minValue, SInt16 maxValue, SInt16 flag);
        void OnCheckValues(wxCommandEvent& event);
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
        DECLARE_EVENT_TABLE()
	ProjectionPursuitSpecsPtr		m_projectionPursuitSpecsPtr;
        
        wxBoxSizer* bSizer267;
        wxBoxSizer* bSizer271;
        
        wxRadioButton* m_radioBtn29;
        wxRadioButton* m_radioBtn30;
        wxRadioButton* m_radioBtn31;
        wxRadioButton* m_radioBtn32;
        wxRadioButton* m_radioBtn33;
        wxRadioButton* m_radioBtn34;
        wxRadioButton* m_radioBtn35;
        wxCheckBox* m_checkBox57;
        wxStaticText* m_staticText257;
        wxStaticText* m_staticText258;
        wxStaticText* m_staticText259;
        wxStaticText* m_staticText260;
        wxStaticText* m_staticText261;
        wxStaticText* m_staticText262;
        wxStaticText* m_staticText263;
        wxStaticText* m_staticText264;
        wxTextCtrl* m_textCtrl109;
        wxTextCtrl* m_textCtrl110;
        wxTextCtrl* m_textCtrl111;
        wxTextCtrl* m_textCtrl112;
        wxTextCtrl* m_textCtrl113;
        wxTextCtrl* m_textCtrl114;
        wxButton* m_button68;
        wxButton* m_button67;
        
        
public:
	void OnBnClickedAlgorithm(wxCommandEvent& event);
	void OnBnClickedNumericaloptimization(wxCommandEvent& event);
	void OnBnClickedInitialBandGrouping(wxCommandEvent& event);
	void OnBnClickedBandGroupingMethod(wxCommandEvent& event);
};
#endif // !defined __LPROJECTIONPURSUITDLG_H__
