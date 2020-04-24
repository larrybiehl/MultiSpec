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
// File:                SArcView.cpp
//
//	Authors:             Larry L. Biehl
//
//	Revision date:       04/11/2020
//
//	Language:            C
//
//	System:              Linux, Macintosh and Windows Operating Systems
//
//	Brief description:   This file contains routines which are used to access
//	                     various Esri ArcView type disk files including .bil, .bsq,
//                      .bip, and shape files.
//
/* Template for debugging
      int numberChars = sprintf ((char*)gTextString3,
                                  " SArcView:xxx (entered routine. %s",
                                  gEndOfLine)
      ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "SFileStream_class.h"
#include "locale.h"
   
#if defined multispec_wx
	#include "xImageView.h"
   #include "xOverlayParametersDialog.h"
	//#define include_gdal_capability 0
#endif
	
#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_LineColorPrompt		3
	#define	IDC_LineColor				4
	#define	IDC_ValuePrompt			5
	
	#define	IDS_Alert146				146
	
	#define	IDS_FileIO180				180
	#define	IDS_FileIO181				181
	#define	IDS_FileIO182				182
	#define	IDS_FileIO183				183
	#define	IDS_FileIO184				184
	#define	IDS_FileIO185				185
	#define	IDS_FileIO186				186
	#define	IDS_FileIO191				191
	#define	IDS_FileIO203				203
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	#include "WOverlayParametersDialog.h"

	void SetPenCharacteristics (
					CPen*									newPenPtr,
					RGBColor*							rgbColorPtr,
					SInt16								lineThickness);	 
#endif	// defined multispec_win

#define	kAllShapeHandleMemory				0
#define	kVectorMemoryOnly						1

#define	vector_requires_window			1

#include "errno.h"

#if include_gdal_capability
//#ifndef multispec_wx
//		#include "ogr_spatialref.h"
//		#include "dbfopen.h"
//#else

	// oul: added definition of SIZE_UNSIGNED_LONG and SIZEOF_VOIDP
	// which are not defined in cpl_config.h
	
	#if defined multispec_wx
		#if defined NetBeansProject
				// The size of a 'unsigned long', as computed by sizeof.
			#define SIZEOF_UNSIGNED_LONG 8
				// The size of a 'void p', as computed by sizeof.
			#define SIZEOF_VOIDP 8
		#endif
	#endif

	#include "ogr_spatialref.h"
/*
	#ifdef multispec_wx
		#include "dbfopen.h"
	#endif
	#ifdef multispec_mac
		#//include "shapefil.h"
		#include "dbfopen.h"
	#endif
	#ifdef multispec_win
		#include "shapefil.h"
	#endif
*/
//#endif	
#endif	// include_gdal_capability

#define	kDoNotCloseOverlayFileIfNoRecordsRead		0
#define	kCloseOverlayFileIfNoRecordsRead				1

#define	kAVDecimalDegreesCode						1
#define	kAVDecimalMinutesCode						2
#define	kAVDecimalSecondsCode						3
#define	kAVDegreesMinutesSecondsCode				4
#define	kAVRadiansCode									5


	 
#if defined multispec_win || defined multispec_wx
	#pragma pack(4)
#endif	// defined multispec_win || defined multispec_wx

#if defined PRAGMA_STRUCT_ALIGN
	//#pragma options align=mac68k
	#pragma pack(2)
#endif



typedef struct ArcViewMultiPoint
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	DoubleRect					box;
	UInt32						numPoints; 
	ArcViewDoublePoint		points[1];
	
	} ArcViewMultiPoint, *ArcViewMultiPointPtr;
	 
	
typedef struct ArcViewPoint
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	ArcViewDoublePoint		point;
	
	} ArcViewPoint, *ArcViewPointPtr;
	 
	
typedef struct ArcViewPolyLine
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	DoubleRect					box;
	UInt32						numParts;
	UInt32						numPoints; 
	UInt32						parts[1];
	
	} ArcViewPolyLine, *ArcViewPolyLinePtr;
	 
	
typedef struct ArcViewRecordHeader
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	
	} ArcViewRecordHeader, *ArcViewRecordHeaderPtr;
	
#if defined PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#endif
	 
#if defined multispec_win || defined multispec_wx
	#pragma pack()
#endif	// defined multispec_win || defined multispec_wx
							

			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	

SInt16 AddToWindowOverlayList (
				WindowPtr							windowPtr,
				DoubleRect*							boundingRectPtr,
				SInt32								overlayNumber,
				Boolean								mapUnitsFlag);

SInt16 CheckIfOverlayFileLoaded (
				CMFileStream*						fileStreamPointer,
				DoubleRect*							boundingRectPtr,
				SInt16*								overlayNumberPtr,
				UInt16*								versionNumberLoadedPtr);

Boolean CheckIfOverlayMayBeLatLong (
				CMFileStream						*shapeFileStreamPtr,
				DoubleRect*							boundingOverlayRectPtr);

SInt16 CheckIfOverlayNeedsReloaded (
				WindowPtr							newWindow,
				SInt16								overlayNumber,
				DoubleRect*							boundingRectPtr);	

#if include_gdal_capability
	SInt16 CheckIfprjFileDefinesLatLong (
					CMFileStream						*shapeFileStreamPtr);
#endif	// include_gdal_capability

void ConvertLatLongPointToMapPoint (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				ArcViewDoublePointPtr			sdoubleCoordinatePointPtr);

void DisplayNoIntersectionAlert (
				SInt16								stringNumber);

PascalVoid DrawColorBoxInDialogWindow (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

CMFileStream* GetFileStreamPointer (
				ShapeInfoPtr						shapeInfoPtr);

Boolean GetArcViewMapInformation (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				Boolean*								geoSpotHeaderFlagPtr);

void GetLastVectorColorAndWidth (
				SInt32								overlayNumber,
				RGBColor*							lastLineColorPtr,
				UInt16*								lastLineThicknessPtr);

Boolean GetMemoryForVectorData (
				ShapeInfoPtr		 				shapeInfoPtr);

void InitializeOverlay (
				WindowInfoPtr		 				windowInfoPtr,
				SInt32								overlayNumber);

void  ReleaseShapeFileMemory (
				Handle*								shapeHandlePtr,
				SInt16								overlayIndex,
				SInt16								memoryReleaseCode);

void SetLastVectorColorAndWidth (
				SInt32								overlayNumber,
				RGBColor*							lastLineColorPtr,
				UInt16								lastLineThickness);

Boolean ShapeAndWindowAreasIntersect (
				SInt16								overlayNumber,
				DoubleRect*							boundingRectPtr);

Boolean WindowBoundingAreaAndRectIntersect (
				WindowPtr							windowPtr,
				DoubleRect*							boundingRectPtr,
				Boolean								inputIsMapTypeFlag);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddCurrentVectorOverlaysToImageWindow
//
//	Software purpose:	This routine checks if the input overlay file is already loaded
//							into memory.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			DisplayImage in SDisplay.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/08/2006
//	Revised By:			Larry L. Biehl			Date: 04/29/2012

void AddCurrentVectorOverlaysToImageWindow (
				WindowInfoPtr						windowInfoPtr)

{
	DoubleRect							lBoundingRect;
											
	Handle*								shapeHandlePtr;
	MapProjectionInfoPtr 			mapProjectionInfoPtr;
	ShapeInfoPtr						shapeInfoPtr;
	WindowPtr							windowPtr;
	
	Handle 								mapProjectionHandle;	
	
	UInt32								index;
	
	SInt16								overlayCheckReturn,
											overlayNumber;
	
	Boolean								convertedFlag,
											mapUnitsFlag;
											
	
			// Verify that shape files do exist.
			
	if (gNumberShapeFiles <= 0)
																							return;
	
			// If there is no valid map information for the image, then no
			// shape files will intersect with the image.
			
	if (!FindIfMapInformationExists (windowInfoPtr))
																							return;
	
	windowPtr = GetWindowPtr (windowInfoPtr);
		
	shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, kLock);

	if (shapeHandlePtr != NULL) 
		{
		mapProjectionHandle = GetFileMapProjectionHandle2 (
															windowInfoPtr->windowInfoHandle);

		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
															mapProjectionHandle, kLock);

				// Loop through existing overlay files and check if the shape file
				// intesects with the input image.
	
		for (index=0; index<gShapeHandleListLength; index++)
			{
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeHandlePtr[index],
																			kLock);
															
			if (shapeInfoPtr != NULL)
				{										
						// If the input shape file needs to be converted from lat-
						// long units to the map units of the image, then be sure that
						// the conversion can be made.
					
				overlayCheckReturn = 5;
				convertedFlag = TRUE;
				
						// First verify that the grid coordinate system and the 
						// geodetic information for the image window match that
						// for which the vector file is being used for.  	

				if (!CheckIfMapInfoMatches (&shapeInfoPtr->geodetic,
														&mapProjectionInfoPtr->geodetic,
														&shapeInfoPtr->gridCoordinate,
														&mapProjectionInfoPtr->gridCoordinate))
					convertedFlag = FALSE;
					
						// Check for case where the shape file is in lat-long but the
						// map projection for the image window is not in lat-long
						// units.  The shape file will need to be reloaded in a separate
						// File - Open Image/Shape file menu operation.
				
				if (shapeInfoPtr->conversionCode == 0 && 
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode !=
																						kGeographicRSCode)
					convertedFlag = FALSE;
					
						// Also check for case where the shape file has been converted
						// to map units but the map projection for the image window is 
						// in lat-long units.  The shape file will need to be reloaded in 
						// a separate File - Open Image/Shape file menu operation.
				
				if (shapeInfoPtr->conversionCode == 2 && 
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																						kGeographicRSCode)
					convertedFlag = FALSE;
				
				overlayNumber = (SInt16)(index + 1);		
				mapUnitsFlag = TRUE;						
				if (convertedFlag && shapeInfoPtr->conversionCode == 2)
					{
					lBoundingRect = shapeInfoPtr->boundingXYBox;
					convertedFlag = ConvertLatLongRectToMapRect (
												mapProjectionInfoPtr, 
												&lBoundingRect);
					mapUnitsFlag = FALSE;	
					
					}	// end "if (convertedFlag && ...->conversionCode == 2)"
				
						// Add vector overlay to the window if the shape file intersects 
						// with the image.
				
				if (convertedFlag)	
					overlayCheckReturn = AddToWindowOverlayList (
																	windowPtr,
																	&shapeInfoPtr->boundingXYBox,
																	overlayNumber,
																	mapUnitsFlag);
						
				if (overlayCheckReturn == 2)
					{
							// Vector overlay does intersect with the window.
							
					overlayCheckReturn = CheckIfOverlayNeedsReloaded (
																	windowPtr,
																	overlayNumber,
																	&shapeInfoPtr->boundingXYBox);
					
					if (overlayCheckReturn == 0)
						{
								// Release memory for current shape information and set up
								// to reload it.
								
						ReleaseShapeFileMemory (shapeHandlePtr, 
														(SInt16)(overlayNumber-1),
														kVectorMemoryOnly);
						
						if (!GetMemoryForVectorData (shapeInfoPtr))
							break;
						
								// Make sure that the vectors are not drawn until after
								// they have been read in.

						windowInfoPtr->drawVectorOverlaysFlag = FALSE;
						ReadArcViewShapeFile ((SInt16)index, 
														!mapUnitsFlag, 
														kDoNotCloseOverlayFileIfNoRecordsRead);
						windowInfoPtr->drawVectorOverlaysFlag = TRUE;
						
						}	// end "if (overlayCheckReturn == 0)"
						
					}	// end "if (overlayCheckReturn == 2)"
				
				CheckAndUnlockHandle (shapeHandlePtr[index]);
				
				}	// end "if (shapeInfoPtr != NULL)"
											
			}	// end "for (index=0; index<gShapeHandleListLength; index++)"
			
		CheckAndUnlockHandle (mapProjectionHandle);
		CheckAndUnlockHandle (gShapeFilesHandle);
				
		UpdateOverlayControl (windowPtr);
	
		}	// end "if (shapeHandlePtr != NULL)"
	
}	// end "AddCurrentVectorOverlaysToImageWindow"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddToWindowOverlayList
//
//	Software purpose:	This routine checks if the overlay intersects with the specified
//							window and adds it to the overlay list if it does.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	= 1: input parameters do not make sense
//							= 2: overlay was added to the window
//							= 3: overlay was already included in the window
//							= 5: overlay was not added to the window, it did not intersect it
//
// Called By:			CheckIfOverlayFileLoaded in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/29/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

SInt16 AddToWindowOverlayList (
				WindowPtr							windowPtr,
				DoubleRect*							boundingRectPtr,
				SInt32								overlayNumber,
				Boolean								mapUnitsFlag)

{	
	WindowInfoPtr						windowInfoPtr;
	Handle								windowInfoHandle;
	
	UInt32								index;
	
	SInt16								returnCode = 1;
	
			
		// Now load the list for the window.
			
	windowInfoHandle = GetWindowInfoHandle (windowPtr);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL)
		{
		if (WindowBoundingAreaAndRectIntersect ((WindowPtr)windowInfoPtr->windowPtr,
																boundingRectPtr,
																mapUnitsFlag))
			{
					// Now check if the overlay is already included in the list.
					
			for (index=0; index<windowInfoPtr->numberVectorOverlays; index++)
				{
				if (abs (windowInfoPtr->overlayList[index].index) == overlayNumber)
					{
					returnCode = 3;
					break;
					
					}	// end "if (abs (windowInfoPtr->overlayList[index].index) == ..."
				
				}	// end "for (index=0; index<windowInfoPtr->numberVectorOverlays; ..."
			
			if (returnCode == 1)
				{	
						// Add the overlay to the list. Use default color and line
						// thickness.
						
				InitializeOverlay (windowInfoPtr, overlayNumber);
				returnCode = 2;
				
				}	// end "if (returnCode == 1)"
			
			}	// end "if (AreaIntersects (...->boundingMapRectangle, ..."
			
		else	// !AreasIntersect (&windowInfoPtr->boundingMapRectangle,
			returnCode = 5;
			
		}	// end "if (windowInfoPtr != NULL)"
		
	return (returnCode);
	
}	// end "AddToWindowOverlayList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CheckIfOverlayFileLoaded
//
//	Software purpose:	This routine checks if the input overlay file is already loaded
//							into memory.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	= 0: overlay does not exist yet in the overlay list
//							= 2: overlay was added to the window
//							= 3: overlay was already included in the window
//							= 5: overlay was not added to the window, it did not intersect it
//							= 10: overlay does not exist yet in the overlay list.  It needs
//										to be load but converted from lat-long to map units.
//
// Called By:			ReadArcViewShapeHeader in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/29/2001
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 CheckIfOverlayFileLoaded (
				CMFileStream*						fileStreamPointer,
				DoubleRect*							boundingRectPtr,
				SInt16*								overlayNumberPtr,
				UInt16*								versionNumberLoadedPtr)

{
	DoubleRect							lBoundingRect;
	FileStringPtr						fileNamePPointer,
											newOverlayFileNamePPointer;
											
	Handle*								shapeHandlePtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	Handle								mapProjectionHandle;
	
	UInt32								index,
											overlayNumber = 0;
	
	SInt16								conversionCode,
											overlayCheckReturn = 0,
											referenceSystemCode;
											
	UInt16								versionCount = 1;
	
	Boolean								alreadyLoadedFlag = FALSE,
											convertedFlag,
											mapInfoSameFlag,
											mapUnitsFlag,
											mayBeLatLongUnitsFlag;
	
	
	conversionCode = 0;
	mapUnitsFlag = TRUE;		
	
	newOverlayFileNamePPointer =
					(FileStringPtr)GetFileNamePPointerFromFileStream (fileStreamPointer);

	mayBeLatLongUnitsFlag = CheckIfOverlayMayBeLatLong (fileStreamPointer, 
																			boundingRectPtr);
		
	shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, kLock);

	if (shapeHandlePtr != NULL) 
		{
				// Get pointer to the map projection structure if we need to convert
				// to map units.									
			
		mapProjectionHandle = GetFileMapProjectionHandle2 (gActiveImageWindowInfoH);											
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle,
																				kLock);
																	
		referenceSystemCode = 0;
		if (mapProjectionInfoPtr != NULL)
			referenceSystemCode =
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode;
																	
				// Loop through existing overlay files and check if file name
				// already exists. Currently this will not catch case where there
				// are two files of the same name but in different directories.
	
		for (index=0; index<gShapeHandleListLength; index++)
			{
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
																	shapeHandlePtr[index],
																	kLock);
															
			if (shapeInfoPtr != NULL)
				{										
				fileNamePPointer =
							(FileStringPtr)GetFileNamePPointerFromShapeInfo (shapeInfoPtr);
			
				if (StringCompare (&newOverlayFileNamePPointer[2],
												 &fileNamePPointer[2]) == 0)
					{
					versionCount++;
					
					if (mapProjectionInfoPtr != NULL)
						{
								// First verify that the map information that the shape file
								// has been used for is the same as that for this image window.
							
						mapInfoSameFlag = CheckIfMapInfoMatches (
															&shapeInfoPtr->geodetic,
															&mapProjectionInfoPtr->geodetic,
															&shapeInfoPtr->gridCoordinate,
															&mapProjectionInfoPtr->gridCoordinate);
						
						lBoundingRect = *boundingRectPtr;
						convertedFlag = FALSE;
						if (mapInfoSameFlag)
							{
									// If the input shape file needs to be converted from lat-
									// long units to the map units of the image, then be sure
									// that the conversion can be made.
									// Note that the shape file will have to be reloaded if the
									// shape file has been converted to map units and the
									// active image window is in lat-long units.
							
							convertedFlag = TRUE;
							mapUnitsFlag = TRUE;
							
							if (shapeInfoPtr->conversionCode == 2 &&
											referenceSystemCode == kGeographicRSCode)
								convertedFlag = FALSE;
							
							else if (shapeInfoPtr->conversionCode == 2)
								{
								convertedFlag = ConvertLatLongRectToMapRect (
																	mapProjectionInfoPtr, &lBoundingRect);
								mapUnitsFlag = FALSE;
								
								}	// end "else if (shapeInfoPtr->conversionCode == 2"
								
							}	// end "if (mapInfoSameFlag)"
						
								// Now verify that the bounding area of the shape file
								// intersects with that for the active image window.  If not
								// then continue checking for other shape files.
							
						if (convertedFlag &&
								WindowBoundingAreaAndRectIntersect (gActiveImageWindow,
																				boundingRectPtr,
																				mapUnitsFlag))
							{
									// Check for situation when the image and vector bounding rects
									// intersect but it may very well be that the vector is lat/long
									// and the image map units are not in lat/long.
							
							if (!mayBeLatLongUnitsFlag ||
										referenceSystemCode == kGeographicRSCode ||
												shapeInfoPtr->conversionCode == 2)
								{
								alreadyLoadedFlag = TRUE;
								overlayNumber = index + 1;
								conversionCode = shapeInfoPtr->conversionCode;
								break;
								
								}	// end "if (!mayBeLatLongUnitsFlag || ..."
							
							}	// end "if (WindowBoundingAreaAndRectIntersect (..."
						
						}	// end "if (mapProjectionInfoPtr != NULL)"
					
					}	// end "if (stringCompare == 0)"
				
				CheckAndUnlockHandle (shapeHandlePtr[index]);
				
				}	// end "if (shapeInfoPtr != NULL)"
											
			}	// end "for (index=0; index<gShapeHandleListLength; index++)"
			
				// The overlay file is already loaded. Now check to see if it needs
				// to be added to the active image window.
			
		if (alreadyLoadedFlag)
			{
			if (conversionCode == 2 && mapUnitsFlag)
				ConvertLatLongRectToMapRect (mapProjectionInfoPtr, boundingRectPtr);
															
			overlayCheckReturn = AddToWindowOverlayList (gActiveImageWindow,
																			boundingRectPtr,
																			overlayNumber,
																			mapUnitsFlag);
																			
			}	// end "if (alreadyLoadedFlag)"
																
		if (overlayCheckReturn == 2)
			{
				// Check if shape file needs to be reloaded.
				// Note that this check is done with native units of the windows. 
				// So will need to make sure that the bounding units for the
				// shape file is in the same units which is native to the windows.
				
			overlayCheckReturn = CheckIfOverlayNeedsReloaded (gActiveImageWindow,
																				(SInt16)overlayNumber,
																				&lBoundingRect);
																				
			if (overlayCheckReturn == 0)
				{
						// Release memory for current shape information and set up
						// to reload it.
						
				ReleaseShapeFileMemory (shapeHandlePtr, 
												(SInt16)(overlayNumber-1),
												kAllShapeHandleMemory);
													
				if (gNumberShapeFiles > 0)
					gNumberShapeFiles--;
				
				}	// end "if (overlayCheckReturn == 0)"
																				
			}	// end "if (overlayCheckReturn == 2)"
			
		CheckAndUnlockHandle (mapProjectionHandle);
													
		}	// end "if (shapeHandlePtr != NULL)"
		
	if (overlayCheckReturn == 0)
		{
				// Verify that shape file intersects with the active image window if
				// there is an image window to compare to.
		
		if (!WindowBoundingAreaAndRectIntersect (gActiveImageWindow,
																	boundingRectPtr,
																	mapUnitsFlag))
			{
			if (gActiveImageWindow != NULL)
				overlayCheckReturn = 5;
			
			}	// end "if (!WindowBoundingAreaAndRectIntersect (..."
			
		else	// WindowBoundingAreaAndRectIntersect (...
			{
					// Check for situation when the image and vector bounding rects
					// intersect but it may very well be that the vector is lat/long
					// and the image map units are not in lat/long.
					
			referenceSystemCode = GetFileReferenceSystemCode (gActiveImageWindowInfoH);
			if (mayBeLatLongUnitsFlag && referenceSystemCode != kGeographicRSCode)
				overlayCheckReturn = 10;
				
			}	// end "else WindowBoundingAreaAndRectIntersect (..."
		
		if (overlayCheckReturn == 5 && conversionCode == 0)	
			{
					// Check if converting units from latitude-longitude to map
					// units may help.
				
			if (mayBeLatLongUnitsFlag)
				overlayCheckReturn = 10;
			
			}	// end "if (overlayCheckReturn == 5&& ..."
			
		if (overlayCheckReturn == 0 && conversionCode == 2)
			overlayCheckReturn = 10;
									
		}	// end "if (overlayCheckReturn == 0)"
			
	CheckAndUnlockHandle (gShapeFilesHandle);
	
	*overlayNumberPtr = (SInt16)overlayNumber;
	
	if ((overlayCheckReturn == 0 || overlayCheckReturn == 10) && versionCount > 1)
		*versionNumberLoadedPtr = versionCount;
	
	return (overlayCheckReturn);
	
}	// end "CheckIfOverlayFileLoaded"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfVectorOverlaysIntersectImage
//
//	Software purpose:	This routine checks if the input overlay file is already loaded
//							into memory.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/06/2006
//	Revised By:			Larry L. Biehl			Date: 12/12/2008

Boolean CheckIfVectorOverlaysIntersectImage (
				WindowInfoPtr						windowInfoPtr)

{
	DoubleRect							lBoundingRect;
											
	Handle*								shapeHandlePtr;
	MapProjectionInfoPtr 			mapProjectionInfoPtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	Handle 								mapProjectionHandle;	
	
	UInt32								index;
	
	SInt16								conversionCode;
	
	Boolean								convertedFlag,
											mapInfoSameFlag,
											mapUnitsFlag,
											vectorOverlayIntersectsFlag;
											
	
			// Verify that shape files do exist.
			
	if (gNumberShapeFiles <= 0)
																					return (FALSE);
	
			// If there is no valid map information for the image, then no
			// shape files will intersect with the image.
			
	if (!FindIfMapInformationExists (windowInfoPtr))
																					return (FALSE);
	
	vectorOverlayIntersectsFlag = FALSE;
	conversionCode = 0;
	mapUnitsFlag = TRUE;		
		
	shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, kLock);

	if (shapeHandlePtr != NULL) 
		{
				// Get the map projection handle in case it is needed.
				
		mapProjectionHandle = GetFileMapProjectionHandle2 (
															windowInfoPtr->windowInfoHandle);

		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	mapProjectionHandle, kLock);
													
				// Loop through existing overlay files and check if the shape file
				// intesects with the input image.
	
		for (index=0; index<gShapeHandleListLength; index++)
			{
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
															shapeHandlePtr[index],
															kLock);
															
			if (shapeInfoPtr != NULL)
				{	
						// First verify that the grid coordinate system and the 
						// geodetic information for the image window match that
						// for the vector file.  	

				mapInfoSameFlag = CheckIfMapInfoMatches (
															&shapeInfoPtr->geodetic,
															&mapProjectionInfoPtr->geodetic,
															&shapeInfoPtr->gridCoordinate,
															&mapProjectionInfoPtr->gridCoordinate);
				
				if (mapInfoSameFlag)
					{			
							// If the input shape file needs to be converted from lat-
							// long units to the map units of the image, then be sure that
							// the conversion can be made.
						
					convertedFlag = TRUE;
					mapUnitsFlag = TRUE;		
					lBoundingRect = shapeInfoPtr->boundingXYBox;
					if (shapeInfoPtr->conversionCode == 2)
						{
						convertedFlag = ConvertLatLongRectToMapRect (
													mapProjectionInfoPtr, 
													&lBoundingRect);
						mapUnitsFlag = FALSE;	
						
						}	// end "if (shapeInfoPtr->conversionCode == 2)"
					
							// Check if the shape file intersects with the image.
							
					if (convertedFlag &&	
							WindowBoundingAreaAndRectIntersect (windowInfoPtr,
																			&shapeInfoPtr->boundingXYBox,
																			mapUnitsFlag))
						vectorOverlayIntersectsFlag = TRUE;
					
					CheckAndUnlockHandle (shapeHandlePtr[index]);
					
					if (vectorOverlayIntersectsFlag)
						break;
						
					}	// end "if (mapInfoSameFlag)"
				
				}	// end "if (shapeInfoPtr != NULL)"
											
			}	// end "for (index=0; index<gShapeHandleListLength; index++)"
			
		CheckAndUnlockHandle (mapProjectionHandle);
			
		CheckAndUnlockHandle (gShapeFilesHandle);
	
		}	// end "if (shapeHandlePtr != NULL)"
	
	return (vectorOverlayIntersectsFlag);
	
}	// end "CheckIfVectorOverlaysIntersectImage"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfOverlayMayBeLatLong
//
//	Software purpose:	This routine determines if it is possible for the input shape 
//							file may be in lat-long units. It does this by first checking 
//							for an associated .prj file. If a .prj file is not found then,
//							it does this by checking the extent of the bounding rectangle and 
//							checking if it fits with the extent of latitude-longitude units. 
//							Cases have been found when one of the bounds is out of range. It
//							is difficult to determine if this case really means lat-long or
//							whether it is just the way the map units are for the shape file.
//							Best is to have a .prj file provided.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	
//
// Called By:			CheckIfOverlayFileLoaded in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2005
//	Revised By:			Larry L. Biehl			Date: 01/02/2013

