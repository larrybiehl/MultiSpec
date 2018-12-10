//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SGDALInterface.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			07/27/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems	
//
//	Brief description:	This file contains routines which are used to access
//								gdal library routines to read the header information in 
//								selected image files.
//
//	Functions in file:
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*	Template for debugging.
	int numberChars = sprintf ((char*)gTextString3,
												" GDAL: (filePathPtr hDS): %s %ld%s", 
												filePathPtr,
												hDS,
												gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/		
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	
#if defined multispec_mac
	#define	IDS_Alert144			144
	#define	IDS_FileIO189			189
	#define	IDS_FileIO190			190
	
	#define	IDS_Dialog31			31
#endif	// defined multispec_mac  

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif

#if defined multispec_win
	//#include <string>
#endif	// defined multispec_win

#if include_gdal_capability

			// oul: added definition of SIZE_UNSIGNED_LONG and SIZEOF_VOIDP
			// which are not defined in cpl_config.h
	#if defined multispec_lin
		#if defined NetBeansProject
				// The size of a 'unsigned long', as computed by sizeof.
			#define SIZEOF_UNSIGNED_LONG 8
				// The size of a 'void p', as computed by sizeof.
			#define SIZEOF_VOIDP 8
		#endif	// end "defined NetBeansProject"
	#endif	// end "defined multispec_lin"

#include "gdal.h"
#include "gdal_frmts.h"
#include "gdal_priv.h"
#include "cpl_error.h"
#include "ogr_spatialref.h"
#include "geo_normalize.h"


#include <stdio.h>
//#include "libxml/xmlreader.h"

/*
#if include_hdf5_capability
	#include "H5Zpublic.h"
#endif	// include_hdf5_capability
*/
//#include "SExtGlob.h"

#define	kAVDecimalDegreesCode						1
#define	kAVDecimalMinutesCode						2
#define	kAVDecimalSecondsCode						3
#define	kAVDegreesMinutesSecondsCode				4
#define	kAVRadiansCode									5

typedef struct errHandler
	{
	struct errHandler   *psNext;
	CPLErrorHandler     pfnHandler;
	
	} CPLErrorHandlerNode;

typedef struct 
	{
	int						nLastErrNo;
	CPLErr					eLastErrType;
	CPLErrorHandlerNode	*psHandlerStack;
	int						nLastErrMsgMax;
	char						szLastErrMsg[500];
	
	} CPLErrorContext;
										
										
/*
#ifndef EQUAL
	#if defined(WIN32) || defined(WIN32CE)
		#define EQUALN(a,b,n)           (strnicmp(a,b,n)==0)
		#define EQUAL(a,b)              (stricmp(a,b)==0)
	#else
		#define EQUALN(a,b,n)           (strncasecmp(a,b,n)==0)
		#define EQUAL(a,b)              (strcasecmp(a,b)==0)
	#endif
#endif
*/


		// Routines in SHDF5.cpp 

extern SInt16 GetHDF5CompressionInformation (
				GDALDatasetH						hDS);

extern SInt32 LoadHdf5DataSetNames (
				GDALDatasetH						hDS,
				SInt16								format,
				FileInfoPtr							fileInfoPtr,
				Handle*								hdfDataSetsHandlePtr,
				HdfDataSets**						hdfDataSetsPtrPtr,
				SInt32*								numDataSetsPtr,
				SInt32*								firstDataSetIndexPtr,
				GDALDatasetH*						outGDALDatasetPtr);



		// Routines in GDALInterface.cpp that are used by other routines.
		// Prototype is included here because GDALDatasetH variable is defined
		// by .h files included in this file and not in SPrototype.h

extern GDALDatasetH GetGDALFileReference (
				Handle								hdfHandle,
				UInt16								numberHdfDataSets,
				SInt16								format,
				GDALDatasetH						gdalDataSetH,
				HdfDataSets*						hdfDataSetsPtr,
				UInt32								dataSet);

extern SInt16 LoadGDALHeaderInformation (
				GDALDatasetH						hDS,
				FileInfoPtr 						fileInfoPtr,
				SInt16								format);

extern SInt16 ReadGDALHeaderInformation (
				GDALDatasetH						hDS,
				SInt16								format,
				UInt32*								numberChannelsPtr,
				UInt32*								numberLinesPtr,
				UInt32*								numberColumnsPtr,
				UInt16*								numberBytesPtr,
				UInt16*								numberBitsPtr,
				UInt16*								dataTypeCodePtr,
				UInt16*								dataCompressionCodePtr,
				SInt16*								bandInterleaveFormatPtr,
				Boolean*								signedDataFlagPtr,
				UInt32*								xBlockSizePtr,
				UInt32*								yBlockSizePtr,
				SInt16*								gdalDataTypeCodePtr,
				Boolean*								noDataValueFlagPtr,
				double*								noDataValuePtr);

extern SInt16 ReadGDALProjectionInformation (
				FileInfoPtr							fileInfoPtr,
				GDALDatasetH						hDS);



void		AdjustUpperLeftPixelLocationToCenter (
				MapProjectionInfoPtr				mapProjectionInfoPtr);

Boolean	CheckForGDALHandledHeaders (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode);

SInt16	CheckIfDefaultGDALPaletteExists (
				GDALDatasetH						hDS);

UInt32	GetDataSetGroupNumber (
				Handle								hdfDataSetsHandle,
				UInt32								dataSet);

SInt16	GetDatumCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr,
				UCharPtr								datumNamePtr,
				Boolean								getCodeFlag);

SInt16	GetEllipsoidCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr,
				UCharPtr								ellipsoidNamePtr,
				Boolean								getCodeFlag);

void		GetEPSGCodeName (
				OGRSpatialReferenceH				ogrEPSGSRSPtr,
				UCharPtr								epsgNamePtr);

SInt16	GetGDALSpecificNumericMetadataInformation (
				char**		 						metadataStringPtrPtr,
				char*									masterGroupString,
				double*								valuePtr);

SInt16	GetGDALSpecificTextMetadataInformation (
				char**								metadataStringPtrPtr,
				char*									masterGroupString,
				char*									groupString);

void		GetGDALTiePoints (
				FileInfoPtr							fileInfoPtr,
				GDALDatasetH						hDS);

Boolean	GetGDALTopToBottomFlag (
				FileInfoPtr 						fileInfoPtr,
				SInt16								format,
				GDALDatasetH						hDS);
									
SInt16	GetMapUnitsCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr);

SInt16	GetProjectionCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr,
				SInt16*								referenceSystemCodePtr);

void		GetStatePlaneZoneName (
				OGRSpatialReferenceH				ogrStatePlaneSRSPtr,
				UCharPtr								fipsZoneNamePtr);

SInt16	LoadGDALInformation (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr, 
				SInt16								format);

//void		ProcessRPCXMLNode (
//				xmlTextReaderPtr					reader);

UInt16	ReadGDALCompressionInformation (
				GDALDatasetH						hDS,
				SInt16								format);

void		ReadGDALProjectionInformationFromMetadata (
				FileInfoPtr 						fileInfoPtr,
				GDALDatasetH						hDS);

SInt16	ReadGDALProjectionInformationFromMetadata_HRLDAS (
				GDALDatasetH						hDS,
				double*								noDataValuePtr,
				Boolean*								noDataValueExistsFlagPtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr,	
				Boolean*								convertCenterFromLatLongToMapFlagPtr,			
				Boolean*								convertLowerLeftFromLatLongToMapFlagPtr);

void		ReadXMLRPCs (
				FileInfoPtr							fileInfoPtr);

void		SetProjectionParametersFromGDALInformation (
				MapProjectionInfoPtr 			mapProjectionInfoPtr, 
				OGRSpatialReferenceH				ogrSpatialReferenceCPtr);

void		VerifyEPSG_CSVFolderExits (
				UCharPtr					 			inputStringPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void AdjustUpperLeftPixelLocationToCenter
//
//	Software purpose:	This routine adjusts the upper left pixel location to represent 
//							the center of the pixel instead of the upper left corner of the 
//							upper left pixel.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/07/2014
//	Revised By:			Larry L. Biehl			Date: 08/07/2014

void AdjustUpperLeftPixelLocationToCenter (
				MapProjectionInfoPtr				mapProjectionInfoPtr)

{	
	mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 
			0.5 * fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);
	mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 
			0.5 * fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);
			
	mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
							mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
	mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
							mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
	
}	// end "AdjustUpperLeftPixelLocationToCenter"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForGDALHandledHeaders
//
//	Software purpose:	This routine determines if the input file is one that gdal
//							will handle. This does not catch all, but the ones that this
//							routine does find will save time when the open image dialog
//							box is dispayed.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	True if set up okay.
//							False if not setup.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/14/2012
//	Revised By:			Larry L. Biehl			Date: 03/06/2018

Boolean CheckForGDALHandledHeaders (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)
				
{	
	unsigned char						charKeyCode[4];
	
	SInt16								fileType = 0,
											stringCompare;
		
		
	if (fileInfoPtr != NULL)
		{
				// Check if tiff/geotiff formatted image file.
			
		charKeyCode[0] = 0x4d;
		charKeyCode[1] = 0x4d;
		charKeyCode[2] = 0x00;
		charKeyCode[3] = 0x2a;		                                                          
		stringCompare = (SInt16)strncmp ((char*)headerRecordPtr, 
													(CharPtr)charKeyCode, 
													4);
		
		if (stringCompare != 0)
			{              
			charKeyCode[0] = 0x49;
			charKeyCode[1] = 0x49;
			charKeyCode[2] = 0x2a;
			charKeyCode[3] = 0x00;             
			stringCompare = (SInt16)strncmp ((char*)headerRecordPtr, 
														(CharPtr)charKeyCode, 
														4);
										
			if (stringCompare == 0)
				{
				fileInfoPtr->swapBytesFlag = gBigEndianFlag;
				fileInfoPtr->swapHeaderBytesFlag = gBigEndianFlag;
				
				}	// end "if (stringCompare == 0)"
				
			}	// end "if (stringCompare != 0)"
			
		else	// stringCompare == 0  
			{
			fileInfoPtr->swapBytesFlag = !gBigEndianFlag;
			fileInfoPtr->swapHeaderBytesFlag = !gBigEndianFlag;
			
			}	// end "else stringCompare == 0"
										
		if (stringCompare == 0)
			fileType = kTIFFType;
			
		if (fileType == 0)
			{
					// Check if hdf5 formatted image file.
					
			charKeyCode[0] = 0x89;
			charKeyCode[1] = 'H';
			charKeyCode[2] = 'D';
			charKeyCode[3] = 'F';
			
			if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
				fileType = kHDF5Type;
				
			}	// end "if (fileType == 0)"
			
		if (fileType == 0)
			{
					// Check if netCDF2 formatted image file.
					
			charKeyCode[0] = 'C';
			charKeyCode[1] = 'D';
			charKeyCode[2] = 'F';
			charKeyCode[3] = 0x02;
			
			if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
				fileType = kNETCDF2Type;
				
			}	// end "if (fileType == 0)"
		
		if (fileType == 0)
			{
					// Check if VRT "formatted" file.
					
			charKeyCode[0] = '<';
			charKeyCode[1] = 'V';
			charKeyCode[2] = 'R';
			charKeyCode[3] = 'T';
			
			if (strncmp ((char*)headerRecordPtr, (CharPtr)charKeyCode, 4) == 0)
				fileType = kGDALVRTType;
				
			}	// end "if (fileType == 0)"
		
		fileInfoPtr->format = fileType;
			
				// For now we will not check whether the file is thematic
				// or multispectral
				
		if (formatOnlyCode == kMultiFiles)
			fileInfoPtr->thematicType = FALSE;
		
		else if (formatOnlyCode == kThematicFiles)
			fileInfoPtr->thematicType = TRUE;
		
		}	// end "if (fileInfoPtr != NULL)"
	
	return (fileType != 0);

}	// end "CheckForGDALHandledHeaders"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckIfDefaultGDALPaletteExists
//
//	Software purpose:	This routine gets a new set of hdf file references for the 
//							input file.  It is used when setting up a project file.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	True if set up okay.
//							False if not setup.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/20/2011
//	Revised By:			Larry L. Biehl			Date: 04/21/2011

SInt16 CheckIfDefaultGDALPaletteExists (
				FileInfoPtr							fileInfoPtr)

{		
	SInt16								colorTableCount = 0;
	
	if (fileInfoPtr != NULL)
		colorTableCount = CheckIfDefaultGDALPaletteExists (fileInfoPtr->gdalDataSetH);
	
	return (colorTableCount);
	
}	// end "CheckIfDefaultGDALPaletteExists"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckIfDefaultGDALPaletteExists
//
//	Software purpose:	This routine gets a new set of hdf file references for the 
//							input file.  It is used when setting up a project file.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	True if set up okay.
//							False if not setup.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/20/2011
//	Revised By:			Larry L. Biehl			Date: 05/03/2011

SInt16 CheckIfDefaultGDALPaletteExists (
				GDALDatasetH						hDS)

{		
	GDALColorTableH					gdalColorTableH = NULL;
	//GDALRasterBand*					gdalRasterBandPtr;
	
	SInt32								colorTableCount = 0;
	
	
	if (hDS != NULL)
		{
		//gdalRasterBandPtr = ((GDALDataset*)hDS)->GetRasterBand (1);
		//gdalRasterBandPtr = (GDALDataset*)GDALGetRasterBand (hDS, 1);
		
		
		//if (gdalRasterBandPtr != NULL)
			//gdalColorTablePtr = gdalRasterBandPtr->GetColorTable ();
		gdalColorTableH = GDALGetRasterColorTable	(GDALGetRasterBand (hDS, 1));

		}	// end "if (hDS != NULL)"
	
	if (gdalColorTableH != NULL)
		 colorTableCount = GDALGetColorEntryCount (gdalColorTableH);
	
	if (colorTableCount < 0 || colorTableCount > SInt16_MAX)
		colorTableCount = 0;
		
	return ((SInt16)colorTableCount);
	
}	// end "CheckIfDefaultGDALPaletteExists"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CloseGDALInterface
//
//	Software purpose:	This routine gets a new set of hdf file references for the 
//							input file.  It is used when setting up a project file.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	True if set up okay.
//							False if not setup.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/09/2010
//	Revised By:			Larry L. Biehl			Date: 02/09/2010

void CloseGDALInterface (void)
				
{	
			// Release memory of GDAL structures if they were set up.

	GDALDestroyDriverManager ();

}	// end "CloseGDALInterface"
#endif	// include_gdal_capability


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GDALSetReferenceSystemFromEPSGCode
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters for State Plane based on the input zone using the
//							GDAL code.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CoordinateDialogSetParametersFromEPSGCode in SMapTran.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean GDALSetReferenceSystemFromEPSGCode (
				SInt16								epsgCode,
				UCharPtr								epsgNamePtr,
				UCharPtr								datumNamePtr,
				UCharPtr								ellipsoidNamePtr,
				SInt16*								mapUnitsCodePtr,
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
				double*								falseOriginNorthingPtr,
				double*								semiMajorAxisPtr,
				double*								semiMinorAxisPtr)

{  
	Boolean								parametersSetFlag = FALSE;
	
	#if include_gdal_capability
		double								returnValue;
		
		OGRSpatialReferenceH				ogrEPSGSRSPtr;
		
		OGRErr								ogrError;
		
		SInt16								referenceSystemCode;
		
		
				// This is state plane. Try to get the parameters
				
		ogrEPSGSRSPtr = OSRNewSpatialReference (NULL);
		
		if (ogrEPSGSRSPtr != NULL)
			{
			if (OSRImportFromEPSG (ogrEPSGSRSPtr, epsgCode) == noErr)
			//if (SetEPSGProjCS ((OGRSpatialReference*)ogrEPSGSRSPtr, epsgCode) == noErr)
				{
				*mapUnitsCodePtr = GetMapUnitsCodeFromOGRSpatialReference (
																(OGRSpatialReference*)ogrEPSGSRSPtr);
																				
				GetEPSGCodeName ((OGRSpatialReferenceH)ogrEPSGSRSPtr, epsgNamePtr);
							
				*projectionCodePtr = GetProjectionCodeFromOGRSpatialReference (
								(OGRSpatialReference*)ogrEPSGSRSPtr, &referenceSystemCode);
																									
				GetDatumCodeFromOGRSpatialReference (
								(OGRSpatialReference*)ogrEPSGSRSPtr, datumNamePtr, TRUE);
									
				GetEllipsoidCodeFromOGRSpatialReference (
								(OGRSpatialReference*)ogrEPSGSRSPtr, ellipsoidNamePtr, TRUE);
					
				*longitudeCentralMeridianPtr = OSRGetProjParm (
													ogrEPSGSRSPtr, "central_meridian", 0, NULL);
				*latitudeOriginPtr = OSRGetProjParm (
													ogrEPSGSRSPtr, "latitude_of_origin", 0, NULL);
				*scaleFactorOfCentralMeridianPtr = OSRGetProjParm (
													ogrEPSGSRSPtr, "scale_factor", 1, NULL);
				*falseEastingPtr = OSRGetProjParm (
													ogrEPSGSRSPtr, "false_easting", 0, NULL);
				*falseNorthingPtr = OSRGetProjParm (
													ogrEPSGSRSPtr, "false_northing", 0, NULL);
				*standardParallel1Ptr = OSRGetProjParm (
													ogrEPSGSRSPtr, "standard_parallel_1", 0, NULL);
				*standardParallel2Ptr = OSRGetProjParm (
													ogrEPSGSRSPtr, "standard_parallel_2", 0, NULL);
				
				*falseOriginLongitudePtr = *longitudeCentralMeridianPtr;
				*falseOriginLatitudePtr = *latitudeOriginPtr;
				*falseOriginEastingPtr = *falseEastingPtr;
				*falseOriginNorthingPtr = *falseNorthingPtr;
								
				returnValue	= OSRGetSemiMajor (ogrEPSGSRSPtr, &ogrError);
				if (ogrError == OGRERR_NONE)
					*semiMajorAxisPtr = returnValue;
				
				returnValue	= OSRGetSemiMinor (ogrEPSGSRSPtr, &ogrError);
				if (ogrError == OGRERR_NONE)
					*semiMinorAxisPtr = returnValue;
				
				if (*projectionCodePtr != kNotDefinedCode	&&
							*mapUnitsCodePtr != kNotDefinedCode && 
									datumNamePtr[0] > 0 && 
											ellipsoidNamePtr[0] > 0)
					parametersSetFlag = TRUE;
				
				}	// end "if (error == noErr)"
			
			OSRRelease (ogrEPSGSRSPtr);
			
			}	// end "if (ogrEPSGSRSPtr != NULL)"
	#endif	// include_gdal_capability
	
	return (parametersSetFlag);
			
}	// end "GDALSetReferenceSystemFromEPSGCode" 


      
#if include_gdal_capability             
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GDALSetStatePlaneParametersFromZone
//
//	Software purpose:	The purpose of this routine is to set up the projection
//							parameters for State Plane based on the input zone using the
//							GDAL code.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2012
//	Revised By:			Larry L. Biehl			Date: 03/24/2012

