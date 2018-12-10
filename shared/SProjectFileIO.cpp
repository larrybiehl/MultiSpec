//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SProjecFileIO.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/12/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines that open, close, save and read
//								project files.
//
//	Functions in file:	void						ComputeSumSquaresMatrix
//								void						ComputeSumVector
//								UInt32				 	GetBufferForProjectReadWrites
//								void 						GetCopyOfPFileNameFromProjectInfo
//								UInt16				 	GetLengthOfMaxSum
//								UInt16				 	GetLengthOfMaxSumOfSquares
//								Boolean 					GetProjectFileName
//								SInt32	 				GetSizeOfProjectFile
//								Handle 					GetSpecifiedImageFile
//								SInt16 	 				ReadChannelInformation
//								SInt16 	 				ReadCovarianceInformation
//								SInt16 					ReadMaskInformation
//								SInt16 	 				ReadModifiedStats
//								SInt16 	 				ReadProjectFile
//								SInt16 	 				ReadStatistics
//								char*		 				ReadStringFromFile
//								SInt16 	 				SaveProjectFile
//								Boolean 					SetupClassFieldPointMemory
//								HPtr 						SkipNTabs
//								void						UpdateGraphicStatusBox
//								void 						VerifyClassAndFieldParameters
//								Boolean 					WriteChannelInformation
//								Boolean 					WriteCovarianceInformation
//								SInt16 					WriteStatistics
//								SInt16 	 				WriteProjectFile
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
/*
Template for debugging
   int numberChars = sprintf ((char*)gTextString3,
                              " SProjFIO: (): %s",
                              gEndOfLine);
   ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_win
	#include "CImageWindow.h"
	#include "WMultiSpec.h"
#endif	// defined multispec_win    

#if defined multispec_lin
#include "CImageWindow.h"
#include "CFileStream.h"
#endif

//#include "SExtGlob.h"	

#include "errno.h"

#define	kMacNewLinePosMode					0x0d81
#define	kIBMNewLinePosMode					0x0a81

#define	kArchitectureNotKnown				0
#define	kMacintoshMotorola					1
#define	kMacintoshIntel						2
#define	kWindowsIntel							3

extern void 		WriteProjectName (void);	



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													

void 						ComputeSumSquaresMatrix (
								UInt32								numberChannels, 
								HChannelStatisticsPtr			channelStatsPtr, 
								HCovarianceStatisticsPtr		covariancePtr, 
								SInt64								numberPixels);

void 						ComputeSumVector (
								HChannelStatisticsPtr			channelStatsPtr, 
								UInt32								numberChannels, 
								SInt64								numberPixels);

void 						FinishUpStatisticsRead (
								ParmBlkPtr							paramBlockPtr,
								UCharPtr*							inputStringPtrPtr,
								SInt32								savedIoPosMode, 
								SInt32								savedIoReqCount);

UInt32				 	GetBufferForProjectReadWrites (
								WindowInfoPtr						projectWindowInfoPtr,
								UInt32								currentBufferBytes,
								UCharPtr*							inputStringPtrPtr);

UInt16				 	GetLengthOfMaxSum (
								WindowInfoPtr						projectWindowInfoPtr);

UInt16				  	GetLengthOfMaxSumOfSquares (
								WindowInfoPtr						projectWindowInfoPtr);

Boolean 					GetProjectFileName (
								SInt16								saveCode);

SInt16	 				ReadChannelInformation (
								HChannelStatisticsPtr			chanStatsPtr, 
								UCharPtr* 							inputStringPtrPtr, 
								SInt16								numberStatisticsChannels, 
								UInt32								type);
																			
SInt16	 				ReadCovarianceInformation (
								HSumSquaresStatisticsPtr		sumSquaresStatsPtr, 
								ParmBlkPtr 							paramBlockPtr,
								UCharPtr*							inputStringPtrPtr,
								UInt32								numberStatisticsChannels, 
								UCharPtr								fieldClassStringPtr,
								UInt32								binaryListingCode,
								UInt32								numberEndOfLineBytes,
								double*								boxRightPtr,
								double								boxIncrement);

SInt16 					ReadMaskInformation (
								Str255*								maskFileNamePtr,
								MaskInfoPtr 						maskInfoPtr,
								UCharPtr*							inputStringPtrPtr);
								
SInt16	 				ReadModifiedStats (
								ParmBlkPtr 							paramBlockPtr,
								UCharPtr*							inputStringPtrPtr,
								SInt16	 							numberChannels,
								HChannelStatisticsPtr 			modifiedClassChanStatsPtr,
								HCovarianceStatisticsPtr 		modifiedClassCovStatsPtr,
								UInt32								numberOfEndOfLineCharacters,
								SInt16								formatArchitectureCode);
																	
SInt16	 				ReadProjectFile (void);

SInt16	 				ReadStatistics (
								HPClassNamesPtr 					classNamesPtr, 
								SInt16	 							storageIndex, 
								ParmBlkPtr 							paramBlockPtr,
								UCharPtr*							inputStringPtrPtr, 
								SInt16	 							classFieldCode,
								UInt32								binaryListingCode, 
								double* 								boxRightPtr, 
								double 								boxIncrement,
								Boolean*								statsLoadedFlagPtr,
								Boolean*								sumsSquaresFlagPtr,
								Boolean*								meansCovFlagPtr,
								UInt32								numberEndOfLineBytes,
								SInt16								formatArchitectureCode);

UCharPtr	 				ReadStringFromFile (
								UCharPtr								inputStringPtr, 
								UCharPtr								outputStringPtr, 
								SInt16								numberSkipTabs,
								SInt16								maxStringLength);

SInt16					SetupForStatisticsRead (
								ParmBlkPtr							paramBlockPtr,
								UCharPtr								inputStringPtr,
								UInt32								numberEndOfLineBytes,
								UInt32								numberChannels,
								SInt16								formatArchitectureCode,
								SInt32*								savedIoPosModePtr, 
								SInt32*								savedIoReqCountPtr,
								Boolean*								swapBytesFlagPtr);
								
UCharPtr					SkipNTabs (
								UCharPtr								inputStringPtr, 
								SInt16								numberTabs);

Boolean 					SetupClassFieldPointMemory (void);

void						UpdateGraphicStatusBox (
								double*								rightBoxPtr,
								double								boxIncrement);
								
void 						VerifyClassAndFieldParameters (
								Boolean								differentFileSourceFlag);

Boolean 					WriteChannelInformation (
								HChannelStatisticsPtr			chanStatsPtr, 
								CMFileStream* 						fileStreamPtr,
								SInt16								numberStatisticsChannels, 
								UInt32								type, 
								UCharPtr								stringPtr);
										
Boolean 					WriteCovarianceInformation (
								HCovarianceStatisticsPtr		matrixStatsPtr, 
								HChannelStatisticsPtr			channelStatsPtr,
								SInt64								numberPixelsUsedForStats, 
								CMFileStream* 						fileStreamPtr,
								SInt16								numberStatisticsChannels, 
								UInt32								type, 
								UCharPtr								stringPtr, 
								double*								boxRightPtr, 
								double								boxIncrement);
									
Boolean 					WriteModifiedStats (
								CMFileStream* 						fileStreamPtr,
								SInt16								numberChannels,
								HChannelStatisticsPtr 			modifiedClassChanStatsPtr,
								HCovarianceStatisticsPtr 		modifiedClassCovStatsPtr);

SInt16					WriteProjectFile (
								SInt16								saveCode);
									
SInt16	 				WriteStatistics (
								HChannelStatisticsPtr			channelStatsPtr, 
								HSumSquaresStatisticsPtr		sumSquaresStatsPtr,
								SInt64								numberPixelsUsedForStats,
								CMFileStream* 						fileStreamPtr,
								Boolean								saveMeansCovFlag, 
								Boolean								saveSumsSquaresFlag,
								SInt16								classFieldCode,
								UInt32								binaryListingCode, 
								double*								boxRightPtr, 
								double								boxIncrement);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CloseFile
//
//	Software purpose:	The purpose of this routine is to close the file
//							stream for the input project information structure.
//		
//	Parameters in:		Address to project information structure for file to be closed.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 10/25/1995

void CloseFile (
				ProjectInfoPtr						projectInfoPtr)

{
				// Check input variables.														
				
	if (projectInfoPtr != NULL)
		{
		CMFileStream* fileStreamPtr = GetFileStreamPointer (projectInfoPtr);
			
		CloseFile (fileStreamPtr);
		
		}	// end "if (projectInfoPtr != NULL)"
	
}	// end "CloseFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSumSquaresMatrix
//
//	Software purpose:	The purpose of this routine is to convert a lower
//							triangular covariance matrix read in from a project
//							file to a lower triangular sums of squares matrix.
//
//	Parameters in:		numberChannels.
//							pointer to the channel means and sums.
//							pointer to the covariance matrix.  At end of
//											routine this becomes the pointer to the sums of
//											squares matrix.
//							total number of pixels.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/24/1992
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

void ComputeSumSquaresMatrix (
				UInt32								numberChannels, 
				HChannelStatisticsPtr			channelStatsPtr, 
				HCovarianceStatisticsPtr		covariancePtr, 
				SInt64								numberPixels)

{
	double								channelMean;
	
	HChannelStatisticsPtr			lChannelStatsPtr1,
											lChannelStatsPtr2;
	
	UInt32								channel,
											covChan,
											covStart;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	if (numberChannels > 0 && channelStatsPtr && covariancePtr && numberPixels > 0)
		{
				// Compute the sum of squares matrix.										
				
		if (numberPixels > 1)
			numberPixels--;
		lChannelStatsPtr1 = channelStatsPtr;
		
		for (channel=0; channel<numberChannels; channel++)
			{
			channelMean = lChannelStatsPtr1->mean;
			
			if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
				{
				lChannelStatsPtr2 = lChannelStatsPtr1;
				covStart = channel;
				
				}	// end "if (...->statisticsCode == kMeanStdDevOnly)" 
				
			else	// gProjectInfoPtr->statisticsCode != kMeanStdDevOnly 
				{
				lChannelStatsPtr2 = channelStatsPtr;
				covStart = 0;
				
				}	// end "else ...->statisticsCode != kMeanStdDevOnly" 
			
			for (covChan=covStart; covChan<=channel; covChan++)
				{	
				*covariancePtr = numberPixels * *covariancePtr +
															(channelMean * lChannelStatsPtr2->sum);
			 							
			 	covariancePtr++;
			 	
			 	lChannelStatsPtr2++;
			
				}	// end "for (covChan=0; covChan<=..." 
			
			lChannelStatsPtr1++;
				
			}	// end "for (channel=0; channel<numberChannels; ..." 
			
		}	// end "continueFlag" 
					
}	// end "ComputeSumSquaresMatrix" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ComputeSumVector
//
//	Software purpose:	The purpose of this routine is to compute the
//							sum vector for the input means and number of pixels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/25/1992
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

void ComputeSumVector (
				HChannelStatisticsPtr			channelStatsPtr, 
				UInt32								numberChannels, 
				SInt64								numberPixels)

{
	UInt32								channel;
	Boolean								continueFlag;
	
	
			// Check input values.  Continue only if input parameters are within	
			// proper rangers																		
			
	continueFlag = TRUE;
	if (channelStatsPtr == NULL)
		continueFlag = FALSE;
		
	if (numberPixels <= 0)
		continueFlag = FALSE;
																						
	if (continueFlag)
		{
		for (channel=0; channel<numberChannels; channel++)
			{
			channelStatsPtr->sum = channelStatsPtr->mean * numberPixels;
			channelStatsPtr++;
			
			}	// end "for (channel=0; channel<numberChannels; channel++)" 
			
		}	// end "continueFlag" 
					
}	// end "ComputeSumVector" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetBufferForProjectReadWrites
//
//	Software purpose:	The purpose of this routine is to obtain a buffer to
//							read the maximum possible matrix line into. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1992
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

UInt32 GetBufferForProjectReadWrites (
				WindowInfoPtr						projectWindowInfoPtr,
				UInt32								currentBufferBytes,
				UCharPtr*							inputStringPtrPtr)

{
	UCharPtr								newBufferPointer;
	
	UInt32								bytesNeeded,
											usedBufferBytes;
	
															
			// Now get a buffer to read the rest of the project lines into	that will
			// be long enough for any matrix data.
	
	bytesNeeded = GetLengthOfMaxSumOfSquares (projectWindowInfoPtr) + 1;
	bytesNeeded = MAX (23, bytesNeeded);
	
	bytesNeeded *= gProjectInfoPtr->numberStatisticsChannels;
	bytesNeeded += 10;
	bytesNeeded = MAX (1000, bytesNeeded);
	
	if (bytesNeeded > currentBufferBytes)
		{
		newBufferPointer = (UCharPtr)MNewPointer (bytesNeeded);
		
		if (newBufferPointer != NULL)
			{
					// Move the information in the current buffer to 
					// the new buffer
				
			BlockMoveData (gCharBufferPtr1, newBufferPointer, (size_t)currentBufferBytes);
			
			newBufferPointer[currentBufferBytes] = kNullTerminator;
			                                   
			usedBufferBytes = (UInt32)(*inputStringPtrPtr - (UCharPtr)gCharBufferPtr1);
			*inputStringPtrPtr = newBufferPointer + usedBufferBytes;
														
			}	// end "if (newBufferPointer != NULL)"
			
		else	// newBufferPointer == NULL
			*inputStringPtrPtr = NULL;
		
		CheckAndDisposePtr (gCharBufferPtr1); 
	
		gCharBufferPtr1 = (char*)newBufferPointer;
		
		}	// end "if (bytesNeeded > currentBufferBytes)"
	
	return (bytesNeeded);
	
}	// end "GetBufferForProjectReadWrites" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCopyOfPFileNameFromProjectInfo
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
//	Coded By:			Larry L. Biehl			Date: 09/01/2017
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void GetCopyOfPFileNameFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr,
				FileStringPtr						outputPtr)

{  
	GetCopyOfPFileNameFromProjectInfo (
											projectInfoPtr, outputPtr, kDefaultFileStringCode);
	
}	// end "GetCopyOfPFileNameFromProjectInfo"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetCopyOfPFileNameFromProjectInfo
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
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void GetCopyOfPFileNameFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr,
				FileStringPtr						outputPtr,
				SInt16								returnCode)

{  
	UCharPtr								charOutputPtr;
	
	
	if (outputPtr != NULL)
		{
		if (projectInfoPtr != NULL)
			{              
			CMFileStream* fileStreamPtr = GetFileStreamPointer (projectInfoPtr);
			GetCopyOfPFileNameFromFileStream (fileStreamPtr, outputPtr, returnCode);
			
			}	// end "if (projectInfoPtr != NULL)"
		
		else	// projectInfoPtr == NULL
			outputPtr[0] = 0;
					
		if (outputPtr[0] == 0)
			{
			if (returnCode == kReturnASCII)
				{
				charOutputPtr = (UCharPtr)outputPtr;
				ConcatPStrings (charOutputPtr, (UCharPtr)"\0Untitled Project\0", 255);
				
				}	// end "if (returnCode == kReturnASCII)"
				
			else	// returnCode != kReturnASCII
				{
				wchar_t*	unicodeOutputPtr = (wchar_t*)outputPtr;
				swprintf ((wchar_t*)&unicodeOutputPtr[1],
								254,
								(wchar_t*)_T("%ls"),
								(wchar_t*)_T("Untitled Project"));
				unicodeOutputPtr[0] = (wchar_t)wcslen (&unicodeOutputPtr[1]);

				}	// end "else returnCode != kReturnASCII"
			
			}	// end "if (outputPtr[0] == 0)"
			
		}	// end "if (outputPtr != NULL)" 
	
}	// end "GetCopyOfPFileNameFromProjectInfo"


                  
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 GetLengthOfMaxSum
//
//	Software purpose:	The purpose of this routine is to determine the length
//							of the string of the maximum possible sum for the 
//							project. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	string length		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1992
//	Revised By:			Larry L. Biehl			Date: 07/19/2006	

UInt16 GetLengthOfMaxSum (
				WindowInfoPtr						projectWindowInfoPtr)

{
	double								maxValue;
	
	UInt16								stringLength;
	
				
	if (!projectWindowInfoPtr)
																							return (10);
																																							
			// Now determine the size of a buffer needed to read the rest of the
			// project lines into that will be long enough for any matrix data.
	
	maxValue = 256;
	if (projectWindowInfoPtr->numberBytes == 2)
		maxValue = projectWindowInfoPtr->numberBins;
	
	else if (projectWindowInfoPtr->numberBytes > 2)
				// This is an estimate for now.  This needs to be refined.
		maxValue = 1000000;
		
	maxValue *= projectWindowInfoPtr->maxNumberLines *
															projectWindowInfoPtr->maxNumberColumns;
	
	sprintf ((char*)gTextString, "%.0f", maxValue);
	stringLength = (UInt16)strlen ((char*)gTextString);
	
	return (stringLength);
	
}	// end "GetLengthOfMaxSum" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt16 GetLengthOfMaxSumOfSquares
//
//	Software purpose:	The purpose of this routine is to determine the length
//							of the string of the maximum possible sum of
//							square values for the project. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	string length		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/26/1992
//	Revised By:			Larry L. Biehl			Date: 07/19/2006	

UInt16 GetLengthOfMaxSumOfSquares (
				WindowInfoPtr						projectWindowInfoPtr)

{
	double								maxValue;
	
	UInt16								stringLength;
	
				
	if (projectWindowInfoPtr == NULL)
																							return (10);
																																							
			// Now get a buffer to read the rest of the project lines into			
			// that will be long enough for any matrix data.							
	
	maxValue = 256;
	if (projectWindowInfoPtr->numberBytes == 2)
		maxValue = projectWindowInfoPtr->numberBins;
	
	else if (projectWindowInfoPtr->numberBytes > 2)
				// This is an estimate for now.  This needs to be refined.
		maxValue = 1000000;
		
	maxValue *= maxValue;
	maxValue *= projectWindowInfoPtr->maxNumberLines *
														projectWindowInfoPtr->maxNumberColumns;
	
	sprintf ((char*)gTextString, "%.0f", maxValue);
	stringLength = (UInt16)strlen ((char*)gTextString);
	
	return (stringLength);
	
}	// end "GetLengthOfMaxSumOfSquares" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetProjectFile
//
//	Software purpose:	This routine allows the user to select the Project
//							file to read the project information from.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
//
// Called By:			OpenProjectFile in project.c
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1988
//	Revised By:			Larry L. Biehl			Date: 09/01/2017	

Boolean GetProjectFile (
				LocalAppFile*						localAppFilePtr)

{
	CMFileStream*						projectFileStreamPtr;
	
	SInt16								errCode,
											numberFileTypes;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	
			// Continure as long as gProjectInfoPtr is not NULL							
	
	if (gProjectInfoPtr != NULL)
		{
		projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
		
				// Get the project file name to read from.	First make sure that 
				// we have a pointer cursor.													

		MInitCursor ();
		
		numberFileTypes = 2;
		#if defined multispec_mac    
			if (gEventRecord.modifiers & optionKey)
				numberFileTypes = -1;
		#endif	// defined multispec_mac 
		
		SetType (projectFileStreamPtr, kTEXTFileType);
		SetReferenceNumber (projectFileStreamPtr, 0);
		
		do
			{
			errCode = GetFile (projectFileStreamPtr, 
										numberFileTypes, 
										gListTypes, 
										localAppFilePtr, 
										NULL,
										NULL, 
										IDS_SelectProject); // kFileIOStr8);
			continueFlag = (errCode == noErr) & FileExists (projectFileStreamPtr);
			
					// Make certain that appFilePtr is now NULL so that if there	
					// was an error the user can select another file if he/she		
					// wants.																		
					
			localAppFilePtr = NULL;
		
					// If file is set up to read project information, read 			
					// format and version strings from the selected file.				
				
			if (continueFlag)
				{
				gProjectInfoPtr->version = 
									CheckIfProjectFile (projectFileStreamPtr, kLoadHeader);
										
				if (gProjectInfoPtr->version < 0)
					{
							// if version == -1, not a MultiSpec project file
							// if version == -2, is an old version
							
					char* projectFileNameCPtr = 
							(char*)GetFileNameCPointerFromFileStream (projectFileStreamPtr);
		
							// Alert user if file is not a project file or the 
							// format version is wrong.									
						
					UInt16 stringID = IDS_Alert33;
									
					if (gProjectInfoPtr->version == -2)
						stringID = IDS_Alert34;
									
					if (LoadSpecifiedStringNumberStringP (kAlertStrID,
																		stringID, 
																		(char*)gTextString,
																		(char*)gTextString2,
																		TRUE,
																		projectFileNameCPtr))
						errCode = DisplayAlert (
												kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
					
					}	// end "if (gProjectInfoPtr->version < 0)"
					
				if (gProjectInfoPtr->version <= 0)
					CloseFile (gProjectInfoPtr);
					
				if (gProjectInfoPtr->version == 0)
					continueFlag = FALSE;
				
				}	// end "if (continueFlag)" 
			
			}	while (continueFlag && gProjectInfoPtr->version <= 0);
			
		}	// end "if (gProjectInfoPtr != NULL)" 

	else		
		continueFlag = FALSE;
  	
	return (continueFlag);
	
}	// end "GetProjectFile" 


                  
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetProjectFileName
//
//	Software purpose:	The purpose of this routine is to get the name of
//							the project file and a path to it.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			SaveProjectFile in project.c
//
//	Coded By:			Larry L. Biehl			Date: 09/26/1988
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean GetProjectFileName (
				SInt16								saveCode)

{
	SInt64								fileSize,
											freeBytes;
	
	CMFileStream*						projectFileStreamPtr;
									
	FileStringPtr						projectFileNamePtr;
	
	UInt32								bytesSpaceNeeded;
	
	SInt16								errCode,
											savedNameLength;
	
	Boolean								returnFlag,
											savedFieldSumsSquaresFlag,
											savedFieldMeansCovFlag,
											savedClassSumsSquaresFlag,
											savedClassMeansCovFlag;
											

	projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
	if (projectFileStreamPtr == NULL)
																							return (FALSE);
	
			// If no name has been given to the project then force a "Save As"	
			// so that the user can name the project file and a path to the		
			// file can be opened.																

	projectFileNamePtr =
				(FileStringPtr)GetFileNamePPointerFromFileStream (projectFileStreamPtr);
		
	if (!FileExists (projectFileStreamPtr))
		saveCode = 1;
				
			// Get estimate of the size of the project file when written to disk.
	
	bytesSpaceNeeded = (UInt32)GetSizeOfProjectFile ();
	
			// If the saveCode flag is 0, then we already have the file name.		
			// Now check if there is enough space on the current to rewrite the	
			// file.  If not then do a "Save As" to allow the user to write to	
			// another volume.																	
			
	if (saveCode == 0)
		{
				// Get space left on the volume.												
		
		char* projectASCIIFileNamePtr =
							(char*)GetFileNamePPointerFromFileStream (projectFileStreamPtr);
		errCode = GetVolumeFreeSpace (
									(Str255*)projectASCIIFileNamePtr,
									GetVolumeReferenceNumber (projectFileStreamPtr),
									&freeBytes);
		/*
		// The following needs more work.
		#if defined multispec_lin	
			wxLongLong wxFreeBytes;
			char* projectFilePathPtr = (char*)projectFileStreamPtr->GetFilePathPtr ();
			bool returnFlag = wxGetDiskSpace (wxString::FromAscii (projectFilePathPtr),
															NULL,
															&wxFreeBytes);
			freeBytes = wxFreeBytes.GetValue ();
			//if (!returnFlag)
			errCode = noErr;
		#endif
		*/
		if (errCode != noErr)														
																								return (0);
		
				// Get the number of bytes in the old file if one is being			
				// replaced.																		
			
		errCode = GetSizeOfFile (projectFileStreamPtr, &fileSize);
		if (errCode != noErr && errCode != fnfErr)														
																								return (0);
		
		if (bytesSpaceNeeded > (freeBytes + fileSize)) 
			saveCode = 1;
			
				// If the current project file is being replaced, make certain		
				// that the file is empty.														
				
		errCode = SetFileSizeToZero (projectFileStreamPtr);
			
		if (errCode != noErr)
			saveCode = 1;
		
		}	// end "if (saveCode == 0)" 
		
			// Get new file name or new volume to write file to.						
	
	returnFlag = TRUE;
	if (saveCode == 1)
		{
				// Save the statistics type save options in case they need to be reset
				// if the user cancels the 'Get File' dialog box. I put this here rather
				// than handling it in the Get File call backs because I can do it here
				// for both Mac and Windows versions. Otherwise I have two different sets
				// of software to set it up in.
		
		savedFieldSumsSquaresFlag = gProjectInfoPtr->saveFieldSumsSquaresFlag;
		savedFieldMeansCovFlag = gProjectInfoPtr->saveFieldMeansCovFlag;
		savedClassSumsSquaresFlag = gProjectInfoPtr->saveClassSumsSquaresFlag;
		savedClassMeansCovFlag = gProjectInfoPtr->saveClassMeansCovFlag;
		
				// Save copy of filename in case it has to be loaded back in 		
				// because the user cancelled.												
		
		savedNameLength = projectFileNamePtr[0];
		//savedNameLength = strlen ((char*)projectFileNameCPtr);
		
		if (projectFileNamePtr[0] == 0)
		//if (savedNameLength == 0)
			{
					// If this is the first time to save the file, then make the 	
					// default volume the same as that for the project image file.	
				
			if (gProjectInfoPtr->windowInfoHandle != NULL)
				{
				SignedByte handleStatus;	
				CMFileStream* imageFileStreamPtr = 
								GetFileStreamPointer (gProjectInfoPtr->windowInfoHandle,
																&handleStatus);
					
				errCode = SetVolume (imageFileStreamPtr, kErrorMessages);
	
				UnlockFileStream (gProjectInfoPtr->windowInfoHandle, handleStatus);
			
				}	// end "if (projectFileNamePtr[0] == 0 && ..." 
		
					// Make the default name the same as the image file name with 	
					// .Project at the end.														
					
			CopyPToP (projectFileNamePtr, (UCharPtr)gProjectInfoPtr->imageFileName);
			RemoveSuffix (projectFileNamePtr);
			
			#if defined multispec_mac 
				ConcatFilenameSuffix (projectFileNamePtr, (StringPtr)"\0.Project\0");
			#endif	// defined multispec_mac                       
			  
         #if defined multispec_win
				ConcatFilenameSuffix (projectFileNamePtr, (StringPtr)"\0.Prj\0");
				projectFileStreamPtr->SetFileName (projectFileNamePtr);
			#endif	// defined multispec_win                   
			  
         #if defined multispec_lin
				ConcatFilenameSuffix (projectFileNamePtr, (StringPtr)"\0.prj\0");
				projectFileStreamPtr->SetFileName (projectFileNamePtr);
			#endif	// defined multispec_lin 
			
			}	// end "if (gProjectInfoPtr->fileName[0] == 0)" 

				// Now get wide character and unicode names.

		SetFileDoesNotExist (projectFileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (projectFileStreamPtr, NULL);
			
				// Force the project file type to TEXT
				
		SetType (projectFileStreamPtr, kTEXTFileType);
		
				// Allow user to name the file and set the volume and folder, 		
				// then open the new file.														
				
		errCode = PutFile (projectFileStreamPtr,
									bytesSpaceNeeded,
									IDS_SaveProjectFile, 
									gCreator);

		projectFileNamePtr =
				(FileStringPtr)GetFileNamePPointerFromFileStream (projectFileStreamPtr);
		returnFlag = (errCode == noErr) & (projectFileNamePtr[0] != 0);			
				
		if (!returnFlag)
			{
					// Load old file name back.							
				
			projectFileNamePtr[0] = (UInt8)savedNameLength;
			
					// Load original save statistics type settings.	
		
			gProjectInfoPtr->saveFieldSumsSquaresFlag = savedFieldSumsSquaresFlag;
			gProjectInfoPtr->saveFieldMeansCovFlag = savedFieldMeansCovFlag;
			gProjectInfoPtr->saveClassSumsSquaresFlag = savedClassSumsSquaresFlag;
			gProjectInfoPtr->saveClassMeansCovFlag = savedClassMeansCovFlag;
		
			}	// end "if (!returnFlag)"
		
		}	// end "if (saveCode == 1)" 
	
	return (returnFlag);

}	// end "GetProjectFileName"


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt32 GetSizeOfProjectFile
//
//	Software purpose:	The purpose of this routine is to compute a
//							reasonable estimate of the size of the project file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/06/199
//	Revised By:			Larry L. Biehl			Date: 06/04/1996	

