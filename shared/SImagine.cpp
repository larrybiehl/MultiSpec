//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SImagine.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/18/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines which are used to access
//								ERDAS Immagine formatted disk files.
//
//	Functions in file:	
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	
#if defined multispec_mac
#endif	// defined multispec_mac    

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif

#if defined multispec_win
	#include "CImageWindow.h"
	#include "WImageDoc.h"
	#include "WFileFormatDialog.h"
#endif	// defined multispec_win

#include "errno.h"

//#include "SExtGlob.h"	

#define	kNotPackedDegrees			0
#define	kRadians						2								
			

#if defined multispec_win || defined multispec_lin
	#pragma pack (push, 2)
#endif	// defined multispec_win || ...
	
//#if PRAGMA_ALIGN_SUPPORTED
#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#endif
	
typedef struct Edms_State
	{
	SInt32							numVirtualBlocks;
	SInt32							numObjectsPerBlock;
	SInt32							nextObjectNum;
	SInt16							compressionType;
	SInt32							numberBlockInfos;
	UInt32							virtualBlockInfoPtr;
	
	} Edms_State;
	
	
typedef struct Edms_VirtualBlockInfo
	{
	SInt16							fileCode;
	UInt32							offset;
	SInt32							size;
	SInt16							logValid;
	SInt16							compressionType;
	
	} Edms_VirtualBlockInfo;
	
	
typedef struct Edsc_BinFunction
	{
	SInt32							numBins;
	SInt16							binFunctionType;
	double							minLimit;
	double							maxLimit;
	SInt32							baseData1;
	SInt32							baseData2;
	
	} Edsc_BinFunction;
	
	
typedef struct Edsc_Column
	{
	SInt32							numRows;
	UInt32							columnDataPtr;
	SInt16							dataType;
	SInt32							maxNumChars;
	
	} Edsc_Column;
	
	
typedef struct Ehfa_Entry
	{
	UInt32							next;
	UInt32							prev;
	UInt32							parent;
	UInt32							child;
	UInt32							data;
	UInt32							dataSize;
	Str63								name;
	Str31								type;
	UInt32							modTime;
	
	} Ehfa_Entry;
	
	
typedef struct Ehfa_File
	{
	SInt32							version;
	UInt32							freeList;
	UInt32							rootEntryPtr;
	SInt16							entryHeaderLength;
	UInt32							dictionaryPtr;
	
	} Ehfa_File;
	
	
typedef struct Eimg_Layer
	{
	SInt32							width;
	SInt32							height;
	UInt16							layerType;
	UInt16							pixelType;
	SInt32							blockWidth;
	SInt32							blockHeight;
	
	} Eimg_Layer;	
	
typedef struct Eprj_Datum
	{
	Str63								name;
	SInt16							type;
	double							params[7];
	Str63								gridName;
	
	} Eprj_Datum;
	
	
typedef struct Eprj_MapInfo
	{
	Str63								projectionName;
	double							xMapCoordinate11;
	double							yMapCoordinate11;
	double							xMapCoordinateLowerRight;
	double							yMapCoordinateLowerRight;
	double							horizontalPixelSize;
	double							verticalPixelSize;
	Str31								mapUnits;
	
	} Eprj_MapInfo;
	
	
typedef struct Eprj_Spheroid
	{
	Str63								sphereName;
	double							a;
	double							b;
	double							eSquared;
	double							radius;
	SInt16							sphereCode;
	
	} Eprj_Spheroid;
	
	
typedef struct Eprj_ProParameters
	{
	UInt16							proType;
	SInt32							proNumber;
	Str63								proExeName;
	Str63								proName;
	SInt32							proZone;
	double							proParams[15];
	Eprj_Spheroid*					proSpheroid;
	
	} Eprj_ProParameters;
	
	
typedef struct Esta_Statistics
	{
	double							minimum;
	double							maximum;
	double							mean;
	double							median;
	double							mode;
	double							stddev;
	
	} Esta_Statistics;
	
typedef struct ExternalRasterDMS
	{
	UInt32							nameLength;
	UInt32							nameOffset;
	Str255							fileName;
	SInt64							layerStackValidFlagsOffset;
	SInt64							layerStackDataOffset;
	UInt32							layerStackCount;
	UInt32							layerStackIndex;
	
	} ExternalRasterDMS;
	
typedef struct LayerStackPrefix
	{
//	char								unknown;
	UInt32							layerCount;
	UInt32							width;
	UInt32							height;
	UInt32							blockWidth;
	UInt32							blockHeight;
	UInt16							unknown2;
	
	} LayerStackPrefix;
	
typedef struct LayerStackValidFlags
	{
	UInt32							unknown;
	UInt32							unknown2;
	UInt32							heightInBlocks;
	UInt32							widthInBlock;
	UInt32							unknown3;
	
	} LayerStackValidFlags;

//#if PRAGMA_ALIGN_SUPPORTED
#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#endif 
	
#if defined multispec_win || defined multispec_lin
	#pragma pack (pop, 2)
#endif	// defined multispec_win || ...

		

			// Prototypes for routines in this file that are only called by		
			// other routines in this file.	

SInt16 				DetermineDatumCode (
							char*									inputStringPtr);

SInt16 				DetermineEllipsoidCode (
							char*									inputStringPtr);

SInt16 				DetermineGridCoordinateCode (
							char*									inputStringPtr);
							
Boolean 				GetEdscColumnNode (
							CMFileStream* 						fileStreamPtr,
							Ehfa_Entry*							ehfaEntryLayerPtr,
							Ehfa_Entry*							ehfaEntryEdscColumnPtr,
							Ehfa_Entry*							ehfaEntryEdscBinFunctionPtr,
							CharPtr								edscColumnNodeNamePtr);

Boolean 				GetDefaultImaginePaletteInfo (
							CMFileStream*						fileStreamPtr, 
							Edsc_Column*						edscColumnRedPtr,
							Edsc_Column*						edscColumnGreenPtr,
							Edsc_Column*						edscColumnBluePtr,
							Edsc_BinFunction*					edscBinFunctionPtr);

Boolean 				GetFirstImagineLayerNode (
							CMFileStream* 						fileStreamPtr,
							Ehfa_Entry*							ehfaEntryPtr);
							
Boolean 				GetNextNode (
							CMFileStream* 						fileStreamPtr,
							Ehfa_Entry*							ehfaEntryParentPtr,
							Ehfa_Entry*							ehfaEntryRequestedPtr,
							char*									nodeTypeStringPtr,
							Boolean								startFlag);

Boolean 				GetLayerForRequestedChannel (
							CMFileStream* 						fileStreamPtr,
							Ehfa_Entry*							ehfaEntryLayerPtr,
							UInt32*								currentLayerChannelPtr,
							UInt32								channel);

Boolean 				LoadImagineImageMapInfoForChannel (
							FileInfoPtr							fileInfoPtr, 
							UInt32								channel, 
							UInt32*								currentLayerChannelPtr,
							Boolean								mapInfoFlag);

Boolean 				ReadImagineClassNameColumn (
							CMFileStream*						fileStreamPtr,
							Edsc_Column*						edscColumnClassNamePtr,
							Edsc_BinFunction*					edscBinFunctionPtr, 
							UCharPtr								classNamePtr);

Boolean 				ReadImagineClassPaletteColumns (
							FileInfoPtr							fileInfoPtr, 
							Edsc_Column*						edscColumnRedPtr,
							Edsc_Column*						edscColumnGreenPtr,
							Edsc_Column*						edscColumnBluePtr,
							Edsc_BinFunction*					edscBinFunctionPtr, 
							ColorSpec*							colorSpecPtr, 
							DisplaySpecsPtr					displaySpecsPtr, 
							UInt16*								classSymbolPtr, 
							UInt16*								paletteCodePtr);

Boolean 				ReadImagineEdscBinFunction (
							CMFileStream*						fileStreamPtr,
							UInt32								edscBinFunctionOffset,
							UInt32								count,
							Edsc_BinFunction*					edscBinFunctionPtr);
							
SInt16				ReadImagineExternalDMSLayerBlockParameters (
							FileInfoPtr							fileInfoPtr,
							CMFileStream*						fileStreamPtr,
							SInt64								layerStackValidFlagsOffset);

SInt16				ReadImagineExternalRasterDMS (
							CMFileStream*						fileStreamPtr,
							UInt32								externalRasterDMSOffset,
							ExternalRasterDMS*				externalRasterDMSPtr);
				
SInt16 				ReadImagineHeader (
							FileInfoPtr 						fileInfoPtr, 
							char*									headerRecordPtr);

SInt16 				ReadImagineEdmsState (
							CMFileStream*						fileStreamPtr,
							UInt32								edmsStateOffset,
							Edms_State*							edmsStatePtr);

SInt16 				ReadImagineEdmsVirtualBlockInfo (
							CMFileStream*						fileStreamPtr,
							UInt32								numVirtualBlocks,
							UInt32								edmsVirtualBlockInfoOffset,
							Edms_VirtualBlockInfo*			edmsVirtualBlockInfoPtr);

Boolean 				ReadImagineEdscColumn (
							CMFileStream*						fileStreamPtr,
							UInt32								edscColumnOffset,
							UInt32								count,
							Edsc_Column*						edscColumnPtr);

SInt16 				ReadImagineEhfaEntry (
							CMFileStream*						fileStreamPtr,
							UInt32								ehfaOffset,
							Ehfa_Entry*							ehfaEntryPtr);

SInt16 				ReadImagineEhfaFile (
							CMFileStream*						fileStreamPtr,
							UInt32								ehfaFileOffset,
							Ehfa_File*							ehfaFilePtr);
/*
SInt16 				ReadImagineEhfaLayer (
							CMFileStream*						fileStreamPtr,
							UInt32								ehfaLayerOffset,
							UInt32								count,
							Ehfa_Layer*							ehfaLayerPtr);
*/
SInt16 				ReadImagineEimgLayer (
							CMFileStream*						fileStreamPtr,
							UInt32								eimgLayerOffset,
							UInt32								count,
							Eimg_Layer*							eimgLayerPtr);

Boolean 				ReadImageEstaStatistics (
							CMFileStream*						fileStreamPtr,
							UInt32								estaStatisticsOffset,
							UInt32								count,
							Esta_Statistics*					estaStatisticsPtr);

Boolean 				ReadImageEprjDatum (
							CMFileStream*						fileStreamPtr,
							UInt32								eprjDatumOffset,
							UInt32								count,
							Eprj_Datum*							eprjDatumPtr);

Boolean 				ReadImageEprjMapInfo (
							CMFileStream*						fileStreamPtr,
							UInt32								eprjMapInfoOffset,
							UInt32								count,
							Eprj_MapInfo*						eprjMapInfoPtr);

Boolean 				ReadImageEprjMapProjection (
							CMFileStream*						fileStreamPtr,
							UInt32								eprjMapProjectionOffset,
							UInt32								count,
							Eprj_ProParameters*				eprjProParametersPtr,
							Eprj_Spheroid*						eprjSpheroidPtr);

Boolean 				ReadImageEprjProParameters (
							CMFileStream*						fileStreamPtr,
							UInt32								eprjProParametersOffset,
							UInt32								count,
							Eprj_ProParameters*				eprjProParametersPtr,
							Eprj_Spheroid*						eprjSpheroidPtr);

Boolean 				ReadImageSpheroid (
							UInt32								eprjSpheroidOffset,
							UInt32								count,
							UCharPtr								bufferPtr,
							Eprj_Spheroid*						eprjSpheroidPtr,
							UCharPtr								endBufferPtr);

Boolean 				ReadImagineHistogramColumn (
							FileInfoPtr							fileInfoPtr,
							CMFileStream*						supportFileStreamPtr,
							HistogramSummaryPtr				histogramSummaryPtr,
							UInt32								histogramColumnOffset,
							UInt32								numberValues,
							UInt16								dataType,
							UInt32								numBins,
							UInt16								binFunctionType,
							double								doubleMinLimit,
							double								doubleMaxLimit,
							HUInt32Ptr							histogramArrayPtr,
							HCharPtr								histrogramBufferPtr);

Boolean 				ReadImagineImageParameters (
							FileInfoPtr 						fileInfoPtr, 
							CMFileStream*						fileStreamPtr,
							Ehfa_Entry*							rootEhfaPtr,
							SInt16*								formatErrorCodePtr);
				
Boolean 				ReadImagineLayerBlockParameters (
							FileInfoPtr 						fileInfoPtr, 
							CMFileStream*						fileStreamPtr,
							Ehfa_Entry*							ehfaEntryLayerPtr,
							SInt16*								formatErrorCodePtr,
							FileStringName255*				extensionFileNamePtr,
							SInt64*								layerStackValidFlagsOffsetPtr,
							UInt32								channel);	

void 					ReadImagineNumberOfClasses (
							FileInfoPtr							fileInfoPtr,
							Ehfa_Entry*							ehfaEntryLayerPtr);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckForImagineEstaStatistics
//
//	Software purpose:	This routine determines if the Esta Statistics (image
// 						statistics) is available for all channels in the image.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			GetDefaultSupportFile in SOpnImag.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 09/05/2017
				
Boolean CheckForImagineEstaStatistics (
				Handle								windowInfoHandle)
{
	CMFileStream						*fileStreamPtr,
											*supportFileStreamPtr;
											
	FileStringPtr						supportFilePathPtr;
	
	Ehfa_Entry							ehfaEntryLayer,
											ehfaEntryEstaStatistics;
											
	UInt32								channel,
											numberChannels;
											
	SInt16								errCode;
	
	Boolean								continueFlag,
											estaStatisticsExistFlag,
											supportFileAlreadyOpenFlag,
											supportFileStreamCreatedFlag;
											
	SignedByte							fileHandleStatus,
											supportFileHandleStatus;
										
		
			// Initialize local variables.
	
	estaStatisticsExistFlag = FALSE;
	supportFileStreamCreatedFlag = FALSE;

	fileStreamPtr = GetFileStreamPointer (windowInfoHandle, &fileHandleStatus);
	
			// Now get the file where the image statistics should be.  Note that if the image is loaded
			// in a .ige file, we will need to get the associated .img file to get the statistics.
			
	supportFileStreamPtr = GetSupportFileStreamPointer (windowInfoHandle, &supportFileHandleStatus);
	
	if (!FileExists (supportFileStreamPtr))
		{
		supportFileStreamCreatedFlag = TRUE;
		
				// Save copy of current support file stream in case we need it
				// later, particularly the name.
				
		CopyFileStream (supportFileStreamPtr, fileStreamPtr);
			
					// Make sure that this file stream setting is closed.
					
		IndicateFileClosed (supportFileStreamPtr);
		
				// Remove the .ige and other suffixes and make sure that .img is the suffix
				
		supportFilePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (supportFileStreamPtr);
		RemoveSuffix (supportFilePathPtr);
		ConcatFilenameSuffix (supportFilePathPtr, (StringPtr)"\0.img\0");
					
		}	// end "if (!FileExists (supportFileStreamPtr))"
		
	supportFileAlreadyOpenFlag = FileOpen (supportFileStreamPtr);
	if (!supportFileAlreadyOpenFlag)
		{
		errCode = OpenFileReadOnly (supportFileStreamPtr, 
												kResolveAliasChains,
												kLockFile, 
												kVerifyFileStream);
	
		if (errCode != fnfErr && errCode != bdNamErr)
			IOCheck (errCode, supportFileStreamPtr);
		
		}	// end "if (!supportFileAlreadyOpenFlag)"
	
			// Set up swap bytes flag depending upon the system
			// architecture
			                 
	gSwapBytesFlag = gBigEndianFlag; 
	
			// Get the first layer node.
	
	continueFlag = GetFirstImagineLayerNode (supportFileStreamPtr, &ehfaEntryLayer);
														
			// Loop through the layer nodes.		
				
	channel = 0;
	numberChannels = GetTotalNumberOfChannels (windowInfoHandle);
	while (channel < numberChannels)
		{
		estaStatisticsExistFlag = FALSE;
		
		if (continueFlag)
			//continueFlag = GetEstaStatisticsNode (fileStreamPtr,
			//														&ehfaEntryLayer,
			//														&ehfaEntryEstaStatistics);
			continueFlag = GetNextNode (supportFileStreamPtr,
													&ehfaEntryLayer,
													&ehfaEntryEstaStatistics,
													(char*)"Esta_Statistics",
													TRUE);
				
		if (continueFlag)
					// Esta statisics found for this layer (or channel).
			estaStatisticsExistFlag = TRUE;
		
		else	// !continueFlag
			break;
													
		channel++;
			
				// Get the next node at this same level.
		
		continueFlag = GetNextNode (supportFileStreamPtr, 
												NULL,
												&ehfaEntryLayer,
												(char*)"Eimg_Layer",
												FALSE);
				
		}	// end "while (channel < numberChannels)"											
				
	gSwapBytesFlag = FALSE;
				
			// Close the default support file if it was closed upon entry to this
			// routine.								
	
	//if (!supportFileAlreadyOpenFlag)
	//	CloseFile (supportFileStreamPtr);

	if (!estaStatisticsExistFlag && supportFileStreamCreatedFlag)
		SetFileDoesNotExist (supportFileStreamPtr, kDoNotKeepUTF8CharName);
			
	UnlockFileStream (windowInfoHandle, fileHandleStatus);
	UnlockSupportFileStream (windowInfoHandle, supportFileHandleStatus);
		
	return (estaStatisticsExistFlag);
	
}	// end "CheckForImagineEstaStatistics" 

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckIfDefaultImaginePaletteExists
//
//	Software purpose:	This routine loads the color palette information from the
//							input imagine formatted thematic image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 10/22/1999

Boolean CheckIfDefaultImaginePaletteExists (
				FileInfoPtr							fileInfoPtr)

{
	Edsc_BinFunction					edscBinFunction;
	
	Edsc_Column							edscColumnBlue,
											edscColumnGreen,
											edscColumnRed;
	
	CMFileStream*						fileStreamPtr;
	
	Boolean								continueFlag = FALSE;
	
	
			// Initialize local variables.
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	if (fileStreamPtr != NULL)	
		{	
				// Set up swap bytes flag depending upon the system
				// architecture
				                 
		gSwapBytesFlag = gBigEndianFlag;

		continueFlag = GetDefaultImaginePaletteInfo (fileStreamPtr, 
																	&edscColumnRed,
																	&edscColumnGreen,
																	&edscColumnBlue,
																	&edscBinFunction);
		
		if (continueFlag)
			{
			if (edscColumnRed.dataType != 1 ||
						edscColumnGreen.dataType != 1 ||
								edscColumnBlue.dataType != 1)
				continueFlag = FALSE;
				
			if (edscColumnRed.numRows == 0 ||
						edscColumnGreen.numRows == 0 ||
								edscColumnBlue.numRows == 0)
				continueFlag = FALSE;
				
			if (edscBinFunction.binFunctionType != 0)
				continueFlag = FALSE;
			
			}	// end "if (continueFlag)"
					
		gSwapBytesFlag = FALSE;
				
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "CheckIfDefaultImaginePaletteExists"	

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DetermineDatumCode
//
//	Software purpose:	This routine determines the MultiSpec code for the input 
//							datum name.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			 in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2003
//	Revised By:			Larry L. Biehl			Date: 11/25/2009

SInt16 DetermineDatumCode (
				char*									inputStringPtr)

{	
	SInt16								datumCode;
	

	datumCode = kNoDatumDefinedCode;
	
	if (inputStringPtr[0] > 0)
		{
		inputStringPtr++;
		
		if (strstr (inputStringPtr, "NAD 27") != NULL)
			datumCode = kNAD27Code;
		
		else if (strstr (inputStringPtr, "NAD27") != NULL)
			datumCode = kNAD27Code;
		
		else if (strstr (inputStringPtr, "NAD 83") != NULL)
			datumCode = kNAD83Code;
		
		else if (strstr (inputStringPtr, "NAD83") != NULL)
			datumCode = kNAD83Code;
		
		else if (strstr (inputStringPtr, "WGS 84") != NULL)
			datumCode = kWGS84Code;
		
		else if (strstr (inputStringPtr, "WGS84") != NULL)
			datumCode = kWGS84Code;
			
				// Datum_Campo_Inchauspe =      4221 or 6221
		else if (strstr (inputStringPtr, "Campo Inchauspe") != NULL)
			datumCode = kCampoCode;
			
		else if (strstr (inputStringPtr, "Geodetic Datum 1949") != NULL)
			datumCode = kNewZealandGeodeticDatum1949Code;
			
		else if (strstr (inputStringPtr, "Ord. Survey G. Britain") != NULL)
					// Ord. Survey G. Britain 1936 = 4277 or 6277
			datumCode = kDatum_OSGB_1936Code;
			
		}	// end "if (inputStringPtr[0] > 0)"
		
	return (datumCode);
	
}	// end "DetermineDatumCode"

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DetermineEllipsoidCode
//
//	Software purpose:	This routine determines the MultiSpec code for the input 
//							ellipsoid name.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			 in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2003
//	Revised By:			Larry L. Biehl			Date: 04/09/2004

SInt16 DetermineEllipsoidCode (
				char*									inputStringPtr)

{	
	SInt16								ellipsoidCode;
	
	
	ellipsoidCode = kNoEllipsoidDefinedMenuItem;
	
	if (inputStringPtr[0] > 0)
		{
		inputStringPtr++;
		
		if (strstr (inputStringPtr, "Clarke 1866") != NULL)
			ellipsoidCode = kClarke1866EllipsoidMenuItem;
		
		else if (strstr (inputStringPtr, "GRS 1880") != NULL)
			ellipsoidCode = kClarke1880EllipsoidMenuItem;
		
		else if (strstr (inputStringPtr, "GRS 1980") != NULL)
			ellipsoidCode = kGRS80EllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "WGS 72") != NULL)
			ellipsoidCode = kWGS72EllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "WGS 84") != NULL)
			ellipsoidCode = kWGS84EllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "Airy") != NULL)
			ellipsoidCode = kAiryEllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "Australian") != NULL)
			ellipsoidCode = kAustralianEllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "Bessel") != NULL)
			ellipsoidCode = kBesselEllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "Everest") != NULL)
			ellipsoidCode = kEverestEllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "International 1909") != NULL)
			ellipsoidCode = kInternational1909EllipsoidMenuItem;
			
		else if (strstr (inputStringPtr, "Krasovsky") != NULL)
			ellipsoidCode = kKrassovskyEllipsoidMenuItem;
			
		}	// end "if (inputStringPtr[0] > 0)"
		
	return (ellipsoidCode-1);
	
}	// end "DetermineEllipsoidCode"

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 DetermineGridCoordinateCode
//
//	Software purpose:	This routine determines the MultiSpec code for the input 
//							ellipsoid name.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			 in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/28/2005
//	Revised By:			Larry L. Biehl			Date: 04/28/2005