Boolean GDALSetStatePlaneParametersFromZone (
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
	OGRErr								error;
	OGRSpatialReferenceH				ogrStatePlaneSRSPtr;
	
	SInt16								projectionCode,
											referenceSystemCode;
	
	Boolean								parametersSetFlag = FALSE;
	
	
			// This is state plane. Try to get the parameters
			
	ogrStatePlaneSRSPtr = OSRNewSpatialReference (NULL);
	
	if (ogrStatePlaneSRSPtr != NULL)
		{
		error = OSRSetStatePlane (
									ogrStatePlaneSRSPtr, fipsZone, (datumCode == kNAD83Code));
		
		if (error == noErr)
			{
			GetStatePlaneZoneName (ogrStatePlaneSRSPtr, fipsZoneNamePtr);
						
					// Get the projection
				
			projectionCode = GetProjectionCodeFromOGRSpatialReference (
							(OGRSpatialReference*)ogrStatePlaneSRSPtr, &referenceSystemCode);
																								
			if (projectionCode != kNotDefinedCode)
				*projectionCodePtr = projectionCode;
				
			*longitudeCentralMeridianPtr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "central_meridian", 0, NULL);
			*latitudeOriginPtr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "latitude_of_origin", 0, NULL);
			*scaleFactorOfCentralMeridianPtr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "scale_factor", 1, NULL);
			*falseEastingPtr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "false_easting", 0, NULL);
			*falseNorthingPtr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "false_northing", 0, NULL);
			*standardParallel1Ptr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "standard_parallel_1", 0, NULL);
			*standardParallel2Ptr = OSRGetProjParm (
											ogrStatePlaneSRSPtr, "standard_parallel_2", 0, NULL);
			
			*falseOriginLongitudePtr = *longitudeCentralMeridianPtr;
			*falseOriginLatitudePtr = *latitudeOriginPtr;
			*falseOriginEastingPtr = *falseEastingPtr;
			*falseOriginNorthingPtr = *falseNorthingPtr;
			
			parametersSetFlag = TRUE;
			
			}	// end "if (error == noErr)"
		
		OSRRelease (ogrStatePlaneSRSPtr);
		
		}	// end "if (ogrStatePlaneSRSPtr != NULL)"
	
	return (parametersSetFlag);
			
}	// end "GDALSetStatePlaneParametersFromZone" 

  

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GDALDatasetH GetDataSetGroupNumber
//
//	Software purpose:	The purpose of this routine is to set the pixel size parameters
//							using the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/22/2012
//	Revised By:			Larry L. Biehl			Date: 04/22/2012

UInt32 GetDataSetGroupNumber (
				Handle								hdfDataSetsHandle,
				UInt32								dataSet)

{ 
	HdfDataSets*						hdfDataSetsPtr;
	
	UInt32								groupedNumber = 0;
	
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (hdfDataSetsHandle);
									
	if (hdfDataSetsPtr != NULL)
		groupedNumber = hdfDataSetsPtr[dataSet].groupedNumber;
									
	return (groupedNumber);
    
}	// end "GetDataSetGroupNumber"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetDatumCodeFromGDALAttributeString
//
//	Software purpose:	This routine determines the projection code from the input OGR 
//							spatial reference pointer.
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
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 GetDatumCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr,
				UCharPtr								datumNamePtr,
				Boolean								getCodeFlag)

{	
	UCharPtr								attributeStringPtr;
	
	SInt16								datumCode = kNotDefinedCode;
	
														
	attributeStringPtr = (UCharPtr)ogrSpatialReferenceCPtr->GetAttrValue ("Datum");
		
	if (attributeStringPtr != NULL)
		{						
		if (datumNamePtr != NULL)
			CtoPstring (attributeStringPtr, datumNamePtr);
			
		if (getCodeFlag)
			{			
			if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"Sphere", 6) == 0)
				datumCode = kSphereDatumCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Beijing_1954", 12) == 0)
				datumCode = kBeijing1954Code;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Campo_In", 8) == 0)
				datumCode = kCampoCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"Geocentric_Datum_of_Australia_1994", 
										34) == 0)
				datumCode = kGeocentricDatumAustralia1994Code;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"Greek_Geodetic", 
										14) == 0)
				datumCode = kGGRS87DatumCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"North_American_Datum_1927", 
										25) == 0)
				datumCode = kNAD27Code;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"NAD83_Canadian", 
										14) == 0)
				datumCode = kNAD83_CSRSCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"North_American_Datum_1983", 
										25) == 0)
				datumCode = kNAD83Code;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"New_Zealand", 
										11) == 0)
				datumCode = kNewZealandGeodeticDatum1949Code;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"NGO_1948_Oslo", 
										13) == 0)
				datumCode = kNGO1948_OslowCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"OSGB_1936", 9) == 0)
				datumCode = kDatum_OSGB_1936Code;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Pulkovo_1942", 12) == 0)
				datumCode = kPulkovo1942DatumCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Pulkovo_1995", 12) == 0)
				datumCode = kPulkovo1995DatumCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"South_American_Datum_1969", 
										25) == 0)
				datumCode = kSAD69DatumCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"WGS_1972", 8) == 0)
				datumCode = kWGS72DatumCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"WGS_1984", 8) == 0)
				datumCode = kWGS84Code;
				
			}	// end "if (getCodeFlag)"
	
		}	// end "if (attributeStringPtr != NULL)"
		
	return (datumCode);
			
}	// end "GetDatumCodeFromOGRSpatialReference" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetEllipsoidCodeFromOGRSpatialReference
//
//	Software purpose:	This routine determines the ellipsoid code from the input OGR 
//							spatial reference pointer.
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
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 GetEllipsoidCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr,
				UCharPtr								ellipsoidNamePtr,
				Boolean								getCodeFlag)

{	
	UCharPtr								attributeStringPtr;
	
	SInt16								ellipsoidCode = kNotDefinedCode;
	
							
	attributeStringPtr = (UCharPtr)((OGRSpatialReference*)ogrSpatialReferenceCPtr)->
																				GetAttrValue ("Spheroid");
		
	if (attributeStringPtr != NULL)
		{		
		if (ellipsoidNamePtr != NULL)
			CtoPstring (attributeStringPtr, ellipsoidNamePtr);
			
		if (getCodeFlag)
			{										
			if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"Sphere", 6) == 0)
				ellipsoidCode = kSphereEllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Airy 1830", 9) == 0)
				ellipsoidCode = kAiryEllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Australian", 10) == 0)
				ellipsoidCode = kAustralianEllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"Bessel Modified", 
										15) == 0)
				ellipsoidCode = kBesselModifiedEllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Bessel", 6) == 0)
				ellipsoidCode = kBesselEllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Clarke 1866", 11) == 0)
				ellipsoidCode = kClarke1866EllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Clarke 1880", 11) == 0)
				ellipsoidCode = kClarke1880EllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Everest", 7) == 0)
				ellipsoidCode = kEverestEllipsoidCode;

			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"GRS 1967", 8) == 0)
				ellipsoidCode = kGRS1967ModifiedEllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"GRS 1980", 8) == 0)
				ellipsoidCode = kGRS80EllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"GRS_1980", 8) == 0)
				ellipsoidCode = kGRS80EllipsoidCode;
				
			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"International 1924", 
										18) == 0)
						// The international 1909 is also known as international 1924
				ellipsoidCode = kInternational1909EllipsoidCode;

			else if (CompareStringsNoCase (
										attributeStringPtr, 
										(UCharPtr)"Krassowsky 1940", 
										15) == 0)
				ellipsoidCode = kKrassovskyEllipsoidCode;

			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"WGS 72", 6) == 0)
				ellipsoidCode = kWGS72EllipsoidCode;

			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"WGS84", 5) == 0)
				ellipsoidCode = kWGS84EllipsoidCode;

			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"WGS 84", 6) == 0)
				ellipsoidCode = kWGS84EllipsoidCode;

			else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"WGS_1984", 8) == 0)
				ellipsoidCode = kWGS84EllipsoidCode;
				
			}	// end "if (getCodeFlag)"

		}	// end "if (attributeStringPtr != NULL)"
		
	return (ellipsoidCode);
    
}	// end "GetEllipsoidCodeFromOGRSpatialReference"

  

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		GDALDatasetH GetGDALFileReference
//
//	Software purpose:	The purpose of this routine is to get the gdal file reference
//							for the specified data set. If none exists then one is created
//							for it if possible.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/22/2012
//	Revised By:			Larry L. Biehl			Date: 07/13/2016

GDALDatasetH GetGDALFileReference (
				Handle								hdfHandle,
				UInt16								numberHdfDataSets,
				SInt16								format,
				GDALDatasetH						fileHDS,
				HdfDataSets*						hdfDataSetsPtr,
				UInt32								dataSet)

{
	char									szKeyName[1024];
	
	char*									pszSubdatasetName;
											
	char**								metadata;
	
	GDALDatasetH						hDS;
	
	
	hDS = fileHDS;
	
	if (hdfDataSetsPtr == NULL)
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (hdfHandle);
									
	if (hdfDataSetsPtr != NULL)
		{
		hDS = (GDALDatasetH)hdfDataSetsPtr[dataSet].sdid;
		
		if (hDS == 0 && (format == kHDF5Type || format == kNITFType))
			{
					// Get a reference to the data set. It may not have been set in the
					// beginning. This is to handle case with slow irods fuse mount.
					
			//metadata = ((GDALDataset*)fileHDS)->GetMetadata ("SUBDATASETS");
			metadata = GDALGetMetadata (fileHDS, "SUBDATASETS");
			
			if (dataSet <= numberHdfDataSets) 
				{
				snprintf (szKeyName,
								sizeof (szKeyName),
								"SUBDATASET_%d_NAME",
								(unsigned int)dataSet);
				szKeyName[sizeof (szKeyName) - 1] = '\0';
				pszSubdatasetName = CPLStrdup (CSLFetchNameValue (metadata, szKeyName));
				hDS = GDALOpen (pszSubdatasetName, GA_ReadOnly);
				CPLFree (pszSubdatasetName);

				if (hDS != NULL)
					hdfDataSetsPtr[dataSet].sdid = hDS;	//(SInt32)					
				
				}	// end "if (dataSet <= numberHdfDataSets)"
							
			}	// end "if (hDS == 0 && (format == kHDF5Type || ..."
				
		}	// end "if (hdfDataSetsPtr != NULL)"
									
	return (hDS);
    
}	// end "GetGDALFileReference"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGDALSpecificNumericMetadataInformation
//
//	Software purpose:	This routine reads the requested numeric information in the gdal
//							metadata string. This routine is currently tuned for the attributes
//							in the hdf5 format files for HRLDAS output data. It may need to be
//							made more general if other data sets use this type of structure.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	= 0, No values were read.
//							!= 0, The number of values that were read.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/20/2012
//	Revised By:			Larry L. Biehl			Date: 12/21/2016

SInt16 GetGDALSpecificNumericMetadataInformation (
				char**		 						metadataStringPtrPtr,
				char*									masterGroupString,
				double*								valuePtr)
				
{	
	char*									stringPtr;
	
 	UInt32								index,
											stringLength;
 	
 	SInt16								returnCode = 0;
 	
 		
 	if (metadataStringPtrPtr != NULL)
		{
		index = 0;
		stringLength = (UInt32)strlen (masterGroupString);
		while (metadataStringPtrPtr[index] != NULL)
			{
			stringPtr = StrStrNoCase (metadataStringPtrPtr[index], masterGroupString);
			if (stringPtr != NULL)
				{
						// Requested title for string was found. Now get the value that
						// follows it.
						
				//stringPtr = metadataStringPtrPtr[index];
						
				returnCode = sscanf (&stringPtr[stringLength], 
											"%lf", 
											valuePtr);
											
				break;
				
				}	// end "if (CompareStringsNoCase (..."
													
			index++;
													
			}	// end "while (metadataStringPtrPtr[index] != NULL)"
			
		}	// end "if (metadataStringPtrPtr != NULL)"
 		
 	return (returnCode);
			
}	// end "GetGDALSpecificNumericMetadataInformation



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetGDALSpecificTextMetadataInformation
//
//	Software purpose:	This routine reads the requested alphanumeric information in the
// 						gdal metadata information.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	= 0, no string was read.
//							!= 0, the number of characters read.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/20/2012
//	Revised By:			Larry L. Biehl			Date: 12/21/2016

SInt16 GetGDALSpecificTextMetadataInformation (
				char**								metadataStringPtrPtr,
				char*									masterGroupString,
				char*									groupString)
				
{												
   UInt32 								index,
											stringLength;
 	
 	SInt16								returnCode;
 	
 		
 	returnCode = 0;
	
	if (metadataStringPtrPtr != NULL)
		{
		index = 0;
		stringLength = (UInt32)strlen (masterGroupString);
		while (metadataStringPtrPtr[index] != NULL)
			{
			if (StrStrNoCase (metadataStringPtrPtr[index], masterGroupString) != NULL)
				{
				if (groupString[0] == 0)
							// All that is wanted is whether the masterGroupSring exists
					returnCode = 1;
					
				else if (StrStrNoCase (metadataStringPtrPtr[index], groupString) != NULL)
					returnCode = 1;
					
				if (returnCode == 1)
					break;
				
				}	// end "if (CompareStringsNoCase (..."
													
			index++;
													
			}	// end "while (metadataStringPtrPtr[index] != NULL)"
			
		}	// end "if (metadataStringPtrPtr != NULL)"
 	 		
 	return (returnCode);
			
}	// end "GetGDALSpecificTextMetadataInformation

  

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetGDALTiePoints
//
//	Software purpose:	The purpose of this routine is to set the pixel size parameters
//							using the information in the GEOTIFF tag.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/17/2012
//	Revised By:			Larry L. Biehl			Date: 08/07/2014

void GetGDALTiePoints (
				FileInfoPtr							fileInfoPtr,
				GDALDatasetH						hDS)

