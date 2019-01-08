// LMapCoordinateDialog.cpp : implementation file
//
// Revised by Larry Biehl on 01/07/2019
//
// Note that m_adjustUpperLeftMapPointFlag has not been fully integrated in this code
// yet. See Mac version for more info. Is not being used right now till a better
// method is made available for the user to control.

#include "SMultiSpec.h"

#include "LMapCoordinateDialog.h"	

#include "wx/valgen.h"
#include "wx/valnum.h"
#include "wx/stream.h"
#include "wx/string.h"
#include "wx/window.h"

extern SInt16 CoordinateDialogCheckIfZoneIsValid (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				UCharPtr								gridZoneDirection);

extern void CoordinateDialogGetMinMaxZone (
				SInt16								referenceSystemCode,
				SInt16*								minZonePtr,
				SInt16*								maxZonePtr,
				UCharPtr								gridZoneDirectionPtr);

extern void CoordinateDialogHideShowProjectionParameters (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				SInt16								projectionCode,
				Boolean								initialFlag,
				SInt16*								datumSelectionPtr,
				SInt16*								ellipsoidSelectionPtr,
				Boolean								setDatumParametersFlag);

extern void CoordinateDialogInitialize (
				DialogPtr							dialogPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16*								mapUnitsSelectionPtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								mapOrientationAnglePtr,
				SInt16*								referenceSystemSelectionPtr,
				SInt16*								epsgCodePtr,
				SInt16*								projectionSelectionPtr,
				SInt16*								datumSelectionPtr,
				SInt16*								ellipsoidSelectionPtr,
				double*								radiusSpheroidPtr,
				double*								semiMajorAxisPtr,
				double*								semiMinorAxisPtr,
				SInt16*								gridZonePtr,
				UCharPtr								gridZoneDirectionPtr,
				SInt16*								gridZoneWithDirectionPtr,
				UCharPtr								epsgNamePtr,
				UCharPtr								datumNamePtr,
				UCharPtr								ellipsoidNamePtr,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								standardParallel1Ptr,
				double*								standardParallel2Ptr,
				DoublePoint*						upperLeftLatLongPointPtr);

extern Boolean CoordinateDialogIsZoneDirectionEditable (
				SInt16								referenceSystemSelection);

extern Boolean CoordinateDialogIsZoneDisplayed (
				SInt16								referenceSystemSelection);

extern void CoordinateDialogOK (
				DialogPtr							dialogPtr,
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SInt16								mapUnitsCode,
				double								xMapCoordinate11,
				double								yMapCoordinate11,
				double								horizontalPixelSize,
				double								verticalPixelSize,
				double								mapOrientationAngle,
				SInt16								referenceSystemCode,
				SInt16								epsgCode,
				SInt16								projectionCode,
				SInt16								datumCode,
				SInt16								ellipsoidCode,
				double								radiusSpheroid,
				double								semiMajorAxis,
				double								semiMinorAxis,
				SInt16								gridZone,
				UCharPtr								gridZoneDirectionPtr,
				UCharPtr								epsgNamePtr,
				UCharPtr								datumNamePtr,
				UCharPtr								ellipsoidNamePtr,
				double								longitudeCentralMeridian,
				double								latitudeOrigin,
				double								falseEasting,
				double								falseNorthing,
				double								scaleFactorOfCentralMeridian,
				double								standardParallel1,
				double								standardParallel2,
				DoublePoint							upperLeftLatLongPoint,
				Boolean								adjustUpperLeftMapPointFlag);

extern SInt16 CoordinateDialogSetDatumParameters (
				DialogPtr							dialogPtr,
				SInt16								datumCode,
				SInt16								ellipsoidCode,
				Boolean								initialFlag);

extern void CoordinateDialogSetEllipsoidParameters (
				DialogPtr							dialogPtr,
				SInt16								ellipsoidCode);

extern Boolean CoordinateDialogSetParametersFromEPSGCode (
				DialogPtr							dialogPtr,
				SInt16								epsgCode,
				Boolean								lastEPSGCodeValidFlag,
				UCharPtr								epsgNamePtr,
				UCharPtr								datumNamePtr,
				UCharPtr								ellipsoidNamePtr,
				SInt16*								mapUnitsSelectionPtr,
				SInt16*								projectionSelectionPtr);

extern void CoordinateDialogSetReferenceSystemParameters (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				UCharPtr								gridZoneDirectionPtr,
				Boolean								initialFlag,
				SInt16*								projectionSelectionPtr,
				SInt16*								datumSelectionPtr,
				SInt16*								ellipsoidSelectionPtr,
				SInt16*								mapUnitsSelectionPtr);

extern void CoordinateDialogSetParametersFromRS (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				SInt16*								projectionSelectionPtr,
				UCharPtr								gridZoneDirectionPtr,
				SInt16								gridZoneWithDirection,
				SInt16								datumCode,
				SInt16								ellipsoidCode,
				SInt16*								gridZonePtr);

/////////////////////////////////////////////////////////////////////////////
// CMMapCoordinateDlg dialog

CMMapCoordinateDlg::CMMapCoordinateDlg(wxWindow* pParent, wxWindowID id, const wxString& title /*=NULL*/)
: CMDialog(CMMapCoordinateDlg::IDD, pParent, title) 
{
	m_referenceSystemSelection = 0;
	m_ellipsoidSelection = 0;
	m_projectionSelection = 0;
	m_mapUnitsSelection = 0;
	m_horizontalPixelSize = 0.0;
	m_verticalPixelSize = 0.0;
	m_xMapCoordinate11 = 0.0;
	m_yMapCoordinate11 = 0.0;
	m_falseEasting = 0.0;
	m_falseNorthing = 0.0;
	m_latitudeOrigin = 0.0;
	m_longitudeCentralMeridian = 0.0;
	m_semiMinorAxis = 0.0;
	m_radiusSpheroid = 0.0;
	m_scaleFactorOfCentralMeridian = 0.0;
	m_gridZoneDirection = _T(" ");
	m_datumSelection = -1;
	m_mapOrientationAngle = 0.0;
	m_standardParallel1 = 0.0;
	m_standardParallel2 = 0.0;

	m_areaUnitsCode = 0;
	m_epsgCode = 0;
	m_datumCode = 0;
	m_referenceSystemCode = 0;
	m_projectionCode = 0;
	m_semiMajorAxis = 0;
	m_ellipsoidCode = 0;
	m_gridZoneDirectionString[0] = 'N';
	// m_gridZoneDirectionString[0] = 0;
	m_gridZoneDirectionString[1] = 0;
	m_epsgName[0] = 0;
	m_datumName[0] = 0;
	m_ellipsoidName[0] = 0;
	m_validEPSGCodeFlag = TRUE;

	m_initializedFlag = CMDialog::m_initializedFlag;
	CreateControls();

}	// end "CMMapCoordinateDlg"


