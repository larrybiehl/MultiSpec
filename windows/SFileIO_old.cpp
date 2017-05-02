//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2003)
//							c Purdue Research Foundation
//									All rights reserved.
//
//					The following was put here for easy access to a template
//  				DebugStr ("\pSFileIO.cpp");
//
//	File:						SFileIO.cpp
//
//	Functions in file:	SInt32	 			ConvertATRealtoInt
//								double 				ConvertATRealtoReal
//								long int 			ConvertRealAT
//								Boolean 				GetBILSpecial
//								SInt32	 			GetLongIntValue
//								SInt16	 			GetShortIntValue
//								Boolean 				GetTRLSupportFile
//								SInt16	 			GetVolumeFreeSpace
//								void 					PackBlockedData
//								UInt32 				PackMaskData
//								void 					SetVolumeReference
//
//	Brief description:	The purpose of the routines in this file is to
//								provide utility type functions in MultiSpec.
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			01/31/2003 

#include	"SMulSpec.h" 

#if !defined _X86_
	#define kOpenPrefKey						1
							
	typedef struct NavUserData
		{
		FileInfoPtr							fileInfoPtr;
		Handle								fileInfoHandle;
		NavMenuItemSpecArrayHandle		navMenuItemSpecHandle;
		SInt32								filterSelection;
		SInt32								initialMenuSelection;
		SInt32								stringIndex;
		
		}	NavUserData, *NavUserDataPtr;
#endif	// !defined _X86_ 

  
#if defined _X86_
	#include "CWinInfo.h"
	#include "CFileStr.h"
//	#include "CFileDlg.h"
	#include "WFileDlg.h"  
	#include <direct.h>
	                                                                   
	#ifndef _WIN32           
		#include <dos.h>  
	#endif	// !WIN32 
//new
	#include <afxdlgs.h>
//end new

#endif	// defined _X86_    

#include "SExtGlob.h"	

extern Handle 			gCustomNavOpenList;
extern Handle 			gCustomNavPut;

//extern OSErr 				OpenOrPrintSelectedFiles (
//									AEDescList*							docListPtr, 
//									Boolean								printFlag);



			// Prototype descriptions for routines in this file that are only		
			// called by routines in this file.

SInt16 				CheckIfSTAFile (
							CMFileStream*		 				fileStreamPtr);

SInt16 				CheckIfTRLFile (
							CMFileStream*		 				fileStreamPtr);

void 					CloseUpHeirarchalFileIOParameters (
							LayerInfoPtr						layerInfoPtr,
							FileInfoPtr							fileInfoPtr, 
							UInt16*								channelListPtr,
							UInt16								numberChannels);
							
SInt16 				GetTRLSupportFile (
							FileInfoPtr							gisFileInfoPtr,
							CMFileStream*						trailerStreamPtr,
							Boolean								promptFlag);
			
#if !defined _X86_				
pascal Boolean 	NavServicesFilterProc( 
							AEDesc* 								theItem, 
							void* 								info, 
							NavCallBackUserData 				callBackUD, 
							NavFilterModes 					filterMode);
							
pascal void 		NavServicesGetEventProc ( 
							NavEventCallbackMessage 		callBackSelector, 
							NavCBRecPtr 						callBackParms, 
							NavCallBackUserData 				callBackUD);

SInt16 				NavServicesGetFile (
							CMFileStream* 						fileStreamPtr, 
							SInt16								numberTypes, 
							OSType*								fileTypesPtr, 
							LocalAppFile*						localAppFilePtr, 
							SInt16								stringIndex);
							
pascal void 		NavServicesPutEventProc ( 
							NavEventCallbackMessage 		callBackSelector, 
							NavCBRecPtr 						callBackParms, 
							NavCallBackUserData 				callBackUD);
							
SInt16 				NavServicesPutFile (
							CMFileStream*		 				fileStreamPtr, 
							SInt32								numberBytes, 
							SInt16								stringIndex, 
							SInt32								creator);
#endif	// !defined _X86_ 
													
void 					PackBISData (
							UInt32								columnInterval, 
							UInt32								numberBufferChannels, 
							UInt32								numberChannels, 
							UInt32								numberSamples, 
							UInt32								numberInBytes, 
							HUCharPtr 							ioCharBufferPtr,
							HUCharPtr 							dataCharBufferPtr, 
							UInt16*								channelListPtr, 
							UInt32								numberOutBytes,
							Boolean								signedInDataFlag);
							
UInt32 				PackMaskData (
							HUInt16Ptr							maskBufferPtr,
							UInt32								maskColumnStart,
							UInt32								numberChannels,
							UInt32								numberInputSamples,
							UInt32								columnInterval,
							HUCharPtr							dataBufferPtr,
							UInt32								numberOutBytes,
							SInt32								maskValueRequest);
							
void 					PackNonBISData (
							UInt32								columnOffset,
							UInt32								columnInterval,
							UInt32								numberColumnsPerChannel,  
							UInt32								fileNumberChannels,
							UInt32								numberChannels,
							HUInt16Ptr							channelListPtr, 
							UInt32								numberSamples,
							UInt32								numberInBytes, 
							HUCharPtr							ioCharBufferPtr, 
							HUCharPtr							dataCharBufferPtr, 
							Boolean								toBISFormat, 
							UInt32								numberOutBytes,
							Boolean								swapBytesFlag,
							Boolean								signedInDataFlag);

void		 			PackGAIAData (
							SInt32								columnStart, 
							SInt32								columnEnd, 
							HSInt16Ptr							inputBufferPtr);

void 					PackBlockedData (
							UInt32								line,
							UInt32								columnStart, 
							UInt32								columnEnd,
							UInt32								channelStartIndex,
							FileInfoPtr							fileInfoPtr,
							HierarchalFileFormatPtr			hfaPtr,
							HUCharPtr							outputBufferPtr);

void 					SetUpHeirarchalFileIOParameters (
							LayerInfoPtr						layerInfoPtr,
							FileInfoPtr							fileInfoPtr,
							UInt16								numberChannels, 
							UInt16*								channelListPtr,
							UInt32								columnStart, 
							UInt32								columnEnd);
								
void 					SwapBytes (
							HUInt16Ptr							twoBytePtr,
							UInt32								numberSamples);
	
	
							
// === Static Member Variable ===

static UInt16				sCustomControlCount = 0;



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckIfSTAFile
//
//	Software purpose:	The purpose of this routine is to determine if the input file
//							is an ERDAS image statistics file.
//		
//	Parameters in:		File stream pointer.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/04/2001
//	Revised By:			Larry L. Biehl			Date: 10/04/2001

SInt16 CheckIfSTAFile (
				CMFileStream*		 				fileStreamPtr)

{
	Str15									tempString;
	
	char									*erdasStatHeader = "TRAIL";
	StringPtr							filePathPtr;		
	
	UInt32								count;
	
	SInt16								errCode,
											returnCode = 0;
											
	UInt16								numSuffixChars;
	
					
	count = 5;
	errCode = MReadData(fileStreamPtr, 
							&count, 
							&tempString,
							kNoErrorMessages);
				
	if (errCode == noErr)
		{  
		if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)
			{
			filePathPtr = GetFilePathPPointer (fileStreamPtr);
	
			if (!CompareSuffixNoCase ( (char*)"\0.trl", 
													filePathPtr,
													&numSuffixChars) )
				returnCode = 1;
				
			}		// end "if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)"
			
		}		// end "if (errCode == noErr)"
		
	return (returnCode);
			
}		// end "CheckIfSTAFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CheckIfTRLFile
//
//	Software purpose:	The purpose of this routine is to determine if the input file
//							is an ERDAS class information file.
//		
//	Parameters in:		File stream pointer.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/04/2001
//	Revised By:			Larry L. Biehl			Date: 10/04/2001

SInt16 CheckIfTRLFile (
				CMFileStream*		 				fileStreamPtr)

{
	Str15									tempString;
	
	char									*erdasStatHeader = "TRAIL";
	StringPtr							filePathPtr;		
	
	UInt32								count;
	
	SInt16								errCode,
											returnCode = 0;
											
	UInt16								numSuffixChars;
	
					
	count = 5;
	errCode = MReadData(fileStreamPtr, 
							&count, 
							&tempString,
							kNoErrorMessages);
				
	if (errCode == noErr)
		{  
		if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)
			{
			filePathPtr = GetFilePathPPointer (fileStreamPtr);
	
			if (!CompareSuffixNoCase ( (char*)"\0.sta", 
													filePathPtr,
													&numSuffixChars) )
				returnCode = 1;
				
			}		// end "if (strncmp ((char*)&tempString, erdasStatHeader, 5) == 0)"
			
		}		// end "if (errCode == noErr)"
		
	return (returnCode);
			
}		// end "CheckIfTRLFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CloseFile
//
//	Software purpose:	The purpose of this routine is to close and unlock
//							the file that is described by fileInfo.
//		
//	Parameters in:		Address to MFileInfo structure for file to be closed.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1988
//	Revised By:			Larry L. Biehl			Date: 10/23/1995

void CloseFile (
				FileInfoPtr							fileInfoPtr)

{
				// Check input variables.														
				
	if (fileInfoPtr != NULL)
		{
		CMFileStream* fileStreamPtr = GetFileStreamPointer(fileInfoPtr);
			
		CloseFile (fileStreamPtr);
		
		}		// end "if (fileInfoPtr != NULL)"
	
}		// end "CloseFile" 



void CloseFile (
				CMFileStream*						fileStreamPtr)

{
				// Check input variables.														
				
	if (fileStreamPtr != NULL)
		{
		#if !defined _X86_
		
			SInt16				errCode;
			
		
			if (fileStreamPtr->fileName[0] != 0 && fileStreamPtr->refNum != 0)
				{
				errCode = FSClose (fileStreamPtr->refNum);
					      			
				if (errCode == noErr)
					{
					#if !TARGET_API_MAC_CARBON	
								// I get an error message under OSX when calling
								// FSpRstFLock.
						if (fileStreamPtr->fSSpecFlag)
							errCode = FSpRstFLock ( (FSSpec*)fileStreamPtr );
							
						else		// !fileStreamPtr->fSSpecFlag 
							errCode = RstFLock (fileStreamPtr->fileName, fileStreamPtr->vRefNum);
					#endif	// !TARGET_API_MAC_CARBON	
					
							// If the file is not found (the user must have moved		
							// the file) do not worry about the error.					
							
					if (errCode == fnfErr)
						errCode = noErr;
						
					}		// end "if (errCode == noErr)"
				
				if (errCode == noErr)
					errCode = FlushVol (NIL, fileStreamPtr->vRefNum);
					
				if (errCode != wPrErr && errCode != afpAccessDenied)
					IOCheck (errCode, fileStreamPtr);
			  		
				fileStreamPtr->refNum = 0;
				
				}		// end "if (fileStreamPtr->fileName[0] != 0 && ..."

		#endif	// !defined _X86_
			 
			
		#if defined _X86_
			
			fileStreamPtr->MCloseFile(); 

		#endif	// defined _X86_  
			
		}		// end "if (fileStreamPtr != NULL)" 
	
}		// end "CloseFile" 



void CloseFile (
				Handle								fileStreamHandle)

{
	CMFileStream*						fileStreamPtr;
	
	SignedByte							handleStatus;
	
	
				// Check input variables.														
				
	fileStreamPtr = (CMFileStream*)GetHandleStatusAndPointer (fileStreamHandle,
																					&handleStatus,
																					kNoMoveHi);
																	
	CloseFile (fileStreamPtr); 
	
	MHSetState (fileStreamHandle, handleStatus);
	
}		// end "CloseFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 CloseUpFileIOInstructions
//
//	Software purpose:	This routine closes up parameters in the file IO instruction
//							structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/22/1998
//	Revised By:			Larry L. Biehl			Date: 10/25/1999

void CloseUpFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				AreaDescription* 					areaDescriptionPtr)

{
	
			// Unlock the mask buffer handle if needed.
	
	if (fileIOInstructionsPtr->maskBufferPtr != NULL && areaDescriptionPtr != NULL)
		{
		CheckAndUnlockHandle (areaDescriptionPtr->maskInfoPtr->maskHandle);
		fileIOInstructionsPtr->maskBufferPtr = NULL;
		
		}		// end "if (fileIOInstructionsPtr->maskBufferPtr != NULL && ..."
	
			// Force flag for Threaded IO to be FALSE.
			
	gUseThreadedIOFlag = FALSE;	

	EndFileIOThread (fileIOInstructionsPtr);		 	 
	
}		// end "CloseUpFileIOInstructions"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpGeneralFileIOInstructions
//
//	Software purpose:	This routine closes the general parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

void CloseUpGeneralFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr)
							
{	
	if (fileIOInstructionsPtr != NULL)
		{
		CloseUpHeirarchalFileIOParameters ( fileIOInstructionsPtr->layerInfoPtr,
															fileIOInstructionsPtr->fileInfoPtr, 
															fileIOInstructionsPtr->channelListPtr,
															(UInt16)fileIOInstructionsPtr->numberChannels);
					
		UnlockHierarchalFileHandles (fileIOInstructionsPtr->windowInfoPtr, 
													fileIOInstructionsPtr->fileInfoPtr);
		
		fileIOInstructionsPtr->windowInfoPtr = NULL;
		fileIOInstructionsPtr->fileInfoPtr = NULL;
		fileIOInstructionsPtr->layerInfoPtr = NULL;
		fileIOInstructionsPtr->channelListPtr = NULL;
		fileIOInstructionsPtr->inputBufferPtrs[0] = NULL;
		fileIOInstructionsPtr->inputBufferPtrs[1] = NULL;
									
		fileIOInstructionsPtr->outputBufferPtrs[0] = NULL;
		fileIOInstructionsPtr->outputBufferPtrs[1] = NULL;
		
		fileIOInstructionsPtr->tiledBufferPtrs[0] = NULL;
		fileIOInstructionsPtr->tiledBufferPtrs[1] = NULL;
		
		fileIOInstructionsPtr->bufferOffset = 0;
		
		fileIOInstructionsPtr->numberChannels = 0;
		
		fileIOInstructionsPtr->forceByteCode = kDoNotForceBytes;
		fileIOInstructionsPtr->forceBISFormatFlag = FALSE;
		fileIOInstructionsPtr->packDataFlag = FALSE;
		
		}		// end "if (fileIOInstructionsPtr != NULL)"
	
}		// end "CloseUpGeneralFileIOInstructions" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseUpHeirarchalFileIOParameters
//
//	Software purpose:	This routine closes the buffer pointers in the HierarchalFileFormat
//							structures in the input fileInfo structure.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/20/1999
//	Revised By:			Larry L. Biehl			Date: 10/26/1999	

void CloseUpHeirarchalFileIOParameters (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				UInt16*								channelListPtr,
				UInt16								numberChannels)

{
	FileInfoPtr							localFileInfoPtr;
	
	HierarchalFileFormatPtr			hfaPtr,
											localHfaPtr;
	
	SInt32								fileInfoIndex,
											nextFileInfoIndex;
	
	UInt32								channelNumber,
											fileChannelNumber,
											index;
											
	Boolean								bandInterleaveByBlockFlag;
											
	
	if (fileInfoPtr != NULL)
		{
		fileChannelNumber = 0;
		fileInfoIndex = -1;
		
		for (index=1; index<=numberChannels; index++)
			{	
			channelNumber = index;								
			if (channelListPtr != NULL)
				channelNumber = channelListPtr[index-1] + 1;
			
			nextFileInfoIndex = 0;
			fileChannelNumber++;
			if (layerInfoPtr != NULL)
				{
				nextFileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
				fileChannelNumber = layerInfoPtr[channelNumber].fileChannelNumber;
				
				}		// end "if (layerInfoPtr != NULL)"
				
			if (fileInfoIndex != nextFileInfoIndex)
				{
				fileInfoIndex = nextFileInfoIndex;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
					
						// Get pointer to the hfa structure.
						
				hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer(
														localFileInfoPtr->hfaHandle,
														kNoLock,
														kNoMoveHi);
														
				bandInterleaveByBlockFlag = FALSE;
				
				}		// end "if (fileInfoIndex != nextFileInfoIndex)"
			
			if (hfaPtr != NULL)
				{
				if (localFileInfoPtr->bandInterleave == kBIBlock)
					{
					fileChannelNumber = 1;
					bandInterleaveByBlockFlag = TRUE;
					
					}		// end "if (localFileInfoPtr->bandInterleave == kBIBlock)"
			
				localHfaPtr = &hfaPtr[fileChannelNumber-1];
				
				localHfaPtr->tiledBufferPtr = NULL;	
				
				localHfaPtr->firstLineRead = 0;
				localHfaPtr->lastLineRead = 0;	
				localHfaPtr->firstColumnStartByte = 0;
											
				localHfaPtr->firstColumnRead = 0;
				localHfaPtr->lastColumnRead = 0;
				localHfaPtr->numberBlocksRead = 0;	
				
				}		// end "if (hfaPtr != NULL)"
				
			}		// end "for (index=1; index<=numberChannels; index++)" 
			
		}		// end "if (fileInfoPtr != NULL)"
	
}		// end "CloseUpHeirarchalFileIOParameters" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseWindowImageFiles
//
//	Software purpose:	This routine closes the open image files associated
//							with the input window information handle and disposes
//							of the description handle and file information handle
//							associated with each of the image files.
//
//	Parameters in:		Handle to window information
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			DeactivateProject in project.c
//							ChangeProjectBaseImage in project.c
//							CloseImageWindow in window.c
//
//	Coded By:			Larry L. Biehl			Date: 06/21/1990
//	Revised By:			Larry L. Biehl			Date: 06/04/1996

void CloseWindowImageFiles (
				Handle								windowInfoHandle)

{
	Handle								fileInfoHandle;
	SignedByte							handleStatus;
	WindowInfoPtr						windowInfoPtr;

		 
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer(
											windowInfoHandle, &handleStatus, kNoMoveHi);

	if (windowInfoPtr != NULL)
		{												
		fileInfoHandle = windowInfoPtr->fileInfoHandle;
		
		DisposeFileInfoHandle (fileInfoHandle);
		
		windowInfoPtr->fileInfoHandle = NULL;
		windowInfoPtr->numberImageFiles = 0;
			
		}		// end "if (windowInfoPtr != NULL)"
		
	MHSetState (windowInfoHandle, handleStatus); 
			
}		// end "CloseWindowImageFiles" 
                    


//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		long int ConvertATRealtoInt
//
//	Software purpose:	This routine converts an IBM PC-AT real number
//							to a Motorola integer number.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			CheckImageHeader
//
//	Coded By:			Larry L. Biehl			Date: 08/23/1988
//	Revised By:			Larry L. Biehl			Date: 04/17/1995

SInt32 ConvertATRealtoInt (
				CharPtr								ATrealPtr)

{
	SInt32								intValue;
	
	
	intValue = (SInt32)( ConvertATRealtoReal (ATrealPtr) + 0.5 );
	
	return (intValue);

}		// end "ConvertATRealtoInt" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double ConvertATRealtoReal
//
//	Software purpose:	This routine converts an IBM PC-AT real number
//							to a Motorola real number (ThinkC double).
//
//							04/17/2000: Allowed for negative values
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
//
// Called By:			ReadERDAS_staFile
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1988
//	Revised By:			Larry L. Biehl			Date: 04/17/2000

double ConvertATRealtoReal (
				CharPtr								ATrealPtr)

{
	double								MOTrealNumber,
											onePlusFraction;
	
	SInt16								actualExponent;
	
	SInt32								exponent,
											fraction,
											sign,
											temp,
											workNumber;
	
	
	if (gSwapBytesFlag)
		{		
		memcpy ( (Ptr)&workNumber, (Ptr)ATrealPtr, 4);
		
				// Set Motorola real number to -1.  If the number is not changed		
				// then a -1 indicates an error.  This assumes that there will be		
				// no TRUE -1's for this application.											
				
		MOTrealNumber = -1;
		
		sign = (workNumber & 0x00000080);
//		if (sign == 0)
//			{
			exponent	= 0;
			exponent = (workNumber & 0x0000007f) << 1;
			temp = workNumber & 0x00008000;
			exponent = exponent | (temp>>15);
			
			if (exponent > 0 && exponent < 255)
				actualExponent = (SInt16)(exponent - 127);
				
			else		// exponent == 0 || exponent == 255
				actualExponent = (SInt16)exponent;
		
					// Make certain that sign does not get shifted right.						
					
			fraction = 0;
			temp = workNumber & 0xff000000;
			fraction = fraction | ((temp>>24) & 0x000000ff);
		
			temp = workNumber & 0x00ff0000;
			fraction = fraction | (temp>>8);
		
			temp = workNumber & 0x00007f00;
			fraction = fraction | (temp<<8);
		
			if (exponent == 0 && fraction == 0)  
				MOTrealNumber = 0;
			
			else		// exponent != 0 || fraction != 0  
				{
				onePlusFraction = 1. + (double)fraction/8388608.;
				MOTrealNumber = ldexp (onePlusFraction, actualExponent);
				
				if (sign != 0)
					MOTrealNumber = - MOTrealNumber;
	
				}		// end "else exponent != 0 || fraction != 0" 
				
//			}		// end "if (sign == 0)" 
		
		return (MOTrealNumber);
		
		}		// end "if (gSwapBytesFlag)"
		
	else		// !gSwapBytesFlag
		{
		float		floatValue; 
		
		
		memcpy ( (Ptr)&floatValue, (Ptr)ATrealPtr, 4);
		return (floatValue);
		
		}		// end "else !gSwapBytesFlag"

}		// end "ConvertATRealtoReal"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 ConvertRealAT
//
//	Software purpose:	This routine converts an real number in Motorola
//							real (double format) to  PC-AT real number format.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadErdasHeader in SFileIO.cpp
//							CreateSTASupportFile in SHistgrm.cpp						
//										
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1988
//	Revised By:			Larry L. Biehl			Date: 01/19/2001

SInt32 ConvertRealAT (
				double								realValue)

{
	double								mantissa;
	
	int									actualExponent;
	
	SInt32								exponent,
											fraction,
											temp,
											workNumber;
											
	Boolean								negativeFlag;
	
	
	if (gSwapBytesFlag)
		{
				// Determine whether real value is negative or not.
		
		negativeFlag = FALSE;
		if (realValue < 0)
			negativeFlag = TRUE;
		
				// Get mantissa and exponent for base 2 number								
		
		realValue = fabs(realValue);		
		mantissa = frexp (realValue, &actualExponent);
		
				// Convert mantissa from range of 0.5 - 1.0 to 1.0 - 2.0.  				
				// Don't forget to adjust the exponent accordingly							
				//	Also work with only the fractional part of the mantissa.	
				// 	Move to before conversion of exponent on 11/28/2000 by Larry Biehl.
				//		Ran into problem with a number like 0.6 with it the other way.			
		
		if (mantissa > 0)
			{		
			mantissa *= 2;
			mantissa = mantissa - 1;
			actualExponent--;
		
					// Convert exponent to biased exponent between 0 and 255.				
					// If the actual exponent is 0, the biased exponent is also 0.			
					
			if (actualExponent >= -126 && actualExponent <= 127)
				exponent = actualExponent + 127;
				
			else if (actualExponent < -126)	// actualExponent < -126
				exponent = 0;
				
			else // actualExponent > 127
				exponent = 255;
			
			}		// end "if (mantissa > 0)"
			
		else		// mantissa == 0
			exponent = 0;
		
				// Get the fraction of the mantissa to 23 bits								
		
		fraction = (long int)(mantissa*8388608. + (double)0.5);
		
				// Load in the sign flag															
				
		workNumber = 0;
		if (negativeFlag)  
			workNumber = workNumber | 0x00000080;
		
				// Load the exponent in																
				
		temp = (exponent & 0x000000fe) >> 1;
		workNumber = workNumber | temp;
		
		temp = (exponent & 0x00000001) << 15;
		workNumber = workNumber | temp;
		
				// Load the fraction in																
				
		temp = (fraction & 0x000000ff) << 24;
		workNumber = workNumber | temp;
		
		temp = (fraction & 0x0000ff00) << 8;
		workNumber = workNumber | temp;
		
		temp = (fraction & 0x007f0000) >> 8;
		workNumber = workNumber | temp;
		
		}		// end "gSwapBytesFlag"
		
	else		// !gSwapBytesFlag
		{
		float floatValue = (float)realValue;
		memcpy ( (Ptr)&workNumber, (Ptr)&floatValue, 4);
		
		}		// end "else !gSwapBytesFlag"
	
			// Return the converted number.  It is now in PC-AT floating point	
			// format (Real)																		
			
	return (workNumber);

}		// end "ConvertRealAT"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyFileStream
//
//	Software purpose:	The purpose of this routine is to intialize a new file stream
//							structure using the information from another input file
//							stream structure. The new file stream structure is left
//							with no file being opened even if the input file stream
//							represents a file that is opened.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/30/1997
//	Revised By:			Larry L. Biehl			Date: 12/21/1998	

void CopyFileStream (
				CMFileStream*						newFileStreamPtr,
				CMFileStream*						oldFileStreamPtr)

{  		
	if (newFileStreamPtr != NULL && oldFileStreamPtr != NULL)
		{   
				// Now copy the information needed so that the new file stream 
				// represents an opened file in the same state as that in the
				// input file stream.
				                      
		#if !defined _X86_                                                         		
				
			newFileStreamPtr->vRefNum = oldFileStreamPtr->vRefNum;
			newFileStreamPtr->parID = oldFileStreamPtr->parID;
			CopyPToP ( (char*)newFileStreamPtr->fileName, (char*)oldFileStreamPtr->fileName );
			newFileStreamPtr->creator = oldFileStreamPtr->creator;
			newFileStreamPtr->type = oldFileStreamPtr->type;    
			newFileStreamPtr->fSSpecFlag = oldFileStreamPtr->fSSpecFlag;
			                         
			newFileStreamPtr->refNum = oldFileStreamPtr->refNum;
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_	
         
         StringPtr	oldFilePathPtr = oldFileStreamPtr->GetFilePathPtr();              
			newFileStreamPtr->SetFilePath (oldFilePathPtr, FALSE);
			newFileStreamPtr->mCreator = oldFileStreamPtr->mCreator;
			newFileStreamPtr->mFileType = oldFileStreamPtr->mFileType; 
			
         newFileStreamPtr->m_hFile = oldFileStreamPtr->m_hFile;
		
		#endif	// defined _X86_ 
		
		}		// end "if (newFileStreamPtr != NULL && ..."

}		// end "CopyFileStream" 

 
		
Boolean CopyFileStream (
				FileInfoPtr							inputFileInfoPtr,
				Handle*								outputFileStreamHandlePtr,
				CMFileStream**						outputFileStreamPtrPtr) 

{  
	CMFileStream						*inputFileStreamPtr,
											*newFileStreamPtr;
					
	Boolean								returnFlag = FALSE;
	
	
	inputFileStreamPtr = GetFileStreamPointer (inputFileInfoPtr);
			
	if (inputFileStreamPtr != NULL)
		{   
				// Now copy the information needed so that the new file stream 
				// represents an opened file in the same state as that in the
				// input file stream.
				                      
		#if !defined _X86_                                                         		
				
			if (outputFileStreamHandlePtr != NULL)
				{
				if (*outputFileStreamHandlePtr == NULL)
					*outputFileStreamHandlePtr = MNewHandle( sizeof(CMFileStream) );
					
				if (*outputFileStreamHandlePtr != NULL)
					{
					newFileStreamPtr = (CMFileStream*)GetHandlePointer(
															*outputFileStreamHandlePtr,
															kLock,
															kNoMoveHi);
															
					newFileStreamPtr->vRefNum = inputFileStreamPtr->vRefNum;
					newFileStreamPtr->parID = inputFileStreamPtr->parID;
					CopyPToP ( (char*)newFileStreamPtr->fileName, 
										(char*)inputFileStreamPtr->fileName );
					newFileStreamPtr->creator = inputFileStreamPtr->creator;
					newFileStreamPtr->type = inputFileStreamPtr->type;    
					newFileStreamPtr->fSSpecFlag = inputFileStreamPtr->fSSpecFlag;
					                         
					newFileStreamPtr->refNum = inputFileStreamPtr->refNum;
					
							// Make sure that the refNum in the input file stream does
							// does not now refer to an open file. We do not want
							// two structures to refer to the same open file.
							
					inputFileStreamPtr->refNum = 0;
					
					CheckAndUnlockHandle (*outputFileStreamHandlePtr);
					
					if (outputFileStreamPtrPtr != NULL)
						*outputFileStreamPtrPtr = NULL;
		         
		         returnFlag = TRUE;
					
					}		// end "if (*outputFileStreamHandlePtr != NULL)"
				
				}		// end "if (outputFileStreamHandlePtr != NULL)"
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_	
		
			if (outputFileStreamPtrPtr != NULL)
				{
				if ( *outputFileStreamPtrPtr == NULL)
					*outputFileStreamPtrPtr = new CMFileStream;
					
				if (*outputFileStreamPtrPtr != NULL)
					{
					newFileStreamPtr = *outputFileStreamPtrPtr;
					
		         StringPtr	oldFilePathPtr = inputFileStreamPtr->GetFilePathPtr();              
					newFileStreamPtr->SetFilePath (oldFilePathPtr, FALSE);
					newFileStreamPtr->mCreator = inputFileStreamPtr->mCreator;
					newFileStreamPtr->mFileType = inputFileStreamPtr->mFileType; 
					
		         newFileStreamPtr->m_hFile = inputFileStreamPtr->m_hFile;
		         
		         returnFlag = TRUE;
		         
		      	}		// end "if (*outputFileStreamPtrPtr != NULL)"
         
         	}		// end "if (outputFileStreamPtrPtr != NULL)"
		
		#endif	// defined _X86_ 
		
		}		// end "if (inputFileStreamPtr != NULL)"
		
	return (returnFlag);

}		// end "CopyFileStream"      



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int CreateNewFile
//
//	Software purpose:	The purpose of this routine is to create a new file
//							on the specified volume and open the file.  If the
//							file already exists, clear the old file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetTRLSupportFile in SFileIO.cpp
//							PutFile in SFileIO.cpp
//							WriteArcViewBLWFile in SSaveWrt.cpp
//							WriteArcViewHeaderFile in SSaveWrt.cpp
//							FS_make_cor_mean_std in submslee.c
//							CreateResultsDiskFile in SUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/23/1990
//	Revised By:			Larry L. Biehl			Date: 08/22/2001

SInt16 CreateNewFile (
				CMFileStream* 						fileStreamPtr, 
				SInt16								vRefNum, 
				SInt32								creator,
				SInt16								messageCode,
				Boolean								replaceFlag)

{
	SInt16								errCode = noErr;
	
	
	#if !defined _X86_
	
		SInt32								parID;
	
		if (fileStreamPtr->fSSpecFlag)
			parID = fileStreamPtr->parID;
	
		else		// !fileStreamPtr->fSSpecFlag
			parID = 0;
	
				// Create the file on the selected volume
				
		errCode = HCreate (vRefNum, 
                           parID,
									fileStreamPtr->fileName, 
									(OSType)creator,
									(OSType)fileStreamPtr->type);
									
		if (errCode != dupFNErr && messageCode == kErrorMessages)
			IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
			
		if (errCode != noErr && errCode != dupFNErr)	
																				return (errCode);
																				
		if (!replaceFlag && errCode == dupFNErr)	
																				return (errCode);
		
		errCode = noErr;							
		if (!fileStreamPtr->fSSpecFlag)
			{											
			errCode = FSMakeFSSpec (vRefNum,
											0,
											fileStreamPtr->fileName,
											&gFileFSSpec);
				
			if (errCode != noErr)	
				{
				if (messageCode == kErrorMessages)
					IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
																				return (errCode);
																				
				}		// end "if (errCode != noErr)"
			
			vRefNum = gFileFSSpec.vRefNum;
			parID = gFileFSSpec.parID;
											
			}		// end "if (!fileStreamPtr->fSSpecFlag)"
		
				// Open the file ready for write mode at the beginning.					
													
		fileStreamPtr->vRefNum = vRefNum;
		fileStreamPtr->parID = parID;	
		fileStreamPtr->fSSpecFlag = TRUE;						
		errCode = SetFileSizeToZero (fileStreamPtr);
			
				// Make certain that the creator and file type are correct.				
				
		if (errCode == noErr)
			{	
			errCode = HGetFInfo (vRefNum, parID, fileStreamPtr->fileName, &gFinderInfo);
			
			if ( errCode == noErr && (gFinderInfo.fdCreator != creator || 
												gFinderInfo.fdType != fileStreamPtr->type) )
				{
				gFinderInfo.fdCreator = creator;
				gFinderInfo.fdType = fileStreamPtr->type;
			
				errCode = HSetFInfo (vRefNum, parID, fileStreamPtr->fileName, &gFinderInfo);
				
				}		// end "if (errCode == noErr)" 
			
			if (messageCode == kErrorMessages)	
				IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
			
			}		// end "if (errCode == noErr)"
			
	#endif	// !defined _X86_ 
	
			
	#if defined _X86_ 
	
		errCode = fileStreamPtr->MCreateNewFile ();
			
	#endif	// defined _X86_ 
	
	return (errCode);

}		// end "CreateNewFile"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateTRLSupportFile
//
//	Software purpose:	This routine creates a trl file. If the 
//							classNameCode is 'kFromDescriptionCode', then this
//							is called from the Reformat-Change Image File Format
//							processor. The class names and colors come from those
//							currently being used in the active image window.
//
//							AreasToThematicFileControl (paletteID)
//							CovertMultispectralToThematic (paletteHandle)
//							CreateTrailerFiles (paletteID)
//							ChangeFormatToBILorBIS (paletteHandle)
//							WriteThematicClassesAs (colorSpecPtr)
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			AreasToThematicFileControl in fieldsToThematicFile.c
//							CovertMultispectralToThematic in fieldsToThematicFile.c
//							CreateTrailerFiles in SClassfy.cpp
//							ChangeFormatToBILorBIS in SReform1.cpp
//							WriteThematicClassesAs in SSaveWrt.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 02/21/1990
//	Revised By:			Larry L. Biehl			Date: 12/14/2000	

Boolean CreateTRLSupportFile (
				FileInfoPtr							gisFileInfoPtr, 
				UInt16								numberClasses, 
				SInt16*								classPtr,
				UInt16								numberListClasses,  
				ColorSpec*							inputColorSpecPtr,
				CMPaletteInfo						paletteHandle,
				UInt16*								paletteIndexPtr,
				SInt16								paletteType,
				SInt16								paletteOffset, 
				SInt16								classNameCode, 
				SInt16								trailerCode)

