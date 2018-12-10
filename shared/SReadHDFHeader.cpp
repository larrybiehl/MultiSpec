//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//									Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SReadHDFHeader.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/29/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh & Windows Operating Systems	
//
//	Brief description:	This file contains routines which are used to access
//								various disk files.
//
//	Functions in file:
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#if include_hdf_capability
	
#if defined multispec_mac
#endif	// defined multispec_mac    

#if defined multispec_win
	#include "SMultiSpec.h"
	#include "CFileStream.h"
#endif	// defined multispec_win  

#if defined multispec_lin
	#include "SMultiSpec.h"
	#include "CFileStream.h"
	
	#ifndef HDF
		#define HDF 
	#endif
	#include "hdfi.h"
	#include "hfile.h"
	#include "local_nc.h"
	#include "mfhdf.h"

	//#if !defined multispec_wxmac
		extern intn SDgetoffset (
					int32  sdsid, int32 *dataSetOffset);
	
		extern intn SDgetspecialinfo (
					int32 sdsid, sp_info_block_t* info_blockPtr);
	//#endif
	
	extern int32 hdf_get_vp_aid (
					NC *handle, NC_var *vp);
	
	extern NC* SDIhandle_from_id (
					int32 id, intn typ);
	
	extern NC_var* SDIget_var (
					NC *handle, int32 sdsid);
	
	extern int32 SDIfreevarAID (
					NC * handle, int32 index);
#endif	// defined multispec_lin
 
#include 	"dfsd.h"
#include 	"hfile.h"
#include 	"mfhdf.h"
#include 	"dfrig.h"
//#include 	"local_nc.h"	
    
//extern void ncFree_cdfs (void);

intn SDgetspecialinfo (
				int32									sdsid, 
				sp_info_block_t*					info_blockPtr);

extern void SetHDF4HasHFSPlusAPISetting (
				Boolean								hasHFSPlusAPIsFlag);
							
							
void CloseHDFGlobals (void);

double FindClosestValue (
				double		 						inputValue,
				double*								outputValueListPtr,
				UInt32								sizeList);

UCharPtr GetAttributeBuffer (
				SInt32		 						id,
				UInt32								numberAttributes);
							
Boolean GetBottomToTopFlag (
				SInt16								format,
				SInt32								file_id,
				SInt32		 						sdid);

Boolean GetFillDataValue (
				SInt32		 						sdid,
				double*								fillValuePtr);

intn GetHDFDataSetInformation (
				SInt32		 						sdid,
				SInt16								format,
				SInt32*								returnCodePtr,
				UInt32*								numberChannelsPtr,
				UInt32*								numberLinesPtr,
				UInt32*								numberColumnsPtr,
				UInt32*								dataSetOffsetPtr,
				UInt16*								numberBytesPtr,
				UInt16*								dataTypeCodePtr,
				UInt16*								dataCompressionCodePtr,
				UInt16*								bandInterleaveFormatPtr,
				Boolean*								signedDataFlagPtr,
				Boolean*								swapBytesFlagPtr,
				Boolean*								doNotUseHDFLineReadFlagPtr,
				UCharPtr								extensionNamePtr,
				Boolean*								noDataValueFlagPtr,
				double*								noDataValuePtr);

SInt16 GetHDFInstrumentCode (
				SInt32		 						id);

intn GetHDFProjectionInformation (
				SInt32		 						id,
				UCharPtr								suffix,
				SInt16								format,
				FileInfoPtr							fileInfoPtr,
				SInt16								groupNumber,
				SInt16								numberInGroup,
				SInt32*								returnCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								mapOrientationAnglePtr,
				SInt16*								mapReferenceSystemCodePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								gridZonePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr,
				double*								lowerRightCoordinatesPtr,
				double*								spatialResolutionPtr,
				double*								upperLeftImagePtr,
				Boolean*								convertCenterFromLatLongToMapFlagPtr,
				Boolean*								convertUpperLeftFromLatLongToMapFlagPtr,
				Boolean*								convertLowerLeftFromLatLongToMapFlagPtr);
							
void GetMapInfoFromGridStructure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				UCharPtr								bufferPtr,
				SInt32*								returnCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								mapOrientationAnglePtr,
				SInt16*								mapReferenceSystemCodePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								gridZonePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr);

void GetMapInfoFromNetCDF_HRLDAS_Structure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				UCharPtr								bufferPtr,
				SInt32*								returnCodePtr,
				UInt32								numberLines,
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

void GetMapInfoFromNetCDF_LIS_Structure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				UCharPtr								bufferPtr,
				SInt32*								returnCodePtr,
				UInt32								numberLines,
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
				Boolean*								convertLowerLeftFromLatLongToMapFlagPtr);

void GetMapInfoFromSeaSpaceStructure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				FileInfoPtr							fileInfoPtr,
				SInt16								format,
				UCharPtr								bufferPtr,
				SInt16								groupNumber,
				SInt16								numberInGroup,
				SInt32*								returnCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								mapOrientationAnglePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								gridZonePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr,
				double*								upperLeftImagePtr,
				Boolean*								convertUpperLeftFromLatLongToMapFlagPtr);

SInt32 GetSdidValue (
				HdfDataSets*		 				hdfDataSetsPtr);

SInt16 GetSpecificTextAttributeInformation (
				SInt32		 						id,
				SInt32								numberAttributes,
				UCharPtr								masterGroupString,
				UCharPtr								groupString,
				UCharPtr								objectString,
				Boolean								numValFieldUsedFlag,
				Boolean								equalSignUsedFlag,
				UCharPtr								bufferPtr,
				SInt32								numberValues,
				UCharPtr								outputString1Ptr,
				UCharPtr								outputString2Ptr);

SInt16 GetSpecificNumericAttributeInformation (
				SInt32		 						id,
				SInt32								numberAttributes,
				UCharPtr								masterGroupString,
				UCharPtr								groupString,
				UCharPtr								objectString,
				Boolean								numValFieldUsedFlag,
				Boolean								equalSignUsedFlag,
				UCharPtr								bufferPtr,
				SInt32								numberValues,
				double*								valuesPtr);
							
//SInt32 GetNumberOfValidDataSets (
//				SInt32		 						sdid,
//				SInt32								numDataSets);

Boolean ListAttributeInformation (
				SInt32		 						id,
				UInt32								numberAttributes);

SInt32 LoadHdfDataSetNames (
				SInt32		 						file_id,
				SInt16								format,
				HdfDataSets*						hdfDataSetsPtr,
				SInt32								numDataSets,
				CMFileStream*						extensionFileStreamPtr,
				SInt16								instrumentCode,
				SInt32*								firstDataSetIndexPtr);

Boolean LoadNonBSQOffsetBytesInfo (
				FileInfoPtr							fileInfoPtr,
				HdfDataSets*						hdfDataSetsPtr,
				SInt16								groupNumber);
							
void RemoveTrailingCharacters (
				UCharPtr								stringPtr);
							
#define	DATESIZE					9
#define	kNumValFieldNotUsed	0
#define	kNumValFieldUsed		1
#define	kEqualSignNotUsed		0
#define	kEqualSignUsed			1

SInt32 gNumberHDFFilesOpen	= 0;

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void AppendGroupIndicater
//
//	Software purpose:	This routine appends the group identifier to the data set
//							identifier name making sure that the total length of the string
//							with pascal length and c-string terminator is not more than 80
//							characters.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/18/2006
//	Revised By:			Larry L. Biehl			Date: 06/18/2006

void AppendGroupIndicater (
				char*									namePtr,
				UInt32								groupNumber)
				
{
	SInt16								numChars;
	
	UInt16								charIndex;
	
	
			// Append indicater that data set is part of a group.
			
	charIndex = namePtr[0] + 1;
	charIndex = MIN (charIndex, 75);
	
			// Group number is limited to 2 digits.  Should never come close to this.
			
	groupNumber = MIN (groupNumber, 99);
	numChars = sprintf (&namePtr[charIndex], "_G%ld", groupNumber);
	namePtr[0] += (UInt8)numChars;
	charIndex = namePtr[0];
	namePtr[charIndex+1] = 0;
	
}	// end "AppendGroupIndicater"
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseHDF4File
//
//	Software purpose:	This routine closes an hdf file.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/2007
//	Revised By:			Larry L. Biehl			Date: 08/17/2018

void CloseHDF4File (
				FileInfoPtr 						fileInfoPtr)
				
{	
	HdfDataSets*						hdfDataSetsPtr;
	
	UInt32								dataSet;
	
	
	if (fileInfoPtr->hdfHandle != NULL)
		{
				// This is an hdf file. Release the sdid's and the file_id.
		
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle,
																			kLock);
																
		for (dataSet=1; dataSet<=fileInfoPtr->numberHdfDataSets; dataSet++) 
			{
			if (hdfDataSetsPtr[dataSet].sdid != NULL)
				{
				//SInt64 sdid64 = (SInt64)hdfDataSetsPtr[dataSet].sdid;
				//SInt32 sdid32 = (SInt32)sdid64;
				//SDendaccess (sdid32);
				SDendaccess (GetSdidValue (&hdfDataSetsPtr[dataSet]));
				//SDendaccess ((int32)hdfDataSetsPtr[dataSet].sdid);
				
				}	// end "if (hdfDataSetsPtr[dataSet].sdid != NULL)"
			
			hdfDataSetsPtr[dataSet].sdid = NULL;
		
			}	// end "for (dataSet=1; dataSet<=fileInfoPtr->numberHdfDataSets; ..."
			
		CheckAndUnlockHandle (fileInfoPtr->hdfHandle);
			
		}	// end "if (fileInfoPtr->hdfHandle != NULL)"
			
	if (fileInfoPtr->hdf4FileID > 0)
		{
		SDend (fileInfoPtr->hdf4FileID);
		fileInfoPtr->hdf4FileID = 0;
		gNumberHDFFilesOpen--;
		gNumberOfOpenFiles--;
		
				// Close hdf globals
		
		//if (SDget_numopenfiles () == 0)
		#ifndef multispec_lin
			if (gNumberHDFFilesOpen <= 0)
				CloseHDFGlobals ();
		#endif
			
		}	// end "if (fileInfoPtr->hdf4FileID > 0)"
    
}	// end "CloseHDF4File"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseHDFGlobals
//
//	Software purpose:	This routine closes the HDF routine globals.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/30/2001
//	Revised By:			Larry L. Biehl			Date: 11/27/2007

void CloseHDFGlobals (void)
				
{
	HPend();
	/*
	ncFree_cdfs ();	
		
	DFGRPshutdown ();
	VSPhshutdown ();
	DFR8Pshutdown ();
	DFSDPshutdown ();
	VPshutdown ();
	VPparse_shutdown ();
	VSPshutdown ();
	*/
	
}	// end "CloseHDFGlobals"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double FindClosestValue
//
//	Software purpose:	This routine determines the the value in the value list that is 
//							closest to the input value.  The closest value in the list is
//							returned.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadHDFDataSetInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/16/2006
//	Revised By:			Larry L. Biehl			Date: 07/17/2006

double FindClosestValue (
				double		 						inputValue,
				double*								outputValueListPtr,
				UInt32								sizeList)
				
{
	double								closestDifference,
											difference,
											returnValue;
											
	UInt32								closestIndex,
											index;												
	
	
	closestDifference = fabs (inputValue-outputValueListPtr[0]);
	closestIndex = 0;
	
	for (index=1; index<sizeList; index++)
		{
		difference = fabs (inputValue-outputValueListPtr[index]);
		if (difference < closestDifference)
			{
			closestIndex = index;
			closestDifference = difference;
			
			}	// end "if (difference < closestDifference)"
		
		}	// end "for (index=0; index<sizeList; index++)"
		
	returnValue = 0;
	if (closestDifference < 5)
		returnValue = outputValueListPtr[closestIndex];	
	
	return (returnValue);
    
}	// end "FindClosestValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		char* GetAttributeBuffer
//
//	Software purpose:	This routine gets a buffer that can be used to read the attribute
//							information. Allow for the longest attribute.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	= Maximum number of buffer bytes needed.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/07/2002
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

UCharPtr GetAttributeBuffer (
				SInt32		 						id,
				UInt32								numberAttributes)
				
				
{
	UCharPtr								bufferPtr;
	
	#if TARGET_RT_MAC_MACHO
		int									attributeSize,
												attributeType;
	
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								attributeSize,
												attributeType;
	#endif	// TARGET_RT_MAC_MACHO, else...
											
	UInt32								bufferSize,
											index;
	
	
	bufferSize = 0;
	bufferPtr = NULL;
	
	for (index=1; index<=numberAttributes; index++)
		{
		if (SDattrinfo ((int)id, 
								index-1, 
								(char*)&gTextString, 
								&attributeType, 
								&attributeSize) == noErr)
			{			
			switch (attributeType)
				{
				case DFNT_CHAR8:
				case DFNT_INT8:
				case DFNT_UINT8:
					bufferSize = MAX (bufferSize, (UInt32)attributeSize);
					break;
					
				case DFNT_INT16:
				case DFNT_UINT16:
					bufferSize = MAX (bufferSize, (UInt32)2*attributeSize);
					break;
					
				case DFNT_INT32:
				case DFNT_UINT32:
				case DFNT_FLOAT32:
					bufferSize = MAX (bufferSize, (UInt32)4*attributeSize);
					break;
					
				case DFNT_FLOAT64:
					bufferSize = MAX (bufferSize, (UInt32)8*attributeSize);
					break;
					
				}	// end "switch (attributeType)"

			}	// end "if (SDattrinfo (sdid, index-1, buffer, ..."

		}	// end "for (index=1; index<=nattrs; index++)"

	bufferSize = MAX (256, bufferSize);

			// Now get the attribute buffer. Allow a pad. At least enough for a
			// c-terminator at the end of a string.

	bufferPtr = (UCharPtr)MNewPointer (bufferSize + 8);
	
	return (bufferPtr);
	
}	// end "GetAttributeBuffer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetBottomToTopFlag
//
//	Software purpose:	This routine determines if the image data in the file needs to
//							be treated as being stored bottom to top. Currently
//							the only case where this occurs is with the HRLDAS or LIS model 
//							output in hdf5 (not handled here) and netcdf files. The 
//							information needed is stored in the data set attributes.
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
//	Coded By:			Larry L. Biehl			Date: 11/08/2013
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean GetBottomToTopFlag (
				SInt16								format,
				SInt32								file_id,
				SInt32		 						sdid)

{	
	UCharPtr								bufferPtr = NULL;
	
	double								returnValue;
											
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									dims[10],
   											numberAttributes = 0,
												numberType,
												numDataSets,
												nglobals,
   											rank;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								dims[10],
   											numberAttributes = 0,
   											numberType,
												numDataSets,
   											nglobals,
   											rank;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
	SInt16								returnCode;
	
	Boolean								bottomToTopFlag = FALSE;


		// Get global file metadata if there is any
	
	if (format == kNETCDFType &&
			file_id != FAIL &&
				SDfileinfo (file_id, &numDataSets, &nglobals) == noErr)
		{
		if (nglobals > 0)
			{
					// Get the buffer to be used to read the attribute information.
			
			bufferPtr = (UCharPtr)GetAttributeBuffer (file_id, nglobals);
			
			if (bufferPtr == NULL)
				nglobals = 0;
			
			}	// end "if (nglobals > 0)"
		
		if (nglobals > 0)
			{
					// See if a "SOUTH-NORTH" attribute is included.
				
			returnCode = GetSpecificNumericAttributeInformation (
															file_id,
															nglobals,
															(UCharPtr)"SOUTH-NORTH_GRID_DIMENSION",
															(UCharPtr)"",
															(UCharPtr)"",
															kNumValFieldNotUsed,
															kEqualSignNotUsed,
															bufferPtr,
															1,
															&returnValue);
			
			if (returnCode == 1)
				bottomToTopFlag = TRUE;
			
			bufferPtr = CheckAndDisposePtr (bufferPtr);
				
			}	// end "if (nglobals > 0)"
			
		}	// end "if (file_id != FAIL && SDfileinfo (file_id, &numDataSets, &nglobals) == noErr)"
	
		// Get data set metadata if there is any.
			
	if (format == kNETCDFType && sdid != FAIL && !bottomToTopFlag)
		{
				// The default for netCDF files is for the bottomToTopFlag to be set
				// to true.
		
		hdfReturn = SDgetinfo (sdid, 
										(char*)&gTextString, 
										&rank, 
										dims, 
										&numberType, 
										&numberAttributes);
										
		if (hdfReturn == FAIL)
			numberAttributes = 0; 
		
		if (numberAttributes > 0)
			{
					// Get the buffer to be used to read the attribute information.
							
			bufferPtr = (UCharPtr)GetAttributeBuffer (sdid, numberAttributes);
					
			if (bufferPtr == NULL)
				numberAttributes = 0;
				
			}	// end "if (numberAttributes > 0)"
		
		if (numberAttributes > 0)
			{
					// See if a "data_stored_bottom_to_top" attribute is included.

			returnCode = GetSpecificTextAttributeInformation (
																sdid,
																numberAttributes,
																(UCharPtr)"data_stored_bottom_to_top",
																(UCharPtr)"yes",
																(UCharPtr)"",
																kNumValFieldNotUsed,
																kEqualSignNotUsed,
																bufferPtr,
																0,
																NULL,
																NULL);
																			
			if (returnCode == 1)
				bottomToTopFlag = TRUE;
			
			CheckAndDisposePtr (bufferPtr);
			
			}	// end "if (numberAttributes > 0)"
																																		
		}	// end "if (format == kNETCDFType && sdid != NULL)"
			
	return (bottomToTopFlag);

}	// end "GetBottomToTopFlag"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetFillDataValue
//
//	Software purpose:	This routine looks for the fill data value for the input image
//							data set.
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
//	Coded By:			Larry L. Biehl			Date: 03/15/2014
//	Revised By:			Larry L. Biehl			Date: 03/15/215

Boolean GetFillDataValue (
				SInt32		 						sdid,
				double*								fillValuePtr)

{	
	UCharPtr								bufferPtr = NULL;
											
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									dims[10],
   											numberAttributes = 0,
   											numberType,
   											rank;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								dims[10],
   											numberAttributes = 0,
   											numberType,
   											rank;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
	SInt16								returnCode;
	
	Boolean								fillDataValueExistsFlag = FALSE;


			// Get metadata if there is any.
			
	*fillValuePtr = 0;
	if (sdid != FAIL)
		{						
		hdfReturn = SDgetinfo (sdid, 
										(char*)&gTextString, 
										&rank, 
										dims, 
										&numberType, 
										&numberAttributes);
										
		if (hdfReturn == FAIL)
			numberAttributes = 0; 
		
		if (numberAttributes > 0)
			{
					// Get the buffer to be used to read the attribute information.
							
			bufferPtr = (UCharPtr)GetAttributeBuffer (sdid, numberAttributes);
					
			if (bufferPtr == NULL)
				numberAttributes = 0;
				
			}	// end "if (numberAttributes > 0)"
		
		if (numberAttributes > 0)
			{
					// See if a "data_stored_bottom_to_top" attribute is included.

			returnCode = GetSpecificNumericAttributeInformation (sdid,
																					numberAttributes,
																					(UCharPtr)"_FillValue",
																					(UCharPtr)"",
																					(UCharPtr)"",
																					kNumValFieldUsed,
																					kEqualSignNotUsed,
												 									bufferPtr,
																					1,
																					fillValuePtr);
																			
			if (returnCode == 1)
				fillDataValueExistsFlag = TRUE;
																				
			}	// end "if (numberAttributes > 0)"
																																		
		}	// end "if (sdid != NULL)"

	CheckAndDisposePtr (bufferPtr);
			
	return (fillDataValueExistsFlag);

}	// end "GetFillDataValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		intn GetHDFInformation
//
//	Software purpose:	This routine checks the input HDF data set to determine
//							if MultiSpec can handle it.
//
//	Parameters in:					
//
//	Parameters out:	*returnCodePtr
//								= 0, Image data set was found, No Error
//								= 1, No image data set was found
//								= 2, Image data set was found but it is compressed.
//								= 3, Image data set was found but it is 32-bit integer.
//								= 4, Image data set was found but it is 32-bit float.
//								= 5, Image data set was found but it is 64-bit double.
//								= 6, Image data set was found but cannot hande data format.
//								= 7, The selected data set is not an image.	
//								= 8, The selected data set is in a file that is not with
//											the hdf header file.	
//
//	Value Returned:	= FALSE, MultiSpec cannot handle the input data set.
//							= TRUE, MultiSpec can handle the data set.
//
// Called By:			LoadHdfDataSetNames in SReadHDFHeader.cpp
//							LoadHDFDataSetInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/27/2001
//	Revised By:			Larry L. Biehl			Date: 11/29/2018

intn GetHDFDataSetInformation (
				SInt32		 						sdid,
				SInt16								format,
				SInt32*								returnCodePtr,
				UInt32*								numberChannelsPtr,
				UInt32*								numberLinesPtr,
				UInt32*								numberColumnsPtr,
				UInt32*								dataSetOffsetPtr,
				UInt16*								numberBytesPtr,
				UInt16*								dataTypeCodePtr,
				UInt16*								dataCompressionCodePtr,
				UInt16*								bandInterleaveFormatPtr,
				Boolean*								signedDataFlagPtr,
				Boolean*								swapBytesFlagPtr,
				Boolean*								doNotUseHDFLineReadFlagPtr,
				UCharPtr								extensionNamePtr,
				Boolean*								noDataValueFlagPtr,
				double*								noDataValuePtr)
				
