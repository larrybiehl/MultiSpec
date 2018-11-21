// LFeatureExtractionDialog.h : header file
//      
#if !defined __LFEXTDLG_H__
	#define	__LFEXTDLG_H__             
	     
	#include "LDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CMFeatureExtractionDialog dialog

class CMFeatureExtractionDialog : public CMDialog
{
// Construction
public:
	CMFeatureExtractionDialog(wxWindow* pParent = NULL, wxWindowID id = IDD_FeatureExtraction, const wxString& title = wxT("Set Feature Extraction Specification"));	// standard constructor     
	
							~CMFeatureExtractionDialog();		// standard desctructor 
	
	Boolean			DoDialog (          
							FeatureExtractionSpecsPtr		featureExtractionSpecsPtr);

// Dialog Data
	//{{AFX_DATA(CMFeatureExtractionDialog)
	enum { IDD = IDD_FeatureExtraction };
	float	m_interclassThrehold;
	float	m_withinClassThreshold;
        float	m_optimizeThreshold;

	bool	m_optimizeClassesFlag;
	int	m_minThresholdNumber;
	int	m_maxPixelsPerClass;
	bool	m_specialOptionsFlag;
	bool	m_listTransformationFlag;
	int		m_interClassWeightsSelection; 
	int		m_weightsSelection;
	int		m_preprocessSelection;
	int		m_algorithmCode;
        int m_preprocess_Saved;	
	
	//}}AFX_DATA

// Implementation
protected:
//	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
        void CreateControls();
	// Generated message map functions
	//{{AFX_MSG(CMFeatureExtractionDialog)
	 void OnInitDialog(wxInitDialogEvent& event);    
	void OnOptimizeClasses(wxCommandEvent& event);
	void OnSpecialOptions(wxCommandEvent& event);
	void OnDecisionBoundary(wxCommandEvent& event);
	void OnSelendokChannelCombo(wxCommandEvent& event);
	void OnSelendokClassWeightsCombo(wxCommandEvent& event);
	void OnSelendokClassPairWeightsCombo(wxCommandEvent& event);
        void OnSelendokChannelComboDN(wxCommandEvent& event);
        void OnCbnSelendokPreprocessingDropDown(wxCommandEvent& event);
	void OnSelendokClassWeightsComboDN(wxCommandEvent& event);
	void OnSelendokClassPairWeightsComboDN(wxCommandEvent& event);
	//}}AFX_MSG

        DECLARE_EVENT_TABLE()
	
	
	FeatureExtractionSpecsPtr		m_featureExtractionSpecsPtr;
	
	float*								m_classWeightsPtr;
	
	SInt16*								m_localClassPairWeightsListPtr;
	
	SInt16								m_localDefaultClassPairWeight;
	
        wxBoxSizer* bSizer250;	
        wxStaticText* m_staticText244;
        wxStaticText* m_staticText245;
        wxStaticText* m_staticText246;
        wxStaticText* m_staticText247;
        wxStaticText* m_staticText248;
        wxStaticText* m_staticText249;
        wxStaticText* m_staticText250;
        wxStaticText* m_staticText251;
        wxStaticText* m_staticText252;
        wxStaticText* m_staticText253;
        wxStaticText* m_staticText254;
        wxStaticText* m_staticText255;
        wxStaticText* m_staticText256;
        wxComboBox* m_comboBox49;
        wxComboBox* m_comboBox50;
        wxComboBox* m_comboBox45;
        wxComboBox* m_comboBox46;
        wxComboBox* m_comboBox47;
        wxComboBox* m_comboBox48;
        wxCheckBox* m_checkBox54;
        wxCheckBox* m_checkBox55;
        wxCheckBox* m_checkBox56;
        wxTextCtrl* m_textCtrl104;
        wxTextCtrl* m_textCtrl105;
        wxTextCtrl* m_textCtrl106;
        wxTextCtrl* m_textCtrl107;
        wxTextCtrl* m_textCtrl108;
        wxButton* m_button65;
        wxButton* m_button66;
               
        wxString m_iclassThresString;
        wxString m_wclassThresString;
        wxString m_percentString;
        wxString m_maxpixeltString;
        wxString m_minThrestString;
        
public:
	void OnCbnSelendokPreprocessingcombo(wxCommandEvent& event);
};                 
  
#endif // !defined __LFEXTDLG_H__
