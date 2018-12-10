//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SHDF5.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			07/29/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems	
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
/*	Template for debugging.
	int numberChars = sprintf ((char*)gTextString3,
										" SHDF5: (filePathPtr hDS): %s %ld%s", 
										filePathPtr,
										hDS,
										gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/		
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif	// defined multispec_lin
	
#if defined multispec_mac
#endif	// defined multispec_mac    

#if defined multispec_win
	#include "CFileStream.h"
#endif	// defined multispec_win

#if include_gdal_capability
#if include_hdf5_capability

		// Data product codes
		
#define	kCAI_L1B						1
#define	kCAI_L1BPlus				2
#define	kL3_Global_Radiance		3
#define	kL3_Global_Reflectance	4
#define	kL3_Global_CO2				5
#define	kL3_Global_NDVI			6

#define	kUseNoConversion						0
#define	kUseUpperLeftCorner					1
#define	kUseUpperLeftLowerRightCorners	2
#define	kUseFourCorners						4

#ifdef multispec_lin
	// oul: added definition of SIZE_UNSIGNED_LONG and SIZEOF_VOIDP
	// which are not defined in cpl_config.h

	#if defined NetBeansProject
		/* The size of a 'unsigned long', as computed by sizeof. */
		#define SIZEOF_UNSIGNED_LONG 8
		/* The size of a 'void p', as computed by sizeof. */
		#define SIZEOF_VOIDP 8
	#endif
#endif

#include "hdf5.h"
#include "H5Epublic.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "gh5_convenience.h"
//#include "gdal_pam.h"
 


struct opdata 
	{
	UInt32					recursionLevel;	// recursion level.  0=root 
	struct opdata*			prevOpData;			// pointer to previous opdata 
	UInt32					groupno[2];			// unique group number
	 
	};


		// Routines in GDALInterface.cpp

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

										
		// Routines created in a GDAL routine

extern hid_t GetHDF5Pointer (
				GDALDatasetH						hDS);



		// Routines in SHDF5.cpp that are used by other routines.
		// Prototype is included here because GDALDatasetH variable is defined
		// by .h files included in this file and not in SPrototype.h

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
										
							
#define	DATESIZE	9
#define	kNumValFieldNotUsed	0
#define	kNumValFieldUsed		1
#define	kEqualSignNotUsed		0
#define	kEqualSignUsed			1

SInt16	CheckIfDataSetStringCompares (
				hid_t									file_id,
				char*									dataSetStringPtr,
				char*									compareStringPtr);
				
herr_t	file_info (
				hid_t									loc_id, 
				const char*							name, 
				void*									opdata);

SInt16	GetDataProductCodeFromHDF5File (
				hid_t									file_id);

SInt16	GetInstumentCodeFromHDF5File (
				hid_t									file_id);

Boolean GetMapInfoFromHDF5_CAIGlobalRadiance (
				hid_t									file_id,
				SInt16*								referenceSystemCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								xLowerRightCoordinatePtr,
				double*								yLowerRightCoordinatePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				SInt16*								projectionCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				double*								projectionParametersPtr,
				SInt16*								convertFromLatLongToMapCodePtr,
				Boolean*								adjustUpperLeftOffsetFlagPtr);

Boolean GetMapInfoFromHDF5_CAIGlobalReflectance (
				hid_t									file_id,
				SInt16*								referenceSystemCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								xLowerRightCoordinatePtr,
				double*								yLowerRightCoordinatePtr,
				SInt16*								projectionCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				double*								projectionParametersPtr,
				SInt16*								convertFromLatLongToMapCodePtr,
				Boolean*								adjustUpperLeftOffsetFlagPtr);

Boolean	GetMapInfoFromHDF5_CAIL1BPlus (
				hid_t									file_id,
				SInt16*								referenceSystemCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				SInt16*								mapProjectionCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				double*								projectionParametersPtr,
				double*								xUpperRightCoordinatePtr,
				double*								yUpperRightCoordinatePtr,
				double*								xLowerRightCoordinatePtr,
				double*								yLowerRightCoordinatePtr,
				double*								xLowerLeftCoordinatePtr,
				double*								yLowerLeftCoordinatePtr,
				SInt16*								convertFromLatLongToMapCodePtr,
				Boolean*								adjustUpperLeftOffsetFlagPtr);

SInt16	GetDataSetValueAsNumericalValue (
				hid_t									file_id,
				char*									dataSetStringPtr,
				double*								valuePtr);

SInt32	group_check (
				struct opdata*						op_data, 
				unsigned long						target_groupno[2]);

SInt16	GetDataSetValueAsString (
				hid_t									dataset_id,
				hid_t									dataspace,
				char*									returnStringPtr,
				UInt32								maxReturnStringLength);

SInt16	ReadDataSetNumerical (
				hid_t									file_id,
				char*									dataSetStringPtr,
				double*								returnValuePtr);

UInt32	ReadDataSetNumerical (
				hid_t									dataset_id,
				hid_t									dataspace,
				UInt32								maxNumberValues,
				double*								returnValuePtr,
				UInt32*								numberPossibleValuesPtr);

SInt16	ReadDataSetString (
				hid_t									file_id,
				char*									dataSetStringPtr,
				char*									returnStringPtr,
				UInt32								maxStringLength);

SInt16	ReadDataSetString2 (
				hid_t									dataset_id,
				hid_t									dataspace,
				char*									returnStringPtr,
				UInt32								maxReturnStringLength);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseHDF5DataSetInfo
//
//	Software purpose:	This routine closes access id's into hdf5 subdatasets.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			CloseImageFile in SDatFile.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/19/2012
//	Revised By:			Larry L. Biehl			Date: 04/16/2013

void CloseHDF5DataSetInfo (
				FileInfoPtr 						fileInfoPtr)
				
{	
	HdfDataSets*						hdfDataSetsPtr;
	
	UInt32								dataSet;
	
	
	if (fileInfoPtr->hdfHandle != NULL)
		{
				// This is an hdf5 file. Release the hDS's.
		
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (fileInfoPtr->hdfHandle,
																			kLock);
																
		for (dataSet=1; dataSet<=fileInfoPtr->numberHdfDataSets; dataSet++) 
			{
			if (hdfDataSetsPtr[dataSet].sdid != 0)
				{
				GDALClose ((GDALDatasetH)hdfDataSetsPtr[dataSet].sdid);
				
				if (fileInfoPtr->gdalDataSetH == (GDALDatasetH)hdfDataSetsPtr[dataSet].sdid)
					fileInfoPtr->gdalDataSetH = 0;
				
				}	// end "if (hdfDataSetsPtr[dataSet].sdid != 0)"
			
			hdfDataSetsPtr[dataSet].sdid = 0;
		
			}	// end "for (dataSet=1; dataSet<=fileInfoPtr->numberHdfDataSets; dataSet++)"
			
		CheckAndUnlockHandle (fileInfoPtr->hdfHandle);
			
		}	// end "if (fileInfoPtr->hdfHandle != NULL)"
    
}	// end "CloseHDF5DataSetInfo" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		herr_t file_info
//
//	Software purpose:	This routine is an operator function for H5Giterate to list
//							information about the hdf5 groups.
//							Code for this routine comes from "h5ex_g_traverse" found on HDF
//							Group ftp site.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/02/2012
//	Revised By:			Larry L. Biehl			Date: 06/27/2012

herr_t file_info (
				hid_t									loc_id, 
				const char*							name,	
				void*									operatorData)

{
	char									listString[1024];
	struct opdata						next_op_data;
	
	struct opdata*						opDataPtr;
	H5O_info_t							objectInfo;
	
	H5G_stat_t							statbuf;
	herr_t								returnValue;
	
	hid_t									dataset_id,
											dataspace;
											
	hsize_t								dims[4],
											maxDims[4];
											
	int									spaces;			// number of white spaces to repend to output
	SInt32								numberDimensions;
	size_t								maxStringLength;
	
	SInt16								length,
											stringLength;
											
	Boolean								callH5GiterateFlag,
											continueFlag;
											
	 
	returnValue = 1;
	
			// Allow for c-string end of line terminator.
	maxStringLength = sizeof (listString) - gNumberOfEndOfLineCharacters - 1;
	
	opDataPtr = (struct opdata*)operatorData;
	spaces = 2 * (opDataPtr->recursionLevel+3);
	
			 // Get type of the object and display its name and type.
			 // The name of the object is passed to this function by 
			 // the Library. Some magic :-)
			 
	H5Gget_objinfo (loc_id, name, FALSE, &statbuf);
	H5Oget_info (loc_id, &objectInfo);
	callH5GiterateFlag = FALSE;
	 
	switch (statbuf.type) 
		{
		case H5G_GROUP: 
			stringLength = sprintf (listString,
											"%*s%s%s",
												spaces,
												" ",
												name,
												gEndOfLine);
												
					// Check group objno against linked list of operator data 
					// structures.  Only necessary if there is more than 1 link to 
					// the group.
												
			if ((statbuf.nlink > 1) && group_check (opDataPtr, statbuf.objno)) 
				{
				// printf ("%*s  Warning: Loop detected!\n", spaces, "");
            }
				
			else	// No loop detected.
				{
						// Initialize new operator data structure and begin recursive iteration 
						// on the discovered group.  The new opdata structure is given a
						// pointer to the current one.
				 
				next_op_data.recursionLevel = opDataPtr->recursionLevel + 1;
				next_op_data.prevOpData = opDataPtr;
				next_op_data.groupno[0] = statbuf.objno[0];
				next_op_data.groupno[1] = statbuf.objno[1];
				 
				callH5GiterateFlag = TRUE;
				
				}	// end "else no loop detected."
			break;
				
		case H5G_DATASET:
			stringLength = sprintf (listString,
											"%*s%s: ",
												spaces,
												" ",
												name);
												
			numberDimensions = 0;
			dataset_id = H5Dopen2 (loc_id, name, H5P_DEFAULT);
			if (dataset_id > 0)
				{
				dataspace = H5Dget_space (dataset_id);
				if (dataspace > 0)
					numberDimensions = H5Sget_simple_extent_ndims (dataspace);
													
				if (numberDimensions <= 1)
					length = GetDataSetValueAsString (
															dataset_id, 
															dataspace, 
															&listString[stringLength],
															(UInt32)(maxStringLength-stringLength));
					
				else	// numberDimensions > 1. Implies image data set
					{
					stringLength -= 2;						
					length = sprintf (&listString[stringLength], " image data set");
					stringLength += length;
					
					if (numberDimensions <= 4)
						H5Sget_simple_extent_dims (dataspace, dims, maxDims);
					
					length = 0;
					if (numberDimensions == 2)
						length = sprintf (&listString[stringLength], 
													" (%ld x %ld)", 
													dims[0], 
													dims[1]);
						
					else if (numberDimensions == 3)
						length = sprintf (&listString[stringLength], 
													" (%ld x %ld x %ld)", 
													dims[0], 
													dims[1], 
													dims[2]);
						
					else if (numberDimensions == 4)
						length = sprintf (&listString[stringLength], 
													" (%ld x %ld x %ld x %ld)", 
													dims[0], 
													dims[1], 
													dims[2], 
													dims[3]);
					
					}	// end "else numberDimensions > 1. Implies image data set"
					
				stringLength += length;

				H5Sclose (dataspace);
				H5Dclose (dataset_id);
				
				}	// end "if (dataset_id > 0)"
			
					// Add carriage return
					
			length = sprintf (&listString[stringLength],
											"%s",
											gEndOfLine);
			stringLength += length;
			
			returnValue = 0;
			break;
				
		case H5G_TYPE: 
			stringLength = sprintf (listString,
												"%*s%s named datatype%s",
												spaces,
												" ",
												name,
												gEndOfLine);
			returnValue = 0;
			break;
				
		default:
			stringLength = sprintf (listString,
												"%*sUnable to identify an object.%s",
												spaces,
												" ",
												gEndOfLine);
			returnValue = 0;
				
		}	// end "switch (statbuf.type)"

	continueFlag = ListString (listString,  
										stringLength,  
										gOutputTextH);
										
	//if (callH5GiterateFlag && opDataPtr->recursionLevel == 0)
	//	{
				// List any attribute information for the root level.
					
	//	continueFlag = ListHDF5DataSetAttributes (loc_id, objectInfo.num_attrs);
			
	//	}	// end "if (callH5GiterateFlag && opDataPtr->recursionLevel == 0)"
										
	if (callH5GiterateFlag && continueFlag)
		returnValue = H5Giterate (loc_id, name, NULL, file_info, (void*)&next_op_data);
		
    return (returnValue);
	
}	// end "file_info"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHDF5CompressionInformation
//
//	Software purpose:	This routine determines the compression, if any, being used for
//							for the HDF5 data set.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/01/2012
//	Revised By:			Larry L. Biehl			Date: 05/03/2015