{
	CMFileStream						trailerStream;
											
	EchoClassifierVarPtr				echoClassifierVarPtr;
	CMFileStream*						trailerStreamPtr;
	ColorSpec*							colorSpecPtr;
	HFileIOBufferPtr					ioBufferPtr;
	
	char									*erdasHeader = "TRAIL74";
										
	UCharPtr								classNamePtr,
											ioTempBufferPtr;
										
	SInt32								classNumber;
	
	UInt32								count,
											index,
											numberBytes;
	
	SInt16								errCode,
											numberProjectClasses,
											numberTRLClasses,
											thematicPaletteType;
										
	UInt16								classIndex,
											inputPaletteOffset,
											paletteCode,
											paletteIndex;
										
	Boolean								backgroundFlag,
											continueFlag,
											promptFlag;
	
	
	if (gisFileInfoPtr == NULL)
																				return (FALSE);
	
			// Do not create a trailer file if there are more than the class		
			// limit.																				
			
	if (numberClasses > gClassListLimit)
																				return (TRUE);
																				
			// Make certain that we have the proper information for the 			
			// palette information.																
			
	if (paletteHandle != NULL || inputColorSpecPtr != NULL)
		paletteType = 0;
		
	else		// paletteHandle == NULL && inputColorSpecPtr == NULL
		if (paletteType <= 0)
																				return (FALSE);
		
	continueFlag = TRUE;
	numberProjectClasses = 0;
	if (gProjectInfoPtr != NULL)
		numberProjectClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Determine the number of trailer classes.									
			
	numberTRLClasses = numberClasses;
	if (classPtr != NULL)
		numberTRLClasses = numberProjectClasses;
		
			// Make sure that the number of classes is not more than 256.
			// Allow for the background class if needed.
			
	if (classNameCode < kFromDescriptionCode)
		numberTRLClasses = MIN(kMaxNumberStatClasses-1, numberTRLClasses);
	numberTRLClasses = MIN(kMaxNumberStatClasses, numberTRLClasses);
	
			// Get buffer to load trailer file information into.  Try to get 		
			// buffer to hold one complete set of the 3 types of information -	
			// color scheme, class histogram, and class name. Note that even
			// though we are limited to colors for only 256 classes, we can
			// have class names for up to 'gClassListLimit' classes.					
			
	numberBytes = (((SInt32)numberClasses+4)/4) * 128;
	numberBytes = MAX ( (9*128), numberBytes);
	ioBufferPtr = (HFileIOBufferPtr)MNewPointer (numberBytes);
	continueFlag = (ioBufferPtr != NULL);
	
			// Get buffer to load color table into if needed.
			
	colorSpecPtr = inputColorSpecPtr;
	if (continueFlag && inputColorSpecPtr == NULL)
		{	
				// Get table for palette information. Allow enough for the
				// number of classes, white, black, and background. The
				// white, black and background will probably not be need for
				// the trailer file, but the routines being called will
				// fix those colors.
					
		colorSpecPtr = (ColorSpec*)MNewPointer ( 
									(SInt32)(numberTRLClasses+3)*sizeof(ColorSpec) );
		continueFlag = (colorSpecPtr != NULL);
				
		}		// end "if (continueFlag && colorSpecPtr != NULL)"
	
	if (continueFlag)
		{
		ioTempBufferPtr = (UCharPtr)ioBufferPtr;
					
				// Get the pointer to the file stream block and initialize it.			
			
		trailerStreamPtr = &trailerStream;
		InitializeFileStream (trailerStreamPtr);
		
		promptFlag = FALSE;
		if (inputColorSpecPtr != NULL)
			promptFlag = TRUE;
		
		errCode = GetTRLSupportFile (gisFileInfoPtr, trailerStreamPtr, promptFlag);
		
		continueFlag = FileExists(trailerStreamPtr);
		
		}		// end "if (continueFlag)"
	
			// Continue if memory not full													
			
	if (continueFlag && errCode == noErr)
		{
				// Put writing trailer file message in status dialog.	

		SInt16 stringNumber = 0;
		if (gStatusIDNumber == kUpdateStatsInfoID)
			stringNumber = IDC_Status11;
		else if (gStatusIDNumber == kShortStatusInfoID)
			stringNumber = IDC_ShortStatusText;

		if (stringNumber != 0)
			{
			MGetString (gTextString, kClassifyStrID, IDS_Classify1);
			LoadDItemString (gStatusDialogPtr, stringNumber, (Str255*)&gTextString);

			}		// end "if (stringNumber != 0)"
	
					// Load in 'TRAILER' or 'TRAIL74'										
			
		BlockMoveData ( (Ptr)erdasHeader, (Ptr)ioTempBufferPtr, 7);
	
				// Set bytes 8 to 896 to zeros.												
	
		ioTempBufferPtr = &ioTempBufferPtr[7];
		for (index=7; index<896; index++)
			{
			*ioTempBufferPtr = 0;
			ioTempBufferPtr++;
			
			}		// end "for (index=7; index<896; index++)" 
					
		ioTempBufferPtr = (unsigned char*)ioBufferPtr;
		ioTempBufferPtr = &ioTempBufferPtr[128];
		
				// Set up the palette type variable depending upon the source
				// for the palette information.
				
		thematicPaletteType = paletteType;
		inputPaletteOffset = 0;
		if (thematicPaletteType == 0)
			{
			if (inputColorSpecPtr == NULL)
				{
				if (paletteHandle != NULL)
					{
					thematicPaletteType = kPaletteHandle;
					
					if (gProcessorCode != kMultispecToThematicProcessor)
						inputPaletteOffset = 1;
					
					}		// end "if (paletteHandle != NULL)"
					
				else		// resourcePHandle == NULL 
					thematicPaletteType = kComputedGrays;
				
				}		// end "if (colorSpecPtr == NULL)"
			
			}		// end "if (thematicPaletteType == 0)"
			
				// Get the requested color scheme for the classes.
		
		if (thematicPaletteType != 0)
			continueFlag = LoadColorSpecTable (NULL, 
											NULL,
											colorSpecPtr,
											paletteHandle,
											inputPaletteOffset,
											MIN(numberTRLClasses+3, 256),
											numberTRLClasses,
											thematicPaletteType,
											numberTRLClasses,
											kClassDisplay,
											&paletteCode);
											
				// Now load the colors into the trailer file.
				
				// If the trailer file is being created for a classification
				// image, then allow for class 0, which is a background class.
				// The color for this class is white. This class is not
				// counted in 'numberTRLClasses'.	

		if (classNameCode < kFromDescriptionCode)
			{
			ioTempBufferPtr[0] = 255;
			ioTempBufferPtr[256] = 255;
			ioTempBufferPtr[512] = 255;
			ioTempBufferPtr++;
			
			}		// end "if (classNameCode < kFromDescriptionCode)"
		
		paletteIndex = 0;
		classIndex = 0;	
		for (classNumber=1; classNumber<=numberTRLClasses; classNumber++)
			{
					// This section checks if the class is included in the list.
					// If there is no list, then we assume that the class is
					// included. If the class list is available and a given
					// class is not included, the white (background) color is used
					// for its color. This occurs when the user selects a subset
					// of the classes for a classification.
						
			backgroundFlag = FALSE;
			if (classPtr != NULL)
				{
				if (classNumber == classPtr[classIndex])
					{
					if (classIndex < numberListClasses - 1)
						classIndex++;
						
					}		// end "if (classNumber == classPtr[index])" 
				
				else		// classNumber != classPtr[index] 
					backgroundFlag = TRUE;
				
				}		// end "if (classPtr != NULL)" 
			
			if (backgroundFlag)
				ioTempBufferPtr[0] = ioTempBufferPtr[256] = 
															ioTempBufferPtr[512] = 255;
			
			else		// !backgroundFlag 
				{
				if (paletteIndexPtr != NULL)
					index = paletteIndexPtr[paletteIndex];
					
				else		// newPaletteIndexPtr == NULL
					index = paletteIndex + paletteOffset;
							
				ioTempBufferPtr[0] = (UInt8)(colorSpecPtr[index].rgb.green >> 8);
				ioTempBufferPtr[256] = (UInt8)(colorSpecPtr[index].rgb.red >> 8);
				ioTempBufferPtr[512] = (UInt8)(colorSpecPtr[index].rgb.blue >> 8);
									
				}		// end "else !backgroundFlag"
				
			ioTempBufferPtr++;
			paletteIndex++;
			
			}		// end "for (classNumber=1; classNumber<=..." 
			
				// Write buffer of data	to statistics file.								
					
		count = 7*128;
		errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
		
				// Write 0's for class histograms.											
				
		if ( errCode == noErr && (trailerCode & kHistogram) )
			{
					// Set bytes 1 to 1152 to zeros.											
		
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			for (index=1; index<=1152; index++)
				{
				*ioTempBufferPtr = 0;
				ioTempBufferPtr++;
				
				}		// end "for (index=1; index<=numberBytes; index++)" 
			
					// Write buffer of data	to trailer file.								
						
			count = 1152;
			errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
			
			}		// end "if (errCode == noErr && ...)" 
		
				// Write class names if there is no error.								
			
		if ( errCode == noErr && (trailerCode & kClassNames) )
			{
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			numberBytes = (((long int)numberClasses+4)/4) * 128;
			for (index=0; index<numberBytes; index++)
				{
				*ioTempBufferPtr = 0;
				ioTempBufferPtr++;
				
				}		// end "for (index=0; index<numberBytes; index++)" 
				
					// Write class names.														
					
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			
			if (classNameCode == 0 || classNameCode == kProbFormatCode)
				BlockMoveData ( "background~", (Ptr)ioTempBufferPtr, 11);
				
			if (classNameCode & (kEchoFieldsCode + kEchoClassesCode) )
				{
				BlockMoveData ( "threshold(", (Ptr)ioTempBufferPtr, 10);

				echoClassifierVarPtr = (EchoClassifierVarPtr)GetHandlePointer (
										gClassifySpecsPtr->echoClassifierVarH,
										kLock,
										kNoMoveHi);
					
				if (echoClassifierVarPtr->fixed_homogeneityThreshold_option)
					sprintf( (char*)&ioTempBufferPtr[10], "L");
					
				else		// !...fixed_homogeneityThreshold_option 
					sprintf( (char*)&ioTempBufferPtr[10], "P");
				
				if (classNameCode & kEchoFieldsCode)
					sprintf( (char*)&ioTempBufferPtr[11], 
						"%.1f,A%.1f)~", 
						echoClassifierVarPtr->homogeneityThreshold,
						echoClassifierVarPtr->annexationThreshold);
						
				else		// !(classNameCode & kEchoFieldsCode) 
					sprintf( (char*)&ioTempBufferPtr[11], 
						"%.1f)~", echoClassifierVarPtr->homogeneityThreshold);
				
				CheckAndUnlockHandle (gClassifySpecsPtr->echoClassifierVarH);
				
				}		// end "if (... & (kEchoFieldsCode + kEchoClassesCode) )" 
								
			ioTempBufferPtr += 32;
			          
			if (classPtr != NULL)
				{
				classIndex = 0;
				for (classNumber=1; classNumber<=numberProjectClasses; classNumber++)
					{
					SInt16 nameLength = LoadClassName (
													(SInt16)classNumber, 
													(SInt16)numberListClasses, 
													classPtr, 
													&classIndex, 
													(char*)ioTempBufferPtr,
													0);
													
					ioTempBufferPtr[nameLength] = '~';
					
					ioTempBufferPtr += 32;
				
					}		// end "for ( classNumber=1; classNumber<=..."
					
				}		// end "if (classPtr != NULL)" 
				
			else		// classPtr == NULL
				{
				if (classNameCode == kProbFormatCode)
					{
					if (numberClasses == gNumberProbabilityClasses)
						{
								// Likelihood probabilities.
								
						double rejectionPercent = 0;
						double rejectionStep = 1e-13;
						
						for (index=1; index<=gNumberProbabilityClasses; index++)
							{
							if (rejectionPercent < 0.1)
								sprintf( (char*)ioTempBufferPtr, 
										"%.0e - %.0e~", 
										rejectionPercent, rejectionPercent+rejectionStep);
										
							else		// rejectionPercent >= 0.1 
								sprintf( (char*)ioTempBufferPtr, 
										"%3.1f-%3.1f~", 
										rejectionPercent, rejectionPercent+rejectionStep);
										
							rejectionPercent += rejectionStep;
										
							if (index < 13)
								rejectionStep *= 10;
								
							else		// index >= 14 
								{
								if (index == 13)
									rejectionStep = .1;
									
								if (index == 22)
									rejectionStep = 1.;
									
								}		// end "else index >= 13" 
								
							ioTempBufferPtr = &ioTempBufferPtr[32];
							
							}		// end "for (index=1; index<=..."
							
						}		// end "if (numberClasses == gNumberProbabilityClasses)"
						
					else if (numberClasses == 102)
						{
								// CEM values
						
						double CEMValue = 0;
						double CEMStep = 0.02;
						
						for (index=1; index<=numberClasses; index++)
							{
							if (index == 1)
								sprintf( (char*)ioTempBufferPtr, 
										"<0.00~");
										
							else if (index <= 101) 
								{
								sprintf( (char*)ioTempBufferPtr, 
										"%3.2f-%3.2f~", 
										CEMValue, 
										CEMValue+CEMStep);
								CEMValue += CEMStep;
								
								}		// end "else if (index <= 102)"
								
							else		// index > 101
								sprintf( (char*)ioTempBufferPtr, 
										">2.00~");
										
								
							ioTempBufferPtr = &ioTempBufferPtr[32];
							
							}		// end "for (index=1; index<=..."
						
						}		// end "if (numberClasses == 102)"
						
					else if (numberClasses == 91)
						{
								// Correlation values
						
						UInt32 correlationValue = 90;
						UInt32 correlationStep = 1;
						
						for (index=1; index<=numberClasses; index++)
							{
							if (index == 1)
								sprintf( (char*)ioTempBufferPtr, 
										">90~");
										
							else	// index > 1 
								{
								sprintf( (char*)ioTempBufferPtr, 
										"%ld-%ld~", 
										correlationValue-correlationStep, 
										correlationValue);
								correlationValue -= correlationStep;
								
								}		// end "else index > 1"
								
							ioTempBufferPtr = &ioTempBufferPtr[32];
							
							}		// end "for (index=1; index<=..."
						
						}		// end "if (numberClasses == 91)"
					
					}		// end "if (classNameCode == kProbFormatCode)" 
					
				else if (classNameCode == kEchoFieldsCode)
					{
					for ( index=1; index<=numberClasses; index++)
						{
						sprintf( (char*)ioTempBufferPtr, "field %ld~", index);
						ioTempBufferPtr = &ioTempBufferPtr[32];
					
						}		// end "for (index=1; index<=numberClasses; index++)" 
					
					}		// end "else if (classNameCode == kEchoFieldsCode)"
/*					
				else if (classNameCode == kClusterMaskCode)
					{
					for ( index=1; index<=numberClasses; index++)
						{
						sprintf( (char*)ioTempBufferPtr, "Cluster %ld~", index);
						ioTempBufferPtr = &ioTempBufferPtr[32];
					
						}		// end "for (index=1; index<=numberClasses; index++)" 
					
					}		// end "else if (classNameCode == kEchoFieldsCode)" 
*/					
				else if ( (classNameCode == kFromDescriptionCode) || 
																	(classNameCode == kClusterMaskCode) )
					{
					classNamePtr = (UCharPtr)GetHandlePointer (
											gisFileInfoPtr->descriptionH,
											kLock,
											kNoMoveHi);
					
					if (classNameCode == kFromDescriptionCode)	
							// Go back to location for class 0	
						ioTempBufferPtr -= 32;
					
					for ( index=1; index<=numberClasses; index++)
						{
						count = MIN(classNamePtr[0], 31);
	 					BlockMoveData (&classNamePtr[1], ioTempBufferPtr, count);
						ioTempBufferPtr[ count ] = '~';
					
						ioTempBufferPtr += 32;
						classNamePtr += 32;
					
						}		// end "for (index=1; index<=numberClasses; index++)" 
						
					CheckAndUnlockHandle (gisFileInfoPtr->descriptionH);
					
					}		// end "else if ( (classNameCode == kFromDescriptionCode) || ..." 
					
				}		// end "else classPtr = NULL" 
				
					// Write buffer of data	to trailer file.								
			
			if (classNameCode != 0 || classPtr != NULL)
				{
				count = numberBytes;
				errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
				
				}		// end "if (classNameCode == kProbFormatCode || ..." 
			
			}		// end "if ( errCode == noErr && ...)" 
		  		
		  		// Close the image statistics file											
		  				
		CloseFile (trailerStreamPtr);
			
		}		// end "if (continueFlag && errCode == noErr) 
	
			// Unlock dispose of handles to temporary buffer.							
				
	CheckAndDisposePtr ( (Ptr)ioBufferPtr );
	
	if (inputColorSpecPtr == NULL)
		CheckAndDisposePtr ( (Ptr)colorSpecPtr );
	
	return (continueFlag && errCode == noErr);

}		// end "CreateTRLSupportFile"  


/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateTRLSupportFile
//
//	Software purpose:	This routine creates a trl file. If the 
//							classNameCode is 'kFromDescriptionCode', then this
//							is called from the Reformat-Change Image File Format
//							processor. The class names and colors come from those
//							currently being used in the active image window.
//
//							AreasToThematicFileControl (paletteID)
//							CovertMultispectralToThematic (paletteHandle)
//							CreateTrailerFiles (paletteID)
//							ChangeFormatToBILorBIS (paletteHandle)
//							WriteThematicClassesAs (colorSpecPtr)
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			AreasToThematicFileControl in fieldsToThematicFile.c
//							CovertMultispectralToThematic in fieldsToThematicFile.c
//							CreateTrailerFiles in SClassfy.cpp
//							ChangeFormatToBILorBIS in SReform1.cpp
//							WriteThematicClassesAs in SSaveWrt.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 02/21/1990
//	Revised By:			Larry L. Biehl			Date: 10/07/2002	

Boolean CreateTRLSupportFile (
				FileInfoPtr							gisFileInfoPtr, 
				UInt16								numberClasses, 
				SInt16*								classPtr,
				UInt16								numberListClasses,  
				ColorSpec*							colorSpecPtr,
				UInt16*								paletteIndexPtr,
				SInt16								paletteOffset, 
				SInt16								classNameCode, 
				SInt16								trailerCode)

{
	CMFileStream						trailerStream;
											
	EchoClassifierVarPtr				echoClassifierVarPtr;
	CMFileStream*						trailerStreamPtr;
	HFileIOBufferPtr					ioBufferPtr;
	
	char									*erdasHeader = "TRAIL74";
										
	UCharPtr								classNamePtr,
											ioTempBufferPtr;
										
	SInt32								classNumber;
	
	UInt32								count,
											index,
											numberBytes;
	
	SInt16								errCode,
											numberProjectClasses,
											numberTRLClasses,
											thematicPaletteType;
										
	UInt16								classIndex,
											inputPaletteOffset,
											paletteCode,
											paletteIndex;
										
	Boolean								backgroundFlag,
											continueFlag,
											promptFlag;
	
			
	continueFlag = TRUE;
	numberProjectClasses = 0;
	if (gProjectInfoPtr != NULL)
		numberProjectClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Get buffer to load trailer file information into.  Try to get 		
			// buffer to hold one complete set of the 3 types of information -	
			// color scheme, class histogram, and class name. Note that even
			// though we are limited to colors for only 256 classes, we can
			// have class names for up to 'gClassListLimit' classes.					
			
	numberBytes = (((SInt32)numberClasses+4)/4) * 128;
	numberBytes = MAX ( (9*128), numberBytes);
	ioBufferPtr = (HFileIOBufferPtr)MNewPointer (numberByte);
	continueFlag = (ioBufferPtr != NULL);
	
	if (continueFlag)
		{
		ioTempBufferPtr = (UCharPtr)ioBufferPtr;
					
				// Get the pointer to the file stream block and initialize it.			
			
		trailerStreamPtr = &trailerStream;
		InitializeFileStream (trailerStreamPtr);
		
		promptFlag = FALSE;
		if (inputColorSpecPtr != NULL)
			promptFlag = TRUE;
		
		errCode = GetTRLSupportFile (gisFileInfoPtr, trailerStreamPtr, promptFlag);
		
		continueFlag = FileExists(trailerStreamPtr);
		
		}		// end "if (continueFlag)"
	
			// Continue if memory not full													
			
	if (continueFlag && errCode == noErr)
		{
				// Put writing trailer file message in status dialog.	

		SInt16 stringNumber = 0;
		if (gStatusIDNumber == kUpdateStatsInfoID)
			stringNumber = IDC_Status11;
		else if (gStatusIDNumber == kShortStatusInfoID)
			stringNumber = IDC_ShortStatusText;

		if (stringNumber != 0)
			{
			MGetString (gTextString, kClassifyStrID, IDS_Classify1);
			LoadDItemString (gStatusDialogPtr, stringNumber, (Str255*)&gTextString);

			}		// end "if (stringNumber != 0)"
	
					// Load in 'TRAILER' or 'TRAIL74'										
			
		BlockMoveData ( (Ptr)erdasHeader, (Ptr)ioTempBufferPtr, 7);
	
				// Set bytes 8 to 896 to zeros.												
	
		ioTempBufferPtr = &ioTempBufferPtr[7];
		for (index=7; index<896; index++)
			{
			*ioTempBufferPtr = 0;
			ioTempBufferPtr++;
			
			}		// end "for (index=7; index<896; index++)" 
					
		ioTempBufferPtr = (unsigned char*)ioBufferPtr;
		ioTempBufferPtr = &ioTempBufferPtr[128];
											
				// Now load the colors into the trailer file.
				
				// If the trailer file is being created for a classification
				// image, then allow for class 0, which is a background class.
				// The color for this class is white. This class is not
				// counted in 'numberTRLClasses'.	

		if (classNameCode < kFromDescriptionCode)
			{
			ioTempBufferPtr[0] = 255;
			ioTempBufferPtr[256] = 255;
			ioTempBufferPtr[512] = 255;
			ioTempBufferPtr++;
			
			}		// end "if (classNameCode < kFromDescriptionCode)"
		
		paletteIndex = 0;
		classIndex = 0;	
		for (classNumber=1; classNumber<=numberTRLClasses; classNumber++)
			{
					// This section checks if the class is included in the list.
					// If there is no list, then we assume that the class is
					// included. If the class list is available and a given
					// class is not included, the white (background) color is used
					// for its color. This occurs when the user selects a subset
					// of the classes for a classification.
						
			backgroundFlag = FALSE;
			if (classPtr != NULL)
				{
				if (classNumber == classPtr[classIndex])
					{
					if (classIndex < numberListClasses - 1)
						classIndex++;
						
					}		// end "if (classNumber == classPtr[index])" 
				
				else		// classNumber != classPtr[index] 
					backgroundFlag = TRUE;
				
				}		// end "if (classPtr != NULL)" 
			
			if (backgroundFlag)
				ioTempBufferPtr[0] = ioTempBufferPtr[256] = 
															ioTempBufferPtr[512] = 255;
			
			else		// !backgroundFlag 
				{
				if (paletteIndexPtr != NULL)
					index = paletteIndexPtr[paletteIndex];
					
				else		// newPaletteIndexPtr == NULL
					index = paletteIndex + paletteOffset;
							
				ioTempBufferPtr[0] = (UInt8)(colorSpecPtr[index].rgb.green >> 8);
				ioTempBufferPtr[256] = (UInt8)(colorSpecPtr[index].rgb.red >> 8);
				ioTempBufferPtr[512] = (UInt8)(colorSpecPtr[index].rgb.blue >> 8);
									
				}		// end "else !backgroundFlag"
				
			ioTempBufferPtr++;
			paletteIndex++;
			
			}		// end "for (classNumber=1; classNumber<=..." 
			
				// Write buffer of data	to trl file.								
					
		count = 7*128;
		errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
		
				// Write 0's for class histograms.											
				
		if ( errCode == noErr && (trailerCode & kHistogram) )
			{
					// Set bytes 1 to 1152 to zeros.											
		
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			for (index=1; index<=1152; index++)
				{
				*ioTempBufferPtr = 0;
				ioTempBufferPtr++;
				
				}		// end "for (index=1; index<=numberBytes; index++)" 
			
					// Write buffer of data	to trailer file.								
						
			count = 1152;
			errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
			
			}		// end "if (errCode == noErr && ...)" 
		
				// Write class names if there is no error.								
			
		if ( errCode == noErr && (trailerCode & kClassNames) )
			{
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			numberBytes = (((long int)numberClasses+4)/4) * 128;
			for (index=0; index<numberBytes; index++)
				{
				*ioTempBufferPtr = 0;
				ioTempBufferPtr++;
				
				}		// end "for (index=0; index<numberBytes; index++)" 
				
					// Write class names.														
					
			ioTempBufferPtr = (unsigned char*)ioBufferPtr;
			
			if (classNameCode == 0 || classNameCode == kProbFormatCode)
				BlockMoveData ( "background~", (Ptr)ioTempBufferPtr, 11);
				
			if (classNameCode & (kEchoFieldsCode + kEchoClassesCode) )
				{
				BlockMoveData ( "threshold(", (Ptr)ioTempBufferPtr, 10);

				echoClassifierVarPtr = (EchoClassifierVarPtr)GetHandlePointer (
										gClassifySpecsPtr->echoClassifierVarH,
										kLock,
										kNoMoveHi);
					
				if (echoClassifierVarPtr->fixed_homogeneityThreshold_option)
					sprintf( (char*)&ioTempBufferPtr[10], "L");
					
				else		// !...fixed_homogeneityThreshold_option 
					sprintf( (char*)&ioTempBufferPtr[10], "P");
				
				if (classNameCode & kEchoFieldsCode)
					sprintf( (char*)&ioTempBufferPtr[11], 
						"%.1f,A%.1f)~", 
						echoClassifierVarPtr->homogeneityThreshold,
						echoClassifierVarPtr->annexationThreshold);
						
				else		// !(classNameCode & kEchoFieldsCode) 
					sprintf( (char*)&ioTempBufferPtr[11], 
						"%.1f)~", echoClassifierVarPtr->homogeneityThreshold);
				
				CheckAndUnlockHandle (gClassifySpecsPtr->echoClassifierVarH);
				
				}		// end "if (... & (kEchoFieldsCode + kEchoClassesCode) )" 
								
			ioTempBufferPtr += 32;
			          
			if (classPtr != NULL)
				{
				classIndex = 0;
				for (classNumber=1; classNumber<=numberProjectClasses; classNumber++)
					{
					SInt16 nameLength = LoadClassName (
													(SInt16)classNumber, 
													(SInt16)numberListClasses, 
													classPtr, 
													&classIndex, 
													(char*)ioTempBufferPtr,
													0);
													
					ioTempBufferPtr[nameLength] = '~';
					
					ioTempBufferPtr += 32;
				
					}		// end "for ( classNumber=1; classNumber<=..."
					
				}		// end "if (classPtr != NULL)" 
				
			else		// classPtr == NULL
				{
				if (classNameCode == kProbFormatCode)
					{
					if (numberClasses == gNumberProbabilityClasses)
						{
								// Likelihood probabilities.
								
						double rejectionPercent = 0;
						double rejectionStep = 1e-13;
						
						for (index=1; index<=gNumberProbabilityClasses; index++)
							{
							if (rejectionPercent < 0.1)
								sprintf( (char*)ioTempBufferPtr, 
										"%.0e - %.0e~", 
										rejectionPercent, rejectionPercent+rejectionStep);
										
							else		// rejectionPercent >= 0.1 
								sprintf( (char*)ioTempBufferPtr, 
										"%3.1f-%3.1f~", 
										rejectionPercent, rejectionPercent+rejectionStep);
										
							rejectionPercent += rejectionStep;
										
							if (index < 13)
								rejectionStep *= 10;
								
							else		// index >= 14 
								{
								if (index == 13)
									rejectionStep = .1;
									
								if (index == 22)
									rejectionStep = 1.;
									
								}		// end "else index >= 13" 
								
							ioTempBufferPtr = &ioTempBufferPtr[32];
							
							}		// end "for (index=1; index<=..."
							
						}		// end "if (numberClasses == gNumberProbabilityClasses)"
						
					else if (numberClasses == 102)
						{
								// CEM values
						
						double CEMValue = 0;
						double CEMStep = 0.02;
						
						for (index=1; index<=numberClasses; index++)
							{
							if (index == 1)
								sprintf( (char*)ioTempBufferPtr, 
										"<0.00~");
										
							else if (index <= 101) 
								{
								sprintf( (char*)ioTempBufferPtr, 
										"%3.2f-%3.2f~", 
										CEMValue, 
										CEMValue+CEMStep);
								CEMValue += CEMStep;
								
								}		// end "else if (index <= 102)"
								
							else		// index > 101
								sprintf( (char*)ioTempBufferPtr, 
										">2.00~");
										
								
							ioTempBufferPtr = &ioTempBufferPtr[32];
							
							}		// end "for (index=1; index<=..."
						
						}		// end "if (numberClasses == 102)"
						
					else if (numberClasses == 91)
						{
								// Correlation values
						
						UInt32 correlationValue = 90;
						UInt32 correlationStep = 1;
						
						for (index=1; index<=numberClasses; index++)
							{
							if (index == 1)
								sprintf( (char*)ioTempBufferPtr, 
										">90~");
										
							else	// index > 1 
								{
								sprintf( (char*)ioTempBufferPtr, 
										"%ld-%ld~", 
										correlationValue-correlationStep, 
										correlationValue);
								correlationValue -= correlationStep;
								
								}		// end "else index > 1"
								
							ioTempBufferPtr = &ioTempBufferPtr[32];
							
							}		// end "for (index=1; index<=..."
						
						}		// end "if (numberClasses == 91)"
					
					}		// end "if (classNameCode == kProbFormatCode)" 
					
				else if (classNameCode == kEchoFieldsCode)
					{
					for ( index=1; index<=numberClasses; index++)
						{
						sprintf( (char*)ioTempBufferPtr, "field %ld~", index);
						ioTempBufferPtr = &ioTempBufferPtr[32];
					
						}		// end "for (index=1; index<=numberClasses; index++)" 
					
					}		// end "else if (classNameCode == kEchoFieldsCode)"
				
//				else if (classNameCode == kClusterMaskCode)
//					{
//					for ( index=1; index<=numberClasses; index++)
//						{
//						sprintf( (char*)ioTempBufferPtr, "Cluster %ld~", index);
//						ioTempBufferPtr = &ioTempBufferPtr[32];
//					
//						}		// end "for (index=1; index<=numberClasses; index++)" 
//					
//					}		// end "else if (classNameCode == kEchoFieldsCode)" 
					
				else if ( (classNameCode == kFromDescriptionCode) || 
																	(classNameCode == kClusterMaskCode) )
					{
					classNamePtr = (UCharPtr)GetHandlePointer (
											gisFileInfoPtr->descriptionH,
											kLock,
											kNoMoveHi);
					
					if (classNameCode == kFromDescriptionCode)	
							// Go back to location for class 0	
						ioTempBufferPtr -= 32;
					
					for ( index=1; index<=numberClasses; index++)
						{
						count = MIN(classNamePtr[0], 31);
	 					BlockMoveData (&classNamePtr[1], ioTempBufferPtr, count);
						ioTempBufferPtr[ count ] = '~';
					
						ioTempBufferPtr += 32;
						classNamePtr += 32;
					
						}		// end "for (index=1; index<=numberClasses; index++)" 
						
					CheckAndUnlockHandle (gisFileInfoPtr->descriptionH);
					
					}		// end "else if ( (classNameCode == kFromDescriptionCode) || ..." 
					
				}		// end "else classPtr = NULL" 
				
					// Write buffer of data	to trailer file.								
			
			if (classNameCode != 0 || classPtr != NULL)
				{
				count = numberBytes;
				errCode = MWriteData (trailerStreamPtr, &count, ioBufferPtr, kErrorMessages);
				
				}		// end "if (classNameCode == kProbFormatCode || ..." 
			
			}		// end "if ( errCode == noErr && ...)" 
		  		
		  		// Close the image statistics file											
		  				
		CloseFile (trailerStreamPtr);
			
		}		// end "if (continueFlag && errCode == noErr) 
	
			// Unlock dispose of handles to temporary buffer.							
				
	CheckAndDisposePtr ( (Ptr)ioBufferPtr );
	
	return (continueFlag && errCode == noErr);

}		// end "CreateTRLSupportFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteSupportInformation
//
//	Software purpose:	This routine writes the support information for an image file
//							including the class names and class colors for ERDAS TRL files
//							associated with ERDAS .gis files and the color table for 
//							TIFF/GeoTIFF files.
//							If the classNameCode is 'kFromDescriptionCode', then this
//							is called from the Reformat-Change Image File Format
//							processor. The class names and colors come from those
//							currently being used in the active image window.
//
//							AreasToThematicFileControl (paletteID)
//							CovertMultispectralToThematic (paletteHandle)
//							CreateTrailerFiles (paletteID)
//							ChangeFormatToBILorBIS (paletteHandle)
//							WriteThematicClassesAs (colorSpecPtr)
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			AreasToThematicFileControl in fieldsToThematicFile.c
//							CovertMultispectralToThematic in fieldsToThematicFile.c
//							CreateTrailerFiles in SClassfy.cpp
//							ChangeFormatToBILorBIS in SReform1.cpp
//							WriteThematicClassesAs in SSaveWrt.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 10/07/2002
//	Revised By:			Larry L. Biehl			Date: 10/07/2002	

Boolean WriteSupportInformation (
				FileInfoPtr							gisFileInfoPtr, 
				UInt16								numberClasses, 
				SInt16*								classPtr,
				UInt16								numberListClasses,  
				ColorSpec*							inputColorSpecPtr,
				CMPaletteInfo						paletteHandle,
				UInt16*								paletteIndexPtr,
				SInt16								paletteOffset, 
				SInt16								classNameCode, 
				SInt16								trailerCode)