{
	const GDAL_GCP*					gdalGCPPtr;
	
	ControlPointsPtr					controlPointsPtr;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	UInt32								geoIndex,
											index,
											numberControlPoints;
	
	//Boolean							checkLoadControlPointsFlag = FALSE,
	//										loadControlPointsFlag;
	
						
	numberControlPoints = GDALGetGCPCount (hDS);	
			
	mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);

				// At least 2 control points are expected.
	
	if (numberControlPoints >= 2 && mapProjectionHandle != NULL)
		{										
		gdalGCPPtr = GDALGetGCPs (hDS);
		
		if (gdalGCPPtr != NULL)
			{ 										
			/*					
					// The following lines are currently for loading control points into
					// the Corona data.
					
			loadControlPointsFlag = FALSE;
			count = imageFileDirectoryPtr->count;
			if (checkLoadControlPointsFlag &&
						mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize == 1 && 
								count == 6)
				{
				count = 72;
				loadControlPointsFlag = TRUE;
				
				}	// end "if (checkLoadControlPointsFlag && ..."
	
			if (loadControlPointsFlag)
				{
				geoTiePointsPtr[0] = 1147;
				geoTiePointsPtr[1] = 933;
				geoTiePointsPtr[3] = -86.60633;
				geoTiePointsPtr[4] = 39.53560;
				
				geoTiePointsPtr[6] = 100026;
				geoTiePointsPtr[7] = 1237;
				geoTiePointsPtr[9] = -83.49399;
				geoTiePointsPtr[10] = 40.12384;
				
				geoTiePointsPtr[12] = 101020;
				geoTiePointsPtr[13] = 7489;
				geoTiePointsPtr[15] = -83.36302;
				geoTiePointsPtr[16] = 39.96659;
				
				geoTiePointsPtr[18] = 1215;
				geoTiePointsPtr[19] = 8068;
				geoTiePointsPtr[21] = -86.58610;
				geoTiePointsPtr[22] = 39.33462;
				
				geoTiePointsPtr[24] = 1410;
				geoTiePointsPtr[25] = 4410;
				geoTiePointsPtr[27] = -86.58870;
				geoTiePointsPtr[28] = 39.44090;
				
				geoTiePointsPtr[30] = 49168;
				geoTiePointsPtr[31] = 1259;
				geoTiePointsPtr[33] = -85.09008;
				geoTiePointsPtr[34] = 39.81425;
				
				geoTiePointsPtr[36] = 49632;
				geoTiePointsPtr[37] = 3597;
				geoTiePointsPtr[39] = -85.06191;
				geoTiePointsPtr[40] = 39.76594;
				
				geoTiePointsPtr[42] = 100456;
				geoTiePointsPtr[43] = 3651;
				geoTiePointsPtr[45] = -83.44250;
				geoTiePointsPtr[46] = 40.06459;
				
				geoTiePointsPtr[48] = 50782;
				geoTiePointsPtr[49] = 7154;
				geoTiePointsPtr[51] = -85.00614;
				geoTiePointsPtr[52] = 39.69308;
				
				geoTiePointsPtr[54] = 22518;
				geoTiePointsPtr[55] = 4371;
				geoTiePointsPtr[57] = -85.83667;
				geoTiePointsPtr[58] = 39.59409;
				
				geoTiePointsPtr[60] = 22947;
				geoTiePointsPtr[61] = 1014;
				geoTiePointsPtr[63] = -85.83877;
				geoTiePointsPtr[64] = 39.68333;
				
				geoTiePointsPtr[66] = 22510;
				geoTiePointsPtr[67] = 7467;
				geoTiePointsPtr[69] = -85.82248;
				geoTiePointsPtr[70] = 39.51889;
					
				}	// end "if (loadControlPointsFlag)"
			*/
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
									GetHandlePointer (mapProjectionHandle, kLock);

					// Note: If this is a geographic description, make sure x-direction tie points are within 
					// range of -180 to 180 and not 0 to 360.
			
				                          
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = gdalGCPPtr->dfGCPX;
			if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																			kGeographicRSCode &&
																				gdalGCPPtr->dfGCPX > 180)
				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 -= 360;
			
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = gdalGCPPtr->dfGCPY; 
                                                                      
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
																						gdalGCPPtr->dfGCPX;
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
																						gdalGCPPtr->dfGCPY;
																							
					// If the number of control points is 4 and they represent the four 
					// corners of the image and the x location values are the same for top 
					// and bottom and the y location values are the same for left and 
					// right, then one can get the x and y pixel size values. One does 
					// not need to get a polynomial model from the control points.
					
			if (numberControlPoints == 4 &&
					gdalGCPPtr[3].dfGCPPixel == fileInfoPtr->numberColumns && 
						gdalGCPPtr[3].dfGCPLine == fileInfoPtr->numberLines && 
							gdalGCPPtr[0].dfGCPX == gdalGCPPtr[1].dfGCPX && 
								gdalGCPPtr[2].dfGCPX == gdalGCPPtr[3].dfGCPX && 
									gdalGCPPtr[0].dfGCPY == gdalGCPPtr[2].dfGCPY && 
										gdalGCPPtr[1].dfGCPY == gdalGCPPtr[3].dfGCPY)
				{
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
					(gdalGCPPtr[3].dfGCPX - gdalGCPPtr[0].dfGCPX)/gdalGCPPtr[3].dfGCPPixel;
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
					(gdalGCPPtr[0].dfGCPY - gdalGCPPtr[3].dfGCPY)/gdalGCPPtr[3].dfGCPLine;
							
				if (gdalGCPPtr[0].dfGCPPixel == 0 && gdalGCPPtr[0].dfGCPLine == 0)
							// The current location value represents the location of the 
							// upper left corner of the upper left pixel. Change to the 
							// center of the upper left pixel.
					AdjustUpperLeftPixelLocationToCenter (mapProjectionInfoPtr);
				
				}	// end "if (numberControlPoints == 4)"
				
					// If there are 3 or more control points, save them in case they will
					// be need to create a polynomial model to relate line/column to 
					//	map location.
					
			else if (numberControlPoints >= 3)
				{					
						// Get memory for the control points.
						
				fileInfoPtr->controlPointsHandle = MNewHandle (sizeof (ControlPoints) + 
								numberControlPoints * (4*sizeof (double) + sizeof (SInt16)));
								
				controlPointsPtr = (ControlPointsPtr)GetHandlePointer (
																	fileInfoPtr->controlPointsHandle);
				if (controlPointsPtr != NULL)
					controlPointsPtr->count = numberControlPoints;
								
				controlPointsPtr = GetControlPointVectorPointers (
																	fileInfoPtr->controlPointsHandle,
																	kLock);
																	
				if (controlPointsPtr != NULL)
					{			
					geoIndex = 0;
					for (index=0; index<numberControlPoints; index++)
						{
						controlPointsPtr->easting1Ptr[index] = 
																	gdalGCPPtr[geoIndex].dfGCPPixel;
						controlPointsPtr->northing1Ptr[index] = 
																	gdalGCPPtr[geoIndex].dfGCPLine;
						
						controlPointsPtr->easting2Ptr[index] = gdalGCPPtr[geoIndex].dfGCPX;
						if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																				kGeographicRSCode &&
																		gdalGCPPtr[index].dfGCPX > 180)
							controlPointsPtr->easting2Ptr[index] -= 360;
						
						controlPointsPtr->northing2Ptr[index] = gdalGCPPtr[geoIndex].dfGCPY;
						
						controlPointsPtr->statusPtr[index] = 1;
						
						geoIndex++;
						
						}	// end "for (index=0; index<numberControlPoints; index++)"
					
					}	// end "if (controlPointsPtr->easting1Ptr != NULL)"
					
				CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
			
						// Get polynomial model to relate line/column to map location if
						// needed and can.
					
				if (mapProjectionInfoPtr->gridCoordinate.projectionCode <= 0)
					{
					GetPolynomialModel (fileInfoPtr);
					
							// Update map values for the upper left pixel.  The current 
							// values may not represent the map values for pixel (1,1).
							
					UpdateUpperLeftMapValues (fileInfoPtr);
					
					}	// end "if (mapProjectionInfoPtr->gridCoordinate.code <= 0)"
				
				}	// end "if (numberControlPoints >= 3)"
				
			CheckAndUnlockHandle (mapProjectionHandle);
			
			}	// end "if (gdalGCPPtr != NULL)"
			
		}	// end "if (numberControlPoints >= 2 && mapProjectionHandle != NULL)"
			
}	// end "GetGDALTiePoints"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetGDALTopToBottomFlag
//
//	Software purpose:	This routine determines if the image data in the file needs to
//							be treated as being stored bottom to top. Currently
//							the only case where this occurs is with the HRLDAS model output
//							in hdf5 files. The information needed is stored in the gdal 
//							metadata.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/20/2012
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean GetGDALTopToBottomFlag (
				FileInfoPtr 						fileInfoPtr,
				SInt16								format,
				GDALDatasetH						hDS)

{	
	char									**metadataStringPtrPtr = NULL;
	
	SInt16								returnCode;
	
	Boolean								returnFlag = FALSE;


			// Get metadata if there is any.
			
	if (format == kHDF5Type && hDS != NULL)
		metadataStringPtrPtr = GDALGetMetadata (hDS, NULL);
	
	if (metadataStringPtrPtr != NULL)
		{					
		returnCode = GetGDALSpecificTextMetadataInformation (metadataStringPtrPtr,
																				(char*)"TITLE=",
																				(char*)"FROM HRLDAS");
																
		if (returnCode == 1)
			returnFlag = TRUE;
			
		if (returnFlag)
			{
					// Check if this is a HRLDAS data set that is not be be flipped
					// Get metadata for the specified data set.
					
			metadataStringPtrPtr = GDALGetMetadata (GDALGetRasterBand (hDS, 1), NULL);	
				
			if (metadataStringPtrPtr != NULL)
				{
				returnCode = GetGDALSpecificTextMetadataInformation (
																metadataStringPtrPtr,
																(char*)"data_stored_bottom_to_top=",
																(char*)"no");
				if (returnCode == 1)
					returnFlag = FALSE;
															
				}	// end "if (metadataStringPtrPtr != NULL)"
				
			}	// end "if (returnFlag)"
																																		
		}	// end "if (metadataStringPtrPtr != NULL)"

	else if (format == kArcGISASCIIGridType)
		{			
					// If the is the ArcGIS Ascii format and the file name starts with mint
					// or maxt, then assume the image needs to be flipped from top to 
					// bottom. This is for the U2U project.
					
		FileStringPtr fileNamePtr = 
								(FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);
		if (!CompareStringsNoCase (fileNamePtr, (UCharPtr)"mint", 4) ||
						!CompareStringsNoCase (fileNamePtr, (UCharPtr)"maxt", 4) ||
										!CompareStringsNoCase (fileNamePtr, (UCharPtr)"gdd", 3))
			returnFlag = TRUE;
		
		}	// end "if (format == kArcGISASCIIGridType)"
			
	return (returnFlag);

}	// end "GetGDALTopToBottomFlag"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetEPSGCodeName
//
//	Software purpose:	This routine gets the name for the epsg code represented by
//							the input OGS spatial reference.
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
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetEPSGCodeName (
				OGRSpatialReferenceH				ogrEPSGSRSPtr,
				UCharPtr								epsgNamePtr)

{	
	UCharPtr								attributeStringPtr;


	epsgNamePtr[0] = 0;
	
	attributeStringPtr = 
				(UCharPtr)((OGRSpatialReference*)ogrEPSGSRSPtr)->GetAttrValue ("PROJCS");
				
	if (attributeStringPtr != NULL)
		{
				// First check if the string contains "unkown". This implies that gdal 
				// cannot access the epsg folder.
				
		if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"unnamed", 7) == 0)
			{
			VerifyEPSG_CSVFolderExits (attributeStringPtr);
			//CtoPstring ((char*)"epsg_csv folder not found", fipsZoneNamePtr);

			}	// end "if (CompareStringsNoCase (attributeStringPtr, "unnamed", 7) == 0)"
		
		else	// !CompareStringsNoCase
			CtoPstring (attributeStringPtr, epsgNamePtr);
		
		}	// end "if (attributeStringPtr != NULL)"
		
	else	// attributeStringPtr == NULL
		{
				// Invalid epsg number. List message indicating such.
				
		MGetString ((unsigned char*)epsgNamePtr, kDialogStrID, IDS_Dialog31);
		
		}	// end "else attributeStringPtr == NULL"

}	// end "GetEPSGCodeName"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMapUnitsCodeFromOGRSpatialReference
//
//	Software purpose:	This routine determines the map units code from the input OGR 
//							spatial reference pointer.
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
//	Revised By:			Larry L. Biehl			Date: 05/15/2012

SInt16 GetMapUnitsCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr)

{	
	UCharPtr								attributeStringPtr;
	
	SInt16								mapUnitsCode = 0;
	
							
	attributeStringPtr = (UCharPtr)ogrSpatialReferenceCPtr->GetAttrValue ("Unit");
			
	if (attributeStringPtr != NULL)
		{
				// Units code
		
		if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"DMS", 3) == 0)
				// Decimal seconds
			mapUnitsCode = kAVDegreesMinutesSecondsCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"DM", 2) == 0)
				// Decimal minutes
			mapUnitsCode = kAVDecimalMinutesCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"DS", 2) == 0)
				// Decimal seconds
			mapUnitsCode = kAVDecimalSecondsCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"D", 1) == 0)
				// Decimal degrees
			mapUnitsCode = kAVDecimalDegreesCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"rad", 3) == 0)
				// Decimal seconds
			mapUnitsCode = kAVRadiansCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"MET", 3) == 0)			
			mapUnitsCode = kMetersCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"foot_us", 7) == 0)			
			mapUnitsCode = kUSSurveyFeetCode;
		
		else if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"foot", 4) == 0)			
			mapUnitsCode = kFeetCode;
		
		else if (CompareStringsNoCase (
									attributeStringPtr, (UCharPtr)"US survey foot", 14) == 0)			
			mapUnitsCode = kUSSurveyFeetCode;
		
		else if (CompareStringsNoCase (
									attributeStringPtr, (UCharPtr)"us_survey_feet", 14) == 0)			
			mapUnitsCode = kUSSurveyFeetCode;
		
		}	// end "if (attributeStringPtr != NULL)"
		
	return (mapUnitsCode);

}	// end "GetMapUnitsCodeFromOGRSpatialReference"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetNewGDALFileReference
//
//	Software purpose:	This routine gets a new set of hdf file references for the 
//							input file.  It is used when setting up a project file.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	True if set up okay.
//							False if not setup.
//
// Called By:			LoadProjectFileAndLayerInformation in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2009
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean GetNewGDALFileReference (
				FileInfoPtr		 					fileInfoPtr)
				
{	
	char									szKeyName[1024];
	UInt8									filePathString[512];
											
	char									*pszSubdatasetName;
	FileStringPtr						filePathPtr;
											
	char**								metadata;
	
	HdfDataSets*						hdfDataSetsPtr;
	
	GDALDatasetH						hDS;
	
	UInt32								dataSet;
	
	SInt16								returnCode;
	
	Boolean								continueFlag = TRUE;
												
				   
   returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
														filePathString, 
														510,
														&filePathPtr,
														kGDAL_Library);
	if (returnCode != noErr)	
																							return (FALSE);	
					
	hDS = GDALOpen ((char*)filePathPtr, GA_ReadOnly);
	fileInfoPtr->gdalDataSetH = hDS;
	
	if (hDS == NULL)
																							return (FALSE);	
		
	else	// hDS != NULL
		{
		gNumberOfOpenFiles++;
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (
																fileInfoPtr->hdfHandle,
																kLock,
																kNoMoveHi);
																
		if (hdfDataSetsPtr != NULL)
			{
			//metadata = ((GDALDataset*)hDS)->GetMetadata ("SUBDATASETS");
			metadata = GDALGetMetadata (hDS, "SUBDATASETS");
			
			for (dataSet=1; dataSet<=fileInfoPtr->numberHdfDataSets; dataSet++) 
				{
				snprintf (szKeyName,
								sizeof (szKeyName),
								"SUBDATASET_%d_NAME",
								(unsigned int)dataSet);
				szKeyName[sizeof (szKeyName) - 1] = '\0';
				pszSubdatasetName = CPLStrdup (CSLFetchNameValue (metadata, szKeyName));
				hDS = GDALOpen (pszSubdatasetName, GA_ReadOnly);
				CPLFree (pszSubdatasetName);
				
				if (hDS != NULL)
					hdfDataSetsPtr[dataSet].sdid = hDS;	//(SInt32)					
				
				else	// hDS == NULL
					{
					hdfDataSetsPtr[dataSet].sdid = 0;
					continueFlag = FALSE;
					
					}	// end "else hDS == NULL"
			
				}	// end "for (dataSet=1; dataSet<=fileInfoPtr->numberHdfDataSets; ..."
				
			CheckAndUnlockHandle (fileInfoPtr->hdfHandle);
				
			}	// end "if (hdfDataSetsPtr != NULL)"
			
		}	// end "else hDS != NULL"
			
	return (continueFlag);
    
}	// end "GetNewGDALFileReference"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetProjectionCodeFromOGRSpatialReference
//
//	Software purpose:	This routine determines the projection code from the input OGR 
//							spatial reference pointer.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2012
//	Revised By:			Larry L. Biehl			Date: 04/09/2012

SInt16 GetProjectionCodeFromOGRSpatialReference (
				OGRSpatialReference*				ogrSpatialReferenceCPtr,
				SInt16*								referenceSystemCodePtr)