SInt16 GetHDF5CompressionInformation (
				GDALDatasetH						hDS)
				
{	
	SInt32								dataCompressionCode = kNoCompression;
	
	
	#ifndef multispec_win
		dataCompressionCode = (SInt32)GH5_GetDataSetCompressionCode (hDS);
	#endif
	
	if (dataCompressionCode != kNoCompression)
		{
				// Change HDF5 compression code to that used by MultiSpec
		
		if (dataCompressionCode == H5Z_FILTER_DEFLATE)
			dataCompressionCode = kGZIPCompression;
		
		else if (dataCompressionCode == H5Z_FILTER_SHUFFLE)
			dataCompressionCode = kShuffleCompression;
				
		else if (dataCompressionCode == H5Z_FILTER_FLETCHER32)
			dataCompressionCode = kFletcher32Compression;
		
		else if (dataCompressionCode == H5Z_FILTER_SZIP)
			dataCompressionCode = kSzipCompression;
		
		else if (dataCompressionCode == H5Z_FILTER_NBIT)						
			dataCompressionCode = kNBITCompression;
		
		else if (dataCompressionCode == H5Z_FILTER_SCALEOFFSET)						
			dataCompressionCode = kScaleOffsetCompression;
		
		else if (dataCompressionCode > 0)						
			dataCompressionCode = kUnknownCompression;
			
		else	// assume no compression
			dataCompressionCode = kNoCompression;
			
		}	// end "if (dataCompressionCode != kNoCompression)"
		
	return ((SInt16)dataCompressionCode);
    
}	// end "GetHDF5CompressionInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetHDF5ProjectionInformation
//
//	Software purpose:	This routine reads through the groups to obtain information
//							about the map projection information. It will first try to
//							determine the satellite/instrument type and then call the routine
//							specific for that "standard" group organization
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/24/2012
//	Revised By:			Larry L. Biehl			Date: 04/27/2012

SInt16 GetHDF5ProjectionInformation (
				FileInfoPtr							fileInfoPtr)
				
{	
	double								projectionParameters[13];

	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	double								horizontalPixelSize,
											verticalPixelSize,
											xLowerLeftCoordinate,
											xLowerRightCoordinate,
											xMapCoordinate11,
											xUpperRightCoordinate,
											yLowerLeftCoordinate,
											yLowerRightCoordinate,
											yMapCoordinate11,
											yUpperRightCoordinate;
	
	hid_t									file_id;
 	
 	SInt16								convertFromLatLongToMapCode,
											dataProductCode,
											index,
											instrumentCode,				
											mapProjectionCode,
											mapProjectionEllipsoidCode,
											mapProjectionDatumCode,
											mapUnitsCode,
											referenceSystemCode,
											returnCode = noErr;
											
	SignedByte							handleStatus;
	
	Boolean								adjustUpperLeftOffsetFlag,
											convertedFlag,
											mapInfoFlag = FALSE;
   
   			
	if (fileInfoPtr->format != kHDF5Type)
																						return (noErr);
																						
	instrumentCode = 0;
	mapUnitsCode = 0;
	xMapCoordinate11 = 0;
	yMapCoordinate11 = 0;
	horizontalPixelSize = 0;
	verticalPixelSize = 0;
	mapProjectionCode = 0;
	mapProjectionDatumCode = 0;
	mapProjectionEllipsoidCode = 0;
	
	convertFromLatLongToMapCode = 0;
	adjustUpperLeftOffsetFlag = FALSE;
	
	for (index=0; index<13; index++)
		projectionParameters[index] = 0;
										
	file_id = GetHDF5Pointer (fileInfoPtr->gdalDataSetH);
																	
	fileInfoPtr->instrumentCode = GetInstumentCodeFromHDF5File (file_id);
	
	dataProductCode = GetDataProductCodeFromHDF5File (file_id);
	
	if (dataProductCode == kCAI_L1BPlus)
		mapInfoFlag = GetMapInfoFromHDF5_CAIL1BPlus (file_id,
																	&referenceSystemCode,
																	&mapUnitsCode,
																	&xMapCoordinate11,
																	&yMapCoordinate11,
																	&horizontalPixelSize,
																	&verticalPixelSize,
																	&mapProjectionCode,
																	&mapProjectionDatumCode,
																	&mapProjectionEllipsoidCode,
																	projectionParameters,
																	&xUpperRightCoordinate,
																	&yUpperRightCoordinate,
																	&xLowerRightCoordinate,
																	&yLowerRightCoordinate,
																	&xLowerLeftCoordinate,
																	&yLowerLeftCoordinate,
																	&convertFromLatLongToMapCode,
																	&adjustUpperLeftOffsetFlag);
	
	else if (dataProductCode == kL3_Global_Radiance ||
										dataProductCode == kL3_Global_NDVI)
		mapInfoFlag = GetMapInfoFromHDF5_CAIGlobalRadiance (
																	file_id,
																	&referenceSystemCode,
																	&mapUnitsCode,
																	&xMapCoordinate11,
																	&yMapCoordinate11,
																	&xLowerRightCoordinate,
																	&yLowerRightCoordinate,
																	&horizontalPixelSize,
																	&verticalPixelSize,
																	&mapProjectionCode,
																	&mapProjectionDatumCode,
																	&mapProjectionEllipsoidCode,
																	projectionParameters,
																	&convertFromLatLongToMapCode,
																	&adjustUpperLeftOffsetFlag);
	
	else if (dataProductCode == kL3_Global_Reflectance ||
													dataProductCode == kL3_Global_CO2)
		mapInfoFlag = GetMapInfoFromHDF5_CAIGlobalReflectance (
																	file_id,
																	&referenceSystemCode,
																	&mapUnitsCode,
																	&xMapCoordinate11,
																	&yMapCoordinate11,
																	&xLowerRightCoordinate,
																	&yLowerRightCoordinate,
																	&mapProjectionCode,
																	&mapProjectionDatumCode,
																	&mapProjectionEllipsoidCode,
																	projectionParameters,
																	&convertFromLatLongToMapCode,
																	&adjustUpperLeftOffsetFlag);
																																
	if (mapInfoFlag)
		{
		mapProjectionInfoPtr = (MapProjectionInfoPtr)
					GetHandleStatusAndPointer (fileInfoPtr->mapProjectionHandle,
															&handleStatus);
										
				// Verify that the map projection structure has been initialized.
				
		InitializeMapProjectionStructure (mapProjectionInfoPtr);
		
		mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = referenceSystemCode;
		mapProjectionInfoPtr->gridCoordinate.projectionCode = mapProjectionCode;
		  
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
																				
		mapProjectionInfoPtr->planarCoordinate.mapUnitsCode = mapUnitsCode;																		
		
		mapProjectionInfoPtr->geodetic.datumCode = mapProjectionDatumCode;
		mapProjectionInfoPtr->geodetic.spheroidCode = mapProjectionEllipsoidCode;
							
				// Set the ellipsoid based on the datum if it has not been set yet.
		
		if (mapProjectionInfoPtr->geodetic.spheroidCode	== kNoEllipsoidDefinedCode)	
			SetEllipsoidFromDatum (mapProjectionInfoPtr);
																		
		if (mapProjectionCode == kPolarStereographicCode)
			SetPolarStereographicParameters (mapProjectionInfoPtr,
															projectionParameters[0],	// radiusSpheroid
															projectionParameters[0],	// radiusSpheroid
															projectionParameters[4],	// centralMeridian
															projectionParameters[5],	// latitudeOrigin
															projectionParameters[6],	// falseEasting
															projectionParameters[7]);	// falseNorthing
																																					
		else if (mapProjectionCode == kMercatorCode)
			SetMercatorParameters (mapProjectionInfoPtr,
															projectionParameters[0],	// radiusSpheroid
															projectionParameters[0],	// radiusSpheroid
															projectionParameters[4],	// longitudeCentralMeridian
															projectionParameters[5],	// latitudeOrigin
															projectionParameters[6],	// falseEasting
															projectionParameters[7]);	// falseNorthing		
																				
		else if (mapProjectionCode == kEquirectangularCode)
			SetEquirectangularParameters (mapProjectionInfoPtr,
															projectionParameters[0],	// radiusSpheroid
															projectionParameters[4],	// longitudeCentralMeridian
															projectionParameters[5]);	// standard parallel
						
		MHSetState (fileInfoPtr->mapProjectionHandle, handleStatus);
			
		LoadSpheroidInformation (fileInfoPtr->mapProjectionHandle);
			
		mapProjectionInfoPtr = (MapProjectionInfoPtr)
											GetHandlePointer (fileInfoPtr->mapProjectionHandle);
													
		if (convertFromLatLongToMapCode > 0)
				// Convert the lat-long coordinates to map coordinates					
			convertedFlag = ConvertLatLongPointToMapPoint (
								mapProjectionInfoPtr,
								&mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11, 
								&mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11);
						
		if (horizontalPixelSize <= 0 || verticalPixelSize <= 0)
			{
					// Verify pixel spacing.  If not available, see if it can be 
					// calculated.
				
					// Find an approximation for the pixel size based on the image map 
					// horizontal size using the number of corners specified. I hope end the 
					// end there is a better way than this.
			
			if (convertFromLatLongToMapCode >= kUseUpperLeftLowerRightCorners)
						// Convert the lat-long coordinates to map coordinates
				ConvertLatLongPointToMapPoint (
									mapProjectionInfoPtr,
									&xLowerRightCoordinate, 
									&yLowerRightCoordinate);
			
			if (convertFromLatLongToMapCode == kUseFourCorners)
				{
						// Convert the lat-long coordinates to map for upper right coordinates
						
				ConvertLatLongPointToMapPoint (
									mapProjectionInfoPtr,
									&xUpperRightCoordinate, 
									&yUpperRightCoordinate);
					
						// Convert the lat-long coordinates to map for lower left coordinates
						
				ConvertLatLongPointToMapPoint (
									mapProjectionInfoPtr,
									&xLowerLeftCoordinate, 
									&yLowerLeftCoordinate);
									
				}	// end "if (convertFromLatLongToMapCode == kUseFourCorners)"

			if (horizontalPixelSize <= 0)
				{							
				double		leftCoordinate, rightCoordinate;
				SInt32		numberColumns;
				
				numberColumns = fileInfoPtr->numberColumns;
				if (!adjustUpperLeftOffsetFlag)
					numberColumns -= 1;
													
				leftCoordinate = mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;										
				if (convertFromLatLongToMapCode == kUseFourCorners)				
					leftCoordinate = (leftCoordinate + xLowerLeftCoordinate)/2;
													
				rightCoordinate = xLowerRightCoordinate;
				if (convertFromLatLongToMapCode == kUseFourCorners)
					rightCoordinate = (rightCoordinate + xUpperRightCoordinate)/2;
													
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 
												(rightCoordinate - leftCoordinate)/numberColumns;
																		
				}	// end "if (horizontalPixelSize <= 0)"
																										
			if (verticalPixelSize <= 0)
				{
				double		topCoordinate, bottomCoordinate;
				SInt32		numberLines;
				
				numberLines = fileInfoPtr->numberLines;
				if (!adjustUpperLeftOffsetFlag)
					numberLines -= 1;
						
				topCoordinate = mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;										
				if (convertFromLatLongToMapCode == kUseFourCorners)				
					topCoordinate = (topCoordinate + yUpperRightCoordinate)/2;
						
				bottomCoordinate = yLowerRightCoordinate;										
				if (convertFromLatLongToMapCode == kUseFourCorners)				
					bottomCoordinate = (bottomCoordinate + yLowerLeftCoordinate)/2;
				
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 
											(topCoordinate - bottomCoordinate)/numberLines;
													
				}	// end "if (verticalPixelSize <= 0)"
																	
			}	// end "if (horizontalPixelSize <= 0 || verticalPixelSize <= 0)"	
			
		if (adjustUpperLeftOffsetFlag)
			{
			mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 += 
									mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize/2;
			mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 -= 
							 		mapProjectionInfoPtr->planarCoordinate.verticalPixelSize/2;
													  
			mapProjectionInfoPtr->planarCoordinate.xMapOrientationOrigin =    
									mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11;
			mapProjectionInfoPtr->planarCoordinate.yMapOrientationOrigin =    
									mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11;
			
			}	// end "if (adjustUpperLeftOffsetFlag)"
		
		}	// end "if (mapInfoFlag)"
										
	return (returnCode);
    
}	// end "GetHDF5ProjectionInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetInstumentCodeFromHDF5File
//
//	Software purpose:	This routine tries to obtain the instrument code for the data in
//							the input HDF5 file
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	Instrument code
//							0 is returned if no instrument code is found.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2012
//	Revised By:			Larry L. Biehl			Date: 07/27/2015