{
	CMFileStream						trailerStream;
											
	EchoClassifierVarPtr				echoClassifierVarPtr;
	CMFileStream*						trailerStreamPtr;
	ColorSpec*							colorSpecPtr;
	HFileIOBufferPtr					ioBufferPtr;
										
	UCharPtr								classNamePtr,
											ioTempBufferPtr;
										
	SInt32								classNumber;
	
	UInt32								count,
											index,
											numberBytes;
	
	SInt16								errCode,
											numberProjectClasses,
											numberTRLClasses,
											thematicPaletteType;
										
	UInt16								classIndex,
											inputPaletteOffset,
											paletteCode,
											paletteIndex;
										
	Boolean								backgroundFlag,
											continueFlag,
											promptFlag;
	
	
	if (gisFileInfoPtr == NULL)
																				return (FALSE);
	
			// Do not create a trailer file if there are more than the class		
			// limit.																				
			
	if (numberClasses > gClassListLimit)
																				return (TRUE);
																				
			// Make certain that we have the proper information for the 			
			// palette information.																
			
	if (paletteHandle != NULL || inputColorSpecPtr != NULL)
		paletteType = 0;
		
	else		// paletteHandle == NULL && inputColorSpecPtr == NULL
		if (paletteType <= 0)
																				return (FALSE);
		
	continueFlag = TRUE;
	numberProjectClasses = 0;
	if (gProjectInfoPtr != NULL)
		numberProjectClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Determine the number of trailer classes.									
			
	numberTRLClasses = numberClasses;
	if (classPtr != NULL)
		numberTRLClasses = numberProjectClasses;
		
			// Make sure that the number of classes is not more than 256.
			// Allow for the background class if needed.
			
	if (classNameCode < kFromDescriptionCode)
		numberTRLClasses = MIN(kMaxNumberStatClasses-1, numberTRLClasses);
	numberTRLClasses = MIN(kMaxNumberStatClasses, numberTRLClasses);
	
			// Get buffer to load color table into if needed.
			
	colorSpecPtr = inputColorSpecPtr;
	if (continueFlag && inputColorSpecPtr == NULL)
		{	
				// Get table for palette information. Allow enough for the
				// number of classes, white, black, and background. The
				// white, black and background will probably not be need for
				// the trailer file, but the routines being called will
				// fix those colors.
					
		colorSpecPtr = (ColorSpec*)MNewPointer ( 
									(SInt32)(numberTRLClasses+3)*sizeof(ColorSpec) );
		continueFlag = (colorSpecPtr != NULL);
				
		}		// end "if (continueFlag && colorSpecPtr != NULL)"
	
			// Continue if memory not full													
			
	if (continueFlag)
		{
				// Put writing trailer file message in status dialog.	

		SInt16 stringNumber = 0;
		if (gStatusIDNumber == kUpdateStatsInfoID)
			stringNumber = IDC_Status11;
		else if (gStatusIDNumber == kShortStatusInfoID)
			stringNumber = IDC_ShortStatusText;

		if (stringNumber != 0)
			{
			MGetString (gTextString, kClassifyStrID, IDS_Classify1);
			LoadDItemString (gStatusDialogPtr, stringNumber, (Str255*)&gTextString);

			}		// end "if (stringNumber != 0)"
		
				// Set up the palette type variable depending upon the source
				// for the palette information.
				
		thematicPaletteType = paletteType;
		inputPaletteOffset = 0;
		if (thematicPaletteType == 0)
			{
			if (inputColorSpecPtr == NULL)
				{
				if (paletteHandle != NULL)
					{
					thematicPaletteType = kPaletteHandle;
					
					if (gProcessorCode != kMultispecToThematicProcessor)
						inputPaletteOffset = 1;
					
					}		// end "if (paletteHandle != NULL)"
					
				else		// resourcePHandle == NULL 
					thematicPaletteType = kComputedGrays;
				
				}		// end "if (colorSpecPtr == NULL)"
			
			}		// end "if (thematicPaletteType == 0)"
			
				// Get the requested color scheme for the classes.
		
		if (thematicPaletteType != 0)
			continueFlag = LoadColorSpecTable (NULL, 
											NULL,
											colorSpecPtr,
											paletteHandle,
											inputPaletteOffset,
											MIN(numberTRLClasses+3, 256),
											numberTRLClasses,
											thematicPaletteType,
											numberTRLClasses,
											kClassDisplay,
											&paletteCode);
											
		if (continueFlag)
			{
			switch (thematicFileInfoPtr->format)
				{
				case kTIFFType:
				case kGeoTIFFType:
					continueFlag = WriteTIFFColorMap (outFileInfoPtr,
																	colorSpecPtr, 
																	numberTRLClasses,
																	paletteIndexPtr,
																	trailerCode);

					break;
				
				default:
					continueFlag = CreateTRLSupportFile (gisFileInfoPtr, 
																		numberClasses, 
																		classPtr,
																		numberListClasses,  
																		inputColorSpecPtr,
																		paletteIndexPtr,
																		paletteType,
																		paletteOffset, 
																		classNameCode, 
																		trailerCode);
				
				}		// end "switch (thematicFileInfoPtr->format)"
			
			}		// end "if (continueFlag)"
			
		}		// end "if (continueFlag) 
	
	if (inputColorSpecPtr == NULL)
		CheckAndDisposePtr ( (Ptr)colorSpecPtr );
	
	return (continueFlag && errCode == noErr);

}		// end "WriteSupportInformation" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		DiskFullAlert
//
//	Software purpose:	This routine puts up an alert warning the user that
//							there is not enough space on the selected volume
//							to write the file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/07/1988
//	Revised By:			Larry L. Biehl			Date: 05/02/2001

void DiskFullAlert (
				SInt32								bytesNeeded, 
				Str255*								fileNamePtr)

{
	Str255								bytesNeededString;
	
	char* 								charFileNamePtr;
	
	SInt16								length,
											numberCharacters;
											
											
	charFileNamePtr = (char*)fileNamePtr;
	
			// Add commas to number of bytes needed.
			
	numberCharacters = sprintf( (char*)&bytesNeededString, 
											"%ld",  
											bytesNeeded);

	numberCharacters = InsertCommasInNumberString (
										(char*)&bytesNeededString, 
										numberCharacters, 
										-1,
										numberCharacters);
										
	MGetString (gTextString, kFileIOStrID, IDS_FileIO01);
										
//	SInt16 length = sprintf( (char*)&gTextString2[1],
//										(char*)&gTextString[1],
//										&charFileNamePtr[1],
//										bytesNeeded);
										
	length = sprintf( (char*)&gTextString2[1],
										(char*)&gTextString[1],
										&charFileNamePtr[1],
										(char*)&bytesNeededString[1]);

	gTextString2[0] = (SInt8)length;

	DisplayAlert (kErrorAlertID, 
							kStopAlert, 
							0, 
							0, 
							0, 
							(UCharPtr)&gTextString2);

}		// end "DiskFullAlert"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FileExists
//
//	Software purpose:	This routine returns TRUE if a file currently exists
//							for the input file stream and FALSE otherwise. It
//							checks whether a file name is available.
//
//	Parameters in:		The file stream pointer			
//
//	Parameters out:	None			
//
// Value Returned:	TRUE if file name has been determined
//							FALSE if file name is not currently available
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

Boolean FileExists (
				CMFileStream*						fileStreamPtr)

{
	if (fileStreamPtr != NULL)
		{
		#if !defined _X86_
		
			return (fileStreamPtr->fileName[0] > 0); 
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_
		
			return (fileStreamPtr->mFileType != -1);
		
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)"
		
	return (FALSE);

}		// end "FileExists"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean FileOpen
//
//	Software purpose:	This routine returns TRUE if a file is currently open
//							for the input file stream and FALSE otherwise.
//
//	Parameters in:		The file stream pointer			
//
//	Parameters out:	None			
//
// Value Returned:	TRUE if file is already open
//							FALSE if file is not open
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/16/1995
//	Revised By:			Larry L. Biehl			Date: 11/16/1995

Boolean FileOpen (
				CMFileStream*						fileStreamPtr)

{
	if (fileStreamPtr != NULL)
		{
		#if !defined _X86_
		
			return (fileStreamPtr->refNum != 0); 
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_
		
			return ( fileStreamPtr->FileOpen() );
		
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)"
		
	return (FALSE);

}		// end "FileOpen"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetBILSpecial
//
//	Software purpose:	The purpose of this routine is to set
//							the BILSpecial flag.  This flag, when TRUE,
//							indicates for the routine 'GetLineOfData' that
//							all requested channels can be read with one
//							disk IO read.
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned: 
//
// Called By:			BiPlotFieldData in biPlotData.c
//							MosaicImagesSideBySide in mosaic.c
//							RectifyImage in rectification.c
//							ClassifyPerPointArea in SClassfy.cpp
//							ClusterClassification in SCluster.cpp
//							GetNextClusterArea in SCluster.cpp
//							DisplayImagesSideBySide in SDisMulc.cpp
//							GetDisplayIOBuffer in SDisMulc.cpp
//							EchoClassifier in SEchoSpc.cpp
//							ComputeHistogram in SHistgrm.cpp
//							ListFieldData in SLstData.cpp
//							GetAreaStats in SMatUtil.cpp
//							GetFieldDataValues in SPMatUtl.cpp
//							ChangeFormatToBILorBIS in SReform1.cpp
//							UpdateProjectMaskStats in SStatCom.cpp
//							HistogramFieldStats in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 11/29/1989
//	Revised By:			Larry L. Biehl			Date: 01/24/2000

Boolean GetBILSpecial(
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				UInt16*		 						channelListPtr, 
				UInt16								numberListChannels,
				UInt32								columnStart, 
				UInt32								columnEnd, 
				UInt32								columnInterval,
				UInt16								specialBILCode)

{  
	FileInfoPtr 						fileInfoPtr,
											localFileInfoPtr;
											
	LayerInfoPtr						layerInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	                                
	UInt32								channel,
											fileInfoIndex,
											numberChannelsToRead;
	
	Boolean								BILSpecialFlag; 
	
	
			// Check input request.
			
	if (specialBILCode == kSetSpecialBILFlagFalse)
																				return (FALSE);
			
	if (specialBILCode == kSetSpecialBILFlagTrue)
																				return (TRUE);
																				
			// Initialize local variables.
			
	windowInfoPtr = fileIOInstructionsPtr->windowInfoPtr;
	layerInfoPtr = fileIOInstructionsPtr->layerInfoPtr;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	
	localFileInfoPtr = fileInfoPtr;
	numberChannelsToRead = 0;
	
			// If there is more than one image file for the window "file", then	
			// determine if the channels all come from the same image file.		
			// If the selected do not then return BILSpecialFlag as FALSE. It gets	
			// more complicated to determine whether BILSpecialFlag can be used.		
			
	if (windowInfoPtr != NULL && windowInfoPtr->numberImageFiles > 1)
		{
		if (channelListPtr == NULL)
																						return (FALSE);
																				
		else		// channelListPtr != NULL 
			{
			if (layerInfoPtr == NULL)
																						return (FALSE);
			
					// Determine if the selected channels all come from the same 	
					// image file.																	
					
			BILSpecialFlag = TRUE;
			fileInfoIndex = layerInfoPtr[ channelListPtr[0]+1 ].fileInfoIndex;
			
			for (channel=0; channel<numberListChannels; channel++)
				{
				if (fileInfoIndex != 
								layerInfoPtr[ channelListPtr[channel]+1 ].fileInfoIndex)
					BILSpecialFlag = FALSE;
					
				}		// end "for (channel=0; channel<..." 
				
			if (!BILSpecialFlag)
																						return (FALSE);
																					
			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
																					
			}		// end "else channelListPtr != NULL" 
			
		}		// end "if (if (windowInfoPtr != NULL && ...->numberImageFiles > 1)"
	
			// Do not set the special flag if the number of channels is only 1.
			
	if (localFileInfoPtr->numberChannels == 1) 
																					return (FALSE);
																				
			// If BIL formatted data and all channels and all columns are to be	
			// read in, then set flag to read all bytes at one time.					
	
	BILSpecialFlag = FALSE;
	if ( localFileInfoPtr->bandInterleave == kBIL && 
			localFileInfoPtr->numberChannels >= numberListChannels &&
			localFileInfoPtr->numberBits >= 8)
		{
		if (channelListPtr == NULL)
			{
			BILSpecialFlag = TRUE;
			numberChannelsToRead = numberListChannels;
			
			}		// end "if (channelListPtr == NULL)"
			
		else		// channelListPtr != NULL 
			{
			BILSpecialFlag = 
					DetermineIfChannelsInOrder (channelListPtr, (UInt32)numberListChannels);
			
			if (BILSpecialFlag)
				{	
				numberChannelsToRead = 
								channelListPtr[numberListChannels-1] - channelListPtr[0] + 1;
		
						// Do not read all channels if the number being used is less than
						// about 3 percent of the total being read.
					
				if (numberChannelsToRead > 30 && 
													numberListChannels < numberChannelsToRead/30)
					BILSpecialFlag = FALSE;
					
				}		// end "if (BILSpecialFlag)"
							
			}		// end "else channelListPtr != NULL"
		
		}		// end "if ( localFileInfoPtr->bandInterleave == kBIL && ..." 
	
	if (BILSpecialFlag && ( fileInfoPtr->numberPreChannelBytes != 0 ||
										fileInfoPtr->numberPostChannelBytes != 0) )
		BILSpecialFlag = FALSE;
	
	if (BILSpecialFlag)
		fileIOInstructionsPtr->bilSpecialNumberChannels = (UInt16)numberChannelsToRead;
		
	return (BILSpecialFlag);
	      
}		// end "GetBILSpecial" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCopyOfPFileName
//
//	Software purpose:	The purpose of this routine is to copy the actual file 
//							name not including the path to the specified location. 
//							The file name will output in pascal format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1995
//	Revised By:			Larry L. Biehl			Date: 12/11/1995	

void GetCopyOfPFileName (
				CMFileStream*						fileStreamPtr,
				UCharPtr								outputPtr)

{  
	UCharPtr								fileNamePtr;
	UInt16								nameLength = 0;
	
	
	if (outputPtr != NULL)
		{
		if (fileStreamPtr != NULL)
			{
			fileNamePtr = (UCharPtr)GetFileNameCPointer (fileStreamPtr);
			nameLength = strlen( (char*)fileNamePtr );
			strcpy ( (char*)&outputPtr[1], (char*)fileNamePtr); 
			
			}		// end "if (fileInfoPtr != NULL)" 
			                    
		outputPtr[0] = (UInt8)nameLength;
			
		}		// end "if (outputPtr != NULL)" 
	
}		// end "GetCopyOfPFileName"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCopyOfPFileName
//
//	Software purpose:	The purpose of this routine is to copy the actual file 
//							name not including the path to the specified location. 
//							The file name will output in pascal format.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/12/1995
//	Revised By:			Larry L. Biehl			Date: 12/12/1995	

void GetCopyOfPFileName (
				FileInfoPtr							fileInfoPtr,
				UCharPtr								outputPtr)

{  
	if (outputPtr != NULL)
		{
		if (fileInfoPtr != NULL)
			{              
			CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			
			GetCopyOfPFileName(fileStreamPtr, outputPtr);
			
			}		// end "if (fileInfoPtr != NULL)"
			
		else		// fileInfoPtr == NULL
			outputPtr[0] = 0;
			
		}		// end "if (outputPtr != NULL)" 
	
}		// end "GetCopyOfPFileName"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetFSSpecFlag
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							directory id.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002	

Boolean GetFSSpecFlag (
				FileInfoPtr							fileInfoPtr)

{  
	Boolean								fSSpecFlag = FALSE;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer(fileInfoPtr);
	
	
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
			fSSpecFlag = fileStreamPtr->fSSpecFlag;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 
	

	return (fSSpecFlag);
	
}		// end "GetFSSpecFlag"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetFSSpecFlag
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002	

Boolean GetFSSpecFlag (
				Handle								fileInfoHandle)

{  
	Boolean								fSSpecFlag = FALSE;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
													fileInfoHandle, kNoLock, kNoMoveHi);
													
	fSSpecFlag = GetFSSpecFlag (fileInfoPtr);

	return (fSSpecFlag);
	
}		// end "GetFSSpecFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard get file dialog box to allow the user to
//							select the file to read from.
//		
//	Parameters in:		Address to MFileInfo structure for selected file.
//							Number of parameters in type list.
//							Address of type list.
//
//	Parameters out:	MFileInfo Structure parameters for selected file.
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetSTASupportFile in histogram.c
//							OpenImageFile in openImageRoutines.c
//							GetProjectFile in project.c
//							UserLocateProjectBaseImage in project.c
//							LoadTransformationFile in saveWrite.c
//
//	Coded By:			Larry L. Biehl			Date: 05/28/1988
//	Revised By:			Larry L. Biehl			Date: 08/24/2001

SInt16 GetFile (
				CMFileStream* 						fileStreamPtr, 
				SInt16								numberTypes, 
				OSType*								fileTypesPtr, 
				LocalAppFile*						localAppFilePtr, 
				SInt16								stringIndex)

{
	SInt16								errCode = noErr;
	
	             
	#if !defined _X86_

		if (gNavServicesExistsFlag)
			{
			errCode = NavServicesGetFile (fileStreamPtr, 
														numberTypes, 
														fileTypesPtr, 
														localAppFilePtr, 
														stringIndex);
	
																						return (errCode);
					
			}		// end "if (gNavServicesExistsFlag)"
			
	#endif	// !defined _X86_

	#if !TARGET_API_MAC_CARBON		
		
			// Initialize variables.															
			
	gGetFileStatus = 3;
	             
	#if !defined _X86_
	 
		Point						where;
		
				// Make certain that MultiSpec is not in the background.					
				
		PauseIfInBackground ();
			
				// Check if this routine called from MultiSpec startup, in which		
				// case the file to open has already been selected.						
				
		if (localAppFilePtr != NULL)
			{
			fileStreamPtr->vRefNum = localAppFilePtr->vRefNum;
			fileStreamPtr->parID = localAppFilePtr->parID;
			CopyPToP ( (char*)fileStreamPtr->fileName, (char*)localAppFilePtr->fNamePtr);
			fileStreamPtr->fSSpecFlag = localAppFilePtr->fSSpecFlag;
			
			}		// end "if (localAppFilePtr != NULL)" 
		
				// Else file has not been selected.												
				// Put the file dialog box up to select the input file.					
		
		else		// localAppFilePtr == NULL 
			{	
					// Calculate point where dialog will be centered. 						
				
			GetDlogOrigin (getDlgID, &where);
		
					// Get the requested prompt string.											
					
			if ( !GetSpecifiedStringNumber (
								kFileIOStrID, stringIndex, &gTextString, TRUE ) )
																				return (memFullErr);
			
			if (gMultipleImageFileCode)
				{
				sprintf( (char*)&gTextString[13], 
						"%3d", 
						((WindowInfoPtr)*gActiveImageWindowInfoH)->numberImageFiles+1 );
				gTextString[16] = ' ';
				gGetFileStatus = 2;
				
				}		// end "if (gMultipleImageFileCode)" 
				
			else		// !gMultipleImageFileCode 
				{
				gGetFileStatus = -1;
				if (stringIndex == IDS_SelectImage)
					gGetFileStatus = 0;
					
				}		// end "else !gMultipleImageFileCode" 
				
			gFileReply.good = FALSE;
				
					// Only call standard io get file if there is a minimum amount of	
					// memory available.																
					
			if ( CheckIfMemoryAvailable ( 15000 ) )
				{	
				if (stringIndex == IDS_WhereIsProjectImage ||
						stringIndex == IDS_FileIO103 ||
							stringIndex == IDS_FileIO104)
					ParamText ( gTextString, fileStreamPtr->fileName, "\p?", "\p" );
					
				else		// stringIndex == IDS_WhereIsProjectImage 
					ParamText ( gTextString, "\p", "\p", "\p" );
				
				SFGetFile (where, 
								NULL, 
								NULL, 
								numberTypes, 
								fileTypesPtr, 
								gGetFileHookPtr, 
								&gFileReply);
								
				ParamText ("\p", "\p", "\p", "\p");
					
						// Make certain that all previous mouse events have been			
						// removed. This is done in case the user double clicked			
						// to select the file.														
										
		    	FlushEvents (mUpMask,0);
				
				}		// end "if ( CheckIfMemoryAvailable ( 15000 )" 
			
					// If file is to be added to the active image window, then the		
					// file type has to be forced to multispectral type.					
					
			if (gMultipleImageFileCode != 0)
				gGetFileImageType = kImageType;
			
			CheckSomeEvents (activMask+updateMask);
		
			if (!gFileReply.good)
				{
				fileStreamPtr->fileName[0] = 0;
																						return(0);
																						
				}		// end "if (!gFileReply.good)"
			
			if (!fileStreamPtr->fSSpecFlag)
				{	
						// Create the FSSpec record
						
				FSMakeFSSpec (gFileReply.vRefNum, 
									0, 
									gFileReply.fName, 
									(FSSpec*)fileStreamPtr);
				fileStreamPtr->fSSpecFlag = TRUE;
				
				}		// end "if (!fileStreamPtr->fSSpecFlag)"
				
//			CopyPToP ( (char*)fileStreamPtr->fileName, (char*)gFileReply.fName);
//			fileStreamPtr->vRefNum = gFileReply.vRefNum;
//			fileStreamPtr->parID = 0;
//			fileStreamPtr->fSSpecFlag = FALSE;
	
					// Also make the filename a useable c string
	
			fileStreamPtr->fileName[ fileStreamPtr->fileName[0]+1 ] = 0x00;
			
			}		// end "else localAppFilePtr == NULL" 
	
				// Get the file type and creator					
					
		if (fileStreamPtr->fSSpecFlag)
			errCode = FSpGetFInfo ( (FSSpec*)fileStreamPtr, &gFinderInfo);
			
		else		// !fileInfoPtr->fSSpecFlag 
			errCode = HGetFInfo (
					fileStreamPtr->vRefNum, 0, fileStreamPtr->fileName, &gFinderInfo);
	
		if (errCode == noErr)
			{
			fileStreamPtr->creator = gFinderInfo.fdCreator;
			fileStreamPtr->type = gFinderInfo.fdType;
	
			}		// end "if (errCode == noErr)"
			
	#endif	// !defined _X86_
	
	
	#if defined _X86_ 

		SInt16 filterStringIndex = IDS_FilterString;
		if (stringIndex == IDS_SelectProject)
			filterStringIndex = IDS_ProjectFilterString;
			
		else if (stringIndex == IDS_FileIO62)
			filterStringIndex = IDS_TransformFilterString;
			
		else if (stringIndex == IDS_FileIO76)
			filterStringIndex = IDS_TRLFilterString;
			
		else if (stringIndex == IDS_FileIO95)
			filterStringIndex = IDS_ClassificationFilterString;
			
		else if (stringIndex == IDS_FileIO96)
			filterStringIndex = IDS_ClassificationFilterString;
			
		else if (stringIndex == IDS_SelectNewBaseImage) 
			filterStringIndex = IDS_MultispectralFilterString;
			
		else if (stringIndex == IDS_FileIO105) 
			filterStringIndex = IDS_ClassificationFilterString;
			
		else if (stringIndex == IDS_SelectImageStatistics) 
			filterStringIndex = IDS_StatFilterString;
		
		if ( !GetSpecifiedStringNumber (
				kFileIOStrID, filterStringIndex, (Str255*)&gTextString, TRUE ) )
																				return (1);

		gFileFilterIndex = 1;
		if (filterStringIndex == IDS_FilterString)
			gFileFilterIndex = gImageFileFilterIndex;
	 
		CMOpenFileDialog		dialog(NULL);
		CString 					filePathName;
		
		
		if (localAppFilePtr != NULL)
			{
			
			}		// end "if (localAppFilePtr != NULL)"
			
		else		// localAppFilePtr == NULL
			{
			if (gMultipleImageFileCode)
				{
				gGetFileStatus = 2;
				
				}		// end "if (gMultipleImageFileCode)"
				
			else		// !gMultipleImageFileCode	
				{
				gGetFileStatus = -1;
				if (stringIndex == IDS_SelectImage)
					gGetFileStatus = 0; 
				
				}		// end "else !gMultipleImageFileCode"
				
			if ( !dialog.DoDialog(stringIndex) )
				{
				SetFileDoesNotExist(fileStreamPtr);
																						return(0);
																						
				}		// end "if ( !dialog.DoDialog() )"
			
			filePathName = dialog.GetPathName();
			localAppFilePtr = filePathName.GetBuffer(255);
			
			}		// end "else localAppFilePtr == NULL"
			
				// Get copy of the path name so that it can be modified.
				// Convert to Pascal string so that it is in the same format
				// as the file name in the Macintosh version.
				// Make certain that there is a C string terminator at the end.
				
		UChar pathName[_MAX_PATH];              	   
		CtoPstring ((CharPtr)localAppFilePtr, (CharPtr)pathName);
		pathName[ pathName[0]+1 ] = 0;
		
		fileStreamPtr->SetFilePath(pathName, TRUE);
		
		errCode = CMFileStream::GetFileType (
													pathName, &fileStreamPtr->mFileType);

				// Force the pascal file name to be updated in case user is
				// expecting to use it.

		fileStreamPtr->GetFileNamePPtr ();
			
	#endif	// defined _X86_
	
	if (errCode == noErr)
		errCode = OpenFileReadOnly (fileStreamPtr, 
											kResolveAliasChains,
											kLockFile);
					
	IOCheck (errCode, fileStreamPtr);
		
			// To allow this will also involve checking for this refnum in other	
			// windows when closing a file, because it is the same path as that	
			// obtained for the first open.													
				
//	if (errCode == opWrErr)
//		errCode = noErr;		
	
	if (errCode != noErr)	
		SetReferenceNumber(fileStreamPtr, 0);
		
	#endif	// !TARGET_API_MAC_CARBON
	
	return (errCode);

}		// end "GetFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetFileCreator
//
//	Software purpose:	The purpose of this routine is to return the file
//							creator represented by the input file stream pointer.
//
//	Parameters in:		File Stream Pointer
//
//	Parameters out:	None
//
//	Value Returned:	file creator
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt32 GetFileCreator (
				CMFileStream*						fileStreamPtr)