SInt32 GetSizeOfProjectFile (void)

{
	WindowInfoPtr						projectWindowInfoPtr;
	
	UInt32								fileSize,
											numberChannels,
											numberCovarianceEntries;
											
	UInt16								maxLengthOfValue,
											maxSizeOfSumOfSquares,
											maxSizeOfSum;
	
	SignedByte							handleStatus;
	
	
			// Get the project window pointer.												
			
	projectWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
							gProjectInfoPtr->windowInfoHandle, &handleStatus, kNoMoveHi);
	
			// Initialize local variables														
	
	maxLengthOfValue = 14;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	numberCovarianceEntries = gProjectInfoPtr->numberCovarianceEntries;
	maxSizeOfSumOfSquares = GetLengthOfMaxSumOfSquares (projectWindowInfoPtr) + 1;
	maxSizeOfSum = GetLengthOfMaxSum (projectWindowInfoPtr) + 1;
	
	MHSetState (gProjectInfoPtr->windowInfoHandle, handleStatus);
	
			// Size of 'P' lines.																
			
	fileSize = 	32 + 
						281 + gProjectInfoPtr->imageFileName[0] +
							100 +
								4 + 4 * numberChannels;
	
			// Size of 'Training Statistics' line.											
					
	fileSize +=	20;
	
			// Size of 'C1' lines.																
					
	fileSize +=	61 * gProjectInfoPtr->numberStatisticsClasses;
	
			// Size of 'F1' lines.																
					
	fileSize +=	65 * gProjectInfoPtr->numberStatisticsFields;
	
	if (gProjectInfoPtr->saveClassMeansCovFlag)
		{
				// Size of 'CM' lines.															
			
		fileSize += gProjectInfoPtr->numberStatTrainClasses *
														(3 + maxLengthOfValue * numberChannels);
				
				// Size of 'CV' lines.															
				
		fileSize += gProjectInfoPtr->numberStatTrainClasses *
					(3 * numberChannels + maxLengthOfValue * numberCovarianceEntries);
				
		}	// end "if (gProjectInfoPtr->saveClassMeansCovFlag)" 
		
	if (gProjectInfoPtr->fieldSumSquaresStatsPtr)
		{
		if (gProjectInfoPtr->saveClassSumsSquaresFlag)
			{
					// Size of 'CS' lines.														
					
			fileSize += gProjectInfoPtr->numberStatTrainClasses * 
															(3 + maxSizeOfSum * numberChannels);
					
					// Size of 'CSS' lines.														
					
			fileSize += gProjectInfoPtr->numberStatTrainClasses * 
				(4 * numberChannels + maxSizeOfSumOfSquares * numberCovarianceEntries);
					
			}	// end "if (gProjectInfoPtr->saveClassSumsSquaresFlag)" 
			
				// Size of 'CN', 'CX' lines.													
			
		if (gProjectInfoPtr->saveClassMeansCovFlag || 
													gProjectInfoPtr->saveClassSumsSquaresFlag)
			fileSize += gProjectInfoPtr->numberStatTrainClasses * 
																	2 * (3 + 6 * numberChannels);
		
		if (gProjectInfoPtr->saveFieldMeansCovFlag)
			{
					// Size of 'FM' line.														
				
			fileSize += gProjectInfoPtr->numberStatTrainFields * 
														(3 + maxLengthOfValue * numberChannels);
					
					// Size of 'FV' lines.														
					
			fileSize += gProjectInfoPtr->numberStatTrainFields * 
						(3 * numberChannels + maxLengthOfValue * numberCovarianceEntries);
					
			}	// end "if (gProjectInfoPtr->saveFieldMeansCovFlag)" 
		
		if (gProjectInfoPtr->saveFieldSumsSquaresFlag)
			{
					// Size of 'FS' lines.														
					
			fileSize += gProjectInfoPtr->numberStatTrainFields *
																(3 + maxSizeOfSum * numberChannels);
					
					// Size of 'FSS' lines.														
					
			fileSize += gProjectInfoPtr->numberStatTrainFields * 
				(4 * numberChannels + maxSizeOfSumOfSquares * numberCovarianceEntries);
					
			}	// end "if (gProjectInfoPtr->saveFieldSumsSquaresFlag)" 
			
				// Size of 'FN', 'FX' lines.													
			
		if (gProjectInfoPtr->saveFieldMeansCovFlag || 
													gProjectInfoPtr->saveFieldSumsSquaresFlag)
			fileSize += gProjectInfoPtr->numberStatTrainFields * 
																		2 * (3 + 6 * numberChannels);
																	
		}	// end "if (gProjectInfoPtr->fieldSumSquaresStatsPtr)" 
		
			// Size of 'V1' lines.																
		
	fileSize += 15 * gProjectInfoPtr->numberTotalPoints;
	
			// Size of 'Test Fields' line.													
					
	fileSize +=	12;
					
	return (fileSize);

}	// end "GetSizeOfProjectFile" 


                   
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadChannelInformation
//
//	Software purpose:	The purpose of this routine is to read channel
//							information to the project file for the selected
//							class or field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1988
//	Revised By:			Larry L. Biehl			Date: 03/26/2006	

SInt16 ReadChannelInformation (
				HChannelStatisticsPtr		chanStatsPtr, 
				UCharPtr* 						inputStringPtrPtr, 
				SInt16							numberStatisticsChannels, 
				UInt32							type)

{			
	UCharPtr								inputStringPtr;

	SInt16								channel,
											sizeDouble;
	
	
	inputStringPtr = *inputStringPtrPtr;
	
	sizeDouble = sizeof (double);		
	
	switch (type)
		{
		case 1:
					// If type is 1, read channel means.									
			
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->mean = strtod ((char*)inputStringPtr, (Ptr*)&inputStringPtr);
				if (errno == ERANGE)											 	
					break;
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
					
		case 2:		
				// If type is 2, read channel sums.											
				
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->sum = strtod ((char*)inputStringPtr, (Ptr*)&inputStringPtr);

				if (errno == ERANGE)											 	
					break;
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
					
		case 3:		
				// If type is 3, read channel minimums.									
				
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->minimum =
							(SInt32)strtod ((char*)inputStringPtr, (Ptr*)&inputStringPtr);
				if (errno == ERANGE)											 	
					break;
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
					
		case 4:		
				// If type is 4, read channel maximums.									
				
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->maximum =
							(SInt32)strtod ((char*)inputStringPtr, (Ptr*)&inputStringPtr);
				if (errno == ERANGE)											 	
					break;
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
			
		case 11:
					// If type is 1, read channel means.									
			
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->mean = GetDoubleValue (inputStringPtr);
								
				chanStatsPtr++;
				inputStringPtr += sizeDouble;
			
				}	// end "for (channel=0; channel<..." 
			break;
					
		case 12:		
				// If type is 2, read channel sums.											
				
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->sum = GetDoubleValue (inputStringPtr);
					
				chanStatsPtr++;
				inputStringPtr += sizeDouble;
			
				}	// end "for (channel=0; channel<..." 
			break;
					
		case 13:		
				// If type is 3, read channel minimums.									
				
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->minimum = GetDoubleValue (inputStringPtr);
				
				chanStatsPtr++;
				inputStringPtr += sizeDouble;
			
				}	// end "for (channel=0; channel<..." 
			break;
					
		case 14:		
				// If type is 14, read channel maximums.									
				
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				chanStatsPtr->maximum = GetDoubleValue (inputStringPtr);
				
				chanStatsPtr++;
				inputStringPtr += sizeDouble;
			
				}	// end "for (channel=0; channel<..." 
			break;
			
		}	// end "switch (type)" 
		
			// Read the carriage return at the end of the line.						
	
	if ((*inputStringPtr != kCarriageReturn && *inputStringPtr != kLineFeed) || 
																						errno == ERANGE)
		{
		*inputStringPtrPtr = NULL;											 	
																								return (0);
																					
		}	// end "if ((*inputStringPtr != kCarriageReturn && ..." 
		
	*inputStringPtrPtr = inputStringPtr;	
	
	return (1);
	
}	// end "ReadChannelInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadCovarianceInformation
//
//	Software purpose:	The purpose of this routine is to read covariance
//							information to the project file for the selected
//							class or field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1988
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

SInt16 ReadCovarianceInformation (
				HSumSquaresStatisticsPtr		sumSquaresStatsPtr, 
				ParmBlkPtr 							paramBlockPtr,
				UCharPtr*							inputStringPtrPtr,
				UInt32								numberStatisticsChannels, 
				UCharPtr								fieldClassStringPtr,
				UInt32								binaryListingCode,
				UInt32								numberEndOfLineBytes,
				double*								boxRightPtr,
				double								boxIncrement)

{                                              
	UInt32								channel,
											covChan,										
											numberBytesToBeRead;
										
	SInt16								returnCode,
											sizeDouble,
											statisticsCode;
											
	Boolean								badStatisticsFlag = FALSE;
	
	char									character;
	
	                                   
	sizeDouble = sizeof (double);
	statisticsCode = gProjectInfoPtr->statisticsCode;
	
	for (channel=0; channel<numberStatisticsChannels; channel++)
		{                                 
		returnCode = strncmp ((char*)*inputStringPtrPtr, (char*)fieldClassStringPtr, 3);
		if (returnCode != 0)													
																								return (0);
		*inputStringPtrPtr += 3;
		
		if (sumSquaresStatsPtr != NULL)
			{
			if (statisticsCode == kMeanStdDevOnly)
				channel = numberStatisticsChannels - 1;
			
			if (binaryListingCode == 0)
				{
				for (covChan=0; covChan<=channel; covChan++)
					{
					*sumSquaresStatsPtr = 
								strtod ((char*)*inputStringPtrPtr, (Ptr*)inputStringPtrPtr);

					if (errno == ERANGE)											 	
						break;
						
					sumSquaresStatsPtr++;
					
					}	// end "for (covChan=0; covChan<=channel; covChan++)" 
					
				}	// end "if (binaryListingCode == 0)"
				
			else	// binaryListingCode == 10
				{
				for (covChan=0; covChan<=channel; covChan++)
					{
					*sumSquaresStatsPtr = GetDoubleValue (*inputStringPtrPtr);
					
					*inputStringPtrPtr += sizeDouble;
					sumSquaresStatsPtr++;
					
					}	// end "for (covChan=0; covChan<=channel; covChan++)" 
					
				}	// end "else binaryListingCode == 10"
		
					// Read the carriage return at the end of the line.							
	
			if ((**inputStringPtrPtr != kCarriageReturn && 
										**inputStringPtrPtr != kLineFeed) || errno == ERANGE)
				{
						// The statistics may be corrupted. An entire line of stats
						// was not read. Read the file until we are at the start
						// of the next line of statistics.
						
				badStatisticsFlag = TRUE;	
				
						// Check if it is possible that an entire line of stats
						// was not read. This could be the case if there is no
						// carriage return or line feed at the end of the line
						// and the number of bytes read is exactly equal to the
						// number of bytes possible in the buffer.
						
				#if defined multispec_mac
					while (paramBlockPtr->ioParam.ioActCount == 
																paramBlockPtr->ioParam.ioReqCount)
						{
						returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
						
						if (returnCode != noErr)
							break;
						
						character = paramBlockPtr->ioParam.ioBuffer[
															paramBlockPtr->ioParam.ioActCount-1];
															
						if (character == kCarriageReturn || character != kLineFeed)
							break;
						
						}	// end "if (paramBlockPtr->ioParam.ioActCount == ..."
				#endif	// defined multispec_mac
						
            #if defined multispec_win || defined multispec_lin
					while (paramBlockPtr->ioActCount == paramBlockPtr->ioReqCount)
						{
						returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
						
						if (returnCode != noErr)
							break;
						
						character = paramBlockPtr->ioBuffer[paramBlockPtr->ioActCount-1];
															
						if (character == kCarriageReturn || character != kLineFeed)
							break;
						
						}	// end "if (paramBlockPtr->ioParam.ioActCount == ..."				
				#endif	// defined multispec_win
				
				}	// end "if ((**inputStringPtrPtr != kCarriageReturn && ..."
			
			}	// end "if (sumSquaresStatsPtr)" 
			
		UpdateGraphicStatusBox (boxRightPtr, boxIncrement);

		if (binaryListingCode == 10)
			{
					// This data is in binary.  Reset the read start if needed and set
					// the correct number of bytes to be read.
					
			numberBytesToBeRead = 3 + sizeDouble*(channel+1) + numberEndOfLineBytes;
					
			#if defined multispec_mac
				paramBlockPtr->ioParam.ioPosOffset -= 
								(paramBlockPtr->ioParam.ioReqCount - numberBytesToBeRead);
				paramBlockPtr->ioParam.ioReqCount = numberBytesToBeRead + sizeDouble;
			#endif	// defined multispec_mac 

         #if defined multispec_win || defined multispec_lin
				paramBlockPtr->ioPosOffset -= 
											(paramBlockPtr->ioReqCount - numberBytesToBeRead);
				paramBlockPtr->ioReqCount = numberBytesToBeRead + sizeDouble; 
			#endif	// defined multispec_win  
			
			}	// end "if (binaryListingCode == 10)"
			
				// Read the next record.													
				                                      
		returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
		if (returnCode != noErr)
																								return (0);
	
		}	// end "for (channel=0; channel<..." 
	
	if (badStatisticsFlag)
		return (-1);
		
	else	// !badStatisticsFlag
		return (1);
	
}	// end "ReadCovarianceInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadMaskInformation
//
//	Software purpose:	The purpose of this routine is to read the mask information
//							from the project file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 ReadMaskInformation (
				Str255*								maskFileNamePtr,
				MaskInfoPtr 						maskInfoPtr,
				UCharPtr*							inputStringPtrPtr)
                                       	
{
	UCharPtr								savedInputStringPtr;
	int									returnCode;
	
	 
			// Read Mask Image File Name.												
	
	*inputStringPtrPtr = ReadStringFromFile (*inputStringPtrPtr, 
															(UCharPtr)maskFileNamePtr, 
															1, 
															255);
														
	if (*inputStringPtrPtr == NULL)										
																							return (-1);
																						
			// Now get the layer that the mask is in within the image file.
			
	savedInputStringPtr = *inputStringPtrPtr;
	returnCode = sscanf  ((char*)*inputStringPtrPtr, 
									"\t%d\r",
									&maskInfoPtr->fileLayer);
	*inputStringPtrPtr = savedInputStringPtr;
	if (returnCode != 1)													
																							return (-1);
																						
			// Make sure that the mask layer is not 0. A value other than 0 is an
			// indication that a mask file name does exist.
			
	if (maskInfoPtr->fileLayer == 0)
		maskInfoPtr->fileLayer = 1;
	
	return (0);	
	
}	// end "ReadMaskInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadModifiedStats
//
//	Software purpose:	The purpose of this routine is to read the modified
//							statistics for the given class from the project file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1993
//	Revised By:			Larry L. Biehl			Date: 03/15/2015

SInt16 ReadModifiedStats (
				ParmBlkPtr 							paramBlockPtr,
				UCharPtr*							inputStringPtrPtr,
				SInt16								numberChannels,
				HChannelStatisticsPtr 			modifiedClassChanStatsPtr,
				HCovarianceStatisticsPtr 		modifiedClassCovStatsPtr,
				UInt32								numberEndOfLineBytes,
				SInt16								formatArchitectureCode)
                                       	