Boolean CheckIfOverlayMayBeLatLong (
				CMFileStream						*shapeFileStreamPtr,
				DoubleRect*							boundingOverlayRectPtr)

{
	SInt16								latLongCode = -1;
	
	Boolean								mayBeLatLongUnitsFlag = FALSE;
	
	
	#if include_gdal_capability
				// If gdal capability exists, then use this library to get information
				// from an associated prj file.
				
		latLongCode = CheckIfprjFileDefinesLatLong (shapeFileStreamPtr);
		if (latLongCode == 1)
			mayBeLatLongUnitsFlag = TRUE;				
	#endif	// include_gdal_capability
	
	if (latLongCode == -1)
		{
				// Base the determination on the bounding overlay rectangle.
				
		if (boundingOverlayRectPtr->left >= -180 &&
				boundingOverlayRectPtr->right <= 180 &&
				boundingOverlayRectPtr->top >= -90 &&
				boundingOverlayRectPtr->bottom <= 90)
			mayBeLatLongUnitsFlag = TRUE;
			
		}	// end "if (latLongCode == -1)"
		
	return (mayBeLatLongUnitsFlag);
	
}	// end "CheckIfOverlayMayBeLatLong"


#if include_gdal_capability
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckIfprjFileDefinesLatLong
//
//	Software purpose:	This routine determines if a prj file is associated with the 
//							input shape file and if so then determines whether the units
//							for the shape file are in lat-long degree units. The gdal library
//							is required for this determination.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	-1: Indicates that no prj file existed and therefore do no know.
//							0: Indicates that the shape file is not in lat-long units
//							1: Indicates that the shape file is in lat-long units.
//
// Called By:			CheckIfOverlayMayBeLatLong in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/02/2013
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 CheckIfprjFileDefinesLatLong (
				CMFileStream						*shapeFileStreamPtr)

{
	CMFileStream						prjFileStream;
	
	char									prjString[512];
	
	FileStringPtr						prjFileNamePtr;
	
	CMFileStream						*prjFileStreamPtr = NULL;
	
	UInt32								count;
	
	SInt16								errCode,
											geogcsCode,
											latLongCode = -1,
											projcsCode,
											unitsCode;

	
			// Get copy of the file stream structure to use for the prj file.
			
	prjFileStreamPtr = &prjFileStream;		
	InitializeFileStream (prjFileStreamPtr, shapeFileStreamPtr);
	prjFileNamePtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (prjFileStreamPtr);
	
			// Remove the '.shp' suffix at the end of the file name
			// and then look for a corresponding '.prj' file.
		
 	RemoveCharsNoCase ((char*)"\0.???\0", prjFileNamePtr);
	
	ConcatFilenameSuffix (prjFileNamePtr, (StringPtr)"\0.prj\0");

			// This will be an attempt to read a file with the prj suffix.
	
	errCode = OpenFileReadOnly (prjFileStreamPtr, 
												kResolveAliasChains, 
												kLockFile,
												kVerifyFileStream);

	count = 500;
	if (errCode == noErr)
		{
		errCode = MReadData (prjFileStreamPtr, 
										&count, 
										&prjString[0], 
										kNoErrorMessages);
										
		if (errCode == eofErr && count >= 10)
			errCode = noErr;
			
		CloseFile (prjFileStreamPtr);
		
		if (errCode == noErr)
			{	
			prjString[count] = 0;
			
			if (ReadPRJFileInformation (prjString, &projcsCode, &geogcsCode, &unitsCode))
				{
				if (projcsCode == 1)
					latLongCode = 0;
					
				else if (projcsCode == 0 &&
								geogcsCode == 1 &&
										unitsCode == kAVDecimalDegreesCode)
					latLongCode = 1;
					
				}	// end "if (ReadPRJFileInformation (..."
			
			}	// end "if (errCode == noErr)"
		
		}	// end "if (errCode == noErr)"
		
	return (latLongCode);		
	
}	// end "CheckIfprjFileDefinesLatLong"
#endif	// include_gdal_capability


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CheckIfOverlayNeedsReloaded
//
//	Software purpose:	This routine determines if the input shape file needs to be
//							reloaded based on the image map limits of the new image window
//							that it is being assigned to. The shape file will be reloaded
//							for all cases accept when the map limits for the new image window
//							are within the map limits for one of the current image windows.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	0: indicates that vector overlay needs to be reloaded
//							2: indicates that vector overlay does not need to be reloaded
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/01/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

SInt16 CheckIfOverlayNeedsReloaded (
				WindowPtr							newWindow,
				SInt16								overlayNumber,
				DoubleRect*							boundingRectPtr)

{
	DoubleRect							newMapRectangle;
	
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;

	UInt32								index;
	
	SInt16								overlayCheckReturn = 0,
											windowCount,
											windowListIndex;
											
	Boolean								needToLoadFlag;
	

			// Get the map overage of the new image window.
			
	windowInfoHandle = GetWindowInfoHandle (newWindow);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
	newMapRectangle = windowInfoPtr->boundingMapRectangle;
										
			// Now check if the map rectangle falls within any of the
			// image windows that vector file has already been loaded
			// for.

	windowCount = 0;
	windowListIndex = kImageWindowStart;
	needToLoadFlag = TRUE;
		
	do
		{
		windowPtr = gWindowList[windowListIndex]; 
		
		if (windowPtr != newWindow)
			{       
			windowInfoHandle = GetWindowInfoHandle (windowPtr);
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
												
					// Now check if the overlay is already included in the list.
					
			for (index=0; index<windowInfoPtr->numberVectorOverlays; index++)
				{
				if (abs (windowInfoPtr->overlayList[index].index) == overlayNumber)
					{
					needToLoadFlag = FALSE;

							// Now check if any of the new image is outside of this
							// image window.
										
					if (newMapRectangle.left < windowInfoPtr->boundingMapRectangle.left &&
							boundingRectPtr->left < windowInfoPtr->boundingMapRectangle.left)
						needToLoadFlag = TRUE;
						
					else if (newMapRectangle.top <
												windowInfoPtr->boundingMapRectangle.top &&
								boundingRectPtr->top <
												windowInfoPtr->boundingMapRectangle.top)
						needToLoadFlag = TRUE;
						
					else if (newMapRectangle.right >
												windowInfoPtr->boundingMapRectangle.right &&
									boundingRectPtr->right >
												windowInfoPtr->boundingMapRectangle.right)
						needToLoadFlag = TRUE;
						
					else if (newMapRectangle.bottom > windowInfoPtr->boundingMapRectangle.bottom &&
								boundingRectPtr->bottom > windowInfoPtr->boundingMapRectangle.bottom)
						needToLoadFlag = TRUE;
					/*
					overallMapRectangle.left = MIN (overallMapRectangle.left,
													windowInfoPtr->boundingMapRectangle.left);
				
					overallMapRectangle.right = MAX (overallMapRectangle.right,
													windowInfoPtr->boundingMapRectangle.right);
				
					overallMapRectangle.top = MIN (overallMapRectangle.top,
													windowInfoPtr->boundingMapRectangle.top);
				
					overallMapRectangle.bottom = MAX (overallMapRectangle.bottom,
													windowInfoPtr->boundingMapRectangle.bottom);
					*/
					}	// end "if (abs (windowInfoPtr->overlayList[index].index) == ..."
					
				if (!needToLoadFlag)
					break;
				
				}	// end "for (index=0; index<windowInfoPtr->numberVectorOverlays; ..."
					
			if (!needToLoadFlag)
				break;
				
			}	// end "if (windowPtr != newWindow)"
		
		windowListIndex++;
		windowCount++;
													
		}	while (windowCount<gNumberOfIWindows);
	/*	
			// Now determine if the new overall size would include more vectors
			// from the shape file.
			
	windowInfoHandle = GetWindowInfoHandle (newWindow);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr->boundingMapRectangle.left < overallMapRectangle.left &&
											boundingRectPtr->left < overallMapRectangle.left)
		overlayCheckReturn = 0;
		
	else if (windowInfoPtr->boundingMapRectangle.top < overallMapRectangle.top &&
											boundingRectPtr->top < overallMapRectangle.top)
		overlayCheckReturn = 0;		
		
	else if (windowInfoPtr->boundingMapRectangle.right > overallMapRectangle.right &&
											boundingRectPtr->right > overallMapRectangle.right)
		overlayCheckReturn = 0;						
		
	else if (windowInfoPtr->boundingMapRectangle.bottom > overallMapRectangle.bottom &&
											boundingRectPtr->bottom > overallMapRectangle.bottom)
		overlayCheckReturn = 0;	
	*/
	if (!needToLoadFlag)
		overlayCheckReturn = 2;

	return (overlayCheckReturn);
	
}	// end "CheckIfOverlayNeedsReloaded"	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseAllVectorOverlayFiles
//
//	Software purpose:	This routine closes all of the overlay files and releases the
//							memory associated with them.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/28/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2003

void CloseAllVectorOverlayFiles (void)

{	
	UInt32								index;
	
					
			// Dispose of the overlays
	
	for (index=0; index<gShapeHandleListLength; index++)
		{
		CloseOverlayFile (index);
		
		}	// end "for (index=0; index<gShapeHandleListLength; index++)"
		
	gShapeFilesHandle = UnlockAndDispose (gShapeFilesHandle);
	gShapeHandleListLength = 0;
	gNumberShapeFiles = 0;
	
}	// end "CloseAllVectorOverlayFiles"		



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseOverlayFile
//
//	Software purpose:	This routine determines if the input bounding area intersects
//							with any of the window area.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	True, if file was read in okay
//							Fale, if file was not read in okay
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/10/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

void CloseOverlayFile (
				UInt32			 					overlayIndex)

{
	Handle*								shapeHandlePtr;
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;

	UInt32								overlayListIndex;
	
	SInt16								windowCount,
											windowListIndex;
	
	Boolean								moveFlag;
	
	
	if (overlayIndex < gShapeHandleListLength)
		{
		shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, 
																	kLock);
																	
		if (shapeHandlePtr != NULL)
			{												
			ReleaseShapeFileMemory (shapeHandlePtr, 
											(SInt16)overlayIndex,
											kAllShapeHandleMemory);
				
					// Now search through all of the image windows and removed this
					//	overlay from the window overlay list. If the overlay file was
					// included in the list compact the list. Also if the overlay file
					// was included make sure that the window is invalidated to force
					// a redraw if the overlay was being shown at the time.	
					
			windowCount = 0;
			windowListIndex = kImageWindowStart;
			
			do
				{
				windowPtr = gWindowList[windowListIndex];         
				windowInfoHandle = GetWindowInfoHandle (windowPtr);
				windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																windowInfoHandle, kLock);
			
				if (windowInfoPtr != NULL)
					{
					overlayListIndex = 0;
					moveFlag = FALSE;
					while (overlayListIndex < windowInfoPtr->numberVectorOverlays)
						{
						if (abs (windowInfoPtr->overlayList[overlayListIndex].index) == 
																				(UInt8)(overlayIndex + 1))
							{
							if (windowInfoPtr->overlayList[overlayListIndex].index > 0)
								InvalidateWindow (windowPtr, 
															kImageFrameArea, 
															FALSE);
								
							moveFlag = TRUE;
							
							}	// end "if (abs (...->overlayList[overlayListIndex]..."
							
						if (moveFlag && overlayListIndex+1 < windowInfoPtr->numberVectorOverlays)
							windowInfoPtr->overlayList[overlayListIndex] = 
												windowInfoPtr->overlayList[overlayListIndex+1];
						
						overlayListIndex++;
						
						}	// end "while (overlayListIndex < windowInfoPtr->numberVectorOverlays)"
						
					if (moveFlag)
						{
						windowInfoPtr->numberVectorOverlays--;
						UpdateOverlayControl (windowPtr);
						
						}	// end "if (moveFlag)"
						
					}	// end "if (windowInfoPtr != NULL)"
					
				CheckAndUnlockHandle (windowInfoHandle);
				
				windowListIndex++;
				windowCount++;
															
				}	while (windowCount<gNumberOfIWindows);
				
			if (gNumberShapeFiles > 0)
				gNumberShapeFiles--;
			
			}	// end "if (shapeInfoPtr != NULL)"
			
		CheckAndUnlockHandle (gShapeFilesHandle);
			
		}	// end "if (overlayIndex < gShapeHandleListLength)"
	
}	// end "CloseOverlayFile"		



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClearVectorOverlay
//
//	Software purpose:	This routine closes the overlay file as requested from the
//							selected clear overlay submenu item.
//
//	Parameters in:		menu item index
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/28/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2003

void ClearVectorOverlay (
				UInt32			 					menuItemIndex)

{
	Handle*								shapeHandlePtr;
	
	SInt32								selectedOverlay;
	
	UInt32								count,
											index;
	
	
	if (menuItemIndex <= gShapeHandleListLength)
		{
		shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle);
		
		count = 0;
		selectedOverlay = -1;															
		for (index=0; index<gShapeHandleListLength; index++)
			{
			if (shapeHandlePtr[index] != NULL)
				{
				count++;
				
				if (count == menuItemIndex)
					{
					selectedOverlay = index;
					break;
					
					}	// end "if (count == menuItemIndex)"
					
				}	// end "if (shapeHandlePtr[index] != NULL)"
				
			}	// end "for (index=0; index<gShapeHandleListLength; index++)"
		
		if (selectedOverlay >= 0)
			CloseOverlayFile (selectedOverlay);
																	
		}	// end "if (menuItemIndex <= gShapeHandleListLength)"
	
}	// end "ClearVectorOverlay"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ConvertLatLongPointToMapPoint
//
//	Software purpose:	This routine converts the input lat-long coordinates to 
//							map coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2005
//	Revised By:			Larry L. Biehl			Date: 02/22/2007		

void ConvertLatLongPointToMapPoint (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				ArcViewDoublePointPtr			sdoubleCoordinatePointPtr)

{		
	DoublePoint							coordinatePoint;
	
	
			// We need to do this in case the machine is a pre-powerpc Mac computer.
			// The double on this machines was 80 bits not 64 bits.
													
	coordinatePoint.v = sdoubleCoordinatePointPtr->y;
	coordinatePoint.h = sdoubleCoordinatePointPtr->x;

	ConvertLatLongPointToMapPoint (mapProjectionInfoPtr, &coordinatePoint);
													
	sdoubleCoordinatePointPtr->y = coordinatePoint.v;
	sdoubleCoordinatePointPtr->x = coordinatePoint.h;

}	// end "ConvertLatLongPointToMapPoint" 


/*
// This routine is now left out since code is now not being generated for pre-powerpc
// processors
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ConvertLatLongRectToMapRect
//
//	Software purpose:	This routine converts the input lat-long coordinates to 
//							map coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2005
//	Revised By:			Larry L. Biehl			Date: 04/28/2012		

Boolean ConvertLatLongRectToMapRect (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				SDoubleRect*						sdoubleCoordinateRectanglePtr)

{		
	DoubleRect							coordinateRectangle;
	
	Boolean								convertedFlag;
	
	
			// We need to do this in case the machine is a pre-powerpc Mac computer.
			// The double on this machines was 80 bits not 64 bits.
													
	coordinateRectangle.top = sdoubleCoordinateRectanglePtr->top;
	coordinateRectangle.left = sdoubleCoordinateRectanglePtr->left;
	coordinateRectangle.bottom = sdoubleCoordinateRectanglePtr->bottom;
	coordinateRectangle.right = sdoubleCoordinateRectanglePtr->right;

	convertedFlag = ConvertLatLongRectToMapRect (mapProjectionInfoPtr, &coordinateRectangle);
	
													
	sdoubleCoordinateRectanglePtr->top = coordinateRectangle.top;
	sdoubleCoordinateRectanglePtr->left = coordinateRectangle.left;
	sdoubleCoordinateRectanglePtr->bottom = coordinateRectangle.bottom;
	sdoubleCoordinateRectanglePtr->right = coordinateRectangle.right;
	
	return (convertedFlag);

}	// end "ConvertLatLongRectToMapRect" 
*/


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateCLRSupportFile
//
//	Software purpose:	This routine creates a clr file. 
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	TRUE if completed okay.
//
// Called By:			CreateThematicSupportFile in SFileIO.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 05/15/2011
//	Revised By:			Larry L. Biehl			Date: 05/16/2018

Boolean CreateCLRSupportFile (
				CMFileStream*						trailerStreamPtr, 
				UInt16								numberSupportFileClasses, 
				RGBCharColorPtr					classColorTablePtr,
				char*									classNameTablePtr,
				UInt16								numberGroups,
				SInt16*								classToGroupPtr,
				RGBCharColorPtr					groupColorTablePtr,
				char*									groupNamePtr,
				Boolean*								listAllGroupInfoPtr,
				UInt16*								classSymbolPtr,
				UInt16								numberClassSymbols,
				Boolean								asciiSymbolsFlag)

{
	char									groupName[48];
	
	char*									localGroupNamePtr;
	HFileIOBufferPtr					ioBufferPtr;
										
	UCharPtr								ioTempBufferPtr;
									
	SInt32								classNumber;
	
	UInt32								count,
											numberBytes;
	
	SInt16								classValue,
											errCode,
											groupNumber,
											stringLength;
											
	UInt16								classSymbolIndex;
										
	Boolean								continueFlag;
	
	
	if (asciiSymbolsFlag && classSymbolPtr == NULL)
																				return (FALSE);
	
	if (asciiSymbolsFlag)
		numberSupportFileClasses = numberClassSymbols;
	
			// Do not create a trailer file if there are more than the class		
			// limit.																				
			
	if (numberSupportFileClasses > gClassListLimit)
																				return (TRUE);

			// Get buffer to load clr file information into.  Try to get 		
			// buffer to hold one complete set of the 3 types of information -	
			// color scheme, class histogram, and class name. Note that even
			// though we are limited to colors for only 256 classes, we can
			// have class names for up to 'gClassListLimit' classes.			
			
	numberBytes = (numberSupportFileClasses+1) * 256;
	ioBufferPtr = (HFileIOBufferPtr)MNewPointer (numberBytes);
	continueFlag = (ioBufferPtr != NULL);
			
	if (continueFlag)
		{
		ioTempBufferPtr = (UCharPtr)ioBufferPtr;
		
				// Now load the colors, class names and group information if it exists
		
		classSymbolIndex = 0;
		for (classNumber=0; classNumber<numberSupportFileClasses; classNumber++)
			{
			if (classSymbolPtr == NULL || 
						asciiSymbolsFlag ||
								classNumber == classSymbolPtr[classSymbolIndex])
				{
						// Only list those classes which are included in the class symbol
						// list. These are the ones that are actually included in the file.
						// Take into account if this is actually a ascii symbol
						// classification file.
						
				classValue = (SInt16)classNumber;
				if (asciiSymbolsFlag)
					classValue = classSymbolPtr[classSymbolIndex];
						
				groupNumber = -1;
				if (numberGroups > 0)
					{
					groupNumber = classToGroupPtr[classSymbolIndex];
					
					if (groupNumber >= 0)
						{
						if (listAllGroupInfoPtr[classNumber])
							{
							localGroupNamePtr = &groupNamePtr[groupNumber*32];
							BlockMoveData (&localGroupNamePtr[1], 
													groupName, localGroupNamePtr[0]);
							groupName[(int)localGroupNamePtr[0]] = 0;
							
							stringLength = sprintf (
												(char*)ioTempBufferPtr,
												"%d %3d %3d %3d n:%s~ g:%d %3d %3d %3d %s~%s",
												classValue,
												classColorTablePtr->red,
												classColorTablePtr->green,
												classColorTablePtr->blue,
												classNameTablePtr,
												groupNumber,
												groupColorTablePtr[groupNumber].red,
												groupColorTablePtr[groupNumber].green,
												groupColorTablePtr[groupNumber].blue,
												groupName,
												gEndOfLine);
							
							}	// end "if (listAllGroupInfoPtr[classNumber])"
						
						else	// !listAllGroupInfoPtr[classNumber]
							stringLength = sprintf ((char*)ioTempBufferPtr, 
															"%d %3d %3d %3d n:%s~ g:%d%s",
															classValue,
															classColorTablePtr->red,
															classColorTablePtr->green,
															classColorTablePtr->blue,
															classNameTablePtr,
															groupNumber,
															gEndOfLine);
													
						}	// end "if (groupNumber >= 0)"
													
					}	// end "if (numberGroups > 0)"
					
				if (groupNumber < 0)
					stringLength = sprintf ((char*)ioTempBufferPtr, 
													"%d %3d %3d %3d n:%s~%s",
													classValue,
													classColorTablePtr->red,
													classColorTablePtr->green,
													classColorTablePtr->blue,
													classNameTablePtr,
													gEndOfLine);
				
				if (classSymbolIndex < numberClassSymbols-1)
					classSymbolIndex++;	
									
				ioTempBufferPtr += stringLength;
																						
				}	// end "if (classSymbolPtr == NULL || classNumber == ..."
			
			classColorTablePtr++;
			classNameTablePtr += 32;			
											 
			}	// end "for (classNumber=0; classNumber<...)"
			
				// Write buffer of data	to trailer file.								
					
		count = (SInt32)(ioTempBufferPtr - (StringPtr)ioBufferPtr);
		count = MIN (count, numberBytes);
		errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
						
		}	// end "if (continueFlag && errCode == noErr) 
	
			// Unlock dispose of handles to temporary buffer.							
				
	CheckAndDisposePtr ((Ptr)ioBufferPtr);
	
	return (continueFlag && errCode == noErr);

}	// end "CreateCLRSupportFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DisplayNoIntersectionAlert
//
//	Software purpose:	The purpose of this routine is to display the no intersection
//							for the image window and the overlay file alert.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/10/2001
//	Revised By:			Larry L. Biehl			Date: 02/28/2019

void DisplayNoIntersectionAlert (
				SInt16								stringNumber)
				
{  
	SInt16								stringLength,
											stringStart;
	
			
	MGetString (gTextString3, kAlertStrID, stringNumber);
	stringLength = gTextString3[0];
	
	MGetString (&gTextString3[stringLength+1],
					kAlertStrID, IDS_Alert87,
					254-stringLength);
	stringLength += gTextString3[stringLength+1];
	gTextString3[stringLength+1] = 0;
	
			// Remove the character count at the beginning of the last string.
			
	gTextString3[gTextString3[0]+1] = ' ';
	
			// Now remove beginning carriage return and 2 spaces and get the string
			// length.
			
	stringStart = gNumberOfEndOfLineCharacters + 2;
	stringLength -= stringStart;
	
	gTextString3[stringStart] = (UInt8)stringLength;
	
			// Display an alert indicating that the input shape file does not
			// intersect with any of the open image windows. The file will
			// not be used.													
	
	DisplayAlert (kErrorAlertID, 
						kStopAlert, 
						0, 
						0,
						0, 
						&gTextString3[stringStart]);
	
}	// end "DisplayNoIntersectionAlert"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoShowOverlaySelection
//
//	Software purpose:	The purpose of this routine is to handle the selection that
//							the user made in the show overlay popup menu.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			OnToolBarOverlaySelection in xMainFrame.cpp & WMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2020

void DoShowOverlaySelection (
				WindowPtr							windowPtr,
				Handle								windowInfoHandle,
				SInt16								selection,
				Boolean								optionKeyFlag,
				Boolean								shiftKeyFlag)
				
{  
	OverlaySpecsPtr					overlayListPtr;
	ImageOverlaySpecsPtr				imageOverlayListPtr;
	WindowInfoPtr						windowInfoPtr;
	
	UInt32								index,
											numberImageOverlays,
											numberVectorOverlays;
											
	Boolean								callFlag,
											invalidateFlag;
											
	SignedByte							windowHandleStatus;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
													windowInfoHandle, &windowHandleStatus);

	if (windowInfoPtr != NULL) 
		{
		callFlag = FALSE;
		
				// Now get a pointer to the overlay list and the number of overlays.
							
		numberVectorOverlays = windowInfoPtr->numberVectorOverlays;
		overlayListPtr = windowInfoPtr->overlayList;
							
		numberImageOverlays = windowInfoPtr->numberImageOverlays;
		imageOverlayListPtr = windowInfoPtr->imageOverlayList;
		
		if (selection == 1)
			{
					// Hide all image overlays
					
			callFlag = HideAllImageOverlays (imageOverlayListPtr,
															numberImageOverlays);
				
					// Hide all vector overlays
					
			for (index=0; index<numberVectorOverlays; index++)
				{
				if (overlayListPtr[index].index > 0)
					{
					overlayListPtr[index].index = -abs (overlayListPtr[index].index);
					callFlag = TRUE;
					
					}	// end "if (overlayListPtr[index].index > 0)"
					
				}	// end "for (index=0; index<numberVectorOverlays; index++)"
			
			windowInfoPtr->drawBaseImageFlag = TRUE;
				
			invalidateFlag = TRUE;
			
			}	// end "if (selection == 1)"
			
		else if (selection == 2)
			{
					// Show all image overlays
					
			for (index=0; index<numberImageOverlays; index++)
				{
				if (imageOverlayListPtr[index].index < 0)
					{
					imageOverlayListPtr[index].index = 
														abs (imageOverlayListPtr[index].index);
					callFlag = TRUE;
					
					}	// end "if (overlayListPtr[index].index > 0)"
					
				}	// end "for (index=0; index<numberImageOverlays; index++)"
			
			windowInfoPtr->drawBaseImageFlag = TRUE;
				
					// Show all vector overlays
					
			for (index=0; index<numberVectorOverlays; index++)
				{
				if (overlayListPtr[index].index < 0)
					{
					overlayListPtr[index].index = abs (overlayListPtr[index].index);
					callFlag = TRUE;
					
					}	// end "if (overlayListPtr[index].index > 0)"
					
				}	// end "for (index=0; index<numberVectorOverlays; index++)"
				
			#if defined multispec_mac
				invalidateFlag = FALSE;
				if (numberImageOverlays > 0) 	
					invalidateFlag = TRUE;         
			#endif	// defined multispec_mac
				
			#if defined multispec_win || defined multispec_wx
				invalidateFlag = TRUE;          
			#endif	// defined multispec_win || defined multispec_wx
			
			}	// end "else if (selection == 2)"
			
		else	// selection > 2
			{
			index = selection - 4;
			
			if (index < numberImageOverlays)
				{
						// Set hide/show for the requested image overlay.
			
				if (optionKeyFlag)
					{
					imageOverlayListPtr[index].index = 
															abs (imageOverlayListPtr[index].index);

					invalidateFlag = TRUE; 
					callFlag = TRUE; 
						
					
					}	// end "if (optionKeyFlag)"
				
				else	// !optionKeyFlag
					{
					imageOverlayListPtr[index].index = 
														-1 * imageOverlayListPtr[index].index;
				
					invalidateFlag = TRUE;
					
					callFlag = TRUE;
					
					}	// end "else !optionKeyFlag"
				
				if (imageOverlayListPtr[index].index > 0)
					{
					windowInfoPtr->drawBaseImageFlag = GetDrawBaseImageFlag (
																imageOverlayListPtr[index].index - 1);
					
					if (!shiftKeyFlag)
						{
							// Hide all of the other image overlays unless he shift key
							// is down.
							
						HideAllImageOverlays (imageOverlayListPtr,
														numberImageOverlays);
														
						imageOverlayListPtr[index].index =  
																abs (imageOverlayListPtr[index].index);
																
						}	// end "if (imageOverlayListPtr[index].index > 0&& ..."
					
					}	// end "if (!shiftKeyFlag)"
				
				else	// imageOverlayListPtr[index].index <= 0)
					windowInfoPtr->drawBaseImageFlag = TRUE;
					
				}	// end "if (index < numberImageOverlays)"
				
			else	// index >= numberImageOverlays
				{
				index -= numberImageOverlays;
				
						// Set hide/show for the requested vector overlay.
			
				if (optionKeyFlag)
					{
					overlayListPtr[index].index = abs (overlayListPtr[index].index);
					
					invalidateFlag = TRUE; 
					callFlag = TRUE;   
					
					}	// end "if (optionKeyFlag)"
				
				else	// !optionKeyFlag
					{
					overlayListPtr[index].index = -1 * overlayListPtr[index].index;
				
					invalidateFlag = TRUE;
					#if defined multispec_mac
						if (overlayListPtr[index].index > 0)
							invalidateFlag = FALSE;         
					#endif	// defined multispec_mac
					
					callFlag = TRUE;
					
					}	// end "else !optionKeyFlag"
					
				}	// end "if (index < numberImageOverlays)"
			
			}	// end "else selection > 2"
		
		if (invalidateFlag && callFlag)	
			InvalidateWindow (windowPtr, kImageFrameArea, FALSE);
			
		else if (!invalidateFlag && callFlag && numberVectorOverlays > 0)
			DrawArcViewShapes (windowPtr,
										windowInfoHandle,
										NULL,
										kToImageWindow,
										NULL);
		
		MHSetState (windowInfoHandle, windowHandleStatus);
		
		}	// end "if (windowInfoPtr != NULL)"	
	
}	// end "DoShowOverlaySelection"	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawArcViewShapes
//
//	Software purpose:	The purpose of this routine is to return the pointer
//							to the file stream structure or class.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			CopyOffScreenImage in xUtilities.cpp and WUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2000
//	Revised By:			Larry L. Biehl			Date: 02/28/2020