SInt16 DetermineGridCoordinateCode (
				char*									inputStringPtr)

{	
	SInt16								gridCoordinateCode;
	
	
	gridCoordinateCode = kNoEllipsoidDefinedMenuItem;
	
	if (inputStringPtr[0] > 0)
		{
		inputStringPtr++;
		
		if (strstr (inputStringPtr, "New Zealand Map Grid") != NULL)
			gridCoordinateCode = kNewZealandMapGridCode;
			
		}	// end "if (inputStringPtr[0] > 0)"
		
	return (gridCoordinateCode);
	
}	// end "DetermineGridCoordinateCode"

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetDefaultImaginePaletteInfo
//
//	Software purpose:	This routine loads the color palette information from the
//							input imagine formatted thematic image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 10/22/1999

Boolean GetDefaultImaginePaletteInfo (
				CMFileStream*						fileStreamPtr, 
				Edsc_Column*						edscColumnRedPtr,
				Edsc_Column*						edscColumnGreenPtr,
				Edsc_Column*						edscColumnBluePtr,
				Edsc_BinFunction*					edscBinFunctionPtr)

{
	Ehfa_Entry							ehfaEntryEdscBinFunction,
											ehfaEntryEdscColumnBlue,
											ehfaEntryEdscColumnGreen,
											ehfaEntryEdscColumnRed,
											ehfaEntryLayer;
	
	Boolean								continueFlag = FALSE;
	
	
	
	if (fileStreamPtr != NULL)	
		{	
		continueFlag = GetFirstImagineLayerNode (fileStreamPtr,
																	&ehfaEntryLayer);
																	
		if (continueFlag)
			continueFlag = GetEdscColumnNode (fileStreamPtr,
															&ehfaEntryLayer,
															&ehfaEntryEdscColumnRed,
															&ehfaEntryEdscBinFunction,
															(char*)"Red");
																	
		if (continueFlag)
			continueFlag = GetEdscColumnNode (fileStreamPtr,
															&ehfaEntryLayer,
															&ehfaEntryEdscColumnGreen,
															&ehfaEntryEdscBinFunction,
															(char*)"Green");
																	
		if (continueFlag)
			continueFlag = GetEdscColumnNode (fileStreamPtr,
															&ehfaEntryLayer,
															&ehfaEntryEdscColumnBlue,
															&ehfaEntryEdscBinFunction,
															(char*)"Blue");
							
		if (continueFlag)
			continueFlag = ReadImagineEdscBinFunction (
															fileStreamPtr,
															ehfaEntryEdscBinFunction.data,
															ehfaEntryEdscBinFunction.dataSize,
															edscBinFunctionPtr);
							
		if (continueFlag)
			continueFlag = ReadImagineEdscColumn (fileStreamPtr,
															ehfaEntryEdscColumnRed.data,
															ehfaEntryEdscColumnRed.dataSize,
															edscColumnRedPtr);
							
		if (continueFlag)
			continueFlag = ReadImagineEdscColumn (fileStreamPtr,
															ehfaEntryEdscColumnGreen.data,
															ehfaEntryEdscColumnGreen.dataSize,
															edscColumnGreenPtr);
							
		if (continueFlag)
			continueFlag = ReadImagineEdscColumn (fileStreamPtr,
															ehfaEntryEdscColumnBlue.data,
															ehfaEntryEdscColumnBlue.dataSize,
															edscColumnBluePtr);
				
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "GetDefaultImaginePaletteInfo"


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetEdscColumnNode
//
//	Software purpose:	This routine gets the requested edsc_column for the input layer.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1999
//	Revised By:			Larry L. Biehl			Date: 10/22/1999

Boolean GetEdscColumnNode (
				CMFileStream* 						fileStreamPtr,
				Ehfa_Entry*							ehfaEntryLayerPtr,
				Ehfa_Entry*							ehfaEntryEdscColumnPtr,
				Ehfa_Entry*							ehfaEntryEdscBinFunctionPtr,
				CharPtr								edscColumnNodeNamePtr)

{
	Ehfa_Entry							ehfaEntryEdscTable;
	
	SInt16								errCode,
											stringCompare;
	
	Boolean								columnStartFlag,
											columnNotDoneFlag,
											continueFlag,
											edscColumnExistsFlag,
											tableStartFlag,
											tableNotDoneFlag;
										
		
			// Initialize local variables.													
	
	continueFlag = FALSE;
	errCode = noErr;
	edscColumnExistsFlag = FALSE;
		
	if (fileStreamPtr != NULL)
		{	
		tableStartFlag = TRUE;		
		tableNotDoneFlag = TRUE;
		while (tableNotDoneFlag)
			{				
					// Get a Edsc_Table node. 	
					
			continueFlag = GetNextNode (fileStreamPtr,
													ehfaEntryLayerPtr,
													&ehfaEntryEdscTable,
													(char*)"Edsc_Table",
													tableStartFlag);
			
			if (continueFlag)
				{					
				columnStartFlag = TRUE;		
				columnNotDoneFlag = TRUE;							
				while (columnNotDoneFlag)
					{					
							// Now get a Edsc_Column node under the Edsc_Table node. 	
				
					if (continueFlag)
						continueFlag = GetNextNode (fileStreamPtr,
																&ehfaEntryEdscTable,
																ehfaEntryEdscColumnPtr,
																(char*)"Edsc_Column",
																columnStartFlag);
														
						// Now check if the Edsc_Column node is the requested column.
					
					if (continueFlag)	
						{
								// An Edsc_Column node was found for this layer (or channel).
								// Now verify that it is the requested Edsc_Column.
							
						stringCompare = strcmp ((char*)ehfaEntryEdscColumnPtr->name, 
															edscColumnNodeNamePtr);
															
						if (stringCompare == 0)
							{
							edscColumnExistsFlag = TRUE;
							columnNotDoneFlag = FALSE;
							tableNotDoneFlag = FALSE;
							
							}	// end "if (stringCompare == 0)"
							
						}	// end "if (continueFlag)"
						
					else	// !continueFlag
						{
						columnNotDoneFlag = FALSE;
						
						if (ehfaEntryEdscColumnPtr->next == 0)
							continueFlag = TRUE;
						
						}	// end "else !continueFlag"
							
					columnStartFlag = FALSE;
														
					}	// end "while (columnNotDoneFlag)"
				
				}	// end "if (continueFlag)"
			
			else	// !continueFlag
				tableNotDoneFlag = FALSE;	
				
			tableStartFlag = FALSE;
				
			}	// end "while (tableNotDoneFlag)"
		
		if (edscColumnExistsFlag)
			{
					// Now get the bin function for this table.
					
			continueFlag = GetNextNode (fileStreamPtr,
													&ehfaEntryEdscTable,
													ehfaEntryEdscBinFunctionPtr,
													(char*)"Edsc_BinFunction",
													TRUE);
																	
			}	// end "if (edscColumnExistsFlag)"
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (edscColumnExistsFlag);
	
}	// end "GetEdscColumnNode" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetFirstImagineLayerNode
//
//	Software purpose:	This routine find the first layer node under the root node.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 10/19/1999

Boolean GetFirstImagineLayerNode (
				CMFileStream* 						fileStreamPtr,
				Ehfa_Entry*							ehfaEntryLayerPtr)

{
	Ehfa_Entry							ehfaEntryRoot;
	Ehfa_File							ehfaFile;
										
	UInt32								count,
											headerOffset,
											posOff;
	
	SInt16								errCode;
	
	Boolean								continueFlag;
										
		
			// Initialize local variables.													
			
	continueFlag = FALSE;
	errCode = noErr;
		
	if (fileStreamPtr != NULL)		
		{
				// Get Ehfa_HeaderTag headerPtr.	
		
		posOff = 16;
		errCode = MSetMarker (fileStreamPtr, fsFromStart, posOff, kErrorMessages);	
	
		count = 4;
		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, &count, &headerOffset, kNoErrorMessages); 								
				
		headerOffset = GetLongIntValue ((char*)&headerOffset);
		
				// Read the Ehfa_File				
		
		if (errCode == noErr)
			errCode = ReadImagineEhfaFile (fileStreamPtr,
														headerOffset,
														&ehfaFile);
			
				// Read the root node					
		
		if (errCode == noErr)
			errCode = ReadImagineEhfaEntry (fileStreamPtr,
														ehfaFile.rootEntryPtr,
														&ehfaEntryRoot);

				// Find the first node under the root node.	
		
		if (errCode == noErr)												
			continueFlag = GetNextNode (fileStreamPtr,
														&ehfaEntryRoot,
														ehfaEntryLayerPtr,
														(char*)"Eimg_Layer",
														TRUE);
																	
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "GetFirstImagineLayerNode" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HCharPtr GetImagineHistogramBuffer
//
//	Software purpose:	This routine gets the buffer to use to read the Imagine 
//							histogram vector.  Enough space is obtained to read all the 
//							data for one requested channel.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1999
//	Revised By:			Larry L. Biehl			Date: 10/19/1999

HCharPtr GetImagineHistogramBuffer (
				UInt32								numberBins)

{
	HCharPtr								statBufferPtr;
	
	UInt32								numberBytes;
	
		
			// Get buffer for the requested number of bins and assume each bin
			// will be a double. (It could be a signed long.)
			
	numberBytes = numberBins * sizeof (double);
		
	statBufferPtr = (HCharPtr)MNewPointer (numberBytes);
	
	return (statBufferPtr);
		
}	// end "GetImagineHistogramBuffer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetNextNode
//
//	Software purpose:	This routine gets the next node for the requested type under the
//							input parent node. If the startFlag is set then the routine
//							will get the first node of the requested type under the
//							input parent node.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 04/27/2005

Boolean GetNextNode (
				CMFileStream* 						fileStreamPtr,
				Ehfa_Entry*							ehfaEntryParentPtr,
				Ehfa_Entry*							ehfaEntryRequestedPtr,
				char*									nodeTypeStringPtr,
				Boolean								startFlag)

{
	SInt16								errCode,
											stringCompare;
	
	Boolean								reqestedNodeExistsFlag,
											notDoneFlag;
										
		
			// Initialize local variables.													
			
	errCode = noErr;
	reqestedNodeExistsFlag = FALSE;
		
	if (fileStreamPtr != NULL)
		{			
		notDoneFlag = TRUE;
								
				// Find the first node under the layer node.	
		
		if (startFlag)
			{
			if (ehfaEntryParentPtr == NULL)
				errCode = 1;						
			
			if (errCode == noErr)
				errCode = ReadImagineEhfaEntry (fileStreamPtr,
															ehfaEntryParentPtr->child,
															ehfaEntryRequestedPtr);
														
			if (errCode != noErr)
				notDoneFlag = FALSE;
														
			}	// end "if (startFlag)"
				
				// Find node with the requested type. 	
					
		while (notDoneFlag)
			{
			if (!startFlag)
				{
				if (ehfaEntryRequestedPtr->next != 0) 
					{
							// Get the next node at this same level.
															
					errCode = ReadImagineEhfaEntry (fileStreamPtr,
																ehfaEntryRequestedPtr->next,
																ehfaEntryRequestedPtr);
																
					if (errCode != noErr)	
						notDoneFlag = FALSE;	
						
					}	// end "if (ehfaEntryEdscTablePtr->next != 0)"
														
				else	// ehfaEntryRequestedPtr->next == 0	
					notDoneFlag = FALSE;	
					
				}	// end "if (!startFlag)"
					
			if (notDoneFlag)
				{
				startFlag = FALSE;
				
						// Determine if this is a Edsc_Table node.
						
				stringCompare = strcmp ((char*)ehfaEntryRequestedPtr->type,
													nodeTypeStringPtr);
														
				if (stringCompare == 0)
					{
					reqestedNodeExistsFlag = TRUE;
					notDoneFlag = FALSE;
					
					}	// end "if (stringCompare == 0)"
				
				}	// end "if (notDoneFlag)"
				
			}	// end "while (notDoneFlag)"	
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (reqestedNodeExistsFlag);
	
}	// end "GetNextNode" 
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetLayerForRequestedChannel
//
//	Software purpose:	This routine find the first layer node under the root node.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 10/18/1999

Boolean GetLayerForRequestedChannel (
				CMFileStream* 						fileStreamPtr,
				Ehfa_Entry*							ehfaEntryLayerPtr,
				UInt32*								currentLayerChannelPtr,
				UInt32								channel)

{
	Boolean								continueFlag,
											notDoneFlag;
										
		
			// Initialize local variables.													
			
	continueFlag = TRUE;
		
	if (fileStreamPtr != NULL)		
		{			
		notDoneFlag = TRUE;
		
		if (channel < *currentLayerChannelPtr)
			{
			continueFlag = GetFirstImagineLayerNode (fileStreamPtr, ehfaEntryLayerPtr);
			
			if (continueFlag)
				*currentLayerChannelPtr = 0;
				
			else	// !continueFlag
				notDoneFlag = FALSE;
			
			}	// end "if (channel < *currentLayerChannelPtr)"
			
		if (channel == *currentLayerChannelPtr)
			notDoneFlag = FALSE;
										
					// Now find 'Layer' node that matches the requested channel		
					
		while (notDoneFlag)
			{	
			continueFlag = GetNextNode (fileStreamPtr, 
													NULL,
													ehfaEntryLayerPtr,
													(char*)"Eimg_Layer",
													FALSE);
				
			if (continueFlag)
				{
				(*currentLayerChannelPtr)++;
				
				if (channel == *currentLayerChannelPtr)
					notDoneFlag = FALSE;
					
				}	// end "if (continueFlag)"
				
			else	// !continueFlag
				notDoneFlag = FALSE;	
					
			}	// end "while (notDoneFlag)"
		
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "GetLayerForRequestedChannel" 

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadImagineImageMapInfoForChannel
//
//	Software purpose:	This routine loads the image map information for the
//							request channel from the imagine formatted image file into memory.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			ReadImagineHeader in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/02/2001
//	Revised By:			Larry L. Biehl			Date: 03/23/2012

Boolean LoadImagineImageMapInfoForChannel (
				FileInfoPtr							fileInfoPtr, 
				UInt32								channel, 
				UInt32*								currentLayerChannelPtr,
				Boolean								mapInfoFlag)

{
	Ehfa_Entry							ehfaEntryEprjDatum,
											ehfaEntryEprjMapInfo,
											ehfaEntryEprjMapProjection,
											ehfaEntryEprjProParameters,
											ehfaEntryLayer;
											
	Eprj_Datum							eprjDatum;
	Eprj_MapInfo						eprjMapInfo;
	Eprj_ProParameters				eprjProParameters;
	Eprj_Spheroid						eprjSpheroid;
		
	CMFileStream*						fileStreamPtr;
	
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	PlanarCoordinateSystemInfoPtr	planarCoordinatePtr;
	
	Boolean								continueFlag = FALSE,
											continueFlag2 = FALSE,
											datumExistFlag = FALSE,
											proParametersExistFlag = FALSE;
	
	
			// Initialize local variables.
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	if (fileStreamPtr != NULL)	
		{	
		continueFlag = GetLayerForRequestedChannel (fileStreamPtr,
																	&ehfaEntryLayer,
																	currentLayerChannelPtr,
																	channel);

				// Get the histogram summary information if requested.				
				
		if (mapInfoFlag)
			{
			if (continueFlag)
				continueFlag = GetNextNode (fileStreamPtr, 
														&ehfaEntryLayer,
														&ehfaEntryEprjMapInfo,
														(char*)"Eprj_MapInfo",
														TRUE);
							
			if (continueFlag)
				{
				continueFlag = ReadImageEprjMapInfo (fileStreamPtr,
																	ehfaEntryEprjMapInfo.data,
																	ehfaEntryEprjMapInfo.dataSize,
																	&eprjMapInfo);
																	
				if (!continueFlag)
					{													
							// Display an alert indicating that there was an error reading
							// the Imagine map information.													
					
					DisplayAlert (kErrorAlertID, 
										kNoteAlert, 
										kAlertStrID, 
										IDS_Alert82,
										0, 
										NULL);
										
					}	// end "if (!continueFlag)"
																	
				}	// end "if (continueFlag)"
			
			if (continueFlag)
				{
				fileInfoPtr->mapProjectionHandle = GetMapProjectionHandle ();
				
				if (fileInfoPtr->mapProjectionHandle != NULL)
					{ 								
					mapProjectionInfoPtr = (MapProjectionInfoPtr)
								GetHandlePointer (fileInfoPtr->mapProjectionHandle);
															
					planarCoordinatePtr = &mapProjectionInfoPtr->planarCoordinate;
					
							// Determine the code for the projection type.
																	 
					if (CompareStringsNoCase (eprjMapInfo.projectionName, 
																		(UCharPtr)"utm", 3) == 0)
						{
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = kUTMRSCode;
						mapProjectionInfoPtr->gridCoordinate.projectionCode =
																					kTransverseMercatorCode;
						
						}	// end "if (CompareStringsNoCase (..., "utm", 3) == 0)"
						
					else if (CompareStringsNoCase (eprjMapInfo.projectionName, 
																		(UCharPtr)"state plane", 11) == 0)
						{
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
																					kStatePlaneNAD27RSCode;
						
						}	// end "else if (CompareStringsNoCase (..., "state plane..."
						
					else if (CompareStringsNoCase (eprjMapInfo.projectionName, 
																(UCharPtr)"Lambert Azimuthal", 16) == 0)
						mapProjectionInfoPtr->gridCoordinate.projectionCode = 
																		kLambertAzimuthalEqualAreaCode;
						
					else if (strstr ((char*)eprjMapInfo.projectionName,
												"Lambert_Azimuthal") != NULL)
						mapProjectionInfoPtr->gridCoordinate.projectionCode = 
																		kLambertAzimuthalEqualAreaCode;
						
							// Load the pixel size
	                                                                      
					planarCoordinatePtr->horizontalPixelSize = 
																	eprjMapInfo.horizontalPixelSize; 
					planarCoordinatePtr->verticalPixelSize = 
																	eprjMapInfo.verticalPixelSize; 
																
							// Load the location of the upper left pixel.
	                                                                               
					planarCoordinatePtr->xMapCoordinate11 = eprjMapInfo.xMapCoordinate11;
					planarCoordinatePtr->yMapCoordinate11 = eprjMapInfo.yMapCoordinate11;
                                                                      
					planarCoordinatePtr->xMapOrientationOrigin = eprjMapInfo.xMapCoordinate11;
					planarCoordinatePtr->yMapOrientationOrigin = eprjMapInfo.yMapCoordinate11;
																		
							// Determine the map units.
					
					planarCoordinatePtr->mapUnitsCode = kUnknownCode;													 
					if (CompareStringsNoCase (eprjMapInfo.mapUnits, (UCharPtr)"feet", 4) == 0)
						planarCoordinatePtr->mapUnitsCode = kFeetCode;
						
					else if (CompareStringsNoCase (eprjMapInfo.mapUnits, (UCharPtr)"meters", 5)  == 0)
						planarCoordinatePtr->mapUnitsCode = kMetersCode;
						
					else if (CompareStringsNoCase (eprjMapInfo.mapUnits, (UCharPtr)"degrees", 7)  == 0)
						planarCoordinatePtr->mapUnitsCode = kDecimalDegreesCode;
					
					}	// end "if (fileInfoPtr->mapProjectionHandle != NULL)"
				
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
				continueFlag = GetNextNode (fileStreamPtr, 
														&ehfaEntryLayer,
														&ehfaEntryEprjProParameters,
														(char*)"Eprj_ProParameters",
														TRUE);
														
				proParametersExistFlag = continueFlag;
							
				if (continueFlag)
					{
					continueFlag = ReadImageEprjProParameters (fileStreamPtr,
															ehfaEntryEprjProParameters.data,
															ehfaEntryEprjProParameters.dataSize,
															&eprjProParameters,
															&eprjSpheroid);
				
					if (continueFlag)
						{
						if (ehfaEntryEprjProParameters.child != 0)
							{
							continueFlag2 = GetNextNode (fileStreamPtr, 
																	&ehfaEntryEprjProParameters,
																	&ehfaEntryEprjDatum,
																	(char*)"Eprj_Datum",
																	TRUE);
														
							datumExistFlag = continueFlag2;

							if (continueFlag2)
								continueFlag2 = ReadImageEprjDatum (
																fileStreamPtr,
																ehfaEntryEprjDatum.data,
																ehfaEntryEprjDatum.dataSize,
																&eprjDatum);
																
							}	// end "if (ehfaEntryEprjProParameters.child != 0)"
																										
						}	// end "if (continueFlag)"
						
					}	// end "if (continueFlag)"
																	
				if (continueFlag)
					{
					if (eprjProParameters.proType == 0)
						{
						mapProjectionInfoPtr->gridCoordinate.referenceSystemCode = 
							GetMapProjectionCodeFromGCTPNumber (
											(SInt16)eprjProParameters.proNumber,
											TRUE,
											&mapProjectionInfoPtr->gridCoordinate.projectionCode);																	
						
						}	// end "if (eprjProParameters.proType == 0)"
						
					else if (eprjProParameters.proType == 1)
						{
								// Defined externally; determine from projection name.
								
						mapProjectionInfoPtr->gridCoordinate.projectionCode =
							DetermineGridCoordinateCode ((char*)&eprjProParameters.proName[0]);
						
						}	// end "else if (eprjProParameters.proType == 1)"
							
							// Get the UTM or State Plane Zone number if there.
							
					mapProjectionInfoPtr->gridCoordinate.gridZone = 
																(SInt16)eprjProParameters.proZone;
																
							// If the reference system is UTM, Get the UTM Zone and determine 
							// the hemisphere.
																
					if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode >=
																						kUTM_NAD27RSCode &&
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode <=
																						kUTMRSCode &&
																	eprjProParameters.proParams[3] < 0)
						{
						mapProjectionInfoPtr->gridCoordinate.gridZone = 
																-(SInt16)eprjProParameters.proZone;
						
						}	// end "if (eprjProParameters.referenceSystemCode == kUTM..."
						
							// Make sure the State Plane Zone number is positive.
							// Do not know why it is negative in some files at this time
							// 11/29/2006										
					if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																					kStatePlaneNAD27RSCode)
						mapProjectionInfoPtr->gridCoordinate.gridZone = 
											abs (mapProjectionInfoPtr->gridCoordinate.gridZone);
						
					mapProjectionInfoPtr->gridCoordinate.projectionCode = 
								GetProjectionCodeFromReferenceSystemCode (
											mapProjectionInfoPtr->gridCoordinate.referenceSystemCode,
											mapProjectionInfoPtr->gridCoordinate.gridZone);
					
							// Get the projection parameters. Just assume that they are 
							// there for now.
														
					SetProjectionParameters (mapProjectionInfoPtr,
														eprjProParameters.proParams,
														kNotPackedDegrees,
														kRadians);
					/*
					mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian =
																			eprjProParameters.proParams[2];
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = 
												eprjProParameters.proParams[4] * kRadiansToDegrees;
					mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = 
												eprjProParameters.proParams[5] * kRadiansToDegrees;
					mapProjectionInfoPtr->gridCoordinate.falseEasting = 
																			eprjProParameters.proParams[6];
					mapProjectionInfoPtr->gridCoordinate.falseNorthing = 
																			eprjProParameters.proParams[7];
					*/
							// Spheroid Code and specifications.
					
					mapProjectionInfoPtr->geodetic.spheroidCode =
								DetermineEllipsoidCode ((char*)&eprjSpheroid.sphereName[0]);
																		
					mapProjectionInfoPtr->geodetic.radiusSpheroid = eprjSpheroid.radius;
					mapProjectionInfoPtr->geodetic.semiMajorAxis = eprjSpheroid.a;
					mapProjectionInfoPtr->geodetic.semiMinorAxis = eprjSpheroid.b;
								
							// Datum Code	
					
					if (continueFlag2)	
						mapProjectionInfoPtr->geodetic.datumCode = 
													DetermineDatumCode ((char*)&eprjDatum.name[0]);
						
							// If the projection is State Plane, get the datum.
							
					if (!continueFlag2 &&
									mapProjectionInfoPtr->gridCoordinate.referenceSystemCode ==
																					kStatePlaneNAD27RSCode)
						{
						if (eprjProParameters.proParams[0] == 0)	
							mapProjectionInfoPtr->geodetic.datumCode = kNAD27Code;
						
						else if (eprjProParameters.proParams[0] == 1)
							{
							mapProjectionInfoPtr->geodetic.datumCode = kNAD83Code;
							mapProjectionInfoPtr->gridCoordinate.referenceSystemCode =
																					kStatePlaneNAD83RSCode;
							
							}	// end "else if (eprjProParameters.proParams[0] == 1)"
						
						}	// end "if (...->gridCoordinate.code == kStatePlaneCode)"
						
							// If datum has been set and there is no spheroid information,
							// the spheroid can be set.
							
					if (mapProjectionInfoPtr->geodetic.spheroidCode == 0 && 
											mapProjectionInfoPtr->geodetic.datumCode == kNAD27Code)
						mapProjectionInfoPtr->geodetic.spheroidCode = 
																			kClarke1866EllipsoidCode;
							
					else if (mapProjectionInfoPtr->geodetic.spheroidCode == 0 && 
											mapProjectionInfoPtr->geodetic.datumCode == kNAD83Code)
						mapProjectionInfoPtr->geodetic.spheroidCode = kGRS80EllipsoidCode;
					
					}	// end "if (continueFlag)"
																	
				}	// end "if (continueFlag)"
				
			if (!proParametersExistFlag || !datumExistFlag)
				{
						// See if a text description of the map parameters exists.
						
				continueFlag = GetNextNode (fileStreamPtr, 
														&ehfaEntryLayer,
														&ehfaEntryEprjMapProjection,
														(char*)"Eprj_MapProjection842",
														TRUE);
		
				if (continueFlag)
					{
					continueFlag = ReadImageEprjMapProjection (
															fileStreamPtr,
															ehfaEntryEprjMapProjection.data,
															ehfaEntryEprjMapProjection.dataSize,
															&eprjProParameters,
															&eprjSpheroid);
															
					if (continueFlag)	
						{
						if (!proParametersExistFlag)
							{
									// Get the projection parameters. Just assume that they are 
									// there for now.
							
							mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian = 
																		eprjProParameters.proParams[2];
							mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = 
																		eprjProParameters.proParams[4];
							mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = 
																		eprjProParameters.proParams[5];
							mapProjectionInfoPtr->gridCoordinate.falseEasting = 
																		eprjProParameters.proParams[6];
							mapProjectionInfoPtr->gridCoordinate.falseNorthing = 
																		eprjProParameters.proParams[7];
								
							}	// end "if (!proParametersExistFlag)"
							
						if (!datumExistFlag)
							{
							if (mapProjectionInfoPtr->geodetic.spheroidCode == 0)
								mapProjectionInfoPtr->geodetic.spheroidCode =
																					eprjSpheroid.sphereCode;
																				
							mapProjectionInfoPtr->geodetic.radiusSpheroid = eprjSpheroid.radius;
							mapProjectionInfoPtr->geodetic.semiMajorAxis = eprjSpheroid.a;
							mapProjectionInfoPtr->geodetic.semiMinorAxis = eprjSpheroid.b;
								
							}	// end "if (!datumExistFlag)"
							
						}	// end "if (continueFlag)"
						
					}	// end "if (continueFlag)"
														
				}	// end "if (if (!proParametersExistFlag || !datumExistFlag))"
				
			}	// end "if (mapInfoFlag)" 
				
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "LoadImagineImageMapInfoForChannel"


	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadImagineImageStatistics
//
//	Software purpose:	This routine loads the image statistics information from the 
//							imagine formatted image file into memory. For now this routine
//							will only read the image statistics and not the histogram
//							itself.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadSupportFile in SHistogrm.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 07/08/2009

Boolean LoadImagineImageStatistics (
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr,
				HUInt32Ptr							histogramArrayPtr, 
				SInt16								channelStartIndex, 
				Boolean								summaryFlag)

{
	HistogramSummaryPtr				histogramSummaryPtr;
	HUInt32Ptr							savedHistogramArrayPtr;
	
	SInt16								*channelListPtr;
	
	UInt32								channel,
											channelIndex,
											currentLayerChannel,
											numberChannelLoops,
											numberChannels;
	
	Boolean								continueFlag = TRUE;
											

			// Set up swap bytes flag depending upon the system
			// architecture
			                 
	gSwapBytesFlag = gBigEndianFlag;
	
			// Initialize some local variables.
			
	savedHistogramArrayPtr = histogramArrayPtr;
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	
			// The channels handle is already locked.
																								
	channelListPtr = (SInt16*)GetHandlePointer (histogramSpecsPtr->channelsHandle);
	
	if (channelStartIndex == 0)
		{
				// Get buffer to load histogram vector into. Get space for one channel.													
		
		if (histogramSpecsPtr->statBufferPtr == NULL)
			{
			histogramSpecsPtr->numberStatBufferChannels = 1;
			histogramSpecsPtr->statBufferPtr = GetImagineHistogramBuffer (
																		fileInfoPtr->numberBins);
							
			}	// end "if (...statBufferPtr == NULL)" 
						
		}	// end "if (channelStartIndex == 0)"

			// Force 'LoadImagineImageStatisticsForChannel' routine to start at the
			// first channel.
			
	currentLayerChannel = LONG_MAX;

			// Continue if everything is okay.	
	
	if (continueFlag)	
		{
				// Make cursor the watch cursor to indicate to the user that work	
				// is in process.																	

		MSetCursor (kWait);
		
				// Put appropriate message in the status box								
				
		LoadDItemStringNumber (kHistogramStrID, 
										IDS_HistogramStatus4, 
										gStatusDialogPtr, 
										IDC_Status2, 
										(Str255*)gTextString);
		
		if (histogramSpecsPtr->allChannelsAtOnceFlag)
			{
			numberChannels = 1;
			numberChannelLoops = histogramSpecsPtr->numberChannels;
			
			}	// end "...histogramSpecs.allChannelsAtOnceFlag" 
			
		else	// !...->histogramSpecs.allChannelsAtOnceFlag 
			{	
			numberChannels = 1;
			numberChannelLoops = channelStartIndex + 1;
			
			}	// end "else !...->histogramSpecs.allChannelsAtOnceFlag" 
			
		for (channelIndex=channelStartIndex; 
				channelIndex<numberChannelLoops; 
				channelIndex++)
			{
			channel = channelListPtr[channelIndex];
			
			continueFlag = LoadImagineImageStatisticsForChannel (
																	fileInfoPtr,
																	histogramSummaryPtr, 
																	histogramArrayPtr, 
																	histogramSpecsPtr->statBufferPtr,
																	channel,
																	&currentLayerChannel,
																	fileInfoPtr->numberBins, 
																	summaryFlag);
			
					// Get the overall min and max values and estimate the number of
					// decimal places required.  This information is used for listing
					// data values.
		
			fileInfoPtr->maxNumberEDecimalDigits = 
									MAX (fileInfoPtr->maxNumberEDecimalDigits,
											histogramSummaryPtr[channelIndex].numberEDecimalDigits);
		
			fileInfoPtr->maxNumberFDecimalDigits = 
									MAX (fileInfoPtr->maxNumberFDecimalDigits,
											histogramSummaryPtr[channelIndex].numberFDecimalDigits);
			 								
			fileInfoPtr->maxDataValue = MAX (
												fileInfoPtr->maxDataValue, 
												histogramSummaryPtr[channelIndex].maxValue);
			fileInfoPtr->minDataValue = MIN (
												fileInfoPtr->minDataValue, 
												histogramSummaryPtr[channelIndex].minValue);
																	
			if (histogramArrayPtr != NULL)
				histogramArrayPtr += histogramSummaryPtr[channelIndex].numberBins;
							
			if (!continueFlag)
				break;
				
			}	// end "for (channel=0; ..." 
																	
		if (!continueFlag)
			{													
					// Display an alert indicating that there was an error reading
					// the Imagine image statistics information.													
			
			DisplayAlert (kErrorAlertID, 
								kNoteAlert, 
								kAlertStrID, 
								IDS_Alert83,
								0, 
								NULL);
								
			}	// end "if (!continueFlag)"
			
		if (channelStartIndex == 0)
			{
					// Find the total number of pixels. We will just count 			
					// those for the first channel.											 
	
			if (histogramSpecsPtr->totalPixels <= 0)
				histogramSpecsPtr->totalPixels =
						CountTotalNumberHistogramPixels (savedHistogramArrayPtr,
																	histogramSummaryPtr[0].numberBins);
		
					// Set number bad lines to -1 since that information is 			
					// not stored in the image statistics file.							
			
			histogramSpecsPtr->numBadLines = -1;
			
			}	// end "if (channelStartIndex == 0)" 
			
		}	// end "if (continueFlag)											
				
	gSwapBytesFlag = FALSE;
		
	return (continueFlag);
	
}	// end "LoadImagineImageStatistics"

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadImagineImageStatisticsForChannel
//
//	Software purpose:	This routine loads the image statistics information for the
//							request channel from the imagine formatted image file into memory.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadImagineImageStatistics in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1999
//	Revised By:			Larry L. Biehl			Date: 07/08/2009

Boolean LoadImagineImageStatisticsForChannel (
				FileInfoPtr							fileInfoPtr, 
				HistogramSummaryPtr				histogramSummaryPtr, 
				HUInt32Ptr							histogramArrayPtr, 
				HCharPtr								statBufferPtr,
				UInt32								channel, 
				UInt32*								currentLayerChannelPtr,
				UInt32								expectedNumberBins,
				Boolean								summaryFlag)

{
	CMFileStream*						supportFileStreamPtr;
											
	Esta_Statistics					estaStatistics;
	
	Edsc_BinFunction					edscBinFunction;
	
	Edsc_Column							edscColumnHistogram;
	
	Ehfa_Entry							ehfaEntryEdscBinFunction,
											ehfaEntryEdscColumnHistogram,
											ehfaEntryEstaStatistics,
											ehfaEntryLayer;
	
	Boolean								continueFlag = FALSE;
	
	
			// Initialize local variables.
	
	supportFileStreamPtr = OpenSupportFile ();
	
	if (supportFileStreamPtr != NULL)	
		{	
		continueFlag = GetLayerForRequestedChannel (supportFileStreamPtr,
																	&ehfaEntryLayer,
																	currentLayerChannelPtr,
																	channel);

				// Get the histogram summary information if requested.				
				
		if (summaryFlag)
			{
			if (continueFlag)
				continueFlag = GetNextNode (supportFileStreamPtr, 
														&ehfaEntryLayer,
														&ehfaEntryEstaStatistics,
														(char*)"Esta_Statistics",
														TRUE);
														
			if (continueFlag)
				{
				if (ehfaEntryEstaStatistics.dataSize > sizeof (Esta_Statistics))
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)"
								
			if (continueFlag)
				continueFlag = ReadImageEstaStatistics (supportFileStreamPtr,
																		ehfaEntryEstaStatistics.data,
																		ehfaEntryEstaStatistics.dataSize,
																		&estaStatistics);
			
			if (continueFlag)
				{
						// Get channel mean.															
		
				histogramSummaryPtr[channel].averageValue = estaStatistics.mean;
		
						// Get channel medium.														
		
				histogramSummaryPtr[channel].medianValue = (SInt32)estaStatistics.median;
		
						// Get channel standard deviation.										
		
				histogramSummaryPtr[channel].stdDeviation = estaStatistics.stddev;
		
						// Get maximum value for band.					
				
				histogramSummaryPtr[channel].maxValue = estaStatistics.maximum;
		
						// Get minimum value for band.	
										
				histogramSummaryPtr[channel].minValue = estaStatistics.minimum;
				
						// Initialize values for minimum and maximum non-saturated		
						// values. As I understand it, this table does not keep track of 
						// them.																	
						
				histogramSummaryPtr[channel].maxNonSatValue = 
																	histogramSummaryPtr[channel].maxValue;
				histogramSummaryPtr[channel].minNonSatValue = 
																	histogramSummaryPtr[channel].minValue;
														
					// Get the number of decimal digits to use when listing data
					// values.

				GetNumberDecimalDigits (fileInfoPtr->dataTypeCode,
												histogramSummaryPtr[channel].minValue,
												histogramSummaryPtr[channel].maxValue,
												histogramSummaryPtr[channel].minValue,
												&histogramSummaryPtr[channel].numberEDecimalDigits,
												&histogramSummaryPtr[channel].numberFDecimalDigits);
												
				if (fileInfoPtr->numberBytes > 2)
							// Indicate that the bin width will definitely not be one since the
							// the number of data bytes is more than 2.
					histogramSummaryPtr->binType = kBinWidthNotOne;
					
				histogramSummaryPtr[channel].numberBins = fileInfoPtr->numberBins;
				
						// This may get modified after the histogram info has been read.
						
				histogramSummaryPtr[channel].binFactor = 1;
												
				histogramSummaryPtr[channel].availableFlag = TRUE;
				
				}	// end "if (continueFlag)"
				
			}	// end "if (summaryFlag)" 
			
				// Get information about the way the histogram is stored when the number of 
				// data bytes is more than 2.
				
		if (histogramArrayPtr != NULL || fileInfoPtr->numberBytes > 2)
			{
			if (continueFlag)
				continueFlag = GetEdscColumnNode (supportFileStreamPtr,
																&ehfaEntryLayer,
																&ehfaEntryEdscColumnHistogram,
																&ehfaEntryEdscBinFunction,
																(char*)"Histogram");
														
			if (continueFlag)
				{
				if (ehfaEntryEdscBinFunction.dataSize > sizeof (edscBinFunction))
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)"
							
			if (continueFlag)
				continueFlag = ReadImagineEdscBinFunction (supportFileStreamPtr,
																		ehfaEntryEdscBinFunction.data,
																		ehfaEntryEdscBinFunction.dataSize,
																		&edscBinFunction);
																	
			if (continueFlag && 
						edscBinFunction.binFunctionType == 1 && fileInfoPtr->numberBytes > 2)
				histogramSummaryPtr[channel].binFactor = (double)edscBinFunction.numBins / 
												(edscBinFunction.maxLimit - edscBinFunction.minLimit);
																	
			}	// end "if (histogramArrayPtr != NULL || fileInfoPtr->numberBytes > 2)"
		
			
				// Read histogram stastistics if needed.									
				// Set up channel pointer for median array.								
		
		if (histogramArrayPtr != NULL)
			{									
			if (continueFlag)
				{
				if (ehfaEntryEdscColumnHistogram.dataSize > sizeof (edscColumnHistogram))
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)"
							
			if (continueFlag)
				continueFlag = ReadImagineEdscColumn (supportFileStreamPtr,
																	ehfaEntryEdscColumnHistogram.data,
																	ehfaEntryEdscColumnHistogram.dataSize,
																	&edscColumnHistogram);
																	
			if (continueFlag)
				{
				if (expectedNumberBins < (UInt32)edscColumnHistogram.numRows)
					continueFlag = FALSE;
							
				if (continueFlag)
					{
					//if (fileInfoPtr->numberBytes > 2)
					//	histogramSummaryPtr[channel].numberBins = edscBinFunction.numBins;
					
					continueFlag = ReadImagineHistogramColumn (
																	fileInfoPtr,
																	supportFileStreamPtr,
																	&histogramSummaryPtr[channel],
																	edscColumnHistogram.columnDataPtr,
																	edscColumnHistogram.numRows,
																	edscColumnHistogram.dataType,
																	edscBinFunction.numBins,
																	edscBinFunction.binFunctionType,
																	edscBinFunction.minLimit,
																	edscBinFunction.maxLimit,
																	histogramArrayPtr,
																	statBufferPtr);
																	
					}	// end "if (continueFlag)"
																	
				}	// end "if (continueFlag)"
				
			}	// end "if (histogramArrayPtr != NULL)"
				
		}	// end "if (supportFileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "LoadImagineImageStatisticsForChannel"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineClassNameColumn
//
//	Software purpose:	This routine reads the column of class names from the 
//							input offset location within the file and copies the data to
//							the class name vector.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadImagineClassNameColumn (
				CMFileStream*						fileStreamPtr,
				Edsc_Column*						edscColumnClassNamePtr,
				Edsc_BinFunction*					edscBinFunctionPtr, 
				UCharPtr								classNamePtr)

{
	UInt16								*classSymbolPtr;
										
	SInt32								numberCharacters;
	
	UInt32								classNumber,
											count,
											numberClasses;	
	
	SInt16								errCode,
											firstNonBlankCharacter;
	
	Boolean								checkIOErrorFlag,
											continueFlag = FALSE;
											
				
				// Make sure that the data type can be handled.
			
	if (edscColumnClassNamePtr->dataType == 3)
		{
				// Initialize some local variables.
				
		checkIOErrorFlag = TRUE;
		
				// If everything is okay, position file to start reading in			
				// the class names.	
				
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										edscColumnClassNamePtr->columnDataPtr,
										kErrorMessages);
			
				// Read the class names.  If the class name is blank or we reach	
				// the end of the file before all the names have been read, fill	
				// in a default name.															
		
		if (errCode == noErr)
			{
			numberClasses = edscColumnClassNamePtr->numRows;
	
					// Get a pointer to storage for the class symbols.					
					
			classSymbolPtr = 
				(unsigned short int*)&classNamePtr[numberClasses*sizeof (Str31)];
			
			for (classNumber=1; classNumber<=numberClasses; classNumber++)
				{	
				count = MIN (edscColumnClassNamePtr->maxNumChars, 255);
				
				if (errCode == noErr)
					errCode = MReadData (fileStreamPtr, 
												&count, 
												gTextString,
												kNoErrorMessages);
				if (errCode == eofErr)
					checkIOErrorFlag = FALSE;
					
						// Determine the number of characters in the class name.		
				
				numberCharacters = 0;
				firstNonBlankCharacter = 0;
				
				if (errCode == noErr)
					{
							// Find the number of characters in the class name.
							
					numberCharacters = (SInt32)strlen ((char*)gTextString);
					
							// Force the length to be no more than 31 characters.
							
					numberCharacters = MIN (numberCharacters, 31);
					
							// Remove any leading blanks in the class name.				
							
					while (numberCharacters != 0 && 
													gTextString[firstNonBlankCharacter] == ' ')
						{
						firstNonBlankCharacter++;
						numberCharacters--;
						
						}	// end "while (numberCharacters != 0 && ..." 
						
					}	// end "if (errCode == noErr)" 
					
				if (numberCharacters == 0)
					{
					if (classNumber == 1)
						{
								// Class 0 is assumed to be the background class			
								
						CopyPToP (classNamePtr, (UCharPtr)"\0background\0");
						
						}	// end "if (classNumber == 0)" 
						
					else	// classNumber != 1 
						CreateDefaultClassName (classNamePtr, classNumber);
					
					}	// end "if (numberCharacters == 1)" 
				
				else	// numberCharacters != 0 
					{
					BlockMoveData (&gTextString[firstNonBlankCharacter], 
											&classNamePtr[1],
											numberCharacters);
					classNamePtr[0] = (UInt8)numberCharacters;
					
					}	// end "else numberCharacters != 0" 
					
				classNamePtr += 32;
				
						// Load the assumed class symbol if this is being read
						// from a default trl file when the image is first being
						// loaded									
					
				classSymbolPtr[classNumber-1] = (UInt16)(classNumber-1);
				
				}	// end "for (classNumber=1; classNumber<=..." 
			
					// Indicate that the class names have been created.				
					
			continueFlag = TRUE;
			
			}	// end "if (errCode == noErr)" 
		
		if (checkIOErrorFlag)
			IOCheck (errCode, fileStreamPtr);
		
		}	// end "if (edscColumnClassNamePtr->dataType == 3)" 
		
	return (continueFlag);
	
}	// end "ReadImagineClassNameColumn" 

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineClassNames
//
//	Software purpose:	This routine loads the image statistics information for the
//							request channel from the imagine formatted image file into memory.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadImagineImageStatistics in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadImagineClassNames (
				FileInfoPtr							fileInfoPtr, 
				UCharPtr								classNamePtr)

{
	Edsc_BinFunction					edscBinFunction;
	
	Edsc_Column							edscColumnClassName;
	
	Ehfa_Entry							ehfaEntryEdscBinFunction,
											ehfaEntryEdscColumnClassName,
											ehfaEntryLayer;
	
	CMFileStream*						fileStreamPtr;
	
	Boolean								continueFlag = FALSE;
	
	
			// Initialize local variables.
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	if (fileStreamPtr != NULL)	
		{	
				// Set up swap bytes flag depending upon the system
				// architecture
				                 
		gSwapBytesFlag = gBigEndianFlag;
	
		continueFlag = GetFirstImagineLayerNode (fileStreamPtr,
																	&ehfaEntryLayer);
																	
		if (continueFlag)
			continueFlag = GetEdscColumnNode (fileStreamPtr,
															&ehfaEntryLayer,
															&ehfaEntryEdscColumnClassName,
															&ehfaEntryEdscBinFunction,
															(char*)"Class_Names");
							
		if (continueFlag)
			continueFlag = ReadImagineEdscBinFunction (fileStreamPtr,
																	ehfaEntryEdscBinFunction.data,
																	ehfaEntryEdscBinFunction.dataSize,
																	&edscBinFunction);
							
		if (continueFlag)
			continueFlag = ReadImagineEdscColumn (fileStreamPtr,
																ehfaEntryEdscColumnClassName.data,
																ehfaEntryEdscColumnClassName.dataSize,
																&edscColumnClassName);
								
		if (continueFlag)		
			continueFlag = ReadImagineClassNameColumn (fileStreamPtr,
																		&edscColumnClassName,
																		&edscBinFunction, 
																		classNamePtr);			
			
		gSwapBytesFlag = FALSE;
				
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "ReadImagineClassNames"

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineClassPalette
//
//	Software purpose:	This routine loads the color palette information from the
//							input imagine formatted thematic image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 10/22/1999

Boolean ReadImagineClassPalette (
				FileInfoPtr							fileInfoPtr, 
				ColorSpec*							colorSpecPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt16*								paletteCodePtr)

{
	Edsc_BinFunction					edscBinFunction;
	
	Edsc_Column							edscColumnBlue,
											edscColumnGreen,
											edscColumnRed;
	
	CMFileStream*						fileStreamPtr;
	
	Boolean								continueFlag = FALSE;
	
	
			// Initialize local variables.
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	if (fileStreamPtr != NULL)	
		{	
				// Set up swap bytes flag depending upon the system
				// architecture
				                 
		gSwapBytesFlag = gBigEndianFlag;

		continueFlag = GetDefaultImaginePaletteInfo (fileStreamPtr, 
																	&edscColumnRed,
																	&edscColumnGreen,
																	&edscColumnBlue,
																	&edscBinFunction);
								
		if (continueFlag)		
			continueFlag = ReadImagineClassPaletteColumns (fileInfoPtr,
																			&edscColumnRed,
																			&edscColumnGreen,
																			&edscColumnBlue,
																			&edscBinFunction,
																			colorSpecPtr, 
																			displaySpecsPtr, 
																			classSymbolPtr, 
																			paletteCodePtr);			
			
		gSwapBytesFlag = FALSE;
				
		}	// end "if (fileStreamPtr != NULL)"
		
	return (continueFlag);
	
}	// end "ReadImagineClassPalette"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineClassPaletteColumns
//
//	Software purpose:	This routine reads the column of class names from the 
//							input offset location within the file and copies the data to
//							the class name vector.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 10/22/1999

Boolean ReadImagineClassPaletteColumns (
				FileInfoPtr							fileInfoPtr, 
				Edsc_Column*						edscColumnRedPtr,
				Edsc_Column*						edscColumnGreenPtr,
				Edsc_Column*						edscColumnBluePtr,
				Edsc_BinFunction*					edscBinFunctionPtr, 
				ColorSpec*							colorSpecPtr, 
				DisplaySpecsPtr					displaySpecsPtr, 
				UInt16*								classSymbolPtr, 
				UInt16*								paletteCodePtr)

{
	RGBColor								colorValue;
	
	CMFileStream*						fileStreamPtr;
	ColorSpec*							savedColorSpecPtr; 
	
	double								*redVectorPtr,
											*greenVectorPtr,
											*blueVectorPtr;
											
	SInt16								*classToGroupPtr,
											*groupToPalettePtr;
	
	UInt32								count,
											entry,
											index,
											inputIndex,
											numberLevels,
											numRows,
											startIndex;	
	
	SInt16								errCode;
	
	Boolean								classCodeFlag;
	
	
			// Check input parameters.
			
	if (fileInfoPtr == NULL || 
			colorSpecPtr == NULL ||
				displaySpecsPtr == NULL)
																						return (FALSE);
	
	if (edscColumnRedPtr->dataType != 1 ||
			edscColumnGreenPtr->dataType != 1 ||
				edscColumnBluePtr->dataType != 1)
																						return (FALSE);
				
	if (edscBinFunctionPtr->binFunctionType != 0)
																						return (FALSE);
	
			// Check that there is a valid class to group handle if needed.		
			
	if (displaySpecsPtr->classGroupCode != kClassDisplay && 
									fileInfoPtr->groupTablesHandle == NULL)
																						return (FALSE);
	
			// Get memory to load color table into.
			
	numRows = MIN (edscColumnRedPtr->numRows, edscColumnGreenPtr->numRows);
	numRows = MIN (numRows, (UInt32)edscColumnBluePtr->numRows);
									
	if (numRows == 0)
																						return (FALSE);
			
	redVectorPtr = (double*)MNewPointer (3 * numRows * sizeof (double));
	if (redVectorPtr == NULL)
																						return (FALSE);
																						
	greenVectorPtr = &redVectorPtr[numRows];	
	blueVectorPtr = &redVectorPtr[2*numRows];
	
			// Initialize local variables.
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
			// If everything is okay, position file to start reading in			
			// the red vector for the palette.	
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									edscColumnRedPtr->columnDataPtr,
									kErrorMessages);
		
	count = numRows * sizeof (double);
	if (errCode == noErr)
		errCode = MReadData (fileStreamPtr, 
										&count, 
										redVectorPtr,
										kNoErrorMessages);
		
			// If everything is okay, position file to start reading in			
			// the green vector for the palette.	
					
	if (errCode == noErr)
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart,
										edscColumnGreenPtr->columnDataPtr,
										kErrorMessages);
		
	count = numRows * sizeof (double);
	if (errCode == noErr)
		errCode = MReadData (fileStreamPtr, 
										&count, 
										greenVectorPtr,
										kNoErrorMessages);
		
			// If everything is okay, position file to start reading in			
			// the blue vector for the palette.	
					
	if (errCode == noErr)
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart,
										edscColumnBluePtr->columnDataPtr,
										kErrorMessages);
		
	count = numRows * sizeof (double);
	if (errCode == noErr)
		errCode = MReadData (fileStreamPtr, 
										&count, 
										blueVectorPtr,
										kNoErrorMessages);
		
			// Continue if okay.																	

	if (errCode == noErr)
		{
			// Loop for the number of classes.  Convert double value to		
			// a two byte value.	
			
		entry = 1;
	
				// Get the number of classes.
				
		numberLevels = (UInt32)MIN (256, displaySpecsPtr->numberPaletteLevels);

		classCodeFlag = (displaySpecsPtr->classGroupCode == kClassDisplay);
	
				// Get the class to group pointer if needed.							
		
		if (!classCodeFlag)
			{		
			classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);
			groupToPalettePtr = GetGroupToPalettePointer (fileInfoPtr);
			
			}	// end "if (!classCodeFlag)" 
  				
				// If there are more than 255 classes, then check if the 		
				// first color in the palette is white.  If so skip it, 			
				// since white has already been loaded in.							
		
		startIndex = 0;
		if (displaySpecsPtr->numberPaletteLevels >= 255)
			{
			colorValue.red = 
							(UInt16)(65535*GetDoubleValue ((UCharPtr)&redVectorPtr[0]));
			colorValue.green = 
							(UInt16)(65535*GetDoubleValue ((UCharPtr)&greenVectorPtr[0]));
			colorValue.blue = 
							(UInt16)(65535*GetDoubleValue ((UCharPtr)&blueVectorPtr[0]));
			
			if (colorValue.red >= 65280 && 
							colorValue.green >= 65280 && 
										colorValue.blue >= 65280)
				{
				startIndex = 1;
				numberLevels++;
				*paletteCodePtr |= 0x0001;
				
				}	// end "if (red2ByteStringPtr[0] >= 65280 && ..." 
				
			}	// end "if (...->numberPaletteLevels >= 255)"
			
		savedColorSpecPtr = colorSpecPtr;
  				
		for (index=startIndex; index<numberLevels; index++)
			{
			if (classSymbolPtr != NULL)
				{
				if (classCodeFlag)
					inputIndex = classSymbolPtr[index];
					
				else	// !classCodeFlag
					{ 
					inputIndex = classToGroupPtr[index];
					inputIndex = groupToPalettePtr[inputIndex];
					inputIndex = classSymbolPtr[inputIndex];
					
					}	// end "else  !classCodeFlag"
					
				}	// end "if (classSymbolPtr != NULL)"
				
			else	// classSymbolPtr == NULL
				inputIndex = index;
				
			inputIndex = MIN (inputIndex, numRows-1);

			colorSpecPtr->value     = (SInt16)entry;
			
			colorSpecPtr->rgb.red   = 
						(UInt16)(65535*GetDoubleValue ((UCharPtr)&redVectorPtr[inputIndex]));
			colorSpecPtr->rgb.green = 
						(UInt16)(65535*GetDoubleValue ((UCharPtr)&greenVectorPtr[inputIndex]));
			colorSpecPtr->rgb.blue  = 
						(UInt16)(65535*GetDoubleValue ((UCharPtr)&blueVectorPtr[inputIndex]));
			
			colorSpecPtr++;
			entry++;
			
			}	// end "for (index=0; index<=numberLevels; index++)" 
  				
				// If there are more than 255 classes, then check if the 		
				// last color in the palette is black.  If so indicate			
				// that the last black color will be used to display				
				// the class.																	
		
  		if (numberLevels < numRows &&
  						displaySpecsPtr->numberPaletteLevels > 
  											displaySpecsPtr->numPaletteEntriesUsed &&
  												displaySpecsPtr->numberPaletteLevels == 256)
  			{
  			if (savedColorSpecPtr[numberLevels].rgb.red == 0 && 
  								savedColorSpecPtr[numberLevels].rgb.green == 0 && 
  											savedColorSpecPtr[numberLevels].rgb.blue == 0)
  					*paletteCodePtr |= 0x0002;
  					
  			}	// end "if (...->numberPaletteLevels > ...)" 
		
				// Unlock the class to group handle in case it is locked.		
			
		UnlockGroupTablesHandle (fileInfoPtr);
	
		}	// end "if (errCode == noErr)" 
		
	CheckAndDisposePtr (redVectorPtr);
	
	return (errCode == noErr);
	
}	// end "ReadImagineClassPaletteColumns" 
							


//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineHeader
//
//	Software purpose:	This routine reads the file specifications from
//							a TIFF formatted file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
// 
//	Coded By:			Larry L. Biehl			Date: 10/11/1999
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 ReadImagineHeader (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr,
				SInt16								formatOnlyCode)

{
	FileStringName255					extensionFileName;
	
	SInt64								layerStackValidFlagsOffset;
											
	CharPtr								externRasterMagicHeader = (char*)"ERDAS_IMG_EXTERNAL_RASTER";
	CharPtr								imagineHeader = (char*)"EHFA_HEADER_TAG";
	
	Ehfa_Entry							ehfaEntry;
	
	CMFileStream*						fileStreamPtr;
	FileStringPtr						filePathPtr;
										
	UInt32								channel,
											currentLayerChannel = ULONG_MAX;
	
	SInt16								errCode,
											fileType,
											formatErrorCode,
											returnCode;
	
	Boolean								continueFlag,
											externalFileFlag,
											notDoneFlag;
										
		
			// Initialize local variables.													
	
	fileType = 0;
	externalFileFlag = FALSE;		
	returnCode = 1;
	errCode = noErr;
	extensionFileName[0] = 0;
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
	if (fileStreamPtr != NULL && headerRecordPtr != NULL)		
		{
		if (strncmp (headerRecordPtr, (char*)imagineHeader, 15) == 0)
			fileType = kImagineType;
			
		else if (strncmp (headerRecordPtr, (char*)externRasterMagicHeader, 25) == 0)
			fileType = kImagineType;
						
		}	// end "if (fileStreamPtr != NULL && headerRecordPtr != NULL)"
		
	if (fileType != 0)
		{		
		if (formatOnlyCode != kLoadHeader)
			{
			fileInfoPtr->format = fileType;
			
					// For now we will not check whether the file is thematic
					// or multispectral
					
			if (formatOnlyCode == kMultiFiles)
				fileInfoPtr->thematicType = FALSE;
			
			else if (formatOnlyCode == kThematicFiles)
				fileInfoPtr->thematicType = TRUE;
				
																							return (noErr);
																								
			}	// end "if (formatOnlyCode != kLoadHeader)"
						
		}	// end "if (fileType != 0)"
		
	if (fileType != 0 && errCode == noErr)
		{	
		returnCode = noErr;
		
				// If this is an Imagine .rrd or .ige file, find the associated .img file.
				
		filePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);		
		if (CompareSuffixNoCase ((char*)"\0.rrd", filePathPtr, NULL))
			{						
			RemoveCharsNoCase ((char*)"\0.rrd\0", filePathPtr);			
			externalFileFlag = TRUE;
			
			}	// end "if (CompareSuffixNoCase ((char*)"\0.rrd", filePathPtr, NULL))"
			
		else if (CompareSuffixNoCase ((char*)"\0.ige", filePathPtr, NULL))
			{
			RemoveCharsNoCase ((char*)"\0.ige\0", filePathPtr);			
			externalFileFlag = TRUE;
			
			}	// end "else if (CompareSuffixNoCase ((char*)"\0.ige", filePathPtr, NULL))"
			
		if (externalFileFlag)
			{
					// Close the external input file. 
										
			CloseFile (fileStreamPtr);
			
			ConcatFilenameSuffix (filePathPtr, (StringPtr)"\0.img\0");
		
					// This will be an attempt to read the associated .img file.
			
			errCode = OpenFileReadOnly (fileStreamPtr, 
													kResolveAliasChains, 
													kLockFile,
													kVerifyFileStream);
			
			}	// end "if (externalFileFlag)"
				
				// Now get the size of the file and store it away for later use.
				// This routine will automatically store it in the file stream
				// structure.
								
		errCode = GetSizeOfFile (fileStreamPtr, NULL);
			
		}	// end "if (fileType != 0 && errCode == noErr)"
		
	if (fileType != 0 && errCode == noErr)
		{			
				// Signify that file is of "Imagine" type. 									
				
		fileInfoPtr->format = kImagineType;
		fileInfoPtr->ancillaryInfoformat = kImagineType;
		
				// Make the default band interleave format Band Non-sequential.
				
		fileInfoPtr->bandInterleave = kBNonSQ;
		
				// Set up swap bytes flag depending upon the system
				// architecture
				                 
		gSwapBytesFlag = gBigEndianFlag; 
		
				// Find first layer node.
				
		continueFlag = GetFirstImagineLayerNode (fileStreamPtr, &ehfaEntry);
		
					// Get the number of lines, columns, channels, data value bits 
					// and data value bytes. Make sure that they are the same for all 
					// of the channels. MultiSpec is not set up to handle layers
					// with differing number of lines, columns, etc.
					// Also MultiSpec is not set up to handle any compressed data.
				
		if (continueFlag)
			continueFlag = ReadImagineImageParameters (fileInfoPtr,
																		fileStreamPtr,
																		&ehfaEntry,
																		&formatErrorCode);
			
		if (continueFlag)
			{
					// Get the storage for the heirarchal information.
					
			fileInfoPtr->hfaHandle = InitializeHierarchalFileStructure (
																		NULL,
																		fileInfoPtr,
																		fileInfoPtr->numberChannels);
			
			if (fileInfoPtr->hfaHandle == NULL)
				continueFlag = FALSE;
				
			}	// end "if (continueFlag)"
							
		if (continueFlag)
			{			
					// Get pointer to the hfa structure.
					
			fileInfoPtr->hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer (
																				fileInfoPtr->hfaHandle,
																				kLock);
			
					// Start at the first layer node again.	
				
			continueFlag = GetFirstImagineLayerNode (fileStreamPtr, &ehfaEntry);
														
					// Find the layer nodes. Assume that the number of layer nodes is 
					// equal to the number of channels.		
						
			channel = 0;
			notDoneFlag = TRUE;
			while (notDoneFlag)
				{
				if (continueFlag)
					{
							// Get the layer offset bytes to the raster data.
							
					continueFlag = ReadImagineLayerBlockParameters (fileInfoPtr,
																					fileStreamPtr,
																					&ehfaEntry,
																					&formatErrorCode,
																					&extensionFileName,
																					&layerStackValidFlagsOffset,
																					channel);
																
					channel++;
					
							// If there is an extension (.ige) file then we are done
							// reading information for the channels. Cannot do this for now
							// because the hfa structure needs to be loaded for each channel.
							
					//if (extensionFileName[0] > 0)
					//	channel = fileInfoPtr->numberChannels;
					
							// Get the next layer node.
					
					if (continueFlag && channel < fileInfoPtr->numberChannels)
						continueFlag = GetNextNode (fileStreamPtr, 
																NULL,
																&ehfaEntry,
																(char*)"Eimg_Layer",
																FALSE);	
					
					else	// channel >= fileInfoPtr->numberChannels																	
						notDoneFlag = FALSE;	
					
					}	// end "if (continueFlag)"
							
				if (!continueFlag)						
					notDoneFlag = FALSE;	
						
				}	// end "while (notDoneFlag)"
					
			CheckAndUnlockHandle (fileInfoPtr->hfaHandle);
			fileInfoPtr->hfaPtr = NULL;
				
			}	// end "if (continueFlag)"
		
				// Set the number of classes in the file info structure if this
				// is a thematic image file.
					
		if (continueFlag && fileInfoPtr->thematicType)
			ReadImagineNumberOfClasses (fileInfoPtr, &ehfaEntry);
			
				// Get the map information for the first channel. Assume for now that
				// it is the same for all channels.
				
		LoadImagineImageMapInfoForChannel (fileInfoPtr, 
														0, 
														&currentLayerChannel,
														TRUE);
														
				// If an extension file actually contains the image data, it is now
				// time to set that file in the file information structure.
				
		if (extensionFileName[0] > 0)
			{													
			CloseFile (fileStreamPtr);
			
					// Set the new name for the file in the file information structure
				
			SetCFileName (fileStreamPtr, (FileStringPtr)&extensionFileName[1]);
	
					// Now try opening this file up to see if it exists.
			
			errCode = OpenFileReadOnly (fileStreamPtr, 
													kResolveAliasChains, 
													kLockFile, 
													kVerifyFileStream);
													
			if (errCode == noErr)
				errCode = ReadImagineExternalDMSLayerBlockParameters (fileInfoPtr,
																						fileStreamPtr,
																						layerStackValidFlagsOffset);
			
			if (errCode != noErr)
				formatErrorCode = 2;
			
			}	// end "if (extensionFileName[0] > 0)"
				
		if (formatErrorCode > 0)
			{
					// Display an alert if MultiSpec cannot read this type of imagine
					//	file or if there was a problem finding the needed parameters.													
			
			SInt16 alertType = kStopAlert;
			if (formatErrorCode == 1)
				alertType = kNoteAlert;
			
			DisplayAlert (kErrorAlertID, 
								alertType, 
								kAlertStrID, 
								IDS_Alert58 + abs (formatErrorCode),
								0, 
								NULL);
								
			}	// end "if (formatErrorCode != noErr)" 
		
		if (!continueFlag)
			returnCode = 1;
			
		if (returnCode == noErr)
			{
					// Set the flag for swapping bytes. 2-byte data is always set for
					// Intel type architecture.
					
			if (fileInfoPtr->numberBytes == 1)
				fileInfoPtr->swapBytesFlag = FALSE;
				
			else	// !fileInfoPtr->numberBytes > 1
				fileInfoPtr->swapBytesFlag = gBigEndianFlag;
			
			if (gGetFileImageType == 0)
				{
				if (fileInfoPtr->thematicType)
					gGetFileImageType = kThematicImageType;
					
				else	// !fileInfoPtr->thematicType 
					gGetFileImageType = kMultispectralImageType;
					
				}	// end "if (gGetFileImageType == 0)" 
				
			else	// gGetFileImageType != 0
				{
						// If this image is being treated as a multispectral image then 
						// make sure the number of classes is 0.
						
				if (gGetFileImageType == kMultispectralImageType)
					fileInfoPtr->numberClasses = 0;
				
				}	// end "else gGetFileImageType != 0"
				
			}	// end "if (returnCode == noErr)"
		/*
		if (returnCode == noErr)
			{
					// Check whether this is an auxilary image file. If so look for
					// the associated image file.
			
			StringPtr			filePathPtr;		
			UInt16				numSuffixChars;
			
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			filePathPtr = GetFilePathPPointer (fileStreamPtr);
						
			if (CompareSuffixNoCase ((char*)"\0.aux", 
														filePathPtr,
														&numSuffixChars))
				{
				fileNameCode = 4;
				
				}			// end ""
				
			}	// end "if (returnCode == noErr)"												
		*/
		gSwapBytesFlag = FALSE;
		
		}	// end "if (fileType != 0 && errCode == noErr)"  
		
	return (returnCode);
	
}	// end "ReadImagineHeader" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineEdmsState
//
//	Software purpose:	This routine reads a Edms_State from the input offset location
//							within the file and takes care of any needed byte swapping for
//							the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 10/12/1999

SInt16 ReadImagineEdmsState (
				CMFileStream*						fileStreamPtr,
				UInt32								edmsStateOffset,
				Edms_State*							edmsStatePtr)

{
	UInt32								count;
	
	SInt16								errCode;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									edmsStateOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		count = sizeof (Edms_State);
		errCode = MReadData (fileStreamPtr, 
										&count, 
										edmsStatePtr,
										kErrorMessages);
										
		edmsStatePtr->numVirtualBlocks = 
							GetLongIntValue ((CharPtr)&edmsStatePtr->numVirtualBlocks);
		edmsStatePtr->numObjectsPerBlock = 
							GetLongIntValue ((CharPtr)&edmsStatePtr->numObjectsPerBlock);
											
		edmsStatePtr->nextObjectNum = 
							GetLongIntValue ((CharPtr)&edmsStatePtr->nextObjectNum);
		edmsStatePtr->compressionType = 
							GetShortIntValue ((CharPtr)&edmsStatePtr->compressionType);
		edmsStatePtr->numberBlockInfos = 
							GetShortIntValue ((CharPtr)&edmsStatePtr->numberBlockInfos);
		edmsStatePtr->virtualBlockInfoPtr = 
							GetLongIntValue ((CharPtr)&edmsStatePtr->virtualBlockInfoPtr);
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "ReadImagineEdmsState" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineEdmsVirtualBlockInfo
//
//	Software purpose:	This routine reads a Edms_State from the input offset location
//							within the file and takes care of any needed byte swapping for
//							the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 10/21/1999

SInt16 ReadImagineEdmsVirtualBlockInfo (
				CMFileStream*						fileStreamPtr,
				UInt32								numVirtualBlocks,
				UInt32								edmsVirtualBlockInfoOffset,
				Edms_VirtualBlockInfo*			edmsVirtualBlockInfoPtr)

{
	UInt32								count;
	
	SInt16								errCode;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									edmsVirtualBlockInfoOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		count = sizeof (Edms_VirtualBlockInfo);
		if (numVirtualBlocks > 1)
			count *= 2;
		
		errCode = MReadData (fileStreamPtr, 
										&count, 
										edmsVirtualBlockInfoPtr,
										kErrorMessages);
										
		edmsVirtualBlockInfoPtr[0].fileCode = 
						GetShortIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[0].fileCode);
		edmsVirtualBlockInfoPtr[0].offset = 
						GetLongIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[0].offset);
		edmsVirtualBlockInfoPtr[0].size = 
						GetLongIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[0].size);
		edmsVirtualBlockInfoPtr[0].logValid = 
						GetShortIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[0].logValid);
		edmsVirtualBlockInfoPtr[0].compressionType = 
				GetShortIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[0].compressionType);
				
		if (numVirtualBlocks > 1)
			{						
			edmsVirtualBlockInfoPtr[1].fileCode = 
						GetShortIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[1].fileCode);
			edmsVirtualBlockInfoPtr[1].offset = 
						GetLongIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[1].offset);
			edmsVirtualBlockInfoPtr[1].size = 
						GetLongIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[1].size);
			edmsVirtualBlockInfoPtr[1].logValid = 
						GetShortIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[1].logValid);
			edmsVirtualBlockInfoPtr[1].compressionType = 
				GetShortIntValue ((CharPtr)&edmsVirtualBlockInfoPtr[1].compressionType);
			
			}	// end "if (numVirtualBlocks > 1)"
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "ReadImagineEdmsVirtualBlockInfo"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineEdscBinFunction
//
//	Software purpose:	This routine reads an Esta_BinFunction structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/21/1999
//	Revised By:			Larry L. Biehl			Date: 10/21/1999