{ 
	double								value;
	
	OSErr 								returnCode; 
	
	SInt32								count,
											savedIoReqCount;
		                     
	UInt32								numberValueBytes,
											i,
											localNumberChannels;
											
	SInt16								savedIoPosMode;
	
	Boolean								swapBytesFlag;
	
	
	localNumberChannels = (UInt32)numberChannels;
				
			// If the field card is next, then modified statistics are not present
			// and the modified stats flag should be changed to FALSE.
			
	returnCode = strncmp ((char*)*inputStringPtrPtr, "F1", 2);
	if (returnCode == 0)
																							return (-1);								
	
			// Save the position mode and the requested count.	
				
	#if defined multispec_mac 
		savedIoPosMode = paramBlockPtr->ioParam.ioPosMode;
		savedIoReqCount = paramBlockPtr->ioParam.ioReqCount;
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_lin	
		savedIoPosMode = paramBlockPtr->ioPosMode;
		savedIoReqCount = paramBlockPtr->ioReqCount;
	#endif	// defined multispec_win 

			// Now verify that modified statistics are present. If not
			// then indicate a card error by returning 32.
	
	if (strncmp ((char*)*inputStringPtrPtr, (char*)&gTextString3[57], 4) == 0)
		numberValueBytes = 8;
	
	else if (strncmp ((char*)*inputStringPtrPtr, (char*)&gTextString3[49], 3) == 0)
		numberValueBytes = k68881Bytes;
	
	else	// something wrong
																							return (32);              
				
			// Read the last line again with different parameters so that the		
			// binary data will be handled properly.										
			
	#if defined multispec_mac 
		paramBlockPtr->ioParam.ioPosOffset -= paramBlockPtr->ioParam.ioActCount;
		paramBlockPtr->ioParam.ioPosMode = fsFromStart;
		
				// Read the modified means to the project file.								
				
		paramBlockPtr->ioParam.ioReqCount = 
					4 + numberEndOfLineBytes + localNumberChannels * numberValueBytes;
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_lin		 		
		UInt32 numberCharsToReread = (UInt32)(paramBlockPtr->ioActCount - 
								(*inputStringPtrPtr - paramBlockPtr->ioBuffer));							
				
		paramBlockPtr->ioPosOffset -= numberCharsToReread;
		paramBlockPtr->ioPosMode = fsFromStart;
		
				// Read the modified means to the project file.								
				
		paramBlockPtr->ioReqCount = 
					4 + numberEndOfLineBytes + localNumberChannels * numberValueBytes;
	#endif	// defined multispec_win 

	returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
	if (returnCode != noErr)												
																							return (32);
			
	if (strncmp ((char*)*inputStringPtrPtr, (char*)&gTextString3[57], 4) == 0)
		numberValueBytes = 8;
	
	else if (strncmp ((char*)*inputStringPtrPtr, (char*)&gTextString3[49], 3) == 0)
		numberValueBytes = k68881Bytes;
	
	else	// something wrong
																							return (32); 

			// Set up swap bytes flag depending upon the system architecture
			
	if (formatArchitectureCode == kArchitectureNotKnown)
		{	
				// Try to determine from the number of end of line bytes
				
		if (numberEndOfLineBytes == 2) 
					// This is known for sure
			swapBytesFlag = gBigEndianFlag;
		
		else	// numberEndOfLineBytes != 2
			{
					// It could be either way depending on whether written with
					// Macintosh PPC or Intel. This is a simple check that should
					// get most but not all cases. Note that it will not know
					// correctly if the mean is actually 0.
					
			swapBytesFlag = !gBigEndianFlag;
			if (numberValueBytes != k68881Bytes)
				{
				gSwapBytesFlag = swapBytesFlag;
				value = GetDoubleValue (*inputStringPtrPtr);
				value = fabs (value);
				if (value > UInt32_MAX || value < 0.000000001)
					swapBytesFlag = gBigEndianFlag;
				
				}	// end "if (numberValueBytes != k68881Bytes)"
			
			}	// end "else numberEndOfLineBytes != 2"
			
		}	// end "if (formatArchitectureCode == kArchitectureNotKnown)"
		
	else if (formatArchitectureCode == kMacintoshMotorola)
		swapBytesFlag = !gBigEndianFlag;
		
	else // formatArchitectureCode == kMacintoshIntel || 
			//		formatArchitectureCode == kWindowsIntel)
		swapBytesFlag = gBigEndianFlag;
		
	gSwapBytesFlag = swapBytesFlag; 
			
	*inputStringPtrPtr += 4;		
		
	for (i=0; i<localNumberChannels; i++)
		{
		if (numberValueBytes == k68881Bytes)
			{
			#if defined multispec_mac 		
				#ifdef _MC68881_
					BlockMoveData (*inputStringPtrPtr, 
											&modifiedClassChanStatsPtr->mean, 
											numberValueBytes);
				#else	// not _MC68881_
					#if TARGET_RT_MAC_MACHO
								// Reading project files with sane double values will not
								// be allowed in MachO linked version of MultiSpec.
						modifiedClassChanStatsPtr->mean = 0;
						returnCode = -1;
					#else	// !TARGET_RT_MAC_MACHO
						x96tononx96 ((extended96*)*inputStringPtrPtr, 
											&modifiedClassChanStatsPtr->mean);
					#endif // TARGET_RT_MAC_MACHO, else...
				#endif		// _MC68881_ 
			#endif	// defined multispec_mac 
	
         #if defined multispec_win || defined multispec_lin
				modifiedClassChanStatsPtr->mean = 0;
				returnCode = -1;
			#endif	// defined multispec_win 
			
			}	// end "if (numberValueBytes == k68881Bytes)"
			
		else	// numberValueBytes == 8
			modifiedClassChanStatsPtr->mean = GetDoubleValue (*inputStringPtrPtr);
			
		modifiedClassChanStatsPtr->sum = -1;
		modifiedClassChanStatsPtr->minimum = DBL_MAX;
		modifiedClassChanStatsPtr->maximum = -DBL_MAX;
		modifiedClassChanStatsPtr++;
		*inputStringPtrPtr += numberValueBytes;
			
		}	// end "for (i=0; i<numberChannels; i++)" 
		
	gSwapBytesFlag = FALSE;
			
			// Read the modified covariance matrix to the project file.				
	
	modifiedClassChanStatsPtr -= localNumberChannels;
	for (i=1; i<=localNumberChannels; i++)
		{
		count = i * numberValueBytes;
		
		*inputStringPtrPtr = (UCharPtr)gCharBufferPtr1;

		#if defined multispec_mac
			paramBlockPtr->ioParam.ioReqCount = 4 + numberEndOfLineBytes + count;
		#endif	// defined multispec_mac 

      #if defined multispec_win || defined multispec_lin
			paramBlockPtr->ioReqCount = 4 + numberEndOfLineBytes + count; 
		#endif	// defined multispec_win  

		returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
		if (returnCode != noErr)												
																							return (32);
			
		if (strncmp ((char*)*inputStringPtrPtr, (char*)&gTextString3[61], 4) == 0)
			numberValueBytes = 8;
		
		else if (strncmp ((char*)*inputStringPtrPtr, (char*)&gTextString3[53], 3) == 0)
			numberValueBytes = k68881Bytes;
		
		else	// something wrong
																							return (32);
																							
		*inputStringPtrPtr += 4;
			
		if (numberValueBytes == k68881Bytes)
			{
			#if defined multispec_mac
				#ifdef _MC68881_ 
					BlockMoveData (*inputStringPtrPtr, modifiedClassCovStatsPtr, count);
					modifiedClassCovStatsPtr += i - 1;
				#else	// not _MC68881_ 
					UInt32					j;
					
					#if TARGET_RT_MAC_MACHO
								// Reading project files with sane double values will not
								// be allowed in MachO linked version of MultiSpec.

						for (j=0; j<i; j++)
							*inputStringPtrPtr += k68881Bytes;

						returnCode = -1;
					#else	// !TARGET_RT_MAC_MACHO
						for (j=0; j<i; j++)
							{
							x96tononx96 ((extended96*)*inputStringPtrPtr, 
												modifiedClassCovStatsPtr);
								
							modifiedClassCovStatsPtr++;
							*inputStringPtrPtr += k68881Bytes;
								
							}	// end "for (j=0; j<i; j++)" 
								
						modifiedClassCovStatsPtr--;
					#endif // TARGET_RT_MAC_MACHO, else...
				#endif		// _MC68881_ 
			#endif	// defined multispec_mac 
	
         #if defined multispec_win || defined multispec_lin
				UInt32					j;

				for (j=0; j<i; j++)
					*inputStringPtrPtr += k68881Bytes;

				returnCode = -1;
			#endif	// defined multispec_win || defined multispec_lin
			
			}	// end "if (numberValueBytes == k68881Bytes)"
			
		else	// numberValueBytes == 8
			{ 
					// Set up swap bytes flag depending upon the system
					// architecture
		
			gSwapBytesFlag = swapBytesFlag; 
	
			UInt32					j;
		
			for (j=0; j<i; j++)
				{
				*modifiedClassCovStatsPtr = GetDoubleValue (*inputStringPtrPtr);
					
				modifiedClassCovStatsPtr++;
				*inputStringPtrPtr += 8;
					
				}	// end "for (j=0; j<i; j++)" 
					
			modifiedClassCovStatsPtr--;
		
			gSwapBytesFlag = FALSE;
			
			}	// end "else numberValueBytes == 8"
			
		if (*modifiedClassCovStatsPtr > 0)
			modifiedClassChanStatsPtr->standardDev = sqrt (*modifiedClassCovStatsPtr);
			
		else	// *modifiedClassCovStatsPtr <= 0 
			modifiedClassChanStatsPtr->standardDev = 0;
	
		modifiedClassChanStatsPtr++;
		modifiedClassCovStatsPtr++;
			
		}	// end "for (i=1; i<=localNumberChannels; i++)" 										
				
	gSwapBytesFlag = FALSE;
	
			// Set reading back to 'New Line' mode and read the next line to		
			// be worked on.																		
			
	#if defined multispec_mac 
		paramBlockPtr->ioParam.ioPosMode = savedIoPosMode;
		paramBlockPtr->ioParam.ioReqCount = savedIoReqCount;
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_lin																	
		paramBlockPtr->ioPosMode = savedIoPosMode;
		paramBlockPtr->ioReqCount = savedIoReqCount;
	#endif	// defined multispec_win 
	                                             
	returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
	
	if (returnCode != noErr)												
																							return (32);
		
	return (returnCode);
	
}	// end "ReadModifiedStats" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadProjectFile
//
//	Software purpose:	The purpose of this routine is to read the project
//							file informaton from a disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			OpenProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1988
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

SInt16 ReadProjectFile (void)