void CMMapCoordinateDlg::CreateControls() 
{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	//wxBoxSizer* bSizer96;
	bSizer96 = new wxBoxSizer (wxVERTICAL);
	
//	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText107 = new wxStaticText( this, wxID_ANY, wxT("Planar Coordinate Information"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText107->Wrap( -1 );   
	//bSizer97->Add (m_staticText107, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer97->Add (m_staticText107, wxSizerFlags(0).Center().Border(wxBOTTOM,5));
	
	
	//bSizer97->Add (20, 0, 0, wxEXPAND, 5 );
	bSizer97->Add (20, 0, 0, wxEXPAND);
	
	m_staticText108 = new wxStaticText( this, IDC_UnitsPrompt, wxT("Units:"), wxDefaultPosition, //wxDefaultSize, 0|wxVSCROLL );
      wxDefaultSize, 0);
	m_staticText108->Wrap( -1 );
   SetUpToolTip(m_staticText108, IDS_ToolTip290);
	//bSizer97->Add( m_staticText108, 0, wxALIGN_CENTER|wxALL, 5 );
	bSizer97->Add( m_staticText108, wxSizerFlags(0).Center().Border(wxRIGHT|wxBOTTOM,5));
	
	m_comboBox8 = new wxComboBox( this, IDC_MapUnitsCombo, wxT("Meters"), wxDefaultPosition, //wxDefaultSize, 0, NULL, wxCB_READONLY|wxVSCROLL );
   wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_comboBox8->Append( wxT("Unknown Units") );
	m_comboBox8->Append( wxT("Decimal Degrees") );
	m_comboBox8->Append( wxT("Kilometers") );
	m_comboBox8->Append( wxT("Meters") );
	m_comboBox8->Append( wxT("Centimeters") );
	m_comboBox8->Append( wxT("Milimeters") );
	m_comboBox8->Append( wxT("Micrometers") );
	m_comboBox8->Append( wxT("Miles") );
	m_comboBox8->Append( wxT("Yards") );
	m_comboBox8->Append( wxT("US Survey Feet") );
	m_comboBox8->Append( wxT("Feet") );
	m_comboBox8->Append( wxT("Inches") );
   SetUpToolTip(m_comboBox8, IDS_ToolTip290);
	//bSizer97->Add (m_comboBox8, 0, wxALL, 5 );
	bSizer97->Add (m_comboBox8, wxSizerFlags(0).Border(wxBOTTOM,5));
	
	m_staticText307 = new wxStaticText( this, IDC_MapUnitsName, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText307->Wrap( -1 );
	//bSizer97->Add (m_staticText307, 0, wxALIGN_CENTER|wxALL, 5 );
	bSizer97->Add (m_staticText307, wxSizerFlags(0).Center().Border(wxBOTTOM,5));
        
	//bSizer96->Add (bSizer97, 0, wxALL|wxEXPAND, 5 );
	bSizer96->Add (bSizer97, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT,12));
        
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFloatingPointValidator<double> doubleValue4Digits(4, &m_doubleValueCheck);
	wxFloatingPointValidator<double> doubleValue8Digits(&m_doubleValueCheck, wxNUM_VAL_NO_TRAILING_ZEROES );
	
	m_staticText109 = new wxStaticText( this, IDC_X11CoordinatePrompt, wxT("X map coordinate for center of upper-left pixel (1,1): . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText109->Wrap( -1 );
   SetUpToolTip(m_staticText109, IDS_ToolTip291);
	//fgSizer4->Add (m_staticText109, 0, wxALIGN_CENTER, 5 );
	fgSizer4->Add (m_staticText109, wxSizerFlags(0).Left().Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT,12));
	
	m_textCtrl52 = new wxTextCtrl( this, IDC_X11Coordinate, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0);
   //m_textCtrl52->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
   m_textCtrl52->SetValidator(doubleValue8Digits);
	m_textCtrl52->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl52, IDS_ToolTip291);
	//fgSizer4->Add (m_textCtrl52, 0, 0, 5);
	fgSizer4->Add (m_textCtrl52, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));	
	/*
	int width, height;
	m_textCtrl52->GetSize (&width, &height);
	int numberChars = sprintf ((char*)&gTextString3,
												" LMapCDlg::CreateControls (default width, height): %d, %d%s",
												width, 
												height,
												gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH); 
	*/	
	m_staticText110 = new wxStaticText( this, IDC_Y11CoordinatePrompt, wxT("Y map coordinate for center of upper-left pixel (1,1): . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText110->Wrap( -1 );
   SetUpToolTip(m_staticText110, IDS_ToolTip292);
	//fgSizer4->Add (m_staticText110, 0, wxALIGN_CENTER, 5 );
	fgSizer4->Add (m_staticText110, wxSizerFlags(0).Left().Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT,12));
	
	m_textCtrl53 = new wxTextCtrl( this, IDC_Y11Coordinate, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   //m_textCtrl53->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
   m_textCtrl53->SetValidator(doubleValue8Digits);
	m_textCtrl53->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl53, IDS_ToolTip292);
	//fgSizer4->Add( m_textCtrl53, 0, 0, 5 );
	fgSizer4->Add (m_textCtrl53, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText111 = new wxStaticText( this, IDC_HorizontalSizePrompt, wxT("Horizontal pixel size: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
   SetUpToolTip(m_staticText111, IDS_ToolTip293);
	//fgSizer4->Add ( m_staticText111, 0, wxALIGN_CENTER, 5);
	fgSizer4->Add (m_staticText111, wxSizerFlags(0).Left().Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT,12));
	
	m_textCtrl54 = new wxTextCtrl( this, IDC_HorizontalSize, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   //m_textCtrl54->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
   m_textCtrl54->SetValidator(doubleValue8Digits);
	m_textCtrl54->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl54, IDS_ToolTip293);
	//fgSizer4->Add( m_textCtrl54, 0, 0, 5 );
	fgSizer4->Add (m_textCtrl54, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText112 = new wxStaticText( this, IDC_VerticalSizePrompt, wxT("Vertical pixel size: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText112->Wrap( -1 );
   SetUpToolTip(m_staticText112, IDS_ToolTip294);
	//fgSizer4->Add (m_staticText112, 0, wxALIGN_CENTER, 5);
	fgSizer4->Add (m_staticText112, wxSizerFlags(0).Left().Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT,12));
	
	m_textCtrl55 = new wxTextCtrl( this, IDC_VerticalSize, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   //m_textCtrl55->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
   m_textCtrl55->SetValidator(doubleValue8Digits);
	m_textCtrl55->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl55, IDS_ToolTip294);
	//fgSizer4->Add( m_textCtrl55, 0, 0, 5 );
	fgSizer4->Add (m_textCtrl55, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText113 = new wxStaticText( this, wxID_ANY, wxT("Map orientation angle: . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText113->Wrap( -1 );
   SetUpToolTip(m_staticText113, IDS_ToolTip295);
	//fgSizer4->Add (m_staticText113, 0, wxALIGN_CENTER, 5);
	fgSizer4->Add (m_staticText113, wxSizerFlags(0).Left().Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT,12));
	
	m_textCtrl56 = new wxTextCtrl( this, IDC_OrientationAngle, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
	wxFloatingPointValidator<double> doubleValueAngle(6, &m_doubleValueCheck);
	doubleValueAngle.SetRange(-180, 180);
	m_textCtrl56->SetValidator(doubleValueAngle);
	m_textCtrl56->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl56, IDS_ToolTip295);
	//m_textCtrl56->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer4->Add( m_textCtrl56, 0, 0, 5 );
	fgSizer4->Add (m_textCtrl56, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
        
	//bSizer96->Add (fgSizer4, 0, wxEXPAND|wxLEFT|wxRIGHT, 15 );
	bSizer96->Add (fgSizer4, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));
	
	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText115 = new wxStaticText( this, IDC_ReferenceSystemPrompt, wxT("Grid Coordinate System:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText115->Wrap( -1 );
   SetUpToolTip(m_staticText115, IDS_ToolTip296);
	//bSizer98->Add (m_staticText115, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer98->Add (m_staticText115, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));
	
	m_comboBox9 = new wxComboBox (this, IDC_ReferenceSystemCombo, wxT("User Defined"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox9->Append( wxT("none defined") );
	m_comboBox9->Append( wxT("User Defined") );
	m_comboBox9->Append( wxT("Geographic (lat-long)") );
	m_comboBox9->Append( wxT("Gauss Kruger") );
	m_comboBox9->Append( wxT("GDA94") );
	m_comboBox9->Append( wxT("Greek Geographic Reference System 1987") );
	m_comboBox9->Append( wxT("State Plane - NAD27") );
	m_comboBox9->Append( wxT("State Plane - NDA83") );
	m_comboBox9->Append( wxT("UTM - NAD27") );
	m_comboBox9->Append( wxT("UTM - NAD83") );
	m_comboBox9->Append( wxT("UTM - SAD69") );
	m_comboBox9->Append( wxT("UTM - WGS72") );
	m_comboBox9->Append( wxT("UTM - WGS84") );
	m_comboBox9->Append( wxT("UTM") );
	m_comboBox9->Append( wxT("Defined by EPSG Code:") );
	m_comboBox9->SetSelection( 1 );
   SetUpToolTip(m_comboBox9, IDS_ToolTip296);
	//bSizer98->Add (m_comboBox9, 0, wxALL, 5 );
	bSizer98->Add (m_comboBox9, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	
	//bSizer96->Add (bSizer98, 0, wxALL|wxEXPAND, 5);
	bSizer96->Add (bSizer98, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));
	
//	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText116 = new wxStaticText( this, IDC_ZonePrompt, wxT("FIPS Zone:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText116->Wrap( -1 );
   SetUpToolTip(m_staticText116, IDS_ToolTip297);
	//bSizer99->Add (m_staticText116, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer99->Add (m_staticText116, 0, wxALIGN_CENTER|wxTOP|wxRIGHT|wxBOTTOM, 5);
	
	m_textCtrl181 = new wxTextCtrl( this, IDC_EPSGCode, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl181->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
	m_textCtrl181->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl181, IDS_ToolTip300);
	bSizer99->Add (m_textCtrl181, 0, wxALL, 5);
	
	m_textCtrl57 = new wxTextCtrl( this, IDC_Zone, wxEmptyString, wxDefaultPosition, wxSize( 40,-1 ), 0 );
   m_textCtrl57->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_stringCheck));
	m_textCtrl57->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip(m_textCtrl57, IDS_ToolTip297);
	bSizer99->Add (m_textCtrl57, 0, wxALL, 5);
	
	m_textCtrl67 = new wxTextCtrl( this, IDC_ZoneDirection, wxEmptyString, wxDefaultPosition, wxSize( 25,-1 ), 0 );
        m_textCtrl67->SetMaxLength(1);
        wxTextValidator valid_char = wxTextValidator(wxFILTER_ASCII|wxFILTER_INCLUDE_CHAR_LIST);
//        wxArrayString valid_str = wxArrayString(2,"NS");
//        valid_char.SetIncludes(valid_str);
        wxString valid_str = "'N','S','n','s'";
        valid_char.SetCharIncludes(valid_str);        
        m_textCtrl67->SetValidator(valid_char);
	bSizer99->Add (m_textCtrl67, 0, wxALL, 5);
	
        
	m_staticText306 = new wxStaticText( this, IDC_FIPSZoneName, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText306->Wrap( -1 );
	bSizer99->Add (m_staticText306, 0, wxALIGN_CENTER|wxALL, 5);
	
	
	//bSizer96->Add (bSizer99, 0, wxEXPAND|wxLEFT, 15);
	bSizer96->Add (bSizer99, wxSizerFlags(0).Expand().Border(wxLEFT,24));
	
//	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText119 = new wxStaticText( this, IDC_ProjectionPrompt, wxT("Projection:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText119->Wrap( -1 );
	//bSizer100->Add (m_staticText119, 0, wxALIGN_CENTER|wxALL|wxLEFT, 5);
	bSizer100->Add (m_staticText119, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));
	
	m_comboBox12 = new wxComboBox( this, IDC_ProjectionCombo, wxT("Transverse Mercator"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBox12->Append( wxT("None defined") );
	m_comboBox12->Append( wxT("Alaska Conformal") );
	m_comboBox12->Append( wxT("Albers Conical Equal Area") );
	m_comboBox12->Append( wxT("Azimuthal Equidistant") );
	m_comboBox12->Append( wxT("Cylindrical Equal Area") );
	m_comboBox12->Append( wxT("Equidistant Conic") );
	m_comboBox12->Append( wxT("Equirectangular") );
	m_comboBox12->Append( wxT("General Vertical Near-Side Perspective") );
	m_comboBox12->Append( wxT("Gnomonic") );
	m_comboBox12->Append( wxT("Hammer") );
	m_comboBox12->Append( wxT("Integerized Sinusoidal") );
	m_comboBox12->Append( wxT("Interrupted Goode Homolosine") );
	m_comboBox12->Append( wxT("Interrupted Mollweide") );
	m_comboBox12->Append( wxT("Krovak") );
	m_comboBox12->Append( wxT("Lambert Azimuthal Equal-area") );
	m_comboBox12->Append( wxT("Lambert Conformal Conic") );
	m_comboBox12->Append( wxT("Mercator") );
	m_comboBox12->Append( wxT("Miller Cylindrical") );
	m_comboBox12->Append( wxT("Modified Transverse Mercator") );
	m_comboBox12->Append( wxT("Mollweide") );
	m_comboBox12->Append( wxT("New Zealand Map Grid") );
	m_comboBox12->Append( wxT("Oblated Equal Area") );
	m_comboBox12->Append( wxT("Oblique Mercator") );
	m_comboBox12->Append( wxT("Orthographic") );
	m_comboBox12->Append( wxT("Polar Stereographic") );
	m_comboBox12->Append( wxT("Polyconic") );
	m_comboBox12->Append( wxT("Robinson") );
	m_comboBox12->Append( wxT("Sinusoidal") );
	m_comboBox12->Append( wxT("Space Oblique Mercator") );
	m_comboBox12->Append( wxT("Stereographic") );
	m_comboBox12->Append( wxT("Transverse Mercator") );
	m_comboBox12->Append( wxT("Transverse Mercator South Oriented") );
	m_comboBox12->Append( wxT("Van der Grinten I") );
	m_comboBox12->Append( wxT("Wagner IV") );
	m_comboBox12->Append( wxT("Wagner VII") );
	//m_comboBox12->Append( wxT("Gnomonic") );
	//bSizer100->Add (m_comboBox12, 0, wxALL, 5);
	bSizer100->Add (m_comboBox12, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	m_staticText301 = new wxStaticText( this, IDC_ProjectionName, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText301->Wrap( -1 );
	//bSizer100->Add (m_staticText301, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer100->Add (m_staticText301, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	
	//bSizer96->Add (bSizer100, 0, wxEXPAND|wxLEFT, 15);
	bSizer96->Add (bSizer100, wxSizerFlags(0).Expand().Border(wxLEFT,24));
	

	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 7, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFloatingPointValidator<double> doubleValue2Digits(2, &m_doubleValueCheck);
	wxFloatingPointValidator<double> doubleValue6Digits(6, &m_doubleValueCheck);
	wxFloatingPointValidator<double> doubleValue10Digits(10, &m_doubleValueCheck);
	
	m_staticText121 = new wxStaticText( this, IDC_LongitudePrompt, wxT("Longitude of central meridian (decimal degrees): . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText121->Wrap( -1 );
	//fgSizer5->Add (m_staticText121, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText121, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl58 = new wxTextCtrl( this, IDC_Longitude, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl58->SetValidator(doubleValue6Digits);
	m_textCtrl58->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl58->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl58, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl58, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText123 = new wxStaticText( this, IDC_LatitudePrompt, wxT("Latitude of origin (decimal degrees): . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText123->Wrap( -1 );
	//fgSizer5->Add (m_staticText123, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText123, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl59 = new wxTextCtrl( this, IDC_Latitude, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl59->SetValidator(doubleValue6Digits);
	m_textCtrl59->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl59->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl59, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl59, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText125 = new wxStaticText( this, IDC_FalseEastingPrompt, wxT("False Easting: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText125->Wrap( -1 );
	//fgSizer5->Add (m_staticText125, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText125, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl60 = new wxTextCtrl( this, IDC_FalseEasting, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl60->SetValidator(doubleValue6Digits);
	m_textCtrl60->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl60->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl60, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl60, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText127 = new wxStaticText( this, IDC_FalseNorthingPrompt, wxT("False Northing: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText127->Wrap( -1 );
	//fgSizer5->Add (m_staticText127, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText127, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl61 = new wxTextCtrl( this, IDC_FalseNorthing, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl61->SetValidator(doubleValue6Digits);
	m_textCtrl61->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl61->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl61, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl61, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText129 = new wxStaticText( this, IDC_ScaleFactorPrompt, wxT("Scale factor of central meridian: . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText129->Wrap( -1 );
	//fgSizer5->Add (m_staticText129, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText129, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl62 = new wxTextCtrl( this, IDC_ScaleFactor, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl62->SetValidator(doubleValue10Digits);
	m_textCtrl62->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl62->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl62, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl62, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText142 = new wxStaticText( this, IDC_StandardParallel1Prompt, wxT("Standard parallel 1:  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText142->Wrap( -1 );
	//fgSizer5->Add (m_staticText142, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText142, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl65 = new wxTextCtrl( this, IDC_StandardParallel1, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl65->SetValidator(doubleValue6Digits);
	m_textCtrl65->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl65->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl65, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl65, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText143 = new wxStaticText( this, IDC_StandardParallel2Prompt, wxT("Standard parallel 2:  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText143->Wrap( -1 );
	//fgSizer5->Add (m_staticText143, 0, wxALIGN_CENTER, 5);
	fgSizer5->Add (m_staticText143, wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl66 = new wxTextCtrl( this, IDC_StandardParallel2, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl66->SetValidator(doubleValue6Digits);
	m_textCtrl66->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl66->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//fgSizer5->Add( m_textCtrl66, 0, 0, 5 );
	fgSizer5->Add (m_textCtrl66, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
        
	//bSizer96->Add (fgSizer5, 0, wxEXPAND|wxLEFT, 25);
	bSizer96->Add (fgSizer5, wxSizerFlags(0).Expand().Border(wxLEFT,36));
	
//	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText133 = new wxStaticText( this, IDC_DatumPrompt, wxT("Geodetic Model       Datum:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText133->Wrap( -1 );
   SetUpToolTip(m_staticText133, IDS_ToolTip298);
	//bSizer101->Add (m_staticText133, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer101->Add (m_staticText133, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));
	
	m_comboBox10 = new wxComboBox( this, IDC_DatumCombo, wxT("NAD 27"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN );
	m_comboBox10->Append( wxT("None defined") );
	m_comboBox10->Append( wxT("Beijing 1954") );
	m_comboBox10->Append( wxT("Campo Inchauspe") );
	m_comboBox10->Append( wxT("Geocentric Datum of Australia 1994") );
	m_comboBox10->Append( wxT("Greek Geodetic Reference System 1987") );
	m_comboBox10->Append( wxT("NAD 27") );
	m_comboBox10->Append( wxT("NAD 83") );
	m_comboBox10->Append( wxT("NAD 83 (CSRS)") );
	m_comboBox10->Append( wxT("New Zealand Geodetic Datum 1949") );
	m_comboBox10->Append( wxT("NGO 1948 (Oslow)") );
	m_comboBox10->Append( wxT("Ord. Survey G. Britain 1936") );
	m_comboBox10->Append( wxT("Pulkovo 1942") );
	m_comboBox10->Append( wxT("Pulkovo 1995") );
	m_comboBox10->Append( wxT("South American 1969") );
	m_comboBox10->Append( wxT("WGS 72") );
	m_comboBox10->Append( wxT("WGS 84") );
	m_comboBox10->Append( wxT("Sphere") );
	m_comboBox10->SetSelection( 5 );
   SetUpToolTip(m_comboBox10, IDS_ToolTip298);
	//bSizer101->Add (m_comboBox10, 0, wxALL, 5 );
	bSizer101->Add (m_comboBox10, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	m_staticText302 = new wxStaticText( this, IDC_DatumName, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText302->Wrap( -1 );
   SetUpToolTip(m_staticText302, IDS_ToolTip298);
	//bSizer101->Add (m_staticText302, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer101->Add (m_staticText302, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	
	//bSizer96->Add (bSizer101, 0, wxALL|wxEXPAND, 5);
	bSizer96->Add (bSizer101, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,12));
	
//	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText136 = new wxStaticText( this, IDC_ProjectionEllipsoidPrompt, wxT("Projection Ellipsoid:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText136->Wrap( -1 );
   SetUpToolTip(m_staticText136, IDS_ToolTip299);
	//bSizer102->Add (m_staticText136, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer102->Add (m_staticText136, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));
	
	m_comboBox11 = new wxComboBox( this, IDC_EllipsoidCombo, wxT("None defined"), wxDefaultPosition, //wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxVSCROLL );
      wxDefaultSize, 0, NULL, wxCB_DROPDOWN );
	m_comboBox11->Append( wxT("None defined") );
	m_comboBox11->Append( wxT("User Defined") );
	m_comboBox11->Append( wxT("Airy") );
	m_comboBox11->Append( wxT("Australian") );
	m_comboBox11->Append( wxT("Bessel 1841") );
	m_comboBox11->Append( wxT("Bessel Modified") );
	m_comboBox11->Append( wxT("Clarke 1866") );
	m_comboBox11->Append( wxT("Clarke 1880") );
	m_comboBox11->Append( wxT("Everest") );
	m_comboBox11->Append( wxT("GRS 1967 Modified") );
	m_comboBox11->Append( wxT("GRS 80") );
	m_comboBox11->Append( wxT("International 1909") );
	m_comboBox11->Append( wxT("Krassovsky") );
	m_comboBox11->Append( wxT("Sphere") );
	m_comboBox11->Append( wxT("WGS 72") );
	m_comboBox11->Append( wxT("WGS 84") );
   SetUpToolTip(m_comboBox11, IDS_ToolTip299);
	//bSizer102->Add (m_comboBox11, 0, wxALL, 5);
	bSizer102->Add (m_comboBox11, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	m_staticText303 = new wxStaticText( this, IDC_EllipsoidName, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText303->Wrap( -1 );
   SetUpToolTip(m_staticText303, IDS_ToolTip299);
	//bSizer102->Add (m_staticText303, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer102->Add (m_staticText303, wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	
	//bSizer96->Add (bSizer102, 0, wxEXPAND|wxLEFT, 15);
	bSizer96->Add (bSizer102, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT,24));
	
	wxBoxSizer* bSizer103;
	bSizer103 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer106;
	bSizer106 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText139 = new wxStaticText( this, IDC_RadiusPrompt, wxT("Radius of sphere (earth): "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText139->Wrap( -1 );
	//bSizer106->Add (m_staticText139, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer106->Add (m_staticText139, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,2));
	
	m_staticText304 = new wxStaticText( this, IDC_MajorAxisPrompt, wxT("Major axis: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText304->Wrap( -1 );
	//bSizer106->Add (m_staticText304, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer106->Add (m_staticText304, wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,2));
	
	m_textCtrl63 = new wxTextCtrl( this, IDC_Radius, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl63->SetValidator(doubleValue4Digits);
	m_textCtrl63->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl63->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//bSizer106->Add (m_textCtrl63, 0, wxALL, 5);
	bSizer106->Add (m_textCtrl63, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM,2));
	
	
	//bSizer104->Add( bSizer106, 0, wxALIGN_RIGHT|wxLEFT, 36);
	bSizer104->Add( bSizer106, 0, wxALIGN_CENTER|wxLEFT, 36);
	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText140 = new wxStaticText( this, IDC_MinorAxisPrompt, wxT("Minor axis: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText140->Wrap( -1 );
	//bSizer107->Add (m_staticText140, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer107->Add (m_staticText140, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxALIGN_CENTER|wxTOP|wxRIGHT, 2 );
	
	m_textCtrl64 = new wxTextCtrl( this, IDC_MinorAxis, wxEmptyString, wxDefaultPosition, wxSize(120,-1), 0 );
   m_textCtrl64->SetValidator(doubleValue4Digits);
	m_textCtrl64->SetWindowStyle (wxTE_RIGHT);
   //m_textCtrl64->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_stringCheck));
	//bSizer107->Add (m_textCtrl64, 0, wxALL, 5);
	bSizer107->Add (m_textCtrl64, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN|wxTOP|wxRIGHT, 2);
	
	
	//bSizer104->Add (bSizer107, 0, wxALIGN_RIGHT|wxLEFT, 25);
	bSizer104->Add (bSizer107, wxSizerFlags(0).Right().Border(wxLEFT,24));
	
	
	//bSizer103->Add( bSizer104, 1, wxALIGN_BOTTOM|wxALIGN_RIGHT, 5 );
	bSizer103->Add( bSizer104, 1, wxALIGN_BOTTOM, 5 );
	
	bSizer103->Add( 40, 0, 0, wxEXPAND, 5 );
	/*
	wxBoxSizer* bSizer105;
	bSizer105 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button12 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer105->Add (m_button12, 0, wxALL, 5 );
	bSizer105->Add (m_button12, wxSizerFlags(0).Border(wxRIGHT,6));
	
	m_button13 = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	//bSizer105->Add (m_button13, 0, wxALL, 5 );
	bSizer105->Add (m_button13, wxSizerFlags(0));
	
	//bSizer103->Add (bSizer105, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT, 5);
	//bSizer103->Add (bSizer105, wxSizerFlags(0).Right().Bottom());
   bSizer103->Add (bSizer105, 0, wxALIGN_BOTTOM, 5);
	*/
	//bSizer96->Add (bSizer103, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT, 5 );
	//bSizer96->Add (bSizer103, wxSizerFlags(0).Expand().Right().Border(wxRIGHT|wxBOTTOM,12));
	bSizer96->Add (bSizer103, wxSizerFlags(0).Expand().Border(wxRIGHT|wxBOTTOM,12));
	
	CreateStandardButtons (bSizer96);
	
	this->SetSizerAndFit (bSizer96);
//	this->Layout();
	//this->Centre();
	
}		// end "CreateControls"



BEGIN_EVENT_TABLE(CMMapCoordinateDlg, CMDialog)
EVT_INIT_DIALOG(CMMapCoordinateDlg::OnInitDialog)
EVT_COMBOBOX(IDC_MapUnitsCombo, CMMapCoordinateDlg::OnSelendokMapUnitsCombo)
EVT_COMBOBOX(IDC_ReferenceSystemCombo, CMMapCoordinateDlg::OnSelendokReferenceSystemCombo)
EVT_COMBOBOX(IDC_ProjectionCombo, CMMapCoordinateDlg::OnSelendokProjectionCombo)
EVT_COMBOBOX(IDC_DatumCombo, CMMapCoordinateDlg::OnSelendokDatumCombo)
EVT_COMBOBOX(IDC_EllipsoidCombo, CMMapCoordinateDlg::OnSelendokEllipsoidCombo)
EVT_TEXT(IDC_ZoneDirection, CMMapCoordinateDlg::OnChangeZoneDirection)
EVT_TEXT(IDC_Zone, CMMapCoordinateDlg::OnEnChangeZone)
EVT_TEXT(IDC_EPSGCode, CMMapCoordinateDlg::OnEnChangeEPSGCode)
//EVT_CHAR_HOOK(CMMapCoordinateDlg::OnButtonPress)
END_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////////////
// CMMapCoordinateDlg message handlers



//-----------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2000
//	Revised By:			Larry L. Biehl			Date: 04/05/2004	

Boolean CMMapCoordinateDlg::DoDialog (
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr) 
{
	Boolean								OKFlag = FALSE;

	SInt16								returnCode;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																				return (FALSE);

	m_fileInfoPtr = fileInfoPtr;
	m_mapProjectionInfoPtr = mapProjectionInfoPtr;

	returnCode = ShowModal();

	if (returnCode == wxID_OK) 
		{
		OKFlag = TRUE;

		m_gridZoneDirectionString[0] = 1;

		if (m_gridZoneDirection.Length() > 0)
			m_gridZoneDirectionString[1] = m_gridZoneDirection[0];
		else //
			m_gridZoneDirectionString[1] = 0;

		m_gridZoneDirectionString[2] = 0;

		CoordinateDialogOK (this,
									 m_fileInfoPtr,
									 m_mapProjectionInfoPtr,
									 abs(m_mapUnitsSelection),
									 m_xMapCoordinate11,
									 m_yMapCoordinate11,
									 m_horizontalPixelSize,
									 m_verticalPixelSize,
									 m_mapOrientationAngle,
									 m_referenceSystemSelection,
									 m_epsgCode,
									 m_projectionCode,
									 m_datumCode,
									 m_ellipsoidCode,
									 m_radiusSpheroid,
									 m_semiMajorAxis,
									 m_semiMinorAxis,
									 m_gridZone,
									 m_gridZoneDirectionString,
									 m_epsgName,
									 m_datumName,
									 m_ellipsoidName,
									 m_longitudeCentralMeridian,
									 m_latitudeOrigin,
									 m_falseEasting,
									 m_falseNorthing,
									 m_scaleFactorOfCentralMeridian,
									 m_standardParallel1,
									 m_standardParallel2,
									 m_upperLeftLatitudeLongitudePoint,
									 m_adjustUpperLeftMapPointFlag);

		}	// end "if (returnCode == IDOK)"

	return (OKFlag);

} // end "DoDialog"

/*
//Press keyboard "Enter"  -> wxID_OK
void CMMapCoordinateDlg::OnButtonPress(wxKeyEvent& event)
{
	if (event.GetKeyCode() == WXK_RETURN) {
		if(Validate() && TransferDataFromWindow())
			this->EndModal(wxID_OK);
	} else {
		event.Skip();
	}
}
*/

void CMMapCoordinateDlg::AdjustDlgLayout()
{
    bSizer97->Layout();
    bSizer99->Layout();
    bSizer100->Layout();
    bSizer101->Layout();
    bSizer102->Layout();
}


void CMMapCoordinateDlg::OnInitDialog (
				wxInitDialogEvent&				event)
{
	SInt16								datumSelection,
											ellipsoidSelection,
											gridZone,
											mapUnitsSelection,
											projectionSelection,
											referenceSystemSelection;


	wxDialog::OnInitDialog(event);

			// Initialize dialog variables.

	CoordinateDialogInitialize (this,
											m_mapProjectionInfoPtr,
											&mapUnitsSelection,
											&m_xMapCoordinate11,
											&m_yMapCoordinate11,
											&m_horizontalPixelSize,
											&m_verticalPixelSize,
											&m_mapOrientationAngle,
											&referenceSystemSelection,
											&m_epsgCode,
											&projectionSelection,
											&datumSelection,
											&ellipsoidSelection,
											&m_radiusSpheroid,
											&m_semiMajorAxis,
											&m_semiMinorAxis,
											&gridZone,
											m_gridZoneDirectionString,
											&m_gridZoneWithDirection,
											m_epsgName,
											m_datumName,
											m_ellipsoidName,
											&m_longitudeCentralMeridian,
											&m_latitudeOrigin,
											&m_falseEasting,
											&m_falseNorthing,
											&m_scaleFactorOfCentralMeridian,
											&m_standardParallel1,
											&m_standardParallel2,
											&m_upperLeftLatitudeLongitudePoint);

	m_mapUnitsSelection = mapUnitsSelection;
	m_referenceSystemSelection = referenceSystemSelection;
	m_projectionSelection = abs(projectionSelection);
	m_datumSelection = abs(datumSelection);
	m_ellipsoidSelection = abs(ellipsoidSelection);

	m_referenceSystemCode = abs(referenceSystemSelection);
	m_projectionCode = abs(m_projectionSelection);
	m_datumCode = abs(m_datumSelection);
	m_ellipsoidCode = abs(m_ellipsoidSelection);

	m_gridZone = gridZone;

	/*	
	m_referenceSystemSelection = GetComboListSelection (IDC_ReferenceSystemCombo,
																			m_referenceSystemCode);

	m_projectionSelection = GetComboListSelection (IDC_ProjectionCombo,
																	m_projectionCode);

	m_datumSelection = GetComboListSelection (IDC_DatumCombo,
															m_datumCode);

	m_ellipsoidSelection = GetComboListSelection (IDC_EllipsoidCombo,
																	m_ellipsoidCode);
	*/
	m_gridZoneDirection = CString(&m_gridZoneDirectionString[1]);

			// This is needed because m_radiusSpheroid is what is used for
			// the major axis box.

	if (abs(m_ellipsoidCode) != kSphereEllipsoidCode)
		m_radiusSpheroid = m_semiMajorAxis;

	if (TransferDataToWindow())
		PositionDialogWindow();

			// Set default text selection to first edit text item	

	SelectDialogItemText (this, IDC_X11Coordinate, 0, SInt16_MAX);
	//SetSizerAndFit(bSizer96);
	AdjustDlgLayout();
	//this->Layout();
	//this->Fit();

}	// end "OnInitDialog"


void CMMapCoordinateDlg::OnSelendokMapUnitsCombo(wxCommandEvent& event) 
{

}


void CMMapCoordinateDlg::OnSelendokEllipsoidCombo(wxCommandEvent& event) 
{
	int						lastEllipsoidSelection;


	lastEllipsoidSelection = m_ellipsoidSelection;
	wxComboBox* ellipsoidcb = (wxComboBox *) FindWindow(IDC_EllipsoidCombo);
	m_ellipsoidSelection = ellipsoidcb->GetSelection();

	if (lastEllipsoidSelection != m_ellipsoidSelection) 
		{
		CoordinateDialogSetEllipsoidParameters (
                this,
                m_ellipsoidSelection);

		}	// end "if (lastEllipsoidSelection != m_ellipsoidSelection)"
    
	AdjustDlgLayout();
    
	//this->Layout();
	//this->Fit();
	
}	// end "OnSelendokEllipsoidCombo"


void CMMapCoordinateDlg::OnSelendokReferenceSystemCombo(wxCommandEvent& event) 
{
	int						lastReferenceSelection;

	SInt16					datumSelection,
								ellipsoidSelection,
								gridZone,
								mapUnitsSelection,
								projectionSelection;


	lastReferenceSelection = m_referenceSystemSelection;
	wxComboBox* referenceSystemSelection = (wxComboBox *) FindWindow(IDC_ReferenceSystemCombo);
	m_referenceSystemSelection = referenceSystemSelection->GetSelection();

	if (lastReferenceSelection != m_referenceSystemSelection) 
		{
		m_referenceSystemCode = abs(m_referenceSystemSelection);

		projectionSelection = m_projectionSelection;
		datumSelection = m_datumSelection;
		ellipsoidSelection = m_ellipsoidSelection;
		mapUnitsSelection = m_mapUnitsSelection;

		CoordinateDialogSetReferenceSystemParameters (
													this,
													m_referenceSystemCode,
													m_gridZoneDirectionString,
													FALSE,
													&projectionSelection,
													&datumSelection,
													&ellipsoidSelection,
													&mapUnitsSelection);

		if (m_projectionSelection != abs(projectionSelection)) 
			{
			m_projectionSelection = abs(projectionSelection);
			wxComboBox* projectionSelection = (wxComboBox*)FindWindow(IDC_ProjectionCombo);
			projectionSelection->SetSelection(m_projectionSelection);
			m_projectionCode = m_projectionSelection;

			}	// end "if (m_projectionSelection != projectionSelection)"

		if (m_datumSelection != abs(datumSelection)) 
			{
			m_datumSelection = abs(datumSelection);
			wxComboBox* datumSelection = (wxComboBox *) FindWindow(IDC_DatumCombo);
			datumSelection->SetSelection(m_datumSelection);

			m_datumCode = m_datumSelection;

			}	// end "if (m_datumSelection != datumSelection)"

		if (m_ellipsoidSelection != abs(ellipsoidSelection)) 
			{
			m_ellipsoidSelection = abs(ellipsoidSelection);
			wxComboBox* ellipsoidSelection = (wxComboBox *) FindWindow(IDC_EllipsoidCombo);
			ellipsoidSelection->SetSelection(m_ellipsoidSelection);
			m_ellipsoidCode = m_ellipsoidSelection;

			}	// end "if (m_ellipsoidSelection != abs(ellipsoidSelection))"

		if (m_mapUnitsSelection != abs(mapUnitsSelection)) 
			{
			m_mapUnitsSelection = abs(mapUnitsSelection);
			wxComboBox* mapUnitsSelection = (wxComboBox *) FindWindow(IDC_MapUnitsCombo);
			mapUnitsSelection->SetSelection(m_mapUnitsSelection);

			}	// end "if (m_mapUnitsSelection != abs(mapUnitsSelection))"

				// Set the projection parameters based on the selected reference system

		gridZone = m_gridZone;
		CoordinateDialogSetParametersFromRS (this,
														 m_referenceSystemSelection,
														 &projectionSelection,
														 m_gridZoneDirectionString,
														 m_gridZoneWithDirection,
														 m_datumCode,
														 m_ellipsoidCode,
														 (SInt16*)&m_gridZone);

		if (m_projectionSelection != abs(projectionSelection)) 
			{
			m_projectionSelection = abs(projectionSelection);
			wxComboBox* projectionSelection = (wxComboBox *) FindWindow(IDC_ProjectionCombo);
			projectionSelection->SetSelection(m_projectionSelection);

			m_projectionCode = m_projectionSelection;

			}	// end "if (m_projectionSelection != abs(projectionSelection))"

		if (m_gridZone != gridZone) 
			{
			wxTextCtrl* gridZonetc = (wxTextCtrl *) FindWindow(IDC_Zone);
			gridZonetc->ChangeValue(wxString::Format(wxT("%i"), m_gridZone));            
			}

		if (m_referenceSystemSelection == kByEPSGCodeCode) 
			{
			m_validEPSGCodeFlag = CoordinateDialogSetParametersFromEPSGCode (
																	  this,
																	  m_epsgCode,
																	  m_validEPSGCodeFlag,
																	  m_epsgName,
																	  m_datumName,
																	  m_ellipsoidName,
																	  &mapUnitsSelection,
																	  &projectionSelection);

			if (m_projectionSelection != abs(projectionSelection)) 
				{
				m_projectionSelection = abs(projectionSelection);
				wxComboBox* projectionSelection = (wxComboBox*)FindWindow(IDC_ProjectionCombo);
				projectionSelection->SetSelection(m_projectionSelection);

				m_projectionCode = m_projectionSelection;

				}	// end "if (m_projectionSelection = projectionSelection)"

			if (m_mapUnitsSelection != abs(mapUnitsSelection)) 
				{
				m_mapUnitsSelection = abs(mapUnitsSelection);
				wxComboBox* mapUnitsSelection = (wxComboBox *) FindWindow(IDC_MapUnitsCombo);
				mapUnitsSelection->SetSelection(m_mapUnitsSelection);

				}	// end "if (m_mapUnitsSelection != abs(mapUnitsSelection)"
			
			SetUpToolTip(m_staticText116, IDS_ToolTip300);
			SetUpToolTip(m_textCtrl57, IDS_ToolTip300);
			
			} // end "if (m_referenceSystemSelection == kByEPSGCodeCode)"
	  
		else	// m_referenceSystemSelection != kByEPSGCodeCode
			{
			SetUpToolTip(m_staticText116, IDS_ToolTip297);
			SetUpToolTip(m_textCtrl57, IDS_ToolTip297);
			
			}	// end "else m_referenceSystemSelection != kByEPSGCodeCode"

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastReferenceSelection != m_referenceSystemSelection)"
    
	AdjustDlgLayout();
	//this->Layout();
	//this->Fit();
	
}	// end "OnSelendokReferenceSystemCombo"


void CMMapCoordinateDlg::OnSelendokProjectionCombo(wxCommandEvent& event) 
{
	int						lastProjectionSelection;

	SInt16					datumSelection,
								ellipsoidSelection;


	lastProjectionSelection = m_projectionSelection;
	wxComboBox* projectionSelection = (wxComboBox *) FindWindow(IDC_ProjectionCombo);
	m_projectionSelection = projectionSelection->GetSelection();

	if (lastProjectionSelection != m_projectionSelection) 
		{
		m_projectionCode = abs(m_projectionSelection);

		datumSelection = m_datumSelection;
		ellipsoidSelection = m_ellipsoidSelection;

		CoordinateDialogHideShowProjectionParameters (
													 this,
													 m_referenceSystemCode,
													 m_projectionCode,
													 FALSE,
													 &datumSelection,
													 &ellipsoidSelection,
													 FALSE);

		if (m_datumSelection != abs(datumSelection)) 
			{
			m_datumSelection = abs(datumSelection);
			wxComboBox* datumSelection = (wxComboBox *) FindWindow(IDC_DatumCombo);
			datumSelection->SetSelection(m_datumSelection);
			m_datumCode = m_datumSelection;

			}	// end "if (m_datumSelection != datumSelection)"

		if (m_ellipsoidSelection != ellipsoidSelection) 
			{
			m_ellipsoidSelection = abs(ellipsoidSelection);
			wxComboBox* ellipsoidSelection = (wxComboBox*)FindWindow(IDC_EllipsoidCombo);
			ellipsoidSelection->SetSelection(m_ellipsoidSelection);
			m_ellipsoidCode = m_ellipsoidSelection;

			}	// end "if (m_ellipsoidSelection != ellipsoidSelection)"

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastGridSystemCode != m_gridSystemCode)"
	
	//this->Layout();
	//this->Fit();
	
}	// end "OnSelendokProjectionCombo"


void CMMapCoordinateDlg::OnSelendokDatumCombo(wxCommandEvent& event) 
{
	int						lastDatumSelection;


	lastDatumSelection = m_datumSelection;
	wxComboBox* datumSelection = (wxComboBox *) FindWindow(IDC_DatumCombo);
	m_datumSelection = datumSelection->GetSelection();

	if (lastDatumSelection != m_datumSelection) 
		{
		m_ellipsoidSelection = CoordinateDialogSetDatumParameters (
											 this,
											 m_datumSelection,
											 abs(m_ellipsoidSelection),
											 FALSE);

		m_ellipsoidSelection = abs(m_ellipsoidSelection);
		m_ellipsoidCode = m_ellipsoidSelection;

		wxComboBox* ellipsoidSelection = (wxComboBox *) FindWindow(IDC_EllipsoidCombo);
		ellipsoidSelection->SetSelection(m_ellipsoidSelection);
		m_datumCode = abs(m_datumSelection);

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastDatumSelection != m_datumSelection)"

	AdjustDlgLayout();
	
	//this->Layout();
	//this->Fit();
	
}	// end "OnSelendokDatumCombo"


void CMMapCoordinateDlg::OnChangeZoneDirection(wxCommandEvent& event) 
{
	SInt16					gridZone,
								projectionSelection;


	char lastGridZoneDirection = m_gridZoneDirection[0];

	m_gridZoneDirection = event.GetString();
	m_gridZoneDirection = m_gridZoneDirection.Upper();
    
	wxTextCtrl* gridZoneDirection = (wxTextCtrl*)FindWindow(IDC_ZoneDirection);
	gridZoneDirection->ChangeValue(m_gridZoneDirection);
	 
	m_datumCode = abs(m_datumSelection);

			// Set up gridZoneDirectionString to be used in the shared routines.

	m_gridZoneDirectionString[1] = m_gridZoneDirection[0];

	m_gridZoneWithDirection = m_gridZone;

	if (m_gridZoneDirection.IsSameAs('S',false))
		m_gridZoneWithDirection = -m_gridZone;

	projectionSelection = m_projectionSelection;
	CoordinateDialogSetParametersFromRS (this,
													 m_referenceSystemCode,
													 &projectionSelection,
													 m_gridZoneDirectionString,
													 m_gridZoneWithDirection,
													 m_datumCode,
													 m_ellipsoidCode,
													 &gridZone);

	if (m_projectionSelection != abs(projectionSelection)) 
		{
		m_projectionSelection = abs(projectionSelection);
		wxComboBox* projectionSelection = (wxComboBox*) FindWindow(IDC_ProjectionCombo);
		projectionSelection->SetSelection(m_projectionSelection);
		
		}	// end "if (m_projectionSelection = projectionSelection)"

	//this->Layout();
	//this->Fit();
	
}	// end "OnChangeZoneDirection"


void CMMapCoordinateDlg::OnEnChangeZone(wxCommandEvent& event) 
{

	SInt16				gridZone,
							maxZone,
							minZone,
							projectionSelection,
							savedGridZone;

	savedGridZone = m_gridZone;
	wxTextCtrl* gridZonetext = (wxTextCtrl *) FindWindow(IDC_Zone);
	m_gridZone = wxAtoi(gridZonetext->GetValue());

	CoordinateDialogGetMinMaxZone (m_referenceSystemCode,
												&minZone,
												&maxZone,
												m_gridZoneDirectionString);

	if (m_gridZone >= minZone && m_gridZone <= maxZone) 
		{
		m_gridZoneWithDirection = m_gridZone;

		if (m_gridZoneDirection.IsSameAs('S',false))
			m_gridZoneWithDirection = -m_gridZone;
			
		gridZone = m_gridZone;

		projectionSelection = m_projectionSelection;
		CoordinateDialogSetParametersFromRS (this,
														 m_referenceSystemCode,
														 &projectionSelection,
														 m_gridZoneDirectionString,
														 m_gridZoneWithDirection,
														 m_datumCode,
														 m_ellipsoidCode,
														 &gridZone);

		if (m_projectionSelection != abs(projectionSelection)) 
			{
			m_projectionSelection = abs(projectionSelection);
			wxComboBox* projectionSelection = (wxComboBox*) FindWindow(IDC_ProjectionCombo);
			projectionSelection->SetSelection(m_projectionSelection);
			
			}	// end "if (m_projectionSelection = projectionSelection)"

		}	// end "else grid zone is within the proper range
	 
	//this->Layout();
	//this->Fit();
	
} // end "OnEnChangeZone"


void CMMapCoordinateDlg::OnEnChangeEPSGCode(wxCommandEvent& event) 
{
	SInt16						mapUnitsSelection,
									projectionSelection,
									savedEPSGCode;


	savedEPSGCode = m_epsgCode;
	wxTextCtrl* epsgCode = (wxTextCtrl*)FindWindow(IDC_EPSGCode);
	m_epsgCode = wxAtoi(epsgCode->GetValue());

	if (m_epsgCode != savedEPSGCode) 
		{
		projectionSelection = m_projectionSelection;
		mapUnitsSelection = m_mapUnitsSelection;

		m_validEPSGCodeFlag = CoordinateDialogSetParametersFromEPSGCode (
											 this,
											 m_epsgCode,
											 m_validEPSGCodeFlag,
											 m_epsgName,
											 m_datumName,
											 m_ellipsoidName,
											 &mapUnitsSelection,
											 &projectionSelection);

		if (m_projectionSelection != abs(projectionSelection)) 
			{
			m_projectionSelection = abs(projectionSelection);
			wxComboBox* projectionSelection = (wxComboBox*)FindWindow(IDC_ProjectionCombo);
			projectionSelection->SetSelection(m_projectionSelection);
			
			}	// end "if (m_projectionSelection = projectionSelection)"

		if (m_mapUnitsSelection != abs(mapUnitsSelection)) 
			{
			m_mapUnitsSelection = abs(mapUnitsSelection);
			wxComboBox* mapUnitsSelection = (wxComboBox*)FindWindow(IDC_MapUnitsCombo);
			mapUnitsSelection->SetSelection(m_mapUnitsSelection);
			
			}	// end "if (m_mapUnitsSelection != abs(mapUnitsSelection)"

		}	// end "else grid zone is within the proper range
	 
	//this->Layout();
	//this->Fit();
	
}	// end "OnEnChangeEPSGCode"


bool CMMapCoordinateDlg::TransferDataFromWindow() 
{
	wxComboBox* referenceSystemcb = (wxComboBox *) FindWindow(IDC_ReferenceSystemCombo);
	wxComboBox* ellipsoidcb = (wxComboBox *) FindWindow(IDC_EllipsoidCombo);
	wxComboBox* projectioncb = (wxComboBox *) FindWindow(IDC_ProjectionCombo);
	wxComboBox* mapunitscb = (wxComboBox *) FindWindow(IDC_MapUnitsCombo);
	wxComboBox* datumSelection = (wxComboBox *) FindWindow(IDC_DatumCombo);

	wxTextCtrl* gridZone = (wxTextCtrl*) FindWindow(IDC_Zone);
	wxTextCtrl* epsgCode = (wxTextCtrl*) FindWindow(IDC_EPSGCode);
	wxTextCtrl* horizontalPixelSize = (wxTextCtrl*) FindWindow(IDC_HorizontalSize);
	wxTextCtrl* verticalPixelSize = (wxTextCtrl*) FindWindow(IDC_VerticalSize);
	wxTextCtrl* xMapCoordinate11 = (wxTextCtrl*) FindWindow(IDC_X11Coordinate);
	wxTextCtrl* yMapCoordinate11 = (wxTextCtrl*) FindWindow(IDC_Y11Coordinate);
	wxTextCtrl* falseEasting = (wxTextCtrl*) FindWindow(IDC_FalseEasting);
	wxTextCtrl* falseNorthing = (wxTextCtrl*) FindWindow(IDC_FalseNorthing);
	wxTextCtrl* latitudeOrigin = (wxTextCtrl*) FindWindow(IDC_Latitude);
	wxTextCtrl* longitudeCentralMeridian = (wxTextCtrl*) FindWindow(IDC_Longitude);
	wxTextCtrl* semiMinorAxis = (wxTextCtrl*) FindWindow(IDC_MinorAxis);
	wxTextCtrl* radiusSpheroid = (wxTextCtrl*) FindWindow(IDC_Radius);
	wxTextCtrl* scaleFactorOfCentralMeridian = (wxTextCtrl*) FindWindow(IDC_ScaleFactor);
	wxTextCtrl* standardParallel1 = (wxTextCtrl*) FindWindow(IDC_StandardParallel1);
	wxTextCtrl* standardParallel2 = (wxTextCtrl*) FindWindow(IDC_StandardParallel2);
	wxTextCtrl* gridZoneDirection = (wxTextCtrl*) FindWindow(IDC_ZoneDirection);
	wxTextCtrl* mapOrientationAngle = (wxTextCtrl*) FindWindow(IDC_OrientationAngle);

	m_referenceSystemSelection = referenceSystemcb->GetSelection();
	m_ellipsoidSelection = ellipsoidcb->GetSelection();
	m_projectionSelection = projectioncb->GetSelection();
	m_mapUnitsSelection = mapunitscb->GetSelection();
	m_datumSelection = datumSelection->GetSelection();

	m_gridZone = wxAtoi(gridZone->GetValue());
	m_epsgCode = wxAtoi(epsgCode->GetValue());

	m_gridZoneDirection = gridZoneDirection->GetValue();

	wxString horizontalPixelSize_str = horizontalPixelSize->GetValue();
	wxString verticalPixelSize_str = verticalPixelSize->GetValue();
	wxString xMapCoordinate11_str = xMapCoordinate11->GetValue();
	wxString yMapCoordinate11_str = yMapCoordinate11->GetValue();
	wxString falseEasting_str = falseEasting->GetValue();
	wxString falseNorthing_str = falseNorthing->GetValue();
	wxString latitudeOrigin_str = latitudeOrigin->GetValue();
	wxString longitudeCentralMeridian_str = longitudeCentralMeridian->GetValue();
	wxString semiMinorAxis_str = semiMinorAxis->GetValue();
	wxString radiusSpheroid_str = radiusSpheroid->GetValue();
	wxString scaleFactorOfCentralMeridian_str = scaleFactorOfCentralMeridian->GetValue();
	wxString standardParallel1_str = standardParallel1->GetValue();
	wxString standardParallel2_str = standardParallel2->GetValue();

	wxString mapOrientationAngle_str = mapOrientationAngle->GetValue();


	horizontalPixelSize_str.ToDouble(&m_horizontalPixelSize);
	verticalPixelSize_str.ToDouble(&m_verticalPixelSize);
	xMapCoordinate11_str.ToDouble(&m_xMapCoordinate11);
	yMapCoordinate11_str.ToDouble(&m_yMapCoordinate11);
	falseEasting_str.ToDouble(&m_falseEasting);
	falseNorthing_str.ToDouble(&m_falseNorthing);
	latitudeOrigin_str.ToDouble(&m_latitudeOrigin);
	longitudeCentralMeridian_str.ToDouble(&m_longitudeCentralMeridian);
	semiMinorAxis_str.ToDouble(&m_semiMinorAxis);
	radiusSpheroid_str.ToDouble(&m_radiusSpheroid);
	scaleFactorOfCentralMeridian_str.ToDouble(&m_scaleFactorOfCentralMeridian);
	standardParallel1_str.ToDouble(&m_standardParallel1);
	standardParallel2_str.ToDouble(&m_standardParallel2);
	mapOrientationAngle_str.ToDouble(&m_mapOrientationAngle);

	m_referenceSystemCode = abs(m_referenceSystemSelection);
	m_projectionCode = abs(m_projectionSelection);
	m_datumCode = abs(m_datumSelection);
	m_ellipsoidCode = abs(m_ellipsoidSelection);

			// Verify that the grid zone parameter is within range if it is being used.

	if (!CoordinateDialogCheckIfZoneIsValid (this,
														m_referenceSystemCode,
														m_gridZoneDirectionString)) 
		{
		FindWindow(IDC_Zone)->SetFocus();
		return false;

		}	// end "if (!CoordinateDialogCheckIfZoneIsValid (dialogPtr, ..."

	return true;
	 
}	// end "TransferDataFromWindow"


bool CMMapCoordinateDlg::TransferDataToWindow() 
{
	wxComboBox* referenceSystemcb = (wxComboBox *) FindWindow(IDC_ReferenceSystemCombo);
	wxComboBox* ellipsoidcb = (wxComboBox *) FindWindow(IDC_EllipsoidCombo);
	wxComboBox* projectioncb = (wxComboBox *) FindWindow(IDC_ProjectionCombo);
	wxComboBox* mapunitscb = (wxComboBox *) FindWindow(IDC_MapUnitsCombo);
	wxComboBox* datumSelection = (wxComboBox *) FindWindow(IDC_DatumCombo);

	wxTextCtrl* gridZone = (wxTextCtrl*) FindWindow(IDC_Zone);
	wxTextCtrl* epsgCode = (wxTextCtrl*) FindWindow(IDC_EPSGCode);
	wxTextCtrl* horizontalPixelSize = (wxTextCtrl*) FindWindow(IDC_HorizontalSize);
	wxTextCtrl* verticalPixelSize = (wxTextCtrl*) FindWindow(IDC_VerticalSize);
	wxTextCtrl* xMapCoordinate11 = (wxTextCtrl*) FindWindow(IDC_X11Coordinate);
	wxTextCtrl* yMapCoordinate11 = (wxTextCtrl*) FindWindow(IDC_Y11Coordinate);
	wxTextCtrl* falseEasting = (wxTextCtrl*) FindWindow(IDC_FalseEasting);
	wxTextCtrl* falseNorthing = (wxTextCtrl*) FindWindow(IDC_FalseNorthing);
	wxTextCtrl* latitudeOrigin = (wxTextCtrl*) FindWindow(IDC_Latitude);
	wxTextCtrl* longitudeCentralMeridian = (wxTextCtrl*) FindWindow(IDC_Longitude);
	wxTextCtrl* semiMinorAxis = (wxTextCtrl*) FindWindow(IDC_MinorAxis);
	wxTextCtrl* radiusSpheroid = (wxTextCtrl*) FindWindow(IDC_Radius);
	wxTextCtrl* scaleFactorOfCentralMeridian = (wxTextCtrl*) FindWindow(IDC_ScaleFactor);
	wxTextCtrl* standardParallel1 = (wxTextCtrl*) FindWindow(IDC_StandardParallel1);
	wxTextCtrl* standardParallel2 = (wxTextCtrl*) FindWindow(IDC_StandardParallel2);
	wxTextCtrl* gridZoneDirection = (wxTextCtrl*) FindWindow(IDC_ZoneDirection);
	wxTextCtrl* mapOrientationAngle = (wxTextCtrl*) FindWindow(IDC_OrientationAngle);

	gridZone->ChangeValue(wxString::Format(wxT("%i"), m_gridZone));
	epsgCode->ChangeValue(wxString::Format(wxT("%i"), m_epsgCode));
	horizontalPixelSize->ChangeValue(wxString::Format(wxT("%.4f"), m_horizontalPixelSize));
	verticalPixelSize->ChangeValue(wxString::Format(wxT("%.4f"), m_verticalPixelSize));
	xMapCoordinate11->ChangeValue(wxString::Format(wxT("%.4f"), m_xMapCoordinate11));
	yMapCoordinate11->ChangeValue(wxString::Format(wxT("%.4f"), m_yMapCoordinate11));
	falseEasting->ChangeValue(wxString::Format(wxT("%.2f"), m_falseEasting));
	falseNorthing->ChangeValue(wxString::Format(wxT("%.2f"), m_falseNorthing));
	latitudeOrigin->ChangeValue(wxString::Format(wxT("%.6f"), m_latitudeOrigin));
	longitudeCentralMeridian->ChangeValue(wxString::Format(wxT("%.6f"), m_longitudeCentralMeridian));
	semiMinorAxis->ChangeValue(wxString::Format(wxT("%.4f"), m_semiMinorAxis));
	radiusSpheroid->ChangeValue(wxString::Format(wxT("%.4f"), m_radiusSpheroid));
	scaleFactorOfCentralMeridian->ChangeValue(wxString::Format(wxT("%.6f"), m_scaleFactorOfCentralMeridian));
	standardParallel1->ChangeValue(wxString::Format(wxT("%.6f"), m_standardParallel1));
	standardParallel2->ChangeValue(wxString::Format(wxT("%.6f"), m_standardParallel2));
	gridZoneDirection->ChangeValue(m_gridZoneDirection);
	mapOrientationAngle->ChangeValue(wxString::Format(wxT("%.6f"), m_mapOrientationAngle));

	referenceSystemcb->SetSelection(m_referenceSystemSelection);
	ellipsoidcb->SetSelection(m_ellipsoidSelection);
	projectioncb->SetSelection(m_projectionSelection);
	mapunitscb->SetSelection(m_mapUnitsSelection);
	datumSelection->SetSelection(m_datumSelection);
	
	return true;
	 
}	// end "TransferDataToWindow"