{	
	UCharPtr								attributeStringPtr;
	
	SInt16								projectionCode = kNotDefinedCode;
	
						
	attributeStringPtr = (UCharPtr)((OGRSpatialReference*)ogrSpatialReferenceCPtr)->
																			GetAttrValue ("Projection");
							
	if (attributeStringPtr != NULL)
		{						
		if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"GEOGRAPHIC", 10) == 0)
			*referenceSystemCodePtr = kGeographicRSCode;
	
		else if (CompareStringsNoCase (
										attributeStringPtr, (UCharPtr)"Albers_Con", 10) == 0)
			projectionCode = kAlbersConicalEqualAreaCode;
	
		else if (CompareStringsNoCase (
									attributeStringPtr, 
									(UCharPtr)"Azimuthal_Equidistant", 
									21) == 0)
			projectionCode = kAzimuthalEquidistantCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Cylindrical_Equal", 
													17) == 0)
			projectionCode = kCylindricalEqualAreaCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Equidistant_Conic", 
													17) == 0)
			projectionCode = kPolarStereographicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Equirectangular", 
													15) == 0)
			projectionCode = kEquirectangularCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Gnomonic", 
													8) == 0)
			projectionCode = kGnomonicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"LAMBERT_AZ", 
													10) == 0)
			projectionCode = kLambertAzimuthalEqualAreaCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Lambert_Conformal", 
													17) == 0)
			projectionCode = kLambertConformalConicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Mercator", 
													8) == 0)
			projectionCode = kMercatorCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Miller_Cylindrical", 
													18) == 0)
			projectionCode = kMillerCylindricalCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"New_Zealand", 
													11) == 0)
			projectionCode = kNewZealandMapGridCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"ORTHOGRAPH", 
													10) == 0)
			projectionCode = kOrthographicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Polar_Stereographic", 
													19) == 0)
			projectionCode = kPolarStereographicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Polyconic", 
													9) == 0)
			projectionCode = kPolyconicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Robinson", 
													8) == 0)
			projectionCode = kRobinsonCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Sinusoidal", 
													10) == 0)
			projectionCode = kSinusoidalCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Stereographic", 
													13) == 0)
			projectionCode = kStereographicCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Transverse_Mercator_South", 
													25) == 0)
			projectionCode = kTransvMercatorSouthOrientedCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Transverse_Mercator", 
													19) == 0)
			projectionCode = kTransverseMercatorCode;
	
		else if (CompareStringsNoCase (attributeStringPtr, 
													(UCharPtr)"Vandergrinten", 
													13) == 0)
			projectionCode = kVanderGrintenICode;
	
		}	// end "if (attributeStringPtr != NULL)"
		
	return (projectionCode);
    
}	// end "GetProjectionCodeFromOGRSpatialReference"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetStatePlanePCSCode
//
//	Software purpose:	This routine gets the pcs code for the input state plane
//							map system and fips zone number.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/26/2012
//	Revised By:			Larry L. Biehl			Date: 03/26/2012

SInt32 GetStatePlanePCSCode (
				SInt16								datumCode,
				SInt16								fipsZone)

{	
	SInt32								pcsCode = 0;
	
			// Get the pcs code
	
	if (datumCode == kNAD27Code)	
		pcsCode = GTIFMapSysToPCS (MapSys_State_Plane_27, 0, fipsZone);
		
	else if (datumCode == kNAD83Code)
		pcsCode = GTIFMapSysToPCS (MapSys_State_Plane_83, 0, fipsZone);
		
	return (pcsCode);
    
}	// end "GetStatePlanePCSCode"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetStatePlaneZoneName
//
//	Software purpose:	This routine lists the gdal error message in the output text
//							window and displays an alert if the error is fatal.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/24/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetStatePlaneZoneName (
				OGRSpatialReferenceH				ogrStatePlaneSRSPtr,
				UCharPtr								fipsZoneNamePtr)

{	
	UCharPtr								attributeStringPtr;


	fipsZoneNamePtr[0] = 0;
	
	attributeStringPtr = (UCharPtr)((OGRSpatialReference*)ogrStatePlaneSRSPtr)->
																				GetAttrValue ("PROJCS");
	if (attributeStringPtr != NULL)
		{
				// First check if the string contains "unkown". This implies that gdal 
				// cannot access the epsg folder.
				
		if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"unnamed", 7) == 0)
			{
			VerifyEPSG_CSVFolderExits (attributeStringPtr);
			CtoPstring ((UCharPtr)"epsg_csv folder not found", fipsZoneNamePtr);

			}	// end "if (CompareStringsNoCase (attributeStringPtr, "unnamed", 7) == 0)"
		
		else	// !CompareStringsNoCase
				// Get the FIPS Zone Name
				// Could be dangerous by hard wiring to location 8. This could change 
				// with gdal versions.
			CtoPstring ((UCharPtr)&attributeStringPtr[8], fipsZoneNamePtr);
		
		}	// end "if (attributeStringPtr != NULL)"
    
}	// end "GetStatePlaneZoneName"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HandleGDALErrorMessage
//
//	Software purpose:	This routine lists the gdal error message in the output text
//							window and displays an alert if the error is fatal.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/18/2009
//	Revised By:			Larry L. Biehl			Date: 12/18/2009

void HandleGDALErrorMessage (
				SInt16								errCode)

{	
	char*									gdalLastErrorMessagePtr;
	
	
	if (errCode != noErr)
		{
		gdalLastErrorMessagePtr = (char*)CPLGetLastErrorMsg ();
		
		sprintf ((char*)gTextString, 
					"%s  GDAL routines data access error:%s  ", 
					gEndOfLine,
					gEndOfLine);
		
		OutputString (NULL,
							(char*)gTextString, 
							0, 
							(gOutputCode | gOutputForce1Code), 
							TRUE);
							
		OutputString (NULL, 
							gdalLastErrorMessagePtr,
							0, 
							(gOutputCode | gOutputForce1Code), 
							TRUE);
		
		sprintf ((char*)gTextString, 
					"%s", 
					gEndOfLine);
		
		OutputString (NULL,
							(char*)gTextString, 
							0, 
							(gOutputCode | gOutputForce1Code), 
							TRUE);
		
		if (CPLGetLastErrorType () == CE_Fatal)
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								IDS_Alert137,
								0,
								NULL);
		
		}	// end "if (errCode != noErr)"
    
}	// end "HandleGDALErrorMessage"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListGDALDataSetAttributes
//
//	Software purpose:	This routine will list information that data formats being
//							read by gdal contain.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/13/2011
//	Revised By:			Larry L. Biehl			Date: 07/27/2018

Boolean ListGDALDataSetAttributes (
				FileInfoPtr 						fileInfoPtr,
				Boolean								listAllDataSetInfoFlag)
				
{
	const char							*descriptionStringPtr;
	char									**metadataStringPtrPtr = NULL;
	
	GDALDatasetH						hDS;	
	
	UInt32								bandIndex,											
											dataSet,
											dataSetEnd,
											dataSetStart,
											index,
											numberChannels;
   										
   SInt16								stringLength = 0;
   										
   Boolean								continueFlag = TRUE;
   
   
	if (fileInfoPtr->format != kGRIBType && 
			fileInfoPtr->format != kHDF5Type &&
				fileInfoPtr->format != kHDF4Type2 &&
					fileInfoPtr->format != kNETCDF2Type &&
						fileInfoPtr->format != kENVIType &&
							fileInfoPtr->format != kNITFType)
																					return (continueFlag);
																				
			// List global file metadata if there is any.
			
	hDS = GetGDALFileReference (fileInfoPtr->hdfHandle,
											fileInfoPtr->numberHdfDataSets,
											fileInfoPtr->format, 
											(GDALDatasetH)fileInfoPtr->hdf5FileID,
											NULL,
											fileInfoPtr->hdfDataSetSelection);
		
	if (hDS != NULL)
		metadataStringPtrPtr = GDALGetMetadata (hDS, NULL);
	
	if (metadataStringPtrPtr != NULL)
		{						
		stringLength = sprintf ((char*)&gTextString[0],
										"    Global file attribute information:%s",
										gEndOfLine);
										
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,  
												stringLength,  
												gOutputTextH);
										
		index = 0;
		while (metadataStringPtrPtr[index] != NULL)
			{
			if (continueFlag) 
				{
				stringLength = sprintf ((char*)&gTextString[0],
												"      ");
				continueFlag = ListString ((char*)gTextString,  
													stringLength,  
													gOutputTextH);
													
				}	// end "if (continueFlag)"
			
			if (continueFlag) 
				{
				stringLength = (SInt16)strlen (metadataStringPtrPtr[index]);
				stringLength = MIN (stringLength, 32000);
				continueFlag = ListString (metadataStringPtrPtr[index],  
													stringLength,  
													gOutputTextH);
													
				}	// end "if (continueFlag)"
	
					// Add end of line character
					
			if (continueFlag) 
				{
				stringLength = sprintf ((char*)&gTextString[0],
												"%s",
												gEndOfLine);
				continueFlag = ListString ((char*)gTextString,  
													stringLength,  
													gOutputTextH);
													
				}	// end "if (continueFlag)"
													
			index++;
			
			if (!continueFlag)
				break;
													
			}	// end "while (metadataStringPtrPtr[index] != NULL)"
						
		stringLength = sprintf ((char*)&gTextString[0],
										"    End global file attribute information%s%s",
										gEndOfLine,
										gEndOfLine);
										
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,  
												stringLength,  
												gOutputTextH);
												
		}	// end "if (metadataStringPtrPtr != NULL)"	

	#if include_hdf5_capability
		if (fileInfoPtr->format == kHDF5Type)
			continueFlag = ListHDF5FileInformation (fileInfoPtr);
	#endif		// include_hdf5_capability
						
	stringLength = sprintf ((char*)&gTextString[0],
									"    Selected data set attribute information:%s",
									gEndOfLine);
									
	if (continueFlag)
		continueFlag = ListString ((char*)gTextString,  
											stringLength,  
											gOutputTextH);
																				
	if (fileInfoPtr->hdfHandle == NULL)
		{
		dataSetStart = dataSetEnd = 1;
		numberChannels = fileInfoPtr->numberChannels;
			
		}	// end "if (fileInfoPtr->hdfHandle == NULL)"
		
	else	// fileInfoPtr->hdfHandle != NULL
		{
		if (listAllDataSetInfoFlag)
			{
			dataSetStart = 1;
			dataSetEnd = fileInfoPtr->numberHdfDataSets;
			
			}	// end "if (listAllDataSetInfoFlag)"
			
		else	// !listAllDataSetInfoFlag
			{
			dataSetStart = fileInfoPtr->hdfDataSetSelection;
			dataSetEnd = fileInfoPtr->hdfDataSetSelection;
			
			if (GetDataSetGroupNumber (
							fileInfoPtr->hdfHandle, fileInfoPtr->hdfDataSetSelection) > 0)
				dataSetEnd += fileInfoPtr->numberChannels - 1;
			
			}	// end "else !listAllDataSetInfoFlag"
			
		numberChannels = 1;
		
		}	// end "else fileInfoPtr->hdfHandle != NULL"

	for (dataSet=dataSetStart; dataSet<=dataSetEnd; dataSet++)
		{
		stringLength = 0; 
		hDS = GetGDALFileReference (fileInfoPtr->hdfHandle,
												fileInfoPtr->numberHdfDataSets,
												fileInfoPtr->format,
												(GDALDatasetH)fileInfoPtr->hdf5FileID,
												NULL,
												dataSet);
	
		if (hDS != NULL)
			{
			for (bandIndex=1; bandIndex<=numberChannels; bandIndex++)
				{
						// List band description. 
						
				descriptionStringPtr = 
										GDALGetDescription (GDALGetRasterBand (hDS, bandIndex));
				
				if (descriptionStringPtr != NULL && descriptionStringPtr[0] != 0)
					{
					stringLength = sprintf ((char*)gTextString,
													"    Band %ld: description = %s%s", 
													bandIndex,
													descriptionStringPtr,
													gEndOfLine);

					if (continueFlag)
						continueFlag = ListString ((char*)gTextString,  
															stringLength,  
															gOutputTextH);		
															
					}	// end "if (descriptionStringPtr != NULL && ..."
					
						// Get metadata for the band.
						
				metadataStringPtrPtr = 
									GDALGetMetadata (GDALGetRasterBand (hDS, bandIndex), NULL);	
				
				if (metadataStringPtrPtr != NULL)
					{
					index = 0;
					while (metadataStringPtrPtr[index] != NULL)
						{
						stringLength = sprintf ((char*)&gTextString[0],
														"      %s%s", 
														metadataStringPtrPtr[index],
														gEndOfLine);

						if (continueFlag)
							continueFlag = ListString ((char*)gTextString,  
																stringLength,  
																gOutputTextH);
																
						index++;
						
						if (!continueFlag)
							break;
																
						}	// end "while (metadataStringPtrPtr[index] != NULL)"
															
					}	// end "if (metadataStringPtrPtr != NULL)"
						
				if (!continueFlag)
					break;
					
						// Insert a blank line.
				
				if (stringLength > 0 && dataSet < dataSetEnd)
					continueFlag = OutputString (NULL, 
															gEndOfLine, 
															gNumberOfEndOfLineCharacters, 
															gOutputForce1Code, 
															continueFlag);
					
				}	// end "for (bandIndex=1; bandIndex<=numberChannels; ..."
				
			}	// end "if (hDS != NULL)"
			
		}	// end "for (dataSet=dataSetStart; dataSet<=dataSetEnd; dataSet++)"
						
	stringLength = sprintf ((char*)&gTextString[0],
									"    End selected data set attribute information%s%s",
									gEndOfLine,
									gEndOfLine);
									
	if (continueFlag)
		continueFlag = ListString ((char*)gTextString,  
											stringLength,  
											gOutputTextH);
		
	#if include_hdf5_capability
				// Not set up for now. Doing a different way.
		//continueFlag = ListHDF5DataSetAttributes (fileInfoPtr, listAllDataSetInfoFlag);
	#endif		// include_hdf5_capability
		
	return (continueFlag);
	
}	// end "ListGDALDataSetAttributes"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadGDALHeaderInformation
//
//	Software purpose:	This routine reads the file specifications for the selected 
//							image (and subdataset) using the gdal routines. And then loads
//							the information into the MultiSpec file information structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:			ReadHeaderWithGDALLibrary
//
//	Coded By:			Larry L. Biehl			Date: 04/18/2012
//	Revised By:			Larry L. Biehl			Date: 03/14/2014

SInt16 LoadGDALHeaderInformation (
				GDALDatasetH						hDS,
				FileInfoPtr 						fileInfoPtr,
				SInt16								format)

{	
	double								noDataValue;
	
	UInt32								numberChannels,
											numberColumns,
											numberLines,
											xBlockSize,
											yBlockSize;
	
	SInt16								bandInterleaveFormat,
											gdalDataTypeCode,
											returnCode = 0;
											
	UInt16								dataCompressionCode,
											dataTypeCode,
											numberBits,
											numberBytes;
											
	Boolean								noDataValueFlag,
											signedDataFlag;
	
	
	if (hDS != NULL)
		{		
		returnCode = ReadGDALHeaderInformation (hDS,
																format,
																&numberChannels,
																&numberLines,
																&numberColumns,
																&numberBytes,
																&numberBits,
																&dataTypeCode,
																&dataCompressionCode,
																&bandInterleaveFormat,
																&signedDataFlag,
																&xBlockSize, 
																&yBlockSize,
																&gdalDataTypeCode,
																&noDataValueFlag,
																&noDataValue);
		
		if (returnCode == 0)
			{
			fileInfoPtr->numberColumns = numberColumns;
			fileInfoPtr->numberLines = numberLines;
			fileInfoPtr->numberChannels = (UInt16)numberChannels;
		
			fileInfoPtr->numberBytes = numberBytes;
			fileInfoPtr->numberBits = numberBits;
			fileInfoPtr->dataTypeCode = dataTypeCode;
			fileInfoPtr->signedDataFlag = signedDataFlag;
			
			fileInfoPtr->bandInterleave = kBSQ;
			fileInfoPtr->gdalBandInterleave = bandInterleaveFormat;
			
			if (fileInfoPtr->numberBits <= 16)
				fileInfoPtr->numberBins = 
									(UInt32)ldexp ((double)1, fileInfoPtr->numberBits);
			
			else	// fileInfoPtr->numberBits > 16
				fileInfoPtr->numberBins = 2048;
			
			fileInfoPtr->gdalDataSetH = hDS;
			fileInfoPtr->gdalDataTypeCode = gdalDataTypeCode;
			
			fileInfoPtr->blockHeight = yBlockSize;
			fileInfoPtr->blockWidth = xBlockSize;
			
			fileInfoPtr->noDataValueFlag = noDataValueFlag;
			fileInfoPtr->noDataValue = noDataValue;
			/*
			if (format == kTIFFType &&
						yBlockSize == numberLines && 
							xBlockSize == numberColumns && 
								numberChannels > 1 && 
									yBlockSize > 1)
					//(UInt32)yBlockSize * (UInt32)xBlockSize * (UInt32)*numberBytesPtr > 
					//																				SInt32_MAX)
			
						// If the image is not tiled or stripped then, need to set up 
						// MultiSpec to store the data for the entire single tile (image). 
						// It takes more memory but if this is not done, the tiff library 
						// will read the entire image for each line read. Reading the image 
						// becomes very slow. I have not figured out a way to get around 
						// this.
						// The approached used below does not work well. It needs more 
						// work ... so commenting out for now.
			
				{										
				bytesPerStrip = yBlockSize * 
								fileInfoPtr->numberColumns * 
													fileInfoPtr->numberBytes;
				
				bytesPerStripForAllChannels = bytesPerStrip * fileInfoPtr->numberChannels;
				
				stripsPerChannel = fileInfoPtr->numberLines/yBlockSize;
				
				lastBytesPerStrip = (fileInfoPtr->numberLines - 
													stripsPerChannel * yBlockSize) *
																fileInfoPtr->numberColumns * 
																				fileInfoPtr->numberBytes;
				
				if (lastBytesPerStrip > 0)
					stripsPerChannel++;
				
				returnCode = LoadHierarchalFileStructure (fileInfoPtr,
																		stripsPerChannel,
																		NULL,
																		NULL,
																		yBlockSize,
																		bytesPerStrip,
																		lastBytesPerStrip,
																		bytesPerStripForAllChannels);
					
				}	// end "if (yBlockSize > 1)"
			*/
			fileInfoPtr->treatLinesAsBottomToTopFlag = 
												GetGDALTopToBottomFlag (fileInfoPtr, format, hDS);
												
			}	// end "if (returnCode == noErr)" 
		
		else	// returnCode != noErr
			returnCode = 1;
			
		}	// end "if (hDS != NULL)"
	
	else	// hDS == NULL
		returnCode = 1;
	
	return (returnCode);
	
}	// end "LoadGDALHeaderInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadGDALInformation
//
//	Software purpose:	This routine reads the information for the file using the GDAL 
//							libraries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/22/2012
//	Revised By:			Larry L. Biehl			Date: 05/06/2015

