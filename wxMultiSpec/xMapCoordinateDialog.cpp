//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						xMapCoordinateDialog.cpp : class implementation file
//	Class Definition:		xMapCoordinateDialog.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/07/2020
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMMapCoordinateDlg class.
//
// Note that m_adjustUpperLeftMapPointFlag has not been fully integrated in this code
// yet. See Mac version for more info. 3Is not being used right now till a better
// method is made available for the user to control.
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xMapCoordinateDialog.h"	

#include "wx/valgen.h"
#include "wx/valnum.h"
#include "wx/stream.h"
#include "wx/string.h"
#include "wx/window.h"



BEGIN_EVENT_TABLE (CMMapCoordinateDlg, CMDialog)
	EVT_CHOICE (IDC_DatumCombo, CMMapCoordinateDlg::OnSelendokDatumCombo)
	EVT_CHOICE (IDC_EllipsoidCombo, CMMapCoordinateDlg::OnSelendokEllipsoidCombo)
	EVT_CHOICE (IDC_MapUnitsCombo, CMMapCoordinateDlg::OnSelendokMapUnitsCombo)
	EVT_CHOICE (IDC_ProjectionCombo, CMMapCoordinateDlg::OnSelendokProjectionCombo)
	EVT_CHOICE (IDC_ReferenceSystemCombo, CMMapCoordinateDlg::OnSelendokReferenceSystemCombo)

	EVT_INIT_DIALOG (CMMapCoordinateDlg::OnInitDialog)

	EVT_TEXT (IDC_EPSGCode, CMMapCoordinateDlg::OnEnChangeEPSGCode)
	EVT_TEXT (IDC_Zone, CMMapCoordinateDlg::OnEnChangeZone)
	EVT_TEXT (IDC_ZoneDirection, CMMapCoordinateDlg::OnChangeZoneDirection)
END_EVENT_TABLE ()



CMMapCoordinateDlg::CMMapCoordinateDlg (
				wxWindow* 							pParent,
				wxWindowID 							id,
				const wxString& 					title /*=NULL*/)
		: CMDialog (CMMapCoordinateDlg::IDD, pParent, title)

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
	m_gridZoneDirectionString[1] = 0;
	m_epsgName[0] = 0;
	m_datumName[0] = 0;
	m_ellipsoidName[0] = 0;
	m_validEPSGCodeFlag = TRUE;

	m_initializedFlag = CMDialog::m_initializedFlag;
	
	CreateControls ();

}	// end "CMMapCoordinateDlg"



void CMMapCoordinateDlg::CreateControls ()

