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
//	File:						WMapCoordinateDialog.h
//	Implementation:		WMapCoordinateDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMMapCoordinateDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"
#include "afxwin.h"


class CMMapCoordinateDlg : public CMDialog
{
	// Construction
	public:
		CMMapCoordinateDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr);

		// Dialog Data
		//{{AFX_DATA (CMMapCoordinateDlg)
		enum { IDD = IDD_MapCoordinates };
	
		CString								m_gridZoneDirection;
	
		double								m_falseEasting,
												m_falseNorthing,
												m_horizontalPixelSize,
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
	
		int									m_datumSelection,
												m_ellipsoidSelection,
												m_gridZone,
												m_mapUnitsSelection,
												m_projectionSelection,
												m_referenceSystemSelection;
		//}}AFX_DATA


	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
	
		// Generated message map functions
		//{{AFX_MSG (CMMapCoordinateDlg)
		afx_msg void OnCbnSelchangeDatumcombo ();
	
		afx_msg void OnChangeZoneDirection ();
	
		afx_msg void OnEnChangeZone ();
	
		afx_msg void OnEnChangeEPSGCode ();
	
		afx_msg void OnSelendokDatumCombo ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokEllipsoidCombo ();
	
		afx_msg void OnSelendokMapUnitsCombo ();
	
		afx_msg void OnSelendokProjectionCombo ();
	
		afx_msg void OnSelendokReferenceSystemCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()

		DoublePoint							m_upperLeftLatitudeLongitudePoint;

		UInt8									m_datumName[64],
												m_ellipsoidName[64],
												m_epsgName[64],
												m_gridZoneDirectionString[4];
	
		FileInfoPtr							m_fileInfoPtr;
		MapProjectionInfoPtr				m_mapProjectionInfoPtr;

		double								m_semiMajorAxis;

		SInt16								m_areaUnitsCode,
												m_datumCode,
												m_ellipsoidCode,
												m_epsgCode,
												m_gridZoneWithDirection,
												m_projectionCode,
												m_referenceSystemCode,
												m_zoneProjectionCode;
	
		Boolean								m_adjustUpperLeftMapPointFlag,
												m_initializedFlag,
												m_validEPSGCodeFlag;
	
};	// end class CMMapCoordinateDlg