SInt16 LoadGDALInformation (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr, 
				SInt16								format)

{	
	#if include_hdf5_capability
		HdfDataSets*						hdfDataSetsPtr = NULL;
		SInt32								numberDataSets = 0;
	#endif	// include_hdf5_capability
		
	GDALDatasetH						hDS,
											saved_hDS = 0;
	
	Handle								hdfDataSetsHandle = NULL;	
	
	SInt32								dataSetIndex = 0;
											
	SInt16								gdalReturnCode = noErr,
											numberClasses;
		
	//time_t								startTime;
	
	
	if (fileInfoPtr->gdalDataSetH != NULL)
		{
		hDS = fileInfoPtr->gdalDataSetH;
		
		#if include_hdf5_capability
			if (format == kHDF5Type || format == kNETCDF2Type ||
													format == kHDF4Type2 || format == kNITFType)
				{
				saved_hDS = hDS;
				LoadHdf5DataSetNames (hDS,
												format,
												fileInfoPtr,
												&hdfDataSetsHandle,
												&hdfDataSetsPtr,
												&numberDataSets,
												&dataSetIndex,
												&hDS);
					 
				//ListCPUTimeInformation (NULL, true, startTime);
												
				}	// end format == kHDF5Type || ...
		#endif	// include_hdf5_capability	

		gdalReturnCode = LoadGDALHeaderInformation (hDS, fileInfoPtr, format);
		
		#if include_hdf5_capability
			if (gdalReturnCode == noErr && 
					(format == kHDF5Type || format == kNETCDF2Type ||
													format == kHDF4Type2 || format == kNITFType))
				{
				fileInfoPtr->numberHdfDataSets = (UInt16)numberDataSets;
				
				gdalReturnCode = SetUpHDF5_FileInformation (fileInfoPtr, 
																			hdfDataSetsPtr,
																			dataSetIndex,
																			TRUE);
				
				}	// end "if (gdalReturnCode == noErr && (fileType == kHDF5Type || ..."
		#endif	// include_hdf5_capability
		
		if (gdalReturnCode == noErr)
			{
			if (headerRecordPtr != NULL && format == kTIFFType)
				{
				fileInfoPtr->swapHeaderBytesFlag = 
										GetSwapBytesFlagForTiffFile (headerRecordPtr);
			
				fileInfoPtr->numberHeaderBytes = GetTIFFNumberHeaderBytes (fileInfoPtr);
				
				}	// end "if (headerRecordPtr != NULL && format == kTIFFType)"
										
			fileInfoPtr->format = format;	
			fileInfoPtr->ancillaryInfoformat = kArcViewType;
			gdalReturnCode = ReadGDALProjectionInformation (fileInfoPtr, hDS);
		
			//		// For testing:
			//
			//fileInfoPtr->noDataValue = -9999;
			//fileInfoPtr->noDataValueFlag = TRUE;
			
			}	// end "if (gdalReturnCode == noErr)"
			
		if (gdalReturnCode == noErr)
			{
			if (saved_hDS != 0)
					// This means we are using subdata sets. Restore the main hDS pointer
					// to the file. It was overwritten with the one for the subdata set.
					// The one for the subdata set is also in the data sets structure.
				fileInfoPtr->gdalDataSetH = saved_hDS;
				
			fileInfoPtr->dataCompressionCode = ReadGDALCompressionInformation (
																								hDS, format);
			
					// Save hdf5 information into file information structure
					
			fileInfoPtr->hdfHandle = hdfDataSetsHandle;
			fileInfoPtr->hdfDataSetSelection = (UInt16)dataSetIndex;
				
					// Determine if a palette exists for the file. May be needed
					// later.
					
			numberClasses = CheckIfDefaultGDALPaletteExists (hDS);
			
					// Determine the image type if it has not been determined yet.
			
			if (gGetFileImageType == 0)
				{
				gGetFileImageType = kMultispectralImageType;
				fileInfoPtr->numberClasses = 0;
				
				if (fileInfoPtr->numberChannels == 1 && numberClasses > 0)
					gGetFileImageType = kThematicImageType;
							
						// If this is not a thematic type file but the number of
						// channels is one and there is a default arcview color file
						// available, then assume that the user would like for this
						// to be treated as a thematic image file.
						
				if (gGetFileImageType != kThematicImageType && 
							fileInfoPtr->numberChannels == 1 &&
									fileInfoPtr->numberBytes <= 2)
					{
					if (CheckIfSpecifiedFileExists (
										GetFileStreamPointer (fileInfoPtr), (char*)"\0.clr\0"))
						gGetFileImageType = kThematicImageType;
					
					}	// end "if (!fileInfoPtr->thematicType && ..."
				
				}	// end "if (gGetFileImageType == 0)"
			
			fileInfoPtr->thematicType = FALSE;
			if (gGetFileImageType == kThematicImageType)
				{
				fileInfoPtr->thematicType = TRUE;
				fileInfoPtr->numberClasses = numberClasses;

				fileInfoPtr->bandInterleave = kBIL;
				
				if (format == kTIFFType)
					fileInfoPtr->collapseClassSelection = kCollapseClass;
				
				if (numberClasses > 0)
							// This will force the gdal default color table to be read.
					fileInfoPtr->colorTableOffset = 1; 
				
				}	// end "if (gGetFileImageType == kThematicImageType)"	
				
			else	// gGetFileImageType == kMultispectralImageType
				{
						// If the number of channels is one, then indicate that the band
						// interleave format is BIL. (It doesn't make any difference for
						// one channel data except BIS formatted files are not allowed to
						// be linked. Change the band interleave format to BIL will allow
						// this file to be linked.
						// Changed this to kBSQ so that MultiSpec thinks would be handled 
						// correctly if files were linked.
						
				if (fileInfoPtr->numberChannels == 1 && !fileInfoPtr->blockedFlag)
					fileInfoPtr->bandInterleave = kBSQ;		// kBIL
					
				}	// end "else gGetFileImageType == kMultispectralImageType"
				
			CheckAndUnlockHandle (hdfDataSetsHandle);
									
			}	// end "if (gdalReturnCode == noErr)"
		
		else	// gdalReturnCode != noErr
			{
			fileInfoPtr->format = 0;	
			fileInfoPtr->ancillaryInfoformat = 0;
			fileInfoPtr->numberHdfDataSets = 0;
			UnlockAndDispose (hdfDataSetsHandle);
			
			gdalReturnCode = 1;
			
			}	// end "else gdalReturnCode != noErr"
			
		}	// end "if (fileInfoPtr->gdalDataSetH != NULL)"
		
	return (gdalReturnCode);
	
}	// end "LoadGDALInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadGDALColorTable
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
// Called By:			CreateThematicColorPalette in SPalette.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/20/2011
//	Revised By:			Larry L. Biehl			Date: 12/10/2016
 
Boolean ReadGDALColorTable (
				ColorSpec*							colorSpecPtr, 
				FileInfoPtr							imageFileInfoPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt16*								paletteCodePtr)
 
{ 
	GDALDatasetH						hDS;	
	
	GDALColorEntry*					gdalColorEntryPtr;
	//GDALColorTable*					gdalColorTablePtr;
	GDALColorTableH					gdalColorTableH;

	UInt32								colorTableCount,
											index;

	UInt16								vectorBluePtr[256],
											vectorGreenPtr[256],
											vectorRedPtr[256];


	if (displaySpecsPtr == NULL || 
				colorSpecPtr == NULL ||
						imageFileInfoPtr == NULL)
																						return (FALSE);

			// Check that there is a valid class to group handle if needed.		

	if (displaySpecsPtr->classGroupCode != kClassDisplay && 
		 imageFileInfoPtr->groupTablesHandle == NULL)
																						return (FALSE);	
	
	hDS = imageFileInfoPtr->gdalDataSetH;
	
	//gdalColorTablePtr = ((GDALDataset*)hDS)->GetRasterBand (1)->GetColorTable ();
	//if (gdalColorTablePtr == NULL)
	//																					return (FALSE);
	
	gdalColorTableH = GDALGetRasterColorTable	(GDALGetRasterBand (hDS, 1));
	if (gdalColorTableH == NULL)
																						return (FALSE);

			// Initialize the palette entries to all white.
		
	for (index=0; index<256; index++)			
		#ifdef multispec_lin
			vectorBluePtr[index] = vectorGreenPtr[index] = vectorRedPtr[index] = 0xff;
		#endif
			
		#ifndef multispec_lin
			vectorBluePtr[index] = vectorGreenPtr[index] = vectorRedPtr[index] = 0xff00;			
		#endif			

	
			// Get the number of colors in the table. Make sure this count is less than
			// maximum that can be handled of 256.
	
	//colorTableCount = gdalColorTablePtr->GetColorEntryCount ();
	colorTableCount = GDALGetColorEntryCount (gdalColorTableH);
	colorTableCount = MIN (colorTableCount, 256);
			
			// Now load the entire palette into the appropriate locations.
			
	for (index=0; index<colorTableCount; index++)
		{
		//gdalColorEntryPtr = (GDALColorEntry*)gdalColorTablePtr->GetColorEntry (index);
		gdalColorEntryPtr = (GDALColorEntry*)GDALGetColorEntry (gdalColorTableH, index);
					
		if (gdalColorEntryPtr != NULL)
			{	
			#ifdef multispec_lin
				vectorRedPtr[index] = gdalColorEntryPtr->c1;
				vectorGreenPtr[index] = gdalColorEntryPtr->c2;
				vectorBluePtr[index] =  gdalColorEntryPtr->c3;	
			#endif
			
			#ifndef multispec_lin
				vectorRedPtr[index] = gdalColorEntryPtr->c1 * 256;
				vectorGreenPtr[index] = gdalColorEntryPtr->c2 * 256;
				vectorBluePtr[index] =  gdalColorEntryPtr->c3 * 256;			
			#endif			
				
			}	// end "if (gdalColorEntryPtr != NULL)"
			
		}	// end "for (index=0; index<colorTableCount; index++)"
				
		// Now load what is needed for the palette.
	
	LoadTwoBytePalette (colorSpecPtr, 
							  imageFileInfoPtr, 
							  displaySpecsPtr, 
							  classSymbolPtr, 
							  paletteCodePtr,
							  colorTableCount,
							  vectorBluePtr,
							  vectorGreenPtr,
							  vectorRedPtr);

	return (colorTableCount>0);

}	// end "ReadGDALColorTable"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 ReadGDALCompressionInformation
//
//	Software purpose:	This routine reads the projection information for the file
//							if it exists.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/16/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

UInt16 ReadGDALCompressionInformation (
				GDALDatasetH						hDS,
				SInt16								format)

{	
	UCharPtr								metaDataStringPtr;
	
	UInt16								dataCompressionCode = kNoCompression;
	
	
	if (hDS != NULL)
		{		
		metaDataStringPtr = (UCharPtr)GDALGetMetadataItem (
															hDS, "COMPRESSION", "IMAGE_STRUCTURE");
		
		if (metaDataStringPtr == NULL)
					// See if the info is stored at the band level. Will only try the 
					// first band.
			metaDataStringPtr = (UCharPtr)GDALGetMetadataItem (GDALGetRasterBand (hDS, 1), 
																				"COMPRESSION", 
																				"IMAGE_STRUCTURE");
		
		if (metaDataStringPtr != NULL)
			{			
			if (CompareStringsNoCase (metaDataStringPtr, (UCharPtr)"CCITTFAX3", 9) == 0)
				dataCompressionCode = kCCITTGroup3Compression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"CCITTFAX4", 9) == 0)
				dataCompressionCode = kCCITTGroup4Compression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"CCITTRLE", 8) == 0)
				dataCompressionCode = kRLECompression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"JP2000", 6) == 0)
				dataCompressionCode = kJPEG2000Compression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"JPEG", 4) == 0)
				dataCompressionCode = kJPEGCompression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"LZW", 3) == 0)
				dataCompressionCode = kLZWCompression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"PACKBITS", 8) == 0)
				dataCompressionCode = kPackBitsCompression;
				
			else if (CompareStringsNoCase (metaDataStringPtr, (UCharPtr)"RLE", 3) == 0)
				dataCompressionCode = kRLECompression;
				
			else if (CompareStringsNoCase (
												metaDataStringPtr, (UCharPtr)"DEFLATE", 7) == 0)
				dataCompressionCode = kDeflateCompression;
				
			else
				dataCompressionCode = kUnknownCompression;
						
			}	// end "if (metaDataStringPtr != NULL)"
			
		if (dataCompressionCode == kNoCompression)
			{
					// Check if format of image signifies if it is compressed.
			
			if (format == kArcInfoBinaryGridType)
				dataCompressionCode = kRLECompression;
			
			else if (format == kECWType)
				dataCompressionCode = kJPEG2000Compression;
					
			else if (format == kJPEG2000Type)
				dataCompressionCode = kJPEG2000Compression;
			
			else if (format == kJPEGType)
				dataCompressionCode = kSkipHuffmanCompression;
			
			else if (format == kPNGType)						
				dataCompressionCode = kPNGCompression;
				
			}	// end "if (dataCompressionCode == kNoCompression)"
			
		#if include_hdf5_capability
			if (dataCompressionCode == kNoCompression && format == kHDF5Type)
				dataCompressionCode = GetHDF5CompressionInformation (hDS);
		#endif		// include_hdf5_capability
			
		}	// end "if (hDS != NULL)"
		
	return (dataCompressionCode);
	
}	// end "ReadGDALCompressionInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadGDALHeaderInformation
//
//	Software purpose:	This routine reads the file specifications 
//							for the selected image using the gdal routines.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:			ReadHeaderWithGDALLibrary
//
//	Coded By:			Larry L. Biehl			Date: 07/17/2009
//	Revised By:			Larry L. Biehl			Date: 03/14/2014

SInt16 ReadGDALHeaderInformation (
				GDALDatasetH						hDS,
				SInt16								format,
				UInt32*								numberChannelsPtr,
				UInt32*								numberLinesPtr,
				UInt32*								numberColumnsPtr,
				UInt16*								numberBytesPtr,
				UInt16*								numberBitsPtr,
				UInt16*								dataTypeCodePtr,
				UInt16*								dataCompressionCodePtr,
				SInt16*								bandInterleaveFormatPtr,
				Boolean*								signedDataFlagPtr,
				UInt32*								xBlockSizePtr,
				UInt32*								yBlockSizePtr,
				SInt16*								gdalDataTypeCodePtr,
				Boolean*								noDataValueFlagPtr,
				double*								noDataValuePtr)