{  
	SInt32								fileCreator = -1;
	
	
	if (fileStreamPtr != NULL)
		{                               
		#if !defined _X86_                       
	                          
			fileCreator = fileStreamPtr->creator;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                            
		
			fileCreator = fileStreamPtr->mCreator;
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 

	return (fileCreator);
	
}		// end "GetFileCreator" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFilePathPPointer
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							information pointer. The output pointer will represent a
//							pascal pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1996
//	Revised By:			Larry L. Biehl			Date: 01/16/1996	

StringPtr GetFilePathPPointer (
				FileInfoPtr							fileInfoPtr)

{  
	StringPtr								fileNamePtr = NULL;
	
	
	if (fileInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr); 
		
		fileNamePtr = GetFilePathPPointer (fileStreamPtr);
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}		// end "GetFilePathPPointer"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFilePathPPointer
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input file
//							stream pointer. The output pointer will represent a
//							pascal pointer.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

StringPtr GetFilePathPPointer (
				CMFileStream* 						fileStreamPtr)

{                      
	StringPtr							filePathPtr = NULL;
	
	if (fileStreamPtr != NULL)
		{
		#if !defined _X86_                              
	                          
	   	filePathPtr = (StringPtr)&fileStreamPtr->fileName[0];
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                            
		
			filePathPtr = fileStreamPtr->GetFilePathPtr(); 
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 

	return (filePathPtr);
	
}		// end "GetFilePathPPointer"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFileNameCPointer
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list. It return a pointer to a C string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 09/11/1995	

StringPtr GetFileNameCPointer (
				FileInfoPtr							fileInfoPtr)

{  
	StringPtr								fileNamePtr = NULL;
	
	
	if (fileInfoPtr != NULL)
		{            
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		fileNamePtr = GetFileNameCPointer (fileStreamPtr);
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}		// end "GetFileNameCPointer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFileNameCPointer
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list. It return a pointer to a C string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1995
//	Revised By:			Larry L. Biehl			Date: 11/01/1995

StringPtr GetFileNameCPointer (
				CMFileStream*						fileStreamPtr)

{  
	StringPtr							fileNamePtr = NULL;
	
	
	if (fileStreamPtr != NULL)
		{                                  
		#if !defined _X86_                       
	                          
	   	fileNamePtr = (StringPtr)&fileStreamPtr->fileName[1];
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                             
		
			if (fileStreamPtr != NULL)
				fileNamePtr = fileStreamPtr->GetFileNamePtr(); 
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 

	return (fileNamePtr);
	
}		// end "GetFileNameCPointer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFileNamePPointer
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list. It return a pointer to a P string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995	

StringPtr GetFileNamePPointer (
				CMFileStream*						fileStreamPtr)

{  
	StringPtr							fileNamePtr = NULL;
	
	
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	fileNamePtr = (StringPtr)&fileStreamPtr->fileName[0];
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                            
		
			fileNamePtr = fileStreamPtr->GetFileNamePPtr(); 
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}		// end "GetFileNamePPointer"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetFileNamePPointer
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list. It return a pointer to a P string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1995
//	Revised By:			Larry L. Biehl			Date: 12/14/1998	

StringPtr GetFileNamePPointer (
				Handle								fileInfoHandle,
				SignedByte*							handleStatusPointer)

{  
	StringPtr							fileNamePtr = NULL;
	FileInfoPtr							fileInfoPtr;
	
				
	*handleStatusPointer = -1;
	
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer(fileInfoHandle,
																	handleStatusPointer,
																	kNoMoveHi);
																	
	if (fileInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		                                   
		#if !defined _X86_                       
	                          
	   	fileNamePtr = (StringPtr)&fileStreamPtr->fileName[0];
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                            
		
			if (fileStreamPtr != NULL)
				fileNamePtr = fileStreamPtr->GetFileNamePPtr(); 
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}		// end "GetFileNamePPointer" 
 


StringPtr GetFileNamePPointer (
				FileInfoPtr							fileInfoPtr)

{  
	StringPtr							fileNamePtr = NULL;
	
	
	if (fileInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		                                   
		#if !defined _X86_                       
	                          
	   	fileNamePtr = (StringPtr)&fileStreamPtr->fileName[0];
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                            
		
			if (fileStreamPtr != NULL)
				fileNamePtr = fileStreamPtr->GetFileNamePPtr(); 
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (fileNamePtr);
	
}		// end "GetFileNamePPointer"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetFileType
//
//	Software purpose:	The purpose of this routine is to return the file
//							type represented by the input file stream pointer.
//
//	Parameters in:		file stream pointer
//
//	Parameters out:	None
//
//	Value Returned:	the file type
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt32 GetFileType (
				CMFileStream*						fileStreamPtr)

{  
	SInt32								fileType = -1;
	
	
	if (fileStreamPtr != NULL)
		{                               
		#if !defined _X86_                       
	                          
			fileType = fileStreamPtr->type;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_                            
		
			fileType = fileStreamPtr->mFileType;
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 

	return (fileType);
	
}		// end "GetFileType"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetFilePositionOffset
//
//	Software purpose:	The purpose of this routine is to determine the
//							offset in the file for the input line, the number
//							of samples to read and the number of bytes to read.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/29/1996
//	Revised By:			Larry L. Biehl			Date: 01/31/2003

UInt32 GetFilePositionOffset (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr 						fileInfoPtr,
				UInt32								lineNumber, 
				SInt16								channelNumber, 
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32*								numberSamplesPtr,
				UInt32*								countPtr,
				Boolean*								endHalfBytePtr)

{
	HierarchalFileFormatPtr			hfaPtr;
	
	UInt32 								blockLineStart,
			 								halfBytes,
											posOff = 0,
											segmentedEndOffset,
											segmentedStartOffset;
			
	Boolean								BILSpecialFlag,
											startHalfByte;
	
	
			// Initialize local variables.
			
	BILSpecialFlag = fileIOInstructionsPtr->oneReadFlag;
				
	if (BILSpecialFlag)
		{
		columnStart = 1;
		columnEnd = fileInfoPtr->numberColumns;
		
		}		// end "if (BILSpecialFlag)"
	
			// Get the number of samples to read and the initial number of
			// bytes to read.
	
	*numberSamplesPtr = columnEnd - columnStart + 1;
	*countPtr =  *numberSamplesPtr * fileInfoPtr->numberBytes;
	*endHalfBytePtr = FALSE;
					 							
			// Get the relative position to start reading with respect to			
			// the start of the file.															
			
	switch (fileInfoPtr->bandInterleave)
		{
		case kBIL:
				
			posOff = fileInfoPtr->numberHeaderBytes +
			
									 	(lineNumber-1) *
									 			fileInfoPtr->bytesPer1lineAllChans +
									 				
										fileInfoPtr->numberPreLineBytes + 
									 			channelNumber *
													fileInfoPtr->bytesPer1line1chan +
											
										fileInfoPtr->numberPreChannelBytes + 
									 			(columnStart-1) * fileInfoPtr->numberBytes;
			
					// If 4-bit data and the number of columns in the image data	
					// is odd, then modify posOff to allow for 1/2 bytes.				
					
			if (fileInfoPtr->numberBits == 4)
				{
				halfBytes = (lineNumber-1) * 
						fileInfoPtr->numberChannels * fileInfoPtr->numberColumns +
										channelNumber * fileInfoPtr->numberColumns +	
																						columnStart - 1;
						
				startHalfByte = (Boolean)(halfBytes & 0x00000001);
				
				posOff -= (halfBytes + 1)/2;
				
						// Adjust the number of bytes to read in and determine		
						// end half byte flag.													
				
				*endHalfBytePtr = 0;
				*countPtr = (*countPtr + 1)/2;
				
				if (startHalfByte)
					*endHalfBytePtr = !(*numberSamplesPtr & 0x00000001);
									
				else		// !startHalfByte 
					*endHalfBytePtr = (Boolean)(*numberSamplesPtr & 0x00000001);
									
				if (startHalfByte && *endHalfBytePtr)
					(*countPtr)++;
				
				}		// end "if (fileInfoPtr->numberBits == 4)" 
	
					// Handle case when file band interleave format is BIL and all	
					// channels and columns are to be read in at one time.			
					
			if (BILSpecialFlag)
				{
				*numberSamplesPtr *= fileIOInstructionsPtr->bilSpecialNumberChannels;
				*countPtr *= fileIOInstructionsPtr->bilSpecialNumberChannels;
				
				}		// end "if (BILSpecialFlag)" 
				
			break;
		
		case kBSQ:
			posOff = fileInfoPtr->numberHeaderBytes +
								 				
								 	 channelNumber *
												fileInfoPtr->bytesPer1chanAllLines +
		
				 					fileInfoPtr->numberPreChannelBytes +
								 	  (lineNumber-1) *
								 			fileInfoPtr->bytesPer1line1chan +
								 		
								 		(columnStart-1) *
				 							fileInfoPtr->numberBytes +
				 									fileInfoPtr->numberPreLineBytes;
		
			break;
			
		case kBIS:
		
					// Note: For BIS type files all channels for the requested 		
					// columns will be read into the buffer.								
					
			posOff = fileInfoPtr->numberHeaderBytes +
			
									 (lineNumber-1) *
									 		fileInfoPtr->bytesPer1lineAllChans +
											
									fileInfoPtr->numberPreLineBytes + 
									 		(columnStart-1) * fileInfoPtr->numberChannels *
									 			fileInfoPtr->numberBytes;
									 			
			*countPtr *= fileInfoPtr->numberChannels;
			*numberSamplesPtr *= fileInfoPtr->numberChannels;
			
			if (fileInfoPtr->blockedFlag)
				{
				hfaPtr = fileInfoPtr->hfaPtr;
				
				blockLineStart = (lineNumber-1)/hfaPtr->blockHeight;
				posOff = hfaPtr->firstColumnStartByte + hfaPtr->blockOffset * 
											blockLineStart * hfaPtr->numberBlockWidths;
											
				*countPtr = hfaPtr->numberBlocksRead * hfaPtr->blockOffset;
				
				}		// end "if (fileInfoPtr->blockFlag)"
			
			break;
		
		case kBNonSQ:
			posOff = fileInfoPtr->hfaPtr[channelNumber].layerOffsetBytes +
				
						 					fileInfoPtr->numberPreChannelBytes +
										 	  (lineNumber-1) *
										 			fileInfoPtr->bytesPer1line1chan +
										 		
										 		(columnStart-1) *
						 							fileInfoPtr->numberBytes +
						 									fileInfoPtr->numberPreLineBytes;
			
			break;
		
		case kBNonSQBlocked:
		case kBIBlock:
			hfaPtr = &fileInfoPtr->hfaPtr[channelNumber];
			
			blockLineStart = (lineNumber-1)/hfaPtr->blockHeight;
			
			posOff = hfaPtr->firstColumnStartByte + hfaPtr->blockOffset * 
											blockLineStart * hfaPtr->numberBlockWidths;
			
			*countPtr = hfaPtr->numberBlocksRead * hfaPtr->blockOffset;
				
			break;
			
		}		// end "switch (fileInfoPtr->bandInterleave)"
		
	if (fileInfoPtr->format == kGAIAType)
		{
				// Allow for 'segmented bytes'.												
		
		segmentedStartOffset = GetNumberGAIALineSegments (columnStart) * 2;
		segmentedEndOffset = GetNumberGAIALineSegments (columnEnd) * 2;
		
		posOff += segmentedStartOffset;
		*countPtr += (segmentedEndOffset - segmentedStartOffset);
		
		}		// end "if (fileInfoPtr->format == kGAIAType)" 
		
	return (posOff); 
	
}		// end "GetFilePositionOffset" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetFileSize
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/03/2001
//	Revised By:			Larry L. Biehl			Date: 02/03/2001

UInt32 GetFileSize (
				CMFileStream*						fileStreamPtr)

{  
	UInt32								fileSize = 0;
	
	
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	fileSize = fileStreamPtr->fileSize;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_         
	                          
	   	fileSize = fileStreamPtr->mFileSize;
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (fileSize);
	
}		// end "GetFileSize" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLine
//
//	Software purpose:	The purpose of this routine is to read the
//							requested line and channel of image data into a
//							buffer.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1988
//	Revised By:			Larry L. Biehl			Date: 01/31/2003

SInt16 GetLine (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				FileInfoPtr							fileInfoPtr,
				UInt32								lineNumber, 
				SInt16								channelNumber,  
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32*								numberSamplesPtr, 
				HUCharPtr							fileIOBufferPtr)

{
	CMFileStream*						fileStreamPtr;
	HierarchalFileFormatPtr			hfaPtr = NULL;
	HUCharPtr							readBufferPtr; 
	
	UInt32								channelStartIndex,
											count,
											posOff,
											segmentedEndOffset,
											segmentedStartOffset;
	
	SInt16								errCode = noErr;
			
	Boolean								endHalfByte,
											readLineFlag = TRUE;
	
	  	
  			// Get total number of samples to be read from disk.						
  	
  	if (columnEnd < columnStart || lineNumber == 0 || columnEnd == 0)		
																					return (0);
																					
	if (fileInfoPtr->format == kGAIAType)
																					return (-1);
	
			// Initialize local variables.
				
	readBufferPtr = fileIOBufferPtr;
																					
	if (fileInfoPtr->blockedFlag)
		{
		if (fileInfoPtr->hfaPtr == NULL)
																					return (-1);
		
		channelStartIndex = 0;
		if (fileInfoPtr->bandInterleave == kBNonSQBlocked)
			{
			hfaPtr = &fileInfoPtr->hfaPtr[channelNumber];
			channelStartIndex = 0;
			
			}		// end "if (fileInfoPtr->bandInterleave == kBNonSQBlocked)"
			
		else		// fileInfoPtr->bandInterleave == kBIBlock || 
					//				fileInfoPtr->bandInterleave == kBIS
			{
			hfaPtr = fileInfoPtr->hfaPtr;
			
			if (fileInfoPtr->bandInterleave == kBIBlock)
				channelStartIndex = channelNumber;
			
					// Set channel number to start reading at the first channel
					
			channelNumber = 0;
			
			}		// end "else fileInfoPtr->bandInterleave == kBIBlock"
		
				// Check if line needs to be read from disk.
				
		if (lineNumber >= hfaPtr->firstLineRead && lineNumber <= hfaPtr->lastLineRead)
			readLineFlag = FALSE;
			
		if (readLineFlag)
			{
					// Set up correct buffer to read the data into. It will be
					// a buffer to hold several lines.
					
			readBufferPtr = (HUCharPtr)hfaPtr->tiledBufferPtr;
			
			}		// end "if (readLineFlag)"
			
		else		// !readLineFlag
			{
			*numberSamplesPtr = columnEnd - columnStart + 1;
			
			}		// end "else !readLineFlag"
		
		}		// end "if (fileInfoPtr->blockedFlag)"
			
	if (readBufferPtr == NULL)
																					return (-1);
													
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
	if (readLineFlag)
		{
		if (gUseThreadedIOFlag)
			{
			errCode = GetLineFileIOThread ( fileIOInstructionsPtr,
														fileInfoPtr, 
														lineNumber, 
														channelNumber, 
														columnStart, 
														columnEnd,
														numberSamplesPtr, 
														&fileIOBufferPtr);
														 
			count = fileIOInstructionsPtr->count;
			
			}		// end "if (gUseThreadedIOFlag)"
			
		else		// !gUseThreadedIOFlag
			{
					// Get the relative position to start reading with respect to			
					// the start of the file.					
					
			posOff = GetFilePositionOffset ( fileIOInstructionsPtr, 
														fileInfoPtr, 
														lineNumber, 
														channelNumber, 
														columnStart, 
														columnEnd,
														numberSamplesPtr,
														&count,
														&endHalfByte);
														
			errCode = MSetMarker (fileStreamPtr, fsFromStart, posOff, kNoErrorMessages);
			
			if (errCode == noErr)
					errCode = MReadData (fileStreamPtr, 
													&count, 
													readBufferPtr, 
													kNoErrorMessages);
													
			if (errCode == noErr && fileInfoPtr->blockedFlag)
				{
				hfaPtr->firstLineRead = 
							((lineNumber-1)/hfaPtr->blockHeight) * hfaPtr->blockHeight + 1;
				hfaPtr->lastLineRead = hfaPtr->firstLineRead + hfaPtr->blockHeight - 1;
				
//				hfaPtr->nextReadStartByte += 
//													hfaPtr->blockSize * hfaPtr->numberBlockWidths;
				
				}		// end "if (errCode == noErr && fileInfoPtr->blockedFlag)"
				
			}		// end "else !gUseThreadedIOFlag" 
			
		}		// end "if (readLineFlag)"												
	
	if (errCode == noErr)
		{
		if (fileInfoPtr->blockedFlag)
			{
			PackBlockedData (lineNumber,
									columnStart, 
									columnEnd,
									channelStartIndex,
									fileInfoPtr,
									hfaPtr,
									fileIOBufferPtr);
			
					// count is just used for 4-bit data from here on.
					
			if (fileInfoPtr->numberBits == 4)
				{						
				count = (columnEnd - columnStart + 2)/2;
				
				if ( !(columnStart & 0x00000001) && (columnEnd & 0x00000001) )
					count++;
				
				endHalfByte = FALSE;
				if (columnEnd & 0x00000001)
					endHalfByte = TRUE;
				
				}		// end "if (fileInfoPtr->numberBits == 4)"
			
			}		// end "if (fileInfoPtr->blockedFlag)"
			
		if (fileInfoPtr->format == kGAIAType)
			{
					// Remove the 'segmented bytes'.											
			
			segmentedStartOffset = GetNumberGAIALineSegments (columnStart) * 2;
			segmentedEndOffset = GetNumberGAIALineSegments (columnEnd) * 2;												
			
			PackGAIAData (columnStart, columnEnd, (HSInt16Ptr)fileIOBufferPtr);
			count -= (segmentedEndOffset - segmentedStartOffset);
			
			}		// end "if (fileInfoPtr->format == kGAIAType && ...)" 
				
				// Swap the bytes if needed. 
				
		if (fileInfoPtr->swapBytesFlag && fileInfoPtr->numberBytes == 2)
			SwapBytes ( (HUInt16Ptr)fileIOBufferPtr,
							*numberSamplesPtr);
		
		if (gConvertSignedDataFlag && fileInfoPtr->signedDataFlag)
			{
			if (fileInfoPtr->numberBytes == 1)
				{
				HPtr					byte1Ptr;
				
				UInt32				sampleCount;
				char					signedBitMask;
				char					signedValueMask;
				
				signedBitMask = (char)fileInfoPtr->signedBitMask;
				signedValueMask = (char)fileInfoPtr->signedValueMask;
				byte1Ptr = (HPtr)fileIOBufferPtr;
				
				for ( sampleCount=0; sampleCount<*numberSamplesPtr; sampleCount++ )
					{
					*byte1Ptr = (*byte1Ptr ^ signedBitMask);
					*byte1Ptr = (*byte1Ptr & signedValueMask);
					byte1Ptr++;
					
					}		// end "for ( sampleCount=0;  ..." 
					
				}		// end "if (fileInfoPtr->numberBytes == 1)" 
				
			else		// fileInfoPtr->numberBytes == 2 
				{
				HSInt16Ptr				twoBytePtr;
				
				UInt32					sampleCount;
				SInt16					signedBitMask;
				SInt16					signedValueMask;
				
				signedBitMask = fileInfoPtr->signedBitMask;
				signedValueMask = fileInfoPtr->signedValueMask;
				twoBytePtr = (HSInt16Ptr)fileIOBufferPtr;
				
				for ( sampleCount=0; sampleCount<*numberSamplesPtr; sampleCount++ )
					{
					*twoBytePtr = (*twoBytePtr ^ signedBitMask);
					*twoBytePtr = (*twoBytePtr & signedValueMask);
					twoBytePtr++;
					
					}		// end "for ( sampleCount=0; ..." 
					
				}		// end "else fileInfoPtr->numberBytes == 2" 
			
			}		// end "if (gConvertSignedDataFlag && ...)" 
			
				// If 4 bit data, convert to 8 bit data.									
				
		if (fileInfoPtr->numberBits == 4)
			{
			HPtr						byte1Ptr;
			HPtr						byte2Ptr;
			
			UInt32					samplesLeft;
	
					// If number of samples is odd, convert the odd one and then	
					// loop through the samples two at a time.							
					// Note the lower sample number is in the lower 4 bits.			
			
			samplesLeft = *numberSamplesPtr;
			byte1Ptr = (HPtr)&fileIOBufferPtr[samplesLeft-1];
			byte2Ptr = (HPtr)&fileIOBufferPtr[count-1];
			if ( endHalfByte )
				{
				*byte1Ptr = *byte2Ptr & 0x0f;
						
				samplesLeft--;
				byte1Ptr--;
				byte2Ptr--;
						
				}		// end "if ( halfByteFlag )" 
				
			while ( samplesLeft >= 2 )
				{
						// Move high 4 bits.														
						
				*byte1Ptr = (*byte2Ptr & 0xf0)>>4;
				
				samplesLeft--;
				byte1Ptr--;
				
						// Move low 4 bits.														
				
				*byte1Ptr = *byte2Ptr & 0x0f;
				
				samplesLeft--;
				byte1Ptr--;
				byte2Ptr--;
				
				}		// end "while ( samplesLeft >= 0 )" 
				
			if ( samplesLeft )
				*byte1Ptr = (*byte2Ptr & 0xf0)>>4;
			
			}		// end "if (fileInfoPtr->numberBits..." 
			
		}		// end "if (errCode == noErr)" 
		
	IOCheck (errCode, fileStreamPtr);
	return (errCode);
	
}		// end "GetLine"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLineOfData
//
//	Software purpose:	The purpose of this routine is to read
//							all data for a requested line and requested
//							channels of image data into a buffer that has been
//							provided.  If the channel list pointer is NULL, then
//							load the data in for all channels.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 10/25/1999

SInt16 GetLineOfData (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				UInt32								lineNumber, 
				UInt32								columnStart, 
				UInt32								columnEnd, 
				UInt32								columnInterval,
				HUCharPtr							fileIOBufferPtr, 
				HUCharPtr							dataBufferPtr)

{                                                
	FileInfoPtr							fileInfoPtr,
											localFileInfoPtr;
	
	HUCharPtr							ioBufferPtr;
											
	LayerInfoPtr						layerInfoPtr;
	
	UInt16*								channelListPtr;
	
	WindowInfoPtr						windowInfoPtr; 
	
	UInt32								bytesChannelOffset,
											columnOffset,
											fileNumberChannels,
											index,
											numberChannels,
											numberColumnsPerChannel,
											numberSamples,
											numberSamplesRead;
	
	SInt16								channel,
											errCode,
											fileInfoIndex,
											numberBytes;
											
	UInt16								numberOutBytes;
											
	Boolean								BILSpecialFlag,
											callPackLineOfDataFlag,
											differentBuffersFlag,
											packDataFlag,
											toBISFormatFlag;

	
	if (lineNumber <= 0  || fileIOBufferPtr == NULL)
																								return(-1);
		
			// Initialize some local variables.												
		
	numberSamples = columnEnd - columnStart + 1;
	
	channelListPtr = fileIOInstructionsPtr->channelListPtr;
	numberChannels = fileIOInstructionsPtr->numberChannels;
	windowInfoPtr = fileIOInstructionsPtr->windowInfoPtr;
	layerInfoPtr = fileIOInstructionsPtr->layerInfoPtr;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	packDataFlag = fileIOInstructionsPtr->packDataFlag; 
	toBISFormatFlag = fileIOInstructionsPtr->forceBISFormatFlag, 
	numberOutBytes = fileIOInstructionsPtr->forceByteCode;
	BILSpecialFlag = fileIOInstructionsPtr->oneReadFlag;
	callPackLineOfDataFlag = fileIOInstructionsPtr->callPackLineOfDataFlag;
	differentBuffersFlag = fileIOInstructionsPtr->differentBuffersFlag;		
	
	if (BILSpecialFlag)
		{
		columnOffset = columnStart - 1;
		numberColumnsPerChannel = fileInfoPtr->numberColumns;
		
		}		// end "if (BILSpecialFlag)"
		
	else		// !BILSpecialFlag
		{
		columnOffset = 0;
		numberColumnsPerChannel = numberSamples;
		
		}		// end "else !BILSpecialFlag"
																						
	if (fileIOInstructionsPtr->maskBufferPtr != NULL)
		{
		if ( !DetermineIfMaskDataInLine (
												fileIOInstructionsPtr->maskBufferPtr,
												fileIOInstructionsPtr->maskColumnStart,
												numberSamples,
												columnInterval,
												fileIOInstructionsPtr->maskValueRequest) )
																						return(2);
																						
		}		// end "if (fileIOInstructionsPtr->maskBufferPtr != NULL)"
												
			// If the data buffer is NULL, then this implies that the data
			// output buffer can be the same as that used for the file IO
			// buffer.
			
	if (dataBufferPtr == NULL)
		dataBufferPtr = fileIOBufferPtr;
		
			// Initialize rest of local variables.													
		
	bytesChannelOffset = 0;
	localFileInfoPtr = NULL;
	
			// Determine if the line of data will need to be packed.	
			// One last item to be checked. The rest have been checked in 
			// 'SetUpFileIOInstructions'.
			
	if (packDataFlag && BILSpecialFlag && numberColumnsPerChannel > numberSamples)
		callPackLineOfDataFlag = TRUE;
/*			
	callPackLineOfData = FALSE;
	differentBuffersFlag = FALSE;
	
	if (numberOutBytes == kForce2Bytes)
		{
		if (windowInfoPtr->localMaxNumberBytes == 1 || 
															windowInfoPtr->localBytesDifferFlag)
			{
			callPackLineOfData = TRUE;
			differentBuffersFlag = TRUE;
			
			}		// end "if (windowInfoPtr->localMaxNumberBytes == 1 || ...)" 
			
		}		// end "if (numberOutBytes == kForce2Bytes)" 
	
	else if (numberOutBytes == kForce4Bytes)
		{
		callPackLineOfData = TRUE;
		differentBuffersFlag = TRUE;
			
		}		// end "else if (numberOutBytes == kForce4Bytes)" 
																
	if (fileInfoPtr->bandInterleave != kBIS)
		{
		if (packDataFlag)
			{
			if (columnInterval > 1)
				callPackLineOfData = TRUE;
			
			else if (BILSpecialFlag && (SInt32)numberChannels < 
												fileIOInstructionsPtr->bilSpecialNumberChannels)
				callPackLineOfData = TRUE;
			
			else if (BILSpecialFlag && numberColumnsPerChannel > numberSamples)
				callPackLineOfData = TRUE;
				
			}		// end "if (packDataFlag)"
			
		if (toBISFormatFlag)
			{
			callPackLineOfData = TRUE;
			differentBuffersFlag = TRUE;
			
			}		// end "if (toBISFormatFlag)" 
			
		}		// end "if (fileInfoPtr->bandInterleave != kBIS)" 
				
			// Do channel and sample packing for kBIS formats here.					
	
	else		// fileInfoPtr->bandInterleave == kBIS 
		{
		if (packDataFlag && 
				(numberChannels<fileInfoPtr->numberChannels || columnInterval>1) )
			callPackLineOfData = TRUE;
			
		}		// end "else fileInfoPtr->bandInterleave == kBIS" 
*/		
			// Check for error case when data is to be packed but the input and	
			// output buffers are the same.													
			
	if ( (fileIOBufferPtr == dataBufferPtr) && differentBuffersFlag)
																						return(-1);
		
			// Get the buffer to read the data into.										
			// Check for special case when data is to be packed but no effort is	
			// needed to do it other than read data into the data buffer.				
			
	ioBufferPtr = fileIOBufferPtr;
	if (packDataFlag && !callPackLineOfDataFlag)
		ioBufferPtr = dataBufferPtr;
		
			// Get the number of bytes in the data to be returned.					
			
	numberBytes = windowInfoPtr->localMaxNumberBytes;
	if (numberOutBytes == kForce2Bytes)
		numberBytes = 2;
		
	else if (numberOutBytes == kForce4Bytes)
		numberBytes = 4;
		
	fileNumberChannels = 0;
	fileInfoIndex = -1;
	channel = 0;
	
			// Loop through the channels to be read in.									
			
	for ( index=0; index<numberChannels; index++)
		{														
		if (channelListPtr != NULL)
			channel = channelListPtr[index];
		
		channel++;
		if (	fileInfoIndex != (SInt16)layerInfoPtr[channel].fileInfoIndex &&
				callPackLineOfDataFlag &&
				localFileInfoPtr != NULL &&
				localFileInfoPtr->bandInterleave != kBIS)
			{
					// Pack the data in the line leaving the unused columns out 	
					// and/or convert to BIS format if either has been 				
					// requested.  Also repack if 1 byte data is to be 				
					// converted to 2 byte data. Update the data buffer pointer.
					// Note that input BIS data does	not need to be packed here since
					// all channels were read at one time. Packing for that data will
					// be done outside the channel loop.
					// Note that for this call of PackNonBISData, the "channelListPtr"
					// parameter (the 4) one will never be used
				
			PackNonBISData ( columnOffset,
									columnInterval,
									numberColumnsPerChannel,
									(UInt32)fileNumberChannels, 
									(UInt32)numberChannels,
									NULL,		// "channelListPtr"
									numberSamples,
									(UInt32)localFileInfoPtr->numberBytes,
									fileIOBufferPtr, 
									dataBufferPtr, 
									toBISFormatFlag,
									(UInt32)numberOutBytes,
									localFileInfoPtr->swapBytesFlag,
									localFileInfoPtr->signedDataFlag);
			
			if (toBISFormatFlag)
				dataBufferPtr = 
						&dataBufferPtr[(SInt32)numberBytes * fileNumberChannels];
			
			else		// !toBISFormatFlag 						
				dataBufferPtr = &dataBufferPtr[(SInt32)numberBytes * 
					fileNumberChannels * ((numberSamples + columnInterval - 1)/
																					columnInterval)];
					
			ioBufferPtr = (UCharPtr)fileIOBufferPtr;
									
			fileNumberChannels = 0;
			
			}		// end "if (fileInfoIndex != ..." 
			
		fileInfoIndex = layerInfoPtr[channel].fileInfoIndex;
		localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
		
		errCode = GetLine (fileIOInstructionsPtr,
									localFileInfoPtr,
									lineNumber,
									layerInfoPtr[channel].fileChannelNumber-1,
									columnStart,
									columnEnd,
									&numberSamplesRead,
									ioBufferPtr);
											
				// Exit routine if errCode is not noErr.								
				
		if (errCode != noErr)
																			return (errCode);
																			
		if (numberSamplesRead <= 0)
																			return (-1);
								
		bytesChannelOffset = numberSamplesRead*localFileInfoPtr->numberBytes;
		
				// Update buffer pointer to read in the next channel					
					
      ioBufferPtr = &ioBufferPtr[bytesChannelOffset];
      
      fileNumberChannels++;
      
      		// If the image file band interleave is by sample, indicate that	
      		// all channels have been read.  All channels for a line are read	
      		// by GetLine if the band interleave is BIS.	 Also if the read		
      		// was a special case of BIL format then all the data for all		
      		// channels was read at one time.											
      		
      if (localFileInfoPtr->bandInterleave == kBIS || BILSpecialFlag)
      	fileNumberChannels = index = numberChannels;
      			
      }		// end "for ( index=0; index<numberChannels..."
	
			// Repack the line of data	if needed.											
	
	if (callPackLineOfDataFlag)
		{															
		if (fileInfoPtr->bandInterleave != kBIS)
			{
			if (!BILSpecialFlag || numberChannels == 
											fileIOInstructionsPtr->bilSpecialNumberChannels)
				channelListPtr = NULL;
			
					// Pack the data in the line leaving the unused columns out 	
					// and/or convert to BIS format if either has been 				
					// requested.  Also repack if 1 byte data is to be 				
					// converted to 2 byte data or 1 or 2 byte data is to be converted
					// to 4 byte data. Update the data buffer pointer.	
				
			PackNonBISData ( columnOffset,
									columnInterval,
									numberColumnsPerChannel,
									(UInt32)fileNumberChannels, 
									(UInt32)numberChannels,
									(HUInt16Ptr)channelListPtr,
									numberSamples,
									(UInt32)localFileInfoPtr->numberBytes,
									fileIOBufferPtr, 
									dataBufferPtr, 
									toBISFormatFlag,
									(UInt32)numberOutBytes,
									localFileInfoPtr->swapBytesFlag,
									localFileInfoPtr->signedDataFlag);
				
			}		// end "if ( fileInfoPtr->bandInterleave!=kBIS)" 
					
				// Do channel and sample packing for BIS formats here.					
		
		else		// fileInfoPtr->bandInterleave == kBIS 
			{								
			PackBISData (	columnInterval,
								localFileInfoPtr->numberChannels, 
								numberChannels, 
								numberSamples,
								localFileInfoPtr->numberBytes, 
								fileIOBufferPtr,
								dataBufferPtr, 
								(HUInt16Ptr)channelListPtr,
								numberOutBytes,
								fileInfoPtr->signedDataFlag);
									
			}		// end "else fileInfoPtr->bandInterleave==kBIS" 
			
		}		// end "if (callPackLineOfData)" 
	
			// If mask buffer is being used then get just those data values requested
			// by the mask. Note that this is currently only set up for BIS output
			// data.
							
	if (fileIOInstructionsPtr->maskBufferPtr != NULL)
		fileIOInstructionsPtr->numberOutputBufferSamples = PackMaskData (
													fileIOInstructionsPtr->maskBufferPtr,
													fileIOInstructionsPtr->maskColumnStart,
													numberChannels,
													numberSamples,
													columnInterval,
													dataBufferPtr,
													numberOutBytes,
													fileIOInstructionsPtr->maskValueRequest);
    
    return (noErr);
	      
}		// end "GetLineOfData" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetNumberGAIALineSegments
//
//	Software purpose:	The purpose of this is to compute the number of
//							'segments' in a line of data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None	
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1994
//	Revised By:			Larry L. Biehl			Date: 05/03/1995

SInt32 GetNumberGAIALineSegments (
				SInt32								numberColumns)

{
//	return (numberColumns/1022); 
	
	return ( (numberColumns + 5)/1021 );

}		// end "GetNumberGAIALineSegments" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetParID
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							directory id.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/17/2001
//	Revised By:			Larry L. Biehl			Date: 12/17/2001	

SInt32 GetParID (
				FileInfoPtr							fileInfoPtr)

{  
	SInt32								dirID = 0;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer(fileInfoPtr);
	
	
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
			dirID = fileStreamPtr->parID;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 
	

	return (dirID);
	
}		// end "GetParID"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetParID
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002	

SInt32 GetParID (
				Handle								fileInfoHandle)

{  
	SInt32								dirID = 0;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
													fileInfoHandle, kNoLock, kNoMoveHi);
													
	dirID = GetParID (fileInfoPtr);

	return (dirID);
	
}		// end "GetParID"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetReferenceNumber (
				CMFileStream*						fileStreamPtr)

{  
	SInt16								refNum = 0;
	
	
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	refNum = fileStreamPtr->refNum;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 

	return (refNum);
	
}		// end "GetReferenceNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		long int GetShortIntValue
//
//	Software purpose:	The purpose of this routine is to obtain the shot int
//							value from the input.  The bytes will be swapped
//							if needed depending upon the global setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/17/1995
//	Revised By:			Larry L. Biehl			Date: 04/17/1995	

SInt16 GetShortIntValue (
				CharPtr								shortIntPtr)

{
	SInt16								tempInt;
	

	if (gSwapBytesFlag)
		{
		memcpy ( (Ptr)&tempInt, (Ptr)shortIntPtr, 2);
		return ( ((tempInt & 0xff00) >> 8) | ((tempInt & 0x00ff) << 8) );
		
		}		// end "if (gSwapBytesFlag)"
		
	else		// !gSwapBytesFlag
		return ( *((short int*)shortIntPtr) );

}		// end "GetShortIntValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to return the size
//							of the specified file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 10/20/1995 
                       
SInt16 GetSizeOfFile (
				FileInfoPtr							fileInfoPtr,
				UInt32*								sizeBytesPtr)

{ 						
	CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
	return( GetSizeOfFile(fileStreamPtr, sizeBytesPtr) );
		      
}		// end "GetSizeOfFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to return the size
//							of the specified file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 12/15/2003 
                       
SInt16 GetSizeOfFile (
				CMFileStream*						fileStreamPtr,
				UInt32*								sizeBytesPtr)

{  
	UInt32								fileSize;
	
	SInt16								errCode = noErr,
											errCode2 = noErr;
											
	Boolean								closeFileFlag = FALSE;
	
	
			// Initialize local variables.													
			
	fileSize = 0;

			// Determine if file needs to be opened.
			
	if ( !FileOpen (fileStreamPtr) )
		{		
	   errCode = OpenFileReadOnly(
	   						    fileStreamPtr,
	   						    kDontResolveAliasChains,
	   						    kNoLockFile);
	   						    
	   closeFileFlag = TRUE;
	   
	   }		// end "if ( !FileOpen (fileStreamPtr) )" 
	
	if (errCode == noErr)
		{      
		#if !defined _X86_ 
		
					// Note that the maximum file size that GetEOF will handle
					// is 2,147,483,648 bytes.
					// Although on 12/16/03, it appears the max returned is 2,113,506,817
					// for a file that is over 6 gigabytes.
					
			errCode2 = GetEOF (fileStreamPtr->refNum, (SInt32*)&fileSize);
			
			if (errCode == noErr)
				fileStreamPtr->fileSize = fileSize;
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_ 
			errCode2 = fileStreamPtr->MGetSizeOfFile (&fileSize); 
			
			if (errCode == noErr)
				fileStreamPtr->mFileSize = fileSize;
		#endif	// defined _X86_ 
		
		IOCheck (errCode2, fileStreamPtr);              
		
		if (closeFileFlag)
			CloseFile (fileStreamPtr);
		
		}		// end "if (errCode == noErr)" 
                                             
		
	if (errCode != fnfErr)  
		IOCheck (errCode, fileStreamPtr);
	
	if (errCode == noErr)
		errCode = errCode2;
		
	if (sizeBytesPtr != NULL)
		*sizeBytesPtr = fileSize;
							 
	return (errCode);
		      
}		// end "GetSizeOfFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetSizeOfImage
//
//	Software purpose:	The purpose of this routine is to compute the size
//							(in bytes) of the specified image with no channel
//							descriptions or class to group descriptions at the end.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			ReadThematicGroups in displayThematic.c
//							ReadMultiSpecChannelDescriptionsAndValues in openImageRoutines.c
//							ModifyChannelDescriptions in reformat.c
//							WriteChannelDescriptionsAndValues in reformat.c
//							WriteThematicGroups in saveWrite.c
//
//	Coded By:			Larry L. Biehl			Date: 09/15/1992
//	Revised By:			Larry L. Biehl			Date: 05/25/1993

SInt32 GetSizeOfImage (
				FileInfoPtr							fileInfoPtr)

{
	SInt32								sizeOfImage;
	
	
	sizeOfImage = 0;
	
	if (fileInfoPtr != NULL)
		{
				// Get the size of the image without any channel descriptions.		
		
		sizeOfImage = 
					fileInfoPtr->bytesPer1lineAllChans * fileInfoPtr->numberLines;
					
		if (fileInfoPtr->numberBits == 4)
			sizeOfImage = (sizeOfImage +1)/2;
					
		sizeOfImage += 
				fileInfoPtr->numberHeaderBytes + fileInfoPtr->numberTrailerBytes;
					
		}		// end "if (fileInfoPtr != NULL)" 
					
	return (sizeOfImage);

}		// end "GetSizeOfImage"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetTRLSupportFile
//
//	Software purpose:	This routine gets a trl file ready for writing information
//							to.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	File IO error code
//
// Called By:			CreateTRLSupportFile in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/11/1996
//	Revised By:			Larry L. Biehl			Date: 06/19/2001	

SInt16 GetTRLSupportFile (
				FileInfoPtr							gisFileInfoPtr,
				CMFileStream*						trailerStreamPtr,
				Boolean								promptFlag)

{
	StringPtr							gisFilePathPtr,
											trailerFilePathPtr;
										
	CMFileStream*						gisFileStreamPtr;
	
	SInt16								errCode;
	
	
	gisFileStreamPtr = GetFileStreamPointer(gisFileInfoPtr);
					
			// Get the pointer to the file stream block and initialize it.	

	gisFilePathPtr = GetFilePathPPointer(gisFileInfoPtr);
	trailerFilePathPtr = GetFilePathPPointer(trailerStreamPtr);
			
			// Get the output file name and volume to write file to.	Make the 
			// default name the same as the .gis file name with .trl at the	
			// end.  First make sure that we have a pointer cursor. Also 		
			// Check if there is enough space on the selected volume.			
		
	CopyPToP ((char*)trailerFilePathPtr, (char*)gisFilePathPtr);
	RemoveSuffix (trailerFilePathPtr);
	
			// If Windows system, verify that final name will meet DOS
			// requirements.
			
	VerifyDOSFileName ( (UCharPtr)trailerFilePathPtr );
		
	ConcatFilenameSuffix (trailerFilePathPtr, (StringPtr)"\0.trl\0");																		
				
	SetType(trailerStreamPtr, kITRLFileType);
	SInt16 vRefNum = GetVolumeReferenceNumber (gisFileStreamPtr);
		
				// Get the trl file. If it is a new file then create it.	
	
	if (promptFlag)
		{
		UInt32 numberBytes = 7*128;
		numberBytes += 9*128;
		numberBytes += ((gisFileInfoPtr->numberClasses+4)/4) * 128;
		numberBytes += 500;		// Spare
		
		errCode = PutFile (
				trailerStreamPtr, numberBytes, IDS_FileIO75, gCreator);
				
		}		// end "if (promptFlag)"
				
	else		// !promptFlag 
		{
		SetReferenceNumber(trailerStreamPtr, 0);
		SetVolumeReference (gisFileStreamPtr, trailerStreamPtr);
		#if !defined _X86_ 
    		trailerStreamPtr->fSSpecFlag = gisFileStreamPtr->fSSpecFlag;
		#endif	// !defined _X86_ 
    	
		errCode = CreateNewFile (trailerStreamPtr, 
											vRefNum, 
											gCreator, 
											kErrorMessages,
											kReplaceFlag);
		
		}		// end "else !promptFlag"
								
				// Set position to start writing data at the start of the file.
		
	if ( (errCode == noErr) && FileExists(trailerStreamPtr) )
		errCode = MSetMarker (trailerStreamPtr, fsFromStart, 0, kErrorMessages);
		
	return (errCode);

}		// end "GetTRLSupportFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//							 Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				SInt16 GetVolumeFreeSpace
//
//	Software purpose:			The purpose of this routine is to obtain the number
//									of bytes that are free for the specified volume.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1989
//	Revised By:			Larry L. Biehl			Date: 09/13/1995

SInt16 GetVolumeFreeSpace (
				Str255*								fileNamePtr, 
				SInt16								vRefNum, 
				UInt32*								freeBytesPtr)

{
	SInt16								errCode;
	 
		
	#if !defined _X86_
//		SInt16								tmpVRefNum;
		
		HParmBlkPtr							hParmBlkPtr;
		
//		SInt16								drvNum;
		
				
				// Initialize local variables.													
				
		*freeBytesPtr = 0;
		errCode = 1;
		
				// Consider XParamBlockRec for greater than 2 gigabytes.
				//		and PBXGetVInfoSync
						
		hParmBlkPtr = (HParmBlkPtr)MNewPointer ( sizeof(HParamBlockRec) );
		if (hParmBlkPtr != NULL)
			{																
			hParmBlkPtr->volumeParam.ioCompletion = NULL;
			hParmBlkPtr->volumeParam.ioNamePtr = NULL;
			hParmBlkPtr->volumeParam.ioVRefNum = vRefNum;
			hParmBlkPtr->volumeParam.ioVolIndex = 0;
//			errCode = PBHGetVInfo ( hParmBlkPtr, FALSE);
			errCode = PBHGetVInfoSync (hParmBlkPtr);
			
			if (errCode == noErr)
				{  
//				drvNum = hParmBlkPtr->volumeParam.ioVDrvInfo;
				
				*freeBytesPtr = (UInt32)hParmBlkPtr->volumeParam.ioVFrBlk * 
													hParmBlkPtr->volumeParam.ioVAlBlkSiz;
				
				}		// end "if (errCode == noErr)"
				
			DisposePtr ( (Ptr)hParmBlkPtr );
			
//			if (errCode == noErr)
//				errCode = GetVInfo ( drvNum, 
//									gTextString, 
//									&tmpVRefNum, 
//									(SInt32*)freeBytesPtr);
									
			IOCheck (errCode, (CharPtr)fileNamePtr);
			
			}		// end "if (hParmBlkPtr != NULL)"
	
	#endif	// !defined _X86_
	
	#if defined _X86_
		
		*freeBytesPtr = 0;
		errCode = -1;    

				// fill disk free information
				
		struct _diskfree_t diskfree;                                      
		#if !defined WIN32                                           
			if (_dos_getdiskfree(_getdrive(), &diskfree) == 0)
		#else
			if (_getdiskfree(_getdrive(), &diskfree) == 0)
		#endif	// !defined WIN32
				{
				*freeBytesPtr = 
					(DWORD)diskfree.avail_clusters *
							(DWORD)diskfree.sectors_per_cluster *
										(DWORD)diskfree.bytes_per_sector; 
				errCode = noErr;
				
				}		// end "if (..."
		
	#endif	// defined _X86_
	
	return (errCode);
		
}		// end "GetVolumeFreeSpace"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetVolumeReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetVolumeReferenceNumber (
				CMFileStream*						fileStreamPtr)

{  
	SInt16								vRefNum = 0;
	
	
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	vRefNum = fileStreamPtr->vRefNum;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileInfoPtr != NULL)" 

	return (vRefNum);
	
}		// end "GetVolumeReferenceNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetVolumeReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/13/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetVolumeReferenceNumber (
				FileInfoPtr							fileInfoPtr)

{  
	SInt16								vRefNum = 0;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer(fileInfoPtr);
	
	vRefNum = GetVolumeReferenceNumber (fileStreamPtr);

	return (vRefNum);
	
}		// end "GetVolumeReferenceNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetVolumeReferenceNumber
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							volume reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

SInt16 GetVolumeReferenceNumber (
				Handle								fileInfoHandle)

{  
	SInt16								vRefNum = 0;
	
	
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer(
													fileInfoHandle, kNoLock, kNoMoveHi);
													
	vRefNum = GetVolumeReferenceNumber (fileInfoPtr);

	return (vRefNum);
	
}		// end "GetVolumeReferenceNumber"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void IndicateFileClosed
//
//	Software purpose:	The purpose of this routine is to indicate in the input
//							file stream structure that the file is closed and
//							should not be accessed any more.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/30/1997
//	Revised By:			Larry L. Biehl			Date: 06/02/1997	

void IndicateFileClosed (
				CMFileStream*						fileStreamPtr)

{  		
	if (fileStreamPtr != NULL)
		{	 
		#if !defined _X86_
		                                                                          
			fileStreamPtr->refNum = 0;
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_	
          
			fileStreamPtr->m_hFile = CFile::hFileNull;
//         fileStreamPtr->Abort();
		
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL && ..."

}		// end "IndicateFileClosed"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeFileIOInstructions
//
//	Software purpose:	The purpose of this routine is to set up a FileIO thread.
//
//	Parameters in:				
//
//	Parameters out:	
//
//	Value Returned:	 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/31/1996
//	Revised By:			Larry L. Biehl			Date: 10/25/1999

void InitializeFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
			// Load File IO Instructions structure.
			
	fileIOInstructionsPtr->asyncIOThread = 0;
	fileIOInstructionsPtr->wakeUpThread = 0;
	fileIOInstructionsPtr->theAppRef = 0;
	
	fileIOInstructionsPtr->count = 0;
	fileIOInstructionsPtr->posOff = 0;
	
	fileIOInstructionsPtr->bufferUsedForIO = 0;
	fileIOInstructionsPtr->bufferUsedForApp = 0;
	fileIOInstructionsPtr->errCode = noErr;

	fileIOInstructionsPtr->bufferReadyFlag = FALSE;
	fileIOInstructionsPtr->doneFlag = FALSE;
	fileIOInstructionsPtr->fileIOThreadStartedFlag = FALSE;
	
}		// end "InitializeFileIOInstructions"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle InitializeFileStream
//
//	Software purpose:	The purpose of this routine is to get memory if needed for
//							a File Stream Structure and initialize it.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 06/04/1996	
                   
#if !defined _X86_
Handle InitializeFileStream (
				Handle								fileStreamHandle)

{ 
	CMFileStream*						fileStreamPtr;
	SignedByte							handleStatus;
								
	
	if (fileStreamHandle == NULL)     
		fileStreamHandle = MNewHandle ( sizeof(CMFileStream) );
	
	if (fileStreamHandle != NULL)
		fileStreamPtr = (CMFileStream*)
				GetHandleStatusAndPointer (fileStreamHandle, &handleStatus, kNoMoveHi);
	
				// Initialize the some variables and handles in the structure.
		
	if (fileStreamPtr != NULL)
		{		
		InitializeFileStream (fileStreamPtr);	
		
		MHSetState(fileStreamHandle, handleStatus);
	
		}		// end "fileStreamPtr != NULL" 
		
	return (fileStreamHandle);

}		// end "InitializeFileStream"
#endif	// !defined _X86_ 
      
                      
		           	
CMFileStream* InitializeFileStream (
				CMFileStream*						fileStreamPtr)

{     		         
	#if !defined _X86_ 
	
		if (fileStreamPtr == NULL) 
			fileStreamPtr = (CMFileStream*)MNewPointer( sizeof(CMFileStream) );
			
		if (fileStreamPtr != NULL)
			{      
					// Initialize the  structure.			
				
			fileStreamPtr->vRefNum = 0;
			fileStreamPtr->parID = 0;
			fileStreamPtr->fileName[0] = 0;
			fileStreamPtr->creator = -1;
			fileStreamPtr->type = 'TEXT';
			fileStreamPtr->refNum = 0;
			fileStreamPtr->fSSpecFlag = FALSE;
			fileStreamPtr->fileSize = 0;
			
			}		// end "if (fileStreamPtr != NULL)"	
			
	#endif	// !defined _X86_  
	
	
	#if defined _X86_
	
		if (fileStreamPtr == NULL) 
			fileStreamPtr = new CMFileStream;
			
		else		// fileStreamPtr != NULL
			{
			fileStreamPtr->MCloseFile(); 
			fileStreamPtr->mFilePathName[0] = 0;
			fileStreamPtr->mCreator = -1;
			fileStreamPtr->mFileType = -1;
			fileStreamPtr->mFileSize = 0;
					
			}		// end "if (fileStreamPtr != NULL)"
			
	#endif	// defined _X86_  
		
	return (fileStreamPtr);        

}		// end "InitializeFileStream" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle InitializeFileStream
//
//	Software purpose:	The purpose of this routine is to intialize a new file stream
//							structure using the information from another input file
//							stream structure. The new file stream structure is left
//							with no file being opened even if the input file stream
//							represents a file that is opened.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

void InitializeFileStream (
				CMFileStream*						newFileStreamPtr,
				CMFileStream*						oldFileStreamPtr)

{
	if (newFileStreamPtr != NULL && oldFileStreamPtr != NULL)
		{
		#if !defined _X86_
		
					// variables from the 'old' file stream to the 'new' file stream.			
				
			newFileStreamPtr->vRefNum = oldFileStreamPtr->vRefNum;
			newFileStreamPtr->parID = oldFileStreamPtr->parID;
			CopyPToP ( (char*)newFileStreamPtr->fileName, (char*)oldFileStreamPtr->fileName );
			newFileStreamPtr->creator = oldFileStreamPtr->creator;
			newFileStreamPtr->type = oldFileStreamPtr->type;
			newFileStreamPtr->refNum = 0;	// oldFileStreamPtr->refNum;
			newFileStreamPtr->fSSpecFlag = oldFileStreamPtr->fSSpecFlag;
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_	
         
         StringPtr	oldFilePathPtr = oldFileStreamPtr->GetFilePathPtr();              
			newFileStreamPtr->SetFilePath (oldFilePathPtr, TRUE);
			newFileStreamPtr->mCreator = oldFileStreamPtr->mCreator;
			newFileStreamPtr->mFileType = oldFileStreamPtr->mFileType;
		
		#endif	// defined _X86_
		
		}		// end "if (newFileStreamPtr != NULL && ..."

}		// end "InitializeFileStream"                                      



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void IOCheck
//
//	Software purpose:	The purpose of this routine is to check for IO 
//							Errors and post a message if one has occured.
//		
//	Parameters in:		File operation error code.
//							Address of file name.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/1997

void IOCheck (
				SInt16								errCode,
				char*									fileNamePtr)
                
{		
	if (errCode != noErr)
		{                   
		#if !defined _X86_
		
			SInt16							returnCode,
												stringIndex;
			
			                     
					// Get error alert string.														
					
			GetIndString (gTextString, kFileIOStrID, kFileIOStr25);
			
					// Append the file name to the string.										
			
			ConcatPStrings (gTextString, (StringPtr)fileNamePtr, 255);
			ConcatPStrings (gTextString, (StringPtr)"\0'.  \0", 255);
			
					// Convert the error code to a string.										
					
			NumToString ( (long int)errCode, gTextString2);
			
					// Get the error description string.										
			
			stringIndex = 0;
			if (errCode == bdNamErr)			
				stringIndex = kFileIOStr15;
			else if (errCode == dirFulErr)	
				stringIndex = kFileIOStr16;
			else if (errCode == dskFulErr)	
				stringIndex = kFileIOStr17;
			else if (errCode == dupFNErr)		
				stringIndex = kFileIOStr18;
			else if (errCode == eofErr)		
				stringIndex = kFileIOStr19;
			else if (errCode == fLckdErr)		
				stringIndex = kFileIOStr20;
			else if (errCode == fnOpnErr)		
				stringIndex = kFileIOStr21;
			else if (errCode == tmfoErr)		
				stringIndex = kFileIOStr22;
			else if (errCode == wPrErr)		
				stringIndex = kFileIOStr23;
			else if (errCode == opWrErr)		
				stringIndex = kFileIOStr24;
			else if (errCode == nsvErr)		
				stringIndex = 26;
			else if (errCode == wrPermErr)	
				stringIndex = 32;
			else if (errCode == fnfErr)		
				stringIndex = 33;
			else if (errCode == ioErr)		
				stringIndex = 91;
			else if (errCode == notEnoughMemoryErr)		
				stringIndex = 64;
			
			if (stringIndex != 0)
				GetIndString (gTextString3, kFileIOStrID, stringIndex);
				
			else		// stringIndex == 0
				gTextString3[0] = 0;
			
			ParamText ( 	(ConstStr255Param)&gTextString, 
								(ConstStr255Param)&gTextString2, 
								"\p: ", 
								(ConstStr255Param)&gTextString3);
			InitCursor();
			returnCode = DisplayAlert (kErrorAlertID, 3, 0, 0, 0, NULL);
			                  
		#endif	// !defined _X86_


		#if defined _X86_

			Str31 				tempString;
	 
   
			switch (errCode)
				{        
				case CFileException::generic:
					MessageBox(NULL, "Unspecified Error Occurred", NULL, MB_OK); 
					
					#ifdef _DEBUG  
		   			afxDump << "Unspecified error occurred " << errCode << "\n";
		   		#endif
		   		break;           
		   		    
				case CFileException::fileNotFound:
					MessageBox(NULL, "File Could Not be Found", NULL, MB_OK); 
					
					#ifdef _DEBUG  
		   			afxDump << "File could not be found " << errCode << "\n";
		   		#endif
		   		break;           
		   		
				case CFileException::badPath:
					MessageBox(NULL, "Path is Invalid", NULL, MB_OK);
					
					#ifdef _DEBUG   
		   			afxDump << "Path is invalid " << errCode << "\n";
		   		#endif
		   		break;     
		   		
				case CFileException::tooManyOpenFiles:
					MessageBox(NULL, "Too Many Open Files", NULL, MB_OK); 
					
					#ifdef _DEBUG  
		   			afxDump << "Too Many Open Files " << errCode << "\n";
		   		#endif
		   		break;
		   		
				case CFileException::accessDenied:
					MessageBox(NULL, "File Access Denied", NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "File access denied " << errCode << "\n";
		   		#endif
		   		break;
		   		
				case CFileException::invalidFile:
					MessageBox(NULL, "Invalid File Handle", NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "Invalid File Handle " << errCode << "\n";
		   		#endif
		   		break;
		   		
				case CFileException::badSeek:
					MessageBox(NULL, "Error Setting File Pointer", NULL, MB_OK);
					
					#ifdef _DEBUG 
		   			afxDump << "Error setting file pointer " << errCode << "\n";
		   		#endif
		   		break;   
		   		
				case CFileException::hardIO:
					MessageBox(NULL, "Hardware error", NULL, MB_OK);  
					
					#ifdef _DEBUG 
		   			afxDump << "Hardware error " << errCode << "\n";
		   		#endif
		   		break;   
		   		
				case CFileException::sharingViolation:
					MessageBox(NULL, "Sharing Violation", NULL, MB_OK);  
					
					#ifdef _DEBUG 
		   			afxDump << "Sharing violation " << errCode << "\n";
		   		#endif
		   		break;     
		   		
				case CFileException::lockViolation:
					MessageBox(NULL, "Attempt to lock region already locked", NULL, MB_OK);
					
					#ifdef _DEBUG   
		   			afxDump << "Attempt to lock region already locked " << errCode << "\n";
		   		#endif
		   		break;           
		   		
				case CFileException::diskFull:
					MessageBox(NULL, "Disk is Full", NULL, MB_OK); 
					
					#ifdef _DEBUG  
		   			afxDump << "Disk is full " << errCode << "\n";
		   		#endif
		   		break;    
		   		
				case CFileException::endOfFile:
					MessageBox(NULL, "End of File Reached", NULL, MB_OK); 
					
					#ifdef _DEBUG  
		   			afxDump << "End of File Reached " << errCode << "\n";
		   		#endif
		   		break;     
		   		
				default: 
					sprintf( (CharPtr)tempString, "File Exception Error %hd. ", errCode);
					MessageBox(NULL, (CharPtr)tempString, NULL, MB_OK);
					
					#ifdef _DEBUG  
		   			afxDump << "File Exception Error " << errCode << "\n";
		   		#endif
		   		break;
		   		       
		   	}		// end "switch (errCode)"
		                   
			#endif	// defined _X86_ 
	   	
	  		}		// end "if (errCode != noErr)" 
	   
}		// end "IOCheck"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void IOCheck
//
//	Software purpose:	The purpose of this routine is to check for IO 
//							Errors and post a message if one has occured.
//		
//	Parameters in:		File operation error code.
//							Address of file name.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1988
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

void IOCheck (
				SInt16								errCode,
				CMFileStream*						fileStreamPtr)
                
{
	if (errCode != noErr)
		{		
		#if !defined _X86_
	
			if (fileStreamPtr != NULL)
				IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
				
			else		// fileStreamPtr == NULL
				IOCheck (errCode, (CharPtr)NULL);
		                  
		#endif	// !defined _X86_


		#if defined _X86_

			IOCheck (errCode, (CharPtr)NULL);
		                   
		#endif	// defined _X86_ 
		
		}		// end "if (errCode != noErr)"
		
}		// end "IOCheck"   



//------------------------------------------------------------------------------------
//
//								 Copyright (1988-2003)
//							  Ä Purdue Research Foundation
//
//	Function name:		Boolean LoadErdasHeader
//
//	Software purpose:	This routine loads the ERDAS header 
//							record based on the input file information  parameters
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			WriteErdasHeader in saveWrite.c
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1991
//	Revised By:			Larry L. Biehl			Date: 11/28/2000

Boolean	LoadErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				UInt8*								headerRecordPtr, 
				SInt16								ERDASVersion)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	char									*erdasHeader = "HEADER",
											*erdasHeader74 = "HEAD74";
											
	float									tempFloat;
	
	SInt32								tempLongInt;
	
	UInt16								iPack,
											nBands,
											nClasses,
											tempInt;
	
			
			// Set up swap bytes flag depending upon the system architecture.
			
	gSwapBytesFlag = gBigEndianFlag;
		
			// Load in 'HEADER'																	
	
	if (ERDASVersion == kErdas73Type)	
		BlockMoveData ( erdasHeader, (Ptr)headerRecordPtr, 6);
		
	else		// ERDASVersion == kErdas74Type 
		BlockMoveData ( erdasHeader74, (Ptr)headerRecordPtr, 6);
			
			// Write pack type of the data -> 4, 8, or 16 bit data					
				
	iPack = 0;
	
			//			Currently do not write out 4 bit packed data files.			
//	if (fileInfoPtr->numberBits == 4)  
//		iPack = 1;
		
	if (fileInfoPtr->numberBits > 8 || fileInfoPtr->numberBytes > 1) 
		iPack = 2;                               
		
//	if (fileInfoPtr->numberBytes == 4) 
//		iPack = 4;                                                
		                 
	tempInt = GetShortIntValue ( (char*)&iPack );
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[6], 2);

			// Write number of channels in the image data								

	nBands = fileInfoPtr->numberChannels;                                         
	tempInt = GetShortIntValue ( (char*)&nBands );
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[8], 2);
	
			// Write code indicating whether the data is signed.
			
	iPack = 0;
	if (fileInfoPtr->signedDataFlag)
		iPack = 1;                
	tempInt = GetShortIntValue ( (char*)&iPack );
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[10], 2);

			// Write number of columns in the image data									

	tempLongInt = fileInfoPtr->numberColumns;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ( (double)fileInfoPtr->numberColumns );
		
	else		// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ( (char*)&tempLongInt ); 
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[16], 4);

			// Write number of lines in the image data									

	tempLongInt = fileInfoPtr->numberLines;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ( (double)fileInfoPtr->numberLines );
		
	else		// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ( (char*)&tempLongInt );
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[20], 4);

			// Write start column of the image data										

	tempLongInt = fileInfoPtr->startColumn;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ( (double)fileInfoPtr->startColumn );
		
	else		// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ( (char*)&tempLongInt );
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[24], 4);

			// Write start line of the image data											

	tempLongInt = fileInfoPtr->startLine;
	if (ERDASVersion == kErdas73Type)
		tempLongInt = ConvertRealAT ( (double)fileInfoPtr->startLine );
		
	else		// ERDASVersion == kErdas74Type 
		tempLongInt = GetLongIntValue ( (char*)&tempLongInt );
	
	BlockMoveData ((Ptr)&tempLongInt, (Ptr)&headerRecordPtr[28], 4);

			// Write number of classes in the image data.								

	nClasses = (UInt16)fileInfoPtr->numberClasses;                                                   
	tempInt = GetShortIntValue ( (char*)&nClasses );
	BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[90], 2);
	
	if (fileInfoPtr->mapProjectionHandle != NULL)
		{                                            
		mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer(
															fileInfoPtr->mapProjectionHandle, 
															kLock, 
															kNoMoveHi);
		
		iPack = mapProjectionInfoPtr->gridCoordinate.code;                                 
		tempInt = GetShortIntValue ( (char*)&iPack );
		BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[88], 2);
		
		iPack = mapProjectionInfoPtr->planarCoordinate.areaUnitsCode;                                                                                  
		tempInt = GetShortIntValue ( (char*)&iPack );
		BlockMoveData ((Ptr)&tempInt, (Ptr)&headerRecordPtr[106], 2);
	
		tempLongInt = ConvertRealAT (mapProjectionInfoPtr->planarCoordinate.pixelArea );
		BlockMoveData ( (char*)&tempLongInt, &headerRecordPtr[108], 4);
	
		tempLongInt = ConvertRealAT (mapProjectionInfoPtr->planarCoordinate.xMapCoordinate11 );
		BlockMoveData ( (char*)&tempLongInt, &headerRecordPtr[112], 4);
	
		tempLongInt = ConvertRealAT (mapProjectionInfoPtr->planarCoordinate.yMapCoordinate11 );
		BlockMoveData ( (char*)&tempLongInt, &headerRecordPtr[116], 4);
	
		tempFloat = (float)mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize;
		tempLongInt = ConvertRealAT (tempFloat);
		BlockMoveData ( (char*)&tempLongInt, &headerRecordPtr[120], 4);
	
		tempFloat = (float)mapProjectionInfoPtr->planarCoordinate.verticalPixelSize;
		tempLongInt = ConvertRealAT (tempFloat);
		BlockMoveData ( (char*)&tempLongInt, &headerRecordPtr[124], 4);
					
		CheckAndUnlockHandle (fileInfoPtr->mapProjectionHandle);
		
		}		// end "if (fileInfoPtr->mapProjectionHandle != NULL)" 
	
	return (TRUE);
	
}		// end "LoadErdasHeader" 



