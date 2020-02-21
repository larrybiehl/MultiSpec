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
//	File:						WMapCoordinateDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/24/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMMapCoordinateDlg class.
//
// Note that m_adjustUpperLeftMapPointFlag has not been fully integrated in this code
// yet. See Mac version for more info. Is not being used right now till a better
// method is made available for the user to control.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WMapCoordinateDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMMapCoordinateDlg, CMDialog)
	//{{AFX_MSG_MAP (CMMapCoordinateDlg)
	ON_CBN_SELENDOK (IDC_DatumCombo, OnSelendokDatumCombo)
	ON_CBN_SELENDOK (IDC_EllipsoidCombo, OnSelendokEllipsoidCombo)
	ON_CBN_SELENDOK (IDC_MapUnitsCombo, OnSelendokMapUnitsCombo)
	ON_CBN_SELENDOK (IDC_ProjectionCombo, OnSelendokProjectionCombo)
	ON_CBN_SELENDOK (IDC_ReferenceSystemCombo, OnSelendokReferenceSystemCombo)

	ON_EN_CHANGE (IDC_EPSGCode, OnEnChangeEPSGCode)
	ON_EN_CHANGE (IDC_Zone, OnEnChangeZone)
	ON_EN_CHANGE (IDC_ZoneDirection, OnChangeZoneDirection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMMapCoordinateDlg::CMMapCoordinateDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMMapCoordinateDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMMapCoordinateDlg)
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
	//}}AFX_DATA_INIT

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
	
}	// end "CMMapCoordinateDlg"



void CMMapCoordinateDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	USES_CONVERSION;
	

	CMDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMMapCoordinateDlg)
	DDX_CBIndex (pDX, IDC_ReferenceSystemCombo, m_referenceSystemSelection);
	DDX_CBIndex (pDX, IDC_EllipsoidCombo, m_ellipsoidSelection);
	DDX_CBIndex (pDX, IDC_ProjectionCombo, m_projectionSelection);
	DDX_CBIndex (pDX, IDC_MapUnitsCombo, m_mapUnitsSelection);
	DDX_Text (pDX, IDC_Zone, m_gridZone);
	DDX_Text (pDX, IDC_EPSGCode, m_epsgCode);
	DDX_Text2 (pDX, IDC_HorizontalSize, m_horizontalPixelSize);
	DDX_Text2 (pDX, IDC_VerticalSize, m_verticalPixelSize);
	DDX_Text2 (pDX, IDC_X11Coordinate, m_xMapCoordinate11);
	DDX_Text2 (pDX, IDC_Y11Coordinate, m_yMapCoordinate11);
	DDX_Text2 (pDX, IDC_FalseEasting, m_falseEasting);
	DDX_Text2 (pDX, IDC_FalseNorthing, m_falseNorthing);
	DDX_Text2 (pDX, IDC_Latitude, m_latitudeOrigin);
	DDX_Text2 (pDX, IDC_Longitude, m_longitudeCentralMeridian);
	DDX_Text2 (pDX, IDC_MinorAxis, m_semiMinorAxis);
	DDX_Text2 (pDX, IDC_Radius, m_radiusSpheroid);
	DDX_Text2 (pDX, IDC_ScaleFactor, m_scaleFactorOfCentralMeridian);
	DDX_Text2 (pDX, IDC_StandardParallel1, m_standardParallel1);
	DDX_Text2 (pDX, IDC_StandardParallel2, m_standardParallel2);
	DDX_Text (pDX, IDC_ZoneDirection, m_gridZoneDirection);
	DDV_MaxChars (pDX, m_gridZoneDirection, 1);
	DDX_CBIndex (pDX, IDC_DatumCombo, m_datumSelection);
	DDX_Text2 (pDX, IDC_OrientationAngle, m_mapOrientationAngle);
	DDV_MinMaxDouble (pDX, m_mapOrientationAngle, -180., 180.);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
		{
		/*
		CComboBox*				comboBoxPtr;

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ReferenceSystemCombo);
		m_referenceSystemCode =
							(SInt16)comboBoxPtr->GetItemData (m_referenceSystemSelection);

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_ProjectionCombo);
		m_projectionCode = (SInt16)comboBoxPtr->GetItemData (m_projectionSelection);

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_DatumCombo);
		m_datumCode = (SInt16)comboBoxPtr->GetItemData (m_datumSelection);

		comboBoxPtr = (CComboBox*)GetDlgItem (IDC_EllipsoidCombo);
		m_ellipsoidCode = (SInt16)comboBoxPtr->GetItemData (m_ellipsoidSelection);
		*/
		m_referenceSystemCode = abs (m_referenceSystemSelection);
		m_projectionCode = abs (m_projectionSelection);
		m_datumCode = abs (m_datumSelection);
		m_ellipsoidCode = abs (m_ellipsoidSelection);

				// Verify that the grid zone parameter is within range if it is being
				// used.
				
		if (!CoordinateDialogCheckIfZoneIsValid (this,
																m_referenceSystemCode,
																m_gridZoneDirectionString))
			{			
			pDX->PrepareEditCtrl (IDC_Zone);
			pDX->Fail ();

			}	// end "if (!CoordinateDialogCheckIfZoneIsValid (dialogPtr, ..."

		}	// end "if (pDX->m_bSaveAndValidate)"

}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the map coordinates
//							dialog box to the user so that the user can change map settings.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2000
//	Revised By:			Larry L. Biehl			Date: 04/05/2004	

