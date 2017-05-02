//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2011)
//							� Purdue Research Foundation
//									All rights reserved.
//
//					The following was put here for easy access to a template
//  				DebugStr ("\pSGDALInterface.cpp");
//
//	File:						SGDALInterface.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			06/13/2011
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems	
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
//------------------------------------------------------------------------------------

#include "SMulSpec.h"
	
#if defined multispec_mac
	#define	IDS_FileIO189	189
	#define	IDS_FileIO190	190	
#endif	// defined multispec_mac    

#if defined multispec_win
   	 
#endif	// defined multispec_win

#if include_gdal_capability

#include "gdal.h"
#include "gdal_frmts.h"
#include "gdal_priv.h"
#include "cpl_error.h"
#include "ogr_spatialref.h"

#include "SExtGlob.h"

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

typedef struct {
    int     nLastErrNo;
    CPLErr  eLastErrType;
    CPLErrorHandlerNode *psHandlerStack;
    int     nLastErrMsgMax;
    char    szLastErrMsg[500];
} CPLErrorContext;


//extern CPLErrorContext*		CPLGetErrorContext(void);

extern Boolean					GetNewGDALFileReference (
										FileInfoPtr		 					fileInfoPtr);

extern SInt16					LoadHierarchalFileStructure (
										FileInfoPtr 						fileInfoPtr,
										UInt32								numberBlocks,
										UInt32*								channelStartsPtr,
										UInt32*								blockByteCountsPtr,
										UInt32								numberLinesPerStrip,
										UInt32								bytesPerStrip,
										UInt32								lastBytesPerStrip,
										UInt32								bytesPerStripForAllChannels);

extern Boolean					ReadGDALColorTable (
										ColorSpec*							colorSpecPtr, 
										FileInfoPtr							imageFileInfoPtr, 
										DisplaySpecsPtr					displaySpecsPtr, 
										UInt16*								classSymbolPtr, 
										UInt16*								paletteCodePtr);

extern SInt16					ReadGDALHeaderInformation (
										FileInfoPtr 						fileInfoPtr,
										GDALDatasetH						hDS);

extern SInt16					CheckIfDefaultGDALPaletteExists (
										FileInfoPtr							fileInfoPtr);

extern Boolean					SetUTMParametersFromZone (
										SInt16								gridZone,
										double*								longitudeCentralMeridianPtr,
										double*								latitudeOriginPtr,
										double*								scaleFactorOfCentralMeridianPtr,
										double*								falseEastingPtr,
										double*								falseNorthingPtr);

extern void						SetTransverseMercatorParameters (
										MapProjectionInfoPtr				mapProjectionInfoPtr,
										double								radiusSpheroid,
										double								longitudeCentralMeridian,
										double								latitudeOrigin,
										double								scaleFactorOfCentralMeridian,
										double								falseEasting,
										double								falseNorthing);

extern void						SetAlbersEqualAreaParameters (
										MapProjectionInfoPtr				mapProjectionInfoPtr,
										double								standardParallel1,
										double								standardParallel2,
										double								centralMeridian,
										double								latitudeOrigin,
										double								falseEasting,
										double								falseNorthing);

//#ifndef EQUAL
//#if defined(WIN32) || defined(WIN32CE)
//#  define EQUALN(a,b,n)           (strnicmp(a,b,n)==0)
//#  define EQUAL(a,b)              (stricmp(a,b)==0)
//#else
//#  define EQUALN(a,b,n)           (strncasecmp(a,b,n)==0)
//#  define EQUAL(a,b)              (strcasecmp(a,b)==0)
//#endif
//#endif

SInt16						CheckIfDefaultGDALPaletteExists (
									GDALDatasetH						hDS);

extern void					CloseGDALInterface (void);

extern SInt16 				GetHDFFilePathCPointer (
									FileInfoPtr		 					fileInfoPtr,
									char*									filePathStringPtr,
									SInt16								fileStringLength,
									char**								filePathCPtrPtr);