void DrawArcViewShapes (
				WindowPtr							windowPtr,
				Handle								windowInfoHandle,
				Rect*									inputBoundingRectPtr,
				SInt16								windowCode,
				CGContextRef						context)
				
{ 					
	#if defined multispec_win
		CPen									overlayPen;
		CPen*									overlayPenPtr = NULL;
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		wxPen									overlayPen;
		wxPen*								overlayPenPtr = NULL;
	
		double								xScale,
												yScale;
	#endif	// defined multispec_win
				
	DoubleRect							boundingWindowBox,
											polyLineBox;
	
	LongPoint							lastPoint,
											nextPoint;
											
	Rect									clipRect;
											
	MapToWindowUnitsVariables		mapToWindowUnitsVariables;
											
	#if defined multispec_mac
		Pattern								black;
	#endif	// defined multispec_mac
	
	ArcViewDoublePoint*				arcViewDoublePointPtr;
	ArcViewPointPtr					arcViewPointPtr;
	ArcViewPolyLinePtr				arcViewPolyLinePtr;
	ArcViewRecordHeaderPtr			arcViewRecordHeaderPtr;
	GrafPtr								savedPort;
	Handle*								shapeHandlePtr;
	OverlaySpecsPtr					overlayListPtr;
	Ptr									vectorDataPtr;
	ShapeInfoPtr						shapeInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	SInt32								shapeFileIndex;
	
	UInt32								numberOverlays,
											overlayIndex,
											partIndex,
											pointIndex,
											pointStart,
											pointStop,
											recordIndex,
											vectorDataIndex;
											
	SInt16								thicknessOffset;
											
	Boolean								withInLimitsFlag,
											winUseOriginFlag;
	
	SignedByte							windowHandleStatus;

	#if defined multispec_mac
		PenState								penState; 
	#endif	// defined multispec_mac


	#if defined multispec_win
		if (gCDCPointer == NULL)            
																								return; 
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		if (gCDCPointer == NULL)
																								return;
	#endif	// defined multispec_wx
	
	shapeHandlePtr = NULL;
	SetChannelWindowVariables (windowCode, windowInfoHandle, kNotCoreGraphics);
	
	#if defined multispec_win
		CPen* 				pOldPen = NULL;
		
		Boolean				continueFlag = TRUE;
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		//wxPen* 				pOldPen = NULL;
		
		//Boolean				continueFlag = TRUE;
	#endif	// defined multispec_wx
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
														windowInfoHandle, &windowHandleStatus);
	
	if (windowInfoPtr != NULL) 
		{
				// Now get a pointer to the overlay list and the number of overlays.
									
		numberOverlays = windowInfoPtr->numberVectorOverlays;
		overlayListPtr = windowInfoPtr->overlayList;
		
		shapeHandlePtr = (Handle*)GetHandlePointer (
															gShapeFilesHandle, kLock);
														
		}	// end "if (windowInfoPtr != NULL)"
	
	if (shapeHandlePtr != NULL)
		{  
				// Save the current port and make active port the project image	
				//	window.																			
				
		GetPort (&savedPort);
		SetPortWindowPort (windowPtr);
			
		winUseOriginFlag = FALSE;
		
		#if defined multispec_win
			GetWindowClipRectangle (windowPtr, kImageArea, &gViewRect);
			if (!gMFC_Rgn.CreateRectRgn (gViewRect.left,
													gViewRect.top,
													gViewRect.right,
													gViewRect.bottom))
				continueFlag = FALSE;
         
         if (continueFlag)
         	{  
				LOGPEN 						logpen;

				overlayPenPtr = new CPen ();

				logpen.lopnStyle = PS_SOLID;
				logpen.lopnWidth.x = 1;
				logpen.lopnWidth.y = 1;
	
				logpen.lopnColor = RGB (0, 0, 0);
									
				continueFlag = overlayPenPtr->CreatePenIndirect (&logpen);

				pOldPen = gCDCPointer->SelectObject (overlayPenPtr);  
				
				gCDCPointer->SetBkMode (TRANSPARENT);
					
				}	// end " if (continueFlag)"                                
		#endif	// defined multispec_win
			
		#if defined multispec_mac
			if (context == NULL)
				{
						// Save the current pen state.												
						
				GetPenState (&penState);
				
				PenMode (patCopy);
				PenPat (GetQDGlobalsBlack (&black));
					
				}	// end "if (context == NULL"
			
			else	// context != NULL
				gCGContextBeginPathPtr (context);
		#endif	// defined multispec_mac
      
		#if defined multispec_wx
					// Initialize pen to white 
         GetWindowClipRectangle (windowPtr, kImageArea, &gViewRect);
			//inputBoundingRectPtr = &gViewRect;
			overlayPenPtr = new wxPen (*wxWHITE);
				
			gCDCPointer->GetUserScale (&xScale, &yScale);
			gCDCPointer->SetUserScale (1, 1);
		#endif
											
				// Set some parameters for converting from map units to window units.

		SetMapToWindowUnitsVariables (windowInfoHandle,
												windowCode,
												kVectorOverlay,
												kNotCoreGraphics, 
												&mapToWindowUnitsVariables);
	
				// Get the rectangle in map units that is being updated in
				// the current window.	

		GetBoundingMapRectangle (windowPtr,
											windowInfoHandle,
											inputBoundingRectPtr,
											windowCode,
											&boundingWindowBox);

				// Set parameters to properly copy offscreen to destination window.
				
		SetDestinationWindowParameters (windowCode,
													windowPtr);
						
		if (gSideBySideChannels > 1)
			{											
				// If this is a side-by-side channel display then clip to
				// the first channel being displayed.
					
			clipRect = gViewRect;   
				
			#ifdef multispec_mac
				clipRect.left = (SInt16)MAX (0, gStartChannelWindow);
				clipRect.right = (SInt16)MIN (gViewRect.right, 
														gStartChannelWindow + gChannelWindowWidth);
				ClipRect (&clipRect);
			#endif	// multispec_mac

			#if defined multispec_win || defined multispec_wx
				clipRect.left = (int)MAX (0, gStartChannelWindow);
				clipRect.right = (int)MIN (gViewRect.right, 
														gStartChannelWindow + gChannelWindowWidth);
				if (windowCode != kToPrintWindow)
					ClipRect (&clipRect);
			#endif	// defined multispec_win || defined multispec_wx
			
			}	// end "if (gSideBySideChannels > 1)"     

		//SInt16 countPolygons = 0;
		
		for (overlayIndex=0; overlayIndex<numberOverlays; overlayIndex++)
			{
			vectorDataPtr = NULL;
			shapeFileIndex = overlayListPtr[overlayIndex].index;
			
			if (shapeFileIndex > 0)
				{
				shapeFileIndex--;
				shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
																shapeHandlePtr[shapeFileIndex],
																kLock);
			
				if (shapeInfoPtr != NULL)		
					vectorDataPtr = (Ptr)GetHandlePointer (shapeInfoPtr->vectorDataHandle,
																			kLock);
																
				}	// end "if (shapeFileIndex > 0)"
																										
			if (vectorDataPtr != NULL)
				{	
						// Set color for overlay.
				
				vectorDataIndex = 0;
						
				#if defined multispec_mac
					if (context == NULL)
						{
						RGBForeColor (&windowInfoPtr->overlayList[overlayIndex].overlayColor);
						PenSize (windowInfoPtr->overlayList[overlayIndex].lineThickness, 
									windowInfoPtr->overlayList[overlayIndex].lineThickness);
									
						}	// end "if (context == NULL)"
				
					else	// context != NULL
						{
						gCGContextSetLineWidthPtr (
							context,
							(float)windowInfoPtr->overlayList[overlayIndex].lineThickness);
					
						}	// end "else context != NULL"
				#endif	// defined multispec_mac
						
				#if defined multispec_win
					if (overlayPenPtr != NULL)
						delete overlayPenPtr;

					overlayPenPtr = new CPen;

					SetPenCharacteristics (
										overlayPenPtr,
										&windowInfoPtr->overlayList[overlayIndex].overlayColor,
										windowInfoPtr->overlayList[overlayIndex].lineThickness);

					if (windowCode == kToPrintWindow || windowCode == kToClipboardWindow)
						winUseOriginFlag = TRUE;
				#endif	// defined multispec_win

				#if defined multispec_wx
					if (overlayPenPtr != NULL)
						delete overlayPenPtr;
					
					RGBColor pcolor = windowInfoPtr->overlayList[overlayIndex].overlayColor;
					wxColour pwxcolor ((unsigned char)pcolor.red,
												(unsigned char)pcolor.green,
												(unsigned char)pcolor.blue);

               int pthick =
								(int)windowInfoPtr->overlayList[overlayIndex].lineThickness;
					
							// Now make sure pthick is thick enough when zoomed out

					overlayPenPtr = new wxPen (pwxcolor, pthick);
					
					gCDCPointer->SetPen (*overlayPenPtr);

					if (windowCode == kToPrintWindow || windowCode == kToClipboardWindow)
						winUseOriginFlag = TRUE;
				#endif	// defined multispec_wx
							
				thicknessOffset = 
									windowInfoPtr->overlayList[overlayIndex].lineThickness/2;

				for (recordIndex=0; 
							recordIndex<shapeInfoPtr->numberRecords;
									recordIndex++)
					{	
					arcViewRecordHeaderPtr = 
										(ArcViewRecordHeaderPtr)&vectorDataPtr[vectorDataIndex];

					switch (arcViewRecordHeaderPtr->shapeType)
						{			
		      		case 1:	// Point shape
		      		case 11:	// PointZ shape
		      			arcViewPointPtr = (ArcViewPointPtr)arcViewRecordHeaderPtr;
		      			
		      			if (PointInRectangle (&arcViewPointPtr->point, 
															&boundingWindowBox))
		      				{
								withInLimitsFlag = ConvertMapPointToWinPoint (
																	&arcViewPointPtr->point, 
																	&nextPoint, 
																	&mapToWindowUnitsVariables);
																	
								#if defined multispec_mac
									if (withInLimitsFlag)
										{
										nextPoint.h -= thicknessOffset;
										nextPoint.v -= thicknessOffset;
										
										MoveTo ((SInt16)nextPoint.h-2, (SInt16)nextPoint.v);
										LineTo ((SInt16)nextPoint.h+2, (SInt16)nextPoint.v);
										
										MoveTo ((SInt16)nextPoint.h, (SInt16)nextPoint.v-2);
										LineTo ((SInt16)nextPoint.h, (SInt16)nextPoint.v+2);
											
										}	// end "if (withInLimitsFlag)"
								#endif	// defined multispec_mac
								
								#if defined multispec_win
									gCDCPointer->MoveTo ((int)nextPoint.h-3, (int)nextPoint.v); 
									gCDCPointer->LineTo ((int)nextPoint.h+4, (int)nextPoint.v);
									
									gCDCPointer->MoveTo ((int)nextPoint.h, (int)nextPoint.v-3); 
									gCDCPointer->LineTo ((int)nextPoint.h, (int)nextPoint.v+4);
								#endif	// defined multispec_win
								
								#if defined multispec_wx
									gCDCPointer->DrawLine ((int)nextPoint.h-3,
																	(int)nextPoint.v,
																	(int)nextPoint.h+4,
																	(int)nextPoint.v);
									
									gCDCPointer->DrawLine ((int)nextPoint.h,
																	(int)nextPoint.v-3,
																	(int)nextPoint.h,
																	(int)nextPoint.v+4);
								#endif	// defined multispec_wx
								
								}	// end "if (PointInBox (&areaViewPolyLinePtr->box, ..."
		      			break;
		      			
		      		case 3:	// PolyLine shape	
		      		case 5:	// Polygon shape
		      		case 15:	// PolygonZ shape
		      			arcViewPolyLinePtr = (ArcViewPolyLinePtr)arcViewRecordHeaderPtr;
							
									// This is done so the DoubleRect box is on 8 byte boundary
							polyLineBox = arcViewPolyLinePtr->box;
		      			if (AreasIntersect (&boundingWindowBox,
		      										&polyLineBox))
		      				{
		      				pointIndex = 0;
		      				arcViewDoublePointPtr = (ArcViewDoublePoint*)
									&arcViewPolyLinePtr->parts[arcViewPolyLinePtr->numParts];
		      				
		      				for (partIndex=0; 
		      							partIndex<arcViewPolyLinePtr->numParts; 
		      								partIndex++)
		      					{
									withInLimitsFlag = ConvertMapPointToWinPoint (
																		arcViewDoublePointPtr, 
																		&nextPoint, 
																		&mapToWindowUnitsVariables);
																	
									#if defined multispec_mac
										nextPoint.h -= thicknessOffset;
										nextPoint.v -= thicknessOffset;
										
										if (context == NULL)
											MoveTo ((SInt16)nextPoint.h, (SInt16)nextPoint.v);
										
										else	// context != NULL
											gCGContextMoveToPointPtr (context,
																				(float)nextPoint.h,
																				(float)nextPoint.v);
									#endif	// defined multispec_mac
									
									#if defined multispec_win
										gCDCPointer->MoveTo ((int)nextPoint.h, 
																		(int)nextPoint.v); 
									#endif	// defined multispec_win
									
									#if defined multispec_wx
											// wxWidgets does not have Move command; only line.
									#endif	// defined multispec_wx
									
									lastPoint = nextPoint;
									
									arcViewDoublePointPtr++;
									
		      					pointStart = arcViewPolyLinePtr->parts[partIndex] + 1;
		      					
		      					if (partIndex+1 < arcViewPolyLinePtr->numParts)
		      						pointStop = arcViewPolyLinePtr->parts[partIndex+1];
		      					else	// partIndex+1 == areaViewPolyLinePtr->numParts
		      						pointStop = arcViewPolyLinePtr->numPoints;
				
		      					for (pointIndex=pointStart;
													pointIndex<pointStop;
																pointIndex++)
		      						{
										withInLimitsFlag = ConvertMapPointToWinPoint (
																			arcViewDoublePointPtr, 
																			&nextPoint, 
																			&mapToWindowUnitsVariables);
										
										if (nextPoint.h != lastPoint.h ||
																		nextPoint.v != lastPoint.v)
											{							
											#if defined multispec_mac
												if (withInLimitsFlag)
													{
													nextPoint.h -= thicknessOffset;
													nextPoint.v -= thicknessOffset;
										
													if (context == NULL)
														LineTo ((SInt16)nextPoint.h,
																	(SInt16)nextPoint.v);
													
													else	// context != NULL
														gCGContextAddLineToPointPtr (
																					context,
																					(float)nextPoint.h,
																					(float)nextPoint.v);
													}	// end "if (withInLimitsFlag)"
											#endif	// defined multispec_mac
											
											#if defined multispec_win
												gCDCPointer->LineTo ((int)nextPoint.h, 
																			(int)nextPoint.v);
											#endif	// defined multispec_win
											
											#if defined multispec_wx
												gCDCPointer->DrawLine ((int)lastPoint.h, 
																				(int)lastPoint.v, 
																				(int)nextPoint.h, 
																				(int)nextPoint.v);       	
											#endif	// defined multispec_wx
									
											lastPoint = nextPoint;
											
											}	// end "if (nextPoint != lastPoint)"
										
										arcViewDoublePointPtr++;
										
										}	// end "for (pointIndex=partStart; ..."
										
									}	// end "for (partIndex=0; ..."
								
								}	// end "if (AreasIntersect (&areaViewPolyLinePtr->box, ..."
		      			break;
		      			
		      		default:
		      			break;
						
						}	// end "switch (areaViewRecordHeaderPtr->shapeType)"
						
					vectorDataIndex += arcViewRecordHeaderPtr->recordLength;
			
							// Exit routine if user has "command period" down.					
							
					#if defined multispec_mac
						if (gProcessorCode != kZoomButtonControl && 
																		TickCount () >= gNextTime)
							{
							if (!CheckSomeEvents (keyDownMask))
								break;
								
							}	// end "if (gProcessorCode != kZoomButtonControl && ..." 
					#endif	// defined multispec_mac 
					
					}	// end "for (recordIndex=0; ..."
				
				CheckAndUnlockHandle (shapeInfoPtr->vectorDataHandle);
				
				}	// end "if (vectorDataPtr != NULL)"
			
			if (shapeFileIndex >= 0)	
				CheckAndUnlockHandle (shapeHandlePtr[shapeFileIndex]);
			
			}	// end "for (overlayIndex=0; overlayIndex<..."
	
		#if defined multispec_mac
			if (QDIsPortBuffered (GetWindowPort (windowPtr)))
				QDFlushPortBuffer (GetWindowPort (windowPtr), NULL);        
		#endif	// defined multispec_mac
      
		ResetDestinationWindowParameters (windowCode,
														windowPtr);
														
		ResetMapToWindowUnitsVariables (windowInfoHandle, 
													&mapToWindowUnitsVariables);
					
		#if defined multispec_win
			gCDCPointer->SelectObject (pOldPen);
			gCDCPointer->SetBkMode (OPAQUE);
			gMFC_Rgn.DeleteObject ();

			if (overlayPenPtr != NULL)
				delete overlayPenPtr;          
		#endif	// defined multispec_win
			
		#if defined multispec_mac 
			if (context == NULL)
				{
						// Reset the pen state back to the original settings.					
			
				SetPenState (&penState);
					
				}	// end "if (context == NULL"
			
			else	// context != NULL
				gCGContextStrokePathPtr (context);
		#endif	// defined multispec_mac
					
		#if defined multispec_wx
			gCDCPointer->SetUserScale (xScale, yScale);
			gCDCPointer->DestroyClippingRegion ();
         
			if (overlayPenPtr != NULL)
				delete overlayPenPtr;
		#endif	// multispec_wx
		
		ForeColor (blackColor);
				
		SetPort (savedPort);
		
		}	// end "if (shapeHandlePtr != NULL)"
		
	CheckAndUnlockHandle (gShapeFilesHandle);
		
	MHSetState (windowInfoHandle, windowHandleStatus);

}	// end "DrawArcViewShapes"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		FileStringPtr GetFileNamePPointerFromShapeInfo
//
//	Software purpose:	The purpose of this routine is to return the file name
//							associated with the input shapeInfo structure. It return a 
//							pointer to a P string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:			CheckIfOverlayFileLoaded in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNamePPointerFromShapeInfo (
				ShapeInfoPtr						shapeInfoPtr)

{  
	return (GetFileNamePPointerFromShapeInfo (shapeInfoPtr, kDefaultFileStringCode));
	
}	// end "GetFileNamePPointerFromShapeInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		FileStringPtr GetFileNamePPointerFromShapeInfo
//
//	Software purpose:	The purpose of this routine is to return the file name
//							associated with the input shapeInfo structure. It return a 
//							pointer to a P string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:			CheckIfOverlayFileLoaded in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/27/2001
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void* GetFileNamePPointerFromShapeInfo (
				ShapeInfoPtr						shapeInfoPtr,
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	
	
	if (shapeInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (shapeInfoPtr);
		fileNamePtr = GetFileNamePPointerFromFileStream (fileStreamPtr, returnCode);

		}	// end "if (shapeInfoPtr != NULL)"

	return (fileNamePtr);
	
}	// end "GetFileNamePPointerFromShapeInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CMFileStream* GetFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to return the pointer
//							to the file stream structure or class.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/28/2000
//	Revised By:			Larry L. Biehl			Date: 03/12/2015

CMFileStream* GetFileStreamPointer (
				ShapeInfoPtr						shapeInfoPtr)
				
{  
	CMFileStream*						fileStreamPtr = NULL;
	
	if (shapeInfoPtr != NULL)
		{
		#if defined multispec_mac
			fileStreamPtr = &shapeInfoPtr->fileStream;
		#endif	// defined multispec_mac
		
		#if defined multispec_win || defined multispec_wx
			fileStreamPtr = shapeInfoPtr->fileStreamCPtr;
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "if (shapeInfoPtr != NULL)"
		
	return (fileStreamPtr);

}	// end "GetFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetLastVectorColorAndWidth
//
//	Software purpose:	The purpose of this routine is to get the last color and line
//							width used for the input vector.
//
//	Parameters in:				
//
//	Parameters out:	Last color for the input overlay vector
//							Last width used for the input overlay vector
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/09/2006
//	Revised By:			Larry L. Biehl			Date: 10/09/2006

void GetLastVectorColorAndWidth (
				SInt32								overlayNumber,
				RGBColor*							lastLineColorPtr,
				UInt16*								lastLineThicknessPtr)
				
{  
	Handle*								shapeHandlePtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	UInt32								colorIndex;
	
		
	shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle);

	colorIndex = (overlayNumber-1 % 7);
	*lastLineColorPtr = gOverlayColorList[colorIndex];
	*lastLineThicknessPtr = 1;
	
	if (shapeHandlePtr != NULL) 
		{
		shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
																shapeHandlePtr[overlayNumber-1]);
															
		if (shapeInfoPtr != NULL)
			{					
			*lastLineColorPtr = shapeInfoPtr->lastOverlayColor;
			*lastLineThicknessPtr = shapeInfoPtr->lastLineThickness;					

			}	// end "if (shapeInfoPtr != NULL)"
				
		}	// end "if (shapeHandlePtr != NULL)"

}	// end "GetLastVectorColorAndWidth"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetArcViewMapInformation
//
//	Software purpose:	The purpose of this routine is to check for the existance of
//							any ArcView map information in the header file.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/16/2001
//	Revised By:			Larry L. Biehl			Date: 01/17/2019

Boolean GetArcViewMapInformation (
				FileInfoPtr							fileInfoPtr,
				char*									headerRecordPtr,
				Boolean*								geoSpotHeaderFlagPtr)
				