Boolean CMMapCoordinateDlg::DoDialog (
				FileInfoPtr							fileInfoPtr,
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{                              
	USES_CONVERSION;
	
	INT_PTR								returnCode;
	
	Boolean								OKFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
	
	m_fileInfoPtr = fileInfoPtr;
	m_mapProjectionInfoPtr = mapProjectionInfoPtr;
																				            																			
	returnCode = (SInt16)DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE; 
		
		m_gridZoneDirectionString[0] = 1;

		if (m_gridZoneDirection.GetLength () > 0)
			m_gridZoneDirectionString[1] = (UInt8)m_gridZoneDirection[0];
		else    // m_gridZoneDirection.GetLength () <= 0
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
		
}	// end "DoDialog"



void CMMapCoordinateDlg::OnChangeZoneDirection ()

{
			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CMDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

	USES_CONVERSION;
	
	SInt16								projectionSelection;

	
	TBYTE lastGridZoneDirection = m_gridZoneDirection[0];
	
	DDX_Text (m_dialogFromPtr, IDC_ZoneDirection, m_gridZoneDirection);

	if (m_gridZoneDirection != 'N' && m_gridZoneDirection != 'S' &&
			m_gridZoneDirection != 'n' && m_gridZoneDirection != 's')
		{
		m_gridZoneDirection = lastGridZoneDirection;
		DDX_Text (m_dialogToPtr, IDC_ZoneDirection, m_gridZoneDirection);

		}	// end "if (m_gridZoneDirection != 'N' && m_gridZoneDirection != 'S')"

	else	// okay
		{
		SInt16					gridZone;

		m_datumCode = abs (m_datumSelection);
		
				// Set up gridZoneDirectionString to be used in the shared routines.

		m_gridZoneDirectionString[1] = (UInt8)m_gridZoneDirection[0];

		m_gridZoneWithDirection = m_gridZone;
		if (m_gridZoneDirection == 'S')
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
			DDX_CBIndex (m_dialogToPtr, IDC_ProjectionCombo, m_projectionSelection);

			}	// end "if (m_projectionSelection = projectionSelection)"

		}	// end "else okay"
	
}	// end "OnChangeZoneDirection"



void CMMapCoordinateDlg::OnEnChangeZone ()