extern void					HandleGDALErrorMessage (
									SInt16								errCode);

extern void					MultiSpecGDALRegister ();


SInt16						ReadGDALProjectionInformation (
								  FileInfoPtr							fileInfoPtr,
																		 GDALDatasetH							hDS);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
	
}		// end "CheckIfDefaultGDALPaletteExists"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
				GDALDatasetH						hDS)

{		
	GDALColorTable*					gdalColorTablePtr = NULL;
	
	SInt32								colorTableCount = 0;
	
	
	if (hDS != NULL)
		gdalColorTablePtr = ((GDALDataset*)hDS)->GetRasterBand(1)->GetColorTable();
	
	if (gdalColorTablePtr != NULL)
		 colorTableCount = gdalColorTablePtr->GetColorEntryCount();
	
	if (colorTableCount < 0 || colorTableCount > SInt16_MAX)
		colorTableCount = 0;
		
	return ((SInt16)colorTableCount);
	
}		// end "CheckIfDefaultGDALPaletteExists"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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

	GDALDestroyDriverManager();

}		// end "CloseGDALInterface"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2009
//	Revised By:			Larry L. Biehl			Date: 12/17/2009

Boolean GetNewGDALFileReference (
				FileInfoPtr		 					fileInfoPtr)
				
{	
	char									filePathString[512];
	
	char*		 							filePathPtr;
	
	SInt16								returnCode;
	
	GDALDatasetH						hDS;
												
				   
   returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
   												filePathString, 
   												510,
   												&filePathPtr);
	if (returnCode != noErr)	
																							return (FALSE);	
					
	hDS = GDALOpen (filePathPtr, GA_ReadOnly);
	fileInfoPtr->gdalDataSetH = hDS;
	
	if (hDS == NULL)
		return (FALSE);	
		
	else		// hDS != NULL
		return (TRUE);
    
}		// end "GetNewGDALFileReference"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
		
		sprintf ((char*)&gTextString, 
					"%s  GDAL routines data access error:%s  ", 
					gEndOfLine,
					gEndOfLine);
		
		OutputString (NULL,
							(char*)&gTextString, 
							0, 
							gOutputCode, 
							TRUE);
							
		OutputString (NULL, 
							gdalLastErrorMessagePtr,
							0, 
							gOutputCode, 
							TRUE);
		
		sprintf ((char*)&gTextString, 
					"%s", 
					gEndOfLine);
		
		OutputString (NULL,
							(char*)&gTextString, 
							0, 
							gOutputCode, 
							TRUE);
		
		if (CPLGetLastErrorType() == CE_Fatal)					
			DisplayAlert (kErrorAlertID, 
								kStopAlert, 
								kAlertStrID, 
								IDS_Alert137,
								0,
								NULL);
		
		}		// end "if (errCode != noErr)"
    
}		// end "HandleGDALErrorMessage"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MultiSpecGDALRegister
//
//	Software purpose:	This routine registers the formats that MultiSpec currently
//							handles.
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
//	Coded By:			Larry L. Biehl			Date: 12/22/2010
//	Revised By:			Larry L. Biehl			Date: 12/22/2010

void MultiSpecGDALRegister ()

{	
			// FRMT_aigrid
	GDALRegister_AIGrid();
	
			// FRMT_png
	GDALRegister_PNG();
	
			// FRMT_jp2kak	
	GDALRegister_JP2KAK();
	
			// FRMT_ecw	
	GDALRegister_ECW();
	GDALRegister_JP2ECW();
	
			// FRMT_hdf5	
	GDALRegister_HDF5();
	GDALRegister_HDF5Image();

}		// end "MultiSpecGDALRegister"
*/




//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 04/20/2011	
 
Boolean ReadGDALColorTable (
				ColorSpec*							colorSpecPtr, 
				FileInfoPtr							imageFileInfoPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt16*								paletteCodePtr)
 