{  
	double								ulXmap,
											ulYmap,
											xDim,
											yDim;
											
	char*									stringPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	SInt32								value;
	
	SInt16								datumCode,
											gridZone,
											mapUnitsCode,
											projectionCode,
											referenceSystemCode,
											tReturnCode;
	
	Boolean								infoExistsFlag = FALSE;
	
	
	*geoSpotHeaderFlagPtr = FALSE;
	
			// Find "ULXMAP" in the buffer.								
		
	ulXmap = GetFileHeaderRealValue (kFileIOStrID, 
													IDS_FileIO141,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
													
	if (tReturnCode == 0)
		infoExistsFlag = TRUE;
		
	else	// tReturnCode != 0
		ulXmap = 1.;
		
			// Find "ULYMAP" in the buffer.								
		
	ulYmap = GetFileHeaderRealValue (kFileIOStrID, 
													IDS_FileIO142,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
						
	if (tReturnCode == 0)
		infoExistsFlag = TRUE;
		
	else	// tReturnCode != 0
		ulYmap = 1.;
						
			// Find "XDIM" in the buffer.								
		
	xDim = GetFileHeaderRealValue (kFileIOStrID, 
													IDS_FileIO143,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
						
	if (tReturnCode == 0)
		infoExistsFlag = TRUE;
		
	else	// tReturnCode != 0
		xDim = 1.;
						
			// Find "YDIM" in the buffer.								
		
	yDim = GetFileHeaderRealValue (kFileIOStrID, 
													IDS_FileIO144,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
						
	if (tReturnCode == 0)
		infoExistsFlag = TRUE;
		
	else	// tReturnCode != 0
		yDim = -1.;
		
			// The following are not true ArcView parameters. But sometimes the 
			// information is included for documentation purposes. MultiSpec will
			// will write this information if available (and read it).
						
			// Find "MAPUNITS" in the buffer.								
		
	stringPtr = headerRecordPtr;
	tReturnCode = GetFileHeaderString (kFileIOStrID, 
												IDS_FileIO145,
												&stringPtr, 
												4, 
												kDoNotSkipEqual,
												(char*)gTextString,
												0,
												50,
												kNoSubstringAllowed);
						
			// Find "MAP_UNITS" in the buffer.								
		
	if (tReturnCode != 0)
		{
		stringPtr = headerRecordPtr;
		tReturnCode = GetFileHeaderString (kFileIOStrID, 
													IDS_FileIO147,
													&stringPtr, 
													4, 
													kDoNotSkipEqual,
													(char*)gTextString,
													0,
													50,
													kNoSubstringAllowed);
													
		}	// end "if (tReturnCode != 0)"
	
	mapUnitsCode = kUnknownCode;	
	if (tReturnCode == 0)
		{
		if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"METE", 4) == 0)
			mapUnitsCode = kMetersCode;
	
		else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"FEET", 4) == 0)
			mapUnitsCode = kFeetCode;
		
		infoExistsFlag = TRUE;
		
		*geoSpotHeaderFlagPtr = TRUE;
		
		}	// end "if (tReturnCode == 0)"
		
			// Read Datum
	
	datumCode = 0;		
	stringPtr = headerRecordPtr;
	tReturnCode = GetDatumInfo (&stringPtr, &datumCode, IDS_FileIO140, kDoNotSkipEqual);
	if (tReturnCode == 0)
		infoExistsFlag = TRUE;
						
			// Find "PROJECTION_NAME" in the buffer.								
	
	projectionCode = kNotDefinedCode;
	referenceSystemCode = kUserDefinedRSCode;	
	stringPtr = headerRecordPtr;
	tReturnCode = GetFileHeaderString (kFileIOStrID, 
												IDS_FileIO150,
												&stringPtr, 
												15, 
												kDoNotSkipEqual,
												(char*)gTextString,
												0,
												50,
												kNoSubstringAllowed);
	
	if (tReturnCode == 0)
		{	
		projectionCode = GetProjectionNameInfo (&gTextString[1],
																	&referenceSystemCode);
		infoExistsFlag = TRUE;
		
		}	// end "if (tReturnCode == 0)"
		
			// Find "PROJECTION_ZONE" in the buffer.								
	
	gridZone = 0;	
	value = GetFileHeaderValue (kFileIOStrID, 
											IDS_FileIO151,
											headerRecordPtr, 
											1,
											kDoNotSkipEqual, 
											&tReturnCode);
						
	if (tReturnCode == 0)
		{
		gridZone = (SInt16)value;
		if (referenceSystemCode >= kUTM_NAD27RSCode && 
				referenceSystemCode <= kUTMRSCode &&
					gridZone != 0 &&
						ulYmap > 0)
			gridZone = -abs (gridZone);
			
		infoExistsFlag = TRUE;
		
		}	// end "if (tReturnCode == 0)"
		
	if (infoExistsFlag)
		{
				// At least one map parameter was set. Load info into the
				// map projection structure.
										
		mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
		
		if (mapProjectionHandle != NULL)
			{ 								
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
												GetHandlePointer (mapProjectionHandle,
																			kLock);
                                                                      
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = ulXmap;
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = ulYmap;
                                                                      
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = ulXmap;
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = ulYmap;
																			
			mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = xDim;
			mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = yDim;
		
			mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;
			
			mapProjectionInfoPtr->geodetic.datumCode = datumCode;
			mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
																					referenceSystemCode;
			mapProjectionInfoPtr->gridCoordinate.projectionCode = projectionCode;
			
			mapProjectionInfoPtr->gridCoordinate.gridZone = gridZone;
	
			CheckAndUnlockHandle (mapProjectionHandle); 
			
			}	// end "if (mapProjectionHandle != NULL)"
		
		}	// end "if (infoExistsFlag)"
		
	return (infoExistsFlag);

}	// end "GetArcViewMapInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetMemoryForVectorData
//
//	Software purpose:	The purpose of this routine is to obtain the memory that will 
//							be used to store the shape file vector data in.
//
//	Parameters in:		shape file information pointer
//
//	Parameters out:	None
//
//	Value Returned:	TRUE: if memory allocated okay
//							FALSE: if memory not able to be allocated
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/08/2006
//	Revised By:			Larry L. Biehl			Date: 08/25/2010

Boolean GetMemoryForVectorData (
				ShapeInfoPtr		 				shapeInfoPtr)

{	
	SInt64								numberBytes;


	numberBytes = shapeInfoPtr->fileLength - 100;
	numberBytes = ((numberBytes + 7)/8) * 8;
	shapeInfoPtr->vectorDataHandle = MNewHandle (numberBytes);
	
	return (shapeInfoPtr->vectorDataHandle != NULL);

}	// end "GetMemoryForVectorData"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetShapeFileNumberRecordsAndType
//
//	Software purpose:	The purpose of this routine is to return the number of records
//							in the requested shape file
//
//	Parameters in:		Handle to a list of loaded shape files
//							Index within the list to use
//
//	Parameters out:	None
//
//	Value Returned:	number of records
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/12/2013
//	Revised By:			Larry L. Biehl			Date: 04/15/2013

UInt32 GetShapeFileNumberRecordsAndType (
				Handle								shapeFilesHandle, 
				SInt16								shapeFileIndex,
				SInt16*								shapeTypePtr)

{	
	ArcViewRecordHeaderPtr			arcViewRecordHeaderPtr;
	Handle*								shapeHandlePtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	UInt32								numberRecords = 0;
	

	*shapeTypePtr = 0;
	
	if (shapeFileIndex >= 0)
		{
					// Get a pointer to the overlay list and the number of overlays.
					
		shapeHandlePtr = (Handle*)GetHandlePointer (shapeFilesHandle);
															
		if (shapeHandlePtr != NULL)
			{  
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
																	shapeHandlePtr[shapeFileIndex]);
								
			if (shapeInfoPtr != NULL)
				{
				numberRecords = shapeInfoPtr->numberRecords;
					
				arcViewRecordHeaderPtr = (ArcViewRecordHeaderPtr)GetHandlePointer (
																		shapeInfoPtr->vectorDataHandle);
																		
				*shapeTypePtr = (SInt16)arcViewRecordHeaderPtr->shapeType;
				
				}	// end "if (shapeInfoPtr != NULL)"
															
			}	// end "if (shapeFileIndex > 0)"
			
		}	// end "if (shapeFileIndex >= 0)"
		
	return (numberRecords);

}	// end "GetShapeFileNumberRecordsAndType"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeOverlay
//
//	Software purpose:	The purpose of this routine is to initialize the structure for
//							the new overaly.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ArcView header record parameters make sense
//							1 - if ArcView header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/25/2001
//	Revised By:			Larry L. Biehl			Date: 01/13/2020

void InitializeOverlay (
				WindowInfoPtr		 				windowInfoPtr, 
				SInt32								overlayNumber)

{	
	UInt32								index;
	
	
	if (windowInfoPtr != NULL)
		{
		index = windowInfoPtr->numberVectorOverlays;
										
		windowInfoPtr->overlayList[index].index = (SInt8)(overlayNumber);
	
		GetLastVectorColorAndWidth (
										overlayNumber,
										&windowInfoPtr->overlayList[index].overlayColor,
										&windowInfoPtr->overlayList[index].lineThickness);

		windowInfoPtr->numberVectorOverlays++;
	
		}	// end "InitializeOverlay"

}	// end "InitializeOverlay"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListNonIntersectionMessage
//
//	Software purpose:	The purpose of this routine is to list a message indicating
//							that the bounding map rectangle for the image and that for the
//							overlay file do not intersect. The bounding rectangles will
//							be listed. Also a check will be made to see if it is possible
//							that the units for the overlay file may be lat-long degrees and
//							the that for the image file being something else or vice versa.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ArcView header record parameters make sense
//							1 - if ArcView header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/10/2001
//	Revised By:			Larry L. Biehl			Date: 04/03/2001

void ListNonIntersectionMessage (
				Handle		 						windowInfoHandle, 
				DoubleRect*							boundingOverlayRectanglePtr,
				SInt16								stringNumber)

{	
	DoubleRect							boundingMapRectangle;
	
	WindowInfoPtr						windowInfoPtr;
	
	Boolean								continueFlag = TRUE;
															
		
	continueFlag = ListSpecifiedStringNumber (
											kAlertStrID, 
											stringNumber, 
											(unsigned char*)gTextString, 
											NULL, 
											gOutputForce1Code,
											continueFlag);
													
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
										
	if (windowInfoPtr != NULL)
		{
		boundingMapRectangle = windowInfoPtr->boundingMapRectangle;
		
		sprintf ((char*)gTextString, 
						"%s  Bounding map rectangle for image is:%s", 
						gEndOfLine,
						gEndOfLine);
						
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
		
		sprintf ((char*)gTextString, 
						"    x: %f - %f%s", 
						boundingMapRectangle.left,
						boundingMapRectangle.right, 
						gEndOfLine);
						
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
		
		sprintf ((char*)gTextString, 
						"    y: %f - %f%s", 
						boundingMapRectangle.top,
						boundingMapRectangle.bottom, 
						gEndOfLine);
						
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
												
		sprintf ((char*)gTextString, 
						"%s  Bounding map rectangle for shape file is:%s", 
						gEndOfLine,
						gEndOfLine);
						
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
												
		sprintf ((char*)gTextString, 
						"    x: %f - %f%s", 
						boundingOverlayRectanglePtr->left,
						boundingOverlayRectanglePtr->right, 
						gEndOfLine);
						
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
												
		sprintf ((char*)gTextString, 
						"    y: %f - %f%s", 
						boundingOverlayRectanglePtr->top,
						boundingOverlayRectanglePtr->bottom, 
						gEndOfLine);
						
		continueFlag = OutputString (NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
												
				// Check if possible that map units for shape file and that for
				// image file are different.
		
		stringNumber = 0;										
		if (boundingOverlayRectanglePtr->left >= -180 &&
				boundingOverlayRectanglePtr->right <= 180 &&
				boundingOverlayRectanglePtr->top >= -90 &&
				boundingOverlayRectanglePtr->bottom <= 90 &&
				
				(boundingMapRectangle.left < -180 ||
					boundingMapRectangle.right > 180 ||
					boundingMapRectangle.top < -90 ||
					boundingMapRectangle.bottom > 90))
			stringNumber = IDS_Alert85;	
										
		else if (boundingMapRectangle.left >= -180 &&
				boundingMapRectangle.right <= 180 &&
				boundingMapRectangle.top >= -90 &&
				boundingMapRectangle.bottom <= 90 &&
				
				(boundingOverlayRectanglePtr->left < -180 ||
					boundingOverlayRectanglePtr->right > 180 ||
					boundingOverlayRectanglePtr->top < -90 ||
					boundingOverlayRectanglePtr->bottom > 90))
			stringNumber = IDS_Alert86;	
										
		if (boundingOverlayRectanglePtr->left == boundingOverlayRectanglePtr->right ||
				boundingOverlayRectanglePtr->top == boundingOverlayRectanglePtr->bottom ||
				boundingMapRectangle.left == boundingMapRectangle.right ||
				boundingMapRectangle.top == boundingMapRectangle.bottom)
			stringNumber = 0;	
		
		if (stringNumber != 0)	
			continueFlag = ListSpecifiedStringNumber (
													kAlertStrID, 
													stringNumber, 
													(unsigned char*)gTextString, 
													NULL, 
													gOutputForce1Code,
													continueFlag);
			
		}	// end "if (windowInfoPtr != NULL)"

}	// end "ListNonIntersectionMessage"


                       
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean OverlayControlDialog
//
//	Software purpose:	The purpose of this routine is to handle the modal dialog for 
//							selecting the color of the overlay line and the thickness of 
//							the line.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			OnToolBarOverlaySelection in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/24/2001
//	Revised By:			Larry L. Biehl			Date: 01/28/2019

Boolean OverlayControlDialog (
				Handle								windowInfoHandle,
				SInt16								overlayIndex)

{ 	
	WindowInfoPtr						windowInfoPtr;
	
	SInt16								overlayCode;
		
	Boolean								continueFlag = FALSE;
											
	SignedByte							windowHandleStatus;
	
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
													windowInfoHandle, &windowHandleStatus);
	
	if (windowInfoPtr == NULL)
																			return (continueFlag);
																			
	overlayCode = kImageOverlay;
	if (overlayIndex >= (SInt16)windowInfoPtr->numberImageOverlays)
		{												
		overlayCode = kVectorOverlay;
		overlayIndex -= (SInt16)windowInfoPtr->numberImageOverlays;
		
		}	// end "if (overlayIndex >= windowInfoPtr->numberImageOverlays)"
	
	#if defined multispec_mac
		Rect									theBox,
												theColorBox;
												
		RGBColor								newRGB;
		
		DialogPtr							dialogPtr;
		
		UserItemUPP							drawOverlayColorBoxPtr;
	  
		
		Handle								theHandle;
		
		SInt32								maxValue,
												minValue;
		
		SInt16								itemHit,
												theType;
												
		UInt16								lineThickness,
												transparency;
		
		Boolean								modalDone;
		                                      
	
				// Get the modal dialog for the reformat specification					
					
		dialogPtr = LoadRequestedDialog (kOverlayParameterID, kDoNotCopyScrap, 1, 2);
		if (dialogPtr == NULL)														
			{
			MHSetState (windowInfoHandle, windowHandleStatus);
																							return (0);
																							
			}	// end "if (dialogPtr == NULL)"
		
				// Intialize local user item proc pointers.	
				
		drawOverlayColorBoxPtr = NewUserItemUPP (DrawColorBoxInDialogWindow);
		
				// Get box for later used.
			
		GetDialogItem (dialogPtr, 4, &theType, &theHandle, &theColorBox);
	
				// Set Procedure pointers for those dialog items that need them.
				
		SetDialogItemDrawRoutine (dialogPtr, 4, drawOverlayColorBoxPtr);	

		OverlayDialogInitialize (dialogPtr,
											windowInfoPtr,
											overlayCode,
											overlayIndex,
											&gCurrentSelectedColor,
											&lineThickness,
											&transparency);
	
		if (overlayCode == kImageOverlay)
			{
					// Set current transparency
							
			LoadDItemValue (dialogPtr, 6, (SInt32)transparency);
			maxValue = 100;
			minValue = 0;
			
			}	// end "if (overlayCode == kImageOverlay)"
			
		else	// overlayCode == kVectorOverlay
			{
					// Set current line thickness	
						
			LoadDItemValue (dialogPtr, 6, (SInt32)lineThickness);
			maxValue = 20;
			minValue = 1;
			
			}	// end "else overlayCode == kVectorOverlay"
		
				// Center the dialog and then show it.											
				
		ShowDialogWindow (dialogPtr, kOverlayParameterID, kSetUpDFilterTable);
	
				// Set default text selection						
				
		SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);
		
		modalDone = FALSE;
		itemHit = 0;
		do 
			{
		   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
			if (itemHit > 2)
				{
						// If itemHit was a radio button make appropriate control		
						// settings to indicate to the user the present selection.		
						// Get the handle to the itemHit.										
						
				GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
				
				switch (itemHit)
					{	
					case 3:

						if (SelectColor (3, 
												&gCurrentSelectedColor, 
												&newRGB))
							{
							gCurrentSelectedColor = newRGB;
							InvalWindowRect (GetDialogWindow (dialogPtr), &theColorBox);
							
							}	// end "if (SelectColor (3,..."
						break;
						
					case 6:
						break;
							
					}	// end "switch (itemHit)" 
					
				}	// end "if (itemHit > 2)" 
				
			else if (itemHit > 0)	// and <= 2
				{
						// Check line thickness.
											
				if (itemHit == 1)
					itemHit = CheckMaxValue (dialogPtr, 
														6, 
														minValue, 
														maxValue,
														kDisplayRangeAlert);
				
				if (itemHit > 0)
					modalDone = TRUE;
				
				if (itemHit == 1)
					{	
					OverlayDialogOK (windowInfoPtr,
											overlayCode,
											overlayIndex,
											&gCurrentSelectedColor,
											(SInt32)GetDItemValue (dialogPtr, 6));
					
					continueFlag = TRUE;
					
					}	// end "if (itemHit == 1)"
					
				}	// end "else if (itemHit > 0)" 
					
			}	while (!modalDone);
			
		DisposeUserItemUPP (drawOverlayColorBoxPtr);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac   
	
	#if defined multispec_win                         
		CMOverlayParameterDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMOverlayParameterDlg (); 
			                                                                  
			continueFlag = dialogPtr->DoDialog (windowInfoPtr,
																overlayCode,
																overlayIndex); 
			
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL 
	#endif // defined multispec_win 

   #if defined multispec_wx                         
		CMOverlayParameterDlg*		dialogPtr = NULL;
		
		//dialogPtr = new CMOverlayParameterDlg (wxTheApp->GetTopWindow ());
		dialogPtr = new CMOverlayParameterDlg (NULL);
			                                                                  
		continueFlag = dialogPtr->DoDialog (windowInfoPtr,
														overlayCode,
														overlayIndex); 
			
		delete dialogPtr;
	#endif // defined multispec_wx 
		
	MHSetState (windowInfoHandle, windowHandleStatus);
	
	return (continueFlag);
	
}	// end OverlayControlDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void OverlayDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the 
//							parameters in the Overlay Parameter dialog box
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			OverlayControlDialog in controls.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/24/2001
//	Revised By:			Larry L. Biehl			Date: 01/10/2003

void OverlayDialogInitialize (
				DialogPtr							dialogPtr,
				WindowInfoPtr						windowInfoPtr,
				SInt16								overlayCode,
				SInt16								overlayIndex,
				RGBColor*							overlayColorPtr,
				UInt16*								lineThicknessPtr,
				UInt16*								transparencyPtr)
			
{
	RGBColor*							inputRGBColorPtr;
			
		
	if (overlayCode == kImageOverlay)
		{
		HideDialogItem (dialogPtr, IDC_LineColorPrompt);	
		HideDialogItem (dialogPtr, IDC_LineColor);
		
		LoadDItemStringNumber (kDialogStrID,
										IDS_Dialog27,
										dialogPtr, 
										IDC_ValuePrompt, 
										(Str255*)gTextString);
			
		*transparencyPtr = (UInt16)(100 * 
					(1 - windowInfoPtr->imageOverlayList[overlayIndex].opacity));
					
		}	// end "if (overlayCode == kImageOverlay)"
		
	else 	// overlayCode == kVectorOverlay
		{	
				// Get the current overlay color.
		
		inputRGBColorPtr = &windowInfoPtr->overlayList[overlayIndex].overlayColor;
		overlayColorPtr->red = inputRGBColorPtr->red;
		overlayColorPtr->green = inputRGBColorPtr->green;
		overlayColorPtr->blue = inputRGBColorPtr->blue;

				// Get the current line thickness.
			
		*lineThicknessPtr = windowInfoPtr->overlayList[overlayIndex].lineThickness;
		
		}	// end "else overlayCode == kVectorOverlay"
	
}	// end "OverlayDialogInitialize"

									

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void OverlayDialogOK
//
//	Software purpose:	The purpose of this routine is to set up the overlay
//							structure parameters that the user made in the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			DoDialog in xOverlayParametersDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/24/2001
//	Revised By:			Larry L. Biehl			Date: 04/23/2019

void OverlayDialogOK (
				WindowInfoPtr						windowInfoPtr,
				SInt16								overlayCode,
				SInt16								overlayIndex,
				RGBColor*							overlayColorPtr,
				UInt16								value)

{  
	RGBColor*							inputRGBColorPtr;
	
	
	if (overlayCode == kImageOverlay)
		{
				// Set the image overlay transparency.
																
		windowInfoPtr->imageOverlayList[overlayIndex].opacity = 
																			((double)100-value)/100;
		
				// Make sure the drawBaseImageCode is correct for the image layer
				// because of the change in opacity
		
		UpateDrawBaseImageCode (windowInfoPtr, overlayIndex);
		
		}	// end "if (overlayCode == kImageOverlay)"
		
	else 	// overlayCode == kVectorOverlay
		{	
				// Set the vector overlay color.
	
		inputRGBColorPtr = &windowInfoPtr->overlayList[overlayIndex].overlayColor;
		inputRGBColorPtr->red = overlayColorPtr->red;
		inputRGBColorPtr->green = overlayColorPtr->green;
		inputRGBColorPtr->blue = overlayColorPtr->blue;

				// Set the vector line thickness.
																
		value = MAX (1, value);
		windowInfoPtr->overlayList[overlayIndex].lineThickness = value;
		
		SetLastVectorColorAndWidth (
									abs (windowInfoPtr->overlayList[overlayIndex].index),
									overlayColorPtr,
									value);
		
		}	// end "else overlayCode == kVectorOverlay"
	
}	// end "OverlayDialogOK" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadArcViewClassNames
//
//	Software purpose:	The purpose of this routine is to read the class
//							names from the ArcView .clr file.
//
//	Parameters in:		window
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			LoadClassNameDescriptions in SOpenImage.cpp
//							LoadThematicClasses in SSaveWrite.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/10/2011
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

Boolean ReadArcViewClassNames (
				FileInfoPtr							fileInfoPtr, 
				Handle								windowInfoHandle, 
				UCharPtr								classNamePtr)

{
	UInt8									buffer[5300],
											savedChar,
											textString[256];
	
	HParamBlockRec						paramBlock;
										 
	CMFileStream*						supportFileStreamPtr;
	
	UCharPtr								endClassNamePtr,
											localClassNamePtr,
											startClassNamePtr;
											
	UCharPtr								inputStringPtr;
	
	UInt16								*classSymbolPtr;
	
	int									classNumber;
	
	SInt32								classSymbolIndex,
											numberCharacters;
	
	UInt32								numberClasses,
											whileLoopCount;
	
	SInt16								errCode,
											firstClassNameCharacter,
											tReturnCode;
											
	UInt16								blue,
											green,
											tildeCharacter = 0x007E,		// ~
											red;		
	
	SignedByte							supportHandleStatus;
	
	Boolean								checkIOErrorFlag,
											continueFlag,
											returnFlag;
						
	#if defined multispec_win
		UCharPtr								endOfLinePtr;
		UInt8									savedCharacter;
	#endif	// defined multispec_win
	
		//	#if defined multispec_wx
		//		HPtr									savedptr;
		//	#endif
	
	
			// Initialize local variables.
	
	checkIOErrorFlag = TRUE;		
	returnFlag = FALSE;
	
	if (fileInfoPtr == NULL || 
							windowInfoHandle == NULL || classNamePtr == NULL)	
																			return (returnFlag);
	
			// Get file stream pointer to the support file
			
	supportFileStreamPtr = GetSupportFileStreamPointer (windowInfoHandle,
																		&supportHandleStatus);
			
			// If the support file is not open, then open a path to the file 		
			// that contains the class names.												
		
	errCode = OpenFileReadOnly (supportFileStreamPtr, 
											kResolveAliasChains, 
											kLockFile,
											kDoNotVerifyFileStream);
		
	if (errCode == fnfErr || errCode == bdNamErr)
		checkIOErrorFlag = FALSE;	
			
	if (errCode == noErr)
		errCode = PrepareToReadTextFile (&paramBlock,
														supportFileStreamPtr,
														buffer,
														5000,
														NULL,
														NULL);
														
			// Read the class names.  If the class name is blank or we reach	
			// the end of the file before all the names have been read, fill	
			// in a default name.															
	
	if (errCode == noErr)
		{
		numberClasses = fileInfoPtr->numberClasses;

				// Get a pointer to storage for the class symbols.					
				
		classSymbolPtr = (UInt16*)&classNamePtr[numberClasses*sizeof (Str31)];
		
				// First create the default class names. It is possible for classes to
				// be skipped in the .clr file.
		
		if (!fileInfoPtr->classesComputedFlag)
			CreateDefaultClassNames (classNamePtr, classSymbolPtr, numberClasses);
					
				// Now load the class names into the appropriate locations. Allow
				// for classes to be skipped. Skip the palette information in the file.
				// The class (or legend) name will start with "N:" after the palette
				// entries. Note that the way the code is written the first character in
				// the line has to be a number or the line will be skipped. This is per
				// the ArcView .clr specification. Note though that the ArcView .clr
				// specification does not define anything for class name. The class
				// name is something that has been set up for MultiSpec.

		whileLoopCount = 0;
		classSymbolIndex = 0;
		inputStringPtr = NULL;
		
		while (errCode == noErr)
			{
			if (whileLoopCount > fileInfoPtr->numberBins)
				{
					// This check is included to catch problem files which may cause
					// an infinite loop.
				
				int numberChars = sprintf (
								(char*)gTextString3,
								" There is a problem reading the .clr file; default class names will be used.%s",
								gEndOfLine);
				ListString ((char*)gTextString3, numberChars, gOutputTextH);
				
				errCode = eofErr;
				
				}	// end "if (whileLoopCount > fileInfoPtr->numberBins)"
			
			else	 // whileLoopCount <= fileInfoPtr->numberBins
				errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);

			if (errCode == eofErr)
				checkIOErrorFlag = FALSE;
			
					// Skip lines starting with non-numeric characters.

			if (errCode == noErr && inputStringPtr[0] >= '0' && inputStringPtr[0] <= '9')
				{
				#if defined multispec_win
							// For windows version, we need to add a null terminator at the
							// end of the line in place of the carriage return so that those
							// cases with no class names or group information is handled
							// properly. Otherwise sscanf reads beyond the carriage return
							// if parameter like class name, or group information do not
							// exist.
							// Find the carriage return and replace it with null terminator.
							// One needs to replace this character when done reading so that
							// the next line is found correctly.
				
					endOfLinePtr = SkipToNextCarriageReturn (inputStringPtr);
				
					//CheckSomeEvents (
					//				osMask + keyDownMask + updateMask + mDownMask + mUpMask);
					if (endOfLinePtr != NULL)
						{
						endOfLinePtr--;
						savedCharacter = *endOfLinePtr;
						*endOfLinePtr = 0;

						}	// end "if (endOfLinePtr != NULL)"
				#endif	// defined multispec_win

					// Get the data, red, green & blue values.
					// To allow for bad text files force a null line at location 254 in the
					// input string so that textString is not overrun. Save the character
					// at the location and put back in before continuing. Note that the
					// buffer for reading the data into is 300 bytes longer than what is
					// being used to allow for this occurring near the end of the buffer.
				
				savedChar = inputStringPtr[254];
				inputStringPtr[254] = 0;
				
				tReturnCode = sscanf ((char*)inputStringPtr,
											 (char*)"%d%hd%hd%hd%s\r",
											 &classNumber,
											 &red,
											 &green,
											 &blue,
											 (char*)textString);
											 
				inputStringPtr[254] = savedChar;
				
						// Verify parameters make sense and that this is the start of a 
						//	class name. The characters "n:" should be at the start.
						// Otherwise the assumption is that this is just a comment.
				
				continueFlag = FALSE;
				if (tReturnCode == 5 &&
							classNumber >= 0 &&
									classNumber <= gClassListLimit &&
											textString[0] == 'n' &&
													textString[1] == ':')
					{
							// Now determine if this is a class to be used.
					
					if (classSymbolPtr[classSymbolIndex] != classNumber)
						{
						while (classSymbolPtr[classSymbolIndex] < (UInt16)classNumber && 
													classSymbolIndex < (SInt32)(numberClasses-1))
							classSymbolIndex++;
							
						if (classSymbolPtr[classSymbolIndex] == classNumber)
							continueFlag = TRUE;
							
						else	// != classNumber
							{
							while (classSymbolPtr[classSymbolIndex] > classNumber && 
															classSymbolIndex > 0)
								classSymbolIndex--;
								
							if (classSymbolPtr[classSymbolIndex] == classNumber)
								continueFlag = TRUE;
							
							}	// end "else != classNumber"
						
						}	// end "if (classSymbolPtr[classSymbolIndex] != classnumber)"
						
					else	// == classNumber
						continueFlag = TRUE;

					}	// end "if (if (tReturnCode == 5 && classNumber >= 0 && ..."
					
				if (continueFlag)
					{	
							// Get the class name.
							
							// Find the start and end of the class name. textString cannot
							// be used because only the first word of a multiple word class
							// name will be read.
					
					startClassNamePtr = (UCharPtr)strstr ((char*)inputStringPtr, "n:");
					
					endClassNamePtr = (UCharPtr)strchr (
													(char*)inputStringPtr, (int)tildeCharacter);
					
							// Determine the number of characters in the class name.		
					
					numberCharacters = 0;
					firstClassNameCharacter = 0;
			
					if (startClassNamePtr != NULL && endClassNamePtr != NULL)
						{
						startClassNamePtr += 2;
						numberCharacters = (SInt32)(endClassNamePtr - startClassNamePtr);
						if (numberCharacters < 0 || numberCharacters > 31)
							numberCharacters = 31;
							
						}	// end "if (endClassNamePtr != NULL)"
																	
					else	// startClassNamePtr == NULL || ...
						numberCharacters = 0;
					
							// Remove any leading blanks in the class name.				
							
					while (numberCharacters != 0 && 
											startClassNamePtr[firstClassNameCharacter] == ' ')
						{
						firstClassNameCharacter++;
						numberCharacters--;
						
						}	// end "while (numberCharacters != 0 && ..." 
						
					if (numberCharacters > 0) 
						{
						localClassNamePtr = &classNamePtr [classSymbolIndex*32];
						BlockMoveData (&startClassNamePtr[firstClassNameCharacter], 
														&localClassNamePtr[1], numberCharacters);
						localClassNamePtr[0] = (UInt8)numberCharacters;
						if (numberCharacters < 31)
							localClassNamePtr[numberCharacters+1] = kNullTerminator;
						
						}	// end "if (numberCharacters > 0)"
			
					}	// end "if (continueFlag)"

				#if defined multispec_win
							// Set the end of line character back to the original value.
					
					if (endOfLinePtr != NULL)
						*endOfLinePtr = savedCharacter;
					endOfLinePtr = NULL;  	 
				#endif	// defined multispec_win

				}	// end "if (errCode == noErr && inputStringPtr[0] >= '0' && ..."
			
			whileLoopCount++;

			}	// end "while (errCode == noErr)" 
		
				// Indicate that the class names have been created.				
				
		returnFlag = TRUE;
		fileInfoPtr->ancillaryInfoformat = kArcViewDefaultSupportType;
		
		}	// end "if (errCode == noErr)"  
		
	if (checkIOErrorFlag)
		IOCheck (errCode, supportFileStreamPtr);
		  				
	CloseFile (supportFileStreamPtr);
		
	UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);
		
	return (returnFlag);
	
}	// end "ReadArcViewClassNames"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadArcViewColorPalette
//
//	Software purpose:	The purpose of this routine is to read the color
//							palette from the specified ArcView clr file.  
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:			
// 
// Called By:			LoadColorSpecTable in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/11/2011
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