{									
	intn 									hdfReturn,
											hdfReturnCode2;
	
	#if TARGET_RT_MAC_MACHO
		int									attributeSize,
   											attributeType,
   											dataSetOffset,
   											dims[10],
												nattrs,
												numberType,
												rank;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								attributeSize,
   											attributeType,
   											dataSetOffset,
   											dims[10],
												nattrs,
												numberType,
												rank;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								index;
   										
   Boolean								continueFlag,
   										interleaveSetFlag,
											setSwapBytesFlag;
   
	char									name[80];
   comp_coder_t    					compressionType;
   comp_info							compressionInfo;
   sp_info_block_t					info_block;
   
   			
	*returnCodePtr = 1;
	hdfReturn = SUCCEED;
	continueFlag = TRUE;
	interleaveSetFlag = FALSE;
	/*		
			// Get information about the data.
			// I have no test data that has this information to check out. Will
			// not implement now.
			
	Str255								coordsys, 
											format,
											label,
											unit; 
			
	hdfReturn = SDgetdatastrs (sdid,
										(char*)&label, 
										(char*)&unit, 
										(char*)&format, 
										(char*)&coordsys,
										250);
	*/	
	rank = 10;	// Indicate what the max length of dims is.		
	hdfReturn = SDgetinfo (sdid, name, &rank, dims, &numberType, &nattrs);
	/*
	sprintf ((char*)&gTextString, 
									"    %s data set attributes%s",
									formatNamePtr,
									gEndOfLine);
			continueFlag = ListString ((char*)&gTextString, length, gOutputTextH);
	*/
	*dataCompressionCodePtr = kNoCompression;

	if (hdfReturn != FAIL)
		{
		//SDgetcompress (sdid, &compressionType, &compressionInfo);
		hdfReturnCode2 = SDgetcompinfo (sdid, &compressionType, &compressionInfo);

		if (hdfReturnCode2 != FAIL)
			{
			switch (compressionType)
				{
				case COMP_CODE_RLE:
					*dataCompressionCodePtr = kRLECompression;
					break;
					
				case COMP_CODE_NBIT:
					*dataCompressionCodePtr = kNBITCompression;
					break;
					
				case COMP_CODE_SKPHUFF:
					*dataCompressionCodePtr = kSkipHuffmanCompression;
					break;
					
				case COMP_CODE_DEFLATE:
					*dataCompressionCodePtr = kGZIPCompression;
					break;
					
				case COMP_CODE_SZIP:
					*dataCompressionCodePtr = kSzipCompression;
					break;

				}	// end "switch (compressionType)"				

			}	// end "if (hdfReturnCode2 != FAIL)"		

		}	// end "if (hdfReturn != FAIL)"
				
	if (hdfReturn != FAIL)
		{
		info_block.key = 0;
		info_block.path = NULL;
		info_block.cdims = NULL;
		extensionNamePtr[0] = 0;
		#if defined multispec_mac || defined multispec_win
			SDgetspecialinfo (sdid, &info_block);
			//HDget_special_info (sdid, &info_block);
		#endif

		if (info_block.cdims != NULL)
			free (info_block.cdims);
		
		if (info_block.key != 0 && info_block.key != SPECIAL_EXT)
			{
			if (*returnCodePtr == 1)
				*returnCodePtr = 6;
		
			}	// end "if (info_block.key != 0 && ..."
		
		if (extensionNamePtr != NULL)
			{
			if (info_block.key == SPECIAL_EXT)
				strcpy ((char*)extensionNamePtr, info_block.path);
				
			else	// info_block.key != SPECIAL_EXT
				extensionNamePtr[0] = 0;
			
			}	// end "if (extensionNamePtr != NULL)"	
		
		}	// end "if (hdfReturn != FAIL)"		
		
			// Go through attributes to get information about the band interleave
			// description
	
	if (bandInterleaveFormatPtr != NULL)
		{
		*bandInterleaveFormatPtr = 0;
		for (index=1; index<=nattrs; index++)
			{
			if (SDattrinfo (sdid, index-1, name, &attributeType, &attributeSize) == noErr)
				{
				if ((strcmp (name, "Interleave Format") == 0) && 
											(attributeType == DFNT_CHAR8) &&
													(attributeSize < 80))
					{
					if (SDreadattr (sdid, index-1, name) == noErr)
						{
						if (strcmp (name, "BIL") == 0)
							*bandInterleaveFormatPtr = kBIL;
							
						else if (strcmp (name, "BSQ") == 0)
							*bandInterleaveFormatPtr = kBSQ;
							
						else if (strcmp (name, "BIP") == 0)
							*bandInterleaveFormatPtr = kBIS;
						
						}	// end "if (SDreadattr (sdid, index-1, name) == noErr)"
						
					break;
					
					}	// end "if ((strcmp (name, "
					
				}	// end "if (SDattrinfo (sdid, index-1, ..."
			
			}	// end "for (i=1; i<=nattrs; i++)"
		
		if (*bandInterleaveFormatPtr != 0)	
			interleaveSetFlag = TRUE;
		
		else	// *bandInterleaveFormatPtr == 0
			*bandInterleaveFormatPtr = kBSQ;
			
		}	// end "if (bandInterleaveFormatPtr != NULL)"
	/*	
	SInt32 		count,
					dimid;
						
	dimid = SDgetdimid (sdid, 0);
	SDdiminfo (dimid, name, &count, &attributeType, &attributeSize);
	if (attributeType != DFNT_NONE)
		SDgetdimscale (dimid, name);
		
	dimid = SDgetdimid (sdid, 1);
	SDdiminfo (dimid, name, &count, &attributeType, &attributeSize);
	if (attributeType != DFNT_NONE)
		SDgetdimscale (dimid, name);
		
	if (rank == 3)
		{
		dimid = SDgetdimid (sdid, 2);
		SDdiminfo (dimid, name, &count, &attributeType, &attributeSize);
		if (attributeType != DFNT_NONE)
			SDgetdimscale (dimid, name);
		
		}	// end "if (rank == 3)"
	*/
	if (hdfReturn != FAIL && continueFlag)
		{
		if (rank == 1)
			{
			if (numberChannelsPtr != NULL)
				*numberChannelsPtr = 1;
				
			if (numberLinesPtr != NULL)
				*numberLinesPtr = (UInt32)dims[0];
				
			if (numberColumnsPtr != NULL)
				*numberColumnsPtr = 1;
			
			}	// end "if (numberDimensions == 2)"
			
		else if (rank == 2)
			{
			if (numberChannelsPtr != NULL)
				*numberChannelsPtr = 1;
				
			if (numberLinesPtr != NULL)
				*numberLinesPtr = (UInt32)dims[0];
				
			if (numberColumnsPtr != NULL)
				*numberColumnsPtr = (UInt32)dims[1];
			
			}	// end "if (numberDimensions == 2)"
				
		else if (rank == 3)
			{
			if (bandInterleaveFormatPtr != NULL)
				{
						// Try to make a determination of what the interleave is if it
						// has not already been determined. For now we will assume that
						// BIS is not used and the the dimension with the smallest value
						// is the number of channels. Hopefully a flag or some other
						// more determinate way can be found to do this with the HDF
						// format. At this point the band interleave format has defaulted
						// to BSQ.
						// As of 2/21/2012, we will leave the band format to BSQ. Have run
						// into a case where the logic below does not work for determining
						// band interleave. Will need to find case where there is actually
						// BIL and BIS interleaved data for HDF when the rank is 3.
						
				if (format == kHDF4Type && !interleaveSetFlag)
					{
					if (dims[1] < dims[0] && dims[1] < dims[2])
						*bandInterleaveFormatPtr = kBIL;

					else if (dims[2] < dims[0] && dims[2] < dims[1])
						*bandInterleaveFormatPtr = kBIS;
					
					}	// end "if (format == kHDF4Type && !interleaveSetFlag)"
					
				switch (*bandInterleaveFormatPtr)
					{	
					case kBIL:
						if (numberLinesPtr != NULL)
							*numberLinesPtr = (UInt32)dims[0];
							
						if (numberChannelsPtr != NULL)
							*numberChannelsPtr = (UInt16)dims[1];
							
						if (numberColumnsPtr != NULL)
							*numberColumnsPtr = (UInt32)dims[2];
						break;
					
					case kBSQ:
						if (numberChannelsPtr != NULL)
							*numberChannelsPtr = (UInt16)dims[0];
							
						if (numberLinesPtr != NULL)
							*numberLinesPtr = (UInt32)dims[1];
							
						if (numberColumnsPtr != NULL)
							*numberColumnsPtr = (UInt32)dims[2];
						break;
					
					case kBIS:
						if (numberLinesPtr != NULL)
							*numberLinesPtr = (UInt32)dims[0];
							
						if (numberColumnsPtr != NULL)
							*numberColumnsPtr = (UInt32)dims[1];
							
						if (numberChannelsPtr != NULL)
							*numberChannelsPtr = (UInt16)dims[2];
						break;
						
					}	// end "switch (*bandInterleaveFormatPtr)"
					
				}	// end "if (bandInterleaveFormatPtr != NULL)"
			
			}	// end "if (rank == 3)"
				
		else if (rank == 4)
			{
					// This is a test for now. Assume band sequential and assume that the number
					// of channels is dim 0 * dim 1. Will assume dim 2 is number channels;
					// dim 3 is number lines and dim 4 is number columns.
			
			*bandInterleaveFormatPtr = kBSQ;
			*numberChannelsPtr = (UInt16)(dims[0] * dims[1]);
			*numberLinesPtr = (UInt32)dims[2];
			*numberColumnsPtr = (UInt32)dims[3];
			*doNotUseHDFLineReadFlagPtr = TRUE;
							
			}	// end "if (rank == 4)"
			
		else	// numberDimensions is out of range to be handled.
			{
			*returnCodePtr = 7;
			continueFlag = FALSE;
			
			}	// end "else numberDimensions is out of range to be handled"
			
		}	// end "if (hdfReturn != FAIL)"
		
	setSwapBytesFlag = FALSE;
	if (hdfReturn != FAIL && continueFlag)
		{
					// Now get the data type of the scientific data set.
					
		switch (numberType)
			{
			case DFNT_CHAR8:	// 4
			case DFNT_INT8:	// 20:
			case DFNT_UINT8:	// 21:
			case DFNT_LINT8:	
			case DFNT_LUINT8:
				if (dataTypeCodePtr != NULL)
					*dataTypeCodePtr = kIntegerType;
					
				if (numberBytesPtr != NULL)
					*numberBytesPtr = 1;
					
				if (signedDataFlagPtr != NULL)
					{
					*signedDataFlagPtr = TRUE;
					if (numberType == DFNT_UINT8 || numberType == DFNT_LUINT8)
						*signedDataFlagPtr = FALSE;
						
					}	// end "if (signedDataFlagPtr != NULL)"
					
				setSwapBytesFlag = TRUE;
				break;

			case DFNT_INT16:	// 22:
			case DFNT_UINT16:	// 23:
			case DFNT_LINT16:
			case DFNT_LUINT16:
				if (dataTypeCodePtr != NULL)
					*dataTypeCodePtr = kIntegerType;
					
				if (numberBytesPtr != NULL)
					*numberBytesPtr = 2;
					
				if (signedDataFlagPtr != NULL)
					{
					*signedDataFlagPtr = TRUE;
					if (numberType == DFNT_UINT16 || numberType == DFNT_LUINT16)
						*signedDataFlagPtr = FALSE;
						
					}	// end "if (signedDataFlagPtr != NULL)"
					
				setSwapBytesFlag = TRUE;
				break;
				
			case DFNT_FLOAT32:	// 5
			case DFNT_LFLOAT32:
				if (dataTypeCodePtr != NULL)
					*dataTypeCodePtr = kRealType;
					
				if (numberBytesPtr != NULL)
					*numberBytesPtr = 4;
					
				setSwapBytesFlag = TRUE;
				*signedDataFlagPtr = TRUE;
				break;
				
			case DFNT_FLOAT64:	// 6
			case DFNT_LFLOAT64:
				if (dataTypeCodePtr != NULL)
					*dataTypeCodePtr = kRealType;
					
				if (numberBytesPtr != NULL)
					*numberBytesPtr = 8;
					
				setSwapBytesFlag = TRUE;
				*signedDataFlagPtr = TRUE;
				break;
				
			case DFNT_INT32:	// 24
			case DFNT_UINT32:	// 25
			case DFNT_LINT32:
			case DFNT_LUINT32:
				if (dataTypeCodePtr != NULL)
					*dataTypeCodePtr = kIntegerType;
					
				if (numberBytesPtr != NULL)
					*numberBytesPtr = 4;
					
				if (signedDataFlagPtr != NULL)
					{
					*signedDataFlagPtr = TRUE;
					if (numberType == DFNT_UINT32 || numberType == DFNT_LUINT32)
						*signedDataFlagPtr = FALSE;
						
					}	// end "if (signedDataFlagPtr != NULL)"
					
				setSwapBytesFlag = TRUE;
				break;

			default:
				continueFlag = FALSE;
				*returnCodePtr = 6;
				break;

			}	// end "switch (number_type)"
		
		if (setSwapBytesFlag && swapBytesFlagPtr != NULL)
			{
			*swapBytesFlagPtr = !gBigEndianFlag;
			if (numberType & DFNT_LITEND)
				*swapBytesFlagPtr = gBigEndianFlag;
			
			if (*dataCompressionCodePtr != kNoCompression)
						// Byte swapping is done in the decompression algorithms.
				*swapBytesFlagPtr = FALSE;
			
			}	// end "if (setSwapBytesFlag && swapBytesFlagPtr != NULL)"
			
		}	// end "if (hdfReturn != FAIL)"
	
	if (hdfReturn != FAIL && continueFlag)
		{
		dataSetOffset=0;	
		hdfReturn = SDgetoffset (sdid, &dataSetOffset);
		
		if (dataSetOffsetPtr != NULL)
			*dataSetOffsetPtr = dataSetOffset;
		
		if (hdfReturn == FAIL)
			{
			*returnCodePtr = 20;
			hdfReturn = SUCCEED;
			
			}	// end "if (hdfReturn == FAIL)"
			
		}	// end "if (hdfReturn != FAIL)"
		
	if (hdfReturn != FAIL &&  
			*returnCodePtr != 2 && 
				*returnCodePtr != 20 && 
					continueFlag)
		*returnCodePtr = noErr;

	if (SDiscoordvar (sdid) && *returnCodePtr == noErr)
		*returnCodePtr = 7;
		
	*noDataValueFlagPtr = FALSE;
	if (hdfReturn != FAIL)
		*noDataValueFlagPtr = GetFillDataValue (sdid, noDataValuePtr);

	return (hdfReturn);
    
}	// end "GetHDFDataSetInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		intn GetHDFDataSetSpecialInfo
//
//	Software purpose:	This routine checks the input HDF data set to determine
//							if MultiSpec can handle it.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			SetHDFDataSetFileInformation in SOpenDlg.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2001
//	Revised By:			Larry L. Biehl			Date: 01/05/2018

SInt16 GetHDFDataSetSpecialInfo (
				FileInfoPtr		 					fileInfoPtr,
				SInt32								dataSetIndex)
				
{
   sp_info_block_t					info_block;

	CMFileStream*						fileStreamPtr;
	HdfDataSets*						hdfDataSetsPtr;
	
	intn 									hdfReturn;
	
   SInt32 								file_id,
   										sdid;
   										
   SInt16								errCode;
  

	hdfReturn = SUCCEED;
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle,
																		kLock);
									
	if (hdfDataSetsPtr != NULL)
		{
		file_id = fileInfoPtr->hdf4FileID;
		
		if (file_id != FAIL) 
			{
			//sdid = (SInt32)hdfDataSetsPtr[dataSetIndex].sdid;
			sdid = GetSdidValue (&hdfDataSetsPtr[dataSetIndex]);
						
			if (sdid != FAIL)
				{
				info_block.key = 0;
				info_block.path = NULL;
				info_block.cdims = NULL;
				
				#if defined multispec_mac || multispec_win
					SDgetspecialinfo (sdid, &info_block);
					//HDget_special_info (sdid, &info_block);

					if (info_block.cdims != NULL)
						free (info_block.cdims);
				#endif
				
   			errCode = noErr;
				fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
				if (info_block.key == SPECIAL_EXT)
					{
							// Set the new name for the file in the file 
							// information structure.
						
					SetCFileName (fileInfoPtr, (FileStringPtr)info_block.path);

							// Now open this file up.
					
					errCode = OpenFileReadOnly (fileStreamPtr, 
															kResolveAliasChains, 
															kLockFile, 
															kVerifyFileStream);
					
					}	// end "if (info_block.key == SPECIAL_EXT)"
					
				else if (info_block.key == 0)
					{
							// Set the file name to be the same as that for
							// the HDF header information.
						
					SetCFileName (fileInfoPtr, (FileStringPtr)&hdfDataSetsPtr[0].name[1]);

							// No open this file up.
					
					errCode = OpenFileReadOnly (fileStreamPtr, 
															kResolveAliasChains, 
															kLockFile, 
															kVerifyFileStream);
					
					}	// end "else if (info_block.key == 0)"
					
				}	// end "if (sdid != FAIL)"
				
			else	// sdid == FAIL
				hdfReturn = FAIL;
		
			}	// end "if (file_id != FAIL) "
			
		else	// file_id == FAIL
			hdfReturn = FAIL;
			
		}	// end "if (hdfDataSetsPtr != NULL)"
		
	else	// hdfDataSetsPtr == NULL
		hdfReturn = FAIL;
		
	CheckAndUnlockHandle (fileInfoPtr->hdfHandle);

	return (hdfReturn);
    
}	// end "GetHDFDataSetSpecialInfo"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHDFInstrumentCode
//
//	Software purpose:	This routine reads through the attributes to obtain the 
//							instrument code.  This is going to be tough to do.  It will be 
//							based on how different processing centers do the processing.  
//							There is no defined standard on how this information is stored.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	Instrument code.  0 is unknown code.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/01/2005
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 GetHDFInstrumentCode (
				SInt32		 						id)
				
{	
	UCharPtr								bufferPtr;
											
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									numberAttributes,
 	 											numDataSets;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								numberAttributes,
 	 											numDataSets;
	#endif	// TARGET_RT_MAC_MACHO, else...
 	
 	SInt16								instrumentCode,
 											returnCode;
   
   			
	hdfReturn = SUCCEED;
	
			// Initialize output variables.
			
	instrumentCode = 0;

	hdfReturn = SDfileinfo (id, &numDataSets, &numberAttributes);

			// Go through attributes to get information about the band interleave
			// description
	
	if (hdfReturn == SUCCEED)
		{
				// Get the buffer to be used to read the attribute information.
					
		bufferPtr = (UCharPtr)GetAttributeBuffer (id, numberAttributes);
			
		if (bufferPtr != NULL)
			{			
					// Determine if this is ASTER data.

			returnCode = GetSpecificTextAttributeInformation (
																id,
																numberAttributes,
																(UCharPtr)"productmetadata.0",
																(UCharPtr)"ASTERGENERICMETADATA",
																(UCharPtr)"\0",
																kNumValFieldUsed,
																kEqualSignUsed,
																bufferPtr,
																0,
																NULL,
																NULL);
																				
			if (returnCode == 0)
				returnCode = GetSpecificTextAttributeInformation (
																id,
																numberAttributes,
																(UCharPtr)"productmetadata.0",
																(UCharPtr)"ASTERGDSGENERICMETADATA",
																(UCharPtr)"\0",
																kNumValFieldUsed,
																kEqualSignUsed,
																bufferPtr,
																0,
																NULL,
																NULL);
																				
			if (returnCode == 1)
				instrumentCode = kASTER;
				
			}	// end "if (bufferPtr != NULL)"
			
		}	// end "if (hdfReturn == SUCCEED)"

	CheckAndDisposePtr (bufferPtr);

	return (instrumentCode);
    
}	// end "GetHDFInstrumentCode"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHDF4Line
//
//	Software purpose:	This routine reads a line of hdf data.  This routine is called
//							when the data are compressed.  The hdf routines are used to
//							read the data from disk and uncompress the data.  
//
//							I will have to work some to make this more efficient such as 
//							fixing so the hdf file does not have to be opened and closed 
//							with each line read.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/24/2007
//	Revised By:			Larry L. Biehl			Date: 02/21/2012

SInt16 GetHDF4Line (
				FileInfoPtr							fileInfoPtr,
				UInt32								numberChannels,
				UInt32								lineNumber, 
				UInt32								channelNumber, 
				UInt32								columnStart, 
				UInt32								columnEnd, 
				UInt32*								numberSamplesPtr,
				HUCharPtr							readBufferPtr)
				
{	
	HdfDataSets*						hdfDataSetsPtr;
	
	#if TARGET_RT_MAC_MACHO
		int									edge[3],
												start[3];
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								edge[3],
												start[3];
	#endif	// TARGET_RT_MAC_MACHO, else...
			
	SInt32								hdfReturn,
											sdid;
											
	UInt32								dataSetsIndex;
											
	SInt16								groupedNumber;
										
   
	hdfReturn = 1;
		
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle);
															
	if (hdfDataSetsPtr == NULL || fileInfoPtr->channelToHdfDataSetPtr == NULL)
		{
		*numberSamplesPtr = 0;
																							return (1);
		
		}	// end "if (hdfDataSetsPtr == NULL || ..."
	
	dataSetsIndex = fileInfoPtr->channelToHdfDataSetPtr[channelNumber];
	//sdid = (SInt32)hdfDataSetsPtr[dataSetsIndex].sdid;
	sdid = GetSdidValue (&hdfDataSetsPtr[dataSetsIndex]);
	groupedNumber = hdfDataSetsPtr[dataSetsIndex].groupedNumber;
						
	if (sdid != FAIL)
		{
		*numberSamplesPtr = columnEnd - columnStart + 1;
		
				// If this is part of a grouped set of data sets then each data set
				// has only 1 channel.
					
		if (groupedNumber > 0)
			numberChannels = 1;
								
		if (numberChannels == 1)
			{
			start[0] = lineNumber - 1;
			start[1]	= columnStart - 1;
			start[2]	= 0;
			
			edge[0] = 1;
			edge[1] = *numberSamplesPtr;
			edge[2] = 0;
				
			}	// end "if (numberChannels == 1)"
			
		else	// numberChannels > 1
			{
			start[0] = channelNumber;		// channelNumber is 0-based.
			start[1] = lineNumber - 1;
			start[2]	= columnStart - 1;
			
			edge[0] = 1;
			edge[1] = 1;
			edge[2] = *numberSamplesPtr;
				
			}	// end "else numberChannels > 1"
		/*		
		sprintf ((char*)&gTextString,
					"Before SDreaddata. hdfReturn = %ld  %ld.%s",
					hdfReturn,
					start[0],
					gEndOfLine);
		ListString ((char*)&gTextString, strlen ((char*)&gTextString), gOutputTextH);
		*/
		hdfReturn = SDreaddata (sdid, start, NULL, edge, (void*)readBufferPtr);
		/*		
		sprintf ((char*)&gTextString,
					"After SDreaddata. hdfReturn = %ld  %ld.%s",
					hdfReturn,
					start[0],
					gEndOfLine);
		ListString ((char*)&gTextString, strlen ((char*)&gTextString), gOutputTextH);
		*/	
		}	// end "if (sdid != FAIL)"
	
	return ((SInt16)hdfReturn);
							
}	// end "GetHDF4Line"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		intn GetHDFProjectionInformation
//
//	Software purpose:	This routine reads through the attributes to obtain information
//							about the map projection information.  It currently has been
//							"tuned" for that for ASTER image data.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	= FALSE, MultiSpec cannot handle the input data set.
//							= TRUE, MultiSpec can handle the data set.
//
// Called By:			LoadHDFDataSetInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/28/2005
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

intn GetHDFProjectionInformation (
				SInt32		 						id,
				UCharPtr								suffix,
				SInt16								format,
				FileInfoPtr							fileInfoPtr,
				SInt16								groupNumber,
				SInt16								numberInGroup,
				SInt32*								returnCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								mapOrientationAnglePtr,
				SInt16*								mapReferenceSystemCodePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								gridZonePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr,
				double*								lowerRightPixelCoordinatesPtr,
				double*								spatialResolutionPtr,
				double*								upperLeftImagePtr,
				Boolean*								convertCenterFromLatLongToMapFlagPtr,
				Boolean*								convertUpperLeftFromLatLongToMapFlagPtr,
				Boolean*								convertLowerLeftFromLatLongToMapFlagPtr)
				