//------------------------------------------------------------------------------------
//
//								 Copyright (1988-2003)
//							  Ä Purdue Research Foundation
//
//	Function name:		Boolean LoadNewErdasHeader
//
//	Software purpose:	This routine initializes a new Erdas header
//							record and then calls the routine to fill in
//							the parameters.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			??? in statisticsImage.c
//
//	Coded By:			Larry L. Biehl			Date: 11/21/1991
//	Revised By:			Larry L. Biehl			Date: 11/21/1991

Boolean	LoadNewErdasHeader (
				FileInfoPtr							fileInfoPtr, 
				UInt8*								headerRecordPtr, 
				SInt16								ERDASVersion)

{
	SInt32*								localHeaderRecordPtr;
	SInt16								n;
	Boolean								continueFlag;
	
	
	localHeaderRecordPtr = (SInt32*)headerRecordPtr;
	
			// Initialize the header array to all zeros.									
					
	for (n=1; n<=32; n++)
		{
		*localHeaderRecordPtr = 0;
		localHeaderRecordPtr++;
		
		}		// end "for (n=1; n<=32; n++)" 
	
			// Fill the rest of the header and write it to the file.					
			
	continueFlag = LoadErdasHeader (fileInfoPtr, headerRecordPtr, ERDASVersion);
			
	return (continueFlag);
		
}		// end "LoadNewErdasHeader"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MGetMarker
//
//	Software purpose:	The purpose of this routine is to get the current location
//							of the file marker in the current file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/07/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995 
                       
SInt16 MGetMarker (
				CMFileStream*						fileStreamPtr,
				SInt32*								outOffsetPtr,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if !defined _X86_    
	
		errCode = GetFPos (fileStreamPtr->refNum, outOffsetPtr);
		
		if (messageCode == kErrorMessages)		
			IOCheck (errCode, fileStreamPtr);                       

	#endif	// !defined _X86_ 

              
	#if defined _X86_ 
	 
		errCode = fileStreamPtr->MGetMarker (outOffsetPtr, messageCode); 
 
	#endif	// defined _X86_ 
							 
	return (errCode);
		      
}		// end "MGetMarker"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MReadData
//
//	Software purpose:	The purpose of this routine is to read the data from
//							the specified file to the user supplied buffer. The
//							file is assumed to already be opened and positioned
//							to the correct location.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/07/1995
//	Revised By:			Larry L. Biehl			Date: 10/20/1995 
                       
SInt16 MReadData (
				CMFileStream*						fileStreamCPtr,
				UInt32*								numberBytesPtr,
				void* 								inBufferPtr,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if !defined _X86_    

		errCode = FSRead (fileStreamCPtr->refNum, (SInt32*)numberBytesPtr, inBufferPtr);
	
		if (errCode != noErr && messageCode == kErrorMessages)		
			IOCheck (errCode, (CharPtr)&fileStreamCPtr->fileName);                       

	#endif	// !defined _X86_ 

              
	#if defined _X86_ 
	 
		errCode = fileStreamCPtr->MReadData (inBufferPtr, numberBytesPtr, messageCode); 
 
	#endif	// defined _X86_ 
							 
	return (errCode);
		      
}		// end "MReadData"            



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 MSetMarker
//
//	Software purpose:	The purpose of this routine is to set the file marker
//							in the current file.
//
//	Parameters in:		Location to set the marker from the beginning.
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/08/1995 
//	Revised By:			Larry L. Biehl			Date: 10/20/1995  

SInt16 MSetMarker(
				CMFileStream*						fileStreamCPtr,
				SInt16								fromCode,
				SInt32								inOffset,
				SInt16								messageCode)
                    
{       
	SInt16								errCode = noErr;
	
	
	#if !defined _X86_       
	                                                         
		errCode = SetFPos (fileStreamCPtr->refNum, fromCode, inOffset);
		
		if (messageCode == kErrorMessages) 		
			IOCheck (errCode, (CharPtr)&fileStreamCPtr->fileName);
			                        
	#endif	// !defined _X86_


	#if defined _X86_ 
                                 
		errCode = fileStreamCPtr->MSetMarker (inOffset, fromCode, messageCode);
			                        
	#endif	// defined _X86_  
	
	return (errCode);
  
}		// end "MSetMarker" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void MSetSizeOfFile
//
//	Software purpose:	The purpose of this routine is to set the size
//							of the specified file.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/11/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995 
                       
SInt16 MSetSizeOfFile (
				CMFileStream*						fileStreamPtr,
				UInt32								countBytes,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if !defined _X86_    

		errCode = ::SetEOF (fileStreamPtr->refNum, countBytes);
		
		if (messageCode == kErrorMessages)		
			IOCheck (errCode, fileStreamPtr);                       

	#endif	// !defined _X86_ 

              
	#if defined _X86_ 
	 
		errCode = fileStreamPtr->MSetSizeOfFile (countBytes); 
 
	#endif	// defined _X86_ 
							 
	return (errCode);
		      
}		// end "MSetSizeOfFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 MWriteData
//
//	Software purpose:	The purpose of this routine is to write the date in the
//							input buffer to the specified disk file that has already
//							been opened.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/06/1995
//	Revised By:			Larry L. Biehl			Date: 10/24/1995 

SInt16 MWriteData (
				CMFileStream*						fileStreamPtr,
				UInt32*								numberBytesPtr,
				void*									inBufferPtr,
				SInt16								messageCode)

{  
	SInt16								errCode;
	
	                  
	#if !defined _X86_     

		errCode = FSWrite (fileStreamPtr->refNum, (SInt32*)numberBytesPtr, inBufferPtr);
		
		if (messageCode == kErrorMessages)		
			IOCheck (errCode, fileStreamPtr);                       

	#endif	// !defined _X86_ 

              
	#if defined _X86_ 
	 
		errCode = fileStreamPtr->MWriteData (inBufferPtr, numberBytesPtr, messageCode); 
 
	#endif	// defined _X86_ 
							 
	return (errCode);
		      
}		// end "MWriteData" 



#if !defined _X86_ 
// *****************************************************************************
// *
// *	NavServicesFilterProc( )
// *
// *****************************************************************************
pascal Boolean NavServicesFilterProc( 
				AEDesc* 								theItem, 
				void* 								info, 
				NavCallBackUserData 				callBackUD, 
				NavFilterModes 					filterMode )
				
{
// #pragma unused (callBackUD, filterMode)

	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	FSSpec*								fsSpecPtr;
	NavFileOrFolderInfo* 			theInfo;
	NavUserData*						navUserDataPtr;
	
	SInt32								lFilterMode = filterMode;
	
	OSErr									errCode;
	SInt16								callCode,
											formatOnlyCode;
											
	Boolean								display = TRUE;
	
	
	navUserDataPtr = (NavUserData*)callBackUD;		
	theInfo = (NavFileOrFolderInfo*)info;
	
	if (theItem->descriptorType == typeFSS)
		{
		if (!theInfo->isFolder && theInfo->visible)
			{									
			if (navUserDataPtr->filterSelection >= 101)
				{
				fsSpecPtr = (FSSpec*)*theItem->dataHandle;

				fileInfoPtr = navUserDataPtr->fileInfoPtr;
				fileInfoPtr->format = 0;
				fileInfoPtr->thematicType = FALSE;
				
				fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
				fileStreamPtr->vRefNum = fsSpecPtr->vRefNum;
				fileStreamPtr->parID = fsSpecPtr->parID;
				CopyPToP ( (char*)fileStreamPtr->fileName, 
											(char*)fsSpecPtr->name);
				fileStreamPtr->refNum = 0;
				fileStreamPtr->fSSpecFlag = TRUE;
			
				errCode = OpenFileReadOnly (fileStreamPtr, 
														kResolveAliasChains,
														kNoLockFile);
				
				formatOnlyCode = 0;
				callCode = 0;
				if (errCode == noErr)
					{		
					switch (navUserDataPtr->filterSelection)
						{
						case kAllReadableFiles:
						case kAllReadableImageFiles:
						case kMultiFiles:
						case kThematicFiles:
						case kShapeFiles:
							if (CheckImageHeader (navUserDataPtr->fileInfoHandle, 
													NULL, 
													navUserDataPtr->filterSelection) == 0)
								display = FALSE;
							break;
							
						case kSTAFiles:
							if (CheckIfSTAFile (fileStreamPtr) == 0)
								display = FALSE;
							break;
							
						case kProjectFiles:
							if (CheckIfProjectFile (fileStreamPtr, kProjectFiles) == -1)
								display = FALSE;
							break;
							
						case kTRLFiles:
							if (CheckIfTRLFile (fileStreamPtr) == 0)
								display = FALSE;
							break;
							
						case kTransformFiles:
//							if (CheckIfTransformationFile (fileStreamPtr) == 0)
							if (theInfo->fileAndFolder.fileInfo.finderInfo.fdType != 'TEXT' &&
									theInfo->fileAndFolder.fileInfo.finderInfo.fdType != 'TRAN')
								display = FALSE;
							break;
						
						}		// end "switch (navUserDataPtr->filterSelection)"
					
					CloseFile (fileStreamPtr);
						
					}		// end "if (errCode == noErr)"
					
				else 	// errCode != noErr
					display = FALSE;
														
				}		// end "if (navUserDataPtr->filterSelection >= 101)"
		
			}		// end "if (!theInfo->isFolder && theInfo->visible)"
			
		}		// end "if (theItem->descriptorType == typeFSS)"
		
	return display;
	
}		// end "NavServicesFilterProc"



// *****************************************************************************
// *
// *	NavServicesGetEventProc( )	
// *
// *****************************************************************************
pascal void NavServicesGetEventProc ( 
				NavEventCallbackMessage 		callBackSelector, 
				NavCBRecPtr 						callBackParms, 
				NavCallBackUserData 				callBackUD )
				