SInt16 GetInstumentCodeFromHDF5File (
				hid_t									file_id)
				
{	
	char									string[80];
	
 	SInt16								instrumentCode,
											returnCode;
   
	
			// Get the instrument code
			
	returnCode = ReadDataSetString (file_id, 
												(char*)"/Global/metadata/sensorName",
												string,
												79);
				
	instrumentCode = 0;
	if (returnCode > 0)
		{
		if (strcmp (string, "TANSO-CAI") == 0)
			instrumentCode = kTANSO_CAI;
			
		else if (strcmp (string, "TANSO-FTS") == 0)
			instrumentCode = kTANSO_FTS;
		
		}	// end "if (returnCode > 0)"
										
	return (instrumentCode);
    
}	// end "GetInstumentCodeFromHDF5File"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetMapInfoFromHDF5_CAIGlobalRadiance
//
//	Software purpose:	This routine reads through the objects to obtain information
//							about the map projection information for the CAI L1B+ data
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2012
//	Revised By:			Larry L. Biehl			Date: 05/05/2012

Boolean GetMapInfoFromHDF5_CAIGlobalRadiance (
				hid_t									file_id,
				SInt16*								referenceSystemCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								xLowerRightCoordinatePtr,
				double*								yLowerRightCoordinatePtr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				SInt16*								projectionCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				double*								projectionParametersPtr,
				SInt16*								convertFromLatLongToMapCodePtr,
				Boolean*								adjustUpperLeftOffsetFlagPtr)
				