Boolean ReadArcViewColorPalette (
				ColorSpec*							colorSpecPtr,
				FileInfoPtr							imageFileInfoPtr,
				CMFileStream*						paletteFileStreamPtr,
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt16*								paletteCodePtr)

{
	UInt8									buffer[5300];
	
	HParamBlockRec						paramBlock;
	
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
											
	UInt16								*vectorBluePtr,
											*vectorGreenPtr,
											*vectorRedPtr;
	
	UCharPtr								inputStringPtr = NULL;
	
	int									classNumber;
	
	UInt32								index,
											maxNumberClrTableClasses,
											whileLoopCount;
	
	SInt16								errCode,
											tReturnCode;
	
	UInt16								blue,
											green,
											//numberLevels,
											red;
	
	Boolean								classCodeFlag,
											closeFileAtEnd;
	
		
	if (paletteFileStreamPtr == NULL ||
		 displaySpecsPtr == NULL || 
		 colorSpecPtr == NULL ||
		 imageFileInfoPtr == NULL)
																						return (FALSE);
	
			// Check that there is a valid class to group handle if needed.		
	
	if (displaySpecsPtr->classGroupCode != kClassDisplay && 
		 imageFileInfoPtr->groupTablesHandle == NULL)
																						return (FALSE);
																						
			// Get memory for the color vectors.
			
	maxNumberClrTableClasses = MAX (256, imageFileInfoPtr->maxClassNumberValue + 1);
	vectorBluePtr = (UInt16*)MNewPointer (
												3 * maxNumberClrTableClasses * sizeof (UInt16));
	if (vectorBluePtr == NULL)
																						return (FALSE);
																						
	vectorGreenPtr = &vectorBluePtr[maxNumberClrTableClasses];	
	vectorRedPtr = &vectorBluePtr[2*maxNumberClrTableClasses];
		
	classCodeFlag = (displaySpecsPtr->classGroupCode == kClassDisplay);
	
		// Get the class to group pointer if needed.							
	
	if (!classCodeFlag)
		{		
		classToGroupPtr = GetClassToGroupPointer (imageFileInfoPtr);
		groupToPalettePtr = GetGroupToPalettePointer (imageFileInfoPtr);
			
		}	// end "if (!classCodeFlag)" 
	
			// If the file is not open, then open a path to the file that			
			// contains the palette. The OpenFileReadOnly routine will
			// check if the file is already opened.														
	
	errCode = noErr;
	closeFileAtEnd = FALSE;
	if (!FileOpen (paletteFileStreamPtr))
		{
		errCode = OpenFileReadOnly (paletteFileStreamPtr, 
											 kDontResolveAliasChains, 
											 kLockFile, 
											 kDoNotVerifyFileStream);
		
		IOCheck (errCode, paletteFileStreamPtr);
		
		if (errCode == noErr)
			closeFileAtEnd = TRUE;
		
		}	// end "if (!FileOpen (paletteFileStreamPtr))"
	
			// Continue if okay.																	
	
	if (errCode == noErr)
		{
		errCode = PrepareToReadTextFile (&paramBlock,
														paletteFileStreamPtr,
														buffer,
														5000,
														NULL,
														NULL);
		
				// Initialize the palette entries to all white.
		
		for (index=0; index<maxNumberClrTableClasses; index++)
			vectorBluePtr[index] = vectorGreenPtr[index] = vectorRedPtr[index] = 0xffff;
					
				// Now load the entire palette into the appropriate locations.
		
		whileLoopCount = 0;
		while (errCode == noErr)
			{
			if (whileLoopCount > imageFileInfoPtr->numberBins)
				{
					// This check is included to catch problem files which may cause an
					// infinite loop.
				
				int numberChars = sprintf (
							(char*)gTextString3,
							" There is a problem reading the .clr file; default palette will be used.%s",
							gEndOfLine);
				ListString ((char*)gTextString3, numberChars, gOutputTextH);
				
					//cancelWhileLoopFlag = TRUE;
				errCode = eofErr;
				
				}	// end "if (whileLoopCount > fileInfoPtr->numberBins)"
			
			else	 // whileLoopCount <= fileInfoPtr->numberBins
				errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);

				// Skip lines starting with non-numeric characters.
			
			if (errCode == noErr && inputStringPtr[0] >= '0' && inputStringPtr[0] <= '9')
				{
					// Get the data, red, green & blue values.
				
					tReturnCode = sscanf ((char*)inputStringPtr,
											 "%d%hd%hd%hd\r",
											 &classNumber,
											 &red,
											 &green,
											 &blue);
											 
				if (tReturnCode == 4 &&
							classNumber >= 0 &&
									(UInt32)classNumber < maxNumberClrTableClasses)
					{	
					#ifndef multispec_wx
						vectorRedPtr[classNumber] = red * 256;
						vectorGreenPtr[classNumber] = green * 256;
						vectorBluePtr[classNumber] =  blue * 256;	
					#endif
					
					#if defined multispec_wx	
						vectorRedPtr[classNumber] = red;
						vectorGreenPtr[classNumber] = green;
						vectorBluePtr[classNumber] =  blue;		
					#endif		
					}	// end "if (tReturnCode == 4 && classNumber >= 0 && ..."
				
				}	// end "if (errCode == noErr && inputStringPtr[0] >= '0' && ..."
			
			whileLoopCount++;
			
			}	// end "while (errCode == noErr)"
		
		errCode = noErr;
		
				// Now load what is needed for the palette.
		
		LoadTwoBytePalette (colorSpecPtr, 
									imageFileInfoPtr, 
									displaySpecsPtr, 
									classSymbolPtr, 
									paletteCodePtr,
									maxNumberClrTableClasses,
									vectorBluePtr,
									vectorGreenPtr,
									vectorRedPtr);
		
		}	// end "if (errCode == noErr)" 
	
	if (closeFileAtEnd)
		CloseFile (paletteFileStreamPtr);
		
	CheckAndDisposePtr (vectorBluePtr);
	
	return (errCode == noErr);
	
}	// end "ReadArcViewColorPalette"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadArcViewGroups
//
//	Software purpose:	The purpose of this routine is to read the group information
//							from the ArcView .clr file.
//
//	Parameters in:		inputFileInfoPtr - structure for the file that contains the
//														group information
//							outputFileInfoPtr - structure for the thematic image file that
//														the group information is for
//							displaySpecsPtr - pointer to the dispay specification structure
//														which which controls how the group
//														information will be displayed
//							windowInfoHandle - handle to the window information structure
//														for the active image window.
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			LoadThematicDisplaySpecs in SDisplayThematic.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/10/2011
//	Revised By:			Larry L. Biehl			Date: 01/08/2020

Boolean ReadArcViewGroups (
				FileInfoPtr							inputFileInfoPtr,
				FileInfoPtr							outputFileInfoPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				Handle								windowInfoHandle)

{
	UInt8									buffer[5300],
											savedChar,
											textString[256];
											
	HParamBlockRec						paramBlock;
										 
	CMFileStream*						supportFileStreamPtr;
	ColorSpec*							colorSpecPtr;
	CTabPtr								cTablePtr;
	
	char									*classNamePtr,
											*endClassNamePtr,
											*endGroupNamePtr,
											*groupNamePtr,
											*localGroupNamePtr,
											*startGroupInfoPtr,
											*startGroupNamePtr;
											
	UCharPtr								inputStringPtr;
	
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
											
	UInt16								*classSymbolPtr;
	
	int									classNumber,
											groupNumber;
	
	CTabHandle							cTableHandle;
	Handle								groupNameHandle,
											groupTablesHandle;
	
	SInt32								firstGroupClassNumber,
											groupCount,
											maxNumberClasses,
											numberCharacters;
	
	UInt32								bytesNeeded,
											groupToPaletteValue,
											numberClasses,
											numberGroups,
											paletteIndex,
											whileLoopCount;
	
	SInt16								errCode,
											tReturnCode;
											
	UInt16								blue,
											green,
											tildeCharacter = 0x007E,		// ~
											red;		
	
	SignedByte							supportHandleStatus;
	
	Boolean								addGroupFlag,
											changedFlag,
											checkIOErrorFlag = TRUE,
											continueFlag = FALSE,
											firstTimeFlag,
											groupExistsFlag,
											groupNameChangedFlag,
											groupTableChangedFlag,
											notDoneFlag,
											returnFlag = FALSE;

	#if defined multispec_win || defined multispec_wx
		UCharPtr								endOfLinePtr;
		UInt8									savedCharacter;
	#endif

	
	if (inputFileInfoPtr == NULL || outputFileInfoPtr == NULL ||
								displaySpecsPtr == NULL || windowInfoHandle == NULL)	
																			return (returnFlag);

			// Get file stream pointer to the support file.
			// Take into account whether this is from the default clr support
			// file for the image or from a file that the user selected using
			// the File menu.
	
	if (gProcessorCode == kOpenThematicInfo)
		supportFileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);
	
	else	// gProcessorCode != kOpenThematicInfo
		supportFileStreamPtr = GetSupportFileStreamPointer (windowInfoHandle,
																		&supportHandleStatus);
																		
	if (supportFileStreamPtr == NULL)
																			return (returnFlag);
		
	cTableHandle = NULL;
	groupNameHandle = NULL;
	groupTablesHandle = NULL;
	maxNumberClasses = MAX (256, inputFileInfoPtr->maxClassNumberValue+1);
			
			// If the support file is not open, then open a path to the file 		
			// that contains the class names.												
		
	errCode = OpenFileReadOnly (supportFileStreamPtr, 
											kResolveAliasChains, 
											kLockFile,
											kDoNotVerifyFileStream);
		
	if (errCode == fnfErr || errCode == bdNamErr)
		checkIOErrorFlag = FALSE;	
			
	if (errCode == noErr)
		errCode = PrepareToReadTextFile (&paramBlock,
													supportFileStreamPtr,
													buffer,
													5000,
													NULL,
													NULL);
														
			// Read the class names.  If the class name is blank or we reach	
			// the end of the file before all the names have been read, fill	
			// in a default name.															
	
	if (errCode == noErr)
		{		
				// Now make certain that we have memory allocated for 			
				// a class to grouping table. Allow for the maximum number of 
				// classes.
				
		continueFlag = GetGroupStructureMemory (maxNumberClasses,
																&groupTablesHandle,
																&groupNameHandle,
																&groupToPalettePtr,
																&groupNamePtr,
																&groupTableChangedFlag,
																&groupNameChangedFlag);
		
				// Get memory for group color table.
		
		bytesNeeded = 256*sizeof (ColorSpec) + sizeof (ColorTable);
		cTablePtr = (CTabPtr)CheckHandleSize ((Handle*)&cTableHandle,
															&continueFlag,
															&changedFlag,
															bytesNeeded);
		
		if (cTableHandle != NULL)
			{                               
			colorSpecPtr = (ColorSpec*)&cTablePtr->ctTable;
			
			cTablePtr->ctSeed = GetCTSeed ();
			cTablePtr->ctFlags = 0;
			cTablePtr->ctSize = (SInt16)(256 - 1);
			
			}	// end "if (cTableHandle != NULL)" 
									
		}	// end "if (errCode == noErr)"

	if (continueFlag)
		{
		classToGroupPtr = &groupToPalettePtr[maxNumberClasses];
		
				// Intialize group to class vector.
				
		for (classNumber=0; classNumber<maxNumberClasses; classNumber++)
			{
			groupToPalettePtr[classNumber] = -1;
			classToGroupPtr[classNumber] = -1;
			
			}	// end "for (classNumber=0; classNumber<..."
		
				// Now load the group information. Allow for classes to be skipped. 
				// Skip the palette information in the file and class names.
				// The group information will start with "g1:" after the palette
				// entries and class name. Note that the way the code is written the 
				// first character in the line has to be a number or the line will 
				// be skipped. This is per the ArcView .clr specification. Note though 
				// that the ArcView .clr specification does not define anything for 
				// class names or groups. The class name is something that has been 
				// set up for MultiSpec.
		
		whileLoopCount = 0;
		inputStringPtr = NULL;
		while (errCode == noErr)
			{
			if (whileLoopCount > inputFileInfoPtr->numberBins)
				{
						// This check is included to catch problem files which may cause an
						// infinite loop.
				
				int numberChars = sprintf (
							(char*)gTextString3,
							" There is a problem reading the .clr file; default group names will be used.%s",
							gEndOfLine);
				ListString ((char*)gTextString3, numberChars, gOutputTextH);
				
				errCode = eofErr;
				
				}	// end "if (whileLoopCount > fileInfoPtr->numberBins)"
			
			else	 // whileLoopCount <= fileInfoPtr->numberBins
				errCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
			
			if (errCode == eofErr)
				checkIOErrorFlag = FALSE;
			
					// Skip lines starting with non-numeric characters.
			
			if (errCode == noErr && inputStringPtr[0] >= '0' && inputStringPtr[0] <= '9')
				{
				#if defined multispec_win || defined multispec_wx
							// For windows version, we need to add a null terminator at the
							// end of the line in place of the carriage return so that those
							// cases with no class names or group information is handled
							// properly. Otherwise sscanf reads beyond the carriage return
							// if parameter like class name, or group information do not
							// exist.
							// Find the carriage return and replace it with null terminator.
							// One needs to replace this character when done reading so that
							// the next line is found correctly.
					
					endOfLinePtr = SkipToNextCarriageReturn (inputStringPtr);
					if (endOfLinePtr != NULL)
						{
						endOfLinePtr--;
						savedCharacter = *endOfLinePtr;
						*endOfLinePtr = 0;
						
						}	// end "if (endOfLinePtr != NULL)"	 
				#endif	// defined multispec_win || defined multispec_wx

						// Get the data, red, green & blue values.
						// To allow for bad text files force a carriage return at location
						// 254 in the input string so that textString is not overrun. Save
						// the character at the location and put back in before continuing.
						// Note that the buffer for reading the data into is 300 bytes
						// longer than what is being used to allow for this occurring near
						// the end of the buffer.
					
				savedChar = inputStringPtr[254];
				inputStringPtr[254] = 0;
				
				tReturnCode = sscanf ((char*)inputStringPtr, 
											 "%d%hd%hd%hd%s\r",
											 &classNumber,
											 &red,
											 &green,
											 &blue,
											 (char*)&textString);
											 
				inputStringPtr[254] = savedChar;
				
						// Verify parameters make sense and that this is the start of a 
						//	class name. The characters "n:" should be at the start.
						// Otherwise the assumption is that this is just a comment.
				
				if (tReturnCode == 5 &&
						classNumber >= 0 &&
							classNumber < maxNumberClasses &&
								textString[0] == 'n' &&
									textString[1] == ':')
					{
							// Now see if group information exists after the class name
					
					endClassNamePtr = (char*)strchr (
													(char*)inputStringPtr, (int)tildeCharacter);
					
					startGroupInfoPtr = NULL;
					if (endClassNamePtr != NULL)
						startGroupInfoPtr = (char*)strstr (&endClassNamePtr[2], "g:");
						
					groupToPaletteValue = classNumber % 256;
				
					if (startGroupInfoPtr != NULL)
						{
						startGroupInfoPtr += 2; 
				
						tReturnCode = sscanf (startGroupInfoPtr,
													 "%d%hd%hd%hd%s\r",
													 &groupNumber,
													 &red,
													 &green,
													 &blue,
													 (char*)&textString);
													 
						if (tReturnCode >= 1 &&
									groupNumber >= 0 &&
											groupNumber < maxNumberClasses)
							{						
									// Get the group name.
									
									// Find the start and end of the group name. textString
									// cannot be used because only the first word of a
									// multiple word group name will be read. Only the first
									// occurrance for the group name will be used for a group.
								
							if (groupToPalettePtr[groupNumber] == -1)
								groupToPalettePtr[groupNumber] = (SInt16)groupToPaletteValue;
								
							if (tReturnCode == 5)
								{
										// Get the group name
													
								startGroupNamePtr = (char*)strstr (
															startGroupInfoPtr, (CharPtr)textString);
								endGroupNamePtr = (char*)strchr (
															startGroupInfoPtr, (int)tildeCharacter);
								
										// Determine the number of characters in the group name.		
								
								numberCharacters = 0;					
								if (startGroupNamePtr != NULL && endGroupNamePtr != NULL)
									{
									numberCharacters =
													(SInt32)(endGroupNamePtr - startGroupNamePtr);
									if (numberCharacters < 0 || numberCharacters > 31)
										numberCharacters = 31;
										
									}	// end "if (endGroupNamePtr != NULL)"
																				
								else	// startGroupNamePtr == NULL || ...
									numberCharacters = 0; 
									
								if (numberCharacters > 0) 
									{
									localGroupNamePtr = &groupNamePtr[groupNumber*32];
									BlockMoveData (startGroupNamePtr, 
														&localGroupNamePtr[1],
														numberCharacters);
									localGroupNamePtr[0] = (UInt8)numberCharacters;
									if (numberCharacters < 31)
										localGroupNamePtr[numberCharacters+1] = kNullTerminator;
									
									}	// end "if (numberCharacters > 0)"
								
										// Get the color
								
								#ifndef multispec_wx
									colorSpecPtr[groupToPaletteValue].rgb.red = red * 256;
									colorSpecPtr[groupToPaletteValue].rgb.green = green * 256;
									colorSpecPtr[groupToPaletteValue].rgb.blue = blue * 256;
								#endif
					
								#if defined multispec_wx	
									colorSpecPtr[groupToPaletteValue].rgb.red = red;
									colorSpecPtr[groupToPaletteValue].rgb.green = green;
									colorSpecPtr[groupToPaletteValue].rgb.blue = blue;
								#endif		

								groupToPalettePtr[groupNumber] = (SInt16)classNumber;
								
								}	// end "if (tReturnCode == 5)"			
							
							classToGroupPtr[classNumber] = (SInt16)groupNumber;
								
							}	// end "if (tReturnCode > 1 && groupNumber >= 0 ..."
							
						}	// end "if (startGroupInfoPtr != NULL)"
					
					else	// No group information given for this class
						{
						colorSpecPtr[groupToPaletteValue].rgb.red = 0;
						colorSpecPtr[groupToPaletteValue].rgb.green = 0;
						colorSpecPtr[groupToPaletteValue].rgb.blue = 0;
						
						}	// end "else No group information given for this class"
									
					colorSpecPtr[groupToPaletteValue].value = (SInt16)groupToPaletteValue;
						
					}	// end "if (tReturnCode == 5 && dataValue >= 0 && ..."

				#if defined multispec_win || multispec_wx //09.10.2015
							// Set the end of line character back to the original value.
					
					if (endOfLinePtr != NULL)
						*endOfLinePtr = savedCharacter;
					endOfLinePtr = NULL;  	 
				#endif	// defined multispec_win || multispec_wx
				
				}	// end "if (errCode == noErr && inputStringPtr[0] >= '0' && ..."
			
			whileLoopCount++;
			
			}	// end "while (errCode == noErr)" 
			
				// Allow for group colors and names being identified after the first
				// time that group is specified for the class.
				// Note that this may not handle case when the first group class number
				// is larger than 255 correctly; I think it can happen. Need to create
				// an example for this.
				
		for (classNumber=0; classNumber<maxNumberClasses; classNumber++)
			{
			groupNumber = classToGroupPtr[classNumber];
			
			if (groupNumber >= 0)
				{
				groupToPaletteValue = classNumber % 256;
				firstGroupClassNumber = groupToPalettePtr[groupNumber];					
				colorSpecPtr[groupToPaletteValue].rgb =
															colorSpecPtr[firstGroupClassNumber].rgb;
				
				}	// end "if (classToGroupPtr[classNumber] >= 0)"
						
			}	// end "for (classNumber=0; classNumber<numberclasses; classNumber++)"
			
				// Now get the number of groups that were created. Verify group vector
				// is consistant with the classes that are actually being used. Also
				// take into account classes that may not have been assigned to a group.

				// First reduce class to group pointer to just those classes that exist
				// in the data (or at least in the classSymbol vector.
				// And remove unneeded palette entries.
			
				// Only do the when the number of classes is less than or equal to 256.
				// Otherwise the palette may be needed for when group numbers that have
				// wrapped around.
			
		numberClasses = inputFileInfoPtr->numberClasses;
		if (numberClasses <= 256)
			{
			classNamePtr =
							(CharPtr)GetHandlePointer (inputFileInfoPtr->classDescriptionH);
			classSymbolPtr = (UInt16*)&classNamePtr[numberClasses*sizeof (Str31)];
			for (classNumber=0; classNumber<(SInt32)numberClasses; classNumber++)
				{
				classToGroupPtr[classNumber] =
							classToGroupPtr[classSymbolPtr[classNumber]];
				paletteIndex = classSymbolPtr[classNumber];
				if (paletteIndex >= 256)
					paletteIndex = paletteIndex % 256;
				colorSpecPtr[classNumber].rgb = colorSpecPtr[paletteIndex].rgb;
							
				}	// end "for (classNumber=0; classNumber<numberclasses; classNumber++)"
				
					// Now indicate the size of the color table actually being used.
					
			cTablePtr->ctSize = (SInt16)(numberClasses - 1);
			
			}	// end "if (numberClasses <= 256)"
			
				// Now find the maximum group number. Also determine if a group needs 
				// to be added to handle those classes that were not assigned to a group
				
		groupCount = -1;
		addGroupFlag = FALSE;		
		for (classNumber=0; classNumber<(SInt32)numberClasses; classNumber++)
			{
			groupCount = MAX (groupCount, classToGroupPtr[classNumber]);
			
			if (classToGroupPtr[classNumber] < 0)
				addGroupFlag = TRUE;				
			
			}	// end "for (classNumber=0; classNumber<numberclasses; classNumber++)"
		
				// Now determine which groups actually contain classes that exist and 
				// delete those groups in the vectors that do not exist.
		
		numberGroups = 0;
		if (groupCount >= 0)
			numberGroups = groupCount + 1;
		
		if (numberGroups > 0 && numberGroups <= 256)
			{
			groupNumber = 0;
			notDoneFlag = TRUE;
			while (notDoneFlag)
				{
				groupExistsFlag = FALSE;
				for (classNumber=0; classNumber<(SInt32)numberClasses; classNumber++)
					{
					if (classToGroupPtr[classNumber] == groupNumber)
						{
						groupToPalettePtr[groupNumber] = (SInt16)classNumber;
						groupExistsFlag = TRUE;
						break;
						
						}	// end "if (classToGroupPtr[classNumber] == groupNumber)"
					
					}	// end "for (classNumber=0; classNumber<numberclasses; ..."
					
				if (!groupExistsFlag)
					UpdateGroupTables ((SInt16)groupNumber,
												classToGroupPtr,
												groupToPalettePtr,
												(SInt16)numberClasses,
												&numberGroups,
												groupNameHandle);
				
				else	// groupExistsFlag
					groupNumber++;
					
				if (groupNumber >= (SInt32)numberGroups)
					notDoneFlag = FALSE;
				
				}	// end "while (doneFlag)"
					
					// Add a group if needed to include those classes that were not
					// assigned to groups.
					
			if (addGroupFlag)
				{
				groupNumber = numberGroups;
				numberGroups++;
				firstTimeFlag = TRUE;
				for (classNumber=0; classNumber<(SInt32)numberClasses; classNumber++)
					{
					if (classToGroupPtr[classNumber] < 0)
						{
						classToGroupPtr[classNumber] = (SInt16)groupNumber;
						
						if (firstTimeFlag)
							{
							groupToPalettePtr[groupNumber] = (SInt16)classNumber;
							firstTimeFlag = FALSE;
							
							}	// end "if (firstTimeFlag)"
						
						}	// end "if (classToGroupPtr[classNumber] < 0)"
									
					}			// end "for (classNumber=0; classNumber<numberClasses; ..."
					
						// Add the name for the group
						
				localGroupNamePtr = &groupNamePtr[groupNumber*32];
				BlockMoveData ("No group assignment", &localGroupNamePtr[1], 19);
				localGroupNamePtr[0] = (UInt8)19;
				
				}	// end "if (addGroupFlag)"
				
			}	// end "if (numberGroups > 0 && numberGroups <= 256)"
			
				// Indicate that groups have been created.				

		if (numberGroups > 0 && numberGroups <= 256)
			{
			returnFlag = TRUE;
			inputFileInfoPtr->ancillaryInfoformat = kArcViewDefaultSupportType;
			/*
					// Now need to move the classToGroupPtr information to the memory location
					// that it is expected to be in.
					// Do not need to do this now. It is in the correct location.
			
			if (numberClasses < (UInt32)maxNumberClasses)
				{
				newClassToGroupPtr = &groupToPalettePtr[numberClasses];
				for (classNumber=0; classNumber<(SInt32)numberClasses; classNumber++)
					newClassToGroupPtr[classNumber] = classToGroupPtr[classNumber];
					
				}	// end "if (numberClasses < maxNumberClasses)"
			*/						
			}	// end "if (numberGroups > 0 && numberGroups <= 256)"
			
		else	// numberGroups == 0
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
		outputFileInfoPtr->numberGroups = (UInt16)numberGroups;
		
		UnlockAndDispose ((Handle)displaySpecsPtr->savedGroupCTableHandle);
		displaySpecsPtr->savedGroupCTableHandle = cTableHandle;
		
		UnlockAndDispose (outputFileInfoPtr->groupNameHandle);
		outputFileInfoPtr->groupNameHandle = groupNameHandle;
		
		UnlockAndDispose (outputFileInfoPtr->groupTablesHandle);
		outputFileInfoPtr->groupTablesHandle = groupTablesHandle;
		
				// Get the number of entries in the palette that are used for		
				// classes/groups.													
				
		displaySpecsPtr->numPaletteEntriesUsed = 
									(UInt16)MIN (256, displaySpecsPtr->numberPaletteLevels);
		displaySpecsPtr->paletteOffset = 0;
		
		gPaletteOffset = displaySpecsPtr->paletteOffset;
		gClassPaletteEntries = displaySpecsPtr->numPaletteEntriesUsed;		
		
		}	// end "if (continueFlag)"
	
	else	// !continueFlag
		{
		UnlockAndDispose ((Handle)cTableHandle);
		UnlockAndDispose (groupNameHandle);
		UnlockAndDispose (groupTablesHandle);
		
		}	// end "if (!continueFlag)" 
			
	CheckAndUnlockHandle ((Handle)displaySpecsPtr->savedGroupCTableHandle);
	CheckAndUnlockHandle (outputFileInfoPtr->groupNameHandle);
	CheckAndUnlockHandle (outputFileInfoPtr->groupTablesHandle); 
		
	if (checkIOErrorFlag)
		IOCheck (errCode, supportFileStreamPtr);
		  				
	CloseFile (supportFileStreamPtr);
		
	if (gProcessorCode != kOpenThematicInfo)
		UnlockSupportFileStream (windowInfoHandle, supportHandleStatus);
		
	return (returnFlag);
	
}	// end "ReadArcViewGroups"

			

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadArcViewHeader
//
//	Software purpose:	This routine reads the file specifications from an ARCView header 
//							file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ArcView header record parameters make sense
//							1 - if ArcView header record paramters do not make sense.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2000
//	Revised By:			Larry L. Biehl			Date: 04/10/2020