Boolean ReadImagineEdscBinFunction (
				CMFileStream*						fileStreamPtr,
				UInt32								edscBinFunctionOffset,
				UInt32								count,
				Edsc_BinFunction*					edscBinFunctionPtr)

{
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									edscBinFunctionOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
			// For now limit the count to the size of the "Edsc_BinFunction" structure.
			// It may be larger with the dynamic basedata component. I do not 
			// understand it yet.
			
		count = MIN (sizeof (Edsc_BinFunction), count);
			
		errCode = MReadData (fileStreamPtr, 
										&count, 
										edscBinFunctionPtr,
										kErrorMessages);
										
		edscBinFunctionPtr->numBins = 
						GetLongIntValue ((CharPtr)&edscBinFunctionPtr->numBins);
		edscBinFunctionPtr->binFunctionType = 
						GetShortIntValue ((CharPtr)&edscBinFunctionPtr->binFunctionType);
		edscBinFunctionPtr->minLimit = 
						GetDoubleValue ((UCharPtr)&edscBinFunctionPtr->minLimit);
		edscBinFunctionPtr->maxLimit = 
						GetDoubleValue ((UCharPtr)&edscBinFunctionPtr->maxLimit);
			
		}	// end "if (errCode == noErr)"
		
	if (errCode != noErr)
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImagineEdscBinFunction" 		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineEdscColumn
//
//	Software purpose:	This routine reads an Edsc_Column structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1999
//	Revised By:			Larry L. Biehl			Date: 10/19/1999