{
	Point									mousePt;
//	Rect									theBox;
	
	NavMenuItemSpec					*menuItemSpecPtr,
											*navMenuItemSpecPtr;
											
	NavUserData*						navUserDataPtr;
											
	DialogPtr							dialogPtr;
//	WindowPtr							window;
	
//	ControlHandle						theControl;

	SInt32								initialSelection;
	
	OSErr 								errCode = noErr;
	
	SInt16								firstCustomControl,
											itemHit;
											
	SignedByte							handleStatus;
	
	
	switch (callBackSelector)
		{
		case kNavCBEvent:
			{
			switch (callBackParms->eventData.eventDataParms.event->what)
				{
				case nullEvent:
					break;
					
				case updateEvt:
					if (callBackParms->window != 
								(WindowPtr)callBackParms->eventData.eventDataParms.event->message)
  						DoUpdateEvent (callBackParms->eventData.eventDataParms.event);
  					
/*  					else		// if (callBackParms->window ==
  						{
  						errCode = NavCustomControl (callBackParms->context,
													kNavCtlGetFirstControlID,
													&firstCustomControl);
													
						countDITL = CountDITL (callBackParms->window);
												
						GetDialogItem (callBackParms->window, 
												firstCustomControl+1, 
												&theType, 
												(Handle*)&theControl, 
												&theBox);
						
//						if (theType < kStaticTextDialogItem)						
//							ShowControl (theControl);
						
//						ClipRect (GetWindowPortBounds(callBackParms->window, &gTempRect));
//  						DrawControls (callBackParms->window);
  						
  						}		// end "else if (callBackParms->window =="
*/  						
					break;
					
	      	case mouseDown:

					dialogPtr = GetDialogFromWindow (callBackParms->window);
					mousePt = callBackParms->eventData.eventDataParms.event->where;
					GlobalToLocal (&mousePt);
		  			
		  			itemHit = FindDialogItem (dialogPtr, mousePt);
		  			
					if (itemHit > 0)
		  				{
						errCode = NavCustomControl (callBackParms->context,
																kNavCtlGetFirstControlID,
																&firstCustomControl);
						
						if (errCode == noErr)
							{										
							switch (itemHit-firstCustomControl)
								{
								case 0:
									break;
									
								case 1:
									SetDLogControl (dialogPtr, firstCustomControl+2, 1);
									SetDLogControl (dialogPtr, firstCustomControl+3, 0);
									SetDLogControl (dialogPtr, firstCustomControl+4, 0);
									gGetFileImageType = 0;
									break;
			
								case 2:
								
									SetDLogControl (dialogPtr, firstCustomControl+2, 0);
									SetDLogControl (dialogPtr, firstCustomControl+3, 1);
									SetDLogControl (dialogPtr, firstCustomControl+4, 0);
									gGetFileImageType = kImageType;
									break;
									
								case 3:
								
									SetDLogControl (dialogPtr, firstCustomControl+2, 0);
									SetDLogControl (dialogPtr, firstCustomControl+3, 0);
									SetDLogControl (dialogPtr, firstCustomControl+4, 1);
									gGetFileImageType = kThematicType;
									break;
									
								case 4:
								
									if (gMultipleImageFileCode == 2)
										NavSetImageWindowFileFlagToFalse (dialogPtr, 
																					firstCustomControl+1);
										
									else		// gMultipleImageFileCode != 2 
										{
										SetDLogControl (dialogPtr, firstCustomControl+5, 1);
										gMultipleImageFileCode = 2;
									
										HideDialogItem (dialogPtr, firstCustomControl+2);
										HideDialogItem (dialogPtr, firstCustomControl+4);
										SetDLogControl (dialogPtr, firstCustomControl+2, 0);
										SetDLogControl (dialogPtr, firstCustomControl+3, 1);
										SetDLogControl (dialogPtr, firstCustomControl+4, 0);
										
										if( MGetString (gTextString, kFileIOStrID, IDS_SelectFileToLink) )
											{
											sprintf( (char*)&gTextString[13], 
												"%3d", 
												((WindowInfoPtr)*gActiveImageWindowInfoH)->numberImageFiles+1 );
											gTextString[16] = ' ';
											
											SetWTitle (callBackParms->window, 
																(UCharPtr)&gTextString);
											
											}		// end "if( MGetIndString (..." 
											
										}		// end "else !gMultipleImageFileCode" 
										
									break;
								
								}		// end "switch (countDITL-firstCustomControl)"
								
							}		// end "if (errCode == noErr)"
						
						}		// end "if (mouseLoc == inContent)"
						
					break;

				case activateEvt:
					break;

				default:
					break;
					
				}		// end "switch (callBackParms->eventData.eventDataParms.event->what)"
				
			break;
				
			}		// end "case kNavCBEvent"
			
		case kNavCBCustomize:
			{
			if (gProcessorCode == 0)
				{
				if (callBackParms->customRect.bottom == 0)
					{
					callBackParms->customRect.right = 
											callBackParms->customRect.left + 400;
					callBackParms->customRect.bottom = 
											callBackParms->customRect.top + 100;
					
					sCustomControlCount = 0;
					
					}		// end "if (callBackParms->customRect.bottom == 0)"
					
				else		// callBackParms->customRect.bottom > 0
					{
					if (sCustomControlCount == 0)
						{
						sCustomControlCount++;
						
						}		// end "if (sCustomControlCount == 0)"
					
					}		// end "if (callBackParms->customRect.bottom == 0)"
				
				}		// end "if (gProcessorCode == 0)"
			break;
				
			}		// end "case kNavCBCustomize"
			
		case kNavCBStart:
			{
			if (gProcessorCode == 0)
				{
						// Open Image or Shape File
						// 	Additional options need to be added.
						
				errCode = NavCustomControl (callBackParms->context,
													kNavCtlAddControlList,
													gCustomNavOpenList);
			
				if (errCode == noErr)										
					errCode = NavCustomControl (callBackParms->context,
														kNavCtlGetFirstControlID,
														&firstCustomControl);
														
				if (errCode == noErr)
					{
					dialogPtr = GetDialogFromWindow (callBackParms->window);
					
					if (gGetFileStatus == 0)
						{
						gGetFileImageType = 0;
			
						NavSetImageWindowFileFlagToFalse (dialogPtr, 
																	firstCustomControl+1);
			
						if	(gActiveImageWindowInfoH == NULL ||
							((WindowInfoPtr)*gActiveImageWindowInfoH)->windowType != kImageType ||
							((WindowInfoPtr)*gActiveImageWindowInfoH)->projectBaseImageFlag ||
							((WindowInfoPtr)*gActiveImageWindowInfoH)->bandInterleave == kBIS)
								HideDialogItem (dialogPtr, firstCustomControl+5);
								
						}		// end "if (gGetFileStatus == 0)"
		
					if (gMultipleImageFileCode == 2)
						{
						SetDLogControl (dialogPtr, firstCustomControl+2, 0);
						SetDLogControl (dialogPtr, firstCustomControl+3, 1);
						SetDLogControl (dialogPtr, firstCustomControl+4, 0);
						
						HideDialogItem (dialogPtr, firstCustomControl+2);
						HideDialogItem (dialogPtr, firstCustomControl+4);
						
						SetDLogControl (dialogPtr, firstCustomControl+5, 1);
						if (gGetFileStatus == 2)
							SetDLogControlHilite (dialogPtr, firstCustomControl+5, 255);

						}		// end "if (gMultipleImageFileCode == 2)" 
					
					}		// end "if (errCode == noErr)"
													
				}		// end "if (gProcessorCode == 0)"
			
			navUserDataPtr = (NavUserData*)callBackUD;
			initialSelection = navUserDataPtr->initialMenuSelection;
												
			navMenuItemSpecPtr = (NavMenuItemSpec*)GetHandleStatusAndPointer (
												(Handle)navUserDataPtr->navMenuItemSpecHandle,
												&handleStatus,
												kNoMoveHi);
		
			NavCustomControl (callBackParms->context, 
										kNavCtlSelectCustomType, 
										(void*)&navMenuItemSpecPtr[initialSelection]);

			MHSetState ((Handle)navUserDataPtr->navMenuItemSpecHandle, handleStatus);
				
			break;
				
			}		// end "case kNavCBStart"
			
		case kNavCBPopupMenuSelect:
			{
			menuItemSpecPtr = 
					(NavMenuItemSpec*)callBackParms->eventData.eventDataParms.param;
			
			navUserDataPtr = (NavUserData*)callBackUD;		
			navUserDataPtr->filterSelection = menuItemSpecPtr->menuType;
			
			if (navUserDataPtr->stringIndex == IDS_SelectImage)
				gImageFileFilterIndex = menuItemSpecPtr->menuCreator;
		
			break;
				
			}		// end "case kNavCBPopupMenuSelect"
			
		}		// end "switch (callBackSelector)"
		
}		// end "NavServicesGetEventProc" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 NavServicesGetFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							navigation services get file dialog box to allow the user to
//							select the file to read from.
//		
//	Parameters in:		Address to MFileInfo structure for selected file.
//							Number of parameters in type list.
//							Address of type list.
//
//	Parameters out:	MFileInfo Structure parameters for selected file.
//
// Value Returned:	Error code for file operations.
//
// Called By:			GetSTASupportFile in histogram.c
//							OpenImageFile in openImageRoutines.c
//							GetProjectFile in project.c
//							UserLocateProjectBaseImage in project.c
//							LoadTransformationFile in saveWrite.c
//
//	Coded By:			Larry L. Biehl			Date: 08/24/2001
//	Revised By:			Larry L. Biehl			Date: 10/05/2001

SInt16 NavServicesGetFile (
				CMFileStream* 						fileStreamPtr, 
				SInt16								numberTypes, 
				OSType*								fileTypesPtr, 
				LocalAppFile*						localAppFilePtr, 
				SInt16								stringIndex)

{
	NavReplyRecord						theReply;
	NavDialogOptions					dialogOptions;
	
	FileInfoPtr							fileInfoPtr = NULL;
	NavEventUPP							eventUPP;
	NavObjectFilterUPP				filterUPP;
	NavMenuItemSpec*					navMenuItemSpecPtr = NULL;
	
	Handle								fileInfoHandle = NULL;
	NavMenuItemSpecArrayHandle		navMenuItemSpecHandle = NULL;
	
	NavUserData							navUserData;
					
	AEKeyword							keyword;
	DescType								returnedType;
	Size									actSize;
	
	SInt32								itemCount;
	
	SInt16								errCode = noErr,
											filterCode,
											numberPopupMenuItem;
											
	Boolean								fileSelectedFlag;
	
		
			// Initialize variables.															
			
	gGetFileStatus = 3;
	fileSelectedFlag = FALSE;
	
			// Make certain that MultiSpec is not in the background.					
			
	PauseIfInBackground ();
		
			// Check if this routine called from MultiSpec startup, in which		
			// case the file to open has already been selected.						
			
	if (localAppFilePtr != NULL)
		{
		fileStreamPtr->vRefNum = localAppFilePtr->vRefNum;
		fileStreamPtr->parID = localAppFilePtr->parID;
		CopyPToP ( (char*)fileStreamPtr->fileName, (char*)localAppFilePtr->fNamePtr);
		fileStreamPtr->fSSpecFlag = localAppFilePtr->fSSpecFlag;
		
		}		// end "if (localAppFilePtr != NULL)" 
	
			// Else file has not been selected.												
			// Put the file dialog box up to select the input file.					
	
	else		// localAppFilePtr == NULL 
		{	
				// Get the requested prompt string.											
				
		if ( !GetSpecifiedStringNumber (
							kFileIOStrID, stringIndex, &gTextString, TRUE ) )
																			return (memFullErr);
		
		if (gMultipleImageFileCode)
			{
			sprintf( (char*)&gTextString[13], 
					"%3d", 
					((WindowInfoPtr)*gActiveImageWindowInfoH)->numberImageFiles+1 );
			gTextString[16] = ' ';
			gGetFileStatus = 2;
			
			}		// end "if (gMultipleImageFileCode)" 
			
		else		// !gMultipleImageFileCode 
			{
			gGetFileStatus = -1;
			if (stringIndex == IDS_SelectImage)
				gGetFileStatus = 0;
				
			}		// end "else !gMultipleImageFileCode" 
			
		theReply.validRecord = FALSE;
			
				// Only call standard io get file if there is a minimum amount of	
				// memory available.																
				
		if ( CheckIfMemoryAvailable ( 15000 ) )
			{
					// Get the resource number for the filter list.
					
			filterCode = 0;
			numberPopupMenuItem = 0;
			navUserData.stringIndex = 0;
			switch (stringIndex)
				{
				case IDS_SelectImage:
					navUserData.stringIndex = IDS_SelectImage;
					filterCode = kMultiThematicShapeFiles;	// multispectral, thematic, shape
					numberPopupMenuItem = 5;
					navUserData.initialMenuSelection = gImageFileFilterIndex;
					if (gActiveImageWindow == NULL)
						{
						filterCode =  kMultiThematicFiles;	// multispectral, thematic
						numberPopupMenuItem = 4;
						
						if (navUserData.initialMenuSelection == 4)
							navUserData.initialMenuSelection = 0;
						
						}		// end "if (gActiveImageWindow == NULL)"
					break;
					
				case IDS_SelectImageStatistics:	// image statistics (STA)
					filterCode = kSTAFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_SelectProject:		// project
					filterCode = kProjectFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
				
				case IDS_SelectFileToLink:		// multispectral
				case IDS_SelectNewBaseImage:
				case IDS_WhereIsProjectImage:
					filterCode = kMultiFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
				
				case IDS_FileIO62:	// transformation
					filterCode = kTransformFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO65:	// probability
					filterCode = kProbablilityFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO76:	// thematic class info (TRL)
					filterCode = kTRLFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO77:	// thematic
					filterCode = kThematicFiles;
					numberPopupMenuItem = 1;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO51:	// multispectral, thematic
				case IDS_FileIO52:
				case IDS_FileIO95:
				case IDS_FileIO96:
				case IDS_FileIO103:
				case IDS_FileIO104:
				case IDS_FileIO105:
					filterCode = kMultiThematicFiles;
					numberPopupMenuItem = 4;
					navUserData.initialMenuSelection = 0;
					break;
					
				case IDS_FileIO146:	// all
					filterCode = 0;
					numberPopupMenuItem = 0;
					break;
				
				}		// end "switch (stringIndex)"
				
			if (numberPopupMenuItem > 0)
				{
						// A file information structure will be needed.

				fileInfoHandle = MNewHandle ( sizeof(MFileInfo) );
				fileInfoPtr = (FileInfoPtr)GetHandlePointer(fileInfoHandle,
																		kLock,
																		kNoMoveHi);
				
				if (fileInfoPtr != NULL)
					{														
					navMenuItemSpecHandle = (NavMenuItemSpecHandle)MNewHandle (
													numberPopupMenuItem*sizeof(NavMenuItemSpec));
												
					navMenuItemSpecPtr = (NavMenuItemSpec*)GetHandlePointer (
																		(Handle)navMenuItemSpecHandle,
																		kLock,
																		kNoMoveHi);
					
					}		// end "if (fileInfoPtr != NULL)"
													
				}		// end "if (numberPopupMenuItem > 0)"
			
					// Finish setting up navigation user data structure.
					
			navUserData.fileInfoPtr	= fileInfoPtr;
			navUserData.fileInfoHandle	= fileInfoHandle;
			navUserData.navMenuItemSpecHandle = navMenuItemSpecHandle;	
			navUserData.filterSelection = 0;
			
			if (navMenuItemSpecPtr != NULL)
				{
				switch (filterCode)
					{
					case kMultiThematicShapeFiles:
					
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = 0;
						navMenuItemSpecPtr[0].menuType = kAllReadableFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0All Readable Files\0" );
					
						navMenuItemSpecPtr[1].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[1].menuCreator = 1;
						navMenuItemSpecPtr[1].menuType = 0;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[1].menuItemName, 
	      															"\0-\0" );
					
						navMenuItemSpecPtr[2].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[2].menuCreator = 2;
						navMenuItemSpecPtr[2].menuType = kMultiFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[2].menuItemName, 
	      															"\0Multispectral Image Files\0" );
																			
						navMenuItemSpecPtr[3].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[3].menuCreator = 3;
						navMenuItemSpecPtr[3].menuType = kThematicFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[3].menuItemName, 
	      															"\0Thematic Image Files\0" );
										
						navMenuItemSpecPtr[4].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[4].menuCreator = 4;
						navMenuItemSpecPtr[4].menuType = kShapeFiles;
      					CopyPToP ( (char*)navMenuItemSpecPtr[4].menuItemName, 
      															"\0Shape Files\0" );
	      				
						navUserData.filterSelection = kAllReadableFiles;
						break;
					
					case kSTAFiles:
																				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kSTAFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0Image Statistics Files\0" );
						navUserData.filterSelection = kSTAFiles;
						break;
						
					case kProjectFiles:
				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kProjectFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0Project Files\0" );
						navUserData.filterSelection = kProjectFiles;
						break;
						
					case kTRLFiles:
				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kTRLFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      														"\0Thematic Class Info Files\0" );
						navUserData.filterSelection = kTRLFiles;
						break;
					
					case kTransformFiles:
																				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kTransformFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0Transformation Matrix Files\0" );
						navUserData.filterSelection = kTransformFiles;
						break;
						
					case kThematicFiles:
					case kProbablilityFiles:
				
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kThematicFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0Thematic Image Files\0" );
						navUserData.filterSelection = kThematicFiles;
						break;
						
					case kMultiThematicFiles:
					
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = 0;
						navMenuItemSpecPtr[0].menuType = kAllReadableImageFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0All Readable Files\0" );
					
						navMenuItemSpecPtr[1].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[1].menuCreator = 1;
						navMenuItemSpecPtr[1].menuType = 0;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[1].menuItemName, 
	      															"\0---\0" );
					
						navMenuItemSpecPtr[2].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[2].menuCreator = 2;
						navMenuItemSpecPtr[2].menuType = kMultiFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[2].menuItemName, 
	      															"\0Multispectral Image Files\0" );
																			
						navMenuItemSpecPtr[3].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[3].menuCreator = 3;
						navMenuItemSpecPtr[3].menuType = kThematicFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[3].menuItemName, 
	      															"\0Thematic Image Files\0" );
						navUserData.filterSelection = kAllReadableImageFiles;
						break;
						
					case kMultiFiles:
					
						navMenuItemSpecPtr[0].version = kNavMenuItemSpecVersion;	
						navMenuItemSpecPtr[0].menuCreator = gCreator;
						navMenuItemSpecPtr[0].menuType = kMultiFiles;
	      				CopyPToP ( (char*)navMenuItemSpecPtr[0].menuItemName, 
	      															"\0Multispectral Image Files\0" );
						navUserData.filterSelection = kMultiFiles;
						break;
						
					}		// end "switch (filterCode)"
						
				}		// end "if (navMenuItemSpecPtr != NULL)"
						
			CheckAndUnlockHandle ((Handle)navMenuItemSpecHandle);
				
			eventUPP = NewNavEventUPP (NavServicesGetEventProc);
			filterUPP = NewNavObjectFilterUPP (NavServicesFilterProc);

					// Default behavior for browser and dialog
					
			errCode = NavGetDefaultDialogOptions (&dialogOptions);
				
			CopyPToP ((char*)dialogOptions.clientName, (char*)"\0MultiSpec"); 
			CopyPToP ((char*)dialogOptions.windowTitle, (char*)gTextString);
			
			dialogOptions.preferenceKey = kOpenPrefKey;
			
					// We will do the translation ourselves later
					
			dialogOptions.dialogOptionFlags |= kNavDontAutoTranslate;
			
					// Clear the preview option
					
			dialogOptions.dialogOptionFlags ^= kNavAllowPreviews;
			
					// Include 'All Files' in popup
				
			dialogOptions.dialogOptionFlags |= kNavAllFilesInPopup;
			
//			if (openListResourceItem == 0)
//				dialogOptions.dialogOptionFlags |= kNavNoTypePopup;
			
//			dialogOptions.dialogOptionFlags ^= kNavNoTypePopup;
			
					// Force all readable file types in popup to be selected when opened.
				
			dialogOptions.dialogOptionFlags |= kNavSelectAllReadableItem;
				
//			if (stringIndex == IDS_WhereIsProjectImage ||
//					stringIndex == IDS_SelectProject ||
//						stringIndex == IDS_FileIO103 ||
//							stringIndex == IDS_FileIO104)
//				{
						// Do not allow multiple files.
											
				dialogOptions.dialogOptionFlags ^= kNavAllowMultipleFiles;
					
//				}		// end "if (stringIndex == IDS_WhereIsProjectImage || ..."
				
//			else		// stringIndex != IDS_WhereIsProjectImage && ...
//				{
						// Allow multiple files.
						
//				dialogOptions.dialogOptionFlags |= kNavAllowMultipleFiles;
				
//				}		// end "else stringIndex != IDS_WhereIsProjectImage && ..."

			dialogOptions.popupExtension = navMenuItemSpecHandle;
			
			errCode = NavGetFile (NULL,		// use system's default location
									&theReply,
									&dialogOptions,
									eventUPP,
									NULL,		// no custom previews
									filterUPP,		// filterUPP,
									NULL,	// openListHandle
									&navUserData);		// Pointer to custom data

			DisposeNavEventUPP (eventUPP);
			DisposeNavObjectFilterUPP (filterUPP);
			
			navMenuItemSpecHandle = (NavMenuItemSpecHandle)UnlockAndDispose ((Handle)navMenuItemSpecHandle);
			fileInfoHandle = UnlockAndDispose (fileInfoHandle);
			
			if (theReply.validRecord && errCode == noErr)
				{
						// Count the items in the list.												
						
				errCode = AECountItems(&(theReply.selection), &itemCount);
				
				if (errCode == noErr && itemCount > 0)
					{
							// Get the file.															
						
			  		errCode = AEGetNthPtr (&(theReply.selection),
					  									1,
					  									typeFSS,
					  									&keyword,
					  									&returnedType,
					  									(Ptr)&gFileFSSpec,
					  									sizeof(FSSpec),
					  									&actSize);
			  									
//					  		if (errCode == noErr)
//					  			errCode = FSpGetFInfo (&gFileFSSpec, &gFinderInfo);
			  			
			  		if (errCode == noErr)
			  			{
						fileStreamPtr->vRefNum = gFileFSSpec.vRefNum;
						fileStreamPtr->parID = gFileFSSpec.parID;
						CopyPToP ( (char*)fileStreamPtr->fileName, 
													(char*)gFileFSSpec.name);
						fileStreamPtr->fSSpecFlag = TRUE;
						
						fileSelectedFlag = TRUE;
						
						}		// end "if (errCode == noErr)"
						
					}		// end "if (errCode == noErr && itemCount > 0)"
						
//							errCode = OpenOrPrintSelectedFiles (&(theReply.selection), FALSE);
					
				}		// end "if (theReply.validRecord && errCode == noErr)"
				
			NavDisposeReply (&theReply);
				
					// Make certain that all previous mouse events have been			
					// removed. This is done in case the user double clicked			
					// to select the file.														
									
	    	FlushEvents (mUpMask,0);
			
			}		// end "if ( CheckIfMemoryAvailable ( 15000 )" 
		
				// If file is to be added to the active image window, then the		
				// file type has to be forced to multispectral type.					
				
		if (gMultipleImageFileCode != 0)
			gGetFileImageType = kImageType;
		
		CheckSomeEvents (activMask+updateMask);
	
		if (!fileSelectedFlag)
			{
			fileStreamPtr->fileName[0] = 0;
																					return(0);
																					
			}		// end "if (!fileSelectedFlag)"

				// Also make the filename a useable c string

		fileStreamPtr->fileName[ fileStreamPtr->fileName[0]+1 ] = 0x00;
		
		}		// end "else localAppFilePtr == NULL" 

			// Get the file type and creator					
				
	errCode = FSpGetFInfo ( (FSSpec*)fileStreamPtr, &gFinderInfo);

	if (errCode == noErr)
		{
		fileStreamPtr->creator = gFinderInfo.fdCreator;
		fileStreamPtr->type = gFinderInfo.fdType;

		}		// end "if (errCode == noErr)"

	if (errCode == noErr)
		{
		errCode = OpenFileReadOnly (fileStreamPtr, 
											kResolveAliasChains,
											kLockFile);
											
		}
					
	IOCheck (errCode, fileStreamPtr);
		
			// To allow this will also involve checking for this refnum in other	
			// windows when closing a file, because it is the same path as that	
			// obtained for the first open.													
				
//	if (errCode == opWrErr)
//		errCode = noErr;		
	
	if (errCode != noErr)	
		SetReferenceNumber(fileStreamPtr, 0);	
		
	return (errCode);

}		// end "NavServicesGetFile"


  
// *****************************************************************************
// *
// *	NavServicesPutEventProc( )	
// *
// *****************************************************************************
pascal void NavServicesPutEventProc ( 
				NavEventCallbackMessage 		callBackSelector, 
				NavCBRecPtr 						callBackParms, 
				NavCallBackUserData 				callBackUD )
				
{
	Point									mousePt;
	
	DialogPtr							dialogPtr;
	NavMenuItemSpec*					menuItemSpecPtr;
	
	OSErr 								errCode = noErr;
	
	SInt16								firstCustomControl,
											itemHit;
											
	Boolean								changeCheckBoxFlag;
	
	
	switch (callBackSelector)
		{
		case kNavCBEvent:
			{
			switch (callBackParms->eventData.eventDataParms.event->what)
				{
				case nullEvent:
					break;
					
				case updateEvt:
					if (callBackParms->window != 
								(WindowPtr)callBackParms->eventData.eventDataParms.event->message)
  						DoUpdateEvent (callBackParms->eventData.eventDataParms.event);
					break;
					
	      	case mouseDown:

					dialogPtr = GetDialogFromWindow (callBackParms->window);
					mousePt = callBackParms->eventData.eventDataParms.event->where;
					GlobalToLocal (&mousePt);
		  			
		  			itemHit = FindDialogItem (dialogPtr, mousePt);
		  			
					if (itemHit > 0)
		  				{
						errCode = NavCustomControl (callBackParms->context,
																kNavCtlGetFirstControlID,
																&firstCustomControl);
						
						if (errCode == noErr)
							{			
							changeCheckBoxFlag = FALSE;
								
							switch (itemHit-firstCustomControl)
								{
								case 0:
									break;
									
								case 1:
									if (!gProjectInfoPtr->keepClassStatsOnlyFlag)
										{
										gProjectInfoPtr->saveFieldSumsSquaresFlag = !
												gProjectInfoPtr->saveFieldSumsSquaresFlag;
										changeCheckBoxFlag = TRUE;
										
										}		// end "if (!...->keepClassStatsOnlyFlag)"
									break;
			
								case 2:
									if (!gProjectInfoPtr->keepClassStatsOnlyFlag)
										{
										gProjectInfoPtr->saveFieldMeansCovFlag = !
												gProjectInfoPtr->saveFieldMeansCovFlag;
										changeCheckBoxFlag = TRUE;
										
										}		// end "if (!...->keepClassStatsOnlyFlag)"
									break;
									
								case 3:
									if (gProjectInfoPtr->classChanStatsPtr != NULL)
										{
										gProjectInfoPtr->saveClassSumsSquaresFlag = !
												gProjectInfoPtr->saveClassSumsSquaresFlag;
										changeCheckBoxFlag = TRUE;
										
										}		// end "if (...->classChanStatsPtr != NULL)"
									break;
									
								case 4:
									if (gProjectInfoPtr->classChanStatsPtr != NULL)
										{
										gProjectInfoPtr->saveClassMeansCovFlag = !
												gProjectInfoPtr->saveClassMeansCovFlag;
										changeCheckBoxFlag = TRUE;
										
										}		// end "if (...->classChanStatsPtr != NULL)"
									break;
								
								}		// end "switch (countDITL-firstCustomControl)"
	
							if (changeCheckBoxFlag)
								ChangeDLogCheckBox (dialogPtr, itemHit+1);
								
							}		// end "if (errCode == noErr)"
						
						}		// end "if (mouseLoc == inContent)"
						
					break;

				case activateEvt:
					break;

				default:
					break;
					
				}		// end "switch (callBackParms->eventData.eventDataParms.event->what)"
				
			break;
				
			}		// end "case kNavCBEvent"
			
		case kNavCBCustomize:
			{
			if (gProcessorCode == kSaveProjectAsProcessor)
				{
				if (callBackParms->customRect.bottom == 0)
					{
					callBackParms->customRect.right = 
											callBackParms->customRect.left + 400;
					callBackParms->customRect.bottom = 
											callBackParms->customRect.top + 110;
					
					sCustomControlCount = 0;
					
					}		// end "if (callBackParms->customRect.bottom == 0)"
					
				else		// callBackParms->customRect.bottom > 0
					{
					if (sCustomControlCount == 0)
						{
						sCustomControlCount++;
						
						}		// end "if (sCustomControlCount == 0)"
					
					}		// end "if (callBackParms->customRect.bottom == 0)"
				
				}		// end "if (gProcessorCode == kSaveProjectAsProcessor)"
				
			break;
				
			}		// end "case kNavCBCustomize"
			
		case kNavCBStart:
			{
			if (gProcessorCode == kSaveProjectAsProcessor)
				{
						// Save Project File
						// 	Additional options need to be added.
						
				errCode = NavCustomControl (callBackParms->context,
													kNavCtlAddControlList,
													gCustomNavPut);
			
				if (errCode == noErr)										
					errCode = NavCustomControl (callBackParms->context,
														kNavCtlGetFirstControlID,
														&firstCustomControl);
														
				if (errCode == noErr)
					{
					dialogPtr = GetDialogFromWindow (callBackParms->window);
			
					if (gGetFileStatus == 0)
						{
						if (gProjectInfoPtr->keepClassStatsOnlyFlag)
							{
							SetDLogControlHilite (dialogPtr, firstCustomControl+2, 255);
							SetDLogControlHilite (dialogPtr, firstCustomControl+3, 255);
							
							gProjectInfoPtr->saveFieldSumsSquaresFlag = FALSE;
							gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
							
							}		// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)" 
						
						if (gProjectInfoPtr->classChanStatsPtr == NULL)
							{
							SetDLogControlHilite (dialogPtr, firstCustomControl+4, 255);
							SetDLogControlHilite (dialogPtr, firstCustomControl+5, 255);
							
							gProjectInfoPtr->saveClassSumsSquaresFlag = FALSE;
							gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
							
							}		// end "if (gProjectInfoPtr->classChanStatsPtr == ...)" 
							
						SetDLogControl (dialogPtr, 
												firstCustomControl+2, 
												gProjectInfoPtr->saveFieldSumsSquaresFlag);
												
						SetDLogControl (dialogPtr, 
												firstCustomControl+3, 
												gProjectInfoPtr->saveFieldMeansCovFlag);
												
						SetDLogControl (dialogPtr, 
												firstCustomControl+4, 
												gProjectInfoPtr->saveClassSumsSquaresFlag);
												
						SetDLogControl (dialogPtr, 
												firstCustomControl+5, 
												gProjectInfoPtr->saveClassMeansCovFlag);
						
						gGetFileStatus = 2;
						
						}		// end "if (gGetFileStatus == 0)" 
					
					if (gGetFileStatus <= 0)	
						gGetFileStatus = 1;
					
					}		// end "if (errCode == noErr)"
				
				}		// end "else if (gProcessorCode == kSaveProjectAsProcessor)"
				
			break;
				
			}		// end "case kNavCBStart"
			
		case kNavCBPopupMenuSelect:
			{
			menuItemSpecPtr = 
					(NavMenuItemSpec*)callBackParms->eventData.eventDataParms.param;
				
			break;
				
			}		// end "case kNavCBPopupMenuSelect"
			
		case kNavCBAccept:

					// This code will have to be changed for nav services.
					// Don't know how yet.
					
			if (gGetFileStatus == 2)
				{
				if ( !ClusterStatisticsVerification () )
					itemHit = 0;
					
				}		// end "if (gGetFileStatus == 1)" 
				
			break;
			
		}		// end "switch (callBackSelector)"
		
}		// end "NavServicesPutEventProc" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 NavServicesPutFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard put file dialog box to allow the user to
//							select the file to write to.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/30/2001
//	Revised By:			Larry L. Biehl			Date: 08/30/2001

SInt16 NavServicesPutFile (
				CMFileStream*		 				fileStreamPtr, 
				SInt32								numberBytes, 
				SInt16								stringIndex, 
				SInt32								creator)

{
	CMFileStream						tempFileStream;
	NavReplyRecord						theReply;
	NavDialogOptions					dialogOptions;
	
	NavEventUPP							eventUPP;
					
	AEKeyword							keyword;
	DescType								returnedType;
	Size									actSize;
		
	SInt32								bytesNeeded;
	SInt32								itemCount;
	
	UInt32								freeBytes,
											logicalEndOfFile;
	
	SInt16								errCode = noErr;
	Boolean								fileSelected;
		
		
			// Make certain that MultiSpec is not in the background.					
			
	PauseIfInBackground ();
	
	fileSelected = FALSE;
	
	if ( !CheckIfMemoryAvailable ( 6000 ) )
																						return(1);
	
	do	
		{
		gGetFileStatus = -1;
		if (stringIndex == IDS_SaveProjectFile)
			gGetFileStatus = 0;
	
		ReleaseSpareMemoryForWarningMessage ();
		
		GetIndString (gTextString3, kFileIOStrID, stringIndex);
		if (gTextString3[gTextString3[0]] == ':')
			gTextString3[0]--;

				// Default behavior for browser and dialog			
				
		errCode = NavGetDefaultDialogOptions (&dialogOptions);
			
		CopyPToP ((char*)dialogOptions.clientName, (char*)"\0MultiSpec"); 
		
		CopyPToP ((char*)dialogOptions.windowTitle, (char*)gTextString3);
		
		CopyPToP ((char*)dialogOptions.savedFileName, (char*)fileStreamPtr->fileName);
		
		dialogOptions.preferenceKey = kOpenPrefKey;
		
				// We will do the translation ourselves later
				
		dialogOptions.dialogOptionFlags |= kNavDontAutoTranslate;
		dialogOptions.dialogOptionFlags |= kNavNoTypePopup;
			
		eventUPP = NewNavEventUPP (NavServicesPutEventProc);

		errCode = NavPutFile (NULL,	// use system's default location
								&theReply,
								&dialogOptions,
								eventUPP,
								fileStreamPtr->type,
								creator,
								NULL);		// Handle to custom data

		if (eventUPP != NULL)
			DisposeNavEventUPP (eventUPP);
		
		if (theReply.validRecord && errCode == noErr)
			{
					// Count the items in the list.												
					
			errCode = AECountItems(&(theReply.selection), &itemCount);
			
			if (errCode == noErr && itemCount > 0)
				{
						// Get the file.															
					
		  		errCode = AEGetNthPtr (&(theReply.selection),
				  									1,
				  									typeFSS,
				  									&keyword,
				  									&returnedType,
				  									(Ptr)&gFileFSSpec,
				  									sizeof(FSSpec),
				  									&actSize);
				  									
				if (errCode == noErr)
					fileSelected = TRUE;
					
				}		// end "if (errCode == noErr && itemCount > 0)"
				
			}		// end "if (theReply.validRecord && errCode == noErr)"
			
		NavDisposeReply (&theReply);
		
				// Get the spare memory back again.											
			
		SetHandleSize (gSpareWarnMemoryH, (Size)gSpareWarnSize);
		
		CheckSomeEvents (activMask+updateMask);
	
		if (!fileSelected)
			{
			fileStreamPtr->fileName[0] = 0;
																						return(0);
			
			}		// end "if (!fileSelected)" 
			
				// Check if enough space on the volume.									
				
		errCode = GetVolumeFreeSpace ( (Str255*)&gFileFSSpec.name, 
												gFileFSSpec.vRefNum, 
												&freeBytes);
															
		if (errCode != noErr)												
																				return(errCode);
		
				// Use tempFileStream structure as temporary storage for file 		
				// information.			
				
    	tempFileStream.vRefNum = gFileFSSpec.vRefNum;
    	tempFileStream.parID = gFileFSSpec.parID;
		CopyPToP ( (char*)tempFileStream.fileName, (char*)gFileFSSpec.name);
    	tempFileStream.refNum = 0;
    	tempFileStream.type = fileStreamPtr->type;
    	tempFileStream.fSSpecFlag = TRUE;
		
				// If the project file is being saved, the number of bytes for		
				// the file needs to be calculated here since the user can make	
				// changes in the PutFile dialog that affects the size of the		
				// save project file.															
		
		if (stringIndex == IDS_SaveProjectFile)
			numberBytes = GetSizeOfProjectFile ();
		
				// Check if there is enough space on the disk.  If there is not	
				// enough space, even when considering whether an old file is to	
				// be replaced then allow the user to select the file/disk again.	
		
		bytesNeeded = numberBytes + 100 - freeBytes;
		if (bytesNeeded > 0)
			{
					// Get the number of bytes in the old file if one is being		
					// replaced.																	
			
			errCode = GetSizeOfFile (&tempFileStream, &logicalEndOfFile);
			if (errCode != noErr && errCode != fnfErr )
																					return (errCode);
				
			bytesNeeded = bytesNeeded - logicalEndOfFile;
			
			if (bytesNeeded > 0)
				{
				DiskFullAlert ( bytesNeeded, (Str255*)tempFileStream.fileName);
				fileSelected = FALSE;
				
				}		// end "if (bytesNeeded > 0)"
			
			}		// end "if (bytesNeeded > 0)" 
			
				// Make certain that we can write to the file.							
				
		if (fileSelected)
			{
			errCode = FSMakeFSSpec (tempFileStream.vRefNum,
											tempFileStream.parID,
											tempFileStream.fileName,
											&gFileFSSpec);
				
			if (errCode == fnfErr ||  
					errCode == afpAccessDenied)
				errCode = noErr;
				
			if (errCode == noErr)	
				{
				tempFileStream.vRefNum = gFileFSSpec.vRefNum;
				tempFileStream.parID = gFileFSSpec.parID;
    			tempFileStream.fSSpecFlag = TRUE;
			
				errCode = HRstFLock (tempFileStream.vRefNum, 
												tempFileStream.parID,
												tempFileStream.fileName);
				
				if (errCode == fnfErr ||  
						errCode == afpAccessDenied)
					errCode = noErr;
																				
				}		// end "if (errCode == noErr)"
						
			if (errCode != noErr)
				{
				IOCheck (errCode, (CharPtr)&tempFileStream.fileName);
				fileSelected = FALSE;
				
				}		// end "if (errCode != noErr)" 
				
			}		// end "if (fileSelected)" 
		
		} while (!fileSelected);
		
			// Create the file on the selected volume										
			
	errCode = CreateNewFile (&tempFileStream, 
										tempFileStream.vRefNum, 
										creator, 
										kErrorMessages, 
										kReplaceFlag);
				
	if (errCode == noErr)
		{
				// Now copy the file parameters to the supplied structure.		
				
    	fileStreamPtr->vRefNum = tempFileStream.vRefNum;
    	fileStreamPtr->parID = tempFileStream.parID;
		CopyPToP ( (char*)fileStreamPtr->fileName, (char*)tempFileStream.fileName);
    	fileStreamPtr->type = tempFileStream.type;
    	fileStreamPtr->refNum = tempFileStream.refNum;
    	fileStreamPtr->fSSpecFlag = tempFileStream.fSSpecFlag;
		
		}		// end "if (errCode == noErr)"
		
	return (errCode);

}		// end "NavServicesPutFile"  
#endif	// !defined _X86_ 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		OpenFileReadOnly
//
//	Software purpose:	This routine opens the given file with read only permission.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			GetDefaultERDASsupportFile in fileIO.c
//							GetFile in fileIO.c	
//							ReadOneBytePalette in paletteProc.c
//							GetSpecifiedImageFile in project.c	
//							LoadProjectFileAndLayerInformation in project.c
//							OpenProjectImageWindow in project.c
//							InsertNewErdasHeader in reformat.c	
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1991
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