{	
	char									string[80];
	
	double								value;
	
 	SInt16								returnCode;
	
	Boolean								returnFlag = FALSE;
   

			// Get the upper left longitude
			
	returnCode = GetDataSetValueAsNumericalValue (
				file_id, 
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/westBoundLongitude",
				&value);
				
	if (returnCode == 1)
		{
		*xMapCoordinate11Ptr = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
			// Get the upper left latitude
			
	returnCode = GetDataSetValueAsNumericalValue (
				file_id,
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/northBoundLatitude",
				&value);
				
	if (returnCode == 1)
		{
		*yMapCoordinate11Ptr = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
			// Get the lower right longitude
			
	returnCode = GetDataSetValueAsNumericalValue (
				file_id,
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/eastBoundLongitude",
				&value);
				
	if (returnCode == 1)
		*xLowerRightCoordinatePtr = value;
		
			// Get the lower right latitude
			
	returnCode = GetDataSetValueAsNumericalValue (
				file_id, 
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/southBoundLatitude",
				&value);
				
	if (returnCode == 1)
		*yLowerRightCoordinatePtr = value;
		
			// Get the x map scale
			
	returnCode = GetDataSetValueAsNumericalValue (
				file_id, 
				(char*)"/attribute/intervalLengthOfLongitude",
				&value);
				
	if (returnCode == 1)
		*horizontalPixelSizePtr = value;
		
			// Get the y map scale
			
	returnCode = GetDataSetValueAsNumericalValue (
				file_id, 
				(char*)"/attribute/intervalLengthOfLatitude",
				&value);
				
	if (returnCode == 1)
		*verticalPixelSizePtr = value;
	
	/*	
	*xMapCoordinate11Ptr = -180;
	*yMapCoordinate11Ptr = 90;
	*xLowerRightCoordinatePtr = 180;
	*yLowerRightCoordinatePtr = -90;
	*/		
			// Get the Projection Code
			
	returnCode = ReadDataSetString (
				file_id, 
				(char*)"/attribute/mapProjection/projectionMethod",
				string,
				79);
				
	if (returnCode > 0)
		{
		if (strcmp (string, "EQR") == 0)
			*projectionCodePtr = kEquirectangularCode;
									
		returnFlag = TRUE;
		
		}	// end "if (returnCode > 0)"
			
			// Get the Datum
			
	returnCode = ReadDataSetString (
				file_id, 
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/extentReferenceSystem/code",
				string,
				79);
				
	if (returnCode > 0)
		{
		if (strcmp (string, "WGS84") == 0)
			*mapProjectionDatumCodePtr = kWGS84Code;
			
		if (*mapProjectionDatumCodePtr != kNoDatumDefinedCode)
			returnFlag = TRUE;
		
		}	// end "if (returnCode > 0)"
		
		// Make a check to see if this can be geographic if no projection was
		// defined.
		
	if (*projectionCodePtr == kEquirectangularCode)
		{
		*mapUnitsCodePtr = kMetersCode;
		*referenceSystemCodePtr = kUserDefinedRSCode;
		projectionParametersPtr[4] = 0;	// Central Longitude
		projectionParametersPtr[5] = 0;	// Standard Parallel
		
		}	// end "if (*projectionCodePtr == kEquirectangularCode)"

	else if (*projectionCodePtr == kNotDefinedCode &&
					*horizontalPixelSizePtr > 0 &&
								*verticalPixelSizePtr > 0)			
		{
		*mapUnitsCodePtr = kDecimalDegreesCode;
		*referenceSystemCodePtr = kGeographicRSCode;
		*projectionCodePtr = kGeographicCode;
		
		}	// end "if (*projectionCodePtr != kNotDefinedCode && ..."
	
			// Per Watanabe Hiroshi email on 4/27/2012, the geographic reference system
			// should be the NAD83 reference system (GRS80 ellipsoid) for the Mercator
			// projection.  The documentation is not correct.
			
	if (*projectionCodePtr == kMercatorCode && *mapProjectionDatumCodePtr == kWGS84Code)
		*mapProjectionDatumCodePtr = kNAD83Code;
	
	if (*mapProjectionDatumCodePtr == kNoDatumDefinedCode)
		{
		*mapProjectionDatumCodePtr = kSphereDatumCode;
		*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
		
		}	// end "if (*mapProjectionDatumCodePtr == kNoDatumDefinedCode)"
	
	if (*projectionCodePtr == kEquirectangularCode)
		*convertFromLatLongToMapCodePtr = kUseUpperLeftLowerRightCorners;
		
			// The horizontal and vertical width values seem to be given in 
			// lat-long. They will need to be recalculated for Equirectangular 
			// projection.
			// Will also force recalculation for geographic since the vertical and
			// horizontal widths were orginally saved as floats not doubles so do not
			// have the precision that they could have.
			
	*horizontalPixelSizePtr = 0;
	*verticalPixelSizePtr = 0;
		
	*adjustUpperLeftOffsetFlagPtr = TRUE;
										
	return (returnFlag);
    
}	// end "GetMapInfoFromHDF5_CAIGlobalRadiance"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetMapInfoFromHDF5_CAIGlobalReflectance
//
//	Software purpose:	This routine reads through the objects to obtain information
//							about the map projection information for the CAI L1B+ data
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/27/2012
//	Revised By:			Larry L. Biehl			Date: 04/27/2012

Boolean GetMapInfoFromHDF5_CAIGlobalReflectance (
				hid_t									file_id,
				SInt16*								referenceSystemCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								xLowerRightCoordinatePtr,
				double*								yLowerRightCoordinatePtr,
				SInt16*								projectionCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				double*								projectionParametersPtr,
				SInt16*								convertFromLatLongToMapCodePtr,
				Boolean*								adjustUpperLeftOffsetFlagPtr)
				
{	
	char									string[80];
	
 	SInt16								returnCode;
	
	Boolean								returnFlag = FALSE;
   

	*xMapCoordinate11Ptr = -180;
	*yMapCoordinate11Ptr = 90;
	*xLowerRightCoordinatePtr = 180;
	*yLowerRightCoordinatePtr = -90;
			
			// Get the Datum
			
	returnCode = ReadDataSetString (
				file_id, 
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/extentReferenceSystem/code",
				string,
				79);
				
	if (returnCode > 0)
		{
		if (strcmp (string, "WGS84") == 0)
			*mapProjectionDatumCodePtr = kWGS84Code;
			
		if (*mapProjectionDatumCodePtr != kNoDatumDefinedCode)
			returnFlag = TRUE;
		
		}	// end "if (returnCode > 0)"
		
	*mapUnitsCodePtr = kDecimalDegreesCode;
	
	*referenceSystemCodePtr = kGeographicRSCode;
	*projectionCodePtr = kGeographicCode;
		
	if (*mapProjectionDatumCodePtr == kNoDatumDefinedCode)
		{
		*mapProjectionDatumCodePtr = kSphereDatumCode;
		*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
		
		}	// end "if (*mapProjectionDatumCodePtr == kNoDatumDefinedCode)"
	
	*convertFromLatLongToMapCodePtr = kUseNoConversion;
	*adjustUpperLeftOffsetFlagPtr = TRUE;
										
	return (returnFlag);
    
}	// end "GetMapInfoFromHDF5_CAIGlobalReflectance"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetMapInfoFromHDF5_CAIL1BPlus
//
//	Software purpose:	This routine reads through the objects to obtain information
//							about the map projection information for the CAI L1B+ data
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2012
//	Revised By:			Larry L. Biehl			Date: 06/05/2012

Boolean GetMapInfoFromHDF5_CAIL1BPlus (
				hid_t									file_id,
				SInt16*								referenceSystemCodePtr,
				SInt16*								mapUnitsCodePtr,
				double*								xMapCoordinate11Ptr,
				double*								yMapCoordinate11Ptr,
				double*								horizontalPixelSizePtr,
				double*								verticalPixelSizePtr,
				SInt16*								projectionCodePtr,
				SInt16*								mapProjectionDatumCodePtr,
				SInt16*								mapProjectionEllipsoidCodePtr,
				double*								projectionParametersPtr,
				double*								xUpperRightCoordinatePtr,
				double*								yUpperRightCoordinatePtr,
				double*								xLowerRightCoordinatePtr,
				double*								yLowerRightCoordinatePtr,
				double*								xLowerLeftCoordinatePtr,
				double*								yLowerLeftCoordinatePtr,
				SInt16*								convertFromLatLongToMapCodePtr,
				Boolean*								adjustUpperLeftOffsetFlagPtr)
				
{	
	char									string[80];
	
	double								value,
											xScale,
											yScale;
 	
 	SInt16								returnCode;
	
	Boolean								returnFlag = FALSE;
   
		
			// Get the upper left longitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/upperLeftLongitude",
									&value);
				
	if (returnCode == 1)
		{
		*xMapCoordinate11Ptr = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
			// Get the upper left latitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/upperLeftLatitude",
									&value);
				
	if (returnCode == 1)
		{
		*yMapCoordinate11Ptr = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
			// Get the upper right longitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/upperRightLongitude",
									&value);
				
	if (returnCode == 1)
		{
		*xUpperRightCoordinatePtr = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
			// Get the upper right latitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/upperRightLatitude",
									&value);
				
	if (returnCode == 1)
		{
		*yUpperRightCoordinatePtr = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
			// Get the lower right longitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/lowerRightLongitude",
									&value);
				
	if (returnCode == 1)
		*xLowerRightCoordinatePtr = value;
		
			// Get the lower right latitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/lowerRightLatitude",
									&value);
				
	if (returnCode == 1)
		*yLowerRightCoordinatePtr = value;
		
			// Get the lower left longitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/lowerLeftLongitude",
									&value);
				
	if (returnCode == 1)
		*xLowerLeftCoordinatePtr = value;
		
			// Get the lower left latitude
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/frameCorner/lowerLeftLatitude",
									&value);
				
	if (returnCode == 1)
		*yLowerLeftCoordinatePtr = value;
		
			// Get the x map scale
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/mapProjection/mapScale/mapScaleX",
									&value);
	
	xScale = -99;
	if (returnCode == 1)
		xScale = value;
		
			// Get the y map scale
			
	returnCode = ReadDataSetNumerical (
									file_id, 
									(char*)"/frameAttribute/mapProjection/mapScale/mapScaleY",
									&value);
				
	yScale = -99;
	if (returnCode == 1)
		yScale = value;
	
			// Get the Projection Code
			
	returnCode = ReadDataSetString (
									file_id, 
									(char*)"/frameAttribute/mapProjection/projectionMethod",
									string,
									79);
				
	if (returnCode > 0)
		{
		if (strcmp (string, "PS") == 0)
			*projectionCodePtr = kPolarStereographicCode;
			
		else if (strcmp (string, "MER") == 0)
			*projectionCodePtr = kMercatorCode;
			
		if (*projectionCodePtr != kNotDefinedCode)
			returnFlag = TRUE;
		
		}	// end "if (returnCode > 0)"
	
			// Get the Datum
			
	returnCode = ReadDataSetString (
				file_id, 
				(char*)"/Global/MD_Metadata/identificationInfo/MD_DataIdentification/extent/geographicElement/EX_GeographicBoundingBox/extentReferenceSystem/code",
				string,
				79);
				
	if (returnCode > 0)
		{
		if (strcmp (string, "WGS84") == 0)
			*mapProjectionDatumCodePtr = kWGS84Code;
			
		if (*mapProjectionDatumCodePtr != kNoDatumDefinedCode)
			returnFlag = TRUE;
		
		}	// end "if (returnCode > 0)"
	
			// Get the Projection Center Longitude
			
	returnCode = ReadDataSetNumerical (
				file_id, 
				(char*)"/frameAttribute/mapProjection/projectionPoint/projectionCenterLongitude",
				&value);
				
	if (returnCode == 1)
		{
		projectionParametersPtr[4] = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
	
			// Get the Projection Center Latitude
			
	returnCode = ReadDataSetNumerical (
				file_id, 
				(char*)"/frameAttribute/mapProjection/projectionPoint/projectionCenterLatitude",
				&value);
				
	if (returnCode == 1)
		{
		projectionParametersPtr[5] = value;
		returnFlag = TRUE;
		
		}	// end "if (returnCode == 1)"
		
	*mapUnitsCodePtr = kMetersCode;
	
	*referenceSystemCodePtr = kUserDefinedRSCode;
																														
	projectionParametersPtr[6] = 0;		// falseEasting
	projectionParametersPtr[7] = 0;		// falseNorthing
	
			// Per Watanabe Hiroshi email on 4/27/2012, the geographic reference system
			// should be the NAD83 reference system (GRS80 ellipsoid) for the Mercator
			// projection.  The documentation is not correct.
			// And from an email on 5/1/2012, the same for the Polar Stereographic 
			// projection.
			// They actually use a sphere for the calculations. So I am setting this up
			// as a spheroid with the radius being (2 * major + minor)/ 3.
			
	*mapProjectionDatumCodePtr = kSphereDatumCode;
	*mapProjectionEllipsoidCodePtr = kSphereEllipsoidCode;
	projectionParametersPtr[0] = 6371008.7714;
	
			// The pixel size is 500 for Mercator and 1000 for Polar Stereographic. 
			// Dr. Hiroshi indicated that the pixel size was going to change to 500 meters
			// for Polar Stereographic in some future release. I have not found where to
			// find this in the hdf5 metadata though. So will fix here for now.
			// One can also compute the pixel sizes from frame sizes and get them 
			// very close (within a few thousands of a meter).
			
	*horizontalPixelSizePtr = 500;
	if (fabs (xScale - .004) < .000001)
		*horizontalPixelSizePtr = 1000;
		
	*verticalPixelSizePtr = 500;
	if (fabs (yScale - .004) < .000001)
		*verticalPixelSizePtr = 1000;
		
	*convertFromLatLongToMapCodePtr = kUseUpperLeftCorner;
	*adjustUpperLeftOffsetFlagPtr = FALSE;
										
	return (returnFlag);
    
}	// end "GetMapInfoFromHDF5_CAIL1BPlus"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetDataProductCodeFromHDF5File
//
//	Software purpose:	This routine determines what the data product code is for this
//							file.  It is expected that this file will expand as more hdf5
//							image files are read by MultiSpec
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2012
//	Revised By:			Larry L. Biehl			Date: 04/26/2012

SInt16 GetDataProductCodeFromHDF5File (
				hid_t									file_id)
				
{	
	char									string[80];
 	
 	SInt16								dataProductCode = 0,
											returnCode;
	
	
	if (file_id > 0)
		{
				// Get the data product code
			
		returnCode = ReadDataSetString (
									file_id, 
									(char*)"/Global/metadata/productName",
									string,
									79);
		
		if (returnCode == 0)	
					// Try a variation found in some GOSAT files
			returnCode = ReadDataSetString (
										file_id, 
										(char*)"/Global/metadata/ProductName",
										string,
										79);
					
		if (returnCode > 0)
			{
			if (strstr (string, "CAI L1B data") != NULL)
				dataProductCode = kCAI_L1B;
				
			else if (strstr (string, "CAI L1B+ data") != NULL)
				dataProductCode = kCAI_L1BPlus;
				
			else if (strstr (string, "L3 global reflectance distribution") != NULL)
				dataProductCode = kL3_Global_Reflectance;
				
			else if (strstr (string, "L3 global radiance distribution") != NULL)
				dataProductCode = kL3_Global_Radiance;
				
			else if (strstr (string, "L3 global CO2 distribution") != NULL)
				dataProductCode = kL3_Global_CO2;
				
			else if (strstr (string, "L3 global NDVI") != NULL)
				dataProductCode = kL3_Global_NDVI;
			
			}	// end "if (returnCode > 0)"
			
		}	// end "if (file_id > 0)"
		
	return (dataProductCode);
    
}	// end "GetDataProductCodeFromHDF5File"


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHDF5DataSetAttributes
//
//	Software purpose:	This routine will list information that data formats being
//							read by gdal contain. Currently this is restricted to the GRIB
//							format.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/04/2012
//	Revised By:			Larry L. Biehl			Date: 06/04/2012

Boolean ListHDF5DataSetAttributes (
				hid_t									loc_id)
				
