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
//	File:						SMapCoordinates.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			06/22/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains functions that handle map coordinate
//								settings.
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" SMapCoordinates::SetCoordinateViewLocationParameters (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDOK										1
	#define	IDC_X11Coordinate						7
	#define	IDC_OrientationAngle					15
	#define	IDC_ReferenceSystemPrompt			16
	#define	IDC_ReferenceSystemCombo			17
	#define	IDC_ZonePrompt							18
	#define	IDC_Zone									19
	#define	IDC_ZoneDirection						20
	#define	IDC_ProjectionPrompt					21
	#define	IDC_ProjectionCombo					22
	#define	IDC_LongitudePrompt					23
	#define	IDC_Longitude							24
	#define	IDC_LatitudePrompt					25
	#define	IDC_Latitude							26
	#define	IDC_FalseEastingPrompt				27
	#define	IDC_FalseEasting						28
	#define	IDC_FalseNorthingPrompt				29
	#define	IDC_FalseNorthing						30
	#define	IDC_ScaleFactorPrompt				31
	#define	IDC_ScaleFactor						32
	#define	IDC_StandardParallel1Prompt		33
	#define	IDC_StandardParallel1				34
	#define	IDC_StandardParallel2Prompt		35
	#define	IDC_StandardParallel2				36
	#define	IDC_DatumPrompt						37
	#define	IDC_DatumCombo							38
	#define	IDC_ProjectionEllipsoidPrompt		39
	#define	IDC_EllipsoidCombo					40
	#define	IDC_RadiusPrompt						41
	#define	IDC_Radius								42
	#define	IDC_MinorAxisPrompt					43
	#define	IDC_MinorAxis							44
	#define	IDC_MajorAxisPrompt					45
	#define	IDC_FIPSZoneName						46
	#define	IDC_EPSGCode							47
	#define	IDC_DatumName							48
	#define	IDC_EllipsoidName						49
	
	#define	IDS_Dialog31							31
	#define	IDS_Dialog32							32
	
	#define	IDS_Alert143							143
	#define	IDS_Alert145							145	
#endif	// defined multispec_mac || defined multispec_mac_swift   
                             
#if defined multispec_win
	#include "WImageView.h"
	#include "WImageFrame.h"
	#include "WMapCoordinateDialog.h" 	 
#endif	// defined multispec_win 

#if defined multispec_wx
	#include "xMultiSpec.h"
	#include "xImageView.h"
	#include "xImageFrame.h"
	#define IDOK  wxID_OK
	#include "xMapCoordinateDialog.h"
#endif	// defined multispec_wx

#define	kNotPackedDegrees					0
#define	kDegrees								1
#define	kRadians								2

#define MSUCCESS     1 /* SUCCESS */
#define MNPTERR      0 /* NOT ENOUGH POINTS */
#define MUNSOLVABLE -1 /* NOT SOLVABLE */
#define MMEMERR     -2 /* NOT ENOUGH MEMORY */
#define MPARMERR    -3 /* PARAMETER ERROR */
#define MINTERR     -4 /* INTERNAL ERROR */	

#define N_POLE	0
#define S_POLE	1
#define EQUIT	2
#define OBLIQ	3	

#define	kMaxCoordinateValue		1.0e37



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.

SInt16 ConvertDecimalDegreesToDMS (
				double								decimalDegrees,
				SInt16*								degreesPtr,
				SInt16*								minutesPtr,
				double*								secondsPtr,
				Boolean*								outOfRangeFlagPtr);

void ConvertLCRectToMapRect_TransformCoordinate (
				PlanarCoordinateSystemInfo*	planarCoordinatePtr,
				DoubleRect*							lineColumnRectPtr, 
				DoubleRect*							mapRectanglePtr,
				UInt16								boundingCode);

Boolean CoordinateDialogHandleInvalidValidEPSGCode (
				DialogPtr							dialogPtr,
				SInt16								epsgCode,
				Boolean								validEPSGCodeFromGDALFlag,
				Boolean								lastEPSGCodeValidFlag);

void CoordinateDialogSetProjectionParameters (
				DialogPtr							dialogPtr,
				SInt16								projectionCode,
				double								longitudeCentralMeridian,
				double								latitudeOrigin,
				double								scaleFactorOfCentralMeridian,
				double								falseEasting,
				double								falseNorthing,
				double								standardParallel1,
				double								standardParallel2,
				double								falseOriginLongitude,
				double								falseOriginLatitude,
				double								falseOriginEasting,
				double								falseOriginNorthing);

PascalVoid DrawDatumPopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);

PascalVoid DrawEllipsoidPopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);

PascalVoid DrawMapUnitsPopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);

PascalVoid DrawProjectionPopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);

PascalVoid DrawReferenceSystemPopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);	

double e4fn (
				double								x);

void GetBoundingOrientationAngleRect (
				DoubleRect*							boundingMapRectPtr,
				PlanarCoordinateSystemInfoPtr	planarCoordinatePtr,
				UInt16								boundingCode);

SInt16 GetEllipsoidCodeFromDatumCode (
				SInt16								datumCode);

Boolean GetEllipsoidParameters (
				SInt16								ellipsoidCode,
				double*								radiusSpheroidPtr,
				double*								semiMajorAxisPtr,
				double*								semiMinorAxisPtr);	

Boolean SetGDA94ParametersFromZone (
				SInt16								gridZone,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr);

SInt16 GetProjectionCodeFromProjectionMenuItem (
				SInt16								projectionTypeMenuSelection);

SInt16 GetReferenceSystemCodeFromReferenceSystemMenuItem (
				SInt16								referenceSystemMenuSelection);

Boolean SetProjectionParametersFromZone (
				SInt16								referenceSystemCode,
				SInt16								datumCode,
				SInt16*								gridZonePtr,
				UCharPtr								fipsZoneNamePtr,
				SInt16*								projectionCodePtr,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr,
				double*								standardParallel1Ptr,
				double*								standardParallel2Ptr,
				double*								falseOriginLongitudePtr,
				double*								falseOriginLatitudePtr,
				double*								falseOriginEastingPtr,
				double*								falseOriginNorthingPtr);

Boolean SetStatePlaneParametersFromZone (
				SInt16								gridZone,
				SInt16								datumCode,
				UCharPtr								fipsZoneNamePtr,
				SInt16*								projectionCodePtr,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr,
				double*								standardParallel1Ptr,
				double*								standardParallel2Ptr,
				double*								falseOriginLongitudePtr,
				double*								falseOriginLatitudePtr,
				double*								falseOriginEastingPtr,
				double*								falseOriginNorthingPtr);

Boolean SetTMPulkovo1942ParametersFromZone (
				SInt16*								gridZonePtr,
				double*								longitudeCentralMeridian,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr);
/*
void SetUpProjectionPopUpMenu (
				DialogPtr							dialogPtr);

void SetUpEllipsoidPopUpMenu (
				DialogPtr							dialogPtr);

void SetUpReferenceSystemPopUpMenu (
				DialogPtr							dialogPtr);
*/								
								
								
SInt16					gMapUnitsSelection = 0;
SInt16					gDatumSelection = 0;
SInt16					gEllipsoidSelection = 0;
SInt16					gProjectionSelection = 0;
SInt16					gReferenceSystemSelection = 0;



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean AreasIntersect
//
//	Software purpose:	The purpose of this routine is to determine if the two input
//							rectangles overlap.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2000
//	Revised By:			Larry L. Biehl			Date: 04/29/2012			

Boolean AreasIntersect (
				DoubleRect*							boundingRectangle1Ptr,
				DoubleRect*							boundingRectangle2Ptr)

{	
	if (boundingRectangle1Ptr->top > boundingRectangle2Ptr->bottom ||
			boundingRectangle1Ptr->bottom < boundingRectangle2Ptr->top ||
				boundingRectangle1Ptr->left > boundingRectangle2Ptr->right ||
					boundingRectangle1Ptr->right < boundingRectangle2Ptr->left)
		return (FALSE);
		
	return (TRUE);

}	// end "AreasIntersect" 
		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfGeodeticModelInfoMatch
//
//	Software purpose:	This routine checks if the geodetic model information in the
//							two input geodetic structures are the same.
//
//	Parameters in:		Pointer to two input geodetic model structures	
//
//	Parameters out:	None		
//
//	Value Returned:	True if the information in the geodetic structures are the same
//							False if the information are not the same
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2008
//	Revised By:			Larry L. Biehl			Date: 12/12/2008		

Boolean CheckIfGeodeticModelInfoMatch (
				GeodeticModelInfoPtr				geodeticModelInfoPtr,
				GeodeticModelInfoPtr				geodeticModelInfoPtr2)

{ 
	if (geodeticModelInfoPtr->radiusSpheroid != 
								geodeticModelInfoPtr2->radiusSpheroid)
																						return (FALSE);
		
	if (geodeticModelInfoPtr->semiMajorAxis != 
								geodeticModelInfoPtr2->semiMajorAxis)
																						return (FALSE);
		
	if (geodeticModelInfoPtr->semiMinorAxis != 
								geodeticModelInfoPtr2->semiMinorAxis)
																						return (FALSE);
		
	if (geodeticModelInfoPtr->datumCode != 
								geodeticModelInfoPtr2->datumCode)
																						return (FALSE);
		
	if (geodeticModelInfoPtr->spheroidCode != 
								geodeticModelInfoPtr2->spheroidCode)
																						return (FALSE);
																						
	return (TRUE);
			
}	// end "CheckIfGeodeticModelInfoMatch" 
		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfGridCoordinateSystemInfoMatch
//
//	Software purpose:	This routine checks if the grid coordinate system information in 
//							the two input grid coordinate info structures are the same.
//
//	Parameters in:		Pointer to two grid coordinate system information structures	
//
//	Parameters out:	
//
//	Value Returned:	True if the information in the grid coordinate structures are 
//							the same
//							False if the information are not the same
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2008
//	Revised By:			Larry L. Biehl			Date: 03/12/2012		

Boolean CheckIfGridCoordinateSystemInfoMatch (
				GridCoordinateSystemInfoPtr	gridCoordinateSystemInfoPtr,
				GridCoordinateSystemInfoPtr	gridCoordinateSystemInfoPtr2)

{  
	if (gridCoordinateSystemInfoPtr->gridZone != 
								gridCoordinateSystemInfoPtr2->gridZone)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->standardParallel1 != 
								gridCoordinateSystemInfoPtr2->standardParallel1)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->standardParallel2 != 
								gridCoordinateSystemInfoPtr2->standardParallel2)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->falseEasting != 
								gridCoordinateSystemInfoPtr2->falseEasting)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->falseNorthing != 
								gridCoordinateSystemInfoPtr2->falseNorthing)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->falseOriginEasting != 
								gridCoordinateSystemInfoPtr2->falseOriginEasting)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->falseOriginNorthing != 
								gridCoordinateSystemInfoPtr2->falseOriginNorthing)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->falseOriginLatitude != 
								gridCoordinateSystemInfoPtr2->falseOriginLatitude)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->falseOriginLongitude != 
								gridCoordinateSystemInfoPtr2->falseOriginLongitude)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->latitudeOrigin != 
								gridCoordinateSystemInfoPtr2->latitudeOrigin)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->longitudeCentralMeridian != 
								gridCoordinateSystemInfoPtr2->longitudeCentralMeridian)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->scaleFactorOfCentralMeridian != 
								gridCoordinateSystemInfoPtr2->scaleFactorOfCentralMeridian)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->projAzimuthAngle != 
								gridCoordinateSystemInfoPtr2->projAzimuthAngle)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->referenceSystemCode != 
								gridCoordinateSystemInfoPtr2->referenceSystemCode)
																						return (FALSE);
		
	if (gridCoordinateSystemInfoPtr->projectionCode != 
								gridCoordinateSystemInfoPtr2->projectionCode)
																						return (FALSE);
																						
	return (TRUE);
			
}	// end "CheckIfGridCoordinateSystemInfoMatch" 
		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CheckIfMapInfoMatches
//
//	Software purpose:	This routine initializes the grid coordinate system structure.
//
//	Parameters in:		Pointer to the geodetic model structure	
//
//	Parameters out:				
//
//	Value Returned:	True if the information in the map structures are the same
//							False if the information are not the same	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/13/2008
//	Revised By:			Larry L. Biehl			Date: 12/13/2008		

Boolean CheckIfMapInfoMatches (
				GeodeticModelInfoPtr				geodeticModelInfoPtr,
				GeodeticModelInfoPtr				geodeticModelInfoPtr2,
				GridCoordinateSystemInfoPtr	gridCoordinateSystemInfoPtr,
				GridCoordinateSystemInfoPtr	gridCoordinateSystemInfoPtr2)

{  
	Boolean								mapInfoSameFlag;
	
			
	mapInfoSameFlag = CheckIfGridCoordinateSystemInfoMatch (
																gridCoordinateSystemInfoPtr,
																gridCoordinateSystemInfoPtr2);

	if (mapInfoSameFlag)
		mapInfoSameFlag = CheckIfGeodeticModelInfoMatch (
																geodeticModelInfoPtr,
																geodeticModelInfoPtr2);
		
	return (mapInfoSameFlag);	

}	// end "CheckIfMapInfoMatches" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseCoefficientsVectorPointers
//
//	Software purpose:	The purpose of this routine is to unlock the memory for the
//							polygon coefficients and set the coefficient vectors to NULL.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/13/2007
//	Revised By:			Larry L. Biehl			Date: 02/13/2007

void CloseCoefficientsVectorPointers (
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{  
	PlanarCoordinateSystemInfoPtr	planarCoordinateInfoPtr;
	
														
	if (mapProjectionInfoPtr != NULL)
		{			
		CheckAndUnlockHandle (mapProjectionInfoPtr->coefficientsHandle);
		
		planarCoordinateInfoPtr = &mapProjectionInfoPtr->planarCoordinate;
		planarCoordinateInfoPtr->easting1CoefficientsPtr = NULL;
		planarCoordinateInfoPtr->northing1CoefficientsPtr = NULL; 
		planarCoordinateInfoPtr->easting2CoefficientsPtr = NULL;  
		planarCoordinateInfoPtr->northing2CoefficientsPtr = NULL;  
		
		}	// end "if (mapProjectionInfoPtr != NULL)"
	
}	// end "CloseCoefficientsVectorPointers"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeAndSetScaleInformation
//
//	Software purpose:	The purpose of this routine is to compute the scale of the
//							specified image that is displayed.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/10/2000
//	Revised By:			Larry L. Biehl			Date: 12/07/2011			

void ComputeAndSetScaleInformation (
				Handle								windowInfoHandle)

{	
	double								scale = -1.;
											
	DisplaySpecsPtr					displaySpecsPtr;
											
	Handle								displaySpecsHandle;
											
	SignedByte							handleStatus;
	
	
			// Determine the distance across one displayed pixel in inches based on the
			// units specifed by the file planar map information, the zoom factor
			// of the displayed image, and the line/column interval used for
			// the display. Will use the scale in the x (column) direction.
		
	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
	
	displaySpecsPtr = (DisplaySpecsPtr)GetHandleStatusAndPointer (displaySpecsHandle,
																						&handleStatus);

	if (displaySpecsPtr != NULL)
		{
		scale = GetScale (windowInfoHandle,
									displaySpecsPtr->displayedColumnInterval,
									displaySpecsPtr->magnification);
									
		displaySpecsPtr->scale = scale;
		
		}	// end "if (displaySpecsPtr != NULL)"
	
	MHSetState (displaySpecsHandle, handleStatus);

}	// end "ComputeAndSetScaleInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ConvertDecimalDegreesToDMS
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			LoadDMSLatLongStrings in SMapCordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 02/28/2007		

SInt16 ConvertDecimalDegreesToDMS (
				double								decimalDegrees,
				SInt16*								degreesPtr,
				SInt16*								minutesPtr,
				double*								secondsPtr,
				Boolean*								rangeOKFlagPtr)

{ 
	double								decimalMinutes,
											fractionalDegrees;
											
	SInt16								signIndex;
	  
	
	signIndex = 0;
	*rangeOKFlagPtr = TRUE;
	if (decimalDegrees < 0)
		{
		decimalDegrees = fabs (decimalDegrees);
		signIndex = 1;
		
		}	// end "if (decimalDegrees < 0)"
	
	if (decimalDegrees <= 360)	
		{
		*degreesPtr = (SInt16)decimalDegrees;
		
		fractionalDegrees = decimalDegrees - *degreesPtr;
		decimalMinutes = 60 * fractionalDegrees;
		*minutesPtr = (SInt16)(decimalMinutes);
		
		*secondsPtr = 60 * (decimalMinutes - *minutesPtr);
			
		}	// end "if (decimalDegrees <= 360)"
		
	else	// decimalDegrees > 360
			// Value is out of range.
		*rangeOKFlagPtr = FALSE;
	
	return (signIndex);
	
}	// end "ConvertDecimalDegreesToDMS" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertFeetToMeters
//
//	Software purpose:	This routine converts the input linear measurements from the
//							respective feet units to meters if needed.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/07/2012
//	Revised By:			Larry L. Biehl			Date: 04/07/2012		

void ConvertFeetToMeters (
				SInt16								mapUnitsCode, 
				double*								xMeasurementPtr, 
				double*								yMeasurementPtr)

{		
	if (mapUnitsCode == kFeetCode)
		{				
		*xMeasurementPtr *= kFeetToMetersConversion;
		*yMeasurementPtr *= kFeetToMetersConversion;
		
		}	// end "if (mapUnitsCode == kFeetCode)"
		
	else if (mapUnitsCode == kUSSurveyFeetCode)
		{				
		*xMeasurementPtr *= kUSSurveyFeetToMetersConversion;
		*yMeasurementPtr *= kUSSurveyFeetToMetersConversion;
		
		}	// end "else if (mapUnitsCode == kUSSurveyFeetCode)"
	
}	// end "ConvertFeetToMeters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLCPointToMapPoint
//
//	Software purpose:	This routine converts input line column point to a map point.
//							For the way the output map point is currently used, the map point 
//							will be converted to meters if needed.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			WriteKMLFile in SSaveWrite.cpp
//							ListFieldData in SListData.cpp
//							ReprojectNearestNeighborLineColumn in SRectification.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2006
//	Revised By:			Larry L. Biehl			Date: 04/06/2012		

void ConvertLCPointToMapPoint (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				LongPoint*							lineColumnPointPtr, 
				DoublePoint*						mapPointPtr)

{
			
	double								horizontalPixelSize,
											verticalPixelSize,
											xMapCoordinate11,
											yMapCoordinate11;
	
					
	if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
		{
		horizontalPixelSize = 
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		verticalPixelSize = 
					-mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;

		xMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
		yMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
	
		if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0)
			{
			mapPointPtr->v = yMapCoordinate11 + (lineColumnPointPtr->v - 1) *
																				verticalPixelSize;

			mapPointPtr->h = xMapCoordinate11 + (lineColumnPointPtr->h - 1) *
																				horizontalPixelSize;
				
			}	// end "if (...->planarCoordinate.mapOrientationAngle == 0)"
			
		else	// mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0
			{
			double			cosOrientAngle,
								sinOrientAngle,
								xOffset,
								yOffset;
								
			cosOrientAngle = 
					cos (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
			sinOrientAngle = 
					sin (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
					
			xOffset = (lineColumnPointPtr->h - 1) * horizontalPixelSize;
					
			yOffset = (lineColumnPointPtr->v - 1) * verticalPixelSize;

			mapPointPtr->h = mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin + 
											xOffset*cosOrientAngle + yOffset*sinOrientAngle;
					
			mapPointPtr->v = mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin -  
											xOffset*sinOrientAngle + yOffset*cosOrientAngle;
				
			}	// end "if (...->planarCoordinate.mapOrientationAngle != 0)"
						
		}	// end "mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0"
		
	else	// mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0
		{									
		TransformCoordinatePoint (
						(double)lineColumnPointPtr->h,
						(double)lineColumnPointPtr->v,
						&mapPointPtr->h,
						&mapPointPtr->v,
						mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr,
						mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr,
						mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
			
		}	// end "else mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0"
		
			// Convert the output map point to meters if needed.
			
	ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
									&mapPointPtr->h, 
									&mapPointPtr->v);

}	// end "ConvertLCPointToMapPoint" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLCRectToMapRect
//
//	Software purpose:	This routine converts the input map
//							point ArcView coordinates to the current window 
//							point coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ConvertShapeToClassNumber in SShapeToThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/04/2001
//	Revised By:			Larry L. Biehl			Date: 04/04/2001		

void ConvertLCRectToMapRect (
				Handle								windowInfoH, 
				LongRect*							lineColumnRectPtr, 
				DoubleRect*							mapRectPtr)

{
	double								bottom,
											left,
											right,
											top,
											xMapCoordinate11,
											xPixelSize,
											yMapCoordinate11,
											yPixelSize;	
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
											

			// Get point to file map projection specification. We do not need 
			// to lock it here since no other routines are called								
			
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoH);

	if (mapProjectionHandle != NULL)
		{
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);
		
				// Get the map rectangle relative to the offscreen bit/pix map	
				// for the x (column) part of rectangle											
				
		xMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
		xPixelSize = mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		
		left = (lineColumnRectPtr->left - 1.5) * xPixelSize + xMapCoordinate11;
		right = (lineColumnRectPtr->right - 0.5) * xPixelSize + xMapCoordinate11;
						
				// Make sure that the bounding box represents the smallest values in the
				// upper left and largest values in the lower right.
						
		mapRectPtr->left = MIN (left, right);
		mapRectPtr->right = MAX (left, right);
		
				// Get the map rectangle relative to the offscreen bit/pix map	
				// for the y (line) part of rectangle		
				// We are assuming that the y map coordinate increases from north (top)
				// to south (bottom).													
		
		yMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
		yPixelSize = -mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
				
		top = (lineColumnRectPtr->top - 1.5) * yPixelSize + yMapCoordinate11;
		bottom = (lineColumnRectPtr->bottom - 0.5) * yPixelSize + yMapCoordinate11;
						
				// Make sure that the bounding box represents the smallest values in the
				// upper left and largest values in the lower right.
						
		mapRectPtr->top = MIN (top, bottom);
		mapRectPtr->bottom = MAX (top, bottom);
		
		}	// end "if (mapProjectionHandle != NULL)" 
		
	else	// mapProjectionHandle == NULL
		{
		mapRectPtr->left = 0.;
		mapRectPtr->right = 0.;
		mapRectPtr->top = 0.;
		mapRectPtr->bottom = 0.;
		
		}	// end "else mapProjectionHandle == NULL"

}	// end "ConvertLCRectToMapRect" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLCRectToMapRect_TransformCoordinate
//
//	Software purpose:	This routine converts the input line column value to map
//							coordinates for the case when a polynomial transformation is used.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2007
//	Revised By:			Larry L. Biehl			Date: 03/23/2007		

void ConvertLCRectToMapRect_TransformCoordinate (
				PlanarCoordinateSystemInfo*	planarCoordinatePtr,
				DoubleRect*							lineColumnRectPtr, 
				DoubleRect*							mapRectanglePtr,
				UInt16								boundingCode)

{								
	TransformCoordinatePoint (lineColumnRectPtr->left,
										lineColumnRectPtr->top,
										&mapRectanglePtr->left,
										&mapRectanglePtr->top,
										planarCoordinatePtr->easting1CoefficientsPtr,
										planarCoordinatePtr->northing1CoefficientsPtr,
										planarCoordinatePtr->polynomialOrder);
	
	TransformCoordinatePoint (lineColumnRectPtr->right,
										lineColumnRectPtr->bottom,
										&mapRectanglePtr->right,
										&mapRectanglePtr->bottom,
										planarCoordinatePtr->easting1CoefficientsPtr,
										planarCoordinatePtr->northing1CoefficientsPtr,
										planarCoordinatePtr->polynomialOrder);
	
	if (boundingCode == kAllCorners)
		{	
		double							bottom,
											left,
											right,
											top;
											
		TransformCoordinatePoint (lineColumnRectPtr->right,
											lineColumnRectPtr->top,
											&right,
											&top,
											planarCoordinatePtr->easting1CoefficientsPtr,
											planarCoordinatePtr->northing1CoefficientsPtr,
											planarCoordinatePtr->polynomialOrder);
			
		TransformCoordinatePoint (lineColumnRectPtr->left,
											lineColumnRectPtr->bottom,
											&left,
											&bottom,
											planarCoordinatePtr->easting1CoefficientsPtr,
											planarCoordinatePtr->northing1CoefficientsPtr,
											planarCoordinatePtr->polynomialOrder);
			
		mapRectanglePtr->left = MIN (left, mapRectanglePtr->left);
		mapRectanglePtr->right = MAX (right, mapRectanglePtr->right);
		mapRectanglePtr->top = MIN (top, mapRectanglePtr->top);
		mapRectanglePtr->bottom = MAX (bottom, mapRectanglePtr->bottom);
		
		}	// end "if (boundingCode == kAllCorners)"

}	// end "ConvertLCRectToMapRect_TransformCoordinate" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMapPointToLC
//
//	Software purpose:	This routine converts the input map
//							point ArcView coordinates to the current window 
//							point coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ConvertShapeToClassNumber in SShapeToThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2001
//	Revised By:			Larry L. Biehl			Date: 03/23/2007		

void ConvertMapPointToLC (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoublePoint*						mapPointPtr, 
				LongPoint*							lineColumnPtr)

{
	double								doubleColumnValue,
											doubleLineValue,
											xMap11,
											xPixelSize,
											yMap11,
											yPixelSize;	
	
	PlanarCoordinateSystemInfoPtr	planarCoordinateSystemInfoPtr;
	

			// Get point to file map projection specification. We do not need 
			// to lock it here since no other routines are called								
			
	if (mapProjectionInfoPtr != NULL)
		{
		planarCoordinateSystemInfoPtr = &mapProjectionInfoPtr->planarCoordinate;
		
		if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
			{			
					// Get the map rectangle relative to the offscreen bit/pix map	
					// We are assuming that the y map coordinate increases from north (top)
					// to south (bottom).																				
			
			xMap11 = planarCoordinateSystemInfoPtr->xMapCoordinate11;
			yMap11 = planarCoordinateSystemInfoPtr->yMapCoordinate11;
			
			xPixelSize = mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
			yPixelSize = -mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
			
			if (planarCoordinateSystemInfoPtr->mapOrientationAngle != 0)
				{
				double			cosOrAngle,
									sinOrAngle,
									xOffset,
									yOffset,
									xOrientationOrigin,
									yOrientationOrigin;
									
				xOrientationOrigin = 
								mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin;
				yOrientationOrigin = 
								mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin;
									
				cosOrAngle = cos (planarCoordinateSystemInfoPtr->mapOrientationAngle);
				sinOrAngle = sin (planarCoordinateSystemInfoPtr->mapOrientationAngle);
				
				yOffset = (cosOrAngle*(mapPointPtr->v-yOrientationOrigin) + 
										sinOrAngle*(mapPointPtr->h - xOrientationOrigin))/
													(cosOrAngle*cosOrAngle + sinOrAngle*sinOrAngle);
													 
				xOffset = (mapPointPtr->h - xOrientationOrigin - sinOrAngle*yOffset)/
																									cosOrAngle;
				
				mapPointPtr->h = xOrientationOrigin + xOffset;
				mapPointPtr->v = yOrientationOrigin + yOffset;
					
				}	// end "if (planarCoordinateSystemInfoPtr->mapOrientationAngle != 0)"
			
			lineColumnPtr->v = (SInt32)((mapPointPtr->v - yMap11)/yPixelSize + 1.5);
			lineColumnPtr->h = (SInt32)((mapPointPtr->h - xMap11)/xPixelSize + 1.5);
						
			}	// end "mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0"
			
		else	// mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0
			{				
			TransformCoordinatePoint (
							mapPointPtr->h,
							mapPointPtr->v,
							&doubleColumnValue,
							&doubleLineValue,
							mapProjectionInfoPtr->planarCoordinate.easting2CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.northing2CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
							
			lineColumnPtr->h = (SInt32)(doubleColumnValue + 1.50001);
			lineColumnPtr->v = (SInt32)(doubleLineValue + 1.50001);
				
			}	// end "else mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0"
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 

}	// end "ConvertMapPointToLC" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMapPointToLC
//
//	Software purpose:	This routine converts the input map
//							point ArcView coordinates to the current window 
//							point coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ConvertShapeToClassNumber in SShapeToThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2001
//	Revised By:			Larry L. Biehl			Date: 02/23/2007		

void ConvertMapPointToLC (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				ArcViewDoublePoint*				mapPointPtr, 
				LongPoint*							lineColumnPtr)

{
	DoublePoint							lMapPoint;
											

			// Get point to file map projection specification. We do not need 
			// to lock it here since no other routines are called			
		
	lMapPoint.v = mapPointPtr->y;
	lMapPoint.h = mapPointPtr->x;		
	
	ConvertMapPointToLC (mapProjectionInfoPtr,
									&lMapPoint, 
									lineColumnPtr);		
			
}	// end "ConvertMapPointToLC"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMapRectToLCRect
//
//	Software purpose:	This routine converts the input map coordinates to line column
//							coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawImageOverlays in SImageOverlays.cpp
//							ConvertCoordinateRectToLCRect in SSelectionUtility.cpp
//							EditSelectionDialogSetCoordinates in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/23/2004
//	Revised By:			Larry L. Biehl			Date: 04/17/2019

void ConvertMapRectToLCRect (
				Handle								windowInfoHandle,
				DoubleRect*							mapRectPtr, 
				LongRect*							lineColumnRectPtr,
				double								factor)

{
	DoubleRect							lMapRect;
	
	double								doubleColumnValue,
											doubleLineValue,
											xMap11,
											xPixelSize,
											yMap11,
											yPixelSize;
											
											
	MapProjectionInfoPtr 			mapProjectionInfoPtr;
	PlanarCoordinateSystemInfoPtr	planarCoordinateSystemInfoPtr;
	
	Handle								mapProjectionHandle;
											
											
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);

	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
	
	if (mapProjectionInfoPtr != NULL)
		{		
		if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
			{									
			planarCoordinateSystemInfoPtr = &mapProjectionInfoPtr->planarCoordinate;
			
			lMapRect = *mapRectPtr;
														
			xMap11 = planarCoordinateSystemInfoPtr->xMapCoordinate11;
			yMap11 = planarCoordinateSystemInfoPtr->yMapCoordinate11;
			
			xPixelSize = planarCoordinateSystemInfoPtr->horizontalPixelSize;
			yPixelSize = -planarCoordinateSystemInfoPtr->verticalPixelSize;
				
			//factor = GetCoordinateViewFactor (windowInfoHandle);
			
					// Take the unit conversion factor into account so that units are the
					// same as for the upper left pixel (...Map11) and pixel size.
					
			lMapRect.top /= factor;
			lMapRect.left /= factor;
			lMapRect.bottom /= factor;
			lMapRect.right /= factor;
			
			if (planarCoordinateSystemInfoPtr->mapOrientationAngle != 0)
				{
				double			cosOrAngle,
									sinOrAngle,
									xOffset,
									yOffset,
									xOrientationOrigin,
									yOrientationOrigin;
									
				xOrientationOrigin = planarCoordinateSystemInfoPtr->xMapOrientationOrigin;
				yOrientationOrigin = planarCoordinateSystemInfoPtr->yMapOrientationOrigin;
									
				cosOrAngle = cos (planarCoordinateSystemInfoPtr->mapOrientationAngle);
				sinOrAngle = sin (planarCoordinateSystemInfoPtr->mapOrientationAngle);
				
				yOffset = (cosOrAngle*(lMapRect.top-yOrientationOrigin) + 
											sinOrAngle*(lMapRect.left - xOrientationOrigin)) /
													(cosOrAngle*cosOrAngle + sinOrAngle*sinOrAngle);
													 
				xOffset = (lMapRect.left - xOrientationOrigin -
																	sinOrAngle*yOffset)/cosOrAngle;
				
				lMapRect.left = xOrientationOrigin + xOffset;
				lMapRect.top = yOrientationOrigin + yOffset;
				
				yOffset = (cosOrAngle*(lMapRect.bottom-yOrientationOrigin) + 
											sinOrAngle*(lMapRect.right - xOrientationOrigin))/
													(cosOrAngle*cosOrAngle + sinOrAngle*sinOrAngle);
													 
				xOffset = (lMapRect.right - xOrientationOrigin -
																		sinOrAngle*yOffset)/cosOrAngle;
				
				lMapRect.right = xOrientationOrigin + xOffset;
				lMapRect.bottom = yOrientationOrigin + yOffset;
					
				}	// end "if (planarCoordinateSystemInfoPtr->mapOrientationAngle != 0)"
				
					// Get line-column for the input top-left map units.
					// A column or line will be included if more than half of it is with
					// the area to the right or lower of the map coordinate value. And to
					// the left or above the lower right map coordinate value.
					// 	- Add .5 pixel (line or column to take into account that xMap11
					//			and yMap11 represent the center of the upper left pixel.
					// 	- Add 1 to convert resulting value from 0 based line / column to
					//			1 based.
					//		- Add .5 to allow for the above criterea for amount of area to be
					// 		within a pixel to be included for the upper-left pixel in the
					//			selection. One does not need to add this for the lower-right
					//			pixel in the selection.
				
			lineColumnRectPtr->left = (SInt32)((lMapRect.left - xMap11)/xPixelSize + 2.0);
			
			lineColumnRectPtr->top = (SInt32)((lMapRect.top - yMap11)/yPixelSize + 2.0);
					// Get line-column for the input bottom-right map units.	
													
			lineColumnRectPtr->right =
										(SInt32)((lMapRect.right - xMap11)/xPixelSize + 1.499);
			
			lineColumnRectPtr->bottom =
										(SInt32)((lMapRect.bottom - yMap11)/yPixelSize + 1.499);
			/*
						// For debugging.
				int numberChars = sprintf ((char*)gTextString3,
					" SMapCoordinates:ConvertMapRectToLCRect (right, bottom): %f, %f%s",
					(lMapRect.right - xMap11)/xPixelSize + 1.499,
					(lMapRect.bottom - yMap11)/yPixelSize + 1.499,
					gEndOfLine);
				ListString ((char*)gTextString3, numberChars, gOutputTextH);
			*/
					// Make sure the left value is <= the right value
				
			if (lineColumnRectPtr->left > lineColumnRectPtr->right)
				lineColumnRectPtr->right = lineColumnRectPtr->left;
			
					// Make sure the top value is <= the bottom value
				
			if (lineColumnRectPtr->top > lineColumnRectPtr->bottom)
				lineColumnRectPtr->bottom = lineColumnRectPtr->top;
						
			}	// end "mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0"
			
		else	// mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0
			{				
			GetCoefficientsVectorPointers (mapProjectionInfoPtr);
							
			TransformCoordinatePoint (
							mapRectPtr->left,
							mapRectPtr->top,
							&doubleColumnValue,
							&doubleLineValue,
							mapProjectionInfoPtr->planarCoordinate.easting2CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.northing2CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
							
			lineColumnRectPtr->left = (SInt32)(doubleColumnValue + 2.0);	// 1.50001
			lineColumnRectPtr->top = (SInt32)(doubleLineValue + 2.0);		// 1.50001
							
			TransformCoordinatePoint (
							mapRectPtr->right,
							mapRectPtr->bottom,
							&doubleColumnValue,
							&doubleLineValue,
							mapProjectionInfoPtr->planarCoordinate.easting2CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.northing2CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
							
			lineColumnRectPtr->right = (SInt32)(doubleColumnValue + 1.499);	// .50001
			lineColumnRectPtr->bottom = (SInt32)(doubleLineValue + 1.499);	// .50001
							
			CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
				
			}	// end "else mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0"
						
		}	// end "if (mapProjectionInfoPtr != NULL)"

}	// end "ConvertMapRectToLCRect" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMapRectToOffscreenRect
//
//	Software purpose:	This routine converts the input map coordinates to the offscreen
//							units for the input offscreen image.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/20/2003
//	Revised By:			Larry L. Biehl			Date: 01/20/2003		

void ConvertMapRectToOffscreenRect (
				DoubleRect*							mapRectPtr, 
				LongRect*							offscreenRectPtr,
				MapToWindowUnitsVariablesPtr	mapToWindowUnitsVariablesPtr)

{
	LongPoint							lowerRightPoint,
											upperLeftPoint; 
		
	
			// Get the map rectangle relative to the offscreen pixmap	
			// for the x (column) part of rectangle											
			
	upperLeftPoint.h = (SInt32)((mapRectPtr->left - 
						mapToWindowUnitsVariablesPtr->mapXOrigin)/
									mapToWindowUnitsVariablesPtr->xMapPixelSize + 0.5);										
			
	upperLeftPoint.v = (SInt32)((mapRectPtr->top - 
						mapToWindowUnitsVariablesPtr->mapYOrigin)/
									mapToWindowUnitsVariablesPtr->yMapPixelSize + 0.5);					
			
	lowerRightPoint.h = (SInt32)((mapRectPtr->right - 
						mapToWindowUnitsVariablesPtr->mapXOrigin)/
									mapToWindowUnitsVariablesPtr->xMapPixelSize + 0.5);									
			
	lowerRightPoint.v = (SInt32)((mapRectPtr->bottom - 
						mapToWindowUnitsVariablesPtr->mapYOrigin)/
									mapToWindowUnitsVariablesPtr->yMapPixelSize + 0.5);
											
	offscreenRectPtr->top = MIN (upperLeftPoint.v, lowerRightPoint.v);
	offscreenRectPtr->bottom = MAX (upperLeftPoint.v, lowerRightPoint.v);
	offscreenRectPtr->left = MIN (upperLeftPoint.h, lowerRightPoint.h);
	offscreenRectPtr->right = MAX (upperLeftPoint.h, lowerRightPoint.h);

}	// end "ConvertMapRectToOffscreenRect"
			



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMapPointToOrientationAngleMapPoint
//
//	Software purpose:	This routine converts the input map rectangle to the
//							map units represented by taking the orientation angle
//							into account.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//							ConvertMapRectToWinRect
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2005
//	Revised By:			Larry L. Biehl			Date: 02/24/2006		

void ConvertMapPointToOrientationAngleMapPoint (
				double								mapOrientationAngle, 
				double								xMapOrientationOrigin, 
				double								yMapOrientationOrigin,
				DoublePoint*						mapCoordinatePointPtr)

{
	double								cosOrientAngle,
											sinOrientAngle,
											xOffset,
											yOffset;
										
						
	cosOrientAngle = cos (mapOrientationAngle);
	sinOrientAngle = sin (mapOrientationAngle);
						
	xOffset = mapCoordinatePointPtr->h - xMapOrientationOrigin;
	yOffset = mapCoordinatePointPtr->v - yMapOrientationOrigin;

	mapCoordinatePointPtr->h = xMapOrientationOrigin + 
									xOffset*cosOrientAngle + yOffset*sinOrientAngle;
			
	mapCoordinatePointPtr->v = yMapOrientationOrigin -  
									xOffset*sinOrientAngle + yOffset*cosOrientAngle;
		
}	// end "ConvertMapPointToOrientationAngleMapPoint" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ConvertMapPointToWinPoint
//
//	Software purpose:	This routine converts the input map
//							point ArcView coordinates to the current window 
//							point coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//							ConvertMapRectToWinRect
//
//	Coded By:			Larry L. Biehl			Date: 01/02/2001
//	Revised By:			Larry L. Biehl			Date: 03/19/2015		

Boolean ConvertMapPointToWinPoint (
				ArcViewDoublePoint*				mapPoint, 
				LongPoint*							windowPointPtr,
				MapToWindowUnitsVariablesPtr	mapToWindowUnitsVariablesPtr)

{
	DoublePoint							windowDoublePoint;
	
	double								cosOrAngle,
											sinOrAngle,
											magnification,
											mapXOffset,
											mapXPoint,
											mapYOffset,
											mapYPoint,
											offset;
											
	Boolean								returnFlag;
											
	
	
	returnFlag = TRUE;
				
	mapXPoint = mapPoint->x;
	mapYPoint = mapPoint->y;
	
	if (mapToWindowUnitsVariablesPtr->polynomialOrder <= 0)
		{
				// Allow for the map orientation if needed.
				
		if (mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag)
			{
			cosOrAngle = mapToWindowUnitsVariablesPtr->cosOrAngle;
			sinOrAngle = mapToWindowUnitsVariablesPtr->sinOrAngle;
			
			mapXOffset = mapXPoint - mapToWindowUnitsVariablesPtr->xMap11;
			mapYOffset = mapYPoint - mapToWindowUnitsVariablesPtr->yMap11;
			
			mapYOffset = (cosOrAngle*mapYOffset + 
												sinOrAngle*mapXOffset)/
												 (cosOrAngle*cosOrAngle + sinOrAngle*sinOrAngle);
												 
			mapXOffset = (mapXOffset - sinOrAngle*mapYOffset)/cosOrAngle;
			
			mapXPoint = mapToWindowUnitsVariablesPtr->xMap11 + mapXOffset;
			mapYPoint = mapToWindowUnitsVariablesPtr->yMap11 + mapYOffset;
			
			}	// end "if (mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag)"
		
				// Get the map rectangle relative to the offscreen pixmap	
				// for the x (column) part of rectangle											
				
		mapXOffset = mapXPoint - mapToWindowUnitsVariablesPtr->mapXOrigin;
		windowDoublePoint.h = mapXOffset/mapToWindowUnitsVariablesPtr->xMapPixelSize;
		
				// Get the map rectangle relative to the offscreen pixmap	
				// for the y (line) part of rectangle																
		
		mapYOffset = mapYPoint - mapToWindowUnitsVariablesPtr->mapYOrigin;
		windowDoublePoint.v = mapYOffset/mapToWindowUnitsVariablesPtr->yMapPixelSize;
						
		}	// end "if (mapToWindowUnitsVariablesPtr->polynomialOrder <= 0)"
		
	else	// mapToWindowUnitsVariablesPtr->polynomialOrder > 0
		{				
		TransformCoordinatePoint (
						mapXPoint,
						mapYPoint,
						&windowDoublePoint.h,
						&windowDoublePoint.v,
						mapToWindowUnitsVariablesPtr->easting2CoefficientsPtr,
						mapToWindowUnitsVariablesPtr->northing2CoefficientsPtr,
						mapToWindowUnitsVariablesPtr->polynomialOrder);

		windowDoublePoint.h -= mapToWindowUnitsVariablesPtr->windowColumnOrigin;
		windowDoublePoint.v -= mapToWindowUnitsVariablesPtr->windowLineOrigin;

		windowDoublePoint.h /= mapToWindowUnitsVariablesPtr->displayedColumnInterval;
		windowDoublePoint.v /= mapToWindowUnitsVariablesPtr->displayedLineInterval;
			
		}	// end "else mapToWindowUnitsVariablesPtr->polynomialOrder > 0"
		
			// Now take into account the magnification and convert to integer value
			
	magnification = mapToWindowUnitsVariablesPtr->magnification;
	
	offset = mapToWindowUnitsVariablesPtr->offset;
	if (offset < 0)
		{
		windowPointPtr->h = (SInt32)(windowDoublePoint.h * magnification + offset);
		windowPointPtr->v = (SInt32)(windowDoublePoint.v * magnification + offset);
		
		}	// end "if (offset < 0)"
		
	else	// offset >= 0
		{
		windowPointPtr->h = (SInt32)(windowDoublePoint.h * magnification + 
																SIGN (offset, windowDoublePoint.h));
		windowPointPtr->v = (SInt32)(windowDoublePoint.v * magnification + 
																SIGN (offset, windowDoublePoint.v));
		
		}	// end "else offset >= 0"
		
	windowPointPtr->h += mapToWindowUnitsVariablesPtr->imageLeftOffset;
	windowPointPtr->v += mapToWindowUnitsVariablesPtr->imageTopOffset;
	
			// This takes into account the possibility of a side by side	
			// channel display.															
			
	windowPointPtr->h += mapToWindowUnitsVariablesPtr->channelWindowOffset;
	
			// Check if window point is within -32767 and 32767.
			
	if (windowPointPtr->h < SInt16_MIN || windowPointPtr->h > SInt16_MAX ||
			windowPointPtr->v < SInt16_MIN || windowPointPtr->v > SInt16_MAX)
		returnFlag = FALSE;
		
	return (returnFlag);

}	// end "ConvertMapPointToWinPoint" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMapRectToWinRect
//
//	Software purpose:	This routine converts the input map rectangle coordinates to 
//							the current window rectangle coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawImageOverlays in SImageOverlays.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2000
//	Revised By:			Larry L. Biehl			Date: 01/15/2003		

void ConvertMapRectToWinRect (
				DoubleRect*							mapRectPtr, 
				LongRect*							windowRectPtr,
				MapToWindowUnitsVariablesPtr	mapToWindowUnitsVariablesPtr)

{
	ArcViewDoublePoint				sDoubleMapPoint;
	
	LongPoint							lowerRightPoint,
											upperLeftPoint; 
											
	
			// Convert top left point.
			
	sDoubleMapPoint.x = mapRectPtr->left;
	sDoubleMapPoint.y = mapRectPtr->top;
	
	ConvertMapPointToWinPoint (&sDoubleMapPoint, 
											&upperLeftPoint,
											mapToWindowUnitsVariablesPtr);
	
			// Convert bottom right point.
													
	sDoubleMapPoint.x = mapRectPtr->right;
	sDoubleMapPoint.y = mapRectPtr->bottom;
	
	ConvertMapPointToWinPoint (&sDoubleMapPoint, 
											&lowerRightPoint,
											mapToWindowUnitsVariablesPtr);
											
	windowRectPtr->top = MIN (upperLeftPoint.v, lowerRightPoint.v);
	windowRectPtr->bottom = MAX (upperLeftPoint.v, lowerRightPoint.v);
	windowRectPtr->left = MIN (upperLeftPoint.h, lowerRightPoint.h);
	windowRectPtr->right = MAX (upperLeftPoint.h, lowerRightPoint.h);
	
}	// end "ConvertMapRectToWinRect"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertWinPointToMapPoint
//
//	Software purpose:	This routine converts the input window point coordinates to the
//							map point coordinates for the current window.
//								Currently not needed so commented out.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2006
//	Revised By:			Larry L. Biehl			Date: 02/23/2006		

void ConvertWinPointToMapPoint (
				LongPoint*							windowPointPtr,
				ArcViewDoublePoint*				mapPoint, 
				MapToWindowUnitsVariablesPtr	mapToWindowUnitsVariablesPtr)

{
	DoublePoint							windowDoublePoint;
	
	double								cosOrAngle,
											sinOrAngle,
											magnification,
											mapXOffset,
											mapXPoint,
											mapYOffset,
											mapYPoint,
											offset;
											
		
			// Allow for the map orientation if needed.
			
	mapXPoint = mapPoint->x;
	mapYPoint = mapPoint->y;
			
	if (mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag)
		{
		cosOrAngle = mapToWindowUnitsVariablesPtr->cosOrAngle;
		sinOrAngle = mapToWindowUnitsVariablesPtr->sinOrAngle;
		
		mapXOffset = mapXPoint - mapToWindowUnitsVariablesPtr->xMap11;
		mapYOffset = mapYPoint - mapToWindowUnitsVariablesPtr->yMap11;
		
		mapYOffset = (cosOrAngle*mapYOffset + 
											sinOrAngle*mapXOffset)/
											 (cosOrAngle*cosOrAngle + sinOrAngle*sinOrAngle);
											 
		mapXOffset = (mapXOffset - sinOrAngle*mapYOffset)/cosOrAngle;
		
		mapXPoint = mapToWindowUnitsVariablesPtr->xMap11 + mapXOffset;
		mapYPoint = mapToWindowUnitsVariablesPtr->yMap11 + mapYOffset;
		
		}	// end "if (mapToWindowUnitsVariablesPtr->useMapOrientationAngleFlag)"
		
	magnification = mapToWindowUnitsVariablesPtr->magnification;
	
			// Get the map rectangle relative to the offscreen pixmap	
			// for the x (column) part of rectangle											
			
	//windowDoublePoint.h = mapXPoint - mapToWindowUnitsVariablesPtr->mapXOrigin;
	mapXOffset = mapXPoint - mapToWindowUnitsVariablesPtr->mapXOrigin;
	windowDoublePoint.h = mapXOffset/mapToWindowUnitsVariablesPtr->xMapPixelSize;
	
			// Get the map rectangle relative to the offscreen pixmap	
			// for the y (line) part of rectangle																
	
	//windowDoublePoint.v = mapPoint->y - mapToWindowUnitsVariablesPtr->mapYOrigin;
	mapYOffset = mapYPoint - mapToWindowUnitsVariablesPtr->mapYOrigin;
	windowDoublePoint.v = mapYOffset/mapToWindowUnitsVariablesPtr->yMapPixelSize;
	
			// Now take into account the magnification and convert to integer
			// value
					
	offset = mapToWindowUnitsVariablesPtr->offset;
	if (offset < 0)
		{
		windowPointPtr->h = (SInt32)(windowDoublePoint.h * magnification + offset);
		windowPointPtr->v = (SInt32)(windowDoublePoint.v * magnification + offset);
		
		}	// end "if (offset < 0)"
		
	else	// offset >= 0
		{
		windowPointPtr->h = (SInt32)(windowDoublePoint.h * magnification + 
																SIGN (offset, windowDoublePoint.h));
		windowPointPtr->v = (SInt32)(windowDoublePoint.v * magnification + 
																SIGN (offset, windowDoublePoint.v));
		
		}	// end "else offset >= 0"
	
	windowPointPtr->h += mapToWindowUnitsVariablesPtr->imageLeftOffset;
	windowPointPtr->v += mapToWindowUnitsVariablesPtr->imageTopOffset;
	
			// This takes into account the possibility of a side by side	
			// channel display.															
			
	windowPointPtr->h += mapToWindowUnitsVariablesPtr->channelWindowOffset;

}	// end "ConvertWinPointToMapPoint" 
*/



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertMetersToFeet
//
//	Software purpose:	This routine converts the input linear measurements from meters
//							to the respective feet units if needed.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/07/2012
//	Revised By:			Larry L. Biehl			Date: 04/07/2012		

void ConvertMetersToFeet (
				SInt16								mapUnitsCode, 
				double*								xMeasurementPtr, 
				double*								yMeasurementPtr)

{		
	if (mapUnitsCode == kFeetCode)
		{				
		*xMeasurementPtr /= kFeetToMetersConversion;
		*yMeasurementPtr /= kFeetToMetersConversion;
		
		}	// end "if (mapUnitsCode == kFeetCode)"
		
	else if (mapUnitsCode == kUSSurveyFeetCode)
		{				
		*xMeasurementPtr /= kUSSurveyFeetToMetersConversion;
		*yMeasurementPtr /= kUSSurveyFeetToMetersConversion;
		
		}	// end "else if (mapUnitsCode == kUSSurveyFeetCode)"
	
}	// end "ConvertMetersToFeet" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertWinRectToMapRect
//
//	Software purpose:	This routine converts the input window rectangle coordinates to 
//							the correspong map rectangle coordinates.
//
//							Note that the window coordinates represent the "lines" between
//							the pixels not the pixel centers.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//							GetWindowMapRect in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/03/2001
//	Revised By:			Larry L. Biehl			Date: 06/22/2020

void ConvertWinRectToMapRect (
				Handle								windowInfoH,
				LongRect*							windowRectPtr, 
				DoubleRect*							boundingMapRectPtr, 
				Boolean								winUseOriginFlag,
				Boolean								force01MagnificationFlag,
				UInt16								boundingCode)

{
	DoubleRect							lineColumnRect,
											mapRect;
	
	double								magnification,
											originMapUnits,
											xIntervalSize,
											xMapCoordinate11,
											xPixelSize,
											yIntervalSize,
											yMapCoordinate11,
											yPixelSize;	
	
	DisplaySpecsPtr					displaySpecsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								displaySpecsH,
											mapProjectionHandle;
	
	SInt32								displayedColumnStart,
											displayedLineStart; 
											
	UInt32								displayedColumnInterval,
											displayedLineInterval;
	
	SInt16								imageTopOffset,
											legendWidth;
											
	
			// Get pointer to display specification.  We do not need to lock 	
			//	it here since no other routines are called							
	
	displaySpecsH = GetDisplaySpecsHandle (windowInfoH);
													
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsH);
	
			// Get point to file map projection specification. We do not need 
			// to lock it here since no other routines are called								
			
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoH);
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);

	if (displaySpecsPtr != NULL && mapProjectionInfoPtr != NULL)
		{
		magnification = displaySpecsPtr->magnification;
		if (force01MagnificationFlag)
			magnification = 1;
		
				// Get the v (or y) window offset.
		
		imageTopOffset = (SInt16)GetImageTopOffset (windowInfoH);
				// Get the h (or x) window offset.
		                         
		#if defined multispec_mac
			legendWidth = GetLegendWidth (windowInfoH);
		#endif	// defined multispec_mac          
		                         
		#if defined multispec_win || defined multispec_wx
					// Note that the legend in the Windows version is in its on
					// view. It is not the same as that used for the image. Therefore
					// the legend width is always 0 within the image view. However
					// if the legend width is being called for use in a edit copy
					// or print window, then the actual legend width needs to be
					// returned.
			legendWidth = GetLegendWidthForWindow (windowInfoH);
		#endif	// defined multispec_win || defined multispec_wx		
		
				// Initialize mapRect
				// This takes into account the possibility of a side by side	
				// channel display.				
		
		mapRect.left = windowRectPtr->left - legendWidth - gChannelWindowOffset;
		mapRect.right = windowRectPtr->right - legendWidth - gChannelWindowOffset;		
		
		mapRect.top = windowRectPtr->top - imageTopOffset;
		mapRect.bottom = windowRectPtr->bottom - imageTopOffset;	
					
		displayedColumnInterval = displaySpecsPtr->displayedColumnInterval;
		displayedColumnStart = (SInt32)displaySpecsPtr->displayedColumnStart;
		xMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;			
		
		displayedLineInterval = displaySpecsPtr->displayedLineInterval;
		displayedLineStart = (SInt32)displaySpecsPtr->displayedLineStart;
		yMapCoordinate11 = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
		
		
		if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
			{
					// Get the y or v (line) part of the map rectangle relative to the 
					// input top and bottom window values.					
					// We are assuming that the y map coordinate increases from north (top)
					// to south (bottom).											
				
			yPixelSize = -mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
				
			yIntervalSize = yPixelSize * displayedLineInterval;
			
			mapRect.top *= yIntervalSize/magnification;
			mapRect.bottom *= yIntervalSize/magnification;
		
			#if defined multispec_mac
				originMapUnits = displaySpecsPtr->origin[kVertical] * yIntervalSize;
				mapRect.top += originMapUnits;
				mapRect.bottom += originMapUnits;
			#endif	// defined multispec_mac                
		
			#if defined multispec_win || defined multispec_wx
				if (winUseOriginFlag)
					{               
					originMapUnits = displaySpecsPtr->origin[kVertical] * yIntervalSize;                                        
					mapRect.top += originMapUnits;
					mapRect.bottom += originMapUnits;
					
					}	// end "if (winUseOriginFlag)"
			#endif	// defined multispec_win || defined multispec_wx
			
			mapRect.top += yMapCoordinate11 + 
													((double)displayedLineStart - 1.5)*yPixelSize;
			mapRect.bottom += yMapCoordinate11 + 
													((double)displayedLineStart - 1.5)*yPixelSize;
			
			
					// Get the x or h (column) part of the map rectangle relative to the 
					// input left and right window values.						
					
			xPixelSize = mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
			xIntervalSize = xPixelSize * displayedColumnInterval;
			
			mapRect.left *= xIntervalSize/magnification;
			mapRect.right *= xIntervalSize/magnification;
		
			#if defined multispec_mac
				originMapUnits = displaySpecsPtr->origin[kHorizontal] * xIntervalSize;
				mapRect.left += originMapUnits;
				mapRect.right += originMapUnits;
			#endif	// defined multispec_mac                
		
			#if defined multispec_win || defined multispec_wx
				if (winUseOriginFlag)
					{                 
					originMapUnits = displaySpecsPtr->origin[kHorizontal] * xIntervalSize;                                      
					mapRect.left += originMapUnits;
					mapRect.right += originMapUnits;
					
					}	// end "if (winUseOriginFlag)"
			#endif	// defined multispec_win || defined multispec_wx
													
			mapRect.left += xMapCoordinate11 + 
										((double)displayedColumnStart - 1.5)*xPixelSize;
			mapRect.right += xMapCoordinate11 + 
										((double)displayedColumnStart - 1.5)*xPixelSize;
			
			if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0)
				GetBoundingOrientationAngleRect (&mapRect,
															&mapProjectionInfoPtr->planarCoordinate,
															boundingCode);
			
			}	// end "mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0"
			
		else	// mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0
			{		
			lineColumnRect = mapRect;
				
					// Get the column/line coordinates for the rectangle and then convert to
					// map coordinates using a polynomial transformation.
					
					// Get the x or h (column) part of the map rectangle relative to the 
					// input left and right window values.	
			
			lineColumnRect.left *= displayedColumnInterval/magnification;
			lineColumnRect.right *= displayedColumnInterval/magnification;				
		
			#if defined multispec_mac
				originMapUnits =
							displaySpecsPtr->origin[kHorizontal] * displayedColumnInterval;
				lineColumnRect.left += originMapUnits;
				lineColumnRect.right += originMapUnits;
			#endif	// defined multispec_mac                
		
			#if defined multispec_win || defined multispec_wx
				if (winUseOriginFlag)
					{                 
					originMapUnits =
							displaySpecsPtr->origin[kHorizontal] * displayedColumnInterval;
					lineColumnRect.left += originMapUnits;
					lineColumnRect.right += originMapUnits;
					
					}	// end "if (winUseOriginFlag)"
			#endif	// defined multispec_win || defined multispec_wx
													
			lineColumnRect.left += ((double)displayedColumnStart - 1.5);
			lineColumnRect.right += ((double)displayedColumnStart - 1.5);
					
					// Get the y or v (line) part of the map rectangle relative to the 
					// input top and bottom window values.	
			
			lineColumnRect.top *= displayedLineInterval/magnification;
			lineColumnRect.bottom *= displayedLineInterval/magnification;
		
			#if defined multispec_mac
				originMapUnits =
									displaySpecsPtr->origin[kVertical] * displayedLineInterval;
				lineColumnRect.top += originMapUnits;
				lineColumnRect.bottom += originMapUnits;
			#endif	// defined multispec_mac                
		
			#if defined multispec_win || defined multispec_wx
				if (winUseOriginFlag)
					{               
					originMapUnits =
									displaySpecsPtr->origin[kVertical] * displayedLineInterval;
					lineColumnRect.top += originMapUnits;
					lineColumnRect.bottom += originMapUnits;
					
					}	// end "if (winUseOriginFlag)"
			#endif	// defined multispec_win || defined multispec_wx
			
			lineColumnRect.top += ((double)displayedLineStart - 1.5);
			lineColumnRect.bottom += ((double)displayedLineStart - 1.5);
			
			ConvertLCRectToMapRect_TransformCoordinate (
								&mapProjectionInfoPtr->planarCoordinate,
								&lineColumnRect, 
								&mapRect,
								boundingCode);

			}	// end "else mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0"

		*boundingMapRectPtr = mapRect;
		
		}	// end "if (displaySpecsPtr != NULL && mapProjectionInfoPtr != NULL)" 

}	// end "ConvertWinRectToMapRect" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertPackedDegreesToDecimalDegrees
//
//	Software purpose:	This routine converts packed degrees (DDDMMMSSS.SS) to decimal
//							degrees.
//
//	Parameters in:		packed degrees value	
//
//	Parameters out:				
//
//	Value Returned:	decimal degrees value
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/22/2007
//	Revised By:			Larry L. Biehl			Date: 03/22/2007		

double ConvertPackedDegreesToDecimalDegrees (
				double								packedDegrees)

{ 
	double								decimalDegrees,
											degrees,
											minutes,
											seconds;
											
	
	seconds = fmod (packedDegrees, 1000);
	
	minutes = (double)((SInt32)(packedDegrees/1000));
	minutes = fmod (minutes, 1000);
	
	degrees = (double)((SInt32)(packedDegrees/1000000));
	
	decimalDegrees = degrees + minutes/60. + seconds/3600.;
	
	return (decimalDegrees);
	
}	// end "ConvertPackedDegreesToDecimalDegrees" 

											

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CoordinateDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for setting the coordinate mapping
//							parameters. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			OnEditImageMapParameters in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/16/1992
//	Revised By:			Larry L. Biehl			Date: 04/18/2019

Boolean CoordinateDialog (void)

{
	FileInfoPtr							fileInfoPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	SInt16								displayAreaUnitsIndex,
											displayPlanarMapUnitsIndex,
											fileMapUnitsCode,
											newAreaUnits,
											newCoordinatesUnits;
	
	Boolean								returnFlag;
	
	
	if (gActiveImageWindowInfoH == NULL)
																							return (FALSE);
 		
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (gActiveImageFileInfoH,
																	kLock);
	
			// Get a pointer to the map projection information if it exists.		
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);
											
#if defined multispec_mac
	UInt8									datumName[64];
	UInt8									ellipsoidName[64];
	UInt8									epsgName[64];
	
	DoublePoint							upperLeftLatLongPoint;
	
	Rect									theBox,
											theDatumPopupBox,
											theMapUnitsPopupBox,
											theEllipsoidPopupBox,
											theProjectionPopupBox,
											theReferenceSystemPopupBox;
	                   
	double								falseEasting,
											falseNorthing,
											horizontalPixelSize,
											latitudeOrigin,
											longitudeCentralMeridian,
											mapOrientationAngle,
											radiusSpheroid,
											realValue,
											scaleFactorOfCentralMeridian,
											semiMajorAxis,
											semiMinorAxis,
											standardParallel1,
											standardParallel2,
											verticalPixelSize,
											xMapCoordinate11,
											yMapCoordinate11;
											
	DialogPtr							dialogPtr;

	UserItemUPP							drawDatumPopUpPtr,
											drawEllipsoidPopUpPtr,
											drawMapUnitsPopUpPtr,
											drawProjectionPopUpPtr,
											drawReferenceSystemPopUpPtr;
											
	UInt8									gridZoneDirection[4];
	
	Handle								theHandle;
	
	SInt32								theNum;
	
	SInt16								decimalDigits,
											epsgCode,
											gridZone,
											gridZoneWithDirection,
											itemHit,
											maxZone,
											minZone,
											theType,
											value;
	
	Boolean								adjustUpperLeftMapPointFlag,
											//editParametersFlag,
											horizontalSizeChangedFlag,
											modalDone,
											okayFlag,
											xMapChangedFlag,
											yMapChangedFlag,
											validEPSGCodeFlag = TRUE,	
											verticalSizeChangedFlag;
	
	
			// Get the modal dialog for the coordinate window specifications.		
				
	dialogPtr = LoadRequestedDialog (kCoodinateDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)					
		{
		CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
		CheckAndUnlockHandle (gActiveImageFileInfoH);						
																							return (FALSE);
																							
		}	// end "if (dialogPtr == NULL)"
		
			// Intialize local user item proc pointers.	
			
	drawMapUnitsPopUpPtr = NewUserItemUPP (DrawMapUnitsPopUp);
	drawDatumPopUpPtr = NewUserItemUPP (DrawDatumPopUp);
	drawEllipsoidPopUpPtr = NewUserItemUPP (DrawEllipsoidPopUp);
	drawReferenceSystemPopUpPtr = NewUserItemUPP (DrawReferenceSystemPopUp);
	drawProjectionPopUpPtr = NewUserItemUPP (DrawProjectionPopUp);
	
			// Set Procedure pointers for those dialog items that need them.
	
	SetDialogItemDrawRoutine (dialogPtr, 
										5, 
										&theMapUnitsPopupBox, 
										drawMapUnitsPopUpPtr);		
	
	SetDialogItemDrawRoutine (dialogPtr, 
										IDC_ReferenceSystemCombo, 
										&theReferenceSystemPopupBox, 
										drawReferenceSystemPopUpPtr);
	
	SetDialogItemDrawRoutine (dialogPtr, 
										IDC_ProjectionCombo, 
										&theProjectionPopupBox, 
										drawProjectionPopUpPtr);
	
	SetDialogItemDrawRoutine (dialogPtr, 
										IDC_DatumCombo, 
										&theDatumPopupBox, 
										drawDatumPopUpPtr);	
	
	SetDialogItemDrawRoutine (dialogPtr, 
										IDC_EllipsoidCombo, 
										&theEllipsoidPopupBox, 
										drawEllipsoidPopUpPtr);	
	
			// Initialize dialog variables.
	
	CoordinateDialogInitialize (dialogPtr,
											mapProjectionInfoPtr,
											&gMapUnitsSelection,
											&xMapCoordinate11,
											&yMapCoordinate11,
											&horizontalPixelSize,
											&verticalPixelSize,
											&mapOrientationAngle,
											&gReferenceSystemSelection,
											&epsgCode,
											&gProjectionSelection,
											&gDatumSelection,
											&gEllipsoidSelection,
											&radiusSpheroid,
											&semiMajorAxis,
											&semiMinorAxis,
											&gridZone,
											gridZoneDirection,
											&gridZoneWithDirection,
											epsgName,
											datumName,
											ellipsoidName,
											&longitudeCentralMeridian,
											&latitudeOrigin,
											&falseEasting,
											&falseNorthing,
											&scaleFactorOfCentralMeridian,
											&standardParallel1,
											&standardParallel2,
											&upperLeftLatLongPoint);
	
			// Adjust selections for a popup menu selection that starts at
			// 1 not 0;
								
	xMapChangedFlag = FALSE;
	yMapChangedFlag = FALSE;	
	horizontalSizeChangedFlag = FALSE;
	verticalSizeChangedFlag = FALSE;
	
	decimalDigits = 4;
	if (gMapUnitsSelection-1 == kDecimalDegreesCode)
		decimalDigits = 12;
	else if (gMapUnitsSelection-1 == kKilometersCode)
		decimalDigits = 8;
	
			// x-map coordinate.																	
		
	LoadDItemRealValue (dialogPtr, 7, xMapCoordinate11, decimalDigits);
	
			// y-map coordinate.																	
		
	LoadDItemRealValue (dialogPtr, 9, yMapCoordinate11, decimalDigits);
	
			// Horizontal Pixel Size.																
		
	LoadDItemRealValue (dialogPtr, 11, horizontalPixelSize, decimalDigits);
	
			// Vertical Pixel Size.																
		
	LoadDItemRealValue (dialogPtr, 13, verticalPixelSize, decimalDigits);
	
			// Map Orientation Angle.													
		
	LoadDItemRealValue (dialogPtr, IDC_OrientationAngle, mapOrientationAngle, 6);
	
			// EPSG Code
	
	LoadDItemValue (dialogPtr, IDC_EPSGCode, epsgCode);
	
			// UTM Zone
	
	LoadDItemValue (dialogPtr, IDC_Zone, gridZone);
	
			// UTM Zone Direction
	
	LoadDItemString (dialogPtr, IDC_ZoneDirection, (Str255*)&gridZoneDirection);
	
			// State Plane Zone name
			// This is done in the initialize routine
	
	//LoadDItemString (dialogPtr, IDC_FIPSZoneName, (Str255*)&statePlaneZoneName);
	
			// Longitude of central meridian.												
		
	LoadDItemRealValue (dialogPtr, IDC_Longitude, longitudeCentralMeridian, 6);
	
			// Latitude of origin.																
		
	LoadDItemRealValue (dialogPtr, IDC_Latitude, latitudeOrigin, 6);
	
			// False Easting.																		
		
	LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseEasting, 2);
	
			// False Northing.																	
		
	LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseNorthing, 2);
	
			// Scale factor for central meridian.																	
		
	LoadDItemRealValue (dialogPtr, IDC_ScaleFactor, scaleFactorOfCentralMeridian, 10);
	
	LoadDItemRealValue (dialogPtr, IDC_StandardParallel1, standardParallel1, 6);
	
	LoadDItemRealValue (dialogPtr, IDC_StandardParallel2, standardParallel2, 6);
	
			// Radius of Spheroid (earth).													
	
	if (abs (gEllipsoidSelection) == kSphereEllipsoidMenuItem)
		LoadDItemRealValue (dialogPtr, IDC_Radius, radiusSpheroid, 4);
		
	else	// abs (gEllipsoidSelection) != kSphereEllipsoidMenuItem
		{
		LoadDItemRealValue (dialogPtr, IDC_Radius, semiMajorAxis, 4);
		LoadDItemRealValue (dialogPtr, IDC_MinorAxis, semiMinorAxis, 4);
		
		}	// end "else abs (gEllipsoidSelection) != kSphereEllipsoidMenuItem"
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kCoodinateDialogID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[7] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[9] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[11] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[13] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_OrientationAngle] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_ZoneDirection] = kDItemString;
	gDialogItemDescriptorPtr[IDC_Longitude] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_Latitude] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_FalseEasting] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_FalseNorthing] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_ScaleFactor] = kDItemReal;
	gDialogItemDescriptorPtr[IDC_StandardParallel1] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_StandardParallel2] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_Radius] = kDItemReal;
	gDialogItemDescriptorPtr[IDC_MinorAxis] = kDItemReal;
	gDialogItemDescriptorPtr[IDC_StandardParallel1] = kDItemReal + kDItemMinus;
	gDialogItemDescriptorPtr[IDC_StandardParallel2] = kDItemReal + kDItemMinus;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 7, 0, SHRT_MAX);
	
	adjustUpperLeftMapPointFlag = FALSE;
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case  5:	// Units popup menu
					if (gMapUnitsSelection > 0)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																4, 
																5,
																gPopUpMapUnitsMenu, 
																gMapUnitsSelection, 
																kPopUpMapUnitsMenuID);
						
						if (itemHit != 0 && gMapUnitsSelection != itemHit)
							{
							gMapUnitsSelection = itemHit;
							
							decimalDigits = 8;
							if (gMapUnitsSelection-1 == kDecimalDegreesCode)
								decimalDigits = 12;
							
									// Display the values with the proper decimal digits.
									
							xMapCoordinate11 = GetDItemRealValue (dialogPtr, 7);
							yMapCoordinate11 = GetDItemRealValue (dialogPtr, 9);
							horizontalPixelSize = GetDItemRealValue (dialogPtr, 11);
							verticalPixelSize = GetDItemRealValue (dialogPtr, 13);
			
							LoadDItemRealValue (
											dialogPtr, 7, xMapCoordinate11, decimalDigits);
							LoadDItemRealValue (
											dialogPtr, 9, yMapCoordinate11, decimalDigits);
							LoadDItemRealValue (
											dialogPtr, 11, horizontalPixelSize, decimalDigits);
							LoadDItemRealValue (
											dialogPtr, 13, verticalPixelSize, decimalDigits);
							
							}	// end "if (itemHit != 0 && gMapUnitsSelection != itemHit)"
			
								// Make certain that the correct label is drawn in the	
								// pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);	
						
						}	// end "if (gMapUnitsSelection > 0)"
					break;
					
				case  7:	// upper left x map point
					xMapChangedFlag = TRUE;		
					break;
					
				case  9:	// upper left y map point
					yMapChangedFlag = TRUE;	
					break;
					
				case  11: // horizontal map pixel size
					horizontalSizeChangedFlag = TRUE;				
					break;
					
				case  13: // vertical map pizel size
					verticalSizeChangedFlag = TRUE;				
					break;
					
				case IDC_OrientationAngle:				// Map orientation angle		
					realValue = GetDItemRealValue (dialogPtr, itemHit);
					if (realValue < -180 || realValue > 180)
						RealNumberErrorAlert (realValue, dialogPtr, 38, 6);
					break;
					
				case  IDC_ReferenceSystemCombo: // Reference System
					itemHit = StandardPopUpMenu (dialogPtr, 
															IDC_ReferenceSystemPrompt, 
															IDC_ReferenceSystemCombo, 
															gPopUpReferenceSystemMenu, 
															gReferenceSystemSelection, 
															kPopUpReferenceSystemMenuID);
					
					if (itemHit != 0 && gReferenceSystemSelection != itemHit)
						{
						gReferenceSystemSelection = itemHit;
						CoordinateDialogSetReferenceSystemParameters (
																		dialogPtr, 
																		gReferenceSystemSelection-1,
																		gridZoneDirection,
																		FALSE,
																		&gProjectionSelection,
																		&gDatumSelection,
																		&gEllipsoidSelection,
																		&gMapUnitsSelection);
		
								// Set the projection parameters based on the selected
								// reference system
								
						CoordinateDialogSetParametersFromRS (dialogPtr,
																			gReferenceSystemSelection-1,
																			&gProjectionSelection,
																			gridZoneDirection,
																			gridZoneWithDirection,
																			abs (gDatumSelection)-1,
																			abs (gEllipsoidSelection)-1,
																			&gridZone);	
																			
						if (gReferenceSystemSelection == kByEPSGCodeCode+1)												
							validEPSGCodeFlag = CoordinateDialogSetParametersFromEPSGCode (
																				dialogPtr,
																				epsgCode,
																				validEPSGCodeFlag,
																				epsgName,
																				datumName,
																				ellipsoidName,
																				&gMapUnitsSelection,
																				&gProjectionSelection);
						
						adjustUpperLeftMapPointFlag = TRUE;
		
								// Make certain that the correct label is drawn in possible	
								// pop up boxes.			
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theMapUnitsPopupBox);
						InvalWindowRect (GetDialogWindow (dialogPtr), &theProjectionPopupBox);
						InvalWindowRect (GetDialogWindow (dialogPtr), &theDatumPopupBox);
						InvalWindowRect (GetDialogWindow (dialogPtr), &theEllipsoidPopupBox);	
						
						}	// end "if (itemHit != 0)"
		
							// Make certain that the correct label is drawn in the	
							// pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);				
					break;
					
				case  IDC_EPSGCode:	// EPSG Code
					value = GetDItemValue (dialogPtr, itemHit);
												
					if (epsgCode != value)
						{
						epsgCode = value;
						
						validEPSGCodeFlag = CoordinateDialogSetParametersFromEPSGCode (
																					dialogPtr,
																					epsgCode,
																					validEPSGCodeFlag,
																					epsgName,
																					datumName,
																					ellipsoidName,
																					&gMapUnitsSelection,
																					&gProjectionSelection);
																			
						InvalWindowRect (GetDialogWindow (dialogPtr), &theMapUnitsPopupBox);
						InvalWindowRect (GetDialogWindow (dialogPtr), &theProjectionPopupBox);
						
						}	// end "if (epsgCode != value)"
					break;
					
				case  IDC_Zone:	// Grid Zone
					value = GetDItemValue (dialogPtr, itemHit);
					
							// Get min and max for reference system
						
					CoordinateDialogGetMinMaxZone (gReferenceSystemSelection-1,
															&minZone,
															&maxZone,
															gridZoneDirection);
												
					if (value >= minZone && value <= maxZone && gridZone != value)
						{
						gridZone = value;
						
						gridZoneWithDirection = gridZone;
						if (gridZoneDirection[1] == 'S')
							gridZoneWithDirection = -gridZone;
						CoordinateDialogSetParametersFromRS (dialogPtr,
																			gReferenceSystemSelection-1,
																			&gProjectionSelection,
																			gridZoneDirection,
																			gridZoneWithDirection,
																			abs (gDatumSelection)-1,
																			abs (gEllipsoidSelection)-1,
																			&gridZone);
																			
						InvalWindowRect (GetDialogWindow (dialogPtr), &theProjectionPopupBox);
																				
						}	// end "if (value >= minZone || value <= maxZone ...)"
					break;
					
				case  IDC_ZoneDirection:	// Northern or Southern Hemisphere
					GetDialogItemText (theHandle, gTextString);
					if (gTextString[0] > 1 ||
												(gTextString[1] != 'N' && gTextString[1] != 'S'))
						{
						okayFlag = FALSE;
						if (gTextString[0] == 1)
							{
							if (gTextString[1] == 'n')
								{
								gridZoneDirection[1] = 'N';
								okayFlag = TRUE;
								
								}	// end "if (gTextString[1] == 'n')"
								
							else if (gTextString[1] == 's')
								{
								gridZoneDirection[1] = 'S';
								okayFlag = TRUE;
								
								}	// end "else if (gTextString[1] == 's')"
								
							}	// end "if (gTextString[0] == 1)"
							
						SetDialogItemText (theHandle, (UCharPtr)&gridZoneDirection[0]);
						
						if (!okayFlag)
							{
							SelectDialogItemText (dialogPtr, IDC_ZoneDirection, 0, INT16_MAX);
							SysBeep (2);
							
							}	// end "if (!okayFlag)"
						
						}	// end "if (gTextString[0] > 1 || ..."
					
					else	// okay
						{
						gridZoneDirection[1] = gTextString[1];
						okayFlag = TRUE;
							
						}	// end "else okay"
					
					if (okayFlag)
						{
						gridZoneWithDirection = gridZone;
						if (gridZoneDirection[1] == 'S')
							gridZoneWithDirection = -gridZone;
						CoordinateDialogSetParametersFromRS (dialogPtr,
																			gReferenceSystemSelection-1,
																			&gProjectionSelection,
																			gridZoneDirection,
																			gridZoneWithDirection,
																			abs (gDatumSelection)-1,
																			abs (gEllipsoidSelection)-1,
																			&gridZone);
																				
						InvalWindowRect (GetDialogWindow (dialogPtr), &theProjectionPopupBox);
						
						}	// end "if (okayFlag)"
						
					break;
					
				case  IDC_ProjectionCombo: // Projection
					if (gProjectionSelection > 0)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																IDC_ProjectionPrompt, 
																IDC_ProjectionCombo, 
																gPopUpProjectionMenu, 
																gProjectionSelection, 
																kPopUpProjectionMenuID);
						
						if (itemHit != 0 && gProjectionSelection != itemHit)
							{
							gProjectionSelection = itemHit;
							CoordinateDialogHideShowProjectionParameters (
																			dialogPtr, 
																			gReferenceSystemSelection-1,
																			gProjectionSelection-1,
																			FALSE,
																			&gDatumSelection,
																			&gEllipsoidSelection,
																			FALSE);
							
							adjustUpperLeftMapPointFlag = TRUE;
							
							InvalWindowRect (GetDialogWindow (dialogPtr), &theDatumPopupBox);
							InvalWindowRect (
											GetDialogWindow (dialogPtr), &theEllipsoidPopupBox);
							
							}	// end "if (itemHit != 0 && ..."
			
								// Make certain that the correct label is drawn in the	
								// pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						}	// end "if (gProjectionSelection > 0)"
					break;
					
				case  IDC_DatumCombo: // Datum popup menu
					if (gDatumSelection > 0)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																IDC_DatumPrompt, 
																IDC_DatumCombo, 
																gPopUpHorizontalDatumMenu, 
																gDatumSelection, 
																kPopUpDatumMenuID);
						
						if (itemHit != 0)
							{
							gDatumSelection = itemHit;
							
							gEllipsoidSelection = CoordinateDialogSetDatumParameters (
																			dialogPtr,
																			gDatumSelection-1,
																			abs (gEllipsoidSelection)-1,
																			FALSE,
																			FALSE);
							
							adjustUpperLeftMapPointFlag = TRUE;
						
							InvalWindowRect (
											GetDialogWindow (dialogPtr), &theEllipsoidPopupBox);
							
							}	// end "if (itemHit != 0)"
			
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						}	// end "if (gDatumSelection > 0)"
					break;
					
				case  IDC_EllipsoidCombo: // Ellipsoid popup menu
					if (gEllipsoidSelection > 0)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																IDC_ProjectionEllipsoidPrompt, 
																IDC_EllipsoidCombo, 
																gPopUpEllipsoidMenu, 
																gEllipsoidSelection, 
																kPopUpEllipsoidMenuID);
						
						if (itemHit != 0 && gEllipsoidSelection != itemHit)
							{
							gEllipsoidSelection = itemHit;
		
							CoordinateDialogSetEllipsoidParameters (
																	dialogPtr, gEllipsoidSelection-1);
								
							}	// end "if (itemHit != 0 && gEllipsoidSelection != itemHit)"
			
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);	
						
						}	// end "if (gEllipsoidSelection > 0)"
					break;
					
				case IDC_Radius:				// Radius of spheroid		
					realValue = GetDItemRealValue (dialogPtr, itemHit);
					if (realValue <= 0)
						RealNumberErrorAlert (realValue, dialogPtr, IDC_Radius, 1);	
					
					adjustUpperLeftMapPointFlag = TRUE;
					break;
					
				case IDC_MinorAxis:				// Radius of spheroid		
					realValue = GetDItemRealValue (dialogPtr, itemHit);	
					
					adjustUpperLeftMapPointFlag = TRUE;
					break;
						
				}	// end "switch (itemHit)"
				
			}	// end "if (itemHit > 2)" 
			
		else if (itemHit > 0) 	// and itemHit <= 2 
			{
			if	(itemHit == 1)      // User selected OK for information 
				{
						// Verify that the grid zone parameter is within range if it is being
						// used.
						
				itemHit = CoordinateDialogCheckIfZoneIsValid (
																		dialogPtr,
																		gReferenceSystemSelection - 1,
																		gridZoneDirection);
						
				}	// end "if	(itemHit == 1)"
				
			if (itemHit > 0)
				modalDone = TRUE;
				
			if	(itemHit == 1)      // User selected OK for information 
				{
				returnFlag = TRUE;
				
				if (xMapChangedFlag)
					xMapCoordinate11 = GetDItemRealValue (dialogPtr, 7);
				
				if (yMapChangedFlag)
					yMapCoordinate11 = GetDItemRealValue (dialogPtr, 9);
				
				if (horizontalSizeChangedFlag)
					horizontalPixelSize = GetDItemRealValue (dialogPtr, 11);
				
				if (verticalSizeChangedFlag)
					verticalPixelSize = GetDItemRealValue (dialogPtr, 13);
				
				if (xMapChangedFlag || yMapChangedFlag)
					adjustUpperLeftMapPointFlag = FALSE;
				
				CoordinateDialogOK (dialogPtr,
											fileInfoPtr,
											mapProjectionInfoPtr,
											abs (gMapUnitsSelection)-1,
											xMapCoordinate11,
											yMapCoordinate11,
											horizontalPixelSize,
											verticalPixelSize,
											GetDItemRealValue (dialogPtr, IDC_OrientationAngle),
											gReferenceSystemSelection-1,
											epsgCode,
											abs (gProjectionSelection)-1,
											abs (gDatumSelection)-1,
											abs (gEllipsoidSelection)-1,
											GetDItemRealValue (dialogPtr, IDC_Radius),
											semiMajorAxis,
											semiMinorAxis,
											gridZone,
											gridZoneDirection,
											epsgName,
											datumName,
											ellipsoidName,
											GetDItemRealValue (dialogPtr, IDC_Longitude),
											GetDItemRealValue (dialogPtr, IDC_Latitude),
											GetDItemRealValue (dialogPtr, IDC_FalseEasting),
											GetDItemRealValue (dialogPtr, IDC_FalseNorthing),
											GetDItemRealValue (dialogPtr, IDC_ScaleFactor),
											GetDItemRealValue (dialogPtr, IDC_StandardParallel1),
											GetDItemRealValue (dialogPtr, IDC_StandardParallel2),
											upperLeftLatLongPoint,
											adjustUpperLeftMapPointFlag);
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				returnFlag = FALSE;
			
			}	// end "else if (itemHit > 0) and itemHit <= 2" 
				
		} while (!modalDone);
		
	DisposeUserItemUPP (drawMapUnitsPopUpPtr);
	DisposeUserItemUPP (drawDatumPopUpPtr);
	DisposeUserItemUPP (drawEllipsoidPopUpPtr);
	DisposeUserItemUPP (drawReferenceSystemPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
	
	#if defined multispec_win  
		CMMapCoordinateDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMMapCoordinateDlg ();
			
			returnFlag = dialogPtr->DoDialog (fileInfoPtr, mapProjectionInfoPtr);
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
	#endif // defined multispec_win  

	#if defined multispec_wx
		CMMapCoordinateDlg* dialogPtr = NULL;

		try
			{
			dialogPtr = new CMMapCoordinateDlg (NULL);
			returnFlag = dialogPtr->DoDialog (fileInfoPtr, mapProjectionInfoPtr);

			if (dialogPtr != NULL)
				delete dialogPtr;
			}

		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;    
			}
	#endif // defined multispec_wx 
	
	if (returnFlag && fileInfoPtr->mapProjectionHandle != NULL)
		{
				// This will update the coordinate view popup menus if needed.
		
		#if defined multispec_win
			CMImageView* imageViewCPtr = GetWindowPtr (gActiveImageWindowInfoH);
			CMImageFrame* 	imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
			imageFrameCPtr->OnViewCoordinatesBar (
							(Boolean)GetCoordinateHeight (gActiveImageWindowInfoH));
		#endif // defined multispec_win

		#if defined multispec_wx
			CMImageView* imageViewCPtr = GetWindowPtr (gActiveImageWindowInfoH);
			CMImageFrame* imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
			imageFrameCPtr->ShowCoordinateView (3);
		
			SetUpCoordinateUnitsPopUpMenu (NULL,
														gActiveImageWindowInfoH,
														imageFrameCPtr->m_coordinatesBar);
		#endif // defined multispec_wx
		
				// Update the coordinate parameters.
		
		newCoordinatesUnits = -1;
		newAreaUnits = -1;
		displayPlanarMapUnitsIndex = GetCoordinateViewUnits (gActiveImageWindowInfoH);
		displayAreaUnitsIndex = GetCoordinateViewAreaUnits (gActiveImageWindowInfoH);	
		
				// Get the map projection pointer again in case the map projection
				// structure was just added during the Dialog OK.
				
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	fileInfoPtr->mapProjectionHandle,
																	kLock);

		fileMapUnitsCode = mapProjectionInfoPtr->planarCoordinate.mapUnitsCode;
		
		if (fileMapUnitsCode == kUnknownCode)
			{
			if (displayPlanarMapUnitsIndex > kUnknownUnitsMenuItem)
				newCoordinatesUnits = kUnknownUnitsMenuItem;
				
			if (displayAreaUnitsIndex > kAreaUnknownUnitsMenuItem)
				newAreaUnits = kAreaUnknownUnitsMenuItem;
				
			}	// end "if (fileMapUnitsCode == kUnknownCode)"
		
		else if (fileMapUnitsCode == kDecimalDegreesCode)
			{
			if (displayPlanarMapUnitsIndex > kUnknownUnitsMenuItem)
				newCoordinatesUnits = kDecimalLatLongUnitsMenuItem;
				
			if (displayAreaUnitsIndex > kAreaUnknownUnitsMenuItem)
				newAreaUnits = kNumberPixelsUnitsMenuItem;
				
			}	// end "if (fileMapUnitsCode == kUnknownCode)"
		
		else if (fileMapUnitsCode >= kKilometersCode)
			{
			if (displayPlanarMapUnitsIndex == kUnknownUnitsMenuItem)
				newCoordinatesUnits = kMetersUnitsMenuItem;
				
			else if ((displayPlanarMapUnitsIndex == kDecimalLatLongUnitsMenuItem ||
							displayPlanarMapUnitsIndex == kDMSLatLongUnitsMenuItem) &&
										!DetermineIfLatLongPossible (gActiveImageWindowInfoH))
						// Cannot keep the lat-long display coordinate unit setting
						// force to meters
				newCoordinatesUnits = kMetersUnitsMenuItem;
				
			if (displayAreaUnitsIndex == kAreaUnknownUnitsMenuItem)
				newAreaUnits = kSqKilometersUnitsMenuItem;
				
			}	// end "if (fileMapUnitsCode == kUnknownCode)"
		
		if (newCoordinatesUnits < 0)
					// Then use the current setting
			newCoordinatesUnits = displayPlanarMapUnitsIndex;
			
		if (newCoordinatesUnits >= 0)
			{
			SetCoordinateViewUnits (gActiveImageWindowInfoH, newCoordinatesUnits);
			
			#if defined multispec_wx
				int coordinateUnitsSetting = newCoordinatesUnits;
			
				CMImageView* imageViewCPtr = GetWindowPtr (gActiveImageWindowInfoH);
				CMImageFrame* imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
				newCoordinatesUnits = GetComboListSelection (
												imageFrameCPtr->m_coordinatesBar,
												IDC_DisplayUnitsCombo,
												coordinateUnitsSetting);
			#endif
			
			if (newCoordinatesUnits >= 0)
				SetControlValue (GetCoordinateViewUnitsControl (gActiveImageWindowInfoH),
										newCoordinatesUnits);
			
			}	// and "if (newCoordinatesUnits >= 0)"
			
		if (newAreaUnits >= 0)
			{
			SetCoordinateViewAreaUnits (gActiveImageWindowInfoH, newAreaUnits);
			
			#if defined multispec_wx
				int areaUnitsSetting = newAreaUnits;
			
				CMImageView* imageViewCPtr = GetWindowPtr (gActiveImageWindowInfoH);
				CMImageFrame* imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
				newAreaUnits = GetComboListSelection (
															imageFrameCPtr->m_coordinatesBar,
															IDC_AreaUnitsCombo,
															areaUnitsSetting);
			#endif
			
			if (newAreaUnits >= 0)
				SetControlValue (
								GetCoordinateViewAreaUnitsControl (gActiveImageWindowInfoH),
								newAreaUnits);
			
			}	// and "if (newAreaUnits >= 0)"
			
				// Update the bounding map rectangle
			
		SetBoundingMapRectangle (gActiveImageWindowInfoH);
						
		SetCoordinateViewLocationParameters (gActiveImageWindowInfoH);
		UpdateSelectionCoordinates (gActiveImageWindowInfoH);
		
		#if defined multispec_mac  
			InvalidateWindow (gActiveImageWindow, kCoordinateSelectionArea, FALSE);	
			DrawCoordinateViewControls (gActiveImageWindowInfoH);
		#endif // defined multispec_mac 
		
		if (GetNumberImageFiles (gActiveImageWindowInfoH) > 0)
			InvalidateWindow (gActiveImageWindow, kImageFrameArea, FALSE);
		
		}	// end "if (returnFlag && ...)"
	
	CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
	CheckAndUnlockHandle (gActiveImageFileInfoH);
	
	return (returnFlag);
	
}	// end "CoordinateDialog"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogActivateProjectionParameters
//
//	Software purpose:	The purpose of this routine is to activate the projection
//							parameters so that the user may edit them. This is done when
//							the reference system is in user defined mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2012
//	Revised By:			Larry L. Biehl			Date: 03/19/2012	

void CoordinateDialogActivateProjectionParameters (
				DialogPtr							dialogPtr)

{
	SetDialogItemToEditText (dialogPtr, IDC_Longitude);
	SetDialogItemToEditText (dialogPtr, IDC_Latitude);
	SetDialogItemToEditText (dialogPtr, IDC_FalseEasting);
	SetDialogItemToEditText (dialogPtr, IDC_FalseNorthing);
	SetDialogItemToEditText (dialogPtr, IDC_ScaleFactor);
	SetDialogItemToEditText (dialogPtr, IDC_StandardParallel1);
	SetDialogItemToEditText (dialogPtr, IDC_StandardParallel2);
						
}	// end "CoordinateDialogActivateProjectionParameters"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CoordinateDialogCheckIfZoneIsValid
//
//	Software purpose:	The purpose of this routine is to check the the entered
//							zone is within the correct range or a valid state plane 
//							FIPS zone.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

SInt16 CoordinateDialogCheckIfZoneIsValid (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				UCharPtr								gridZoneDirection)

{	
	SInt16								alertStringNumber,
											maxZone,
											minZone,
											returnCode = 1,
											selectItem;
	
	
	if (CoordinateDialogIsZoneDisplayed (referenceSystemCode) || 
											referenceSystemCode == kByEPSGCodeCode)
		{	
				// If an alert has been displayed about the missing EPSG csv folder,
				// then we will not be able to determine if whether the state plane
				// code is correct.				

		if (!gEPSG_csv_FolderAlertDisplayedFlag &&
						(referenceSystemCode == kStatePlaneNAD27RSCode ||
								referenceSystemCode == kStatePlaneNAD83RSCode ||
											referenceSystemCode == kByEPSGCodeCode))
			{
			GetMultiSpecDialogItemText (dialogPtr,
													IDC_FIPSZoneName,
													(unsigned char*)gTextString);

			if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"Invalid", 7) == 0)
				{
				alertStringNumber = IDS_Alert143;
				selectItem = IDC_Zone;
				if (referenceSystemCode == kByEPSGCodeCode)
					{
					alertStringNumber = IDS_Alert145;
					selectItem = IDC_EPSGCode;
					
					}	// end "if (referenceSystemCode == kByEPSGCodeCode)"
				
				SetDLogControlHilite (dialogPtr, 1, 255);
				DisplayAlert (
						kErrorAlertID, kStopAlert, kAlertStrID, alertStringNumber, 0, NULL);
				SetDLogControlHilite (dialogPtr, 1, 0);
				
				SelectDialogItemText (dialogPtr, selectItem, 0, SHRT_MAX);

				returnCode = 0;
										
				}	// end "if (CompareStringsNoCase (..."
			
			}	// end "if (referenceSystemCode == kStatePlaneNAD27RSCode || ..."
			
		else	// referenceSystemCode != statePlane
			{
			CoordinateDialogGetMinMaxZone (referenceSystemCode,
														&minZone,
														&maxZone,
														gridZoneDirection);
													
			returnCode = CheckMaxValue (dialogPtr, 
												IDC_Zone, 
												minZone, 
												maxZone,
												kDisplayRangeAlert);
												
			}	// end "else referenceSystemCode != statePlane"
		
		}	// end "if (CoordinateDialogIsZoneDisplayed (referenceSystemCode))"
		
	return (returnCode);
						
}	// end "CoordinateDialogCheckIfZoneIsValid"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogDeactivateProjectionParameters
//
//	Software purpose:	The purpose of this routine is to deactivate the projection
//							parameters so that the user may not edit them. This is done when
//							the reference system is not in user defined mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/18/2012
//	Revised By:			Larry L. Biehl			Date: 03/19/2012	

void CoordinateDialogDeactivateProjectionParameters (
				DialogPtr							dialogPtr)

{
	SetDialogItemToStaticText (dialogPtr, IDC_Longitude);
	SetDialogItemToStaticText (dialogPtr, IDC_Latitude);
	SetDialogItemToStaticText (dialogPtr, IDC_FalseEasting);
	SetDialogItemToStaticText (dialogPtr, IDC_FalseNorthing);
	SetDialogItemToStaticText (dialogPtr, IDC_ScaleFactor);
	SetDialogItemToStaticText (dialogPtr, IDC_StandardParallel1);
	SetDialogItemToStaticText (dialogPtr, IDC_StandardParallel2);
	
}	// end "CoordinateDialogDeactivateProjectionParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SIn16 CoordinateDialogGetCodeFromSelection
//
//	Software purpose:	The purpose of this routine is to get the code from the
//							input menu selection. This routine will handle the differences
//							in the menu selection number between Mac and Windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2012
//	Revised By:			Larry L. Biehl			Date: 03/27/2012	

SInt16 CoordinateDialogGetCodeFromSelection (
				SInt16								selection)

{	
	SInt16								code;
	
	
	#if defined multispec_mac
		code = abs (selection) - 1;
	#endif	// defined multispec_mac   
                             
	#if defined multispec_win || defined multispec_wx
				// Allow for Windows 0 based menu selections.
		code = abs (selection);
	#endif	// defined multispec_win || defined multispec_wx

	return (code);
	
	
}	// end "CoordinateDialogGetCodeFromSelection"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogGetMinMaxZone
//
//	Software purpose:	The purpose of this routine is to get the min and max zone
//							numbers for the input reference system. The values in the min
//							and max pointers will not be changed if there is no zone 
//							associated with the input reference system.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void CoordinateDialogGetMinMaxZone (
				SInt16								referenceSystemCode,
				SInt16*								minZonePtr,
				SInt16*								maxZonePtr,
				UCharPtr								gridZoneDirectionPtr)

{	
	switch (referenceSystemCode)
		{
		case kGaussKrugerRSCode:
			*minZonePtr = 1;
			*maxZonePtr = 32;
			gridZoneDirectionPtr[1] = 'N';
			break;
			
		case kGDA94RSCode:
			*minZonePtr = 48;
			*maxZonePtr = 58;
			gridZoneDirectionPtr[1] = 'S';
			break;
			
		case kStatePlaneNAD27RSCode:
			*minZonePtr = 101;
			*maxZonePtr = 5010;
			gridZoneDirectionPtr[1] = 'N';
			break;
			
		case kStatePlaneNAD83RSCode:
			*minZonePtr = 101;
			*maxZonePtr = 5010;
			gridZoneDirectionPtr[1] = 'N';
			break;
			
		case kUTM_NAD27RSCode:
			*minZonePtr = 3;
			*maxZonePtr = 22;
			gridZoneDirectionPtr[1] = 'N';
			break;
			
		case kUTM_NAD83RSCode:
			*minZonePtr = 3;
			*maxZonePtr = 23;
			gridZoneDirectionPtr[1] = 'N';
			break;
			
		case kUTM_SAD69RSCode:
			*minZonePtr = 18;
			*maxZonePtr = 25;
			break;
			
		case kUTM_WGS72RSCode:
			*minZonePtr = 1;
			*maxZonePtr = 60;
			break;
			
		case kUTM_WGS84RSCode:
			*minZonePtr = 1;
			*maxZonePtr = 60;
			break;
			
		case kUTMRSCode:
			*minZonePtr = 1;
			*maxZonePtr = 60;
			break;
			
		}	// end "switch (referenceSystemCode)"
	
}	// end "CoordinateDialogGetMinMaxZone" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SIn16 CoordinateDialogGetSelectionFromCode
//
//	Software purpose:	The purpose of this routine is to get the menu selection from the
//							input parameter code. This routine will handle the differences
//							in the menu selection number between Mac and Windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/28/2012
//	Revised By:			Larry L. Biehl			Date: 03/28/2012	

SInt16 CoordinateDialogGetSelectionFromCode (
				SInt16								code)

{	
	SInt16								selection;
	
	
	#if defined multispec_mac
		selection = code + 1;
	#endif	// defined multispec_mac   
                             
	#if defined multispec_win || defined multispec_wx
				// Allow for Windows & Linux 0 based menu selections.
		selection = code;
	#endif	// defined multispec_win || defined multispec_wx

	return (selection);
	
}	// end "CoordinateDialogGetSelectionFromCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CoordinateDialogHandleInvalidValidEPSGCode
//
//	Software purpose:	The purpose of this routine is to set the parameters in the
//							dialog box to the state for an invalid or valid epsg code. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/14/2012
//	Revised By:			Larry L. Biehl			Date: 03/09/2019

Boolean CoordinateDialogHandleInvalidValidEPSGCode (
				DialogPtr							dialogPtr,
				SInt16								epsgCode,
				Boolean								validEPSGCodeFromGDALFlag,
				Boolean								lastEPSGCodeValidFlag)

{	
	Boolean								EPSGCodeValidFlag = FALSE;
	
	
	if (epsgCode >= 2000 && epsgCode <= 32766 && validEPSGCodeFromGDALFlag)
		{
		if (!lastEPSGCodeValidFlag)
			{
			ShowDialogItem (dialogPtr, IDC_ProjectionPrompt);
			#if defined multispec_mac
				ShowDialogItem (dialogPtr, IDC_ProjectionCombo);
			#endif	// defined multispec_mac
			#if defined multispec_win || defined multispec_wx
				ShowDialogItem (dialogPtr, IDC_ProjectionName);
			#endif	// defined multispec_win || defined multispec_wx
			ShowDialogItem (dialogPtr, IDC_DatumPrompt);
			ShowDialogItem (dialogPtr, IDC_DatumName);
			ShowDialogItem (dialogPtr, IDC_ProjectionEllipsoidPrompt);
			ShowDialogItem (dialogPtr, IDC_EllipsoidName);
			ShowDialogItem (dialogPtr, IDC_MajorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_Radius);
			ShowDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_MinorAxis);
			
			SetDLogControlHilite (dialogPtr, IDOK, 0);
			
			}			// end "if (!lastEPSGCodeValidFlag)"
			
		EPSGCodeValidFlag = TRUE;
			
		}	// end "if (epsgCode >= 2000 && epsgCode <= 32766 && ..."
		
	else	// !validEPSGCodeFlag
		{
		if (lastEPSGCodeValidFlag)
			{
			HideDialogItem (dialogPtr, IDC_ProjectionPrompt);
			HideDialogItem (dialogPtr, IDC_ProjectionCombo);
			#if defined multispec_win || defined multispec_wx
				HideDialogItem (dialogPtr, IDC_ProjectionName);
			#endif	// defined multispec_win || defined multispec_wx
			HideDialogItem (dialogPtr, IDC_DatumPrompt);
			HideDialogItem (dialogPtr, IDC_DatumName);
			HideDialogItem (dialogPtr, IDC_ProjectionEllipsoidPrompt);
			HideDialogItem (dialogPtr, IDC_EllipsoidName);
			
			#if !defined multispec_wx
				HideDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			HideDialogItem (dialogPtr, IDC_MajorAxisPrompt);
			HideDialogItem (dialogPtr, IDC_Radius);
			HideDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			HideDialogItem (dialogPtr, IDC_MinorAxis);
			
			CoordinateDialogHideShowProjectionParameters (dialogPtr, 
																		kByEPSGCodeCode, 
																		kNotDefinedCode,
																		FALSE,
																		NULL,
																		NULL,
																		FALSE);
			
			//SetDLogControlHilite (dialogPtr, IDOK, 255);
			
					// Display invalid epsg code message
					
			LoadDItemStringNumber (kDialogStrID, 
											IDS_Dialog32, 
											dialogPtr, 
											IDC_FIPSZoneName, 
											(Str255*)gTextString);
											
			}	// end "if (lastEPSGCodeValidFlag)"
		
		}	// end "else !validEPSGCodeFlag"
		
	return (EPSGCodeValidFlag);
	
}	// end "CoordinateDialogHandleInvalidValidEPSGCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogHideShowProjectionParameters
//
//	Software purpose:	The purpose of this routine is to show the projection parameters
//							for the specified projection. The datum will change if specific
//							projections like Orthographic are selected. This will only occur
//							when the user makes a selection on the projection popup menu.
//							This is signified with datumSelectionPtr and
//							ellipsoidSelectionPtr not being NULL
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2000
//	Revised By:			Larry L. Biehl			Date: 03/15/2019

void CoordinateDialogHideShowProjectionParameters (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				SInt16								projectionCode,
				Boolean								initialFlag,
				SInt16*								datumSelectionPtr,
				SInt16*								ellipsoidSelectionPtr,
				Boolean								setDatumParametersFlag)

{
	SInt16								datumCode,
											ellipsoidCode;
	
	
	if (setDatumParametersFlag)
		datumCode = CoordinateDialogGetCodeFromSelection (abs (*datumSelectionPtr));
			
	if (referenceSystemCode == kGeographicRSCode)
		projectionCode = kGeographicCode;
		
	switch (projectionCode)
		{
		case kGeographicCode:
		case kNotDefinedCode:
		case kAzimuthalEquidistantCode:
		case kObliqueMercatorCode:
		case kSpaceObliqueMercatorCode:
		case kModifiedTransverseMercatorCode:
		case kGeneralVerticalNearSideCode:
		case kKrovakCode:
		default:
			HideDialogItem (dialogPtr, IDC_LongitudePrompt);
			HideDialogItem (dialogPtr, IDC_Longitude);
			HideDialogItem (dialogPtr, IDC_LatitudePrompt);
			HideDialogItem (dialogPtr, IDC_Latitude);
			HideDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			HideDialogItem (dialogPtr, IDC_FalseEasting);
			HideDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			HideDialogItem (dialogPtr, IDC_FalseNorthing);
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel1);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			break;		
			
		case kAlaskaConformalCode:
			HideDialogItem (dialogPtr, IDC_LongitudePrompt);
			HideDialogItem (dialogPtr, IDC_Longitude);
			HideDialogItem (dialogPtr, IDC_LatitudePrompt);
			HideDialogItem (dialogPtr, IDC_Latitude);
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel1);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			
			ShowDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseEasting);
			ShowDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseNorthing);
			break;
		
		case kGnomonicCode:
		case kLambertAzimuthalEqualAreaCode:
		case kMercatorCode:
		case kOrthographicCode:
		case kPolarStereographicCode:
		case kPolyconicCode:
		case kStereographicCode:
		case kVanderGrintenICode:
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel1);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			ShowDialogItem (dialogPtr, IDC_LatitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Latitude);
			ShowDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseEasting);
			ShowDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseNorthing);
			break;
			
		case kTransverseMercatorCode:
		case kTransvMercatorSouthOrientedCode:	
			HideDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel1);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
		
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			ShowDialogItem (dialogPtr, IDC_LatitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Latitude);
			ShowDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseEasting);
			ShowDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseNorthing);
			ShowDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			ShowDialogItem (dialogPtr, IDC_ScaleFactor);
			break;	
		
		case kCylindricalEqualAreaCode:
			HideDialogItem (dialogPtr, IDC_LatitudePrompt);
			HideDialogItem (dialogPtr, IDC_Latitude);
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			ShowDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			ShowDialogItem (dialogPtr, IDC_StandardParallel1);
			ShowDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseEasting);
			ShowDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseNorthing);
			break;
		
		case kSinusoidalCode:
			HideDialogItem (dialogPtr, IDC_LatitudePrompt);
			HideDialogItem (dialogPtr, IDC_Latitude);
			HideDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			HideDialogItem (dialogPtr, IDC_FalseEasting);
			HideDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			HideDialogItem (dialogPtr, IDC_FalseNorthing);
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel1);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			break;
		
		case kEquirectangularCode:
			HideDialogItem (dialogPtr, IDC_LatitudePrompt);
			HideDialogItem (dialogPtr, IDC_Latitude);
			HideDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			HideDialogItem (dialogPtr, IDC_FalseEasting);
			HideDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			HideDialogItem (dialogPtr, IDC_FalseNorthing);
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			ShowDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			ShowDialogItem (dialogPtr, IDC_StandardParallel1);
			break;
			
		case kAlbersConicalEqualAreaCode:
		case kEquidistantConicCode:
		case kLambertConformalConicCode:
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			ShowDialogItem (dialogPtr, IDC_LatitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Latitude);
			ShowDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseEasting);
			ShowDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseNorthing);
			ShowDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			ShowDialogItem (dialogPtr, IDC_StandardParallel1);
			ShowDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			ShowDialogItem (dialogPtr, IDC_StandardParallel2);
			break;
			
		case kHammerCode:
		case kMillerCylindricalCode:
		case kMollweideCode:
		case kRobinsonCode:
		case kWagnerIVCode:
		case kWagnerVIICode:
			HideDialogItem (dialogPtr, IDC_LatitudePrompt);
			HideDialogItem (dialogPtr, IDC_Latitude);
			HideDialogItem (dialogPtr, IDC_ScaleFactorPrompt);
			HideDialogItem (dialogPtr, IDC_ScaleFactor);
			HideDialogItem (dialogPtr, IDC_StandardParallel1Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel1);
			HideDialogItem (dialogPtr, IDC_StandardParallel2Prompt);
			HideDialogItem (dialogPtr, IDC_StandardParallel2);
			
			ShowDialogItem (dialogPtr, IDC_LongitudePrompt);
			ShowDialogItem (dialogPtr, IDC_Longitude);
			ShowDialogItem (dialogPtr, IDC_FalseEastingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseEasting);
			ShowDialogItem (dialogPtr, IDC_FalseNorthingPrompt);
			ShowDialogItem (dialogPtr, IDC_FalseNorthing);
			break;
			
		}	// end "switch (projectionCode)"
		
	switch (projectionCode)
		{
		case kGnomonicCode:
		case kMillerCylindricalCode:
		case kMollweideCode:
		case kOrthographicCode:
		case kVanderGrintenICode:
			datumCode = kSphereDatumCode;
			setDatumParametersFlag = TRUE;
			break;
			
		}	// end "switch (projectionCode)"
		
	if (initialFlag && datumCode == kSphereDatumCode)
		{
				// If this is for the initial display and the projection is only for a
				// sphere and the input datum is for a sphere, then do not allow the
				// datum popup menu to be selectable.
				
		datumCode = CoordinateDialogGetCodeFromSelection (*datumSelectionPtr);
		if (datumCode == kSphereDatumCode)
			*datumSelectionPtr = -CoordinateDialogGetSelectionFromCode (datumCode);
		
		}	// end "if (initialFlag && ..."
		
	if (datumSelectionPtr != NULL && ellipsoidSelectionPtr != NULL)
		{
		if (setDatumParametersFlag)
			{
			ellipsoidCode = CoordinateDialogGetCodeFromSelection (
																		abs (*ellipsoidSelectionPtr));
			
			*ellipsoidSelectionPtr = CoordinateDialogSetDatumParameters (
																			dialogPtr,
																			datumCode,
																			ellipsoidCode,
																			initialFlag,
																			FALSE);
																			
			*datumSelectionPtr = -CoordinateDialogGetSelectionFromCode (datumCode);
			
			}	// end "if (setDatumParametersFlag)"
			
		else	// !setDatumParametersFlag
			*datumSelectionPtr = abs (*datumSelectionPtr);

		#if defined multispec_win || defined multispec_wx
			dialogPtr->SetStaticTextOrDropDownList (abs (*datumSelectionPtr),
																	IDC_DatumCombo, 
																	IDC_DatumName,
																	*datumSelectionPtr<0);
		#endif	// defined multispec_win || defined multispec_wx
															
		}	// end "if (datumSelectionPtr != NULL && ellipsoidSelectionPtr != NULL)"
		
}	// end "CoordinateDialogHideShowProjectionParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the dialog box
//							parameters based on the current map projection structure settings. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2000
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void CoordinateDialogInitialize (
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
				DoublePoint*						upperLeftLatLongPointPtr)

{
	SInt16								referenceSystemCode;
	
	
	if (mapProjectionInfoPtr != NULL)
		{
				// Map units code.																
			
		*mapUnitsSelectionPtr = mapProjectionInfoPtr->planarCoordinate.mapUnitsCode + 1;
			
				// Area units code.																
			
		//*areaUnitsSelectionPtr = mapProjectionInfoPtr->planarCoordinate.areaUnitsCode;
		
				// x-map coordinate.																	
			
		*xMapCoordinate11Ptr = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
		
				// y-map coordinate.																	
			
		*yMapCoordinate11Ptr = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
		
				// Get the current latitude and longitude of the upper left point in case 
				// it is needed later.
		
		upperLeftLatLongPointPtr->h = *xMapCoordinate11Ptr;
		upperLeftLatLongPointPtr->v = *yMapCoordinate11Ptr;
		
		ConvertMapPointToLatLongPoint (mapProjectionInfoPtr,
												 upperLeftLatLongPointPtr);
									
				// Vertical Pixel Size.																
			
		*verticalPixelSizePtr = 
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
		
				// Horizontal Pixel Size.																
			
		*horizontalPixelSizePtr = 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		
				// Map Orientation Angle.																
			
		*mapOrientationAnglePtr =
			mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle * kRadiansToDegrees;
	
				// Reference System code selection.											
			
		referenceSystemCode = mapProjectionInfoPtr->gridCoordinate.referenceSystemCode;
		
				// EPSG Code
				
		*epsgCodePtr = mapProjectionInfoPtr->projectedCSTypeGeoKey;
	
				// Projection code selection.											
			
		*projectionSelectionPtr = mapProjectionInfoPtr->gridCoordinate.projectionCode + 1;
	
				// Grid zone selection.											
			
		*gridZonePtr = abs (mapProjectionInfoPtr->gridCoordinate.gridZone);
		
				// EPSG Name
		
		CopyPToP (epsgNamePtr, mapProjectionInfoPtr->gridCoordinate.epsgName);
		
				// Datum Name
		
		CopyPToP (datumNamePtr, mapProjectionInfoPtr->gridCoordinate.datumName);
		
				// Ellipsoid Name
		
		CopyPToP (ellipsoidNamePtr, mapProjectionInfoPtr->gridCoordinate.ellipsoidName);
		
				// Longitude of central meridian.
			
		*longitudeCentralMeridianPtr = 
								mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
		
				// Latitude of origin.																
			
		*latitudeOriginPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
		
		if (mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																			kLambertConformalConicCode)
			{
			*longitudeCentralMeridianPtr = 
								mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude;								
			
			*latitudeOriginPtr = mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude;
			
			}	// end "if (...->gridCoordinate.code == kLambertConformalConicCode)"
		
				// False Easting and Northing.				
				//		The false easting and northing is always stored in meters.
				//		Convert to feet if that is what the planar units are in.														
			
		*falseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
		*falseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
		
		ConvertMetersToFeet (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										falseEastingPtr, 
										falseNorthingPtr);
		
				// Scale factor of central meridian.																	
			
		*scaleFactorOfCentralMeridianPtr = 
							mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian;
				
		*standardParallel1Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel1;
				
		*standardParallel2Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel2;
	
				// Datum code selection.											
			
		*datumSelectionPtr = mapProjectionInfoPtr->geodetic.datumCode + 1;
	
				// Sheroid code selection.											
			
		*ellipsoidSelectionPtr = mapProjectionInfoPtr->geodetic.spheroidCode + 1;
		
				// Radius of Spheroid (earth).													
			
		*radiusSpheroidPtr = mapProjectionInfoPtr->geodetic.radiusSpheroid;
		
				// Radius of semi major axis of earth ellipsoid.													
			
		*semiMajorAxisPtr = mapProjectionInfoPtr->geodetic.semiMajorAxis;
		
				// Radius of semi minor axis of earth ellipsoid.													
			
		*semiMinorAxisPtr = mapProjectionInfoPtr->geodetic.semiMinorAxis;
			
		}	// end "if (mapProjectionInfoPtr != NULL)"
		
	else	// mapProjectionInfoPtr == NULL
		{
				// Map units code.																
			
		*mapUnitsSelectionPtr = kUnknownCode + 1;
			
				// Area units code.																
			
		//*areaUnitsSelectionPtr = 0;
		
				// x-map coordinate.																	
			
		*xMapCoordinate11Ptr = kXMapCoordinate11;		// -2050000.0
		
				// y-map coordinate.																	
			
		*yMapCoordinate11Ptr = kYMapCoordinate11;		// 752000.0
									
				// Vertical Pixel Size.																
			
		*verticalPixelSizePtr = kVerticalPixelSize;	// 1000.000
		
				// Horizontal Pixel Size.																
			
		*horizontalPixelSizePtr = kHorizontalPixelSize;	//1000.000
		
				// Map Orientation Angle.																
			
		*mapOrientationAnglePtr = 0;
	
				// Reference System code selection.											
			
		referenceSystemCode = kNoRSDefinedCode;
		
				// EPSG Code
				
		*epsgCodePtr = 0;
	
				// Grid map code selection.											
			
		*projectionSelectionPtr = kNoneDefinedMenuItem;
	
				// Grid zone selection.											
			
		*gridZonePtr = 0;
		
		*gridZoneWithDirectionPtr = 0;
		
		epsgNamePtr[0] = 0;
		epsgNamePtr[1] = 0;
		datumNamePtr[0] = 0;
		datumNamePtr[1] = 0;
		ellipsoidNamePtr[0] = 0;
		ellipsoidNamePtr[1] = 0;
		
				// Longitude of central meridian.												
			
		*longitudeCentralMeridianPtr = kCentralMeridian;
		
				// Latitude of origin.																
			
		*latitudeOriginPtr = kOrigin;
		
				// False Easting.																		
			
		*falseEastingPtr = kFalseEasting;
		
				// False Northing.																	
			
		*falseNorthingPtr = kFalseNorthing;
		
				// False Northing.																	
			
		*scaleFactorOfCentralMeridianPtr = 1.0;
				
		*standardParallel1Ptr = 0;
				
		*standardParallel2Ptr = 0;
	
				// Datum code selection.											
			
		*datumSelectionPtr = 1;
	
				// Sheroid code selection.											
			
		*ellipsoidSelectionPtr = 1;
		
				// Radius of Spheroid (earth).													
			
		*radiusSpheroidPtr = kEarthRadius1;
		
				// Radius of semi major axis of earth ellipsoid.													
			
		*semiMajorAxisPtr = kEarthRadius1;
		
				// Radius of semi minor axis of earth ellipsoid.													
			
		*semiMinorAxisPtr = kEarthRadius1;
			
		}	// end "if (mapProjectionInfoPtr != NULL)"
	
			// Set a default grid zone direction. It will be updated with the correct one
			// in CoordinateDialogSetReferenceSystemParameters							
	
	gridZoneDirectionPtr[0] = 1;
	gridZoneDirectionPtr[1] = 'N';
	gridZoneDirectionPtr[2] = 0;

	LoadDItemString (dialogPtr, IDC_FIPSZoneName, (Str255*)epsgNamePtr);
		
	//SetUpReferenceSystemPopUpMenu (dialogPtr);
		
	//SetUpProjectionPopUpMenu (dialogPtr);
			
	//SetUpHorizontalDatumPopUpMenu (dialogPtr);
			
	//SetUpEllipsoidPopUpMenu (dialogPtr);
	
	#if defined multispec_mac
		*referenceSystemSelectionPtr = referenceSystemCode + 1;
	#endif	// defined multispec_mac   
                             
	#if defined multispec_win || defined multispec_wx
				// Adjust for menu selections for Windows 0 based system.
		(*mapUnitsSelectionPtr)--;
		*referenceSystemSelectionPtr = referenceSystemCode;
		(*projectionSelectionPtr)--;
		(*datumSelectionPtr)--;
		(*ellipsoidSelectionPtr)--;
	#endif	// defined multispec_win || defined multispec_wx

			// Set up the dialog items for the specified reference system. Note that
			// we do not want to cause changes in the initial settings for the projection,
			// datum and ellipsoid. Want to know if there are miss matches in the first
			// display of the dialog box.
	
	CoordinateDialogSetReferenceSystemParameters (dialogPtr,
																	referenceSystemCode,
																	gridZoneDirectionPtr,
																	TRUE,
																	projectionSelectionPtr,
																	datumSelectionPtr,
																	ellipsoidSelectionPtr,
																	mapUnitsSelectionPtr);
																		
	if (referenceSystemCode == kByEPSGCodeCode)
		{
		LoadDItemString (dialogPtr, IDC_DatumName, (Str255*)datumNamePtr);
		LoadDItemString (dialogPtr, IDC_EllipsoidName, (Str255*)ellipsoidNamePtr);
		
		}	// end "if (referenceSystemCode == kByEPSGCodeCode)"
																		
			// Make sure grid zone direction is consistant with the false northing value.
			
	if (CoordinateDialogIsZoneDirectionEditable (*referenceSystemSelectionPtr) &&
																		*falseNorthingPtr == 10000000)
		gridZoneDirectionPtr[1] = 'S';
			
			
	*gridZoneWithDirectionPtr = *gridZonePtr;
	if (gridZoneDirectionPtr[1] == 'S')
		*gridZoneWithDirectionPtr = -(*gridZonePtr);
	
}	// end "CoordinateDialogInitialize" 
										
					

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CoordinateDialogIsZoneDirectionEditable
//
//	Software purpose:	The purpose of this routine is to determine if the zone direction
//							box is displayed and editable. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//							CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/21/2012
//	Revised By:			Larry L. Biehl			Date: 03/21/2012	

Boolean CoordinateDialogIsZoneDirectionEditable (
				SInt16								referenceSystemSelection)
	
{
	Boolean								editableFlag = FALSE;
	
	
	switch (referenceSystemSelection)
		{
		case kUTM_SAD69RSMenuItem:
		case kUTM_WGS72RSMenuItem:
		case kUTM_WGS84RSMenuItem:
		case kUTMRSMenuItem:
			editableFlag = TRUE;
			break;
					
		}	// end "switch (referenceSystemSelection)"
		
	return (editableFlag);
	
}	// end "CoordinateDialogIsZoneDirectionEditable" 
										
					

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CoordinateDialogIsZoneDisplayed
//
//	Software purpose:	The purpose of this routine is to determine if the zone box is
//							displayed. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//							CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2012
//	Revised By:			Larry L. Biehl			Date: 04/29/2012	

Boolean CoordinateDialogIsZoneDisplayed (
				SInt16								referenceSystemCode)
	
{
	Boolean								displayedFlag = FALSE;
	
	
	switch (referenceSystemCode)
		{
		case kGaussKrugerRSCode:
		case kGDA94RSCode:
		case kStatePlaneNAD27RSCode:
		case kStatePlaneNAD83RSCode:
		case kUTM_NAD27RSCode:
		case kUTM_NAD83RSCode:
		case kUTM_SAD69RSCode:
		case kUTM_WGS72RSCode:
		case kUTM_WGS84RSCode:
		case kUTMRSCode:
			displayedFlag = TRUE;
			break;
					
		}	// end "switch (referenceSystemCode)"
		
	return (displayedFlag);
	
}	// end "CoordinateDialogIsZoneDisplayed" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogOK
//
//	Software purpose:	The purpose of this routine is to set the map projection
//							parameters based on the user selections made in the dialog box. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2000
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void CoordinateDialogOK (
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
				Boolean								adjustUpperLeftMapPointFlag)

{		
	double								projectionParameters[13];
	
	
	if (mapProjectionInfoPtr == NULL)
		{
		fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();	

		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
															fileInfoPtr->mapProjectionHandle,
															kLock);
															
		}	// end "if (mapProjectionInfoPtr == NULL)"
					
	if (mapProjectionInfoPtr != NULL)
		{
				// Planar Coordinate Parameters
					
		mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;
		
		mapProjectionInfoPtr->planarCoordinate.metersToNativeFactor =		
				GetConversionFromMetersToNativeMapUnits (mapUnitsCode);
		
		//mapProjectionInfoPtr->planarCoordinate.areaUnitsCode = areaUnitsCode;
					
		mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = xMapCoordinate11;
		mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = yMapCoordinate11;
					
		mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = xMapCoordinate11;
		mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = yMapCoordinate11;
				
		mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = horizontalPixelSize;
		mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = verticalPixelSize;
			
		mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle =
															mapOrientationAngle * kDegreesToRadians;
		
		if (referenceSystemCode == kGeographicRSCode)
			{
					// Force map units to be decimal degrees for geographic projection
			mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kDecimalDegreesCode;
			
					// Also be sure that the projection code is set for geographic code.
			projectionCode = kGeographicCode;
			
			}	// end "if (referenceSystemCode == kGeographicRSCode)"
		
				// Grid Coordinate Parameters
		
		mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = referenceSystemCode;
		
		mapProjectionInfoPtr->projectedCSTypeGeoKey = epsgCode;
		
		CopyPToP (mapProjectionInfoPtr->gridCoordinate.epsgName, epsgNamePtr);
		
		if (CoordinateDialogIsZoneDisplayed (referenceSystemCode))
			{
			mapProjectionInfoPtr->gridCoordinate.gridZone = gridZone;
			if (gridZoneDirectionPtr[1] == 'S' || gridZoneDirectionPtr[1] == 's')
				mapProjectionInfoPtr->gridCoordinate.gridZone = -gridZone;
				
			}	// end "if (CoordinateDialogIsZoneDisplayed (referenceSystemCode))"
			
		else	// !CoordinateDialogIsZoneDisplayed (referenceSystemCode)
			mapProjectionInfoPtr->gridCoordinate.gridZone = 0;
		
		mapProjectionInfoPtr->gridCoordinate.projectionCode = projectionCode;
		
		projectionParameters[2] = standardParallel1;
		projectionParameters[3] = standardParallel2;
		projectionParameters[4] = longitudeCentralMeridian;
		projectionParameters[5] = latitudeOrigin;
		
				// Make sure the false easting and northing is stored in meters.
				// Convert if needed.
			
		ConvertFeetToMeters (mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
			
		projectionParameters[6] = falseEasting;
		projectionParameters[7] = falseNorthing;
			
		if (projectionCode == kTransverseMercatorCode)
			projectionParameters[2] = scaleFactorOfCentralMeridian;
			
		if (projectionCode == kMercatorCode || projectionCode == kEquirectangularCode)
			projectionParameters[5] = scaleFactorOfCentralMeridian;
		
		if (projectionCode == kLambertConformalConicCode)
			{
			projectionParameters[4] = longitudeCentralMeridian;
			projectionParameters[5] = latitudeOrigin;
			
			}	// end "if (projectionCode == kLambertConformalConicCode)"

		SetProjectionParameters (mapProjectionInfoPtr,
											projectionParameters,
											kNotPackedDegrees,
											kDegrees);
				
				// Geodetic Model Parameters														
				
		mapProjectionInfoPtr->geodetic.datumCode = datumCode;		
		CopyPToP (mapProjectionInfoPtr->gridCoordinate.datumName, datumNamePtr);
		
 		mapProjectionInfoPtr->geodetic.spheroidCode = ellipsoidCode;		
		CopyPToP (mapProjectionInfoPtr->gridCoordinate.ellipsoidName, ellipsoidNamePtr);
		
		mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMajorAxis = semiMajorAxis;
		mapProjectionInfoPtr->geodetic.semiMinorAxis = semiMinorAxis;
		if (mapProjectionInfoPtr->geodetic.spheroidCode == kSphereEllipsoidCode)
			{
			mapProjectionInfoPtr->geodetic.semiMajorAxis = radiusSpheroid;
			mapProjectionInfoPtr->geodetic.semiMinorAxis = radiusSpheroid;
			
			}	// end "if (...->geodetic.spheroidCode == kSphereEllipsoidCode)"
		
		LoadSpheroidInformation (fileInfoPtr->mapProjectionHandle);
		
				// Do not do this for now. The user need to have control over when this is to be
				// done. There can be times when the upper left map point is correct; the
				// other parameters just needed to be changed.
		
		adjustUpperLeftMapPointFlag = FALSE;
		if (adjustUpperLeftMapPointFlag)
			{
			ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
													 &upperLeftLatLongPoint);
			
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 =
																				upperLeftLatLongPoint.h;
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 =
																				upperLeftLatLongPoint.v;
			
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =
																				upperLeftLatLongPoint.h;
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =
																				upperLeftLatLongPoint.v;
			
			}	// end "if (adjustUpperLeftMapPointFlag)"
		
		adjustUpperLeftMapPointFlag = FALSE;
		
				// Update the map projection handle for the project if this file is the 
				// base image for the project.
		
		if (GetProjectBaseImageFlag (gActiveImageWindowInfoH))
			UpdateProjectMapProjectionHandle (fileInfoPtr->mapProjectionHandle);
					
		}	// end "if (mapProjectionInfoPtr != NULL)"
	
}	// end "CoordinateDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CoordinateDialogSetParametersFromEPSGCode
//
//	Software purpose:	The purpose of this routine is to set the map projection
//							parameters based on the user entered epsg code. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2012
//	Revised By:			Larry L. Biehl			Date: 04/21/2019

Boolean CoordinateDialogSetParametersFromEPSGCode (
				DialogPtr							dialogPtr,
				SInt16								epsgCode,
				Boolean								lastEPSGCodeValidFlag,
				UCharPtr								epsgNamePtr,
				UCharPtr								datumNamePtr,
				UCharPtr								ellipsoidNamePtr,
				SInt16*								mapUnitsSelectionPtr,
				SInt16*								projectionSelectionPtr)

{	
	double								falseEasting,
											falseNorthing,
											falseOriginLongitude,
											falseOriginLatitude,
											falseOriginEasting,
											falseOriginNorthing,
											latitudeOrigin,
											longitudeCentralMeridian,
											scaleFactorOfCentralMeridian,
											semiMajorAxis,
											semiMinorAxis,
											standardParallel1,
											standardParallel2;
		
	SInt16								mapUnitsCode,
											projectionCode;
	
	Boolean								parametersSetFlag;
	
	
	parametersSetFlag = GDALSetReferenceSystemFromEPSGCode (
																	epsgCode,
																	epsgNamePtr,
																	datumNamePtr,
																	ellipsoidNamePtr,
																	&mapUnitsCode,
																	&projectionCode,
																	&longitudeCentralMeridian,
																	&latitudeOrigin,
																	&scaleFactorOfCentralMeridian,
																	&falseEasting,
																	&falseNorthing,
																	&standardParallel1,
																	&standardParallel2,
																	&falseOriginLongitude,
																	&falseOriginLatitude,
																	&falseOriginEasting,
																	&falseOriginNorthing,
																	&semiMajorAxis,
																	&semiMinorAxis);
	
	CoordinateDialogHandleInvalidValidEPSGCode (dialogPtr,
																epsgCode,
																parametersSetFlag,
																lastEPSGCodeValidFlag);
	
	if (parametersSetFlag)
		{
		LoadDItemString (dialogPtr, IDC_FIPSZoneName, (Str255*)epsgNamePtr);
	
		*mapUnitsSelectionPtr = -CoordinateDialogGetSelectionFromCode (mapUnitsCode);

		LoadDItemString (dialogPtr, IDC_DatumName, (Str255*)datumNamePtr);
		
		LoadDItemString (dialogPtr, IDC_EllipsoidName, (Str255*)ellipsoidNamePtr);
						
		CoordinateDialogHideShowProjectionParameters (dialogPtr, 
																		kUserDefinedRSCode,
																		projectionCode,
																		FALSE,
																		NULL,
																		NULL,
																		FALSE);
																	
		CoordinateDialogSetProjectionParameters (dialogPtr,
																projectionCode,
																longitudeCentralMeridian,
																latitudeOrigin,
																scaleFactorOfCentralMeridian,
																falseEasting,
																falseNorthing,
																standardParallel1,
																standardParallel2,
																falseOriginLongitude,
																falseOriginLatitude,
																falseOriginEasting,
																falseOriginNorthing);
																	
		*projectionSelectionPtr = -CoordinateDialogGetSelectionFromCode (projectionCode);
		  
		#if defined multispec_win || defined multispec_wx
			dialogPtr->SetStaticTextOrDropDownList (abs (*mapUnitsSelectionPtr),
																	IDC_MapUnitsCombo, 
																	IDC_MapUnitsName,
																	TRUE);

			dialogPtr->SetStaticTextOrDropDownList (abs (*projectionSelectionPtr),
																	IDC_ProjectionCombo, 
																	IDC_ProjectionName,
																	TRUE);
			/*
			dialogPtr->SetStaticTextOrDropDownList (abs (*datumSelectionPtr),
																	IDC_DatumCombo,
																	IDC_DatumName,
																	TRUE);
			*/
		#endif	// defined multispec_win || defined multispec_wx
		
				// Verify the correct major/minor axis or radius of sphere parameters
				// are being displayed. Note that we only have ellipsoid name at this
				// point, not the actual code. The name could be one that is not include
				// in MultiSpec's list. So if the semiMajorAxis and semiMinorAxis are
				// equal, only the sphere line will be displayed. Otherwise the
				// parameters for major and minor axes will be displayed.
		
		if (semiMajorAxis == semiMinorAxis)
			{
			HideDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			HideDialogItem (dialogPtr, IDC_MinorAxis);
			
			#if defined multispec_wx
				ShowDialogItem (dialogPtr, IDC_MajorAxisPrompt);
				LoadDItemString (dialogPtr,
										IDC_MajorAxisPrompt,
										(CharPtr)"Radius of sphere (earth): ");
			#else
				HideDialogItem (dialogPtr, IDC_MajorAxisPrompt);
				ShowDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			
			SetDialogItemToStaticText (dialogPtr, IDC_Radius);
			
			LoadDItemRealValue (dialogPtr, IDC_Radius, semiMajorAxis, 4);
			
			}
		
		else	// semiMajorAxis != semiMinorAxis
			{
			ShowDialogItem (dialogPtr, IDC_MajorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_Radius);
			ShowDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_MinorAxis);
		
			#if defined multispec_wx
				LoadDItemString (dialogPtr, IDC_MajorAxisPrompt, (CharPtr)"Major axis: ");
			#else
				HideDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			
			SetDialogItemToStaticText (dialogPtr, IDC_Radius);
			SetDialogItemToStaticText (dialogPtr, IDC_MinorAxis);
				
			LoadDItemRealValue (dialogPtr, IDC_Radius, semiMajorAxis, 4);
			LoadDItemRealValue (dialogPtr, IDC_MinorAxis, semiMinorAxis, 4);
			
			}	// end "else semiMajorAxis != semiMinorAxis"
		
		}	// end "if (parametersSetFlag)"
		
	return (parametersSetFlag);
	
}	// end "CoordinateDialogSetParametersFromEPSGCode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogSetParametersFromRS
//
//	Software purpose:	The purpose of this routine is to set the map projection
//							parameters based on the user selections made in the dialog box. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2007
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void CoordinateDialogSetParametersFromRS (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				SInt16*								projectionSelectionPtr,
				UCharPtr								gridZoneDirectionPtr,
				SInt16								gridZoneWithDirection,
				SInt16								datumCode,
				SInt16								ellipsoidCode,
				SInt16*								gridZonePtr)

{	
	UInt8									fipsZoneName[32];
	
	double								falseEasting,
											falseNorthing,
											falseOriginLongitude,
											falseOriginLatitude,
											falseOriginEasting,
											falseOriginNorthing,
											latitudeOrigin,
											longitudeCentralMeridian,
											scaleFactorOfCentralMeridian,
											standardParallel1,
											standardParallel2;
		
	SInt16								gridZone,
											inputProjectionCode,
											maxGridZone,
											minGridZone,
											projectionCode;
	
	Boolean								parametersSetFlag,
											zoneUsedFlag;
	
	
			// Get input projection code. It may be needed later.
			
	inputProjectionCode = CoordinateDialogGetCodeFromSelection (*projectionSelectionPtr);
			
			// Force the zone to fit within what is allowed for the specified 
			// reference system.
	
	if ((zoneUsedFlag = CoordinateDialogIsZoneDisplayed (referenceSystemCode)))
		{
		gridZone = abs (gridZoneWithDirection);		
		CoordinateDialogGetMinMaxZone (referenceSystemCode,
													&minGridZone,
													&maxGridZone,
													gridZoneDirectionPtr);
													
		if (gridZone < minGridZone || gridZone > maxGridZone)
			gridZone = minGridZone;
			
		//if (gridZoneDirectionPtr[1] == 'S' && gridZoneWithDirection < 0)
		if (gridZoneDirectionPtr[1] == 'S')
			gridZone = -gridZone;
			
		}	// end "if (CoordinateDialogIsZoneDisplayed (referenceSystemCode))"
	
	fipsZoneName[0] = 0;					
	parametersSetFlag = SetProjectionParametersFromZone (referenceSystemCode,
																			datumCode,
																			&gridZone,
																			fipsZoneName,
																			&projectionCode,
																			&longitudeCentralMeridian,
																			&latitudeOrigin,
																			&scaleFactorOfCentralMeridian,
																			&falseEasting,
																			&falseNorthing,
																			&standardParallel1,
																			&standardParallel2,
																			&falseOriginLongitude,
																			&falseOriginLatitude,
																			&falseOriginEasting,
																			&falseOriginNorthing);
	
	if (parametersSetFlag)
		{
		if (zoneUsedFlag)
			{
			if (abs (gridZoneWithDirection) != abs (gridZone))
				{
				LoadDItemValue (dialogPtr, IDC_Zone, abs (gridZone));
				SelectDialogItemText (dialogPtr, IDC_Zone, 0, SHRT_MAX);
				
				gridZoneDirectionPtr[1] = 'N';
				if (gridZone < 0) 
					gridZoneDirectionPtr[1] = 'S';
				LoadDItemString (
								dialogPtr, IDC_ZoneDirection, (Str255*)gridZoneDirectionPtr);
				
				*gridZonePtr = abs (gridZone);
				
				}	// end "if (abs (gridZoneWithDirection) != abs (gridZone))"
				
			if (fipsZoneName[0] > 0)
				LoadDItemString (dialogPtr, IDC_FIPSZoneName, (Str255*)fipsZoneName);
			
			}	// end "if (zoneUsedFlag && abs (gridZoneWithDirection) ..."
			
		switch (projectionCode)
			{					
			case kTransverseMercatorCode:
						// Longitude of central meridian.												
					
				LoadDItemRealValue (dialogPtr, IDC_Longitude, longitudeCentralMeridian, 4);
				
						// Latitude of origin.																
					
				LoadDItemRealValue (dialogPtr, IDC_Latitude, latitudeOrigin, 4);
				
						// False Easting.																		
					
				LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseEasting, 2);
				
						// False Northing.																	
					
				LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseNorthing, 2);
				
						// Scale factor for central meridian.																	
					
				LoadDItemRealValue (
								dialogPtr, IDC_ScaleFactor, scaleFactorOfCentralMeridian, 5);
				break;
						
			case kLambertConformalConicCode:
						// Longitude of central meridian.												
					
				LoadDItemRealValue (dialogPtr, IDC_Longitude, falseOriginLongitude, 4);
				
						// Latitude of origin.																
					
				LoadDItemRealValue (dialogPtr, IDC_Latitude, falseOriginLatitude, 4);
				
						// False Easting.																		
					
				LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseOriginEasting, 2);
				
						// False Northing.																	
					
				LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseOriginNorthing, 2);
				
						// Standard Parallel 1.																	
					
				LoadDItemRealValue (dialogPtr, IDC_StandardParallel1, standardParallel1, 4);
				
						// Standard Parallel 2.																	
					
				LoadDItemRealValue (
										dialogPtr, IDC_StandardParallel2, standardParallel2, 4);
				break;
			
			}	// end "switch (projectionCode)"	
		
		if (inputProjectionCode != projectionCode)	
			CoordinateDialogHideShowProjectionParameters (dialogPtr, 
																	referenceSystemCode, 
																	projectionCode,
																	FALSE,
																	NULL,
																	NULL,
																	FALSE);

		//		I don't think this is needed here now: 3/24/2012
		//CoordinateDialogSetEllipsoidParameters (dialogPtr, ellipsoidCode);
		
				// Reset the projection selection. It may have changed for state plane RS
	
		if (referenceSystemCode == kStatePlaneNAD27RSCode || 
												referenceSystemCode == kStatePlaneNAD83RSCode)
			{
			*projectionSelectionPtr =
										-CoordinateDialogGetSelectionFromCode (projectionCode);
			  
			#if defined multispec_win || defined multispec_wx
				dialogPtr->SetStaticTextOrDropDownList (
													abs (*projectionSelectionPtr),
													IDC_ProjectionCombo, 
													IDC_ProjectionName,
													TRUE);
			#endif	// defined multispec_win || defined multispec_wx
			
			}	// end "if (referenceSystemCode == kStatePlaneNAD27RSCode || ..."
		
		}	// end "if (parametersSetFlag)"
		
	else	// !parametersSetFlag
		{
				// Catch case when an invalid zone number is selected for state plane.
				
		if ((referenceSystemCode == kStatePlaneNAD27RSCode || 
										referenceSystemCode == kStatePlaneNAD83RSCode) &&
														fipsZoneName[0] == 0)
			{
			LoadDItemStringNumber (kDialogStrID, 
											IDS_Dialog31, 
											dialogPtr, 
											IDC_FIPSZoneName, 
											(Str255*)gTextString);
			
			}	// end "if ((referenceSystemCode == kStatePlaneNAD27RSCode || ..."
		
		}	// end "else !parametersSetFlag"
	
	if (referenceSystemCode == kByEPSGCodeCode)
		SelectDialogItemText (dialogPtr, IDC_EPSGCode, 0, SHRT_MAX);

}	// end "CoordinateDialogSetParametersFromRS"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CoordinateDialogSetDatumParameters
//
//	Software purpose:	The purpose of this routine is to set the datum parameters
//							for the specified datum. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/19/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2019

SInt16 CoordinateDialogSetDatumParameters (
				DialogPtr							dialogPtr,
				SInt16								datumCode,
				SInt16								ellipsoidCode,
				Boolean								initialFlag,
				Boolean								datumDeactivatedFlag)

{	
	SInt16								ellipsoidSelection;
	
	Boolean								ellipsoidDeactivatedFlag = FALSE;
	
		
	if (datumCode > kNoDatumDefinedCode || datumDeactivatedFlag)
		{
		ellipsoidDeactivatedFlag = TRUE;
		
		if (!initialFlag)
			ellipsoidCode = GetEllipsoidCodeFromDatumCode (datumCode);
		
		}	// end "else if (datumCode > kUserDefinedDatumCode|| ..."
		
	CoordinateDialogSetEllipsoidParameters (dialogPtr, ellipsoidCode);
	
	ellipsoidSelection = CoordinateDialogGetSelectionFromCode (ellipsoidCode);
	                  
	if (ellipsoidDeactivatedFlag)
		ellipsoidSelection = -abs (ellipsoidSelection);

	#if defined multispec_win || defined multispec_wx
		dialogPtr->SetStaticTextOrDropDownList (abs (ellipsoidSelection),
																IDC_EllipsoidCombo, 
																IDC_EllipsoidName,
																ellipsoidDeactivatedFlag);
	#endif	// defined multispec_win || defined multispec_wx
	
	return (ellipsoidSelection);
	
}	// end "CoordinateDialogSetDatumParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogSetEllipsoidParameters
//
//	Software purpose:	The purpose of this routine is to show the ellipsoid parameters
//							for the specified ellipsoid. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//							CoordinateDialogHideShowProjectionTypeParameters2 in
//																						SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/23/2009
//	Revised By:			Larry L. Biehl			Date: 03/09/2019

void CoordinateDialogSetEllipsoidParameters (
				DialogPtr							dialogPtr,
				SInt16								ellipsoidCode)

{
	double								radiusSpheroid,
											semiMajorAxis,
											semiMinorAxis;
											
											
	if (ellipsoidCode > kUserDefinedEllipsoidCode)
		GetEllipsoidParameters (ellipsoidCode,
											&radiusSpheroid,
											&semiMajorAxis,
											&semiMinorAxis);

	switch (ellipsoidCode)
		{
		case kNoEllipsoidDefinedCode:
			#if !defined multispec_wx
				HideDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			HideDialogItem (dialogPtr, IDC_MajorAxisPrompt);
			HideDialogItem (dialogPtr, IDC_Radius);
			HideDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			HideDialogItem (dialogPtr, IDC_MinorAxis);
			break;
			
		case kUserDefinedEllipsoidCode:			
			SetDialogItemToEditText (dialogPtr, IDC_Radius);
			SetDialogItemToEditText (dialogPtr, IDC_MinorAxis);
		
			#if defined multispec_wx
				LoadDItemString (dialogPtr, IDC_MajorAxisPrompt, (CharPtr)"Major axis: ");
			#else
				HideDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			
			ShowDialogItem (dialogPtr, IDC_MajorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_Radius);
			ShowDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_MinorAxis);
			break;
			
		case kAiryEllipsoidCode:
		case kAustralianEllipsoidCode:
		case kBesselEllipsoidCode:
		case kBesselModifiedEllipsoidCode:
		case kClarke1866EllipsoidCode:
		case kClarke1880EllipsoidCode:
		case kEverestEllipsoidCode:
		case kGRS1967ModifiedEllipsoidCode:
		case kGRS80EllipsoidCode:
		case kInternational1909EllipsoidCode:
		case kKrassovskyEllipsoidCode:
		case kWGS72EllipsoidCode:
		case kWGS84EllipsoidCode:
			LoadDItemRealValue (dialogPtr, IDC_Radius, semiMajorAxis, 4);
			LoadDItemRealValue (dialogPtr, IDC_MinorAxis, semiMinorAxis, 4);
			
					// Force selected dialog item to not be Major/Minor axes value entries
			SelectDialogItemText (dialogPtr, IDC_X11Coordinate, 0, SHRT_MAX);
			SetDialogItemToStaticText (dialogPtr, IDC_Radius);
			SetDialogItemToStaticText (dialogPtr, IDC_MinorAxis);
			
			#if defined multispec_wx
				LoadDItemString (dialogPtr, IDC_MajorAxisPrompt, (CharPtr)"Major axis: ");
			#else
				HideDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			
			ShowDialogItem (dialogPtr, IDC_MajorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_Radius);
			ShowDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			ShowDialogItem (dialogPtr, IDC_MinorAxis);
			break;
			
		case kSphereEllipsoidCode:
			LoadDItemRealValue (dialogPtr, IDC_Radius, radiusSpheroid, 4);			
			SetDialogItemToEditText (dialogPtr, IDC_Radius);
			
			#if defined multispec_wx
				ShowDialogItem (dialogPtr, IDC_MajorAxisPrompt);
				LoadDItemString (dialogPtr,
										IDC_MajorAxisPrompt,
										(CharPtr)"Radius of sphere (earth): ");
			#else
				HideDialogItem (dialogPtr, IDC_MajorAxisPrompt);
				ShowDialogItem (dialogPtr, IDC_RadiusPrompt);
			#endif
			
			HideDialogItem (dialogPtr, IDC_MinorAxisPrompt);
			HideDialogItem (dialogPtr, IDC_MinorAxis);
			ShowDialogItem (dialogPtr, IDC_Radius);
			break;
		
		}	// end "switch (switch (ellipsoidCode))"
	
	#if defined multispec_wx
		dialogPtr->Fit ();
	#endif
	
}	// end "CoordinateDialogSetEllipsoidParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogSetProjectionParameters
//
//	Software purpose:	The purpose of this routine is to show the projection parameters
//							for the specified projection. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/13/2012
//	Revised By:			Larry L. Biehl			Date: 04/13/2012	

void CoordinateDialogSetProjectionParameters (
				DialogPtr							dialogPtr,
				SInt16								projectionCode,
				double								longitudeCentralMeridian,
				double								latitudeOrigin,
				double								scaleFactorOfCentralMeridian,
				double								falseEasting,
				double								falseNorthing,
				double								standardParallel1,
				double								standardParallel2,
				double								falseOriginLongitude,
				double								falseOriginLatitude,
				double								falseOriginEasting,
				double								falseOriginNorthing)

{	
		
	switch (projectionCode)
		{
		case kGeographicCode:
		case kNotDefinedCode:
		case kAzimuthalEquidistantCode:
		case kObliqueMercatorCode:
		case kSpaceObliqueMercatorCode:
		case kModifiedTransverseMercatorCode:
		case kGeneralVerticalNearSideCode:
		case kKrovakCode:
		default:
			break;		
			
		case kAlaskaConformalCode:
			LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseEasting, 2);		
			LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseNorthing, 2);
			break;
		
		case kGnomonicCode:
		case kLambertAzimuthalEqualAreaCode:
		case kMercatorCode:
		case kOrthographicCode:
		case kPolarStereographicCode:
		case kPolyconicCode:
		case kStereographicCode:
		case kVanderGrintenICode:			
			LoadDItemRealValue (dialogPtr, IDC_Longitude, longitudeCentralMeridian, 4);	
			LoadDItemRealValue (dialogPtr, IDC_Latitude, latitudeOrigin, 4);	
			LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseEasting, 2);		
			LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseNorthing, 2);	
			break;
			
		case kTransverseMercatorCode:	
		case kTransvMercatorSouthOrientedCode:											
			LoadDItemRealValue (dialogPtr, IDC_Longitude, longitudeCentralMeridian, 4);	
			LoadDItemRealValue (dialogPtr, IDC_Latitude, latitudeOrigin, 4);	
			LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseEasting, 2);		
			LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseNorthing, 2);								
			LoadDItemRealValue (dialogPtr, IDC_ScaleFactor, scaleFactorOfCentralMeridian, 5);	
			break;	
		
		case kCylindricalEqualAreaCode:
			LoadDItemRealValue (dialogPtr, IDC_Longitude, longitudeCentralMeridian, 4);
			LoadDItemRealValue (dialogPtr, IDC_StandardParallel1, standardParallel1, 4);
			LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseEasting, 2);		
			LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseNorthing, 2);	
			break;
		
		case kSinusoidalCode:			
			LoadDItemRealValue (dialogPtr, IDC_Longitude, falseOriginLongitude, 4);
			break;
		
		case kEquirectangularCode:
			LoadDItemRealValue (dialogPtr, IDC_Longitude, falseOriginLongitude, 4);
			LoadDItemRealValue (dialogPtr, IDC_StandardParallel1, standardParallel1, 4);
			break;
			
		case kAlbersConicalEqualAreaCode:
		case kEquidistantConicCode:
		case kLambertConformalConicCode:
			LoadDItemRealValue (dialogPtr, IDC_Longitude, falseOriginLongitude, 4);
			LoadDItemRealValue (dialogPtr, IDC_Latitude, falseOriginLatitude, 4);
			LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseOriginEasting, 2);
			LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseOriginNorthing, 2);
			LoadDItemRealValue (dialogPtr, IDC_StandardParallel1, standardParallel1, 4);
			LoadDItemRealValue (dialogPtr, IDC_StandardParallel2, standardParallel2, 4);
			break;
			
		case kHammerCode:
		case kMillerCylindricalCode:
		case kMollweideCode:
		case kRobinsonCode:
		case kWagnerIVCode:
		case kWagnerVIICode:
			LoadDItemRealValue (dialogPtr, IDC_Longitude, falseOriginLongitude, 4);
			LoadDItemRealValue (dialogPtr, IDC_FalseEasting, falseOriginEasting, 2);
			LoadDItemRealValue (dialogPtr, IDC_FalseNorthing, falseOriginNorthing, 2);
			break;
			
		}	// end "switch (projectionCode)"
		
}	// end "CoordinateDialogSetProjectionParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CoordinateDialogSetReferenceSystemParameters
//
//	Software purpose:	The purpose of this routine is to set the map projection
//							parameters based on the user selections made in the dialog box. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//							CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2000
//	Revised By:			Larry L. Biehl			Date: 03/15/2019

void CoordinateDialogSetReferenceSystemParameters (
				DialogPtr							dialogPtr,
				SInt16								referenceSystemCode,
				UCharPtr								gridZoneDirectionPtr,
				Boolean								initialFlag,
				SInt16*								projectionSelectionPtr,
				SInt16*								datumSelectionPtr,
				SInt16*								ellipsoidSelectionPtr,
				SInt16*								mapUnitsSelectionPtr)

{
	char									epsgCodeString[16] = {10, 'E', 'P', 'S', 'G', ' ', 'C', 'o', 'd', 'e', ':'};
	char									fipsZoneString[16] = {10, 'F', 'I', 'P', 'S', ' ', 'Z', 'o', 'n', 'e', ':'};
	char									mgaZoneString[16] = {9, 'M', 'G', 'A', ' ', 'Z', 'o', 'n', 'e', ':'};
	char									zoneString[16] = {5, 'Z', 'o', 'n', 'e', ':'};
	
	SInt16								datumCode,
											dialogTextItem,
											ellipsoidCode,
											mapUnitsCode,
											projectionCode;
	
	Boolean								datumDeactivatedFlag = FALSE,
											hideDatumComboFlag = FALSE,
											hideEllipsoidComboFlag = FALSE,
											hideProjectionComboFlag = FALSE,
											mapUnitsDeactivateFlag = FALSE,
											projectionDeactivatedFlag = FALSE;
	
	
	projectionCode = CoordinateDialogGetCodeFromSelection (*projectionSelectionPtr);
	datumCode = CoordinateDialogGetCodeFromSelection (*datumSelectionPtr);
	ellipsoidCode = CoordinateDialogGetCodeFromSelection (*ellipsoidSelectionPtr);
	mapUnitsCode = CoordinateDialogGetCodeFromSelection (*mapUnitsSelectionPtr);
	
	#if defined multispec_win || defined multispec_wx
		HideDialogItem (dialogPtr, IDC_ProjectionName);
		HideDialogItem (dialogPtr, IDC_MapUnitsName);
	#endif	// defined multispec_win || defined multispec_wx
	HideDialogItem (dialogPtr, IDC_DatumName);
	HideDialogItem (dialogPtr, IDC_EllipsoidName);
	
	dialogTextItem = IDC_X11Coordinate;
		
	switch (referenceSystemCode)
		{
		case kNoRSDefinedCode:
			HideDialogItem (dialogPtr, IDC_EPSGCode);
			HideDialogItem (dialogPtr, IDC_ZonePrompt);
			HideDialogItem (dialogPtr, IDC_Zone);
			HideDialogItem (dialogPtr, IDC_ZoneDirection);
			HideDialogItem (dialogPtr, IDC_FIPSZoneName);
			
			HideDialogItem (dialogPtr, IDC_ProjectionPrompt);
			HideDialogItem (dialogPtr, IDC_ProjectionCombo);
			HideDialogItem (dialogPtr, IDC_DatumPrompt);
			HideDialogItem (dialogPtr, IDC_DatumCombo);
			HideDialogItem (dialogPtr, IDC_ProjectionEllipsoidPrompt);
			HideDialogItem (dialogPtr, IDC_EllipsoidCombo);
			
			hideProjectionComboFlag = TRUE;
			hideDatumComboFlag = TRUE;
			hideEllipsoidComboFlag = TRUE;
			if (!initialFlag)
				{
				projectionCode = kNotDefinedCode,
				datumCode = kNoDatumDefinedCode;
				ellipsoidCode = kNoEllipsoidDefinedCode;
				
				}	// end "if (!initialFlag)"
			break;
			
		case kUserDefinedRSCode:
			CoordinateDialogActivateProjectionParameters (dialogPtr);				
			
			HideDialogItem (dialogPtr, IDC_EPSGCode);
			HideDialogItem (dialogPtr, IDC_ZonePrompt);
			HideDialogItem (dialogPtr, IDC_Zone);
			HideDialogItem (dialogPtr, IDC_ZoneDirection);
			HideDialogItem (dialogPtr, IDC_FIPSZoneName);
			ShowDialogItem (dialogPtr, IDC_ProjectionPrompt);
			ShowDialogItem (dialogPtr, IDC_ProjectionCombo);
			ShowDialogItem (dialogPtr, IDC_DatumPrompt);
			ShowDialogItem (dialogPtr, IDC_DatumCombo);
			ShowDialogItem (dialogPtr, IDC_ProjectionEllipsoidPrompt);
			ShowDialogItem (dialogPtr, IDC_EllipsoidCombo);

			if (!initialFlag && mapUnitsCode == kUnknownCode)
				mapUnitsCode = kMetersCode;

			if (projectionCode < 0)
				projectionCode = kNotDefinedCode;
			break;
			
		case kGeographicRSCode:				
			HideDialogItem (dialogPtr, IDC_EPSGCode);
			HideDialogItem (dialogPtr, IDC_ZonePrompt);
			HideDialogItem (dialogPtr, IDC_Zone);
			HideDialogItem (dialogPtr, IDC_ZoneDirection);
			HideDialogItem (dialogPtr, IDC_FIPSZoneName);
			HideDialogItem (dialogPtr, IDC_ProjectionPrompt);
			HideDialogItem (dialogPtr, IDC_ProjectionCombo);
			ShowDialogItem (dialogPtr, IDC_DatumPrompt);
			ShowDialogItem (dialogPtr, IDC_DatumCombo);
			ShowDialogItem (dialogPtr, IDC_ProjectionEllipsoidPrompt);
			ShowDialogItem (dialogPtr, IDC_EllipsoidCombo);

			hideProjectionComboFlag = TRUE;
			if (!initialFlag)
				mapUnitsCode = kDecimalDegreesCode;
			break;

		case kGaussKrugerRSCode:
		case kGDA94RSCode:
		case kGGRS87RSCode:
		case kStatePlaneNAD27RSCode:
		case kStatePlaneNAD83RSCode:
		case kUTM_NAD27RSCode:
		case kUTM_NAD83RSCode:
		case kUTM_SAD69RSCode:
		case kUTM_WGS72RSCode:
		case kUTM_WGS84RSCode:
		case kUTMRSCode:		
		case kByEPSGCodeCode:
			HideDialogItem (dialogPtr, IDC_EPSGCode);
			ShowDialogItem (dialogPtr, IDC_ZonePrompt);
			ShowDialogItem (dialogPtr, IDC_Zone);
			ShowDialogItem (dialogPtr, IDC_ZoneDirection);
			HideDialogItem (dialogPtr, IDC_FIPSZoneName);
			ShowDialogItem (dialogPtr, IDC_ProjectionPrompt);
			ShowDialogItem (dialogPtr, IDC_ProjectionCombo);
			ShowDialogItem (dialogPtr, IDC_DatumPrompt);
			ShowDialogItem (dialogPtr, IDC_DatumCombo);
			ShowDialogItem (dialogPtr, IDC_ProjectionEllipsoidPrompt);
			ShowDialogItem (dialogPtr, IDC_EllipsoidCombo);
			
			SetDialogItemToStaticText (dialogPtr, IDC_EPSGCode);
			
			if (!initialFlag)
				mapUnitsCode = kMetersCode;
			
			LoadDItemString (dialogPtr, IDC_ZonePrompt, (Str255*)zoneString);
			dialogTextItem = IDC_Zone;
			
			if (!initialFlag && referenceSystemCode != kStatePlaneNAD27RSCode && 
												referenceSystemCode != kStatePlaneNAD83RSCode)
				projectionCode = kTransverseMercatorCode;
				
			switch (referenceSystemCode) 
				{
				case kGaussKrugerRSCode:
					gridZoneDirectionPtr[1] = 'N';
					LoadDItemString (
								dialogPtr, IDC_ZoneDirection, (Str255*)gridZoneDirectionPtr);
					SetDialogItemToStaticText (dialogPtr, IDC_ZoneDirection);
					
					if (!initialFlag)
						datumCode = kPulkovo1942DatumCode;
					break;

				case kGDA94RSCode:
					LoadDItemString (dialogPtr, IDC_ZonePrompt, (Str255*)mgaZoneString);
					gridZoneDirectionPtr[1] = 'S';
					LoadDItemString (
								dialogPtr, IDC_ZoneDirection, (Str255*)gridZoneDirectionPtr);
					SetDialogItemToStaticText (dialogPtr, IDC_ZoneDirection);
					
					if (!initialFlag)
						datumCode = kGeocentricDatumAustralia1994Code;
					break;
				
				case kGGRS87RSCode:
					HideDialogItem (dialogPtr, IDC_ZonePrompt);
					HideDialogItem (dialogPtr, IDC_Zone);
					HideDialogItem (dialogPtr, IDC_ZoneDirection);
					
					if (!initialFlag)
						datumCode = kGGRS87DatumCode;
						
					dialogTextItem = IDC_X11Coordinate;
					break;
				
				case kStatePlaneNAD27RSCode:
					LoadDItemString (dialogPtr, IDC_ZonePrompt, (Str255*)fipsZoneString);
					HideDialogItem (dialogPtr, IDC_ZoneDirection);
					ShowDialogItem (dialogPtr, IDC_FIPSZoneName);
					if (!initialFlag)
						{
						datumCode = kNAD27Code;
						mapUnitsCode = kUSSurveyFeetCode;
						
						}	// end "if (!initialFlag)"
					break;
					
				case kStatePlaneNAD83RSCode:
					LoadDItemString (dialogPtr, IDC_ZonePrompt, (Str255*)fipsZoneString);
					HideDialogItem (dialogPtr, IDC_ZoneDirection);
					ShowDialogItem (dialogPtr, IDC_FIPSZoneName);
					if (!initialFlag)
						{
						datumCode = kNAD83Code;
						mapUnitsCode = kMetersCode;
						
						}	// end "if (!initialFlag)"
					break;
				
				case kUTM_NAD27RSCode:
					gridZoneDirectionPtr[1] = 'N';
					LoadDItemString (
								dialogPtr, IDC_ZoneDirection, (Str255*)gridZoneDirectionPtr);
					SetDialogItemToStaticText (dialogPtr, IDC_ZoneDirection);
					if (!initialFlag)
						datumCode = kNAD27Code;
					break;
					
				case kUTM_NAD83RSCode:
					gridZoneDirectionPtr[1] = 'N';
					LoadDItemString (
								dialogPtr, IDC_ZoneDirection, (Str255*)gridZoneDirectionPtr);
					SetDialogItemToStaticText (dialogPtr, IDC_ZoneDirection);
					if (!initialFlag)
						datumCode = kNAD83Code;
					break;
					
				case kUTM_SAD69RSCode:
					SetDialogItemToEditText (dialogPtr, IDC_ZoneDirection);
					if (!initialFlag)
						datumCode = kSAD69DatumCode;
					break;
					
				case kUTM_WGS72RSCode:
					SetDialogItemToEditText (dialogPtr, IDC_ZoneDirection);
					if (!initialFlag)
						datumCode = kWGS72DatumCode;
					break;
				
				case kUTM_WGS84RSCode:
					SetDialogItemToEditText (dialogPtr, IDC_ZoneDirection);
					if (!initialFlag)
						datumCode = kWGS84Code;
					break;
					
				case kUTMRSCode:
					SetDialogItemToEditText (dialogPtr, IDC_ZoneDirection);
					if (!initialFlag)
						datumCode = kNoDatumDefinedCode;
					break;
					
				case kByEPSGCodeCode:
					SetDialogItemToEditText (dialogPtr, IDC_EPSGCode);
					LoadDItemString (dialogPtr, IDC_ZonePrompt, (Str255*)epsgCodeString);
					ShowDialogItem (dialogPtr, IDC_EPSGCode);
					HideDialogItem (dialogPtr, IDC_Zone);
					HideDialogItem (dialogPtr, IDC_ZoneDirection);
					ShowDialogItem (dialogPtr, IDC_FIPSZoneName);
					HideDialogItem (dialogPtr, IDC_DatumCombo);
					ShowDialogItem (dialogPtr, IDC_DatumName);
					HideDialogItem (dialogPtr, IDC_EllipsoidCombo);
					ShowDialogItem (dialogPtr, IDC_EllipsoidName);

					mapUnitsDeactivateFlag = TRUE;
					
					//if (initialFlag)
					//	{
					//	datumCode = kNoDatumDefinedCode;
					//						
					//	}	// end "if (initialFlag)"
					break;

				}	// end "switch (referenceSystemCode)"
							
			projectionDeactivatedFlag = TRUE;
			CoordinateDialogDeactivateProjectionParameters (dialogPtr);				
			
			if (referenceSystemCode != kUTMRSCode)
				datumDeactivatedFlag = TRUE;
				
			break;	
					
		}	// end "switch (referenceSystemCode)"
	
	*projectionSelectionPtr = CoordinateDialogGetSelectionFromCode (projectionCode);
	*datumSelectionPtr = CoordinateDialogGetSelectionFromCode (datumCode);
	*mapUnitsSelectionPtr = CoordinateDialogGetSelectionFromCode (mapUnitsCode);

	if (mapUnitsDeactivateFlag)
		*mapUnitsSelectionPtr = -abs (*mapUnitsSelectionPtr);
															
	if (projectionDeactivatedFlag)
		*projectionSelectionPtr = -abs (*projectionSelectionPtr);
		
	if (datumDeactivatedFlag)
		*datumSelectionPtr = -abs (*datumSelectionPtr);

	*ellipsoidSelectionPtr = CoordinateDialogSetDatumParameters (dialogPtr,
																						datumCode,
																						ellipsoidCode,
																						initialFlag,
																						datumDeactivatedFlag);
	
			// Handle the projection parameters
			// Note that the datumSelectionPtr could be set to deactivated state in
			// the following call only if this is the intial set up and the projection
			// is one where only a sphere is allowed.
		
	CoordinateDialogHideShowProjectionParameters (dialogPtr, 
																	referenceSystemCode,
																	projectionCode,
																	initialFlag,
																	datumSelectionPtr,
																	NULL,
																	FALSE);

	#if defined multispec_win || defined multispec_wx
		dialogPtr->SetStaticTextOrDropDownList (abs (*mapUnitsSelectionPtr),
																IDC_MapUnitsCombo, 
																IDC_MapUnitsName,
																*mapUnitsSelectionPtr<0);

		dialogPtr->SetStaticTextOrDropDownList (abs (*projectionSelectionPtr),
																IDC_ProjectionCombo, 
																IDC_ProjectionName,
																*projectionSelectionPtr<0);
		if (hideProjectionComboFlag)
			HideDialogItem (dialogPtr, IDC_ProjectionCombo);
	
		dialogPtr->SetStaticTextOrDropDownList (
										abs (*datumSelectionPtr),
										IDC_DatumCombo,
										IDC_DatumName,
										(*datumSelectionPtr<0) | datumDeactivatedFlag);
		if (hideDatumComboFlag)
			HideDialogItem (dialogPtr, IDC_DatumCombo);

		if (hideEllipsoidComboFlag)
			HideDialogItem (dialogPtr, IDC_EllipsoidCombo);
	#endif	// defined multispec_win || defined multispec_wx
																			
	SelectDialogItemText (dialogPtr, dialogTextItem, 0, SHRT_MAX);
		
}	// end "CoordinateDialogSetReferenceSystemParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle CopyMapProjectionHandle
//
//	Software purpose:	This routine makes a copy of the input map projection handle
//
//	Parameters in:		Handle to the map projection structure that a copy is to be 
//							made of
//
//	Parameters out:	Handle to the new map projection structure.		
//
// Value Returned:	Error code for the operation
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/24/2007
//	Revised By:			Larry L. Biehl			Date: 08/02/2010			

SInt16 CopyMapProjectionHandle (
				Handle								inputMapProjectionHandle,
				Handle*								outputMapProjectionHandlePtr)

{	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
		 
	Handle								inputCoefficientsHandle,
											outputCoefficientsHandle;
											
	SInt16								errCode;
	

	inputCoefficientsHandle = GetCoefficientsHandle (inputMapProjectionHandle);
	outputCoefficientsHandle = inputCoefficientsHandle;
	
	*outputMapProjectionHandlePtr = inputMapProjectionHandle;
	errCode = HandToHand (outputMapProjectionHandlePtr);
	
	if (errCode == noErr && inputCoefficientsHandle != NULL)
		{
		errCode = HandToHand (&outputCoefficientsHandle);
		
		if (errCode == noErr)
			{
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
											GetHandlePointer (*outputMapProjectionHandlePtr);
     
     		mapProjectionInfoPtr->coefficientsHandle = outputCoefficientsHandle;
		
			}	// and "if (errCode == noErr)"
		
		}	// and "if (errCode == noErr)"
		
	if (errCode != noErr)
		{
		if (outputCoefficientsHandle != inputCoefficientsHandle &&
				outputCoefficientsHandle != NULL)
			UnlockAndDispose (outputCoefficientsHandle);
			
		if (*outputMapProjectionHandlePtr != inputMapProjectionHandle &&
				*outputMapProjectionHandlePtr != NULL)
			*outputMapProjectionHandlePtr = 
										UnlockAndDispose (*outputMapProjectionHandlePtr);
		
		}	// end "if (errCode != noErr)"
		
	return (errCode);
	
}	// end "CopyMapProjectionHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisposeMapProjectionHandle
//
//	Software purpose:	This routine clears the input map projection handle of any other
//							handles and then disposes of the map projection handle itself.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/2008
//	Revised By:			Larry L. Biehl			Date: 10/25/2008			

Handle DisposeMapProjectionHandle (
				Handle								mapProjectionHandle)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
		
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);
	if (mapProjectionInfoPtr != NULL)
		UnlockAndDispose (mapProjectionInfoPtr->coefficientsHandle);
	
	mapProjectionHandle = UnlockAndDispose (mapProjectionHandle);
	
	return (mapProjectionHandle);
	
}	// end "DisposeMapProjectionHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawCursorCoordinates
//
//	Software purpose:	This routine draws the cursor coordinates in the coordinate
//							view of the image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			UpdateCursorCoordinates in xImageView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/26/2000
//	Revised By:			Larry L. Biehl			Date: 09/12/2019

void DrawCursorCoordinates (
				Handle								windowInfoHandle)

{		
	SInt16								decimalPlaces,
											numberCharacters,
											numberXChars,
											numberYChars,
											viewUnits,
											xCursorStart;
	
	
	xCursorStart = (SInt16)GetCoordinateViewCursorStart (windowInfoHandle);
	
	if (gPresentCursor == kCross || gPresentCursor == kBlinkOpenCursor2)
		{
		numberCharacters = (SInt16)GetCoordinateViewNumberChars (windowInfoHandle);
		viewUnits = GetCoordinateViewUnits (windowInfoHandle);
	
		switch (viewUnits)
			{
			case kLineColumnUnitsMenuItem:
				numberYChars = sprintf ((char*)gTextString, 
												"%*d",
												numberCharacters,
												(int)gCoordinateLineValue);
												
				numberXChars = sprintf ((char*)gTextString2, 
												"%*d",
												numberCharacters,
												(int)gCoordinateColumnValue);
								
				decimalPlaces = -1;
				
				break;
			
			case kUnknownUnitsMenuItem:
			case kDecimalLatLongUnitsMenuItem:
			case kDMSLatLongUnitsMenuItem:
			case kKilometersUnitsMenuItem:
			case kMetersUnitsMenuItem:
			case kCentimetersUnitsMenuItem:
			case kMillimetersUnitsMenuItem:
			case kMicrometersUnitsMenuItem:
			case kMilesUnitsMenuItem:
			case kYardsUnitsMenuItem:
			case kUSSurveyFeetUnitsMenuItem:
			case kFeetUnitsMenuItem:
			case kInchesUnitsMenuItem:
			
				DoublePoint			coordinatePoint;
			
				double 				factor,
										horizontalPixelSize,
										verticalPixelSize,
										xCoordinateValue,
										xMapCoordinate11,
										yCoordinateValue,
										yMapCoordinate11;
										
				DisplaySpecsPtr	displaySpecsPtr;
										
				Handle				displaySpecsHandle;
				
								
				factor = GetCoordinateViewFactor (windowInfoHandle);
				
				Handle mapProjectionHandle = 
													GetFileMapProjectionHandle2 (windowInfoHandle);
	
						// Get the grid coordinate code.
		
				MapProjectionInfoPtr mapProjectionInfoPtr = 
								(MapProjectionInfoPtr)GetHandlePointer (mapProjectionHandle);
															
				if (mapProjectionInfoPtr == NULL)	
						// Need to fix problem here. Setting is made for map projection
						// from last display. New display for a different hdf data set
						// does not have a projection. The listing was not changed back
						// to line and column display.
																									break;
		                             
				displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
				
				displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																				displaySpecsHandle);
																				
				if (mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
					{
					horizontalPixelSize = 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
					verticalPixelSize = 
									-mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
				
					xMapCoordinate11 =
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
					yMapCoordinate11 =
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
					
					if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0)
						{
						yCoordinateValue = yMapCoordinate11 + (gDoubleCoordinateLineValue - 
											0.5*displaySpecsPtr->displayedLineInterval) *
																						verticalPixelSize;
						yCoordinateValue *= factor;
				
						xCoordinateValue = xMapCoordinate11 + (gDoubleCoordinateColumnValue - 
											0.5*displaySpecsPtr->displayedColumnInterval) *
																						horizontalPixelSize;
						xCoordinateValue *= factor;
							
						}	// end "if (...->planarCoordinate.mapOrientationAngle == 0)"
						
					else	// mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0
						{
						double			cosOrientAngle,
											sinOrientAngle,
											xOffset,
											yOffset;
											
						cosOrientAngle = cos (
									mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
						sinOrientAngle = sin (
									mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
								
						xOffset = (gDoubleCoordinateColumnValue - 
											0.5*displaySpecsPtr->displayedColumnInterval) *
																					horizontalPixelSize;
								
						yOffset = (gDoubleCoordinateLineValue - 
											0.5*displaySpecsPtr->displayedLineInterval) *
																					verticalPixelSize;
				
						xCoordinateValue =
								mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin +
												xOffset*cosOrientAngle + yOffset*sinOrientAngle;
						xCoordinateValue *= factor;
								
						yCoordinateValue =
								mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin -
												xOffset*sinOrientAngle + yOffset*cosOrientAngle;
						yCoordinateValue *= factor;
							
						}	// end "if (...->planarCoordinate.mapOrientationAngle != 0)"
																					
					if (viewUnits == kDecimalLatLongUnitsMenuItem ||
																viewUnits == kDMSLatLongUnitsMenuItem)
						{
						coordinatePoint.h = xCoordinateValue;
						coordinatePoint.v = yCoordinateValue;
						
						ConvertMapPointToLatLongPoint (
														mapProjectionInfoPtr,
														&coordinatePoint);
														
						xCoordinateValue = coordinatePoint.h;
						yCoordinateValue = coordinatePoint.v;
						
						}	// end "if (viewUnits == kDecimalLatLongUnitsMenuItem || ..."
						
					}	// end "mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0"
					
				else	// mapProjectionInfoPtr->planarCoordinate.polynomialOrder > 0
					{	
					GetCoefficientsVectorPointers (mapProjectionInfoPtr);
			
					TransformCoordinatePoint (
							gDoubleCoordinateColumnValue,
							gDoubleCoordinateLineValue,
							&xCoordinateValue,
							&yCoordinateValue,
							mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr,
							mapProjectionInfoPtr->planarCoordinate.polynomialOrder);
															
					CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
						
					}	// end "else ...->planarCoordinate.polynomialOrder > 0"
				
				if (viewUnits == kDMSLatLongUnitsMenuItem)
					{
					numberYChars = LoadDMSLatLongStrings (yCoordinateValue,
																		xCoordinateValue,
																		(char*)gTextString,
																		(char*)gTextString2);
													
					numberXChars = numberYChars;
					
					}	// end "if (viewUnits == kDMSLatLongUnitsMenuItem)"
					
				else	// viewUnits != kDMSLatLongUnitsMenuItem
					{
					//decimalPlaces = 2;
					//if (viewUnits == kDecimalLatLongUnitsMenuItem)
					//	decimalPlaces = 6;
					
					decimalPlaces = GetCoordinateViewUnitDecimalPlaces (windowInfoHandle);
					
					if (numberCharacters < 60)
						{
						numberYChars = sprintf ((char*)gTextString, 
														"%.*f",
														decimalPlaces,  
														yCoordinateValue);
														
						numberXChars = sprintf ((char*)gTextString2, 
														"%.*f",
														decimalPlaces,  
														xCoordinateValue);
							
						}	// end "if (numberCharacters < 60"
						
					else	// numberCharacters >= 60
						{
						numberYChars = sprintf ((char*)gTextString, 
														"%.*e",
														decimalPlaces,  
														yCoordinateValue);
														
						numberXChars = sprintf ((char*)gTextString2, 
														"%.*e",
														decimalPlaces,  
														xCoordinateValue);
							
						}	// end "else numberCharacters >= 60"
													
					}	// end "else viewUnits != kDMSLatLongUnitsMenuItem"
				break;
			
			}	// end "switch (viewUnits)"

		if (numberCharacters < 60 && viewUnits != kDMSLatLongUnitsMenuItem)
			{
			numberYChars = InsertCommasInNumberString ((char*)gTextString,
																		numberYChars, 
																		decimalPlaces,
																		numberCharacters);

			numberXChars = InsertCommasInNumberString ((char*)gTextString2,
																		numberXChars, 
																		decimalPlaces,
																		numberCharacters);
			
			}	// end "if (numberCharacters < 60 && viewUnits != ..."
			
		}	// end "if (gPresentCursor == kCross)"
					
	else	// gPresentCursor != kCross && != kBlinkOpenCursor2
		{
		numberYChars = sprintf ((char*)gTextString, " ");
		numberXChars = sprintf ((char*)gTextString2, " ");
		
		}	// end "else gPresentCursor != kCross && ..."
		        
	#if defined multispec_mac
		TextFont (gWindowTextFont); 			// monaco  
		TextSize (9);
		TextMode (srcCopy);	
		
		RGBBackColor (&gCoordinateViewBackgroundColor);
					
		MoveTo (xCursorStart, 10);
		DrawText (gTextString, 0, numberYChars); 
			
		MoveTo (xCursorStart, 21);
		DrawText (gTextString2, 0, numberXChars); 
		
		TextMode (srcOr);
		BackColor (whiteColor);
	#endif	// defined multispec_mac   
	                             
	#if defined multispec_win
		CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
		CMImageFrame* 	imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
		imageFrameCPtr->UpdateCursorCoordinates ((char*)gTextString,
																(char*)gTextString2);
	#endif	// defined multispec_win 

	#if defined multispec_wx
		CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
		CMImageFrame* imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();	
		imageFrameCPtr->UpdateCursorCoordinates ((char*) gTextString,
																(char*) gTextString2);
	#endif	// defined multispec_wx

}	// end "DrawCursorCoordinates" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawMapUnitsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							map units popup menu box.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2000
//	Revised By:			Larry L. Biehl			Date: 04/18/2000	

pascal void DrawMapUnitsPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpMapUnitsMenu, 
								gMapUnitsSelection, 
								TRUE);
	
}	// end "DrawMapUnitsPopUp"
#endif	// defined multispec_mac 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawDatumPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							datum selection for datum pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/30/2001
//	Revised By:			Larry L. Biehl			Date: 05/30/2001	

pascal void DrawDatumPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpHorizontalDatumMenu, 
								gDatumSelection, 
								TRUE);
	
}	// end "DrawDatumPopUp"
#endif	// defined multispec_mac 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawEllipsoidPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							classification pocedure selection for 
//							classification procedure pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2000
//	Revised By:			Larry L. Biehl			Date: 04/18/2000	

pascal void DrawEllipsoidPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpEllipsoidMenu, 
								gEllipsoidSelection, 
								TRUE);
	
}	// end "DrawEllipsoidPopUp"
#endif	// defined multispec_mac 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawProjectionPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							classification pocedure selection for 
//							classification procedure pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/11/2012
//	Revised By:			Larry L. Biehl			Date: 03/11/2012	

pascal void DrawProjectionPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpProjectionMenu, 
								gProjectionSelection, 
								TRUE);
	
}	// end "DrawProjectionPopUp"
#endif	// defined multispec_mac 


#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawReferenceSystemPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							classification pocedure selection for 
//							classification procedure pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/12/2012
//	Revised By:			Larry L. Biehl			Date: 03/14/2012	

pascal void DrawReferenceSystemPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpReferenceSystemMenu, 
								gReferenceSystemSelection, 
								TRUE);
	
}	// end "DrawReferenceSystemPopUp"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawScaleInformation
//
//	Software purpose:	This draws the current scale information in the coordinate
//							view of the specified image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/10/2000
//	Revised By:			Larry L. Biehl			Date: 12/07/2011			

void DrawScaleInformation (
				Handle								windowInfoHandle)

{	
	Str255								scaleTitleString,
											scaleValueString;
	
	double								scale;
	
	DisplaySpecsPtr					displaySpecsPtr;
	
	Handle								displaySpecsHandle;
		
	SInt16								numberTitleChars,
											numberValueChars,
											xCursorStart;
		 
		                             
	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
	
	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (displaySpecsHandle);
	
	if (displaySpecsPtr != NULL)
		{																	
		scale = displaySpecsPtr->scale;	
				
		if (scale > 0)
			{	
			xCursorStart = (SInt16)GetCoordinateViewScaleStart (windowInfoHandle);
			
			numberTitleChars = sprintf ((char*)&scaleTitleString, "Scale");
				
			if (scale >= 1)
				{						
				numberValueChars = sprintf ((char*)gTextString2, 
														"%.0f",
														scale);

				numberValueChars = InsertCommasInNumberString ((char*)gTextString2,
																				numberValueChars, 
																				-1,
																				numberValueChars);
					
				}	// end "if (scale >= 1)"
				
			else	// scale < 1
				{						
				numberValueChars = sprintf ((char*)gTextString2, 
														"%f",
														scale);
					
				}	// end "else scale < 1"
				
			numberValueChars = sprintf ((char*)&scaleValueString, 
													"1:%*s",
													-17,
													(char*)gTextString2);
				
			#if defined multispec_mac  		
				RGBColor								savedForeGroundColor;
		
				TextFont (gWindowTextFont); 
				TextSize (9);
				TextMode (srcCopy);
				RGBBackColor (&gCoordinateViewBackgroundColor);
				
				GetForeColor (&savedForeGroundColor);
				if (GetWindowPtr (windowInfoHandle) != FrontWindow ())
							// Draw gray text since this is an inactive window. 
					RGBForeColor (&gInactiveForeGroundColor);
					
				MoveTo (xCursorStart, 10);
				DrawText (scaleTitleString, 0, numberTitleChars);
												
				MoveTo (xCursorStart, 21);
				DrawText (scaleValueString, 0, numberValueChars); 
				
				TextMode (srcOr);
				BackColor (whiteColor);
				RGBForeColor (&savedForeGroundColor);
			#endif	// defined multispec_mac  
				
			}	// end "if (scale > 0)"  
												  
		#if defined multispec_win || defined multispec_wx
			else	// scale <= 0
				sprintf ((char*)&scaleValueString, "");
			
			CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
			CMImageFrame* 	imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
			imageFrameCPtr->UpdateScaleInformation (scale, (char*)scaleValueString);
		#endif	// defined multispec_win || lin
		
		}	// end "if (displaySpecsPtr != NULL)"

}	// end "DrawScaleInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawSelectedAreaInformation
//
//	Software purpose:	This routine draws the selected area information in the 
//							coordinate view of the image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
//	Called By:			RectangleSelection in SSelectionUtility.cpp
//							ShowSelection in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/22/2000
//	Revised By:			Larry L. Biehl			Date: 04/16/2019

void DrawSelectedAreaInformation (
				Handle								windowInfoHandle,
				SInt64								numberPixels)

{
	double								areaFactor;
	
	ControlHandle						areaUnitsControl;
	
	SInt16								areaUnitsCode,
											numberChars,
											numberTitleChars,
											numberViewCharacters;
											
	#if defined multispec_mac
		SInt16								xCursorStart;
	#endif	// defined multispec_mac    
	
	#if defined multispec_win || defined multispec_wx
		CMImageFrame* 						imageFrameCPtr;
		CMImageView* 						imageViewCPtr;
	#endif	// defined multispec_win || lin
	
	
	areaUnitsCode = GetCoordinateViewAreaUnits (windowInfoHandle);
	areaUnitsControl = GetCoordinateViewAreaUnitsControl (windowInfoHandle);
	
			// Get the area description text.
		
	#if defined multispec_mac	
		GetControlPopUpMenuText (areaUnitsControl,
											areaUnitsCode,
											(char*)gTextString2);
	#endif	// defined multispec_mac   
	                             
	#if defined multispec_win || defined multispec_wx
		imageViewCPtr = GetWindowPtr (windowInfoHandle);
		imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();

		#if defined multispec_win
			imageFrameCPtr->GetCoordinateViewComboText ((char*)gTextString2,
																		IDC_AreaUnitsCombo);
		#endif	// defined multispec_win
	#endif	// defined multispec_win || lin
	
	numberTitleChars = sprintf ((char*)gTextString, 
									"%s",
									&gTextString2[1]);
			
			// Draw the number of pixels or area represented by the number of pixels.
									
	numberViewCharacters = 
					(SInt16)GetCoordinateViewNumberPixelsChars (windowInfoHandle);
			
	if (GetCoordinateViewAreaUnits (windowInfoHandle) <= kNumberPixelsUnitsMenuItem)
		{
		numberChars = sprintf ((char*)gTextString2, 
										"%lld",
										numberPixels);

		numberChars = InsertCommasInNumberString (
										(char*)gTextString2, 
										numberChars, 
										-1,
										-numberViewCharacters);
										
		}	// end "if (GetCoordinateViewAreaUnits ... <= kNumberPixelsUnitsMenuItem)"
										
	else	// coordinateViewAreaUnitCode > kNumberPixelsUnitsMenuItem
		{
		areaFactor = GetCoordinateViewAreaFactor (windowInfoHandle);
		
		numberChars = sprintf ((char*)gTextString2, 
										"%.2f",
										(double)numberPixels * areaFactor);

		numberChars = InsertCommasInNumberString ((char*)gTextString2,
																numberChars, 
																2,
																-numberViewCharacters);
		
		}	// end "else coordinateViewAreaUnitCode > kNumberPixelsUnitsMenuItem"
		
	#if defined multispec_mac	
		xCursorStart = GetCoordinateViewNumberPixelsStart (windowInfoHandle);
										
		MoveTo (xCursorStart, 10);
		DrawText (gTextString, 0, numberTitleChars);
										
		MoveTo (xCursorStart+4, 21);
		DrawText (gTextString2, 0, numberChars); 
	#endif	// defined multispec_mac   
	                             
	#if defined multispec_win || defined multispec_wx
		imageViewCPtr = GetWindowPtr (windowInfoHandle);
		imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
		#if defined multispec_wx
			CMCoordinateBar* coordinatesBar = imageFrameCPtr->m_coordinatesBar;
			//((wxStaticText*)coordinatesBar->FindWindow (IDC_NumberPixelsPrompt))->Show (true);
			((wxStaticText*)coordinatesBar->FindWindow (IDC_NumberPixels))->Show (true);
			((wxWindow*)coordinatesBar->FindWindow (IDC_AreaUnitsCombo))->Show (true);
		#endif	// multispec_wx

		imageFrameCPtr->UpdateSelectedAreaInformation ((char*)gTextString,
																		(char*)gTextString2);
	#endif	// defined multispec_win || lin
			
}	// end "DrawSelectedAreaInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawSelectionCoordinates
//
//	Software purpose:	This routine draws the selected area coordinates in the 
//							coordinate view of the image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
//	Called By:			RectangleSelection in SSelectionUtility.cpp
//							ShowSelection in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/01/2000
//	Revised By:			Larry L. Biehl			Date: 04/16/2019

void DrawSelectionCoordinates (
				Handle								windowInfoHandle,
				LongRect*							lineColumnRectPtr,
				DoubleRect*							coordinateRectPtr,
				SInt64								numberPixels)

{
	Str63									beginYTextString,
											beginXTextString,
											endYTextString,
											endXTextString;
	
	SInt16								coordinateViewUnits,
											decimalPlaces,
											numberChars,
											numberEndChars,
											numberViewCharacters,
											numberXChars,
											numberYChars;
											
	#if defined multispec_mac
		RGBColor								savedForeGroundColor;
		
		SInt16								xCursorStart;
	#endif	// defined multispec_mac   
											

			// Initialize local variables.
	
	numberViewCharacters = (SInt16)GetCoordinateViewNumberChars (windowInfoHandle);
	coordinateViewUnits = GetCoordinateViewUnits (windowInfoHandle);

			// Now draw the current selection information
	
	if (coordinateViewUnits == kLineColumnUnitsMenuItem)
		{							
		numberChars = sprintf ((char*)&beginYTextString, 
											"%d",
											(int)lineColumnRectPtr->top);

		numberChars = InsertCommasInNumberString (
											(char*)&beginYTextString, 
											numberChars, 
											-1,
											numberViewCharacters);
										
		numberChars = sprintf ((char*)&endYTextString, 
											"%d",
											(int)lineColumnRectPtr->bottom);

		numberChars = InsertCommasInNumberString (
											(char*)&endYTextString, 
											numberChars, 
											-1,
											-numberViewCharacters);
											
				// Load beginning and ending column (x) selections.
										
		numberChars = sprintf ((char*)&beginXTextString, 
											"%d",
											(int)lineColumnRectPtr->left);

		numberChars = InsertCommasInNumberString (
											(char*)&beginXTextString, 
											numberChars, 
											-1,
											numberViewCharacters);
										
		numberChars = sprintf ((char*)&endXTextString, 
											"%d",
											(int)lineColumnRectPtr->right);

		numberChars = InsertCommasInNumberString (
											(char*)&endXTextString, 
											numberChars, 
											-1,
											-numberViewCharacters);
													
		numberYChars = sprintf ((char*)gTextString, 
											"%s-%s",
											beginYTextString, 
											endYTextString);
													
		numberXChars = sprintf ((char*)gTextString2, 
											"%s-%s",
											beginXTextString, 
											endXTextString);
		
		}	// end "if (coordinateViewUnits == kLineColumnUnitsMenuItem)"
		
	else	// coordinateViewUnits != kLineColumnUnitsMenuItem
		{
		if (coordinateViewUnits == kDMSLatLongUnitsMenuItem)
			{
			LoadDMSLatLongStrings (coordinateRectPtr->top,
											coordinateRectPtr->left,
											(char*)&beginYTextString,
											(char*)&beginXTextString);
											
			LoadDMSLatLongStrings (coordinateRectPtr->bottom,
											coordinateRectPtr->right,
											(char*)&endYTextString,
											(char*)&endXTextString);
			
			}	// end "if (coordinateViewUnits == kDMSLatLongUnitsMenuItem)"
					
		else	// coordinateViewUnits != kDMSLatLongUnitsMenuItem
			{									
			//decimalPlaces = 2;
			//if (coordinateViewUnits == kDecimalLatLongUnitsMenuItem)
			//	decimalPlaces = 6;
			
			decimalPlaces = GetCoordinateViewUnitDecimalPlaces (windowInfoHandle);
						
					// Get the length of the end x and y strings	
							
			if (numberViewCharacters < 60)
				{
				numberXChars = sprintf ((char*)&endXTextString, 
													"%.*f",
													decimalPlaces,
													coordinateRectPtr->right);
				numberXChars = InsertCommasInNumberString (
													(char*)&endXTextString, 
													numberXChars, 
													decimalPlaces,
													numberXChars);
											
				numberYChars = sprintf ((char*)&endYTextString,  
													"%.*f",
													decimalPlaces,
													coordinateRectPtr->bottom);
				numberYChars = InsertCommasInNumberString (
													(char*)&endYTextString, 
													numberYChars, 
													decimalPlaces,
													numberYChars);
													
				numberEndChars = MAX (numberXChars, numberYChars);
				
						// Load beginning and ending line (y) selections.
												
				numberChars = sprintf ((char*)&beginYTextString,  
													"%.*f",
													decimalPlaces,
													coordinateRectPtr->top);
				numberChars = InsertCommasInNumberString (
													(char*)&beginYTextString, 
													numberChars, 
													decimalPlaces,
													numberViewCharacters);
												
				numberChars = sprintf ((char*)&endYTextString,  
													"%.*f",
													decimalPlaces,
													coordinateRectPtr->bottom);
				numberChars = InsertCommasInNumberString (
													(char*)&endYTextString, 
													numberChars, 
													decimalPlaces,
													numberEndChars);
													
						// Load beginning and ending column (x) selections.
												
				numberChars = sprintf ((char*)&beginXTextString,  
													"%.*f",
													decimalPlaces,
													coordinateRectPtr->left);
				numberChars = InsertCommasInNumberString (
													(char*)&beginXTextString, 
													numberChars, 
													decimalPlaces,
													numberViewCharacters);
												
				numberChars = sprintf ((char*)&endXTextString, 
													"%.*f",
													decimalPlaces,
													coordinateRectPtr->right);
				numberChars = InsertCommasInNumberString (
													(char*)&endXTextString, 
													numberChars, 
													decimalPlaces,
													numberEndChars);
					
				}	// end "if (numberViewCharacters < 60)"
				
			else	// numberViewCharacters >= 60
				{
						// Use e-format
				numberXChars = sprintf ((char*)&endXTextString, 
													"%.*e",
													decimalPlaces,
													coordinateRectPtr->right);
											
				numberYChars = sprintf ((char*)&endYTextString,  
													"%.*e",
													decimalPlaces,
													coordinateRectPtr->bottom);
													
				numberEndChars = MAX (numberXChars, numberYChars);
				
						// Load beginning and ending line (y) selections.
												
				numberChars = sprintf ((char*)&beginYTextString,  
													"%.*e",
													decimalPlaces,
													coordinateRectPtr->top);
												
				numberChars = sprintf ((char*)&endYTextString,  
													"%.*e",
													decimalPlaces,
													coordinateRectPtr->bottom);
													
						// Load beginning and ending column (x) selections.
												
				numberChars = sprintf ((char*)&beginXTextString,  
													"%.*e",
													decimalPlaces,
													coordinateRectPtr->left);
												
				numberChars = sprintf ((char*)&endXTextString, 
													"%.*e",
													decimalPlaces,
													coordinateRectPtr->right);
					
				}	// end "else numberViewCharacters >= 60"
												
			}	// end "else viewUnits != kDMSLatLongUnitsMenuItem"
													
		numberYChars = sprintf ((char*)gTextString, 
											"%s - %s",
											beginYTextString, 
											endYTextString);
												
		numberXChars = sprintf ((char*)gTextString2, 
											"%s - %s",
											beginXTextString, 
											endXTextString);
		
		}	// end "else coordinateViewUnits != kLineColumnUnitsMenuItem"
			
	#if defined multispec_mac	
				// Set the text parameters.					
		TextSize (9);
		TextFont (gWindowTextFont);
		TextMode (srcCopy);
		RGBBackColor (&gCoordinateViewBackgroundColor);
			
		GetForeColor (&savedForeGroundColor);
		if (GetWindowPtr (windowInfoHandle) != FrontWindow ())
					// Draw gray text since this is an inactive window. 
			RGBForeColor (&gInactiveForeGroundColor);
			
		xCursorStart = GetCoordinateViewSelectionStart (windowInfoHandle);
											
		MoveTo (xCursorStart, 10);
		DrawText (gTextString, 0, numberYChars);
												
		MoveTo (xCursorStart, 21);
		DrawText (gTextString2, 0, numberXChars); 
		
				// Draw the number of pixels.
		
		DrawSelectedAreaInformation (windowInfoHandle, numberPixels);

		TextMode (srcOr);
		BackColor (whiteColor);
		RGBForeColor (&savedForeGroundColor);
	#endif	// defined multispec_mac   
	                             
	#if defined multispec_win || defined multispec_wx
		CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
		CMImageFrame* 	imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();

		#if defined multispec_wx
			((wxStaticText*)imageFrameCPtr->m_coordinatesBar->FindWindow (
																	IDC_SelectionLine))->Show (true);
			((wxStaticText*)imageFrameCPtr->m_coordinatesBar->FindWindow (
																	IDC_SelectionColumn))->Show (true);
		#endif	// multispec_wx
		
		imageFrameCPtr->UpdateSelectionCoordinates ((char*)gTextString,
																	(char*)gTextString2);
		
				// Draw the number of pixels.
		
		DrawSelectedAreaInformation (windowInfoHandle, numberPixels);
	#endif	// defined multispec_win || lin
	
}	// end "DrawSelectionCoordinates" 



//- ----------------------------------------------------------------------------------
// Function to compute the constant e4 from the input of the eccentricity
//   of the spheroid, x.  This constant is used in the Polar Stereographic
//   projection.

double e4fn (
				double								x)

{
	double								con,
											com;
								
								
	con = 1.0 + x;
	com = 1.0 - x;
	return (sqrt ((pow (con,con))*(pow (com,com))));
 
}	// end "e4fn"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FindIfMapInformationExists
//
//	Software purpose:	This routine draws the selected area coordinates in the 
//							coordinate view of the image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
//	Called By:			AddCurrentVectorOverlaysToImageWindow in SArcView.cpp
//							CheckIfVectorOverlaysIntersectImage in SArcView.cpp
//							SetUpImageOverlayInformation in SImageOverlays.cpp
//							UpdateFileImageSaveAs in SMenus.cpp
//							ChangeImageFormatDialogInitialize in SReformatChangeImageFileFormat.cpp
//							InitializeReformatStructure in SReformatChangeImageFileFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/01/2002
//	Revised By:			Larry L. Biehl			Date: 06/28/2010			

Boolean FindIfMapInformationExists (
				WindowInfoPtr						windowInfoPtr)

{
	FileInfoPtr							fileInfoPtr = NULL;
	
	Boolean								mapInfoExistsFlag = FALSE;
	
	
			// Determine if any planar coordinate information exists to write
			// to a blw file. First, get a pointer to the map projection information 
			// if it exists.	
	
	if (windowInfoPtr	!= NULL)
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (windowInfoPtr->fileInfoHandle);
								
	if (fileInfoPtr != NULL)							
		mapInfoExistsFlag = FindIfMapInformationExists (
																fileInfoPtr->mapProjectionHandle);
		
	return (mapInfoExistsFlag);
			
}	// end "FindIfMapInformationExists" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FindIfMapInformationExists
//
//	Software purpose:	This routine draws the selected area coordinates in the 
//							coordinate view of the image window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
//	Called By:			FindIfMapInformationExists (WindowInfoPtr)
//							UpdateOutputFileStructure
//
//	Coded By:			Larry L. Biehl			Date: 06/28/2010
//	Revised By:			Larry L. Biehl			Date: 04/14/2012			

Boolean FindIfMapInformationExists (
				Handle								mapProjectionHandle)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;
	
	Boolean								mapInfoExistsFlag = FALSE;
	
	
			// Determine if any planar coordinate information exists to write
			// to a blw file. First, get a pointer to the map projection information 
			// if it exists.	
	

	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																			mapProjectionHandle);
	
	if (mapProjectionInfoPtr != NULL)
		{			
		mapInfoExistsFlag = TRUE;
						
		if ((mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize == 0 ||
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize == 0) &&
								mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
			mapInfoExistsFlag = FALSE;
														
		if (mapInfoExistsFlag && 
					mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 0 &&
								mapProjectionInfoPtr->gridCoordinate.projectionCode == 0)
			mapInfoExistsFlag = FALSE;
			
		}	// end "if (mapProjectionInfoPtr != NULL)"
		
	return (mapInfoExistsFlag);
			
}	// end "FindIfMapInformationExists" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetAreaNumberWidths
//
//	Software purpose:	The purpose of this routine is to determine the number of 
//							digits need before and after the decimal point for a give
//							range of values. 
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//							
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/20/2003
//	Revised By:			Larry L. Biehl			Date: 12/16/2005

void GetAreaNumberWidths (
				double								areaConversionFactor,
				SInt64								maxNumberPoints,
				SInt16*								totalNumberDigitsPtr,
				SInt16*								numberDigitsAfterDecimalPtr)

{
	double								maxAreaValue,
											minAreaValue;
	
	SInt16								index,
											indexEnd,
											numberChars,
											numberDigitsAfter,
											numberDigitsBefore;
	
	
	if (areaConversionFactor > 0)
		{
				// Get digits in front of decimal.											
				
		maxAreaValue = (double)maxNumberPoints * 10 * areaConversionFactor;
		numberDigitsBefore = sprintf ((char*)gTextString, "%.0f", maxAreaValue) - 1;
		
				// Get digits needed for after decimal.	
				// Make sure that minimum decimal value will represent at least .1%
				// of the total number of points.
		
		minAreaValue = .1 * maxAreaValue * .001;
		if (minAreaValue >= 1)
			{		
			numberDigitsAfter = 3;
			numberChars = sprintf ((char*)gTextString, 
											"%15.*f",
											numberDigitsAfter, 
											minAreaValue);
			numberChars--;
			
			indexEnd = numberChars - numberDigitsAfter;
			for (index=numberChars; index>indexEnd; index--)
				{
				if (gTextString[index] == '0')
					numberDigitsAfter--;
					
				else	// gTextString[index] != '0' 
					break;
					
				}	// end "for (index=numberChars; index>indexEnd; index--)" 
				
			}	// end "if (areaConversionFactor >= 1)"
			
		else	// minAreaValue < 1
			{
			numberDigitsAfter = (SInt16)GetNumberLeadingDecimalZeros (minAreaValue);
			/*
			numberDigitsAfter = 0;
			while (minAreaValue < 1)
				{
				numberDigitsAfter++;
				minAreaValue *= 10;
								
				}	// end "while (minAreaValue < 1)"
			*/
			numberDigitsAfter = MAX (numberDigitsAfter, 3);
			
			}	// end "else minAreaValue < 1"
			
		}	// end "if (areaConversionFactor > 0)"
		
	else	// areaConversionFactor <= 0
		{
		numberDigitsBefore = 1;
		numberDigitsAfter = 1;
		
		}	// end "else areaConversionFactor <= 0"
		
	*totalNumberDigitsPtr = numberDigitsBefore + numberDigitsAfter + 1;
	*numberDigitsAfterDecimalPtr = numberDigitsAfter;
			
}	// end "GetAreaNumberWidths" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetAreaUnits
//
//	Software purpose:	The purpose of this routine is to get the area units. 
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//							
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2003
//	Revised By:			Larry L. Biehl			Date: 02/27/2003

SInt16 GetAreaUnits (
				Handle								imageWindowInfoHandle,
				Boolean								fromCoordinatesViewFlag)

{
	SInt16 								areaUnitsIndex;
	
		
	areaUnitsIndex = gDefaultAreaUnits;
	
	if (fromCoordinatesViewFlag)
		areaUnitsIndex = GetCoordinateViewAreaUnits (imageWindowInfoHandle);
		
	return (areaUnitsIndex);
			
}	// end "GetAreaUnits" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetAreaUnitString
//
//	Software purpose:	The purpose of this routine is to get the area units string. 
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//							
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/20/2003
//	Revised By:			Larry L. Biehl			Date: 02/27/2003

SInt16 GetAreaUnitString (
				Handle								imageWindowInfoHandle,
				Boolean								fromCoordinateViewFlag,
				char*									areaUnitsStringPtr)

{
	SInt16 								areaUnitsIndex;
	
	
			//	Get the units for the area string. Note that hectares is used
			// unless the display area units options is set to something other 
			// than number of pixels.
	
	areaUnitsIndex = GetAreaUnits (imageWindowInfoHandle, fromCoordinateViewFlag);
	
	#if defined multispec_mac
		GetMenuItemText (gPopUpAreaUnitsMenu, 
								areaUnitsIndex, 
								(UCharPtr)areaUnitsStringPtr);
								
				// Check the end of the string for the c-string terminator. If there,
				// remove from the count for the length of the string.
				
		if (areaUnitsStringPtr[areaUnitsStringPtr[0]] == 0)
			areaUnitsStringPtr[0]--;
		
		areaUnitsStringPtr[areaUnitsStringPtr[0]+1] = 0;
	#endif	// defined multispec_mac   
                       
	#if defined multispec_win || defined multispec_wx
		MGetString ((UCharPtr)areaUnitsStringPtr,
							0,
							IDS_MapAreaUnits01+areaUnitsIndex-2);
	#endif	// defined multispec_win
	
	return (areaUnitsStringPtr[0]);
			
}	// end "GetAreaUnitString" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetConversionFromMetersToNativeMapUnits
//
//	Software purpose:	The purpose of this routine is to determine the factor to use to 
//							convert from meters (which is output of all non-geographic 
//							conversions to the native units for the image file. This is not 
//							many but some state planes are in survey feet. 
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	Conversion factor to use
//
// Called By:			CoordinateDialogOK
//							FinishMapInformationSetUp in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/29/2012
//	Revised By:			Larry L. Biehl			Date: 04/29/2012

double GetConversionFromMetersToNativeMapUnits (
				SInt16								mapUnitsCode)

{
	SInt16								fileMapUnitsIndex;
	
	
	fileMapUnitsIndex = mapUnitsCode - kKilometersCode;
	
	if (fileMapUnitsIndex <= 1)
		return (1);
		
	else
		return (gDistanceFileConversionFactors[1] / 
											gDistanceFileConversionFactors[fileMapUnitsIndex]);
			
}	// end "GetConversionFromMetersToNativeMapUnits" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetCursorCoordinates
//
//	Software purpose:	The purpose of this routine is to calculate the current cursor
//							coordinates in the requested units. 
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	True, if coordinates have changed
//							False, if coordinates have not changed.
//
// Called By:			UpdateCursorCoordinates in xImageView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/08/2000
//	Revised By:			Larry L. Biehl			Date: 05/17/2012

Boolean GetCursorCoordinates (
				LongPoint*							longMousePointPtr)

{
	DoublePoint							doubleMouseLC;
	
	LongPoint							mouseLC;
											
	Boolean								updateMapCoordinateFlag;
	
	  
	updateMapCoordinateFlag = FALSE;
	
	switch (GetCoordinateViewUnits (gActiveImageWindowInfoH))
		{
		case kLineColumnUnitsMenuItem:
			ConvertWinPointToLC (longMousePointPtr, &mouseLC);
			
			if (gCoordinateLineValue != mouseLC.v)
				{
				gCoordinateLineValue = mouseLC.v;
				updateMapCoordinateFlag = TRUE;
				
				}	// end "if (gCoordinateLineValue != mouseLC.v)" 
				
			if (gCoordinateColumnValue != mouseLC.h)
				{
				gCoordinateColumnValue = mouseLC.h;
				updateMapCoordinateFlag = TRUE;
				
				}	// end "if (gCoordinateColumnValue != mouseLC.h)"
			break;
		
		case kUnknownUnitsMenuItem:
		case kDecimalLatLongUnitsMenuItem:
		case kDMSLatLongUnitsMenuItem:
		case kKilometersUnitsMenuItem:
		case kMetersUnitsMenuItem:
		case kCentimetersUnitsMenuItem:
		case kMillimetersUnitsMenuItem:
		case kMicrometersUnitsMenuItem:
		case kMilesUnitsMenuItem:
		case kYardsUnitsMenuItem:
		case kUSSurveyFeetUnitsMenuItem:
		case kFeetUnitsMenuItem:
		case kInchesUnitsMenuItem:
			ConvertWinPointToDoubleLC (longMousePointPtr, &doubleMouseLC);
			
			if (gDoubleCoordinateLineValue != doubleMouseLC.v)
				{
				gDoubleCoordinateLineValue = doubleMouseLC.v;
				updateMapCoordinateFlag = TRUE;
				
				}	// end "if (gDoubleCoordinateLineValue != mouseLC.v)" 
				
			if (gDoubleCoordinateColumnValue != doubleMouseLC.h)
				{
				gDoubleCoordinateColumnValue = doubleMouseLC.h;
				updateMapCoordinateFlag = TRUE;
				
				}	// end "if (gDoubleCoordinateColumnValue != mouseLC.h)"
			break;
		
		}	// end "switch (GetCoordinateViewUnits (windowInfoHandle))"
		
	return (updateMapCoordinateFlag);
			
}	// end "GetCursorCoordinates"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetBoundingMapRectangle
//
//	Software purpose:	This routine returns a rectangle in map units that
//							represents the input window rectangle for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			DrawArcViewShapes in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/15/2003
//	Revised By:			Larry L. Biehl			Date: 06/22/2020

void GetBoundingMapRectangle (
				WindowPtr							windowPtr,
				Handle								windowInfoHandle,
				Rect*									windowRectPtr,
				SInt16								windowCode,
				DoubleRect*							boundingMapRectPtr)

{ 	
	DoubleRect							boundingMapRect;
	LongRect								windowLongRect;
	
		
			// Get the rectangle in map units that is being updated in
			// the current window.

	if (windowRectPtr != NULL && windowCode == kToImageWindow)
		{	   		
		windowLongRect.top = windowRectPtr->top;
		windowLongRect.left = windowRectPtr->left;
		windowLongRect.bottom = windowRectPtr->bottom;
		windowLongRect.right = windowRectPtr->right;

		ConvertWinRectToMapRect (windowInfoHandle,
											&windowLongRect, 
											&boundingMapRect, 
											FALSE,
											FALSE,
											kAllCorners);	
		
		}	// end "if (windowRectPtr != NULL && ..."

	else	// windowRectPtr = NULL || windowCode != kToImageWindow
		GetWindowMapRect (windowPtr,
									windowInfoHandle, 
									&boundingMapRect, 
									FALSE,
									windowCode,
									kAllCorners);
									
			// Force top-left to be minimum values and bottom-right to be largest
			// values.
	
	boundingMapRectPtr->top = MIN (boundingMapRect.top, boundingMapRect.bottom);
	boundingMapRectPtr->bottom = MAX (boundingMapRect.top, boundingMapRect.bottom);
	boundingMapRectPtr->left = MIN (boundingMapRect.left, boundingMapRect.right);
	boundingMapRectPtr->right = MAX (boundingMapRect.left, boundingMapRect.right);
			
}	// end "GetBoundingMapRectangle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetBoundingOrientationAngleRect
//
//	Software purpose:	This routine get the bounding rectangle for the input map
//							coordinate rectangle which has not had the map orientation angle
//							taken into account.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ConvertWinRectToMapRect in SMapCoordinates.cpp
//							SetBoundingMapRectangle in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2005
//	Revised By:			Larry L. Biehl			Date: 07/25/2006		

void GetBoundingOrientationAngleRect (
				DoubleRect*							boundingMapRectPtr,
				PlanarCoordinateSystemInfoPtr	planarCoordinatePtr,
				UInt16								boundingCode)

{
	DoubleRect							lBoundingMapRect;
	DoublePoint							mapPoint;
	
	
	if (boundingCode == kAllCorners)
		{
		mapPoint.h = boundingMapRectPtr->left;
		mapPoint.v = boundingMapRectPtr->top;
		ConvertMapPointToOrientationAngleMapPoint (
									planarCoordinatePtr->mapOrientationAngle, 
									planarCoordinatePtr->xMapOrientationOrigin, 
									planarCoordinatePtr->yMapOrientationOrigin,
									&mapPoint);
		lBoundingMapRect.top = mapPoint.v;
		lBoundingMapRect.left = mapPoint.h;
		
		mapPoint.h = boundingMapRectPtr->right;
		mapPoint.v = boundingMapRectPtr->top;
		ConvertMapPointToOrientationAngleMapPoint (
									planarCoordinatePtr->mapOrientationAngle, 
									planarCoordinatePtr->xMapOrientationOrigin, 
									planarCoordinatePtr->yMapOrientationOrigin,
									&mapPoint);
		lBoundingMapRect.top = MAX (lBoundingMapRect.top, mapPoint.v);
		lBoundingMapRect.right = mapPoint.h;
		
		mapPoint.h = boundingMapRectPtr->right;
		mapPoint.v = boundingMapRectPtr->bottom;
		ConvertMapPointToOrientationAngleMapPoint (
									planarCoordinatePtr->mapOrientationAngle, 
									planarCoordinatePtr->xMapOrientationOrigin, 
									planarCoordinatePtr->yMapOrientationOrigin,
									&mapPoint);
		lBoundingMapRect.bottom = mapPoint.v;
		lBoundingMapRect.right = MAX (lBoundingMapRect.right, mapPoint.h);
		
		mapPoint.h = boundingMapRectPtr->left;
		mapPoint.v = boundingMapRectPtr->bottom;
		ConvertMapPointToOrientationAngleMapPoint (
									planarCoordinatePtr->mapOrientationAngle, 
									planarCoordinatePtr->xMapOrientationOrigin, 
									planarCoordinatePtr->yMapOrientationOrigin,
									&mapPoint);
		lBoundingMapRect.bottom = MIN (lBoundingMapRect.bottom, mapPoint.v);
		lBoundingMapRect.left = MIN (lBoundingMapRect.left, mapPoint.h);
		
		*boundingMapRectPtr = lBoundingMapRect;
		
		}	// end "if (boundingCode == kAllCorners)"
		
	else	// boundingCode != kAllCorners
		{
		mapPoint.h = boundingMapRectPtr->left;
		mapPoint.v = boundingMapRectPtr->top;
		ConvertMapPointToOrientationAngleMapPoint (
									planarCoordinatePtr->mapOrientationAngle, 
									planarCoordinatePtr->xMapOrientationOrigin, 
									planarCoordinatePtr->yMapOrientationOrigin,
									&mapPoint);
		boundingMapRectPtr->left = mapPoint.h;
		boundingMapRectPtr->top = mapPoint.v;
		
		mapPoint.h = boundingMapRectPtr->right;
		mapPoint.v = boundingMapRectPtr->bottom;
		ConvertMapPointToOrientationAngleMapPoint (
									planarCoordinatePtr->mapOrientationAngle, 
									planarCoordinatePtr->xMapOrientationOrigin, 
									planarCoordinatePtr->yMapOrientationOrigin,
									&mapPoint);
		boundingMapRectPtr->right = mapPoint.h;
		boundingMapRectPtr->bottom = mapPoint.v;
		
		}	// end "else boundingCode != kAllCorners"

}	// end "GetBoundingOrientationAngleRect" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle GetCoefficientsHandle
//
//	Software purpose:	This routine returns the code for the grid coordinate system.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/24/2007
//	Revised By:			Larry L. Biehl			Date: 02/24/2007		

Handle GetCoefficientsHandle (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->coefficientsHandle);    
	     
	return (NULL);
	
}	// end "GetCoefficientsHandle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetCoefficientsVectorPointers
//
//	Software purpose:	The purpose of this routine is to set the pointers to the
// 						plygon coefficient vectors.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/13/2007
//	Revised By:			Larry L. Biehl			Date: 02/13/2007

void GetCoefficientsVectorPointers (
				MapProjectionInfoPtr				mapProjectionInfoPtr)
				
{  
	PlanarCoordinateSystemInfoPtr	planarCoordinateInfoPtr;
	
	SInt32								numberCoefficients;
	
														
	if (mapProjectionInfoPtr != NULL && 
								mapProjectionInfoPtr->coefficientsHandle != NULL)
		{			
		numberCoefficients = mapProjectionInfoPtr->numberCoefficients;
		planarCoordinateInfoPtr = &mapProjectionInfoPtr->planarCoordinate;
		
		planarCoordinateInfoPtr->easting1CoefficientsPtr =
				(double*)GetHandlePointer (mapProjectionInfoPtr->coefficientsHandle,
														kLock);
		
		planarCoordinateInfoPtr->northing1CoefficientsPtr = 
				&planarCoordinateInfoPtr->easting1CoefficientsPtr[numberCoefficients]; 
		planarCoordinateInfoPtr->easting2CoefficientsPtr = 
				&planarCoordinateInfoPtr->northing1CoefficientsPtr[numberCoefficients];  
		planarCoordinateInfoPtr->northing2CoefficientsPtr = 
				&planarCoordinateInfoPtr->easting2CoefficientsPtr[numberCoefficients];  
		
		}	// end "if (controlPointsPtr != NULL)"
	
}	// end "GetCoefficientsVectorPointers"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetEllipsoidCodeFromDatumCode
//
//	Software purpose:	This routine returns the code for the ellipsoid based on the
//							datum.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/11/2012
//	Revised By:			Larry L. Biehl			Date: 03/21/2012		

SInt16 GetEllipsoidCodeFromDatumCode (
				SInt16								datumCode)

{
	SInt16							spheroidCode = kNoEllipsoidDefinedCode;
	

	switch (datumCode)
		{
		case kCampoCode:
			spheroidCode = kInternational1909EllipsoidCode;
			break;
			
		case kGeocentricDatumAustralia1994Code:
		case kGGRS87DatumCode:							
			spheroidCode = kGRS80EllipsoidCode;
			break;
			
		case kNAD27Code:						
			spheroidCode = kClarke1866EllipsoidCode;
			break;
			
		case kNAD83Code:						
			spheroidCode = kGRS80EllipsoidCode;
			break;
			
		case kNewZealandGeodeticDatum1949Code:
			spheroidCode = kInternational1909EllipsoidCode;
			break;
			
		case kDatum_OSGB_1936Code:
			spheroidCode = kAiryEllipsoidCode;
			break;
			
		case kPulkovo1942DatumCode:
			spheroidCode = kKrassovskyEllipsoidCode;
			break;
			
		case kSAD69DatumCode:
			spheroidCode = kGRS1967ModifiedEllipsoidCode;
			break;
			
		case kWGS72DatumCode:
			spheroidCode = kWGS72EllipsoidCode;
			break;
			
		case kWGS84Code:
			spheroidCode = kWGS84EllipsoidCode;
			break;
			
		case kSphereDatumCode:
			spheroidCode = kSphereEllipsoidCode;
			break;
			
		}	// end "switch (datumCode)"
		
	return (spheroidCode);
			
}	// end "GetEllipsoidCodeFromDatumCode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				Boolean GetEllipsoidParameters
//
//	Software purpose:			This routine returns the major and minor axis lengths
//									for the input ellipsoid code.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2012
//	Revised By:			Larry L. Biehl			Date: 04/14/2012			

Boolean GetEllipsoidParameters (
				SInt16								ellipsoidCode,
				double*								radiusSpheroidPtr,
				double*								semiMajorAxisPtr,
				double*								semiMinorAxisPtr)

{
	Boolean								parametersSetFlag = TRUE;
	

	switch (ellipsoidCode)
		{
		case kNoEllipsoidDefinedMenuItem:
		default:
			parametersSetFlag = FALSE;
			break;
			
		case kAiryEllipsoidCode:
			*semiMajorAxisPtr = 6377563.396;
			*semiMinorAxisPtr = 6356256.909;
			break;
			
		case kAustralianEllipsoidCode:
			*semiMajorAxisPtr = 6378160;
			*semiMinorAxisPtr = 6356774.719;
			break;
			
		case kBesselEllipsoidCode:
			*semiMajorAxisPtr = 6377397.155;
			*semiMinorAxisPtr = 6356079.963;
			break;
			
		case kBesselModifiedEllipsoidCode:
			*semiMajorAxisPtr = 6377492.018;
			*semiMinorAxisPtr = 6356173.508712696;
			break;
			
		case kClarke1866EllipsoidCode:
			*semiMajorAxisPtr = 6378206.4;
			*semiMinorAxisPtr = 6356583.8;
			break;
			
		case kClarke1880EllipsoidCode:
			*semiMajorAxisPtr = 6378249.145;
			*semiMinorAxisPtr = 6356514.87;
			break;
			
		case kEverestEllipsoidCode:
			*semiMajorAxisPtr = 6377276.345;
			*semiMinorAxisPtr = 6356075.413;
			break;
			
		case kGRS80EllipsoidCode:
			*semiMajorAxisPtr = 6378137.0;
			*semiMinorAxisPtr = 6356752.314140;
			break;
			
		case kInternational1909EllipsoidCode:
			*semiMajorAxisPtr = 6378388;
			*semiMinorAxisPtr = 6356911.946;
			break;
			
		case kKrassovskyEllipsoidCode:
			*semiMajorAxisPtr = 6378245;
			*semiMinorAxisPtr = 6356863.019;
			break;
			
		case kGRS1967ModifiedEllipsoidCode:
			*semiMajorAxisPtr = 6378160;
			*semiMinorAxisPtr = 6356774.719;
			break;
			
		case kWGS72EllipsoidCode:
			*semiMajorAxisPtr = 6378135;
			*semiMinorAxisPtr = 6356750.52;
			break;
			
		case kWGS84EllipsoidCode:
			*semiMajorAxisPtr = 6378137;
			*semiMinorAxisPtr = 6356752.3142;
			break;
			
		case kSphereEllipsoidCode:
			*radiusSpheroidPtr = 6370997;
			*semiMajorAxisPtr = 6370997;
			*semiMinorAxisPtr = 6370997;
			break;
		
		}	// end "switch (ellipsoidCode+1)"
		
	return (parametersSetFlag);
		
}	// end "GetEllipsoidParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFileProjectionCode
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 03/12/2012		

SInt16 GetFileProjectionCode (
				Handle								windowInfoHandle)

{ 
	Handle								mapProjectionHandle;
	  
		
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	     
	return (GetProjectionCode (mapProjectionHandle));
	
}	// end "GetFileProjectionCode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFileGridZone
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 04/03/2004		

SInt16 GetFileGridZone (
				Handle								windowInfoHandle)

{ 
	Handle								mapProjectionHandle;
	  
		
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	     
	return (GetGridZone (mapProjectionHandle));
	
}	// end "GetFileGridZone" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFilePlanarMapUnitsCode
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/09/2000
//	Revised By:			Larry L. Biehl			Date: 12/28/2000		

SInt16 GetFilePlanarMapUnitsCode (
				Handle								windowInfoHandle)

{ 
	Handle								mapProjectionHandle;
	  
		
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	     
	return (GetPlanarMapUnitsCode (mapProjectionHandle));
	
}	// end "GetFilePlanarMapUnitsCode" 




//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFileReferenceSystemCode
//
//	Software purpose:	This routine returns the reference system code for input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/13/2012
//	Revised By:			Larry L. Biehl			Date: 03/13/2012		

SInt16 GetFileReferenceSystemCode (
				Handle								windowInfoHandle)

{ 
	Handle								mapProjectionHandle;
	  
		
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	     
	return (GetReferenceSystemCode (mapProjectionHandle));
	
}	// end "GetFileReferenceSystemCode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetFileSpheroidCode
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 04/03/2004		

SInt16 GetFileSpheroidCode (
				Handle								windowInfoHandle)

{ 
	Handle								mapProjectionHandle;
	  
		
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	     
	return (GetSpheroidCode (mapProjectionHandle));
	
}	// end "GetFileSpheroidCode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetGridZone
//
//	Software purpose:	This routine returns the code for the grid coordinate system.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 04/03/2004		

SInt16 GetGridZone (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->gridCoordinate.gridZone);    
	     
	return (0);
	
}	// end "GetGridZone" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetMapOrientationAngle
//
//	Software purpose:	This routine returns the code for the spheroid.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2005
//	Revised By:			Larry L. Biehl			Date: 11/02/2005		

double GetMapOrientationAngle (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);    
	     
	return (0);
	
}	// end "GetMapOrientationAngle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle GetMapProjectionHandle
//
//	Software purpose:	The purpose of this routine is to get the memory for
//							the map projection structure and initialize the
//							structure.
//
//	Parameters in:		None.
//
//	Parameters out:	None
//
// Value Returned:  	Handle to map projection structure.  The handle
//							returned will be NULL if there is not enough
//							memory.
//
// Called By:			ReadArcViewMapInfo in SOpenImage.cpp
//							ReadERDASHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/08/1994
//	Revised By:			Larry L. Biehl			Date: 03/03/2005

Handle GetMapProjectionHandle (void)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	
	mapProjectionHandle = MNewHandle (sizeof (MapProjectionInfo));
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																			mapProjectionHandle);
	
	InitializeMapProjectionStructure (mapProjectionInfoPtr);
		
	return (mapProjectionHandle);
			
}	// end "GetMapProjectionHandle" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetPixelSize
//
//	Software purpose:	This routine returns the the pixel sizes.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/23/2017
//	Revised By:			Larry L. Biehl			Date: 08/23/2017		

void GetPixelSize (
				Handle								mapProjectionHandle,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL)
		{
		*horizontalPixelSizePtr =
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		*verticalPixelSizePtr =
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
		
		}	// end "if (mapProjectionInfoPtr != NULL)"
		
	else	// mapProjectionInfoPtr == NULL
		{
		*horizontalPixelSizePtr = 0; 
		*verticalPixelSizePtr = 0;
		
		}	// end "else mapProjectionInfoPtr == NULL"
	     	
}	// end "GetPixelSize" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetPlanarMapUnitsCode
//
//	Software purpose:	This routine returns the project window flag for the input
//							window information handle.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/06/2000
//	Revised By:			Larry L. Biehl			Date: 03/26/2001		

SInt16 GetPlanarMapUnitsCode (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode);    
	     
	return (-1);
	
}	// end "GetPlanarMapUnitsCode" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetPolynomialOrder
//
//	Software purpose:	This routine returns the order of the polynomial transformation.
//							If the order is 0, then no polynomial transformation is being
//							used.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/01/2007
//	Revised By:			Larry L. Biehl			Date: 03/01/2007		

SInt16 GetPolynomialOrder (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->planarCoordinate.polynomialOrder);    
	     
	return (0);
	
}	// end "GetPolynomialOrder" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetProjectedCSTypeGeoKey
//
//	Software purpose:	This routine returns the epsg code for the Projected CS Type
//							Geo Key.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2012
//	Revised By:			Larry L. Biehl			Date: 04/09/2012		

SInt16 GetProjectedCSTypeGeoKey (
				FileInfoPtr							fileInfoPtr)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																fileInfoPtr->mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->projectedCSTypeGeoKey);   
	     
	return (0);
	
}	// end "GetProjectedCSTypeGeoKey" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetProjectionCode
//
//	Software purpose:	This routine returns the code for the grid coordinate system.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/12/2012
//	Revised By:			Larry L. Biehl			Date: 03/13/2012		

SInt16 GetProjectionCode (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->gridCoordinate.projectionCode);    
	     
	return (-1);
	
}	// end "GetProjectionCode" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetProjectionCodeFromProjectionMenuItem
//
//	Software purpose:	This routine returns the projection code based on the input
//							projection menu item value.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/20/2007
//	Revised By:			Larry L. Biehl			Date: 03/15/2012		

SInt16 GetProjectionCodeFromProjectionMenuItem (
				SInt16								projectionTypeMenuSelection)

{
			// Grid Coordinate Parameters
		
	return (projectionTypeMenuSelection - 1);
	
}	// end "GetProjectionCodeFromProjectionMenuItem" 

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetProjectionCodeFromReferenceSystemCode
//
//	Software purpose:	This routine returns the zone projection code from the input UTM, 
//							State Plane or Transverse Mercator zone.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/31/2007
//	Revised By:			Larry L. Biehl			Date: 05/02/2012

SInt16 GetProjectionCodeFromReferenceSystemCode (
				SInt16								referenceSystemCode,
				SInt16								gridZone)

{ 
	SInt16								projectionCode;
	
	
	projectionCode = 0;
	switch (referenceSystemCode)
		{
		case kGeographicRSCode:	
			projectionCode = -1;
			break;
			
		case kGaussKrugerRSCode:
		case kGDA94RSCode:
		case kGGRS87RSCode:
		case kUTM_NAD27RSCode:
		case kUTM_NAD83RSCode:
		case kUTM_SAD69RSCode:
		case kUTM_WGS72RSCode:
		case kUTM_WGS84RSCode:
		case kUTMRSCode:	
			projectionCode = kTransverseMercatorCode;
			break;
			
		case kStatePlaneNAD27RSCode:
		case kStatePlaneNAD83RSCode:		
			projectionCode = kTransverseMercatorCode;
				
			if (gridZone == 3104)
						// Set zone projection for New York Long Island.	
				projectionCode = kLambertConformalConicCode;
			break;
		
		}	// end "switch (referenceSystemCode)"
		
	return (projectionCode);
		
}	// end "GetProjectionCodeFromReferenceSystemCode"  

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetReferenceSystemCode
//
//	Software purpose:	This routine returns the code for the grid coordinate system.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/13/2012
//	Revised By:			Larry L. Biehl			Date: 03/13/2012		

SInt16 GetReferenceSystemCode (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode);    
	     
	return (-1);
	
}	// end "GetReferenceSystemCode" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetReferenceSystemCodeFromReferenceSystemMenuItem
//
//	Software purpose:	This routine returns the reference system code based on the input
//							reference system menu item value.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/12/2012
//	Revised By:			Larry L. Biehl			Date: 03/14/2012		

SInt16 GetReferenceSystemCodeFromReferenceSystemMenuItem (
				SInt16								referenceSystemMenuSelection)

{ 		
	SInt16								referenceSystemCode;
	
	
			// Grid Coordinate Parameters
	
	if (referenceSystemMenuSelection == kUserDefinedRSMenuItem)
		referenceSystemCode = kUserDefinedRSCode;
		
	else if (referenceSystemMenuSelection == kGeographicRSMenuItem)
		referenceSystemCode = kGeographicRSCode;
	
	else	// referenceSystemMenuSelection > kGeographicRSMenuItem
		referenceSystemCode = referenceSystemMenuSelection - 1;
	
	return (referenceSystemCode);
	
}	// end "GetReferenceSystemCodeFromReferenceSystemMenuItem" 

											

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetScale
//
//	Software purpose:	The purpose of this routine is to compute the scale of the
//							image based on the input parameters. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ComputeAndSetScaleInformation
//
//	Coded By:			Larry L. Biehl			Date: 12/14/2000
//	Revised By:			Larry L. Biehl			Date: 09/21/2001	

double GetScale (
				Handle								windowInfoHandle,
				SInt32								columnInterval,
				double								magnification)

{
	double								factor,
											horizontalPixelSize,
											scale = -1.;
											
	MapProjectionInfoPtr				mapProjectionInfoPtr;
											
	Handle								mapProjectionHandle;
		
	SInt16								filePlanarMapUnitsIndex,
											screenHorizontalResolution;

					
			
	filePlanarMapUnitsIndex = 
					GetFilePlanarMapUnitsCode (windowInfoHandle) - kKilometersCode;
	
	if (filePlanarMapUnitsIndex >= 0)
		{	
				// Get the screen resolution.
				
		#if defined multispec_mac 
			//ScreenRes (&screenHorizontalResolution, &screenVerticalResolution);
			screenHorizontalResolution = LMGetScrHRes ();
			if (screenHorizontalResolution <= 0)
				screenHorizontalResolution = 72;
		#endif	// defined multispec_mac   
		                             
		#if defined multispec_win || defined multispec_wx
			CMImageView* imageViewCPtr = GetWindowPtr (windowInfoHandle);
			screenHorizontalResolution = imageViewCPtr->m_xPixelsPerInch;
		#endif	// defined multispec_win 

		mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);

		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);

		if (mapProjectionInfoPtr != NULL)
			{
			horizontalPixelSize = 
					fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);
	
			factor = gDistanceFileConversionFactors[filePlanarMapUnitsIndex] / 
																	gDistanceFileConversionFactors[9];
		
			scale = horizontalPixelSize * factor * 
								(double)screenHorizontalResolution *
												columnInterval / magnification;	

			}	// end "if (mapProjectionInfoPtr != NULL)"

		}	// end "if (filePlanarMapUnitsIndex >= 0)"

	return (scale);

}	// end "GetScale" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetSpecificUTMRSFromDatumOrEllipsoid
//
//	Software purpose:	This routine determines the specific type of UTM reference system
//							based on the input datum or ellipsoid.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/16/2012
//	Revised By:			Larry L. Biehl			Date: 03/17/2012			

SInt16 GetSpecificUTMRSFromDatumOrEllipsoid (
				SInt16								datumCode,
				SInt16								ellipsoidCode,
				SInt16								assumedUTMCode)

{
	SInt16								referenceSystemCode;
	
	
	referenceSystemCode = assumedUTMCode;
	
	if (datumCode == kGeocentricDatumAustralia1994Code)
		referenceSystemCode = kGDA94RSCode;
	
	else if (datumCode == kNAD27Code)
		referenceSystemCode = kUTM_NAD27RSCode;
		
	else if (datumCode == kNAD83Code)
		referenceSystemCode = kUTM_NAD83RSCode;
		
	else if (datumCode == kSAD69DatumCode)
		referenceSystemCode = kUTM_SAD69RSCode;
		
	else if (datumCode == kWGS84Code)
		referenceSystemCode = kUTM_WGS84RSCode;
		
	else if (ellipsoidCode == kClarke1866EllipsoidCode)
		referenceSystemCode = kUTM_NAD27RSCode;
		
	else if (ellipsoidCode == kGRS80EllipsoidCode)
		referenceSystemCode = kUTM_NAD83RSCode;
		
	else if (ellipsoidCode == kGRS1967ModifiedEllipsoidCode)
		referenceSystemCode = kUTM_SAD69RSCode;
		
	else if (ellipsoidCode == kWGS72EllipsoidCode)
		referenceSystemCode = kUTM_WGS72RSCode;
		
	else if (ellipsoidCode == kWGS84EllipsoidCode)
		referenceSystemCode = kUTM_WGS84RSCode;
		
	return (referenceSystemCode);
	 
}	// end "GetSpecificUTMRSFromDatumOrEllipsoid"

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetSpheroidCode
//
//	Software purpose:	This routine returns the code for the spheroid.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 04/03/2004		

SInt16 GetSpheroidCode (
				Handle								mapProjectionHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
										
	if (mapProjectionInfoPtr != NULL) 
	  return (mapProjectionInfoPtr->geodetic.spheroidCode);    
	     
	return (-1);
	
}	// end "GetSpheroidCode" 

 

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetSpheroidCodeFromMajorMinorAxes
//
//	Software purpose:	This routine returns the code for the spheroid based on the
//							input major and minor axes.
//							Not sure how well this will work. Probably well for sphere
//							but not sure for rest.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/15/2012
//	Revised By:			Larry L. Biehl			Date: 03/24/2012		

SInt16 GetSpheroidCodeFromMajorMinorAxes (
				double								semiMajorAxis,
				double								semiMinorAxis)

{ 
	SInt16								ellipsoidCode;
	
	
	ellipsoidCode = kNoEllipsoidDefinedCode;
	
	if (semiMajorAxis > 0 && semiMinorAxis > 0)
		{												
		if (semiMajorAxis == semiMinorAxis)
			ellipsoidCode = kSphereEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6377563.396) < .00001 && 
								fabs (semiMinorAxis - 6356256.909) < .00001)
			ellipsoidCode = kAiryEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378160) < .00001 && 
								fabs (semiMajorAxis - 6356774.719) < .00001)
			ellipsoidCode = kAustralianEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6377397.155) < .00001 && 
								fabs (semiMajorAxis - 6356079.963) < .00001)
			ellipsoidCode = kBesselEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378206.4) < .00001 && 
								fabs (semiMajorAxis - 6356583.8) < .00001)
			ellipsoidCode = kClarke1866EllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378249.145) < .00001 && 
								fabs (semiMajorAxis - 6356514.87) < .00001)
			ellipsoidCode = kClarke1880EllipsoidCode;
			
		else if (semiMajorAxis == 6377276.3 && semiMinorAxis == 6356075.4)
			ellipsoidCode = kEverestEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378137.0) < .00001 && 
								fabs (semiMinorAxis - 6356752.314140) < .00001)
			ellipsoidCode = kGRS80EllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378388) < .00001 && 
								fabs (semiMinorAxis - 6356911.946) < .00001)
			ellipsoidCode = kInternational1909EllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378245) < .00001 && 
								fabs (semiMinorAxis - 6356863.0) < .00001)
			ellipsoidCode = kKrassovskyEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378160) < .00001 && 
								fabs (semiMinorAxis - 6356774.719) < .00001)
			ellipsoidCode = kGRS1967ModifiedEllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378135) < .00001 && 
								fabs (semiMinorAxis - 6356750.5) < .00001)
			ellipsoidCode = kWGS72EllipsoidCode;
			
		else if (fabs (semiMajorAxis - 6378137) < .00001 && 
								fabs (semiMinorAxis - 6356752.3142) < .00001)
			ellipsoidCode = kWGS84EllipsoidCode;
							
		}	// end "if (semiMajorAxis > 0 && semiMinorAxis > 0)"
			     
	return (ellipsoidCode);
	
}	// end "GetSpheroidCodeFromMajorMinorAxes" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetWindowMapRect
//
//	Software purpose:	This routine converts the input window
//							rectangle coordinates to the correspong map 
//							rectangle coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/03/2001
//	Revised By:			Larry L. Biehl			Date: 06/22/2020

void GetWindowMapRect (
				WindowPtr							windowPtr,
				Handle								windowInfoHandle, 
				DoubleRect*							boundingMapRectPtr, 
				Boolean								winUseOriginFlag,
				SInt16								windowCode,
				UInt16								boundingCode)

{
	LongRect								windowLongRect;
	Rect									windowRect;
												
	
			// Get the image portion of the window that could be visible.
	
	if (windowPtr == NULL)
		windowRect = gViewRect;
		
	else	// windowPtr != NULL
		{
		GetWindowClipRectangle (windowPtr, 
											kImageFrameArea, 
											&windowRect); 

		#if defined multispec_win  
			if (windowCode == kToPrintWindow || windowCode == kToClipboardWindow)
				{
				if (gCDCPointer != NULL)
					{
					RECT clipBox;

					gCDCPointer->GetClipBox (&clipBox);

					windowRect.left = 
							clipBox.left + GetLegendWidthForWindow (windowInfoHandle);      
					windowRect.right = clipBox.right;        
					windowRect.top = clipBox.top + GetTitleHeight (windowInfoHandle);       
					windowRect.bottom = clipBox.bottom; 

					//ClipRect (&windowRect);
					
					}	// end "if (gCDCPointer != NULL)" 
						
				winUseOriginFlag = TRUE;

				}	// end "if (windowCode == kToPrintWindow || ..."
		#endif	// defined multispec_win  

		}	// end "else windowPtr != NULL"
										
	windowLongRect.top = windowRect.top;
	windowLongRect.left = windowRect.left;
	windowLongRect.bottom = windowRect.bottom;
	windowLongRect.right = windowRect.right;
											
	ConvertWinRectToMapRect (windowInfoHandle,
										&windowLongRect, 
										boundingMapRectPtr, 
										winUseOriginFlag,
										FALSE,
										boundingCode);

}	// end "GetWindowMapRect" 
		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeGeodeticModelStructure
//
//	Software purpose:	This routine initializes the geodetic model structure.
//
//	Parameters in:		Pointer to the geodetic model structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2008
//	Revised By:			Larry L. Biehl			Date: 01/06/2012		

void InitializeGeodeticModelStructure (
				GeodeticModelInfoPtr				geodeticModelInfoPtr)

{ 
	geodeticModelInfoPtr->radiusSpheroid = 0; // 6370997.0
	geodeticModelInfoPtr->semiMajorAxis = 0;
	geodeticModelInfoPtr->semiMinorAxis = 0;
	geodeticModelInfoPtr->e = 1.;
	geodeticModelInfoPtr->eSquared = 1.;
	geodeticModelInfoPtr->e4 = 1.;
	geodeticModelInfoPtr->f0 = 0.;
	geodeticModelInfoPtr->mcs = 1.;
	geodeticModelInfoPtr->M0 = 0.;
	geodeticModelInfoPtr->rh = 0.;
	geodeticModelInfoPtr->tcs = 1.;
	geodeticModelInfoPtr->ind = 0;
	geodeticModelInfoPtr->datumCode = 0;
	geodeticModelInfoPtr->spheroidCode = 0;
			
}	// end "InitializeGeodeticModelStructure 
		


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeGridCoordinateSystemStructure
//
//	Software purpose:	This routine initializes the grid coordinate system structure.
//
//	Parameters in:		Pointer to the geodetic model structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/12/2008
//	Revised By:			Larry L. Biehl			Date: 04/14/2012		

void InitializeGridCoordinateSystemStructure (
				GridCoordinateSystemInfoPtr	gridCoordinateSystemInfoPtr)

{  	
	gridCoordinateSystemInfoPtr->epsgName[0] = 0;
	gridCoordinateSystemInfoPtr->epsgName[1] = 0;
	gridCoordinateSystemInfoPtr->datumName[0] = 0;
	gridCoordinateSystemInfoPtr->datumName[1] = 0;
	gridCoordinateSystemInfoPtr->ellipsoidName[0] = 0;
	gridCoordinateSystemInfoPtr->ellipsoidName[1] = 0;
	gridCoordinateSystemInfoPtr->gridZone = 0;	
	gridCoordinateSystemInfoPtr->standardParallel1 = 0.;
	gridCoordinateSystemInfoPtr->standardParallel2 = 0.;
	gridCoordinateSystemInfoPtr->falseEasting = 0.;
	gridCoordinateSystemInfoPtr->falseNorthing = 0.;
	gridCoordinateSystemInfoPtr->falseOriginEasting = 0.;
	gridCoordinateSystemInfoPtr->falseOriginNorthing = 0.;
	gridCoordinateSystemInfoPtr->falseOriginLatitude = 0.;
	gridCoordinateSystemInfoPtr->falseOriginLongitude = 0.;
	gridCoordinateSystemInfoPtr->latitudeOrigin = 0.;
	gridCoordinateSystemInfoPtr->longitudeCentralMeridian = 0.;
	gridCoordinateSystemInfoPtr->scaleFactorOfCentralMeridian = 1.;
	gridCoordinateSystemInfoPtr->projAzimuthAngle = 0.;
	gridCoordinateSystemInfoPtr->referenceSystemCode = 0;
	gridCoordinateSystemInfoPtr->projectionCode = 0;
			
}	// end "InitializeGridCoordinateSystemStructure" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeMapProjectionStructure
//
//	Software purpose:	This routine initializes the Map Projection structure.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2005
//	Revised By:			Larry L. Biehl			Date: 12/12/2008		

void InitializeMapProjectionStructure (
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->coefficientsHandle = NULL;
		mapProjectionInfoPtr->numberCoefficients = 0;
		mapProjectionInfoPtr->projectedCSTypeGeoKey = 0;
		
		InitializeGeodeticModelStructure (&mapProjectionInfoPtr->geodetic);
		
		InitializeGridCoordinateSystemStructure (&mapProjectionInfoPtr->gridCoordinate);
		
		InitializePlanarCoordinateInfo (&mapProjectionInfoPtr->planarCoordinate);
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "InitializeMapProjectionStructure"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializePlanarCoordinateInfo
//
//	Software purpose:	This routine initializes the PlanarCoordinateSystemInfo structure.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/13/2003
//	Revised By:			Larry L. Biehl			Date: 04/29/2012		

void InitializePlanarCoordinateInfo (
				PlanarCoordinateSystemInfo*	planarCoordinatePtr)

{ 
	planarCoordinatePtr->easting1CoefficientsPtr = NULL;
	planarCoordinatePtr->northing1CoefficientsPtr = NULL;
	planarCoordinatePtr->easting2CoefficientsPtr = NULL;
	planarCoordinatePtr->northing2CoefficientsPtr = NULL;
	planarCoordinatePtr->metersToNativeFactor = 1.;
	planarCoordinatePtr->horizontalPixelSize = 0.;
	planarCoordinatePtr->mapOrientationAngle = 0.;
	planarCoordinatePtr->pixelArea = 1.;
	planarCoordinatePtr->verticalPixelSize = 0.;
	planarCoordinatePtr->xMapCoordinate11 = 1.;
	planarCoordinatePtr->yMapCoordinate11 = 1.;
	planarCoordinatePtr->xMapOrientationOrigin = 1.;
	planarCoordinatePtr->yMapOrientationOrigin = 1.;
	planarCoordinatePtr->areaUnitsCode = 0;
	planarCoordinatePtr->mapUnitsCode = kUnknownCode;
	planarCoordinatePtr->zMapUnitsCode = kUnknownCode;
	planarCoordinatePtr->polynomialOrder = 0;
			
}	// end "InitializePlanarCoordinateInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void LoadDMSLatLongStrings
//
//	Software purpose:			This routine converts the input decimal values for 
//									latitude and longitude to strings with degree minutes and
//									seconds along with the identifier for the earth location
//									(E-W and N-S).
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/03/2004
//	Revised By:			Larry L. Biehl			Date: 02/28/2007			

SInt16 LoadDMSLatLongStrings (
				double								latitudeValue,
				double								longitudeValue,
				char*									latitudeStringPtr,
				char*									longitudeStringPtr)

{
	char									latitudeDirection[2] = {'N', 'S'},
											longitudeDirection[2] = {'E', 'W'};
											
	double								seconds;
	
	SInt16								degrees,
											minutes,
											numberChars,
											signIndex;
											
	Boolean								rangeOKFlag;
						
						
	signIndex = ConvertDecimalDegreesToDMS (latitudeValue,
															&degrees,
															&minutes,
															&seconds,
															&rangeOKFlag);
						
	if (rangeOKFlag)
		numberChars = sprintf (latitudeStringPtr, 
											"%3d %2d %5.2f%c",
											degrees,  
											minutes,
											seconds,
											latitudeDirection[signIndex]);
											
	else	// !rangeOKFlag
		numberChars = sprintf (latitudeStringPtr, 
											"%3s %2s %5s%c",
											"***",  
											"**",
											"**.**",
											latitudeDirection[signIndex]);
						
	signIndex = ConvertDecimalDegreesToDMS (longitudeValue,
															&degrees,
															&minutes,
															&seconds,
															&rangeOKFlag);
				
	if (rangeOKFlag)		
		numberChars = sprintf (longitudeStringPtr, 
											"%3d %2d %5.2f%c",
											degrees,  
											minutes,
											seconds,
											longitudeDirection[signIndex]);
											
	else	// !rangeOKFlag
		numberChars = sprintf (longitudeStringPtr, 
											"%3s %2s %5s%c",
											"***",  
											"**",
											"**.**",
											longitudeDirection[signIndex]);
											
	return (numberChars);
			
}	// end "LoadDMSLatLongStrings" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void LoadPlanarCoordinates
//
//	Software purpose:			The purpose of this routine is to load the planar 
//									coordinates structure with the input transformation values.
//									These values are based on the order that they are in the
//									world file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/01/2012
//	Revised By:			Larry L. Biehl			Date: 10/04/2019

void LoadPlanarCoordinates (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								dataValue1,
				double								dataValue2,
				double								dataValue3,
				double								dataValue4,
				double								dataValue5,
				double								dataValue6)

{
			// Load the map transform information
			
	mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 0;
	if (dataValue2 != 0 && dataValue1 != 0)
				// dataValue1 == 0 implies 0 width pixel
		mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 
										-atan2 (dataValue2, dataValue1);
	
	mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
																			dataValue1;
	mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
																			-dataValue4;
					
	mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = dataValue5;
	mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = dataValue6;
				
	if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle != 0)
		{ 
		mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize /=
						cos (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
						
		mapProjectionInfoPtr->planarCoordinate.verticalPixelSize /=
						cos (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle);
		
		}	// end "else ...->planarCoordinate.mapOrientationAngle != 0"
	
	mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
								mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
	mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
								mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
			
}	// end "LoadPlanarCoordinates"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void LoadSpheroidInformation
//
//	Software purpose:			This routine sets up parameters to be used to convert from
//									map units to lat-long and vice versa based on the geodetic
//									parameters of the ellipsoid.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//												
// Called By:			ReadNDFHeader in SNDFFormat.cpp
//							FinishMapInformationSetUp in SOpenImage.cpp
//							LoadHDF4DataSetInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/02/2004
//	Revised By:			Larry L. Biehl			Date: 04/26/2012			

void LoadSpheroidInformation (
				Handle								mapProjectionHandle)

{
	double								a,
											b,
											centerLatitude,
											con1,
											cosCon1,
											e,
											e2,
											radiusSpheroid,
											phi0,
											semiMajorAxis,
											semiMinorAxis,
											sinCon1;
											
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	SInt16								projectionCode;


			// Get pointer to the map projection structure.							
			
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);

	if (mapProjectionInfoPtr == NULL)
																										return;
											
	if (GetEllipsoidParameters (mapProjectionInfoPtr->geodetic.spheroidCode,
												&radiusSpheroid,
												&semiMajorAxis,
												&semiMinorAxis))
		{			
		if (mapProjectionInfoPtr->geodetic.spheroidCode == kSphereEllipsoidCode)
			{
			if (mapProjectionInfoPtr->geodetic.radiusSpheroid == 0)
				mapProjectionInfoPtr->geodetic.radiusSpheroid = 
												mapProjectionInfoPtr->geodetic.semiMajorAxis;
												
					// Give a default radius for the spheroid if needed.
					// Note that the radius at the equator is: 6378137
					
			if (mapProjectionInfoPtr->geodetic.radiusSpheroid == 0)
				{
				mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
				mapProjectionInfoPtr->geodetic.semiMajorAxis = semiMajorAxis;
				mapProjectionInfoPtr->geodetic.semiMinorAxis = semiMinorAxis;
				
				}	// end "if (...->geodetic.radiusSpheroid == 0)"
			
					// Make sure that the semiMinorAxis is set correctly since it
					// is used to compute eSquared.
			
			mapProjectionInfoPtr->geodetic.semiMinorAxis = 
												mapProjectionInfoPtr->geodetic.semiMajorAxis;
				
					// Check if datum was set for sphere.
					
			if (mapProjectionInfoPtr->geodetic.datumCode == kNoDatumDefinedCode)
				mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
				
			}	// end "if (...->geodetic.spheroidCode == kSphereEllipsoidCode"
			
		else	// ...->geodetic.spheroidCode != kSphereEllipsoidCode
			{
			mapProjectionInfoPtr->geodetic.semiMajorAxis = semiMajorAxis;
			mapProjectionInfoPtr->geodetic.semiMinorAxis = semiMinorAxis;
			
			}	// end "else ...->geodetic.spheroidCode != kSphereEllipsoidCode"
		
		}	// end "if (GetEllipsoidParameters (..."
		
	else	// !GetEllipsoidParameters (mapProjectionInfoPtr->geodetic.spheroidCode
		{
		if (mapProjectionInfoPtr->geodetic.semiMajorAxis > 0 &&
					mapProjectionInfoPtr->geodetic.semiMinorAxis > 0 &&
						mapProjectionInfoPtr->geodetic.semiMajorAxis == 
								mapProjectionInfoPtr->geodetic.semiMinorAxis)
			{
			mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
			mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidCode;
			mapProjectionInfoPtr->geodetic.radiusSpheroid = 
											mapProjectionInfoPtr->geodetic.semiMajorAxis;
		
			}	// end "!GetEllipsoidParameters (..."
			
		}	// end "else !GetEllipsoidParameters (...->geodetic.spheroidCode ..."
		
			// Get some default parameters for those projection based on zones.
			
	SetProjectionParametersFromZone (
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode,
						mapProjectionInfoPtr->geodetic.datumCode,
						&mapProjectionInfoPtr->gridCoordinate.gridZone,
						mapProjectionInfoPtr->gridCoordinate.epsgName,
						&projectionCode,
						&mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian,
						&mapProjectionInfoPtr->gridCoordinate.latitudeOrigin,
						&mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian,
						&mapProjectionInfoPtr->gridCoordinate.falseEasting,
						&mapProjectionInfoPtr->gridCoordinate.falseNorthing,
						&mapProjectionInfoPtr->gridCoordinate.standardParallel1,
						&mapProjectionInfoPtr->gridCoordinate.standardParallel2,
						&mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude,
						&mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude,
						&mapProjectionInfoPtr->gridCoordinate.falseOriginEasting,
						&mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing);
						
	if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kOrthographicCode)
		{
		if (mapProjectionInfoPtr->geodetic.radiusSpheroid == 0)
			mapProjectionInfoPtr->geodetic.radiusSpheroid = 
														mapProjectionInfoPtr->geodetic.semiMajorAxis;
			
		}	// end "else if (...->gridCoordinate.projectionCode == kOrthograhicCode)"
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kEquirectangularCode)
		{			
				// If a non spherical ellipsoid is specified for this projection, calculate
				// an approximation to the spheroid as given by those running the CAI program
				
		if (mapProjectionInfoPtr->geodetic.semiMajorAxis != 
													mapProjectionInfoPtr->geodetic.semiMinorAxis)
			mapProjectionInfoPtr->geodetic.radiusSpheroid = 
					(2*mapProjectionInfoPtr->geodetic.semiMajorAxis + 
													mapProjectionInfoPtr->geodetic.semiMinorAxis)/3.;
		
			
		}	// end "else if (...->gridCoordinate.projectionCode == kEquirectangularCode)"
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kMercatorCode)
		{
				// This will handle case where scaleFactor is not loaded because of
				// a missing GeoTIFF tag. Came across this on 8/23/2006. Do not know yet
				// if this is correct but seems to make ArcGIS and Imagine happy.
				
		if (mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian == 0)
			mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian = 1;
			
				// For now the converstion from Mercator units to lat-long and vice versa
				// are based on a sphere. The following will be an approximation to use
				// for the radius of the sphere based on the major and minor axes values.
				
		if (mapProjectionInfoPtr->geodetic.semiMajorAxis != 
														mapProjectionInfoPtr->geodetic.semiMinorAxis)
			mapProjectionInfoPtr->geodetic.radiusSpheroid = 
					(2*mapProjectionInfoPtr->geodetic.semiMajorAxis + 
													mapProjectionInfoPtr->geodetic.semiMinorAxis)/3.;
		
			
		}	// end "else if (...->gridCoordinate.projectionCode == kMercatorCode)"
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode ==
																		kLambertAzimuthalEqualAreaCode)
		{
				// Handle case where this is not an earth sphere. The conversion code for
				// Lambert Azimuthal Equal Area is only for spheres at the current time.
				// Note that this will cause errors in calculation but does allow some work
				// to be done. Larry Biehl: 3/22/2007.
				
		if (mapProjectionInfoPtr->geodetic.semiMajorAxis != 
														mapProjectionInfoPtr->geodetic.semiMinorAxis)
			mapProjectionInfoPtr->geodetic.radiusSpheroid = 
					(mapProjectionInfoPtr->geodetic.semiMajorAxis + 
													mapProjectionInfoPtr->geodetic.semiMinorAxis)/2.;
			
		}	// end "else if (... .projectionCode == kLambertAzimuthalEqualAreaCode)"
		
			// Get the square of the eccentricity of the ellipsoid.
			
	a = mapProjectionInfoPtr->geodetic.semiMajorAxis;
	b = mapProjectionInfoPtr->geodetic.semiMinorAxis;
	
	if (a > 0)
		mapProjectionInfoPtr->geodetic.eSquared = 1.0 - (b*b)/(a*a);
	
	else	// a <= 0. Something not set correctly.
		mapProjectionInfoPtr->geodetic.eSquared = 1.;
		
			// Get the eccentricity of the ellipsoid. Used by Polar Stereographic projection
			
	mapProjectionInfoPtr->geodetic.e = sqrt (mapProjectionInfoPtr->geodetic.eSquared);
		
			// Get a contant used by Polar Stereographic projection
			
	mapProjectionInfoPtr->geodetic.e4 = e4fn (mapProjectionInfoPtr->geodetic.e);
	
			// Get the distance from the equator to the latitude of the origin.
	
	e2 = mapProjectionInfoPtr->geodetic.eSquared;
	phi0 = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;		
	mapProjectionInfoPtr->geodetic.M0 = a*(
				(1 - e2*(.25 + e2*((double)3/64 + e2*(double)5/256))) * phi0 -
					e2*(.375 + e2*((double)3/32 + e2*(double)45/1024)) * sin (2*phi0) +
						e2*(e2*((double)15/256 + e2*(double)45/1024)) * sin (4*phi0) -
							e2*e2*e2*(double)35/3072 * sin (6*phi0));
							
			// Get some other contants used by projection algorithms
			
	centerLatitude = phi0;

	mapProjectionInfoPtr->geodetic.fac = 1.0;
	if (centerLatitude < 0)
		mapProjectionInfoPtr->geodetic.fac = -1.0;
	
	e = mapProjectionInfoPtr->geodetic.e;
	mapProjectionInfoPtr->geodetic.ind = 0;
	if (fabs (fabs (centerLatitude) - kHALF_PI) > kEPSLN)
		{
		mapProjectionInfoPtr->geodetic.ind = 1;
		con1 = mapProjectionInfoPtr->geodetic.fac * centerLatitude; 
		cosCon1 = cos (con1);
		sinCon1 = sin (con1);
		mapProjectionInfoPtr->geodetic.mcs = msfnz (e, sinCon1, cosCon1);
		mapProjectionInfoPtr->geodetic.tcs = tsfnz (e, con1, sinCon1);
		
		}	// end "if (fabs (fabs (centerLatitude) - kHALF_PI) > kEPSLN)"
		
	if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kLambertConformalConicCode)
		{
		double	cosLat1,
					cosLat2,
					f0,
					lat1,
					lat2,
					ms1,
					ms2,
					ns,
					sinCenterLat,
					sinLat1,
					sinLat2,
					ts0,
					ts1,
					ts2;
						
		lat1 = mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
		lat2 = mapProjectionInfoPtr->gridCoordinate.standardParallel2 * kDegreesToRadians;
		centerLatitude = 
				mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude * kDegreesToRadians;
		
				// Cannot compute if standard parallels are equal on opposite sides of
				// the equator. Set paramaters so that computation will be made but 
				// obviously wrong.
		
		if (fabs (lat1+lat2) < kEPSLN)
			{
			mapProjectionInfoPtr->geodetic.ns = 0;
			mapProjectionInfoPtr->geodetic.rh = 0;
			mapProjectionInfoPtr->geodetic.f0 = 0;
			
			}	// end "if (fabs (lat1+lat2) < kEPSLN)"
			
		else	// fabs (lat1+lat2) >= kEPSLN
			{								
			cosLat1 = cos (lat1);
			sinLat1 = sin (lat1);
			ms1 = msfnz (e, sinLat1, cosLat1);
			ts1 = tsfnz (e, lat1, sinLat1);
			
			cosLat2 = cos (lat2);
			sinLat2 = sin (lat2);
			ms2 = msfnz (e, sinLat2, cosLat2);
			ts2 = tsfnz (e, lat2, sinLat2);
			
			sinCenterLat = sin (centerLatitude);
			ts0 = tsfnz (e, centerLatitude, sinCenterLat);

			if (fabs (lat1 - lat2) > kEPSLN)
				 ns = log (ms1/ms2)/ log (ts1/ts2);
			else
				 ns = sinLat1;
				 
			f0 = ms1 / (ns * pow (ts1,ns));
			
			mapProjectionInfoPtr->geodetic.ns = ns;
			mapProjectionInfoPtr->geodetic.rh = a * f0 * pow (ts0,ns);
			mapProjectionInfoPtr->geodetic.f0 = f0;
				
			}	// end "else fabs (lat1+lat2) >= kEPSLN"
			
		}	// end "else if (...->gridCoordinate.code == kLambertConformalConicCode)"
		
}	// end "LoadSpheroidInformation" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean PointInRectangle
//
//	Software purpose:	The purpose of this routine is to determine if the input
//							arc view double point is within the input rectangle.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/03/2001
//	Revised By:			Larry L. Biehl			Date: 01/03/2001			

Boolean PointInRectangle (
				ArcViewDoublePoint*				arcViewDoublePointPtr,
				DoubleRect*							boundingRectanglePtr)

{	
	if (arcViewDoublePointPtr->y > boundingRectanglePtr->bottom ||
			arcViewDoublePointPtr->y < boundingRectanglePtr->top ||
				arcViewDoublePointPtr->x > boundingRectanglePtr->right ||
					arcViewDoublePointPtr->x < boundingRectanglePtr->left)
		return (FALSE);
		
	return (TRUE);

}	// end "PointInRectangle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetAlbersEqualAreaParameters
//
//	Software purpose:	This routine sets the parameters for the Cylindrical Equal
//							Area projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/06/2007
//	Revised By:			Larry L. Biehl			Date: 04/06/2007		

void SetAlbersEqualAreaParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								standardParallel1,
				double								standardParallel2,
				double								centralMeridian,
				double								latitudeOrigin,
				double								falseEasting,
				double								falseNorthing)

{ 
	if (mapProjectionInfoPtr != NULL)
		{
		mapProjectionInfoPtr->gridCoordinate.standardParallel1 = standardParallel1;
		mapProjectionInfoPtr->gridCoordinate.standardParallel2 = standardParallel2;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = centralMeridian;
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = latitudeOrigin;
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetAlbersEqualAreaParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetBoundingMapRectangle
//
//	Software purpose:	The purpose of this routine is to set the bounding map
//							rectangle for the input window based on the planar map
//							parameters and the number of lines and columns in the window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//							UpdateLayerInfoStructure in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/2000
//	Revised By:			Larry L. Biehl			Date: 12/22/2015	

void SetBoundingMapRectangle (
				Handle								windowInfoHandle)

{
	DoubleRect							boundingLatLongRectangle;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	double								factor;
	
	Handle								mapProjectionHandle;
	
	SInt16								gridCode;
	
	
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);

	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);

	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
												
	if (windowInfoPtr != NULL && mapProjectionInfoPtr != NULL)
		{
		GetCoefficientsVectorPointers (mapProjectionInfoPtr);
	
		SetBoundingMapRectangle (&mapProjectionInfoPtr->planarCoordinate,
												windowInfoPtr->maxNumberColumns,
												windowInfoPtr->maxNumberLines,
												&windowInfoPtr->boundingMapRectangle,
												kAllCorners);		

				// Also set the bounding units in latitude - longitude degrees if
				// possible. Note that all 4 corners will need to be check to be
				// sure to get the overall bounding lat-long rectangle. Depending on
				// the projection the side of the window may not have the same latitude
				// or longitude along the entire side.
				
		gridCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
														
				// Make sure the units for the bounding map rectangle is in meters.
				// The algorithms for converting to lat-long expect units of meters.
		
		factor = 1;
		if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode > kMetersCode)
			{
			SInt16 filePlanarMapUnitsIndex = 
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode - kKilometersCode;
			factor = gDistanceFileConversionFactors[filePlanarMapUnitsIndex] / 
																gDistanceFileConversionFactors[1];
									
			}	// end "if (...->planarCoordinate.mapUnitsCode > kMetersCode)"
		
				// This represents lower-left and upper-right points
		windowInfoPtr->boundingLatLongRectangle.left =
											windowInfoPtr->boundingMapRectangle.left * factor;
		windowInfoPtr->boundingLatLongRectangle.top =
											windowInfoPtr->boundingMapRectangle.bottom * factor;
		windowInfoPtr->boundingLatLongRectangle.right =
											windowInfoPtr->boundingMapRectangle.right * factor;
		windowInfoPtr->boundingLatLongRectangle.bottom =
											windowInfoPtr->boundingMapRectangle.top * factor;
		ConvertMapRectToLatLongRect (mapProjectionInfoPtr,
												gridCode,
												&windowInfoPtr->boundingLatLongRectangle);
														
				// This represents upper-left and lower-right points
		boundingLatLongRectangle.left =
											windowInfoPtr->boundingMapRectangle.left * factor;
		boundingLatLongRectangle.top =
											windowInfoPtr->boundingMapRectangle.bottom * factor;
		boundingLatLongRectangle.right =
											windowInfoPtr->boundingMapRectangle.right * factor;
		boundingLatLongRectangle.bottom =
											windowInfoPtr->boundingMapRectangle.top * factor;
		ConvertMapRectToLatLongRect (mapProjectionInfoPtr,
												gridCode,
												&boundingLatLongRectangle);
												
		windowInfoPtr->boundingLatLongRectangle.left =
												MIN (windowInfoPtr->boundingLatLongRectangle.left,
														boundingLatLongRectangle.left);
												
		windowInfoPtr->boundingLatLongRectangle.right =
												MAX (windowInfoPtr->boundingLatLongRectangle.right,
														boundingLatLongRectangle.right);
												
		windowInfoPtr->boundingLatLongRectangle.top =
												MIN (windowInfoPtr->boundingLatLongRectangle.top,
														boundingLatLongRectangle.bottom);
												
		windowInfoPtr->boundingLatLongRectangle.bottom =
												MAX (windowInfoPtr->boundingLatLongRectangle.bottom,
														boundingLatLongRectangle.top);
		
				// Handle situations where the left and right bounding longitude values
				// may be in reverse order.  This can happen with the sinusoidal
				// projection.
				
		if (windowInfoPtr->boundingLatLongRectangle.left > 
													windowInfoPtr->boundingLatLongRectangle.right)
			{
			double savedValue = windowInfoPtr->boundingLatLongRectangle.left;
			windowInfoPtr->boundingLatLongRectangle.left = 
													windowInfoPtr->boundingLatLongRectangle.right;
			windowInfoPtr->boundingLatLongRectangle.right = savedValue;
			
			}	// end "if (windowInfoPtr->boundingLatLongRectangle.left > ..."
												
				// Do a specific check here for the case when all of the lat-long values
				// are -1000.  This implies that the map coordinates map to lat
				// long values that are outside of the globe which can happen with
				// the orthographic projectiion.  For this case, determine the extent of
				// the lat-long based on the center latitude and longitude.
				
		if (gridCode == kOrthographicCode &&
				windowInfoPtr->boundingLatLongRectangle.left == -1000 &&
				windowInfoPtr->boundingLatLongRectangle.top == -1000 &&
				windowInfoPtr->boundingLatLongRectangle.right == -1000 &&
				windowInfoPtr->boundingLatLongRectangle.bottom == -1000)
			{
			windowInfoPtr->boundingLatLongRectangle.left = 
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian - 90;
			windowInfoPtr->boundingLatLongRectangle.right = 
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian + 90;
					
			windowInfoPtr->boundingLatLongRectangle.top = 
					mapProjectionInfoPtr->gridCoordinate.latitudeOrigin - 90;
			windowInfoPtr->boundingLatLongRectangle.bottom = 
					mapProjectionInfoPtr->gridCoordinate.latitudeOrigin + 90;
			
			}	// end "gridCode == kOrthographicCode && ..."
													
		CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
		
		}	// end "if (windowInfoPtr != NULL && mapProjectionInfoPtr != NULL)"
		
}	// end "SetBoundingMapRectangle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetBoundingMapRectangle
//
//	Software purpose:	The purpose of this routine is to set the bounding map
//							rectangle for the input window based on the planar map
//							parameters and the number of lines and columns in the window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			SetUpImageOverlayInformation om SImageOverlays.cpp
//							SetBoundingMapRectangle
//
//	Coded By:			Larry L. Biehl			Date: 12/28/2000
//	Revised By:			Larry L. Biehl			Date: 03/23/2007	

void SetBoundingMapRectangle (
				PlanarCoordinateSystemInfo*	planarCoordinatePtr,
				SInt32								numberColumns,
				SInt32								numberLines,
				DoubleRect*							boundingMapRectanglePtr,
				UInt16								boundingCode)

{
	DoubleRect							boundingMapRectangle,
											lineColumnRect;
	
												
	if (planarCoordinatePtr != NULL && boundingMapRectanglePtr != NULL)
		{
		if (planarCoordinatePtr->polynomialOrder <= 0)
			{
			boundingMapRectangle.top = planarCoordinatePtr->yMapCoordinate11 +
													0.5 * planarCoordinatePtr->verticalPixelSize;
						
			boundingMapRectangle.left = planarCoordinatePtr->xMapCoordinate11 -
													0.5 * planarCoordinatePtr->horizontalPixelSize;
		
			boundingMapRectangle.bottom = boundingMapRectangle.top - 
										numberLines * planarCoordinatePtr->verticalPixelSize;
		
			boundingMapRectangle.right = boundingMapRectangle.left + 
										numberColumns * planarCoordinatePtr->horizontalPixelSize;
										
					// Now set up in output bounding map rectangle.
			
			if (planarCoordinatePtr->mapOrientationAngle != 0)
				GetBoundingOrientationAngleRect (&boundingMapRectangle,
															planarCoordinatePtr,
															boundingCode);
						
			}	// end "if (planarCoordinatePtr->polynomialOrder <= 0)"
			
		else	// planarCoordinatePtr->polynomialOrder > 0
			{
			lineColumnRect.left = 1;
			lineColumnRect.top = 1;
			lineColumnRect.right = numberColumns;
			lineColumnRect.top = numberLines;
						
			ConvertLCRectToMapRect_TransformCoordinate (
								planarCoordinatePtr,
								&lineColumnRect, 
								&boundingMapRectangle,
								boundingCode);
								
			}	// end "else planarCoordinatePtr->polynomialOrder > 0"
															
		if (boundingCode == kAllCorners)
			{
					// This is for window bounding rectangle.  Make sure that the upper
					// left point is the lowest value and the lower right point is the
					// upper value.
		
			boundingMapRectanglePtr->top = MIN (
									boundingMapRectangle.top, boundingMapRectangle.bottom);
		
			boundingMapRectanglePtr->bottom = MAX (
									boundingMapRectangle.top, boundingMapRectangle.bottom);
		
			boundingMapRectanglePtr->left = MIN (
									boundingMapRectangle.left, boundingMapRectangle.right);
		
			boundingMapRectanglePtr->right = MAX (
									boundingMapRectangle.left, boundingMapRectangle.right);
			
			}	// and "if (boundingCode == kAllCorners)"
			
		else	// boundingCode == kUpperLeftLowerRightCorners
			*boundingMapRectanglePtr = boundingMapRectangle;
									
		}	// end "if (planarCoordinatePtr != NULL &&..."
		
}	// end "SetBoundingMapRectangle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetCoefficientsHandle
//
//	Software purpose:	This routine sets the code for the grid coordinate system.
//
//	Parameters in:				
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/02/2010
//	Revised By:			Larry L. Biehl			Date: 08/02/2010		

void SetCoefficientsHandle (
				Handle								mapProjectionHandle,
				Handle								coefficientsHandle)

{ 
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
	
	if (mapProjectionInfoPtr != NULL) 
		mapProjectionInfoPtr->coefficientsHandle = coefficientsHandle;
	
}	// end "SetCoefficientsHandle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetCoordinateViewLocationParameters
//
//	Software purpose:	The purpose of this routine is to set the location 
//							parameters for the cursor and selection coordinates.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			CoordinateDialog in SMapCoordinates.cpp
//							
//
//	Coded By:			Larry L. Biehl			Date: 11/08/2000
//	Revised By:			Larry L. Biehl			Date: 03/07/2020

void SetCoordinateViewLocationParameters (
				Handle								windowInfoHandle)

{
	double								conversion,
											dpiScale = 1,
											factor = 1.;
									
	MapProjectionInfoPtr 			mapProjectionInfoPtr;
	
	ControlHandle						areaUnitsControl;
							
	Handle								mapProjectionHandle;
	
	UInt32								maxNumberColumns,
											maxNumberLines,
											maxNumberPixels,
											number;
	
	SInt16								areaUnitsCode,
											decimalDigits,
											displayAreaUnitsIndex,
											displayPlanarMapUnitsIndex,
											filePlanarMapUnitsIndex,
											projectionCode,
											maxNumberCharacters = 6,
											maxNumberSelectionCharacters = 6,
											numberChars,
											viewUnits,
											width;
											
	Boolean								convertMapProjectionToLatLongFlag;
	
	#if defined multispec_wx
		CMCoordinateBar*					coordinatesBar;
		CMImageView*						imageViewCPtr;
		CMImageFrame*						imageFrameCPtr;
		int									linWidth,
												linHeight;
	#endif	// defined multispec_wx
	
	#if defined multispec_win
		CMImageView*						imageViewCPtr;
		CMImageFrame*						imageFrameCPtr;
	#endif	// defined multispec_win


			// Initialize some local variables.
			
	maxNumberColumns = GetMaxNumberColumns (windowInfoHandle);
	maxNumberLines = GetMaxNumberLines (windowInfoHandle);
	maxNumberPixels = maxNumberColumns * maxNumberLines;
			
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	
			// Set the coordinate item location parameters
			//		First find the maximum number of characters that need to be
			//		allowed for and then save that parameter and the length of the
			//		space in the coordinate view for the cursor coordinates.
			
			// Get the factor to use to convert from the units defined in the image to
			// the units requested in the coordinate view.
	
	viewUnits = GetCoordinateViewUnits (windowInfoHandle);
			
	filePlanarMapUnitsIndex = 
					GetFilePlanarMapUnitsCode (windowInfoHandle) - kKilometersCode;
					
	displayPlanarMapUnitsIndex = viewUnits - kKilometersUnitsMenuItem;
				
			// Now determine if lat-long units are being used and if so if they
			// are being computed from a map projection.
	
	projectionCode = GetProjectionCode (mapProjectionHandle);
	
	convertMapProjectionToLatLongFlag = FALSE;
	if (viewUnits == kDecimalLatLongUnitsMenuItem || 
														viewUnits == kDMSLatLongUnitsMenuItem)
		{			
		if (projectionCode == kTransverseMercatorCode ||
					projectionCode == kLambertConformalConicCode ||
						projectionCode == kLambertAzimuthalEqualAreaCode ||
							projectionCode == kSinusoidalCode ||
								projectionCode == kKrovakCode)
			{
			convertMapProjectionToLatLongFlag = TRUE;
			displayPlanarMapUnitsIndex = kMetersUnitsMenuItem - kKilometersUnitsMenuItem;
			
			}	// end "if (projectionCode == kTransverseMercatorCode || ..."

		}	// end "if (viewUnits == kDecimalLatLongUnitsMenuItem || ..."

	if (filePlanarMapUnitsIndex >= 0 && displayPlanarMapUnitsIndex >= 0)
		factor = gDistanceFileConversionFactors[filePlanarMapUnitsIndex] / 
									gDistanceFileConversionFactors[displayPlanarMapUnitsIndex];
	
	switch (viewUnits)
		{
		case kLineColumnUnitsMenuItem:
			number = MAX (maxNumberColumns, maxNumberLines);
			
			maxNumberCharacters = sprintf ((char*)gTextString, 
														"%u",
														(unsigned int)number);
			maxNumberCharacters = InsertCommasInNumberString (
														(char*)gTextString, 
														maxNumberCharacters, 
														-1,
														maxNumberCharacters);
														
			maxNumberSelectionCharacters = 2*maxNumberCharacters + 1;
			break;
			
		case kUnknownUnitsMenuItem:
		case kDecimalLatLongUnitsMenuItem:
		case kDMSLatLongUnitsMenuItem:
		case kKilometersUnitsMenuItem:
		case kMetersUnitsMenuItem:
		case kCentimetersUnitsMenuItem:
		case kMillimetersUnitsMenuItem:
		case kMicrometersUnitsMenuItem:
		case kMilesUnitsMenuItem:
		case kYardsUnitsMenuItem:
		case kUSSurveyFeetUnitsMenuItem:
		case kFeetUnitsMenuItem:
		case kInchesUnitsMenuItem:
			DoubleRect					coordinateRectangle;
			
			double 						horizontalPixelSize,
											verticalPixelSize;
	
			mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle);
		
			horizontalPixelSize = 
								mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
			verticalPixelSize = 
								mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
				
			coordinateRectangle.left = 
					factor * mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
			coordinateRectangle.top = 
					factor * mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
	
			coordinateRectangle.right = coordinateRectangle.left +
									factor * (maxNumberColumns - 1) * horizontalPixelSize;
			coordinateRectangle.bottom = coordinateRectangle.top -
									factor * (maxNumberLines - 1) * verticalPixelSize;
						
			if (convertMapProjectionToLatLongFlag)
				{
				ConvertMapRectToLatLongRect (mapProjectionInfoPtr, 
														projectionCode,
														&coordinateRectangle);
														
				}	// end "if (convertMapProjectionToLatLongFlag)"
			
			if (viewUnits == kDMSLatLongUnitsMenuItem)
				{
				maxNumberCharacters = 13;
				
				}	// end "if (kDMSLatLongUnitsMenuItem)"
				
			else	// viewUnits != kDMSLatLongUnitsMenuItem
				{	
				decimalDigits = GetCoordinateViewUnitDecimalPlaces (windowInfoHandle);
				
				if (fabs (coordinateRectangle.top) < kMaxCoordinateValue)
					{
					maxNumberCharacters = sprintf ((char*)gTextString, 
															"%.*f",
															decimalDigits, 
															coordinateRectangle.top);
					maxNumberCharacters = InsertCommasInNumberString (
															(char*)gTextString, 
															maxNumberCharacters, 
															2,
															maxNumberCharacters);
						
					}	// end "if (fabs (coordinateRectangle.top) < kMaxCoordinateValue)"
					
				else	// fabs (coordinateRectangle.top) >= kMaxCoordinateValue
					maxNumberCharacters = 60;
										
				if (fabs (coordinateRectangle.bottom) < kMaxCoordinateValue)
					{
					numberChars = sprintf ((char*)gTextString, 
															"%.*f", 
															decimalDigits, 
															coordinateRectangle.bottom);
					numberChars = InsertCommasInNumberString (
															(char*)gTextString, 
															numberChars, 
															2,
															numberChars);
					maxNumberCharacters = MAX (maxNumberCharacters, numberChars);
						
					}	// end "if (fabs (coordinateRectangle.bottom) < kMaxCoordinateValue)"
					
				else	// fabs (coordinateRectangle.bottom) >= kMaxCoordinateValue
					maxNumberCharacters = 60;
										
				if (fabs (coordinateRectangle.left) < kMaxCoordinateValue)
					{
					numberChars = sprintf ((char*)gTextString, 
															"%.*f", 
															decimalDigits, 
															coordinateRectangle.left);
					numberChars = InsertCommasInNumberString (
															(char*)gTextString, 
															numberChars, 
															2, 
															numberChars);
					maxNumberCharacters = MAX (maxNumberCharacters, numberChars);
						
					}	// end "if (fabs (coordinateRectangle.left) < kMaxCoordinateValue)"
					
				else	// fabs (coordinateRectangle.left) >= kMaxCoordinateValue
					maxNumberCharacters = 60;
										
				if (fabs (coordinateRectangle.right) < kMaxCoordinateValue)
					{
					numberChars = sprintf ((char*)gTextString, 
															"%.*f", 
															decimalDigits, 
															coordinateRectangle.right);
					numberChars = InsertCommasInNumberString (
															(char*)gTextString, 
															numberChars, 
															2, 
															numberChars);
					maxNumberCharacters = MAX (maxNumberCharacters, numberChars);
						
					}	// end "if (fabs (coordinateRectangle.right) < kMaxCoordinateValue)"
					
				else	// fabs (coordinateRectangle.right) >= kMaxCoordinateValue
					maxNumberCharacters = 60;
				
				}	// end "else viewUnits != kDMSLatLongUnitsMenuItem"
														
			maxNumberSelectionCharacters = 2*maxNumberCharacters + 3;
													
			break;
		
		}	// end "switch (viewUnits)"
													
			// Store the factor to be used to convert the coordinates to user
			// specified units.
			
	SetCoordinateViewFactor (windowInfoHandle, factor);
		
			// Store the max number of characters needed to write the cursor
			// and selection coordinates
											
	SetCoordinateViewNumberChars (windowInfoHandle, maxNumberCharacters);
	
			// Now get the width required for the cursor coordinate string.
	
	if (maxNumberCharacters >= 60)
		maxNumberCharacters = 14;
		
	numberChars = sprintf ((char*)gTextString, 
									"%*s",
									maxNumberCharacters, 
									" ");
			
	#if defined multispec_mac 
		GrafPtr			savedPort;
		
		GetPort (&savedPort);
		SetPortWindowPort (GetWindowPtr (windowInfoHandle));
		TextFont (gWindowTextFont); 			// monaco  		
		TextSize (9);
	#endif	// defined multispec_mac   
                             
	#if defined multispec_win
		imageViewCPtr = (CMImageView*)GetWindowPtr (windowInfoHandle);
		dpiScale = imageViewCPtr->m_dpiScale;
	#endif	// defined multispec_win 
						
	width = (SInt16)(dpiScale * TextWidth (gTextString, 0, numberChars) + 10);
	
			// Store the location to start writing the selection information.
	
	SetCoordinateViewSelectionStart (
						windowInfoHandle, 
						GetCoordinateViewCursorStart (windowInfoHandle)+width);

	#if defined multispec_wx
		imageViewCPtr = GetWindowPtr (windowInfoHandle);
		imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();
		coordinatesBar = imageFrameCPtr->m_coordinatesBar;
	
		((wxStaticText*)(coordinatesBar->FindWindow (IDC_CursorLine)))->
																GetSize (&linWidth, &linHeight);
		((wxStaticText*)(coordinatesBar->FindWindow (IDC_CursorLine)))->
																SetSize (width, linHeight);
	
		((wxStaticText*)(coordinatesBar->FindWindow (IDC_CursorColumn)))->
																GetSize (&linWidth, &linHeight);
		((wxStaticText*)(coordinatesBar->FindWindow (IDC_CursorColumn)))->
																SetSize (width, linHeight);
	#endif	// defined multispec_wx
	
			// Now get the width required for the selection coordinate string.
	
	if (maxNumberSelectionCharacters >= 123)
		maxNumberSelectionCharacters = 31;
	
	numberChars = sprintf ((char*)gTextString, 
									"%*s",
									maxNumberSelectionCharacters, 
									" ");
									
	width = (SInt16)(dpiScale * TextWidth (gTextString, 0, numberChars) + 10);
	
			// Store the location to start writing the number of pixels.
	
	SetCoordinateViewNumberPixelsStart (
						windowInfoHandle, 
						GetCoordinateViewSelectionStart (windowInfoHandle)+width);
						
	#if defined multispec_wx
		((wxStaticText*)(coordinatesBar->FindWindow (
										IDC_SelectionLine)))->GetSize (&linWidth, &linHeight);
		((wxStaticText*)(coordinatesBar->FindWindow (
										IDC_SelectionLine)))->SetSize (width, linHeight);
	
		((wxStaticText*)(coordinatesBar->FindWindow (
										IDC_SelectionColumn)))->GetSize (&linWidth, &linHeight);
		((wxStaticText*)(coordinatesBar->FindWindow (
										IDC_SelectionColumn)))->SetSize (width, linHeight);
	#endif	// defined multispec_wx

		// Get the factor to use to convert the number of selected pixels to area.
			
	displayAreaUnitsIndex = 
			GetCoordinateViewAreaUnits (windowInfoHandle) - kSqKilometersUnitsMenuItem;
	
	factor = 1.;
	if (GetCoordinateViewAreaUnits (windowInfoHandle) >= kAreaUnknownUnitsMenuItem)
		{
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);
													
		factor *= fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);
		factor *= fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);
		/*
				// Allow for a display of every nth line and mth column								
			
		Handle displaySpecsHandle = GetDisplaySpecsHandle (windowInfoHandle);
		DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
														displaySpecsHandle);
														
		factor *= displaySpecsPtr->displayedColumnInterval;
		factor *= displaySpecsPtr->displayedLineInterval;	
		*/
		}	// end "if (GetCoordinateViewAreaUnits... >= kAreaUnknownUnitsMenuItem)"
		
	if (filePlanarMapUnitsIndex >= 0 && displayAreaUnitsIndex >= 0)
		{
		conversion = gDistanceFileConversionFactors[filePlanarMapUnitsIndex] / 
										gDistanceDisplayConversionFactors[displayAreaUnitsIndex];
													
		factor *= conversion;
													
		factor *= conversion;
									
		}	// end "if (filePlanarMapUnitsIndex >= 0 && ..."
		
	SetCoordinateViewAreaFactor (windowInfoHandle, factor);
		
			// Store the max number of characters needed to write the number
			// of pixels or the user specified area units in the selected area.
			
	if (GetCoordinateViewAreaUnits (windowInfoHandle) <= kNumberPixelsUnitsMenuItem)
		{
		maxNumberCharacters = sprintf ((char*)gTextString, 
												"%u",
												(unsigned int)maxNumberPixels);
		
		maxNumberCharacters = InsertCommasInNumberString (
												(char*)gTextString, 
												maxNumberCharacters, 
												-1,
												maxNumberCharacters);
												
		}	// end "if (GetCoordinateViewAreaUnits ... <= kNumberPixelsUnitsMenuItem)"
										
	else	// coordinateViewAreaUnitCode > kNumberPixelsUnitsMenuItem
		{
		maxNumberCharacters = sprintf ((char*)gTextString, 
												"%.2f",
												(double)maxNumberPixels * factor);
		
		maxNumberCharacters = InsertCommasInNumberString (
												(char*)gTextString, 
												maxNumberCharacters, 
												2,
												maxNumberCharacters);
		
		}	// end "else coordinateViewAreaUnitCode > kNumberPixelsUnitsMenuItem"
											
	SetCoordinateViewNumberPixelsChars (windowInfoHandle, 
													maxNumberCharacters);
	
			// Store the location to start the area unit popup menu.
			//		First get the width of the maximum number of pixels.
			
	width = (SInt16)(dpiScale * TextWidth (gTextString, 0, maxNumberCharacters) + 4);
			
	areaUnitsCode = GetCoordinateViewAreaUnits (windowInfoHandle);
	areaUnitsControl = GetCoordinateViewAreaUnitsControl (windowInfoHandle);
	           
	#if defined multispec_mac
		GetControlPopUpMenuText (areaUnitsControl,
											areaUnitsCode,
											(char*)gTextString); 
	#endif	// defined multispec_mac 
	                             
	#if defined multispec_win
		imageFrameCPtr = imageViewCPtr->GetImageFrameCPtr ();

		if (imageFrameCPtr != NULL)
			imageFrameCPtr->GetCoordinateViewComboText (
															(char*)gTextString, IDC_AreaUnitsCombo);
	#endif	// defined multispec_win

	width = MAX (width, (SInt16)(dpiScale * StringWidth (gTextString)));
	
	SetCoordinateViewAreaPopupStart (
						windowInfoHandle, 
						GetCoordinateViewNumberPixelsStart (windowInfoHandle)+width+4);
	
			// Store the location to start the scale.
	
	width = GetCoordinateViewAreaPopupStart (windowInfoHandle) +
																			(SInt16)(dpiScale * 28);
	SetCoordinateViewScaleStart (
						windowInfoHandle, 
						width);
			
	#if defined multispec_mac 
		SetPort (savedPort);
	#endif	// defined multispec_mac
	
	#if defined multispec_wx
		coordinatesBar->Layout ();
	#endif
		
}	// end "SetCoordinateViewLocationParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetCylindricalEqualAreaParameters
//
//	Software purpose:	This routine sets the parameters for the Cylindrical Equal
//							Area projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/11/2006
//	Revised By:			Larry L. Biehl			Date: 04/22/2019

void SetCylindricalEqualAreaParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								radiusSpheroid,
				double								centralMeridian,
				double								standardParallel,
				double								falseEasting,
				double								falseNorthing)

{ 
	if (mapProjectionInfoPtr != NULL)
		{
		if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)
			{
			mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
			mapProjectionInfoPtr->geodetic.semiMajorAxis = radiusSpheroid;
			mapProjectionInfoPtr->geodetic.semiMinorAxis = radiusSpheroid;
			
			}	// end "if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)"
		
					// If the false easting and northing is in feet, convert
					// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
			
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		
		mapProjectionInfoPtr->gridCoordinate.standardParallel1 = standardParallel;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = centralMeridian;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetCylindricalEqualAreaParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetEllipsoidFromDatum
//
//	Software purpose:	This routine sets the ellipsoid based on the datum.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:			GetGeoKeyParameters in SGeoTIFF.cpp
//							ReadNDFHeader in SNDFFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/07/2007
//	Revised By:			Larry L. Biehl			Date: 03/11/2012		

void SetEllipsoidFromDatum (
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{ 
	if (mapProjectionInfoPtr != NULL)
		{
		mapProjectionInfoPtr->geodetic.spheroidCode = GetEllipsoidCodeFromDatumCode (
															mapProjectionInfoPtr->geodetic.datumCode);
																					
		}	// end "if (mapProjectionInfoPtr != NULL)"
			
}	// end "SetEllipsoidFromDatum" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetEquirectangularParameters
//
//	Software purpose:	This routine sets the parameters for the Sinusoidal projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/18/2007
//	Revised By:			Larry L. Biehl			Date: 10/18/2007		

void SetEquirectangularParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								radiusSpheroid,
				double								centralMeridian,
				double								standardParallel)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMajorAxis = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMinorAxis = radiusSpheroid;
		
		mapProjectionInfoPtr->gridCoordinate.standardParallel1 = standardParallel;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = centralMeridian;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetEquirectangularParameters"   


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetGDA94ParametersFromZone
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters for GDA94 reference system based on the input zone.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2012
//	Revised By:			Larry L. Biehl			Date: 03/17/2012

Boolean SetGDA94ParametersFromZone (
				SInt16								gridZone,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr)

{   
	Boolean								parametersSetFlag = FALSE;
	
	 
			// Get the central meridian for UTM
	
	if (abs (gridZone) >= 48 && abs (gridZone) <= 58)
		{
		*longitudeCentralMeridianPtr = -183 + abs (gridZone) * 6;
									
				// Set the latitude of the origin.
										
		*latitudeOriginPtr = 0.;
			
				// Set the scale factor of the central meridian for UTM
		
		*scaleFactorOfCentralMeridianPtr = 0.9996;
			
				// Get the false easting for UTM
		
		*falseEastingPtr = 500000.;
			
				// Get the false northing for UTM
		
		*falseNorthingPtr = 10000000.;
			
		parametersSetFlag = TRUE;
			
		}	// end "if (gridZone >= 48 && gridZone <= 58)"
		
	return (parametersSetFlag);
			
}	// end "SetGDA94ParametersFromZone"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetKrovakParameters
//
//	Software purpose:	This routine sets the parameters for the Krovak projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/30/2006
//	Revised By:			Larry L. Biehl			Date: 11/30/2006		

void SetKrovakParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = 49.5;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = 42.5;
		mapProjectionInfoPtr->gridCoordinate.projAzimuthAngle = 30.28813972222222;
		mapProjectionInfoPtr->gridCoordinate.standardParallel1 = 78.5;
		mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian = 0.9999;
		mapProjectionInfoPtr->gridCoordinate.falseEasting = 0;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = 0;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetKrovakParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetLambertAzimuthalEqualAreaParameters
//
//	Software purpose:	This routine sets the parameters for the Cylindrical Equal
//							Area projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/22/2007
//	Revised By:			Larry L. Biehl			Date: 04/07/2012		

void SetLambertAzimuthalEqualAreaParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								centralMeridian,
				double								latitudeOrigin,
				double								falseEasting,
				double								falseNorthing)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
				// If the false easting and northing is in feet, convert
				// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
			
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = latitudeOrigin;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = centralMeridian;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetLambertAzimuthalEqualAreaParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetLambertConformalConicParameters
//
//	Software purpose:	This routine sets the parameters for the Lambert Conformal
//							Conic projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/2012
//	Revised By:			Larry L. Biehl			Date: 04/06/2012		

void SetLambertConformalConicParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								standardParallel1,
				double								standardParallel2,
				double								centralMeridian,
				double								latitudeOrigin,
				double								falseEasting,
				double								falseNorthing)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->gridCoordinate.standardParallel1 = standardParallel1;
		mapProjectionInfoPtr->gridCoordinate.standardParallel2 = standardParallel2;
		mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude = centralMeridian;
		mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude = latitudeOrigin;
		
				// If the false easting and northing is in feet, convert
				// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
			
		mapProjectionInfoPtr->gridCoordinate.falseOriginEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing = falseNorthing;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
					
}	// end "SetLambertConformalConicParameters" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetMercatorParameters
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters with the input mercator parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2012
//	Revised By:			Larry L. Biehl			Date: 04/26/2012

void SetMercatorParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								semiMajorSpheroid,
				double								semiMinorSpheroid,
				double								longitudeCentralMeridian,
				double								latitudeOrigin,
				double								falseEasting,
				double								falseNorthing)

{   
	if (mapProjectionInfoPtr != NULL)
		{ 
		if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)
			{
			mapProjectionInfoPtr->geodetic.radiusSpheroid = semiMajorSpheroid;
			mapProjectionInfoPtr->geodetic.semiMajorAxis = semiMajorSpheroid;
			mapProjectionInfoPtr->geodetic.semiMinorAxis = semiMinorSpheroid;
			
			}	// end "if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)"
		
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = latitudeOrigin;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian =
																				longitudeCentralMeridian;
		
				// If the false easting and northing is in feet, convert
				// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
		
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
					
}	// end "SetMercatorParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetOrthographicParameters
//
//	Software purpose:	This routine sets the parameters for the Cylindrical Equal
//							Area projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/30/2006
//	Revised By:			Larry L. Biehl			Date: 08/10/2006		

void SetOrthographicParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								radiusSpheroid,
				double								longitudeOrigin,
				double								latitudeOrigin)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMajorAxis = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMinorAxis = radiusSpheroid;
		
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = latitudeOrigin;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = longitudeOrigin;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetOrthographicParameters" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetPolarStereographicParameters
//
//	Software purpose:	This routine sets the parameters for the Polar Stereographic projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/07/2011
//	Revised By:			Larry L. Biehl			Date: 04/07/2012		

void SetPolarStereographicParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								semiMajorSpheroid,
				double								semiMinorSpheroid,
				double								centralMeridian,
				double								latitudeOrigin,
				double								falseEasting,
				double								falseNorthing)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->geodetic.radiusSpheroid = semiMajorSpheroid;
		mapProjectionInfoPtr->geodetic.semiMajorAxis = semiMajorSpheroid;
		mapProjectionInfoPtr->geodetic.semiMinorAxis = semiMinorSpheroid;
		
				// If the false easting and northing is in feet, convert
				// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
			
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = latitudeOrigin;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = centralMeridian;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetPolarStereographicParameters"  

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetProjectionParameters
//
//	Software purpose:	This routine returns the parameters specific for the input
// 						projection code.  This routine handles most but not all of
//							the USGS Projection numbers 1 thru 30.  Those not handled 
//							are 8, 20, 21, 22 and 30.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			LoadImagineImageMapInfoForChannel in SImagine.cpp
//							ReadNDFHeader in SNDFFormat.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/06/2007
//	Revised By:			Larry L. Biehl			Date: 03/12/2012

void SetProjectionParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double*								projectionParametersPtr,
				Boolean								packedDegreesFlag,
				SInt16								angleUnitsCode)

{ 
	SInt16								projectionCode;
	
															
	projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
	
	if (packedDegreesFlag)
		{
		switch (projectionCode)
			{
			case kAlbersConicalEqualAreaCode:	
			case kLambertConformalConicCode:
			case kEquidistantConicCode:
				projectionParametersPtr[2] = 
					ConvertPackedDegreesToDecimalDegrees (projectionParametersPtr[2]);
				projectionParametersPtr[3] = 
					ConvertPackedDegreesToDecimalDegrees (projectionParametersPtr[3]);
					
			default:		
				if (projectionCode != kObliqueMercatorCode &&
										projectionCode !=  kSpaceObliqueMercatorCode)
					projectionParametersPtr[4] = 
						ConvertPackedDegreesToDecimalDegrees (projectionParametersPtr[4]);
					
				if (projectionCode != kMercatorCode &&
										projectionCode !=  kEquirectangularCode)
					projectionParametersPtr[5] = 
						ConvertPackedDegreesToDecimalDegrees (projectionParametersPtr[5]);
				break;
			
			}	// end "switch (projectionCode)"
		
		}	// end "if (packedDegreesFlag)"
	
	if (angleUnitsCode == kRadians)
		{
		switch (projectionCode)
			{
			case kAlbersConicalEqualAreaCode:	
			case kLambertConformalConicCode:
			case kEquidistantConicCode:
				projectionParametersPtr[2] *= kRadiansToDegrees;
				projectionParametersPtr[3] *= kRadiansToDegrees;
					
			default:		
				if (projectionCode != kObliqueMercatorCode &&
										projectionCode !=  kSpaceObliqueMercatorCode)
					projectionParametersPtr[4] *= kRadiansToDegrees;
					
				if (projectionCode != kMercatorCode &&
										projectionCode !=  kEquirectangularCode)
					projectionParametersPtr[5] *= kRadiansToDegrees;
				break;
			
			}	// end "switch (projectionCode)"
		
		}	// end "if (angleUnitsCode == kRadians)"
		
	switch (projectionCode)
		{
		case kAlbersConicalEqualAreaCode:	
		case kEquidistantConicCode:
			mapProjectionInfoPtr->gridCoordinate.standardParallel1 = projectionParametersPtr[2];
			mapProjectionInfoPtr->gridCoordinate.standardParallel2 = projectionParametersPtr[3];
			
		default:
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = projectionParametersPtr[4];
			mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = projectionParametersPtr[5];
			mapProjectionInfoPtr->gridCoordinate.falseEasting = projectionParametersPtr[6];
			mapProjectionInfoPtr->gridCoordinate.falseNorthing = projectionParametersPtr[7];
			
			if (projectionCode == kTransverseMercatorCode)
				mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian = projectionParametersPtr[2];
			
			if (projectionCode == kMercatorCode || projectionCode == kEquirectangularCode)
				mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian = projectionParametersPtr[5];
			break;
			
		case kLambertConformalConicCode:
			mapProjectionInfoPtr->gridCoordinate.standardParallel1 = projectionParametersPtr[2];
			mapProjectionInfoPtr->gridCoordinate.standardParallel2 = projectionParametersPtr[3];
			mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude = projectionParametersPtr[4];
			mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude = projectionParametersPtr[5];
			mapProjectionInfoPtr->gridCoordinate.falseOriginEasting = projectionParametersPtr[6];
			mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing = projectionParametersPtr[7];
			break;
			
		case kCylindricalEqualAreaCode:
			mapProjectionInfoPtr->gridCoordinate.standardParallel1 = projectionParametersPtr[2];
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = projectionParametersPtr[4];
			mapProjectionInfoPtr->gridCoordinate.falseOriginEasting = projectionParametersPtr[6];
			mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing = projectionParametersPtr[7];
			break;
		
		}	// end "switch (mapProjectionInfoPtr->gridCoordinate.code)"
		
}	// end "SetProjectionParameters"   

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetProjectionParametersFromReferenceSystem
//
//	Software purpose:	This routine returns the parameters specific for the input 
//							projection and zone.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			ReadGDALProjectionInformation in SGDALInterface.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 03/15/2012
//	Revised By:			Larry L. Biehl			Date: 03/26/2012

Boolean SetProjectionParametersFromReferenceSystem (
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{ 
	Boolean								parametersSetFlag = FALSE;
	
	
	if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode > kUserDefinedRSCode)
		parametersSetFlag = SetProjectionParametersFromZone (
							mapProjectionInfoPtr->gridCoordinate.referenceSystemCode,
							mapProjectionInfoPtr->geodetic.datumCode,
							&mapProjectionInfoPtr->gridCoordinate.gridZone,
							mapProjectionInfoPtr->gridCoordinate.epsgName,
							&mapProjectionInfoPtr->gridCoordinate.projectionCode,
							&mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian,
							&mapProjectionInfoPtr->gridCoordinate.latitudeOrigin,
							&mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian,
							&mapProjectionInfoPtr->gridCoordinate.falseEasting,
							&mapProjectionInfoPtr->gridCoordinate.falseNorthing,
							&mapProjectionInfoPtr->gridCoordinate.standardParallel1,
							&mapProjectionInfoPtr->gridCoordinate.standardParallel2,
							&mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude,
							&mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude,
							&mapProjectionInfoPtr->gridCoordinate.falseOriginEasting,
							&mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing);
		
	return (parametersSetFlag);
		
}	// end "SetProjectionParametersFromReferenceSystem"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetProjectionParametersFromZone
//
//	Software purpose:	This routine returns the parameters specific for the input 
//							projection and zone.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			CoordinateDialogSetParametersFromRS in SMapCoordinates.cpp
//							LoadSpheroidInformation in SMapCoordinates.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 04/19/2007
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean SetProjectionParametersFromZone (
				SInt16								referenceSystemCode,
				SInt16								datumCode,
				SInt16*								gridZonePtr,
				UCharPtr								fipsZoneNamePtr,
				SInt16*								projectionCodePtr,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr,
				double*								standardParallel1Ptr,
				double*								standardParallel2Ptr,
				double*								falseOriginLongitudePtr,
				double*								falseOriginLatitudePtr,
				double*								falseOriginEastingPtr,
				double*								falseOriginNorthingPtr)

{ 
	Boolean								parametersSetFlag = FALSE;
	
	switch (referenceSystemCode)
		{			
		case kGeographicRSCode:
			*projectionCodePtr = kGeographicCode;
			parametersSetFlag = TRUE;
			break;
			
		case kGaussKrugerRSCode:
			//if (datumCode == kPulkovo1942DatumCode && *gridZonePtr >= 0)
			parametersSetFlag = SetTMPulkovo1942ParametersFromZone (
																gridZonePtr,
																longitudeCentralMeridianPtr,
																latitudeOriginPtr,
																scaleFactorOfCentralMeridianPtr,
																falseEastingPtr,
																falseNorthingPtr);
				
			if (parametersSetFlag)
				*projectionCodePtr = kTransverseMercatorCode;
			break;
			
		case kGDA94RSCode:
			parametersSetFlag= SetGDA94ParametersFromZone (
															*gridZonePtr,
															longitudeCentralMeridianPtr,
															latitudeOriginPtr,
															scaleFactorOfCentralMeridianPtr,
															falseEastingPtr,
															falseNorthingPtr);
								
			if (parametersSetFlag)
				*projectionCodePtr = kTransverseMercatorCode;
			break;
		
		case kGGRS87RSCode:
			*longitudeCentralMeridianPtr = 24.;
			*latitudeOriginPtr = 0;
			*scaleFactorOfCentralMeridianPtr = 0.9996;
			*falseEastingPtr = 500000;
			*falseNorthingPtr = 0;
			parametersSetFlag = TRUE;
			*projectionCodePtr = kTransverseMercatorCode;
			break;
			
		case kStatePlaneNAD27RSCode:
		case kStatePlaneNAD83RSCode:	
			parametersSetFlag = SetStatePlaneParametersFromZone (
								*gridZonePtr,
								datumCode,
								fipsZoneNamePtr,
								projectionCodePtr,
								longitudeCentralMeridianPtr,
								latitudeOriginPtr,
								scaleFactorOfCentralMeridianPtr,
								falseEastingPtr,
								falseNorthingPtr,
								standardParallel1Ptr,
								standardParallel2Ptr,
								falseOriginLongitudePtr,
								falseOriginLatitudePtr,
								falseOriginEastingPtr,
								falseOriginNorthingPtr);
			break;
		
		case kUTM_NAD27RSCode:
		case kUTM_NAD83RSCode:
		case kUTM_SAD69RSCode:
		case kUTM_WGS72RSCode:
		case kUTM_WGS84RSCode:	
		case kUTMRSCode:	
			parametersSetFlag = SetUTMParametersFromZone (
								*gridZonePtr,
								longitudeCentralMeridianPtr,
								latitudeOriginPtr,
								scaleFactorOfCentralMeridianPtr,
								falseEastingPtr,
								falseNorthingPtr);
								
			if (parametersSetFlag)
				*projectionCodePtr = kTransverseMercatorCode;
			break;
			
		default:
			*projectionCodePtr = -2;
			
		}	// end "switch (referenceSystemCode)"

	//if (parametersSetFlag)
	//	*projectionCodePtr = GetProjectionCodeFromReferenceSystemCode (
	//																				referenceSystemCode,
	//																				*gridZonePtr);
		
	return (parametersSetFlag);
		
}	// end "SetProjectionParametersFromZone"  


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetTMPulkovo1942ParametersFromZone
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters for Transvers Mercator - Pulkovo 1942 / Gauss-Kruger
//							projection based on the input zone.
//
// 						This is what is called a Pulkovo 1942 / Gauss-Kruger projection
// 						For GeoTIFF tags, the numbers start at 28404.
// 						For EPSG tags, the numbers start at 28402. Don't know the reason for
// 						the difference.
//
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2007
//	Revised By:			Larry L. Biehl			Date: 04/30/2007

Boolean SetTMPulkovo1942ParametersFromZone (
				SInt16*								gridZonePtr,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr)

{  	
	SInt32								gridZone;
	
	Boolean								parametersSetFlag = FALSE;
	
	
			// If the grid zone is not available but the longitude of the central
			// meridian is available then, compute the grid zone.
			
	if (*gridZonePtr == 0 &&
			*longitudeCentralMeridianPtr >= 3 &&
				*longitudeCentralMeridianPtr <= 189)
		*gridZonePtr = (SInt16)(*longitudeCentralMeridianPtr + 3) / 6;
	
	gridZone = abs (*gridZonePtr);
	
	if (gridZone >= 1 && gridZone <= 32)
		{
				// Get the central meridian for the projection.
			
		*longitudeCentralMeridianPtr = -3 + gridZone * 6;
									
				// Set the latitude of the origin.
										
		*latitudeOriginPtr = 0.;
			
				// Set the scale factor of the central meridian
		
		*scaleFactorOfCentralMeridianPtr = 1.0000;
			
				// Get the false easting
		
		*falseEastingPtr = 500000. + gridZone * 1000000.;
			
				// Get the false northing
		
		*falseNorthingPtr = 0;
		
		parametersSetFlag = TRUE;
			
		}	// end "if (gridZone >= 1 && gridZone <= 32)"
		
	return (parametersSetFlag);
		
}	// end "SetTMPulkovo1942ParametersFromZone" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetSinusoidalParameters
//
//	Software purpose:	This routine sets the parameters for the Sinusoidal projection.
//
//	Parameters in:		Pointer to the map projection structure	
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/30/2006
//	Revised By:			Larry L. Biehl			Date: 04/06/2012		

void SetSinusoidalParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								radiusSpheroid,
				double								centralMeridian,
				double								standardParallel,
				double								falseEasting,
				double								falseNorthing)

{ 
	if (mapProjectionInfoPtr != NULL)
		{ 
		mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMajorAxis = radiusSpheroid;
		mapProjectionInfoPtr->geodetic.semiMinorAxis = radiusSpheroid;
		
				// If the false easting and northing is in feet, convert
				// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
			
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		
		mapProjectionInfoPtr->gridCoordinate.standardParallel1 = standardParallel;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = centralMeridian;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
			
}	// end "SetSinusoidalParameters"  


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetStatePlaneParametersFromZone
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters for State Plane based on the input zone.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2007
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean SetStatePlaneParametersFromZone (
				SInt16								fipsZone,
				SInt16								datumCode,
				UCharPtr								fipsZoneNamePtr,
				SInt16*								projectionCodePtr,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr,
				double*								standardParallel1Ptr,
				double*								standardParallel2Ptr,
				double*								falseOriginLongitudePtr,
				double*								falseOriginLatitudePtr,
				double*								falseOriginEastingPtr,
				double*								falseOriginNorthingPtr)

{    
	Boolean								parametersSetFlag = FALSE,
											useGDALFlag = FALSE;
	
	
	#if include_gdal_capability
		parametersSetFlag = GDALSetStatePlaneParametersFromZone (
																	fipsZone,
																	datumCode,
																	fipsZoneNamePtr,
																	projectionCodePtr,
																	longitudeCentralMeridianPtr,
																	latitudeOriginPtr,
																	scaleFactorOfCentralMeridianPtr,
																	falseEastingPtr,
																	falseNorthingPtr,
																	standardParallel1Ptr,
																	standardParallel2Ptr,
																	falseOriginLongitudePtr,
																	falseOriginLatitudePtr,
																	falseOriginEastingPtr,
																	falseOriginNorthingPtr);
	
		useGDALFlag = TRUE;
	#endif	// include_gdal_capability
	
	if (!useGDALFlag)
		{
				// Get the central meridian for State Plane
				// Default projection is kTransverseMercatorCode;
				
		*projectionCodePtr = kTransverseMercatorCode;
		
		if (fipsZone == 1301)
			{		
					// Set parameters for Indiana East.
			
			*longitudeCentralMeridianPtr = -85.6666666666667;				
			*latitudeOriginPtr = 37.5;
			*scaleFactorOfCentralMeridianPtr = 0.9999666667;
		
			*falseEastingPtr = 152400.3048;
			*falseNorthingPtr = 0.;
			if (datumCode == kNAD83Code)
				{
				*falseEastingPtr = 100000.;
				*falseNorthingPtr = 250000.;
				
				}	// end "if (datumCode == kNAD83Code)"
				
			parametersSetFlag = TRUE;
				
			}	// end "if (fipsZone == 1301)"
			
		else if (fipsZone == 1302)
			{		
					// Set parameters for Indiana West.		
			
			*longitudeCentralMeridianPtr = -87.08333333333333;		
			*latitudeOriginPtr = 37.5;
			*scaleFactorOfCentralMeridianPtr = 0.9999666667;
			
			*falseEastingPtr = 152400.3048;
			*falseNorthingPtr = 0.;
			if (datumCode == kNAD83Code)
				{
				*falseEastingPtr = 900000.;
				*falseNorthingPtr = 250000.;
				
				}	// end "if (datumCode == kNAD83Code)"
				
			parametersSetFlag = TRUE;
				
			}	// end "else if (gridZone == 1302)"
			
		else if (fipsZone == 3104)
			{		
					// Set parameters for New York Long Island.
					// kLambertConformalConicCode;

			if (datumCode == kNAD27Code || datumCode == kNAD83Code)
				{	
				*projectionCodePtr = kLambertConformalConicCode;
		
				*falseOriginLongitudePtr = -74.0;		
				*falseOriginLatitudePtr = 40.5;
				*standardParallel1Ptr = 40.66666666666667;
				*standardParallel2Ptr = 41.03333333333333;
				
				*falseOriginEastingPtr = 609601.21920;
				*falseOriginNorthingPtr = 30480.06096;
				if (datumCode == kNAD83Code)
					{	
					*falseOriginLatitudePtr = 40.16666666666667;
					*falseOriginEastingPtr = 300000.;
					*falseOriginNorthingPtr = 0.;
					
					}	// end "if (datumCode == kNAD83Code)"
					
				parametersSetFlag = TRUE;

				}	// end "if (datumCode == kNAD27Code || ..."
				
			}	// end "else if (fipsZone == 3104)"
			
		}	// end "if (!useGDALFlag)"
		
	return (parametersSetFlag);
			
}	// end "SetStatePlaneParametersFromZone" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetTransverseMercatorParameters
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters with the input transverse mercator parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/16/2008
//	Revised By:			Larry L. Biehl			Date: 04/07/2012

void SetTransverseMercatorParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double								radiusSpheroid,
				double								longitudeCentralMeridian,
				double								latitudeOrigin,
				double								scaleFactorOfCentralMeridian,
				double								falseEasting,
				double								falseNorthing)

{   
	if (mapProjectionInfoPtr != NULL)
		{ 
		if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)
			{
			mapProjectionInfoPtr->geodetic.radiusSpheroid = radiusSpheroid;
			mapProjectionInfoPtr->geodetic.semiMajorAxis = radiusSpheroid;
			mapProjectionInfoPtr->geodetic.semiMinorAxis = radiusSpheroid;
			
			}	// end "if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)"
		
		mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = latitudeOrigin;
		mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian =
																			longitudeCentralMeridian;
		mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian =
																			scaleFactorOfCentralMeridian;
		
				// If the false easting and northing is in feet, convert
				// to meters for use in calculations.
			
		ConvertFeetToMeters (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										&falseEasting, 
										&falseNorthing);
		
		mapProjectionInfoPtr->gridCoordinate.falseEasting = falseEasting;
		mapProjectionInfoPtr->gridCoordinate.falseNorthing = falseNorthing;
		
		}	// end "if (mapProjectionInfoPtr != NULL)" 
					
}	// end "SetTransverseMercatorParameters" 


/*                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpEllipsoidPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the coordinate units
//							popup menu according to the map information available.
//								This is currently not used so commented out.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/04/2000
//	Revised By:			Larry L. Biehl			Date: 12/12/2000

void SetUpEllipsoidPopUpMenu (
				DialogPtr							dialogPtr)

{    
	#if defined multispec_mac
		MenuHandle menuHandle = gPopUpEllipsoidMenu;
	
		EnableMenuItem (menuHandle, kNoEllipsoidDefinedMenuItem);	 		
		EnableMenuItem (menuHandle, kAiryEllipsoidMenuItem);	 		
		EnableMenuItem (menuHandle, kAustralianEllipsoidMenuItem);
		EnableMenuItem (menuHandle, kBesselEllipsoidMenuItem);
		EnableMenuItem (menuHandle, kClarke1866EllipsoidMenuItem);
		EnableMenuItem (menuHandle, kClarke1880EllipsoidMenuItem);
		EnableMenuItem (menuHandle, kEverestEllipsoidMenuItem);
		EnableMenuItem (menuHandle, kGRS80EllipsoidMenuItem);
		EnableMenuItem (menuHandle, kInternational1909EllipsoidMenuItem);
		EnableMenuItem (menuHandle, kKrassovskyEllipsoidMenuItem);
		EnableMenuItem (menuHandle, kSphereEllipsoidMenuItem);
		EnableMenuItem (menuHandle, kWGS72EllipsoidMenuItem);
		EnableMenuItem (menuHandle, kWGS84EllipsoidMenuItem);
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_wx
		SInt16							index;
			
		CComboBox* comboBoxPtr = 
					(CComboBox*)dialogPtr->GetDlgItem (IDC_EllipsoidCombo);

				// Set the item data value for each popup menu item.

		index = comboBoxPtr->GetCount () - 1;
		while (index >= 0)
			{
			comboBoxPtr->SetItemData (index, index);
				
			index--;

			}	// end "while (index > 0)"
	#endif	// defined multispec_win || defined multispec_wx
			
}	// end "SetUpEllipsoidPopUpMenu" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpHorizontalDatumPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the coordinate units
//							popup menu according to the map information available.
//								This is currently not used so commented out.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/30/2001
//	Revised By:			Larry L. Biehl			Date: 03/23/2022

void SetUpHorizontalDatumPopUpMenu (
				DialogPtr							dialogPtr)

{    
	#if defined multispec_mac	
		MenuHandle menuHandle = gPopUpHorizontalDatumMenu;
	
		EnableMenuItem (menuHandle, kNoDatumDefinedMenuItem);	 		
		EnableMenuItem (menuHandle, kNAD27MenuItem);	 		
		EnableMenuItem (menuHandle, kNAD83MenuItem);
		EnableMenuItem (menuHandle, kWGS84MenuItem);			
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_wx
		SInt16							index;
			
		CComboBox* comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_DatumCombo);

				// Set the item data value for each popup menu item.

		index = comboBoxPtr->GetCount () - 1;
		while (index >= 0)
			{
			comboBoxPtr->SetItemData (index, index);
				
			index--;

			}	// end "while (index > 0)"
	#endif	// defined multispec_win || defined multispec_wx
			
}	// end "SetUpHorizontalDatumPopUpMenu"  


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpProjectionPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the coordinate units
//							popup menu according to the map information available.
//								This is currently not used so commented out.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/10/2000
//	Revised By:			Larry L. Biehl			Date: 03/23/2012

void SetUpProjectionPopUpMenu (
				DialogPtr							dialogPtr)

{    
	#if defined multispec_mac	
		MenuHandle menuHandle = gPopUpProjectionMenu;
	
		EnableMenuItem (menuHandle, kNoneDefinedMenuItem);
		EnableMenuItem (menuHandle, kAlbersConicalEqualAreaMenuItem);
		EnableMenuItem (menuHandle, kLambertConformalConicMenuItem);
		EnableMenuItem (menuHandle, kMercatorMenuItem);
		EnableMenuItem (menuHandle, kPolarStereographicMenuItem);
		EnableMenuItem (menuHandle, kPolyconicMenuItem);
		EnableMenuItem (menuHandle, kEquidistantConicMenuItem);
		EnableMenuItem (menuHandle, kTransverseMercatorMenuItem);
		EnableMenuItem (menuHandle, kStereographicMenuItem);
		EnableMenuItem (menuHandle, kLambertAzimuthalEqualAreaMenuItem);
		EnableMenuItem (menuHandle, kAzimuthalEquidistantMenuItem);
		EnableMenuItem (menuHandle, kGnomonicMenuItem);
		EnableMenuItem (menuHandle, kOrthographicMenuItem);
		EnableMenuItem (menuHandle, kGeneralVerticalNearSideMenuItem);
		EnableMenuItem (menuHandle, kSinusoidalMenuItem);
		EnableMenuItem (menuHandle, kEquirectangularMenuItem);
		EnableMenuItem (menuHandle, kMillerCylindricalMenuItem);
		EnableMenuItem (menuHandle, kVanderGrintenIMenuItem);
		EnableMenuItem (menuHandle, kObliqueMercatorMenuItem);
		EnableMenuItem (menuHandle, kSpaceObliqueMercatorMenuItem);
		EnableMenuItem (menuHandle, kModifiedTransverseMercatorMenuItem);			
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_wx
		SInt16							index;
			
		CComboBox* comboBoxPtr = 
					(CComboBox*)dialogPtr->GetDlgItem (IDC_ProjectionCombo);

				// Set the item data value for each popup menu item.

		index = comboBoxPtr->GetCount () - 1;
		while (index >= 0)
			{
			comboBoxPtr->SetItemData (index, index);
				
			index--;

			}	// end "while (index > 0)"			
	#endif	// defined multispec_win || defined multispec_wx
			
}	// end "SetUpProjectionPopUpMenu"      


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpReferenceSystemPopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the reference system
//							popup menu.
//								This is currently not used so commented out.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/22/2012
//	Revised By:			Larry L. Biehl			Date: 03/23/2012

void SetUpReferenceSystemPopUpMenu (
				DialogPtr							dialogPtr)

{    
	#if defined multispec_mac
	
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_wx
		SInt16							index;
			
		CComboBox* comboBoxPtr = 
					(CComboBox*)dialogPtr->GetDlgItem (IDC_ReferenceSystemCombo);

				// Set the item data value for each popup menu item.

		index = comboBoxPtr->GetCount () - 1;
		while (index >= 0)
			{
			comboBoxPtr->SetItemData (index, index);
				
			index--;

			}	// end "while (index > 0)"
	#endif	// defined multispec_win || defined multispec_wx
			
}	// end "SetUpReferenceSystemPopUpMenu"  
*/

                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUTMParametersFromZone
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters for UTM based on the input zone.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogInitialize in SMapCoordinates.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2007
//	Revised By:			Larry L. Biehl			Date: 04/30/2007

Boolean SetUTMParametersFromZone (
				SInt16								gridZone,
				double*								longitudeCentralMeridianPtr,
				double*								latitudeOriginPtr,
				double*								scaleFactorOfCentralMeridianPtr,
				double*								falseEastingPtr,
				double*								falseNorthingPtr)

{   
	Boolean								parametersSetFlag = FALSE;
	
	 
			// Get the central meridian for UTM
	
	if (gridZone != 0 && abs (gridZone) <= 60)
		{
		*longitudeCentralMeridianPtr = -183 + abs (gridZone) * 6;
									
				// Set the latitude of the origin.
										
		*latitudeOriginPtr = 0.;
			
				// Set the scale factor of the central meridian for UTM
		
		*scaleFactorOfCentralMeridianPtr = 0.9996;
			
				// Get the false easting for UTM
		
		*falseEastingPtr = 500000.;
			
				// Get the false northing for UTM
		
		*falseNorthingPtr = 0.;
		if (gridZone < 0)
			*falseNorthingPtr = 10000000.;
			
		parametersSetFlag = TRUE;
			
		}	// end "if (gridZone != 0 && abs (gridZone) <= 60)"
		
	return (parametersSetFlag);
			
}	// end "SetUTMParametersFromZone"  
								
								

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 TransformCoordinatePoint
//
//	Software purpose:	The purpose of this routine is to determine if a polynomial
//							model is need and if so get the coefficients for that model.
//
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/09/2007
//	Revised By:			Larry L. Biehl			Date: 02/10/2007	

SInt16 TransformCoordinatePoint (
				double								inputEasting,
				double								inputNorthing,
				double* 								eastingPtr,
				double* 								northingPtr,
				double* 								eCoefPtr,
				double*								nCoefPtr,
				SInt16								order)

{
	double 								e2, 
											e2n,
											e3,
											en, 
											en2,
											n2, 
											n3;
											
											
	switch (order)
		{
		case 1:

			*eastingPtr = eCoefPtr[0] + 
							eCoefPtr[1] * inputEasting + eCoefPtr[2] * inputNorthing;
							
			*northingPtr = nCoefPtr[0] + 
							nCoefPtr[1] * inputEasting + nCoefPtr[2] * inputNorthing;
			break;

		case 2:

			e2 = inputEasting * inputEasting;
			n2 = inputNorthing * inputNorthing;
			en = inputEasting * inputNorthing;

			*eastingPtr = eCoefPtr[0] + 
					eCoefPtr[1] * inputEasting + eCoefPtr[2] * inputNorthing +
					eCoefPtr[3] * e2 + eCoefPtr[4] * en + eCoefPtr[5] * n2;
				
			*northingPtr = nCoefPtr[0] + 
					nCoefPtr[1] * inputEasting + nCoefPtr[2] * inputNorthing +
					nCoefPtr[3] * e2 + nCoefPtr[4] * en + nCoefPtr[5] * n2;
			break;

		case 3:

			e2  = inputEasting * inputEasting;
			en  = inputEasting * inputNorthing;
			n2  = inputNorthing * inputNorthing;
			e3  = inputEasting * e2;
			e2n = e2 * inputNorthing;
			en2 = inputEasting * n2;
			n3  = inputNorthing * n2;

			*eastingPtr = eCoefPtr[0] +
					eCoefPtr[1] * inputEasting + eCoefPtr[2] * inputNorthing  +
					eCoefPtr[3] * e2 + eCoefPtr[4] * en  + eCoefPtr[5] * n2  +
					eCoefPtr[6] * e3 + eCoefPtr[7] * e2n + eCoefPtr[8] * en2 +
					eCoefPtr[9] * n3;
				
			*northingPtr = nCoefPtr[0] +
					nCoefPtr[1] * inputEasting + nCoefPtr[2] * inputNorthing  +
					nCoefPtr[3] * e2 + nCoefPtr[4] * en  + nCoefPtr[5] * n2  +
					nCoefPtr[6] * e3 + nCoefPtr[7] * e2n + nCoefPtr[8] * en2 +
					nCoefPtr[9] * n3;
			break;

		default:
			return MPARMERR;
			
		}	// end "switch (order)"

	return MSUCCESS;
	
}	// end "TransformCoordinatePoint"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateScaleInformation
//
//	Software purpose:	The purpose of this routine is to compute the scale of the
//							specified image that is displayed.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/10/2000
//	Revised By:			Larry L. Biehl			Date: 11/10/2000			

void UpdateScaleInformation (
				Handle								windowInfoHandle)

{		
	if (GetCoordinateHeight (windowInfoHandle) > 0)
		{
		ComputeAndSetScaleInformation (windowInfoHandle);
		
		DrawScaleInformation (windowInfoHandle);
		
		}	// end "if (GetCoordinateHeight (windowInfoHandle) > 0)"

}	// end "UpdateScaleInformation" 