{
	HParamBlockRec						paramBlock;
	
	Str255								testMaskFileName,
											trainingMaskFileName;
	
	double								fieldIncrement,
											fieldRight,
											lineIncrement,
											lineRight;
	
	CMFileStream*						projectFileStreamPtr;
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	WindowInfoPtr						projectWindowInfoPtr;
	
	UCharPtr								inputStringPtr,
											savedInputStringPtr,
											stringPtr;
											
	int									sscanfReturnCode;
	
	SInt32								bytesNeeded,
											longIntTemp,
											versionNumber;
												
	UInt32								binaryListingCode,
											bufferBytes,
											numberEndOfLineBytes;
	
	SInt16								channel,   
											endOfLineCode,
											field,
											fieldIndex,
											intTemp,
											intTemp2,
											fieldCount,
											fieldStatsLoadedCode,
											formatArchitectureCode,
											newPreviousField,
											point,
											pointIndex,
											returnCode,
											theType;
												
	UInt16								classIndex;
	
	Boolean								changedFlag,
											continueFlag,
											differentFileSourceFlag,
											enhancedStatsExistFlag,
											statsLoadedFlag;
	
			
			// Initialize local variables.													
			
	continueFlag = TRUE;
	differentFileSourceFlag = FALSE;
	formatArchitectureCode = 0;
   returnCode = -1;
   intTemp = 0;
   intTemp2 = 0;
	gNextTime = TickCount ();    
	
			// Check input pointers.  Continue if not NULL.								
			
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->windowInfoHandle)
		{
		projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
		
		projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													gProjectInfoPtr->windowInfoHandle, kLock);
	
				// Show status information dialog box if available.					
				
		if (gStatusDialogPtr)
			{
			gStatusBoxIncrement = 
						(double)(gStatusGraphicsBox.right - gStatusGraphicsBox.left);
			ShowDialogItem (gStatusDialogPtr, IDC_Status3);
			ShowDialogItem (gStatusDialogPtr, IDC_Status4);
			ShowDialogItem (gStatusDialogPtr, IDC_Status5);
			ShowDialogItem (gStatusDialogPtr, IDC_Status6);
			ShowDialogItem (gStatusDialogPtr, IDC_Status7);
			ShowDialogWindow (
					gStatusDialogPtr, kGraphicStatusDialogID, kDoNotSetUpDFilterTable);
			SetPortDialogPort (gStatusDialogPtr);
			DrawDialog (gStatusDialogPtr);
			LoadDItemStringNumber (kProjectStrID, 
											IDS_Project27, 
											gStatusDialogPtr, 
											IDC_Status2, 
											(Str255*)gTextString);
			
			}	// end "if (gStatusDialogPtr)" 
			
				// Get buffer to use to read project file information.
      
      bufferBytes = 5000;
		gCharBufferPtr1 = (HPtr)MNewPointer (bufferBytes);
		if (gCharBufferPtr1 == NULL)
																							return (-2);
		
				// Read a section of the project file. 			
				
		#if defined multispec_mac
			paramBlock.ioParam.ioPosOffset = 0;
			paramBlock.ioParam.ioRefNum = projectFileStreamPtr->refNum;
			paramBlock.ioParam.ioCompletion = NULL;
			paramBlock.ioParam.ioBuffer = gCharBufferPtr1;
			paramBlock.ioParam.ioReqCount = bufferBytes - 1;	// Allow for \0.
		
					// Find the end of line character that is used in this file.
					
			paramBlock.ioParam.ioPosMode = FindEndOfLineCode (
																		(ParmBlkPtr)&paramBlock,
																		projectFileStreamPtr,
																		&returnCode,
																		&endOfLineCode,
																		&numberEndOfLineBytes); 
		
					// Set flag indicating whether this project file was written by
					// a windows version of MultiSpec.
														
			if (endOfLineCode == kIBMNewLinePosMode)
				differentFileSourceFlag = TRUE;
		#endif	// defined multispec_mac
		
      #if defined multispec_win || defined multispec_lin
			paramBlock.ioPosOffset = 0;
			paramBlock.fileStreamPtr = projectFileStreamPtr;
			paramBlock.ioBuffer = (UCharPtr)gCharBufferPtr1;
			paramBlock.ioReqCount = bufferBytes - 1;	// Allow for \0.
		
					// Find the end of line character that is used in this
					// file.
					
			paramBlock.ioPosMode = FindEndOfLineCode ((ParmBlkPtr)&paramBlock,
																	projectFileStreamPtr,
																	&returnCode,
																	&endOfLineCode,
																	&numberEndOfLineBytes); 
		
					// Set flag indicating whether this project file was written by
					// a Mac version of MultiSpec.
														
			if (endOfLineCode == kMacNewLinePosMode)
				differentFileSourceFlag = TRUE;
		#endif	// defined multispec_win || defined multispec_lin 
		
		if (returnCode != noErr)
																							return (1);
				
		inputStringPtr = NULL;							

		returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
		
		IOCheck (returnCode, projectFileStreamPtr);
		if (returnCode != noErr)
																							return (1);
						
				// Verify the project file format version.
		
		sscanfReturnCode = sscanf (
					(char*)inputStringPtr,
					#if defined multispec_mac || defined multispec_win || defined multispec_mac_swift
						"PROJECT FORMAT VERSION %ld %ld\r",
					#endif
					#if defined multispec_lin
						"PROJECT FORMAT VERSION %d %d\r",
					#endif
					&versionNumber, 
					&longIntTemp);
		
		if (sscanfReturnCode != 2)	
																							return (10);
									
		if (gProjectInfoPtr->version != versionNumber)	
																							return (10);
		
		if (gProjectInfoPtr->version > 890105)	
			gProjectInfoPtr->otherInformationPosition = longIntTemp;             
		
				// Read "P1" record.																
				
		returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
		if (returnCode != noErr)
																							return (11);
																					
				// Verify that this is the correct line.									
				
		returnCode = strncmp ((char*)inputStringPtr, "P1 PROJECT NAME", 15);
		if (returnCode != 0)													
																							return (11);
																					
		inputStringPtr = SkipNTabs (inputStringPtr, 2);
		if (inputStringPtr == NULL)										
																							return (11);
																					
				// Read Image File Name.														
		
		inputStringPtr = ReadStringFromFile (
								inputStringPtr, gProjectInfoPtr->imageFileName, 1, 255);
		if (inputStringPtr == NULL)										
																							return (11);
																					
				// Read "P2" record.																
				
		returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
		if (returnCode != noErr)
																							return (12);
					
		if (gProjectInfoPtr->version == 890105)
			{	
			sscanfReturnCode = sscanf ((const char*)inputStringPtr, 
						"P2\t%d\t%d\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\r",
						&projectWindowInfoPtr->maxNumberLines,
						&projectWindowInfoPtr->maxNumberColumns,
						&projectWindowInfoPtr->totalNumberChannels, 
						&projectWindowInfoPtr->numberBytes,
						&gProjectInfoPtr->numberStatisticsClasses,
						&gProjectInfoPtr->numberStatTrainClasses,
						&gProjectInfoPtr->numberStatisticsFields,
						&gProjectInfoPtr->numberStatTrainFields,
						&gProjectInfoPtr->numberStatTestFields,
						&gProjectInfoPtr->numberTotalPoints,
						&intTemp2,
						&intTemp);
			if (sscanfReturnCode != 12)													
																							return (12);
																						
			}	// end "if (gProjectInfoPtr->version == 890105)" 
					
		else if (gProjectInfoPtr->version == 900508)
			{
			sscanfReturnCode = sscanf  (
					(char*)inputStringPtr, 
					"P2\t%d\t%d\t%hd\t%hd\t%d\t%d\t%hd\t%hd\t%hd\t%hd"
						"\t%hd\t%hd\t%hd\t%hd\r",
					&projectWindowInfoPtr->maxNumberLines,
					&projectWindowInfoPtr->maxNumberColumns,
					&projectWindowInfoPtr->totalNumberChannels, 
					&projectWindowInfoPtr->numberBytes,
					&gProjectInfoPtr->startLine,
					&gProjectInfoPtr->startColumn,
					&gProjectInfoPtr->numberStatisticsClasses,
					&gProjectInfoPtr->numberStatTrainClasses,
					&gProjectInfoPtr->numberStatisticsFields,
					&gProjectInfoPtr->numberStatTrainFields,
					&gProjectInfoPtr->numberStatTestFields,
					&gProjectInfoPtr->numberTotalPoints,
					&intTemp2,
					&intTemp);
			if (sscanfReturnCode != 14)
																							return (12);
																						
			}	// end "if (gProjectInfoPtr->version == 900508)" 
					
		else if (gProjectInfoPtr->version >= 911106)
			{
			SInt16	intTemp3 = 0;
			SInt16	intTemp4 = 0;
			SInt16	intTemp5 = 1;
			SInt16	intTemp6 = 0;
																				
			sscanfReturnCode = sscanf  (
					(const char*)inputStringPtr, 
					#if defined multispec_mac || defined multispec_win || defined multispec_mac_swift
						"P2\t%ld\t%ld\t%hd\t%hd\t%ld\t%ld\t%hd\t%hd\t%hd\t%ld\t%ld\t%ld\t%ld"
						"\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\r",
					#endif
					#if defined multispec_lin
						"P2\t%d\t%d\t%d\t%hd\t%d\t%d\t%hd\t%hd\t%hd\t%d\t%d\t%d\t%d"
						"\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\r",
					#endif
					&projectWindowInfoPtr->maxNumberLines, 
					&projectWindowInfoPtr->maxNumberColumns,
					&projectWindowInfoPtr->totalNumberChannels, 
					&projectWindowInfoPtr->numberBytes,
					&gProjectInfoPtr->startLine,
					&gProjectInfoPtr->startColumn,
					&projectWindowInfoPtr->numberBits,
					&intTemp3,
					&projectWindowInfoPtr->bandInterleave,
					&gProjectInfoPtr->numberHeaderBytes,
					&gProjectInfoPtr->numberTrailerBytes,
					&gProjectInfoPtr->numberPreLineBytes,
					&gProjectInfoPtr->numberPostLineBytes,
					&gProjectInfoPtr->numberStatisticsClasses,
					&gProjectInfoPtr->numberStatTrainClasses,
					&gProjectInfoPtr->numberStatisticsFields,
					&gProjectInfoPtr->numberStatTrainFields,
					&gProjectInfoPtr->numberStatTestFields,
					&gProjectInfoPtr->numberTotalPoints,
					&intTemp2,										// Number statistics channels
					&intTemp,
					&gProjectInfoPtr->statisticsCode,
					&theType,
					&intTemp4,										 // Added for 971008 version
					&intTemp5,										 // Added for 971008 version
					&gProjectInfoPtr->covarianceStatsToUse, // Added for 971008 version
					&projectWindowInfoPtr->dataTypeCode, 	// Added for 20060124 version
					&intTemp6,										// Added for 20060124 version
					&formatArchitectureCode);					// Added for 20110722 version

			if (sscanfReturnCode < 21 || sscanfReturnCode > 29)
																							return (12);
			
			gProjectInfoPtr->swapBytesFlag = intTemp3;
			gProjectInfoPtr->signedDataFlag = intTemp4;
			gProjectInfoPtr->useCommonCovarianceInLOOCFlag = intTemp5;
			
			gProjectInfoPtr->keepClassStatsOnlyFlag = FALSE;
			if (gProjectInfoPtr->version >= 930322)
				gProjectInfoPtr->keepClassStatsOnlyFlag = theType;
			
			if (gProjectInfoPtr->version >= 20060124)
				gProjectInfoPtr->hdfDataSetSelection = intTemp6;
            
			}	// end "else if (gProjectInfoPtr->version >= 911106)"
							
		gProjectInfoPtr->statsUpToDate = intTemp;
		
		if (gProjectInfoPtr->version == 911106)
			{
			gProjectInfoPtr->statisticsCode = kMeanCovariance;
			errno = 0;
			
			}	// end "if (gProjectInfoPtr->version == 971008)" 
			
				// Set type of statistics to be saved to false. They will be set
				// according to the type of statistics that are loaded.
		
		gProjectInfoPtr->saveFieldSumsSquaresFlag = FALSE;
		gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
		gProjectInfoPtr->saveClassSumsSquaresFlag = FALSE;
		gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
			
				// Make sure that the number of statistics channels is less
				// than or equal to the number of channels in the image.
				
		if (projectWindowInfoPtr->totalNumberChannels < (UInt16)intTemp2)											
																							return (12);
																					
				// Update some project window information in case this is an older
				// project file.
				
		if (projectWindowInfoPtr->numberBits <= 0)
			{
			projectWindowInfoPtr->numberBits = 16;
			if (projectWindowInfoPtr->numberBytes == 1)
				projectWindowInfoPtr->numberBits = 8;
				
			}	// end "if (projectWindowInfoPtr->numberBits <= 0)"
				
		if (projectWindowInfoPtr->numberBins <= 0)
			{
			if (projectWindowInfoPtr->numberBytes <= 2)
				projectWindowInfoPtr->numberBins = 
								(UInt32)ldexp ((double)1, projectWindowInfoPtr->numberBits);
			else	// projectWindowInfoPtr->numberBytes > 2
				projectWindowInfoPtr->numberBins = 2048;
				
			}	// end "if (projectWindowInfoPtr->numberBins <= 0)"
							
				// Read "P3" record.	Channels.												
				// First check that there is enough space to read the channel 		
				// list into.																		
				
		if (!ChangeProjectChannelsList (projectWindowInfoPtr->totalNumberChannels))
																							return (-2);
																					
		gProjectInfoPtr->numberStatisticsChannels = intTemp2;
													
		if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
			gProjectInfoPtr->numberCovarianceEntries = intTemp2;
									
		else	// gProjectInfoPtr->statisticsCode != kMeanStdDevOnly 
			gProjectInfoPtr->numberCovarianceEntries = 
														(SInt32)intTemp2 * (intTemp2 + 1)/2;
																					
				// Now get a buffer to read the rest of the project lines into		
				// that will be long enough for any matrix data.						
		
		bufferBytes = GetBufferForProjectReadWrites (
										projectWindowInfoPtr, bufferBytes, &inputStringPtr);
			                                         
		if (gCharBufferPtr1 == NULL)
																							return (-2);
										
		#if defined multispec_mac											
			paramBlock.ioParam.ioBuffer = gCharBufferPtr1;
			paramBlock.ioParam.ioReqCount = bufferBytes - 1;
		#endif	// defined multispec_mac
										
      #if defined multispec_win || defined multispec_lin											
			paramBlock.ioBuffer = (UCharPtr)gCharBufferPtr1;
			paramBlock.ioReqCount = bufferBytes - 1;
		#endif	// defined multispec_win                                                                  
				
		returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr); 
																					                          
		if (returnCode != noErr)
																							return (13);
		
		returnCode = strncmp ((char*)inputStringPtr, "P3", 2);
		if (returnCode != 0)													
																							return (13);
																					
		inputStringPtr += 2;
		errno = 0;
																					
		for (channel=0; 
				channel<gProjectInfoPtr->numberStatisticsChannels; 
				channel++)
			{
			inputStringPtr++;
			
			longIntTemp = strtol ((char*)inputStringPtr, (Ptr*)&inputStringPtr, 0);
			if (errno == ERANGE)													
																							return (13);
			gProjectInfoPtr->channelsPtr[channel]	= (SInt16)(longIntTemp - 1);
			
			}	// end "for (channel=1; channel<gProjectInfoPtr->..." 
				
		returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
		if (returnCode != noErr)
																							return (14);
		
				// Check if there is a training mask file.
				
		returnCode = strncmp ((char*)inputStringPtr, "P4 TRAINING MASK NAME", 21);
		if (returnCode == 0)
			{
					// Read training mask information.	
					
			if (ReadMaskInformation (&trainingMaskFileName,
												&gProjectInfoPtr->trainingMask,
												&inputStringPtr) < 0)	
																							return (14);
				
			returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
			if (returnCode != noErr)
																							return (15);
			
			}	// end "if (returnCode == 0)"
		
				// Check if there is a test mask file.
				
		returnCode = strncmp ((char*)inputStringPtr, "P5 TEST MASK NAME", 17);
		if (returnCode == 0)
			{
					// Read test mask information.	
					
			if (ReadMaskInformation (&testMaskFileName,
												&gProjectInfoPtr->testMask,
												&inputStringPtr) < 0)	
																							return (15);
				
			returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
			if (returnCode != noErr)
																							return (20);
			
			}	// end "if (returnCode == 0)"
			
				// Verify that the next line is the start of the training
				// statistics.
		
		returnCode = strncmp ((char*)inputStringPtr, "Training Statistics", 19);
		if (returnCode != 0)													
																							return (20);
		
				// Finish initializing project information structure.					
				
		gProjectInfoPtr->numberStorageClasses =
													gProjectInfoPtr->numberStatisticsClasses;
		gProjectInfoPtr->numberStorageFields = 
													gProjectInfoPtr->numberStatisticsFields;
		gProjectInfoPtr->numberStoragePoints = 
													gProjectInfoPtr->numberTotalPoints;
		gProjectInfoPtr->numberStorageStatFields =
													gProjectInfoPtr->numberStatTrainFields;
													
				// Check that there is enough space to read the storage class 		
				// list into.																		
				
		bytesNeeded = (SInt32)gProjectInfoPtr->numberStorageClasses * sizeof (SInt16);
		gProjectInfoPtr->storageClass = (SInt16*)CheckHandleSize (
															&gProjectInfoPtr->storageClassHandle,
															&continueFlag,    
															&changedFlag, 
															bytesNeeded);
		
		if (!continueFlag)
																							return (-2);
																						
		for (classIndex=0; 
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
					classIndex++)
			gProjectInfoPtr->storageClass[classIndex] = classIndex;
												
		gProjectInfoPtr->newProjectFlag = FALSE;
		
				// Allocate memory for the project class, field and point			
				// information.																	
			
		continueFlag = SetupClassFieldPointMemory ();
		
				// Memory for the class and field statistics will only be setup	
				// if information for them is found in the input file.				
				// This is done in 'ReadStatistics'.										
			
		if (!continueFlag)
																							return (-2);
		
				// Initialize index pointers for the fieldIdent, fieldPoints and	
				// statistics structures.														
		
		fieldIndex = 0;
		pointIndex = 0;
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
			
				// Read next record.																
	
		returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
		if (returnCode != noErr)											
																							return (21);
		
		if (gStatusDialogPtr)
			{
					// Get the number of class/field stat lines to read.				
					// returnCode is used here as a temporary variable.				
			
			returnCode = 4 * (gProjectInfoPtr->numberStatisticsChannels + 1);
			
			fieldIncrement = gStatusBoxIncrement /
														gProjectInfoPtr->numberStatTrainFields;
			lineIncrement = fieldIncrement/returnCode;
			
			fieldRight = (double)gStatusGraphicsBox.left;
			lineRight = (double)gStatusGraphicsBox.left;
		
			LoadDItemStringNumber (kProjectStrID, 
											IDS_Project28, 
											gStatusDialogPtr, 
											IDC_Status2, 
											(Str255*)gTextString);
								
			fieldCount = 1;
			LoadDItemValue (gStatusDialogPtr, IDC_Status4, (SInt32)fieldCount);
			LoadDItemValue (gStatusDialogPtr,
									IDC_Status6,
									(SInt32)gProjectInfoPtr->numberStatTrainFields);
			
			}	// end "if (gStatusDialogPtr)" 
	
				// Get a string that will be used later.									
				
		MGetString (gTextString3, kProjectStrID, IDS_Project25);
		
		enhancedStatsExistFlag = FALSE;
		
				// If the number of bytes of data is 4 or more then the statistics will
				// be read in as binary data not ascii number strings.  This is done 
				// so as not to loose precision with these larger numbers.
		
		binaryListingCode = 0;	
		if (projectWindowInfoPtr->numberBytes >= 4)
			binaryListingCode = 10;													
	
				// Read Class information for training statistics from disk file.	
				
		for (classIndex=0; 
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
				classIndex++)
			{									
			returnCode = strncmp ((char*)inputStringPtr, "C1", 2);
			if (returnCode != 0)												
																							return (21);
																					
			inputStringPtr += 2;
			
					// Read class name.															
			
			inputStringPtr = ReadStringFromFile (
													inputStringPtr, classNamesPtr->name, 1, 31);

			if (inputStringPtr == NULL)									
																							return (21);
			
					// Initialize some class parameters.	
					
			classNamesPtr->classNumber = classIndex + 1;	
			
			classNamesPtr->numberStatisticsPixels = 0;					
						
			classNamesPtr->priorWeights[0] = (float)10.;
			classNamesPtr->priorWeights[1] = (float)10.;
			classNamesPtr->priorWeights[2] = (float)10.;
			
					// Assume equal weighting for common covariance matrix.
					
			classNamesPtr->priorWeights[3] = (float)10.;
			
					// Indicate the leave-one-out covariance value has not been computed.
					
			classNamesPtr->looCovarianceValue = -1;
			
			classNamesPtr->userMixingParameter = 1.;
			
			classNamesPtr->covarianceStatsToUse = kOriginalStats;
			
			classNamesPtr->mixingParameterCode = kComputedOptimum;
				
			classNamesPtr->modifiedStatsStorage = -1;
			
					// Load in the class number.												
					// Read "number of fields", "number of training fields",			
					// "number of test fields", "number of train pixels", and		
					// "statistics up-to-date flag".											
			
			intTemp = 0;
			intTemp2 = 0;		
			
			sscanfReturnCode = sscanf ((char*)inputStringPtr,
						#if defined multispec_mac || defined multispec_win || defined multispec_mac_swift
							"\t%hd\t%hd\t%hd\t%lld\t%hd\t%hd\t%f\t%f\t%f\t%lf\t%hd\r",
						#endif
						#if defined multispec_lin
							"\t%hd\t%hd\t%hd\t%ld\t%hd\t%hd\t%f\t%f\t%f\t%f\t%hd\r",
						#endif
						&classNamesPtr->numberOfFields,
						&classNamesPtr->numberOfTrainFields,
						&classNamesPtr->numberOfTestFields,
						&classNamesPtr->numberTrainPixels,
						&intTemp,
						&intTemp2,
						&classNamesPtr->priorWeights[0],
						&classNamesPtr->priorWeights[1],
						&classNamesPtr->priorWeights[3],
						&classNamesPtr->looCovarianceValue,
						&classNamesPtr->covarianceStatsToUse);
			
			if (gProjectInfoPtr->version < 940105 && sscanfReturnCode != 5)
																							return (21);
																					
			if (gProjectInfoPtr->version == 940105 && sscanfReturnCode != 8)
																							return (21);
																					
			if (gProjectInfoPtr->version >= 971008 && sscanfReturnCode != 11)
																							return (21);
															
			classNamesPtr->listMessageFlag = TRUE;						
			classNamesPtr->statsUpToDate = intTemp;
			classNamesPtr->modifiedStatsFlag = intTemp2;
			
			statsLoadedFlag = FALSE;
			
			if (gProjectInfoPtr->keepClassStatsOnlyFlag)
				{
						// Read the rest of the class statistics.  They may be in	
						// any order.																
		
				returnCode = ReadStatistics (classNamesPtr,
														classIndex,
														(ParmBlkPtr)&paramBlock,
														&inputStringPtr,
														2,
														binaryListingCode,
														&lineRight, 
														lineIncrement,
														&statsLoadedFlag,
														&gProjectInfoPtr->saveClassSumsSquaresFlag,
														&gProjectInfoPtr->saveClassMeansCovFlag,
														numberEndOfLineBytes,
														formatArchitectureCode);
				
				if (returnCode != 0)									
																				return (returnCode);
			
				}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)" 
				
			else	// !...->keepClassStatsOnlyFlag 
				{		
				do 
					{                                                                        
							// Read next record.													
			
					returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
					if (returnCode != noErr)									
																							return (22);
					UpdateGraphicStatusBox (&lineRight, lineIncrement);                    
					
					stringPtr = inputStringPtr + 1;
					
					}	while (*stringPtr == 'M' || *stringPtr == 'N' || 
							*stringPtr == 'X' || *stringPtr == 'C'|| 
							*stringPtr == 'S');
				
				}	// end "else !...->keepClassStatsOnlyFlag" 
					
			if (classNamesPtr->modifiedStatsFlag)
				{                   
							// Make certain that memory is allocated for modified 		
							// statistics.												
					
				if (gProjectInfoPtr->modifiedClassChanStatsPtr == NULL)
					{		
					if (!SetupModifiedStatsMemory (
											gProjectInfoPtr->numberStatisticsClasses))
																							return (-2);
																						
					}	// end "if (...->modifiedClassChanStatsPtr == NULL)" 
						
						// Read "CEM" record.  "Class enhanced means."					
						// Read "CEC" record.  "Class enhanced covariances."			
					
				classNamesPtr->modifiedStatsStorage = classIndex; 
					
				returnCode = ReadModifiedStats (
							(ParmBlkPtr)&paramBlock,
							&inputStringPtr,
							gProjectInfoPtr->numberStatisticsChannels,
							&gProjectInfoPtr->modifiedClassChanStatsPtr[
										classIndex*gProjectInfoPtr->numberStatisticsChannels],
							&gProjectInfoPtr->modifiedClassCovStatsPtr[
										classIndex*gProjectInfoPtr->numberCovarianceEntries],
							numberEndOfLineBytes,
							formatArchitectureCode);
				
				if (returnCode == -1)
					{
					classNamesPtr->modifiedStatsFlag = FALSE;
					returnCode = 0;
					 
					}	// end "if (returnCode == -1)"
					
				else if (returnCode == 0)
					enhancedStatsExistFlag = TRUE;
					
				}	// end "if (classNamesPtr->modifiedStatsFlag)"
				
			if (returnCode != 0)									
																				return (returnCode);
																			
					// If the number of training pixels are defined for the class	
					// then indicate that at least some statistics have been			
					// loaded for the project.													
					
			if (classNamesPtr->numberStatisticsPixels > 0)
				gProjectInfoPtr->statsLoaded = TRUE;
			
					// Initialize first field number and last field number for the class.
					
			classNamesPtr->firstFieldNumber = -1;
			classNamesPtr->lastFieldNumber = -1;
			
					// Finish initializing class information structure.				
					
			classNamesPtr->classNumber = classIndex+1;
		
					// Read training field information into the project file for	
					// the class.																	
			
			for (field=1; field<=classNamesPtr->numberOfTrainFields; field++)
				{
						// Exit routine if user has "command period" down				
						
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (keyDownMask+mDownMask+mUpMask))
																							return (-2);
						
					}	// end "if (TickCount () >= nextTime)" 
		
				LoadDItemValue (gStatusDialogPtr, IDC_Status4, (SInt32)fieldCount);  
				fieldCount++; 
					
				fieldStatsLoadedCode = 0;
				
						// Make certain that memory was allocated for this field.	
				
				if (fieldIndex >= gProjectInfoPtr->numberStorageFields)
																							return (31);
				
				if (fieldIndex >= gProjectInfoPtr->numberStorageStatFields)
																							return (31);
																					
				fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldIndex];
				
						// Read "F1" record.  "Field summary."								
						
				returnCode = strncmp ((char*)inputStringPtr, "F1", 2);
				if (returnCode != 0)
																							return (31);
				inputStringPtr += 2;
			
						// Read field name.														
			
				inputStringPtr = ReadStringFromFile (
											inputStringPtr, fieldIdentPtr->name, 1, 31);
				if (inputStringPtr == NULL)									
																							return (31);
			
						// Initialize some field parameters that may not get read
						// end depending on the project file version.	
						
				fieldIdentPtr->maskValue = 0;
				fieldIdentPtr->numberPixelsUsedForStats = 0;
			
            sscanfReturnCode = sscanf ((char*)inputStringPtr,
                        #if defined multispec_mac || defined multispec_win || defined multispec_mac_swift
                           "\t%hd\t%hd\t%hd\t%hd\t%lld\t%hd\t%hd\t%hd\t%lld\r",
                        #endif
                        #if defined multispec_lin
                           "\t%hd\t%hd\t%hd\t%hd\t%ld\t%hd\t%hd\t%hd\t%ld\r",
                        #endif
								&fieldIdentPtr->numberOfPolygonPoints,
								&fieldIdentPtr->classStorage,
								&fieldIdentPtr->fieldType,
								&fieldIdentPtr->pointType,
								&fieldIdentPtr->numberPixels,
								&intTemp,
								&intTemp2,
								&fieldIdentPtr->maskValue,
								&fieldIdentPtr->numberPixelsUsedForStats);	// Added for 20130424 version);
				
				if (gProjectInfoPtr->version < 990107 && sscanfReturnCode != 7)
																							return (31);
																					
				if (gProjectInfoPtr->version >= 990107 && 
							gProjectInfoPtr->version <= 20110722 && sscanfReturnCode != 8)
																							return (31);
																					
				if (gProjectInfoPtr->version >= 20130424 && sscanfReturnCode != 9)
																							return (31);
																					
				fieldIdentPtr->classStorage--;
				fieldIdentPtr->loadedIntoClassStats = intTemp;
				fieldIdentPtr->statsUpToDate = intTemp2;
				if (classIndex != (UInt16)fieldIdentPtr->classStorage)
																							return (31);
																					
				if (gProjectInfoPtr->version < 20130424)
					fieldIdentPtr->numberPixelsUsedForStats = fieldIdentPtr->numberPixels;
				
						// Load field statistics set number.								
						
				fieldIdentPtr->trainingStatsNumber = fieldIndex;
			
				if (gProjectInfoPtr->keepClassStatsOnlyFlag)
					{
							// Skip the rest of the field records.  Those records  	
							// may be 'FM', 'FN', 'FX', 'FCV', and 'FSS'. 				
							// Only the class statistics are being kept.			 		
						
					do 
						{
								// Read next record.												
				
						returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
						if (returnCode != noErr)									
																							return (32);
						UpdateGraphicStatusBox (&lineRight, lineIncrement);
					
						stringPtr = inputStringPtr + 1;
						
						}	while (*stringPtr == 'M' || *stringPtr == 'N' || 
								*stringPtr == 'X' || *stringPtr == 'C'|| 
								*stringPtr == 'S');
				
					}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)" 
			
				else	// !gProjectInfoPtr->keepClassStatsOnlyFlag 
					{
							// If the field statistics are being kept, read the 		
							// rest of the field statistics.  They may be in any 		
							// order.																
		
					returnCode = ReadStatistics (
													classNamesPtr,
													fieldIndex,
													(ParmBlkPtr)&paramBlock,
													&inputStringPtr,
													1,
													binaryListingCode,
													&lineRight, 
													lineIncrement,
													&statsLoadedFlag,
													&gProjectInfoPtr->saveFieldSumsSquaresFlag,
													&gProjectInfoPtr->saveFieldMeansCovFlag,
													numberEndOfLineBytes,
													formatArchitectureCode);
													
					if (returnCode != 0)									
																					return (returnCode);
					
					}	// end "else !gProjectInfoPtr->keepClassStatsOnlyFlag"

						// Make certain that the field parameters make sense
						// relative to the statistics that are available.			
						
				if (!statsLoadedFlag)
					fieldIdentPtr->statsUpToDate = FALSE;
							
						// Make certain that there is enough memory for the field 	
						// coordinates.  This check was added in case the project	
						// file was hand edited and the total number of points		
						// given in the project P2 information is not correct.
						// Note that the first two points for polygon fields represent
						// the bounding rectangle for the polygon area.		
			
				bytesNeeded = (UInt32)(pointIndex + 2 +
											fieldIdentPtr->numberOfPolygonPoints) *
																		sizeof (ProjectFieldPoints);
				gProjectInfoPtr->fieldPointsPtr = 
						(HPFieldPointsPtr)CheckHandleSize (
														&gProjectInfoPtr->fieldCoordinatesHandle,
														&continueFlag,    
														&changedFlag, 
														bytesNeeded);
				
				if (!continueFlag)
																							return (41);
																					
				if (fieldIdentPtr->pointType <= kPolygonType)
					{    
							// Read "V1" records.  "Coordinates of field."					
					
					fieldIdentPtr->firstPolygonPoint = pointIndex;
					
					if (fieldIdentPtr->pointType == kPolygonType)
						pointIndex += 2;
						
					for (point=0;
						  point<fieldIdentPtr->numberOfPolygonPoints;
						  point++)
						{
						sscanfReturnCode = sscanf (
										(char*)inputStringPtr,
										"V1\t%d\t%d\r",
										&gProjectInfoPtr->fieldPointsPtr[pointIndex].line,
										&gProjectInfoPtr->fieldPointsPtr[pointIndex].col);

                  if (sscanfReturnCode != 2)
																							return (41);
						
								// Read next record.													
				
						returnCode = GetNextLine ((ParmBlkPtr)&paramBlock,
															&inputStringPtr);
						if (returnCode != noErr)								
																							return (41);
						
						pointIndex++;
						
						}	// end "for (point=0; point<fieldIdentPtr..."
						
					}	// end "if (fieldIdentPtr->pointType <= kPolygonType)"
				
						// Update first field number and last field for this class.	
				
				newPreviousField = classNamesPtr->lastFieldNumber;
				
				if (newPreviousField == -1)
					classNamesPtr->firstFieldNumber = fieldIndex;
						
				classNamesPtr->lastFieldNumber = fieldIndex;
				
						// Update previous field and next field items.					
					
				fieldIdentPtr->previousField = newPreviousField;
				
				if (newPreviousField != -1)
					gProjectInfoPtr->fieldIdentPtr[newPreviousField].nextField = 
																							fieldIndex;
					
				fieldIdentPtr->nextField = -1;
				
						// Get the label point if this is a polygon field.
				
				if (fieldIdentPtr->pointType == kPolygonType)		
					GetPolygonLabelPoint (fieldIdentPtr,
													gProjectInfoPtr->fieldPointsPtr,
													&fieldIdentPtr->labelPoint);
					
				fieldIndex++;
				
				UpdateGraphicStatusBox (&fieldRight, fieldIncrement);
					
							// Sync the progress bar indicator for statistics with
							// that being used for fields.
							
				lineRight = fieldRight;
				
				}	// end "for (field=1; field<=..." 
				
			classNamesPtr++;
			
			}	// end "for (classIndex=1; ..." 
										
		if (gProjectInfoPtr->modifiedClassChanStatsPtr != NULL)
			{         
			gProjectInfoPtr->enhancedStatsExistFlag = enhancedStatsExistFlag;
			
			if (enhancedStatsExistFlag)
				{
				if (gProjectInfoPtr->version < 971008)
					gProjectInfoPtr->covarianceStatsToUse = kEnhancedStats;
			
						// The unequal weights will be used by default.
					
				gProjectInfoPtr->modifiedClassWeightSet = kUnequalWeights; 
				
				}	// end "if (enhancedStatsExistFlag)"
			
			}	// end "if (...->modifiedClassChanStatsPtr != NULL)" 
			
				
		//returnCode = strncmp (inputStringPtr, "Test Fields", 11);
		CompareStringsNoCase (inputStringPtr, (UCharPtr)"Test Fields", 11);
		if (returnCode != 0)
																						return (130);
			
				// Read the test field information from the disk in order of		
				// class.																			
				
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		for (classIndex=0;
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
				classIndex++)
			{
					// Read training field information into the project file for	
					// the class.																	
			
			for (field=1; field<=classNamesPtr->numberOfTestFields; field++)
				{
						// Make certain that memory was allocated for this field.	
				
				if (fieldIndex >= gProjectInfoPtr->numberStorageFields)
																						return (131);
																					
				fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldIndex];
																						
						// Read "F1" record.  "Field summary."								
			
				returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, &inputStringPtr);
				if (returnCode != noErr)									
																						return (131);
				
						// Read field name.														
		
				returnCode = strncmp ((char*)inputStringPtr, "F1", 2);
				if (returnCode != 0)
																						return (131);
			
				inputStringPtr = ReadStringFromFile (
											inputStringPtr, fieldIdentPtr->name, 1, 31);
				if (inputStringPtr == NULL)								
																						return (131);
			
						// Initialize some field parameters that may not get read
						// end depending on the project file version.	
						
				fieldIdentPtr->maskValue = 0;
				fieldIdentPtr->numberPixelsUsedForStats = 0;
						
				savedInputStringPtr = inputStringPtr;
				sscanfReturnCode = sscanf ((char*)inputStringPtr, 
													"\t%hd\t%hd\t%hd\t%hd\t%lld\t%hd\t%hd\t%hd\r",
													&fieldIdentPtr->numberOfPolygonPoints,
													&fieldIdentPtr->classStorage,
													&fieldIdentPtr->fieldType,
													&fieldIdentPtr->pointType,
													&fieldIdentPtr->numberPixels,
													&intTemp,
													&intTemp2,
													&fieldIdentPtr->maskValue);
				inputStringPtr = savedInputStringPtr;
								
				if (gProjectInfoPtr->version < 990107 && sscanfReturnCode != 7)
																						return (131);
																					
				if (gProjectInfoPtr->version >= 990107 && sscanfReturnCode != 8)
																						return (131);
																					
				fieldIdentPtr->classStorage--;
				if (classIndex != (UInt16)fieldIdentPtr->classStorage)
																						return (131);
							
						// Make certain that there is enough memory for the field 	
						// coordinates.  This check was added in case the project	
						// file was hand edited and the total number of points		
						// given in the project P2 information is not correct.		
			
				bytesNeeded = (UInt32)(pointIndex + 2 +
												fieldIdentPtr->numberOfPolygonPoints) *
																		sizeof (ProjectFieldPoints);
				gProjectInfoPtr->fieldPointsPtr =
									(HPFieldPointsPtr)CheckHandleSize (
													&gProjectInfoPtr->fieldCoordinatesHandle,
													&continueFlag,     
													&changedFlag, 
													bytesNeeded);
				
				if (!continueFlag)
																						return (141);
				
				if (fieldIdentPtr->pointType <= kPolygonType)
					{
							// Read "V1" records.  "Coordinates of field."
						
					fieldIdentPtr->firstPolygonPoint = pointIndex;
					
					if (fieldIdentPtr->pointType == kPolygonType)
						pointIndex += 2;
						
					for (point=0;
							point<fieldIdentPtr->numberOfPolygonPoints;
								point++)
						{
						returnCode = GetNextLine ((ParmBlkPtr)&paramBlock, 
															&inputStringPtr);
						if (returnCode != noErr)								
																						return (141);
					
						savedInputStringPtr = inputStringPtr;
						sscanfReturnCode = sscanf (
										(char*)inputStringPtr,
										"V1\t%d\t%d\r",
										&gProjectInfoPtr->fieldPointsPtr[pointIndex].line,
										&gProjectInfoPtr->fieldPointsPtr[pointIndex].col);
						inputStringPtr = savedInputStringPtr;
						if (sscanfReturnCode != 2)										
																						return (141);
						pointIndex++;
						
						}	// end "for (point=0; point<fieldIdentPtr..."
						
					}	// end "if (fieldIdentPtr->pointType <= kPolygonType)" 
				
						// Update first field number and last field for this class.	
				
				newPreviousField = classNamesPtr->lastFieldNumber;
				
				if (newPreviousField == -1)
					classNamesPtr->firstFieldNumber = fieldIndex;
						
				classNamesPtr->lastFieldNumber = fieldIndex;
				
						// Update previous field and next field items.					
					
				fieldIdentPtr->previousField = newPreviousField;
				
				if (newPreviousField != -1)
					gProjectInfoPtr->fieldIdentPtr[newPreviousField].nextField = 
																							fieldIndex;
					
				fieldIdentPtr->nextField = -1;
				
						// Get the label point if this is a polygon field.
				
				if (fieldIdentPtr->pointType == kPolygonType)		
					GetPolygonLabelPoint (fieldIdentPtr,
													gProjectInfoPtr->fieldPointsPtr,
													&fieldIdentPtr->labelPoint);
					
				fieldIndex++;
				
				}	// end "for (field=1; field<=..." 
				
			classNamesPtr++;
					
			}	// end "for (classIndex=1; ..." 
			
				// Dispose of the buffer used to read the data into here. This 
				// needs to be done here in case 'gCharBufferPtr1' is used
				// during the "GetSpecifiedMaskFile" operation.
				
		gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
			
				// Read in the training mask if one exists.
				
		if (gProjectInfoPtr->trainingMask.fileLayer > 0)
			GetSpecifiedMaskFile (kTrainingMaskSet,
											(UCharPtr)&trainingMaskFileName,
											&gProjectInfoPtr->trainingMask,
											TRUE);
		
				// Read in the test mask if one exists.
				
		if (gProjectInfoPtr->testMask.fileLayer > 0)
			GetSpecifiedMaskFile (kTestMaskSet,
											(UCharPtr)&testMaskFileName,
											&gProjectInfoPtr->testMask,
											TRUE); 
		
				// Make certain that the total number of points is correct.			
		
		if (gProjectInfoPtr->numberStoragePoints != pointIndex)
			gProjectInfoPtr->changedFlag = TRUE;
				
		gProjectInfoPtr->numberStoragePoints = 
											gProjectInfoPtr->numberTotalPoints = pointIndex;
										
		CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
		
		VerifyClassAndFieldParameters (differentFileSourceFlag);
		
		returnCode = 0;
		
		}	// end "if (projectFilePtr != NULL && ...)" 

	return (returnCode);
	
}	// end "ReadProjectFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 ReadStatistics
//
//	Software purpose:	The purpose of this routine is to read the statistics
//							information to the project file for the selected class or field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ReadProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/08/1992
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