SInt16 OpenFileReadOnly (
				CMFileStream*						fileStreamPtr, 
				Boolean								resolveAliasFlag,
				Boolean								lockFileFlag)

{
	SInt16								errCode = noErr;
	
	
	#if !defined _X86_
	
		if (fileStreamPtr->refNum == 0)
			{
			Boolean	wasAliasedFlag = FALSE;
			
					// First resolve any aliases if requested.									
					
			if (resolveAliasFlag)
				errCode = ResolveAnyAliases (fileStreamPtr, &wasAliasedFlag);
				
					// Set to read only permission and reopen		
					
			#if TARGET_API_MAC_CARBON	
				lockFileFlag = FALSE;	
			#endif	// TARGET_API_MAC_CARBON						
				
			if (errCode == noErr && lockFileFlag)
				{
				if (fileStreamPtr->fSSpecFlag)
					errCode = FSpSetFLock ( (FSSpec*)fileStreamPtr );
					
				#if !TARGET_API_MAC_CARBON	
					else		// !fileInfoPtr->fSSpecFlag 
						errCode = SetFLock (fileStreamPtr->fileName, 
													fileStreamPtr->vRefNum);
				#endif	// !TARGET_API_MAC_CARBON	
				
				if (	errCode == wPrErr || 
									errCode == fLckdErr || 
									errCode == afpAccessDenied ||
									errCode == permErr)
					errCode = noErr;
					
				else		// errCode != wPrErr && errCode != fLckdErr && ... 
					if (errCode == noErr)
						errCode = FlushVol (NIL, fileStreamPtr->vRefNum);
						
				}		// end "if (errCode == noErr && lockFileFlag)" 
					
			if (errCode == noErr)
				{
				if (fileStreamPtr->fSSpecFlag)
					errCode = FSpOpenDF ( (FSSpec*)fileStreamPtr, 
													fsRdPerm, 
													&fileStreamPtr->refNum);
																	
				else		// !fileInfoPtr->fSSpecFlag 
					errCode = HOpen (fileStreamPtr->vRefNum, 
											0, 
											fileStreamPtr->fileName, 
											fsRdPerm, 
											&fileStreamPtr->refNum);
																	
				}		// end "if (errCode == noErr)" 
				
			}		// end "if (fileStreamPtr->refNum == 0)"
		
	#endif	// !defined _X86_
	
	
	#if defined _X86_
		
		errCode = fileStreamPtr->MOpenFile (
										(SInt16)(kRead),
										(SInt16)(kNoErrorMessages) );
	
	#endif	// defined _X86_
	
	return (errCode);
	
}		// end "OpenFileReadOnly"           



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PackBISData
//
//	Software purpose:	The purpose of this routine is to pack the line
//							of data for BIS formated files if the column
//							interval is more than 1 and/or the number of
//							reqested channels is less than the number of 
//							available channels.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//Value Returned:		Error code for file operations.
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1989
//	Revised By:			Larry L. Biehl			Date: 10/27/1999

void PackBISData (
				UInt32								columnInterval, 
				UInt32								numberBufferChannels, 
				UInt32								numberChannels, 
				UInt32								numberSamples, 
				UInt32								numberInBytes, 
				HUCharPtr 							ioCharBufferPtr,
				HUCharPtr 							dataCharBufferPtr, 
				UInt16*								channelListPtr, 
				UInt32								numberOutBytes,
				Boolean								signedInDataFlag)

{
	SInt32								skipSamples;
			
	
	skipSamples = columnInterval * numberBufferChannels;
	
	if (numberInBytes == 1)
		{
		if (numberOutBytes == kDoNotForceBytes || numberOutBytes == kForce1Byte)
			{
			UInt32				channel,
									index,
									sample;
			HUCharPtr	 		fromCharPtr;
			HUCharPtr 			toCharPtr;
			
			toCharPtr = dataCharBufferPtr;
			fromCharPtr = ioCharBufferPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				while (index < numberChannels)
					{		
					if ( channelListPtr)
						channel = channelListPtr[index];
						
					*toCharPtr = fromCharPtr[channel];
					toCharPtr++;
					index++;
					channel++;
					
					}		// end "while (index < numberChannels)" 
					
				fromCharPtr += skipSamples;
				
				}		// end "for (sample=0;  sample<numberColumns; ..." 
				
			}		// end "if (numberOutBytes == 1)" 
			
		else if (numberOutBytes == kForce2Bytes) 
			{
			UInt32				index,
									sample,
									channel;
			HSInt16Ptr 			toIntPtr;
			HUCharPtr 			fromCharPtr;
			
			toIntPtr = (HSInt16Ptr)dataCharBufferPtr;
			fromCharPtr = ioCharBufferPtr;
			
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				while (index < numberChannels)
					{
					if ( channelListPtr )
						channel = channelListPtr[index];
						
					*toIntPtr = fromCharPtr[channel];
					toIntPtr++;
					index++;
					channel++;
					
					}		// end "for (index = 0; index < numberColumns;..." 
					
				fromCharPtr += skipSamples;
				
				}		// end "for (sample=0;  sample<numberChanSamples; ..." 
				
			}		// end "else if (numberOutBytes == kForce2Bytes)"  
			
		else		// numberOutBytes == kForce4Bytes 
			{
			if (signedInDataFlag)
				{
				UInt32				index,
										sample,
										channel;
				HSInt32Ptr 			toIntPtr;
				HSInt8Ptr 			fromCharPtr;
				
				toIntPtr = (HSInt32Ptr)dataCharBufferPtr;
				fromCharPtr = (HSInt8Ptr)ioCharBufferPtr;
				
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					index = 0;
					channel = 0;
					while (index < numberChannels)
						{
						if ( channelListPtr )
							channel = channelListPtr[index];
							
						*toIntPtr = fromCharPtr[channel];
						toIntPtr++;
						index++;
						channel++;
						
						}		// end "for (index = 0; index < numberColumns;..." 
						
					fromCharPtr += skipSamples;
					
					}		// end "for (sample=0;  sample<numberChanSamples; ..."
					
				}		// end "if (signedInDataFlag)" 
				
			else		// !signedInDataFlag
				{
				UInt32				index,
										sample,
										channel;
				HSInt32Ptr 			toIntPtr;
				HUCharPtr 			fromCharPtr;
				
				toIntPtr = (HSInt32Ptr)dataCharBufferPtr;
				fromCharPtr = ioCharBufferPtr;
				
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					index = 0;
					channel = 0;
					while (index < numberChannels)
						{
						if ( channelListPtr )
							channel = channelListPtr[index];
							
						*toIntPtr = fromCharPtr[channel];
						toIntPtr++;
						index++;
						channel++;
						
						}		// end "for (index = 0; index < numberColumns;..." 
						
					fromCharPtr += skipSamples;
					
					}		// end "for (sample=0;  sample<numberChanSamples; ..."
					
				}		// end "else !signedInDataFlag" 
				
			}		// end "else numberOutBytes == kForce4Bytes" 
			
		}		// end "if (numberBytes == 1)" 
	
	if (numberInBytes == 2)
		{
		if (numberOutBytes == kDoNotForceBytes || numberOutBytes == kForce2Bytes)
			{
			UInt32					index,
										sample,
										channel;
			HSInt16Ptr 				fromIntPtr;
			HSInt16Ptr 				toIntPtr;
			
			toIntPtr = (HSInt16Ptr)dataCharBufferPtr;
			fromIntPtr = (HSInt16Ptr)ioCharBufferPtr;
				
			for (sample=0;  sample<numberSamples; sample+=columnInterval)
				{
				index = 0;
				channel = 0;
				while (index < numberChannels)
					{
					if ( channelListPtr )
						channel = channelListPtr[index];
							
					*toIntPtr = fromIntPtr[channel];
					toIntPtr++;
					index++;
					channel++;
					
					}		// end "for (index = 0; index < numberColumns;..." 
					
				fromIntPtr += skipSamples;
				
				}		// end "for (sample=0;  sample<numberChanSamples; ..."
				
			}		// end "if (numberOutBytes == kDoNotForceBytes || ...)" 
			
		else		// numberOutBytes == kForce4Bytes 
			{
			if (signedInDataFlag)
				{
				UInt32				index,
										sample,
										channel;
				HSInt32Ptr 			toIntPtr;
				HSInt16Ptr 			fromCharPtr;
				
				toIntPtr = (HSInt32Ptr)dataCharBufferPtr;
				fromCharPtr = (HSInt16Ptr)ioCharBufferPtr;
				
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					index = 0;
					channel = 0;
					while (index < numberChannels)
						{
						if ( channelListPtr )
							channel = channelListPtr[index];
							
						*toIntPtr = fromCharPtr[channel];
						toIntPtr++;
						index++;
						channel++;
						
						}		// end "for (index = 0; index < numberColumns;..." 
						
					fromCharPtr += skipSamples;
					
					}		// end "for (sample=0;  sample<numberChanSamples; ..."
					
				}		// end "if (signedInDataFlag)" 
				
			else		// !signedInDataFlag
				{
				UInt32				index,
										sample,
										channel;
				HSInt32Ptr 			toIntPtr;
				HUInt16Ptr 			fromCharPtr;
				
				toIntPtr = (HSInt32Ptr)dataCharBufferPtr;
				fromCharPtr = (HUInt16Ptr)ioCharBufferPtr;
				
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					index = 0;
					channel = 0;
					while (index < numberChannels)
						{
						if ( channelListPtr )
							channel = channelListPtr[index];
							
						*toIntPtr = fromCharPtr[channel];
						toIntPtr++;
						index++;
						channel++;
						
						}		// end "for (index = 0; index < numberColumns;..." 
						
					fromCharPtr += skipSamples;
					
					}		// end "for (sample=0;  sample<numberChanSamples; ..."
					
				}		// end "else !signedInDataFlag" 
				
			}		// end "else numberOutBytes == kForce4Bytes"  
			
		}		// end "if (numberInBytes == 2)" 
		
}		// end "PackBISData"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PackBlockedData
//
//	Software purpose:	The purpose of this routine is to pack the data for those
//							image files that are stored in a tiled or blocked format.
//							
//							Note that the 'bytesPerColumn' variable represents the number
//								of bytes per column of data. If the band interleave format
//								is BIS then this represents the number of bytes for all
//								channels in that one column.
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 10/27/1999

void PackBlockedData (
				UInt32								line,
				UInt32								columnStart, 
				UInt32								columnEnd,
				UInt32								channelStartIndex,
				FileInfoPtr							fileInfoPtr,
				HierarchalFileFormatPtr			hfaPtr,
				HUCharPtr							outputBufferPtr)

{
	HUCharPtr							inputBufferPtr;
	
	UInt32								bytesPerColumn,
											block,
											blockStartByte,
											bytesToMove,
											firstByteToMove,
											firstColumnRead,
											numberBlocksRead;
	
	
	inputBufferPtr = hfaPtr->tiledBufferPtr;
	
	bytesPerColumn = fileInfoPtr->numberBytes;
	if (fileInfoPtr->bandInterleave == kBIS)
		bytesPerColumn *= fileInfoPtr->numberChannels;
	
	blockStartByte = 
			(line - hfaPtr->firstLineRead) * hfaPtr->blockWidth * bytesPerColumn;
	
	if (fileInfoPtr->numberBits == 4)
		blockStartByte /= 2;
	
			// For the BIS band interleave case, channelStartIndex will always be 0.
				
	blockStartByte += channelStartIndex * hfaPtr->blockSize;
	
			// This section is needed for the case when the first column being
			// read changes such as it does in the cluster processors.
			
	if (columnStart - hfaPtr->firstColumnRead >= hfaPtr->blockWidth)
		{
		numberBlocksRead = hfaPtr->numberBlocksRead - 1;
		firstColumnRead = hfaPtr->firstColumnRead + hfaPtr->blockWidth;
		
		}		// end "if (columnStart - hfaPtr->firstColumnRead >= ...->blockWidth)"
		
	else		// columnStart - hfaPtr->firstColumnRead < hfaPtr->blockWidth
		{
		numberBlocksRead = hfaPtr->numberBlocksRead;
		firstColumnRead = hfaPtr->firstColumnRead;
		
		}		// end "if (columnStart - hfaPtr->firstColumnRead >= ...->blockWidth)"
	
	for (block=1; block<=numberBlocksRead; block++)
		{
		firstByteToMove = blockStartByte;
			
		if (block == 1)
			{
			if (fileInfoPtr->numberBits != 4)
				{
				firstByteToMove += (columnStart - firstColumnRead) * bytesPerColumn;
			
				if (numberBlocksRead == 1)
					bytesToMove = columnEnd - columnStart + 1;
			
				else		// numberBlocksRead > 1
					bytesToMove = firstColumnRead + hfaPtr->blockWidth - columnStart;
								
				}		// end "if (fileInfoPtr->numberBits != 4)"
								
			else		// fileInfoPtr->numberBits == 4
				{
				firstByteToMove += (columnStart - firstColumnRead) * bytesPerColumn / 2;
			
				if (numberBlocksRead == 1)
					{
					bytesToMove = columnEnd - columnStart + 1;
				
					if ( !(columnStart & 0x00000001) && (columnEnd & 0x00000001) )
						bytesToMove++;
					
					}		// end "if (numberBlocksRead == 1)"
			
				else		// numberBlocksRead > 1
					bytesToMove = firstColumnRead + hfaPtr->blockWidth - columnStart;
				
				}		// end "else fileInfoPtr->numberBits == 4"
			
			}		// end "if (block == 1)"
			
		else if (block != numberBlocksRead)
			{
			bytesToMove = hfaPtr->blockWidth;
			
			}		// end "else if (block != numberBlocksRead)" 
			
		else		// block == numberBlocksRead
			{
			bytesToMove = columnEnd - (hfaPtr->lastColumnRead - hfaPtr->blockWidth);
			
			}		// end "else if (block != numberBlocksRead)" 
			
		bytesToMove *= bytesPerColumn;
			
		if (fileInfoPtr->numberBits == 4)
			{
			bytesToMove = (bytesToMove+1)/2;
			
			}		// end "else fileInfoPtr->numberBits == 4"
			
		BlockMoveData (&inputBufferPtr[firstByteToMove], 
								outputBufferPtr, 
								(SInt32)bytesToMove);
								
		outputBufferPtr += bytesToMove;
			
		blockStartByte += hfaPtr->blockOffset;
		
		}		// end "for (block=1; block<=numberBlocks; block++)"
		
}		// end "PackBlockedData"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PackGAIAData
//
//	Software purpose:	The purpose of this routine is to pack the data from
//							a line of GAIA data.  This includes removing the 
//							segmented bytes that will be at columns 1016, 2038,
//							3060, 4082, etc
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1994
//	Revised By:			Larry L. Biehl			Date: 05/03/1995

void PackGAIAData (
				SInt32								columnStart, 
				SInt32								columnEnd, 
				HSInt16Ptr							inputBufferPtr)

{
	HSInt16Ptr							outputBufferPtr;
	
	SInt32								lastColumnMoved,
											samplesToMove;
	
	
	outputBufferPtr = inputBufferPtr;
	
			// Determine the bytes to skip which will not have to be moved.	
	
	lastColumnMoved = 1015;
	lastColumnMoved = MIN(lastColumnMoved, columnEnd);
	
	samplesToMove = lastColumnMoved - columnStart + 1;
	
	while (lastColumnMoved < columnEnd)
		{
		inputBufferPtr += samplesToMove + 1;
		outputBufferPtr += samplesToMove;
	
		columnStart += samplesToMove;
		lastColumnMoved += 1021;
		lastColumnMoved = MIN(lastColumnMoved, columnEnd);
		
		samplesToMove = lastColumnMoved - columnStart + 1;
		
		BlockMoveData (inputBufferPtr, outputBufferPtr, samplesToMove*2);
		
		}		// end "while (lastColumnMoved < columnEnd)" 
		
}		// end "PackGAIAData"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 PackMaskData
//
//	Software purpose:	The purpose of this routine is to reduce the input line of data
//							to just that requested by the mask information.
//
//	Parameters in:				
//
//	Parameters out:	Number of mask samples included in the buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/28/1998
//	Revised By:			Larry L. Biehl			Date: 12/21/1998

UInt32 PackMaskData (
				HUInt16Ptr							maskBufferPtr,
				UInt32								maskColumnStart,
				UInt32								numberChannels,
				UInt32								numberInputSamples,
				UInt32								columnInterval,
				HUCharPtr							dataBufferPtr,
				UInt32								numberOutBytes,
				SInt32								maskValueRequest)

{ 
	HUCharPtr							inDataBufferPtr,
											outDataBufferPtr;
											
	UInt32								numberBytesToMove,
											numberOutputSamples,
											sample;
	
	
			// Get the relative position to start reading in the mask buffer with 
			// respect to the start of the file.					
			
	maskBufferPtr += maskColumnStart;
	
	inDataBufferPtr = dataBufferPtr;
	outDataBufferPtr = dataBufferPtr;
	numberBytesToMove = numberOutBytes * numberChannels;
	numberOutputSamples = 0;
	
	if (maskValueRequest == 0)
		{										
		for (sample=0; sample<numberInputSamples; sample+=columnInterval)
			{
			if (*maskBufferPtr != 0)
				{
				if (inDataBufferPtr != outDataBufferPtr)
					BlockMoveData (inDataBufferPtr, outDataBufferPtr, numberBytesToMove);
					
				outDataBufferPtr += numberBytesToMove;
				numberOutputSamples++;
				
				}		// end "if (maskBufferPtr[maskIndex] != 0)"
				
			inDataBufferPtr += numberBytesToMove;
				
			maskBufferPtr += columnInterval;
				
			}		// end "for (sample=0; sample<count; sample++)"
			
		}		// end "if (maskValueRequest == 0)"
		
	else		// maskValueRequest != 0
		{										
		for (sample=0; sample<numberInputSamples; sample+=columnInterval)
			{
			if ((SInt32)*maskBufferPtr == maskValueRequest)
				{
				if (inDataBufferPtr != outDataBufferPtr)
					BlockMoveData (inDataBufferPtr, outDataBufferPtr, numberBytesToMove);
					
				outDataBufferPtr += numberBytesToMove;
				numberOutputSamples++;
				
				}		// end "if (maskBufferPtr[maskIndex] == maskValueRequest)"
				
			inDataBufferPtr += numberBytesToMove;
				
			maskBufferPtr += columnInterval;
				
			}		// end "for (sample=0; sample<count; sample++)"
			
		}		// end "else maskValueRequest != 0"
		
	return (numberOutputSamples);
	      
}		// end "PackMaskData"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void PackNonBISData
//
//	Software purpose:	The purpose of this routine is to pack the line
//							of data for nonBIS formated files if the column
//							interval is more than 1 and/or convert the line
//							of data to BIS formatted style.
//
//	Parameters in:				
//
//	Parameters out:	Number of samples loaded into buffer
//
//	Value Returned:	Error code for file operations. 
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1989
//	Revised By:			Larry L. Biehl			Date: 02/15/2003

void PackNonBISData (
				UInt32								columnOffset,
				UInt32								columnInterval,
				UInt32								numberColumnsPerChannel, 
				UInt32								numberBufferChannels,
				UInt32								numberChannels,
				HUInt16Ptr							channelListPtr, 
				UInt32								numberSamples,
				UInt32								numberInBytes, 
				HUCharPtr							ioCharBufferPtr, 
				HUCharPtr							dataCharBufferPtr, 
				Boolean								toBISFormat, 
				UInt32								numberOutBytes,
				Boolean								swapBytesFlag,
				Boolean								signedInDataFlag)

{
	UInt32								channel,
											currentChannel,
											dataPtrIncrement,
											sample;
	
	
			// Get increment to use for buffer depending on if data is to be 		
			// converted to BIS format.														
			
	dataPtrIncrement = 1;
	if (toBISFormat)
		dataPtrIncrement = numberChannels;
		
			// Set up parameters for skipping some input channels if needed.
		
	currentChannel = 0;	
	if (channelListPtr != NULL)
		currentChannel = channelListPtr[0];
	
	if (numberInBytes == 1)
		{
		if (numberOutBytes == kDoNotForceBytes || numberOutBytes == kForce1Byte)
			{
			HUCharPtr			dataCharPtr;
			HUCharPtr			ioCharPtr;
			
			dataCharPtr = dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels. Allow for case when some of
					// the input channels need to be skipped.	
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}		// end "while (channelListPtr[channel] != currentChannel)"
						
					}		// end "if (channelListPtr != NULL)"
				
				if (toBISFormat)
					dataCharPtr = &dataCharBufferPtr[channel];
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataCharPtr = ioCharPtr[sample];
					dataCharPtr += dataPtrIncrement;
					
					}		// end "for (sample=0;  sample<numberChan..."
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}		// end "for (channel=0; channel<...)" 
				
			}		// end "if (numberOutBytes == kDoNotForceBytes || ..." 
			
		else if (numberOutBytes == kForce2Bytes)
			{
			HSInt16Ptr			dataIntPtr;
			HUCharPtr			ioCharPtr;
		
			dataIntPtr = (HSInt16Ptr)dataCharBufferPtr;
			ioCharPtr = ioCharBufferPtr;
			ioCharPtr = &ioCharPtr[columnOffset];
			
					// Loop through by number of channels. Allow for case when some of
					// the input channels need to be skipped.				
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}		// end "while (channelListPtr[channel] != currentChannel)"
						
					}		// end "if (channelListPtr != NULL)"
					
				if (toBISFormat)
					dataIntPtr = (HSInt16Ptr)&dataCharBufferPtr[channel*2];
			
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataIntPtr = ioCharPtr[sample];
					dataIntPtr += dataPtrIncrement;
					
					}		// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of	
						// data for next channel.												
						
				ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}		// end "for (channel=0; channel<...)" 
			
			if (swapBytesFlag)
				{
				numberSamples = (numberSamples + columnInterval)/columnInterval;
				numberSamples *= numberBufferChannels;
				
				SwapBytes ( (HUInt16Ptr)dataCharBufferPtr, numberSamples );
				
				}		// end "if (swapBytesFlag)" 
					
			}		// end "else if (numberOutBytes == kForce2Bytes)" 
			
		else 		// numberOutBytes == kForce4Bytes
			{
			if (signedInDataFlag)
				{
				HSInt32Ptr			dataSInt32Ptr;
				HSInt8Ptr			ioCharPtr;
			
				dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
				ioCharPtr = (HSInt8Ptr)ioCharBufferPtr;
				ioCharPtr = &ioCharPtr[columnOffset];
				
						// Loop through by number of channels.									
					
				for (channel=0; channel<numberBufferChannels; channel++)
					{
					if (channelListPtr != NULL)
						{
						while (channelListPtr[channel] != currentChannel)
							{
							ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
							currentChannel++;
							
							}		// end "while (channelListPtr[channel] != currentChannel)"
							
						}		// end "if (channelListPtr != NULL)"
					
					if (toBISFormat)
						dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
				
					for (sample=0;  sample<numberSamples; sample+=columnInterval)
						{
						*dataSInt32Ptr = ioCharPtr[sample];
						dataSInt32Ptr += dataPtrIncrement;
						
						}		// end "for (sample=0;  sample<numberChan..." 
						
							// Update the input buffer pointer to point to beginning of	
							// data for next channel.												
							
					ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
					currentChannel++;
					
					}		// end "for (channel=0; channel<...)"
					
				}		// end "if (signedInDataFlag)"
				
			else		// !signedInDataFlag
				{
				HSInt32Ptr			dataSInt32Ptr;
				HUInt8Ptr			ioCharPtr;
			
				dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
				ioCharPtr = ioCharBufferPtr;
				ioCharPtr = &ioCharPtr[columnOffset];
				
						// Loop through by number of channels.									
					
				for (channel=0; channel<numberBufferChannels; channel++)
					{
					if (channelListPtr != NULL)
						{
						while (channelListPtr[channel] != currentChannel)
							{
							ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
							currentChannel++;
							
							}		// end "while (channelListPtr[channel] != currentChannel)"
							
						}		// end "if (channelListPtr != NULL)"
					
					if (toBISFormat)
						dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
				
					for (sample=0;  sample<numberSamples; sample+=columnInterval)
						{
						*dataSInt32Ptr = ioCharPtr[sample];
						dataSInt32Ptr += dataPtrIncrement;
						
						}		// end "for (sample=0;  sample<numberChan..." 
						
							// Update the input buffer pointer to point to beginning of	
							// data for next channel.												
							
					ioCharPtr = &ioCharPtr[numberColumnsPerChannel];
					currentChannel++;
					
					}		// end "for (channel=0; channel<...)"
				
				}		// else !signedInDataFlag
					
			}		// end "else numberOutBytes == kForce4Bytes" 
			
		}		// end "if (numberInBytes == 1)" 
				
	else		// numberInBytes == 2 
		{
		if (numberOutBytes == kDoNotForceBytes || numberOutBytes == kForce2Bytes)
			{
			HSInt16Ptr					dataIntPtr,
											ioIntPtr;
		
			dataIntPtr = (HSInt16Ptr)dataCharBufferPtr;
			
			ioIntPtr = (HSInt16Ptr)ioCharBufferPtr;
			ioIntPtr = &ioIntPtr[columnOffset];
			
					// Loop through by number of channels.										
				
			for (channel=0; channel<numberBufferChannels; channel++)
				{
				if (channelListPtr != NULL)
					{
					while (channelListPtr[channel] != currentChannel)
						{
						ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
						currentChannel++;
						
						}		// end "while (channelListPtr[channel] != currentChannel)"
						
					}		// end "if (channelListPtr != NULL)"
					
				if (toBISFormat)
					dataIntPtr = (HSInt16Ptr)&dataCharBufferPtr[channel*2];
					
				for (sample=0;  sample<numberSamples; sample+=columnInterval)
					{
					*dataIntPtr = ioIntPtr[sample];
					dataIntPtr += dataPtrIncrement;
					
					}		// end "for (sample=0;  sample<numberChan..." 
					
						// Update the input buffer pointer to point to beginning of		
						// data for next channel.													
				
				ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
				currentChannel++;
				
				}		// end "for (channel=0; channel<numberBufferChannels; channel++)" 
				
			}		// end "if (numberOutBytes == kDoNotForceBytes || ..."
			
		else		 // numberOutBytes == kForce4Bytes
			{
			if (signedInDataFlag)
				{
				HSInt32Ptr					dataSInt32Ptr;
				HSInt16Ptr					ioIntPtr;
			
				dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
				ioIntPtr = (HSInt16Ptr)ioCharBufferPtr;
				ioIntPtr = &ioIntPtr[columnOffset];
				
						// Loop through by number of channels.										
					
				for (channel=0; channel<numberBufferChannels; channel++)
					{
					if (channelListPtr != NULL)
						{
						while (channelListPtr[channel] != currentChannel)
							{
							ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
							currentChannel++;
							
							}		// end "while (channelListPtr[channel] != currentChannel)"
							
						}		// end "if (channelListPtr != NULL)"
					
					if (toBISFormat)
						dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
						
					for (sample=0;  sample<numberSamples; sample+=columnInterval)
						{
						*dataSInt32Ptr = ioIntPtr[sample];
						dataSInt32Ptr += dataPtrIncrement;
						
						}		// end "for (sample=0;  sample<numberChan..." 
						
							// Update the input buffer pointer to point to beginning of		
							// data for next channel.													
					
					ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
					currentChannel++;
					
					}		// end "for (channel=0; channel<numberBufferChannels; channel++)"
					
				}		// end "if (signedInDataFlag)"
				
			else		// !signedInDataFlag
				{
				HSInt32Ptr					dataSInt32Ptr;
				HUInt16Ptr					ioIntPtr;
			
				dataSInt32Ptr = (HSInt32Ptr)dataCharBufferPtr;
				ioIntPtr = (HUInt16Ptr)ioCharBufferPtr;
				ioIntPtr = &ioIntPtr[columnOffset];
				
						// Loop through by number of channels.										
					
				for (channel=0; channel<numberBufferChannels; channel++)
					{
					if (channelListPtr != NULL)
						{
						while (channelListPtr[channel] != currentChannel)
							{
							ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
							currentChannel++;
							
							}		// end "while (channelListPtr[channel] != currentChannel)"
							
						}		// end "if (channelListPtr != NULL)"
					
					if (toBISFormat)
						dataSInt32Ptr = (HSInt32Ptr)&dataCharBufferPtr[channel*4];
						
					for (sample=0;  sample<numberSamples; sample+=columnInterval)
						{
						*dataSInt32Ptr = ioIntPtr[sample];
						dataSInt32Ptr += dataPtrIncrement;
						
						}		// end "for (sample=0;  sample<numberChan..." 
						
							// Update the input buffer pointer to point to beginning of		
							// data for next channel.													
					
					ioIntPtr = &ioIntPtr[numberColumnsPerChannel];
					currentChannel++;
					
					}		// end "for (channel=0; channel<numberBufferChannels; channel++)"
					
				}		// end "if (!signedInDataFlag)"
				
			}		// end "else numberOutBytes == kForce4Bytes"
				
		}		// end "else numberInBytes == 2" 

}		// end "PackNonBISData"   



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 PutFile
//
//	Software purpose:	The purpose of this routine is to put up the
//							standard put file dialog box to allow the user to
//							select the file to write to.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Error code for file operations.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: ??/??/1988
//	Revised By:			Larry L. Biehl			Date: 10/13/1997	

SInt16 PutFile (
				CMFileStream*		 				fileStreamPtr, 
				SInt32								numberBytes, 
				SInt16								stringIndex, 
				SInt32								creator)