SInt16 ReadArcViewHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	CMFileStream						headerFileStream;
	
	char									headerRecord[1040];
	
	CharPtr								stringPtr;
	
	FileStringPtr						headerFileNamePtr,
											imageFileNamePtr;
											
	CMFileStream						*fileStreamPtr,
											*headerFileStreamPtr;
	
	SInt32								value;
											
	UInt32								count;
	
	SInt16								errCode,
											fileNameCode = 0,
											fileType = 0,
											formatErrorCode = noErr,
											returnCode = 1,
											tReturnCode;
											
	Boolean								geoSpotHeaderFlag,
											mapInfoExistsFlag = FALSE;
	
			
	if (headerRecordPtr != NULL && fileInfoPtr != NULL)		
		{
				// Now check if this is an ArcView image file. This may be the case
				// if the suffix of the file is .bil, .bsq, .bip, or .hdr.
		
		FileStringPtr		filePathPtr;
		
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		
		if (CompareSuffixNoCase ((char*)"\0.bil", 
											filePathPtr,
											NULL))
			fileNameCode = 1;
			
		else if (CompareSuffixNoCase ((char*)"\0.bsq", 
													filePathPtr,
													NULL))
			fileNameCode = 2;
			
		else if (CompareSuffixNoCase ((char*)"\0.bip", 
													filePathPtr,
													NULL))
			fileNameCode = 3;
			
		else if (CompareSuffixNoCase ((char*)"\0.hdr", 
													filePathPtr,
													NULL))
			fileNameCode = 4;
			
		if (fileNameCode > 0)
			fileType = kArcViewType;
			
		}	// end "if (headerRecordPtr != NULL && fileInfoPtr != NULL)"
		
	if (fileType != 0)
		{
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			fileInfoPtr->thematicType = FALSE;
																					return (noErr);
																								
			}	// end "if (formatOnlyCode != kLoadHeader)"
			
		errCode = noErr;
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
				// Get pointer to the file name and close the input file. If it is
				// a header file, the information has already been read. If it is
				// an image file then we don't need it for now and need to look for
				// the ArcView header file.
			
		imageFileNamePtr = 
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		
		if (fileNameCode == 4)
			{
					// Close the input file. It is a header file and it has already
					// been ready. We don't need it any more.
					
			CloseFile (fileStreamPtr);
		
					// The header file has been read. 
					// Remove the ".hdr" at the end of the file name.
				
			RemoveCharsNoCase ((char*)"\0.hdr\0", imageFileNamePtr);
		
					// Determine what the 'LAYOUT' of the file is to determine the
					// suffix to look for for the ArcView image file.	
			
			stringPtr = headerRecordPtr;	
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO129,
															&stringPtr, 
															3, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);
								
			if (tReturnCode == 0)
				{
						// Get the band interleave format
		      
				if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"BIL", 3) == 0)
					ConcatFilenameSuffix (imageFileNamePtr, (UCharPtr)"\0.bil\0");
					
				else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"BSQ", 3) == 0)
					ConcatFilenameSuffix (imageFileNamePtr, (UCharPtr)"\0.bsq\0");
					
				else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"BIP", 3) == 0)
					ConcatFilenameSuffix (imageFileNamePtr, (UCharPtr)"\0.bip\0");
					
				else	// not known
					errCode = -1;
					
				}	// end "if (tReturnCode == 0)"
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
		
					// This will be an attempt to read a file with the expected suffix.
			
			if (errCode == noErr)			
				errCode = OpenFileReadOnly (fileStreamPtr, 
														kResolveAliasChains, 
														kLockFile,
														kVerifyFileStream);
				
			}	// end "if (fileNameCode == 4)"
			
		else	// fileNameCode != 4
			{
					// This may be an ArcView image file, find the header file for it.
					
			headerFileStreamPtr = &headerFileStream;		
			InitializeFileStream (headerFileStreamPtr, fileStreamPtr);
			headerFileNamePtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (headerFileStreamPtr);
			
					// Remove the ".bil", ".bsq", ".bip" at the end.
					
			RemoveCharsNoCase ((char*)"\0.???\0", headerFileNamePtr);
		
					// Try getting file with a name that has ".hdr" suffix at the end.
				
			ConcatFilenameSuffix (headerFileNamePtr, (StringPtr)"\0.hdr\0");
					
			errCode = OpenFileReadOnly (headerFileStreamPtr, 
													kResolveAliasChains, 
													kLockFile,
													kVerifyFileStream);
		
			count = 1024;
			if (errCode == noErr)
				{
				errCode = MReadData (headerFileStreamPtr, 
												&count, 
												&headerRecord[0], 
												kNoErrorMessages);
												
				if (errCode == eofErr && count > 10)
					errCode = noErr;
				
				if (errCode == noErr)
					{	
					headerRecord[count] = 0;
					headerRecordPtr = &headerRecord[0];
					
					}	// end "if (errCode == noErr)"
				
				CloseFile (headerFileStreamPtr);
				
				}	// end "if (errCode == noErr)"
			
			}	// end "else fileNameCode != 4"
			
				// If there has been no error to this point then try to read the
				// ArcView header to see if the needed parameters are there.
		
		if (errCode == noErr)
			{
					// Find "NCOLS" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO125,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual,
													&tReturnCode);
								
			if (tReturnCode == 0 && value > 0)
				fileInfoPtr->numberColumns = value;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Find "NROWS" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO126,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0 && value > 0)
				fileInfoPtr->numberLines = value;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr)
			{
					// Find "NBANDS" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO127,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0 && value > 0)
				fileInfoPtr->numberChannels = (UInt16)value;
			
			else if (tReturnCode == 6)		// nbands is not found; default is 1.
				fileInfoPtr->numberChannels = 1;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)"
		
		if (errCode == noErr)
			{
					// Find "NBITS" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO128,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0)
				{
				if (value >= 8 && value <= 64)
					{
					fileInfoPtr->numberBits = 8;
					fileInfoPtr->numberBytes = 1;
						
					if (value == 16)
						{
						fileInfoPtr->numberBits = 16;
						fileInfoPtr->numberBytes = 2;
						
						}	// end "if (value == 16)"
						
					else if (value == 32)
						{
						fileInfoPtr->numberBits = 32;
						fileInfoPtr->numberBytes = 4;
						
						}	// end "else if (value == 32)"
						
					else if (value == 64)
						{
						fileInfoPtr->numberBits = 64;
						fileInfoPtr->numberBytes = 8;
						
						}	// end "else if (value == 32)"
							
					}	// end "if (tReturnCode == 0)"
					
				else	// value < 8 || value > 64
					{
					fileInfoPtr->numberBytes = 1;
					
					if (value < 8)
						formatErrorCode = 4;

					else	// value > 64
						{
						formatErrorCode = 4;

						}	// end "else value > 4"

					}	// end "else value < 8 || value > 32"

				}	// end "if (tReturnCode == 0)"
			
			else if (tReturnCode == 5)	// nbits is not found; default is 8.
				{
				fileInfoPtr->numberBits = 8;
				fileInfoPtr->numberBytes = 1;
				
				}	// end "else if (tReturnCode == 5) nbits is not found; default is 8."
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)" 
		
		if (errCode == noErr)
			{
					// Find "LAYOUT" in the buffer.								
			
			stringPtr = headerRecordPtr;	
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO129,
															&stringPtr, 
															3, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);
								
			if (tReturnCode == 0)
				{
						// Get the band interleave format
		      
				if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"BIL", 3) == 0)
					fileInfoPtr->bandInterleave = kBIL;
					
				else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"BSQ", 3) == 0)
					fileInfoPtr->bandInterleave = kBSQ;
					
				else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"BIP", 3) == 0)
					fileInfoPtr->bandInterleave = kBIS;
					
				else	// not known
					errCode = -1;
					
				}	// end "if (tReturnCode == 0)"
			
			else if (tReturnCode == 6)		// layout is not found; default is bil.
				fileInfoPtr->bandInterleave = kBIL;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr)"     
		
		if (errCode == noErr && fileInfoPtr->numberBytes >= 2)
			{
					// Find "BYTEORDER" in the buffer.	
					
			stringPtr = headerRecordPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO130,
															&stringPtr, 
															1, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);
								
			if (tReturnCode == 0)
				{              
				if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"M", 1) == 0)
					fileInfoPtr->swapBytesFlag = !gBigEndianFlag; 
					
				else	// CompareStringsNoCase ((char*)&gTextString[1], "M", 1) != 0
					fileInfoPtr->swapBytesFlag = gBigEndianFlag;
				
				}	// end "if (tReturnCode == 0)"
			
			else if (tReturnCode == 9)		// byteorder is not found; default is bil.
				fileInfoPtr->swapBytesFlag = gBigEndianFlag;
				
			else	// tReturnCode != 0
				errCode = tReturnCode;
			
			}	// end "if (errCode == noErr && fileInfoPtr->numberBytes >= 2)"   
		
		if (errCode == noErr)
			{
					// Find "SKIPBYTES" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO131,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0 && value > 0)
				//fileInfoPtr->numberPostLineBytes = value;
				fileInfoPtr->numberHeaderBytes = value;
			
			}	// end "if (errCode == noErr)"    
		
		if (errCode == noErr && fileInfoPtr->bandInterleave == kBIL)
			{
					// Find "BANDROWBYTES" in the buffer.								
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO148,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0 && 
					value > 0 && 
					value > (SInt32)(fileInfoPtr->numberColumns*fileInfoPtr->numberBytes))
				fileInfoPtr->numberPostChannelBytes = value - 
										fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;
			
			}	// end "if (errCode == noErr && ...)"    
		
		if (errCode == noErr && fileInfoPtr->bandInterleave == kBIL)
			{
					// Find "NODATA" in the buffer. Use this as a clue to whether the data 
					// is signed or unsigned.						
				
			value = GetFileHeaderValue (kFileIOStrID, 
													IDS_FileIO180,
													headerRecordPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
								
			if (tReturnCode == 0 && value < 0)
				fileInfoPtr->signedDataFlag = TRUE;
			
			}	// end "if (errCode == noErr && ...)"  
		
		if (errCode == noErr)
			{
					// Find "DATAVALUETYPE" in the buffer. This is not a true ArcView 
					// keyword. I added to make things easier for the users of MultiSpec								
				
			stringPtr = headerRecordPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO149,
															&stringPtr, 
															6, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);
								
			if (tReturnCode == 0)
				{
						// Get the band interleave format
		      
				if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"SIGNED", 6) == 0)
					fileInfoPtr->signedDataFlag = TRUE;
					
				}	// end "if (tReturnCode == 0)"
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr)
			{
					// Find "PIXELTYPE" in the buffer if it is there								
				
			stringPtr = headerRecordPtr;
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO203,
															&stringPtr, 
															6, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);
								
			if (tReturnCode == 0)
				{
						// Get the band interleave format
		      
				if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"SIGNED", 6) == 0)
					fileInfoPtr->signedDataFlag = TRUE;
		      
				if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"FLOAT", 5) == 0)
					fileInfoPtr->dataTypeCode = kRealType;
					
				}	// end "if (tReturnCode == 0)"
			
			}	// end "if (errCode == noErr)"  
		
		if (errCode == noErr)
					// Check if ArcView map information exists. 
					
			mapInfoExistsFlag = GetArcViewMapInformation (fileInfoPtr, 
																			headerRecordPtr, 
																			&geoSpotHeaderFlag);
		
		if (errCode == noErr)
			{
			if (formatErrorCode == noErr)
				{
				returnCode = 0;
			
						// Determine the image type if it has not been determined yet.
						
				if (gGetFileImageType == 0)
					{
					gGetFileImageType = kImageWindowType;
					fileInfoPtr->numberClasses = 0;
					
					}	// end "if (gGetFileImageType == 0)"
						
				fileInfoPtr->format = kArcViewType;
				
						// For now the ancillary informaton will come from the ERDAS .trl
						// file.
				
				fileInfoPtr->ancillaryInfoformat = kErdas74Type;
				
				fileInfoPtr->thematicType = FALSE;
				if (gGetFileImageType == kThematicImageType)
					fileInfoPtr->thematicType = TRUE;
					
						// Make sure the data type is real if the number of bytes is 8.
						
				if (fileInfoPtr->numberBytes == 8)
					fileInfoPtr->dataTypeCode = kRealType;
					
						// Get map information from a world file if it exists. It will 
						// override any that has already been loaded.
						// Now being read later
				
				//ReadArcViewWorldFile (fileInfoPtr);
				
						// Get projection information from a projection (prj) file if it
						// exists.
						
				//ReadGTOPO30PrjFile (fileInfoPtr, headerRecordPtr);

				}	// end "if (formatErrorCode == noErr)"
			/*
			else	// formatErrorCode != noErr
				{
						// Display an alert if MultiSpec cannot read this type of ArcView
						//	file.						

				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert69 + formatErrorCode,
									0, 
									NULL);
									
				}	// end "if (formatErrorCode != noErr)" 
			*/			
			}	// end "if (errCode == noErr)"
			
		if (returnCode != 0 && fileNameCode == 4)
			{
					// Add the .hdr suffix back to the name. It may be needed later.
					
			ConcatFilenameSuffix (imageFileNamePtr, (UCharPtr)"\0.hdr\0");
			
			}	// end "if (returnCode != 0 && fileNameCode == 4)"
		
		}	// end "if (fileType != 0)"
		
	return (returnCode);
	
}	// end "ReadArcViewHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ReadArcViewWorldFile
//
//	Software purpose:	This routine reads the information in an ARCView blw 
//							file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	true - if ArcView blw file found and read.
//							false - if ArcView blw file not found and read.
//
// Called By:			ReadArcViewHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/11/2000
//	Revised By:			Larry L. Biehl			Date: 04/11/2020

Boolean ReadArcViewWorldFile (
				FileInfoPtr 						fileInfoPtr)

{
	CMFileStream						blwFileStream;
	
	char									blwRecord[1040];
	
	CharPtr								blwRecordPtr;
	
	FileStringPtr						blwFilePathNamePtr;
											//*imageFileNamePtr;

	CMFileStream						*fileStreamPtr,
											*blwFileStreamPtr;
											
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	UInt8									suffix[8];
	
	double								dataValue1,
											dataValue2,
											dataValue3,
											dataValue4,
											dataValue5,
											dataValue6;
											
	UInt32								count;
	
	SInt16								errCode,
											numberCharacters,
											returnCode;
											
	UInt16								numChars;
											
	Boolean								fileReadFlag = FALSE;
	

	if (fileInfoPtr != NULL)
		{
		errCode = noErr;
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
				// Get pointer to the file name and close the input file. If it is
				// a header file, the information has already been read. If it is
				// an image file then we don't need it for now and need to look for
				// the ArcView header file.
			
		//imageFileNamePtr = (FileStringPtr)GetFilePathPPointerFromFileStream (
		//														fileStreamPtr, &filePathNameLength);
		
				// The suffix for the planar map information file consists of the
				// 1st and 3rd characters in the image file suffix plus w as the third
				// charcter. In other words the planar map information file for
				// and image file named "image.bil" will be "image.blw".
				//
				// If no suffix exist just use the 3rd to last and last character
				// of the image file with w as the last character for the planar
				// map information file suffix.
					
		blwFileStreamPtr = &blwFileStream;		
		InitializeFileStream (blwFileStreamPtr, fileStreamPtr);
		blwFilePathNamePtr = (FileStringPtr)GetFilePathPPointerFromFileStream (
																						blwFileStreamPtr);
		
		//numberCharacters = blwFilePathNamePtr[0];
		numberCharacters = GetFileStringLength (blwFilePathNamePtr);
		suffix[0] = 4;	
		suffix[1] = '.';
		suffix[3] = blwFilePathNamePtr[numberCharacters+1];
		suffix[4] = 'w';
		suffix[5] = 0;		
		
				// Check if the image file contains the suffix .tiff.
				
		if (CompareSuffixNoCase ((char*)"\0.tiff\0",
											blwFilePathNamePtr,
											&numChars))
			{
			suffix[2] = blwFilePathNamePtr[numberCharacters-2];
							
			RemoveCharsNoCase ((char*)"\0.tiff\0", blwFilePathNamePtr);
			
			}	// end "if (CompareSuffixNoCase ("\0.tiff\0", ..."
		
		else	// suffix is not .tiff
			{
			suffix[2] = blwFilePathNamePtr[numberCharacters-1];
							
			RemoveCharsNoCase ((char*)"\0.???\0", blwFilePathNamePtr);
			
			}	// end "else suffix is not .tiff"
		
		ConcatFilenameSuffix (blwFilePathNamePtr, suffix);
					
		errCode = OpenFileReadOnly (blwFileStreamPtr, 
												kResolveAliasChains, 
												kLockFile,
												kVerifyFileStream);

		if (errCode != noErr)
			{
						// Try with uppercase 'W'

			suffix[4] = 'W';
			RemoveCharsNoCase ((char*)"\0.???\0", blwFilePathNamePtr);
			ConcatFilenameSuffix (blwFilePathNamePtr, suffix);

			errCode = OpenFileReadOnly (blwFileStreamPtr, 
												kResolveAliasChains, 
												kLockFile,
												kVerifyFileStream);

			}	// end "if (errCode != noErr)"
		
		count = 1024;
		if (errCode == noErr)
			{
			errCode = MReadData (blwFileStreamPtr, 
											&count, 
											&blwRecord[0], 
											kNoErrorMessages);
											
			if (errCode == eofErr && count > 20)
				errCode = noErr;
			
			if (errCode == noErr)
				{	
				blwRecord[count] = 0;
				blwRecordPtr = &blwRecord[0];
				
				}	// end "if (errCode == noErr)"
			
			CloseFile (blwFileStreamPtr);
			
			}	// end "if (errCode == noErr)"
			
				// If there has been no error to this point then try to read the
				// ArcView blw information. There should be 6 floating point 
				// values
		
		if (errCode == noErr)
			{
					// Get the horizontal (column) interval.	
					
			#if defined multispec_wx
            CharPtr savedcharptr = blwRecordPtr;
			#endif
			
					// Check if numbers in the file are in interenational numeric
					// format. I.e. a comma exists.
			
			char*		commaPtr;
			commaPtr = strchr (blwRecordPtr, ',');
			if (commaPtr != NULL)
				{
						// Assume that the values in the file are in international
						// numberic format. Change commas to periods.
				
				while ((commaPtr = strchr (blwRecordPtr, ',')) != NULL)
					*commaPtr = '.';
				
				}	// end "if (commaPtr != NULL)"
			
			returnCode = sscanf (blwRecordPtr,
										"%lf %lf %lf %lf %lf %lf",
										&dataValue1,
										&dataValue2,
										&dataValue3,
										&dataValue4,
										&dataValue5,
										&dataValue6);
			
			//setlocale (LC_NUMERIC, saved_locale);
									
			#if defined multispec_wx
				blwRecordPtr = savedcharptr;
			#endif
								
			if (returnCode == 6)
				{
				if (fileInfoPtr->mapProjectionHandle == NULL)
					fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
				
				if (fileInfoPtr->mapProjectionHandle != NULL)
					{ 								
					mapProjectionInfoPtr = (MapProjectionInfoPtr)
											GetHandlePointer (fileInfoPtr->mapProjectionHandle);
															
					LoadPlanarCoordinates (mapProjectionInfoPtr,
													dataValue1,
													dataValue2,
													dataValue3,
													dataValue4,
													dataValue5,
													dataValue6);
						
					}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
				
				}	// end "if (returnCode == 6)"
				
			else	// tReturnCode != 0
				errCode = 1;
			
			}	// end "if (returnCode == 6)" 
			
		fileReadFlag = (errCode == noErr);
		
		}	// end "if (fileInfoPtr != NULL && ...)"
		
	return (fileReadFlag);
	
}	// end "ReadArcViewWorldFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadArcViewShapeFile
//
//	Software purpose:	This routine reads the shape file information into memory.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	=1, not shape file format
//							=5, shape file does not intersect with window
//							=6, shape file may be corrupted
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/21/2000
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 ReadArcViewShapeFile (
				SInt16		 						shapeFileIndex,
				Boolean								convertToMapUnitsFlag,
				Boolean								okayToCloseOverlayFileFlag)