{	
	UInt8									attributeName[80],
											groupString[80],
											localSuffix[8],
											objectString[80],
											projectionEllipsoid[80],
											projectionMethodString[80],
											projectionDatum[80],
											unitString[80];
									
	UCharPtr								bufferPtr;
											
	double								centerLatLong[2],
											coordinates[2],
											mapOrientationAngle,
											noDataValue = 0,
											pixelSize[2],
											upperLeftPixelCoordinates[2],
											utmZoneCode;
	
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									numberAttributes,
 	 											numDataSets;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								numberAttributes,
 	 											numDataSets;
	#endif	// TARGET_RT_MAC_MACHO, else...
 	 										
 	UInt32								index;
 	
 	SInt16								returnCode,
 											suffixNumber;
 											
 	Boolean								noDataValueExistsFlag = FALSE,
											spatialResolutionLoadedFlag;
   
   			
	hdfReturn = SUCCEED;
	spatialResolutionLoadedFlag = FALSE;
	
			// If anything is loaded then set contents of returnCodePtr to noErr.
			
	*returnCodePtr = 1;
	
			// Initialize output variables.
			
	*xMapCoordinate11Ptr = 0;
	*yMapCoordinate11Ptr = 0;
	*mapOrientationAnglePtr = 0;
	*mapReferenceSystemCodePtr = kNoRSDefinedCode;
	*mapProjectionCodePtr = kNotDefinedCode;
	*gridZonePtr = 0;
	*mapProjectionEllipsoidCodePtr = 0;
	*mapProjectionDatumCodePtr = 0;
	*mapUnitsCodePtr = 0;
	*horizontalPixelSizePtr = 0;
	*verticalPixelSizePtr = 0;
	upperLeftImagePtr[0] = 0;
	upperLeftImagePtr[1] = 0;
	*convertCenterFromLatLongToMapFlagPtr = FALSE;
	*convertLowerLeftFromLatLongToMapFlagPtr = FALSE;
	*convertUpperLeftFromLatLongToMapFlagPtr = FALSE;
	
	for (index=0; index<13; index++)
		projectionParametersPtr[index] = 0;
	
	//rank = 10;	// Indicate what the max length of dims is.		
	//hdfReturn = SDgetinfo (sdid, name, &rank, dims, &numberType, &nattrs);

	hdfReturn = SDfileinfo (id, &numDataSets, &numberAttributes);

			// Go through attributes to get information about the band interleave
			// description
	
	if (hdfReturn == SUCCEED)
		{
				// Get the buffer to be used to read the attribute information.
					
		bufferPtr = GetAttributeBuffer (id, numberAttributes);
			
		if (bufferPtr != NULL)
			{
					// Get the spatial resolution of the data.  This will be for Level 1 data.
			
			returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEINFORMATION",
																		(UCharPtr)"SPATIALRESOLUTION",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		3,
																		spatialResolutionPtr);
																					
			if (returnCode == 3)
				{
				spatialResolutionLoadedFlag = TRUE;
				*returnCodePtr = noErr;
				
				}	// end "if (returnCode == 3)"
			
					// Get the coordinates of the upper left corner pixel.
			
			returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEFOURCORNERS",
																		(UCharPtr)"UPPERLEFT",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		2,
																		upperLeftPixelCoordinates);
																					
			if (returnCode == 2)
				{
				*xMapCoordinate11Ptr = upperLeftPixelCoordinates[1];
				*yMapCoordinate11Ptr = upperLeftPixelCoordinates[0];
				*returnCodePtr = noErr;
				
				}	// end "if (returnCode == 2)"
			
					// Get the coordinates of the upper right corner pixel in case it is 
					// needed later.
			
			returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEFOURCORNERS",
																		(UCharPtr)"UPPERRIGHT",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		2,
																		coordinates);
																					
			if (returnCode == 2)
				lowerRightPixelCoordinatesPtr[1] = coordinates[1];
			
					// Get the coordinates of the lower left corner pixel in case it is 
					// needed later.
			
			returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEFOURCORNERS",
																		(UCharPtr)"LOWERLEFT",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		2,
																		coordinates);
																					
			if (returnCode == 2)
				lowerRightPixelCoordinatesPtr[0] = coordinates[0];
			
					// Get the map orientation angle.
			
			returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEINFORMATION",
																		(UCharPtr)"MAPORIENTATIONANGLE",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		1,
																		&mapOrientationAngle);
																					
			if (returnCode == 1)
				{
				*mapOrientationAnglePtr = mapOrientationAngle * kDegreesToRadians;
				*returnCodePtr = noErr;
				
				}	// end "if (returnCode == 1)"	
				
					// Set up to get the attribute name (at least for ASTER data).
					
			localSuffix[0] = suffix[0];
			localSuffix[1] = suffix[1];
			localSuffix[2] = suffix[2];
			
					// Handle 3B and 3N suffix case to be ready to convert to a number.
					
			if (localSuffix[1] == 'B' || localSuffix[1] == 'N')
				localSuffix[1] = 0;
			
					// Now get the atribute name.  Also set up the pixel size in case this
					// is Level 1 data.
			
			suffixNumber = 0;
			sscanf ((char*)localSuffix, "%hd", &suffixNumber);
			switch (suffixNumber)
				{
				case 1:
				case 2:
				case 3:
					sprintf ((char*)attributeName, "productmetadata.v");
					pixelSize[0] = spatialResolutionPtr[0];
					break;
					
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
					sprintf ((char*)attributeName, "productmetadata.s");
					pixelSize[0] = spatialResolutionPtr[1];
					break;
					
					
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					sprintf ((char*)attributeName, "productmetadata.t");
					pixelSize[0] = spatialResolutionPtr[2];
					break;
					
				default:
					pixelSize[0] = 0;
					spatialResolutionLoadedFlag = FALSE;
					break;
				
				}	// end "switch (suffix)"
			
					// Load the pixel size if available.
					
			if (spatialResolutionLoadedFlag)
				{
				*horizontalPixelSizePtr = pixelSize[0];
				*verticalPixelSizePtr = pixelSize[0];
				
				}	// end "if (spatialResolutionLoadedFlag)"
				
					// Now get the proper group name (at least for ASTER data).
					
			sprintf ((char*)groupString, "PROCESSINGPARAMETERS%s", suffix);
			
					// Get the ellipsoid and datum.
					
			sprintf ((char*)objectString, "ELLIPSOIDANDDATUM%s", suffix);
			returnCode = GetSpecificTextAttributeInformation (id,
																				numberAttributes,
																				attributeName,
																				groupString,
																				objectString,
																				kNumValFieldUsed,
																				kEqualSignUsed,
											 									bufferPtr,
											 									2,
																				projectionEllipsoid,
																				projectionDatum);
			if (returnCode > 0)
				{
				if (CompareStringsNoCase (projectionEllipsoid, (UCharPtr)"WGS84", 5) == 0)
					{
					*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
					*returnCodePtr = noErr;
						
					}	// end "if (strcmp (projectionEllipsoid, "WGS84") == 0)"
					
				if (CompareStringsNoCase (projectionDatum, (UCharPtr)"WGS84", 5) == 0)
					{
					*mapProjectionDatumCodePtr = kWGS84Code;
					*returnCodePtr = noErr;
						
					}	// end "if (strcmp (projectionDatum, "WGS84") == 0)"
				
				}	// end "if (returnCode > 0)"
			
					// Get the map projection method.

			sprintf ((char*)objectString, "MPMETHOD%s", suffix);
			returnCode = GetSpecificTextAttributeInformation (id,
																				numberAttributes,
																				attributeName,
																				groupString,
																				objectString,
																				kNumValFieldUsed,
																				kEqualSignUsed,
											 									bufferPtr,
											 									1,
																				projectionMethodString,
																				NULL);
			if (returnCode > 0)
				{
				if (CompareStringsNoCase (projectionMethodString, (UCharPtr)"UTM", 3) == 0)
					{
					*mapReferenceSystemCodePtr = kUTM_WGS84RSCode;
					*returnCodePtr = noErr;
						
					}	// end "if (strcmp (projectionMethodString, "UTM") == 0)"
				
				}	// end "if (returnCode > 0)"
			
					// Get the utm zone.
			
			sprintf ((char*)objectString, "PROJECTIONPARAMETERS%s", suffix);
			returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				attributeName,
																				groupString,
																				objectString,
																				kNumValFieldUsed,
																				kEqualSignUsed,
																				bufferPtr,
																				4,
																				projectionParametersPtr);
																					
			if (returnCode == 4)
				{
				if (projectionParametersPtr[0] == 6378137 && 
															projectionParametersPtr[1] == 6356752.3)
					{
					*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
					*mapProjectionDatumCodePtr = kWGS84Code;
					
					}	// end "if (projectionParametersPtr[0] == 6378137 && ..."
					
				*returnCodePtr = noErr;
				
				}	// end "if (returnCode == 4)"	
			
					// Get the utm zone.
			
			sprintf ((char*)objectString, "UTMZONECODE%s", suffix);
			returnCode = GetSpecificNumericAttributeInformation (id,
																					numberAttributes,
																					attributeName,
																					groupString,
																					objectString,
																					kNumValFieldUsed,
																					kEqualSignUsed,
												 									bufferPtr,
																					1,
																					&utmZoneCode);
																					
			if (returnCode == 1)
				{
				*gridZonePtr = (SInt16)utmZoneCode;
				if (*yMapCoordinate11Ptr < 0)
					*returnCodePtr = noErr;
				
				}	// end "if (returnCode == 1)"	
			
					// Get the pixel spacing units
			
			sprintf ((char*)objectString, "UNIT%s", suffix);
			returnCode = GetSpecificTextAttributeInformation (id,
																				numberAttributes,
																				attributeName,
																				groupString,
																				objectString,
																				kNumValFieldUsed,
																				kEqualSignUsed,
											 									bufferPtr,
											 									1,
																				unitString,
																				NULL);
																					
			if (returnCode > 0)
				{
				if (unitString[0] == 'M')
					{
					*mapUnitsCodePtr = kMetersCode;
					*returnCodePtr = noErr;
						
					}	// end "if (unitString[0] == "M")"
				
				}	// end "if (returnCode > 0)"	
			
					// Get the pixel spacing.
			
			sprintf ((char*)objectString, "PIXELSPACING%s", suffix);
			returnCode = GetSpecificNumericAttributeInformation (id,
																					numberAttributes,
																					attributeName,
																					groupString,
																					objectString,
																					kNumValFieldUsed,
																					kEqualSignUsed,
												 									bufferPtr,
																					2,
																					pixelSize);
																					
			if (returnCode == 2)
				{
				*horizontalPixelSizePtr = pixelSize[0];
				*verticalPixelSizePtr = pixelSize[1];
				*returnCodePtr = noErr;
				
				}	// end "if (returnCode == 2)"	
				
			if (*mapProjectionCodePtr == 0)
				{
						// Try a different attribute name. The following is for Level 1 data.
						
				sprintf ((char*)attributeName, "level_1_carryover");
				sprintf ((char*)groupString, "MASTERGROUP");
						
						// Get the ellipsoid and datum.
						
				sprintf ((char*)objectString, "ELLIPSOIDANDDATUM%s", suffix);
				returnCode = GetSpecificTextAttributeInformation (id,
																					numberAttributes,
																					attributeName,
																					groupString,
																					objectString,
																					kNumValFieldUsed,
																					kEqualSignUsed,
												 									bufferPtr,
												 									2,
																					projectionEllipsoid,
																					projectionDatum);
				if (returnCode > 0)
					{
					if (CompareStringsNoCase (
												projectionEllipsoid, (UCharPtr)"WGS84", 5) == 0)
						{
						*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
						*returnCodePtr = noErr;
							
						}	// end "if (strcmp (projectionEllipsoid, "WGS84") == 0)"
						
					if (CompareStringsNoCase (projectionDatum, (UCharPtr)"WGS84", 5) == 0)
						{
						*mapProjectionDatumCodePtr = kWGS84Code;
						*returnCodePtr = noErr;
							
						}	// end "if (strcmp (projectionDatum, "WGS84") == 0)"
					
					}	// end "if (returnCode > 0)"
				
						// Get the map projection method.

				sprintf ((char*)objectString, "MPMETHOD%s", suffix);
				returnCode = GetSpecificTextAttributeInformation (id,
																					numberAttributes,
																					attributeName,
																					groupString,
																					objectString,
																					kNumValFieldUsed,
																					kEqualSignUsed,
												 									bufferPtr,
												 									1,
																					projectionMethodString,
																					NULL);
				if (returnCode > 0)
					{
					if (CompareStringsNoCase (projectionMethodString, (UCharPtr)"UTM", 3) == 0)
						{
						*mapReferenceSystemCodePtr = kUTM_WGS84RSCode;
						*returnCodePtr = noErr;
							
						}	// end "if (strcmp (projectionMethodString, "UTM") == 0)"
					
					}	// end "if (returnCode > 0)"
				
						// Get the utm zone.
				
				sprintf ((char*)objectString, "PROJECTIONPARAMETERS%s", suffix);
				returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				attributeName,
																				groupString,
																				objectString,
																				kNumValFieldUsed,
																				kEqualSignUsed,
																				bufferPtr,
																				4,
																				projectionParametersPtr);
																						
				if (returnCode == 4)
					{
					if (projectionParametersPtr[0] == 6378137 && 
															projectionParametersPtr[1] == 6356752.3)
						{
						*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
						*mapProjectionDatumCodePtr = kWGS84Code;
						
						}	// end "if (projectionParametersPtr[0] == 6378137 && ..."
						
					*returnCodePtr = noErr;
					
					}	// end "if (returnCode == 4)"	
				
						// Get the utm zone.
				
				sprintf ((char*)objectString, "UTMZONECODE%s", suffix);
				returnCode = GetSpecificNumericAttributeInformation (id,
																						numberAttributes,
																						attributeName,
																						groupString,
																						objectString,
																						kNumValFieldUsed,
																						kEqualSignUsed,
													 									bufferPtr,
																						1,
																						&utmZoneCode);
																						
				if (returnCode == 1)
					{
					*gridZonePtr = (SInt16)utmZoneCode;
					*returnCodePtr = noErr;
					
					}	// end "if (returnCode == 1)"	
				
						// Get the pixel spacing units
				
				sprintf ((char*)objectString, "UNIT%s", suffix);
				returnCode = GetSpecificTextAttributeInformation (id,
																					numberAttributes,
																					attributeName,
																					groupString,
																					objectString,
																					kNumValFieldUsed,
																					kEqualSignUsed,
												 									bufferPtr,
												 									1,
																					unitString,
																					NULL);
																						
				if (returnCode > 0)
					{
					if (unitString[0] == 'M')
						{
						*mapUnitsCodePtr = kMetersCode;
						*returnCodePtr = noErr;
							
						}	// end "if (unitString[0] == "M")"
					
					}	// end "if (returnCode > 0)"	
				
						// Get the pixel spacing.
				
				sprintf ((char*)objectString, "PIXELSPACING%s", suffix);
				returnCode = GetSpecificNumericAttributeInformation (id,
																						numberAttributes,
																						attributeName,
																						groupString,
																						objectString,
																						kNumValFieldUsed,
																						kEqualSignUsed,
													 									bufferPtr,
																						2,
																						pixelSize);
																						
				if (returnCode == 2)
					{
					*horizontalPixelSizePtr = pixelSize[0];
					*verticalPixelSizePtr = pixelSize[1];
					*returnCodePtr = noErr;
					
					}	// end "if (returnCode == 2)"	
					
				}	// end "if (*mapProjectionCodePtr == 0)"
				
			if (*mapProjectionCodePtr == 0)
				{
						// Try a different attribute name. The following is for Level 2 data.
						
				sprintf ((char*)attributeName, "coremetadata.0");
				sprintf ((char*)groupString, "INVENTORYMETADATA");
				
						// Get the map projection name.

				sprintf ((char*)objectString, "MAPPROJECTIONNAME");
				returnCode = GetSpecificTextAttributeInformation (id,
																					numberAttributes,
																					attributeName,
																					groupString,
																					objectString,
																					kNumValFieldUsed,
																					kEqualSignUsed,
												 									bufferPtr,
												 									1,
																					projectionMethodString,
																					NULL);
				if (returnCode > 0)
					{
							// Note that the entire string is "Universal Transverse Mercator. But
							// only the first full word is found.  This check may need to be 
							// fixed in the future.
							
					if (CompareStringsNoCase (
										projectionMethodString, (UCharPtr)"Universal", 9) == 0)
						{
						*mapReferenceSystemCodePtr = kUTM_WGS84RSCode;
						*returnCodePtr = noErr;
						
						if (*gridZonePtr == 0)
							{
									// Get the grid zone based on the center latitude and 
									// longitude.
									
							sprintf ((char*)attributeName, "productmetadata.0");
							sprintf ((char*)groupString, "ASTERGENERICMETADATA");
							sprintf ((char*)objectString, "SCENECENTER");
							returnCode = GetSpecificNumericAttributeInformation (
																						id,
																						numberAttributes,
																						attributeName,
																						groupString,
																						objectString,
																						kNumValFieldUsed,
																						kEqualSignUsed,
													 									bufferPtr,
																						2,
																						centerLatLong);
																						
							if (returnCode == 2)
								{
								if (centerLatLong[1] >= -180 && centerLatLong[1] <= 180)
									*gridZonePtr = (SInt16)((centerLatLong[1]+180)/6) + 1;
								
								}	// end "if (returnCode == 2)"
							
							}	// end "if (*gridZonePtr == 0)"
							
						}	// end "if (strcmp (projectionMethodString, "Universal ...)"
						
					}	// end "if (returnCode > 0)"
					
				}	// end "if (*mapProjectionCodePtr == 0)"
				
			if (*mapProjectionCodePtr == 0 && *mapReferenceSystemCodePtr == 0)
				{
						// Try a different attribute name. The following is again for Level 
						// 1 data in the NASA data pool.
						// Get the spatial resolution of the data.  
				
				returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				(UCharPtr)"coremetadata",
																				(UCharPtr)"RESOLUTION",
																				(UCharPtr)"XPIXELSIZE",
																				kNumValFieldUsed,
																				kEqualSignUsed,
																				bufferPtr,
																				1,
																				&pixelSize[0]);
																						
				if (returnCode == 1)
					{
					*horizontalPixelSizePtr = pixelSize[0];
					*returnCodePtr = noErr;
					
					}	// end "if (returnCode == 1)"
				
				returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				(UCharPtr)"coremetadata",
																				(UCharPtr)"RESOLUTION",
																				(UCharPtr)"YPIXELSIZE",
																				kNumValFieldUsed,
																				kEqualSignUsed,
																				bufferPtr,
																				1,
																				&pixelSize[1]);
																						
				if (returnCode == 1)
					{
					*verticalPixelSizePtr = pixelSize[1];
					*returnCodePtr = noErr;
					
					}	// end "if (returnCode == 1)"
			
						// Get the pixel spacing units
				
				returnCode = GetSpecificTextAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"coremetadata",
																			(UCharPtr)"RESOLUTION",
																			(UCharPtr)"XYPIXELSIZEUNIT",
																			kNumValFieldUsed,
																			kEqualSignUsed,
																			bufferPtr,
																			1,
																			unitString,
																			NULL);
																						
				if (returnCode > 0)
					{
					if (unitString[0] == 'M')
						{
						*mapUnitsCodePtr = kMetersCode;
						*returnCodePtr = noErr;
							
						}	// end "if (unitString[0] == "M")"
					
					}	// end "if (returnCode > 0)"	
				
						// Get the coordinates of the upper left corner pixel.
				
				returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"coremetadata",
																		(UCharPtr)"GRIDSTRUCTUREINFO",
																		(UCharPtr)"UPPERLEFTCORNER",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		2,
																		upperLeftPixelCoordinates);
																						
				if (returnCode == 2)
					{
					*xMapCoordinate11Ptr = upperLeftPixelCoordinates[0];
					*yMapCoordinate11Ptr = upperLeftPixelCoordinates[1];
					*returnCodePtr = noErr;
					
					}	// end "if (returnCode == 2)"
			
						// Get the coordinates of the upper right corner pixel in case it 
						// is needed later.
				
				returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEFOURCORNERS",
																		(UCharPtr)"UPPERRIGHT",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		2,
																		coordinates);
																						
				if (returnCode == 2)
					lowerRightPixelCoordinatesPtr[0] = coordinates[0];
				
						// Get the coordinates of the lower left corner pixel in case it is 
						// needed later.
				
				returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"productmetadata.0",
																		(UCharPtr)"SCENEFOURCORNERS",
																		(UCharPtr)"LOWERLEFT",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		2,
																		coordinates);
																						
				if (returnCode == 2)
					lowerRightPixelCoordinatesPtr[1] = coordinates[1];
				
						// Get the map projection name.

				returnCode = GetSpecificTextAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"coremetadata",
																		(UCharPtr)"PROJECTION_INFO",
																		(UCharPtr)"PROJECTION",
																		kNumValFieldUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		1,
																		projectionMethodString,
																		NULL);
				if (returnCode > 0)
					{
							// Note that the entire string is "Universal Transverse Mercator. But
							// only the first full word is found.  This check may need to be 
							// fixed in the future.
							
					if (CompareStringsNoCase (
										projectionMethodString, (UCharPtr)"Universal", 9) == 0)
						{
						*mapReferenceSystemCodePtr = kUTM_WGS84RSCode;
						*returnCodePtr = noErr;
						
						if (*gridZonePtr == 0)
							{
									// Get the grid zone.
									
							returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"coremetadata",
																			(UCharPtr)"PROJECTION_INFO",
																			(UCharPtr)"UTMZONE",
																			kNumValFieldUsed,
																			kEqualSignUsed,
																			bufferPtr,
																			1,
																			&utmZoneCode);
																						
							if (returnCode == 1)
								*gridZonePtr = (SInt16)utmZoneCode;
							
							}	// end "if (*gridZonePtr == 0)"
							
						}	// end "if (strcmp (projectionMethodString, "Universal") == 0)"
					
					}	// end "if (returnCode > 0)"
			
						// Get the datum (and ellipsoid).
						
				returnCode = GetSpecificTextAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"coremetadata",
																			(UCharPtr)"PROJECTION_INFO",
																			(UCharPtr)"DATUM",
																			kNumValFieldUsed,
																			kEqualSignUsed,
																			bufferPtr,
																			1,
																			projectionDatum,
																			NULL);
				if (returnCode > 0)
					{
					if (CompareStringsNoCase (projectionDatum, (UCharPtr)"WGS", 3) == 0)
						{
						*mapProjectionDatumCodePtr = kWGS84Code;
						*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
						*returnCodePtr = noErr;
							
						}	// end "if (strcmp (projectionDatum, "WGS 1984") == 0)"
					
					}	// end "if (returnCode > 0)"
					
				}	// end "if (*mapProjectionCodePtr == 0)"
				
			if (*mapProjectionCodePtr == 0)
				GetMapInfoFromGridStructure (id,
														numberAttributes,
														bufferPtr,
														returnCodePtr,
														xMapCoordinate11Ptr,
														yMapCoordinate11Ptr,
														mapOrientationAnglePtr,
														mapReferenceSystemCodePtr,
														mapProjectionCodePtr,
														gridZonePtr,
														mapProjectionEllipsoidCodePtr,
														mapProjectionDatumCodePtr,
														mapUnitsCodePtr,
														horizontalPixelSizePtr,
														verticalPixelSizePtr,
														projectionParametersPtr);
				
			if (*mapProjectionCodePtr == 0)
				GetMapInfoFromSeaSpaceStructure (id,
															numberAttributes,
															fileInfoPtr,
															format,
															bufferPtr,
															groupNumber,
															numberInGroup,
															returnCodePtr,
															xMapCoordinate11Ptr,
															yMapCoordinate11Ptr,
															mapOrientationAnglePtr,
															mapProjectionCodePtr,
															gridZonePtr,
															mapProjectionEllipsoidCodePtr,
															mapProjectionDatumCodePtr,
															mapUnitsCodePtr,
															horizontalPixelSizePtr,
															verticalPixelSizePtr,
															projectionParametersPtr,
															upperLeftImagePtr,
															convertUpperLeftFromLatLongToMapFlagPtr);
				
			if (*mapProjectionCodePtr == 0 && format == kNETCDFType)
				{
				GetMapInfoFromNetCDF_HRLDAS_Structure (
														id,
														numberAttributes,
														bufferPtr,
														returnCodePtr,
														fileInfoPtr->numberLines,
														xMapCoordinate11Ptr,
														yMapCoordinate11Ptr,
														mapProjectionCodePtr,
														mapProjectionEllipsoidCodePtr,
														mapProjectionDatumCodePtr,
														mapUnitsCodePtr,
														horizontalPixelSizePtr,
														verticalPixelSizePtr,
														projectionParametersPtr,
														convertCenterFromLatLongToMapFlagPtr,
														convertLowerLeftFromLatLongToMapFlagPtr);
				
				if (*mapProjectionCodePtr == 0)
					GetMapInfoFromNetCDF_LIS_Structure (
														id,
														numberAttributes,
														bufferPtr,
														returnCodePtr,
														fileInfoPtr->numberLines,
														&noDataValue,
														&noDataValueExistsFlag,
														xMapCoordinate11Ptr,
														yMapCoordinate11Ptr,
														mapProjectionCodePtr,
														mapProjectionEllipsoidCodePtr,
														mapProjectionDatumCodePtr,
														mapUnitsCodePtr,
														horizontalPixelSizePtr,
														verticalPixelSizePtr,
														projectionParametersPtr,
														convertLowerLeftFromLatLongToMapFlagPtr);
														
				fileInfoPtr->noDataValue = noDataValue;
				fileInfoPtr->noDataValueFlag = noDataValueExistsFlag;
				/*
				if (*mapProjectionCodePtr != 0)
					fileInfoPtr->treatLinesAsBottomToTopFlag = TRUE;
					
				else	// *mapProjectionCodePtr == 0
					{
							// Determine if this file was created from a grib file. If so
							// the lines will need to be flipped to to bottom.
							
					if (GetSpecificTextAttributeInformation (id,
																			numberAttributes,
																			"grib_source",
																			"",
																			"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
																			bufferPtr,
																			0,
																			NULL,
																			NULL) > 0)
						fileInfoPtr->treatLinesAsBottomToTopFlag = TRUE;
						
							// Determine if this file was created with a CCSM model. If so
							// the lines will need to be flipped to to bottom.
							
					else if (GetSpecificTextAttributeInformation (id,
																					numberAttributes,
																					"CCSM_source",
																					"",
																					"",
																					kNumValFieldNotUsed,
																					kEqualSignNotUsed,
																					bufferPtr,
																					0,
																					NULL,
																					NULL) > 0)
						fileInfoPtr->treatLinesAsBottomToTopFlag = TRUE;
						
					}	// end "else *mapProjectionCodePtr == 0"
				*/														
				}	// end "if (*mapProjectionCodePtr == 0 && format == kNETCDFType)"
				
			if (*mapReferenceSystemCodePtr == kNoRSDefinedCode && 
															*mapProjectionCodePtr > kNotDefinedCode)
				*mapReferenceSystemCodePtr = kUserDefinedRSCode;
				
					// Set grid zone for southern hemisphere if needed.
						
			if (*gridZonePtr != 0 && *yMapCoordinate11Ptr < 0)
				*gridZonePtr = -abs (*gridZonePtr);
			
			if (fileInfoPtr->instrumentCode == kASTER && 
												*mapReferenceSystemCodePtr == kUTM_WGS84RSCode)
				{
				*convertUpperLeftFromLatLongToMapFlagPtr = TRUE;
				 
				if (*mapProjectionDatumCodePtr == 0 || 
															*mapProjectionEllipsoidCodePtr == 0)
					{
							// Ellipsoid and datum are always WGS84 for ASTER level 1b and 
							// level 2 data.
							// May have to make a check for level sometime down the road.
						
					*mapProjectionDatumCodePtr = kWGS84Code;
					*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
					
					}	// end "if (*mapProjectionDatumCodePtr == 0 || ..."
				
				}	// end "if (...->instrumentCode == kASTER &&"

			CheckAndDisposePtr (bufferPtr);
				
			}	// end "if (bufferPtr != NULL)"
			
		}	// end "if (hdfReturn == SUCCEED)"

	return (hdfReturn);
    
}	// end "GetHDFProjectionInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMapInfoFromGridStructure
//
//	Software purpose:	This routine reads the map information that is stored in what is
//							called a Grid Structure in the metadata.
//
//	Parameters in:			
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/12/2006
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetMapInfoFromGridStructure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				UCharPtr								bufferPtr,
				SInt32*								returnCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								mapOrientationAnglePtr,
				SInt16*								mapReferenceSystemCodePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								gridZonePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr)
				