Boolean ReadImagineEdscColumn (
				CMFileStream*						fileStreamPtr,
				UInt32								edscColumnOffset,
				UInt32								count,
				Edsc_Column*						edscColumnPtr)

{
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									edscColumnOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		count = MIN (sizeof (Edsc_Column), count);
		
		errCode = MReadData (fileStreamPtr, 
										&count, 
										edscColumnPtr,
										kErrorMessages);
										
		edscColumnPtr->numRows = 
								GetLongIntValue ((CharPtr)&edscColumnPtr->numRows);
		edscColumnPtr->columnDataPtr = 
								GetLongIntValue ((CharPtr)&edscColumnPtr->columnDataPtr);
		edscColumnPtr->dataType = 
								GetShortIntValue ((CharPtr)&edscColumnPtr->dataType);
		edscColumnPtr->maxNumChars = 
								GetLongIntValue ((CharPtr)&edscColumnPtr->maxNumChars);
			
		}	// end "if (errCode == noErr)"
		
	if (errCode != noErr)
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImagineEdscColumn" 		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineEhfaEntry
//
//	Software purpose:	This routine reads a Ehfa_File from the input offset location
//							within the file and takes care of any needed byte swapping for
//							the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 10/12/1999

SInt16 ReadImagineEhfaEntry (
				CMFileStream*						fileStreamPtr,
				UInt32								ehfaOffset,
				Ehfa_Entry*							ehfaEntryPtr)