{
	char									string_out[80];
	
	hid_t									attr,
											atype,
											atype_mem;
	
	herr_t								returnCode;
	
	H5O_info_t							objectInfo;
	H5T_class_t							type_class;
	
	UInt32								index;
	
	SInt16								stringLength;
   										
   Boolean								continueFlag = TRUE;
   
   		
	if (loc_id > 0)
		{
		H5Oget_info (loc_id, &objectInfo);
		for (index=0; index<objectInfo.num_attrs; index++)
			{
			attr = H5Aopen_by_idx (loc_id, 
											".", 
											H5_INDEX_CRT_ORDER, 
											H5_ITER_INC, 
											(hsize_t)index, 
											H5P_DEFAULT, 
											H5P_DEFAULT);
											
			atype = H5Aget_type (attr);
			type_class = H5Tget_class (atype);
			if (type_class == H5T_STRING) 
				{
				atype_mem = H5Tget_native_type (atype, H5T_DIR_ASCEND);
				returnCode = H5Aread (attr, atype_mem, string_out);
				returnCode = H5Tclose (atype_mem);
				
				stringLength = sprintf ((char*)&gTextString[0],
																"      %s%s", 
																string_out,
																gEndOfLine);

				continueFlag = ListString ((char*)gTextString,  
																stringLength,  
																gOutputTextH);
				
				}	// end "if (type_class == H5T_STRING)"

			returnCode = H5Aclose (attr);
			returnCode = H5Tclose (atype);
			
			}	// end "for (index=0; index<objectInfo.num_attrs; index++)"
				
		}	// end "if (loc_id > 0)"
		
	return (continueFlag);
	
}	// end "ListHDF5DataSetAttributes"   
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 group_check
//
//	Software purpose:	This routine recursively searches the linked list of opdata 
//							structures for one whose groupno field matches target_groupno.  							
//							Code for this routine comes from "h5ex_g_traverse" found on HDF
//							Group ftp site.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	1 if a match is found,
//							0 otherwise.
//
// Called By:			file_info in SHDF5.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/03/2012
//	Revised By:			Larry L. Biehl			Date: 05/03/2012

SInt32 group_check (
				struct opdata*						opDataPtr, 
				unsigned long						target_groupno[2])

{
	if ((opDataPtr->groupno[0] == target_groupno[0]) &&
											(opDataPtr->groupno[1] == target_groupno[1]))
				// Group number match
		return (1);
		  
	else if (!opDataPtr->recursionLevel)
				// Root group reached with no matches
		return (0);
		  
	else
				// Recursively examine the next node
		return (group_check (opDataPtr->prevOpData, target_groupno));
	
}	// end "group_check" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHDF5FileInformation
//
//	Software purpose:	This routine will list the group information in the HDF5 file
//							that are not data sets. The data set information will get 
//							listed separately.
//							Code for this routine comes from "h5ex_g_traverse" found on HDF
//							Group ftp site.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			ListDescriptionInformation in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/02/2012
//	Revised By:			Larry L. Biehl			Date: 05/14/2012

Boolean ListHDF5FileInformation (
				FileInfoPtr							fileInfoPtr)
				
{ 
	struct opdata						op_data;
	H5G_stat_t							statbuf;
	herr_t								status;
	hid_t									file_id;
	
	SInt16								stringLength;
	
	Boolean								continueFlag = TRUE;
	

	file_id = GetHDF5Pointer (fileInfoPtr->gdalDataSetH);
	
	if (file_id > 0)
		{   
				// Initialize the operature data structure
				
		status = H5Gget_objinfo (file_id, "/", 0, &statbuf);
		
		op_data.recursionLevel = 0;
		op_data.prevOpData = NULL;
		op_data.groupno[0] = statbuf.objno[0];
		op_data.groupno[1] = statbuf.objno[1];

				// List the header for the listing

		stringLength = sprintf ((char*)&gTextString[0],
										"    HDF5 file metadata and information:%s",
										gEndOfLine);

		continueFlag = ListString ((char*)gTextString,  
											stringLength,  
											gOutputTextH);
										
				// List any attribute information for the file level.
		
		//if (continueFlag)
		//	continueFlag = ListHDF5DataSetAttributes (file_id);

		if (continueFlag)
			H5Giterate (file_id, "/", NULL, file_info, (void*)&op_data);
			
		stringLength = sprintf ((char*)&gTextString[0],
										"    End HDF5 file metadata and information%s%s",
										gEndOfLine,
										gEndOfLine);
										
		if (continueFlag)
			continueFlag = ListString ((char*)gTextString,  
												stringLength,  
												gOutputTextH);
													
		}	// end "if (file_id > 0)"

	return (continueFlag);
	
}	// end "ListHDF5FileInformation" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 LoadHdf5DataSetNames
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
//	Coded By:			Larry L. Biehl			Date: 04/18/2012
//	Revised By:			Larry L. Biehl			Date: 07/29/2018

SInt32 LoadHdf5DataSetNames (
				GDALDatasetH						hDS,
				SInt16								format,
				FileInfoPtr							fileInfoPtr,
				Handle*								hdfDataSetsHandlePtr,
				HdfDataSets**						hdfDataSetsPtrPtr,
				SInt32*								numDataSetsPtr,
				SInt32*								firstDataSetIndexPtr,
				GDALDatasetH*						outGDALDatasetPtr)
				