{ 
	GDALDatasetH						hDS;	
	
	GDALColorEntry*					gdalColorEntryPtr;
	GDALColorTable*					gdalColorTablePtr;

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
	
	gdalColorTablePtr = ((GDALDataset*)hDS)->GetRasterBand(1)->GetColorTable();
	if (gdalColorTablePtr == NULL)
																						return (FALSE);

			// Initialize the palette entries to all white.
		
	for (index=0; index<256; index++)
		vectorBluePtr[index] = vectorGreenPtr[index] = vectorRedPtr[index] = 0xff00;
	
		// Get the number of colors in the table.
	
	colorTableCount = gdalColorTablePtr->GetColorEntryCount();
			
			// Now load the entire palette into the appropriate locations.
			
	for (index=0; index<colorTableCount; index++)
		{
		gdalColorEntryPtr = (GDALColorEntry*)gdalColorTablePtr->GetColorEntry(index);
					
		if (gdalColorEntryPtr != NULL)
			{	
			vectorRedPtr[index] = gdalColorEntryPtr->c1 * 256;
			vectorGreenPtr[index] = gdalColorEntryPtr->c2 * 256;
			vectorBluePtr[index] =  gdalColorEntryPtr->c3 * 256;					
				
			}		// end "if (gdalColorEntryPtr != NULL)"
			
		}		// end "for (index=0; index<colorTableCount; index++)"
				
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

}		// end "ReadGDALColorTable"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/17/2009
//	Revised By:			Larry L. Biehl			Date: 04/20/2011

SInt16 ReadGDALHeaderInformation (
				 FileInfoPtr 						fileInfoPtr,
				 GDALDatasetH						hDS)