{
	double								doubleNumberBytes,
											longitudeCentralMeridian;
	
	DoublePoint							doublePoint;
	
	//SDoubleRect							boundingBox;
	DoubleRect							boundingBox,
											boundingBox2,
											boundingBox3;
	
	SInt64								posOff;
	
	UCharPtr								bufferPtr = NULL,
											ioBufferPtr = NULL;
	
	ArcViewMultiPointPtr				arcViewMultiPointPtr;
	ArcViewPointPtr					arcViewPointPtr;
	ArcViewPolyLinePtr				arcViewPolyLinePtr;
	ArcViewRecordHeaderPtr			arcViewRecordHeaderPtr;
	CMFileStream*						shapeFileStreamPtr = NULL;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	SDouble*								doublePtr;
	ShapeInfoPtr						shapeInfoPtr;
											
	Handle*								handlePtr;
	
	Handle								mapProjectionHandle,
											shapeFileHandle;
											
	SInt32								numberRecords = 0,
											recordNumber,
											shapeType;
											
	UInt32								bufferIndex,
											bufferStart,
											count,
											index,
											numberBytes,
											numberParts,
											numberPoints,
											outputIndex,
											recordLength,
											totalCount;	
	
	SInt16								errCode = noErr,
											overlayNumber,
											returnCode = 1;
	
	Boolean								includeFlag,
											notDoneFlag,
											quitFlag = FALSE;
	
	
	if (shapeFileIndex < 0)
																					return (returnCode);
						
			// Set wait cursor on

	MSetCursor (kWait);
																					
	overlayNumber = shapeFileIndex + 1;
																					
	handlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle);
															
	shapeFileHandle = handlePtr[shapeFileIndex];
	
	shapeInfoPtr = NULL;
	if (shapeFileHandle != NULL)
		{
		shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeFileHandle,
																					kLock);
							
		shapeFileStreamPtr = GetFileStreamPointer (shapeInfoPtr);
		
		}	// end "if (shapeFileHandle != NULL)"
		
	if (shapeFileStreamPtr != NULL)
		bufferPtr = (UCharPtr)GetHandlePointer (shapeInfoPtr->vectorDataHandle,
															kLock);
		
	if (bufferPtr != NULL)
		{																			
				// Get pointer to the map projection structure if we need to convert
				// to map units.									
			
		mapProjectionHandle = GetFileMapProjectionHandle2 (gActiveImageWindowInfoH);											
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																				mapProjectionHandle,
																				kLock);
																	
				// Open the shape file.
							
		errCode = OpenFileReadOnly (shapeFileStreamPtr, 
												kResolveAliasChains, 
												kLockFile,
												kDoNotVerifyFileStream);
													
				// Make certain that we are at the beginning of the file.
		
		posOff = 100;
		if (errCode == noErr)
			errCode = MSetMarker (
									shapeFileStreamPtr, fsFromStart, posOff, kErrorMessages);
		
				// Read the shape file records.
		
		numberBytes = shapeInfoPtr->fileLength - 100;
		
		notDoneFlag = TRUE;
		ioBufferPtr = bufferPtr;
		totalCount = 0;
		
				// Determine if the status dialog box needs to be displayed.
				
		gNextTime = SInt32_MAX;
		gNextStatusTime = TickCount () + gNextStatusTimeOffset;
		
		if (numberBytes > 4000000)
			{
					// Get status information dialog box.									
						
			gStatusDialogPtr = GetStatusDialog (kGraphicStatusDialogID, 
															TRUE);
			
			LoadDItemStringNumber (kFileIOStrID, 
											IDS_FileIO138, 
											gStatusDialogPtr, 
											IDC_Status2, 
											(Str255*)gTextString);
											
			CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);
											
			gNextTime = TickCount ();
			
			}	// end "if (numberBytes > 4000000)"
			
		while (errCode == noErr && notDoneFlag)
			{
			count = MIN (4000000, numberBytes);
			errCode = MReadData (shapeFileStreamPtr, &count, ioBufferPtr, kErrorMessages);
			
			totalCount += count;
			
			if (count < numberBytes)
				{
				ioBufferPtr += count;
				numberBytes -= count;
				
				}	// end "if (count < numberBytes)"
				
			else	// count >= numberBytes
				notDoneFlag = FALSE;
					
			if (gStatusProgressControlHandle != NULL)		
				MSetControlValue (
								(WindowPtr)gStatusDialogPtr,
								gStatusProgressControlHandle,
								(SInt32)((double)totalCount/shapeInfoPtr->fileLength*100));
										
			if (!CheckSomeEvents (osMask+keyDownMask+mDownMask+mUpMask))
				{
      		notDoneFlag = FALSE;
      		quitFlag = TRUE;
				break;
				
				}	// end "if (!CheckSomeEvents (osMask + ..." 
			
			}	// end "while (errCode == noErr && notDoneFlag)"
			
		numberBytes = totalCount;
			
		if (!quitFlag && errCode == noErr)
			{
					// Now determine the number of records in the file that intersect
					// with the area represented by the image window(s).
					
			notDoneFlag = TRUE;
			bufferStart = 0;
			outputIndex = 0;
			
			while (notDoneFlag)
				{
				includeFlag = FALSE;
						
						// Get record number and length of record.
						
				gSwapBytesFlag = !gBigEndianFlag; 
		      recordNumber = GetLongIntValue ((char*)&bufferPtr[bufferStart]);
		      recordLength = GetLongIntValue ((char*)&bufferPtr[bufferStart+4]);
		      
		      		// Convert 16-bit length to 8-bit byte length.
		      		
		      recordLength *= 2;
		      
		      		// Get the shape type for the record.
		      		
				gSwapBytesFlag = gBigEndianFlag;
		      shapeType = GetShortIntValue ((char*)&bufferPtr[bufferStart+8]);
		      
		      switch (shapeType)
		      	{
		      	case 0:	// Null shape
		      		break;
		      		
		      	case 1:	// Point shape
		      	case 11:	// PointZ shape
		
								// Get the point for the shape
				      
						includeFlag = TRUE;
				      boundingBox.left = 
				      				GetShortDoubleValue (&bufferPtr[bufferStart+12]);
				      boundingBox.top = 
				      				GetShortDoubleValue (&bufferPtr[bufferStart+20]);
				      boundingBox.right = boundingBox.left;
				      boundingBox.bottom = boundingBox.top;
				      
				      if (convertToMapUnitsFlag)
					      includeFlag = 
					      	ConvertLatLongRectToMapRect (mapProjectionInfoPtr,
																		&boundingBox);
				      
				      if (includeFlag)
				      	includeFlag = ShapeAndWindowAreasIntersect (
				      												overlayNumber, &boundingBox);
		      		break;
		      		
		      	case 3:	// PolyLine shape
		      	case 5:	// Polygon shape
		      	case 8:	// MultiPoint shape
		      	case 13:	// PolyLineZ shape
		      	case 15:	// PolygonZ shape
		      	case 18:	// MultiPointZ shape
		      	case 21:	// PointM shape
		      	case 23:	// PolyLineM shape
		      	case 25:	// PolygonM shape
		      	case 28:	// MultiPointM shape
		      	case 31:	// MultiPatch shape
		
								// Get the Bounding Box for the shape
				      
						includeFlag = TRUE;
				      boundingBox.left = 
				      				GetShortDoubleValue (&bufferPtr[bufferStart+12]);
				      boundingBox.top = 
				      				GetShortDoubleValue (&bufferPtr[bufferStart+20]);
				      boundingBox.right = 
				      				GetShortDoubleValue (&bufferPtr[bufferStart+28]);
				      boundingBox.bottom = 
				      				GetShortDoubleValue (&bufferPtr[bufferStart+36]);
				      
				      if (convertToMapUnitsFlag)
							{
									// Note that one needs to check all four corners to be
									// sure that the bounding box does include all four
									// corners.
									
							boundingBox2 = boundingBox;
							boundingBox3 = boundingBox;
							
							boundingBox2.top = boundingBox.bottom;
							boundingBox2.bottom = boundingBox.top;
									
					      includeFlag = 
					      		ConvertLatLongRectToMapRect (mapProjectionInfoPtr,
																			&boundingBox);
									
							if (includeFlag)
								includeFlag = 
					      		ConvertLatLongRectToMapRect (mapProjectionInfoPtr,
																			&boundingBox2);
									
							if (includeFlag)
								{
								boundingBox.left = MIN (boundingBox.left, boundingBox2.left);
								boundingBox.right = MAX (boundingBox.right, boundingBox2.right);
								boundingBox.top = MIN (boundingBox.top, boundingBox2.bottom);
								boundingBox.bottom = MAX (boundingBox.bottom, boundingBox2.top);
								
								}	// end "if (includeFlag)"
								
									// Also need to allow for central meridian if within the
									// bounding box. The lowest point in map units may not be
									// the corners.
							
							longitudeCentralMeridian =
								mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
							if (longitudeCentralMeridian == 0)
								longitudeCentralMeridian =
									mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude;
								
							if (includeFlag &&
									longitudeCentralMeridian < boundingBox3.right &&
										longitudeCentralMeridian > boundingBox3.left)
								{
								doublePoint.h = longitudeCentralMeridian; 
								doublePoint.v = MIN (boundingBox3.bottom, boundingBox3.top);
								includeFlag = ConvertLatLongPointToMapPoint (
																mapProjectionInfoPtr, &doublePoint);
								
								if (includeFlag)
									boundingBox.top = MIN (boundingBox.top, doublePoint.v);
									
								}	// end "if (includeFlag)"

																	
							}	// end "if (convertToMapUnitsFlag)"
				      
				      if (includeFlag)
				      	includeFlag = ShapeAndWindowAreasIntersect (
																		overlayNumber, &boundingBox);
		      		break;
		      	
		      	}	// end "switch (shapeType)"
		      	
		      if (includeFlag)
		      	{
		      			// Convert data values to the format that is normal to the
		      			// machine being used.
		      	
		      	arcViewRecordHeaderPtr = 
		      								(ArcViewRecordHeaderPtr)&bufferPtr[outputIndex];
		      								
		      	arcViewRecordHeaderPtr->recordNumber = recordNumber;
		      	arcViewRecordHeaderPtr->recordLength = recordLength + 8;
		      	arcViewRecordHeaderPtr->shapeType = shapeType;
		      			
		      	switch (shapeType)
		      		{
		      		case 1:	// Point shape
		      		case 11:	// PointZ shape
		      			arcViewPointPtr = (ArcViewPointPtr)arcViewRecordHeaderPtr;
		      			arcViewPointPtr->point.x = boundingBox.left;
		      			arcViewPointPtr->point.y = boundingBox.top;
		      			break;
		      			
		      		case 3:	// PolyLine shape
		      		case 5:	// Polygon shape
		      		case 15:	// PolygonZ shape
							numberParts =
										GetLongIntValue ((char*)&bufferPtr[bufferStart+44]);
							numberPoints =
										GetLongIntValue ((char*)&bufferPtr[bufferStart+48]);
		      			doubleNumberBytes =
										(double)44 + numberParts * 4 + numberPoints * 16;
		      			if (shapeType == 15)
		      				{
		      				doubleNumberBytes += (double)16 + numberPoints * 8;
								
		      				if (doubleNumberBytes < recordLength)
											// M information may be available. Add that.
		      					doubleNumberBytes += (double)16 + numberPoints * 8;
									
		      				}	// end "if (shapeType == 15)"
		      																
		      			if (doubleNumberBytes == (double)recordLength)
		      				{
			      			arcViewPolyLinePtr =
													(ArcViewPolyLinePtr)arcViewRecordHeaderPtr;
			      			arcViewPolyLinePtr->box = boundingBox;
			      			arcViewPolyLinePtr->numParts = numberParts;
			      			arcViewPolyLinePtr->numPoints = numberPoints;
			      													
			      			bufferIndex = bufferStart + 52;
			      			for (index=0; index<numberParts; index++)
			      				{
			      				arcViewPolyLinePtr->parts[index] = 
												GetLongIntValue ((char*)&bufferPtr[bufferIndex]);
			      				bufferIndex += 4;
			      				
			      				}	// end "for (index=0; index<..."
			      				
			      					// Now move the points
			      						
			      			doublePtr = (SDouble*)&arcViewPolyLinePtr->parts[
			      													arcViewPolyLinePtr->numParts];
			      			
			      			count = 2 * numberPoints;								
			      			for (index=0; index<count; index++)
			      				{
			      				*doublePtr = GetShortDoubleValue (&bufferPtr[bufferIndex]);
			      				doublePtr++;
			      				bufferIndex += 8;
			      
			      				}	// end "for (index=0; index<count; index++)"
			      				
			      			if (convertToMapUnitsFlag)
				      			{	
				      			doublePtr = (SDouble*)&arcViewPolyLinePtr->parts[
																		arcViewPolyLinePtr->numParts];
			      											
				      			for (index=0; index<numberPoints; index++)
				      				{
										ConvertLatLongPointToMapPoint (mapProjectionInfoPtr, 
																	(ArcViewDoublePointPtr)doublePtr);
				      				doublePtr += 2;
				      
				      				}	// end "for (index=0; index<count; index++)"
				      				
				      			}	// end "if (convertToMapUnitsFlag)"
			      				
			      			}	// end "if (doubleNumberBytes == (double)recordLength)"
		      				
		      			else	// doubleNumberBytes != (double)recordLength
		      				{
		      				includeFlag = FALSE;
								returnCode = 6;
								
								}	// end "else doubleNumberBytes != (double)recordLength"
		      				
		      			break;
		      			
		      		case 8:	// MultiPoint shape
		      			arcViewMultiPointPtr = 
		      									(ArcViewMultiPointPtr)arcViewRecordHeaderPtr;
		      			
		      			arcViewMultiPointPtr->box = boundingBox;
		      			arcViewMultiPointPtr->numPoints = 
		      							GetLongIntValue ((char*)&bufferPtr[bufferStart+44]);
		      			
		      			doubleNumberBytes = 48 + arcViewMultiPointPtr->numPoints * 16;			
		      			if (doubleNumberBytes == (double)recordLength)
		      				{
			      					// Now move the points
			      						
			      			bufferIndex = bufferStart + 48;
			      			doublePtr = (SDouble*)&arcViewMultiPointPtr->points[0];
			      			
			      			count = 2 * arcViewMultiPointPtr->numPoints;								
			      			for (index=0; index<count; index++)
			      				{
			      				*doublePtr = GetShortDoubleValue (&bufferPtr[bufferIndex]);
			      				doublePtr++;
			      				bufferIndex += 8;
			      
			      				}	// end "for (index=0; index<count; index++)"
			      				
			      			if (convertToMapUnitsFlag)
				      			{	
			      				doublePtr = (SDouble*)&arcViewMultiPointPtr->points[0];
			      											
				      			for (index=0;
											index<arcViewMultiPointPtr->numPoints;
												index++)
				      				{
										ConvertLatLongPointToMapPoint (
																	mapProjectionInfoPtr,
																	(ArcViewDoublePointPtr)doublePtr);
				      				doublePtr += 2;
				      
				      				}	// end "for (index=0; index<count; index++)"
				      				
				      			}	// end "if (convertToMapUnitsFlag)"
		      				
		      				}	// end "if (doubleNumberBytes == (double)recordLength)"	
		      					
		      			else	// doubleNumberBytes != (double)recordLength
		      				{
		      				includeFlag = FALSE;
								returnCode = 6;
								
								}	// end "else doubleNumberBytes != (double)recordLength"
		      				
		      			break;
		      			
		      		default:
		      	
					      if (outputIndex != bufferStart)
					      	{
					      	BlockMoveData (&bufferPtr[bufferStart], 
					      							&bufferPtr[outputIndex], 
					      							recordLength+8);
					      	
					      	}	// end "if (outputIndex != bufferStart)"
		      				
		      			break;
		      		
		      		}	// switch (shapeType)
			      
			      if (includeFlag)
			      	{							
			      	outputIndex += recordLength + 8;
		      		numberRecords++;
		      		
		      		}	// end "if (includeFlag)"
		      	
		      	}	// end "if (includeFlag)"
		      	
		      bufferStart += recordLength + 8;
		      
		      if (bufferStart >= numberBytes)
		      	notDoneFlag = FALSE;
			
						// Exit routine if user has "command period" down.					
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+mDownMask+mUpMask))
						{
		      		notDoneFlag = FALSE;
		      		quitFlag = TRUE;
						break;
						
						}	// end "if (!CheckSomeEvents (osMask + ..." 
						
					}	// end "if (TickCount () >= nextTime)" 
				
				}	// end "while (notDoneFlag)"
				
					// Dispose of status box.													
					
			CloseStatusDialog (TRUE);
			
			if (returnCode == 6)
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert108,
									0, 
									NULL);
			
			else if (!quitFlag && numberRecords == 0)
				{
						// Display an alert indicating that the input shape file does not
						// intersect with any of the open image windows. The file will
						// not be used.
						// Note that number of records needs to be set to 0 in case an
						// update event occurs when displaying the alert.						
						
				shapeInfoPtr->numberRecords = 0;
				DisplayNoIntersectionAlert (IDS_Alert88);	
									
				returnCode = 5;
									
				}	// end "if (!quitFlag && numberRecords == 0)"
				
			else if (!quitFlag)
				{	
				shapeInfoPtr->numberRecords = numberRecords;
				
						// Set handle size to just the size of the data that was
						// read in. Force the handle size to be a multiple of 8 bytes.
				
				outputIndex = ((outputIndex + 7)/8) * 8;
				
				if (MSetHandleSize (&shapeInfoPtr->vectorDataHandle, outputIndex))
					returnCode = 0;

						// Make sure that the windows are updated before the shape file
						// is drawn.
						// Don't do this now since change in CopyOffscreen routine in 8/2013
									
				// CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);
				
				}	// end "else numberRecords != 0"
			
			}	// end "if (!quitFlag && errCode == noErr)"

		CheckAndUnlockHandle (mapProjectionHandle);
			
		}	// end "if (bufferPtr != NULL)"
	
	if (shapeInfoPtr != NULL)
		CheckAndUnlockHandle (shapeInfoPtr->vectorDataHandle);
	CheckAndUnlockHandle (shapeFileHandle);
	
	if (returnCode != 0 && okayToCloseOverlayFileFlag)
		CloseOverlayFile (shapeFileIndex);
					
			// Change cursor back to pointer.								
				
	MInitCursor ();
		
	return (returnCode);
	
}	// end "ReadArcViewShapeFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ReadArcViewShapeHeader
//
//	Software purpose:	This routine reads the file specifications from an ARCView shape 
//							file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if ArcView shape file header record parameters make sense
//										and file needs to be loaded.
//							1 - if ArcView shape file header record paramters do not make 
//									sense.
//							2 - shape file was added to the window. It is already  loaded.
//							3 - shape file was already included in the window.
//							4 - this is a shape file, but no image window has been
//									opened yet.
//							5 - This is a shape file but it does not intersect with the 
//									active image window.
//							6 - no map information has been defined for the active image
//									window. The shape file cannot be loaded.
//							7 - This is a shape file in a multiple selection list. The shape
//									file will be ignored.
//						  10 - Overlay does not exist yet in the overlay list.  It needs
//										to be load but converted from lat-long to map units.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/2000
//	Revised By:			Larry L. Biehl			Date: 04/15/2020

SInt16 ReadArcViewShapeHeader (
				FileInfoPtr 						fileInfoPtr, 
				UCharPtr								headerRecordPtr,
				SInt16*								shapeFileIndexPtr,
				DoubleRect*							boundingShapeRectanglePtr,
				SInt16								formatOnlyCode)

{
	#if include_gdal_capability
		UInt8									filePathString[_MAX_PATH];
	#endif
											
	CMFileStream						shapeFileStream;
	
	UInt8									headerRecord[128];
	
	FileStringPtr						shapeFileNamePtr;
	
	DoubleRect							boundingBox;
	
	CMFileStream						*fileStreamPtr,
											*shapeFileStreamPtr = NULL;
	
	Handle*								handlePtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;										
	ShapeInfoPtr						shapeInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	#if include_gdal_capability
		DBFHandle							dbfHandle = NULL;
   #else	// include_gdal_capability
		Handle								dbfHandle = NULL;
   #endif	// include_gdal_capability
	
	Handle								mapProjectionHandle,
											shapeInfoHandle,
											windowInfoHandle;
											
	SInt32								fileCode,
											fileLength,
											shapeType,
											version,
											zeroField4,
											zeroField8,
											zeroField12,
											zeroField16,
											zeroField20;
											
	UInt32								count,
											index;
	
	SInt16								colorIndex,
											errCode = noErr,
											fileType = 0,
											overlayCheckReturn = 0,
											overlayNumber = 0,
											returnCode = 1,	
											string;
											
	UInt16								versionNumberLoaded = 0;
											
	Boolean								changedFlag,
											continueFlag = TRUE;
	
			
	if (headerRecordPtr == NULL || fileInfoPtr == NULL)
																			return (returnCode);		

			// Now check if this is an Arc View Index file (suffix of '.shx')
			// or a dBASE file (suffix of '.dbf'). If so, then look for a 
			// corresponding Arc View Shape file (suffix of '.shp').

	FileStringPtr		filePathPtr;
	UInt16				numSuffixChars;
	SInt16				fileNameCode = 0;
	
	*shapeFileIndexPtr = -1;
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
	
	if (CompareSuffixNoCase ((char*)"\0.shx", 
										filePathPtr,
										&numSuffixChars))
		fileNameCode = 1;
	
	else if (CompareSuffixNoCase ((char*)"\0.dbf", 
										filePathPtr,
										&numSuffixChars))
		fileNameCode = 2;
	
	else if (CompareSuffixNoCase ((char*)"\0.sbx", 
										filePathPtr,
										&numSuffixChars))
		fileNameCode = 3;
	
	else if (CompareSuffixNoCase ((char*)"\0.sbn", 
										filePathPtr,
										&numSuffixChars))
		fileNameCode = 4;
	
	if (fileNameCode != 0)
		{
		if (formatOnlyCode == kShapeFiles)
			{
					// This is for just searching for shape files. We will ignore
					// any files with .shx, .dgf, .sbx & .sbn suffixes.
					
																					return (0);
			
			}	// end "if (formatOnlyCode == kShapeFormatOnly)"
		
				// The user may be requesting that a shape file be opened. Look
				// for the shape file that goes with the .shx, .dbf, sbx or .sbn file.
				// If one does not exist then treat the incoming .xxx file
				// as a yet unknown image file.
				
		shapeFileStreamPtr = &shapeFileStream;		
		InitializeFileStream (shapeFileStreamPtr, fileStreamPtr);
		shapeFileNamePtr = (FileStringPtr)GetFilePathPPointerFromFileStream (shapeFileStreamPtr);
		
				// Remove the '.shx', '.dbf' or '.sbx' suffix at the end of the file name
				// and then look for a corresponding '.shp' file.
			
		RemoveCharsNoCase ((char*)"\0.???\0", shapeFileNamePtr);
		
		ConcatFilenameSuffix (shapeFileNamePtr, (StringPtr)"\0.shp\0");
	
				// This will be an attempt to read a file with the expected suffix.
		
		if (errCode == noErr)			
			errCode = OpenFileReadOnly (shapeFileStreamPtr, 
													kResolveAliasChains, 
													kLockFile,
													kVerifyFileStream);
	
		count = 100;
		if (errCode == noErr)
			{
			errCode = MReadData (shapeFileStreamPtr, 
											&count, 
											&headerRecord[0], 
											kNoErrorMessages);
											
			if (errCode == eofErr && count >= 100)
				errCode = noErr;
				
			CloseFile (shapeFileStreamPtr);
			
			if (errCode == noErr)
				{	
				headerRecord[count] = 0;
				headerRecordPtr = &headerRecord[0];
				
						// Change the file stream to be used for the arc view shape
						// file to this file.
						
				fileStreamPtr = shapeFileStreamPtr;
				
				}	// end "if (errCode == noErr)"
			
			}	// end "if (errCode == noErr)"
			
		}	// end "if (fileNameCode != 0)"
		
	if (errCode == noErr)
		{
				// Set up swap bytes flag depending upon arc view definition and
				// system architecture
				                 
		gSwapBytesFlag = !gBigEndianFlag; 
		
				// Get the File Code
      
      fileCode = GetLongIntValue ((char*)&headerRecordPtr[0]);
      
      		// Get zero fields
      		
      zeroField4 = GetLongIntValue ((char*)&headerRecordPtr[4]);
      zeroField8 = GetLongIntValue ((char*)&headerRecordPtr[8]);
      zeroField12 = GetLongIntValue ((char*)&headerRecordPtr[12]);
      zeroField16 = GetLongIntValue ((char*)&headerRecordPtr[16]);
      zeroField20 = GetLongIntValue ((char*)&headerRecordPtr[20]);
		
				// Get the File Length
      
      fileLength = GetLongIntValue ((char*)&headerRecordPtr[24]);
		
				// Get the Version
      
		gSwapBytesFlag = gBigEndianFlag; 
      version = GetLongIntValue ((char*)&headerRecordPtr[28]);
		
				// Get the Shape Type
      
      shapeType = GetLongIntValue ((char*)&headerRecordPtr[32]);
		
				// Get the Bounding Box
      
      boundingBox.left = GetShortDoubleValue (&headerRecordPtr[36]);
    	boundingBox.top = GetShortDoubleValue (&headerRecordPtr[44]);
      boundingBox.right = GetShortDoubleValue (&headerRecordPtr[52]);
      boundingBox.bottom = GetShortDoubleValue (&headerRecordPtr[60]);
      
      		// Now check parameters to see if this file makes sense as an ArcView
      		// shape file.
      		
		fileType = kArcViewShapeType;
      if (fileCode != 9994)
			fileType = 0;
			
      else if (zeroField4 != 0 || zeroField8 != 0 || 
      				zeroField12 != 0 || zeroField16 != 0 || zeroField20 != 0)
			fileType = 0;
			
		else if (version != 1000)
			fileType = 0;
			
		else if (shapeType < 0 || shapeType > 31)
			fileType = 0;
			
		else if (boundingBox.left > boundingBox.right || 
									boundingBox.top > boundingBox.bottom)
			fileType = 0;
			
		}	// end "if (errCode == noErr)"
		
	if (fileType == kArcViewShapeType)
		{
		if (formatOnlyCode != kLoadHeader)
			{
					// This is for just searching for shape files. Indicate that
					// this is a shape file but do not load the header information.
					
			fileInfoPtr->format = kArcViewShapeType;
			fileInfoPtr->thematicType = FALSE;
			
					// Do not allow shape files in multiple selection options
					
			if (gOpenImageSelectionListCount > 1)
				fileInfoPtr->format = 0;
			
																					return (0);
			
			}	// end "if (formatOnlyCode != kLoadHeader)"
				
		if (gOpenImageSelectionListCount > 1)
			{
					// Display an alert indicating that MultiSpec will ignore
					// shape files included in list of multiple selections.													
			
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								IDS_Alert146,
								0, 
								NULL);
								
			fileInfoPtr->format = kArcViewShapeType;
			//gGetFileImageType = kVectorImageType;
																					return (7);
			
			}	// end "if (gOpenImageSelectionListCount > 1)"
		
		}	// end "if (fileType == kArcViewShapeType)"
		
	if (fileType == kArcViewShapeType)
		{			
				// Save the bounding rectangle for the shape file in case needed later.
				
		boundingShapeRectanglePtr->left = boundingBox.left;
		boundingShapeRectanglePtr->top = boundingBox.top;
		boundingShapeRectanglePtr->right = boundingBox.right;
		boundingShapeRectanglePtr->bottom = boundingBox.bottom;
		
				// Check if MultiSpec can handle the shape file type.
		
		switch (shapeType)
			{
      	case 0:	// Null shape
      	case 1:	// Point shape
      	case 3:	// PolyLine shape
      	case 5:	// Polygon shape
      	case 8:	// MultiPoint shape
      	case 11:	// PointZ shape
      	case 15:	// PolygonZ shape
      				// MultiSpec can read these shape files.
      				
      		string = 0;
      		break;
      		
      	case 13:	// PolyLineZ shape
      	//case 15:	// PolygonZ shape
      	case 18:	// MultiPointZ shape
      		string = IDS_Alert76;
      		break;
      		
      	case 21:	// PointM shape
      	case 23:	// PolyLineM shape
      	case 25:	// PolygonM shape
      	case 28:	// MultiPointM shape
      		string = IDS_Alert77;
      		break;
      		
      	case 31:	// MultiPatch shape
      		string = IDS_Alert78;
      		break;
      		
      	default:
      		string = IDS_Alert79;
      		break;
								
			}	// end "switch (shapeType)"
      
      if (string != 0)
      	{		
					// Display an alert indicating that MultiSpec cannot read this 
					// type of arc view shape file.													
			
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								string,
								0, 
								NULL);
								
			fileType = 0;
			
			}	// end "if (string != 0)"
		
		}	// end "if (fileType == kArcViewShapeType)"
	#if vector_requires_window
		if (fileType == kArcViewShapeType && gActiveImageWindow == NULL)
			{
					// Display an alert indicating that an image file needs to be opened
					// before MultiSpec can handle an ArcView shape file.
			
			DisplayAlert (kErrorAlertID,
								kStopAlert,
								kAlertStrID,
								IDS_Alert81,
								0,
								NULL);
								
			overlayCheckReturn = 4;
			
			}	// end "if (fileType == kArcViewShapeType && "
	#endif
	windowInfoHandle = GetWindowInfoHandle (gActiveImageWindow);
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);
	#if vector_requires_window
		if (fileType == kArcViewShapeType && overlayCheckReturn == 0)
			{
			if (mapProjectionHandle == NULL)
			//if (windowInfoHandle != NULL && mapProjectionHandle == NULL)
				{
						// Display an alert indicating that no map information has been
						// defined for the active image window. The shape file cannot be
						// loaded..
				
				DisplayAlert (kErrorAlertID,
									kStopAlert,
									kAlertStrID,
									IDS_Alert89,
									0,
									NULL);
									
				overlayCheckReturn = 6;
				
				}	// end "if (windowInfoHandle != NULL && mapProjectionHandle == NULL)"
			
			}	// end "if (fileType == kArcViewShapeType && "
	#endif
	if (fileType == kArcViewShapeType && overlayCheckReturn == 0)
		{				
				// Check if shape file is already loaded.
				
		overlayCheckReturn = CheckIfOverlayFileLoaded (fileStreamPtr, 
																		&boundingBox,
																		&overlayNumber,
																		&versionNumberLoaded);

		if (overlayCheckReturn == 5)
					// Display an alert indicating that the input shape file does not
					// intersect with any of the open image windows. The file will
					// not be used.	
						
			DisplayNoIntersectionAlert (IDS_Alert80);	
			
		}	// end "fileType == kArcViewShapeType && overlayCheckReturn == 0"
		
	if (fileType == kArcViewShapeType &&
									(overlayCheckReturn == 0 || overlayCheckReturn == 10))
		{
				// Load the shape file structure.
		
		if (gNumberShapeFiles >= gShapeHandleListLength)
			{		
			handlePtr = (Handle*)CheckHandleSize (&gShapeFilesHandle,
																&continueFlag, 
																&changedFlag, 
																(gNumberShapeFiles+1)*sizeof (Handle));
				
			if (continueFlag)
				{
				handlePtr[gNumberShapeFiles] = NULL;
				gShapeHandleListLength++;
				
				}	// end "if (continueFlag)"	
				
			}	// end "if (gNumberShapeFiles >= gShapeHandleListLength)"
			
		else	// gNumberShapeFiles < gShapeHandleListLength
			handlePtr = (Handle*)GetHandlePointer (
												gShapeFilesHandle,
												kLock);
			
		if (handlePtr == NULL)
			continueFlag = FALSE;
											
		if (continueFlag)
			{
			shapeInfoHandle = MNewHandle (sizeof (ShapeInfo));
			shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeInfoHandle, kLock);
			
			continueFlag = (shapeInfoPtr != NULL);
	
			#if defined multispec_win || defined multispec_wx
				if (continueFlag)
					{
					shapeInfoPtr->fileStreamCPtr = new CMFileStream;

					if (shapeInfoPtr->fileStreamCPtr == NULL)
						continueFlag = FALSE;
						
					}	// end "if (continueFlag)"
			#endif	// defined multispec_win || defined multispec_wx
			
			}	// end "if (continueFlag)"
						
		if (continueFlag)
			{		
					// Get the info for the dbf if it is available.
					
			#if include_gdal_capability
				returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
																filePathString,
																510,
																&filePathPtr,
																kGDAL_Library);
															  
				if (returnCode == noErr)
					dbfHandle = DBFOpen ((char*)filePathPtr, "rb");
			#endif	// include_gdal_capability

					// Copy file stream parameters to shape file structure.
			
			shapeFileStreamPtr = GetFileStreamPointer (shapeInfoPtr);			
			InitializeFileStream (shapeFileStreamPtr, fileStreamPtr);
		
					// Initialize the map information to that for the image window
					// that this shape file is being used for.  It will be needed if 
					// the vector is in (or being converted to) map units.
				
			InitializeGeodeticModelStructure (&shapeInfoPtr->geodetic);
			InitializeGridCoordinateSystemStructure (&shapeInfoPtr->gridCoordinate);
			
			mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																					mapProjectionHandle);
				
			if (mapProjectionInfoPtr != NULL)
				{
						// Set to that for the image window
				
				shapeInfoPtr->geodetic = mapProjectionInfoPtr->geodetic;
				shapeInfoPtr->gridCoordinate = mapProjectionInfoPtr->gridCoordinate;
				
				}	// end "if (mapProjectionInfoPtr != NULL)"

			shapeInfoPtr->shapeType = shapeType;
			
					// Convert from number of 16-bit words to number of 8-bit
					// bytes.
					
			shapeInfoPtr->fileLength = 2 * fileLength;
			
			shapeInfoPtr->boundingXYBox = boundingBox;
		
			shapeInfoPtr->boundingZMBox.top = DBL_MAX;
			shapeInfoPtr->boundingZMBox.bottom = -DBL_MAX;
			shapeInfoPtr->boundingZMBox.left = DBL_MAX;
			shapeInfoPtr->boundingZMBox.right = -DBL_MAX;
			
			shapeInfoPtr->dbfInfoPtr = dbfHandle;
			shapeInfoPtr->indexRecordHandle = NULL;

			colorIndex = (SInt16)(gNumberShapeFiles % 7);
			shapeInfoPtr->lastOverlayColor = gOverlayColorList[colorIndex];
			shapeInfoPtr->lastLineThickness = 1;
			shapeInfoPtr->versionNumberLoaded = versionNumberLoaded;
			
					// Store whether shape file is in lat-long units, map units, or
					// has been converted from lat-long to map units.
					
			shapeInfoPtr->conversionCode = 0;
			if (GetReferenceSystemCode (mapProjectionHandle) != kGeographicRSCode)
				shapeInfoPtr->conversionCode = 1;
			
			if (overlayCheckReturn == 10)
				shapeInfoPtr->conversionCode = 2;
				
			shapeInfoPtr->unitsCodeForConversion = 0;
			if (mapProjectionInfoPtr != NULL && shapeInfoPtr->conversionCode == 2)
				shapeInfoPtr->unitsCodeForConversion = 
										mapProjectionInfoPtr->planarCoordinate.mapUnitsCode;
			
					// Get buffer to read shape file into.
					
			if (!GetMemoryForVectorData (shapeInfoPtr))
				continueFlag = FALSE;
			
			CheckAndUnlockHandle (shapeInfoHandle);
			
			}	// end "if (continueFlag)"
		
		if (continueFlag)
			{		
			if (overlayNumber > 0)
				{
						// Load the handle into the structure where the previous was
						// deleted.
						
				*shapeFileIndexPtr = overlayNumber - 1;
				handlePtr[*shapeFileIndexPtr] = shapeInfoHandle;
				
				}	// end "if (overlayNumber > 0)"
				
			else	// overlayNumber == 0
				{	
						// Load handle for shape info structure into the first available
						// location in the shape file handle list.
				
				for (index=0; index<gShapeHandleListLength; index++)
					{
					if (handlePtr[index]	== NULL)
						{
						handlePtr[index] = shapeInfoHandle;
						*shapeFileIndexPtr = (SInt16)index;
						break;
						
						}	// end "if (handlePtr[index]	== NULL)"
						
					}	// end "for (index=0; index<gShapeHandleListLength; index++)"
					
				}	// end "else overlayNumber == 0"
			
			if (*shapeFileIndexPtr >= 0)
				{				
				gNumberShapeFiles++;
				returnCode = 0;
				
						// If this is a new shape file and not a reloaded shape file, add it
						// to the active image window list.
						
				if (overlayNumber == 0)
					{
					windowInfoHandle = GetWindowInfoHandle (gActiveImageWindow);
					windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
														
					InitializeOverlay (windowInfoPtr, *shapeFileIndexPtr+1);
					
					}	// end "if (overlayNumber == 0)"
			
				}	// end "if (*shapeFileIndexPtr >= 0)"
				
			else	// *shapeFileIndexPtr < 0
				continueFlag = FALSE;
			
			}	// end "if (continueFlag)"
			
		if (!continueFlag)
			{
			UnlockAndDispose (shapeInfoHandle);
			
			}	// if (!continueFlag)
					
		CheckAndUnlockHandle (gShapeFilesHandle);
		
		}	// end "if (fileType == kArcViewShapeType && (overlayCheckReturn == 0 || ..."
		
	if (fileType == kArcViewShapeType)
		{
				// Set the file format.
				
		fileInfoPtr->format = kArcViewShapeType;
		fileInfoPtr->ancillaryInfoformat = kArcViewShapeType;
		
		returnCode = overlayCheckReturn;

		}	// end "if (fileType == kArcViewShapeType)"
		
	if (returnCode != 0 && shapeFileStreamPtr != NULL)
		CloseFile (shapeFileStreamPtr);
		
	return (returnCode);
	
}	// end "ReadArcViewShapeHeader"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReadGTOPO30PrjFile
//
//	Software purpose:	This routine reads the prj file to get projection information.
//							Note that this routine only handles a subset of variations that
//							may be included in prj files.  It will be updated as needed.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	0 - if projection information was read.
//							1 - if projection information was not read.
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/27/2009
//	Revised By:			Larry L. Biehl			Date: 01/17/2019