SInt16 ReadStatistics (
				HPClassNamesPtr					classNamesPtr, 
				SInt16								storageIndex, 
				ParmBlkPtr							paramBlockPtr,
				UCharPtr*							inputStringPtrPtr, 
				SInt16								classFieldCode,
				UInt32								binaryListingCode,
				double*								boxRightPtr, 
				double								boxIncrement,
				Boolean*								statsLoadedFlagPtr,
				Boolean*								sumsSquaresFlagPtr,
				Boolean*								meansCovFlagPtr,
				UInt32								numberEndOfLineBytes,
				SInt16								formatArchitectureCode)

{							
	SInt64								numberPixels;
	
	HChannelStatisticsPtr			chanStatsPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	HSumSquaresStatisticsPtr		sumSquaresStatsPtr,
											tSumSquaresStatsPtr;
												
	UCharPtr								inputStringPtr;
	
	SInt32								savedIoPosMode, 
											savedIoReqCount;

	SInt16								cardCode,
											errCode,
											numberStatisticsChannels,
											returnCode,	// -1 = line of stats not read correctly; 
															//  0 = line of project file could not be read
															//	 1 = line of project file read okay
											statsLoadedCode,
											stringStart;
	
	Boolean								initializedFlag,
											maxValuesReadFlag,
											returnFlag,
											statisticsBadFlag,
											statsUpToDate,
											swapBytesFlag;
	
	
	initializedFlag = FALSE;
	maxValuesReadFlag = FALSE;
	returnFlag = FALSE;
	*statsLoadedFlagPtr = FALSE;
	statisticsBadFlag = FALSE;
	chanStatsPtr = NULL;
	sumSquaresStatsPtr = NULL;
	numberStatisticsChannels = gProjectInfoPtr->numberStatisticsChannels;
	stringStart = 24 * (classFieldCode-1) + 1;
	
			// Get the statistics pointers. They will only be needed if statistics
			// actually exist in the file for the input class or field. If memory
			// has not been allocated yet for the statistics, the return pointers
			// will be set to NULL.
	
	GetProjectStatisticsPointers (classFieldCode, 
											storageIndex, 
											&chanStatsPtr, 
											&sumSquaresStatsPtr,
											NULL,
											NULL);
	
			// Prepare to read binary data if needed.
			
	if (binaryListingCode == 10)	
		{									
		returnCode = SetupForStatisticsRead (paramBlockPtr,
															*inputStringPtrPtr,
															numberEndOfLineBytes,
															numberStatisticsChannels,
															formatArchitectureCode,
															&savedIoPosMode, 
															&savedIoReqCount,
															&swapBytesFlag);
															
		if (returnCode != noErr)
																							return (32);
																					
		}	// end "if (binaryListingCode == 10)"
									
	statsLoadedCode = 0;
	cardCode = 0; 
	
	inputStringPtr = *inputStringPtrPtr;
	
	do
		{                                
		if (cardCode < 5)
			{
			returnCode = GetNextLine (paramBlockPtr, &inputStringPtr);
			if (returnCode != noErr)
																							return (32);
			
			}	// end "if (cardCode < 5)" 
		
		cardCode = 0;
		if (strncmp ((char*)inputStringPtr,  
										(char*)&gTextString3[stringStart+20], 3) == 0)
			cardCode = 6;		// FCV or CCV
		
		else if (strncmp ((char*)inputStringPtr,  
										(char*)&gTextString3[stringStart+16], 3) == 0)
			cardCode = 5;		// FSS or CSS
		
		else if (strncmp ((char*)inputStringPtr,  
										(char*)&gTextString3[stringStart+12], 2) == 0)
			cardCode = 4;		// FX or CX
			
		else if (strncmp ((char*)inputStringPtr,  
										(char*)&gTextString3[stringStart+8], 2) == 0)
			cardCode = 3;		// FN or CN
			
		else if (strncmp ((char*)inputStringPtr, 
										(char*)&gTextString3[stringStart+4], 2) == 0)		
			cardCode = 2;		// FS or CS
		
		else if (strncmp ((char*)inputStringPtr, 
										(char*)&gTextString3[stringStart], 2) == 0)		
			cardCode = 1;		// FM or CM
		
		else if (classFieldCode == kClassStatsOnly)
			{
					// Check for modified class statistics.								
		
			if (strncmp ((char*)inputStringPtr, (char*)&gTextString3[49], 3) == 0)
				cardCode = -1;
				
			}	// end "else if (classFieldCode == kClassStatsOnly)" 
		
		if (cardCode > 0)
			{
					// Some statistics do exist for the project.  Make certain 		
					// that memory has been allocated for it.								
				
			if (chanStatsPtr == NULL)
				{
						// Memory has not been allocated. Get memory for the statistics.
						
				if (!SetupStatsMemory ())
																					return (-2);
					
						// Memory setup was okay. Get the pointers again.											
	
				GetProjectStatisticsPointers (classFieldCode, 
														storageIndex, 
														&chanStatsPtr, 
														&sumSquaresStatsPtr,
														NULL,
														NULL);
				
				}	// end "if (chanStatsPtr == NULL)"
				
					// Initialize the statistics memory for this class or field.
			
			if (!initializedFlag)
				{	
				ZeroStatisticsMemory (chanStatsPtr, 
												sumSquaresStatsPtr, 
												numberStatisticsChannels,
												gProjectInfoPtr->statisticsCode,
												kTriangleOutputMatrix);
												
				initializedFlag = TRUE;
				
				}	// end "if (!initializedFlag)"
																					
			}	// end "if (cardCode > 0)"
			
		switch (cardCode)
			{	
			case 1:
						// Read "CM" record.  "Class channel means."						
						// Read "FM" record.  "Field channel means."						
						// Do not read this record if the field channel sums have	
						// already been read.													 
				                    
				returnCode = 1;
				inputStringPtr += 2;
				
						// Do not read the channel means if the channel sums have been
						// read.
						
				if (!(statsLoadedCode & 0x0002))
					{
					statsLoadedCode |= 0x0001;
					errCode = 32;
					returnCode = ReadChannelInformation (chanStatsPtr,
																		&inputStringPtr,
																		numberStatisticsChannels, 
																		binaryListingCode+1);
					
					}	// end "if (!(fieldStatsLoadedCode & 0x0002))" 
					
				*meansCovFlagPtr = TRUE;
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
				break;
				
			case 2:
						// Read "CS" record.  "Class channel sums."						
						// Read "FS" record.  "Field channel sums."						
						              
				statsLoadedCode |= 0x0002;
				errCode = 33; 
				inputStringPtr += 2;
				returnCode = ReadChannelInformation (chanStatsPtr,
																	&inputStringPtr,
																	numberStatisticsChannels, 
																	binaryListingCode+2);
					
				*sumsSquaresFlagPtr = TRUE;
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
				break;
				
			case 3:
						// Read "CN" record.  "Class channel minimums."					
						// Read "FN" record.  "Field channel minimums."					
						
				statsLoadedCode |= 0x0004;
				errCode = 34; 
				inputStringPtr += 2;
				returnCode = ReadChannelInformation (chanStatsPtr,
																	&inputStringPtr,
																	numberStatisticsChannels, 
																	binaryListingCode+3);
					
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
				break;
				
			case 4:
						// Read "CX" record.  "Class channel maximums."					
						// Read "FX" record.  "Field channel maximums."					
				
				maxValuesReadFlag = TRUE;
				statsLoadedCode |= 0x0008;
				errCode = 35;
				inputStringPtr += 2;
				returnCode = ReadChannelInformation (chanStatsPtr,
																	&inputStringPtr,
																	numberStatisticsChannels, 
																	binaryListingCode+4);
					
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
				break;
				
			case 5:				
						// Read "CSS" record.  "Class channel sum of squares."		
						// Read "FSS" record.  "Field channel sum of squares."		
					
				statsLoadedCode |= 0x0020;
				errCode = 36;
				returnCode = ReadCovarianceInformation (sumSquaresStatsPtr,
																		paramBlockPtr, 
																		&inputStringPtr,
																		numberStatisticsChannels, 
																		&gTextString3[stringStart+16],
																		binaryListingCode,
																		numberEndOfLineBytes,
																		boxRightPtr, 
																		boxIncrement);
					
				*sumsSquaresFlagPtr = TRUE;
				break;
				
			case 6:
						// Read "CCV" records.  "Class covariances."						
						// Read "FCV" records.  "Field covariances."						
						// Do not read these records if the field channel sums 		
						// of squares have already been read.								 
				
				tSumSquaresStatsPtr = sumSquaresStatsPtr;
				if (statsLoadedCode & 0x0020)
					tSumSquaresStatsPtr = NULL;
					
				statsLoadedCode |= 0x0010;
				errCode = 37;
				returnCode = ReadCovarianceInformation (tSumSquaresStatsPtr,
																		paramBlockPtr, 
																		&inputStringPtr, 
																		numberStatisticsChannels, 
																		&gTextString3[stringStart+20],
																		binaryListingCode,
																		numberEndOfLineBytes,
																		boxRightPtr, 
																		boxIncrement);
					
				*meansCovFlagPtr = TRUE;
				break;
				
			case -1:
				errCode = 38;
				returnCode = 1;						
				break;
				
				
			default:
				returnCode = 0;
				errCode = 39;						
				break;
				
			}	// end "switch (cardCode)" 
			
		if ((cardCode != 0) && (returnCode == 0))
			{				
			returnFlag = TRUE;
			break;
			
			}	// end "if ((cardCode != 0) && (returnCode == 0))"
			
		if ((cardCode != 0) && (returnCode == -1))				
			statisticsBadFlag = TRUE;									
		
		if (binaryListingCode == 10 && cardCode >= 1 && cardCode <= 4)
			{	
					// One character too many was read; back up read start by 1.
			#if defined multispec_mac 
				paramBlockPtr->ioParam.ioPosOffset -= 1;
			#endif	// defined multispec_mac 

         #if defined multispec_win || defined multispec_lin	
				paramBlockPtr->ioPosOffset -= 1;
			#endif	// defined multispec_win 
			
			}	// end "if (binaryListingCode == 10 && cardCode >= 1 && cardCode <= 4)"

		}		while (cardCode > 0);
		
	if (binaryListingCode == 10)
		FinishUpStatisticsRead (paramBlockPtr,
											&inputStringPtr,
											savedIoPosMode, 
											savedIoReqCount);
									
	if (returnFlag)
																					return (errCode);
	
	*statsLoadedFlagPtr = TRUE;
	if (classFieldCode == 1)
		{
				// Field Statistics being read.
						
		fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[storageIndex];
		
				// Make certain that the field parameters make sense 				
				// relative to the statistics that are available.
				// If covariances are being used and no covariance information
				// exists, then indicate that the statistics are not up-to-date.
																				
		if (gProjectInfoPtr->statisticsCode == kMeanCovariance && 
																!(statsLoadedCode & 0x0030))
			fieldIdentPtr->statsUpToDate = FALSE;
			
				// If no mean information exists, then indicate that the statistics 
				// are not up-to-date.
																				
		if (!(statsLoadedCode & 0x0003))
			fieldIdentPtr->statsUpToDate = FALSE;
				
				// If returnCode is -1, then the data for the statistics could be corrupted.
				// This usually means that the data could not be read correctly from the
				// project file. Force the statistics to be recomputed.
		
		if (statisticsBadFlag)						
			fieldIdentPtr->numberPixels = 0;
			
		if (fieldIdentPtr->numberPixels <= 0)
			{
			fieldIdentPtr->numberPixels = 0;																		
			fieldIdentPtr->statsUpToDate = FALSE;
																					
			}	// end "if (fieldIdentPtr->numberPixels <= 0)"
			
		if (!maxValuesReadFlag)															
			fieldIdentPtr->statsUpToDate = FALSE;
			
		if (!fieldIdentPtr->statsUpToDate)
			{
			gProjectInfoPtr->statsUpToDate = FALSE;
			classNamesPtr->statsUpToDate = FALSE;
			*statsLoadedFlagPtr = FALSE;
			
			}	// end "if (!fieldIdentPtr->statsUpToDate)" 
			
		numberPixels = fieldIdentPtr->numberPixels;
		statsUpToDate = fieldIdentPtr->statsUpToDate;
			
		}	// end "if (classFieldCode == 1)" 
	
	else	// classFieldCode == 2 
		{		
				// Make certain that the class parameters make sense relative to the 
				// statistics that are available.
				// If covariances are being used and no covariance information
				// exists, then indicate that the statistics are not up-to-date.
				//
				// Note that we are assuming that if 'looCovarianceValue' exists 
				// , i.e. not -1, then it is still valid. It was valid when the
				// project file was written to disk. If 'statsUpToDate' flag is
				// not set then it means that the orginal statistics were not saved
				// to disk and therefore need to be recalculated. looCovarianceValue
				// does not need to be recalculated.
				// We are also going to keep the number of training pixels so that we
				// know how many pixels are in the class even if the stats are not loaded.	
																				
		if (gProjectInfoPtr->statisticsCode == kMeanCovariance && 
																		!(statsLoadedCode & 0x0030))
			classNamesPtr->statsUpToDate = FALSE;
			
				// If no mean information exists, then indicate that the statistics 
				// are not up-to-date.
																				
		if (!(statsLoadedCode & 0x0003))
			classNamesPtr->statsUpToDate = FALSE;
				
				// If returnCode is -1, then the data for the statistics could be corrupted.
				// This usually means that the data could not be read correctly from the
				// project file. Force the statistics to be recomputed.
		
		if (statisticsBadFlag)						
			classNamesPtr->numberTrainPixels = 0;
			
				// If the number of pixels is 0 or less, then force the statistics to 
				// be recomputed.
				
		if (classNamesPtr->numberTrainPixels <= 0)
			{
			classNamesPtr->numberTrainPixels = 0;			
			classNamesPtr->statsUpToDate = FALSE;
																					
			}	// end "if (classNamesPtr->numberTrainPixels <= 0)"
			
		if (!maxValuesReadFlag)															
			classNamesPtr->statsUpToDate = FALSE;
		
				// Get the number of training pixels. After the entire project file has
				// been read, the number of training pixels used for calculating the
				// statistics will be "classNamesPtr->numberStatisticsPixels". At this
				// point we must assume that the number is "classNamesPtr->numberTrainPixels".
				// If this is not correct, the the situation will be corrected in the
				// "VerifyClassAndFieldParameters" routine which is called at the end
				// of the "ReadProjectFile" routine.
				
		numberPixels = classNamesPtr->numberTrainPixels;
		statsUpToDate = classNamesPtr->statsUpToDate;
			
		}	// end "else classFieldCode == 2" 
		
	if (statsUpToDate && chanStatsPtr != NULL)
		{
					// Initialize those vectors that were not loaded if memory	
					// has been allocated.													
					
		//if (!(statsLoadedCode & 0x0001))
		//	ComputeMeanVector (chanStatsPtr, numberStatisticsChannels, numberPixels);
			
		if (!(statsLoadedCode & 0x0002))
			ComputeSumVector (chanStatsPtr, numberStatisticsChannels, numberPixels);
			
		//if (!(statsLoadedCode & 0x0004))
		//	InitializeChannelMinimums (chanStatsPtr, numberStatisticsChannels);
		
		//if (!(statsLoadedCode & 0x0008))
		//	InitializeChannelMaximums (chanStatsPtr, numberStatisticsChannels);
		
		if (!(statsLoadedCode & 0x0020))
			ComputeSumSquaresMatrix (numberStatisticsChannels, 
												chanStatsPtr, 
												sumSquaresStatsPtr, 
												numberPixels);

				// Compute the field standard deviation vector if 						
				// statistics exist for the field.											
		
		ComputeMeanStdDevVector (chanStatsPtr, 
											sumSquaresStatsPtr,
											numberStatisticsChannels, 
											numberPixels,
											gProjectInfoPtr->statisticsCode,
											kTriangleInputMatrix);
		
		}	// end "if (statsUpToDate && chanStatsPtr != NULL)" 
		
	*inputStringPtrPtr = inputStringPtr;
	
	return (0);
	
}	// end "ReadStatistics"			



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SetupForStatisticsRead
//
//	Software purpose:	The purpose of this routine is to set up the parameter instruction
//                   blocks to read the statistics information correctly depending on
//							whether the date are listed as ascii characters or in binary. 
//							Note that pre PowerPC processors are not being handled. I.e. 80
//							bit doubles.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2006
//	Revised By:			Larry L. Biehl			Date: 03/14/2012	

SInt16 SetupForStatisticsRead (
				ParmBlkPtr							paramBlockPtr,
				UCharPtr								inputStringPtr,
				UInt32								numberEndOfLineBytes,
				UInt32								numberChannels,
				SInt16								formatArchitectureCode,
				SInt32*								savedIoPosModePtr, 
				SInt32*								savedIoReqCountPtr,
				Boolean*								swapBytesFlagPtr)