{
	SInt16								errCode = noErr;
	
	#if !TARGET_API_MAC_CARBON	
		Boolean								fileSelected;
	#endif	// !TARGET_API_MAC_CARBON	
	
	#if !defined _X86_ 
	
		#if !TARGET_API_MAC_CARBON	
		CMFileStream						tempFileStream;
		Point									where;
		
		SInt32								bytesNeeded;
	
		UInt32								freeBytes,
												logicalEndOfFile;
		#endif	// !TARGET_API_MAC_CARBON	
		
	
		if (gNavServicesExistsFlag)
			{
			errCode = NavServicesPutFile (fileStreamPtr, 
														numberBytes, 
														stringIndex, 
														creator);
	
																						return (errCode);
					
			}		// end "if (gNavServicesExistsFlag)"

		#if !TARGET_API_MAC_CARBON		
		
				// Make certain that MultiSpec is not in the background.					
				
		PauseIfInBackground ();
		
				// Calculate point where dialog will be centered. 							
				
		GetDlogOrigin (putDlgID, &where);
		
		fileSelected = FALSE;
		
		if ( !CheckIfMemoryAvailable ( 6000 ) )
																							return(1);
		
		do	
			{
			gGetFileStatus = -1;
			if (stringIndex == IDS_SaveProjectFile)
				gGetFileStatus = 0;
		
			ReleaseSpareMemoryForWarningMessage ();
			
			GetIndString (gTextString3, kFileIOStrID, stringIndex);
			
			SFPutFile (where, 
							gTextString3, 
							fileStreamPtr->fileName, 
							gPutFileHookPtr, 
							&gFileReply);
/*															
			CustomPutFile (gTextString3, 
								fileStreamPtr->fileName,
								standardFileReply,
								0,
								where,
								gPutFileHookPtr,
								NULL,
								NULL,
								NULL,
								NULL);
*/			
					// Get the spare memory back again.											
				
			SetHandleSize (gSpareWarnMemoryH, (Size)gSpareWarnSize);
			
			CheckSomeEvents (activMask+updateMask);
		
			if (!gFileReply.good)
				{
				fileStreamPtr->fileName[0] = 0;
																							return(0);
				
				}		// end "if (!gFileReply.good)" 
				
					// Check if enough space on the volume.									
					
			errCode = GetVolumeFreeSpace ( (Str255*)&gFileReply.fName, 
													gFileReply.vRefNum, 
													&freeBytes);
																
			if (errCode != noErr)												
																					return(errCode);
			
					// Use tempFileStream structure as temporary storage for file 		
					// information.			
					
	    	tempFileStream.vRefNum = gFileReply.vRefNum;
	    	tempFileStream.parID = 0;
			CopyPToP ( (char*)tempFileStream.fileName, (char*)gFileReply.fName);
	    	tempFileStream.refNum = 0;
	    	tempFileStream.type = fileStreamPtr->type;
	    	tempFileStream.fSSpecFlag = FALSE;
			
					// If the project file is being saved, the number of bytes for		
					// the file needs to be calculated here since the user can make	
					// changes in the PutFile dialog that affects the size of the		
					// save project file.															
			
			if (stringIndex == IDS_SaveProjectFile)
				numberBytes = GetSizeOfProjectFile ();
			
					// Check if there is enough space on the disk.  If there is not	
					// enough space, even when considering whether an old file is to	
					// be replaced then allow the user to select the file/disk again.	
			
			bytesNeeded = numberBytes + 100 - freeBytes;
			if ( bytesNeeded > 0 )
				{
						// Get the number of bytes in the old file if one is being		
						// replaced.																	
				
				errCode = GetSizeOfFile (&tempFileStream, &logicalEndOfFile);
				if (errCode != noErr && errCode != fnfErr )
																						return (errCode);
					
				bytesNeeded = bytesNeeded - logicalEndOfFile;
				
				if (bytesNeeded > 0)
					DiskFullAlert ( bytesNeeded, (Str255*)tempFileStream.fileName);
				else		
					fileSelected = TRUE;
				
				}		// end "if (bytesNeeded > 0)" 
				
			else 		
				fileSelected = TRUE;
				
					// Make certain that we can write to the file.							
					
			if (fileSelected)
				{
				errCode = FSMakeFSSpec (tempFileStream.vRefNum,
												0,
												tempFileStream.fileName,
												&gFileFSSpec);
					
				if (errCode == fnfErr ||  
						errCode == afpAccessDenied)
					errCode = noErr;
					
				if (errCode == noErr)	
					{
					tempFileStream.vRefNum = gFileFSSpec.vRefNum;
					tempFileStream.parID = gFileFSSpec.parID;
	    			tempFileStream.fSSpecFlag = TRUE;
				
					errCode = HRstFLock (tempFileStream.vRefNum, 
													tempFileStream.parID,
													tempFileStream.fileName);
					
					if (errCode == fnfErr ||  
							errCode == afpAccessDenied)
						errCode = noErr;
																					
					}		// end "if (errCode == noErr)"
							
				if (errCode != noErr)
					{
					IOCheck (errCode, (CharPtr)&tempFileStream.fileName);
					fileSelected = FALSE;
					
					}		// end "if (errCode != noErr)" 
					
				}		// end "if (fileSelected)" 
			
			} while (!fileSelected);
			
				// Create the file on the selected volume										
				
		errCode = CreateNewFile (&tempFileStream, 
											tempFileStream.vRefNum, 
											creator, 
											kErrorMessages, 
											kReplaceFlag);
					
		if (errCode == noErr)
			{
					// Now copy the file parameters to the supplied structure.		
					
	    	fileStreamPtr->vRefNum = tempFileStream.vRefNum;
	    	fileStreamPtr->parID = tempFileStream.parID;
			CopyPToP ( (char*)fileStreamPtr->fileName, (char*)tempFileStream.fileName);
	    	fileStreamPtr->type = tempFileStream.type;
	    	fileStreamPtr->refNum = tempFileStream.refNum;
	    	fileStreamPtr->fSSpecFlag = tempFileStream.fSSpecFlag;
			
			}		// end "if (errCode == noErr)"
			
		#endif	// !TARGET_API_MAC_CARBON	
	
	#endif	// !defined _X86_
	
	
	#if defined _X86_ 
		StringPtr						fileNamePtr;
		
		
		SInt16 filterStringIndex = IDS_FilterString;

		if (stringIndex == IDS_SaveProjectFile)
			filterStringIndex = IDS_ProjectFilterString;
		else if (stringIndex == IDS_FileIO61)
			filterStringIndex = IDS_TransformFilterString;
		else if (stringIndex == IDS_FileIO69)
			filterStringIndex = IDS_ImageFilterString;
		else if (stringIndex == IDS_FileIO75)
			filterStringIndex = IDS_ThematicFilterString;
		else if (stringIndex == IDS_SaveImageStats)
			filterStringIndex = IDS_StatFilterString;
		else if (stringIndex == IDS_SaveClassificationAs)
			filterStringIndex = IDS_ClassificationFilterString;
		else if (stringIndex == IDS_SaveProbabilityAs)
			filterStringIndex = IDS_ClassificationFilterString; 
			
		GetSpecifiedStringNumber (
								kFileIOStrID, filterStringIndex, &gTextString, TRUE );
		CFileDialog  temp(FALSE, 
						NULL, 
						NULL, 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
						(char*)&gTextString[1],
						NULL);
		temp.m_ofn.nFilterIndex = 1; 
	
		fileSelected = FALSE;
				
		fileNamePtr = fileStreamPtr->GetFileNamePtr();
			
		do	
			{
			gGetFileStatus = -1;
			if (stringIndex == IDS_SaveProjectFile)
				gGetFileStatus = 0;
				    
			CString newName = fileNamePtr;  
			
			StringHandle		stringHandle = NULL;
			CharPtr				stringPtr = NULL;

			temp.m_ofn.lpstrFile=(char*)fileNamePtr;
			if (stringIndex == IDS_SaveProjectFile)
				{
				temp.m_ofn.lpstrDefExt="prj";
				GetSpecifiedString(IDS_SaveProjectFile, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				}  
				
			else if (stringIndex == IDS_FileIO69)
				{
				temp.m_ofn.lpstrDefExt="tif";
				GetSpecifiedString(IDS_FileIO69, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				}   
				
			else if (stringIndex == IDS_FileIO61)
				{
				temp.m_ofn.lpstrDefExt="tra";
				GetSpecifiedString(IDS_FileIO61, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				}
				
			else if (stringIndex == IDS_FileIO75)
				{
				temp.m_ofn.lpstrDefExt="tra";
				GetSpecifiedString(IDS_FileIO75, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				} 
				
			else if (stringIndex == IDS_SaveImageStats)
				{
				temp.m_ofn.lpstrDefExt="sta";
				GetSpecifiedString(IDS_SaveImageStats, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				}
			
			else if (stringIndex == IDS_SaveClassificationAs)
				{
				temp.m_ofn.lpstrDefExt="gis";
				GetSpecifiedString(IDS_SaveClassificationAs, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				}
				
			else if (stringIndex == IDS_SaveProbabilityAs)
				{
				temp.m_ofn.lpstrDefExt="gis";
				GetSpecifiedString(IDS_SaveProbabilityAs, &stringHandle, &stringPtr);
				temp.m_ofn.lpstrTitle=&stringPtr[1];
				}
				
			if (temp.DoModal() == IDCANCEL)			
																	return(-1);

			fileNamePtr = (StringPtr)temp.m_ofn.lpstrFile; 
			
					// Get the path length.
					
			size_t stringLength = strlen((CharPtr)fileNamePtr);
			if (stringLength > gFileNameLengthLimit)
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert92,
									0, 
									NULL);
				
			else	// stringLength <= gFileNameLengthLimit
				{
				UChar pathName[_MAX_PATH];              	   
				CtoPstring ((CharPtr)fileNamePtr, (CharPtr)pathName);
				pathName[ pathName[0]+1 ] = 0;
				newName.ReleaseBuffer();
		   		
		   		fileNamePtr = pathName;
					
				fileSelected = TRUE;
				
				fileStreamPtr->SetFilePath(fileNamePtr, TRUE); 
				
				}		// end "else stringLength <= gFileNameLengthLimit"
						
			} while (!fileSelected);
			
				// Create the file on the selected volume										
				
		errCode = fileStreamPtr->MCreateNewFile ();
	
	#endif	// defined _X86_
	
	return (errCode);

}		// end "PutFile"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ResolveAnyAliases
//
//	Software purpose:	The purpose of this routine is to resolve the input
//							file aliases if the input file is an alias.
//
//	Parameters in:		Pointer to AppFile structure which contains the
//											volume reference number and file name.
//							Index in the list
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			GetImageNameFromSupportFile in fileIO.c
//							OpenFileReadOnly in fileIO.c
//
//	Coded By:			Larry L. Biehl			Date: 09/08/1993
//	Revised By:			Larry L. Biehl			Date: 10/24/1995	

SInt16 ResolveAnyAliases (
				CMFileStream*						fileStreamPtr, 
				Boolean*								wasAliasedFlagPtr)

{							
	SInt16								errCode = noErr;
	
	#if !defined _X86_ 
		Boolean								targetIsFolderFlag;
		
		
		errCode = noErr;
		*wasAliasedFlagPtr = FALSE;
				
		if (gAliasMgrPresentFlag)
			{
					// Load a FSSpec record.													
					
			if ( fileStreamPtr->fSSpecFlag )
				{
				gFileFSSpec.vRefNum = fileStreamPtr->vRefNum;
				gFileFSSpec.parID = fileStreamPtr->parID;
				CopyPToP ( (char*)gFileFSSpec.name, (char*)fileStreamPtr->fileName);
				
				}		// end "if ( fileInfoPtr->fSSpecFlag )" 
												
			else		// !fileInfoPtr->fSSpecFlag 
				errCode = FSMakeFSSpec (fileStreamPtr->vRefNum,
												0,
												fileStreamPtr->fileName,
												&gFileFSSpec);
				
			if (errCode == noErr)
				errCode = ResolveAliasFile ( 	&gFileFSSpec,
														TRUE,
														&targetIsFolderFlag,
														wasAliasedFlagPtr);
			
			if ( !targetIsFolderFlag && *wasAliasedFlagPtr && 
												(errCode == noErr || errCode == fnfErr) )
				{						
				fileStreamPtr->vRefNum = gFileFSSpec.vRefNum;
				fileStreamPtr->parID = gFileFSSpec.parID;
				CopyPToP ( (char*)fileStreamPtr->fileName, (char*)gFileFSSpec.name);
												
				fileStreamPtr->fSSpecFlag = TRUE;
				
				}		// end "if (!targetIsFolderFlag && wasAliasedFlag && ...)" 
			
			}		// end "if (gAliasMgrPresentFlag)"
				                        
	#endif	// !defined _X86_ 


	#if defined _X86_ 
	
		*wasAliasedFlagPtr = FALSE;
				                        
	#endif	// defined _X86_
		
	return (errCode);
			
}		// end "ResolveAnyAliases" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetCFileName
//
//	Software purpose:	The purpose of this routine is to set the file name in the
//							input file information structure to the new input c name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/28/2001
//	Revised By:			Larry L. Biehl			Date: 11/28/2001	

void SetCFileName (
				FileInfoPtr							fileInfoPtr,
				StringPtr							inputFileNameCPtr)

{  
	CMFileStream*						fileStreamPtr;
	
	
	if (fileInfoPtr != NULL && inputFileNameCPtr != NULL)
		{
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		SetCFileName (fileStreamPtr, inputFileNameCPtr);
		
		}		// end "if (fileInfoPtr != NULL && fileNameCPtr != NULL)"
	
}		// end "SetCFileName" 



void SetCFileName (
				CMFileStream*						fileStreamPtr,
				StringPtr							inputFileNameCPtr)

{  
	StringPtr							fileNamePtr,
											filePathPPtr;
	
	SInt32								characterOverRun,
											newNameLength,
											oldNameLength,
											oldPathNameLength;
	
	
	if (fileStreamPtr != NULL && inputFileNameCPtr != NULL)
		{
				// First make sure the current file is closed before changing
				// the name.
		
		CloseFile (fileStreamPtr);
		
				// Get the location of the start of the file name. We are assuming 
				// that the new file name is in the same folder as the previous file
				// name.
		
		fileNamePtr = GetFileNameCPointer (fileStreamPtr);
		filePathPPtr = GetFilePathPPointer (fileStreamPtr);
		
		oldNameLength = strlen( (char*)fileNamePtr );
		newNameLength = strlen( (char*)inputFileNameCPtr );
		oldPathNameLength = filePathPPtr[0];
		
		characterOverRun = 
			oldPathNameLength - oldNameLength + newNameLength - gFileNameLengthLimit;
			
		if (characterOverRun > 0)
			newNameLength -= characterOverRun;
			
		newNameLength = MAX (newNameLength, 0);
		
		strncpy ( (char*)fileNamePtr, (char*)inputFileNameCPtr, newNameLength);
		
				// Make sure that there is a c terminator.
				
		fileNamePtr[newNameLength] = 0;
		
				// Now update the character count.
		
		filePathPPtr[0] = strlen ((char*)&filePathPPtr[1]);
		
		}		// end "if (fileStreamPtr != NULL && fileNameCPtr != NULL)"
	
}		// end "SetCFileName"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFileDoesNotExist
//
//	Software purpose:	This routine sets the information in the input file stream
//							structure (class) to indicate that a file has not been
//							defined yet.
//
//	Parameters in:		The file stream pointer			
//
//	Parameters out:	None			
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/24/1995
//	Revised By:			Larry L. Biehl			Date: 01/31/2003

void SetFileDoesNotExist (
				CMFileStream*						fileStreamPtr)

{
			// Initialize for pascal and c strings.
			
	if (fileStreamPtr != NULL)
		{	
		#if !defined _X86_
			fileStreamPtr->fileName[0] = 0; 
			fileStreamPtr->fileName[1] = 0;
		#endif	// !defined _X86_
		
		#if defined _X86_
			fileStreamPtr->mFileType = -1;
			fileStreamPtr->mFilePathName[0] = 0;
			fileStreamPtr->mFilePathName[1] = 0;
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)"

}		// end "SetFileDoesNotExist"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFileReadOnly
//
//	Software purpose:	This routine sets the given file to read only permission.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			ChangeErdasHeader in reformat.c
//							ModifyChannelDescriptions in reformat.c
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1988
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

void SetFileReadOnly (
				CMFileStream*						fileStreamPtr)

{
	SInt16								errCode;
	
	
			// Close the file																		
			// Set to read only permission and reopen										
		
	CloseFile (fileStreamPtr);
	
	errCode = OpenFileReadOnly (fileStreamPtr, 
											kDontResolveAliasChains, 
											kLockFile);				
	IOCheck (errCode, fileStreamPtr);
	
}		// end "SetFileReadOnly"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int SetFileSizeToZero
//
//	Software purpose:	The purpose of this routine is to set the size of the
//							file to '0'.  In other words make the file empty.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			CreateNewFile in fileIO.c
//							GetProjectFileName in project.c
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1989
//	Revised By:			Larry L. Biehl			Date: 04/09/1996

SInt16 SetFileSizeToZero (
				CMFileStream*						fileStreamPtr)

{
	SInt16							errCode,
										errCode2;
										
	
	errCode = SetFileWriteEnabled (fileStreamPtr); 
		
	if (errCode == noErr) 
		{                   
		#if !defined _X86_
			errCode2 = SetEOF (fileStreamPtr->refNum, (SInt32)0 );
		#endif	// !defined _X86_
	                  
		#if defined _X86_ 
			errCode2 = fileStreamPtr->MSetSizeOfFile (0); 
		#endif	// defined _X86_
			
		IOCheck (errCode2, fileStreamPtr);
			
		}		// end "if (errCode == noErr)"
		
	if (errCode == noErr)
		errCode = errCode2;
			
	if (errCode != noErr)
		
				// There is a file error.  Close the file and return error code.	
					
		CloseFile (fileStreamPtr); 
		
	return (errCode);
				
}		// end "SetFileSizeToZero" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SetFileWriteEnabled
//
//	Software purpose:	This routine sets the given file to write enabled permission.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			WriteErdasHeader in saveWrite.c
//							InsertNewErdasHeader in reformat.c
//							ModifyChannelDescriptions in reformat.c
//							SetFileSizeToZero in fileIO.c	
//
//	Coded By:			Larry L. Biehl			Date: 09/10/1992
//	Revised By:			Larry L. Biehl			Date: 10/24/1995

SInt16 SetFileWriteEnabled (
				CMFileStream* 						fileStreamPtr)

{
	SInt16								errCode;
	
	
			// First close the file
			
	CloseFile (fileStreamPtr);
		
	#if !defined _X86_
																		
				// Now set to write permission and reopen	
		
		if (fileStreamPtr->fSSpecFlag)
			errCode = FSpOpenDF (	(FSSpec*)fileStreamPtr, 
											fsWrPerm, 
											&fileStreamPtr->refNum);
		
		else		// fileStreamPtr->fSSpecFlag 									
			errCode = HOpen (	fileStreamPtr->vRefNum, 
									0, 
									fileStreamPtr->fileName, 
									fsWrPerm, 
									&fileStreamPtr->refNum);
																			
		IOCheck (errCode, (CharPtr)&fileStreamPtr->fileName);
	
	#endif	// !defined _X86_
	
	#if defined _X86_
																			
				// Set to write permission and reopen
		
		errCode = fileStreamPtr->MOpenFile (
							(SInt16)(CFile::modeReadWrite|CFile::typeBinary),
							(SInt16)kErrorMessages);
	
	#endif	// defined _X86_
	
	return (errCode);
	
}		// end "SetFileWriteEnabled"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetFSSpecFlag
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002

void SetFSSpecFlag (
				CMFileStream*						fileStreamPtr,
				Boolean								fSSpecFlag)

{  
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	fileStreamPtr->fSSpecFlag = fSSpecFlag;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 
	
}		// end "SetFSSpecFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetParID
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 10/31/2002

void SetParID (
				CMFileStream*						fileStreamPtr,
				SInt32								parID)

{  
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	fileStreamPtr->parID = parID;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 
	
}		// end "SetParID" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetReferenceNumber
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							reference number.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995	

void SetReferenceNumber (
				CMFileStream*						fileStreamPtr,
				SInt16								refNum)

{  
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	fileStreamPtr->refNum = refNum;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 
	
}		// end "SetReferenceNumber" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetType
//
//	Software purpose:	The purpose of this routine is to set the input file 
//							type.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1995
//	Revised By:			Larry L. Biehl			Date: 10/26/1995	

void SetType (
				CMFileStream*						fileStreamPtr,
				SInt32								type)

{  
	if (fileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	                          
	   	fileStreamPtr->type = type;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_             
	                          
	   	fileStreamPtr->mFileType = type;
	 
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)" 
	
}		// end "SetType"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpFileIOInstructions
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			BiPlotFieldData in biPlotData.c
//							ClassifyPerPointArea in SClassfy.cpp
//							ClusterClassification in SCluster.cpp
//							CreateClusterMaskFile in SCluster.cpp
//							GetClusterAreaStatistics in SCluster.cpp
//							ISODATAClusterPass in SClustID.cpp
//							OnePassClusterAreas in SClustSP.cpp
//							ListFieldData in SLstData.cpp
//							GetAreaResults in SLstRslt.cpp
//							GetAreaStats in SMatUtil.cpp
//							GetFieldDataValues in SPMatUtl.cpp
//							HistogramFieldStats in statHistogram.c
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1998
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

SInt16 SetUpFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				AreaDescription* 					areaDescriptionPtr,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,
				UInt16								specialBILCode)

{
	SInt32								errCode;
	

	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													areaDescriptionPtr,
													areaDescriptionPtr->lineStart,
													areaDescriptionPtr->lineEnd,
													areaDescriptionPtr->lineInterval,
													areaDescriptionPtr->columnStart,
													areaDescriptionPtr->columnEnd,
													areaDescriptionPtr->columnInterval,
													numberListChannels,
													channelListPtr,
													specialBILCode);
													
	return ( (SInt16)errCode );
	
}		// end "SetUpFileIOInstructions"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpFileIOInstructions
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			DisplayCImage in SDisMulc.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/13/1999
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

SInt16 SetUpFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				AreaDescription* 					areaDescriptionPtr,
				UInt32								lineStart,
				UInt32								lineEnd,
				UInt32								lineInterval,
				UInt32								columnStart,
				UInt32								columnEnd,
				UInt32								columnInterval,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,
				UInt16								specialBILCode)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	SInt32								errCode = noErr;
	
	Boolean								BILSpecialFlag,
											callPackLineOfDataFlag,
											differentBuffersFlag;
	
	
	if (fileIOInstructionsPtr == NULL)
																					return (noErr);
																								
			// Load Some of the File IO Instructions structure.
			
	fileIOInstructionsPtr->columnEnd = columnEnd;
	fileIOInstructionsPtr->columnInterval = columnInterval;
	fileIOInstructionsPtr->columnStart = columnStart;
	
	fileIOInstructionsPtr->lineEnd = lineEnd;
	fileIOInstructionsPtr->lineInterval = lineInterval;
	fileIOInstructionsPtr->lineStart = lineStart;
	
	fileIOInstructionsPtr->numberOutputBufferSamples = 
			(fileIOInstructionsPtr->columnEnd - fileIOInstructionsPtr->columnStart + 
					fileIOInstructionsPtr->columnInterval)/fileIOInstructionsPtr->columnInterval;
	
	fileIOInstructionsPtr->channelListPtr = channelListPtr;
	fileIOInstructionsPtr->numberChannels = numberListChannels;
						
			// Set flag to indicate whether entire line of data for all 		
			// channels can be read in at one time, rather than separate 		
			// reads for each channel.														
		
	fileIOInstructionsPtr->oneReadFlag = GetBILSpecial(
															fileIOInstructionsPtr, 
															channelListPtr, 
															numberListChannels, 
															columnStart,
															columnEnd,
															1,
															specialBILCode);
	
			// Determine if the line of data will need to be packed.					
			
	callPackLineOfDataFlag = FALSE;
	differentBuffersFlag = FALSE;
	fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
	windowInfoPtr = fileIOInstructionsPtr->windowInfoPtr;
	BILSpecialFlag = fileIOInstructionsPtr->oneReadFlag;
	
	if (fileIOInstructionsPtr->forceByteCode == kForce2Bytes)
		{
		if (windowInfoPtr->localMaxNumberBytes == 1 || 
															windowInfoPtr->localBytesDifferFlag)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}		// end "if (windowInfoPtr->localMaxNumberBytes == 1 || ...)" 
			
		}		// end "if (fileIOInstructionsPtr->forceByteCode == kForce2Bytes)" 
	
	else if (fileIOInstructionsPtr->forceByteCode == kForce4Bytes)
		{
		callPackLineOfDataFlag = TRUE;
		differentBuffersFlag = TRUE;
			
		}		// end "else if (fileIOInstructionsPtr->forceByteCode == kForce4Bytes)" 
																
	if (fileInfoPtr->bandInterleave != kBIS)
		{
		if (fileIOInstructionsPtr->packDataFlag)
			{
			if (fileIOInstructionsPtr->columnInterval > 1)
				callPackLineOfDataFlag = TRUE;
			
			else if (BILSpecialFlag && fileIOInstructionsPtr->numberChannels < 
												fileIOInstructionsPtr->bilSpecialNumberChannels)
					callPackLineOfDataFlag = TRUE;
				
			}		// end "if (fileIOInstructionsPtr->packDataFlag)"
			
		if (fileIOInstructionsPtr->forceBISFormatFlag)
			{
			callPackLineOfDataFlag = TRUE;
			differentBuffersFlag = TRUE;
			
			}		// end "if (fileIOInstructionsPtr->toBISFormatFlag)" 
			
		}		// end "if (fileInfoPtr->bandInterleave != kBIS)" 				
	
	else		// fileInfoPtr->bandInterleave == kBIS 
		{
		if (fileIOInstructionsPtr->packDataFlag && 
				(fileIOInstructionsPtr->numberChannels<fileInfoPtr->numberChannels || 
															fileIOInstructionsPtr->columnInterval>1) )
			callPackLineOfDataFlag = TRUE;
			
		}		// end "else fileInfoPtr->bandInterleave == kBIS" 
		
	fileIOInstructionsPtr->callPackLineOfDataFlag = callPackLineOfDataFlag;
	fileIOInstructionsPtr->differentBuffersFlag = differentBuffersFlag;
	
	if (areaDescriptionPtr != NULL && areaDescriptionPtr->maskInfoPtr != NULL)
		{
		fileIOInstructionsPtr->maskBufferPtr = 
					(HUInt16Ptr)GetHandlePointer(
								areaDescriptionPtr->maskInfoPtr->maskHandle,
								kLock,
								kNoMoveHi);
								
		fileIOInstructionsPtr->maskBufferPtr += 
					(areaDescriptionPtr->maskLineStart - 1) * 
										(areaDescriptionPtr->maskInfoPtr->numberColumns + 1);
											
		fileIOInstructionsPtr->maskColumnStart = areaDescriptionPtr->maskColumnStart;
											
		fileIOInstructionsPtr->maskValueRequest = areaDescriptionPtr->maskValueRequest;
		
				// Get the number of columns in the mask buffer to skip to go from
				// one line to the next. Allow for a line interval other than 1.
				
		fileIOInstructionsPtr->numberMaskColumnsPerLine = 
										areaDescriptionPtr->maskInfoPtr->numberColumns + 1;
				
		fileIOInstructionsPtr->numberMaskColumnsPerLine *= 
																areaDescriptionPtr->lineInterval;
		
		}		// end "if (areaDescriptionPtr != NULL && ..."
		
			// Set up heirarchal tile parameters, if needed.
		
	SetUpHeirarchalFileIOParameters ( fileIOInstructionsPtr->layerInfoPtr,
													fileIOInstructionsPtr->fileInfoPtr,
													(UInt16)fileIOInstructionsPtr->numberChannels,
													(UInt16*)fileIOInstructionsPtr->channelListPtr,
													columnStart,
													columnEnd);
		
			// Set flag indicating whether Threaded IO can be used.
			
	gUseThreadedIOFlag = gHasThreadManager && fileIOInstructionsPtr->oneReadFlag;

	errCode = SetupFileIOThread (fileIOInstructionsPtr);
	
	return ( (SInt16)errCode );
	
}		// end "SetUpFileIOInstructions"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetUpGeneralFileIOInstructions
//
//	Software purpose:	This routine sets up the parameters in the file IO
//							instruction structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 10/25/1999

void SetUpGeneralFileIOInstructions (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				WindowInfoPtr						windowInfoPtr,
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberListChannels,
				HUInt16Ptr							channelListPtr,
				HUCharPtr							inputBufferPtr,
				HUCharPtr							outputBufferPtr,
				HUCharPtr							tiledBufferPtr,
				UInt32								bufferOffset,
				Boolean								packDataFlag,
				Boolean								forceBISFormatFlag,	
				UInt16								forceByteCode,
				FileIOInstructionsPtr*			outputFileIOInstructionsPtrPtr)
							
{
	fileIOInstructionsPtr->windowInfoPtr = windowInfoPtr;
	fileIOInstructionsPtr->fileInfoPtr = fileInfoPtr;
	fileIOInstructionsPtr->layerInfoPtr = layerInfoPtr;
	
	fileIOInstructionsPtr->channelListPtr = channelListPtr;
	fileIOInstructionsPtr->numberChannels = numberListChannels;
	fileIOInstructionsPtr->channelStart = 0;
	fileIOInstructionsPtr->channelEnd = 0;
	
	fileIOInstructionsPtr->inputBufferPtrs[0] = inputBufferPtr;
	fileIOInstructionsPtr->outputBufferPtrs[0] = outputBufferPtr;
	
	if (inputBufferPtr != NULL)
		fileIOInstructionsPtr->inputBufferPtrs[1] = &inputBufferPtr[bufferOffset];
	
	if (outputBufferPtr != NULL)
		fileIOInstructionsPtr->outputBufferPtrs[1] = &outputBufferPtr[bufferOffset];
	
	fileIOInstructionsPtr->tiledBufferPtrs[0] = tiledBufferPtr;
	
	fileIOInstructionsPtr->bufferOffset = bufferOffset;
	
	fileIOInstructionsPtr->forceByteCode = forceByteCode;
	fileIOInstructionsPtr->forceBISFormatFlag = forceBISFormatFlag;
	fileIOInstructionsPtr->packDataFlag = packDataFlag;
	
	fileIOInstructionsPtr->callPackLineOfDataFlag = FALSE;
	fileIOInstructionsPtr->differentBuffersFlag = FALSE;
	
	if (tiledBufferPtr != NULL)
		GetSetTiledIOBufferBytes (layerInfoPtr,
											fileInfoPtr,  
											(UInt16*)channelListPtr,
											(UInt16)numberListChannels,
											tiledBufferPtr);	

	GetHierarchalFilePointers (windowInfoPtr, fileInfoPtr);
	
	*outputFileIOInstructionsPtrPtr = fileIOInstructionsPtr;
	
}		// end "SetUpGeneralFileIOInstructions"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpHeirarchalFileIOParameters
//
//	Software purpose:	The purpose of this routine is to set the parameter in the 
//							heirarchal file io structure that indicates which block are to
//							be read from the disk file that will include the input area.
//
//	Parameters in:				
//
//	Parameters out:			
//
//	Value Returned:
//
// Called By:			SetUpFileIOInstructions in SFileIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1999
//	Revised By:			Larry L. Biehl			Date: 01/31/2003

void SetUpHeirarchalFileIOParameters (
				LayerInfoPtr						layerInfoPtr,
				FileInfoPtr							fileInfoPtr,
				UInt16								numberChannels,
				UInt16*								channelListPtr,
				UInt32								columnStart, 
				UInt32								columnEnd) 

{
	ldiv_t 								ratio;
	
	FileInfoPtr							localFileInfoPtr;
	
	HierarchalFileFormatPtr			hfaPtr,
											localHfaPtr;
	
	SInt32								fileInfoIndex,
											nextFileInfoIndex;
	
	UInt32								blockColumnEnd,
											blockColumnStart,
											channelNumber,
											fileChannelNumber,
											index;
											
	Boolean								bandInterleaveByBlockFlag;		// Currently not used.
											
	
	if (fileInfoPtr != NULL)
		{
		fileChannelNumber = 0;
		fileInfoIndex = -1;
		
		for (index=1; index<=numberChannels; index++)
			{
			channelNumber = index;								
			if (channelListPtr != NULL)
				channelNumber = channelListPtr[index-1] + 1;
			
			nextFileInfoIndex = 0;
			fileChannelNumber++;
			if (layerInfoPtr != NULL)
				{
				nextFileInfoIndex = layerInfoPtr[channelNumber].fileInfoIndex;
				fileChannelNumber = layerInfoPtr[channelNumber].fileChannelNumber;
				
				}		// end "if (layerInfoPtr != NULL)"
				
			if (fileInfoIndex != nextFileInfoIndex)
				{
				fileInfoIndex = nextFileInfoIndex;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
					
						// Get pointer to the hfa structure.
						
				hfaPtr = (HierarchalFileFormatPtr)GetHandlePointer(
														localFileInfoPtr->hfaHandle,
														kNoLock,
														kNoMoveHi);
														
				bandInterleaveByBlockFlag = FALSE;
				
				}		// end "if (if (fileInfoIndex != layerInfoPtr...)"
			
			if (hfaPtr != NULL)
				{
				if (localFileInfoPtr->bandInterleave == kBIBlock ||
													localFileInfoPtr->bandInterleave == kBIS)
					{
					fileChannelNumber = 1;
					bandInterleaveByBlockFlag = TRUE;
					
					}		// end "if (localFileInfoPtr->bandInterleave == kBIBlock || ..."
			
				localHfaPtr = &hfaPtr[fileChannelNumber-1];
				
				if (localHfaPtr->blockWidth > 0)
					{
					blockColumnStart = (columnStart-1)/localHfaPtr->blockWidth;
				
					localHfaPtr->firstLineRead = 0;
					localHfaPtr->lastLineRead = 0;	
					localHfaPtr->firstColumnStartByte =  localHfaPtr->layerOffsetBytes + 
																localHfaPtr->blockOffset * blockColumnStart;
					
					ratio = ldiv (columnEnd, localHfaPtr->blockWidth);
					blockColumnEnd = ratio.quot;
					if (ratio.rem > 0)
						blockColumnEnd++;
												
					localHfaPtr->firstColumnRead = localHfaPtr->blockWidth * blockColumnStart + 1;
					localHfaPtr->lastColumnRead = localHfaPtr->blockWidth * blockColumnEnd;
					localHfaPtr->numberBlocksRead = blockColumnEnd - blockColumnStart;
					
					}		// end "if (localHfaPtr->blockWidth > 0)"
				
				}		// end "if (hfaPtr != NULL)"
				
			}		// end "for (index=0; index<numberChannels; index++)"
			
		}		// end "if (fileInfoPtr != NULL)"
	
}		// end "SetUpHeirarchalFileIOParameters"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								Ä Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetVolume
//
//	Software purpose:	This routine sets the default volume.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			GetProjectFileName in project.c
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 12/19/1997

SInt16 SetVolume (
				CMFileStream* 						fileStreamPtr,
				SInt16								messageCode)

{
	SInt16								errCode = noErr;
	
	
	if (fileStreamPtr != NULL)
		{	
		#if !defined _X86_
	
			if (fileStreamPtr->fSSpecFlag)
				errCode = HSetVol (NULL, 
										fileStreamPtr->vRefNum, 
										fileStreamPtr->parID);  
			
			#if !TARGET_API_MAC_CARBON	
				else		// !fileStreamPtr->fSSpecFlag
					errCode = SetVol (NIL, fileStreamPtr->vRefNum);
			#endif	// !TARGET_API_MAC_CARBON	
		
			if (messageCode == kErrorMessages)		
				IOCheck (errCode, (CharPtr)NULL);            
		
		#endif	// !defined _X86_
		
		
		#if defined _X86_
		
		#endif	// defined _X86_
		
		}		// end "if (fileStreamPtr != NULL)"
	
	return (errCode);
	
}		// end "SetVolume"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetVolumeReference
//
//	Software purpose:	The purpose of this routine is to set the output file 
//							volume reference values to those in the input file
//							stream.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1997
//	Revised By:			Larry L. Biehl			Date: 10/09/2001	

void SetVolumeReference (
				CMFileStream*						inputFileStreamPtr,
				CMFileStream*						outputFileStreamPtr)

{  
	if (inputFileStreamPtr != NULL && outputFileStreamPtr != NULL)
		{              
		#if !defined _X86_                       
	      
		      		// Copying both will handle for both cases of a FSSpec record
		      		// and a non-FSSpec record.
		      		
		   	outputFileStreamPtr->vRefNum = inputFileStreamPtr->vRefNum;
		   	outputFileStreamPtr->parID = inputFileStreamPtr->parID;
		   	outputFileStreamPtr->fSSpecFlag = inputFileStreamPtr->fSSpecFlag;
	   	
		#endif	// !defined _X86_ 
	
	              
		#if defined _X86_  
	 
		#endif	// defined _X86_
		
		}		// end "if (inputFileStreamPtr != NULL && outputFileStreamPtr != NULL)" 
	
}		// end "SetVolumeReference"    



//------------------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SwapBytes
//
//	Software purpose:	The purpose of this routine is to swap the bytes
//							in the input buffer.
//
//	Parameters in:		Pointer to buffer of two-byte data
//							Number of samples in the buffer
//
//	Parameters out:	None
//
//	Value Returned:  
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/20/1994
//	Revised By:			Larry L. Biehl			Date: 04/28/1995

void SwapBytes (
				HUInt16Ptr							twoBytePtr,
				UInt32								numberSamples)
								
{	
	UInt32								sampleCount;
	
		
	for ( sampleCount=0; sampleCount<numberSamples; sampleCount++ )
		{
		*twoBytePtr = ((*twoBytePtr & 0xff00) >> 8) | ((*twoBytePtr & 0x00ff) << 8);
		twoBytePtr++;
				
		}		// end "for ( sampleCount=0;  ..." 
	
}		// end "SwapBytes" 