{
			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CMDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

	USES_CONVERSION;

	SInt16								gridZone,
											maxZone,
											minZone,
											projectionSelection,
											savedGridZone;


	savedGridZone = m_gridZone;
	DDX_Text (m_dialogFromPtr, IDC_Zone, m_gridZone);
	
	CoordinateDialogGetMinMaxZone (m_referenceSystemCode,
												&minZone,
												&maxZone,
												m_gridZoneDirectionString);
		
	if (m_gridZone >= minZone && m_gridZone <= maxZone)
		{
		m_gridZoneWithDirection = m_gridZone;
		if (m_gridZoneDirection == 'S')
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
			DDX_CBIndex (m_dialogToPtr, IDC_ProjectionCombo, m_projectionSelection);

			}	// end "if (m_projectionSelection = projectionSelection)"
																
		}	// end "else grid zone is within the proper range

}	// end "OnEnChangeZone"



void CMMapCoordinateDlg::OnEnChangeEPSGCode ()

{
			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CMDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

	USES_CONVERSION;
	
	SInt16								mapUnitsSelection,
											projectionSelection,
											savedEPSGCode;

	
	savedEPSGCode = m_epsgCode;
	DDX_Text (m_dialogFromPtr, IDC_EPSGCode, m_epsgCode);
	
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
			DDX_CBIndex (m_dialogToPtr, IDC_ProjectionCombo, m_projectionSelection);

			}	// end "if (m_projectionSelection = projectionSelection)"
	
		if (m_mapUnitsSelection != abs (mapUnitsSelection))
			{
			m_mapUnitsSelection = abs (mapUnitsSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_MapUnitsCombo, m_mapUnitsSelection);

			}	// end "if (m_mapUnitsSelection != abs (mapUnitsSelection)"
																
		}	// end "else grid zone is within the proper range

}	// end "OnEnChangeEPSGCode"



BOOL CMMapCoordinateDlg::OnInitDialog ()

{
	SInt16								datumSelection,
											ellipsoidSelection,
											gridZone,
											mapUnitsSelection,
											projectionSelection,
											referenceSystemSelection;


	CDialog::OnInitDialog ();
	
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
											(UCharPtr)&m_gridZoneDirectionString,
											&m_gridZoneWithDirection,
											(UCharPtr)&m_epsgName,
											(UCharPtr)&m_datumName,
											(UCharPtr)&m_ellipsoidName,
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

	m_gridZoneDirection = CString (&m_gridZoneDirectionString[1]);

			// This is needed because m_radiusSpheroid is what is used for
			// the major axis box.

	if (abs (m_ellipsoidCode) != kSphereEllipsoidCode)
		m_radiusSpheroid = m_semiMajorAxis;
	                 
	if (UpdateData (FALSE))                   
		PositionDialogWindow ();
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_X11Coordinate, 0, SInt16_MAX);        
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	              
}	// end "OnInitDialog"



void CMMapCoordinateDlg::OnSelendokMapUnitsCombo ()

{
			// Add your control notification handler code here
	
}



void CMMapCoordinateDlg::OnSelendokEllipsoidCombo ()

{
	int						lastEllipsoidSelection;


	lastEllipsoidSelection = m_ellipsoidSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_EllipsoidCombo, m_ellipsoidSelection);

	if (lastEllipsoidSelection != m_ellipsoidSelection)
		{
		CoordinateDialogSetEllipsoidParameters (this,
																m_ellipsoidSelection);

		}	// end "if (lastEllipsoidSelection != m_ellipsoidSelection)"
	
}	// end "OnSelendokEllipsoidCombo"



void CMMapCoordinateDlg::OnSelendokReferenceSystemCombo ()