{
	UInt32								count;
	
	SInt16								errCode;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									ehfaOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		count = sizeof (Ehfa_Entry);
		errCode = MReadData (fileStreamPtr, 
										&count, 
										ehfaEntryPtr,
										kErrorMessages);
										
		ehfaEntryPtr->next = GetLongIntValue ((CharPtr)&ehfaEntryPtr->next);
		ehfaEntryPtr->prev = GetLongIntValue ((CharPtr)&ehfaEntryPtr->prev);
		ehfaEntryPtr->parent = GetLongIntValue ((CharPtr)&ehfaEntryPtr->parent);
		ehfaEntryPtr->child = GetLongIntValue ((CharPtr)&ehfaEntryPtr->child);
		ehfaEntryPtr->data = GetLongIntValue ((CharPtr)&ehfaEntryPtr->data);
		ehfaEntryPtr->dataSize = GetLongIntValue ((CharPtr)&ehfaEntryPtr->dataSize);
		ehfaEntryPtr->modTime = GetLongIntValue ((CharPtr)&ehfaEntryPtr->modTime);
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "ReadImagineEhfaEntry" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineEhfaFile
//
//	Software purpose:	This routine reads an Ehfa_File from the input offset location
//							within the file and takes care of any needed byte swapping for
//							the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 10/12/1999

SInt16 ReadImagineEhfaFile (
				CMFileStream*						fileStreamPtr,
				UInt32								ehfaFileOffset,
				Ehfa_File*							ehfaFilePtr)

{
	UInt32								count;
	
	SInt16								errCode;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									ehfaFileOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		count = sizeof (Ehfa_File);
		errCode = MReadData (fileStreamPtr, 
										&count, 
										ehfaFilePtr,
										kErrorMessages);
										
		ehfaFilePtr->version = GetLongIntValue ((CharPtr)&ehfaFilePtr->version);
		ehfaFilePtr->freeList = 
								GetLongIntValue ((CharPtr)&ehfaFilePtr->freeList);
		ehfaFilePtr->rootEntryPtr = 
								GetLongIntValue ((CharPtr)&ehfaFilePtr->rootEntryPtr);
		ehfaFilePtr->entryHeaderLength = 
								GetShortIntValue ((CharPtr)&ehfaFilePtr->entryHeaderLength);
		ehfaFilePtr->dictionaryPtr = 
								GetLongIntValue ((CharPtr)&ehfaFilePtr->dictionaryPtr);
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "ReadImagineEhfaFile" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineEimgLayer
//
//	Software purpose:	This routine reads an Eimg_Layer from the input offset location
//							within the file and takes care of any needed byte swapping for
//							the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 10/12/1999

SInt16 ReadImagineEimgLayer (
				CMFileStream*						fileStreamPtr,
				UInt32								eimgLayerOffset,
				UInt32								count,
				Eimg_Layer*							eimgLayerPtr)

{
	SInt16								errCode;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									eimgLayerOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		errCode = MReadData (fileStreamPtr, 
										&count, 
										eimgLayerPtr,
										kErrorMessages);
										
		eimgLayerPtr->width = GetLongIntValue ((CharPtr)&eimgLayerPtr->width);
		eimgLayerPtr->height = GetLongIntValue ((CharPtr)&eimgLayerPtr->height);
		eimgLayerPtr->layerType = GetShortIntValue ((CharPtr)&eimgLayerPtr->layerType);
		eimgLayerPtr->pixelType = 
										GetShortIntValue ((CharPtr)&eimgLayerPtr->pixelType);
		eimgLayerPtr->blockWidth = 
										GetLongIntValue ((CharPtr)&eimgLayerPtr->blockWidth);
		eimgLayerPtr->blockHeight = 
										GetLongIntValue ((CharPtr)&eimgLayerPtr->blockHeight);
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "ReadImagineEimgLayer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineExternalDMSLayerBlockParameters
//
//	Software purpose:	This routine reads an layer block information in an external DMS
//							file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 07/02/2009
//	Revised By:			Larry L. Biehl			Date: 07/06/2009

SInt16 ReadImagineExternalDMSLayerBlockParameters (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						fileStreamPtr,
				SInt64								layerStackValidFlagsOffset)

{
	CharPtr								magicHeader = (char*)"ERDAS_IMG_EXTERNAL_RASTER";
	char									buffer[256];
	LayerStackPrefix					layerStackPrefix;
	LayerStackValidFlags				layerStackValidFlags;
	
	UInt32								count;
	
	SInt16								errCode,
											stringCompare;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									0,
									kErrorMessages);					

	if (errCode == noErr)
		{
				// Note: will also read the unknown character at the beginning of the Layer Stack Prefix.
				
		count = 27;  
		errCode = MReadData (fileStreamPtr, 
										&count, 
										(char*)buffer,
										kErrorMessages);
			
		}	// end "if (errCode == noErr)"
		
	if (errCode == noErr)
		{
				// Verify that this file has the magic header for a "spill" file.
				
		stringCompare = strncmp ((char*)buffer, (char*)magicHeader, 25);				
		if (stringCompare == 0)
			{
					// This is a "spill" file.  Read the layer stack prefix information.
			
			count = 22;
			errCode = MReadData (fileStreamPtr, 
										&count, 
										(char*)&layerStackPrefix.layerCount,
										kErrorMessages);
										
			if (errCode == noErr)
				{
				layerStackPrefix.layerCount = 
					GetLongIntValue ((CharPtr)&layerStackPrefix.layerCount);
					
				layerStackPrefix.width = 
					GetLongIntValue ((CharPtr)&layerStackPrefix.width);
					
				layerStackPrefix.height = 
					GetLongIntValue ((CharPtr)&layerStackPrefix.height);
					
				layerStackPrefix.blockWidth = 
					GetLongIntValue ((CharPtr)&layerStackPrefix.blockWidth);
					
				layerStackPrefix.blockHeight = 
					GetLongIntValue ((CharPtr)&layerStackPrefix.blockHeight);
																	
				//fileInfoPtr->bandInterleave = kBIBlock;
				//fileInfoPtr->hfaPtr[0].blockSize = layerStackPrefix.blockWidth;

				}	// end "if (errCode == noErr)"
				
					// Now read the layer stack valid flag information.
			
			if (errCode == noErr)
				errCode = MSetMarker (fileStreamPtr, 
												fsFromStart, 
												layerStackValidFlagsOffset,
												kErrorMessages);
						
			count = 20;									
			if (errCode == noErr)
				errCode = MReadData (fileStreamPtr, 
											&count, 
											(char*)&layerStackValidFlags.unknown,
											kErrorMessages);
										
			if (errCode == noErr)
				{
				layerStackValidFlags.unknown = 
					GetLongIntValue ((CharPtr)&layerStackValidFlags.unknown);
					
				layerStackValidFlags.unknown2 = 
					GetLongIntValue ((CharPtr)&layerStackValidFlags.unknown2);
					
				layerStackValidFlags.heightInBlocks = 
					GetLongIntValue ((CharPtr)&layerStackValidFlags.heightInBlocks);
					
				layerStackValidFlags.widthInBlock = 
					GetLongIntValue ((CharPtr)&layerStackValidFlags.widthInBlock);
					
				layerStackValidFlags.unknown3 = 
					GetLongIntValue ((CharPtr)&layerStackValidFlags.unknown3);

				}	// end "if (errCode == noErr)"
										
			}	// end "if (stringCompare == 0)"
			
		else	// stringCompare != 0
			errCode = 1;
			
		}	// end "if (errCode == noErr)"
				
	return (errCode);
	
}	// end "ReadImagineExternalDMSLayerBlockParameters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadImagineExternalRasterDMS
//
//	Software purpose:	This routine reads an ExternalRasterDMS from the input offset 
//							location within the file and takes care of any needed byte 
//							swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/29/2009
//	Revised By:			Larry L. Biehl			Date: 07/13/2009

SInt16 ReadImagineExternalRasterDMS (
				CMFileStream*						fileStreamPtr,
				UInt32								externalRasterDMSOffset,
				ExternalRasterDMS*				externalRasterDMSPtr)

{
	UInt32								count;
	
	SInt16								errCode;
											
											
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									externalRasterDMSOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		count = 8;
		errCode = MReadData (fileStreamPtr, 
										&count, 
										externalRasterDMSPtr,
										kErrorMessages);
			
		}	// end "if (errCode == noErr)"
		
	if (errCode == noErr)
		{
		externalRasterDMSPtr->nameLength = 
						GetLongIntValue ((CharPtr)&externalRasterDMSPtr->nameLength);
						
		externalRasterDMSPtr->nameOffset = 
						GetLongIntValue ((CharPtr)&externalRasterDMSPtr->nameOffset);
						
				// Now read the file name.
		
		if (externalRasterDMSPtr->nameLength <= 254)
			{
					// We will leave the null c-string terminator out of the name length
					
			externalRasterDMSPtr->fileName[0] = (UInt8)externalRasterDMSPtr->nameLength - 1;
			
			count = externalRasterDMSPtr->nameLength;
			errCode = MReadData (fileStreamPtr, 
										&count, 
										&externalRasterDMSPtr->fileName[1],
										kErrorMessages);
										
			}	// end "if (externalRasterDMSPtr->nameLength <= 254)"
			
		else	// externalRasterDMSPtr->nameLength > 254
			errCode = 1;
			
		}	// end "if (errCode == noErr)"

	if (errCode == noErr)
		{
		count = 24;
		errCode = MReadData (fileStreamPtr, 
									&count, 
									(char*)&externalRasterDMSPtr->layerStackValidFlagsOffset,
									kErrorMessages);
									
		if (errCode == noErr)
			{
			externalRasterDMSPtr->layerStackValidFlagsOffset = 
				GetLongInt64Value ((CharPtr)&externalRasterDMSPtr->layerStackValidFlagsOffset);

			//externalRasterDMSPtr->layerStackValidFlagsOffset1 =
			//	GetLongIntValue ((CharPtr)&externalRasterDMSPtr->layerStackValidFlagsOffset1);

			externalRasterDMSPtr->layerStackDataOffset = 
				GetLongInt64Value ((CharPtr)&externalRasterDMSPtr->layerStackDataOffset);

			//externalRasterDMSPtr->layerStackDataOffset1 =
			//	GetLongIntValue ((CharPtr)&externalRasterDMSPtr->layerStackDataOffset1);

			externalRasterDMSPtr->layerStackCount = 
				GetLongIntValue ((CharPtr)&externalRasterDMSPtr->layerStackCount);
										
			externalRasterDMSPtr->layerStackIndex = 
				GetLongIntValue ((CharPtr)&externalRasterDMSPtr->layerStackIndex);
				
			}	// end "if (errCode == noErr)"
			
		}	// end "if (errCode == noErr)"
		
	return (errCode);
	
}	// end "ReadImagineExternalRasterDMS" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineHistogramColumn
//
//	Software purpose:	This routine reads the column of histogram data from the 
//							input offset location within the file and copies the data to
//							the output histogram vector of unsigned longs.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadImagineImageStatisticsForChannel
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1999
//	Revised By:			Larry L. Biehl			Date: 07/08/2009

Boolean ReadImagineHistogramColumn (
				FileInfoPtr							fileInfoPtr,
				CMFileStream*						supportFileStreamPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				UInt32								histogramColumnOffset,
				UInt32								numberValues,
				UInt16								dataType,
				UInt32								numBins,
				UInt16								binFunctionType,
				double								doubleMinLimit,
				double								doubleMaxLimit,
				HUInt32Ptr							histogramArrayPtr,
				HCharPtr								histrogramBufferPtr)

{
	double								countPerBin,
											binFactor,
											limitDifference,
											runningBinCount;
				
	UInt32								binCount,
											bufferIndex,
											compactionBinCount,
											count,
											index,
											lastBufferIndex;
											
	SInt32								maxLimit,
											minLimit;
	
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
	
	
	if (histrogramBufferPtr == NULL)
																							return (FALSE);
	
			// Make sure that the data type can be handled.
			
	if (dataType == 0)
		count = numberValues * sizeof (SInt32);
		
	else if (dataType == 1)
		count = numberValues * sizeof (double);
		
	else	// dataType > 1
		continueFlag = FALSE;
		
	if (continueFlag)
		{
		errCode = MSetMarker (supportFileStreamPtr, 
										fsFromStart, 
										histogramColumnOffset,
										kErrorMessages);					

		if (errCode == noErr)
			errCode = MReadData (supportFileStreamPtr, 
											&count, 
											histrogramBufferPtr,
											kErrorMessages);
			
		if (errCode == noErr)
			{		
			limitDifference = doubleMaxLimit - doubleMinLimit;
			
			if (binFunctionType == 1)
				binFactor = (double)numBins/limitDifference;
					
			if (binFunctionType == 1 && fileInfoPtr->numberBytes > 2) 
				{
						// Make sure that the min and max limits for the bins are consistant
						// with those already read for the channel.  This needs to be done for
						// histograms based on a linear combination so that the bin calculations
						// are correct.
					
				histogramSummaryPtr->minValue = doubleMinLimit;
				histogramSummaryPtr->maxValue = doubleMaxLimit;
						
				histogramSummaryPtr->binFactor = binFactor;
				histogramSummaryPtr->binType = kBinWidthNotOne;
				
				}	// end "if (binFunctionType == 1 && fileInfoPtr->numberBytes > 2)"
				
			else	// binFunctionType != 1 || fileInfoPtr->numberBytes <= 2
				{
				histogramSummaryPtr->binFactor = 1;
				histogramSummaryPtr->binType = kDataValueIsBinIndex;
					
				}	// end "else binFunctionType != 1 || fileInfoPtr->numberBytes <= 2"
					
			minLimit = (SInt32)doubleMinLimit;
			minLimit += fileInfoPtr->signedValueOffset;
			minLimit = MAX (minLimit, 0);
			
			maxLimit = (SInt32)doubleMaxLimit;
			maxLimit += fileInfoPtr->signedValueOffset;
			maxLimit = MIN (maxLimit, (SInt32)fileInfoPtr->numberBins);
										
			if (dataType == 0)
				{
						// Counts are stored in the histogram column as SInt32 values.
						
				HSInt32Ptr histogramSInt32BufferPtr = (HSInt32Ptr)histrogramBufferPtr;
				
				if (binFunctionType == 0)
					{
							// The data value is the bin index.
							
					bufferIndex = 0;
					maxLimit = MIN (maxLimit, minLimit+(SInt32)numberValues-1);
					
					for (index=(UInt32)minLimit; index<=(UInt32)maxLimit; index++)
						{
						histogramArrayPtr[index] = (UInt32)GetLongIntValue (
										(CharPtr)&histogramSInt32BufferPtr[bufferIndex]);
						
						bufferIndex++;
										
						}	// end "for (index=minLimit; index<maxLimit; index++)"
							
					}	// end "if (binFunctionType == 0)"
					
				else if (binFunctionType == 1)
					{
							// This refers to the storage being defined by a linear
							// function: index = (value-minLimit)*numberBins/(maxLimit-minLimit).
							
					lastBufferIndex = ULONG_MAX;
					compactionBinCount = 0;
					binCount = 0;
					
					if (fileInfoPtr->numberBytes <= 2)
						{	
						lastBufferIndex = ULONG_MAX;
						compactionBinCount = 0;
						binCount = 0;
				
						for (index=(UInt32)minLimit; index<=(UInt32)maxLimit; index++)
							{
							bufferIndex = (UInt32)((index-doubleMinLimit)*binFactor);
							bufferIndex = MIN (bufferIndex, numberValues-1);
							
							if (bufferIndex != lastBufferIndex)
								{
								if (compactionBinCount != 0)
									{
									histogramArrayPtr[index-1] += compactionBinCount;
									
									}	// end "if (compactionBinCount != 0)"
									
								compactionBinCount = (SInt32)GetLongIntValue (
											(CharPtr)&histogramSInt32BufferPtr[bufferIndex]);
											
								lastBufferIndex = bufferIndex;
								countPerBin = compactionBinCount*binFactor;
								
								runningBinCount = 0;
											
								}	// and "if (bufferIndex != lastBufferIndex)"
								
							runningBinCount += countPerBin;
								
							binCount = (UInt32)runningBinCount;
							histogramArrayPtr[index] = binCount;
							
							runningBinCount -= binCount;
							compactionBinCount -= binCount;
											
							}	// end "for (index=minLimit; index<maxLimit; index++)"
						
						}	// end "if (fileInfoPtr->numberBytes <= 2)"
					
					else	// fileInfoPtr->numberBytes > 2
						{
						for (index=0; index<numBins; index++)
							{
							histogramArrayPtr[index] = (SInt32)GetLongIntValue (
													(CharPtr)&histogramSInt32BufferPtr[index]);
											
							}	// end "for (index=0; index<numBins; index++)"
										
						minLimit = 0;	
						maxLimit = numBins - 1;
							
						}	// end "else fileInfoPtr->numberBytes > 2"
							
					}	// end "else if (binFunctionType == 1)"
					
				}	// end "if (dataType == 0)"
				
			else if (dataType == 1)
				{
						// Counts are stored in the histogram column as double values.
						
				HDoublePtr histogramDoubleBufferPtr = (HDoublePtr)histrogramBufferPtr;
				
				if (binFunctionType == 0)
					{
					bufferIndex = 0;
					maxLimit = MIN (maxLimit, minLimit+(SInt32)numberValues-1);
				
					for (index=(UInt32)minLimit; index<=(UInt32)maxLimit; index++)
						{
						histogramArrayPtr[index] = (UInt32)GetDoubleValue (
										(UCharPtr)&histogramDoubleBufferPtr[bufferIndex]);
						
						bufferIndex++;
										
						}	// end "for (index=minLimit; index<maxLimit; index++)"
							
					}	// end "if (binFunctionType == 0)"
				
				else if (binFunctionType == 1)
					{
							// This refers to the storage being defined by a linear
							// function: index = (value-minLimit)*numberBins/(maxLimit-minLimit).
					
					if (fileInfoPtr->numberBytes <= 2)
						{					
						lastBufferIndex = ULONG_MAX;
						compactionBinCount = 0;
						binCount = 0;

						for (index=(UInt32)minLimit; index<=(UInt32)maxLimit; index++)
							{
							bufferIndex = (UInt32)((index-doubleMinLimit)*binFactor);
							bufferIndex = MIN (bufferIndex, numberValues-1);
							
							if (bufferIndex != lastBufferIndex)
								{
								if (compactionBinCount != 0)
									{
									histogramArrayPtr[index-1] += compactionBinCount;
									
									}	// end "if (compactionBinCount != 0)"
									
								compactionBinCount = (SInt32)GetDoubleValue (
											(UCharPtr)&histogramDoubleBufferPtr[bufferIndex]);
											
								lastBufferIndex = bufferIndex;
								countPerBin = compactionBinCount*binFactor;
								
								runningBinCount = 0;
											
								}	// and "if (bufferIndex != lastBufferIndex)"
								
							runningBinCount += countPerBin;
								
							binCount = (UInt32)runningBinCount;
							histogramArrayPtr[index] = binCount;
							
							runningBinCount -= binCount;
							compactionBinCount -= binCount;
											
							}	// end "for (index=minLimit; index<maxLimit; index++)"
						
						}	// end "if (fileInfoPtr->numberBytes <= 2)"
					
					else	// fileInfoPtr->numberBytes > 2
						{
						for (index=0; index<numBins; index++)
							{
							histogramArrayPtr[index] = (SInt32)GetDoubleValue (
											(UCharPtr)&histogramDoubleBufferPtr[index]);
											
							}	// end "for (index=0; index<numBins; index++)"
											
						minLimit = 0;
						maxLimit = numBins - 1;
							
						}	// end "else fileInfoPtr->numberBytes > 2"
							
					}	// end "else if (binFunctionType == 1)"
					
				}	// end "else if (dataType == 1)"
			
			if (minLimit < maxLimit)
				{
				if (histogramSummaryPtr->binType == kDataValueIsBinIndex)
					{
							// Zero out the histogram value array up to the minimum		
							// data value.											
					
					for (index=0; index<(UInt32)minLimit; index++)
						histogramArrayPtr[index] = 0; 
						
					}	// end "if (histogramSummaryPtr->binType == kDataValueIsBinIndex)"
				
						// Zero out the histogram value array from one more than the
						// maxLimit up to the maximum possible data value.								
							
				UInt32 maxPossibleDataValue = fileInfoPtr->numberBins-1;
				for (index=(UInt32)(maxLimit+1); index<=maxPossibleDataValue; index++)
					histogramArrayPtr[index] = 0; 
					
				}	// end "if (minLimit < maxLimit)"
				
			}	// end "if (errCode == noErr)"
			
		else	// errCode != noErr
			continueFlag = FALSE;
			
		}	// end "if (continueFlag)"
		
	return (continueFlag);
	
}	// end "ReadImagineHistogramColumn" 		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineImageParameters
//
//	Software purpose:	This routine determines the number of channels in the imagine
//							formatted file and reads the number of lines, columns, bits,
//							bytes, and type of image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 12/14/2005

Boolean ReadImagineImageParameters (
				FileInfoPtr 						fileInfoPtr, 
				CMFileStream*						fileStreamPtr,
				Ehfa_Entry*							ehfaEntryPtr,
				SInt16*								formatErrorCodePtr)

{
	CharPtr								eimgLayerType = (char*)"Eimg_Layer";
	
	Eimg_Layer							eimgLayer;
										
	UInt32								numberChannels;
	
	SInt16								errCode,
											layerType,
											pixelType,
											stringCompare;
	
	Boolean								channelNotSameFlag,
											continueFlag,
											notDoneFlag;
										
		
			// Initialize local variables.
			
	channelNotSameFlag = FALSE;
	continueFlag = TRUE;
	errCode = noErr;
	numberChannels = 0;
	
	*formatErrorCodePtr = noErr;
	
			// Find the layer nodes. Assume that the number of layer nodes is equal
			// to the number of channels.		
				
	notDoneFlag = TRUE;
	while (notDoneFlag)
		{
		if (errCode == noErr)
			{
					// Determine if this is a layer node.
					
			stringCompare = strncmp ((char*)ehfaEntryPtr->type, (char*)eimgLayerType, 10);
			if (stringCompare == 0)
				{
				numberChannels++;	
				
						// Now get the basic information for the layer.
							
				errCode = ReadImagineEimgLayer (fileStreamPtr,
															ehfaEntryPtr->data,
															ehfaEntryPtr->dataSize,
															&eimgLayer);
															
				if (numberChannels == 1)
					{
					fileInfoPtr->numberLines = eimgLayer.height;
					fileInfoPtr->numberColumns = eimgLayer.width;	
	
					if (eimgLayer.layerType == 0)
						fileInfoPtr->thematicType = TRUE;
					
					switch (eimgLayer.pixelType)
						{
						case 0:		// 1 bits
							*formatErrorCodePtr = 2;
							break;
							
						case 1:		// 2 bits
							*formatErrorCodePtr = 3;
							break;
							
						case 2:		// 4 bits
							fileInfoPtr->numberBytes = 1;
							fileInfoPtr->numberBits = 4;
							fileInfoPtr->numberBins = 16;
							fileInfoPtr->dataTypeCode = kIntegerType;
							break;
							
						case 3:		// 8 unsigned bits
							fileInfoPtr->numberBytes = 1;
							fileInfoPtr->numberBits = 8;
							fileInfoPtr->numberBins = 256;
							fileInfoPtr->dataTypeCode = kIntegerType;
							break;
							
						case 4:		// 8 signed bits
							fileInfoPtr->numberBytes = 1;
							fileInfoPtr->numberBits = 8;
							fileInfoPtr->numberBins = 256;
							fileInfoPtr->dataTypeCode = kIntegerType;
							fileInfoPtr->signedDataFlag = TRUE;
							break;
							
						case 5:		// 16 unsigned bits
							fileInfoPtr->numberBytes = 2;
							fileInfoPtr->numberBits = 16;
							fileInfoPtr->numberBins = 65536;
							fileInfoPtr->dataTypeCode = kIntegerType;
							break;
							
						case 6:		// 16 signed bits
							fileInfoPtr->numberBytes = 2;
							fileInfoPtr->numberBits = 16;
							fileInfoPtr->numberBins = 65536;
							fileInfoPtr->dataTypeCode = kIntegerType;
							fileInfoPtr->signedDataFlag = TRUE;
							break;
							
						case 7:		// 32 unsigned bits
							fileInfoPtr->numberBytes = 4;
							fileInfoPtr->numberBits = 32;
							fileInfoPtr->numberBins = 2048;
							fileInfoPtr->dataTypeCode = kIntegerType;
							break;
							
						case 8:		// 32 signed bits
							fileInfoPtr->numberBytes = 4;
							fileInfoPtr->numberBits = 32;
							fileInfoPtr->numberBins = 2048;
							fileInfoPtr->dataTypeCode = kIntegerType;
							fileInfoPtr->signedDataFlag = TRUE;
							break;
							
						case 9:		// 32 bit float
							fileInfoPtr->numberBytes = 4;
							fileInfoPtr->numberBits = 32;
							fileInfoPtr->numberBins = 2048;
							fileInfoPtr->dataTypeCode = kRealType;
							fileInfoPtr->signedDataFlag = TRUE;
							break;
							
						case 10:		// 64 bit float
							fileInfoPtr->numberBytes = 8;
							fileInfoPtr->numberBits = 64;
							fileInfoPtr->numberBins = 2048;
							fileInfoPtr->dataTypeCode = kRealType;
							fileInfoPtr->signedDataFlag = TRUE;
							break;
							
						case 11:		// complex
						case 12:		// complex
							*formatErrorCodePtr = 7;
							break;
							
						default:
							*formatErrorCodePtr = 8;
							break;
						
						}	// end "switch (eimgLayer.pixelType)"
						
							// Save the pixel type and layer type for later use.
							
					pixelType = eimgLayer.pixelType;	
					layerType = eimgLayer.layerType;
					
					}	// end "if (numberChannels == 1)"
					
				else	// numberChannels > 1
					{
							// Verify that the information for this channel is the same
							// as for the first channel.
	
					if (eimgLayer.layerType != layerType)
						channelNotSameFlag = TRUE;
					
					else if (eimgLayer.pixelType != pixelType)
						channelNotSameFlag = TRUE;
					
					}	// end "else numberChannels > 1"
				
				}	// end "if (stringCompare == 0)"
			
					// Get the next node at this same level.
					
			if (ehfaEntryPtr->next > 0)						
				errCode = ReadImagineEhfaEntry (fileStreamPtr,
															ehfaEntryPtr->next,
															ehfaEntryPtr);
															
			else	// ehfaEntry.next == 0	
				notDoneFlag = FALSE;	
				
			}	// end "if (errCode == noErr)"
			
		else	// errCode != noErr
			notDoneFlag = FALSE;	
				
		}	// end "while (notDoneFlag)"
	
	if (numberChannels == 0)
		*formatErrorCodePtr = 9;
		
	if (channelNotSameFlag && *formatErrorCodePtr == noErr)
		{
		*formatErrorCodePtr = 1;	
		numberChannels = 1;
		
		}	// end "if (channelNotSameFlag && *formatErrorCodePtr == noErr)"
		
	fileInfoPtr->numberChannels = (UInt16)numberChannels;
	
	if (errCode != noErr || *formatErrorCodePtr > 1)
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImagineImageParameters" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImagineLayerBlockParameters
//
//	Software purpose:	This routine determines the number of channels in the imagine
//							formatted file and reads the number of lines, columns, bits,
//							bytes, and type of image file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/12/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017
				
Boolean ReadImagineLayerBlockParameters (
				FileInfoPtr 						fileInfoPtr, 
				CMFileStream*						fileStreamPtr,
				Ehfa_Entry*							ehfaEntryLayerPtr,
				SInt16*								formatErrorCodePtr,
				FileStringName255*				extensionFileNamePtr,
				SInt64*								layerStackValidFlagsOffsetPtr,
				UInt32								channel)
{
	UCharPtr								ehfaNodeType = (UCharPtr)"Edms_State";
	UCharPtr								externRasterDMSType = (UCharPtr)"ImgExternalRaster";
	
	ldiv_t 								ratio;
	
	Edms_State							edmsState;
	Edms_VirtualBlockInfo			edmsVirtualBlockInfo[2];
	Ehfa_Entry							ehfaEntry;
	Eimg_Layer							eimgLayer;
	ExternalRasterDMS					externalRasterDMS;
	
	UInt32								blockOffset,
											numberBlockHeights,
											numberBlockWidths;
	
	SInt16								errCode,
											stringCompare;
	
	Boolean								blockInfoSetFlag,
											continueFlag,
											layerOffsetBytesSetFlag,
											notDoneFlag;
										
		
			// Initialize local variables.
	
	blockInfoSetFlag = FALSE;
	continueFlag = TRUE;		
	layerOffsetBytesSetFlag = FALSE;
	edmsVirtualBlockInfo[0].offset = 0;
	edmsVirtualBlockInfo[1].offset = 0;
	
	*formatErrorCodePtr = noErr;
				
			// Now get the basic information for the layer.
				
	errCode = ReadImagineEimgLayer (fileStreamPtr,
												ehfaEntryLayerPtr->data,
												ehfaEntryLayerPtr->dataSize,
												&eimgLayer);
												
	if (errCode == noErr)
		{			
		fileInfoPtr->hfaPtr[channel].blockHeight = eimgLayer.blockHeight;
		fileInfoPtr->hfaPtr[channel].blockWidth = eimgLayer.blockWidth;
		
		if (channel == 0 && 
						(UInt32)eimgLayer.blockWidth != fileInfoPtr->numberColumns)
			{
			fileInfoPtr->bandInterleave = kBNonSQBlocked;
			fileInfoPtr->blockedFlag = TRUE;
			
			}	// end "if (channel == 0 && eimgLayer.blockWidth < ..."
			
				// Get the number of bytes to skip to go from the start of one
				// line to the start of the next line, taking the tiles into
				// account.
			
		ratio = ldiv (fileInfoPtr->numberColumns, eimgLayer.blockWidth);
		numberBlockWidths = ratio.quot;
		
		if (ratio.rem > 0)
			numberBlockWidths++;
		
		fileInfoPtr->hfaPtr[channel].numberBlockWidths = numberBlockWidths;
			
				// Get the number of blocks that represent the number of lines in the image.
				// This is only used for images stored in an extension file.
			
		ratio = ldiv (fileInfoPtr->numberLines, eimgLayer.blockHeight);
		numberBlockHeights = ratio.quot;
		
		if (ratio.rem > 0)
			numberBlockHeights++;
		
		fileInfoPtr->hfaPtr[channel].numberBlockHeights = numberBlockHeights;
		
		blockInfoSetFlag = TRUE;
		
		}	// end "if (errCode == noErr)"

			// Find the first node under the layer node.	
							
	errCode = ReadImagineEhfaEntry (fileStreamPtr,
												ehfaEntryLayerPtr->child,
												&ehfaEntry);
												
			// Find the RasterDMS node. 	
				
	notDoneFlag = TRUE;
	while (notDoneFlag)
		{
		if (errCode == noErr)
			{
					// Determine if this is the Edms_State node.
					
			stringCompare = strncmp ((char*)ehfaEntry.type, (char*)ehfaNodeType, 10);				
			if (stringCompare == 0)
				{
						// Now get the edms_state information for the layer.
							
				errCode = ReadImagineEdmsState (fileStreamPtr,
															ehfaEntry.data,
															&edmsState);
				
				if (errCode == noErr)	
					{									
					ReadImagineEdmsVirtualBlockInfo (fileStreamPtr,
																	edmsState.numVirtualBlocks,
																	edmsState.virtualBlockInfoPtr,
																	&edmsVirtualBlockInfo[0]);
						
					fileInfoPtr->hfaPtr[channel].layerOffsetBytes = 
																	edmsVirtualBlockInfo[0].offset;
															
					fileInfoPtr->hfaPtr[channel].blockSize = 
																	edmsVirtualBlockInfo[0].size;
															
					fileInfoPtr->hfaPtr[channel].blockOffset = 
															fileInfoPtr->hfaPtr[channel].blockSize;
															
					if (edmsState.numVirtualBlocks > 1)
						{
								// Check if the blocks skip in memory. This may be done if the
								// blocks are interleaved by band.
							
						blockOffset = edmsVirtualBlockInfo[1].offset - 
																		edmsVirtualBlockInfo[0].offset;
																			
						if (blockOffset > (UInt32)edmsVirtualBlockInfo[0].size)
							{
							fileInfoPtr->hfaPtr[channel].blockOffset = blockOffset;
							
							if (channel == 0)
								{
								fileInfoPtr->bandInterleave = kBIBlock;
								fileInfoPtr->numberHeaderBytes = edmsVirtualBlockInfo[0].offset;
								
								}	// end "if (channel == 0)"
							
							}	// end "if (blockOffset > edmsVirtualBlockInfo[0].size)"
					
						}	// end "if (edmsState.numVirtualBlocks > 1)"
				
					layerOffsetBytesSetFlag = TRUE;
					
					if (edmsState.compressionType != 0 || 
												edmsVirtualBlockInfo[0].compressionType != 0)
						{
						*formatErrorCodePtr = 9;
						errCode = -1;
						
						}	// end "if (edmsState.compressionType != 0 || ..."
					
					}	// end "if (errCode == noErr)"
															
				}	// end "if (stringCompare == 0)"
				
			else	// stringCompare != 0
				{			
						// It is not a RASTERDMS node,
						// determine if this is an ExternalRasterDMS node.
					
				stringCompare = strncmp ((char*)ehfaEntry.type,
													(char*)externRasterDMSType,
													17);
				if (stringCompare == 0)
					{
							// Now get the external rastor information.
								
					errCode = ReadImagineExternalRasterDMS (fileStreamPtr,
																			ehfaEntry.data,
																			&externalRasterDMS);
																			
					if (errCode == noErr)
						{
						if (channel == 0)
							{
							CopyPToP ((UCharPtr)extensionFileNamePtr,
											(UCharPtr)externalRasterDMS.fileName);
							
							fileInfoPtr->bandInterleave = kBIBlock;
							fileInfoPtr->numberHeaderBytes =
													(UInt32)externalRasterDMS.layerStackDataOffset;
															
							*layerStackValidFlagsOffsetPtr =
													externalRasterDMS.layerStackValidFlagsOffset;
													
							}	// end "if (channel == 0)"
						
						fileInfoPtr->hfaPtr[channel].blockSize = 
												eimgLayer.blockWidth * eimgLayer.blockHeight * 
																				fileInfoPtr->numberBytes;
															
						fileInfoPtr->hfaPtr[channel].blockOffset = fileInfoPtr->numberChannels *
																fileInfoPtr->hfaPtr[channel].blockSize;
															
						fileInfoPtr->hfaPtr[channel].layerOffsetBytes =
										fileInfoPtr->numberHeaderBytes +
													channel * fileInfoPtr->hfaPtr[channel].blockSize;
							
						layerOffsetBytesSetFlag = TRUE;
						
						}	// end "if (errCode == noErr)"
					
					}	// end "if (stringCompare == 0)"
				
				}	// end "else stringCompare != 0"
			
					// Get the next node at this same level.
					
			if (errCode == noErr && ehfaEntry.next > 0)						
				errCode = ReadImagineEhfaEntry (fileStreamPtr,
															ehfaEntry.next,
															&ehfaEntry);
															
			else	// ehfaEntry.next == 0	
				notDoneFlag = FALSE;	
				
			}	// end "if (errCode == noErr)"
			
		else	// errCode != noErr
			notDoneFlag = FALSE;	
		
		if (layerOffsetBytesSetFlag)
			notDoneFlag = FALSE;
				
		}	// end "while (notDoneFlag)"
	
	if (errCode != noErr)
		continueFlag = FALSE;
			
	if (!layerOffsetBytesSetFlag || !blockInfoSetFlag)
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImagineLayerBlockParameters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImageEstaStatistics
//
//	Software purpose:	This routine reads an Esta_Statistics structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 10/18/1999

Boolean ReadImageEstaStatistics (
				CMFileStream*						fileStreamPtr,
				UInt32								estaStatisticsOffset,
				UInt32								count,
				Esta_Statistics*					estaStatisticsPtr)

{
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
										
			
	errCode = MSetMarker (fileStreamPtr, 
									fsFromStart, 
									estaStatisticsOffset,
									kErrorMessages);					

	if (errCode == noErr)
		{
		errCode = MReadData (fileStreamPtr, 
										&count, 
										estaStatisticsPtr,
										kErrorMessages);
										
		estaStatisticsPtr->minimum = 
										GetDoubleValue ((UCharPtr)&estaStatisticsPtr->minimum);
		estaStatisticsPtr->maximum = 
										GetDoubleValue ((UCharPtr)&estaStatisticsPtr->maximum);
		estaStatisticsPtr->mean = 
										GetDoubleValue ((UCharPtr)&estaStatisticsPtr->mean);
		estaStatisticsPtr->median = 
										GetDoubleValue ((UCharPtr)&estaStatisticsPtr->median);
		estaStatisticsPtr->mode = 
										GetDoubleValue ((UCharPtr)&estaStatisticsPtr->mode);
		estaStatisticsPtr->stddev = 
										GetDoubleValue ((UCharPtr)&estaStatisticsPtr->stddev);
			
		}	// end "if (errCode == noErr)"
		
	if (errCode != noErr)
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImageEstaStatistics" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImageEprjMapInfo
//
//	Software purpose:	This routine reads an Eprj_MapInfo structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadImagineImageMapInfoForChannel in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/02/2001
//	Revised By:			Larry L. Biehl			Date: 02/03/2001

Boolean ReadImageEprjMapInfo (
				CMFileStream*						fileStreamPtr,
				UInt32								eprjMapInfoOffset,
				UInt32								count,
				Eprj_MapInfo*						eprjMapInfoPtr)

{
	UCharPtr								bufferPtr,
											lBufferPtr;
											
	SInt32								numberArrays,
											numberCharacters,
											offset = 0;
	
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
	
	
	eprjMapInfoPtr->projectionName[0] = 0;
	eprjMapInfoPtr->xMapCoordinate11 = 0.;
	eprjMapInfoPtr->yMapCoordinate11 = 0.;
	eprjMapInfoPtr->xMapCoordinateLowerRight = 0.;
	eprjMapInfoPtr->yMapCoordinateLowerRight = 0.;
	eprjMapInfoPtr->horizontalPixelSize = 1.;
	eprjMapInfoPtr->verticalPixelSize = 1.;
	eprjMapInfoPtr->mapUnits[0] = 0;
				
	bufferPtr = (UCharPtr)MNewPointer (count);
	
	if (bufferPtr != NULL)
		{
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										eprjMapInfoOffset,
										kErrorMessages);					

		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											bufferPtr,
											kErrorMessages);
				
		if (errCode == noErr)
			{	
					// Get the projection name
			
			lBufferPtr = bufferPtr;		
			numberCharacters = GetLongIntValue ((char*)lBufferPtr);
			
			if (numberCharacters <= 64)
				{
						// The following represents the offset into the file where
						// the map projection string name starts. Take into account
						// the fact that this buffer does not start from the beginning
						// of the file.
						
				lBufferPtr += 4;
				offset = GetLongIntValue ((char*)lBufferPtr) - eprjMapInfoOffset;	
			
				if (offset >= 0 && offset + numberCharacters < (SInt32)count)
					{
					BlockMoveData (&bufferPtr[offset], 
											eprjMapInfoPtr->projectionName, 
											numberCharacters);
											
					lBufferPtr = &bufferPtr[offset] + numberCharacters;
					
					}	// end "if (offset >= 0 && ..."
											
				else	// offset < 0 || ...
					continueFlag = FALSE;
										
				}	// end "if (numberCharacters <= 64)"
				
			if (continueFlag)
				{
						// Get the map coordinates for the upper left corner.
						
				numberArrays = GetLongIntValue ((char*)lBufferPtr);	
				
				if (numberArrays == 1)
					{		
							// Get offset into the file.
									
					lBufferPtr += 4;
					offset = GetLongIntValue ((char*)lBufferPtr) - eprjMapInfoOffset;
					
					if (offset < 0 || offset > (SInt32)count)
						continueFlag = TRUE;
					
					if (continueFlag)
						{
						lBufferPtr = &bufferPtr[offset];
						eprjMapInfoPtr->xMapCoordinate11 = GetDoubleValue (lBufferPtr);
						
						lBufferPtr += 8;
						eprjMapInfoPtr->yMapCoordinate11 = GetDoubleValue (lBufferPtr);
						
						lBufferPtr += 8;
						
						}	// end "if (continueFlag)"
					
					}	// end "if (numberArrays == 1)"	
					
				else	// numberArrays != 1
					continueFlag = FALSE;
					
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
						// Get the map coordinates for the lower right corner.
						
				numberArrays = GetLongIntValue ((char*)lBufferPtr);	
				
				if (numberArrays == 1)
					{		
							// Get offset into the file.
									
					lBufferPtr += 4;
					offset = GetLongIntValue ((char*)lBufferPtr) - eprjMapInfoOffset;
					
					if (offset < 0 || offset > (SInt32)count)
						continueFlag = TRUE;
					
					if (continueFlag)
						{
						lBufferPtr = &bufferPtr[offset];
						eprjMapInfoPtr->xMapCoordinateLowerRight = 
																		GetDoubleValue (lBufferPtr);
						
						lBufferPtr += 8;
						eprjMapInfoPtr->yMapCoordinateLowerRight = 
																		GetDoubleValue (lBufferPtr);
						
						lBufferPtr += 8;
						
						}	// end "if (continueFlag)"
					
					}	// end "if (numberArrays == 1)"	
					
				else	// numberArrays != 1
					continueFlag = FALSE;
					
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
						// Get the pixel size.
						
				numberArrays = GetLongIntValue ((char*)lBufferPtr);	
				
				if (numberArrays == 1)
					{		
							// Get offset into the file.
									
					lBufferPtr += 4;
					offset = GetLongIntValue ((char*)lBufferPtr) - eprjMapInfoOffset;
					
					if (offset < 0 || offset > (SInt32)count)
						continueFlag = TRUE;
					
					if (continueFlag)
						{
						lBufferPtr = &bufferPtr[offset];
						eprjMapInfoPtr->horizontalPixelSize = GetDoubleValue (lBufferPtr);
						
						lBufferPtr += 8;
						eprjMapInfoPtr->verticalPixelSize = GetDoubleValue (lBufferPtr);
						
						lBufferPtr += 8;
						
						}	// end "if (continueFlag)"
					
					}	// end "if (numberArrays == 1)"	
					
				else	// numberArrays != 1
					continueFlag = FALSE;	
					
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
						// Get the map units
					
				numberCharacters = GetLongIntValue ((char*)lBufferPtr);
				
				if (numberCharacters <= 32)
					{
							// Get offset into the file.
							
					lBufferPtr += 4;
					offset = GetLongIntValue ((char*)lBufferPtr) - eprjMapInfoOffset;	
				
					if (offset >= 0 && offset + numberCharacters <= (SInt32)count)
						BlockMoveData (&bufferPtr[offset], 
												eprjMapInfoPtr->mapUnits, 
												numberCharacters);
												
					else	// offset < 0 || ...
						continueFlag = FALSE;
											
					}	// end "if (numberCharacters <= 64)"
					
				else	// numberCharacters > 32
					continueFlag = FALSE;	
					
				}	// end "if (continueFlag)"
				
			}	// end "if (errCode == noErr)"
		
		else	// errCode != noErr)
			continueFlag = FALSE;
			
		}	// end "if (bufferPtr != NULL)"
		
	else	// bufferPtr == NULL	
		continueFlag = FALSE;
		
	CheckAndDisposePtr (bufferPtr);
		
	return (continueFlag);
	
}	// end "ReadImageEprjMapInfo"  		



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImageEprjMapProjection
//
//	Software purpose:	This routine reads an Eprj_MapProjection structure from the 
//							input offset location within the file. I have not found a 
//							description of this structure but it appears to be a text 
//							for the most part that describes a user defined map projection.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			 in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/23/2006
//	Revised By:			Larry L. Biehl			Date: 01/24/2006

Boolean ReadImageEprjMapProjection (
				CMFileStream*						fileStreamPtr,
				UInt32								eprjMapProjectionOffset,
				UInt32								count,
				Eprj_ProParameters*				eprjProParametersPtr,
				Eprj_Spheroid*						eprjSpheroidPtr)

{
	char									*bufferPtr,
											*lBufferPtr,
											*stringPtr;
											
	SInt32								index,
											maxNumberProParams = 15,
											numberCharacters,
											offset = 0;
	
	SInt16								errCode,
											number;
	
	Boolean								continueFlag = TRUE;
	
	
	eprjProParametersPtr->proType = 0;
	eprjProParametersPtr->proNumber = 0;
	eprjProParametersPtr->proExeName[0] = 0;
	eprjProParametersPtr->proName[0] = 0;
	eprjProParametersPtr->proZone = 0;
	
	for (index=0; index<maxNumberProParams; index++)
		eprjProParametersPtr->proParams[index] = 0.;
	
	eprjSpheroidPtr->sphereName[0] = 0;
	eprjSpheroidPtr->sphereCode = 0;
	eprjSpheroidPtr->a = 0.;
	eprjSpheroidPtr->b = 0.;
	eprjSpheroidPtr->eSquared = 0.;
	eprjSpheroidPtr->radius = 0.;
	
	bufferPtr = MNewPointer (count+1);
	
	if (bufferPtr != NULL)
		{
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										eprjMapProjectionOffset,
										kErrorMessages);					

		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											bufferPtr,
											kErrorMessages);
											
		bufferPtr[count] = 0;
				
		if (errCode == noErr)
			{	
			lBufferPtr = bufferPtr;
			
					// The first string should be "PE_CORDSYS" (Projection Engine Coordinate 
					// System.
					
			numberCharacters = GetLongIntValue (lBufferPtr);
			lBufferPtr += 4;
			
			offset = GetLongIntValue (lBufferPtr) - eprjMapProjectionOffset;
			lBufferPtr = &bufferPtr[offset];
			
			stringPtr = strstr (lBufferPtr, "PE_COORDSYS");
			if (stringPtr == NULL)
				continueFlag = FALSE;
			
			if (continueFlag)
				{
						// The next string should be the description of what follows.
						
				lBufferPtr += numberCharacters;
				numberCharacters = GetLongIntValue (lBufferPtr);
				lBufferPtr += 4;
			
				offset = GetLongIntValue (lBufferPtr) - eprjMapProjectionOffset;
				lBufferPtr = &bufferPtr[offset];
				
				stringPtr = strstr (lBufferPtr, "{0:pcstring,}");
				if (stringPtr == NULL)
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
						// The next string will be the PROJCS information.  There are two
						// number characters/offsets in a row here.  Do not know why yet.
						
				lBufferPtr += numberCharacters;
				numberCharacters = GetLongIntValue (lBufferPtr);
				lBufferPtr += 4;
			
				offset = GetLongIntValue (lBufferPtr) - eprjMapProjectionOffset;
				lBufferPtr = &bufferPtr[offset];
				
						// Skip second number characters/offset (which must be something else).
						
				//numberCharacters = GetLongIntValue (lBufferPtr);
				//lBufferPtr += 4;
							
				//offset = GetLongIntValue (lBufferPtr) - eprjMapProjectionOffset;
				//lBufferPtr = &bufferPtr[offset];

				lBufferPtr += 8;
				
						// This string should contain "PROJCS".
				
				stringPtr = strstr (lBufferPtr, "PROJCS");
				if (stringPtr == NULL)
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)"
				
			if (continueFlag)
				{
						// Get the spheroid and the projection parameters.
						// For now this is specific to Lambert_Azimuthal_Equal_Area. Don't
						// know if there is a specific template that is followed for this
						// string.
				
						// Check for user define spheroid
							
				stringPtr = strstr (lBufferPtr, "User_Defined_Spheroid");
				if (stringPtr != NULL)
					{
					eprjSpheroidPtr->sphereCode = kSphereEllipsoidCode;
					number = sscanf (stringPtr, 
											"User_Defined_Spheroid\",%lf,%lf",
											&eprjSpheroidPtr->a, 
											&eprjSpheroidPtr->b);
											
					eprjSpheroidPtr->radius = eprjSpheroidPtr->a;
					eprjSpheroidPtr->b = eprjSpheroidPtr->a;
					
					}	//  end "if (stringPtr != NULL)"
					
						// Get info for the central meridian.
							
				stringPtr = strstr (lBufferPtr, "Central_Meridian");
				if (stringPtr != NULL)
					{
					number = sscanf (stringPtr, 
											"Central_Meridian\",%lf",
											&eprjProParametersPtr->proParams[4]);
					
					}	//  end "if (stringPtr != NULL)"
					
						// Get info for the latitude of origin.
							
				stringPtr = strstr (lBufferPtr, "Latitude_Of_Origin");
				if (stringPtr != NULL)
					{
					number = sscanf (stringPtr, 
											"Latitude_Of_Origin\",%lf",
											&eprjProParametersPtr->proParams[5]);
					
					}	//  end "if (stringPtr != NULL)"
					
						// Get info for false easting.
							
				stringPtr = strstr (lBufferPtr, "False_Easting");
				if (stringPtr != NULL)
					{
					number = sscanf (stringPtr, 
											"False_Easting\",%lf",
											&eprjProParametersPtr->proParams[6]);
					
					}	//  end "if (stringPtr != NULL)"
					
						// Get info for false northing.
							
				stringPtr = strstr (lBufferPtr, "False_Northing");
				if (stringPtr != NULL)
					{
					number = sscanf (stringPtr, 
											"False_Easting\",%lf",
											&eprjProParametersPtr->proParams[7]);
					
					}	//  end "if (stringPtr != NULL)"
				/*
				mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian =
																		eprjProParameters.proParams[2];
				mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian = 
											eprjProParameters.proParams[4] * kRadiansToDegrees;
				mapProjectionInfoPtr->gridCoordinate.latitudeOrigin = 
											eprjProParameters.proParams[5] * kRadiansToDegrees;
				mapProjectionInfoPtr->gridCoordinate.falseEasting = 
																		eprjProParameters.proParams[6];
				mapProjectionInfoPtr->gridCoordinate.falseNorthing = 
																		eprjProParameters.proParams[7];
				*/
				}	// end "if (continueFlag)"
				
			}	// end "if (errCode == noErr)"
		
		else	// errCode != noErr)
			continueFlag = FALSE;
			
				// Dispose of the buffer. We do not need it any more.
			
		CheckAndDisposePtr (bufferPtr);
			
		}	// end "if (bufferPtr != NULL)"
		
	else	// bufferPtr == NULL	
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImageEprjMapProjection" 		 	 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImageEprjDatum
//
//	Software purpose:	This routine reads an Eprj_Datum structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries. For now only the datum name
//							will be used.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			 in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/22/2003
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadImageEprjDatum (
				CMFileStream*						fileStreamPtr,
				UInt32								eprjDatumOffset,
				UInt32								count,
				Eprj_Datum*							eprjDatumPtr)

{
	UCharPtr								bufferPtr,
											lBufferPtr;
											
	SInt32								numberCharacters,
											numberParemeters,
											offset = 0;
	
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE;
	
	
	eprjDatumPtr->name[0] = 0;
	eprjDatumPtr->type = 0;
	eprjDatumPtr->params[0] = 0;
	eprjDatumPtr->params[1] = 0;
	eprjDatumPtr->params[2] = 0;
	eprjDatumPtr->params[3] = 0;
	eprjDatumPtr->params[4] = 0;
	eprjDatumPtr->params[5] = 0;
	eprjDatumPtr->params[6] = 0;
	eprjDatumPtr->gridName[0] = 0;
				
	bufferPtr = (UCharPtr)MNewPointer (count);
	
	if (bufferPtr != NULL)
		{
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										eprjDatumOffset,
										kErrorMessages);					

		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											bufferPtr,
											kErrorMessages);
				
		if (errCode == noErr)
			{	
					// Get the datum name
			
			lBufferPtr = bufferPtr;	
			numberCharacters = GetLongIntValue ((char*)lBufferPtr);
			lBufferPtr += 4;
				
					// Now get offset to the characters.
					
			offset = GetLongIntValue ((char*)lBufferPtr);
			if (offset != 0)
				{
				offset -= eprjDatumOffset;
				lBufferPtr = &bufferPtr[offset] + numberCharacters;
				
				numberCharacters = MIN (numberCharacters, 63);
				BlockMoveData (&bufferPtr[offset], 
										&eprjDatumPtr->name[1], 
										numberCharacters);
										
				eprjDatumPtr->name[0] = (UInt8)numberCharacters;
				
				}	// end "if (offset != 0)"
				
					// Now get the datum type.
					
			eprjDatumPtr->type = GetShortIntValue ((char*)lBufferPtr);
			lBufferPtr += 2;
			
					// Get the count of the number of parameters. 7 is expected.
			
			numberParemeters = GetLongIntValue ((char*)lBufferPtr);
			lBufferPtr += 4;
			
			if (numberParemeters > 0)
				{	
						// Now get offset to the parameters.
						
				offset = GetLongIntValue ((char*)lBufferPtr);
				if (offset != 0)
					{
					offset -= eprjDatumOffset;
					lBufferPtr = &bufferPtr[offset];
			
							// Now get the datum parameters.
							
					eprjDatumPtr->params[0] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
							
					eprjDatumPtr->params[1] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
							
					eprjDatumPtr->params[2] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
							
					eprjDatumPtr->params[3] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
							
					eprjDatumPtr->params[4] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
							
					eprjDatumPtr->params[5] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
							
					eprjDatumPtr->params[6] = GetDoubleValue (lBufferPtr);
					lBufferPtr += 8;
					
					}	// end "if (offset != 0)"
					
				}	// end "if (numberParemeters > 0)"
				
					// Get the grid name.
					
			numberCharacters = GetLongIntValue ((char*)lBufferPtr);
			lBufferPtr += 4;
				
			}	// end "if (errCode == noErr)"
		
		else	// errCode != noErr)
			continueFlag = FALSE;
			
				// Dispose of the buffer. We do not need it any more.
			
		CheckAndDisposePtr (bufferPtr);
			
		}	// end "if (bufferPtr != NULL)"
		
	else	// bufferPtr == NULL	
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "ReadImageEprjDatum" 		 	 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImageEprjProParameters
//
//	Software purpose:	This routine reads an Eprj_ProParameters structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadImagineImageMapInfoForChannel in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/08/2003
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadImageEprjProParameters (
				CMFileStream*						fileStreamPtr,
				UInt32								eprjProParametersOffset,
				UInt32								count,
				Eprj_ProParameters*				eprjProParametersPtr,
				Eprj_Spheroid*						eprjSpheroidPtr)