{	
	double								noDataValue;
	
	UCharPtr								metaDataStringPtr;
	
	int									pbSuccess,
											testXBlockSize,
											testYBlockSize,
											xBlockSize,
											yBlockSize;
	
	UInt32								channelNumber;
	
	SInt16								dataType,
											returnCode = 0;
	
	
	if (hDS != NULL)
		{		
		*numberColumnsPtr = GDALGetRasterXSize (hDS);
		*numberLinesPtr = GDALGetRasterYSize (hDS);
		*numberChannelsPtr = GDALGetRasterCount (hDS);
		
		dataType = GDALGetRasterDataType (GDALGetRasterBand (hDS, 1));
		
		//swapBytesFlag = GDALGetNativeOrder (hDS);
		//swapBytesFlag = ((GDALDataset *)hDS)->GetNativeOrder ();
		
				// Verify that the data type is the same for all channels.
		
		for (channelNumber=2; channelNumber<=*numberChannelsPtr; channelNumber++)
			{
			if (GDALGetRasterDataType (
											GDALGetRasterBand (hDS, channelNumber)) != dataType)
				returnCode = 2;
			
			}	// end "for (channelNumber=2; channelNumber<=..."
		
		switch (dataType)
			{
			case GDT_Unknown:		// Not known
			case GDT_CInt16:		// Complex Int16
			case GDT_CInt32:		// Complex Int32
			case GDT_CFloat32:	// Complex Float32
			case GDT_CFloat64:	// Complex Float64
			default:
				returnCode = 1;
				break;
			
			case GDT_Byte:		// Eight bit unsigned integer
				*dataTypeCodePtr = kIntegerType;
				*numberBitsPtr = 8;
				
						// Determine if signed byte.  Will assume that all bands are like the
						// first. Will handle above if the assumption is found to be false.
						
				*signedDataFlagPtr = FALSE;
				metaDataStringPtr = (UCharPtr)GDALGetMetadataItem (
																			GDALGetRasterBand (hDS, 1), 
																			"PIXELTYPE", 
																			"IMAGE_STRUCTURE");
				if (metaDataStringPtr != NULL)
					{
					if (CompareStringsNoCase (metaDataStringPtr, (UCharPtr)"SIGNEDBYTE", 10) == 0)
						*signedDataFlagPtr = TRUE;
						
					}	// end "if (metaDataStringPtr != NULL)"
				break;
			
			case GDT_UInt16:		// Sixteen bit unsigned integer
				*dataTypeCodePtr = kIntegerType;
				*numberBitsPtr = 16;
				*signedDataFlagPtr = FALSE;
				break;
			
			case GDT_Int16:		// Sixteen bit signed integer
				*dataTypeCodePtr = kIntegerType;
				*numberBitsPtr = 16;
				*signedDataFlagPtr = TRUE;
				break;
			
			case GDT_UInt32:		// Thirty two bit unsigned integer
				*dataTypeCodePtr = kIntegerType;
				*numberBitsPtr = 32;
				*signedDataFlagPtr = FALSE;
				break;
			
			case GDT_Int32:		// Thirty two bit signed integer
				*dataTypeCodePtr = kIntegerType;
				*numberBitsPtr = 32;
				*signedDataFlagPtr = TRUE;
				break;
			
			case GDT_Float32:		// Thirty two bit floating point
				*dataTypeCodePtr = kRealType;
				*numberBitsPtr = 32;
				*signedDataFlagPtr = TRUE;
				break;
			
			case GDT_Float64:		//  Sixty four bit floating point
				*dataTypeCodePtr = kRealType;
				*numberBitsPtr = 64;
				*signedDataFlagPtr = TRUE;
				break;
			
			}	// end "switch (dataType)"
		
		if (returnCode == noErr)
			{
			*numberBytesPtr = *numberBitsPtr/8;
						
			*gdalDataTypeCodePtr = dataType;
						
			*bandInterleaveFormatPtr = kBSQ;
			metaDataStringPtr = (UCharPtr)GDALGetMetadataItem (
															hDS, "INTERLEAVE", "IMAGE_STRUCTURE");
			if (metaDataStringPtr != NULL)
				{
				if (CompareStringsNoCase (metaDataStringPtr, (UCharPtr)"PIXEL", 5) == 0)
					*bandInterleaveFormatPtr = kBIS;
				else if (CompareStringsNoCase (
													metaDataStringPtr, (UCharPtr)"LINE", 4) == 0)
					*bandInterleaveFormatPtr = kBIL;
					
				}	// end "if (metaDataStringPtr != NULL)"
			
					// Get the block size.
			
			GDALGetBlockSize (GDALGetRasterBand (hDS, 1), &xBlockSize, &yBlockSize);
			
				// Verify that the block size is the same for all channels.
			
			for (channelNumber=2; channelNumber<=*numberChannelsPtr; channelNumber++)
				{
				GDALGetBlockSize (GDALGetRasterBand (hDS, channelNumber), 
										&testXBlockSize, 
										&testYBlockSize);
				if (testXBlockSize != xBlockSize || testYBlockSize != yBlockSize)
					returnCode = 3;
				
				}	// end "for (channelNumber=2; channelNumber<=..."
					
			*xBlockSizePtr = xBlockSize;
			*yBlockSizePtr = yBlockSize;
				
			*dataCompressionCodePtr = ReadGDALCompressionInformation (hDS, format);
			
			pbSuccess = 0;
			noDataValue = 
						GDALGetRasterNoDataValue (GDALGetRasterBand (hDS, 1), &pbSuccess);
			*noDataValueFlagPtr = FALSE;
			*noDataValuePtr = 0;
			if (pbSuccess != 0)
				{
				*noDataValueFlagPtr = TRUE;
				*noDataValuePtr = noDataValue;
				
				}	// end "if (pbSuccess != 0)"
						
			}	// end "if (returnCode == noErr)" 
		
		else	// returnCode != noErr
			returnCode = 1;

		}	// end "if (hDS != NULL)"
	
	else	// hDS == NULL
		returnCode = 1;
	
	return (returnCode);
	
}	// end "ReadGDALHeaderInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadGDALProjectionInformation
//
//	Software purpose:	This routine reads the projection information for the file
//							if it exists.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2011
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 ReadGDALProjectionInformation (
				FileInfoPtr 						fileInfoPtr,
				GDALDatasetH						hDS)

{	
	double								adfGeoTransform[6],
											returnValue;
	
	const char							*projectionStringPtr;
	UCharPtr								attributeStringPtr;
	const char							*authorityCodeStringPtr;
	const char							*authorityNameStringPtr;
	
	int									datum,
											northFlag,
											pZone;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;
	
	OGRSpatialReferenceH				ogrSpatialReferenceCPtr;
	
	OGRErr								ogrError;
	
	SInt32								pcsCode = 0;
	
	SInt16								gtModelTypeGeoKey = 0,
											mapUnitsCode,
											projectionCode = kNotDefinedCode,
											referenceSystemCode = kNoRSDefinedCode,
											returnCode = 0,
											utmZone;
	
	Boolean								parametersSetFlag = FALSE,
											projectionStringFlag = FALSE,
											transformStringFlag = FALSE;
	
			// This is currently here for testing
	GDALRPCInfo							rpcInfo;
	char									**rpcStringPtr;
	
	
	if (hDS != NULL)
		{					
		projectionStringPtr = GDALGetProjectionRef (hDS);
		
		rpcStringPtr = GDALGetMetadata (hDS, "RPC");
		if (rpcStringPtr != NULL)
			GDALExtractRPCInfo (rpcStringPtr, &rpcInfo);
			//ReadXMLRPCs (fileInfoPtr);
		
		if (projectionStringPtr != NULL && projectionStringPtr[0] != 0)
			projectionStringFlag = TRUE;	
		
		if (GDALGetGeoTransform (hDS, adfGeoTransform) == CE_None)
			transformStringFlag = TRUE;
			
		if (fileInfoPtr->format == kTIFFType)		
					// Note that GeoTIFF is not set until end of this routine.
			gtModelTypeGeoKey = GetGTModelTypeGeoKey (fileInfoPtr);
		
		if (projectionStringFlag || transformStringFlag || gtModelTypeGeoKey == 2)
			{
			if (fileInfoPtr->mapProjectionHandle == NULL)
				fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
			
			if (fileInfoPtr->mapProjectionHandle != NULL)
				{ 								
				mapProjectionInfoPtr = (MapProjectionInfoPtr)
				GetHandlePointer (fileInfoPtr->mapProjectionHandle);
					
				}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
			
			if (mapProjectionInfoPtr == NULL)
				returnCode = 1;
			
			if (returnCode == noErr)
				{					
				if (projectionStringFlag)	
					{
					ogrSpatialReferenceCPtr = OSRNewSpatialReference (projectionStringPtr);
					if (ogrSpatialReferenceCPtr != NULL)
						{	
								// Check for common reference systems
						
						attributeStringPtr = (UCharPtr)((OGRSpatialReference*)
										ogrSpatialReferenceCPtr)->GetAttrValue ("PROJCS");
										
								// Need to find another way to verify this. What is currently 
								// used is too stringent.
						//VerifyEPSG_CSVFolderExits ((char*)attributeStringPtr);

						authorityCodeStringPtr = ((OGRSpatialReference*)
										ogrSpatialReferenceCPtr)->GetAuthorityCode ("PROJCS");
						if (authorityCodeStringPtr != NULL)
							StringToNumber ((char*)authorityCodeStringPtr, &pcsCode);

						authorityNameStringPtr = ((OGRSpatialReference*)
										ogrSpatialReferenceCPtr)->GetAuthorityName ("PROJCS");
						
								// Check if utm zone number exists.
								
						utmZone = OSRGetUTMZone (ogrSpatialReferenceCPtr, &northFlag);
																	
								// Get UTM type reference codes
								
						if (attributeStringPtr != NULL && utmZone > 0)
							{											
							if (CompareStringsNoCase (attributeStringPtr, 
																(UCharPtr)"GDA94", 
																5) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																								kGDA94RSCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, 
																		(UCharPtr)"NAD27", 
																		5) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																							kUTM_NAD27RSCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, 
																		(UCharPtr)"NAD83", 
																		5) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																							kUTM_NAD83RSCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, 
																		(UCharPtr)"SAD69", 
																		5) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																							kUTM_SAD69RSCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, 
																		(UCharPtr)"WGS 72", 
																		6) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																							kUTM_WGS72RSCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, 
																		(UCharPtr)"WGS 84", 
																		6) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																							kUTM_WGS84RSCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, 
																		(UCharPtr)"Pulkovo", 
																		7) == 0)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																						kGaussKrugerRSCode;
							
							}	// end "if (attributeStringPtr != NULL && utmZone > 0)"	

								// Set up some other information for UTM reference systems
						
						if (utmZone > 0)
							{
							if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																							kNoRSDefinedCode)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																									kUTMRSCode;
							
							if (!northFlag)
								utmZone = -utmZone;
							mapProjectionInfoPtr->gridCoordinate.gridZone = utmZone;
							
							mapProjectionInfoPtr->gridCoordinate.projectionCode = 
																				kTransverseMercatorCode;
							
							}	// end "else if (OSRGetUTMZone (ogrSpatialReferenceCPtr, ..."
									
						else if (OSRIsGeographic (ogrSpatialReferenceCPtr))
							mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																						kGeographicRSCode;
							
						else if (pcsCode > 0 && pcsCode < 32767)
							{
									// check if one of State Plane sets.
							
							GTIFPCSToMapSys (pcsCode, &datum, &pZone);
							
							if (pZone > 0 && pZone < 32767 && datum > 0 && datum < 32767)
								{		
								mapProjectionInfoPtr->gridCoordinate.gridZone = pZone;
														
								if (datum == 4267)
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																					kStatePlaneNAD27RSCode;
									
								else if (datum == 4269)
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																					kStatePlaneNAD83RSCode;	
								
								}	// end "if (pZone > 0 ... && datum > 0 ...)"	
									
							if (pcsCode == 2100)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																								kGGRS87RSCode;
								
							if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																							kNoRSDefinedCode)
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																							kByEPSGCodeCode;
								
									// Get the epsg code name
								
							GetEPSGCodeName (ogrSpatialReferenceCPtr, 
													mapProjectionInfoPtr->gridCoordinate.epsgName);
							
							}	// end "else if (pcsCode > 0 && pcsCode < 32767)"							
								
								// Save this geo key in case needed to write out to a new 
								// file.
								
						mapProjectionInfoPtr->projectedCSTypeGeoKey = (UInt16)pcsCode;
						
								// Get the projection if needed. Projection is implied 
								// already if the reference system has been set. In other 
								// words the reference system will override the projection. 
								// Decided in March 2012 not force the projection based on 
								// the reference system. Want to allow one to see the 
								// information actually in the file.
								// The user can override this if needed.
						/*		
						attributeStringPtr = NULL;
						if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																						kNoRSDefinedCode ||
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																				kStatePlaneNAD27RSCode ||
								mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																					kStatePlaneNAD83RSCode)
						*/
						projectionCode = GetProjectionCodeFromOGRSpatialReference (
													(OGRSpatialReference*)ogrSpatialReferenceCPtr, 
													&referenceSystemCode);
																											
						if (projectionCode != kNotDefinedCode)
							mapProjectionInfoPtr->gridCoordinate.projectionCode = 
																							projectionCode;
							
						else if (referenceSystemCode == kGeographicRSCode)
							mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																						kGeographicRSCode;
							
								// Get the Datum
								
						mapProjectionInfoPtr->geodetic.datumCode = 
								GetDatumCodeFromOGRSpatialReference (
											(OGRSpatialReference*)ogrSpatialReferenceCPtr, 
											mapProjectionInfoPtr->gridCoordinate.datumName,
											TRUE);
																					
								// Get the Spheroid
								
						mapProjectionInfoPtr->geodetic.spheroidCode = 
								GetEllipsoidCodeFromOGRSpatialReference (
											(OGRSpatialReference*)ogrSpatialReferenceCPtr, 
											mapProjectionInfoPtr->gridCoordinate.ellipsoidName,
											TRUE);
																					
								// Get the linear units	
								
						mapUnitsCode = GetMapUnitsCodeFromOGRSpatialReference (
													(OGRSpatialReference*)ogrSpatialReferenceCPtr);
		
						if (mapUnitsCode != 0)
							mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = 
																								mapUnitsCode;
							
						returnValue	= OSRGetSemiMajor (ogrSpatialReferenceCPtr, &ogrError);
						if (ogrError == OGRERR_NONE)
							{
							mapProjectionInfoPtr->geodetic.semiMajorAxis = returnValue;
							
							}	// end "if (ogrError == OGRERR_NONE)"
						
						returnValue	= OSRGetSemiMinor (ogrSpatialReferenceCPtr, &ogrError);
						if (ogrError == OGRERR_NONE)
							{
							mapProjectionInfoPtr->geodetic.semiMinorAxis = returnValue;
							
							}	// end "if (ogrError == OGRERR_NONE)"
							
								// Try to get parameters from the string if the information 
								// was not found by the gdal routines.
						/*		
						if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																						kNoRSDefinedCode &&
								mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																							kNotDefinedCode)
						*/
						SetProjectionInformationFromString (mapProjectionInfoPtr,
																		(char*)projectionStringPtr);
																				
								// Try a last ditch effort to get information from the geotiff string that GDAL
								// may not be getting.
								
						if (fileInfoPtr->format == kTIFFType &&
								(mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																						kNotDefinedCode ||
									mapProjectionInfoPtr->geodetic.datumCode == 
																					kNoDatumDefinedCode))
							SetProjectionInformationFromString2 (fileInfoPtr,
																				mapProjectionInfoPtr);
							
								// Check if this may be a geographic projection. I have not found that
								// "geographic" is listed as output of a in gdal.
						/*		
						if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																					kUserDefinedRSCode &&
								mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																					kNotDefinedCode &&
									mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == 
																					kAVDecimalDegreesCode)
							mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																						kGeographicRSCode;
						*/
								// If spheroid and datum have not been defined, try to get 
								// spheroid from any available major and minor axes.
								
						if (mapProjectionInfoPtr->geodetic.spheroidCode == 0 && 
												mapProjectionInfoPtr->geodetic.datumCode == 
																					kNoDatumDefinedCode)
							mapProjectionInfoPtr->geodetic.spheroidCode = 
										GetSpheroidCodeFromMajorMinorAxes (
													mapProjectionInfoPtr->geodetic.semiMajorAxis,
													mapProjectionInfoPtr->geodetic.semiMinorAxis);

								// Check if spherical datum
							
						if (mapProjectionInfoPtr->geodetic.datumCode == 
																			kNoDatumDefinedCode &&
								mapProjectionInfoPtr->geodetic.spheroidCode == 
																					kSphereEllipsoidCode)
							mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
							
								// Set the ellipsoid based on the datum if it has not been 
								// set yet.
						
						if (mapProjectionInfoPtr->geodetic.spheroidCode	== 
																				kNoEllipsoidDefinedCode)	
							SetEllipsoidFromDatum (mapProjectionInfoPtr);
							
								// Set projection paramaters from the reference system
								
						//parametersSetFlag = SetProjectionParametersFromReferenceSystem (
						//															mapProjectionInfoPtr);
													
								// Set the projection parameters from GDAL information if 
								// possible.
						
						if (!parametersSetFlag)		
							SetProjectionParametersFromGDALInformation (
																				mapProjectionInfoPtr, 
																				ogrSpatialReferenceCPtr);	
																											
						OSRRelease (ogrSpatialReferenceCPtr);	
						
						}	// end "if (ogrSpatialReferenceCPtr != NULL)"
					
					}	// end "if (projectionStringFlag)"
				
				if (transformStringFlag)
					{
							// Get the map transform information
							// Note that GDALGetGeoTransform return the location for the 
							// upper left corner of the upper left pixel. MultiSpec needs 
							// to have the value as the center of the upper left pixel
							
							// The following is to catch a situation for PLEIADES data in 
							// jpeg2000 format for geographic degrees, where the transform 
							// returned from gdal is reversed for lat-long. The end result 
							// is that one comes up with a rotation of -90 degrees. Don't 
							// know if this will cause problems for other situations or not.
							
					if (adfGeoTransform[1] == 0 && adfGeoTransform[5] == 0 &&
															fileInfoPtr->format == kJPEG2000Type)
						{
						adfGeoTransform[1] = adfGeoTransform[2];
						adfGeoTransform[2] = 0;
						
						adfGeoTransform[5] = adfGeoTransform[4];
						adfGeoTransform[4] = 0;
						
						}	// end "if (adfGeoTransform[1] == 0 && ..."
															
					LoadPlanarCoordinates (mapProjectionInfoPtr,
													adfGeoTransform[1],
													adfGeoTransform[2],
													adfGeoTransform[4],
													-fabs (adfGeoTransform[5]),
													adfGeoTransform[0],
													adfGeoTransform[3]);
								
					if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0)
						{ 						
						AdjustUpperLeftPixelLocationToCenter (mapProjectionInfoPtr);
						/*
						mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 0.5 * 
							fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);
						mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 0.5 * 
							fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);
								
						mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
						mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
						*/
						
						}	// end "if (...->planarCoordinate.mapOrientationAngle == 0)"
					/*							
					mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 0;
					if (adfGeoTransform[2] != 0)
						mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 
												-atan2 (adfGeoTransform[2], adfGeoTransform[1]);
					
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
																				adfGeoTransform[1];
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
																				fabs (adfGeoTransform[5]);
									
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = 
																					adfGeoTransform[0];
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = 
																					adfGeoTransform[3];
								
					if (mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle == 0)
						{ 						
						mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 0.5 * 
							fabs (mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize);
						mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 0.5 * 
							fabs (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize);
						
						}	// end "if (...->planarCoordinate.mapOrientationAngle == 0)"
						
					else	// ...->planarCoordinate.mapOrientationAngle != 0
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
					*/					
							// This is to catch situations which have been found for Landsat 
							// images in UTM from south of the equater not having a false 
							// northing of 10000000 to indicated that the UTM zone is south. 
							// All one has to go on is that the yMapCoordinate11 is less 
							// than 0. Currently this is set for only the UTM_WGS84.
							// It can be expanded to others if they are found.
							
					if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																						kUTM_WGS84RSCode &&
								mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 < 0 &&
										mapProjectionInfoPtr->gridCoordinate.gridZone > 0)
						{
						mapProjectionInfoPtr->gridCoordinate.gridZone = 
													-mapProjectionInfoPtr->gridCoordinate.gridZone;
						mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = 10000000 + 
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
						
						}	// end "if (...->gridCoordinate.referenceSystemCode == ..."
						
					}	// end "if (tranformStringFlag)"	
		
				if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
														kNotDefinedCode && gtModelTypeGeoKey == 2)
					{
							// gtModelTypeGeoKey equal 2 means ModelTypeGeographic
							
					mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																					kGeographicRSCode;
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = 
																					kDecimalDegreesCode;
					
					}	// end "if (GetGTModelTypeGeoKey (fileInfoPtr) == 2)"
					
				}	// end "if (returnCode == noErr)"
				
			if (fileInfoPtr->format == kTIFFType)				
				fileInfoPtr->format = kGeoTIFFType;
			
			}	// end "if (projectionStringFlag || tranformStringFlag || ..."
			
				// If the map projection information was not set, see if one can get it 
				// from the gdal metadata.
				
		if (mapProjectionInfoPtr == NULL)
			ReadGDALProjectionInformationFromMetadata (fileInfoPtr, hDS);
			
				// Get any tie points if they exist and set a polynomial transformation
				// using them if no projection information exists.
				
		GetGDALTiePoints (fileInfoPtr, hDS);
		
		#if include_hdf5_capability
			if (fileInfoPtr->format == kHDF5Type || fileInfoPtr->format == kHDF4Type2)
				GetHDF5ProjectionInformation (fileInfoPtr);
		#endif	// include_hdf5_capability
			
		}	// end "if (hDS != NULL)"
	
	else	// hDS == NULL
		returnCode = 1;
	
	return (returnCode);
	
}	// end "ReadGDALProjectionInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadGDALProjectionInformationFromMetadata
//
//	Software purpose:	This routine tries to read the projection information for the 
//							file if it does not exist already from the metadata. Currently
//							the only case where this occurs is with the HRLDAS model output
//							and hdf5 format.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/20/2012
//	Revised By:			Larry L. Biehl			Date: 05/06/2015