void ReadGTOPO30PrjFile (
				FileInfoPtr 						fileInfoPtr,
				char*									prjBufferPtr)

{
	CMFileStream						prjFileStream;
	
	double								realValue,
											realValues[15],
											xShift,
											yShift;
	
	CharPtr								strPtr;
											
	CMFileStream						*fileStreamPtr,
											*prjFileStreamPtr;
											
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	FileStringPtr						prjFileNamePtr;
	
	UInt32								count;
	
	SInt16								errCode,
											mapUnitsCode,
											tReturnCode;
	
	SignedByte							handleStatus = 0;
	
		
	errCode = noErr;
	mapProjectionInfoPtr = NULL;
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
	prjFileStreamPtr = &prjFileStream;		
	InitializeFileStream (prjFileStreamPtr, fileStreamPtr);
	prjFileNamePtr =
					(FileStringPtr)GetFilePathPPointerFromFileStream (prjFileStreamPtr);
			
			// Remove the ".bil", ".bsq", ".bip" at the end.
					
	RemoveCharsNoCase ((char*)"\0.???\0", prjFileNamePtr); 
		
			// Try getting file with ".prj" suffix at the end.
	
	ConcatFilenameSuffix (prjFileNamePtr, (StringPtr)"\0.prj\0");
			
	errCode = OpenFileReadOnly (prjFileStreamPtr, 
											kResolveAliasChains, 
											kLockFile, 
											kVerifyFileStream);
		
	if (errCode == noErr)		
		{
				// A prj file is available, read the project file information.
		
		errCode = MSetMarker (prjFileStreamPtr, fsFromStart, 0, kErrorMessages);
	
				// Read header.
		
		count = 1024;
		if (errCode == noErr)
			errCode = MReadData (prjFileStreamPtr, &count, prjBufferPtr, kNoErrorMessages);
		
		if (errCode == eofErr)
			errCode = noErr;
		
				// The prj file has been read.  Close the prj file.
			
		CloseFile (prjFileStreamPtr);
			
		}	// end "if (errCode == noErr)"
		
	if (errCode == noErr)
		{
				// Check if this is in the WKT coordinate system definition. If so ignore
				// as the code after does not know how to parse this info. The WKT can be
				// parsed by the OGR library of routines for those image files being read
				// by gdal.
				// For now a "[" in the string implies a WKT description
		
		strPtr = (char*)StrStrNoCase (prjBufferPtr, "[");
		
		if (strPtr != NULL)
			errCode = 1;		
		
		}	// end "if (errCode == noErr)"
					
	if (errCode == noErr)
		{
		if (fileInfoPtr->mapProjectionHandle == NULL)
			fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
		
		if (fileInfoPtr->mapProjectionHandle != NULL)
			{
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
						GetHandleStatusAndPointer (
											fileInfoPtr->mapProjectionHandle,
											&handleStatus);
											
			}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
			
		if (mapProjectionInfoPtr == NULL)
			errCode = 1;
			
		}	// end "if (errCode == noErr)"
					
	if (errCode == noErr)
		{	
				// Make sure that there is a c string terminator at the end of
				// the data that was read to force searches to end there now.
				// Also put a c string terminator at the end of that which was read
				// in.
		
		prjBufferPtr[1023] = 0;	
		prjBufferPtr[count] = 0;	
		
				// Find "Projection" in the buffer.		
				
		strPtr = prjBufferPtr;
		tReturnCode = GetFileHeaderString (kFileIOStrID, 
														IDS_FileIO186, 	// Projection
														&strPtr, 
														10, 
														kDoNotSkipEqual,
														(char*)gTextString,
														0,
														50,
														kNoSubstringAllowed);	
						
		if (tReturnCode == 0)
			{
					// Get the projection name
			
			if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"GEOGRAPHIC", 10) == 0)
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
																						kGeographicRSCode;
						
			else if (CompareStringsNoCase (
											&gTextString[1], (UCharPtr)"ORTHOGRAPH", 10) == 0)
				mapProjectionInfoPtr->gridCoordinate.projectionCode = kOrthographicCode;
				
			else if (CompareStringsNoCase (
											&gTextString[1], (UCharPtr)"LAMBERT_AZ", 10) == 0)
				mapProjectionInfoPtr->gridCoordinate.projectionCode =
																	kLambertAzimuthalEqualAreaCode;
				
			}	// end "if (tReturnCode == 0)"
			
				// Find "Datum" in the buffer.	
				
		strPtr = prjBufferPtr;
		GetDatumInfo (&strPtr, 
							&mapProjectionInfoPtr->geodetic.datumCode,
							IDS_FileIO140,
							kDoNotSkipEqual);
							
				// Find "Zunits" in the buffer.	
				
		strPtr = prjBufferPtr;
		tReturnCode = GetFileHeaderString (kFileIOStrID, 
														IDS_FileIO181, 	// Zunits
														&strPtr, 
														4, 
														kDoNotSkipEqual,
														(char*)gTextString,
														0,
														50,
														kNoSubstringAllowed);	
						
		if (tReturnCode == 0)
			{
					// Get the vertical units
			
			if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"METE", 4) == 0)			
				mapProjectionInfoPtr->planarCoordinate.zMapUnitsCode = kMetersCode;
			
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"FEET", 4) == 0)	
				mapProjectionInfoPtr->planarCoordinate.zMapUnitsCode = kFeetCode;
				
			}	// end "if (tReturnCode == 0)"
			
				// Find "Units" in the buffer.	
				
		strPtr = prjBufferPtr;
		tReturnCode = GetFileHeaderString (kFileIOStrID, 
														IDS_FileIO182, 	// Units
														&strPtr, 
														3, 
														kDoNotSkipEqual,
														(char*)gTextString,
														0,
														50,
														kNoSubstringAllowed);	
		
		mapUnitsCode = 0;				
		if (tReturnCode == 0)
			{
					// Units code
			
			if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"DMS", 3) == 0)
						// Decimal seconds
				mapUnitsCode = kAVDegreesMinutesSecondsCode;
				
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"DM", 2) == 0)
						// Decimal minutes
				mapUnitsCode = kAVDecimalMinutesCode;
				
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"DS", 2) == 0)
						// Decimal seconds
				mapUnitsCode = kAVDecimalSecondsCode;
				
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"D", 1) == 0)
						// Decimal degrees
				mapUnitsCode = kAVDecimalDegreesCode;
				
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"rad", 3) == 0)
						// Decimal seconds
				mapUnitsCode = kAVRadiansCode;
				
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"MET", 3) == 0)			
				mapUnitsCode = kMetersCode;
				
			else if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"FEE", 3) == 0)			
				mapUnitsCode = kFeetCode;
			
			mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;
				
			}	// end "if (tReturnCode == 0)"
			
				// Find "Spheroid" in the buffer.								
		
		strPtr = prjBufferPtr;														
		tReturnCode = GetFileHeaderString (kFileIOStrID, 
														IDS_FileIO183, 	// Spheroid
														&strPtr, 
														5, 
														kDoNotSkipEqual,
														(char*)gTextString,
														0,
														50,
														kNoSubstringAllowed);	
						
		if (tReturnCode == 0)
			{
					// Get the number of bytes per pixel
			
			if (CompareStringsNoCase (&gTextString[1], (UCharPtr)"WGS84", 5) == 0)
				mapProjectionInfoPtr->geodetic.spheroidCode = kWGS84EllipsoidCode;
				
			}	// end "if (tReturnCode == 0)"	
			
		else	// tReturnCode != 0
			{
					// Spheroid not found; Set according to datum definition
				
			if (mapProjectionInfoPtr->geodetic.datumCode == kSphereDatumCode)
				mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidCode;
			
			}	// end "else tReturnCode != 0"
			
				// Find "Xshift" in the buffer.		
				
		realValue = GetFileHeaderRealValue (kFileIOStrID, 
													IDS_FileIO184, 	// Xshift
													prjBufferPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
							
		if (tReturnCode == 0)
			{
			xShift = realValue;
							
			}	// end "if (tReturnCode == 0)"
			
				// Find "Yshift" in the buffer.		
				
		realValue = GetFileHeaderRealValue (kFileIOStrID, 
													IDS_FileIO185, 	// Yshift
													prjBufferPtr, 
													1,
													kDoNotSkipEqual, 
													&tReturnCode);
							
		if (tReturnCode == 0)
			{
			yShift = realValue;
							
			}	// end "if (tReturnCode == 0)"  
		
		if (errCode == noErr)
			{
					// Find "Parameters" in the buffer.	
					// Will not use this for now until there is an example file that
					// includes PARAMETERS to test with.
									
			strPtr = prjBufferPtr;					
			tReturnCode = GetFileHeaderString (kFileIOStrID, 
															IDS_FileIO191, 	// Parameters
															&strPtr, 
															-1, 
															kDoNotSkipEqual,
															(char*)gTextString,
															0,
															50,
															kNoSubstringAllowed);		
								
			if (tReturnCode == 0)
				{
						// The projection info line was found.  Now read the 15 strings
						//	that should be in this line that are separated by commas.
						
				#if defined multispec_wx
					CharPtr savedcharptr = strPtr;
				#endif
						
				tReturnCode = sscanf (strPtr, 
											"%lf %s", 
											&realValues[0], 
											(char*)gTextString);
											
				#if defined multispec_wx
					strPtr = savedcharptr;
				#endif
										
				}	// end "if (tReturnCode == 0)"
			
			}	// end "if (errCode == noErr)" 	
			
				// Adjust the upper left map location if needed.
				
		if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																						kGeographicRSCode)
			{
			if (mapUnitsCode == kAVDecimalSecondsCode)
				{
						// Convert from decimal seconds to decimal degrees.
						
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 /= 3600;
				mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 /= 3600;
                                                                      
				mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
				mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
									mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
									
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize /= 3600;
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize /= 3600; 
																		
				mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kDecimalDegreesCode;
													
				}	// end "if (mapUnitsCode == kAVDecimalSecondsCode)"
			
			}	// end "if (...->gridCoordinate.referenceSystemCode == kGeographicRSCode)"
			
		}	// end "if (errCode == noErr)"	
	
	MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);
		
	return;
	
}	// end "ReadGTOPO30PrjFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseShapeFileMemory
//
//	Software purpose:	This routine release the memory used by the shape file.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/01/2001
//	Revised By:			Larry L. Biehl			Date: 09/06/2013

void ReleaseShapeFileMemory (
				Handle*								shapeHandlePtr,
				SInt16								overlayIndex,
				SInt16								memoryReleaseCode)

{	
	CMFileStream*						fileStreamPtr;
	ShapeInfoPtr						shapeInfoPtr;
	
	
	shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeHandlePtr[overlayIndex],
																		kLock);
		
	if (shapeInfoPtr != NULL)
		{
		shapeInfoPtr->indexRecordHandle = 
								UnlockAndDispose (shapeInfoPtr->indexRecordHandle);
		shapeInfoPtr->vectorDataHandle =
								UnlockAndDispose (shapeInfoPtr->vectorDataHandle);
		
		if (memoryReleaseCode == kAllShapeHandleMemory)
			{		
			#if include_gdal_capability
				if (shapeInfoPtr->dbfInfoPtr != NULL)
					DBFClose (shapeInfoPtr->dbfInfoPtr);
			#endif	// include_gdal_capability
				
			shapeInfoPtr->dbfInfoPtr = NULL;
						
			fileStreamPtr = GetFileStreamPointer (shapeInfoPtr);
			CloseFile (fileStreamPtr);
	
			#if defined multispec_win || defined multispec_wx
				if (shapeInfoPtr->fileStreamCPtr != NULL)
					delete shapeInfoPtr->fileStreamCPtr;
			#endif	// defined multispec_win || defined multispec_wx
		
			UnlockAndDispose (shapeHandlePtr[overlayIndex]);
			shapeHandlePtr[overlayIndex] = NULL;

			}	// end "if (memoryReleaseCode == kAllShapeHandleMemory)"
			
		else	// memoryReleaseCode != kAllShapeHandleMemory
			{
			shapeInfoPtr->numberRecords = 0;
			CheckAndUnlockHandle (shapeHandlePtr[overlayIndex]);

			}	// end "else memoryReleaseCode != kAllShapeHandleMemory"
		
		}	// end "if (shapeInfoPtr != NULL)"
	
}	// end "ReleaseShapeFileMemory"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean WindowBoundingAreaAndRectIntersect
//
//	Software purpose:	This routine determines if the window bounding area intersects
//							with the input rectangle.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	True, if window bounding area intersect with input rectangle
//							Fale, they do not intesect
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/07/2006
//	Revised By:			Larry L. Biehl			Date: 04/29/2012

Boolean WindowBoundingAreaAndRectIntersect (
				WindowInfoPtr						windowInfoPtr,
//				SDoubleRect*		 				boundingRectPtr,
				DoubleRect*							boundingRectPtr,
				Boolean								inputIsMapTypeFlag)

{
	DoubleRect*							boundingRectanglePtr;
	Boolean								areasIntersectFlag = FALSE;
	

			// Verify that shape file intersects with the active image window.
			
	if (windowInfoPtr != NULL)
		{
		boundingRectanglePtr = &windowInfoPtr->boundingMapRectangle;
		if (!inputIsMapTypeFlag)
			boundingRectanglePtr = &windowInfoPtr->boundingLatLongRectangle;
		
		if (AreasIntersect (boundingRectanglePtr, 
									boundingRectPtr))
			areasIntersectFlag = TRUE;
			
		}	// end "if (windowInfoPtr != NULL)"
		
	return (areasIntersectFlag);
	
}	// end "WindowBoundingAreaAndRectIntersect"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean WindowBoundingAreaAndRectIntersect
//
//	Software purpose:	This routine determines if the window bounding area intersects
//							with the input rectangle.
//
//	Parameters in:		
//
//	Parameters out:	None
//
//	Value Returned:	True, if window bounding area intersect with input rectangle
//							Fale, they do not intesect
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2005
//	Revised By:			Larry L. Biehl			Date: 04/29/2012

Boolean WindowBoundingAreaAndRectIntersect (
				WindowPtr							windowPtr,
				DoubleRect*							boundingRectPtr,
				Boolean								inputIsMapTypeFlag)

{
	WindowInfoPtr						windowInfoPtr;
	Handle								windowInfoHandle;
	Boolean								areasIntersectFlag = FALSE;
	

			// Verify that shape file intersects with the active image window.
			
	windowInfoHandle = GetWindowInfoHandle (gActiveImageWindow);
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
			
	areasIntersectFlag = WindowBoundingAreaAndRectIntersect (
														windowInfoPtr,
										 				boundingRectPtr,
														inputIsMapTypeFlag);

	return (areasIntersectFlag);
	
}	// end "WindowBoundingAreaAndRectIntersect"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetLastVectorColorAndWidth
//
//	Software purpose:	The purpose of this routine is to set the last color and line
//							width used for the input vector.
//
//	Parameters in:				
//
//	Parameters out:	Last color for the input overlay vector
//							Last width used for the input overlay vector
//
//	Value Returned:	
//
// Called By:			OverlayDialogOK in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/09/2006
//	Revised By:			Larry L. Biehl			Date: 07/08/2011

void SetLastVectorColorAndWidth (
				SInt32								overlayNumber,
				RGBColor*							lastLineColorPtr,
				UInt16								lastLineThickness)
				
{  
	Handle*								shapeHandlePtr;
	ShapeInfoPtr						shapeInfoPtr;
	
		
	shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle);

	if (shapeHandlePtr != NULL) 
		{
		shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (
															shapeHandlePtr[overlayNumber-1]);
															
		if (shapeInfoPtr != NULL)
			{					
			shapeInfoPtr->lastOverlayColor = *lastLineColorPtr;
			shapeInfoPtr->lastLineThickness = lastLineThickness;					

			}	// end "if (shapeInfoPtr != NULL)"
				
		}	// end "if (shapeHandlePtr != NULL)"

}	// end "SetLastVectorColorAndWidth"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ShapeAndWindowAreasIntersect
//
//	Software purpose:	This routine determines if the input bounding area intersects
//							with any of the window area.
//
//	Parameters in:		file information pointer
//							pointer to header record information
//
//	Parameters out:	None
//
//	Value Returned:	True, if file was read in okay
//							Fale, if file was not read in okay
//
// Called By:			CheckImageHeader in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2000
//	Revised By:			Larry L. Biehl			Date: 01/13/2020

Boolean ShapeAndWindowAreasIntersect (
				SInt16								overlayNumber,
				DoubleRect*							boundingRectPtr)

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;

	UInt32								index;
	
	SInt16								windowCount,
											windowListIndex;
											
	Boolean								includeFlag;
	
	
			// Now verify that the shape file intersects with at least one
			// of the image windows that it is assigned to.
	
	includeFlag = FALSE;		
	windowCount = 0;
	windowListIndex = kImageWindowStart;
		
	do
		{
		windowPtr = gWindowList[windowListIndex];         
		windowInfoHandle = GetWindowInfoHandle (windowPtr);
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
		
		if (windowInfoPtr != NULL)
			{
					// Now check if the overlay is already included in the list.
			
			for (index=0; index<windowInfoPtr->numberVectorOverlays; index++)
				{
				if (abs (windowInfoPtr->overlayList[index].index) == overlayNumber)
					{
					if (AreasIntersect (&windowInfoPtr->boundingMapRectangle,
													boundingRectPtr))
						{
						includeFlag = TRUE;
						break;
						
						}	// end "if (AreaIntersects (...->boundingMapRectangle, ..."
					
					}	// end "if (abs (windowInfoPtr->overlayList[index].index) == ..."
				
				}	// end "for (index=0; index<windowInfoPtr->numberVectorOverlays; ..."
			
			}	// end "if (windowInfoPtr != NULL)"
		
		else	// windowInfoPtr != NULL
					// This implies it is for an empty window. Default is to include.
			includeFlag = TRUE;
			
		if (includeFlag)
			break;
		
		windowListIndex++;
		windowCount++;
													
		}	while (windowCount<gNumberOfIWindows);
		
	return (includeFlag);
	
}	// end "ShapeAndWindowAreasIntersect"