{	
	UInt32								numberValueBytes = 8;
	
	//SInt16								returnCode;
	
	
			// Save the position mode and the requested count.	
				
	#if defined multispec_mac 
		*savedIoPosModePtr = paramBlockPtr->ioParam.ioPosMode;
		*savedIoReqCountPtr = paramBlockPtr->ioParam.ioReqCount;
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_lin	
		*savedIoPosModePtr = paramBlockPtr->ioPosMode;
		*savedIoReqCountPtr = paramBlockPtr->ioReqCount;
	#endif	// defined multispec_win || defined multispec_lin
				
			// Read the last line again with different parameters so that the		
			// binary data will be handled properly.										
			
	#if defined multispec_mac 
		//paramBlockPtr->ioParam.ioPosOffset -= paramBlockPtr->ioParam.ioActCount;
		paramBlockPtr->ioParam.ioPosMode = fsFromStart;
		
				// Set count to read vector of channels bytes to the project file.								
				
		paramBlockPtr->ioParam.ioReqCount = 
				3 + numberEndOfLineBytes + numberChannels * numberValueBytes;
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_lin	
		if (paramBlockPtr->ioPosMode == kNewLinePosMode ||
								paramBlockPtr->ioPosMode == kIBMNewLinePosMode)
			{
					// Find the start of the next line.
								
			inputStringPtr = SkipToNextCarriageReturn (inputStringPtr);
		
			if (inputStringPtr != NULL && *inputStringPtr == kCarriageReturn)
				{
						// This is special case when files are copied from Windows to
						// Unix to Mac (and vice versa) and some of the transfers are
						// done as binary and some as text. One can end up with a double
						// carriage return. We need to read another line.
			
				inputStringPtr++;
			
				}	// end "if (inputStringPtr != NULL && *inputStringPtr == ..."	

			}	// end "if (paramBlockPtr->ioPosMode == kNewLinePosMode || ..."
		
		UInt32 numberCharsToReread = (UInt32)(paramBlockPtr->ioActCount - 
												(inputStringPtr - paramBlockPtr->ioBuffer));							
				
		paramBlockPtr->ioPosOffset -= numberCharsToReread;
		paramBlockPtr->ioPosMode = fsFromStart;
		
				// Set count to read vector of channels bytes to the project file.								
				
		paramBlockPtr->ioReqCount = 
				3 + numberEndOfLineBytes + numberChannels * numberValueBytes;
	#endif	// defined multispec_win || defined multispec_lin
	/*
	returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);
	
	if (returnCode != noErr)	
		{
		FinishUpStatisticsRead (paramBlockPtr,
										*savedIoPosModePtr,
										*savedIoReqCountPtr);
																							return (32);
																							
		}	// end "if (returnCode != noErr)"
	*/
			// Set up swap bytes flag depending upon the system
			// architecture
	                        
	if (formatArchitectureCode == kArchitectureNotKnown)
		{	        
				// This is a project file written before 7/22/2011, use the 
				// number of end of line bytes to set swap bytes flag. May
				// not be correct for all cases.
				
		if (numberEndOfLineBytes == 2) 
			*swapBytesFlagPtr = gBigEndianFlag;
		
		else	// numberEndOfLineBytes != 2              
			*swapBytesFlagPtr = !gBigEndianFlag;
			
		}	// end "if (formatArchitectureCode == kArchitectureNotKnown)"
		
	else if (formatArchitectureCode == kMacintoshMotorola)
		*swapBytesFlagPtr = !gBigEndianFlag;
		
	else // formatArchitectureCode == kMacintoshIntel || 
			//		formatArchitectureCode == kWindowsIntel)
		*swapBytesFlagPtr = gBigEndianFlag;
		
	gSwapBytesFlag = *swapBytesFlagPtr; 
	
	return (noErr);
	
}	// end "SetupForStatisticsRead"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FinishUpStatisticsRead
//
//	Software purpose:	The purpose of this routine is to finish up after reading
//							binary statistics data in. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2006
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void FinishUpStatisticsRead (
				ParmBlkPtr							paramBlockPtr,
				UCharPtr*							inputStringPtrPtr,
				SInt32								savedIoPosMode, 
				SInt32								savedIoReqCount)

{	
	SInt16								returnCode;
	
	
	gSwapBytesFlag = FALSE;
	
			// Set reading back to 'New Line' mode and reread the next line to		
			// be worked on.																		
			
	#if defined multispec_mac 
		paramBlockPtr->ioParam.ioPosOffset -= paramBlockPtr->ioParam.ioActCount;
		paramBlockPtr->ioParam.ioPosMode = savedIoPosMode;
		paramBlockPtr->ioParam.ioReqCount = savedIoReqCount;
	#endif	// defined multispec_mac 

   #if defined multispec_win || defined multispec_lin
		UInt32 numberCharsToReread = (UInt32)(paramBlockPtr->ioActCount - 
										(*inputStringPtrPtr - paramBlockPtr->ioBuffer));							
				
		paramBlockPtr->ioPosOffset -= numberCharsToReread;
		paramBlockPtr->ioReqCount = savedIoReqCount;

				// Force the current line to be read again.

		*inputStringPtrPtr = 0;
	#endif	// defined multispec_win || defined multispec_lin
	                                             
	returnCode = GetNextLine (paramBlockPtr, inputStringPtrPtr);

	#if defined multispec_win || defined multispec_lin
		paramBlockPtr->ioPosMode = (SInt16)savedIoPosMode;
	#endif	// defined multispec_win || defined multispec_lin
	
}	// end "FinishUpStatisticsRead"	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		char* ReadStringFromFile
//
//	Software purpose:	The purpose of this routine is to read a string
//							of given length from the input string. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1988
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

UCharPtr ReadStringFromFile (
				UCharPtr								inputStringPtr, 
				UCharPtr								outputStringPtr, 
				SInt16								numberSkipTabs,
				SInt16								maxStringLength)

{
	int									returnCode;
	SInt16								strLength;
	
	
   strLength = 0;
	if (numberSkipTabs > 0)
		inputStringPtr = SkipNTabs (inputStringPtr, numberSkipTabs);
	
	
			// Get the string length.															
	
	if (inputStringPtr != NULL)
		returnCode = sscanf ((char*)inputStringPtr, "%hd", &strLength);
	
	inputStringPtr = SkipNTabs (inputStringPtr, 1);
	
	if (strLength > 255 || inputStringPtr == NULL)
		strLength = 0;
	
			// Load count into first position in character string to create		
			// pascal type string.																
	
	outputStringPtr[0] = (UInt8)strLength;
	
	BlockMoveData (inputStringPtr, &outputStringPtr[1], (SInt32)strLength);
	
	inputStringPtr += strLength;
	
	if (strLength < maxStringLength)
		outputStringPtr[strLength+1] = 0;
	
	return (inputStringPtr); 

}	// end "ReadStringFromFile"


                  
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 SaveProjectFile
//
//	Software purpose:	The purpose of this routine is to save the project
//							file informaton.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClusterDialog in menus.c
//							CloseTheProject in menus.c
//							Menus in menus.c
//							CreateNewProject in project.c
//							ProjectMenuClearStatistics in projectUtilities.c
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1988
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

SInt16 SaveProjectFile (
				SInt16								saveCode)

{
	CMFileStream*						projectFileStreamPtr;
	
	GrafPtr								savedPort;
	
	SInt16								itemHit,
											returnCode = 0;
	
	
	projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
	
			// If "saveCode" is 2, then present a dialog box to ask user if		
			// he/she wants to save the project file.	 If return code is 1, 		
			// save the project file; if return code is 2, cancel operation; and	
			// if returnCode is 3, do not save the project.								
	
	if (saveCode == 2)
		{
		gTextString3[0] = 0;
		
		if (FileExists (projectFileStreamPtr))
			GetCopyOfPFileNameFromProjectInfo (
												gProjectInfoPtr, (FileStringPtr)gTextString3);
		
		if (gTextString3[0] == 0)
			CopyPToP (gTextString3, (UCharPtr)"\0Untitled Project\0");
			
		SInt16 stringID = IDS_Alert38;
		if (gProcessorCode == kClusterProcessor)
			stringID = IDS_Alert37;		//  changes before creating new project?
		
		itemHit = 2;						
		if (LoadSpecifiedStringNumberStringP (kAlertStrID,
															stringID, 
															(char*)gTextString,
															(char*)gTextString2,
															TRUE,
															(char*)&gTextString3[1]))
			itemHit = DisplayAlert (kSaveAlertID, kStopAlert, 0, 0, 0, gTextString);
		
		if (itemHit == 1)  
			saveCode = 0;
		else if (itemHit == 2)  
			returnCode = 1;
		else if (itemHit == 3)  
			returnCode = 0;
		
		}	// end "if (saveCode == 2)" 
	
			// Continue if return code is 1.													
			
	if (saveCode <= 1)
		{		
		returnCode = -1;
		
				// Make certain that we have the file name for the project.			
				
		if (GetProjectFileName (saveCode))
			{
					// Get status information dialog box as long as we are not	
					// saving due to a memory limit.										
				
			if (gGetMemoryTries == 0)	
				gStatusDialogPtr = GetStatusDialog (
												kGraphicStatusDialogID, FALSE);
		
			GetPort (&savedPort);
			
			SignedByte handleStatus = MHGetState (gProjectInfoPtr->windowInfoHandle);
			MHLock (gProjectInfoPtr->windowInfoHandle);
			returnCode = WriteProjectFile (saveCode);
			MHSetState (gProjectInfoPtr->windowInfoHandle, handleStatus);
			
			SetPort (savedPort);
			
					// Dispose of status box.												
			
			CloseStatusDialog (TRUE);
					
			if (returnCode != 0)
				{
				if (returnCode > 0)
					{		
					if (LoadSpecifiedStringNumberLongP (kAlertStrID,
																	IDS_Alert39, 
																	(char*)gTextString,
																	(char*)gTextString2,
																	TRUE,
																	(SInt32)returnCode,
																	0))
						returnCode = DisplayAlert (
												kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
				
					}	// end "if (returnCode > 0)" 
				
				}	// end "if (returnCode != 0)" 
			
			else	// returnCode == 0 
				{
				gProjectInfoPtr->changedFlag = FALSE;
		
						// Force project name to be rewritten	

				WriteProjectName ();		
				
				}	// end "else returnCode == 0" 
		 				
			CloseFile (gProjectInfoPtr);
			
			}	// end "if (GetProjectFileName (saveCode))" 
			
		}	// end "if (saveCode <= 1)" 
		
			// Update menu item.																	
	
	gUpdateProjectMenuItemsFlag = TRUE;
	
	return (returnCode);

}	// end "SaveProjectFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetupClassFieldPointMemory
//
//	Software purpose:	The purpose of this routine is to update the 
//							memory allocated for the project class, field, and 
//							point information if needed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/29/198
//	Revised By:			Larry L. Biehl			Date: 12/16/1993	

Boolean SetupClassFieldPointMemory (void)

{
	UInt32								bytesNeeded;
	
	Boolean								changedFlag,
											continueFlag;
	
	
	continueFlag = TRUE;
	
				// Change size of handle for class information if needed.			
						
	bytesNeeded = (UInt32)gProjectInfoPtr->numberStatisticsClasses *
																		sizeof (ProjectClassNames);
	gProjectInfoPtr->classNamesPtr = (HPClassNamesPtr)CheckHandleSize (
																&gProjectInfoPtr->classNamesHandle,
																&continueFlag, 
																&changedFlag, 
																bytesNeeded);
	
	if (continueFlag)
		{
				// Change size of handle for field names if needed.					
			
		bytesNeeded = (UInt32)gProjectInfoPtr->numberStatisticsFields *
																sizeof (ProjectFieldIdentifiers);
		gProjectInfoPtr->fieldIdentPtr = (HPFieldIdentifiersPtr)CheckHandleSize (
														&gProjectInfoPtr->fieldIdentifiersHandle,
														&continueFlag,  
														&changedFlag, 
														bytesNeeded);
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Change size of handle for field coordinates if needed.			
			
		bytesNeeded = (UInt32)gProjectInfoPtr->numberTotalPoints *
																	sizeof (ProjectFieldPoints);
		gProjectInfoPtr->fieldPointsPtr = (HPFieldPointsPtr)CheckHandleSize (
														&gProjectInfoPtr->fieldCoordinatesHandle,
														&continueFlag, 
														&changedFlag, 
														bytesNeeded);
		
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
		
}	// end "SetupClassFieldPointMemory" 



//------------------------------------------------------------------------------------
//								 	Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HPtr SkipNTabs
//
//	Software purpose:	This routine skips through the input count
//							of tabs in the input string and returns the
//							character pointer to the next character after
//							input tabs have been skipped.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	The new input string pointer.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/17/1992
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

UCharPtr SkipNTabs (
				UCharPtr								inputStringPtr, 
				SInt16								numberTabs)

{
	SInt16								count;
	
	
	if (inputStringPtr != NULL)
		{
		count = 0;
		
		while (count < numberTabs && *inputStringPtr != '\n')
			{
			if (*inputStringPtr == '\t')
				count++;
			inputStringPtr++;
			
			}	// end "while (count < numberTabs && ..." 
			
		if (count != numberTabs)
			inputStringPtr = NULL;
			
		}	// end "if (inputStringPtr != NULL)" 
	
	return (inputStringPtr);
		
}	// end "SkipNTabs" 


                  
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateGraphicStatusBox
//
//	Software purpose:	The purpose of this routine is to update the graphic
//							status line if it is displayed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			SaveProjectFile in project.c
//
//	Coded By:			Larry L. Biehl			Date: 02/21/1992
//	Revised By:			Larry L. Biehl			Date: 01/28/2001	

void UpdateGraphicStatusBox (
				double*								rightBoxPtr,
				double								boxIncrement)

{	
	#if defined multispec_mac
		Pattern								black;
	#endif	// defined multispec_mac    
	
	
	if (gStatusDialogPtr != NULL)
		{                  
		*rightBoxPtr += boxIncrement;
		gStatusGraphicsBox.right = (SInt16)*rightBoxPtr;
		
		#if defined multispec_mac
			if (gStatusProgressControlHandle != NULL)		
				SetControlValue (gStatusProgressControlHandle, 
										gStatusGraphicsBox.right);
			
			else	// gStatusProgressControlHandle == NULL
				FillRect (&gStatusGraphicsBox, GetQDGlobalsBlack (&black));
			/*
			RGBColor color;
			color.red = 0xCCCC;
			color.green = 0xCCCC;
			color.blue = 0xFFFF;
			RGBForeColor (&color);
			PaintRect (&gStatusGraphicsBox);
			*/
		#endif	// defined multispec_mac    
		
		}	// end "if (gStatusDialogPtr != NULL)" 

}	// end "UpdateGraphicStatusBox" 

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void VerifyClassAndFieldParameters
//
//	Software purpose:	The purpose of this routine is to check the class and field
//							parameters to verify that they make sense. This routine called
//							after reading a project file in.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			ReadProjectFile in SProjFIO.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1999
//	Revised By:			Larry L. Biehl			Date: 08/23/2010

void VerifyClassAndFieldParameters (
				Boolean								differentFileSourceFlag)

{ 
	SInt64								previousNumberTrainPixels,
											savedNumberPixels;
	
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt32								fieldNumber;
	
	UInt32								classNumber,
											classStorage,
											numberClasses,
											numberOfTestFields,
											numberOfTrainFields;
	
	SInt16								returnCode;
	
	Boolean								classTrainPixelsDifferedFlag;
											
	
			// Initialize local variables.
	
	returnCode = 0;
	
	gProjectInfoPtr->statsLoaded = FALSE;											
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Make sure that there is a default setting for the type of statistics to
			// save.
		
	if (gProjectInfoPtr->keepClassStatsOnlyFlag)
		{
		if (!gProjectInfoPtr->saveClassSumsSquaresFlag && 
														!gProjectInfoPtr->saveClassMeansCovFlag)
			gProjectInfoPtr->saveClassSumsSquaresFlag = TRUE;
	
		}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)" 
		
	else	// !gProjectInfoPtr->keepClassStatsOnlyFlag
		{
		if (!gProjectInfoPtr->saveFieldSumsSquaresFlag && 
														!gProjectInfoPtr->saveFieldMeansCovFlag)
			gProjectInfoPtr->saveFieldSumsSquaresFlag = TRUE;
	
		}	// end "else !gProjectInfoPtr->keepClassStatsOnlyFlag" 
	
			// Continue only if number of classes is one or more.						
	
	if (numberClasses > 0)
		{		
		for (classNumber=0; classNumber<numberClasses; classNumber++)
			{					
			numberOfTrainFields = 0;
			numberOfTestFields = 0;
			
					// Get the class storage number.													
								
			classStorage = gProjectInfoPtr->storageClass[classNumber];
			classNamesPtr = &gProjectInfoPtr->classNamesPtr[classStorage];
			
			fieldNumber = classNamesPtr->firstFieldNumber;
			
			while (fieldNumber != -1)
				{
				fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
				
						// Get the number of pixels in the field if needed. Alway double 
						// check the count for polygon fields in case a project file is being
						// transferred back and forth between Mac and Windows machines.
						// The pixels used in a polygon in the Windows version may be
						// different than the pixels used in a polygon in the Mac version.
				
				savedNumberPixels = fieldIdentPtr->numberPixels;
				
				if (fieldIdentPtr->pointType < kClusterType)
					fieldIdentPtr->numberPixels = 
												GetTotalNumberPixelsInField ((SInt16)fieldNumber);		
						
				if (fieldIdentPtr->fieldType == kTrainingType)
					{
					numberOfTrainFields++;
					if ((fieldIdentPtr->pointType == kPolygonType && 
																differentFileSourceFlag) ||
											(savedNumberPixels != fieldIdentPtr->numberPixels))
						{
						gProjectInfoPtr->statsUpToDate = FALSE;
						classNamesPtr->statsUpToDate = FALSE;
						fieldIdentPtr->statsUpToDate = FALSE;
						fieldIdentPtr->loadedIntoClassStats = FALSE;
						
						}	// end "if (fieldIdentPtr->pointType == kPolygonType && ..."
					
							// If the statistics are up to date and they have been loaded
							// into the class then indicate that some statistics have been 
							//	loaded for the project.	
							
					if (gProjectInfoPtr->keepClassStatsOnlyFlag)
						fieldIdentPtr->statsUpToDate = FALSE;	
						
					if (!fieldIdentPtr->statsUpToDate && 
															!gProjectInfoPtr->keepClassStatsOnlyFlag)
						{
						gProjectInfoPtr->statsUpToDate = FALSE;
						classNamesPtr->statsUpToDate = FALSE;
						fieldIdentPtr->loadedIntoClassStats = FALSE;
										
						}	// end "else !fieldIdentPtr->statsUpToDate && ..."
					
					}	// end "if (fieldIdentPtr->fieldType == kTrainingType)" 
					
				else if (fieldIdentPtr->fieldType == kTestingType)
					{
					numberOfTestFields++;
												
					fieldIdentPtr->loadedIntoClassStats = FALSE;
					fieldIdentPtr->statsUpToDate = FALSE;
					fieldIdentPtr->trainingStatsNumber = -1;
					
					}	// end "else if (fieldIdentPtr->fieldType == kTestingType)"
					
				if (fieldIdentPtr->pointType >= kClusterType)
					{
					fieldIdentPtr->numberOfPolygonPoints = 0;
					fieldIdentPtr->firstPolygonPoint = -1;
					fieldIdentPtr->labelPoint.h = 1;
					fieldIdentPtr->labelPoint.v = 1;
					
					}	// end "if (fieldIdentPtr->pointType >= kClusterType)"
					
				fieldNumber = fieldIdentPtr->nextField;
				
				}	// end "while (fieldNumber != -1)"

			if (numberOfTrainFields != (UInt32)classNamesPtr->numberOfTrainFields)
				classNamesPtr->statsUpToDate = FALSE;
				
			classNamesPtr->numberOfTrainFields = (SInt16)numberOfTrainFields;
			classNamesPtr->numberOfTestFields = (SInt16)numberOfTestFields;
			classNamesPtr->numberOfFields = 
											(SInt16)(numberOfTrainFields + numberOfTestFields);
								
					// Verify that the count of the number of pixels in the class
					// is correct.
			
			classNamesPtr->numberStatisticsPixels =  
					GetNumberOfPixelsLoadedInClass (classNamesPtr, 
																gProjectInfoPtr->fieldIdentPtr);
			
			previousNumberTrainPixels = classNamesPtr->numberTrainPixels;		
			classNamesPtr->numberTrainPixels =  
					GetNumberOfTrainPixelsInClass (classNamesPtr, 
																gProjectInfoPtr->fieldIdentPtr);
			
			classTrainPixelsDifferedFlag = FALSE;													
			if (previousNumberTrainPixels != classNamesPtr->numberTrainPixels)
				{
				classNamesPtr->statsUpToDate = FALSE;
				classTrainPixelsDifferedFlag = TRUE;
				
				}	// end "if (previousNumberTrainPixels != ..."
			
			if (!classNamesPtr->statsUpToDate && 
														classNamesPtr->numberOfTrainFields > 0)
				gProjectInfoPtr->statsUpToDate = FALSE;
				
			if (classNamesPtr->statsUpToDate == FALSE)
				{
						// Make sure that the field stats correctly reflect the fact
						// that the class stats are not up to date. This flag may
						// have been changed above.
						
				fieldNumber = classNamesPtr->firstFieldNumber;
				
				while (fieldNumber != -1)
					{
					fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];	
							
					if (fieldIdentPtr->fieldType == kTrainingType)
						{
								// If the class statistics are not up to date, make sure that
								// the loadedIntoClassStats flag for the field is set to FALSE.	
								
						if (gProjectInfoPtr->keepClassStatsOnlyFlag)
							fieldIdentPtr->loadedIntoClassStats = FALSE;
						
						}	// end "if (fieldIdentPtr->fieldType == kTrainingType)" 
						
					fieldNumber = fieldIdentPtr->nextField;
					
					}	// end "while (fieldNumber != -1)"
					
				if (classTrainPixelsDifferedFlag)
					classNamesPtr->looCovarianceValue = -1;
					
				}	// end "if (classNamesPtr->statsUpToDate == FALSE)"
				
					// Set the project statisticsLoaded flag.
						
			fieldNumber = classNamesPtr->firstFieldNumber;
			while (fieldNumber != -1)
				{
				fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];	
						
				if (fieldIdentPtr->fieldType == kTrainingType)
					{
					if (fieldIdentPtr->loadedIntoClassStats)
						gProjectInfoPtr->statsLoaded = TRUE;
					
					}	// end "if (fieldIdentPtr->fieldType == kTrainingType)" 
					
				fieldNumber = fieldIdentPtr->nextField;
				
				}	// end "while (fieldNumber != -1)"
			
			}	// end "for (classNumber=0; ... 
			
				// Verify that the statistics are consistant.
						
		SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
			
		}	// end "if (numberClasses > 0)"
	
}	// end "VerifyClassAndFieldParameters"  


                  
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteChannelInformation
//
//	Software purpose:	The purpose of this routine is to write channel
//							information to the project file for the selected
//							class or field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1988
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