void ReadGDALProjectionInformationFromMetadata (
				FileInfoPtr 						fileInfoPtr,
				GDALDatasetH						hDS) 
				
{
	double								horizontalPixelSize,
											noDataValue = 0,
											projectionParameters[13],
											verticalPixelSize,
											xMapCoordinate11,
											yMapCoordinate11;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	SInt32								mapReturnCode = 1;
	
	SInt16								mapProjectionCode,
											mapProjectionEllipsoidCode,
											mapProjectionDatumCode,
											mapUnitsCode;
											
	Boolean								adjustToUpperLeftFlag,
											convertCenterFromLatLongToMapFlag = FALSE,
											convertLowerLeftFromLatLongToMapFlag = FALSE,
											noDataValueExistsFlag = FALSE;
											
	SignedByte							handleStatus;

	
	if (fileInfoPtr->format == kHDF5Type || 
				fileInfoPtr->format == kNETCDF2Type ||
						fileInfoPtr->format == kHDF4Type2)
		mapReturnCode = ReadGDALProjectionInformationFromMetadata_HRLDAS (
														hDS,
														&noDataValue,
														&noDataValueExistsFlag,
														&xMapCoordinate11,
														&yMapCoordinate11,
														&mapProjectionCode,
														&mapProjectionEllipsoidCode,
														&mapProjectionDatumCode,
														&mapUnitsCode,
														&horizontalPixelSize,
														&verticalPixelSize,
														projectionParameters,
														&convertCenterFromLatLongToMapFlag,				
														&convertLowerLeftFromLatLongToMapFlag);
														
	if (mapReturnCode == noErr)
		{				
		fileInfoPtr->noDataValue = noDataValue;
		fileInfoPtr->noDataValueFlag = noDataValueExistsFlag;
		
		if (fileInfoPtr->mapProjectionHandle == NULL)
			fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
		
		if (fileInfoPtr->mapProjectionHandle != NULL)
			{ 		
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
							GetHandleStatusAndPointer (fileInfoPtr->mapProjectionHandle,
																&handleStatus);
											
					// Verify that the map projection structure has been initialized.
					
			InitializeMapProjectionStructure (mapProjectionInfoPtr);
			
			if (mapProjectionCode > kNotDefinedCode)
				mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																					kUserDefinedRSCode;
			
			mapProjectionInfoPtr->gridCoordinate.projectionCode = mapProjectionCode;
			  
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = xMapCoordinate11;
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = yMapCoordinate11;
			  
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
																						xMapCoordinate11;
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
																						yMapCoordinate11;
																									 
			mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
																					horizontalPixelSize;         
			mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
																					verticalPixelSize;
																											
			mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 0;
																					
			mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;																		
			
			mapProjectionInfoPtr->geodetic.spheroidCode = mapProjectionEllipsoidCode;
			mapProjectionInfoPtr->geodetic.datumCode = mapProjectionDatumCode;
			
			if (mapProjectionEllipsoidCode == kUserDefinedEllipsoidCode ||
											mapProjectionEllipsoidCode == kSphereEllipsoidCode)
				{
						// Set the radius for the axes if provided. If not provided, they 
						// will be set later based on the ellipsoid code.
						
				mapProjectionInfoPtr->geodetic.radiusSpheroid = projectionParameters[0];
				mapProjectionInfoPtr->geodetic.semiMajorAxis = projectionParameters[0];
				mapProjectionInfoPtr->geodetic.semiMinorAxis = projectionParameters[1];
				
				}	// endd "if (mapProjectionEllipsoidCode == ..."
				
					// Set projection paramaters based on the map projection
					
			if (mapProjectionCode == kLambertConformalConicCode)
				SetLambertConformalConicParameters (
												mapProjectionInfoPtr,
												projectionParameters[8],	// standardParallel1
												projectionParameters[9],	// standardParallel2
												projectionParameters[4],	// centralMeridian
												projectionParameters[5],	// latitudeOrigin
												projectionParameters[6],	// falseEasting
												projectionParameters[7]);	// falseNorthing
														
			else if (mapProjectionCode == kPolarStereographicCode)
				SetPolarStereographicParameters (
												mapProjectionInfoPtr,
												projectionParameters[0],	// radiusSpheroid
												projectionParameters[0],	// radiusSpheroid
												projectionParameters[4],	// centralMeridian
												projectionParameters[5],	// latitudeOrigin
												projectionParameters[6],	// falseEasting
												projectionParameters[7]);	// falseNorthing
			
					// Make sure the map units code is set correctly
			
			if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == kUnknownCode &&
					mapProjectionInfoPtr->gridCoordinate.projectionCode != kNotDefinedCode)
				{										
				if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																					kStatePlaneNAD27RSCode)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kFeetCode;
					
				else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																						kGeographicRSCode)
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = 
																						kDecimalDegreesCode;
							
				else	// ...->gridCoordinate.code != kStatePlaneCode && ...
					mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = kMetersCode;
					
				}	// end "if (...->planarCoordinate.mapUnitsCode == kUnknownCode && ..."
			
			MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);
				
			LoadSpheroidInformation (fileInfoPtr->mapProjectionHandle);
				
			mapProjectionInfoPtr = (MapProjectionInfoPtr)
											GetHandlePointer (fileInfoPtr->mapProjectionHandle);
						
			if (convertCenterFromLatLongToMapFlag)
				{
						// Convert the lat-long coordinates to map coordinates
																
				if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kLambertConformalConicCode)
					{
					ConvertLatLongToLambertConformalConic (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
									
							// This represents the map coordinates for the center of the 
							// pixel. Translate to the upper left pixel.
							
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 -=
							((double)fileInfoPtr->numberColumns/2 - 0.5) * 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
							
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 +=
							((double)fileInfoPtr->numberLines/2 - 0.5) * 
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
						
					}	// end "if (mapProjectionInfoPtr->gridCoordinate.code == ..."
					
				}	// end "if (convertCenterFromLatLongToMapFlag)"
			
			if (convertLowerLeftFromLatLongToMapFlag)
				{
						// Convert the lat-long coordinates to map coordinates
															
				adjustToUpperLeftFlag = FALSE;
				
				if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kLambertConformalConicCode)
					{
					ConvertLatLongToLambertConformalConic (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
									
					adjustToUpperLeftFlag = TRUE;
							
					}	// end "if (mapProjectionInfoPtr->gridCoordinate.code == ..."
					
				else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kMercatorCode)
					{
					ConvertLatLongToMercator (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
									
					adjustToUpperLeftFlag = TRUE;
									
					}	// end "else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kMercatorCode"
									
							// This represents the map coordinates for the lower left pixel. 
							// Translate to the upper left pixel.
				
				if (adjustToUpperLeftFlag)			
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 +=
							(fileInfoPtr->numberLines - 1) * 
									mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
					
				}	// end "if (convertLowerLeftFromLatLongToMapFlag)"
			  
					// Make sure the map orientation origin is up to date.
					
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =    
									mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
				
			}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
			
		}	// end "if (mapReturnCode == noErr)"																			

}	// end "ReadGDALProjectionInformationFromMetadata"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadGDALProjectionInformationFromMetadata_HRLDAS
//
//	Software purpose:	This routine tries to read the projection information for the 
//							file if it does not exist already from the metadata. Currently
//							the only case where this occurs is with the HRLDAS model output
//							and hdf5 format.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/20/2012
//	Revised By:			Larry L. Biehl			Date: 04/30/2013

SInt16 ReadGDALProjectionInformationFromMetadata_HRLDAS (
				GDALDatasetH						hDS,
				double*								noDataValuePtr,
				Boolean*								noDataValueExistsFlagPtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr,
				Boolean*								convertCenterFromLatLongToMapFlagPtr,
				Boolean*								convertLowerLeftFromLatLongToMapFlagPtr)

{	
	double								projectionCode;
	
	char									**dataSetMetadataStringPtrPtr = NULL,
											**metadataStringPtrPtr = NULL;
	
	SInt16								returnCode,
											returnCode2,
											returnMapCode = 1,
											titleCode = 0;
   
   	
			// Get metadata if there is any.
			
	if (hDS != NULL)
		metadataStringPtrPtr = GDALGetMetadata (hDS, NULL);

	if (metadataStringPtrPtr != NULL)
		{					
		titleCode = GetGDALSpecificTextMetadataInformation (metadataStringPtrPtr,
																				(char*)"TITLE=",
																				(char*)"FROM HRLDAS");
																
		if (titleCode == 0)
			{
			returnCode = GetGDALSpecificTextMetadataInformation (metadataStringPtrPtr,
																					(char*)"TITLE=",
																					(char*)"FROM REAL_EM");
																	
			if (returnCode == 1)
				titleCode = 2;

			}	// end "if (titleCode == 0)"
																		
		if (titleCode > 0)
			{		
					// Get the missing_value if it exists.
							
			returnCode = GetGDALSpecificNumericMetadataInformation (
																				metadataStringPtrPtr,
																				(char*)"missing_value=",
																				noDataValuePtr);
															
			if (returnCode == 1)
				*noDataValueExistsFlagPtr = TRUE;
					
					
					// Get the map projection name.

			returnCode = GetGDALSpecificNumericMetadataInformation (
																				metadataStringPtrPtr,
																				(char*)"MAP_PROJ=",
																				&projectionCode);
			
			if (returnCode > 0)
				{
						// Check for the projection codes.
						
				if (projectionCode == 1)
					{
					*mapProjectionCodePtr = kLambertConformalConicCode;
					*mapUnitsCodePtr = kMetersCode;
										
					}	// end "if (projectionCode == 1)"
						
				else if (projectionCode == 2)
					{
					*mapProjectionCodePtr = kPolarStereographicCode;
					*mapUnitsCodePtr = kMetersCode;
						
					}	// end "else if (projectionCode == 2)"
						
				else if (projectionCode == 3)
					{
					*mapProjectionCodePtr = kMercatorCode;
					*mapUnitsCodePtr = kMetersCode;
						
					}	// end "else if (projectionCode == 3)"
				
				}	// end "if (returnCode > 0)"
				
					// Now verify at the data set level that a map projection exists
					
			dataSetMetadataStringPtrPtr = 
											GDALGetMetadata (GDALGetRasterBand (hDS, 1), NULL);	
				
			if (dataSetMetadataStringPtrPtr != NULL)
				{
				returnCode = GetGDALSpecificNumericMetadataInformation (
																			dataSetMetadataStringPtrPtr,
																			(char*)"map_proj=",
																			&projectionCode);
																							
				if (returnCode > 0 && projectionCode == 0)
					*mapProjectionCodePtr = 0;
															
				}	// end "if (dataSetMetadataStringPtrPtr != NULL)"
			
					// Get the projection parameters.

			if (*mapProjectionCodePtr != 0)
				{
				returnMapCode = noErr;
				
				if (titleCode == 1)
					{
							// Get the latitude and longitude for the lower left pixel.
							// This will be needed to convert to the map units location
							// for the upper left pixel later
							
					returnCode = GetGDALSpecificNumericMetadataInformation (
																					metadataStringPtrPtr,
																					(char*)"LAT1=",
																					yMapCoordinate11Ptr);
																					
							
					returnCode2 = GetGDALSpecificNumericMetadataInformation (
																					metadataStringPtrPtr,
																					(char*)"LON1=",
																					xMapCoordinate11Ptr);
																			
					if (returnCode > 0 || returnCode2 > 2)
						*convertLowerLeftFromLatLongToMapFlagPtr = TRUE;
																	
					}	// end "if (titleCode == 1)"
					
				else	// titleCode == 2
					{
							// Get the latitude and longitude for the center of image.
							// This will be needed to convert to the map units location
							// for the upper left pixel later
							
					returnCode = GetGDALSpecificNumericMetadataInformation (
																					metadataStringPtrPtr,
																					(char*)"CEN_LAT=",
																					yMapCoordinate11Ptr);
																					
							
					returnCode2 = GetGDALSpecificNumericMetadataInformation (
																					metadataStringPtrPtr,
																					(char*)"CEN_LON=",
																					xMapCoordinate11Ptr);
																			
					if (returnCode > 0 || returnCode2 > 2)
						*convertCenterFromLatLongToMapFlagPtr = TRUE;
																	
					}	// end "else titleCode == 2"
				
						// Get the pixel width
						
				returnCode = GetGDALSpecificNumericMetadataInformation (
																				metadataStringPtrPtr,
																				(char*)"DX=",
																				horizontalPixelSizePtr);
																				
						// Get the pixel height
						
				returnCode = GetGDALSpecificNumericMetadataInformation (
																				metadataStringPtrPtr,
																				(char*)"DY=",
																				verticalPixelSizePtr);
																				
						// Get the standard latitude1
						
				returnCode = GetGDALSpecificNumericMetadataInformation (
																		metadataStringPtrPtr,
																		(char*)"TRUELAT1=",
																		&projectionParametersPtr[8]);
																				
						// Get the standard latitude2
						
				returnCode = GetGDALSpecificNumericMetadataInformation (
																		metadataStringPtrPtr,
																		(char*)"TRUELAT2=",
																		&projectionParametersPtr[9]);
																				
						// Get the central latitude. Same as the standard latitude
						
				projectionParametersPtr[5] = projectionParametersPtr[8];
																				
						// Get the central longitude
						
				returnCode = GetGDALSpecificNumericMetadataInformation (
																		metadataStringPtrPtr,
																		(char*)"STAND_LON=",
																		&projectionParametersPtr[4]);
																				
						// For now it appears that HRLDAS Preprocessor used 
						// Sphere for datum
						
				*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
				*mapProjectionDatumCodePtr = kSphereDatumCode;
				
						// The radius that is used for the sphere appears to be 6,370,000
						// See map_init in module_llxy.F and called by map_set which is 
						// called by routines in consolidate_grib.F
						
				projectionParametersPtr[0] = 6370000;
				projectionParametersPtr[1] = 6370000;
			
						// Set the false easting and false northing. Assume that it is 0 
						// for now.
						
				projectionParametersPtr[6] = 0;
				projectionParametersPtr[7] = 0;
				
				}	// end "if (*mapProjectionCodePtr != 0 && .."
				
			}	// end "if (returnCode == 0)"
																																		
		}	// end "if (metadataStringPtrPtr != NULL)"
		
	return (returnMapCode);

}	// end "ReadGDALProjectionInformationFromMetadata_HRLDAS"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadHeaderWithGDALLibrary
//
//	Software purpose:	This routine reads the input file using the GDAL library to 
//							determine if the file can be handled as an image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	ReturnCode
//								0: No Error
//								not 0: Error handling the file
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2010
//	Revised By:			Larry L. Biehl			Date: 07/26/2017