{
	UCharPtr								bufferPtr,
											endBufferPtr,
											lBufferPtr;
											
	SInt32								index,
											maxNumberProParams = 15,
											numberCharacters,
											numberParameters,
											offset = 0;
											
	UInt32								eprjSpheroidOffset;
	
	SInt16								errCode;
	
	Boolean								continueFlag = TRUE,
											continueFlag2 = FALSE,
											continueReadingInfoFlag = TRUE;
	
	
	eprjProParametersPtr->proType = 0;
	eprjProParametersPtr->proNumber = 0;
	eprjProParametersPtr->proExeName[0] = 0;
	eprjProParametersPtr->proName[0] = 0;
	eprjProParametersPtr->proZone = 0;
	
	for (index=0; index<maxNumberProParams; index++)
		eprjProParametersPtr->proParams[index] = 0.;
	
	if (count < 42)	
		{
		continueReadingInfoFlag = FALSE;
		bufferPtr = NULL;		
		
		}	// end "if (count < 42)"
		
	else	// count > 42
		bufferPtr = (UCharPtr)MNewPointer (count);
	
	if (bufferPtr != NULL)
		{
		endBufferPtr = bufferPtr + count;
		
		errCode = MSetMarker (fileStreamPtr, 
										fsFromStart, 
										eprjProParametersOffset,
										kErrorMessages);					

		if (errCode == noErr)
			errCode = MReadData (fileStreamPtr, 
											&count, 
											bufferPtr,
											kErrorMessages);
				
		if (errCode == noErr)
			{	
					// Get the projection type
			
			lBufferPtr = bufferPtr;		
			eprjProParametersPtr->proType = GetShortIntValue ((char*)lBufferPtr);
			lBufferPtr += 2;
			
			eprjProParametersPtr->proNumber = GetLongIntValue ((char*)lBufferPtr);
			lBufferPtr += 4;
			
					// Skip the executable name (char p).
					// The first 4 bytes represent the number of characters in the 
					// 	executable name.
					// After the count of characters will be the offset where the characters
					// 	are actually stored.  for all cases that I have found so far this
					// 	starts right after the offset value.
					// Take into account the fact that this buffer does not start from the 
					// 	beginning of the file.
					// Note: 4/1/2005 found case where needed to check for bad data to be
					//		sure that references to data are kept within bounds.
					
			numberCharacters = GetLongIntValue ((char*)lBufferPtr);
			lBufferPtr += 4;
			
					// Now get offset to the characters.
			
			offset = GetLongIntValue ((char*)lBufferPtr);
			lBufferPtr += 4;
			
			if (numberCharacters != 0)
				{
				if (numberCharacters > 0)
					{
							// Force offset to represent offset from start of data read into
							// the buffer not from the start of the file.
							
					offset -= eprjProParametersOffset;
					
					if (offset >= 6 && offset+numberCharacters < (SInt32)count)
						{
						lBufferPtr = &bufferPtr[offset];
						lBufferPtr += numberCharacters;
						
						}	// end "if (offset >= 6 && offset..."
					
					else	// offset < 0 || offset > count
						continueReadingInfoFlag = FALSE;
				
					}	// end "if (numberCharacters > 0)"
					
				else	// numberCharacters < 0
					continueReadingInfoFlag = FALSE;
				
				}	// end "if (numberCharacters != 0)"
				
			if (lBufferPtr > endBufferPtr - 8)
				continueReadingInfoFlag = FALSE;
			
			if (continueReadingInfoFlag)
				{
						// Load the projection name (char p).
						// See notes above for executable name.
						
				numberCharacters = GetLongIntValue ((char*)lBufferPtr);
				lBufferPtr += 4;
			
						// Now get offset to the characters.
			
				offset = GetLongIntValue ((char*)lBufferPtr);
				lBufferPtr += 4;
						
				if (numberCharacters != 0)
					{
					if (numberCharacters > 0)
						{
								// Force offset to represent offset from start of data read into
								// the buffer not from the start of the file.
							
						offset -= eprjProParametersOffset;
					
						if (offset >= 6 && offset+numberCharacters < (SInt32)count)
							{
							lBufferPtr = &bufferPtr[offset];
							lBufferPtr += numberCharacters;
							
							numberCharacters = MIN (numberCharacters, 63);
							BlockMoveData (&bufferPtr[offset], 
												&eprjProParametersPtr->proName[1], 
												numberCharacters);
												
							eprjProParametersPtr->proName[0] = (UInt8)numberCharacters;
							
							}	// end "if (offset >= 6 && offset+numberCharacters < count)"
						
						else	// offset < 0 || offset > count
							continueReadingInfoFlag = FALSE;
							
						}	// end "if (numberCharacters > 0)"
						
					else	// numberCharacters < 0
						continueReadingInfoFlag = FALSE;
					
					}	// end "if (offset != NULL)"
					
				}	// end "if (continueReadingInfoFlag)"
				
			if (lBufferPtr > endBufferPtr - 4)
				continueReadingInfoFlag = FALSE;
			
			if (continueReadingInfoFlag)
				{
						// Get the projection zone if any.
						
				eprjProParametersPtr->proZone = GetLongIntValue ((char*)lBufferPtr);
				lBufferPtr += 4;
				
				}	// end "if (continueReadingInfoFlag)"
				
			if (lBufferPtr > endBufferPtr - 8)
				continueReadingInfoFlag = FALSE;
			
			if (continueReadingInfoFlag)
				{
						// Get the projection parameters.
						// First will be the number of parameters.
						
				numberParameters = GetLongIntValue ((char*)lBufferPtr);
				lBufferPtr += 4;
				
							// Get the offset where the parameters start.
							
				offset = GetLongIntValue ((char*)lBufferPtr);
				lBufferPtr += 4;
				
				if (numberParameters != 0)
					{
					if (numberParameters > 0)
						{
						offset -= eprjProParametersOffset;
							
						if (offset >= 6 && offset+numberParameters*8 < (SInt32)count)
							{
							lBufferPtr = &bufferPtr[offset];
							
									// Read the projection parameters up to the maximum of
									// 'maxNumberProParams'.
							
							maxNumberProParams = MIN (maxNumberProParams, numberParameters);
							for (index=0; index<maxNumberProParams; index++)
								{
								eprjProParametersPtr->proParams[index] = 
																			GetDoubleValue (lBufferPtr);
								lBufferPtr += 8;
								
								}	// end "for (index=0; index<maxNumberProParams; index++)"
								
							for (index=maxNumberProParams; index<numberParameters; index++)
								lBufferPtr += 8;
								
							}	// end "if (offset >= 0 && offset..."
						
						else	// offset < 0 || ...
							continueReadingInfoFlag = FALSE;
						
						}	// end "if (numberParameters > 0)"
						
					else	// numberParameters < 0
						continueReadingInfoFlag = FALSE;
					
					}	// end "if (numberParameters != 0)"
					
				}	// end "if (continueReadingInfoFlag)"
				
			if (lBufferPtr > endBufferPtr - 8)
				continueReadingInfoFlag = FALSE;
			
			if (continueReadingInfoFlag)
				{
						// Get the number of spheroid structures.
						
				numberParameters = GetLongIntValue ((char*)lBufferPtr);
				lBufferPtr += 4;
				
						// Get the spheroid structure pointer
			
				eprjSpheroidOffset = GetLongIntValue ((char*)lBufferPtr);
				
				if (numberParameters > 0)
					{
							// Note that will only read one spheroid even if number of
							// parameters is more than 1.
							
					offset = eprjSpheroidOffset - eprjProParametersOffset;
				
					if (offset >= 6 && offset+40 < (SInt32)count)
						continueFlag2 = ReadImageSpheroid (eprjSpheroidOffset,
																		count,
																		&bufferPtr[offset],
																		eprjSpheroidPtr,
																		endBufferPtr);
																		
					}	// end "if (numberParameters > 0)"
					
				}	// end "if (continueReadingInfoFlag)"
				
			}	// end "if (errCode == noErr)"
		
		else	// errCode != noErr)
			continueFlag = FALSE;
			
				// Dispose of the buffer. We do not need it any more.
			
		CheckAndDisposePtr (bufferPtr);
			
		}	// end "if (bufferPtr != NULL)"
		
	else	// bufferPtr == NULL	
		{
		if (continueReadingInfoFlag)
			continueFlag = FALSE;
		
		}	// end "else bufferPtr == NULL"
		
	return (continueFlag);
	
}	// end "ReadImageEprjProParameters" 		 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadImageSpheroid
//
//	Software purpose:	This routine reads an Eprj_Spheroid structure from the 
//							input offset location within the file and takes care of any 
//							needed byte swapping for the entries.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			LoadImagineImageMapInfoForChannel in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/17/2003
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