{	
	UInt8									projectionMethodString[80];
											
	double								degrees,
											lowerRightPixelCoordinates[2],
											minutes,
											packedMinutesSeconds,
											seconds,
											sphereCode,
											upperLeftPixelCoordinates[2],
											xDim,
											yDim;
											
	SInt16								returnCode;
	
	Boolean								continueFlag;
	
		
			// Get the coordinates of the upper left corner pixel.
	
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"StructMetadata.0",
																	(UCharPtr)"GridStructure",
																	(UCharPtr)"UpperLeftPointMtrs",
																	kNumValFieldNotUsed,
																	kEqualSignUsed,
																	bufferPtr,
																	2,
																	upperLeftPixelCoordinates);
																			
	if (returnCode == 2)
		{
		*xMapCoordinate11Ptr = upperLeftPixelCoordinates[0];
		*yMapCoordinate11Ptr = upperLeftPixelCoordinates[1];
		*returnCodePtr = noErr;
		
				// Also get the lower right coordinates to be able to compute the
				// spatial resolution.
	
		continueFlag = TRUE;
		returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"StructMetadata.0",
																	(UCharPtr)"GridStructure",
																	(UCharPtr)"LowerRightMtrs",
																	kNumValFieldNotUsed,
																	kEqualSignUsed,
																	bufferPtr,
																	2,
																	lowerRightPixelCoordinates);									
		if (returnCode != 2)
			continueFlag = FALSE;
		
		if (continueFlag)
			{
			returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"StructMetadata.0",
																	(UCharPtr)"GridStructure",
																	(UCharPtr)"XDim",
																	kNumValFieldNotUsed,
																	kEqualSignUsed,
																	bufferPtr,
																	1,
																	&xDim);
			
			if (returnCode != 1)
				continueFlag = FALSE;
			
			}	// end "if (continueFlag)"
		
		if (continueFlag)
			{
			returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"StructMetadata.0",
																	(UCharPtr)"GridStructure",
																	(UCharPtr)"YDim",
																	kNumValFieldNotUsed,
																	kEqualSignUsed,
																	bufferPtr,
																	1,
																	&yDim);
			
			if (returnCode != 1)
				continueFlag = FALSE;
			
			}	// end "if (continueFlag)"
			
		if (continueFlag)
			{
			*horizontalPixelSizePtr = 
					(lowerRightPixelCoordinates[0] - upperLeftPixelCoordinates[0])/xDim;
							
			*verticalPixelSizePtr = 
					(upperLeftPixelCoordinates[1] - lowerRightPixelCoordinates[1])/yDim;
							
					// Now adjust the upper left map coordinate to represent the center 
					// of the pixel not the outside edge of the pixel.
					
			*xMapCoordinate11Ptr += *horizontalPixelSizePtr/2;
			*yMapCoordinate11Ptr -= *verticalPixelSizePtr/2;
			
			}	// end "if (continueFlag)"
			
		}	// end "if (returnCode == 2)"
	
			// Get the map projection name.

	returnCode = GetSpecificTextAttributeInformation (id,
																		numberAttributes,
																		(UCharPtr)"StructMetadata.0",
																		(UCharPtr)"GridStructure",
																		(UCharPtr)"Projection",
																		kNumValFieldNotUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		1,
																		projectionMethodString,
																		NULL);
	if (returnCode > 0)
		{
				// Check for the GCTP codes.
				
		if (CompareStringsNoCase (
									projectionMethodString, (UCharPtr)"GCTP_GEO", 8) == 0)
			{
			*mapReferenceSystemCodePtr = kGeographicCode;
			*mapUnitsCodePtr = kDecimalDegreesCode;
			*returnCodePtr = noErr;
				
			}	// end "if (strcmp (projectionMethodString, "GCTP_GEO") == 0)"
				
		else if (CompareStringsNoCase (
									projectionMethodString, (UCharPtr)"GCTP_UTM", 8) == 0)
			{
			*mapReferenceSystemCodePtr = kUTM_WGS84RSCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "GCTP_UTM") == 0)"
				
		else if (CompareStringsNoCase (
								projectionMethodString, (UCharPtr)"GCTP_ALBERS", 11) == 0)
			{
			*mapProjectionCodePtr = kAlbersConicalEqualAreaCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "GCTP_ALBERS") == 0)"
				
		else if (CompareStringsNoCase (
								projectionMethodString, (UCharPtr)"GCTP_LAMCC", 10) == 0)
			{
			*mapProjectionCodePtr = kLambertConformalConicCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "GCTP_LAMCC") == 0)"
				
		else if (CompareStringsNoCase (
								projectionMethodString, (UCharPtr)"GCTP_CEA", 8) == 0)
			{
			*mapProjectionCodePtr = kCylindricalEqualAreaCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "GCTP_CEA") == 0)"
				
		else if (CompareStringsNoCase (
								projectionMethodString, (UCharPtr)"GCTP_SNSOID", 11) == 0)
			{
			*mapProjectionCodePtr = kSinusoidalCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "GCTP_SNSOID") == 0)"
				
		else if (CompareStringsNoCase (
								projectionMethodString, (UCharPtr)"GCTP_PS", 7) == 0)
			{
			*mapProjectionCodePtr = kPolarStereographicCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "GCTP_PS") == 0)"
		
		}	// end "if (returnCode > 0)"

			// Get the datum (and ellipsoid).
			
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"StructMetadata.0",
																	(UCharPtr)"GridStructure",
																	(UCharPtr)"SphereCode",
																	kNumValFieldNotUsed,
																	kEqualSignUsed,
																	bufferPtr,
																	1,
																	&sphereCode);
	if (returnCode > 0)
		{
		if (sphereCode == -1)
			{
			*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
			*mapProjectionDatumCodePtr = kSphereDatumCode;
			*returnCodePtr = noErr;
				
			}	// end "if (sphereCode == -1)"
		
		}	// end "if (returnCode > 0)"
	
			// Get the projection parameters.

	if (*mapProjectionCodePtr != 0 && *mapProjectionEllipsoidCodePtr != 0)
		{
		returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"StructMetadata.0",
																		(UCharPtr)"GridStructure",
																		(UCharPtr)"ProjParams",
																		kNumValFieldNotUsed,
																		kEqualSignUsed,
																		bufferPtr,
																		13,
																		projectionParametersPtr);
																		
		if (returnCode >= 6 && (*mapProjectionCodePtr == kCylindricalEqualAreaCode ||
												*mapProjectionCodePtr == kPolarStereographicCode))
			{
					// Convert parameter 6 from packed decimal degree to decimal degrees
					
			degrees = (double)((SInt32)(projectionParametersPtr[5]/1000000));
			packedMinutesSeconds = fmod (projectionParametersPtr[5], 1000000);
			minutes = (double)((SInt32)(packedMinutesSeconds/1000));
			seconds = fmod (packedMinutesSeconds, 1000);
			projectionParametersPtr[5] = degrees + minutes/60 + seconds/3600;
			
					// Move standard latitude1 to the position in the parameter vector
					// that it is expected to be in later.
					
			projectionParametersPtr[8] = projectionParametersPtr[5];
					
			}	// end "if (returnCode >= 6 && (*mapProjectionCodePtr == ..."
																		
		if (returnCode >= 6 && *mapProjectionCodePtr == kPolarStereographicCode)
			{
					// Convert parameter 5 from packed decimal degree to decimal degrees
					
			degrees = (double)((SInt32)(projectionParametersPtr[4]/1000000));
			packedMinutesSeconds = fmod (projectionParametersPtr[4], 1000000);
			minutes = (double)((SInt32)(packedMinutesSeconds/1000));
			seconds = fmod (packedMinutesSeconds, 1000);
			projectionParametersPtr[4] = degrees + minutes/60 + seconds/3600;
					
			}	// end "if (returnCode >= 6 && (*mapProjectionCodePtr == ..."
			
		if (*mapProjectionCodePtr == kCylindricalEqualAreaCode && 
					xDim == 1383 && yDim == 586)
			{
					// This is probably data in EASE Grid. There appears to be an error
					// in the data stored in the hdf file for this data. The values for
					// the upper left x and the lower right y (both negative values)
					// appear to be rounded.
					
			if (upperLeftPixelCoordinates[0] == -17334194 && 
						lowerRightPixelCoordinates[1] == -7344785)
				{
				upperLeftPixelCoordinates[0] = -17334193.537500;
				lowerRightPixelCoordinates[1] = -7344784.825000;
				
				*xMapCoordinate11Ptr = upperLeftPixelCoordinates[0];
				*yMapCoordinate11Ptr = upperLeftPixelCoordinates[1];
			
				*horizontalPixelSizePtr = (lowerRightPixelCoordinates[0] - 
																upperLeftPixelCoordinates[0])/xDim;
							
				*verticalPixelSizePtr = (upperLeftPixelCoordinates[1] - 
																lowerRightPixelCoordinates[1])/yDim;
							
						// Now adjust the upper left map coordinate to represent the 
						// center of the pixel not the outside edge of the pixel.
					
				*xMapCoordinate11Ptr += *horizontalPixelSizePtr/2;
				*yMapCoordinate11Ptr -= *verticalPixelSizePtr/2;
				
				}	// end "if (upperLeftPixelCoordinates[0] == -17334194 && ..."
						
			}	// end "if (*mapProjectionCodePtr == kCylindricalEqualAreaCode && ..."
		
		}	// end "if (*mapProjectionCodePtr != 0 && .."
    
}	// end "GetMapInfoFromGridStructure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMapInfoFromNetCDF_HRLDAS_Structure
//
//	Software purpose:	This routine reads the map information that is stored in HRLDAS
//							model data.
//
//	Parameters in:			
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/06/2012
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetMapInfoFromNetCDF_HRLDAS_Structure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				UCharPtr								bufferPtr,
				SInt32*								returnCodePtr,
				UInt32								numberLines,
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
											
	SInt16								returnCode,
											returnCode2;
	
		
			// Determine if netCDF HRLDAS file

	returnCode = GetSpecificTextAttributeInformation (id,
																		numberAttributes,
																		(UCharPtr)"TITLE",
																		(UCharPtr)"FROM HRLDAS",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
									 									bufferPtr,
									 									0,
																		NULL,
																		NULL);

	if (returnCode == 0)
				// Try another variation on the title which has similar attribute
				// names for the projection information
		returnCode = GetSpecificTextAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"TITLE",
																		(UCharPtr)"OUTPUT FROM REAL_EM",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
																		bufferPtr,
																		0,
																		NULL,
																		NULL);

	if (returnCode == 0)
				// Try another variation on the title which has similar attribute
				// names for the projection information
		returnCode = GetSpecificTextAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"TITLE",
																			(UCharPtr)"FROM GEOGRID",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
																			bufferPtr,
																			0,
																			NULL,
																			NULL);

	if (returnCode == 0)
				// Try another variation on the title which has similar attribute
				// names for the projection information
		returnCode = GetSpecificTextAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"TITLE",
																	(UCharPtr)"FROM CONSOLIDATE_GRIB",
																	(UCharPtr)"",
																	kNumValFieldNotUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	0,
																	NULL,
																	NULL);

	if (returnCode == 0)
				// Try another variation on the title which has similar attribute
				// names for the projection information
		returnCode = GetSpecificTextAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"TITLE",
																		(UCharPtr)"FROM GRID_EXTRACT",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
																		bufferPtr,
																		0,
																		NULL,
																		NULL);
																		
	if (returnCode > 0)
		{		
				// Get the map projection name.

		returnCode = GetSpecificNumericAttributeInformation (id,
																				numberAttributes,
																				(UCharPtr)"MAP_PROJ",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				&projectionCode);
		
		if (returnCode > 0)
			{
					// Check for the projection codes.
					
			if (projectionCode == 1)
				{
				*mapProjectionCodePtr = kLambertConformalConicCode;
				*mapUnitsCodePtr = kMetersCode;
				*returnCodePtr = noErr;
									
				}	// end "if (projectionCode == 1)"
					
			else if (projectionCode == 2)
				{
				*mapProjectionCodePtr = kPolarStereographicCode;
				*mapUnitsCodePtr = kMetersCode;
				*returnCodePtr = noErr;
					
				}	// end "else if (projectionCode == 2)"
					
			else if (projectionCode == 3)
				{
				*mapProjectionCodePtr = kMercatorCode;
				*mapUnitsCodePtr = kMetersCode;
				*returnCodePtr = noErr;
					
				}	// end "else if (projectionCode == 3)"
			
			}	// end "if (returnCode > 0)"
		
				// Get the projection parameters.

		if (*mapProjectionCodePtr != 0)
			{
					// Get the latitude and longitude for the lower left pixel.
					// This will be needed to convert to the map units location
					// for the upper left pixel later
					
			returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				(UCharPtr)"LAT1",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				yMapCoordinate11Ptr);
			
			if (returnCode == 0)
						// Try LA1. Some versions of Grib Consolidate outputs it as this.
				returnCode = GetSpecificNumericAttributeInformation (id,
																				numberAttributes,
																				(UCharPtr)"LA1",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				yMapCoordinate11Ptr);																			
					
			returnCode2 = GetSpecificNumericAttributeInformation (id,
																					numberAttributes,
																					(UCharPtr)"LON1",
																					(UCharPtr)"",
																					(UCharPtr)"",
																					kNumValFieldUsed,
																					kEqualSignNotUsed,
																					bufferPtr,
																					1,
																					xMapCoordinate11Ptr);
			
			if (returnCode2 == 0)
						// Try LA1. Some versions of Grib Consolidate outputs it as this.																								
				returnCode2 = GetSpecificNumericAttributeInformation (
																					id,
																					numberAttributes,
																					(UCharPtr)"LO1",
																					(UCharPtr)"",
																					(UCharPtr)"",
																					kNumValFieldUsed,
																					kEqualSignNotUsed,
																					bufferPtr,
																					1,
																					xMapCoordinate11Ptr);

																			
			if (returnCode > 0 || returnCode2 > 2)
				*convertLowerLeftFromLatLongToMapFlagPtr = TRUE;
				
			if (returnCode == 0 && returnCode2 == 0)
				{
						// Try to get center lat long for computation of the upper left
						// map coordinate values laters
						
				returnCode = GetSpecificNumericAttributeInformation (
																					id,
																					numberAttributes,
																					(UCharPtr)"CEN_LON",
																					(UCharPtr)"",
																					(UCharPtr)"",
																					kNumValFieldUsed,
																					kEqualSignNotUsed,
																					bufferPtr,
																					1,
																					xMapCoordinate11Ptr);
						
				returnCode2 = GetSpecificNumericAttributeInformation (
																					id,
																					numberAttributes,
																					(UCharPtr)"CEN_LAT",
																					(UCharPtr)"",
																					(UCharPtr)"",
																					kNumValFieldUsed,
																					kEqualSignNotUsed,
																					bufferPtr,
																					1,
																					yMapCoordinate11Ptr);
																			
				if (returnCode > 0 || returnCode2 > 2)
					*convertCenterFromLatLongToMapFlagPtr = TRUE;
				
				}	// end "if (returnCode == 0 && returnCode2 == 0)"
			
					// Get the pixel width
					
			returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				(UCharPtr)"DX",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				horizontalPixelSizePtr);
																			
					// Get the pixel height
					
			returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				(UCharPtr)"DY",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				verticalPixelSizePtr);
																			
					// Make correction for error in Consolidate GRIB code for HRLDAS
					
			if (*verticalPixelSizePtr < 0 && *horizontalPixelSizePtr > 0)
				*verticalPixelSizePtr = *horizontalPixelSizePtr;
																			
					// Get the standard latitude1
					
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"TRUELAT1",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
										 									1,
																			&projectionParametersPtr[8]);
																			
					// Get the standard latitude2
					
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"TRUELAT2",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
										 									1,
																			&projectionParametersPtr[9]);
																			
					// Get the central latitude. Same as the standard latitude
					
			projectionParametersPtr[5] = projectionParametersPtr[8];
																			
					// Get the central longitude
					
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"STAND_LON",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
										 									1,
																			&projectionParametersPtr[4]);
																			
					// For now it appears that HRLDAS and REAL_EM Preprocessor used 
					// Sphere for datum
					
			*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
			*mapProjectionDatumCodePtr = kSphereDatumCode;
				
					// The radius that is used for the sphere appears to be 6,370,000
					// See map_init in module_llxy.F and called by map_set which is called
					// by routines in consolidate_grib.F
					
			projectionParametersPtr[0] = 6370000;
			projectionParametersPtr[1] = 6370000;
		
					// Set the false easting and false northing. Assume that it is 0 
					// for now.
					
			projectionParametersPtr[6] = 0;
			projectionParametersPtr[7] = 0;
			
			}	// end "if (*mapProjectionCodePtr != 0 && .."
			
		}	// end "if (returnCode == 0)"
    
}	// end "GetMapInfoFromNetCDF_HRLDAS_Structure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMapInfoFromNetCDF_LIS_Structure
//
//	Software purpose:	This routine reads the map information that is stored in LIS
//							model data.
//
//	Parameters in:			
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 09/30/2013
//	Revised By:			Larry L. Biehl			Date: 11/08/2013

void GetMapInfoFromNetCDF_LIS_Structure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				UCharPtr								bufferPtr,
				SInt32*								returnCodePtr,
				UInt32								numberLines,
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
				Boolean*								convertLowerLeftFromLatLongToMapFlagPtr)
				
{	
	SInt16								returnCode,
											returnCode2;
	
		
			// Determine if netCDF HRLDAS file

	returnCode = GetSpecificTextAttributeInformation (id,
																		numberAttributes,
																		(UCharPtr)"TITLE",
																		(UCharPtr)"LIS land",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
									 									bufferPtr,
									 									0,
																		NULL,
																		NULL);
																		
	if (returnCode > 0)
		{		
				// Get the missing_value if it exists.
											
		returnCode = GetSpecificNumericAttributeInformation (id,
																				numberAttributes,
																				(UCharPtr)"missing_value",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				noDataValuePtr);
														
		if (returnCode == 1)
			*noDataValueExistsFlagPtr = TRUE;
				
				// Get the map projection name.
				
		returnCode = GetSpecificTextAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"MAP_PROJECTION",
																			(UCharPtr)"LAMBERT CONFORMAL",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
																			bufferPtr,
																			0,
																			NULL,
																			NULL);
		
		if (returnCode > 0)
			{
					// Set code for Lambert Conformal Conic
					
			*mapProjectionCodePtr = kLambertConformalConicCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
			
			}	// end "if (returnCode > 0)"
		
				// Get the projection parameters.

		if (*mapProjectionCodePtr != 0)
			{
					// Get the latitude and longitude for the lower left pixel.
					// This will be needed to convert to the map units location
					// for the upper left pixel later
					
			returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"SOUTH_WEST_CORNER_LAT",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	yMapCoordinate11Ptr);
			
			returnCode2 = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"SOUTH_WEST_CORNER_LON",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	xMapCoordinate11Ptr);

																			
			if (returnCode > 0 || returnCode2 > 2)
				*convertLowerLeftFromLatLongToMapFlagPtr = TRUE;
			
					// Get the pixel width
					
			returnCode = GetSpecificNumericAttributeInformation (
																				id,
																				numberAttributes,
																				(UCharPtr)"DX",
																				(UCharPtr)"",
																				(UCharPtr)"",
																				kNumValFieldUsed,
																				kEqualSignNotUsed,
																				bufferPtr,
																				1,
																				horizontalPixelSizePtr);
			if (returnCode > 0)
					// Convert to meters
				*horizontalPixelSizePtr *= 1000;
																			
					// Get the pixel height
					
			returnCode = GetSpecificNumericAttributeInformation (id,
																					numberAttributes,
																					(UCharPtr)"DY",
																					(UCharPtr)"",
																					(UCharPtr)"",
																					kNumValFieldUsed,
																					kEqualSignNotUsed,
																					bufferPtr,
																					1,
																					verticalPixelSizePtr);
			if (returnCode > 0)
					// Convert to meters
				*verticalPixelSizePtr *= 1000;
																			
					// Get the standard latitude1
					
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"TRUELAT1",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
										 									1,
																			&projectionParametersPtr[8]);
																			
					// Get the standard latitude2
					
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"TRUELAT2",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
										 									1,
																			&projectionParametersPtr[9]);
																			
					// Get the central latitude. Same as the standard latitude
					
			projectionParametersPtr[5] = projectionParametersPtr[8];
																			
					// Get the central longitude
					
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"STANDARD_LON",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
										 									1,
																			&projectionParametersPtr[4]);
																			
					// For now it appears that LIS used Sphere for datum
					
			*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
			*mapProjectionDatumCodePtr = kSphereDatumCode;
				
					// The radius that is used for the sphere appears to be 6,370,000
					// See map_init in module_llxy.F and called by map_set which is called
					// by routines in consolidate_grib.F
					
			projectionParametersPtr[0] = 6370000;
			projectionParametersPtr[1] = 6370000;
		
					// Set the false easting and false northing. Assume that it is 0 
					// for now.
					
			projectionParametersPtr[6] = 0;
			projectionParametersPtr[7] = 0;
			
			}	// end "if (*mapProjectionCodePtr != 0 && .."
			
		}	// end "if (returnCode == 0)"
    
}	// end "GetMapInfoFromNetCDF_LIS_Structure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMapInfoFromSeaSpaceStructure
//
//	Software purpose:	This routine reads the map information that is stored in what is
//							called a Grid Structure in the metadata.
//
//	Parameters in:			
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/30/2006
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void GetMapInfoFromSeaSpaceStructure (
				SInt32		 						id,
				SInt32 								numberAttributes,
				FileInfoPtr							fileInfoPtr,
				SInt16								format,
				UCharPtr								bufferPtr,
				SInt16								groupNumber,
				SInt16								numberInGroup,
				SInt32*								returnCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								mapOrientationAnglePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								gridZonePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				double*								projectionParametersPtr,
				double*								upperLeftImagePtr,
				Boolean*								convertUpperLeftFromLatLongToMapFlagPtr)
				
{	
	MapProjectionInfo					mapProjectionInfo;
	
	UInt8									datumNameString[80],
											projectionMethodString[80];
											
	double								centerImageColumn,
											centerImageRow,
											centerLatitude,
											centerLongitude,
											earthFlattening,
											equatorialRadius,
											imageCenterXOffset,
											imageCenterYOffset,
											projOriginLatitude,
											projOriginLongitude,
											projPixelHeight,
											projPixelWidth,
											stdParallel1,
											stdParallel2,
											upperLeftImageColumn,
											upperLeftImageRow,
											upperLeftLatitude,
											upperLeftLongitude;
											
	SInt16								returnCode;
	
	Boolean								canCalulateUpperLeftFlag,
											determineEllipsoidFlag;
	
		
			// Get the map projection name.

	returnCode = GetSpecificTextAttributeInformation (id,
																		numberAttributes,
																		(UCharPtr)"projection_name",
																		(UCharPtr)"",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
									 									bufferPtr,
									 									1,
																		projectionMethodString,
																		NULL);
	if (returnCode > 0)
		{
				// Check for the SeaSpace codes.
				
		if (CompareStringsNoCase (
								projectionMethodString, (UCharPtr)"Orthographic", 12) == 0)
			{
			*mapProjectionCodePtr = kOrthographicCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "if (strcmp (projectionMethodString, "Orthographic") == 0)"
				
		else if (CompareStringsNoCase (
						projectionMethodString, (UCharPtr)"Transverse Mercator", 19) == 0)
			{
			*mapProjectionCodePtr = kTransverseMercatorCode;
			*mapUnitsCodePtr = kMetersCode;
			projectionParametersPtr[2] = 1.;
			projectionParametersPtr[6] = 0;
			projectionParametersPtr[7] = 0;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "Transverse ..."
			
		else if (CompareStringsNoCase (
						projectionMethodString, (UCharPtr)"TransverseMercator", 18) == 0)
			{
			*mapProjectionCodePtr = kTransverseMercatorCode;
			*mapUnitsCodePtr = kMetersCode;
			projectionParametersPtr[2] = 1.;
			projectionParametersPtr[6] = 0;
			projectionParametersPtr[7] = 0;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "Transverse ..."
			
		else if (CompareStringsNoCase (
							projectionMethodString, (UCharPtr)"Equirectangular", 15) == 0)
			{
			*mapProjectionCodePtr = kEquirectangularCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "Equirectangular ..."
			
		else if (CompareStringsNoCase (
						projectionMethodString, (UCharPtr)"EqualAreaCylindrical", 20) == 0)
			{
			*mapProjectionCodePtr = kCylindricalEqualAreaCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "Equirectangular ..."
			
		else if (CompareStringsNoCase (
							projectionMethodString, (UCharPtr)"AlbersEqualArea", 15) == 0)
			{
			*mapProjectionCodePtr = kAlbersConicalEqualAreaCode;
			*mapUnitsCodePtr = kMetersCode;
			*returnCodePtr = noErr;
				
			}	// end "else if (strcmp (projectionMethodString, "Equirectangular ..."
							
		}	// end "if (returnCode > 0)"
	
			// Get the projection origin latitude.
	
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"proj_origin_latitude",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldNotUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	&projOriginLatitude);
	if (returnCode > 0 && projOriginLatitude != 99999)
		{
		projectionParametersPtr[5] = projOriginLatitude;
							
		}	// end "if (returnCode > 0 && projOriginLatitude != 99999)"
	
			// Get the projection origin longitude.
	
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"proj_origin_longitude",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldNotUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	&projOriginLongitude);
	if (returnCode > 0 && projOriginLongitude != 99999)
		{
		projectionParametersPtr[4] = projOriginLongitude;
							
		}	// end "if (returnCode > 0 && projOriginLongitude != 99999)"
	
			// Get the standard parallel 1.
	
	returnCode = GetSpecificNumericAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"std_parallel_1",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
																			1,
																			&stdParallel1);
	if (returnCode > 0 && stdParallel1 != 99999)
		{
		projectionParametersPtr[8] = stdParallel1;
							
		}	// end "if (returnCode > 0)"
	
			// Get the standard parallel 2.
	
	returnCode = GetSpecificNumericAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"std_parallel_2",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
																			1,
																			&stdParallel2);
	if (returnCode > 0 && stdParallel2 != 99999)
		{
		projectionParametersPtr[9] = stdParallel2;
							
		}	// end "if (returnCode > 0)"
	
			// Get the projection pixel width.
	
	returnCode = GetSpecificNumericAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"proj_pixel_width",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
																			1,
																			&projPixelWidth);
	if (returnCode > 0 && projPixelWidth != 99999)
		{
		*horizontalPixelSizePtr = projPixelWidth * 1000;
							
		}	// end "if (returnCode > 0 && projPixelWidth != 99999)"
	
			// Get the projection pixel height.
	
	returnCode = GetSpecificNumericAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"proj_pixel_height",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
																			1,
																			&projPixelHeight);
	if (returnCode > 0 && projPixelHeight != 99999)
		{
		*verticalPixelSizePtr = projPixelHeight * 1000;
							
		}	// end "if (returnCode > 0 && projPixelHeight != 99999)"
		
			// Get the datum name.

	determineEllipsoidFlag = FALSE;
	returnCode = GetSpecificTextAttributeInformation (id,
																		numberAttributes,
																		(UCharPtr)"datum_name",
																		(UCharPtr)"",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
									 									bufferPtr,
									 									1,
																		datumNameString,
																		NULL);
	if (returnCode > 0)
		{
				// Check for the SeaSpace codes.
				
		if (CompareStringsNoCase (
									datumNameString, (UCharPtr)"UserDefinedSphere", 17) == 0)
			{
			*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
			*mapProjectionDatumCodePtr = kSphereDatumCode;
				
			}	// end "if (strcmp (datumNameString, "UserDefinedSphere") == 0)"
				
		else if (CompareStringsNoCase (datumNameString, (UCharPtr)"WGS-84", 6) == 0)
			{
			*mapProjectionEllipsoidCodePtr = kWGS84EllipsoidCode;
			*mapProjectionDatumCodePtr = kWGS84Code;
				
			}	// end "if (strcmp (datumNameString, "WGS-84") == 0)"
				
		else if (CompareStringsNoCase (
								datumNameString, (UCharPtr)"UserDefinedEllipsoid", 20) == 0)
			{
			determineEllipsoidFlag = TRUE;
				
			}	// end "if (strcmp (datumNameString, "UserDefinedEllipsoid") == 0)"
							
		}	// end "if (returnCode > 0)"
		
			// Get the equatorial radius.
	
	returnCode = GetSpecificNumericAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"equatorial_radius",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
																			1,
																			&equatorialRadius);
	if (returnCode > 0 && equatorialRadius != 99999)
		{				
		projectionParametersPtr[0] = equatorialRadius * 1000;
							
		}	// end "if (returnCode > 0 && equatorialRadius != 99999)"
		
			// Get the earth flattening value if it exists
	
	returnCode = GetSpecificNumericAttributeInformation (id,
																			numberAttributes,
																			(UCharPtr)"earth_flattening",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
										 									bufferPtr,
																			1,
																			&earthFlattening);
																			
	if (returnCode > 0 && earthFlattening != 99999 && determineEllipsoidFlag)
		{
		if (projectionParametersPtr[0] == 6378137 && earthFlattening == 0.003352810681)
			{
			*mapProjectionEllipsoidCodePtr = kGRS80EllipsoidCode;
			*mapProjectionDatumCodePtr = kNAD83Code;
			
			}	// if (projectionParametersPtr[0] == 6378137 && ...
							
		}	// end "if (returnCode > 0 && earthFlattening != 99999 && ..."
		
			// Get the upper left image row.
	
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"upper_left_image_row",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldNotUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	&upperLeftImageRow);
	if (returnCode > 0 && upperLeftImageRow != 99999)
		{				
		upperLeftImagePtr[1] = upperLeftImageRow;
							
		}	// end "if (returnCode > 0 && upperLeftImageRow != 99999)"
		
			// Get the upper left image column.
	
	returnCode = GetSpecificNumericAttributeInformation (
																id,
																numberAttributes,
																(UCharPtr)"upper_left_image_column",
																(UCharPtr)"",
																(UCharPtr)"",
																kNumValFieldNotUsed,
																kEqualSignNotUsed,
																bufferPtr,
																1,
																&upperLeftImageColumn);
	if (returnCode > 0 && upperLeftImageColumn != 99999)
		{				
		upperLeftImagePtr[0] = upperLeftImageColumn;
							
		}	// end "if (returnCode > 0 && upperLeftImageColumn != 99999)"
		
			// Get the upper left image row.
	
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"upper_left_latitude",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldNotUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	&upperLeftLatitude);
	if (returnCode > 0 && upperLeftLatitude != 99999)
		{				
		*yMapCoordinate11Ptr = upperLeftLatitude;
							
		}	// end "if (returnCode > 0 && upperLeftLatitude != 99999)"
		
			// Get the upper left image column.
	
	returnCode = GetSpecificNumericAttributeInformation (
																	id,
																	numberAttributes,
																	(UCharPtr)"upper_left_longitude",
																	(UCharPtr)"",
																	(UCharPtr)"",
																	kNumValFieldNotUsed,
																	kEqualSignNotUsed,
																	bufferPtr,
																	1,
																	&upperLeftLongitude);
	if (returnCode > 0 && upperLeftLongitude != 99999)
		{				
		*xMapCoordinate11Ptr = upperLeftLongitude;
		if (*yMapCoordinate11Ptr != 0)
			*convertUpperLeftFromLatLongToMapFlagPtr = TRUE;
							
		}	// end "if (returnCode > 0 && upperLeftLongitude != 99999)"
																				
			// This is a check for some specific information which implies utm
			// projection. SeaSpace does not set the hdf file up correctly as of the
			// end of 2010. The following will make adjustments for it but it will
			// not be fool proof.
			// As of 5/2/2011 this change is commented out. It appears that the
			// scale factor being used is really still 1.0 not 0.9996 defined for UTM
			// and the spheroid always appears to be a sphere.
			
	if (*mapProjectionCodePtr == kTransverseMercatorCode &&
			projOriginLongitude == -87 &&
				projOriginLatitude == 0)
		{
				// This is probably actually UTM projection
					
		//*mapProjectionCodePtr = kUTMCode;
		//*gridZonePtr = (SInt16)((projOriginLongitude + 183)/6);
		
		*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
		*mapProjectionDatumCodePtr = kSphereDatumCode;
		
		}	// end "if (*mapProjectionCodePtr == kTransverseMercatorCode)"
		
			// This is a check to for MODIS data sets which have more than one group.
			// There currently is not a good clue to get the actuall pixel size if
			// if the data set is not from the first group of data sets. This check
			// will be very specific for now for cases we have run into.
			
	if (groupNumber == 2 && numberInGroup == 5 && *horizontalPixelSizePtr == 250)
		{
		*horizontalPixelSizePtr = 500;
		*verticalPixelSizePtr = 500;
		
		}	// end "if (groupNumber == 2 && numberInGroup == 5 &&"
		
			// If the information is not available for the upper left corner, see
			// if it can be computed used information for the center of the image.
			
	if (upperLeftLongitude == 99999 || upperLeftLatitude == 99999)
		{
				// Get the center image row.
		
		returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"center_image_row",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
																			bufferPtr,
																			1,
																			&centerImageRow);
			
				// Get the center image column.
		
		if (returnCode == 1)
			returnCode = GetSpecificNumericAttributeInformation (
																		id,
																		numberAttributes,
																		(UCharPtr)"center_image_column",
																		(UCharPtr)"",
																		(UCharPtr)"",
																		kNumValFieldNotUsed,
																		kEqualSignNotUsed,
																		bufferPtr,
																		1,
																		&centerImageColumn);
			
				// Get the center image latitude.
		
		if (returnCode == 1)
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"center_latitude",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
																			bufferPtr,
																			1,
																			&centerLatitude);
			
				// Get the center image longitude.
		
		if (returnCode == 1)
			returnCode = GetSpecificNumericAttributeInformation (
																			id,
																			numberAttributes,
																			(UCharPtr)"center_longitude",
																			(UCharPtr)"",
																			(UCharPtr)"",
																			kNumValFieldNotUsed,
																			kEqualSignNotUsed,
																			bufferPtr,
																			1,
																			&centerLongitude);
																				
		if (returnCode == 1 && 
				centerImageRow != 99999 && centerImageColumn != 99999 &&
					centerLatitude != 99999 && centerLongitude != 99999)
			{
			canCalulateUpperLeftFlag = FALSE;
			imageCenterXOffset = 0;
			imageCenterYOffset = 0;	
			if (projOriginLongitude != centerLongitude ||
																projOriginLatitude != centerLatitude)
				{
				if (*mapProjectionCodePtr == kOrthographicCode)
					{
					mapProjectionInfo.geodetic.spheroidCode = 
																		*mapProjectionEllipsoidCodePtr;
					mapProjectionInfo.gridCoordinate.longitudeCentralMeridian =
																						projOriginLongitude;
					mapProjectionInfo.gridCoordinate.latitudeOrigin = projOriginLatitude;
					mapProjectionInfo.geodetic.radiusSpheroid = equatorialRadius * 1000;
					
					imageCenterXOffset = centerLongitude;
					imageCenterYOffset = centerLatitude;
					
					if (ConvertLatLongToOrthographic (&mapProjectionInfo,
																	&imageCenterXOffset, 
																	&imageCenterYOffset))
						{
						canCalulateUpperLeftFlag = TRUE;
						
						}
					
					}	// end "if (*mapProjectionCodePtr == kOrthographicCode)"
				
				}	// end "if (projOriginLongitude != centerLongitude || ..."
				
			else	// projOriginLongitude == centerLongitude && projOriginLatitude ...
				canCalulateUpperLeftFlag = TRUE;
				
			if (canCalulateUpperLeftFlag)
				{
						// Get the map coordinates of the upper left point.
							
				*xMapCoordinate11Ptr = -(centerImageColumn - upperLeftImagePtr[0]) *
																				*horizontalPixelSizePtr;
				*xMapCoordinate11Ptr += imageCenterXOffset;
																					
				if (upperLeftImagePtr[0] == -.5)
						// Convert upper left x map value to represent the center of the 
						// pixel not the left side of the pixel.
					*xMapCoordinate11Ptr += *horizontalPixelSizePtr * .5;				
							
				*yMapCoordinate11Ptr = (centerImageRow - upperLeftImagePtr[1]) *
																					*verticalPixelSizePtr;
				*yMapCoordinate11Ptr += imageCenterYOffset;
																					
				if (upperLeftImagePtr[1] == -.5)
						// Convert upper left y map value to represent the center of the 
						// pixel not the top side of the pixel.
					*yMapCoordinate11Ptr -= *verticalPixelSizePtr * .5;
					
				}	// end "if (canCalulateUpperLeftFlag)"
								
			}	// end "if (returnCode > 0 && upperLeftLongitude != 99999)"
		
		}	// end "if (upperLeftLongitude == 99999 || ..."
		
	if (*mapProjectionCodePtr != 0 && format == kNETCDFType)
				// SeaSpace software does not write images to netcdf files from bottom 
				// to top (at least for those I have as of 2/23/2012)
		fileInfoPtr->treatLinesAsBottomToTopFlag = FALSE;
				
}	// end "GetMapInfoFromSeaSpaceStructure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetNewHDF4FileReferences
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
//	Coded By:			Larry L. Biehl			Date: 10/29/2007
//	Revised By:			Larry L. Biehl			Date: 01/05/2018