SInt16 ReadHeaderWithGDALLibrary (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{	
	UInt8									filePathString[512];

	UCharPtr								fileFormatDescriptionPtr;

	FileStringPtr						filePathPtr;
											
	GDALDatasetH						hDS;
	
	SInt16								fileType = 0,
											gdalReturnCode,
											returnCode;
																						

	if (formatOnlyCode != kLoadHeader)
		{
				// As many file types as possible are handled here to reduce the time
				// it takes to just determine if it is a format that can be handled.
				// GDAL also loads in all map information, etc.
				
		if (CheckForGDALHandledHeaders (fileInfoPtr, headerRecordPtr, formatOnlyCode))
																						return (noErr);
		
		}	// end "if (formatOnlyCode != kLoadHeader)"
	
	returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
														filePathString, 
														510,
														&filePathPtr,
														kGDAL_Library);
	if (returnCode != noErr)	
																					return (returnCode);	

	if (formatOnlyCode == kLoadHeader)
		returnCode = noErr;

	GDALAllRegister ();
	
	hDS = GDALOpen ((char*)filePathPtr, GA_ReadOnly);
	/*
	if (hDS == NULL)
		{
		rlimit						resourceLimits;
		getrlimit (RLIMIT_NOFILE, &resourceLimits);
	
		sprintf ((char*)gTextString, " rlim_cur = %d%s gNumberOfOpenFilesLimit = %d%s",
						(int)resourceLimits.rlim_cur,
						gEndOfLine, 
						//resourceLimits.rlim_max,
						//gEndOfLine,
						gNumberOfOpenFilesLimit,
						gEndOfLine);
		OutputString (NULL, 
						(char*)gTextString, 
						0, 
						gOutputForce1Code,
						true);
		
		}
	*/
	if (hDS != NULL)
		{		
		gNumberOfOpenFiles++;
		GDALDriverH dataSetDriver = GDALGetDatasetDriver (hDS);
		fileFormatDescriptionPtr = (UCharPtr)GDALGetDescription ((GDALMajorObjectH)dataSetDriver);

		if (CompareStringsNoCase (fileFormatDescriptionPtr, (UCharPtr)"JP2KAK\0") == 0)
			fileType = kJPEG2000Type;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"JP2ECW\0") == 0)
			fileType = kJPEG2000Type;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"JP2OpenJPEG\0") == 0)
			fileType = kJPEG2000Type;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"GTiff\0") == 0)
			fileType = kTIFFType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"JPEG\0") == 0)
			fileType = kJPEGType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"PNG\0") == 0)
			fileType = kPNGType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"AIG\0") == 0)
			fileType = kArcInfoBinaryGridType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"ECW\0") == 0)
			fileType = kECWType;
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"EHdr\0") == 0)
			fileType = kArcViewType;
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"ENVI\0") == 0)
			fileType = kENVIType;
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"GRIB\0") == 0)
			fileType = kGRIBType;
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"HFA\0") == 0)
			fileType = kImagineType;
		
		#if include_hdf5_capability
			else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"HDF5\0") == 0)
				fileType = kHDF5Type;
			
			else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"HDF5Image\0") == 0)
				fileType = kHDF5Type;
				
			else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"BAG\0") == 0)
				fileType = kHDF5Type;
		#endif		// include_hdf5_capability
		
		#if defined multispec_lin
			else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"hdf4\0") == 0)
				fileType = kHDF4Type2;
		#endif		// defined multispec_lin
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"netCDF\0") == 0)
			fileType = kNETCDF2Type;
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"AAIGrid\0") == 0)
			fileType = kArcGISASCIIGridType;
			
		else if (CompareStringsNoCase (
								fileFormatDescriptionPtr, (UCharPtr)"GRASSASCIIGrid\0") == 0)
			fileType = kGRASSASCIIGridType;
			
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"PDS\0") == 0)
			fileType = kPDSType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"VRT\0") == 0)
			fileType = kGDALVRTType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"NITF\0") == 0)
			fileType = kNITFType;
		
		else if (CompareStringsNoCase (
									fileFormatDescriptionPtr, (UCharPtr)"PCIDSK\0") == 0)
			fileType = kPCIDSKType;
		
		if (fileType != 0)
			{
			if (formatOnlyCode != kLoadHeader)
				{
				fileInfoPtr->format = fileType;
				
						// For now we will not check whether the file is thematic
						// or multispectral
								
				if (CheckIfDefaultGDALPaletteExists (hDS) > 0)
					fileInfoPtr->thematicType = TRUE;
				
				if (formatOnlyCode == kMultiFiles)
					fileInfoPtr->thematicType = FALSE;
				
				//else if (formatOnlyCode == kThematicFiles)
				//	fileInfoPtr->thematicType = TRUE;
				
				GDALClose (hDS);
				gNumberOfOpenFiles--;
																							return (noErr);
				
				}	// end "if (formatOnlyCode != kLoadHeader)"
			
			else	// formatOnlyCode == kLoadHeader
				{
				fileInfoPtr->gdalDataSetH = hDS;
				gdalReturnCode = LoadGDALInformation (fileInfoPtr, 
																	headerRecordPtr,
																	fileType);

				if (gdalReturnCode != noErr)
					{					
							// Display an alert indicating that MultiSpec could not read or 
							// had problems reading this file.
					
					unsigned char						formatName[256];
					Boolean								continueFlag;
							
					continueFlag = GetSpecifiedStringNumber (kFileTypeStrID, 
																			IDS_FileType01+fileType, 
																			formatName, 
																			TRUE);
						
					continueFlag = LoadSpecifiedStringNumberStringP (
														kAlertStrID, 
														(SInt16)(IDS_Alert95 + gdalReturnCode - 1), 
														(char*)gTextString,
														(char*)gTextString2,
														continueFlag,
														(char*)&formatName[1]);
											
					if (continueFlag)
						DisplayAlert (kErrorAlertID, 
											kStopAlert, 
											0, 
											0,
											0, 
											gTextString);
					/*
					DisplayAlert (kErrorAlertID, 
									  kStopAlert, 
									  kAlertStrID, 
									  (SInt16)(IDS_Alert95 + gdalReturnCode - 1),
									  0, 
									  NULL);
					*/				  
					}	// end "if (gdalReturnCode != noErr)"
				
				}	// end "if (formatOnlyCode == kLoadHeader)" 
							
			}	// end "if (fileType != 0)"
			
		}	// end "if (hDS != NULL)"
	
	else	// hDS == NULL
		returnCode = 1;
	
	if (returnCode != noErr)
		{
		if (hDS != 0)
			gNumberOfOpenFiles--;
			
		GDALClose (hDS);
		fileInfoPtr->gdalDataSetH = 0;
		fileInfoPtr->gdalDataTypeCode = 0;
		
		}	// end "if (returnCode != noErr)"
	
	return (returnCode);
	
}	// end "ReadHeaderWithGDALLibrary" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReadPRJFileInformation
//
//	Software purpose:	This routine sets the projection parameters based on the 
//							projection and the information in the input gdal reference 
//							structure (class).
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/02/2013
//	Revised By:			Larry L. Biehl			Date: 01/02/2013

Boolean ReadPRJFileInformation (
				char									*prjStringPtr,
				SInt16								*projcsCodePtr,
				SInt16								*geogcsCodePtr,
				SInt16								*unitsCodePtr)

{ 
	UCharPtr								attributeStringPtr;
	
	OGRSpatialReferenceH				ogrSpatialReferenceCPtr;
	
	Boolean								prjFileFoundFlag = FALSE;
	

	*projcsCodePtr = 0;
	*geogcsCodePtr = 0;
	*unitsCodePtr = 0;

	ogrSpatialReferenceCPtr = OSRNewSpatialReference (prjStringPtr);
	if (ogrSpatialReferenceCPtr != NULL)
		{	
				// Check for common reference systems
		

		attributeStringPtr = (UCharPtr)((OGRSpatialReference*)
											ogrSpatialReferenceCPtr)->GetAttrValue ("PROJCS");
		if (attributeStringPtr != NULL)
			*projcsCodePtr = 1;
	
		attributeStringPtr = (UCharPtr)((OGRSpatialReference*)
											ogrSpatialReferenceCPtr)->GetAttrValue ("GEOGCS");
		if (attributeStringPtr != NULL)
			*geogcsCodePtr = 1;
	
		attributeStringPtr = (UCharPtr)((OGRSpatialReference*)
											ogrSpatialReferenceCPtr)->GetAttrValue ("UNIT");
		if (attributeStringPtr != NULL)
			{
			if (CompareStringsNoCase (attributeStringPtr, (UCharPtr)"D", 1) == 0)
						// Decimal degrees
				*unitsCodePtr = kAVDecimalDegreesCode;
				
			}	// end "if (attributeStringPtr != NULL)"
																											
		OSRRelease (ogrSpatialReferenceCPtr);
		
		prjFileFoundFlag = TRUE;
		
		}	// end "if (ogrSpatialReferenceCPtr != NULL)"
		
	return (prjFileFoundFlag);
	
}	// end "ReadPRJFileInformation" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetProjectionParametersFromGDALInformation
//
//	Software purpose:	This routine sets the projection parameters based on the 
//							projection and the information in the input gdal reference 
//							structure (class).
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
//	Revised By:			Larry L. Biehl			Date: 04/15/2013

void SetProjectionParametersFromGDALInformation (
				MapProjectionInfoPtr 			mapProjectionInfoPtr, 
				OGRSpatialReferenceH				ogrSpatialReferenceCPtr)

{	
	double								centralMeridian;
	
	OGRErr								ogrErrCode;
	
	
			// Set the projection parameters if possible.
															
	if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kAlbersConicalEqualAreaCode)
		SetAlbersEqualAreaParameters (
				mapProjectionInfoPtr,
				OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_1", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_2", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kCylindricalEqualAreaCode)
		SetCylindricalEqualAreaParameters (
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_1", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kEquirectangularCode)
		SetEquirectangularParameters (
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL));
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																		kLambertAzimuthalEqualAreaCode)
		SetLambertAzimuthalEqualAreaParameters (
				mapProjectionInfoPtr,
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
			
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kLambertConformalConicCode)
		SetLambertConformalConicParameters (
				mapProjectionInfoPtr,
				OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_1", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_2", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
															
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kMercatorCode)
		SetMercatorParameters (
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid	
				mapProjectionInfoPtr->geodetic.semiMinorAxis,
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
															
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kOrthographicCode)
		{
				// Have come across situations where central meridian is stored in two different
				// places.
				
		centralMeridian = OSRGetProjParm (
							ogrSpatialReferenceCPtr, "longitude_of_origin", 0, &ogrErrCode);
		if (ogrErrCode != noErr)
			centralMeridian = OSRGetProjParm (
							ogrSpatialReferenceCPtr, "central_meridian", 0, &ogrErrCode);
		
		SetOrthographicParameters (		
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
				centralMeridian,
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL));
							
		}	// end "else if (...->gridCoordinate.projectionCode == kOrthographicCode)"
															
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																				kPolarStereographicCode)
		SetPolarStereographicParameters (
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	
				mapProjectionInfoPtr->geodetic.semiMinorAxis,	
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
															
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == kSinusoidalCode)
		SetSinusoidalParameters (
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_1", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
															
	else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																				kTransverseMercatorCode)
		SetTransverseMercatorParameters (
				mapProjectionInfoPtr,
				mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
				OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "scale_factor", 1, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
				OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
	
}	// end "SetProjectionParametersFromGDALInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void VerifyEPSG_CSVFolderExits
//
//	Software purpose:	This routine checks whether the input string contains "unnamed"
//							at the beginning of the string. This implies that one of the
//							csv files was not found in the epsg_csv folder or more probably
//							the folder was not found at all. A message is displayed one
//							time per MultiSpec session. This is not foolproof but it
//							may be enough.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/29/2012
//	Revised By:			Larry L. Biehl			Date: 03/29/2012

void VerifyEPSG_CSVFolderExits (
				UCharPtr								inputStringPtr)

{	
	if (!gEPSG_csv_FolderAlertDisplayedFlag &&
			CompareStringsNoCase (inputStringPtr, (UCharPtr)"unnamed", 7) == 0)
		{
		DisplayAlert (kErrorAlertID, 
							kNoteAlert, 
							kAlertStrID, 
							IDS_Alert144,
							0, 
							NULL);
		
		gEPSG_csv_FolderAlertDisplayedFlag = TRUE;

		}	// end "if (!gEPSG_csv_FolderAlertDisplayedFlag && ..."

}	// end "VerifyEPSG_CSVFolderExits"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReadXMLRPCs
//
//	Software purpose:	This reads the RPCs in the PeruSat-1 RPC XML file. Code is from:
// section: xmlReader
// synopsis: Parse an XML file with an xmlReader
// purpose: Demonstrate the use of xmlReaderForFile() to parse an XML file
//          and dump the informations about the nodes found in the process.
//          (Note that the XMLReader functions require libxml2 version later
//          than 2.6.)
// usage: reader1 <filename>
// test: reader1 test2.xml > reader1.tmp && diff reader1.tmp $(srcdir)/reader1.res
// author: Daniel Veillard
// copy: see Copyright for the status of this software.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/17/2018
//	Revised By:			Larry L. Biehl			Date: 08/17/2018

void ReadXMLRPCs (
				FileInfoPtr							fileInfoPtr)

{
	UInt8									filePathString[512];
	FileStringPtr						filePathPtr;
	xmlTextReaderPtr					reader;
	
	int									ret;
	
	
	//RPC_PER1_20170204145604_SEN_MS_001277.XML
	
	ret = GetHDFFilePathCPointer (fileInfoPtr,
														filePathString, 
														510,
														&filePathPtr,
														kGDAL_Library);

	reader = xmlReaderForFile ((char*)filePathPtr, NULL, 0);
	if (reader != NULL) 
		{
		ret = xmlTextReaderRead (reader);
		while (ret == 1) 
			{
			ProcessRPCXMLNode (reader);
			ret = xmlTextReaderRead(reader);
			}
 
		xmlFreeTextReader (reader);
		if (ret != 0) 
			{
			fprintf(stderr, "%s : failed to parse\n", filePathPtr);
			}
		} 
		
	else 
		fprintf(stderr, "Unable to open %s\n", filePathPtr);
 
}	// end "ReadXMLRPCs"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ProcessRPCXMLNode
//
//	Software purpose:	This routine processes a given input node.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/17/2018
//	Revised By:			Larry L. Biehl			Date: 08/17/2018

void ProcessRPCXMLNode (
				xmlTextReaderPtr					reader)

{
	const xmlChar		*name,
							*value;

	name = xmlTextReaderConstName (reader);
	if (name == NULL)
		name = BAD_CAST "--";

	value = xmlTextReaderConstValue (reader);

	printf ("%d %d %s %d %d",
				xmlTextReaderDepth (reader),
				xmlTextReaderNodeType (reader),
				name,
				xmlTextReaderIsEmptyElement (reader),
				xmlTextReaderHasValue (reader));
 
	if (value == NULL)
		printf("\n");
	else 
		{
		if (xmlStrlen(value) > 40)
			printf(" %.40s...\n", value);
		else
			printf(" %s\n", value);
		}
		
}	// end "ProcessRPCXMLNode"
*/

/*
// processNode:
// @reader: the xmlReader
//
// Dump information about the current node

static void processNode(xmlTextReaderPtr reader)
	{
    const xmlChar *name, *value;

    name = xmlTextReaderConstName(reader);
    if (name == NULL)
	name = BAD_CAST "--";

    value = xmlTextReaderConstValue(reader);

    printf("%d %d %s %d %d", 
	    xmlTextReaderDepth(reader),
	    xmlTextReaderNodeType(reader),
	    name,
	    xmlTextReaderIsEmptyElement(reader),
	    xmlTextReaderHasValue(reader));
    if (value == NULL)
	printf("\n");
    else {
        if (xmlStrlen(value) > 40)
            printf(" %.40s...\n", value);
        else
	    printf(" %s\n", value);
    }
}

// streamFile:
// @filename: the file name to parse
//
// Parse and print information about an XML file.

static void streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
}

int main(int argc, char **argv) 
	{
    if (argc != 2)
        return(1);

     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
 
    LIBXML_TEST_VERSION

    streamFile(argv[1]);

     * Cleanup function for the XML library.
 
    xmlCleanupParser();
	
     * this is to debug memory for regression tests
	
    xmlMemoryDump();
    return(0);
}
*/
#endif	// include_gdal_capability