Boolean ReadImageSpheroid (
				UInt32								eprjSpheroidOffset,
				UInt32								count,
				UCharPtr								bufferPtr,
				Eprj_Spheroid*						eprjSpheroidPtr,
				UCharPtr								endBufferPtr)

{
	UCharPtr								lBufferPtr;
											
	SInt32								numberCharacters,
											offset = 0;
	
	Boolean								continueReadingInfoFlag = TRUE;
	
	
	eprjSpheroidPtr->sphereName[0] = 0;
	eprjSpheroidPtr->a = 0.;
	eprjSpheroidPtr->b = 0.;
	eprjSpheroidPtr->eSquared = 0.;
	eprjSpheroidPtr->radius = 0.;
	
			// Get the spheroid name
	
	lBufferPtr = bufferPtr;
	numberCharacters = GetLongIntValue ((char*)lBufferPtr);
	lBufferPtr += 4;
	
			// The following represents the offset into the file where
			// the spheroid string name starts. Take into account
			// the fact that this buffer does not start from the beginning
			// of the file.
			
	offset = GetLongIntValue ((char*)lBufferPtr);
	lBufferPtr += 4;
	
	if (numberCharacters != 0)
		{
		if (numberCharacters > 0)	
			{
					// Force offset to represent offset from start of data read into
					// the buffer not from the start of the file.
					
			offset -= eprjSpheroidOffset;
					
			if (offset >= 8 && offset+numberCharacters < (SInt32)count)
				{
				lBufferPtr = &bufferPtr[offset];
				lBufferPtr += numberCharacters;
				numberCharacters = MIN (numberCharacters, 63);
				BlockMoveData (&bufferPtr[offset], 
									&eprjSpheroidPtr->sphereName[1], 
									numberCharacters);
									
				eprjSpheroidPtr->sphereName[0] = (UInt8)numberCharacters;
				
			
				}	// end "if (offset >= 8 && offset+numberCharacters < count)"
								
			}	// end "if (numberCharacters > 0)"
					
		else	// numberCharacters < 0
			continueReadingInfoFlag = FALSE;
		
		}	// end "if (numberCharacters != 0)"
				
	if (lBufferPtr > endBufferPtr - 32)
		continueReadingInfoFlag = FALSE;
		
	if (continueReadingInfoFlag)
		{
				// Get spheroid major axis.
					
		eprjSpheroidPtr->a = GetDoubleValue (lBufferPtr);
		lBufferPtr += 8;	
		
				// Get spheroid minor axis.
					
		eprjSpheroidPtr->b = GetDoubleValue (lBufferPtr);
		lBufferPtr += 8;
		
		eprjSpheroidPtr->eSquared = GetDoubleValue (lBufferPtr);
		lBufferPtr += 8;
		
		eprjSpheroidPtr->radius = GetDoubleValue (lBufferPtr);
			
		}	// end "if (continueReadingInfoFlag)"
		
	return (continueReadingInfoFlag);
	
}	// end "ReadImageSpheroid" 	

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2019)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReadImagineNumberOfClasses
//
//	Software purpose:	This routine gets the number of classes in the imagine 
//							thematic layer. It will only look for the number of classes
//							in the first imagine layer.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadImagineImageStatistics in SImagine.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1999
//	Revised By:			Larry L. Biehl			Date: 04/17/2006