Boolean GetNewHDF4FileReferences (
				FileInfoPtr		 					fileInfoPtr)
				
{	
	UInt8									filePathString[512];
	
	HdfDataSets*						hdfDataSetsPtr;
	
	FileStringPtr						filePathPtr;
	
	SInt32								file_id,
											sdid;
	
	UInt32								dataSet,
											index;
											
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
	
	#if defined multispec_win
		USES_CONVERSION;
	#endif

   errCode = GetHDFFilePathCPointer (fileInfoPtr, 
   												filePathString, 
   												510,
   												&filePathPtr,
													kHDF4_Library);
	if (errCode != noErr) 
																							return FALSE;
	
	file_id = SDstart (T2A((LPTSTR)filePathPtr), DFACC_RDONLY);
	if (file_id == FAIL) 
																							return FALSE;
	
	fileInfoPtr->hdf4FileID = file_id;
	gNumberHDFFilesOpen++;
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle, kLock);
															
	if (hdfDataSetsPtr != NULL)
		{
		for (index=1; index<=fileInfoPtr->numberHdfDataSets; index++) 
			{
			dataSet = hdfDataSetsPtr[index].dataSet;
			
			sdid = SDselect (file_id, dataSet);
			
			if (sdid != FAIL)
				hdfDataSetsPtr[index].sdid = (void*)sdid;
				//hdfDataSetsPtr[index].sdid = sdid;
			
			else	// sdid == FAIL
				{
				hdfDataSetsPtr[index].sdid = 0;
				continueFlag = FALSE;
				
				}	// end "else sdid == FAIL"
		
			}	// end "for (index=1; index<=fileInfoPtr->numberHdfDataSets; index++)"
			
		CheckAndUnlockHandle (fileInfoPtr->hdfHandle);
			
		}	// end "if (hdfDataSetsPtr != NULL)"
		
	else	// end "hdfDataSetsPtr == NULL
		continueFlag = FALSE;
	
	return (continueFlag);
    
}	// end "GetNewHDF4FileReferences"




//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetSdidValue
//
//	Software purpose:	This routine returns the SInt32 version of the sdid value. The
//							routine is needed for 64 bit compilers. HdfDataSets has sdid
//							declared as a void* for use by HDF5 and HDF4 libraries.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	sdid value
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/28/2015
//	Revised By:			Larry L. Biehl			Date: 07/28/2015

SInt32 GetSdidValue (
				HdfDataSets*		 				hdfDataSetsPtr)
				
{	
	SInt32								sdidValue;
	
	#if defined multispec_mac || multispec_win
		sdidValue = (SInt32)hdfDataSetsPtr->sdid;
	#endif
	
	#if defined multispec_lin
		SInt64 sdid64 = (SInt64)hdfDataSetsPtr->sdid;
		sdidValue = (SInt32)sdid64;
	#endif
	
	return sdidValue;
    
}	// end "GetSdidValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetSpecificTextAttributeInformation
//
//	Software purpose:	This routine reads the requested alphanumeric information in the
// 						hdf attributes.
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
//	Coded By:			Larry L. Biehl			Date: 03/01/2005
//	Revised By:			Larry L. Biehl			Date: 01/05/2018

SInt16 GetSpecificTextAttributeInformation (
				SInt32		 						id,
				SInt32								numberAttributes,
				UCharPtr								masterGroupString,
				UCharPtr								groupString,
				UCharPtr								objectString,
				Boolean								numValFieldUsedFlag,
				Boolean								equalSignUsedFlag,
				UCharPtr								bufferPtr,
				SInt32								numberValues,
				UCharPtr								outputString1Ptr,
				UCharPtr								outputString2Ptr)
				
{	
	UInt8									name[80];
	
	UCharPtr								stringPtr;
	
	#if TARGET_RT_MAC_MACHO
		int									attributeSize,
   											attributeType;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								attributeSize,
   											attributeType;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								index,
   										length,
   										lengthObjectString,
   										objectNumberValues;
 
 	UInt32								bufferIndex;
 	
 	SInt16								returnCode;
 	
 	Boolean								continueFlag = TRUE;
 	
 		
 	returnCode = 0;
 	
	for (index=1; index<=numberAttributes; index++)
		{
				// Loop through the attributes to find the requested master group.
				
		if (SDattrinfo (
						id, index-1, (char*)name, &attributeType, &attributeSize) == noErr)
			{
			if ((strcmp ((char*)name, (char*)masterGroupString) == 0) && 
																		(attributeType == DFNT_CHAR8))
				{
				if (SDreadattr (id, index-1, bufferPtr) == noErr)
					{
					bufferIndex = 0;
					
							// Search for requested group string
					
					if (groupString[0] != 0)
						{												
						stringPtr = (UCharPtr)strstr ((char*)bufferPtr, (char*)groupString);
						continueFlag = (stringPtr != NULL);
						
						}	// end "if (groupString[0] != 0)"
						
					else	// groupString[0] == 0
						stringPtr = bufferPtr;
					
					lengthObjectString = (SInt32)strlen ((char*)objectString);
					
					if (continueFlag && 
									lengthObjectString == 0 && 
												outputString1Ptr == NULL)
						{
								// We are done. The requested group string was found.
								
						returnCode = 1;
						break;
						
						}	// "if (continueFlag && ..."
						
					if (outputString1Ptr == NULL)
						continueFlag = FALSE;
								
					if (continueFlag && lengthObjectString > 0)
						{
								// Group string was found, now search for object string
								
						length = (SInt32)(stringPtr - &bufferPtr[bufferIndex]);
						bufferIndex += length;
						attributeSize -= length;			
						stringPtr = (UCharPtr)strstr ((char*)&bufferPtr[bufferIndex], (char*)objectString);
						continueFlag = (stringPtr != NULL);
						
						}	// end "if (continueFlag && lengthObjectString > 0)"
								
					if (continueFlag && numValFieldUsedFlag)
						{
								// Object string was found. Now get the NUM_VAL string.
								
						length = (SInt32)(stringPtr - &bufferPtr[bufferIndex]);
						bufferIndex += length;
						attributeSize -= length;
						stringPtr = (UCharPtr)strstr (
														(char*)&bufferPtr[bufferIndex], "NUM_VAL");
						continueFlag = (stringPtr != NULL);
						
						}	// end "if (continueFlag && numValFieldUsedFlag)"
								
					if (continueFlag && numValFieldUsedFlag)
						{
								// NUM_VAL string was found. Now get the following = string.
								
						length = (SInt32)(stringPtr - &bufferPtr[bufferIndex]);
						bufferIndex += length;
						attributeSize -= length;
						stringPtr = (UCharPtr)strstr ((char*)&bufferPtr[bufferIndex], "=");
						continueFlag = (stringPtr != NULL);
						
						}	// end "if (continueFlag && numValFieldUsedFlag)"
								
					if (continueFlag && numValFieldUsedFlag)
						{
								// The = string was found. Now get the number of values.
								// Skip "="
								
						length = stringPtr - &bufferPtr[bufferIndex];
						bufferIndex += length;
						attributeSize -= length;
				 		returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
				 										"= %ld", 
				 										&objectNumberValues);
						continueFlag = (returnCode == 1);
						
						}	// end "if (continueFlag && numValFieldUsedFlag)"
								
					if (continueFlag && numValFieldUsedFlag)
						{
								// The number of values were found. Now find the VALUE 
								// string.
								
						stringPtr = (UCharPtr)strstr (
														(char*)&bufferPtr[bufferIndex], "VALUE");
						continueFlag = (stringPtr != NULL);
						
						}	// end "if (continueFlag && numValFieldUsedFlag)"
								
					if (continueFlag && equalSignUsedFlag)
						{
								// VALUE string was found. Now get the following = string.
								
						length = stringPtr - &bufferPtr[bufferIndex];
						bufferIndex += length;
						attributeSize -= length;
						stringPtr = (UCharPtr)strstr ((char*)&bufferPtr[bufferIndex], "=");
						continueFlag = (stringPtr != NULL);
						
						}	// end "if (continueFlag && equalSignUsedFlag)"
								
					if (continueFlag)
						{
								// The = string found. Now read the requested string.
									
						length = stringPtr - &bufferPtr[bufferIndex];
						bufferIndex += length;
						attributeSize -= length;
							
								// If NUMVAL field is not used, then we need to determine 
								// whether there is just one value or multiple values (implied
								// by existance of '(').
						
						if (!numValFieldUsedFlag)
							{
							objectNumberValues = 1;
							if (stringPtr[1] == '(')
								objectNumberValues = 2;
							
							}	// end "if (!numValFieldUsedFlag)"
						
						returnCode = 0;
								
						if (objectNumberValues == 1)
							{
									// Number of values is one, get the value.
							
							if (equalSignUsedFlag)
								{	
								if (numValFieldUsedFlag)
					 				returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
					 											"= \"%s\"\n", 
					 											outputString1Ptr);
									
								else	// !numValFieldUsedFlag
					 				returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
					 											"=%s\n", 
					 											outputString1Ptr);
					 											
					 			}	// end "if (equalSignUsedFlag)"
					 			
					 		else	// !equalSignUsedFlag
				 				returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
				 											"%s\n", 
				 											outputString1Ptr);
						 										
						 	if (returnCode == 1)
						 		RemoveTrailingCharacters (outputString1Ptr);
							
							}	// end "if (objectNumberValues == 1)"
								
						else	// objectNumberValues > 1
							{
							if (numberValues == 1)
								{
										// Now get the parameters for the object
										
						 		returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
						 										"= (%s", 
						 										outputString1Ptr);
						 										
							 	if (returnCode == 1)
							 		RemoveTrailingCharacters (outputString1Ptr);
									
								}	// end "if (numberValues == 1)"
								
							else if (numberValues == 2)
								{
										// Now get the 1st string parameters for the object
										
						 		returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
						 										"= (\"%s\"", 
						 										outputString1Ptr);
						 										
						 		if (returnCode == 1)
							 		{
							 				// Get the 2nd string parameter
							 				
									length = strlen ((char*)outputString1Ptr) + 5;
									bufferIndex += length;
									stringPtr = &bufferPtr[bufferIndex];
										
						 			returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
						 										"\"%s\")", 
						 										outputString2Ptr);
						 										
						 			if (returnCode == 1)
						 				returnCode = 2;
							 		
							 		}	// end "if (returnCode == 1)"
							 		
							 	if (returnCode == 2)
							 		{
							 				// Remove the trailing characters
							 				
							 		RemoveTrailingCharacters (outputString1Ptr);
							 		RemoveTrailingCharacters (outputString2Ptr);
							 			
							 		}	// end "if (returnCode == 2)"
									
								}	// else if (numberValues == 2)
								
							}	// else objectNumberValues > 1
				 		
				 		break;
						
						}	// end "if (continueFlag)"
						
					}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				
				}	// end "if ((strcmp (name, "
				
			}	// end "if (SDattrinfo (sdid, index-1, ..."
		
		}	// end "for (index=1; index<=numberAttributes; index++)"
 		
 	return (returnCode);
			
}	// end "GetSpecificTextAttributeInformation



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetSpecificNumericAttributeInformation
//
//	Software purpose:	This routine reads the requested numeric information in the
// 						hdf attributes.
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
//	Coded By:			Larry L. Biehl			Date: 02/28/2005
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 GetSpecificNumericAttributeInformation (
				SInt32		 						id,
				SInt32								numberAttributes,
				UCharPtr								masterGroupString,
				UCharPtr								groupString,
				UCharPtr								objectString,
				Boolean								numValFieldUsedFlag,
				Boolean								equalSignUsedFlag,
				UCharPtr								bufferPtr,
				SInt32								numberValues,
				double*								valuesPtr)
				
{	
	UInt8									name[80];
	
	UCharPtr								stringPtr;
	
	#if TARGET_RT_MAC_MACHO
		int									attributeSize,
   											attributeType;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								attributeSize,
   											attributeType;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								index,
   										length,
   										objectNumberValues;
 
 	UInt32								bufferIndex;
 	
 	SInt16								returnCode;
 	
 	Boolean								continueFlag = TRUE;
 	
 		
 	returnCode = 0;
 	
 	if (numberValues <= 13)		
 		{
		for (index=1; index<=numberAttributes; index++)
			{
					// Loop through the attributes to find the requested master group.
					
			if (SDattrinfo (
						id, index-1, (char*)name, &attributeType, &attributeSize) == noErr)
				{
				if (strcmp ((char*)name, (char*)masterGroupString) == 0) 
					{
					if (attributeType == DFNT_CHAR8)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							bufferIndex = 0;
						
									// Search for requested group string
							
							if (groupString[0] != 0)
								{												
								stringPtr = (UCharPtr)strstr (
																(char*)bufferPtr, (char*)groupString);
								continueFlag = (stringPtr != NULL);
								
								}	// end "if (groupString[0] != 0)"
								
							else	// groupString[0] == 0
								stringPtr = bufferPtr;
										
							if (continueFlag)
								{
										// Now search for object string
										
								length = stringPtr - &bufferPtr[bufferIndex];
								bufferIndex += length;
								attributeSize -= length;			
								stringPtr = (UCharPtr)strstr (
											(char*)&bufferPtr[bufferIndex], (char*)objectString);
								continueFlag = (stringPtr != NULL);
								
								}	// end "if (continueFlag)"
										
							if (continueFlag && numValFieldUsedFlag)
								{
										// Object string was found. Now get the NUM_VAL string.
										
								length = stringPtr - &bufferPtr[bufferIndex];
								bufferIndex += length;
								attributeSize -= length;
								stringPtr = (UCharPtr)strstr (
														(char*)&bufferPtr[bufferIndex], "NUM_VAL");
								continueFlag = (stringPtr != NULL);
								
								}	// end "if (continueFlag && numValFieldUsedFlag)"
										
							if (continueFlag && numValFieldUsedFlag)
								{
										// NUM_VAL string was found. Now get the following 
										// = string.
										
								length = stringPtr - &bufferPtr[bufferIndex];
								bufferIndex += length;
								attributeSize -= length;
								stringPtr = (UCharPtr)strstr (
																(char*)&bufferPtr[bufferIndex], "=");
								continueFlag = (stringPtr != NULL);
								
								}	// end "if (continueFlag && numValFieldUsedFlag)"
										
							if (continueFlag && numValFieldUsedFlag)
								{
										// The = string was found. Now get the number of values.
										// Skip "="
										
								length = stringPtr - &bufferPtr[bufferIndex];
								bufferIndex += length;
								attributeSize -= length;
						 		returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
						 										"= %ld", 
						 										&objectNumberValues);
								continueFlag = (returnCode == 1);
								
								}	// end "if (continueFlag && numValFieldUsedFlag)"
										
							if (continueFlag && numValFieldUsedFlag)
								{
										// The number of values were found. Now find the VALUE 
										// string.
										
								stringPtr = (UCharPtr)strstr (
														(char*)&bufferPtr[bufferIndex], "VALUE");
								continueFlag = (stringPtr != NULL);
								
								}	// end "if (continueFlag && numValFieldUsedFlag)"
										
							if (continueFlag && equalSignUsedFlag)
								{
										// VALUE string was found. Now get the following 
										// = string.
										
								length = stringPtr - &bufferPtr[bufferIndex];
								bufferIndex += length;
								attributeSize -= length;
								stringPtr = (UCharPtr)strstr (
															(char*)&bufferPtr[bufferIndex], "=");
								continueFlag = (stringPtr != NULL);
								
								}	// end "if (continueFlag && equalSignUsedFlag)"
										
							if (continueFlag)
								{
										// The = string found. Now read the requested values.
											
								length = stringPtr - &bufferPtr[bufferIndex];
								bufferIndex += length;
								attributeSize -= length;
								
										// If NUMVAL field is not used, then we need to determine whether
										// there is just one value or multiple values (implied by existance
										// of '('.
								
								if (!numValFieldUsedFlag)
									{
									objectNumberValues = 1;
									if (stringPtr[1] == '(')
										objectNumberValues = 2;
									
									}	// end "if (!numValFieldUsedFlag)"
								
								returnCode = 999;
										
								if (objectNumberValues == 1)
									{
											// Number of values is one, get the value.
									
									if (equalSignUsedFlag)	
							 			returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
							 										"= %lf", 
							 										&valuesPtr[0]);
									
									else	// !equalSignUsedFlag	
							 			returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
							 										"%lf", 
							 										&valuesPtr[0]);
									
									}	// end "if (objectNumberValues == 1)"
									
								else	// objectNumberValues > 1
									{
											// Now get the parameters for the object
											
									returnCode = sscanf ((char*)&bufferPtr[bufferIndex], 
																"= (%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", 
																&valuesPtr[0], 
																&valuesPtr[1], 
																&valuesPtr[2], 
																&valuesPtr[3], 
																&valuesPtr[4], 
																&valuesPtr[5], 
																&valuesPtr[6], 
																&valuesPtr[7], 
																&valuesPtr[8], 
																&valuesPtr[9], 
																&valuesPtr[10], 
																&valuesPtr[11], 
																&valuesPtr[12]);
									}	// else objectNumberValues > 1
						 										
						 		if (returnCode != numberValues)
						 			returnCode = 0;
						 		
						 		break;
								
								}	// end "if (continueFlag)"
								
							}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
							
						}	// end "if (attributeType == DFNT_CHAR8)"
						
					else if (attributeType == DFNT_INT8)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((SInt8*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_INT8)"
						
					else if (attributeType == DFNT_UINT8)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((UInt8*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_UINT8)"
						
					else if (attributeType == DFNT_INT16)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((SInt16*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_INT16)"
						
					else if (attributeType == DFNT_UINT16)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((UInt16*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_UINT16)"
						
					else if (attributeType == DFNT_INT32)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((SInt32*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_INT32)"
						
					else if (attributeType == DFNT_UINT32)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((UInt32*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_UINT32)"
						
					else if (attributeType == DFNT_FLOAT32)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = (double)(*((float*)bufferPtr));
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_FLOAT32)"
						
					else if (attributeType == DFNT_FLOAT64)
						{
						if (SDreadattr (id, index-1, bufferPtr) == noErr)
							{
							valuesPtr[0] = *((double*)bufferPtr);
							returnCode = 1;
							
							break;
					 										
					 		}	// end "if (SDreadattr (id, index-1, bufferPtr) == noErr)"
				 										
				 		}	// end "else if (attributeType == DFNT_FLOAT64)"
					
					}	// end "if ((strcmp (name, "
					
				}	// end "if (SDattrinfo (sdid, index-1, ..."
			
			}	// end "for (index=1; index<=numberAttributes; index++)"
		
 		}	// end "if (numberValues <= 4)"
 		
 	return (returnCode);
			
}	// end "GetSpecificNumericAttributeInformation


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetNumberOfValidDataSets
//
//	Software purpose:	This routine reads the file specifications from
//							a HDF formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	= 0, Image data set was found, No Error
//							= 1, No image data set was found
//							= 2, Image data set was found but it is compressed.
//							= 3, Image data set was found but it is 4-byte integer.
//							= 4, There are no more image data sets in the file.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/27/2001
//	Revised By:			Larry L. Biehl			Date: 11/27/2001

SInt32 GetNumberOfValidDataSets (
				SInt32		 						file_id,
				SInt32								numDataSets)
				
{
	intn 									hdfReturn;
	
   SInt32 								dataSet,
   										returnCode,
   										sdid,
   										validDataSetCount;
   
   
   validDataSetCount = 0;
	for (dataSet=0; dataSet<numDataSets; dataSet++)
		{
		sdid = SDselect (file_id, dataSet);
			
		hdfReturn = GetHDFInformation (sdid,
													&returnCode,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);
				
		SDendaccess (sdid);
		
		if (hdfReturn == FAIL)
			break;
			
		if (returnCode == noErr)
			validDataSetCount++;
		
		}	// end "for (dataSet=0; dataSet<numDataSets; dataSet++)"
		
	return (validDataSetCount);
    
}	// end "GetNumberOfValidDataSets"
*/



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListAttributeInformation
//
//	Software purpose:	This routine checks the input HDF data set to determine
//							if MultiSpec can handle it.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/07/2002
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ListAttributeInformation (
				SInt32		 						id,
				UInt32								numberAttributes)
				
{
	UInt8									prefixString[8];
	
	UCharPtr								bufferPtr,
											stringPtr;
											
	double*								doublePtr;
	float*								floatPtr;
	SInt16*								int16Ptr;
	UInt16*								uint16Ptr;
	SInt32*								int32Ptr;
	UInt32*								uint32Ptr;
	
	#if TARGET_RT_MAC_MACHO
		int									attributeSize,
   											attributeType;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								attributeSize,
   											attributeType;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								length,
											lineLength,
   										maxLength,
   										numberValues,
   										prefixLength,
   										totalLength,
   										valueIndex;

	UInt32								bufferIndex,
											index;
   										
   SInt16								twoByteValue;
   										
   UInt16								unsignedTwoByteValue;
   										
   Boolean								continueFlag;
   
	char									stringBuffer[2048];
	
		
			// Get the buffer to be used to read the attribute information.
					
	bufferPtr = (UCharPtr)GetAttributeBuffer (id, numberAttributes);
			
	if (bufferPtr == NULL)
		numberAttributes = 0;
				
			// Set up to read attribute buffers.
			
	continueFlag = TRUE;
	
	doublePtr = (double*)bufferPtr;
	floatPtr = (float*)bufferPtr;
	int16Ptr = (SInt16*)bufferPtr;
	uint16Ptr = (UInt16*)bufferPtr;
	int32Ptr = (SInt32*)bufferPtr;
	uint32Ptr = (UInt32*)bufferPtr;
	
	prefixString[0] = ' ';
	prefixString[1] = ' ';
	prefixString[2] = ' ';
	prefixString[3] = ' ';
	prefixString[4] = ' ';
	prefixString[5] = ' ';
	prefixString[6] = ' ';
	prefixString[7] = 0;
						
			// Turn spin cursor on

	gPresentCursor = kSpin;
	
	gNextTime = TickCount ();
		
			// Go through attributes to get information about the band interleave
			// description
	
	for (index=1; index<=numberAttributes; index++)
		{
		maxLength = 2000;
		
		if (SDattrinfo (
				id, index-1, (char*)bufferPtr, &attributeType, &attributeSize) == noErr)
			{
			lineLength = 0;
			totalLength = sprintf (stringBuffer, "      %s:", bufferPtr);
			
			if (attributeType != DFNT_CHAR8 && attributeSize > 1)
				{
				length = sprintf (&stringBuffer[totalLength-1], " (%ld):", attributeSize);
				totalLength += length - 1;
				
				}	// end "if (attributeType != DFNT_CHAR8 && ..."
			
			continueFlag = ListString (stringBuffer, totalLength, gOutputTextH);
			
			maxLength -= totalLength;
			
			if (continueFlag && SDreadattr (id, index-1, bufferPtr) == noErr)
				{
				length = 0;
				numberValues = attributeSize;
				
				switch (attributeType)
					{
					case DFNT_CHAR8:
						bufferIndex = 0;
						prefixLength = 1;
						while (attributeSize >  0)
							{
									// Find the end of the line character.
																	
							stringPtr = (UCharPtr)memchr ((char*)&bufferPtr[bufferIndex], 
																	'\n', 
																	attributeSize);
							
									// Now get the length of the string to be listed.
									
							if (stringPtr == NULL)
								{
								if (bufferPtr[bufferIndex] == 0)
									attributeSize = 0;
									
								length = attributeSize;
								
								}	// end "if (stringPtr == NULL)"
								
							else	// stringPtr != NULL
								length = stringPtr - &bufferPtr[bufferIndex];
							
									// Limit the line length to maxLength characters.
									
							length = MIN (maxLength, length);
							if (length > 0)
								{
								continueFlag = ListString ((char*)prefixString, 
																	prefixLength, 
																	gOutputTextH);
								
								if (continueFlag)									
									continueFlag = ListString ((char*)&bufferPtr[bufferIndex], 
																		length, 
																		gOutputTextH);
					
								if (continueFlag)	
									continueFlag = ListString (gEndOfLine, 
																		gNumberOfEndOfLineCharacters, 
																		gOutputTextH);
																		
								lineLength = totalLength + prefixLength + length + 
																			gNumberOfEndOfLineCharacters;
								gMaxCharsInLine = MAX (gMaxCharsInLine, lineLength);
								
										// Make sure that there is no c terminator at end of
										// prefixString. This occurs in the Windows version.

								if (prefixLength == 1)
									prefixString[prefixLength] = ' ';
								prefixLength = 7;
								totalLength = 0;					
				
								if (TickCount () >= gNextTime)
									{
									if (!CheckSomeEvents (
												osMask+keyDownMask+updateMask+mDownMask+mUpMask))
										{
										continueFlag = FALSE;
										break;
										
										}	// end "if (!CheckSomeEvents (..."
										
									}	// end "if (TickCount () >= gNextTime)"
																	
								}	// end "if (length > 0)"
								
							if (stringPtr != NULL)
								bufferIndex += length + 1;
								
							attributeSize -= (length+1);
															
							}	// end "while (attributeSize >  0)"
							
						attributeSize = 0;
						length = 0;
						break;
						
					case DFNT_INT8:
						twoByteValue = bufferPtr[0];
						length = sprintf (stringBuffer, " %hd", twoByteValue);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							twoByteValue = bufferPtr[valueIndex];
							length += sprintf (&stringBuffer[length], ", %hd", twoByteValue);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
							
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					case DFNT_UINT8:
						unsignedTwoByteValue = (UInt16)((UInt8)bufferPtr[0]);
						length = sprintf (stringBuffer, " %hu", unsignedTwoByteValue);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							twoByteValue = (UInt16)((UInt8)bufferPtr[valueIndex]);
							length += sprintf (&stringBuffer[length], 
														", %hu", 
														unsignedTwoByteValue);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					case DFNT_INT16:
						length = sprintf (stringBuffer, " %hd", *int16Ptr);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							length += sprintf (&stringBuffer[length], 
														", %hd", 
														int16Ptr[valueIndex]);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
														
						break;
						
					case DFNT_UINT16:
						length = sprintf (stringBuffer, " %hu", *uint16Ptr);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							length += sprintf (&stringBuffer[length], 
														", %hu", 
														uint16Ptr[valueIndex]);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					case DFNT_INT32:
						length = sprintf (stringBuffer, " %ld", *int32Ptr);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							length += sprintf (&stringBuffer[length], 
														", %ld", 
														int32Ptr[valueIndex]);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					case DFNT_UINT32:
						length = sprintf (stringBuffer, " %lu", *uint32Ptr);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							length += sprintf (&stringBuffer[length], 
														", %lu", 
														uint32Ptr[valueIndex]);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					case DFNT_FLOAT32:
						length = sprintf (stringBuffer, " %.6f", *floatPtr);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							length += sprintf (&stringBuffer[length], 
														", %.6f", 
														floatPtr[valueIndex]);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					case DFNT_FLOAT64:
						length = sprintf (stringBuffer, " %.12f", *doublePtr);
						
						for (valueIndex=1; valueIndex<attributeSize; valueIndex++)
							{
							length += sprintf (&stringBuffer[length], 
														", %.12f", 
														doublePtr[valueIndex]);
							
							if (length > maxLength)
								{
								numberValues = valueIndex + 1;
								break;
								
								}	// end "if (length > maxLength)"
								
							}	// end "for (valueIndex=0; valueIndex<..."
						break;
						
					}	// end "switch (attributeType)"
														
				if (numberValues < attributeSize)
					length += sprintf (&stringBuffer[length], 
														", ...");
				
				if (continueFlag && length > 0)	
					continueFlag = ListString (stringBuffer, 
															length, 
															gOutputTextH);
				
				}	// end "if (continueFlag && SDreadattr (id, ..."
					
			if (continueFlag && lineLength == 0)	
				continueFlag = ListString (gEndOfLine, 
													gNumberOfEndOfLineCharacters, 
													gOutputTextH);
			
					// Update the maximum number of characters in a line of text so that 
					// the output window scroll bar is handled correctly.
															
			totalLength += length + gNumberOfEndOfLineCharacters;
			gMaxCharsInLine = MAX (gMaxCharsInLine, totalLength);
				
			}	// end "if (SDattrinfo (sdid, index-1, ..."				
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				continueFlag = FALSE;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (TickCount () >= gNextTime)"
			
		if (!continueFlag)
			break;
		
		}	// end "for (index=1; index<=nattrs; index++)"
					
			// Change cursor back to pointer.								
				
	MInitCursor ();

	CheckAndDisposePtr (bufferPtr);
		
	return (continueFlag);
    
}	// end "ListAttributeInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHDF4DataSetAttributes
//
//	Software purpose:	This list the attribute information for the requested data sets.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/02/2002
//	Revised By:			Larry L. Biehl			Date: 01/05/2018