{
	USES_CONVERSION;
	
	int									lastReferenceSelection;

	SInt16								datumSelection,
											ellipsoidSelection,
											gridZone,
											mapUnitsSelection,
											projectionSelection;

	
	lastReferenceSelection = m_referenceSystemSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_ReferenceSystemCombo, m_referenceSystemSelection);

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
			DDX_CBIndex (m_dialogToPtr, IDC_ProjectionCombo, m_projectionSelection);

			m_projectionCode = m_projectionSelection;

			}	// end "if (m_projectionSelection != projectionSelection)"
		
		if (m_datumSelection != abs (datumSelection))
			{
			m_datumSelection = abs (datumSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_DatumCombo, m_datumSelection);

			m_datumCode = m_datumSelection;

			}	// end "if (m_datumSelection != datumSelection)"

		if (m_ellipsoidSelection != abs (ellipsoidSelection))
			{
			m_ellipsoidSelection = abs (ellipsoidSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_EllipsoidCombo, m_ellipsoidSelection);

			m_ellipsoidCode = m_ellipsoidSelection;

			}	// end "if (m_ellipsoidSelection != abs (ellipsoidSelection))"

		if (m_mapUnitsSelection != abs (mapUnitsSelection))
			{
			m_mapUnitsSelection = abs (mapUnitsSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_MapUnitsCombo, m_mapUnitsSelection);

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
															&gridZone);
	
		if (m_projectionSelection != abs (projectionSelection))
			{
			m_projectionSelection = abs (projectionSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_ProjectionCombo, m_projectionSelection);

			m_projectionCode = m_projectionSelection;

			}	// end "if (m_projectionSelection != abs (projectionSelection))"
		
		if (m_gridZone != gridZone)
			DDX_Text (m_dialogToPtr, IDC_Zone, m_gridZone);
																			
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
				DDX_CBIndex (m_dialogToPtr, IDC_ProjectionCombo, m_projectionSelection);

				m_projectionCode = m_projectionSelection;

				}	// end "if (m_projectionSelection = projectionSelection)"
	
			if (m_mapUnitsSelection != abs (mapUnitsSelection))
				{
				m_mapUnitsSelection = abs (mapUnitsSelection);
				DDX_CBIndex (m_dialogToPtr, IDC_MapUnitsCombo, m_mapUnitsSelection);

				}	// end "if (m_mapUnitsSelection != abs (mapUnitsSelection)"

			}	// end "if (m_referenceSystemSelection == kByEPSGCodeCode)"

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastReferenceSelection != m_referenceSystemSelection)"
	
}	// end "OnSelendokReferenceSystemCombo"



void CMMapCoordinateDlg::OnSelendokProjectionCombo ()

{
	int									lastProjectionSelection;

	SInt16								datumSelection,
											ellipsoidSelection;


	lastProjectionSelection = m_projectionSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_ProjectionCombo, m_projectionSelection);

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
			DDX_CBIndex (m_dialogToPtr, IDC_DatumCombo, m_datumSelection);

			m_datumCode = m_datumSelection;

			}	// end "if (m_datumSelection != datumSelection)"

		if (m_ellipsoidSelection != ellipsoidSelection)
			{
			m_ellipsoidSelection = abs (ellipsoidSelection);
			DDX_CBIndex (m_dialogToPtr, IDC_EllipsoidCombo, m_ellipsoidSelection);

			m_ellipsoidCode = m_ellipsoidSelection;

			}	// end "if (m_ellipsoidSelection != ellipsoidSelection)"

		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastGridSystemCode != m_gridSystemCode)"
	
}	// end "OnSelendokProjectionCombo"



void CMMapCoordinateDlg::OnSelendokDatumCombo ()

{
	int									lastDatumSelection;


	lastDatumSelection = m_datumSelection;
	DDX_CBIndex (m_dialogFromPtr, IDC_DatumCombo, m_datumSelection);

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

		DDX_CBIndex (m_dialogToPtr, 
							IDC_EllipsoidCombo, 
							m_ellipsoidSelection);

		m_datumCode = abs (m_datumSelection);
		
		m_adjustUpperLeftMapPointFlag = TRUE;

		}	// end "if (lastDatumSelection != m_datumSelection)"
	
}	// end "OnSelendokDatumCombo"