Boolean WriteChannelInformation (
				HChannelStatisticsPtr			chanStatsPtr, 
				CMFileStream* 						fileStreamPtr,
				SInt16								numberStatisticsChannels, 
				UInt32								type, 
				UCharPtr								stringPtr)

{
	UInt32								count;
	
	SInt16								channel,
											errCode = noErr,
											stringLength = 0; 
											
	Boolean								continueFlag;
									
	              
   #if defined multispec_win || defined multispec_lin 	
		      // Get string length. Have to do this for Windows and Linux versions.
		      
		stringLength = 2; 
	#endif	// defined multispec_win || defined multispec_lin
	 
			// Write card identifier.															
				
	continueFlag = OutputString (fileStreamPtr, 
											(char*)stringPtr,
											stringLength, 
											gOutputCode, 
											TRUE);
	
	count = sizeof (double);	
	
	switch (type)
		{
		case 1:
					// If type is 1, write channel means.	
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				sprintf ((char*)gTextString, 
							"\t%.9g",
							chanStatsPtr->mean);
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gTextString,
														0, 
														gOutputCode, 
														continueFlag);
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
				
		case 2:				
					// If type is 2, write channel sums.
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				sprintf ((char*)gTextString, 
							"\t%.0f",
							chanStatsPtr->sum);
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gTextString,
														0, 
														gOutputCode, 
														continueFlag);
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
				
		case 3:			
					// If type is 3, write channel minimums.	
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				sprintf ((char*)gTextString, 
							"\t%.0f",
							chanStatsPtr->minimum);
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gTextString,
														0, 
														gOutputCode, 
														continueFlag);
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..."
			break;
				
		case 4:				
					// If type is 4, write channel maximums.
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				sprintf ((char*)gTextString, 
							"\t%.0f",
							chanStatsPtr->maximum);
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gTextString,
														0, 
														gOutputCode, 
														continueFlag);
				
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
			
		case 11:
					// If type is 11, write channel means.
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													&chanStatsPtr->mean, 
													kErrorMessages);
							
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
				
		case 12:				
					// If type is 12, write channel sums.
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													&chanStatsPtr->sum, 
													kErrorMessages);
							
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
				
		case 13:			
					// If type is 13, write channel minimums.	
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													&chanStatsPtr->minimum, 
													kErrorMessages);
							
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..."
			break;
				
		case 14:				
					// If type is 4, write channel maximums.
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													&chanStatsPtr->maximum, 
													kErrorMessages);
							
				chanStatsPtr++;
			
				}	// end "for (channel=0; channel<..." 
			break;
		
		}	// end "switch (type)"
					
	if (errCode != noErr)
		continueFlag = FALSE;
		
			// Put a carriage return at the end of the line.							
				
	continueFlag = OutputString (fileStreamPtr, 
											(char*)gEndOfLine,
											gNumberOfEndOfLineCharacters, 
											gOutputCode, 
											continueFlag);
	
	return (continueFlag);
	
}	// end "WriteChannelInformation" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteCovarianceInformation
//
//	Software purpose:	The purpose of this routine is to write covariance
//							information to the project file for the selected
//							class or field. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1988
//	Revised By:			Larry L. Biehl			Date: 04/24/2013	

Boolean WriteCovarianceInformation (
				HCovarianceStatisticsPtr		matrixStatsPtr, 
				HChannelStatisticsPtr			channelStatsPtr,
				SInt64								numberPixelsUsedForStats, 
				CMFileStream* 						fileStreamPtr,
				SInt16								numberStatisticsChannels, 
				UInt32								type, 
				UCharPtr								stringPtr, 
				double*								boxRightPtr, 
				double								boxIncrement)

{
	SInt64								numberPixelsLessOne;
	
	double								channelMean,
											covariance;

	HChannelStatisticsPtr			lChannelStatsPtr1,
											lChannelStatsPtr2;
											
	UInt32								count,
											numberBytes;
	
	SInt16								channel,
											covChan,
											errCode = noErr,
											stringLength = 0;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE; 	
	              
   #if defined multispec_win || defined multispec_lin
		      // Get string length. Have to do this for Windows and Linux versions.
		      
		stringLength = 3; 
	#endif	// defined multispec_win || defined multispec_lin
			
	count = sizeof (double);
	numberPixelsLessOne = numberPixelsUsedForStats - 1;
	
	switch (type)
		{
		case 1:
					// If type is 1, write covariance matrix.													
			lChannelStatsPtr1 = channelStatsPtr;
			
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				continueFlag = OutputString (fileStreamPtr, 
														(char*)stringPtr, 
														stringLength, 
														gOutputCode, 
														continueFlag);
				
				lChannelStatsPtr2 = channelStatsPtr;
					
				if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
					{
					channel = numberStatisticsChannels - 1;
					
					for (covChan=0; covChan<=channel; covChan++)
						{
						covariance = (*matrixStatsPtr - 
								(lChannelStatsPtr2->mean * lChannelStatsPtr2->sum)) /
																				numberPixelsLessOne;
																					
						sprintf ((char*)gTextString, 
									"\t%.9g",
									covariance);
							
						continueFlag = OutputString (fileStreamPtr, 
																(char*)gTextString,
																0, 
																gOutputCode, 
																continueFlag);
						
						matrixStatsPtr++;
						lChannelStatsPtr2++;
						
						}	// end "for (covChan=0; covChan<=channel; covChan++)" 
					
					}	// end "if (...->statisticsCode == kMeanStdDevOnly)" 
				
				else	// gProjectInfoPtr->statisticsCode != kMeanStdDevOnly 
					{
					channelMean = lChannelStatsPtr1->mean;
				
					for (covChan=0; covChan<=channel; covChan++)
						{
						covariance = 
								(*matrixStatsPtr - (channelMean * lChannelStatsPtr2->sum)) /
																						numberPixelsLessOne;
																					
						sprintf ((char*)gTextString, 
									"\t%.9g",
									covariance);
							
						continueFlag = OutputString (fileStreamPtr, 
																(char*)gTextString,
																0, 
																gOutputCode, 
																continueFlag);
						
						matrixStatsPtr++;
						lChannelStatsPtr2++;
						
						}	// end "for (covChan=0; covChan<=channel; covChan++)" 
						
					}	// end "else ...->statisticsCode != kMeanStdDevOnly" 
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gEndOfLine,
														gNumberOfEndOfLineCharacters, 
														gOutputCode, 
														continueFlag);
				
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
				
				lChannelStatsPtr1++;
			
				}	// end "for (channel=0; channel<..." 
			break;
			
		case 2:			
					// If type is 2, write channel sum of squares.								
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				continueFlag = OutputString (fileStreamPtr, 
														(char*)stringPtr, 
														stringLength, 
														gOutputCode, 
														continueFlag);
				
				if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
					channel = numberStatisticsChannels - 1;
					
				for (covChan=0; covChan<=channel; covChan++)
					{															
					sprintf ((char*)gTextString, 
								"\t%.0f",
								*matrixStatsPtr);
						
					continueFlag = OutputString (fileStreamPtr, 
															(char*)gTextString,
															0, 
															gOutputCode, 
															continueFlag);
						
					matrixStatsPtr++;
					
					}	// end "for (covChan=0; covChan<=channel; covChan++)" 
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gEndOfLine,
														gNumberOfEndOfLineCharacters, 
														gOutputCode, 
														continueFlag);
				
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
			
				}	// end "for (channel=0; channel<gProjectInfoPtr->..." 
			break;
			
		case 11:
					// If type is 1, write covariance matrix.													
			lChannelStatsPtr1 = channelStatsPtr;
			
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				continueFlag = OutputString (fileStreamPtr, 
														(char*)stringPtr, 
														stringLength, 
														gOutputCode, 
														continueFlag);
				
				lChannelStatsPtr2 = channelStatsPtr;
					
				if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
					{
					channel = numberStatisticsChannels - 1;
					
					for (covChan=0; covChan<=channel; covChan++)
						{
						covariance = (*matrixStatsPtr - 
								(lChannelStatsPtr2->mean * lChannelStatsPtr2->sum)) /
																					numberPixelsLessOne;
																				
						if (errCode == noErr)
							errCode = MWriteData (fileStreamPtr, 
															&count, 
															matrixStatsPtr, 
															kErrorMessages);
						
						matrixStatsPtr++;
						lChannelStatsPtr2++;
						
						}	// end "for (covChan=0; covChan<=channel; covChan++)" 
					
					if (errCode != noErr)
						continueFlag = FALSE;
					
					}	// end "if (...->statisticsCode == kMeanStdDevOnly)" 
				
				else	// gProjectInfoPtr->statisticsCode != kMeanStdDevOnly 
					{
					channelMean = lChannelStatsPtr1->mean;
				
					for (covChan=0; covChan<=channel; covChan++)
						{
						covariance = 
								(*matrixStatsPtr - (channelMean * lChannelStatsPtr2->sum)) /
																						numberPixelsLessOne;
																				
						if (errCode == noErr)
							errCode = MWriteData (fileStreamPtr, 
															&count, 
															matrixStatsPtr, 
															kErrorMessages);
						
						matrixStatsPtr++;
						lChannelStatsPtr2++;
						
						}	// end "for (covChan=0; covChan<=channel; covChan++)" 
					
					if (errCode != noErr)
						continueFlag = FALSE;
						
					}	// end "else ...->statisticsCode != kMeanStdDevOnly" 
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gEndOfLine, 
														gNumberOfEndOfLineCharacters, 
														gOutputCode, 
														continueFlag);
					
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
				
				lChannelStatsPtr1++;
			
				}	// end "for (channel=0; channel<..." 
			break;
			
		case 12:			
					// If type is 12, write channel sum of squares.								
			for (channel=0; channel<numberStatisticsChannels; channel++)
				{
				continueFlag = OutputString (fileStreamPtr, 
														(char*)stringPtr, 
														stringLength, 
														gOutputCode, 
														continueFlag);
				
				if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
					channel = numberStatisticsChannels - 1;
				
				numberBytes = count * (channel+1);
					
				if (errCode == noErr)
					errCode = MWriteData (fileStreamPtr, 
													&numberBytes, 
													matrixStatsPtr, 
													kErrorMessages);
									
				matrixStatsPtr += (channel+1);
						
				if (errCode != noErr)
					continueFlag = FALSE;
					
				continueFlag = OutputString (fileStreamPtr, 
														(char*)gEndOfLine,
														gNumberOfEndOfLineCharacters, 
														gOutputCode, 
														continueFlag);
				
				UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
			
				}	// end "for (channel=0; channel<gProjectInfoPtr->..." 
			break;
			
		}	// end "switch (type)"
		
	return (continueFlag);
	
}	// end "WriteCovarianceInformation" 


           
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean WriteModifiedStats
//
//	Software purpose:	The purpose of this routine is to write the modified
//							statistics for the given class to the project file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/14/1993
//	Revised By:			Larry L. Biehl			Date: 11/08/1995

Boolean WriteModifiedStats (
				CMFileStream* 						fileStreamPtr,
				SInt16		 						numberChannels,
				HChannelStatisticsPtr 			modifiedClassChanStatsPtr,
				HCovarianceStatisticsPtr 		modifiedClassCovStatsPtr)

{ 
	#if defined multispec_mac
		#ifndef _MC68881_ 
			//char								tempValue[12];
			//SInt32								j;
		#endif
	#endif	// defined multispec_mac 

	
	UInt32							count,
										stringIndex;
	
	OSErr 							errCode;
	SInt32							i; 
	
	
			// Write the modified means to the project file.							

	count = 4;

	#ifdef _MC68881_
		stringIndex = 49;
	#else	// not _MC68881_ 
		stringIndex = 57;
	#endif		// _MC68881_, else ...
		
	errCode = MWriteData (fileStreamPtr, 
									&count,
									&gTextString3[stringIndex],
									kErrorMessages);
	
	for (i=0; i<numberChannels; i++)
		{
		#if defined multispec_mac
			#ifdef _MC68881_ 
				count = k68881Bytes;
				if (errCode == noErr)		
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													&modifiedClassChanStatsPtr->mean, 
													kErrorMessages);
					
			#else	// not _MC68881_ 
				count = sizeof (double);
				//nonx96tox96 (&modifiedClassChanStatsPtr->mean,
				//					(extended96*)&tempValue[0]);
				
				if (errCode == noErr)		
					errCode = MWriteData (fileStreamPtr, 
													&count,
													&modifiedClassChanStatsPtr->mean, 
													kErrorMessages);
				
			#endif		// _MC68881_ 
		#endif	// defined multispec_mac 

      #if defined multispec_win || defined multispec_lin
			count = sizeof (double);
			
			if (errCode == noErr)		
				errCode = MWriteData (fileStreamPtr, 
												&count, 
												&modifiedClassChanStatsPtr->mean, 
												kErrorMessages);
		#endif	// defined multispec_win || defined multispec_lin
			
		modifiedClassChanStatsPtr++;
		
		}	// end "for (i=0; i<numberChannels; i++)" 
	
			// Add a carriage return to the end of this line.							
			
	count = gNumberOfEndOfLineCharacters;
	if (errCode == noErr)		
		errCode = MWriteData (fileStreamPtr, 
										&count, 
										gEndOfLine, 
										kErrorMessages);
	
	if (errCode == noErr)
		{
				// Write the modified covariance matrix to the project file.

		#ifdef _MC68881_
			stringIndex = 53;
		#else	// not _MC68881_
			stringIndex = 61;
		#endif		// _MC68881_, else ...
		
		for (i=1; i<=numberChannels; i++)
			{
			count = 4;	
			errCode = MWriteData (fileStreamPtr, 
											&count,
											&gTextString3[stringIndex],
											kErrorMessages);
			
			#if defined multispec_mac
				#ifdef _MC68881_ 
					count = i * k68881Bytes;
					if (errCode == noErr)			
						errCode = MWriteData (fileStreamPtr, 
														&count,
														modifiedClassCovStatsPtr, 
														kErrorMessages);
			
					modifiedClassCovStatsPtr += i;
			
				#else	// not _MC68881_
					/*
					count = k68881Bytes;
					for (j=0; j<i; j++)
						{
						nonx96tox96 (modifiedClassCovStatsPtr, 
											(extended96*)&tempValue[0]);
					
						if (errCode == noErr)			
							errCode = MWriteData (fileStreamPtr, 
															&count,
															&tempValue[0], 
															kErrorMessages);
 
						modifiedClassCovStatsPtr++;
							
						}	// end "for (j=0; j<i; j++)" 
					*/
					count = i * sizeof (double);
					if (errCode == noErr)			
						errCode = MWriteData (fileStreamPtr, 
														&count, 
														modifiedClassCovStatsPtr, 
														kErrorMessages);
				
					modifiedClassCovStatsPtr += i;
					
				#endif		// _MC68881_ 
			#endif	// defined multispec_mac 

         #if defined multispec_win || defined multispec_lin
				count = i * sizeof (double);
				if (errCode == noErr)			
					errCode = MWriteData (fileStreamPtr, 
													&count, 
													modifiedClassCovStatsPtr, 
													kErrorMessages);
			
				modifiedClassCovStatsPtr += i;
			#endif	// defined multispec_win || defined multispec_lin
		
					// Add a carriage return to the end of this line.					
				
			count = gNumberOfEndOfLineCharacters;
			if (errCode == noErr)		
				errCode = MWriteData (fileStreamPtr, 
												&count,
												gEndOfLine, 
												kErrorMessages);
			
			if (errCode != noErr)
				break;
			
			}	// end "for (i=1; i<=numberChannels; i++)" 
		
		}	// end "if (errCode == noErr)" 
		
	return (errCode == noErr);
	
}	// end "WriteModifiedStats"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void WriteProjectFile
//
//	Software purpose:	The purpose of this routine is to write the project
//							file informaton to disk.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	-1 = Operation was cancelled by the user.
//							0  = Save completed okay.
//							1  = File was not found.
//							2  = Error writing the project.
//										
// 
// Called By:			SaveProjectFile in project.c
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1998
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

SInt16 WriteProjectFile (
				SInt16								saveCode)

