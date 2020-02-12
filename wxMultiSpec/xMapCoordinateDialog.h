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
//	File:						xMapCoordinateDialog.h
//	Implementation:		xMapCoordinateDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMMapCoordinateDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMMapCoordinateDlg : public CMDialog
{
		// Construction
	public:
				// standard constructor
		CMMapCoordinateDlg (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
													wxT("Set Map Coordinate Specifications"));

		Boolean DoDialog (
				FileInfoPtr 						fileInfoPtr,
				MapProjectionInfoPtr 			mapProjectionInfoPtr);


		enum { IDD = IDD_MapCoordinates };

		wxString 							m_gridZoneDirection;
	
		double								m_horizontalPixelSize,
												m_falseEasting,
												m_falseNorthing,
												m_latitudeOrigin,
												m_longitudeCentralMeridian,
												m_mapOrientationAngle,
												m_radiusSpheroid,
												m_scaleFactorOfCentralMeridian,
												m_semiMinorAxis,
												m_standardParallel1,
												m_standardParallel2,
												m_verticalPixelSize,
												m_xMapCoordinate11,
												m_yMapCoordinate11;
	
		int 									m_datumSelection,
												m_ellipsoidSelection,
												m_gridZone,
												m_mapUnitsSelection,
												m_projectionSelection,
												m_referenceSystemSelection;


				// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnChangeZoneDirection (
				wxCommandEvent& 					event);
	
		void OnEnChangeEPSGCode (
				wxCommandEvent& 					event);
	
		void OnEnChangeZone (
	  			wxCommandEvent& 					event);
	
		void OnSelendokDatumCombo (
				wxCommandEvent& 					event);
	
		void OnSelendokEllipsoidCombo (
				wxCommandEvent& 					event);
	
		void OnSelendokMapUnitsCombo (
				wxCommandEvent& 					event);
	
		void OnSelendokProjectionCombo (
				wxCommandEvent& 					event);
	
		void OnSelendokReferenceSystemCombo (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void AdjustDlgLayout ();
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		DoublePoint							m_upperLeftLatitudeLongitudePoint;

		UInt8									m_datumName[64],
												m_ellipsoidName[64],
												m_epsgName[64],
												m_gridZoneDirectionString[4];

		double								m_semiMajorAxis;

		FileInfoPtr							m_fileInfoPtr;
		MapProjectionInfoPtr				m_mapProjectionInfoPtr;

		wxBoxSizer							*bSizer96,
												*bSizer97,
												*bSizer99,
												*bSizer100,
												*bSizer101,
												*bSizer102,
												*bSizer106,
												*bSizer107;
	
		wxChoice								*m_datumCtrl,
												*m_ellipsoidCtrl,
												*m_mapUnitsCtrl,
												*m_projectionCtrl,
												*m_refSystemCtrl;

		wxStaticText						*m_staticText107,
												*m_staticText108,
												*m_staticText109,
												*m_staticText110,
												*m_staticText111,
												*m_staticText112,
												*m_staticText113,
												*m_staticText115,
												*m_staticText116,
												*m_staticText119,
												*m_staticText121,
												*m_staticText123,
												*m_staticText125,
												*m_staticText127,
												*m_staticText129,
												*m_staticText133,
												*m_staticText136,
												*m_staticText139,
												*m_staticText140,
												*m_staticText142,
												*m_staticText143,
												*m_staticText301,
												*m_staticText302,
												*m_staticText303,
												*m_staticText306,
												*m_staticText307;

		wxTextCtrl							*m_textCtrl52,
												*m_textCtrl53,
												*m_textCtrl54,
												*m_textCtrl55,
												*m_textCtrl56,
												*m_textCtrl57,
												*m_textCtrl58,
												*m_textCtrl59,
												*m_textCtrl60,
												*m_textCtrl61,
												*m_textCtrl62,
												*m_textCtrl63,
												*m_textCtrl64,
												*m_textCtrl65,
												*m_textCtrl66,
												*m_textCtrl67,
												*m_textCtrl181;

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
	
};	// end "class CMainFrame"