Boolean ListHDF4DataSetAttributes (
				FileInfoPtr 						fileInfoPtr,
				char*									formatNamePtr,
				Boolean								listAllDataSetInfoFlag)
				
{
	//CMFileStream						hdfHeaderFileStream;
	
	char*									bufferPtr;
	HdfDataSets*						hdfDataSetsPtr;
	//StringPtr 							filePathPtr;
	
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									dims[10],
   											nattrs = 0,
   											numberType,
   											numDataSets,
   											nglobals,
   											rank;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								dims[10],
   											nattrs = 0,
   											numberType,
   											numDataSets,
   											nglobals,
   											rank;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								file_id,
   										sdid = FAIL;
   										
   SInt16								length;
											
	UInt32								dataSet,
											dataSetEnd,
											dataSetStart;
   										
   Boolean								continueFlag;
   
   			
	continueFlag = TRUE;
   file_id = FAIL;
	hdfReturn = SUCCEED;
	bufferPtr = NULL;
	
	hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle, kLock);
															
	if (hdfDataSetsPtr == NULL)
																							return (TRUE);
				
	file_id = fileInfoPtr->hdf4FileID;

	if (file_id != FAIL && SDfileinfo (file_id, &numDataSets, &nglobals) == noErr)
		{
		if (nglobals > 0)
			{
			length = sprintf ((char*)&gTextString, 
										"    %s global file attributes%s",
										&formatNamePtr[1],
										gEndOfLine);
			continueFlag = ListString ((char*)&gTextString, length, gOutputTextH);
			
			if (continueFlag)
				continueFlag = ListAttributeInformation (file_id, nglobals);
				
			}	// end "if (nglobals > 0)"	
			
		else	// nglobals <= 0
			{
			length = sprintf ((char*)&gTextString, 
										"    No %s global file attributes%s",
										&formatNamePtr[1],
										gEndOfLine);
			continueFlag = ListString ((char*)&gTextString, length, gOutputTextH);
			
			}	// end "else nglobals <= 0"
					
		if (continueFlag)
			continueFlag = ListString (gEndOfLine, 
												gNumberOfEndOfLineCharacters, 
												gOutputTextH);
			
		}	// end "if (file_id != FAIL && ...)"
	
	if (file_id != FAIL)
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
			
			}	// end "else !listAllDataSetInfoFlag"

		for (dataSet=dataSetStart; dataSet<=dataSetEnd; dataSet++)
			{
			//sdid = (SInt32)hdfDataSetsPtr[dataSet].sdid;
			sdid = GetSdidValue (&hdfDataSetsPtr[dataSet]);
		
			if (sdid == FAIL)
				hdfReturn = FAIL;
				
			rank = 10;	// Indicate what the max length of dims is.
			if (hdfReturn != FAIL)
				hdfReturn = SDgetinfo (sdid, 
												(char*)&gTextString, 
												&rank, 
												dims, 
												&numberType, 
												&nattrs);
				
			if (hdfReturn == FAIL)
				nattrs = 0;
				
			if (nattrs > 0)
				{
				length = sprintf ((char*)&gTextString, 
											"    %s data set attributes for data set %ld%s",
											&formatNamePtr[1],
											dataSet,
											gEndOfLine);
				if (continueFlag)
					continueFlag = ListString ((char*)&gTextString, length, gOutputTextH);
					
				if (continueFlag)
					continueFlag = ListAttributeInformation (sdid, nattrs);
				
				}	// end "if (nattrs > 0)"
					
			else	// nattrs <= 0
				{
				length = sprintf ((char*)&gTextString, 
											"    No %s data set attributes for data set%ld%s",
											&formatNamePtr[1],
											dataSet,
											gEndOfLine);
				
				if (continueFlag)
					continueFlag = ListString ((char*)&gTextString, length, gOutputTextH);
				
				}	// end "else nattrs <= 0"
					
			if (continueFlag)
				continueFlag = ListString (gEndOfLine, 
														gNumberOfEndOfLineCharacters, 
														gOutputTextH);
														
			}	// end "for (dataSet=dataSetStart; dataSetStart<=dataSetEnd; dataSet++)"
		
		}	// end "if (file_id != FAIL)"
		
	CheckAndUnlockHandle (fileInfoPtr->hdfHandle);
		
	return (continueFlag);
	
}	// end "ListHDF4DataSetAttributes"
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadNonBSQOffsetBytesInfo
//
//	Software purpose:	This routine reads the file specifications from a HDF formatted 
//							file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadHDFInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/10/2005
//	Revised By:			Larry L. Biehl			Date: 06/12/2006

Boolean LoadNonBSQOffsetBytesInfo (
				FileInfoPtr							fileInfoPtr,
				HdfDataSets*						hdfDataSetsPtr,
				SInt16								groupNumber)
				
{
	UInt32								channel,
											index;
											
	Boolean								continueFlag = TRUE;
	

	if (fileInfoPtr != NULL)
		{
				// Get the storage for the heirarchal information.
				
		fileInfoPtr->hfaHandle = InitializeHierarchalFileStructure (
																NULL,
																fileInfoPtr,
																(UInt32)fileInfoPtr->numberChannels);
						
				// Get pointer to the hfa structure.

		fileInfoPtr->hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
																					fileInfoPtr->hfaHandle,
																					kLock);
		
		if (fileInfoPtr->hfaPtr == NULL)
			continueFlag = FALSE;
		
		if (continueFlag)
			{
			channel = 0;
			
			for (index=1; index<=fileInfoPtr->numberHdfDataSets; index++)
				{
				if (hdfDataSetsPtr[index].groupedNumber == groupNumber)
					{
							// Load the byte offset to the start of each of the channels
																
					fileInfoPtr->hfaPtr[channel].layerOffsetBytes = 
															hdfDataSetsPtr[index].dataSetOffsetBytes;
														
					channel++;
					
					if (channel >= fileInfoPtr->numberChannels)
						break;
					
					}	// end "if (hdfDataSetsPtr[index].groupedNumber == groupNumber)"
					
				}	// end "for (index=1; channel<=fileInfoPtr->..."
			
			}	// end "if (continueFlag)"
				
		CheckAndUnlockHandle (fileInfoPtr->hfaHandle);
		fileInfoPtr->hfaPtr = NULL;
		
		}	// end "if (fileInfoPtr != NULL)"
		
	return (continueFlag);
	
}	// end "LoadNonBSQOffsetBytesInfo"
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 LoadHdfDataSetNames
//
//	Software purpose:	This routine reads the file specifications from
//							a HDF formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadHDFInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/27/2001
//	Revised By:			Larry L. Biehl			Date: 09/04/2018

SInt32 LoadHdfDataSetNames (
				SInt32		 						file_id,
				SInt16								format,
				HdfDataSets*						hdfDataSetsPtr,
				SInt32								numDataSets,
				CMFileStream*						extensionFileStreamPtr,
				SInt16								instrumentCode,
				SInt32*								firstDataSetIndexPtr)
				
{
	UInt8									extensionName[80],
											firstExtensionName[80],
											//fileName[80],
											groupName[16],
											name[80],
											suffix[16];
											
	double								noDataValue,
											set0NoDataValue;
	
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									dims[10],
   											nattrs = 0,
   											numberType,
   											rank;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								dims[10],
   											nattrs = 0,
   											numberType,
   											rank;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								index,
   										dataSet,
   										numChars,
   										returnCode,
   										sdid,
   										set0DataSet,
   										suffixNumber;
   										
   UInt32								dataSetOffsetBytes,
   										groupNumber,
   										numberChannels,
											numberColumns,
											numberLines,
											setNumberForGroup,
											set0NumberColumns,
											set0NumberLines;
											
	SInt16								errCode;
   
	UInt16								bandInterleaveFormat,
											dataCompressionCode,
											dataTypeCode,
											numberBytes,
											set0DataCompressionCode,
											set0DataTypeCode,
											set0NumberBytes;

   Boolean								//checkForGroupFlag,
											differentExtensionFileFlag,
   										doNotUseHDFLineReadFlag,
											noDataValueFlag,
											signedDataFlag,
   										swapBytesFlag,
											set0NoDataValueFlag,
   										set0SignedDataFlag,
   										set0SwapBytesFlag;
   
  
	#if defined multispec_win
		USES_CONVERSION;
	#endif

   index = 1;
   errCode = noErr;
   *firstDataSetIndexPtr = 0;
   extensionName[0] = 0;
   firstExtensionName[0] = 0;
   differentExtensionFileFlag = FALSE;
	//checkForGroupFlag = TRUE;
   setNumberForGroup = 0;
   groupNumber = 0;
	doNotUseHDFLineReadFlag = FALSE;
   
   		// Get name of file that will be used as prefix to the data set group names.
   /*		
	strcpy (&fileName[1], (char*)&hdfDataSetsPtr[0].name[1]);
	RemoveSuffix (&fileName[1]);
	fileName[0] = strlen (&fileName[1]);
   */
	if (instrumentCode == kASTER)
		{
		hdfDataSetsPtr[numDataSets+1].groupedNumber = 0;
		hdfDataSetsPtr[numDataSets+2].groupedNumber = 0;
		hdfDataSetsPtr[numDataSets+3].groupedNumber = 0;
		
		}	// end "if (instrumentCode == kASTER)"
	
	for (dataSet=0; dataSet<numDataSets; dataSet++)
		{
		if (gFromToolParameterFileFlag)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)(dataSet+1));
			CheckSomeEvents (osMask+updateMask);
			
			}	// end "if (gFromToolParameterFileFlag)"

		sdid = SDselect (file_id, dataSet);
		
		hdfReturn = GetHDFDataSetInformation (sdid,
															format,
															&returnCode,
															&numberChannels,
															&numberLines,
															&numberColumns,
															&dataSetOffsetBytes,
															&numberBytes,
															&dataTypeCode,
															&dataCompressionCode,
															&bandInterleaveFormat,
															&signedDataFlag,
															&swapBytesFlag,
															&doNotUseHDFLineReadFlag,
															extensionName,
															&noDataValueFlag,
															&noDataValue);	
		/*	
		hdfReturn = GetHDFDataSetInformation (sdid,
															&returnCode,
															NULL,
															NULL,
															NULL,
															&dataSetOffsetBytes,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															extensionName);
		*/			
		if (hdfReturn != FAIL)
			{
			rank = 10;	// Indicate what the max length of dims is.
			
					// Now get the name of the data set.
					
			hdfReturn = SDgetinfo (sdid, (char*)name, &rank, dims, &numberType, &nattrs);
			
			if (hdfReturn != FAIL)
				{
						// Make sure that data set file is available.
						
				if (returnCode == noErr && extensionName[0] != 0)
					{
							// Save the extension file name to compare against any other 
							// extension names. Note that if there is any extension file 
							// name then the data sets cannot be grouped.  It makes it too 
							// complex to handle extension name for grouped data sets.  The 
							// data sets will need to be linked instead. There has to be a 
							// fileInfo structure for each data set.
					
					if (dataSet == 0)		
						CopyPToP (firstExtensionName, extensionName);
					
					else	// end "dataSet >= 0"
						{
						if (strncmp ((char*)firstExtensionName, 
											(char*)extensionName, 
											extensionName[0]) != 0)
							differentExtensionFileFlag = TRUE;
						
						}	// end "else "dataSet >= 0"
					
							// Set the new name for the file in the file information 
							// structure
						
					SetCFileName (extensionFileStreamPtr, (FileStringPtr)extensionName);
			
							// Now try opening this file up to see if it exists.
					
					returnCode = OpenFileReadOnly (extensionFileStreamPtr, 
																kResolveAliasChains, 
																kLockFile, 
																kVerifyFileStream);
															
					CloseFile (extensionFileStreamPtr);
															
					}	// end "if (returnCode == noErr && extensionName[0] != 0)"
															
				if (returnCode == noErr)
					{
					hdfDataSetsPtr[index].name[1] = '*';
					
					if (*firstDataSetIndexPtr == 0)
   					*firstDataSetIndexPtr = index;
					
					}	// end "if (returnCode == noErr)"
					
				else	// returnCode != noErr
					hdfDataSetsPtr[index].name[1] = '_';
					
						// Make sure that the name is not more than 73 characters to allow 
						// for * or _, c-string end, pascal string length character, Group 
						// number (_Gxx)
						
				name[73] = 0;
					
				strcpy ((char*)&hdfDataSetsPtr[index].name[2], (char*)name);
				hdfDataSetsPtr[index].name[0] = 
												strlen ((char*)&hdfDataSetsPtr[index].name[1]);
				/*
				hdfDataSetsPtr[index].name[0] = CreateDataSetIdentifierName (
																fileName,
																name,
																(char*)&hdfDataSetsPtr[index].name[2],
																FALSE) + 1;
				*/
				hdfDataSetsPtr[index].vRefNum = hdfDataSetsPtr[0].vRefNum;
				hdfDataSetsPtr[index].dirID = hdfDataSetsPtr[0].dirID;
				hdfDataSetsPtr[index].sdid = (void*)sdid;
				
				hdfDataSetsPtr[index].dataSetOffsetBytes = dataSetOffsetBytes;
				
				hdfDataSetsPtr[index].dataSet = (SInt16)dataSet;
				hdfDataSetsPtr[index].dataSetType = 3;
				hdfDataSetsPtr[index].instrumentChannelNumber = 0;
				
						// Handle data set grouping
				
				hdfDataSetsPtr[index].groupedNumber = 0;
				if (instrumentCode == kASTER)
					{
					numChars = 0;
					if (strstr ((char*)name, "ImageData") != NULL)
						numChars = sscanf ((char*)name, "ImageData%s", suffix);
														
					else if (strstr ((char*)name, "Band") != NULL)
						numChars = sscanf ((char*)name, "Band%s", suffix);
														
					if (numChars <= 0 || numChars > 2)
						numChars = 0;
					
					if (numChars > 0)
						{
								// Handle 3B and 3N suffix case to be ready to convert to a 
								// number.
								
						if (suffix[1] == 'N')
							suffix[1] = 0;
						
								// Treat channel 3B as a separate channel.
								
						if (suffix[1] == 'B')
							suffix[0] = 0;
						
								// Now set the group information parameters.
								// 	groupedNumber is the number of channels in the group.
								//		instrumentChannelNumber is the ASTER channel number for 
								//			the first channel in the group. 
						
						suffixNumber = 0;
						sscanf ((char*)suffix, "%d", &suffixNumber);
						
						if (suffixNumber >= 1 &&
									suffixNumber <= 3 &&
												numDataSets-dataSet >= 3 &&
															hdfDataSetsPtr[numDataSets+1].groupedNumber == 0)
							{
							hdfDataSetsPtr[numDataSets+1].name[0] =
									sprintf ((char*)&hdfDataSetsPtr[numDataSets+1].name[1],
									" VNIR");
							
							hdfDataSetsPtr[numDataSets+1].vRefNum = 0;
							hdfDataSetsPtr[numDataSets+1].dirID = 0;
							hdfDataSetsPtr[numDataSets+1].dataSetOffsetBytes = 0;
							hdfDataSetsPtr[numDataSets+1].dataSet = (SInt16)dataSet;
							hdfDataSetsPtr[numDataSets+1].dataSetType = 0;
							hdfDataSetsPtr[numDataSets+1].groupedNumber = 3;
							hdfDataSetsPtr[numDataSets+1].instrumentChannelNumber = 1;
							
							*firstDataSetIndexPtr = index;
							
							}	// end "if (suffixNumber >= 1 && ..."
							
						switch (suffixNumber)
							{
							case 1:
								/*
								if ((numDataSets-dataSet) >= 3)
									{
									hdfDataSetsPtr[numDataSets+1].name[0] = 
											sprintf ((char*)&hdfDataSetsPtr[numDataSets+1].name[1],
											" VNIR");
									
									//hdfDataSetsPtr[numDataSets+1].name[0] =
									//		CreateDataSetIdentifierName (
									//				fileName,
									//				"VNIR",
									//				(char*)&hdfDataSetsPtr[numDataSets+1].name[1],
									//				TRUE);
									
									hdfDataSetsPtr[numDataSets+1].vRefNum = 0;
									hdfDataSetsPtr[numDataSets+1].dirID = 0;
									hdfDataSetsPtr[numDataSets+1].dataSetOffsetBytes = 0;
									hdfDataSetsPtr[numDataSets+1].dataSet = (SInt16)dataSet;
									hdfDataSetsPtr[numDataSets+1].dataSetType = 0;
									hdfDataSetsPtr[numDataSets+1].groupedNumber = 3;
									hdfDataSetsPtr[numDataSets+1].instrumentChannelNumber = 1;
									
   								*firstDataSetIndexPtr = index;
										
									}	// end "if ((numDataSets-dataSet) >= 3)"
								
								else	// if ((numDataSets-dataSet) < 3)
									hdfDataSetsPtr[numDataSets+1].groupedNumber = 0;
								*/
							case 2:
							case 3:
								if (hdfDataSetsPtr[numDataSets+1].groupedNumber == 3)
									{
									hdfDataSetsPtr[index].groupedNumber = 1;
									
											// Append indicater that data set is part of a group.
											
									AppendGroupIndicater ((char*)hdfDataSetsPtr[index].name, 1);
									/*								
									charIndex = hdfDataSetsPtr[index].name[0] + 1;
									sprintf ((char*)&hdfDataSetsPtr[index].name[charIndex], 
													"_G1");
									charIndex = hdfDataSetsPtr[index].name[0] += 3;
									hdfDataSetsPtr[index].name[charIndex+1] = 0;
									*/									
									}	// end "if (hdfDataSetsPtr[numDataSets+1]. ..."
								break;
								
							case 4:
								if ((numDataSets-dataSet) >= 6)
									{
									hdfDataSetsPtr[numDataSets+2].name[0] = sprintf (
													(char*)&hdfDataSetsPtr[numDataSets+2].name[1], 
													" SWIR");
									/*
									hdfDataSetsPtr[numDataSets+2].name[0] = 
											CreateDataSetIdentifierName (
													fileName,
													"SWIR",
													(char*)&hdfDataSetsPtr[numDataSets+2].name[1],
													TRUE);
									*/
									hdfDataSetsPtr[numDataSets+2].vRefNum = 0;
									hdfDataSetsPtr[numDataSets+2].dirID = 0;
									hdfDataSetsPtr[numDataSets+2].dataSetOffsetBytes = 0;
									hdfDataSetsPtr[numDataSets+2].dataSet = (SInt16)dataSet;
									hdfDataSetsPtr[numDataSets+2].dataSetType = 0;
									hdfDataSetsPtr[numDataSets+2].groupedNumber = 6;
									hdfDataSetsPtr[numDataSets+2].instrumentChannelNumber = 4;
										
									}	// end "if ((numDataSets-dataSet) >= 6)"
								
								else	// if ((numDataSets-dataSet) < 6)
									hdfDataSetsPtr[numDataSets+2].groupedNumber = 0;
									
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
								if (hdfDataSetsPtr[numDataSets+2].groupedNumber == 6)
									{
									hdfDataSetsPtr[index].groupedNumber = 2;
									
											// Append indicater that data set is part of a group.
											
									AppendGroupIndicater ((char*)hdfDataSetsPtr[index].name, 2);
									
									}	// end "if (hdfDataSetsPtr[numDataSets+1]. ..."
								break;
								
							case 10:
								if ((numDataSets-dataSet) >= 5)
									{
									hdfDataSetsPtr[numDataSets+3].name[0] = 
										sprintf ((char*)&hdfDataSetsPtr[numDataSets+3].name[1], 
													" TIR");
									/*
									hdfDataSetsPtr[numDataSets+3].name[0] = 
											CreateDataSetIdentifierName (
													fileName,
													"TIR",
													(char*)&hdfDataSetsPtr[numDataSets+3].name[1],
													TRUE);
									*/
									hdfDataSetsPtr[numDataSets+3].vRefNum = 0;
									hdfDataSetsPtr[numDataSets+3].dirID = 0;
									hdfDataSetsPtr[numDataSets+3].dataSetOffsetBytes = 0;
									hdfDataSetsPtr[numDataSets+3].dataSet = (SInt16)dataSet;
									hdfDataSetsPtr[numDataSets+3].dataSetType = 0;
									hdfDataSetsPtr[numDataSets+3].groupedNumber = 5;
									hdfDataSetsPtr[numDataSets+3].instrumentChannelNumber = 10;
										
									}	// end "if ((numDataSets-dataSet) >= 5)"
								
								else	// if ((numDataSets-dataSet) < 5)
									hdfDataSetsPtr[numDataSets+3].groupedNumber = 0;
									
							case 11:
							case 12:
							case 13:
							case 14:
								if (hdfDataSetsPtr[numDataSets+3].groupedNumber == 5)
									{
									hdfDataSetsPtr[index].groupedNumber = 3;
									
											// Append indicater that data set is part of a group.
											
									AppendGroupIndicater ((char*)hdfDataSetsPtr[index].name, 3);
									
									}	// end "if (hdfDataSetsPtr[numDataSets+1]. ..."
								break;
							
							}	// end "switch (suffix)"
							
						if (suffixNumber > 0)
							hdfDataSetsPtr[index].instrumentChannelNumber = 
																					(SInt16)suffixNumber;
							
						}	// end "if (numChars > 0)"
					
					}	// end "if (instrumentCode == kASTER)"
					
				else	// instrumentCode != kASTER
					{
							// Check if first 4 characters of data set name is the same as 
							// the previous data set. If so verify that the number of 
							// channels is 1 and the number of lines, columns, and data type 
							// are the same. Also be sure that if no data values exist, they 
							// are the same for any grouped data sets. If so automatically 
							// group this data set with the previous one.
							
							// Note that if the data sets are stored in different files then
							// do not group. Current the logic used may fail if the first 
							// few data sets are not stored in extension files and later 
							// ones are. Don't know if those exist. Some may get grouped 
							// before we find out that we should not group them.  But I have 
							// not seen those cases.
							
					if (setNumberForGroup > 0 && 
									hdfDataSetsPtr[index].name[1] != '_' &&
											!differentExtensionFileFlag)
						{
								// Verify that first 4 characters are the same as that for 
								// the first data set.
								
						if (strncmp ((char*)groupName, 
											(char*)&hdfDataSetsPtr[index].name[2], 
											4) == 0)
							{
									// Now verify that the number of lines, columns, data 
									// bytes and data type are the same and that the number of 
									// channels is 1. Also verify that the compression is none.
									// If so then group this data set with the first one.
									
							if (numberColumns != set0NumberColumns)
								setNumberForGroup = 0;
									
							if (numberLines != set0NumberLines)
								setNumberForGroup = 0;
									
							if (dataTypeCode != set0DataTypeCode)
								setNumberForGroup = 0;
									
							if (dataCompressionCode != set0DataCompressionCode)
								setNumberForGroup = 0;
									
							if (numberBytes != set0NumberBytes)
								setNumberForGroup = 0;
									
							if (signedDataFlag != set0SignedDataFlag)
								setNumberForGroup = 0;
									
							if (swapBytesFlag != set0SwapBytesFlag)
								setNumberForGroup = 0;
								
							if (noDataValueFlag != set0NoDataValueFlag)
								setNumberForGroup = 0;
								
							if (noDataValue != set0NoDataValue)
								setNumberForGroup = 0;
									
							if (numberChannels != 1)
								setNumberForGroup = 0;
							
							if (setNumberForGroup > 0)
								{
								if (setNumberForGroup == 1)
									{
									groupNumber++;
								
									hdfDataSetsPtr[numDataSets+groupNumber].name[0] = 
											sprintf ((char*)&hdfDataSetsPtr[
																	numDataSets+groupNumber].name[1], 
														" %s",
														groupName);
									/*
									hdfDataSetsPtr[numDataSets+groupNumber].name[0] = 
										CreateDataSetIdentifierName (
											fileName,
											groupName,
											(char*)&hdfDataSetsPtr[
																	numDataSets+groupNumber].name[1],
											TRUE);
									*/
									hdfDataSetsPtr[numDataSets+groupNumber].vRefNum = 0;
									hdfDataSetsPtr[numDataSets+groupNumber].dirID = 0;
									hdfDataSetsPtr[numDataSets+groupNumber].dataSetOffsetBytes = 
																												0;
									hdfDataSetsPtr[numDataSets+groupNumber].dataSet = 
																					(SInt16)(set0DataSet);
									hdfDataSetsPtr[numDataSets+groupNumber].dataSetType = 0;
									hdfDataSetsPtr[
											numDataSets+groupNumber].instrumentChannelNumber = 0;
									
									hdfDataSetsPtr[set0DataSet+1].groupedNumber = 
																						(SInt16)groupNumber;
								
											// Append indicater that data set is part of a group.
									
									AppendGroupIndicater (
													(char*)hdfDataSetsPtr[set0DataSet+1].name, 
													groupNumber);
											
									}	// end "if (setNumberForGroup == 1)"
								
								setNumberForGroup++;
								hdfDataSetsPtr[numDataSets+groupNumber].groupedNumber = 
																			(SInt16)setNumberForGroup;
									
								hdfDataSetsPtr[index].groupedNumber = (SInt16)groupNumber;
								
										// Append indicater that data set is part of a group.
									
								AppendGroupIndicater (
												(char*)hdfDataSetsPtr[index].name, groupNumber);
										
								}	// end "if (setNumberForGroup > 0)"
							
							}	// end "if (strncmp () == 0"
							
						else	// strncmp != 0
							setNumberForGroup = 0;
						
						}	// end "if (setNumberForGroup > 0 && ...)"
							
					if (setNumberForGroup == 0)
						{
								// Save first 4 characters of data set name
								
						strncpy ((char*)groupName, (char*)&hdfDataSetsPtr[index].name[2], 4);
						groupName[4] = 0;
						
								// Save information for first data set.
						
						set0NumberColumns = numberColumns;
						set0NumberLines = numberLines;
						set0DataTypeCode = dataTypeCode;
						set0DataCompressionCode = dataCompressionCode;
						set0NumberBytes = numberBytes;
						set0SignedDataFlag = signedDataFlag;
						set0SwapBytesFlag = swapBytesFlag;
						set0NoDataValue = noDataValue;
						set0NoDataValueFlag = noDataValueFlag;
						
						set0DataSet = dataSet;
								
						if (numberChannels == 1)
							setNumberForGroup = 1;
						
						}	// end "if (setNumberForGroup == 0)"
					
					}	// end "else instrumentCode != kASTER"
			
				index++;
				
				}	// end "if (hdfReturn != FAIL)"
			
			}	// end "if (hdfReturn != FAIL)"
		
		if (hdfReturn == FAIL)
			{
			SDendaccess (sdid);
			break;
			
			}	// end "if (hdfReturn == FAIL)"			
				
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				index = 0;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (TickCount () >= gNextTime)"
		
		}	// end "for (dataSet=0; dataSet<numDataSets; dataSet++)"
	
			// If there are no valid data sets then just point to the first data set.
				
	if (*firstDataSetIndexPtr == 0)
		*firstDataSetIndexPtr = 1;
		
	return (index-1);
    
}	// end "LoadHdfDataSetNames"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadHDF4DataSetInformation
//
//	Software purpose:	This routine reads the file specifications from
//							a HDF formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	= 0, Image data set was found, No Error
//							= 1, No image data set was found
//							= 2, Image data set was found but it is compressed.
//							= 3, Image data set was found but it is 4-byte integer.
//							= 4, There are no more image data sets in the file.
//
//							Parameter vector description. Not sure how consistant this now is
//							across the several hdf data sets. This is what I have as of
//							6/30/2011
//								parameter[0]: equatorial radius
//								parameter[1]: polar radius
//								parameter[2]: scale factor
//								parameter[3]: ....
//								parameter[4]: central meridian
//								parameter[5]: latitude of origin
//								parameter[6]: false easting
//								parameter[7]: false northing
//								parameter[8]: standard parallel1
//								parameter[9]: standard parallel2
//								parameter[10]: ....
//								parameter[11]: ....
//								parameter[12]: ....
//
// Called By:			FileSpecificationDialogSetHDFValues in SOpenDlg.cpp
//							LoadHDFInformation in SReadHDFHeader.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/26/1995
//	Revised By:			Larry L. Biehl			Date: 07/30/2018