{
	char									groupName[16],
											szKeyName[1024];
											
	char**								metadata;
							
	char									*dataSetNamePtr,
											*lastLevelStringPtr,
											*pszSubdatasetName,
											*pszHDF4SubdatasetName,
											*stringPtr;
	
	HdfDataSets*						hdfDataSetsPtr;
	
	UCharPtr								fileNamePtr;
	
	double								noDataValue;
	
	GDALDatasetH						gdalDataSetH;
	Handle								dataSetsHandle = NULL;
	
   SInt32 								index,
   										dataSet,
   										numberDataSets = 0,
											returnCode,
   										set0DataSet,
											totalNumberDataSets;
   										
   UInt32								groupNumber,
   										numberChannels,
											numberColumns,
											numberLines,
											setNumberForGroup,
											set0NumberColumns,
											set0NumberLines,
											xBlockSize,
											yBlockSize;
											
	SInt16								bandInterleaveFormat,
											errCode,
											gdalDataTypeCode,
											length;
   
	UInt16								dataCompressionCode,
											dataTypeCode,
											numberBits,
											numberBytes,
											numberCompareCharacters,
											set0DataCompressionCode,
											set0DataTypeCode,
											set0NumberBytes;
		
	time_t								startDataSetRead,
											startTime;

   Boolean								noDataValueFlag,
											signedDataFlag,
   										set0SignedDataFlag,
											statusDialogCreatedFlag;
   
   										   
   index = 1;
   errCode = noErr;
   *firstDataSetIndexPtr = 0;
   setNumberForGroup = 0;
   groupNumber = 0;
	numberCompareCharacters = 4;
	startDataSetRead = time (NULL);
	gdalDataSetH = hDS;
	statusDialogCreatedFlag = FALSE;
	hdfDataSetsPtr = NULL;
	
	if (format != kNITFType)
		H5Eset_auto2 (NULL, NULL, NULL);
	
	//gFromToolParameterFileFlag = TRUE;
	
			// Determine how many subdatasets exist in the hdf5 formatted file.
			
	//metadata = ((GDALDataset*)hDS)->GetMetadata ("SUBDATASETS");
	metadata = GDALGetMetadata (hDS, "SUBDATASETS");
	numberDataSets = CSLCount (metadata)/2;    
					
	if (numberDataSets > 0) 
		{
				// Allow for grouped information in the hdf data set.
				
		totalNumberDataSets = numberDataSets * 2;
				
		dataSetsHandle = MNewHandle ((totalNumberDataSets+1)*sizeof (HdfDataSets));
		hdfDataSetsPtr = (HdfDataSets*)GetHandlePointer (dataSetsHandle, kLock);
									
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
			
		}	// end "if (numberDataSets > 0)"		

	if (hdfDataSetsPtr != NULL && numberDataSets > 0)
		{
		for (dataSet=1; dataSet<=numberDataSets; dataSet++)
			{
			if (dataSet == 1)
				startTime = TickCount ();
			
			if (dataSet == 2 && gStatusDialogPtr == NULL)
				{
				if ((TickCount () - startTime) * numberDataSets > 3 * gTimeOffset)
					{
					gStatusDialogPtr = GetStatusDialog (kShortStatusInfoID, FALSE);
					if (gStatusDialogPtr != NULL)
						{
						ShowHideDialogItem (gStatusDialogPtr, IDC_ShortStatusText, TRUE);
						ShowHideDialogItem (gStatusDialogPtr, IDC_ShortStatusValue, TRUE);
						MGetString (gTextString, kAlertStrID, IDS_Alert62);
						LoadDItemString (gStatusDialogPtr, IDC_ShortStatusText, (Str255*)gTextString);
						LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)2);
						ShowStatusDialogWindow (kShortStatusInfoID);
						
						statusDialogCreatedFlag = TRUE;
						
						}	// end "if (gStatusDialogPtr != NULL)"
					
					}	// end "if (TickCount () - startTime * numberDataSet > 3 * gTimeOffset)"
				
				}	// end "if (dataSet == 2 && gStatusDialogPtr == NULL)"
				
			//gNextTime = TickCount () + gTimeOffset;
			pszHDF4SubdatasetName = NULL;
			//pszSDSName = CPLStrdup (CSLFetchNameValue (poDS->papszSubDatasets,
         //                   "SUBDATASET_1_NAME"));
									 
			snprintf (szKeyName, sizeof (szKeyName), "SUBDATASET_%d_NAME", (int)dataSet);
			szKeyName[sizeof (szKeyName) - 1] = '\0';
			pszSubdatasetName = CPLStrdup (CSLFetchNameValue (metadata, szKeyName));
			
			if (gStatusDialogPtr != NULL)
				{
				LoadDItemValue (gStatusDialogPtr, IDC_ShortStatusValue, (SInt32)dataSet);
				CheckSomeEvents (osMask+updateMask);
				
				}	// end "if (gStatusDialogPtr != NULL)"

			//if (gStatusDialogPtr == NULL)
			if (dataSet >= 1)
				{
				hDS = GDALOpen (pszSubdatasetName, GA_ReadOnly);
			
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
			
				if (returnCode == noErr)
					{
					hdfDataSetsPtr[index].name[1] = '*';
					
					if (*firstDataSetIndexPtr == 0)
						*firstDataSetIndexPtr = index;
					
					}	// end "if (returnCode == noErr)"
					
				else	// returnCode != noErr
					hdfDataSetsPtr[index].name[1] = '_';
				
				}	// end "if (gStatusDialogPtr == NULL)"
				
			else // gStatusDialogPtr != NULL
				{
				hdfDataSetsPtr[index].name[1] = ' ';
				hDS = 0;
					
				}	// end "else gStatusDialogPtr != NULL"
					
					// Make sure that the name is not more than 73 characters to allow for
					// * or _, c-string end, pascal string length character, Group number 
					// (_Gxx)
			
			if (pszSubdatasetName[0] != 0)	
				{		
				if (format == kHDF5Type)
					{
							// Use the characters after the last '/' character for the data 
							// set identifier name.
							
					dataSetNamePtr = strstr (pszSubdatasetName, "//");
					if (dataSetNamePtr != NULL)
						{
						dataSetNamePtr += 2;
						stringPtr = dataSetNamePtr;
						lastLevelStringPtr = dataSetNamePtr;
						while (stringPtr != NULL)
							{
							stringPtr = strchr (stringPtr, '/');
							if (stringPtr != NULL && *stringPtr == '/')
								{
								*stringPtr = '_';
								lastLevelStringPtr = stringPtr + 1;
								
								}	// end "if (stringPtr != NULL && *stringPtr == '/')"
							
							}	// end "while (stringPtr != NULL)"
					
						}	// end "if (dataSetNamePtr != NULL)"
							
					}	// end "if (format == kHDF5Type)"
						
				else if (format == kNETCDF2Type)
					{
							// Use the characters after the last ':' character for the data 
							// set identifier name.
							
					dataSetNamePtr = strchr (pszSubdatasetName, ':');
					if (dataSetNamePtr != NULL)
						{
						dataSetNamePtr += 1;
						stringPtr = dataSetNamePtr;
						lastLevelStringPtr = dataSetNamePtr;
						while (stringPtr != NULL)
							{
							stringPtr = strchr (stringPtr, ':');
							if (stringPtr != NULL && *stringPtr == ':')
								{
								stringPtr++;
								lastLevelStringPtr = stringPtr;
								dataSetNamePtr = stringPtr;
								
								}	// end "if (stringPtr != NULL && *stringPtr == ':')"
							
							}	// end "while (stringPtr != NULL)"
					
						}	// end "if (dataSetNamePtr != NULL)"
							
					}	// end "else if (format == kNETCDF2Type)"
						
				else if (format == kHDF4Type2)
					{
					snprintf (szKeyName, sizeof (szKeyName), "SUBDATASET_%d_DESC", (int)dataSet);
					szKeyName[sizeof (szKeyName) - 1] = '\0';
					pszHDF4SubdatasetName = CPLStrdup (CSLFetchNameValue (metadata, szKeyName));

					if (pszHDF4SubdatasetName[0] != 0)
						{
								// Use the characters after the last ':' character for the data set
								// identifier name.
								
						dataSetNamePtr = strchr (pszHDF4SubdatasetName, ']');

						if (dataSetNamePtr != NULL)
							{
							dataSetNamePtr += 2;
							lastLevelStringPtr = dataSetNamePtr;
							
							stringPtr = dataSetNamePtr;
							
							stringPtr = strchr (stringPtr, '(');
							
							if (stringPtr != NULL)
								{
								stringPtr--;
								
								while (*stringPtr == ' ')
									stringPtr--;
									
								if (stringPtr > dataSetNamePtr)
									stringPtr[1] = 0;
								
								}	// end "if (stringPtr != NULL)"
						
							}	// end "if (dataSetNamePtr != NULL)"

						}	// end "if (pszHDF4SubdatasetName[0] != 0)"
							
					}	// end "else if (format == kHDF4Type2)"
					
				else	// format == kNITFType
					{
					snprintf (szKeyName, sizeof (szKeyName), "SUBDATASET_%d_DESC", (int)dataSet);
					szKeyName[sizeof (szKeyName) - 1] = '\0';
					pszHDF4SubdatasetName = CPLStrdup (CSLFetchNameValue (metadata, szKeyName));

					if (pszHDF4SubdatasetName[0] != 0)
						{
								// Use the characters before 'of ...'
						
						dataSetNamePtr = strstr (pszHDF4SubdatasetName, " of");
						*dataSetNamePtr = 0;
						dataSetNamePtr = pszHDF4SubdatasetName;
						lastLevelStringPtr = dataSetNamePtr;
						
						}	// end "if (pszHDF4SubdatasetName[0] != 0)"
					
					}	// end "else format == kNITFType"

				if (dataSetNamePtr != NULL)
					{
					length = (SInt16)strlen (dataSetNamePtr);
					
					numberCompareCharacters = MIN (
								numberCompareCharacters, (UInt16)strlen (lastLevelStringPtr));
					
					}	// end "if (dataSetNamePtr != NULL)"
				
				else	// dataSetNamePtr == NULL
					pszSubdatasetName[0] = 0;
									
				}	// end "if (pszSubdatasetName[0] != 0)"
				
			if (pszSubdatasetName[0] == 0)
				{
						// Implies that there is no name for the data set or more likely
						// one that I do not know how to handle.
						
				dataSetNamePtr = szKeyName;
				length = (SInt16)strlen (dataSetNamePtr);
				lastLevelStringPtr = dataSetNamePtr;
				
				}	// end "if (pszSubdatasetName[0] == 0)"
				
			length = MIN (length, 73);
			strncpy ((char*)&hdfDataSetsPtr[index].name[2], dataSetNamePtr, length);
			hdfDataSetsPtr[index].name[0] = length + 1;
			hdfDataSetsPtr[index].name[length+2] = 0;
			
			hdfDataSetsPtr[index].vRefNum = hdfDataSetsPtr[0].vRefNum;
			hdfDataSetsPtr[index].dirID = hdfDataSetsPtr[0].dirID;
			hdfDataSetsPtr[index].sdid = hDS;		// (SInt32)
			
			hdfDataSetsPtr[index].dataSetOffsetBytes = 0;
			
			hdfDataSetsPtr[index].dataSet = (SInt16)dataSet;
			hdfDataSetsPtr[index].dataSetType = 3;
			hdfDataSetsPtr[index].instrumentChannelNumber = 0;
			
					// Handle data set grouping
			
			hdfDataSetsPtr[index].groupedNumber = 0;
								
						// Check if first 4 characters of data set name is the same as the
						// previous data set. If so verify that the number of channels is 1
						// and the number of lines, columns, and data type are the same. If
						// so automatically group this data set with the previous one.
						
						// Note that if the data sets are stored in different files then
						// do not group. Current the logic used may fail if the first few 
						// data sets are not stored in extension files and later ones are.
						// Don't know if those exist. Some may get grouped before we find
						// out that we should not group them.  So are I have not seen those
						// cases.
						
			if (setNumberForGroup > 0 && 
							hdfDataSetsPtr[index].name[1] != '_' && hdfDataSetsPtr[index].name[1] != ' ')
				{
						// Verify that first 'numberCompareCharacters' characters of the 
						// last section of the data set name are the same as that for the 
						// first data set.
				/*						
				stringPtr = strrchr (dataSetNamePtr, '_');
				if (stringPtr == NULL)
					stringPtr = (char*)&hdfDataSetsPtr[index].name[2];
				else 
					stringPtr++;
				*/						
				if (strncmp (groupName, lastLevelStringPtr, numberCompareCharacters) == 0)
					{
							// Now verify that the number of lines, columns, data bytes and
							// data type are the same and that the number of channels is 1. 
							// Also verify that the compression is none.  If
							// so then group this data set with the first one.
							
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
							
					if (numberChannels != 1)
						setNumberForGroup = 0;
					
					if (setNumberForGroup > 0)
						{
						if (setNumberForGroup == 1)
							{
							groupNumber++;
						
							hdfDataSetsPtr[numberDataSets+groupNumber].name[0] = 
								sprintf ((char*)&hdfDataSetsPtr[numberDataSets+groupNumber].name[1], 
												" %s",
												groupName);
												
							hdfDataSetsPtr[numberDataSets+groupNumber].vRefNum = 0;
							hdfDataSetsPtr[numberDataSets+groupNumber].dirID = 0;
							hdfDataSetsPtr[numberDataSets+groupNumber].dataSetOffsetBytes = 0;
							hdfDataSetsPtr[numberDataSets+groupNumber].dataSet = 
																				(SInt16)(set0DataSet);
							hdfDataSetsPtr[numberDataSets+groupNumber].dataSetType = 0;
							hdfDataSetsPtr[numberDataSets+groupNumber].instrumentChannelNumber = 0;
							
							hdfDataSetsPtr[set0DataSet+1].groupedNumber = 
																				(SInt16)groupNumber;
						
									// Append indicater that data set is part of a group.
							
							AppendGroupIndicater ((char*)hdfDataSetsPtr[set0DataSet+1].name, 
																groupNumber);
									
							}	// end "if (setNumberForGroup == 1)"
						
						setNumberForGroup++;
						hdfDataSetsPtr[numberDataSets+groupNumber].groupedNumber = (SInt16)setNumberForGroup;
							
						hdfDataSetsPtr[index].groupedNumber = (SInt16)groupNumber;
						
								// Append indicater that data set is part of a group.
							
						AppendGroupIndicater ((char*)hdfDataSetsPtr[index].name, groupNumber);
								
						}	// end "if (setNumberForGroup > 0)"
					
					}	// end "if (strncmp () == 0"
					
				else	// strncmp != 0
					setNumberForGroup = 0;
				
				}	// end "if (setNumberForGroup > 0 && ...)"
					
			if (setNumberForGroup == 0)
				{
						// Save first 'numberCompareCharacters' characters of the 
						// last section of the data set name
				/*		
				stringPtr = strrchr (dataSetNamePtr, '_');
				if (stringPtr == NULL)
					stringPtr = (char*)&hdfDataSetsPtr[index].name[2];
				else 
					stringPtr++;
				*/				
				strncpy (groupName, lastLevelStringPtr, numberCompareCharacters);
				groupName[numberCompareCharacters] = 0;
				
						// Save information for first data set.
				
				set0NumberColumns = numberColumns;
				set0NumberLines = numberLines;
				set0DataTypeCode = dataTypeCode;
				set0DataCompressionCode = dataCompressionCode;
				set0NumberBytes = numberBytes;
				set0SignedDataFlag = signedDataFlag;
				
				set0DataSet = dataSet - 1;
						
				if (numberChannels == 1)
						setNumberForGroup = 1;
				
				}	// end "if (setNumberForGroup == 0)"
		
			index++;
			/*
			if (returnCode != noErr)
				{
				SDendaccess (sdid);
				break;
				
				}	// end "if (returnCode != noErr)"	
			*/
					// Don't need this string any more so free the memory up.
					
			if (pszHDF4SubdatasetName != NULL)
				CPLFree (pszHDF4SubdatasetName);
			
			CPLFree (pszSubdatasetName);
					
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					{
					index = 0;
					break;
					
					}	// end "if (!CheckSomeEvents (..."
					
				}	// end "if (TickCount () >= gNextTime)"
			
			}	// end "for (dataSet=1; dataSet<=numDataSets; dataSet++)"
			
		*hdfDataSetsHandlePtr = dataSetsHandle;
		*hdfDataSetsPtrPtr = hdfDataSetsPtr;
		*numDataSetsPtr = index - 1;
		
				// Check if the requested data set has already been identified which is 
				// indicated by the hdfDataSetSelection value being greater than 0 in the 
				// file info structure.
				// This happens when being opened as part of a project file being opened. 
				// Use that data set as the one to be read.
			
		if (fileInfoPtr->hdfDataSetSelection > 0 && 
											fileInfoPtr->hdfDataSetSelection <= *numDataSetsPtr)
			*firstDataSetIndexPtr = fileInfoPtr->hdfDataSetSelection;
				
				// If there are no valid data sets then just point to the first data set.
					
		if (*firstDataSetIndexPtr == 0)
			*firstDataSetIndexPtr = 1;
		
		*outGDALDatasetPtr = (GDALDatasetH)hdfDataSetsPtr[*firstDataSetIndexPtr].sdid;
		if (*outGDALDatasetPtr == 0)
			{
					// This handles case where not all of the datasets were opened. This 
					// can occur for MultiSpec Online when reading iRods fuse mounted files.
					
			fileInfoPtr->numberHdfDataSets = (UInt16)*numDataSetsPtr;
			*outGDALDatasetPtr = GetGDALFileReference (dataSetsHandle,
																	(UInt16)*numDataSetsPtr,
																	format, 
																	gdalDataSetH,
																	hdfDataSetsPtr,
																	*firstDataSetIndexPtr);
																	
			}	// end "if (*outGDALDatasetPtr == NULL)"
		
		if (gStatusDialogPtr != NULL)
			ListCPUTimeInformation (NULL, TRUE, startDataSetRead);
			
		if (statusDialogCreatedFlag)
			CloseStatusDialog (TRUE);
			
		}	// end "if (hdfDataSetsPtr != NULL && numberDataSets > 0)"
		
	return (index-1);
    
}	// end "LoadHdf5DataSetNames"
								


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 LoadHDF5HeaderInformation
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
// Called By:			FileSpecificationDialogSetHDFValues in SOpenDlg.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/21/2012
//	Revised By:			Larry L. Biehl			Date: 07/13/2016

