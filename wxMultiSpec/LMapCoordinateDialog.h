// LMapCoordinateDialog.h : header file
//
// Revised by Larry Biehl on 09/18/2017
//
#include "LDialog.h"   
//#include "afxwin.h"
//typedef bool BOOL;
typedef wxString CString;
/////////////////////////////////////////////////////////////////////////////
// CMMapCoordinateDlg dialog

class CMMapCoordinateDlg : public CMDialog {
    // Construction
public:
    CMMapCoordinateDlg(wxWindow* pParent = NULL,  wxWindowID id = wxID_ANY, const wxString& title = wxT("Set Map Coordinate Specifications")); // standard constructor

    // Dialog Data
    //{{AFX_DATA(CMMapCoordinateDlg)

    enum {
        IDD = IDD_MapCoordinates
    };
    int m_ellipsoidSelection;
    int m_projectionSelection;
    int m_mapUnitsSelection;
    int m_gridZone;
    int m_referenceSystemSelection;
    double m_horizontalPixelSize;
    double m_verticalPixelSize;
    double m_xMapCoordinate11;
    double m_yMapCoordinate11;
    double m_falseEasting;
    double m_falseNorthing;
    double m_latitudeOrigin;
    double m_longitudeCentralMeridian;
    double m_semiMinorAxis;
    double m_radiusSpheroid;
    double m_scaleFactorOfCentralMeridian;
    CString m_gridZoneDirection;
    int m_datumSelection;
    double m_mapOrientationAngle;
    double m_standardParallel1;
    double m_standardParallel2;
    //}}AFX_DATA

    Boolean DoDialog(
            FileInfoPtr fileInfoPtr,
            MapProjectionInfoPtr mapProjectionInfoPtr);


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMMapCoordinateDlg)
protected:
    //virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    void CreateControls();
    void OnInitDialog(wxInitDialogEvent& event);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    void AdjustDlgLayout();
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
	//void OnButtonPress(wxKeyEvent& event);
	void OnSelendokMapUnitsCombo(wxCommandEvent& event);
	void OnSelendokEllipsoidCombo(wxCommandEvent& event);
	void OnSelendokReferenceSystemCombo(wxCommandEvent& event);
	void OnSelendokProjectionCombo(wxCommandEvent& event);
	void OnSelendokDatumCombo(wxCommandEvent& event);
	void OnChangeZoneDirection(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
            
	DoublePoint							m_upperLeftLatitudeLongitudePoint;

	UInt8									m_datumName[64],
											m_ellipsoidName[64],
											m_epsgName[64],
											m_gridZoneDirectionString[4];

	double								m_semiMajorAxis;

	FileInfoPtr							m_fileInfoPtr;
	MapProjectionInfoPtr				m_mapProjectionInfoPtr;

	SInt16								m_areaUnitsCode,
											m_datumCode,
											m_ellipsoidCode,
											m_epsgCode,
											m_gridZoneWithDirection,
											m_projectionCode,
											m_referenceSystemCode,
											m_zoneProjectionCode;

	Boolean								m_initializedFlag,
											m_adjustUpperLeftMapPointFlag,
											m_validEPSGCodeFlag;

	wxBoxSizer* bSizer96;
	wxBoxSizer* bSizer97;
	wxBoxSizer* bSizer99;
	wxBoxSizer* bSizer100;
	wxBoxSizer* bSizer101;
	wxBoxSizer* bSizer102;
	wxComboBox* m_comboBox8;
	wxComboBox* m_comboBox9;
	wxComboBox* m_comboBox10;
	wxComboBox* m_comboBox11;
	wxComboBox* m_comboBox12;

	wxTextCtrl* m_textCtrl35;
	wxStaticText* m_staticText107;
	wxStaticText* m_staticText108;
	wxStaticText* m_staticText109;
	wxStaticText* m_staticText110;
	wxStaticText* m_staticText111;
	wxStaticText* m_staticText112;
	wxStaticText* m_staticText113;
	wxStaticText* m_staticText115;
	wxStaticText* m_staticText116;
	wxStaticText* m_staticText119;
	wxStaticText* m_staticText121;
	wxStaticText* m_staticText123;
	wxStaticText* m_staticText125;
	wxStaticText* m_staticText127;
	wxStaticText* m_staticText129;
	wxStaticText* m_staticText301;
	wxStaticText* m_staticText302;
	wxStaticText* m_staticText303;
	wxStaticText* m_staticText304;
	wxStaticText* m_staticText133;
	wxStaticText* m_staticText136;
	wxStaticText* m_staticText139;
	wxStaticText* m_staticText140;
	wxStaticText* m_staticText142;
	wxStaticText* m_staticText143;
	wxStaticText* m_staticText306;
	wxStaticText* m_staticText307;

	wxTextCtrl* m_textCtrl52;
	wxTextCtrl* m_textCtrl53;
	wxTextCtrl* m_textCtrl54;
	wxTextCtrl* m_textCtrl55;
	wxTextCtrl* m_textCtrl56;
	wxTextCtrl* m_textCtrl57;
	wxTextCtrl* m_textCtrl58;
	wxTextCtrl* m_textCtrl59;
	wxTextCtrl* m_textCtrl60;
	wxTextCtrl* m_textCtrl61;
	wxTextCtrl* m_textCtrl64;
	wxTextCtrl* m_textCtrl65;
	wxTextCtrl* m_textCtrl62;
	wxTextCtrl* m_textCtrl63;
	wxTextCtrl* m_textCtrl66;
	wxTextCtrl* m_textCtrl67;
	wxTextCtrl* m_textCtrl181;

	wxCheckBox* m_checkBox7;
	wxButton* m_button12;
	wxButton* m_button13;
    
public:
     void OnEnChangeZone(wxCommandEvent& event);
     void OnEnChangeEPSGCode(wxCommandEvent& event);
     void OnCbnSelchangeDatumcombo(wxCommandEvent& event);
};