SInt16 LoadHDF4DataSetInformation (
				SInt32								file_id,
				HdfDataSets*						hdfDataSetsPtr,
				FileInfoPtr 						fileInfoPtr,
				SInt16								format,
				SInt32								dataSetIndex,
				Boolean								useGroupedDataSetsFlag)
				
{
	UInt8									dataSetName[80],
											extensionName[80],
											suffix[80];
											
	double								lowerRightCoordinates[2],
											noDataValue,
											projectionParameters[13],
											spatialResolution[3],
											upperLeftImage[2];
	
	CMFileStream*						fileStreamPtr;
	
	intn 									hdfReturn;
	
   SInt32 								returnCode,
   										sdid;
   										
	UInt32								numberChannels,
											numberLines,
											numberColumns,
											dataSetOffset;
   										
   SInt16								dataSet,
   										errCode,
   										groupNumber,
   										numChars,
   										numberDataSets,
   										numberInGroup;
   										
	UInt16								bandInterleaveFormat,
											dataCompressionCode,
											dataTypeCode,
											numberBytes;
	
	Boolean								checkUpperLeftOffsetFlag,
											continueFlag,
											convertCenterFromLatLongToMapFlag,
											convertLowerLeftFromLatLongToMapFlag,
											convertUpperLeftFromLatLongToMapFlag,
											doNotUseHDFLineReadFlag,
											noDataValueFlag,
											signedDataFlag,
											swapBytesFlag;
   

	#if defined multispec_win
		USES_CONVERSION;
	#endif

   continueFlag = TRUE;
   errCode = noErr;
   returnCode = 1;
	numberDataSets = fileInfoPtr->numberHdfDataSets;
	groupNumber = 0;
	numberInGroup = 0;
	hdfReturn = SUCCEED;
	doNotUseHDFLineReadFlag = FALSE;
	
   extensionName[0] = 0;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
   
	if (file_id == 0)
		{
				// Make sure that the working volume is set correctly
				// to look up a new file.
				
		file_id = fileInfoPtr->hdf4FileID;
		
		}	// end "if (file_id == 0)"
		
	if (file_id != FAIL)
		{
		dataSet = hdfDataSetsPtr[dataSetIndex].dataSet;
		
				// Now determine if this is part of a group of hdf data sets.
						
		if (useGroupedDataSetsFlag && hdfDataSetsPtr[dataSetIndex].groupedNumber > 0)	
			{
					// Force to read information from the first data set in the group.
					//		Decided not to do this for now (3/11/2005)
					
			groupNumber = hdfDataSetsPtr[dataSetIndex].groupedNumber;
			
			numberInGroup = hdfDataSetsPtr[numberDataSets+groupNumber].groupedNumber;
				
			}	// end "if (useGroupedDataSetsFlag && ...)"
		
		//sdid = (SInt32)hdfDataSetsPtr[dataSetIndex].sdid;
		sdid = GetSdidValue (&hdfDataSetsPtr[dataSetIndex]);
	
		if (sdid == FAIL)
			hdfReturn = FAIL;
			
		if (hdfReturn != FAIL)
			hdfReturn = GetHDFDataSetInformation (sdid,
																format,
																&returnCode,
																&numberChannels,
																&numberLines,
																&numberColumns,
																&dataSetOffset,
																&numberBytes,
																&dataTypeCode,
																&dataCompressionCode,
																&bandInterleaveFormat,
																&signedDataFlag,
																&swapBytesFlag,
																&doNotUseHDFLineReadFlag,
																extensionName,
																&noDataValueFlag,
																&noDataValue);	
		
		if (hdfReturn != FAIL && 
				(returnCode == noErr || returnCode == 2 || returnCode == 20))
			{	
			if (extensionName[0] != 0)
				{
						// Set the new name for the file in the file information structure
					
				SetCFileName (fileInfoPtr, (FileStringPtr)extensionName);
		
						// Now open this file up.
				
				errCode = OpenFileReadOnly (fileStreamPtr, 
														kResolveAliasChains, 
														kLockFile, 
														kVerifyFileStream);
				
				}	// end "if (hdfReturn != FAIL && extensionName[0] != 0)"
				
					// Load the number of channels.  It will be needed if 
					// LoadNonBSQOffsetBytesInfo is called..
				
			if (groupNumber > 0)
				numberChannels = numberInGroup;
			fileInfoPtr->numberChannels = (UInt16)numberChannels;
			
					// Load the number of offset bytes.
			
			if (groupNumber == 0)	
				fileInfoPtr->numberHeaderBytes = (UInt32)dataSetOffset;
			
			else	// groupNumber != 0
				continueFlag = LoadNonBSQOffsetBytesInfo (fileInfoPtr,
																		hdfDataSetsPtr,
																		groupNumber);
			
			fileInfoPtr->numberLines = numberLines;
			fileInfoPtr->numberColumns = numberColumns;
			fileInfoPtr->numberBytes = numberBytes;
			fileInfoPtr->signedDataFlag = signedDataFlag;
			fileInfoPtr->dataTypeCode = dataTypeCode;
			fileInfoPtr->dataCompressionCode = dataCompressionCode;
			
			fileInfoPtr->callGetHDFLineFlag = FALSE;
			if (fileInfoPtr->dataCompressionCode != kNoCompression || 
					(format == kNETCDFType && fileInfoPtr->numberChannels > 1 && 
												groupNumber == 0 && !doNotUseHDFLineReadFlag))
				{
				fileInfoPtr->callGetHDFLineFlag = TRUE;
				swapBytesFlag = FALSE;
				
				}	// end "if (fileInfoPtr->dataCompressionCode != kNoCompression || ..."
			

			//if (dataCompressionCode != kNoCompression)
			//	fileInfoPtr->dataCompressionCode = kNoCompression;
			
			if (fileInfoPtr->numberBytes == 1)
				{
				fileInfoPtr->numberBits = 8;
				fileInfoPtr->numberBins = 256;
				fileInfoPtr->swapBytesFlag = FALSE;
				
				}	// end "if (fileInfoPtr->numberBytes == 1)" 
				
			else if (fileInfoPtr->numberBytes == 2)
				{
				fileInfoPtr->numberBits = 16;
				fileInfoPtr->numberBins = 65536;
				fileInfoPtr->swapBytesFlag = swapBytesFlag;
				
				}	// end "else if (fileInfoPtr->numberBytes == 2)"
				
			else if (fileInfoPtr->numberBytes == 4)
				{
				fileInfoPtr->numberBits = 32;
				fileInfoPtr->swapBytesFlag = swapBytesFlag;
				
				if (dataTypeCode == kIntegerType)
					fileInfoPtr->numberBins = 2048;
				
				else	// dataTypeCode == kRealType)
					fileInfoPtr->numberBins = 2048;
				
				}	// end "else if (fileInfoPtr->numberBytes == 4)"
				
			else if (fileInfoPtr->numberBytes == 8)
				{
				fileInfoPtr->numberBits = 64;
				fileInfoPtr->numberBins = 2048;
				fileInfoPtr->swapBytesFlag = swapBytesFlag;
				
				}	// end "else if (fileInfoPtr->numberBytes == 8)"
					
			fileInfoPtr->asciiSymbols = FALSE;
			fileInfoPtr->thematicType = FALSE;
			
			fileInfoPtr->bandInterleave = bandInterleaveFormat;
			if (groupNumber > 0)
				fileInfoPtr->bandInterleave = kBNonSQ;
			
			if (fileInfoPtr->thematicType)
				fileInfoPtr->bandInterleave = kBIL;
			
			fileInfoPtr->numberTrailerBytes = 0;
			fileInfoPtr->numberPreLineBytes = 0;
			fileInfoPtr->numberPreChannelBytes = 0;
			fileInfoPtr->numberPostChannelBytes = 0;
			
			fileInfoPtr->treatLinesAsBottomToTopFlag = 
														GetBottomToTopFlag (format, file_id, sdid);
			
			fileInfoPtr->noDataValueFlag = noDataValueFlag;
			fileInfoPtr->noDataValue = noDataValue;
			
			if (returnCode != 2 && returnCode != 20)
				returnCode = noErr;
			
			if (returnCode == noErr)
				returnCode = SetUpHDF_FileInformation (fileInfoPtr, 
																		hdfDataSetsPtr,
																		dataSetIndex,
																		useGroupedDataSetsFlag);
				
			suffix[0] = 0;
			if (returnCode == noErr)
				{
				if (fileInfoPtr->instrumentCode == kASTER)
					{
							// Get the data set name less the starting character and 
							// any group information suffix.
					
					numChars = hdfDataSetsPtr[dataSet+1].name[0] - 1;
					strncpy ((char*)dataSetName, 
									(char*)&hdfDataSetsPtr[dataSet+1].name[2],
									numChars);
									
					if (groupNumber > 0)
						{
								// Remove the group part at end of the name. G should be
								// either the 2nd to last or 3rd to last character from 
								// the end. Also remove the '_' before the G.
								
						if (dataSetName[numChars-2] == 'G')
							dataSetName[numChars-3] = 0;
						
						else if (dataSetName[numChars-3] == 'G')
							dataSetName[numChars-4] = 0;
							
						}	// end "if (groupNumber > 0)"
					
							// Get the suffix for the data set being processed.
					
					numChars = sscanf ((char*)dataSetName, 
													"ImageData%s", 
													suffix);
													
					if (numChars <= 0 || numChars > 2)
						{
							// Check if the data set name is Band...
					
						numChars = sscanf ((char*)dataSetName, 
													"Band%s", 
													suffix);
													
						if (numChars <= 0 || numChars > 2)
							suffix[0] = 0;
							
						}	// end "if (numChars <= 0 || numChars > 2)"
						
					}	// end "if (fileInfoPtr->instrumentCode == kASTER)"
				
				MapProjectionInfoPtr				mapProjectionInfoPtr;
				SInt32								mapReturnCode;
				double								horizontalPixelSize;
				double								verticalPixelSize;
				double								xMapCoordinate11;
				double								yMapCoordinate11;
				double								mapOrientationAngle;
				SInt16								mapReferenceSystemCode;
				SInt16								mapProjectionCode;
				SInt16								mapProjectionEllipsoidCode;
				SInt16								mapProjectionDatumCode;
				SInt16								mapUnitsCode;
				SInt16								gridZone;
				SignedByte							handleStatus;
				
				if (fileInfoPtr->mapProjectionHandle == NULL)
					fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
				
				if (fileInfoPtr->mapProjectionHandle != NULL)
					{ 		
					GetHDFProjectionInformation (file_id,
															suffix,
															format,
															fileInfoPtr,
															groupNumber,
															numberInGroup,
															&mapReturnCode,
															&xMapCoordinate11,
															&yMapCoordinate11,
															&mapOrientationAngle,
															&mapReferenceSystemCode,
															&mapProjectionCode,
															&gridZone,
															&mapProjectionEllipsoidCode,
															&mapProjectionDatumCode,
															&mapUnitsCode,
															&horizontalPixelSize,
															&verticalPixelSize,
															projectionParameters,
															lowerRightCoordinates,
															spatialResolution,
															upperLeftImage,
															&convertCenterFromLatLongToMapFlag,
															&convertUpperLeftFromLatLongToMapFlag,
															&convertLowerLeftFromLatLongToMapFlag);
															
					if (mapReturnCode == noErr)
						{
						mapProjectionInfoPtr = (MapProjectionInfoPtr)
									GetHandleStatusAndPointer (
																	fileInfoPtr->mapProjectionHandle,
																	&handleStatus);
														
								// Verify that the map projection structure has been 
								// initialized.
								
						InitializeMapProjectionStructure (mapProjectionInfoPtr);
						
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
																					mapReferenceSystemCode;
						mapProjectionInfoPtr->gridCoordinate.gridZone = gridZone;
						mapProjectionInfoPtr->gridCoordinate.projectionCode = 
																						mapProjectionCode;
						  
						mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 =    
																						xMapCoordinate11;
						mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 =    
																						yMapCoordinate11;
						  
						mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
																						xMapCoordinate11;
						mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
																						yMapCoordinate11;
		                                                                               
						mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
																						horizontalPixelSize;         
						mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
																						verticalPixelSize;
		            		                                                                  
						mapProjectionInfoPtr->planarCoordinate.mapOrientationAngle = 
																						mapOrientationAngle;
																								
						mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;																		
						
						mapProjectionInfoPtr->geodetic.spheroidCode = 
																			mapProjectionEllipsoidCode;
						mapProjectionInfoPtr->geodetic.datumCode = mapProjectionDatumCode;
			
						if (mapProjectionEllipsoidCode == kUserDefinedEllipsoidCode ||
											mapProjectionEllipsoidCode == kSphereEllipsoidCode)
							{
									// Set the radius for the axes if provided. If not 
									// provided, they will be set later based on the 
									// ellipsoid code.
									
							mapProjectionInfoPtr->geodetic.radiusSpheroid = 
																				projectionParameters[0];
							mapProjectionInfoPtr->geodetic.semiMajorAxis = 
																				projectionParameters[0];
							mapProjectionInfoPtr->geodetic.semiMinorAxis = 
																				projectionParameters[1];
							
							}	// endd "if (mapProjectionEllipsoidCode == ..."
							
								// Set projection paramaters from the reference system
								
						if (!SetProjectionParametersFromReferenceSystem (
																					mapProjectionInfoPtr))
							{
							if (mapProjectionCode == kAlbersConicalEqualAreaCode)
								SetAlbersEqualAreaParameters (
													mapProjectionInfoPtr,
													projectionParameters[8],	// standardParallel1
													projectionParameters[9],	// standardParallel2
													projectionParameters[4],	// centralMeridian
													projectionParameters[5],	// latitudeOrigin
													projectionParameters[6],	// falseEasting
													projectionParameters[7]);	// falseNorthing
							
							else if (mapProjectionCode == kCylindricalEqualAreaCode)
								SetCylindricalEqualAreaParameters (
													mapProjectionInfoPtr,
													projectionParameters[0],	// radiusSpheroid
													projectionParameters[4],	// centralMeridian
													projectionParameters[8],	// standardParallel1
													projectionParameters[6],	// falseEasting
													projectionParameters[7]);	// falseNorthing
							
							else if (mapProjectionCode == kLambertConformalConicCode)
								SetLambertConformalConicParameters (
													mapProjectionInfoPtr,
													projectionParameters[8],	// standardParallel1
													projectionParameters[9],	// standardParallel2
													projectionParameters[4],	// centralMeridian
													projectionParameters[5],	// latitudeOrigin
													projectionParameters[6],	// falseEasting
													projectionParameters[7]);	// falseNorthing
																										
							else if (mapProjectionCode == kOrthographicCode)
								SetOrthographicParameters (
													mapProjectionInfoPtr,
													projectionParameters[0],	// radiusSpheroid
													projectionParameters[4],	// longitudeOrigin
													projectionParameters[5]);	// latitudeOrigin
																		
							else if (mapProjectionCode == kSinusoidalCode)
								SetSinusoidalParameters (
													mapProjectionInfoPtr,
													projectionParameters[0],	// radiusSpheroid
													projectionParameters[4],	// centralMeridian
													projectionParameters[5],	// standardParallel
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
																																					
							else if (mapProjectionCode == kTransverseMercatorCode)
								SetTransverseMercatorParameters (
									mapProjectionInfoPtr,
									projectionParameters[0],	// radiusSpheroid
									projectionParameters[4],	// longitudeCentralMeridian
									projectionParameters[5],	// latitudeOrigin
									projectionParameters[2],	// scaleFactorOfCentralMeridian
									projectionParameters[6],	// falseEasting
									projectionParameters[7]);	// falseNorthing

							else if (mapProjectionCode == kEquirectangularCode)
								SetEquirectangularParameters (
													mapProjectionInfoPtr,
													projectionParameters[0],	// radiusSpheroid
													projectionParameters[4],	// centralMeridian
													projectionParameters[5]);	// standardParallel
							/*
							else if (mapReferenceSystemCode == kUTMRSCode)
								SetUTMParametersFromZone (
									gridZone,
									&mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian,
									&mapProjectionInfoPtr->gridCoordinate.latitudeOrigin,
									&mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian,
									&mapProjectionInfoPtr->gridCoordinate.falseEasting,
									&mapProjectionInfoPtr->gridCoordinate.falseNorthing);
							*/			
							}	// end "if (!SetProjectionParametersFromReferenceSystem (mapProjectionInfoPtr))"
						
								// Make sure the map units code is set correctly
						
						if (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode == 
																							kUnknownCode &&
									mapProjectionInfoPtr->gridCoordinate.projectionCode != 
																							kNotDefinedCode)
							{										
							if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																					kStatePlaneNAD27RSCode)
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = 
																									kFeetCode;
								
							else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode
																						== kGeographicRSCode)
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = 
																						kDecimalDegreesCode;
										
							else	// ...->gridCoordinate.code != kStatePlaneCode && ...
								mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = 
																								kMetersCode;
								
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
												
										// This represents the map coordinates for the center of the pixel. Translate to
										// the upper left pixel.
										
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
																			
							if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kLambertConformalConicCode)
								{
								ConvertLatLongToLambertConformalConic (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
												
										// This represents the map coordinates for the lower 
										// left pixel. Translate to the upper left pixel.
										
								mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 +=
									(fileInfoPtr->numberLines - 1) * 
										mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
										
									
								}	// end "if (mapProjectionInfoPtr->gridCoordinate.code == ..."
								
							}	// end "if (convertLowerLeftFromLatLongToMapFlag)"
						
						if (convertUpperLeftFromLatLongToMapFlag)
							{
									// Convert the lat-long coordinates to map coordinates
							
							checkUpperLeftOffsetFlag = FALSE;												
							if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																			kAlbersConicalEqualAreaCode)
								{
								ConvertLatLongToAlbersEqualArea (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
									
								}	// end "if (mapProjectionInfoPtr->gridCoordinate.code == ..."
																	
							else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode
																					== kUTM_WGS84RSCode || 
										mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																					kTransverseMercatorCode)
								{
								ConvertLatLongToTransverseMercator (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
									
								}	// end "else if (... projectionCode == kUTMCode)"
								
							else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																							kOrthographicCode)
								{
								ConvertLatLongToOrthographic (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
												
								checkUpperLeftOffsetFlag = TRUE;
									
								}	// end "else if (... projectionCode == kOrthographicCode)"
								
							else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																						kEquirectangularCode)
								{
								ConvertLatLongToEquirectangular (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
												
								checkUpperLeftOffsetFlag = TRUE;
									
								}	// end "else if (... projectionCode == kEquirectangularCode)"
								
							else if (mapProjectionInfoPtr->gridCoordinate.projectionCode == 
																				kCylindricalEqualAreaCode)
								{
								ConvertLatLongToCylindricalEqualArea (
									mapProjectionInfoPtr,
									&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
									&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
												
								checkUpperLeftOffsetFlag = TRUE;
									
								}	// end "else if (... == kCylindricalEqualAreaCode)"
								
												
							if (checkUpperLeftOffsetFlag)
								{
							 			// If the upper left pixel is define as -0.5, -0.5, then 
										// this means that the lat-long is for the upper left 
										// corner of the upper left pixel.  MultiSpec assumes 
										// that the coordinates of the upper left pixel 
										// represents the center of the pixel.  Make the 
										// adjustment if needed.
							 			
							 	if (upperLeftImage[0] == -0.5)
							 		mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 
																					horizontalPixelSize/2;
							 			
							 	if (upperLeftImage[1] == -0.5)
							 		mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 
																						verticalPixelSize/2;
																								
								}	// end "if (checkUpperLeftOffsetFlag)"
											
									// Verify pixel spacing.  If not available, see if it can 
									// be calculated. There are some ASTER products where the 
									// pixel size is not easily determined.  
									// For now this is only set up for UTM projection.
									
							if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode >= 
																						kUTM_NAD27RSCode &&
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode <=
																							kUTM_WGS84RSCode)
								{
								if (horizontalPixelSize <= 0 || verticalPixelSize <= 0)
									{
									ConvertLatLongToTransverseMercator (
																			mapProjectionInfoPtr,
																			&lowerRightCoordinates[1], 
																			&lowerRightCoordinates[0]);
							 							
							 		if (horizontalPixelSize <= 0)
							 			{
							 			horizontalPixelSize = (lowerRightCoordinates[1] - 
							 				mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11)/
							 												(fileInfoPtr->numberColumns-1);
							 			
							 					// Find size in list that is the closet.
							 			
							 			mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
																FindClosestValue (horizontalPixelSize, 
																							spatialResolution, 
																							3);
							 																
							 			}	// end "if (horizontalPixelSize <= 0)"
							 							
							 		if (verticalPixelSize <= 0)
							 			{
							 			verticalPixelSize = 
							 				(mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -
							 									lowerRightCoordinates[0])/
							 												(fileInfoPtr->numberLines-1);
							 			
							 					// Find size in list that is the closet.
							 			
										mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
																FindClosestValue (verticalPixelSize, 
																							spatialResolution, 
																							3);  
							 																
							 			}	// end "if (verticalPixelSize <= 0)"
									
									}	// end "if (horizontalPixelSize <= 0 || verticalPixelSize <= 0)"
								
								}	// end "if (mapProjectionInfoPtr->gridCoordinate.code == kUTMCode)"
							
							}	// end "if (convertUpperLeftFromLatLongToMapFlag)"
						  
								// Make sure the map orientation origin is up to date.
								
						mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
										mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
						mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =    
										mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
						
						}	// end "if (mapReturnCode == noErr)"
						
					}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
					
				}	// end "if (returnCode == noErr)"
				
			}	// end "if (hdfReturn != FAIL && (returnCode == noErr || ..."
		/*
		if (sdid != FAIL)	
			SDendaccess (sdid);
		
		if (closeFileIdFlag)
			SDend (file_id);
		*/
		}	// end "if (file_id != FAIL)"
	
			// Close hdf globals
	/*
	if (closeFileIdFlag)
		CloseHDFGlobals ();
	*/												
	if (errCode != noErr || !continueFlag)
		returnCode = 8;

	return ((SInt16)returnCode);
    
}	// end "LoadHDF4DataSetInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 LoadHDF4Information
//
//	Software purpose:	This routine reads the file specifications from
//							a HDF formatted file.  If specifiedDataSet is greater than 0, 
//							then load the information for that data set.  This will happen 
//							when a project is opened whose base image is an hdf or netcdf 
//							formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	= 0, Image data set was found, No Error
//							= 1, No image data set was found
//							= 2, Image data set was found but it is compressed.
//							= 3, Image data set was found but it is 4-byte integer.
//							= 4, There are no more image data sets in the file.
//
// Called By:			ReadHDFHeader in SHDF.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/26/1995
//	Revised By:			Larry L. Biehl			Date: 11/21/2018