{
	double								fieldIncrement,
											fieldRight,
											lineIncrement,
											lineRight;
	
	SInt64								outLongLongTemp;
												
	FileStringPtr						maskFileNamePPointer,
											projectFileNamePtr;
											
	CMFileStream*						projectFileStreamPtr;
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	WindowInfoPtr						projectWindowInfoPtr;
	
	SInt32								numberChannels,
											numberCovarianceEntries;
											
	UInt32								binaryListingCode;
	
	SInt16								channel,
											classStorage,
											field,
											fieldCount,
											fieldStatsNumber,
											formatArchitectureCode,
											index,
											outTemp,
											outTemp2,
											point,
											returnCode,
											strLength;
	
	UInt16								classIndex;
	
	Boolean								continueFlag,
											modifiedStatsFlag;
											
	SignedByte							maskFileStreamHandleStatus;
	
	
			// Initialize local variables.													
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
	numberCovarianceEntries = gProjectInfoPtr->numberCovarianceEntries;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	gNextTime = TickCount ();
	gOutputCode = kAsciiFormatCode;
	gOutputForce1Code = (gOutputCode | 0x0001);
	continueFlag = TRUE;
	  
	#if defined multispec_mac
		formatArchitectureCode = kMacintoshIntel;
		if (gBigEndianFlag)
			formatArchitectureCode = kMacintoshMotorola;		
	#endif	// defined multispec_mac
	
	#if defined multispec_win || defined multispec_lin
		formatArchitectureCode = kWindowsIntel;
	#endif	// defined multispec_win

	projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
	
			// Note 'gProjectInfoPtr->windowInfoHandle' was locked in the			
			// calling routine.																	
			
	projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
	
			// Change cursor to watch cursor until done with process.				
				
	MSetCursor (kWait);
	
			// Show status dialog box if available.										
			
	if (gStatusDialogPtr)
		{
		gStatusBoxIncrement = 
							(double)(gStatusGraphicsBox.right - gStatusGraphicsBox.left);
		ShowDialogItem (gStatusDialogPtr, IDC_Status3);
		ShowDialogItem (gStatusDialogPtr, IDC_Status4);
		ShowDialogItem (gStatusDialogPtr, IDC_Status5);
		ShowDialogItem (gStatusDialogPtr, IDC_Status6);
		ShowDialogItem (gStatusDialogPtr, IDC_Status7);
		ShowDialogWindow (
					gStatusDialogPtr, kGraphicStatusDialogID, kDoNotSetUpDFilterTable);
		SetPortDialogPort (gStatusDialogPtr);
		DrawDialog (gStatusDialogPtr);
		
				// Get the number of class/field stat lines to write.					
				// returnCode is used here as a temporary variable.					
		
		returnCode = 0;
		if (gProjectInfoPtr->saveClassSumsSquaresFlag)
			returnCode += gProjectInfoPtr->numberStatisticsChannels + 1;
		if (gProjectInfoPtr->saveClassMeansCovFlag)
			returnCode += gProjectInfoPtr->numberStatisticsChannels + 1;
			
		if (gProjectInfoPtr->saveFieldSumsSquaresFlag)
			returnCode += gProjectInfoPtr->numberStatisticsChannels + 1;
		if (gProjectInfoPtr->saveFieldMeansCovFlag)
			returnCode += gProjectInfoPtr->numberStatisticsChannels + 1;
		
		fieldIncrement = gStatusBoxIncrement / gProjectInfoPtr->numberStatTrainFields;
		lineIncrement = fieldIncrement/returnCode;
		
		fieldRight = (double)gStatusGraphicsBox.left;
		lineRight = (double)gStatusGraphicsBox.left;
		
		LoadDItemStringNumber (kProjectStrID, IDS_Project29, 
											gStatusDialogPtr, IDC_Status2, (Str255*)gTextString);
		
		}	// end "if (gStatusDialogPtr)" 
		
			// Get a string that will be used later.										
			
	MGetString (gTextString3, kProjectStrID, IDS_Project25);
	
	if (FileOpen (projectFileStreamPtr))
		{
				// Write project file format version.										
		
		sprintf ((char*)gTextString, 
					//"PROJECT FORMAT VERSION 990107 %ld%s",
					//"PROJECT FORMAT VERSION 20060124 %ld%s",
					//"PROJECT FORMAT VERSION 20110722 %ld%s",
					"PROJECT FORMAT VERSION 20130424 %d%s",
					(int)gProjectInfoPtr->otherInformationPosition,
					gEndOfLine);
					
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString, 
												0, 
												gOutputCode, 
												continueFlag);
		
				// Write "P1" record.															
		
				// Make certain that there is a null character at the end of the	
				// project file name.					
				
		projectFileNamePtr =
				(FileStringPtr)GetFileNamePPointerFromFileStream (projectFileStreamPtr);

		//UInt16 stringLength = strlen ((char*)projectFileNamePtr);
		UInt16 stringLength = projectFileNamePtr[0];
		stringLength = MIN (stringLength, 254);
		
		sprintf ((char*)gTextString, 
						"P1 PROJECT NAME\t%hd\t%s",
						stringLength,
						&projectFileNamePtr[1]);
					
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString, 
												0, 
												gOutputCode, 
												continueFlag,
												kUTF8CharString);
		
				// Make certain that there is a null character at the end of the	
				// project image file name.													
		
		returnCode = MIN (gProjectInfoPtr->imageFileName[0], 254);
		gProjectInfoPtr->imageFileName[returnCode+1] = kNullTerminator;
		
		sprintf ((char*)gTextString, 
					"\t%hd\t%s%s",
					(short int)gProjectInfoPtr->imageFileName[0],
					&gProjectInfoPtr->imageFileName[1],
					gEndOfLine);
					
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString, 
												0, 
												gOutputCode, 
												continueFlag,
												kUTF8CharString);		
		
				// Write "P2" record.															
				
		sprintf ((char*)gTextString,
				#if defined multispec_mac || defined multispec_win || defined multispec_mac_swift
					"P2\t%ld\t%ld\t%hd\t%hd\t%ld\t%ld\t%hd\t%d\t%hd\t%ld\t%ld\t%ld\t%ld"
					"\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%d\t%hd\t%d\t%d\t%d\t%hd\t%hd\t%hd\t%hd%s",
				#endif
				#if defined multispec_lin
					"P2\t%ld\t%ld\t%hd\t%hd\t%d\t%d\t%hd\t%d\t%hd\t%ld\t%ld\t%ld\t%ld"
					"\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%hd\t%d\t%hd\t%d\t%d\t%d\t%hd\t%hd\t%hd\t%hd%s",
				#endif
					projectWindowInfoPtr->maxNumberLines,
					projectWindowInfoPtr->maxNumberColumns,
					projectWindowInfoPtr->totalNumberChannels,
					projectWindowInfoPtr->numberBytes,
					gProjectInfoPtr->startLine,
					gProjectInfoPtr->startColumn,
					projectWindowInfoPtr->numberBits,
					(int)(gProjectInfoPtr->swapBytesFlag),
					projectWindowInfoPtr->bandInterleave,
					gProjectInfoPtr->numberHeaderBytes,
					gProjectInfoPtr->numberTrailerBytes,
					gProjectInfoPtr->numberPreLineBytes,
					gProjectInfoPtr->numberPostLineBytes,
					gProjectInfoPtr->numberStatisticsClasses,
					gProjectInfoPtr->numberStatTrainClasses,
					gProjectInfoPtr->numberStatisticsFields,
					gProjectInfoPtr->numberStatTrainFields,
					gProjectInfoPtr->numberStatTestFields,
					gProjectInfoPtr->numberTotalPoints,
					gProjectInfoPtr->numberStatisticsChannels,
					(int)gProjectInfoPtr->statsUpToDate,
					gProjectInfoPtr->statisticsCode,
					(int)gProjectInfoPtr->keepClassStatsOnlyFlag,
					(int)gProjectInfoPtr->signedDataFlag,
					(int)gProjectInfoPtr->useCommonCovarianceInLOOCFlag,
					gProjectInfoPtr->covarianceStatsToUse,
					projectWindowInfoPtr->dataTypeCode,
					gProjectInfoPtr->hdfDataSetSelection,
					formatArchitectureCode,
					gEndOfLine);
					
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString, 
												0, 
												gOutputCode, 
												continueFlag);
		
				// Write "P3" record.															
					
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)"P3", 
												2, 
												gOutputCode, 
												continueFlag);
		
		for (channel=0;
				channel<gProjectInfoPtr->numberStatisticsChannels; 
					channel++)
			{
			sprintf ((char*)gTextString, 
						"\t%d",
						(int)(gProjectInfoPtr->channelsPtr[channel]+1));
					
			continueFlag = OutputString (projectFileStreamPtr,
													(char*)gTextString, 
													0, 
													gOutputCode, 
													continueFlag);
			
			}	// end "for (channel=0; channel<gProjectInfoPtr->..." 															
				
		sprintf ((char*)gTextString,
						"%s",
						gEndOfLine);
				
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString,
												0, 
												gOutputCode, 
												continueFlag);
		
				// Write "P4" record if there is a training mask file.	
				
		if (gProjectInfoPtr->trainingMask.maskHandle != NULL)
			{								
					// Get a pascal pointer to the training mask file.	
		
			maskFileNamePPointer = GetMaskFileNamePPointer (
																	&gProjectInfoPtr->trainingMask,
																	&maskFileStreamHandleStatus);
			
			sprintf ((char*)gTextString, 
						"P4 TRAINING MASK NAME\t%hd\t%s\t%ld%s",
						(SInt16)maskFileNamePPointer[0],
						&maskFileNamePPointer[1],
						gProjectInfoPtr->trainingMask.fileLayer,
						gEndOfLine);
								
			MHSetState (gProjectInfoPtr->trainingMask.fileStreamHandle,
								maskFileStreamHandleStatus);
						
			continueFlag = OutputString (projectFileStreamPtr,
													(char*)gTextString, 
													0, 
													gOutputCode, 
													continueFlag);
				
			}	// end "if (gProjectInfoPtr->trainingMask.maskHandle != NULL)"		
					
				// Write "P5" record if there is a test mask file.	
				
		if (gProjectInfoPtr->testMask.maskHandle != NULL)
			{		
					// Get a pascal pointer to the training mask file.	
		
			maskFileNamePPointer = GetMaskFileNamePPointer (&gProjectInfoPtr->testMask,
																			&maskFileStreamHandleStatus);
			
			sprintf ((char*)gTextString, 
						"P5 TEST MASK NAME\t%hd\t%s\t%ld%s",
						(SInt16)maskFileNamePPointer[0],
						&maskFileNamePPointer[1],
						gProjectInfoPtr->testMask.fileLayer,
						gEndOfLine);
								
			MHSetState (gProjectInfoPtr->testMask.fileStreamHandle,
								maskFileStreamHandleStatus);
						
			continueFlag = OutputString (projectFileStreamPtr,
													(char*)gTextString, 
													0, 
													gOutputCode, 
													continueFlag);
				
			}	// end "if (gProjectInfoPtr->testMask.maskHandle != NULL)"	
		
				// If the number of bytes of data is 4 or more then the statistics will
				// be listed as binary data not ascii number strings that can be easily
				// read.  This is done so as not to loose precision with these larger
				// numbers.
		
		binaryListingCode = 0;	
		if (projectWindowInfoPtr->numberBytes >= 4)
			binaryListingCode = 10;													
				
		sprintf ((char*)gTextString,
						"Training Statistics%s",
						gEndOfLine);
				
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString,
												0, 
												gOutputCode, 
												continueFlag);
		
		LoadDItemStringNumber (kProjectStrID,
										IDS_Project41,
										gStatusDialogPtr,
										IDC_Status2,
										(Str255*)gTextString);
			
					// Display status information.											
					
		LoadDItemValue (gStatusDialogPtr, IDC_Status4, (SInt32)1);
		LoadDItemValue (gStatusDialogPtr, 
								IDC_Status6, 
								(SInt32)gProjectInfoPtr->numberStatTrainFields);
		fieldCount = 1;
		returnCode = 1;
		
				// Write Class information to the file for training statistics.	
				
		for (classIndex=0; 
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
					classIndex++)
			{
					// Get the class storage number.											
					
			classStorage = gProjectInfoPtr->storageClass[classIndex];
					
					// Write "C1" record.														
					
					// Make class parameters make sense relative to the 				
					// statistics being written to the disk file.						
					
			outLongLongTemp = classNamesPtr[classStorage].numberTrainPixels;
			outTemp = classNamesPtr[classStorage].statsUpToDate;
			
			if (gProjectInfoPtr->keepClassStatsOnlyFlag)
				{
				if (!gProjectInfoPtr->saveClassMeansCovFlag &&
													!gProjectInfoPtr->saveClassSumsSquaresFlag)
					{
					//outLongLongTemp = 0;
					
					if (classNamesPtr[classStorage].numberOfTrainFields > 0)
						outTemp = 0;
					
					}	// end "if (!...->saveClassMeansCovFlag && ..." 
										
				}	// end "if (...->keepClassStatsOnlyFlag)" 
					
			pstr ((char*)gTextString2,
						(char*)classNamesPtr[classStorage].name, &strLength);
							
					// Make sure that the modifiedStatsFlag is consistant with the
					// the statsUpToDate flag.
					
			modifiedStatsFlag = classNamesPtr[classStorage].modifiedStatsFlag;
			if (!classNamesPtr[classStorage].statsUpToDate || 
									classNamesPtr[classStorage].numberStatisticsPixels <= 0)
				modifiedStatsFlag = FALSE;
							
			sprintf ((char*)gTextString, 
							"C1\t%d\t%s\t%hd\t%hd\t%hd\t%lld\t%hd\t%d\t%f\t%f\t%f\t%f"
									"\t%hd%s",
							(int)classNamesPtr[classStorage].name[0],
							gTextString2,
							classNamesPtr[classStorage].numberOfFields,
							classNamesPtr[classStorage].numberOfTrainFields,
							classNamesPtr[classStorage].numberOfTestFields,
							outLongLongTemp,		// numberTrainPixels 
							outTemp,					// statsUpToDate 
							(int)modifiedStatsFlag,
							classNamesPtr[classStorage].priorWeights[0],
							classNamesPtr[classStorage].priorWeights[1],
							classNamesPtr[classStorage].priorWeights[3],
							classNamesPtr[classStorage].looCovarianceValue,
							classNamesPtr[classStorage].covarianceStatsToUse,
							gEndOfLine);
						
			continueFlag = OutputString (projectFileStreamPtr,
													(char*)gTextString, 
													0, 
													gOutputCode, 
													continueFlag);
		
			if (classNamesPtr[classStorage].statsUpToDate && 
									classNamesPtr[classStorage].numberStatisticsPixels > 0)	
				{
				if (continueFlag && gProjectInfoPtr->classChanStatsPtr)
					continueFlag = WriteStatistics (
									&gProjectInfoPtr->classChanStatsPtr[
															classStorage*numberChannels],
									&gProjectInfoPtr->classSumSquaresStatsPtr[
															classStorage*numberCovarianceEntries],
									classNamesPtr[classStorage].numberStatisticsPixels, 
									projectFileStreamPtr, 
									gProjectInfoPtr->saveClassMeansCovFlag, 
									gProjectInfoPtr->saveClassSumsSquaresFlag,
									2,
									binaryListingCode,
									&lineRight,
									lineIncrement);
				
				if (continueFlag && classNamesPtr[classStorage].modifiedStatsFlag)
					{
							// Write "MCM" record.  "Modified class means."				
							// Write "MCC" record.  "Modified class covariances."		
				
					index = classNamesPtr[classStorage].modifiedStatsStorage;
					
					continueFlag = WriteModifiedStats (
												projectFileStreamPtr,
												(SInt16)numberChannels,
												&gProjectInfoPtr->modifiedClassChanStatsPtr[
																				index*numberChannels],
												&gProjectInfoPtr->modifiedClassCovStatsPtr[
																	index*numberCovarianceEntries]);
					
					}	// end "if (continueFlag && ..." 
					
				}	// end "if (classNamesPtr[classStorage].statsUpToDate...)" 
		
					// Write Field information to the file for training				
					// statistics.  Write to file in order of class.					
				
			field = classNamesPtr[classStorage].firstFieldNumber;
			while (field > -1 && returnCode > 0)
				{
						// Continue if field is a training field.  Otherwise go		
						// to next field.															
						
				if (fieldIdentPtr[field].fieldType == kTrainingType)
					{
							// Exit routine if user has "command period" down			
							
					if (TickCount () >= gNextTime)
						{
						if (!CheckSomeEvents (keyDownMask+mDownMask+mUpMask))
							{
							returnCode = -1;
							continueFlag = FALSE;
							break;
							
							}	// end "if (!CheckSomeEvents (keyDownMask+mDownMask))"
							
						}	// end "if (TickCount () >= nextTime)" 
					
					LoadDItemValue (gStatusDialogPtr, IDC_Status4, (SInt32)fieldCount);
					fieldCount++;
			
							// Get field statistics set number.								
		
					fieldStatsNumber = fieldIdentPtr[field].trainingStatsNumber;
					
							// Make 'loadedIntoClassStats' flag make sense relative	
							// to the statistics being written to the disk file.		
							
					outTemp = fieldIdentPtr[field].loadedIntoClassStats;
					outTemp2 = fieldIdentPtr[field].statsUpToDate;
					if (gProjectInfoPtr->keepClassStatsOnlyFlag)
						{
						if (!gProjectInfoPtr->saveClassMeansCovFlag &&
													!gProjectInfoPtr->saveClassSumsSquaresFlag)
							outTemp = 0;
						
						}	// end "if (...->keepClassStatsOnlyFlag)" 
						
					else	// !gProjectInfoPtr->keepClassStatsOnlyFlag 
						{
						if (!gProjectInfoPtr->saveFieldMeansCovFlag &&
											!gProjectInfoPtr->saveFieldSumsSquaresFlag)
							outTemp2 = 0;
						
						}	// end "if (...->keepClassStatsOnlyFlag)" 
										
							// Write "F1" record.  "Field summary."						
							
					pstr ((char*)gTextString2,
								(char*)fieldIdentPtr[field].name, 
								&strLength);
							
					sprintf ((char*)gTextString, 
								"F1\t%d\t%s\t%hd\t%d\t%hd\t%hd\t%lld\t%hd\t%hd\t%hd\t%lld%s",
								(int)fieldIdentPtr[field].name[0],
								gTextString2,
								fieldIdentPtr[field].numberOfPolygonPoints,
								(int)(classIndex+1),
								fieldIdentPtr[field].fieldType,
								fieldIdentPtr[field].pointType,
								fieldIdentPtr[field].numberPixels,
								outTemp,			// loadedIntoClassStats 
								outTemp2,		// statsUpToDate
								fieldIdentPtr[field].maskValue,
								fieldIdentPtr[field].numberPixelsUsedForStats,	// Added for version 20130424
								gEndOfLine); 
						
					continueFlag = OutputString (projectFileStreamPtr,
															(char*)gTextString, 
															0, 
															gOutputCode, 
															continueFlag);
		
					if (fieldIdentPtr[field].statsUpToDate)	
						{
						if (returnCode > 0)
							returnCode = WriteStatistics (
												&gProjectInfoPtr->fieldChanStatsPtr[
																fieldStatsNumber * numberChannels], 
												&gProjectInfoPtr->fieldSumSquaresStatsPtr[
													fieldStatsNumber * numberCovarianceEntries],
												fieldIdentPtr[field].numberPixelsUsedForStats, 
												projectFileStreamPtr, 
												gProjectInfoPtr->saveFieldMeansCovFlag, 
												gProjectInfoPtr->saveFieldSumsSquaresFlag,
												1,
												binaryListingCode,
												&lineRight, 
												lineIncrement);
						
						}	// end "if (fieldIdentPtr[field].statsUpToDate)" 
							
							// Write "V1" records.  "Coordinates of field."				
					
					index = fieldIdentPtr[field].firstPolygonPoint;
				
					if (fieldIdentPtr[field].pointType == kPolygonType)
						index += 2;
					
					for (point=0;
							point<fieldIdentPtr[field].numberOfPolygonPoints;
								point++)
						{
						sprintf ((char*)gTextString, 
									"V1\t%ld\t%ld%s",
									gProjectInfoPtr->fieldPointsPtr[index].line,
									gProjectInfoPtr->fieldPointsPtr[index].col,
									gEndOfLine);
						
						continueFlag = OutputString (projectFileStreamPtr,
																(char*)gTextString, 
																0, 
																gOutputCode, 
																continueFlag);
						
						index++;
						
						}	// end "for (point=0; point<fieldIdentPtr..." 
				
					UpdateGraphicStatusBox (&fieldRight, fieldIncrement);
					
							// Sync the progress bar indicator for statistics with
							// that being used for fields.
							
					lineRight = fieldRight;
						
					}	// end "if (fieldIdentPtr[field].fieldType..." 
					
				field = fieldIdentPtr[field].nextField;
				
				}	// end "while (field > -1 && returnCode > 0)" 
				
			if (!continueFlag)
				break;
					
			}	// end "for (classIndex=1; ..." 															
				
		sprintf ((char*)gTextString,
						"Test Fields%s",
						gEndOfLine);
				
		continueFlag = OutputString (projectFileStreamPtr,
												(char*)gTextString,
												0, 
												gOutputCode, 
												continueFlag);
		
				// If test fields exist, write the test field information to 		
				// disk in order of class.														
				
		for (classIndex=0; 
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
					classIndex++)
			{
					// Get the class storage number.											
					
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
			field = classNamesPtr[classStorage].firstFieldNumber;
			while (field > -1 && continueFlag)
				{
						// Continue if field is a test field.  Otherwise go to next field.
						
				if (fieldIdentPtr[field].fieldType == kTestingType)
					{			
							// Write "F1" record.  "Field summary."					
							
					pstr ((char*)gTextString2,
							(char*)fieldIdentPtr[field].name,
							&strLength);
							
					sprintf ((char*)gTextString, 
								"F1\t%d\t%s\t%hd\t%d\t%hd\t%hd\t%lld\t%hd\t%hd\t%hd\t%lld%s",
								(int)(fieldIdentPtr[field].name[0]),
								gTextString2,
								fieldIdentPtr[field].numberOfPolygonPoints,
								(int)(classIndex+1),
								fieldIdentPtr[field].fieldType,
								fieldIdentPtr[field].pointType,
								fieldIdentPtr[field].numberPixels,
								(short int)0,
								(short int)0,
								fieldIdentPtr[field].maskValue,
								fieldIdentPtr[field].numberPixelsUsedForStats,
								gEndOfLine);
						
					continueFlag = OutputString (projectFileStreamPtr,
															(char*)gTextString, 
															0, 
															gOutputCode, 
															continueFlag);
						
							// Write "V1" records.  "Coordinates of field."			
					
					index = fieldIdentPtr[field].firstPolygonPoint;
				
					if (fieldIdentPtr[field].pointType == kPolygonType)
						index += 2;
						
					for (point=0;
							point<fieldIdentPtr[field].numberOfPolygonPoints;
								point++)
						{
						sprintf ((char*)gTextString, 
									"V1\t%ld\t%ld%s",
									gProjectInfoPtr->fieldPointsPtr[index].line,
									gProjectInfoPtr->fieldPointsPtr[index].col,
									gEndOfLine);
						
						continueFlag = OutputString (projectFileStreamPtr,
																(char*)gTextString, 
																0, 
																gOutputCode, 
																continueFlag);
						
						index++;
						
						}	// end "for (point=0; point<fieldIdentPtr..." 
						
					}	// end "if (fieldIdentPtr[field].fieldType..." 
					
				field = fieldIdentPtr[field].nextField;
				
				}	// end "while (field > -1 && returnCode > 0)" 
				
			if (!continueFlag)
				break;
					
			}	// end "for (classIndex=1; gProjectInfoPtr->..." 
		
		if (gStatusDialogPtr != NULL)
			{
			LoadDItemStringNumber (kProjectStrID, 
											IDS_Project30, 
											gStatusDialogPtr, 
											IDC_Status2, 
											(Str255*)gTextString);
			
			HideDialogItem (gStatusDialogPtr, IDC_Status3);
			HideDialogItem (gStatusDialogPtr, IDC_Status4);
			HideDialogItem (gStatusDialogPtr, IDC_Status5);
			HideDialogItem (gStatusDialogPtr, IDC_Status6); 
			
			}	// end "if (gStatusDialogPtr != NULL)" 
		
		if (returnCode != -1)
			{
			if (continueFlag)
				returnCode = 0;
				
			else	// !continueFlag 
				returnCode = 2;
				
			}	// end "if (returnCode != -1)"
		
		}	// end "if (FileOpen (projectFileStreamPtr))" 
		
	else	// !FileOpen (projectFileStreamPtr) 
		returnCode = 1;
		
	MInitCursor ();
		
	return (returnCode);

}	// end "WriteProjectFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 WriteStatistics
//
//	Software purpose:	The purpose of this routine is to write the field or
//							class statistics to the project file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	0 = Do not continue
//							1 = Continue, everything is okay.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/27/1992
//	Revised By:			Larry L. Biehl			Date: 04/24/2013	

SInt16 WriteStatistics (
				HChannelStatisticsPtr			channelStatsPtr, 
				HSumSquaresStatisticsPtr		sumSquaresStatsPtr,
				SInt64								numberPixelsUsedForStats,
				CMFileStream* 						fileStreamPtr,
				Boolean								saveMeansCovFlag, 
				Boolean								saveSumsSquaresFlag,
				SInt16								classFieldCode,
				UInt32								binaryListingCode, 
				double*								boxRightPtr, 
				double								boxIncrement)

{
	SInt16								stringStart;
	
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	stringStart = 24 * (classFieldCode-1) + 1;
	
	if (saveSumsSquaresFlag)
		{		
				// Write "CS" record.  "Class channel sums."							
				// Write "FS" record.  "Field channel sums."							
						
		continueFlag = WriteChannelInformation (
														channelStatsPtr,
														fileStreamPtr, 
														gProjectInfoPtr->numberStatisticsChannels, 
														binaryListingCode+2, 
														&gTextString3[stringStart+4]);

		UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
		
		}	// end "if (saveSumsSquaresFlag)" 
		
	if (continueFlag && saveMeansCovFlag)
		{		
				// Write "CM" record.  "Class means."									
				// Write "FM" record.  "Field means."									
				
		continueFlag = WriteChannelInformation (
														channelStatsPtr,
														fileStreamPtr, 
														gProjectInfoPtr->numberStatisticsChannels, 
														binaryListingCode+1, 
														&gTextString3[stringStart]);

		UpdateGraphicStatusBox (boxRightPtr, boxIncrement);
		
		}	// end "if (...->saveMeansCovFlag)" 
			
			// Write "CN" record.  "Class channel minimums."						
			// Write "FN" record.  "Field channel minimums."						
	
	if (continueFlag && channelStatsPtr->minimum != DBL_MAX && 
														(saveMeansCovFlag || saveSumsSquaresFlag))
		continueFlag = WriteChannelInformation (
													channelStatsPtr,
													fileStreamPtr, 
													gProjectInfoPtr->numberStatisticsChannels, 
													binaryListingCode+3, 
													&gTextString3[stringStart+8]);
	
			// Write "CX" record.  "Class channel maximums."						
			// Write "FX" record.  "Field channel maximums."						
					
	if (continueFlag && 
					channelStatsPtr->maximum != -DBL_MAX &&
											(saveMeansCovFlag || saveSumsSquaresFlag))	
		continueFlag = WriteChannelInformation (
													channelStatsPtr,
													fileStreamPtr, 
													gProjectInfoPtr->numberStatisticsChannels, 
													binaryListingCode+4, 
													&gTextString3[stringStart+12]);
	
	if (continueFlag && saveSumsSquaresFlag)
		{					
				// Write "CSS" record. "Class sum of squares."						
				// Write "FSS" record. "Field sum of squares."						
		
		continueFlag = WriteCovarianceInformation (
													sumSquaresStatsPtr,
													channelStatsPtr,
													numberPixelsUsedForStats,
													fileStreamPtr, 
													gProjectInfoPtr->numberStatisticsChannels, 
													binaryListingCode+2, 
													&gTextString3[stringStart+16], 
													boxRightPtr, 
													boxIncrement);
		
		}	// end "if (...->saveSumsSquaresFlag)" 
		
	if (continueFlag && saveMeansCovFlag)
		{		
				// Write "CCV" record.  "Class covariances."							
				// Write "FCV" record.  "Field covariances."							
		
		continueFlag = WriteCovarianceInformation (
													sumSquaresStatsPtr,
													channelStatsPtr,
													numberPixelsUsedForStats,
													fileStreamPtr, 
													gProjectInfoPtr->numberStatisticsChannels, 
													binaryListingCode+1, 
													&gTextString3[stringStart+20], 
													boxRightPtr, 
													boxIncrement);
		
		}	// end "if (continueFlag && saveMeansCovFlag)" 
		
	return (continueFlag);
							
}	// end "WriteStatistics" 