SInt16 LoadHDF5HeaderInformation (
				HdfDataSets*						hdfDataSetsPtr,
				FileInfoPtr							fileInfoPtr,
				SInt32								dataSetIndex,
				Boolean								useGroupedDataSetsFlag)
				
{
	GDALDatasetH						hDS;
	SInt16								returnCode = noErr;
	
		
	if (hdfDataSetsPtr != NULL)
		{
		hDS = GetGDALFileReference (fileInfoPtr->hdfHandle,
												fileInfoPtr->numberHdfDataSets,
												fileInfoPtr->format,
												fileInfoPtr->gdalDataSetH,
												hdfDataSetsPtr,
												dataSetIndex);
		
		returnCode = LoadGDALHeaderInformation (
										//(GDALDatasetH)hdfDataSetsPtr[dataSetIndex].sdid,
										hDS,
										fileInfoPtr,
										fileInfoPtr->format);
										
		if (returnCode == 1)
					// Adjust to indicate that at this point this is not an HDF5 image file
			returnCode = 7;
				
		if (returnCode == noErr)
			returnCode = SetUpHDF5_FileInformation (fileInfoPtr, 
																	hdfDataSetsPtr,
																	dataSetIndex,
																	useGroupedDataSetsFlag);
																													
		if (returnCode == noErr)
			returnCode = ReadGDALProjectionInformation (
										fileInfoPtr, 
										(GDALDatasetH)hdfDataSetsPtr[dataSetIndex].sdid);
																	
		}	// end "if (hdfDataSetsPtr != NULL)"
															
	return (returnCode);
    
}	// end "LoadHDF5HeaderInformation"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadDataSetNumerical
//
//	Software purpose:	This routine reads the numerical data from the specific group 
//							name.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2012
//	Revised By:			Larry L. Biehl			Date: 04/25/2012

SInt16 ReadDataSetNumerical (
				hid_t									file_id,
				char*									dataSetStringPtr,
				double*								returnValuePtr)
				
{	
	
	hid_t									dataset_id;
											//pid,
											//sid,
											//type_class,
											//type_id;
											
	//hsize_t							size;
	//hssize_t							ssiz;
	//size_t								tsiz;
	
	//float								value;
 	
 	SInt16								returnCode = 0;
	

	if (file_id > 0)
		{
		dataset_id = H5Dopen2 (file_id, dataSetStringPtr, H5P_DEFAULT);
		if (dataset_id > 0)
			{
					// The data set was found
					
			//size = H5Dget_storage_size (dataset_id);
			//pid = H5Dget_create_plist (dataset_id);
			//sid = H5Dget_space (dataset_id);
			//type_id = H5Dget_type (dataset_id);
			//type_class = H5Tget_class (type_id);
			//if (type_class == H5T_FLOAT)
			//	{
			if (H5Dread (dataset_id, 
								H5T_NATIVE_DOUBLE, 
								H5S_ALL, 
								H5S_ALL, 
								H5P_DEFAULT, 
								returnValuePtr) >= 0)
					returnCode = 1;					
				
			//	}	// end "if (type_class == H5T_FLOAT)"
			
			//H5Sclose (sid);
			//H5Pclose (pid);
			//H5Tclose (type_id);
			
			}	// end "if (dataset > 0)"
		
		H5Dclose (dataset_id);
		
		}	// end "if (file_id > 0)"
		
	return (returnCode);
    
}	// end "ReadDataSetNumerical"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 ReadDataSetNumerical
//
//	Software purpose:	This routine reads the numerical data from the specific group 
//							name.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/04/2012
//	Revised By:			Larry L. Biehl			Date: 05/14/2012

UInt32 ReadDataSetNumerical (
				hid_t									dataset_id,
				hid_t									dataspace,
				UInt32								maxNumberValues,
				double*								returnValuePtr,
				UInt32*								numberPossibleValuesPtr)
				
{		
	hid_t									space_id,
											type_id;
											
	hsize_t								dim;
	hssize_t								dataSpaceSize;
											
	size_t								type_size;
	
 	SInt16								returnCode = 0;
	

	if (dataset_id > 0 && dataspace > 0)
		{
		type_id = H5Dget_type (dataset_id);
		type_size = H5Tget_size (type_id);
		dataSpaceSize = H5Sget_simple_extent_npoints (dataspace);
		dim = dataSpaceSize;
		
		if (numberPossibleValuesPtr != NULL)
			*numberPossibleValuesPtr = dim;
		
		space_id = H5S_ALL;
		if (dataSpaceSize > (SInt32)maxNumberValues)
			{
			dim =	maxNumberValues;
			space_id = H5Screate_simple (1, &dim, NULL);
			
			}	// end "if (dataSpaceSize > maxNumberValues)"
		
		//if (dataSpaceSize <= maxNumberValues)
		if (space_id >= 0)
			{
			if (H5Dread (dataset_id, 
								H5T_NATIVE_DOUBLE, 
								space_id, 
								space_id, 
								H5P_DEFAULT, 
								returnValuePtr) >= 0)
				returnCode = (SInt16)dim;
				
			}	// end "if (dataSpaceSize == 1)"
			
		H5Tclose (type_id);
		
		if (space_id != H5S_ALL && space_id > 0)
			H5Sclose (space_id);
		
		}	// end "if (dataset_id > 0 && dataspace > 0)"
		
	return (returnCode);
    
}	// end "ReadDataSetNumerical"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadDataSetString
//
//	Software purpose:	This routine determines if the specified data set string agrees
//							with the user desired input string.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2012
//	Revised By:			Larry L. Biehl			Date: 05/04/2012

SInt16 ReadDataSetString (
				hid_t									file_id,
				char*									dataSetStringPtr,
				char*									returnStringPtr,
				UInt32								maxStringLength)
				
{	
	hid_t									dataset_id,
											dataspace;
 	
 	SInt16								length = 0;
	

	if (file_id > 0)
		{
		dataset_id = H5Dopen2 (file_id, dataSetStringPtr, H5P_DEFAULT);
		if (dataset_id > 0)
			{
			dataspace = H5Dget_space (dataset_id);
			
			if (dataspace > 0)
				length = ReadDataSetString2 (dataset_id,
														dataspace,
														returnStringPtr,
														maxStringLength);
		
			H5Sclose (dataspace);
			H5Dclose (dataset_id);
						
			}	// end "if (dataset_id > 0)"
		
		}	// end "if (file_id > 0)"
		
	return (length);
    
}	// end "ReadDataSetString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadDataSetString2
//
//	Software purpose:	This routine determines if the specified data set string agrees
//							with the user desired input string.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/25/2012
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

SInt16 ReadDataSetString2 (
				hid_t									dataset_id,
				hid_t									dataspace,
				char*									returnStringPtr,
				UInt32								maxReturnStringLength)
				