{	
	int									testXBlockSize,
											testYBlockSize,
											xBlockSize,
											yBlockSize;
	
	UInt32								bytesPerStrip,
											bytesPerStripForAllChannels,
											channelNumber,
											lastBytesPerStrip,
											stripsPerChannel;
	
	SInt16								dataType,
											returnCode = 0;
	
	
	if (hDS != NULL)
		{		
			fileInfoPtr->numberColumns = GDALGetRasterXSize (hDS);
			fileInfoPtr->numberLines = GDALGetRasterYSize (hDS);
			fileInfoPtr->numberChannels = GDALGetRasterCount (hDS);
			
			dataType = GDALGetRasterDataType (GDALGetRasterBand (hDS, 1));
			
				// Verify that the data type is the same for all channels.
			
			for (channelNumber=2; channelNumber<=fileInfoPtr->numberChannels; channelNumber++)
				{
				if (GDALGetRasterDataType (GDALGetRasterBand (hDS, channelNumber)) != dataType)
					returnCode = 2;
				
				}		// end "for (channelNumber=2; channelNumber<=..."
			
			switch (dataType)
			{
				case GDT_Unknown:		// Not known
				case GDT_CInt16:		// Complex Int16
				case GDT_CInt32:		// Complex Int32
				case GDT_CFloat32:		// Complex Float32
				case GDT_CFloat64:		// Complex Float64
				default:
				returnCode = 1;
				break;
				
				case GDT_Byte:		// Eight bit unsigned integer
				fileInfoPtr->dataTypeCode = kIntegerType;
				fileInfoPtr->numberBits = 8;
				fileInfoPtr->signedDataFlag = FALSE;
				break;
				
				case GDT_UInt16:		// Sixteen bit unsigned integer
				fileInfoPtr->dataTypeCode = kIntegerType;
				fileInfoPtr->numberBits = 16;
				fileInfoPtr->signedDataFlag = FALSE;
				break;
				
				case GDT_Int16:		// Sixteen bit signed integer
				fileInfoPtr->dataTypeCode = kIntegerType;
				fileInfoPtr->numberBits = 16;
				fileInfoPtr->signedDataFlag = TRUE;
				break;
				
				case GDT_UInt32:		// Thirty two bit unsigned integer
				fileInfoPtr->dataTypeCode = kIntegerType;
				fileInfoPtr->numberBits = 32;
				fileInfoPtr->signedDataFlag = FALSE;
				break;
				
				case GDT_Int32:		// Thirty two bit signed integer
				fileInfoPtr->dataTypeCode = kIntegerType;
				fileInfoPtr->numberBits = 32;
				fileInfoPtr->signedDataFlag = TRUE;
				break;
				
				case GDT_Float32:		// Thirty two bit floating point
				fileInfoPtr->dataTypeCode = kRealType;
				fileInfoPtr->numberBits = 32;
				fileInfoPtr->signedDataFlag = TRUE;
				break;
				
				case GDT_Float64:		//  Sixty four bit floating point
				fileInfoPtr->dataTypeCode = kRealType;
				fileInfoPtr->numberBits = 64;
				fileInfoPtr->signedDataFlag = TRUE;
				break;
				
				}		// end "switch (dataType)"
			
			if (returnCode == noErr)
				{
				fileInfoPtr->numberBytes = fileInfoPtr->numberBits/8;
				
				if (fileInfoPtr->numberBits <= 16)
					fileInfoPtr->numberBins = 
					(UInt32)ldexp( (double)1, fileInfoPtr->numberBits);
				
				else		// fileInfoPtr->numberBits > 16
					fileInfoPtr->numberBins = 2048;
				
				fileInfoPtr->bandInterleave = kBSQ;
				
				fileInfoPtr->gdalDataSetH = hDS;
				fileInfoPtr->gdalDataTypeCode = dataType;
				
					// Get the block size.
				
				GDALGetBlockSize (GDALGetRasterBand (hDS, 1), &xBlockSize, &yBlockSize);
				
					// Verify that the block size is the same for all channels.
				
				for (channelNumber=2; channelNumber<=fileInfoPtr->numberChannels; channelNumber++)
					{
					GDALGetBlockSize (GDALGetRasterBand (hDS, channelNumber), &testXBlockSize, &testYBlockSize);
					if (testXBlockSize != xBlockSize || testYBlockSize != yBlockSize)
						returnCode = 3;
					
					}		// end "for (channelNumber=2; channelNumber<=..."
				
				if (returnCode == noErr && yBlockSize > 1)
					{										
					bytesPerStrip = yBlockSize * 
									fileInfoPtr->numberColumns * 
														fileInfoPtr->numberBytes;
					
					bytesPerStripForAllChannels = bytesPerStrip * fileInfoPtr->numberChannels;
					
					stripsPerChannel = fileInfoPtr->numberLines/yBlockSize;
					
					lastBytesPerStrip = (fileInfoPtr->numberLines - stripsPerChannel * yBlockSize) *
					fileInfoPtr->numberColumns * fileInfoPtr->numberBytes;
					
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
						
					}		// end "if (returnCode == noErr && yBlockSize > 1)"
				
				}		// end "if (returnCode == noErr)" 
			
			else		// returnCode != noErr
				returnCode = 1;
			
		}		// end "if (hDS != NULL)"
	
	else		// hDS == NULL
		returnCode = 1;
	
	return (returnCode);
	
}		// end "ReadGDALHeaderInformation" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 05/03/2011

SInt16 ReadGDALProjectionInformation (
												  FileInfoPtr 						fileInfoPtr,
												  GDALDatasetH						hDS)