void ReadImagineNumberOfClasses (
				FileInfoPtr							fileInfoPtr,
				Ehfa_Entry*							ehfaEntryLayerPtr)

{
	Edsc_Column							edscColumnClassName;
	
	Ehfa_Entry							ehfaEntryEdscBinFunction,
											ehfaEntryEdscColumnClassName;
	
	CMFileStream*						fileStreamPtr;
	
	Boolean								continueFlag = FALSE;
	
	
			// Initialize local variables.
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
			// Set the number of classes to the number of bins to start with.
					
	fileInfoPtr->numberClasses = fileInfoPtr->numberBins;
	
	if (fileStreamPtr != NULL)	
		{	
		continueFlag = GetEdscColumnNode (fileStreamPtr,
														ehfaEntryLayerPtr,
														&ehfaEntryEdscColumnClassName,
														&ehfaEntryEdscBinFunction,
														(char*)"Class_Names");
							
		if (continueFlag)
			continueFlag = ReadImagineEdscColumn (fileStreamPtr,
															ehfaEntryEdscColumnClassName.data,
															ehfaEntryEdscColumnClassName.dataSize,
															&edscColumnClassName);
								
		if (continueFlag)	
					// Set the number of classes.
			fileInfoPtr->numberClasses = MIN (
							edscColumnClassName.numRows, (SInt32)fileInfoPtr->numberBins);
				
		}	// end "if (fileStreamPtr != NULL)"
	
	if (fileInfoPtr->numberClasses > 0)	
		fileInfoPtr->maxClassNumberValue = fileInfoPtr->numberClasses - 1;
	
}	// end "ReadImagineNumberOfClasses"