{	
	char									charString[1024];
	
	char									*copyStringPtr,
											*stringPtr;
	
	hid_t									space_id,
											type_id;
	
	hsize_t								dim;
	hssize_t								dataSpaceSize;
											
	htri_t								variableLengthReturn;
	
	H5T_class_t							type_class;
											
	size_t								count,
											length = 0,
											stringLength,
											type_size;
	
	UInt32								maxStringLength,
											typeSizeLengthWithSeparator;
 	
 	SInt16								returnCode = -1;

	Boolean								freeStringFlag = FALSE,
											listMoreInfoFlag = FALSE;
	

	stringLength = 0;
	if (dataset_id > 0 && dataspace > 0)
		{
		maxStringLength = sizeof (charString) - 20;
		
		type_id = H5Dget_type (dataset_id);
		type_class = H5Tget_class (type_id);
		type_size = H5Tget_size (type_id);
		variableLengthReturn = H5Tis_variable_str (type_id);
		dataSpaceSize = H5Sget_simple_extent_npoints (dataspace);
		stringLength = type_size * dataSpaceSize;
		
		dim = dataSpaceSize;
		
				// Do not include separator if there is only one value or if the
				// type size is only 1 byte long implying a character. Not sure if
				// this is the correct way to handle all situations. May one wants
				// to have a list of single digit numbers.
				
		typeSizeLengthWithSeparator = (UInt32)type_size;
		if (dim > 1 && type_size > 1)
			typeSizeLengthWithSeparator += 2;
			
		space_id = H5S_ALL;
		if (dataSpaceSize*typeSizeLengthWithSeparator > maxStringLength-4)
			{
			dim =	(maxStringLength-4)/typeSizeLengthWithSeparator;
			space_id = H5Screate_simple (1, &dim, NULL);
			
			stringLength = dim * typeSizeLengthWithSeparator;
			
			}	// end "if (dataSpaceSize > maxStringLength)"
		
		//if (type_class == H5T_STRING && type_size * dataSpaceSize <= maxStringLength)
		if (type_class == H5T_STRING && space_id >= 0)
			{
			if (variableLengthReturn)
				{
						// The address of the string is returned

				stringPtr = NULL;
				freeStringFlag = TRUE;
				returnCode = H5Dread (dataset_id, 
												type_id, 
												H5S_ALL, 
												H5S_ALL, 
												H5P_DEFAULT, 
 												&stringPtr);
												
				type_size = 0;
				if (stringPtr != NULL)
					type_size = strlen (stringPtr);
				
						// Need to allow for 'continuation characters'

				maxReturnStringLength -= 3;
				if (type_size > maxReturnStringLength)
					listMoreInfoFlag = TRUE;
					
				type_size = MIN (type_size, maxReturnStringLength);

				}	// end "if (variableLengthReturn)"
									
			else	// !variableLengthReturn
				{
						// The string itself is returned
						
				stringPtr = &charString[0];
				returnCode = H5Dread (dataset_id, 
												type_id, 
												space_id, 
												space_id, 
												H5P_DEFAULT, 
												stringPtr);
									
				stringPtr[stringLength] = 0;				
									
				}	// end "else !variableLengthReturn"
									
			if (returnCode >= 0)
				{	
						// Copy string to output buffer.
						
				stringLength = 0;
				copyStringPtr = stringPtr;
				if (copyStringPtr != NULL)
					{
					for (count=0; count<dim; count++)
						{
						strncpy (&returnStringPtr[stringLength], copyStringPtr, type_size);
						stringLength += type_size;
						copyStringPtr += type_size;
						
						if (dim > 1 && type_size > 1)
							{
							length = sprintf (&returnStringPtr[stringLength], "| ");
							stringLength += length;
							
							}	// end "if (dim > 1 && type_size > 1)"
						
						}	// end "for (count=0; count<dim; count++)"
						
					}	// end "if (copyStringPtr != NULL)"
					
				if (dim < (hsize_t)dataSpaceSize || listMoreInfoFlag)
					{
					length = sprintf (&returnStringPtr[stringLength], "...");
					stringLength += length;
					
					}	// end "if (dim < dataSpaceSize || listMoreInfoFlag)"
					
				returnStringPtr[stringLength] = 0;
				
				}	// end "if (returnCode >= 0)"

			if (freeStringFlag && stringPtr != NULL)
				free (stringPtr);
				
			}	// end "if (type_class == H5T_STRING && space_id >= 0"
			
		H5Tclose (type_id);
		
		if (space_id != H5S_ALL && space_id > 0)
			H5Sclose (space_id);
						
		}	// end "if (dataset_id > 0 && dataspace > 0)"
		
	return ((SInt16)stringLength);
    
}	// end "ReadDataSetString2"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetDataSetValueAsNumericalValue
//
//	Software purpose:	This routine reads the value for the input hdf5 data set whether
//							string or numberical and returns the value as a double value.
//							if there are more than one value in the string, then nothing is
//							returned.
//
//	Parameters in:					
//
//	Parameters out:	returnCode = 1 if value found; 0 if not found
//
//	Value Returned:	data value
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/05/2012
//	Revised By:			Larry L. Biehl			Date: 05/05/2012

SInt16 GetDataSetValueAsNumericalValue (
				hid_t									file_id,
				char*									dataSetStringPtr,
				double*								valuePtr)
				
{	
	char									returnString[80];
	
	hid_t									dataset_id,
											dataspace,
											type_id;
	
	H5T_class_t							type_class;
 	
 	SInt16								length,
											returnCode;
	

	returnCode = 0;
	
	dataset_id = H5Dopen2 (file_id, dataSetStringPtr, H5P_DEFAULT);
	
	if (dataset_id > 0)
		{
		dataspace = H5Dget_space (dataset_id);
		type_id = H5Dget_type (dataset_id);
		type_class = H5Tget_class (type_id);
		
		if (type_class == H5T_STRING)
			{
			length = ReadDataSetString2 (dataset_id,
													dataspace,
													returnString,
													79);
													
			if (length > 0)
				{
				*valuePtr = strtod (returnString, NULL);
				if (errno != ERANGE)
					returnCode = 1;
				
				}	// end "if (length > 0)"
													
			}	// end "if (type_class == H5T_STRING)"
													
		if (type_class == H5T_INTEGER || type_class == H5T_FLOAT)
			{
			if (ReadDataSetNumerical (dataset_id, dataspace, 1, valuePtr, NULL))
				returnCode = 1;
			
			}	// end "if (type_class == H5T_INTEGER || type_class == H5T_FLOAT)"
					
		H5Tclose (type_id);
		H5Sclose (dataspace);
		H5Dclose (dataset_id);
		
		}	// end "if (dataset_id > 0)"
		
	return (returnCode);
    
}	// end "GetDataSetValueAsNumericalValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetDataSetValueAsString
//
//	Software purpose:	This routine reads the value for the input hdf5 data set.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/03/2012
//	Revised By:			Larry L. Biehl			Date: 05/14/2012

SInt16 GetDataSetValueAsString (
				hid_t									dataset_id,
				hid_t									dataspace,
				char*									returnStringPtr,
				UInt32								maxReturnStringLength)
				
{	
	double								doubleValues[100];
	
	hid_t									type_id;
	
	H5T_class_t							type_class;
	
	UInt32								maxStringLength,
											maxNumberValues,
											numberPossibleValues,
											numberValuesRead,
											valueIndex;
 	
 	SInt16								length = 0,
											totalLength = 0;
	

	if (dataset_id > 0)
		{
		type_id = H5Dget_type (dataset_id);
		type_class = H5Tget_class (type_id);
		
		if (type_class == H5T_STRING)
			totalLength = ReadDataSetString2 (dataset_id,
															dataspace,
															returnStringPtr,
															maxReturnStringLength);
													
		else if (type_class == H5T_INTEGER || type_class == H5T_FLOAT)
			{
			maxNumberValues = sizeof (doubleValues)/sizeof (double);
			
					// Do not allow memory to be overrun.
					// Don't do anything if max return string length is less than 20;
			maxStringLength = 0;
			if (maxReturnStringLength > 20)
				maxStringLength = maxReturnStringLength - 20;
			
			if ((numberValuesRead = ReadDataSetNumerical (dataset_id, 
																			dataspace, 
																			maxNumberValues, 
																			doubleValues, 
																			&numberPossibleValues)) != 0)
				{				
				for (valueIndex=0; valueIndex<numberValuesRead; valueIndex++)
					{
					if (totalLength <= (SInt16)maxStringLength)
						{
						if (type_class == H5T_INTEGER)
							length = sprintf (returnStringPtr, 
														"%.0f, ", 
														doubleValues[valueIndex]);
						
						else	// type_class == H5T_FLOAT
							length = sprintf (returnStringPtr, 
														"%f, ", 
														doubleValues[valueIndex]);
							
						returnStringPtr += length;
					
						totalLength += length;
							
						}	// end "if (totalLength < maxStringLength)"
						
					else	// totalLength > maxStringLength
						break;
						
					}	// end "for (valueIndex=0; valueIndex<numberValuesRead; ..."
					
				if (valueIndex != numberPossibleValues)
					{
					length = sprintf (returnStringPtr, "...");
					totalLength += length;
					
					}	// end "if (valueIndex != numberPossibleValues)"
				
				else if (totalLength > 1)	
							// Remove the last comma and space	
					totalLength -= 2;
				
				}	// end "if (ReadDataSetNumerical (dataset_id, dataspace, ..."
			
			}	// end "if (type_class == H5T_INTEGER || type_class == H5T_FLOAT)"
													
		else	// type_class != STRING, INTEGER or FLOAT
			{
			char				typeName[16];
			
			switch (type_class)
				{
				case H5T_TIME:
					sprintf (typeName, "Time");
					break;
					
				case H5T_BITFIELD:
					sprintf (typeName, "Bitfield");
					break;
					
				case H5T_OPAQUE:
					sprintf (typeName, "Opaque");
					break;
					
				case H5T_COMPOUND:
					sprintf (typeName, "Compound");
					break;
					
				case H5T_REFERENCE:
					sprintf (typeName, "Reference");
					break;
					
				case H5T_ENUM:
					sprintf (typeName, "Enum");
					break;
					
				case H5T_VLEN:
					sprintf (typeName, "Vlen");
					break;
					
				case H5T_ARRAY:
					sprintf (typeName, "Array");
					break;
					
				default:
					sprintf (typeName, "Unknown");
					break;
					
				}	// end "switch (type_class)"
										
			totalLength = sprintf (returnStringPtr, 
											"MultiSpec cannot handle the '%s' datatype.",
											typeName);
			
			}	// end "else type_class != STRING, INTEGER or FLOAT"
			
		H5Tclose (type_id);
		
		}	// end "if (dataset_id > 0)"
		
	return (totalLength);
    
}	// end "GetDataSetValueAsString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpHDF5_FileInformation
//
//	Software purpose:	This sets up the file information that is specific for HDF5
//							format including automatic grouping of data sets and the channel
//							to hdf5 data set vector.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	none.
//
// Called By:			ReadHeaderWithGDALLibrary in SGDALInterface.cpp
//							LoadHDF5HeaderInformation in SHDF5.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/21/2012
//	Revised By:			Larry L. Biehl			Date: 07/30/2018

SInt16 SetUpHDF5_FileInformation (
				FileInfoPtr							fileInfoPtr,
				HdfDataSets*						hdfDataSetsPtr,
				SInt32								hdfDataSetSelection,
				Boolean								useGroupedDataSetsFlag)
				
{	
	SInt16								groupNumber,
											numberDataSets,
											returnCode = noErr;
											
		
			// Now determine if this is part of a group of hdf5 data sets.
			
	numberDataSets = fileInfoPtr->numberHdfDataSets;
	
	if (numberDataSets > 0 && hdfDataSetsPtr != NULL)
		{
		if (useGroupedDataSetsFlag &&
									hdfDataSetsPtr[hdfDataSetSelection].groupedNumber > 0)	
			{					
			groupNumber = hdfDataSetsPtr[hdfDataSetSelection].groupedNumber;
			
			fileInfoPtr->numberChannels = 
							(UInt16)hdfDataSetsPtr[numberDataSets+groupNumber].groupedNumber;
				
			}	// end "if (hdfDataSetsPtr[dataSetIndex].groupedNumber > 0)"
		
				// This forces some structures to be set up for possible sub data sets.
		
		fileInfoPtr->callGetHDFLineFlag = TRUE;
		
		returnCode = SetUpHDF_FileInformation (fileInfoPtr, 
																hdfDataSetsPtr,
																hdfDataSetSelection,
																useGroupedDataSetsFlag);
																
		}	// end "if (numberDataSets > 0 && hdfDataSetsPtr != NULL)"
		
	return (returnCode);
    
}	// end "SetUpHDF5_FileInformation"
																		
#endif	// include_hdf5_capability						
#endif	// include_gdal_capability