{
	this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	bSizer96 = new wxBoxSizer (wxVERTICAL);
	
	bSizer97 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText107 = new wxStaticText (this,
													wxID_ANY,
													wxT("Planar Coordinate Information"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText107->Wrap (-1);
	bSizer97->Add (m_staticText107, wxSizerFlags(0).Center().Border(wxBOTTOM, 5));
	
	bSizer97->Add (20, 0, 0, wxEXPAND);
	
	m_staticText108 = new wxStaticText (this,
													IDC_UnitsPrompt,
													wxT("Units:"),
													wxDefaultPosition,
      											wxDefaultSize,
      											0);
	m_staticText108->Wrap (-1);
   SetUpToolTip (m_staticText108, IDS_ToolTip290);
	//bSizer97->Add(m_staticText108, 0, wxALIGN_CENTER|wxALL, 5);
	bSizer97->Add (m_staticText108,
						wxSizerFlags(0).Center().Border(wxRIGHT|wxBOTTOM, 5));
	
	m_mapUnitsCtrl = new wxChoice (this,
												IDC_MapUnitsCombo,
												//wxT("Meters"),
												wxDefaultPosition,
												wxDefaultSize);
												//0,
												//NULL,
												//wxCB_READONLY);
	m_mapUnitsCtrl->Append (wxT("Unknown Units"));
	m_mapUnitsCtrl->Append (wxT("Decimal Degrees"));
	m_mapUnitsCtrl->Append (wxT("Kilometers"));
	m_mapUnitsCtrl->Append (wxT("Meters"));
	m_mapUnitsCtrl->Append (wxT("Centimeters"));
	m_mapUnitsCtrl->Append (wxT("Milimeters"));
	m_mapUnitsCtrl->Append (wxT("Micrometers"));
	m_mapUnitsCtrl->Append (wxT("Miles"));
	m_mapUnitsCtrl->Append (wxT("Yards"));
	m_mapUnitsCtrl->Append (wxT("US Survey Feet"));
	m_mapUnitsCtrl->Append (wxT("Feet"));
	m_mapUnitsCtrl->Append (wxT("Inches"));
   SetUpToolTip (m_mapUnitsCtrl, IDS_ToolTip290);
	bSizer97->Add (m_mapUnitsCtrl, wxSizerFlags(0).Border(wxBOTTOM, 5));
	
	m_staticText307 = new wxStaticText (this,
													IDC_MapUnitsName,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText307->Wrap (-1);
	bSizer97->Add (m_staticText307, wxSizerFlags(0).Center().Border(wxBOTTOM,5));
        
	bSizer96->Add (bSizer97,
						wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));
        
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer (5, 2, 0, 0);
	fgSizer4->SetFlexibleDirection (wxBOTH);
	fgSizer4->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxFloatingPointValidator<double> doubleValue4Digits (4, &m_double4DigitValueCheck);
	wxFloatingPointValidator<double> doubleValue6Digits (6, &m_double6DigitValueCheck);
	wxFloatingPointValidator<double> doubleValue9Digits (9, &m_double6DigitValueCheck);
	wxFloatingPointValidator<double> doubleValue10Digits (10, &m_double10DigitValueCheck);
	
	m_staticText109 = new wxStaticText (
				this,
				IDC_X11CoordinatePrompt,
				wxT("X map coordinate for center of upper-left pixel (1,1): . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText109->Wrap (-1);
   SetUpToolTip (m_staticText109, IDS_ToolTip291);
	fgSizer4->Add (m_staticText109,
						wxSizerFlags(0).Left().
							Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 12));
	
	m_textCtrl52 = new wxTextCtrl (this,
												IDC_X11Coordinate,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl52->SetValidator (doubleValue6Digits);
	m_textCtrl52->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl52, IDS_ToolTip291);
	//fgSizer4->Add (m_textCtrl52, 0, 0, 5);
	fgSizer4->Add (m_textCtrl52, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2));

	m_staticText110 = new wxStaticText (
				this,
				IDC_Y11CoordinatePrompt,
				wxT("Y map coordinate for center of upper-left pixel (1,1): . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText110->Wrap (-1);
   SetUpToolTip (m_staticText110, IDS_ToolTip292);
	fgSizer4->Add (m_staticText110,
						wxSizerFlags(0).Left().
							Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 12));
	
	m_textCtrl53 = new wxTextCtrl (this,
												IDC_Y11Coordinate,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl53->SetValidator (doubleValue6Digits);
	m_textCtrl53->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl53, IDS_ToolTip292);
	fgSizer4->Add (m_textCtrl53, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText111 = new wxStaticText (
			this,
			IDC_HorizontalSizePrompt,
			wxT("Horizontal pixel size: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
			wxDefaultPosition,
			wxDefaultSize,
			0);
	m_staticText111->Wrap (-1);
   SetUpToolTip (m_staticText111, IDS_ToolTip293);
	fgSizer4->Add (m_staticText111,
						wxSizerFlags(0).Left().
							Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 12));
	
	m_textCtrl54 = new wxTextCtrl (this,
												IDC_HorizontalSize,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl54->SetValidator (doubleValue9Digits);
	m_textCtrl54->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl54, IDS_ToolTip293);
	fgSizer4->Add (m_textCtrl54, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText112 = new wxStaticText (
				this,
				IDC_VerticalSizePrompt,
				wxT("Vertical pixel size: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText112->Wrap (-1);
   SetUpToolTip (m_staticText112, IDS_ToolTip294);
	fgSizer4->Add (m_staticText112,
						wxSizerFlags(0).Left().
							Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 12));
	
	m_textCtrl55 = new wxTextCtrl (this,
												IDC_VerticalSize,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120,-1),
												0);
   m_textCtrl55->SetValidator (doubleValue9Digits);
	m_textCtrl55->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl55, IDS_ToolTip294);
	fgSizer4->Add (m_textCtrl55, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText113 = new wxStaticText (
				this,
				wxID_ANY,
				wxT("Map orientation angle: . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText113->Wrap (-1);
   SetUpToolTip (m_staticText113, IDS_ToolTip295);
	fgSizer4->Add (m_staticText113,
						wxSizerFlags(0).Left().
							Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 12));
	
	m_textCtrl56 = new wxTextCtrl (this,
												IDC_OrientationAngle,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
	wxFloatingPointValidator<double> doubleValueAngle (6, &m_doubleValueCheck);
	doubleValueAngle.SetRange (-180, 180);
	m_textCtrl56->SetValidator (doubleValueAngle);
	m_textCtrl56->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl56, IDS_ToolTip295);
	fgSizer4->Add (m_textCtrl56, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	bSizer96->Add (fgSizer4, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText115 = new wxStaticText (this,
													IDC_ReferenceSystemPrompt,
													wxT("Grid Coordinate System:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText115->Wrap (-1);
   SetUpToolTip (m_staticText115, IDS_ToolTip296);
	bSizer98->Add (m_staticText115,
						wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM, 5));
	
	m_refSystemCtrl = new wxChoice (this,
												IDC_ReferenceSystemCombo,
												//wxT("User Defined"),
												wxDefaultPosition,
												wxSize (295, -1));
												//0,
												//NULL,
												//wxCB_READONLY);
	m_refSystemCtrl->Append (wxT("none defined"));
	m_refSystemCtrl->Append (wxT("User Defined"));
	m_refSystemCtrl->Append (wxT("Geographic (lat-long)"));
	m_refSystemCtrl->Append (wxT("Gauss Kruger"));
	m_refSystemCtrl->Append (wxT("GDA94"));
	m_refSystemCtrl->Append (wxT("Greek Geographic Reference System 1987"));
	m_refSystemCtrl->Append (wxT("State Plane - NAD27"));
	m_refSystemCtrl->Append (wxT("State Plane - NDA83"));
	m_refSystemCtrl->Append (wxT("UTM - NAD27"));
	m_refSystemCtrl->Append (wxT("UTM - NAD83"));
	m_refSystemCtrl->Append (wxT("UTM - SAD69"));
	m_refSystemCtrl->Append (wxT("UTM - WGS72"));
	m_refSystemCtrl->Append (wxT("UTM - WGS84"));
	m_refSystemCtrl->Append (wxT("UTM"));
	m_refSystemCtrl->Append (wxT("Defined by EPSG Code:"));
	m_refSystemCtrl->SetSelection (1);
   SetUpToolTip (m_refSystemCtrl, IDS_ToolTip296);
	bSizer98->Add (m_refSystemCtrl, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	bSizer96->Add (bSizer98, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	bSizer99 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText116 = new wxStaticText (this,
													IDC_ZonePrompt,
													wxT("FIPS Zone:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText116->Wrap (-1);
   SetUpToolTip (m_staticText116, IDS_ToolTip297);
	bSizer99->Add (m_staticText116, 0, wxALIGN_CENTER|wxTOP|wxRIGHT|wxBOTTOM, 5);
	
	m_textCtrl181 = new wxTextCtrl (this,
												IDC_EPSGCode,
												wxEmptyString,
												wxDefaultPosition,
												wxDefaultSize,
												0);
   m_textCtrl181->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
	m_textCtrl181->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl181, IDS_ToolTip300);
	bSizer99->Add (m_textCtrl181, 0, wxALL, 5);
	
	m_textCtrl57 = new wxTextCtrl (this,
												IDC_Zone,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (40,-1),
												0);
   m_textCtrl57->SetValidator (wxTextValidator (wxFILTER_DIGITS, &m_stringCheck));
	m_textCtrl57->SetWindowStyle (wxTE_RIGHT);
   SetUpToolTip (m_textCtrl57, IDS_ToolTip297);
	bSizer99->Add (m_textCtrl57, 0, wxALL, 5);
	
	m_textCtrl67 = new wxTextCtrl (this,
												IDC_ZoneDirection,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (25, -1),
												0);
	m_textCtrl67->SetMaxLength (1);
	wxTextValidator valid_char =
								wxTextValidator (wxFILTER_ASCII|wxFILTER_INCLUDE_CHAR_LIST);
	wxString valid_str = "'N','S','n','s'";
	valid_char.SetCharIncludes (valid_str);
	m_textCtrl67->SetValidator (valid_char);
	bSizer99->Add (m_textCtrl67, 0, wxALL, 5);
        
	m_staticText306 = new wxStaticText (this,
													IDC_FIPSZoneName,
													wxT("MyLabel"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText306->Wrap (-1);
	bSizer99->Add (m_staticText306, 0, wxALIGN_CENTER|wxALL, 5);
	
	bSizer96->Add (bSizer99, wxSizerFlags(0).Expand().Border(wxLEFT,24));
	
	bSizer100 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText119 = new wxStaticText (this,
													IDC_ProjectionPrompt,
													wxT("Projection:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText119->Wrap (-1);
	bSizer100->Add (m_staticText119,
							wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM, 5));
	
	m_projectionCtrl = new wxChoice (this,
												IDC_ProjectionCombo,
												wxDefaultPosition,
												wxSize (285, -1));
	m_projectionCtrl->Append(wxT("None defined"));
	m_projectionCtrl->Append(wxT("Alaska Conformal"));
	m_projectionCtrl->Append(wxT("Albers Conical Equal Area"));
	m_projectionCtrl->Append(wxT("Azimuthal Equidistant"));
	m_projectionCtrl->Append(wxT("Cylindrical Equal Area"));
	m_projectionCtrl->Append(wxT("Equidistant Conic"));
	m_projectionCtrl->Append(wxT("Equirectangular"));
	m_projectionCtrl->Append (wxT("General Vertical Near-Side Perspective"));
	m_projectionCtrl->Append(wxT("Gnomonic"));
	m_projectionCtrl->Append(wxT("Hammer"));
	m_projectionCtrl->Append(wxT("Integerized Sinusoidal"));
	m_projectionCtrl->Append(wxT("Interrupted Goode Homolosine"));
	m_projectionCtrl->Append(wxT("Interrupted Mollweide"));
	m_projectionCtrl->Append(wxT("Krovak"));
	m_projectionCtrl->Append(wxT("Lambert Azimuthal Equal-area"));
	m_projectionCtrl->Append(wxT("Lambert Conformal Conic"));
	m_projectionCtrl->Append(wxT("Mercator"));
	m_projectionCtrl->Append(wxT("Miller Cylindrical"));
	m_projectionCtrl->Append(wxT("Modified Transverse Mercator"));
	m_projectionCtrl->Append(wxT("Mollweide"));
	m_projectionCtrl->Append(wxT("New Zealand Map Grid"));
	m_projectionCtrl->Append(wxT("Oblated Equal Area"));
	m_projectionCtrl->Append(wxT("Oblique Mercator"));
	m_projectionCtrl->Append(wxT("Orthographic"));
	m_projectionCtrl->Append(wxT("Polar Stereographic"));
	m_projectionCtrl->Append(wxT("Polyconic"));
	m_projectionCtrl->Append(wxT("Robinson"));
	m_projectionCtrl->Append(wxT("Sinusoidal"));
	m_projectionCtrl->Append(wxT("Space Oblique Mercator"));
	m_projectionCtrl->Append(wxT("Stereographic"));
	m_projectionCtrl->Append(wxT("Transverse Mercator"));
	m_projectionCtrl->Append(wxT("Transverse Mercator South Oriented"));
	m_projectionCtrl->Append(wxT("Van der Grinten I"));
	m_projectionCtrl->Append(wxT("Wagner IV"));
	m_projectionCtrl->Append(wxT("Wagner VII"));
	bSizer100->Add (m_projectionCtrl, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	m_staticText301 = new wxStaticText (this,
													IDC_ProjectionName,
													wxEmptyString,
													//wxT("MyLabel"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText301->Wrap (-1);
	bSizer100->Add (m_staticText301,
							wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));
	
	bSizer96->Add (bSizer100, wxSizerFlags(0).Expand().Border(wxLEFT,24));
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer (7, 2, 0, 0);
	fgSizer5->SetFlexibleDirection (wxBOTH);
	fgSizer5->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_staticText121 = new wxStaticText (
				this,
				IDC_LongitudePrompt,
				wxT("Longitude of central meridian (decimal degrees): . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText121->Wrap (-1);
	fgSizer5->Add (m_staticText121,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl58 = new wxTextCtrl (this,
												IDC_Longitude,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl58->SetValidator (doubleValue6Digits);
	m_textCtrl58->SetWindowStyle (wxTE_RIGHT);
 	fgSizer5->Add (m_textCtrl58, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText123 = new wxStaticText (
				this,
				IDC_LatitudePrompt,
				wxT("Latitude of origin (decimal degrees): . . . . . . . . . . . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText123->Wrap (-1);
	fgSizer5->Add (m_staticText123,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl59 = new wxTextCtrl (this,
												IDC_Latitude,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl59->SetValidator (doubleValue6Digits);
	m_textCtrl59->SetWindowStyle (wxTE_RIGHT);
 	fgSizer5->Add (m_textCtrl59, wxSizerFlags(0).Border(wxTOP|wxBOTTOM,2));
	
	m_staticText125 = new wxStaticText (
				this,
				IDC_FalseEastingPrompt,
				wxT("False Easting: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
				wxDefaultPosition,
				wxDefaultSize,
				0);
	m_staticText125->Wrap (-1);
	fgSizer5->Add (m_staticText125,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl60 = new wxTextCtrl (this,
												IDC_FalseEasting,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl60->SetValidator (doubleValue6Digits);
	m_textCtrl60->SetWindowStyle (wxTE_RIGHT);
 	fgSizer5->Add (m_textCtrl60, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2));
	
	m_staticText127 = new wxStaticText (this,
													IDC_FalseNorthingPrompt,
													wxT("False Northing: . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText127->Wrap (-1);
	fgSizer5->Add (m_staticText127,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl61 = new wxTextCtrl (this,
												IDC_FalseNorthing,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl61->SetValidator (doubleValue6Digits);
	m_textCtrl61->SetWindowStyle (wxTE_RIGHT);
	fgSizer5->Add (m_textCtrl61, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2));
	
	m_staticText129 = new wxStaticText (this,
													IDC_ScaleFactorPrompt,
													wxT("Scale factor of central meridian: . . . . . . . . . . . . . . . . . ."),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText129->Wrap (-1);
	fgSizer5->Add (m_staticText129,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl62 = new wxTextCtrl (this,
												IDC_ScaleFactor,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl62->SetValidator (doubleValue10Digits);
	m_textCtrl62->SetWindowStyle (wxTE_RIGHT);
	fgSizer5->Add (m_textCtrl62, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2));
	
	m_staticText142 = new wxStaticText (this,
													IDC_StandardParallel1Prompt,
													wxT("Standard parallel 1:  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText142->Wrap (-1);
	fgSizer5->Add (m_staticText142,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl65 = new wxTextCtrl (this,
												IDC_StandardParallel1,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl65->SetValidator (doubleValue6Digits);
	m_textCtrl65->SetWindowStyle (wxTE_RIGHT);
	fgSizer5->Add (m_textCtrl65, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2));
	
	m_staticText143 = new wxStaticText (this,
													IDC_StandardParallel2Prompt,
													wxT("Standard parallel 2:  . . . . . . . . . . . . . . . . . . . . . . . . . . . . ."),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText143->Wrap (-1);
	fgSizer5->Add (m_staticText143,
						wxSizerFlags(0).Left().Align(wxALIGN_CENTER_VERTICAL));
	
	m_textCtrl66 = new wxTextCtrl (this,
												IDC_StandardParallel2,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120,-1),
												0);
   m_textCtrl66->SetValidator (doubleValue6Digits);
	m_textCtrl66->SetWindowStyle (wxTE_RIGHT);
	fgSizer5->Add (m_textCtrl66, wxSizerFlags(0).Border(wxTOP|wxBOTTOM, 2));
        
	bSizer96->Add (fgSizer5, wxSizerFlags(0).Expand().Border(wxLEFT, 36));
	
	bSizer101 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText133 = new wxStaticText (this,
													IDC_DatumPrompt,
													wxT("Geodetic Model       Datum:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText133->Wrap (-1);
   SetUpToolTip (m_staticText133, IDS_ToolTip298);
	bSizer101->Add (m_staticText133,
							wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM,5));
	
	m_datumCtrl = new wxChoice (this,
											IDC_DatumCombo,
											wxDefaultPosition,
											wxSize (285, -1));
	m_datumCtrl->Append (wxT("None defined"));
	m_datumCtrl->Append (wxT("Beijing 1954"));
	m_datumCtrl->Append (wxT("Campo Inchauspe"));
	m_datumCtrl->Append (wxT("Geocentric Datum of Australia 1994"));
	m_datumCtrl->Append (wxT("Greek Geodetic Reference System 1987"));
	m_datumCtrl->Append (wxT("NAD 27"));
	m_datumCtrl->Append (wxT("NAD 83"));
	m_datumCtrl->Append (wxT("NAD 83 (CSRS)"));
	m_datumCtrl->Append (wxT("New Zealand Geodetic Datum 1949"));
	m_datumCtrl->Append (wxT("NGO 1948 (Oslow)"));
	m_datumCtrl->Append (wxT("Ord. Survey G. Britain 1936"));
	m_datumCtrl->Append (wxT("Pulkovo 1942"));
	m_datumCtrl->Append (wxT("Pulkovo 1995"));
	m_datumCtrl->Append (wxT("South American 1969"));
	m_datumCtrl->Append (wxT("WGS 72"));
	m_datumCtrl->Append (wxT("WGS 84"));
	m_datumCtrl->Append (wxT("Sphere"));
	m_datumCtrl->SetSelection (5);
   SetUpToolTip (m_datumCtrl, IDS_ToolTip298);
	bSizer101->Add (m_datumCtrl, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	m_staticText302 = new wxStaticText (this,
													IDC_DatumName,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													//wxSize (240, -1),
													0);
	m_staticText302->Wrap (-1);
   SetUpToolTip (m_staticText302, IDS_ToolTip298);
	bSizer101->Add (m_staticText302,
							wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	bSizer96->Add (bSizer101, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	bSizer102 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText136 = new wxStaticText (this,
													IDC_ProjectionEllipsoidPrompt,
													wxT("Projection Ellipsoid:"),
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText136->Wrap (-1);
   SetUpToolTip (m_staticText136, IDS_ToolTip299);
	bSizer102->Add (m_staticText136,
							wxSizerFlags(0).Center().Border(wxTOP|wxRIGHT|wxBOTTOM, 5));
	
	m_ellipsoidCtrl = new wxChoice (this,
												IDC_EllipsoidCombo,
												wxDefaultPosition,
      										wxDefaultSize);
	m_ellipsoidCtrl->Append (wxT("None defined"));
	m_ellipsoidCtrl->Append (wxT("User Defined"));
	m_ellipsoidCtrl->Append(wxT("Airy"));
	m_ellipsoidCtrl->Append(wxT("Australian"));
	m_ellipsoidCtrl->Append(wxT("Bessel 1841"));
	m_ellipsoidCtrl->Append(wxT("Bessel Modified"));
	m_ellipsoidCtrl->Append(wxT("Clarke 1866"));
	m_ellipsoidCtrl->Append(wxT("Clarke 1880"));
	m_ellipsoidCtrl->Append(wxT("Everest"));
	m_ellipsoidCtrl->Append(wxT("GRS 1967 Modified"));
	m_ellipsoidCtrl->Append(wxT("GRS 1980"));
	m_ellipsoidCtrl->Append(wxT("International 1909"));
	m_ellipsoidCtrl->Append(wxT("Krassovsky"));
	m_ellipsoidCtrl->Append(wxT("Sphere"));
	m_ellipsoidCtrl->Append(wxT("WGS 72"));
	m_ellipsoidCtrl->Append(wxT("WGS 84"));
   SetUpToolTip (m_ellipsoidCtrl, IDS_ToolTip299);
	bSizer102->Add (m_ellipsoidCtrl, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	m_staticText303 = new wxStaticText (this,
													IDC_EllipsoidName,
													wxEmptyString,
													wxDefaultPosition,
													wxDefaultSize,
													0);
	m_staticText303->Wrap (-1);
   SetUpToolTip (m_staticText303, IDS_ToolTip299);
	bSizer102->Add (m_staticText303,
							wxSizerFlags(0).Center().Border(wxLEFT|wxTOP|wxBOTTOM, 5));
	
	bSizer96->Add (bSizer102, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 24));
	
	wxBoxSizer* bSizer103;
	bSizer103 = new wxBoxSizer (wxHORIZONTAL);
	
	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer (wxVERTICAL);
	
	bSizer106 = new wxBoxSizer (wxHORIZONTAL);
	
	wxStaticText* m_staticText304 = new wxStaticText (this,
																		IDC_MajorAxisPrompt,
																		//wxT("Major axis: "),
																		//wxT("blank: "),
																		wxEmptyString,
																		wxDefaultPosition,
																		wxSize (165, -1),
																		0);
	m_staticText304->SetWindowStyle (wxTE_RIGHT);
	bSizer106->Add (m_staticText304,
							wxSizerFlags(0).CenterVertical().Border(wxTOP|wxRIGHT|wxBOTTOM, 2));
	
	m_textCtrl63 = new wxTextCtrl (this,
												IDC_Radius,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl63->SetValidator (doubleValue4Digits);
	m_textCtrl63->SetWindowStyle (wxTE_RIGHT);
	bSizer106->Add (m_textCtrl63, wxSizerFlags(0).Border(wxTOP|wxRIGHT|wxBOTTOM,2));
	
	bSizer104->Add (bSizer106, wxSizerFlags(0));
	
	bSizer107 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText140 = new wxStaticText (this,
													IDC_MinorAxisPrompt,
													wxEmptyString,
													//wxT("Minor axis: "),
													//wxT("blank: "),
													wxDefaultPosition,
													wxSize (165, -1),
													0);
	m_staticText140->SetWindowStyle (wxTE_RIGHT);
	bSizer107->Add (m_staticText140,
							wxSizerFlags(0).CenterVertical().Border(wxTOP|wxRIGHT, 2));
	
	m_textCtrl64 = new wxTextCtrl (this,
												IDC_MinorAxis,
												wxEmptyString,
												wxDefaultPosition,
												wxSize (120, -1),
												0);
   m_textCtrl64->SetValidator (doubleValue4Digits);
	m_textCtrl64->SetWindowStyle (wxTE_RIGHT);
	bSizer107->Add (m_textCtrl64, wxSizerFlags(0).Border(wxTOP|wxRIGHT, 2));
	
	bSizer104->Add (bSizer107, wxSizerFlags(0));
	
	
	bSizer103->Add (bSizer104,
							wxSizerFlags(0).Bottom().
											ReserveSpaceEvenIfHidden().Border(wxLEFT, 50));
	
	bSizer96->Add (bSizer103, wxSizerFlags(0).Border(wxRIGHT|wxBOTTOM,12));
	
	CreateStandardButtons (bSizer96);
	
			// Reset the text for the major & minor axis prompts so that they
			// get right justified.
	
	LoadDItemString (IDC_MajorAxisPrompt, (CharPtr)"Major axis: ");
	LoadDItemString (IDC_MinorAxisPrompt, (CharPtr)"Minor axis: ");
	
	SetSizerAndFit (bSizer96);
	
}	// end "CreateControls"



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

	returnCode = ShowModal ();

	if (returnCode == wxID_OK) 
		{
		OKFlag = TRUE;

		m_gridZoneDirectionString[0] = 1;

		if (m_gridZoneDirection.Length() > 0)
			m_gridZoneDirectionString[1] = m_gridZoneDirection[0];
			
		else	// m_gridZoneDirection.Length() <= 0
			m_gridZoneDirectionString[1] = 0;

		m_gridZoneDirectionString[2] = 0;

		CoordinateDialogOK (this,
									 m_fileInfoPtr,
									 m_mapProjectionInfoPtr,
									 abs (m_mapUnitsSelection),
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

}// end "DoDialog"



void CMMapCoordinateDlg::AdjustDlgLayout ()

{
    bSizer97->Layout ();
    bSizer99->Layout ();
    bSizer100->Layout ();
    bSizer101->Layout ();
    bSizer102->Layout ();
	
}	// end "AdjustDlgLayout"



void CMMapCoordinateDlg::OnInitDialog (
				wxInitDialogEvent&				event)

{
	SInt16								datumSelection,
											ellipsoidSelection,
											gridZone,
											mapUnitsSelection,
											projectionSelection,
											referenceSystemSelection;


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
	m_projectionSelection = abs (projectionSelection);
	m_datumSelection = abs (datumSelection);
	m_ellipsoidSelection = abs (ellipsoidSelection);

	m_referenceSystemCode = abs (referenceSystemSelection);
	m_projectionCode = abs (m_projectionSelection);
	m_datumCode = abs (m_datumSelection);
	m_ellipsoidCode = abs (m_ellipsoidSelection);

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
	m_gridZoneDirection = CString (&m_gridZoneDirectionString[1]);

			// This is needed because m_radiusSpheroid is what is used for
			// the major axis box.

	if (abs (m_ellipsoidCode) != kSphereEllipsoidCode)
		m_radiusSpheroid = m_semiMajorAxis;

	if (TransferDataToWindow ())
		PositionDialogWindow ();

			// Set default text selection to first edit text item	

	SelectDialogItemText (this, IDC_X11Coordinate, 0, SInt16_MAX);
	
	AdjustDlgLayout ();

}	// end "OnInitDialog"



void CMMapCoordinateDlg::OnSelendokMapUnitsCombo (
				wxCommandEvent& 					event)

{

}	// end "OnSelendokMapUnitsCombo"



void CMMapCoordinateDlg::OnSelendokEllipsoidCombo (
				wxCommandEvent& 					event)
				
{
	int									lastEllipsoidSelection;


	lastEllipsoidSelection = m_ellipsoidSelection;
	m_ellipsoidSelection = m_ellipsoidCtrl->GetSelection ();

	if (lastEllipsoidSelection != m_ellipsoidSelection) 
		{
		CoordinateDialogSetEllipsoidParameters (this,
															 m_ellipsoidSelection);

		}	// end "if (lastEllipsoidSelection != m_ellipsoidSelection)"
    
	AdjustDlgLayout ();
	
}	// end "OnSelendokEllipsoidCombo"



void CMMapCoordinateDlg::OnSelendokReferenceSystemCombo (
				wxCommandEvent& 					event)

{
	int									lastReferenceSelection;

	SInt16								datumSelection,
											ellipsoidSelection,
											gridZone,
											mapUnitsSelection,
											projectionSelection;


	lastReferenceSelection = m_referenceSystemSelection;
	m_referenceSystemSelection = m_refSystemCtrl->GetSelection ();

	if (lastReferenceSelection != m_referenceSystemSelection) 
		{
		m_referenceSystemCode = abs (m_referenceSystemSelection);

		projectionSelection = m_projectionSelection;
		datumSelection = m_datumSelection;
		ellipsoidSelection = m_ellipsoidSelection;
		mapUnitsSelection = m_mapUnitsSelection;

		CoordinateDialogSetReferenceSystemParameters (this,
																		m_referenceSystemCode,
																		m_gridZoneDirectionString,
																		FALSE,
																		&projectionSelection,
																		&datumSelection,
																		&ellipsoidSelection,
																		&mapUnitsSelection);

		if (m_projectionSelection != abs (projectionSelection))
			{
			m_projectionSelection = abs (projectionSelection);
			m_projectionCtrl->SetSelection (m_projectionSelection);
			m_projectionCode = m_projectionSelection;

			}	// end "if (m_projectionSelection != projectionSelection)"

		if (m_datumSelection != abs (datumSelection))
			{
			m_datumSelection = abs (datumSelection);
			m_datumCtrl->SetSelection (m_datumSelection);

			m_datumCode = m_datumSelection;

			}	// end "if (m_datumSelection != datumSelection)"

		if (m_ellipsoidSelection != abs (ellipsoidSelection))
			{
			m_ellipsoidSelection = abs (ellipsoidSelection);
			m_ellipsoidCtrl->SetSelection (m_ellipsoidSelection);
			m_ellipsoidCode = m_ellipsoidSelection;

			}	// end "if (m_ellipsoidSelection != abs (ellipsoidSelection))"

		if (m_mapUnitsSelection != abs (mapUnitsSelection))
			{
			m_mapUnitsSelection = abs (mapUnitsSelection);
			m_mapUnitsCtrl->SetSelection (m_mapUnitsSelection);

			}	// end "if (m_mapUnitsSelection != abs (mapUnitsSelection))"

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

		if (m_projectionSelection != abs (projectionSelection))
			{
			m_projectionSelection = abs (projectionSelection);
			m_projectionCtrl->SetSelection (m_projectionSelection);

			m_projectionCode = m_projectionSelection;

			}	// end "if (m_projectionSelection != abs (projectionSelection))"

		if (m_gridZone != gridZone) 
			{
			wxTextCtrl* gridZonetc = (wxTextCtrl*)FindWindow (IDC_Zone);
			gridZonetc->ChangeValue (wxString::Format (wxT("%i"), m_gridZone));            
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

			if (m_projectionSelection != abs (projectionSelection))
				{
				m_projectionSelection = abs (projectionSelection);
				m_projectionCtrl->SetSelection (m_projectionSelection);

				m_projectionCode = m_projectionSelection;

				}	// end "if (m_projectionSelection = projectionSelection)"

			if (m_mapUnitsSelection != abs (mapUnitsSelection))
				{
				m_mapUnitsSelection = abs (mapUnitsSelection);
				m_mapUnitsCtrl->SetSelection (m_mapUnitsSelection);

				}	// end "if (m_mapUnitsSelection != abs (mapUnitsSelection)"
			
			SetUpToolTip (m_staticText116, IDS_ToolTip300);
			SetUpToolTip (m_textCtrl57, IDS_ToolTip300);
			
			}	// end "if (m_referenceSystemSelection == kByEPSGCodeCode)"
	  
		else	// m_referenceSystemSelection != kByEPSGCodeCode
			{
			SetUpToolTip (m_staticText116, IDS_ToolTip297);
			SetUpToolTip (m_textCtrl57, IDS_ToolTip297);
			
			}	// end "else m_referenceSystemSelection != kByEPSGCodeCode"

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastReferenceSelection != m_referenceSystemSelection)"
    
	AdjustDlgLayout ();
	
}	// end "OnSelendokReferenceSystemCombo"



void CMMapCoordinateDlg::OnSelendokProjectionCombo (
				wxCommandEvent& 					event)

{
	int									lastProjectionSelection;

	SInt16								datumSelection,
											ellipsoidSelection;


	lastProjectionSelection = m_projectionSelection;
	m_projectionSelection = m_projectionCtrl->GetSelection ();

	if (lastProjectionSelection != m_projectionSelection) 
		{
		m_projectionCode = abs (m_projectionSelection);

		datumSelection = m_datumSelection;
		ellipsoidSelection = m_ellipsoidSelection;

		CoordinateDialogHideShowProjectionParameters (this,
																	 m_referenceSystemCode,
																	 m_projectionCode,
																	 FALSE,
																	 &datumSelection,
																	 &ellipsoidSelection,
																	 FALSE);

		if (m_datumSelection != abs (datumSelection))
			{
			m_datumSelection = abs (datumSelection);
			m_datumCtrl->SetSelection (m_datumSelection);
			m_datumCode = m_datumSelection;

			}	// end "if (m_datumSelection != datumSelection)"

		if (m_ellipsoidSelection != ellipsoidSelection) 
			{
			m_ellipsoidSelection = abs (ellipsoidSelection);
			m_ellipsoidCtrl->SetSelection (m_ellipsoidSelection);
			m_ellipsoidCode = m_ellipsoidSelection;

			}	// end "if (m_ellipsoidSelection != ellipsoidSelection)"

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastGridSystemCode != m_gridSystemCode)"
	
}	// end "OnSelendokProjectionCombo"


void CMMapCoordinateDlg::OnSelendokDatumCombo (
				wxCommandEvent& 					event)

{
	int									lastDatumSelection;


	lastDatumSelection = m_datumSelection;
	m_datumSelection = m_datumCtrl->GetSelection ();

	if (lastDatumSelection != m_datumSelection) 
		{
		m_ellipsoidSelection = CoordinateDialogSetDatumParameters (
																			 this,
																			 m_datumSelection,
																			 abs (m_ellipsoidSelection),
																			 FALSE,
																			 FALSE);

		m_ellipsoidSelection = abs (m_ellipsoidSelection);
		m_ellipsoidCode = m_ellipsoidSelection;

		m_ellipsoidCtrl->SetSelection (m_ellipsoidSelection);
		m_datumCode = abs (m_datumSelection);

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastDatumSelection != m_datumSelection)"

	AdjustDlgLayout ();
	
}	// end "OnSelendokDatumCombo"


void CMMapCoordinateDlg::OnChangeZoneDirection (
				wxCommandEvent& 					event)

{
	SInt16								gridZone,
											projectionSelection;


	//char lastGridZoneDirection = m_gridZoneDirection[0];

	m_gridZoneDirection = event.GetString ();
	m_gridZoneDirection = m_gridZoneDirection.Upper ();
    
	wxTextCtrl* gridZoneDirection = (wxTextCtrl*)FindWindow (IDC_ZoneDirection);
	gridZoneDirection->ChangeValue (m_gridZoneDirection);
	 
	m_datumCode = abs (m_datumSelection);

			// Set up gridZoneDirectionString to be used in the shared routines.

	m_gridZoneDirectionString[1] = m_gridZoneDirection[0];

	m_gridZoneWithDirection = m_gridZone;

	if (m_gridZoneDirection.IsSameAs ('S',false))
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

	if (m_projectionSelection != abs (projectionSelection))
		{
		m_projectionSelection = abs (projectionSelection);
		m_projectionCtrl->SetSelection (m_projectionSelection);
		
		}	// end "if (m_projectionSelection = projectionSelection)"
	
}	// end "OnChangeZoneDirection"


void CMMapCoordinateDlg::OnEnChangeZone (
				wxCommandEvent& 					event)

{

	SInt16								gridZone,
											maxZone,
											minZone,
											projectionSelection,
											savedGridZone;


	savedGridZone = m_gridZone;
	wxTextCtrl* gridZonetext = (wxTextCtrl*)FindWindow (IDC_Zone);
	m_gridZone = wxAtoi (gridZonetext->GetValue ());

	CoordinateDialogGetMinMaxZone (m_referenceSystemCode,
												&minZone,
												&maxZone,
												m_gridZoneDirectionString);

	if (m_gridZone >= minZone && m_gridZone <= maxZone) 
		{
		m_gridZoneWithDirection = m_gridZone;

		if (m_gridZoneDirection.IsSameAs ('S',false))
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

		if (m_projectionSelection != abs (projectionSelection))
			{
			m_projectionSelection = abs (projectionSelection);
			m_projectionCtrl->SetSelection (m_projectionSelection);
			
			}	// end "if (m_projectionSelection = projectionSelection)"

		}	// end "else grid zone is within the proper range
	
}	// end "OnEnChangeZone"



void CMMapCoordinateDlg::OnEnChangeEPSGCode (
				wxCommandEvent& 					event)

{
	SInt16								mapUnitsSelection,
											projectionSelection,
											savedEPSGCode;


	savedEPSGCode = m_epsgCode;
	wxTextCtrl* epsgCode = (wxTextCtrl*)FindWindow (IDC_EPSGCode);
	m_epsgCode = wxAtoi (epsgCode->GetValue ());

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

		if (m_projectionSelection != abs (projectionSelection))
			{
			m_projectionSelection = abs (projectionSelection);
			m_projectionCtrl->SetSelection (m_projectionSelection);
			
			}	// end "if (m_projectionSelection = projectionSelection)"

		if (m_mapUnitsSelection != abs (mapUnitsSelection))
			{
			m_mapUnitsSelection = abs (mapUnitsSelection);
			m_mapUnitsCtrl->SetSelection (m_mapUnitsSelection);
			
			}	// end "if (m_mapUnitsSelection != abs (mapUnitsSelection)"

		}	// end "else grid zone is within the proper range
	
	AdjustDlgLayout ();
	
}	// end "OnEnChangeEPSGCode"



bool CMMapCoordinateDlg::TransferDataFromWindow ()

{
	m_referenceSystemSelection = m_refSystemCtrl->GetSelection ();
	
	m_ellipsoidSelection = m_ellipsoidCtrl->GetSelection ();
	
	m_projectionSelection = m_projectionCtrl->GetSelection ();
	
	m_mapUnitsSelection = m_mapUnitsCtrl->GetSelection ();
	
	m_datumSelection = m_datumCtrl->GetSelection ();

	wxTextCtrl* gridZone = (wxTextCtrl*)FindWindow (IDC_Zone);
	m_gridZone = wxAtoi (gridZone->GetValue());
	
	wxTextCtrl* epsgCode = (wxTextCtrl*)FindWindow (IDC_EPSGCode);
	m_epsgCode = wxAtoi (epsgCode->GetValue());

	wxTextCtrl* gridZoneDirection = (wxTextCtrl*)FindWindow (IDC_ZoneDirection);
	m_gridZoneDirection = gridZoneDirection->GetValue ();

	wxTextCtrl* horizontalPixelSize = (wxTextCtrl*)FindWindow (IDC_HorizontalSize);
	wxString horizontalPixelSize_str = horizontalPixelSize->GetValue ();
	horizontalPixelSize_str.ToDouble (&m_horizontalPixelSize);
	
	wxTextCtrl* verticalPixelSize = (wxTextCtrl*)FindWindow (IDC_VerticalSize);
	wxString verticalPixelSize_str = verticalPixelSize->GetValue ();
	verticalPixelSize_str.ToDouble (&m_verticalPixelSize);
	
	wxTextCtrl* xMapCoordinate11 = (wxTextCtrl*)FindWindow (IDC_X11Coordinate);
	wxString xMapCoordinate11_str = xMapCoordinate11->GetValue ();
	xMapCoordinate11_str.ToDouble (&m_xMapCoordinate11);
	
	wxTextCtrl* yMapCoordinate11 = (wxTextCtrl*)FindWindow (IDC_Y11Coordinate);
	wxString yMapCoordinate11_str = yMapCoordinate11->GetValue ();
	yMapCoordinate11_str.ToDouble (&m_yMapCoordinate11);
	
	wxTextCtrl* falseEasting = (wxTextCtrl*)FindWindow (IDC_FalseEasting);
	wxString falseEasting_str = falseEasting->GetValue ();
	falseEasting_str.ToDouble (&m_falseEasting);
	
	wxTextCtrl* falseNorthing = (wxTextCtrl*)FindWindow (IDC_FalseNorthing);
	wxString falseNorthing_str = falseNorthing->GetValue ();
	falseNorthing_str.ToDouble (&m_falseNorthing);
	
	wxTextCtrl* latitudeOrigin = (wxTextCtrl*)FindWindow (IDC_Latitude);
	wxString latitudeOrigin_str = latitudeOrigin->GetValue ();
	latitudeOrigin_str.ToDouble (&m_latitudeOrigin);
	
	wxTextCtrl* longitudeCentralMeridian = (wxTextCtrl*)FindWindow (IDC_Longitude);
	wxString longitudeCentralMeridian_str = longitudeCentralMeridian->GetValue ();
	longitudeCentralMeridian_str.ToDouble (&m_longitudeCentralMeridian);
	
	wxTextCtrl* semiMinorAxis = (wxTextCtrl*)FindWindow (IDC_MinorAxis);
	wxString semiMinorAxis_str = semiMinorAxis->GetValue ();
	semiMinorAxis_str.ToDouble (&m_semiMinorAxis);
	
	wxTextCtrl* radiusSpheroid = (wxTextCtrl*)FindWindow (IDC_Radius);
	wxString radiusSpheroid_str = radiusSpheroid->GetValue ();
	radiusSpheroid_str.ToDouble (&m_radiusSpheroid);
	
	wxTextCtrl* scaleFactorOfCentralMeridian =
													(wxTextCtrl*)FindWindow (IDC_ScaleFactor);
	wxString scaleFactorOfCentralMeridian_str =
													scaleFactorOfCentralMeridian->GetValue ();
	scaleFactorOfCentralMeridian_str.ToDouble (&m_scaleFactorOfCentralMeridian);
	
	wxTextCtrl* standardParallel1 = (wxTextCtrl*)FindWindow (IDC_StandardParallel1);
	wxString standardParallel1_str = standardParallel1->GetValue ();
	standardParallel1_str.ToDouble (&m_standardParallel1);
	
	wxTextCtrl* standardParallel2 = (wxTextCtrl*)FindWindow (IDC_StandardParallel2);
	wxString standardParallel2_str = standardParallel2->GetValue ();
	standardParallel2_str.ToDouble (&m_standardParallel2);

	wxTextCtrl* mapOrientationAngle = (wxTextCtrl*)FindWindow (IDC_OrientationAngle);
	wxString mapOrientationAngle_str = mapOrientationAngle->GetValue ();
	mapOrientationAngle_str.ToDouble (&m_mapOrientationAngle);
	
			// This is needed for the "CoordinateDialogOK" call.
	
	m_semiMajorAxis = m_radiusSpheroid;

	m_referenceSystemCode = abs (m_referenceSystemSelection);
	m_projectionCode = abs (m_projectionSelection);
	m_datumCode = abs (m_datumSelection);
	m_ellipsoidCode = abs (m_ellipsoidSelection);

			// Verify that the grid zone parameter is within range if it is being used.

	if (!CoordinateDialogCheckIfZoneIsValid (this,
															m_referenceSystemCode,
															m_gridZoneDirectionString))
		{
		FindWindow(IDC_Zone)->SetFocus ();
																						return false;

		}	// end "if (!CoordinateDialogCheckIfZoneIsValid (dialogPtr, ..."

	return true;
	 
}	// end "TransferDataFromWindow"



bool CMMapCoordinateDlg::TransferDataToWindow ()

{
	wxTextCtrl* gridZone = (wxTextCtrl*)FindWindow (IDC_Zone);
	gridZone->ChangeValue (wxString::Format (wxT("%i"), m_gridZone));
	
	wxTextCtrl* epsgCode = (wxTextCtrl*)FindWindow (IDC_EPSGCode);
	epsgCode->ChangeValue (wxString::Format (wxT("%i"), m_epsgCode));

	if (abs(m_mapUnitsSelection) == kDecimalDegreesCode)
		{
		m_textCtrl54->ChangeValue (
										wxString::Format (wxT("%.9f"), m_horizontalPixelSize));
	
		m_textCtrl55->ChangeValue (
										wxString::Format (wxT("%.9f"), m_verticalPixelSize));
										
		}	//	end "if (m_mapUnitsSelection == kDecimalDegreesCode)"
		
	else	// m_mapUnitsSelection != kDecimalDegreesCode
		{
		m_textCtrl54->ChangeValue (
										wxString::Format (wxT("%.4f"), m_horizontalPixelSize));
	
		m_textCtrl55->ChangeValue (
										wxString::Format (wxT("%.4f"), m_verticalPixelSize));
										
		}	// else	// m_mapUnitsSelection != kDecimalDegreesCode
	
	wxTextCtrl* xMapCoordinate11 = (wxTextCtrl*)FindWindow (IDC_X11Coordinate);
	xMapCoordinate11->ChangeValue (wxString::Format (wxT("%.4f"), m_xMapCoordinate11));
	
	wxTextCtrl* yMapCoordinate11 = (wxTextCtrl*)FindWindow (IDC_Y11Coordinate);
	yMapCoordinate11->ChangeValue (wxString::Format (wxT("%.4f"), m_yMapCoordinate11));
	
	wxTextCtrl* falseEasting = (wxTextCtrl*)FindWindow (IDC_FalseEasting);
	falseEasting->ChangeValue (wxString::Format (wxT("%.2f"), m_falseEasting));
	
	wxTextCtrl* falseNorthing = (wxTextCtrl*)FindWindow (IDC_FalseNorthing);
	falseNorthing->ChangeValue (wxString::Format (wxT("%.2f"), m_falseNorthing));
	
	wxTextCtrl* latitudeOrigin = (wxTextCtrl*)FindWindow (IDC_Latitude);
	latitudeOrigin->ChangeValue (wxString::Format (wxT("%.6f"), m_latitudeOrigin));
	
	wxTextCtrl* longitudeCentralMeridian = (wxTextCtrl*)FindWindow (IDC_Longitude);
	longitudeCentralMeridian->ChangeValue (
								wxString::Format (wxT("%.6f"), m_longitudeCentralMeridian));
	
	wxTextCtrl* semiMinorAxis = (wxTextCtrl*)FindWindow (IDC_MinorAxis);
	semiMinorAxis->ChangeValue (wxString::Format (wxT("%.4f"), m_semiMinorAxis));
	
	wxTextCtrl* radiusSpheroid = (wxTextCtrl*)FindWindow (IDC_Radius);
	radiusSpheroid->ChangeValue (wxString::Format (wxT("%.4f"), m_radiusSpheroid));
	
	wxTextCtrl* scaleFactorOfCentralMeridian = (wxTextCtrl*)FindWindow (IDC_ScaleFactor);
	scaleFactorOfCentralMeridian->ChangeValue (
							wxString::Format (wxT("%.6f"), m_scaleFactorOfCentralMeridian));
	
	wxTextCtrl* standardParallel1 = (wxTextCtrl*)FindWindow (IDC_StandardParallel1);
	standardParallel1->ChangeValue (
										wxString::Format (wxT("%.6f"), m_standardParallel1));
	
	wxTextCtrl* standardParallel2 = (wxTextCtrl*)FindWindow (IDC_StandardParallel2);
	standardParallel2->ChangeValue (
										wxString::Format (wxT("%.6f"), m_standardParallel2));
	
	wxTextCtrl* gridZoneDirection = (wxTextCtrl*)FindWindow (IDC_ZoneDirection);
	gridZoneDirection->ChangeValue (m_gridZoneDirection);
	
	wxTextCtrl* mapOrientationAngle = (wxTextCtrl*)FindWindow (IDC_OrientationAngle);
	mapOrientationAngle->ChangeValue (
										wxString::Format (wxT("%.6f"), m_mapOrientationAngle));

	m_refSystemCtrl->SetSelection (m_referenceSystemSelection);
	
	m_ellipsoidCtrl->SetSelection (m_ellipsoidSelection);
	
	m_projectionCtrl->SetSelection (m_projectionSelection);
	
	m_mapUnitsCtrl->SetSelection (abs (m_mapUnitsSelection));
	
	m_datumCtrl->SetSelection (m_datumSelection);
	
	return true;
	 
}	// end "TransferDataToWindow"