{	
	double								adfGeoTransform[6],
											returnValue;
	
	const char							*projectionStringPtr;
	const char							*attributeStringPtr;
	
	int									northFlag;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr = NULL;
	
	OGRSpatialReferenceH				ogrSpatialReferenceCPtr;
	
	OGRErr								ogrError;
	
	SInt16								mapUnitsCode,
											returnCode = 0,
											utmZone;
	
	Boolean								projectionStringFlag = FALSE,
											transformStringFlag = FALSE;
	
	
	if (hDS != NULL)
		{					
		projectionStringPtr = GDALGetProjectionRef (hDS);
		
		if (projectionStringPtr != NULL && projectionStringPtr[0] != 0)
			projectionStringFlag = TRUE;	
		
		if (GDALGetGeoTransform (hDS, adfGeoTransform) == CE_None)
			transformStringFlag = TRUE;
		
		if (projectionStringFlag || transformStringFlag)
			{
			if (fileInfoPtr->mapProjectionHandle == NULL)
				fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
			
			if (fileInfoPtr->mapProjectionHandle != NULL)
				{ 								
				mapProjectionInfoPtr = (MapProjectionInfoPtr)
				GetHandlePointer (fileInfoPtr->mapProjectionHandle,
											kNoLock,
											kNoMoveHi);
					
				}		// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
			
			if (mapProjectionInfoPtr == NULL)
				returnCode = 1;
			
			if (returnCode == noErr)
				{					
				if (projectionStringFlag)	
					{
					ogrSpatialReferenceCPtr = OSRNewSpatialReference (projectionStringPtr);
					if (ogrSpatialReferenceCPtr != NULL)
						{	
								// Get the projection
							
						attributeStringPtr = ((OGRSpatialReference*)ogrSpatialReferenceCPtr)->GetAttrValue ("Projection");
							
						if (attributeStringPtr != NULL)
							{						
							if (CompareStringsNoCase (attributeStringPtr, "GEOGRAPHIC", 10) == 0)
								mapProjectionInfoPtr->gridCoordinate.code = kGeographicCode;
						
							else if (CompareStringsNoCase (attributeStringPtr, "ORTHOGRAPH", 10) == 0)
								mapProjectionInfoPtr->gridCoordinate.code = kOrthographicCode;
						
							else if (CompareStringsNoCase (attributeStringPtr, "LAMBERT_AZ", 10) == 0)
								mapProjectionInfoPtr->gridCoordinate.code = kLambertAzimuthalEqualAreaCode;
						
							else if (CompareStringsNoCase (attributeStringPtr, "TRANSVERSE", 10) == 0)
								mapProjectionInfoPtr->gridCoordinate.code = kTransverseMercatorCode;
						
							else if (CompareStringsNoCase (attributeStringPtr, "Albers_Con", 10) == 0)
								mapProjectionInfoPtr->gridCoordinate.code = kAlbersConicalEqualAreaCode;
						
							else if (CompareStringsNoCase (attributeStringPtr, "Sinusoidal", 10) == 0)
								mapProjectionInfoPtr->gridCoordinate.code = kSinusoidalCode;
						
							}		// end "if (attributeStringPtr != NULL)"
							
								// Get the Datum
							
						attributeStringPtr = ((OGRSpatialReference*)ogrSpatialReferenceCPtr)->GetAttrValue ("Datum");
							
						if (attributeStringPtr != NULL)
							{						
							if (CompareStringsNoCase (attributeStringPtr, "Sphere", 6) == 0)
								mapProjectionInfoPtr->geodetic.datumCode = kSphereDatumCode;
								
							else if (CompareStringsNoCase (attributeStringPtr, "WGS_1984", 8) == 0)
								mapProjectionInfoPtr->geodetic.datumCode = kWGS84Code;
						
							}		// end "if (attributeStringPtr != NULL)"
							
								// Get the Spheroid
							
						attributeStringPtr = ((OGRSpatialReference*)ogrSpatialReferenceCPtr)->GetAttrValue ("Spheroid");
							
						if (attributeStringPtr != NULL)
							{						
							if (CompareStringsNoCase (attributeStringPtr, "Sphere", 6) == 0)
								mapProjectionInfoPtr->geodetic.spheroidCode = kSphereEllipsoidMenuItem - 1;
			
							else if (CompareStringsNoCase (attributeStringPtr, "WGS84", 5) == 0)
								mapProjectionInfoPtr->geodetic.spheroidCode = kWGS84EllipsoidMenuItem - 1;
			
							else if (CompareStringsNoCase (attributeStringPtr, "WGS 84", 6) == 0)
								mapProjectionInfoPtr->geodetic.spheroidCode = kWGS84EllipsoidMenuItem - 1;
						
							}		// end "if (attributeStringPtr != NULL)"
					
								// Get the linear units	
							
						attributeStringPtr = ((OGRSpatialReference*)ogrSpatialReferenceCPtr)->GetAttrValue ("Unit");
					
						mapUnitsCode = 0;				
						if (attributeStringPtr != NULL)
							{
								// Units code
							
							if (CompareStringsNoCase (attributeStringPtr, "DMS", 3) == 0)
									// Decimal seconds
								mapUnitsCode = kAVDegreesMinutesSecondsCode;
							
							else if (CompareStringsNoCase (attributeStringPtr, "DM", 2) == 0)
									// Decimal minutes
								mapUnitsCode = kAVDecimalMinutesCode;
							
							else if (CompareStringsNoCase (attributeStringPtr, "DS", 2) == 0)
									// Decimal seconds
								mapUnitsCode = kAVDecimalSecondsCode;
							
							else if (CompareStringsNoCase (attributeStringPtr, "D", 1) == 0)
									// Decimal degrees
								mapUnitsCode = kAVDecimalDegreesCode;
							
							else if (CompareStringsNoCase (attributeStringPtr, "rad", 3) == 0)
									// Decimal seconds
								mapUnitsCode = kAVRadiansCode;
							
							else if (CompareStringsNoCase (attributeStringPtr, "MET", 3) == 0)			
								mapUnitsCode = kMetersCode;
							
							else if (CompareStringsNoCase (attributeStringPtr, "FEE", 3) == 0)			
								mapUnitsCode = kFeetCode;
							
							mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;
							
							}		// end "if (tReturnCode == 0)"
						
						returnValue	= OSRGetSemiMajor (ogrSpatialReferenceCPtr, &ogrError);
						if (ogrError == OGRERR_NONE)
							mapProjectionInfoPtr->geodetic.semiMajorAxis = returnValue;
						
						returnValue	= OSRGetSemiMinor (ogrSpatialReferenceCPtr, &ogrError);
						if (ogrError == OGRERR_NONE)
							mapProjectionInfoPtr->geodetic.semiMinorAxis = returnValue;
													
								// Set the projection parameters if possible.
								
						if (mapProjectionInfoPtr->gridCoordinate.code == kLambertAzimuthalEqualAreaCode)
							SetLambertAzimuthalEqualAreaParameters (
												mapProjectionInfoPtr,
												OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
																				
						else if (mapProjectionInfoPtr->gridCoordinate.code == kTransverseMercatorCode)
							{
									// Check if this happens to be UTM
									
							utmZone = OSRGetUTMZone (ogrSpatialReferenceCPtr, &northFlag);
							
							if (utmZone > 0)
								{
								mapProjectionInfoPtr->gridCoordinate.code = kUTMCode;
								SetUTMParametersFromZone (
												utmZone,
												&mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian,
												&mapProjectionInfoPtr->gridCoordinate.latitudeOrigin,
												&mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian,
												&mapProjectionInfoPtr->gridCoordinate.falseEasting,
												&mapProjectionInfoPtr->gridCoordinate.falseNorthing);
								
								mapProjectionInfoPtr->gridCoordinate.gridZone = utmZone;
														
								if (!northFlag)
									{
									mapProjectionInfoPtr->gridCoordinate.falseNorthing = 10000000.;
									mapProjectionInfoPtr->gridCoordinate.gridZone = 
																				-fabs(mapProjectionInfoPtr->gridCoordinate.gridZone);
									
									}		// and "if (!northFlag)"
															
								}		// end "if (utmZone > 0)"
							
							else		// this is not UTM
								SetTransverseMercatorParameters (
												mapProjectionInfoPtr,
												mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
												OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "scale_factor", 1, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
								
							}		// end "else if (...->gridCoordinate.code == kTransverseMercatorCode)"
																				
						else if (mapProjectionInfoPtr->gridCoordinate.code == kSinusoidalCode)
							SetSinusoidalParameters (
												mapProjectionInfoPtr,
												mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
												OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_1", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
																				
						else if (mapProjectionInfoPtr->gridCoordinate.code == kAlbersConicalEqualAreaCode)
							SetAlbersEqualAreaParameters (
												mapProjectionInfoPtr,
												OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_1", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "standard_parallel_2", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
																				
						else if (mapProjectionInfoPtr->gridCoordinate.code == kLambertAzimuthalEqualAreaCode)
							SetLambertAzimuthalEqualAreaParameters (
												mapProjectionInfoPtr,
												OSRGetProjParm (ogrSpatialReferenceCPtr, "central_meridian", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_easting", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "false_northing", 0, NULL));
																				
						else if (mapProjectionInfoPtr->gridCoordinate.code == kOrthographicCode)
							SetOrthographicParameters (		
												mapProjectionInfoPtr,
												mapProjectionInfoPtr->geodetic.semiMajorAxis,	// radiusSpheroid
												OSRGetProjParm (ogrSpatialReferenceCPtr, "longitude_of_origin", 0, NULL),
												OSRGetProjParm (ogrSpatialReferenceCPtr, "latitude_of_origin", 0, NULL));
						
						OSRRelease (ogrSpatialReferenceCPtr);	
						
						}		// end "if (ogrSpatialReferenceCPtr != NULL)"
					
					}		// end "if (projectionStringFlag)"
				
				if (transformStringFlag)
					{
							// Get the map transform information
							
					mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 = adfGeoTransform[0];
					mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 = adfGeoTransform[3];
					
					mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin = 
												mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
					mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin = 
												mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
					
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = adfGeoTransform[1];
					mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = -adfGeoTransform[5]; 
					
					}		// end "if (tranformStringFlag)"
					
				}		// end "if (returnCode == noErr)"
			
			}		// end "if (projectionStringFlag || tranformStringFlag)"			
			
		}		// end "if (hDS != NULL)"
	
	else		// hDS == NULL
		returnCode = 1;
	
	return (returnCode);
	
}		// end "ReadGDALProjectionInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2011)
//								� Purdue Research Foundation
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
//	Revised By:			Larry L. Biehl			Date: 06/13/2011

SInt16 ReadHeaderWithGDALLibrary (
											 FileInfoPtr 						fileInfoPtr,
				 SInt16								formatOnlyCode)

{	
	char									filePathString[512];
	
	char									*fileFormatDescriptionPtr,
											*filePathPtr;
	
	GDALDatasetH						hDS;
	
	SInt16								fileType = 0,
											gdalReturnCode,
											numberClasses,
											returnCode;
	
	
   returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
													 filePathString, 
													 510,
													 &filePathPtr);
	if (returnCode != noErr)	
																							return (returnCode);	
	
	
	
	GDALAllRegister();	
//	MultiSpecGDALRegister ();
//	hDS = NULL;
	hDS = GDALOpen (filePathPtr, GA_ReadOnly);
	if (hDS != NULL)
		{		
		GDALDriverH dataSetDriver = GDALGetDatasetDriver (hDS);
		fileFormatDescriptionPtr = (char*)GDALGetDescription ((GDALMajorObjectH)dataSetDriver);
		
		if (CompareStringsNoCase (fileFormatDescriptionPtr, "JP2KAK\0") == 0)
			fileType = kJPEG2000Type;
		
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "JP2ECW\0") == 0)
			fileType = kJPEG2000Type;
		
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "JPG\0") == 0)
			fileType = kJPEGType;
		
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "PNG\0") == 0)
			fileType = kPNGType;
		
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "AIG\0") == 0)
			fileType = kArcInfoBinaryGridType;
		
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "ECW\0") == 0)
			fileType = kECWType;
		
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "HDF5\0") == 0)
			fileType = kHDF5Type;
			
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "EHdr\0") == 0)
			fileType = kArcViewType;
			
		else if (CompareStringsNoCase (fileFormatDescriptionPtr, "GRIB\0") == 0)
			fileType = kDefaultType;
		
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
				
					//				else if (formatOnlyCode == kThematicFiles)
					//					fileInfoPtr->thematicType = TRUE;
				
				GDALClose (hDS);
				
																							return (noErr);
				
				}		// end "if (formatOnlyCode != kLoadHeader)"
			
			else		// formatOnlyCode == kLoadHeader
				{
/*				if (fileType == kHDF5Type)
					{
					char **metadata = ((GDALDataset*)hDS)->GetMetadata ("SUBDATASETS");    
					
					if (metadata) 
						{
								// need to skip SUBDATASET_?_DESC;
								// we only need SUBDATASET_?_NAME here
						
						for (char **meta = metadata; *meta; meta += 2) 
							{
							const char *name = strstr(*meta, "=");
							
							}		// end ""
						
						}		// end "if (metadata)"							
					
					}		// end fileType == kHDF5Type
*/				
				gdalReturnCode = ReadGDALHeaderInformation (fileInfoPtr, hDS);
				
				if (gdalReturnCode == noErr)
					gdalReturnCode = ReadGDALProjectionInformation (fileInfoPtr, hDS);
					
				if (gdalReturnCode == noErr)
					{
					fileInfoPtr->format = fileType;	
					fileInfoPtr->ancillaryInfoformat = kArcViewType;
					
					if (fileType == kJPEG2000Type)
						fileInfoPtr->dataCompressionCode = kJPEG2000Compression;
					
					else if (fileType == kJPEGType)
						fileInfoPtr->dataCompressionCode = kSkipHuffmanCompression;
					
					else if (fileType == kPNGType)						
						fileInfoPtr->dataCompressionCode = kPNGCompression;
					
					else if (fileType == kArcInfoBinaryGridType)
						fileInfoPtr->dataCompressionCode = kRLECompression;
					
					else if (fileType == kECWType)
						fileInfoPtr->dataCompressionCode = kJPEG2000Compression;
						
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
						
						}		// end "if (gGetFileImageType == 0)"
					
					fileInfoPtr->thematicType = FALSE;
					if (gGetFileImageType == kThematicImageType)
						{
						fileInfoPtr->thematicType = TRUE;
						fileInfoPtr->numberClasses = numberClasses;
						
						}		// end "if (gGetFileImageType == kThematicImageType)"	
					
					}		// end "if (gdalReturnCode == noErr)"
				
				else		// gdalReturnCode != noErr
					{
							// Display an alert indicating that MultiSpec could not read or had
							// problems reading this file.
					
					DisplayAlert (kErrorAlertID, 
									  kStopAlert, 
									  kAlertStrID, 
									  (SInt16)(IDS_Alert95 + gdalReturnCode - 1),
									  0, 
									  NULL);
					
					returnCode = 1;
					
					}		// end "else gdalReturnCode != noErr"
				
				}		// end "if (formatOnlyCode == kLoadHeader)" 
							
			}		// end "if (fileType != 0)"
			
		}		// end "if (hDS != NULL)"
	
	else		// hDS == NULL
		returnCode = 1;
	
	if (returnCode != noErr)
		{
		GDALClose (hDS);
		fileInfoPtr->gdalDataSetH = 0;
		fileInfoPtr->gdalDataTypeCode = 0;
		
		}		// end "if (returnCode != noErr)"
	
	return (returnCode);
	
}		// end "ReadHeaderWithGDALLibrary" 

#endif	// include_gdal_capability