SInt16 LoadHDF4Information (
				FileInfoPtr 						fileInfoPtr,
				SInt16								format,
				Boolean								dataSetSpecifiedFlag)
				
{
	UInt8									filePathString[512];
	
	FileStringPtr						fileNamePtr,
											filePathPtr;
	
	CMFileStream						extensionFileStream;
	
   HdfDataSets*						hdfDataSetsPtr;
   
   Handle								dataSetHandle;
   
	intn 									hdfReturn;
	
	#if TARGET_RT_MAC_MACHO
		int									nglobals,
   											numDataSets;
	#else	// !TARGET_RT_MAC_MACHO
		SInt32								nglobals,
   											numDataSets;
	#endif	// TARGET_RT_MAC_MACHO, else...
	
   SInt32 								dataSetIndex,
											file_id,
											number8BitImages,
   										number24BitImages,
   										totalNumberDataSets;
   										
   SInt16								returnCode;
   
   Boolean								loadHDFDataSetFlag;
   

   dataSetHandle = NULL;
   hdfDataSetsPtr = NULL;
   
   returnCode = GetHDFFilePathCPointer (fileInfoPtr, 
														filePathString, 
														510,
														&filePathPtr,
														kHDF4_Library);
													
	if (returnCode != noErr)	
																					return (returnCode);	
	
   returnCode = 1;
   
			// Turn spin cursor on

	if (gPresentCursor < kSpin || gPresentCursor > kSpinEnd)
		{
		MSetCursor (kSpin);	
		gNextTime = TickCount ();
		
		}	// end "if (gPresentCursor < kSpin || gPresentCursor > kSpinEnd)"
	
			// Force the flag to load an exit routine to be FALSE. The exit
			// routine (PEnd) is called directly from within CloseHDFGlobals.
			
	HDdont_atexit ();

	file_id = SDstart ((char*)filePathPtr, DFACC_RDONLY);
	if (file_id == FAIL) 
		{	
				// Change cursor back to pointer.								
				
		MInitCursor ();
																								return 1;
		
		}	// end "if (file_id == FAIL)"	
	/*	
	Str255								infoString;
   UInt32								majorVersion,
											minorVersion,
											release;
																				
	hdfReturn = Hgetfileversion (file_id, 
											&majorVersion, 
											&minorVersion, 
											&release,
											(char*)&infoString);
	*/
	/*
	Str255								vGName; 
											
	SInt32								numVGEntries,
   										vgid;
   																					
	vgid = Vgetid (file_id, -1);
	if (vgid != FAIL)
		hdfReturn = Vinquire (vgid, &numVGEntries, (char*)&vGName);
	*/
	fileInfoPtr->hdf4FileID = file_id;
	gNumberHDFFilesOpen++;	
	gNumberOfOpenFiles++;
				
	if (TickCount () >= gNextTime)
		{
		if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
			{
					// Change cursor back to pointer.								
				
			MInitCursor ();
																								return 1;
			
			}	// end "if (!CheckSomeEvents (..."
			
		}	// end "if (TickCount () >= gNextTime)"
	/*
			// This is for testing.
	MInitCursor ();
																								return 1;
	*/
	hdfReturn = SDfileinfo (file_id, &numDataSets, &nglobals);
	if (hdfReturn == FAIL)
		numDataSets = 0;
	
			// Determine the instrument code if one can
	
	if (numDataSets > 0)		
		fileInfoPtr->instrumentCode = GetHDFInstrumentCode (file_id);
	/*
			// Get the number of V Groups in the file.
			// Did not find any for the ASTER files that I checked in 3/2005
	
	hdfReturn = Vstart (file_id);
	
	SInt32				idarray[1000];
	hdfReturn = Vlone (file_id, idarray, 1000);
	
	hdfReturn = Vend (file_id);
	*/
			// Determine the total number of data sets in the file.
		
	number8BitImages = DFR8nimages ((char*)filePathPtr);
	number24BitImages = DF24nimages ((char*)filePathPtr);
	
	//validDataSetCount = GetNumberOfValidDataSets (file_id, numDataSets);

	totalNumberDataSets = numDataSets;
	if (totalNumberDataSets == 0 && number8BitImages > 0)
		totalNumberDataSets = 1;
	if (totalNumberDataSets == 0 && number24BitImages > 0)
		totalNumberDataSets = 1;
	
	if (totalNumberDataSets > 0)
		{
				// The first index in the structure will be the name of the
				// header hdf file. It is needed in case the hdf file points
				// to external files for the storage of the data.
				
				// Allow for grouped information in the hdf data set.
				
		totalNumberDataSets *= 2;
				
		dataSetHandle = MNewHandle ((totalNumberDataSets+1)*sizeof (HdfDataSets));
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (dataSetHandle, kLock);
									
		if (hdfDataSetsPtr != NULL)
			{
					// Save the name of the hdf header file in case that it differs
					// from the file used to store the data in.
					
			fileNamePtr = (FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
			CopyPToP (hdfDataSetsPtr[0].name, fileNamePtr);
			hdfDataSetsPtr[0].vRefNum = GetVolumeReferenceNumber (fileInfoPtr);
			hdfDataSetsPtr[0].dirID = GetParID (fileInfoPtr);
			hdfDataSetsPtr[0].sdid = 0;
			hdfDataSetsPtr[0].dataSet = 0;
			hdfDataSetsPtr[0].dataSetType = 0;
			hdfDataSetsPtr[0].groupedNumber = 0;
			
			}	// end "if (hdfDataSetsPtr != NULL)"
										
		}	// end "if (totalNumberDataSets > 0)"
		
	if (numDataSets == 0)
		{
		if (number8BitImages > 0)
			{
			#if TARGET_RT_MAC_MACHO
				int									numberColumns,
														numberLines,
														imageOffset;
			#else	// !TARGET_RT_MAC_MACHO
				SInt32								numberColumns,
														numberLines,
														imageOffset;
			#endif	// TARGET_RT_MAC_MACHO, else...
							
			intn			paletteExists;
			
			//UInt8			palette[768];
			
			//DFRrig*		currentRigPtr;
			
			
			hdfReturn = DFR8restart ();
			
			imageOffset = 0;
			if (hdfReturn != FAIL)
				#if defined multispec_mac || defined multispec_wxmac || defined multispec_win
					hdfReturn = DFR8getdims ((char*)filePathPtr, 
														&numberColumns, 
														&numberLines, 
														&paletteExists,
														&imageOffset);
				#endif
			
				#if defined multispec_lin && !defined multispec_wxmac
					hdfReturn = DFR8getdims ((char*)filePathPtr, 
														&numberColumns, 
														&numberLines, 
														&paletteExists);
				#endif
			/*
			if (hdfReturn != FAIL && paletteExists)									
				hdfReturn = DFR8getimage ((char*)&filePathPtr[1], 
												NULL, 
												numberColumns, 
												numberLines, 
												palette);
			*/
			if (hdfReturn != FAIL)
				{
				fileInfoPtr->numberHeaderBytes = (UInt32)imageOffset;
				fileInfoPtr->numberChannels = 1;
				fileInfoPtr->numberLines = numberLines;
				fileInfoPtr->numberColumns = numberColumns;
				fileInfoPtr->numberBytes = 1;
				fileInfoPtr->signedDataFlag = FALSE;
				
				fileInfoPtr->numberBits = 8;
				fileInfoPtr->numberBins = 256;
				fileInfoPtr->swapBytesFlag = FALSE;
					
				fileInfoPtr->asciiSymbols = FALSE;
				fileInfoPtr->thematicType = FALSE;
				fileInfoPtr->bandInterleave = kBIL;
			
				fileInfoPtr->numberTrailerBytes = 0;
				fileInfoPtr->numberPreLineBytes = 0;
				fileInfoPtr->numberPreChannelBytes = 0;
				fileInfoPtr->numberPostChannelBytes = 0;
				
				hdfDataSetsPtr[1].name[1] = '*';	
				strcpy (
					(char*)&hdfDataSetsPtr[1].name[2], (char*)&hdfDataSetsPtr[0].name[1]);
				hdfDataSetsPtr[1].name[0] = 
												(UInt8)strlen ((char*)&hdfDataSetsPtr[1].name[1]);
				hdfDataSetsPtr[1].vRefNum = hdfDataSetsPtr[0].vRefNum;
				hdfDataSetsPtr[1].dirID = hdfDataSetsPtr[0].dirID;
				hdfDataSetsPtr[1].sdid = 0;
				
				hdfDataSetsPtr[1].dataSet = (SInt16)0;
				hdfDataSetsPtr[1].dataSetType = 1;
				hdfDataSetsPtr[1].groupedNumber = 0;
				
						// Take into account other items on popup menu.
						
				dataSetIndex = 1;
				
				returnCode = noErr;
				
				}	// end "if (hdfReturn != FAIL)"
				
			else	// hdfReturn == FAIL
  				returnCode = 6;
			
			}	// end "if (number8BitImages > 0)"

		else if (number24BitImages > 0)
			{
			//returnCode = 6;
	
			#if TARGET_RT_MAC_MACHO
				int									numberColumns,
														numberLines,
														imageOffset = 0;
			#else	// !TARGET_RT_MAC_MACHO
				SInt32								numberColumns,
														numberLines,
														imageOffset = 0;
			#endif	// TARGET_RT_MAC_MACHO, else...
							
			intn									interlaceCode;
   		uint16		    					compressionType;
			
			
			hdfReturn = DF24restart ();

			imageOffset = 0;
			compressionType = 0;
			if (hdfReturn != FAIL)
				#if defined multispec_mac || defined multispec_wxmac || defined multispec_win
					hdfReturn = DF24getdims ((char*)filePathPtr, 
													&numberColumns, 
													&numberLines, 
													&interlaceCode,
													&compressionType,
													&imageOffset);
				#endif
			
				#if defined multispec_lin && !defined multispec_wxmac
					hdfReturn = DF24getdims ((char*)filePathPtr, 
													&numberColumns, 
													&numberLines, 
													&interlaceCode);
				#endif
												
			if (compressionType != COMP_CODE_NONE)
				returnCode = 2;
			/*
			if (hdfReturn != FAIL && paletteExists)									
				hdfReturn = DFR24getimage ((char*)&filePathPtr[1], 
													NULL, 
													numberColumns, 
													numberLines, 
													palette);
			*/
			if (hdfReturn != FAIL && returnCode != 2)
				{
				fileInfoPtr->numberHeaderBytes = (UInt32)imageOffset;
				fileInfoPtr->numberChannels = 3;
				fileInfoPtr->numberLines = numberLines;
				fileInfoPtr->numberColumns = numberColumns;
				fileInfoPtr->numberBytes = 1;
				fileInfoPtr->signedDataFlag = FALSE;
				
				fileInfoPtr->numberBits = 8;
				fileInfoPtr->numberBins = 256;
				fileInfoPtr->swapBytesFlag = FALSE;
					
				fileInfoPtr->asciiSymbols = FALSE;
				fileInfoPtr->thematicType = FALSE;
				
				if (interlaceCode == DFIL_PIXEL)
					fileInfoPtr->bandInterleave = kBIS;
					
				else if (interlaceCode == DFIL_LINE)
					fileInfoPtr->bandInterleave = kBIL;
					
				else	// interlaceCode == DFIL_PLANE
					fileInfoPtr->bandInterleave = kBSQ;
			
				fileInfoPtr->numberTrailerBytes = 0;
				fileInfoPtr->numberPreLineBytes = 0;
				fileInfoPtr->numberPreChannelBytes = 0;
				fileInfoPtr->numberPostChannelBytes = 0;
				
				hdfDataSetsPtr[1].name[1] = '*';	
				strcpy (
					(char*)&hdfDataSetsPtr[1].name[2], (char*)&hdfDataSetsPtr[0].name[1]);
				hdfDataSetsPtr[1].name[0] = 
											(UInt8)strlen ((char*)&hdfDataSetsPtr[1].name[1]);
				hdfDataSetsPtr[1].vRefNum = hdfDataSetsPtr[0].vRefNum;
				hdfDataSetsPtr[1].dirID = hdfDataSetsPtr[0].dirID;
				hdfDataSetsPtr[1].sdid = 0;
				
				hdfDataSetsPtr[1].dataSet = (SInt16)0;
				hdfDataSetsPtr[1].dataSetType = 1;
				hdfDataSetsPtr[1].groupedNumber = 0;
				
						// Take into account other items on popup menu.
						
				dataSetIndex = 1;
				
				returnCode = noErr;
				
				}	// end "if (hdfReturn != FAIL)"
				
			else	// hdfReturn == FAIL || returnCode == 2
				{
				if (returnCode != 2)
  					returnCode = 6;
  					
  				}	// end "else hdfReturn == FAIL || returnCode == 2"
			
			}	// end "if (number8BitImages > 0)"
  			
  		}	// end "if (numDataSets == 0)"
									
	if (hdfDataSetsPtr != NULL && numDataSets > 0)
		{	
				// Get a copy of the input file stream pointer so that it
				// can be used to check if any extension files exist in the
				// same folder as the hdf header file.
				
		CopyFileStream (&extensionFileStream, GetFileStreamPointer (fileInfoPtr));
		IndicateFileClosed (&extensionFileStream);
		
		numDataSets = LoadHdfDataSetNames (file_id, 
														format,
														hdfDataSetsPtr, 
														numDataSets,
														&extensionFileStream,
														fileInfoPtr->instrumentCode,
														&dataSetIndex);
														
		totalNumberDataSets = numDataSets;
														
		}	// end "if (hdfDataSetsPtr != NULL && numDataSets > 0)"
	/*		
	if (numDataSets > 0 && nglobals == 1)
		{
				// Read global attribute.
				
		hdfReturn = SDattrinfo (file_id, 0, name, &numberType, &size);
		if ((strcmp (name, "Date")) && (numberType == DFNT_CHAR8) && (size == DATESIZE))
			SDreadattr (file_id, 0, date);
			
		}	// end "if (numDataSets > 0 && nglobals == 1)"
	*/
	if (hdfDataSetsPtr != NULL && numDataSets > 0)
		{
		fileInfoPtr->numberHdfDataSets = (UInt16)totalNumberDataSets;
		
				// Load the information for the specified data set.
		
		loadHDFDataSetFlag = TRUE;		
		if (dataSetSpecifiedFlag)	
			{
					// Verify that the selection makes sense relative to the hdf file
					// being read.  A data set is previously specified if the base image 
					// for a project file is being loaded.
			
			if (fileInfoPtr->hdfDataSetSelection > 0 && 
						fileInfoPtr->hdfDataSetSelection <= fileInfoPtr->numberHdfDataSets)		
				dataSetIndex = fileInfoPtr->hdfDataSetSelection;
				
			else	// ...->hdfDataSetSelection <= 0 || ...
				{
				loadHDFDataSetFlag = FALSE;
				returnCode = 3;
					
				}	// end "else ...->hdfDataSetSelection <= 0 || ..."
			
			}	// end "if (dataSetSpecifiedFlag)"
		
		if (loadHDFDataSetFlag)
			{
			hdfReturn = LoadHDF4DataSetInformation (file_id,
																	hdfDataSetsPtr,
																	fileInfoPtr,
																	format,
																	dataSetIndex,
																	TRUE);
																	
			if (hdfReturn != FAIL)
				returnCode = noErr;
				
			}	// end "if (loadHDFDataSetFlag)"
			
		}	// end "if (hdfDataSetsPtr != NULL && numDataSets > 0)"
																
	if (returnCode == noErr)
		{
		fileInfoPtr->hdfHandle = dataSetHandle;
		
				// Take into account other items on popup menu.
		
		fileInfoPtr->hdfDataSetSelection = (UInt16)dataSetIndex;
		
				// Signify that file is of "HDF" type. 									
				
		CheckAndUnlockHandle (dataSetHandle);
			
		}	// end "if (hdfReturn != FAIL)"
		
	else	// returnCode != noErr
		{
		fileInfoPtr->numberHdfDataSets = 0;
		UnlockAndDispose (dataSetHandle);
			
		}	// end "else returnCode ! = noErr"
				
	//SDend (file_id);
	
			// Close hdf globals
	
	//CloseHDFGlobals ();
					
			// Change cursor back to pointer. Do not change back if a series of 
			// image files are being read. It will be set back later.								
				
	if (gLinkOptionsSelection == 1)
		MInitCursor ();

	return (returnCode);
    
}	// end "LoadHDF4Information"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void RemoveTrailingCharacters
//
//	Software purpose:	This routine removes some trailing characters from the input text
//							field that has just been read from an HDF attibute list.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/03/2005
//	Revised By:			Larry L. Biehl			Date: 03/03/2005

void RemoveTrailingCharacters (
				UCharPtr								stringPtr)
				
{	
	SInt32								length;
	
	
	length = (SInt32)strlen ((char*)stringPtr);
	if (length > 0)
		{
		while (stringPtr[length-1] == ',' || 
								stringPtr[length-1] == '"' ||
												stringPtr[length-1] == ')')
			{
			length--;
			
			if (length <= 0)
				break;
					
			}	// end "while (stringPtr[length-1] == ..."
			
		}	// end "if (length > 0)"
		
	stringPtr[length] = 0;
			
}	// end "RemoveTrailingCharacters



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetHDF4HasHFSPlusAPISetting
//
//	Software purpose:	Set the hasHFSPlusAPIxFlag for the hdf4 routines (in hfile.c)
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2014
//	Revised By:			Larry L. Biehl			Date: 01/05/2015

void SetHDF4HasHFSPlusAPISetting (
				Boolean								hasHFSPlusAPIsFlag)
				
{
	#if defined multispec_mac
		SetHDFHasHFSPlusAPISetting (hasHFSPlusAPIsFlag);
	#endif
			
}	// end "SetHDF4HasHFSPlusAPISetting


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpHDF_FileInformation
//
//	Software purpose:	This sets up the vector that is used to relate image channels
//							to the hdf scientific data set access values.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/2007
//	Revised By:			Larry L. Biehl			Date: 04/27/2012

SInt16 SetUpHDF_FileInformation (
				FileInfoPtr							fileInfoPtr,
				HdfDataSets*						hdfDataSetsPtr,
				SInt32								hdfDataSetSelection)
				
{	
	UInt16*								channelToHdfDataSetPtr;
	
	UInt32								channelIndex,
											dataSetIndex;
	
	SInt16								groupedNumber,
											returnCode = noErr;
	
	
	if (fileInfoPtr->callGetHDFLineFlag)
		{
		if (hdfDataSetsPtr == NULL)
																								return (1);
			
				// Get vectors for the sdids.
				// Get new handle if for channel to hdf data set structure if needed
	
		fileInfoPtr->channelToHdfDataSetHandle = MNewHandle (
												fileInfoPtr->numberChannels * sizeof (UInt16));
		if (fileInfoPtr->channelToHdfDataSetHandle == NULL)
																								return (1);
		
		
		channelToHdfDataSetPtr = (UInt16*)GetHandlePointer (
															fileInfoPtr->channelToHdfDataSetHandle);
		
				// Get the group number for the data set.
				
		groupedNumber = hdfDataSetsPtr[hdfDataSetSelection].groupedNumber;
		
		if (groupedNumber > 0)
			{
					// Now find the data sets for each of the channels in the group.
					// Note that 'hdfDataSetSelection' may not point to the first channel
					// in the group so go through the entire list of data sets.
			
			channelIndex = 0;
			for (dataSetIndex=1; 
						dataSetIndex<=fileInfoPtr->numberHdfDataSets; 
						dataSetIndex++) 
				{
				if (groupedNumber == hdfDataSetsPtr[dataSetIndex].groupedNumber)
					{
							// Verify that we have a valid sdid for the data set.
						
					if (hdfDataSetsPtr[dataSetIndex].sdid > 0)
						channelToHdfDataSetPtr[channelIndex] = (UInt16)dataSetIndex;
					
					else	// hdfDataSetsPtr[dataSetIndex].sdid <= 0
						{
						returnCode = 1;
						break;
							
						}	// end "else hdfDataSetsPtr[dataSetIndex].sdid <= 0"
						
					channelIndex++;
					
					}	// end "if (groupedNumber == ..."
					
				if (groupedNumber < hdfDataSetsPtr[dataSetIndex].groupedNumber)
							// No reason to continue checking; last data set for group was 
							// found.
					break;
				
				}	// end "for (dataSetIndex=1; dataSetIndex<=...->numberHdfDataSets; ..."
				
			}	// end "if (groupedNumber > 0)"
			
		else	// groupedNumber = 0
			{
			dataSetIndex = hdfDataSetSelection;
			if (hdfDataSetsPtr[dataSetIndex].sdid > 0)
				{
						// All channels belong to the same data set (and therefore sdid)
						
				for (channelIndex=0; 
							channelIndex<fileInfoPtr->numberChannels; 
							channelIndex++) 
					channelToHdfDataSetPtr[channelIndex] = (UInt16)dataSetIndex;
				
				}	// end "if (hdfDataSetsPtr[dataSetIndex].sdid > 0)"
				
			else	// hdfDataSetsPtr[dataSetIndex].sdid <= 0
				returnCode = 7;
			
			}	// end "else groupedNumber = 0"
			
		}	// end "if (fileInfoPtr->callGetHDFLineFlag)"
		
	return (returnCode);
    
}	// end "SetUpHDF_FileInformation"
*/



#if defined multispec_lin && !defined multispec_wxmac
// Note that these files are not defined in the linux hdf library
/******************************************************************************
 NAME
	SDgetoffset -- get the starting offset for dataset

 DESCRIPTION
    The user is repsonsible for allocating space to hold
    the dataset name.  It can be at most MAX_NC_NAME 
    characters in length.  NULL can be passed for the name
    if it is not required.

    dimsizes should be an array to hold the dimension sizes
    a dataset can have at most MAX_VAR_DIMS dimensions.

 RETURNS
        SUCCEED / FAIL

******************************************************************************/ 
intn SDgetoffset (
						// IN:  dataset ID
				int32  								sdsid,   
						// OUT: starting offset for the dataset
				int32*								dataSetOffsetPtr)
{
	NC*     								handle = NULL;
	NC_var* 								var = NULL;
	intn									ret_value = SUCCEED;

	#ifdef SDDEBUG
		fprintf (stderr, "SDgetoffset: I've been called\n");
	#endif

	#ifdef HAVE_PABLO
		TRACE_ON (PABLO_mask, ID_SDgetoffset);
	#endif

	handle = SDIhandle_from_id ((int)sdsid, SDSTYPE);
	if (handle == NULL) 
		{
		ret_value = FAIL;
		goto done;
		
		}

	if (handle->vars == NULL)
		{
		ret_value = FAIL;
		goto done;
		
		}

	var = SDIget_var (handle, sdsid);
	if (var == NULL)
		{
		ret_value = FAIL;
		goto done;
		
		}
		
	if (var->cdf->file_type == netCDF_FILE)
		{
		*dataSetOffsetPtr = var->begin;
		goto done;
		
		}	// end "if ((var->cdf->file_type == netCDF_FILE)"
		
	if (var->aid == FAIL && hdf_get_vp_aid (handle, var) == FAIL) 
		{
		ret_value = FAIL;
		goto done;
		
		}

	if (HQueryoffset (var->aid, dataSetOffsetPtr) == FAIL)
		{
		ret_value = FAIL;
		goto done;
		
		}

	#ifdef SDDEBUG
		fprintf (stderr, "SDgetoffset: looked up handle as %d\n", handle);
	#endif

	done:
		if (ret_value == FAIL)
			{ /* Failure cleanup */
			*dataSetOffsetPtr = 0;

			}

    		/* free the AID in case it was allocated. */
    
	SDIfreevarAID (handle, sdsid & 0xffff);
			
    /* Normal cleanup */

	#ifdef HAVE_PABLO
		TRACE_OFF (PABLO_mask, ID_SDgetoffset);
	#endif

	return ret_value;
	
} /* SDgetoffset */


/******************************************************************************
 NAME
	SDIhandle_from_id -- get the handle from this object

 DESCRIPTION
    Map an ID to the handle for this file

 RETURNS
    NULL or the handle

******************************************************************************/
NC* SDIhandle_from_id (int32 id, /* IN: an object (file, dim, dataset) ID */
                  intn  typ /* IN: IN: the type of ID this is */)
{
    int32 tmp;
    NC   *ret_value = NULL;

    /* check that it is the proper type of id */
    tmp = (id >> 16) & 0x0f;
    if (tmp != typ)
      {
        ret_value = NULL;
        goto done;
      }

    /* get the file from top 8 bits*/
    tmp = (id >> 20) & 0xfff;
    ret_value = NC_check_id((int)tmp);

done:
    if (ret_value == NULL)
      { /* Failure cleanup */

      }
    /* Normal cleanup */

    return ret_value;
	 
} /* SDIhandle_from_id */


/******************************************************************************
 NAME
	SDIget_var -- get the variable record

 DESCRIPTION
    Map an ID and a handle to the NC_var object for this dataset

 RETURNS
    NULL or the variable object

******************************************************************************/
NC_var* SDIget_var (
				NC*							handle, /* IN: the handle for this file */
				int32							sdsid   /* IN: a dataset ID */)
				
{
    int32      varid;
    NC_array **ap = NULL;
    NC_var    *ret_value = NULL;

    /* varid is low 16bits of sdsid */
    varid = sdsid & 0xffff;

    if (handle->vars != NULL 
       && varid >= 0 
       && varid < handle->vars->count) 
      {
        ap = (NC_array **)handle->vars->values;
        ap += varid;
      } 
    else 
      {
        ret_value = NULL;
        goto done;
      }
    
    ret_value = ((NC_var *)*ap);

done:
    if (ret_value == NULL)
      { /* Failure cleanup */

      }
    /* Normal cleanup */

    return ret_value;    
} /* SDIget_var */


/******************************************************************************
 NAME
	SDIfreevarAID -- free a variables AID

 DESCRIPTION
    Free the AID of the variable with the given index

 RETURNS
    SUCCEED / FAIL 

******************************************************************************/
int32 SDIfreevarAID (
				NC*								handle, /* IN: file handle */
				int32								index   /* IN: variable index */)
				
{
    NC_array **ap = NULL;
    NC_var    *var = NULL;
    int32      ret_value = SUCCEED;

    if (handle == NULL || !handle->vars)
      {
          ret_value = FAIL;
          goto done;
      }

    if (index < 0 || index > handle->vars->count)
      {
          ret_value = FAIL;
          goto done;
      }

    ap = (NC_array **)handle->vars->values;
    ap += index;

    var = (NC_var *) *ap;

    if (var->aid != 0 && var->aid != FAIL)
      {
        if (Hendaccess (var->aid) == FAIL)
          {
              ret_value = FAIL;
              goto done;
          }
      }

    var->aid = FAIL;

done:
    if (ret_value == FAIL)
      { /* Failure cleanup */

      }
    /* Normal cleanup */

    return ret_value;    
}	/* SDIfreevarAID */
#endif	// defined multispec_lin && !defined multispec_wxmac

#endif
