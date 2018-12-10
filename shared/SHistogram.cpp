//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						Histogram.cpp
//
//	Authors:					Larry L. Biehl, Ravi Budruk
//
//	Revision date:			11/29/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Functions in file:	Boolean 					CheckHistogramTextWindowSpaceNeeded
//								void 						CloseUpHistogramArrayReadParameters
//								Boolean 					ComputeHistogram
//								SInt32					CountTotalNumberHistogramPixels
//								void 						CreateERDAS_staFile
//								void 						DecodeERDAS_staFile
//								SInt32					DetermineBytesForHistogramText
//								pascal void 			DrawHistogramMethodPopUp
//								Boolean 					DoHistogramRequests
//								void						ForceHistogramCodeResourceLoad
//								char* 					GetERDAS_staBuffer
//								Boolean 					GetERDAS_staFile
//								UInt32*					GetHistogramValuesMemory
//								void 						HistogramControl
//								Boolean 					HistogramDialog
//								void 						HistogramDialogStatisticsFile
//								Handle				 	InitializeHistogramInfoStructure
//								Boolean 					ListHistogram
//								Boolean 					ListHistogramTitle
//								Boolean 					ListHistogramValues
//								Boolean 					ListHistogramValuesInColumns
//								Boolean 					ListHistogramValuesInLines
//								void 						LoadERDAS_staFile
//								void 						LoadHistogramSpecs
//								FileInfoPtr 			OpenERDAS_staFile		
//								Boolean 					ReadERDAS_statFile
//								void 						HistogramDialogSetListAndEmptyBins
//								Boolean 					SetUpToReadHistogramArray
//								Boolean 					HistogramDialogUpdateAllChannelsAtOnceFlag
//
//	Brief description:	The purpose of the routines in this file is to
//								determine the user specifications for the image 
//								histogram, compute and 
//								manage the use of the image histograms.  A diagram of
//								the use of the functions follows:
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		HistogramControl
//			LoadHistogramSpecs
//				GetSelectedAreaInfo (in selectionArea.c)
//			HistogramDialog
//				LoadDItemValue (in multiSpecUtilities.c)
//				CheckSomeEvents (in multiSpec.c)
//				HiliteOKButton (in dialogUtilities.c)
//				GetERDAS_staFile
//					GetFile (in fileIO.c)
//					IOCheck (fileIO.c)
//			CheckSomeEvents (in multiSpec.c)
//			ReadERDAS_staFile
//				ConvertATRealtoReal (in SFileIO.cpp)
//				ConvertATRealtoInt (in SFileIO.cpp)
//				MoveBytes (in multiSpecUtilities.c)
//				CloseFile (in fileIO.c)
//				UnlockAndDispose (in multiSpecUtilities.c)
//			LoadDItemString (in multiSpecUtilities.c)
//			ComputeHistogram
//				UnlockAndDispose (in multiSpecUtilities.c)
//			CreateERDAS_staFile
//				CloseFile (in fileIO.c)
//				CopyPToP (in multiSpecUtilities.c)
//				ConcatPStrings (in SUtility.cpp)
//				PutFile (in fileIO.c)
//				IOCheck (in fileIO.c)
//				MoveBytes (in multiSpecUtilities.c)
//				ConvertRealAT (in fileIO.c)
//				UnlockAndDispose (in multiSpecUtilities.c)
//			ListHistogram
//				OutputString (in multiSpecUtilities.c)
//			ListHistogramValues
//				OutputString (in multiSpecUtilities.c)
//				UnlockAndDispose (in multiSpecUtilities.c)
//			UpdateOutputWScrolls
//			UnlockAndDispose (in multiSpecUtilities.c)
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/* Template for debugging.
		int numberChars = sprintf (
				(char*)gTextString3,
				" SHistogram:ComputeHistorgram (gauge_range, gStatusGraphicsRight): %d, %f%s",
				gauge_range,
				gStatusGraphicsRight,
				gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
	 
#if defined multispec_lin
	#include "CHistogram.h"
	#include "CImageWindow.h"

	#include "LHistogramDialog.h"
	#include "LImageView.h"
	#include "LMultiSpec.h"
	typedef wxString CString;
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_DefaultFile				5
	#define IDC_LineColFrame			8
	#define IDC_StartEndInterval		8
	#define IDEntireImage				9
	#define IDSelectedImage				9
	#define IDC_LinePrompt				10
	#define IDC_ColumnPrompt			11
	#define IDC_LineStart				12
	#define IDC_LineEnd					13
	#define IDC_LineInterval			14
	#define IDC_ColumnStart				15
	#define IDC_ColumnEnd				16
	#define IDC_ColumnInterval			17
	#define IDC_ComputeOnly				18
	
	#define IDC_ListHistogram			22
	#define IDC_ColumnLinePrompt		23
	#define IDC_ColumnLineFormat		24
	#define IDC_EmptyBins				26
	#define IDC_WriteResultsFrame		27 
	#define IDC_TextWindow				28
	#define IDC_DiskFile					29
#endif	// defined multispec_mac || defined multispec_mac_swift
                             
#if defined multispec_win
	#include "CHistogram.h"
	#include "WImageView.h"
	#include "CImageWindow.h"
	
	#include <afxcmn.h>
	#include "WHistogramDialog.h"
#endif	// defined multispec_win

//#include	"SExtGlob.h"

extern SInt16 		HistogramDialogHandleMethod (
							DialogPtr							dialogPtr,
							HistogramSpecsPtr					histogramSpecsPtr,
							WindowInfoPtr						windowInfoPtr,
							FileInfoPtr							fileInfoPtr,
							CString*								supportFileNamePtr,
							SInt16								itemHit,
							SInt16								okItemNumber,
							SInt16								previousMethodCode,
							Boolean*								updateListHistogramItemsFlagPtr,
							Boolean*								defaultStatFileChangedFlagPtr);

extern void 		HistogramDialogHideAreaItems (
							DialogPtr							dialogptr);

extern void  		HistogramDialogHideListItems (
							DialogPtr							dialogPtr); 
							
extern void 		HistogramDialogInitialize (
							DialogPtr							dialogPtr, 
							HistogramSpecsPtr					histogramSpecsPtr,
							WindowInfoPtr						windowInfoPtr,
							FileInfoPtr							fileInfoPtr,
							CString*								fileNamePtr,
							CString*								supportFileNamePtr,
							SInt16*								histogramMethodCodePtr,
							Boolean*								histogramInMemoryAvailableFlagPtr,
							DialogSelectArea*					dialogSelectAreaPtr,
							UInt16*								localChannelsPtr, 
							UInt16*								localNumberChannelsPtr,
							SInt16*								channelSelectionPtr,
							Boolean*								minMaxetcOnlyFlagPtr,
							Boolean*								listHistogramSummaryFlagPtr,
							Boolean*								listHistogramFlagPtr,
							Boolean*								lineFormatHistFlagPtr,
							Boolean*								includeEmptyBinsFlagPtr,
							Boolean*								textWindowFlagPtr,
							Boolean*								diskFileFlagPtr,
							Boolean*								defaultStatFileChangedFlagPtr,
							Boolean*								updateListHistogramItemsFlagPtr,
							Boolean*								allChannelsAtOnceFlagPtr); 

extern void 		HistogramDialogOK (
							DialogPtr							dialogPtr, 
							HistogramSpecsPtr					histogramSpecsPtr,
							WindowInfoPtr						windowInfoPtr,
							SInt16								histogramMethodCode,
							DialogSelectArea*					dialogSelectAreaPtr,
							UInt16*								localChannelsPtr, 
							UInt16								localNumberChannels,
							SInt16								channelSelection,
							Boolean								minMaxetcOnlyFlag,
							Boolean								listHistogramSummaryFlag,
							Boolean								listHistogramFlag,
							Boolean								lineFormatHistFlag,
							Boolean								includeEmptyBinsFlag,
							Boolean								textWindowFlag,
							Boolean								diskFileFlag,
							Boolean								defaultStatFileChangedFlag,
							Boolean								allChannelsAtOnceFlag);
							
extern void			HistogramDialogSetListAndEmptyBins (
							DialogPtr 							dialogPtr,
							SInt16								histogramMethodCode, 
							Boolean 								minMaxetcOnlyFlag,
							Boolean 								listHistogramFlag,
							Boolean 								includeEmptyBinsFlag,
							Boolean 								lineFormatHistFlag);

extern void 		HistogramDialogShowAreaItems (
							DialogPtr							dialogPtr);

extern void  		HistogramDialogShowListItems (
							DialogPtr							dialogPtr);
											
extern void 		HistogramDialogStatisticsFile (
							HistogramSpecsPtr					histogramSpecsPtr, 
							WindowInfoPtr						imageWindowInfoPtr,
							FileInfoPtr							imageFileInfoPtr, 
							DialogPtr							dialogPtr, 
							SInt16								itemNumber,
							CString*								supportFileNamePtr);
										
extern Boolean 	HistogramDialogUpdateAllChannelsAtOnceFlag (
							DialogPtr 							dialogPtr, 
							UInt16 								numberChannels, 
							Boolean* 							lineFormatHistFlagPtr);

		

			// Prototype descriptions for routines in this file that are only		
			// called by routines in this file.
																			
Boolean 				CreateSTASupportFile (
							FileInfoPtr 						fileInfoPtr, 
							HistogramSpecsPtr 				histogramSpecsPtr,
							HUInt32Ptr			 				histogramArrayPtr,
							short int 							channelIndex);

Boolean 				CheckHistogramTextWindowSpaceNeeded (
							HistogramSpecsPtr					histogramSpecsPtr);
			
Boolean 				ComputeHistogram (
							FileIOInstructionsPtr			fileIOInstructionsPtr, 
							HistogramSpecsPtr					histogramSpecsPtr, 
							HUInt32Ptr							histogramArrayPtr, 
							SInt16								channelIndex, 
							SInt16								computeCode);
							
UInt32 				DetermineBytesForHistogramText (
							HistogramSpecsPtr					histogramSpecsPtr, 
							Boolean								textWindowFlag);

Boolean 				DetermineIfEFormatRequired (
							HistogramSummaryPtr				histogramSummaryPtr,
							UInt32								numberChannels,
							SInt16*								channelsPtr);

Boolean 				DetermineMinAndMaxValuesForEachChannel (
							FileIOInstructionsPtr			fileIOInstructionsPtr,  
							HistogramSpecsPtr					histogramSpecsPtr,
							HistogramSummaryPtr				histogramSummaryPtr,
							UInt16								*channelListPtr,
							SInt16								channelOffset,
							double								maxSaturatedValue,
							Boolean								BISFlag,
							UInt32								interval,
							UInt32								numberSamples,
							UInt16								imageFileNumberChannels);
																			
Boolean 				DoHistogramRequests (
							HistogramSpecsPtr					histogramSpecsPtr);
			
PascalVoid	 		DrawHistogramMethodPopUp (
							DialogPtr 							dialogPtr, 
							SInt16	 							itemNumber);

void 					GetDataFormatString (
							char*									stringPtr,
							double								value,
							UInt16								fieldSize,
							UInt16								ePrecision,
							UInt16								fPrecision,
							Boolean								forceEFormatFlag);
							
Handle* 				GetHistogramSummaryHandlePtr (
							WindowInfoPtr						windowInfoPtr);

UInt32 				GetNumberOfMaximumDataValues (
							HistogramSummaryPtr				histogramSummaryPtr,
							HUInt32Ptr							histogramArrayPtr,
							SInt32								signedValueOffset);

UInt32 				GetNumberOfMinimumDataValues (
							HistogramSummaryPtr				histogramSummaryPtr,
							HUInt32Ptr							histogramArrayPtr,
							SInt32								signedValueOffset);

Boolean	 			GetStatFileBuffer (
							FileInfoPtr							fileInfoPtr,
							HistogramSpecsPtr					histogramSpecsPtr,
							UInt16*								channelListPtr, 
							SInt16								numberChannels);

Boolean 				HistogramDialog (
							FileInfoPtr							fileInfoPtr, 
							HistogramSpecsPtr					histogramSpecsPtr);

void 					InitializeHistogramSummaryStructure (
							HistogramSummaryPtr 				histogramSummaryPtr,
							UInt32								numberChannels);


Boolean 				ListHistogramValuesInColumns (
							HistogramSpecsPtr					histogramSpecsPtr,
							CMFileStream* 						resultsFileStreamPtr,
							FileInfoPtr							fileInfoPtr,
							double	 							minValue, 
							double	 							maxValue, 
							Boolean 								includeEmptyBinsFlag,
							HistogramSummaryPtr				histogramSummaryPtr,  
							HUInt32Ptr			 				medianArrayPtr, 
							UInt32								startChannelIndex,
							UInt32	 							lastChannelIndex,
							SInt16*								featurePtr,
							UInt16								valueCountTextWidth,
							UInt16								columnCountTextWidth,
							UInt16								ePrecision,
							UInt16								fPrecision);
												
Boolean 				ListHistogramValuesInLines (
							HistogramSpecsPtr					histogramSpecsPtr,
							CMFileStream*						resultsFileStreamPtr,
							FileInfoPtr							fileInfoPtr,
							double								minValue, 
							double								maxValue, 
							HistogramSummaryPtr				histogramSummaryPtr,
							HUInt32Ptr							medianArrayPtr, 
							UInt32								channelIndex,
							UInt32								lastChannel,
							SInt16*								featurePtr,
							UInt16								channelTextWidth,
							UInt16								minValueCountTextWidth,
							UInt16								valueCountTextWidth,
							UInt16								maxValueCountTextWidth,
							UInt16								ePrecision,
							UInt16								fPrecision);							

void 					LoadHistogramSpecs (
							FileInfoPtr							fileInfoPtr, 
							HistogramSpecsPtr					histogramSpecsPtr);	
											
Boolean 				LoadSTASupportFile (
							FileInfoPtr							fileInfoPtr, 
							HistogramSpecsPtr					histogramSpecsPtr,
							HUInt32Ptr							histogramArrayPtr, 
							UInt32								channelIndex, 
							Boolean								summaryFlag);

Boolean 				LoadSupportFile (
							WindowInfoPtr						windowInfoPtr,
							FileInfoPtr							fileInfoPtr, 
							HistogramSpecsPtr					histogramSpecsPtr,
							HUInt32Ptr							histogramArrayPtr, 
							UInt32								channelIndex, 
							Boolean								summaryFlag);

void 					SetHistogramLineColumnSpecs (
							HistogramSpecsPtr					histogramSpecsPtr,
							SInt32								lineStart, 
							SInt32								lineEnd, 
							SInt32								columnStart, 
							SInt32								columnEnd,
							SInt32								numberChannels);

void 					SetNumberOfMaximumDataValuesInArray (
							HistogramSummaryPtr				histogramSummaryPtr,
							HUInt32Ptr							histogramArrayPtr,
							SInt32								signedValueOffset);

void 					SetNumberOfMinimumDataValuesInArray (
							HistogramSummaryPtr				histogramSummaryPtr,
							HUInt32Ptr							histogramArrayPtr,
							SInt32								signedValueOffset);

void					UpdateSupportFileTypeSetting (
							WindowInfoPtr						windowInfoPtr,
							HistogramSpecsPtr					histogramSpecsPtr);


ControlRef			sControlRef = NULL;



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CheckHistogramTextWindowSpaceNeeded
//
//	Software purpose:	The purpose of this routine is to check if there is
// 						enough memory to list the histogram values in the
//							text window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned: 		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1993
//	Revised By:			Larry L. Biehl			Date: 11/07/1995	

Boolean CheckHistogramTextWindowSpaceNeeded (
				HistogramSpecsPtr					histogramSpecsPtr)

{
	UInt32								numberBytesNeeded;
	Boolean								returnFlag;
	
	
	if (!(gOutputCode & 0x0001))
																						return (TRUE);
	
			// Initialize local variables.													
		
	numberBytesNeeded = DetermineBytesForHistogramText (histogramSpecsPtr, TRUE);
	
			// Allow space for status dialog box.											
	
	returnFlag = CheckTextWindowSpaceNeeded (
										numberBytesNeeded + gMinMemoryForDrawingDialog);
		
	return (returnFlag);

}	// end "CheckHistogramTextWindowSpaceNeeded"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CloseUpHistogramArrayReadParameters
//
//	Software purpose:	The purpose of this routine is to dispose of the memory
//							for the histogram array for one channel and the buffer
//							used to load the statistics information for one channel from 
//							disk. Also the statistics disk file is closed.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			DisplayMultispectralImage in displayMultispectral.c
//
//	Global Data:
//
//	Coded By:			Larry L. Biehl			Date:	10/01/1993
//	Revised By:			Larry L. Biehl			Date: 12/23/1997

void CloseUpHistogramArrayReadParameters (void)

{ 
	HistogramSpecsPtr					histogramSpecsPtr;
	Handle								histogramSpecsHandle;
	SignedByte							handleStatus;
	
	
	if (gImageWindowInfoPtr != NULL)
		{
		  		// Close the image statistics file												
		  				
		CloseFile (gImageWindowInfoPtr->supportFileStreamPtr);
			
				// Unlock handle to ERDAS statistics file.									
		
		if (gImageWindowInfoPtr->supportFileStreamHandle != NULL)
			{
					// Note that this does nothing in the Windows version.
					
			CheckAndUnlockHandle (gImageWindowInfoPtr->supportFileStreamHandle);
			gImageWindowInfoPtr->supportFileStreamPtr = NULL;
			
			}	// end "if (...->supportFileStreamHandle != NULL)"
			
				// Get handle and pointer to the histogram structure.
				
		histogramSpecsHandle = GetHistogramSpecsHandle (gImageWindowInfoPtr);
			
		histogramSpecsPtr = (HistogramSpecsPtr)GetHandleStatusAndPointer (
																				histogramSpecsHandle,
																				&handleStatus);
		
		if (histogramSpecsPtr != NULL)
			{		
		
					// Dispose of temporary buffers.												
				
			histogramSpecsPtr->statBufferPtr = CheckAndDisposePtr (
															histogramSpecsPtr->statBufferPtr);
			histogramSpecsPtr->numberStatBufferChannels = 0;
			
			histogramSpecsPtr->histogramArrayH = 
									UnlockAndDispose (histogramSpecsPtr->histogramArrayH);
			histogramSpecsPtr->histogramArrayPtr = NULL;
			
			}	// end "if (histogramSpecsPtr != NULL)"
		
		MHSetState (histogramSpecsHandle, handleStatus);
			
		}	// end "if (gImageWindowInfoPtr != NULL)"
		
}	// end "CloseUpHistogramArrayReadParameters"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		ComputeHistogram
//
//	Software purpose:	The purpose of this routine is to compute the 
//		histogram information for the given image.
//
//	Functional description:	The steps in this function follow.
//
//			1) Read the first line of data that is given in histogramSpecsPtr->
//				lineStart.
//
//			2) For each of the channels,
//				check for new min value (HistogramSummary[i]->minValue), new max
//				value (HistogramSummary[i]->maxValue), update channelSum[i],
//				update channelSumSquare[i], and update count of value in 
//				HistogramSummary[i]->vector[n].
//
//			3) Note that the samples used to compute the histogram statistics 
//				within a line should be based on the first sample (HistogramSpecsPtr->
//				columnStart), the last sample (HistogramSpecsPtr->columnEnd), and
//				the column interval (HistogramSpecsPtr->columnInterval).
//
//			4) Get the next line of data based on the line interval
//				(HistogramSpecsPtr->lineInterval) and repeat 2 and 3.
//
//			5) Stop reading lines of data when one reaches the last line
//				(HistogramSpecsPtr->lineEnd)
//
//			6) Compute the average value for each channel i (HistogramSummary[i]->
//				averageValue), the median of the data values for each channel i
//				(HistogramSummary[i]->medianValue), i.e. same NUMBER of above median
//				as below median, and the standard deviation of the values for each
//				channel i (HistogramSummary[i]->stdDeviation).
//
//	Parameters in:		
//							
//
//	Parameters out:
//
//	Value Returned:	None
// 
// Called By:			HistogramControl
//							DisplayImage  in display.c
//
//	Coded By:			Ravi S. Budruk			Date: 06/09/1988
//	Revised By:			Ravi S. Budruk			Date: 07/26/1988	
//	Revised By:			Larry L. Biehl			Date: 03/01/2013	
		
Boolean ComputeHistogram (
				FileIOInstructionsPtr			fileIOInstructionsPtr,  
				HistogramSpecsPtr					histogramSpecsPtr, 
				HUInt32Ptr							histogramArrayPtr, 
				SInt16								channelIndex, 
				SInt16								computeCode)
	
{
	double								binFactor,
											channelSum,
											doubleDataValue,
											limitDifference,
											maxSaturatedValue,
											maxNonSatValue,
											minNonSatValue,
											minValue,
											sumDataValueSquare,
											variance;
											
	FileInfoPtr							fileInfoPtr,
											localFileInfoPtr; 
											
	HDoublePtr							ioDoublePtr;
	
	HistogramSummaryPtr				histogramSummaryPtr;
											
	HSInt32Ptr							ioBuffer4Ptr;
	
	HUInt32Ptr							medianArrayChanPtr,
											savedHistogramArrayPtr; 
	
	UInt16								*channelListPtr,
											*localChannelListPtr;
											    
	SInt32								binIndex,
											dataValue,
											maxBinIndex,
											signedValueOffset;
	
	UInt32								columnEnd,
											columnInterval,
											columnStart,
											index,
											interval,
											line,
											lineEnd,
											lineInterval,
											lineStart,          
											medianPoint,
											numberBins,
											numberSamples,
											sum; 
	
	SInt16								binType,
											chanIndex,
											channelOffset,	
											errCode = noErr,
											savedComputeCode;
	
	UInt16								channel,
											fileInfoIndex,
											fileInfoIndexEnd,
											fileInfoIndexStart,
											imageFileNumberChannels,
											totalNumberChannels; 
											
	Boolean								allChannelsAtTimeFlag,
											BISFlag,
											computeAveEtcFlag,
											integerFlag,
											newLineFlag;  
											
	#if defined multispec_mac
		Pattern								black;
	#endif	// defined multispec_mac    
	
	
	if ((computeCode & 0x0002) && histogramArrayPtr == NULL)
																				return (FALSE);
																				
	if (histogramSpecsPtr == NULL)
																				return (FALSE);
																				
	if (histogramSpecsPtr->computeFlag)
		{									
		histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
																				
		if (histogramSummaryPtr == NULL)
																				return (FALSE);
	
		fileInfoPtr = fileIOInstructionsPtr->fileInfoPtr;
		
				// Initialize number of bad lines											
				
		histogramSpecsPtr->numBadLines = 0;
		
				// Initialize local variables for this section.							
		
		allChannelsAtTimeFlag = histogramSpecsPtr->allChannelsAtOnceFlag;
		savedComputeCode = computeCode;
				
		columnStart = histogramSpecsPtr->columnStart;
		columnEnd = histogramSpecsPtr->columnEnd;
		columnInterval = histogramSpecsPtr->columnInterval;
		lineStart = histogramSpecsPtr->lineStart;
		lineEnd = histogramSpecsPtr->lineEnd;
		lineInterval = histogramSpecsPtr->lineInterval;
		
		numberBins = histogramSpecsPtr->maxNumberBins;
			
		channelOffset = 0; 						
		channelListPtr = (UInt16*)GetHandlePointer (histogramSpecsPtr->channelsHandle);
		localChannelListPtr = &channelListPtr[gImageWindowInfoPtr->totalNumberChannels];
		
		totalNumberChannels = histogramSpecsPtr->numberChannels;
		
		if (!allChannelsAtTimeFlag)
			{
			channelListPtr = &channelListPtr[channelIndex];
			totalNumberChannels = 1;
			
			}	// end "if (!allChannelsAtTimeFlag)" 
	
		integerFlag = FALSE;
		if (fileIOInstructionsPtr->forceByteCode == kForce4Bytes)
			integerFlag = TRUE;
		
		fileInfoIndexStart = gImageLayerInfoPtr[*channelListPtr+1].fileInfoIndex;
		
		fileInfoIndexEnd = gImageLayerInfoPtr[
							channelListPtr[totalNumberChannels-1]+1].fileInfoIndex;
	
		if (computeCode & 0x0002)
			{
					// Initialize the median array.											
					
			medianArrayChanPtr = histogramArrayPtr;
			numberSamples = totalNumberChannels * histogramSpecsPtr->maxNumberBins;
			for (index=0; index<numberSamples; index++)
				{
				*medianArrayChanPtr = 0;
				medianArrayChanPtr++;
				
				}	// end "for (index=0; index<numberSamples; index++)" 
				
			}	// end "if (computeCode & 0x0002)" 
			
		medianArrayChanPtr = histogramArrayPtr;
		
		computeAveEtcFlag = FALSE;
		if (computeCode & 0x0001)
			computeAveEtcFlag = TRUE;

				// Loop through the image files.
				
		for (fileInfoIndex=fileInfoIndexStart; 
				fileInfoIndex<=fileInfoIndexEnd;
				fileInfoIndex++)
			{	
			computeCode = savedComputeCode;
		
			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
			
					// Initialize image file min and max values and the number of decimal
					// digits the data value statistic information is being computed.
	
			if (computeAveEtcFlag)
				{
				localFileInfoPtr->maxNumberEDecimalDigits = 0;
				localFileInfoPtr->maxNumberFDecimalDigits = 0;
				localFileInfoPtr->maxDataValue = -DBL_MAX;
				localFileInfoPtr->minDataValue = DBL_MAX;
				
				}	// end "if (computeAveEtcFlag)"
			
					// Force signed data to be converted to unsigned data if the number
					// of bytes is less than equal to 2. The integer value itself is the
					// index into an array.
		
			gConvertSignedDataFlag = TRUE;
			if (localFileInfoPtr->numberBytes >= 4)
				gConvertSignedDataFlag = FALSE;
			
					// Get some information about this image file.						
					
					// Create a channel list for the current image file.				
			
			imageFileNumberChannels = 0;
			for (index=channelOffset; index<totalNumberChannels; index++)
				{
				chanIndex = channelListPtr[index] + 1;
				if (fileInfoIndex == gImageLayerInfoPtr[chanIndex].fileInfoIndex)
					{
					localChannelListPtr[imageFileNumberChannels] = chanIndex - 1;
					imageFileNumberChannels++;
					
					}	// end "if (fileInfoIndex == ..." 
					
				else	// fileInfoIndex != ... 
					break;
				
				}	// end "for (index=channelOffset+1; index..." 
				
			maxSaturatedValue = (double)(localFileInfoPtr->numberBins - 1);
			numberSamples = (columnEnd - columnStart + columnInterval)/columnInterval;
		
			if (localFileInfoPtr->bandInterleave == kBIS)
				{
				interval = (SInt32)imageFileNumberChannels;
				numberSamples *= imageFileNumberChannels;
				BISFlag = TRUE;
				
				}	// end "if (localFileInfoPtr->bandInterleave == kBIS)" 
				
			else	// localFileInfoPtr->bandInterleave != kBIS 
				{
				interval = 1;
				BISFlag = FALSE;
				
				}	// end "else localFileInfoPtr->bandInterleave != kBIS"
		
					// Set some of the File IO Instructions parameters.
	
			SetUpFileIOInstructions (fileIOInstructionsPtr,
												NULL,
												lineStart,
												lineEnd,
												lineInterval,
												columnStart,
												columnEnd,
												columnInterval, 
												imageFileNumberChannels,
												localChannelListPtr,
												kDetermineSpecialBILFlag);
				
			if (localFileInfoPtr->numberBytes <= 2)
				{	
				for (channel=0; 
						channel<imageFileNumberChannels; 
						channel++)
					{
					chanIndex = channelListPtr[channelOffset+channel];
										
					histogramSummaryPtr[chanIndex].numberBins = 256;
					
					if (localFileInfoPtr->numberBytes == 2)
						histogramSummaryPtr[chanIndex].numberBins = 65536;
				
					histogramSummaryPtr[chanIndex].binType = kDataValueIsBinIndex;
					histogramSummaryPtr[chanIndex].binFactor = 1;
					
					}	// end "if (localFileInfoPtr->numberBytes <= 2)"
				
				}	// end "if (localFileInfoPtr->numberBytes <= 2)"

			else	// localFileInfoPtr->numberBytes > 2
				{
				DetermineMinAndMaxValuesForEachChannel (fileIOInstructionsPtr,
																		histogramSpecsPtr,
																		histogramSummaryPtr,
																		channelListPtr,
																		channelOffset,
																		maxSaturatedValue,
																		BISFlag,
																		interval,
																		numberSamples,
																		imageFileNumberChannels);
				
						// Now get the bin width and the number of bins that will be used.
						
				for (channel=0; 
						channel<imageFileNumberChannels; 
						channel++)
					{
					chanIndex = channelListPtr[channelOffset+channel];					
					
					limitDifference = histogramSummaryPtr[chanIndex].maxNonSatValue - 
													histogramSummaryPtr[chanIndex].minNonSatValue;
					if (limitDifference <= 0)
						limitDifference = 256;
					
					if (localFileInfoPtr->dataTypeCode == kIntegerType)
						{
						if (limitDifference <= 2045)
							{
				  			histogramSummaryPtr[chanIndex].numberBins = (SInt32)limitDifference + 3;
							histogramSummaryPtr[chanIndex].binType = kBinWidthOfOne;
							
							}	// end "if (limitDifference <= 2045)"
							
						else	// limitDifference > 2045)
							{
							histogramSummaryPtr[chanIndex].numberBins = 2048;
							histogramSummaryPtr[chanIndex].binType = kBinWidthNotOne;
							
							}	// end "else if (limitDifference <= 2045)"
						
						}	// end "if (...->dataTypeCode == kIntegerType)"
						
					else	// localFileInfoPtr->dataTypeCode == kRealType)
						{
						histogramSummaryPtr[chanIndex].numberBins = 2048;
						histogramSummaryPtr[chanIndex].binType = kBinWidthNotOne;
						
						}	// end "else localFileInfoPtr->dataTypeCode == kRealType)"
					
					if (histogramSummaryPtr[chanIndex].binType == kBinWidthOfOne)
						histogramSummaryPtr[chanIndex].binFactor = 1;
					
					else	// histogramSummaryPtr[chanIndex].binType == kBinWidthNotOne
						histogramSummaryPtr[chanIndex].binFactor = 
							(double)(histogramSummaryPtr[chanIndex].numberBins-3)/limitDifference;
						
					}	// end "for (channel=0; channel<..." 
					
						// Indicate that the min and max values do not have to be 
						// computed.  However the averages and variances still need to be
						// computed.
						
				computeCode &= 0x0002;
				computeAveEtcFlag = TRUE;
												
				}	// end "else localFileInfoPtr->numberBytes > 2"
				
			savedHistogramArrayPtr = medianArrayChanPtr;

					// Read line of image as specified by HistogramSpecs.				
  	
			for (line=lineStart; line<=lineEnd; line+=lineInterval)
				{
						// Get all channels for the line of image data.  Return 		
						// if there is a file IO error.										
					 
				errCode = GetLineOfData (fileIOInstructionsPtr,
													line, 
													columnStart,
													columnEnd,
													columnInterval,
													gInputBufferPtr,
													gOutputBufferPtr);

				if (errCode != noErr)
					{			
					gConvertSignedDataFlag = FALSE;	
					histogramSpecsPtr->loadedFlag = FALSE;					
																							return (FALSE);
																				
					}	// end "if (errCode != noErr)" 
			
						// This part of the program computes the histogram statistics for 
						// the selected portion of the image data.																
					
				newLineFlag = TRUE;
				medianArrayChanPtr = savedHistogramArrayPtr;
						
				for (channel=0; 
						channel<imageFileNumberChannels; 
						channel++)
					{
					chanIndex = channelListPtr[channelOffset+channel];
					binFactor = histogramSummaryPtr[chanIndex].binFactor;
					binType = histogramSummaryPtr[chanIndex].binType;
					minValue = histogramSummaryPtr[chanIndex].minValue;
					minNonSatValue = histogramSummaryPtr[chanIndex].minNonSatValue;
					maxNonSatValue = histogramSummaryPtr[chanIndex].maxNonSatValue;
					maxBinIndex = histogramSummaryPtr[chanIndex].numberBins - 1;
					
					if (integerFlag)
						{
						ioBuffer4Ptr = (HSInt32Ptr)gOutputBufferPtr;
						
						if (BISFlag)
								// File band interleave format is BIS; adjust the			
								// buffer pointer to start at the channel being 			
								// processed.															
								
							ioBuffer4Ptr = &ioBuffer4Ptr[chanIndex];
					
						else	// !BISFlag
							ioBuffer4Ptr = &ioBuffer4Ptr[channel*numberSamples];
					
						for (index=0; index<numberSamples; index+=interval)
							{
							dataValue = ioBuffer4Ptr[index];
							/*
									// Check for bad data.
							 
							if (dataValue > maxSaturatedValue)
								{
								histogramSummaryPtr[chanIndex].badValues++;
								dataValue = maxSaturatedValue;
								
										// Update count of bad lines.
							 
								if (newLineFlag)
									{
									histogramSpecsPtr->numBadLines++;
									newLineFlag = FALSE;
							 
									}	// end "if (newLine)"
								
								}	// end "if (dataValue >= maxSaturatedValue)"
							*/
							if (computeCode & 0x0001)
								{
										// Get the maximum data values.	
										
								if (dataValue != histogramSummaryPtr[chanIndex].maxValue)
									{		
									if (dataValue > histogramSummaryPtr[chanIndex].maxValue)
										{
										histogramSummaryPtr[chanIndex].maxNonSatValue =
																histogramSummaryPtr[chanIndex].maxValue;
												
										histogramSummaryPtr[chanIndex].maxValue = dataValue;
									
										}	// end "if (dataValue > ...[chanIndex].maxValue)"
									
									else if (dataValue > histogramSummaryPtr[chanIndex].maxNonSatValue &&
													dataValue != histogramSummaryPtr[chanIndex].maxValue)
										histogramSummaryPtr[chanIndex].maxNonSatValue = dataValue;
							
									}	// end "if (dataValue != ...[chanIndex].maxValue)"		
								
										// Get the minimum data values.			
							
								if (dataValue != histogramSummaryPtr[chanIndex].minValue)
									{
									if (dataValue < histogramSummaryPtr[chanIndex].minValue)
										{
										histogramSummaryPtr[chanIndex].minNonSatValue =
																	histogramSummaryPtr[chanIndex].minValue;
													
										histogramSummaryPtr[chanIndex].minValue = dataValue;
										
										}	// end "if (dataValue > ...[chanIndex].maxValue)"
										
									else if (dataValue < histogramSummaryPtr[chanIndex].minNonSatValue &&
													dataValue != histogramSummaryPtr[chanIndex].minValue)
										histogramSummaryPtr[chanIndex].minNonSatValue = dataValue;
							
									}	// end "if (dataValue != ...[chanIndex].minValue)"
							
										// Compute the channel sum.								
									
								histogramSummaryPtr[chanIndex].averageValue += (double)dataValue;
		
										// Compute the sum of the data value square for		
										// each channel												
		
								histogramSummaryPtr[chanIndex].stdDeviation += 
																(double)dataValue * (double)dataValue;		
																
										// Update count of values for histogram						
							
								if (histogramArrayPtr != NULL)	
									medianArrayChanPtr[dataValue]++;
														
								}	// end "if (computeCode & 0x0001)" 
						
									// Increment median arrays.									
							
							else if (computeCode & 0x0002)
								{
								if (binType == kDataValueIsBinIndex)
									binIndex = dataValue;
									
								else	// binType != kDataValueIsBinIndex
									{
									if (dataValue < minNonSatValue)
										binIndex = 0;
										
									else if (dataValue > maxNonSatValue)
										binIndex = maxBinIndex;
									
									else	// dataValue >= minNonSatValue && dataValue <= ...
										{
										if (binType == kBinWidthOfOne)
											binIndex = (SInt32)(dataValue - minNonSatValue + 1);
									
										else	// binType != kBinWidthOfOne
											binIndex = (SInt32)(
														(dataValue - minNonSatValue)*binFactor + 1);
										
										}	// end "else dataValue >= minNonSatValue && ..."
									
									}	// end "else binType != kDataValueIsBinIndex"
									
								binIndex = MAX (0, binIndex);
								binIndex = MIN (binIndex, maxBinIndex);
									
								medianArrayChanPtr[binIndex]++;	
								
								}	// end "else if (computeCode & 0x0002)"
				
							}	// end "for (index=0; ..." 
							
						}	// end "if (integerFlag)
						
					else	// !integerFlag
						{
						ioDoublePtr = (HDoublePtr)gOutputBufferPtr;
						
						if (BISFlag)
								// File band interleave format is BIS; adjust the			
								// buffer pointer to start at the channel being processed.
								
							ioDoublePtr = &ioDoublePtr[chanIndex];
					
						else	// !BISFlag
							ioDoublePtr = &ioDoublePtr[channel*numberSamples];
					
						for (index=0; index<numberSamples; index+=interval)
							{
							doubleDataValue = ioDoublePtr[index];
						
									// Increment median arrays.									
							
							if (computeCode & 0x0002)
								{
								if (binType == kDataValueIsBinIndex)
									binIndex = (UInt32)doubleDataValue;
									
								else	// binType != kDataValueIsBinIndex
									{
									if (doubleDataValue < minNonSatValue)
										binIndex = 0;
										
									else if (doubleDataValue > maxNonSatValue)
										binIndex = maxBinIndex;
									
									else	// doubleDataValue >= minNonSatValue && ...
										{
										if (binType == kBinWidthOfOne)
											binIndex = (SInt32)(doubleDataValue - minNonSatValue + 1);
									
										else	// binType != kBinWidthOfOne
											binIndex = (SInt32)(
													(doubleDataValue - minNonSatValue)*binFactor + 1);
										
										}	// end "else dataValue >= minNonSatValue && ..."
									
									}	// end "else binType != kDataValueIsBinIndex"
									
								binIndex = MAX (0, binIndex);
								binIndex = MIN (binIndex, maxBinIndex);
									
								medianArrayChanPtr[binIndex]++;	
								
								}	// end "else if (computeCode & 0x0002)"
				
							}	// end "for (index=0; ..." 
							
						}	// end "else !integerFlag
					
					if (computeCode & 0x0002)	
						medianArrayChanPtr += histogramSpecsPtr->maxNumberBins;
						
					}	// end "for (channel=0; channel<..." 
					
						// Check if user wants to exit histogramming.					
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
						{
						histogramSpecsPtr->loadedFlag = FALSE;			
						gConvertSignedDataFlag = FALSE;	
																								return (FALSE);
							
						}	// end "if (!CheckSomeEvents (..." 
						
					}	// end "if (TickCount () >= nextTime)" 
				
				if (TickCount () >= gNextStatusTime)
					{
					gStatusGraphicsBox.right = (SInt16)gStatusGraphicsRight;

					#if defined multispec_mac
						if (gStatusProgressControlHandle != NULL)		
							SetControlValue (gStatusProgressControlHandle, 
													gStatusGraphicsBox.right);
						
						else	// gStatusProgressControlHandle == NULL
							FillRect (&gStatusGraphicsBox, GetQDGlobalsBlack (&black));
					#endif	// defined multispec_mac

					#if defined multispec_win
						((CProgressCtrl*)(gStatusDialogPtr->GetDlgItem (IDC_Status7)))->
															SetPos ((SInt32)gStatusGraphicsRight);
					#endif	// defined multispec_win

               #if defined multispec_lin
						//int gauge_range = ((wxGauge*) (gStatusDialogPtr->FindWindow (IDC_Status7)))->GetRange ();
						//gStatusGraphicsRight = MIN (gStatusGraphicsRight, gauge_range);
						/* int numberChars = sprintf (
								(char*)gTextString3,
								" SHistogram: ComputeHistogram (gStatusGraphicsRight): %f%s",
								gStatusGraphicsRight,
								gEndOfLine);
						ListString ((char*)gTextString3, numberChars, gOutputTextH);
						*/
						((wxGauge*)(gStatusDialogPtr->FindWindow (IDC_Status7)))->
																SetValue ((SInt32)gStatusGraphicsRight);
						CheckSomeEvents (osMask+updateMask);
					#endif
					
					gNextStatusTime = TickCount () + gNextStatusTimeOffset;
					
					}	// end "if (TickCount () >= gNextStatusTime)" 
					
						// Update the new right location of the graphics box which	
						// represents the portion of the computation that has been	
						// completed.																
						
				gStatusGraphicsRight += gStatusBoxIncrement;

				}	// end "line=lineStart; ..."
	
			CloseUpFileIOInstructions (fileIOInstructionsPtr, NULL);	
				
			channelOffset += imageFileNumberChannels;
	
			gConvertSignedDataFlag = FALSE;
				
			}	// end "for (fileInfoIndex=0; ..." 
					
		numberSamples = (columnEnd - columnStart + columnInterval)/columnInterval;
		numberSamples *= ((lineEnd - lineStart + lineInterval)/lineInterval);
					
		histogramSpecsPtr->totalPixels = numberSamples;
		
		medianPoint = numberSamples/2;
		
		savedHistogramArrayPtr = histogramArrayPtr;
					
		for (channel=0; 
				channel<totalNumberChannels; 
				channel++)
			{
					// Get the fileInfoPtr for the channel so that we can get the proper
					// signedValueOffset for the data for this channel.  This is done in
					// case we are working with a linked image file with mixed data types.
			
			fileInfoIndex = gImageLayerInfoPtr[channelListPtr[channel]+1].fileInfoIndex;
			localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
			signedValueOffset = localFileInfoPtr->signedValueOffset;
			
			chanIndex = channelListPtr[channel];
			maxSaturatedValue = histogramSummaryPtr[chanIndex].maxValue;
			
			if (numberSamples > 0)
				{	
				if (computeAveEtcFlag)
					{
					channelSum = histogramSummaryPtr[chanIndex].averageValue;
					sumDataValueSquare = histogramSummaryPtr[chanIndex].stdDeviation;
				
							// Make certain that min and max's make sense.			
						
					if (histogramSummaryPtr[chanIndex].minValue == 
															histogramSummaryPtr[chanIndex].maxValue)
						{
						histogramSummaryPtr[chanIndex].maxNonSatValue = 
															histogramSummaryPtr[chanIndex].minValue;
						histogramSummaryPtr[chanIndex].minNonSatValue =
															histogramSummaryPtr[chanIndex].minValue;
						
						}	// end "if (...->histogram[chanIndex]..." 			
						
					else if (histogramSummaryPtr[chanIndex].minValue == 
											histogramSummaryPtr[chanIndex].maxNonSatValue &&
								histogramSummaryPtr[chanIndex].minNonSatValue == 
											histogramSummaryPtr[chanIndex].maxValue)
						{
						histogramSummaryPtr[chanIndex].minNonSatValue = 
															histogramSummaryPtr[chanIndex].minValue;
						histogramSummaryPtr[chanIndex].maxNonSatValue =
															histogramSummaryPtr[chanIndex].maxValue;
						
						}	// end "if (...->histogram[chanIndex]..." 		
					/*
					else if (histogramSummaryPtr[chanIndex].minNonSatValue ==
											histogramSummaryPtr[chanIndex].maxNonSatValue)
						{
						if (histogramSummaryPtr[chanIndex].binType != kBinWidthNotOne)
							{
							if (histogramSummaryPtr[chanIndex].minNonSatValue ==
														histogramSummaryPtr[chanIndex].minValue + 1)
								histogramSummaryPtr[chanIndex].minNonSatValue = 
																histogramSummaryPtr[chanIndex].minValue;
															
							if (histogramSummaryPtr[chanIndex].maxValue ==
												histogramSummaryPtr[chanIndex].maxNonSatValue + 1)
								histogramSummaryPtr[chanIndex].maxNonSatValue =
															histogramSummaryPtr[chanIndex].maxValue;
															
							}	// end "if (histogramSummaryPtr[chanIndex].binType != ..."
						
						}	// end "if (...->histogram[chanIndex]..." 
					*/
					histogramSummaryPtr[chanIndex].averageValue /= numberSamples;
				
							// Compute the histogram standard deviation				
	
					if (numberSamples > 1)
						{
						variance = (sumDataValueSquare - (channelSum * (channelSum /
								 									numberSamples))) / (numberSamples - 1);
								 			
						if (variance < 0.) 
							variance = 0.;
							
						histogramSummaryPtr[chanIndex].stdDeviation = (double)sqrt (variance);
																		
						}	// end "if (numberSamples > 1)" 
						
					else	// numberSamples == 1 
						histogramSummaryPtr[chanIndex].stdDeviation = 0.;
						
							// Adjust for signed data in case needed.	
							
					histogramSummaryPtr[chanIndex].maxValue -= signedValueOffset;
					histogramSummaryPtr[chanIndex].minValue -= signedValueOffset;
					histogramSummaryPtr[chanIndex].maxNonSatValue -= signedValueOffset;
					histogramSummaryPtr[chanIndex].minNonSatValue -= signedValueOffset;
					histogramSummaryPtr[chanIndex].averageValue -= signedValueOffset;
														
						// Get the number of decimal digits to use when listing data
						// values.
					/*
					histogramSummaryPtr[chanIndex].numberFDecimalDigits =
										GetNumberDecimalDigitsForChannel (
																&histogramSummaryPtr[chanIndex],
																localFileInfoPtr->dataTypeCode);
					*/
					GetNumberDecimalDigits (
												localFileInfoPtr->dataTypeCode,
												histogramSummaryPtr[chanIndex].minValue,
												histogramSummaryPtr[chanIndex].maxValue,
												1/histogramSummaryPtr[chanIndex].binFactor,
												&histogramSummaryPtr[chanIndex].numberEDecimalDigits,
												&histogramSummaryPtr[chanIndex].numberFDecimalDigits);
					
							// Get the overall min and max values and estimate the number of
							// decimal places required.  This information is used for listing
							// data values.
				
					localFileInfoPtr->maxNumberEDecimalDigits = 
										MAX (localFileInfoPtr->maxNumberEDecimalDigits,
												histogramSummaryPtr[chanIndex].numberEDecimalDigits);
				
					localFileInfoPtr->maxNumberFDecimalDigits = 
										MAX (localFileInfoPtr->maxNumberFDecimalDigits,
												histogramSummaryPtr[chanIndex].numberFDecimalDigits);
					 								
					localFileInfoPtr->maxDataValue = MAX (
														localFileInfoPtr->maxDataValue, 
														histogramSummaryPtr[chanIndex].maxValue);
														
					localFileInfoPtr->minDataValue = MIN (
														localFileInfoPtr->minDataValue, 
														histogramSummaryPtr[chanIndex].minValue);
						
					histogramSummaryPtr[chanIndex].availableFlag = TRUE;
																				
					}	// end "if (computeAveEtcFlag)" 	
				
				if (computeCode & 0x0002)
					{	
					binFactor = 1;
					if (histogramSummaryPtr[chanIndex].binFactor > 0)
						binFactor = 1./histogramSummaryPtr[chanIndex].binFactor;
					
					binType = histogramSummaryPtr[chanIndex].binType;
						
					medianArrayChanPtr = savedHistogramArrayPtr;
					
							// Now get the values starting with the minimum non saturated value
						
					if (binType == kBinWidthNotOne)
						{
								// Get the minimum value.
								
						sum = *medianArrayChanPtr;
						medianArrayChanPtr++;
					
						minValue = histogramSummaryPtr[chanIndex].minNonSatValue + binFactor/2;
						
						binIndex = 0;
						do	
							{
							binIndex++;
							doubleDataValue = minValue + binIndex*binFactor;
							sum += *medianArrayChanPtr;
							medianArrayChanPtr++;
							
							}	while (sum <= medianPoint && doubleDataValue <= maxSaturatedValue);
							
						doubleDataValue = minValue + (binIndex-1)*binFactor;
															
						}	// end "if (binType == kBinWidthNotOne)"
					
					else	// binType != kBinWidthNotOne
						{
						if (binType == kDataValueIsBinIndex)
							{
							minValue = 0;
							sum = 0;
							
							}	// end "if (binType == kDataValueIsBinIndex)"
							
						else	// binType == kBinWidthOfOne
							{
							minValue = histogramSummaryPtr[chanIndex].minNonSatValue;
							sum = *medianArrayChanPtr;
							medianArrayChanPtr++;
							
							}	// end "if (binType == kDataValueIsBinIndex)"
						
						doubleDataValue = minValue;
						do
							{
							sum += *medianArrayChanPtr;
							doubleDataValue++;
							medianArrayChanPtr++;
							
							}	while (sum <= medianPoint && doubleDataValue <= maxSaturatedValue);
							
						doubleDataValue = MAX (minValue, doubleDataValue-1);
															
						}	// end "if (binType == kDataValueIsBinIndex)"
						
					histogramSummaryPtr[chanIndex].medianValue = doubleDataValue;
						
							// Adjust for signed data in case needed.	
							
					histogramSummaryPtr[chanIndex].medianValue -= signedValueOffset;
					
					savedHistogramArrayPtr += histogramSpecsPtr->maxNumberBins;
					
					}	// end "if (computeCode & 0x0002)" 
																
				}	// end "if (numberSamples > 0)" 
													
			else	// numberSamples <= 0 
				{ 
				histogramSummaryPtr[chanIndex].availableFlag = FALSE;
				histogramSummaryPtr[chanIndex].minValue = 0;
				histogramSummaryPtr[chanIndex].maxValue = 0;
				histogramSummaryPtr[chanIndex].medianValue = 0;
				histogramSummaryPtr[chanIndex].averageValue = 0.;
				histogramSummaryPtr[chanIndex].stdDeviation = 0.;
				histogramSummaryPtr[chanIndex].minNonSatValue = 0;
				histogramSummaryPtr[chanIndex].maxNonSatValue = 0;
				
				}	// end "else numberSamples <= 0" 
				
			chanIndex++;

			}	// end "for (channel=0; channel< ..." 
		
				// Update the status dialog box.												
				
		gStatusGraphicsBox.right = (SInt16)gStatusGraphicsRight;
		#if defined multispec_mac
			if (gStatusProgressControlHandle != NULL)		
				SetControlValue (gStatusProgressControlHandle, 
										gStatusGraphicsBox.right);
			
			else	// gStatusProgressControlHandle == NULL
				FillRect (&gStatusGraphicsBox, GetQDGlobalsBlack (&black));
		#endif	// defined multispec_mac                                            
		
		}	//	end "histogramSpecsPtr->computeFlag" 
		
	return (TRUE);

}	// end "ComputeHistogram" 

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 CountTotalNumberHistogramPixels
//
//	Software purpose:	This routine count the total number of pixels that
//							were histogrammed by getting the sum of the number
//							of pixels in the histogram array.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/15/1993
//	Revised By:			Larry L. Biehl			Date: 06/11/2003

UInt32 CountTotalNumberHistogramPixels (
				HUInt32Ptr							histogramArrayPtr,
				SInt32								numberBins)

{
	UInt32								count,
											index,
											maxIndex;
	
	
	count = 0;

	if (histogramArrayPtr != NULL)
		{
				// If the image histogram values were read in, find the total 		
				// number of pixels; otherwise set the total number of pixels		
				// to 0 to indicate that we do not know the total number.			
				
		maxIndex = numberBins - 1;
		if (histogramArrayPtr != NULL)
			{          
			for (index=0; index<=maxIndex; index++)
				{
				count += *histogramArrayPtr;
				histogramArrayPtr++;
				
				}	// end "for (index=0; ..." 
			
			}	// end "if (histogramArrayPtr && ...)"
			
		}	// end "if (histogramArrayPtr != NULL)" 
		
	return (count);
	
}	// end "CountTotalNumberHistogramPixels"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean CreateSTASupportFile
//
//	Software purpose:	This routine creates an ERDAS statistics file.
//							Some of the fields in the .STA file are specific for MultiSpec
// 						so that the .STA file can handle the statistics for image files
//							with more than 8 bits per data value. ERDAS Imagine will not 
//							read these files correctly. The new fields are (0-based):
//								Bytes 32- 35: 'VER2' (versionCode == 1; not written since 2005)
//								Bytes 36- 37: maximum non-saturated value
//								Bytes 38- 39: minimum non-saturated value
//								Bytes 40- 43: total number of values in the histogram
//								Bytes 44- 47: total number of bad values
//								Bytes 48- 49: histogram compaction value
//
//								Bytes 32- 35: 'VE3I' (versionCode == 2)
//								Bytes 36- 39: maximum value (up to 4-byte integer)
//								Bytes 44- 47: maximum non-saturated (up to 4-byte integer)
//								Bytes 52- 55: minimum value (up to 4-byte integer)
//								Bytes 60- 63: minimum non-saturated (up to 4-byte integer)
//								Bytes 68- 71: total number of values in the histogram
//								Bytes 72- 75: histogram compaction value
//								Bytes 76- 79: number of bins in original histogram
//								Bytes 80- 81: bin type
//
//								Bytes 32- 35: 'VE3R' (versionCode == 3)
//								Bytes 36- 43: maximum value (8-byte real)
//								Bytes 44- 51: maximum non-saturated (8-byte real)
//								Bytes 52- 59: minimum value (8-byte real)
//								Bytes 60- 67: minimum non-saturated (8-byte real)
//								Bytes 68- 71: total number of values in the histogram
//								Bytes 72- 75: histogram compaction value
//								Bytes 76- 79: number of bins in original histogram
//								Bytes 80- 81: bin type
//								Bytes 82- 89: average value (8-byte real)
//								Bytes 90- 97: median value (8-byte real)
//								Bytes 98-105: standard deviation value (8-byte real)
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			DoHistogramRequests in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1988
//	Revised By:			Larry L. Biehl			Date: 11/29/2018

Boolean CreateSTASupportFile (
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr, 
				HUInt32Ptr							medianArrayPtr, 
				short int							channelIndex)

{
	double								compaction,	
											compactionCount,
											nextSTABinEnd,
											tempDoubleValue;
	
	CharPtr								ioChanBufferPtr;
												
	CMFileStream						supportFileStreamCopy,
											*fileStreamPtr,
											*supportFileStreamPtr;
	
	HistogramSummaryPtr				histogramSummaryPtr;
	
	char									*erdasHeader = (char*)"TRAIL74";
											
	UInt32								*ioTempBufferPtr;
	
	SInt32								channel,
											//compaction,
											//endValue,
											fourBytesZero = 0,
											signedValueOffset,
											//startValue,
											tempLongInt,
											tempSInt32;
	
	UInt32								binIndex,
											count,
											//channelOffsetCount,
											endValue,
											imageValue,
											numberBins,
											numberBytes,
											numberDataValues,
											startValue;
	
	SInt16								channelEnd,
											errCode = noErr,
											twoBytesZero = 0;
	
	UInt16								tempInt,
											tempUInt16;
	
	Boolean								continueFlag;
	
	char									theChar;
	
	
	if (gImageWindowInfoPtr == NULL)
																				return (FALSE);
	
			// Exit if the image window is for a Thematic type image.  No			
			// statistics file will be generated for these type of images			
			// since they contain thematic (class) type information as 				
			// apposed to measurement type of data.										
			
	if (gImageWindowInfoPtr->imageType != kMultispectralImageType)
																				return (TRUE);
	
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	if (fileStreamPtr == NULL)
																				return (FALSE);			

	supportFileStreamPtr = gImageWindowInfoPtr->supportFileStreamPtr;
	if (supportFileStreamPtr == NULL)
																				return (FALSE);	
			
			// Save the current support file stream.
				
	InitializeFileStream (&supportFileStreamCopy, supportFileStreamPtr);	
	
			// Initialize local variables.													
			
	continueFlag = TRUE;
	signedValueOffset = gImageFileInfoPtr->signedValueOffset;
	numberBytes = 9 * 128;
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	
			// Get buffer to load statistics into.  First check if buffer is		
			// already available.  If not, try to get buffer to hold					
			// statistics information for one band.										
	
	if (histogramSpecsPtr->statBufferPtr == NULL)
		{
		histogramSpecsPtr->statBufferPtr = (char*)MNewPointerClear (numberBytes);
		continueFlag = (histogramSpecsPtr->statBufferPtr != NULL);
		
		}	// end "if (statBufferPtr == NULL)" 
					
	if (continueFlag && supportFileStreamPtr != NULL)
		{
		if (channelIndex == 0)
			{
					// This is the first channel for the statistics file.  Get		
					// the name for the file and open it.									
					// If the statistics file is open, then close it so that it or	
					//	another file can be opened in write mode.							
				
			CloseFile (supportFileStreamPtr);
					
					// Get the output file name and volume to write file to.	Make the 
					// default name the same as the image file name with .sta at the	
					// end.  First make sure that we have a pointer cursor. Also 		
					// Check if there is enough space on the selected volume.			
			
			MInitCursor ();
	
			GetDefaultSupportFileName (fileStreamPtr, supportFileStreamPtr, kISTAFileType);
			
					// Make the default volume the same as that for the active		
					// image file.																	
					
			SetVolumeReference (fileStreamPtr, supportFileStreamPtr);
			errCode = SetVolume (supportFileStreamPtr, kErrorMessages);
			continueFlag = (errCode == noErr);
			
			#if defined multispec_lin
					// for mygeohub version, we need to make sure that we are not trying to write
					// to the default input image file directory since it is read only
					// For MultiSpec on mygeohub, need to change the working directory for saving files if
					// the current working directory is the read only data directory
					// Use wxSetWorkingDirectory (wxString) or wxFileDialog::SetDirectory to do this. Use wxGetCwd or wxFileDialog::GetDirectory to determine 
					// if the default one is the standard directory.

				#ifndef NetBeansProject
					FileStringPtr			fileNamePtr,
												filePathPtr;
					
					filePathPtr = (FileStringPtr)supportFileStreamPtr->GetFilePathPPtr ();
					wxString wxFilePathName (&filePathPtr[1], wxConvUTF8);
					wxFileName fileName;
					fileName.Assign (wxFilePathName);
					
					if (!fileName.IsDirWritable ())
						{
								// Need to change the directory to the working output directory
								
						fileNamePtr = (FileStringPtr)supportFileStreamPtr->GetFileNameCPtr ();

						wxString workingDirectory = wxGetCwd ();
						workingDirectory.Append ("/");
						workingDirectory.Append (fileNamePtr);
						wxCharBuffer charWorkingDirectory = workingDirectory.ToAscii ();
						
								// Close the current file and reset the path name for new .sta file
								
						supportFileStreamPtr->SetFilePathFromCharString (
																(StringPtr)charWorkingDirectory.data (),
																TRUE);  // force current file to be closed
				
						}	// end "if (!fileName.IsDirWritable ())"
				#endif	// end "#ifndef NetBeansProject"
			#endif	// end "defined multispec_lin"
					
					// Check file already exists in the same location as the image file.
					// If not, then just write the file there. Otherwise prompt the user
					// for the file name and where it should go.
						
			if (continueFlag)
				{
				SetType (supportFileStreamPtr, kISTAFileType);
				
						// Create the file on the selected volume										
						
				errCode = CreateNewFile (supportFileStreamPtr, 
													GetVolumeReferenceNumber (supportFileStreamPtr),
													gCreator,
													kNoErrorMessages,
													kDoNotReplaceFlag);
													
				if (errCode != noErr)
					{
					errCode = PutFile (
								supportFileStreamPtr,
								numberBytes*gImageWindowInfoPtr->totalNumberChannels,
								IDS_SaveImageStats, 
								gCreator);
								
					continueFlag = ((errCode == noErr) && FileExists (supportFileStreamPtr));
					
					}	// end "if (errCode != noErr)"
				
				}	// end "if (continueFlag)"
			
			}	// end "if (channelIndex == 0)" 
		
				// Set position to start writing data at the start of the file		
		
		if (continueFlag)
			{
			count = (UInt32)channelIndex * numberBytes;
			errCode = MSetMarker (
								supportFileStreamPtr, fsFromStart, count, kErrorMessages);
			continueFlag = (errCode == noErr);
			
			}	// if (continueFlag) 
			
		else	// !continueFlag 
			{
			if (errCode != noErr)
				{	
						// Copy the old support file information back.
				
				InitializeFileStream (supportFileStreamPtr, &supportFileStreamCopy);	
				
				}	// end "if (errCode != noErr)" 
			
			}	// end "else !continueFlag" 
			
		}	// end "if (continueFlag && supportFileStreamPtr != NULL)" 
			
	if (continueFlag)
		{
				// Make cursor the watch cursor to indicate to the user that work	
				// is in process.																	
				
		MSetCursor (kWait);
	
				// Put writing stat file message in histogram status dialog.		
				
		if (histogramSpecsPtr->allChannelsAtOnceFlag)
			LoadDItemStringNumber (kHistogramStrID, 
											IDS_HistogramStatus3, 
											gStatusDialogPtr, 
											IDC_Status2, 
											(Str255*)gTextString);
			
				// Get the channel offset bytes to be used for the median array	
				// data.																				
				
		//channelOffsetCount = gImageWindowInfoPtr->numberBins;
	
		ioChanBufferPtr = histogramSpecsPtr->statBufferPtr;
		
		channelEnd = gImageWindowInfoPtr->totalNumberChannels;
		if (!histogramSpecsPtr->allChannelsAtOnceFlag)
			channelEnd = channelIndex + 1;
			
				// Set up swap bytes flag depending upon the system architecture.
				
		gSwapBytesFlag = gBigEndianFlag;
		
		for (channel=channelIndex; 
				channel<channelEnd; 
				channel++)
			{
					// Load in 'TRAIL74'.										
			
			BlockMoveData ((char*)erdasHeader, ioChanBufferPtr, 7);
	
					// Write channel number. 													
			
			theChar = (char)channel+1;
			BlockMoveData (&theChar, &ioChanBufferPtr[7], 1);
	
					// Write maximum value for band.											
	
			theChar = (char)(histogramSummaryPtr[channel].maxValue + signedValueOffset);
			BlockMoveData (&theChar, &ioChanBufferPtr[8], 1);
	
					// Write minimum value for band.											
	
			theChar = (char)(histogramSummaryPtr[channel].minValue + signedValueOffset);
			BlockMoveData (&theChar, &ioChanBufferPtr[9], 1);
	
					// Write zeros for unused bytes.											
	
			BlockMoveData ((char*)&twoBytesZero, &ioChanBufferPtr[10], 2);
	
					// Write channel mean.														
	
			tempLongInt = ConvertRealAT (
					(double)(histogramSummaryPtr[channel].averageValue + signedValueOffset));
			BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[12], 4);
	
					// Write zeros for mode for now.											
	
			BlockMoveData ((char*)&fourBytesZero, &ioChanBufferPtr[16], 4);
	
					// Write channel medium.													
	
			tempLongInt = ConvertRealAT (
					(double)(histogramSummaryPtr[channel].medianValue + signedValueOffset));
			BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[20], 4);
	
					// Write channel standard deviation.									
	
			tempLongInt = ConvertRealAT ((double)histogramSummaryPtr[channel].stdDeviation);
			BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[24], 4);
	
					// Write maximum value for band (two bytes format).				
	
			tempUInt16 = (UInt16)(histogramSummaryPtr[channel].maxValue + signedValueOffset);
      	tempInt = GetShortIntValue ((char*)&tempUInt16);
			BlockMoveData ((char*)&tempInt, &ioChanBufferPtr[28], 2);
	
					// Write minimum value for band (two bytes format).				
	
			tempUInt16 = (UInt16)(histogramSummaryPtr[channel].minValue + signedValueOffset);
      	tempInt = GetShortIntValue ((char*)&tempUInt16);
			BlockMoveData ((char*)&tempInt, &ioChanBufferPtr[30], 2);
			/*
					// Write 'VER2' (MultiSpec specific field)																
			
			BlockMoveData ((char*)"VER2", &ioChanBufferPtr[32], 4);
	
					// Write maximum non-saturated value for band (two bytes 		
					// format). (MultiSpec specific field)															
	
			tempUInt16 = (UInt16)(histogramSummaryPtr[channel].maxNonSatValue + 
																				signedValueOffset);
      	tempInt = GetShortIntValue ((char*)&tempUInt16);
			BlockMoveData ((char*)&tempInt, &ioChanBufferPtr[36], 2);
	
					// Write minimum non-saturated value for band (two bytes 		
					// format).	(MultiSpec specific field)																
	
			tempUInt16 = (UInt16)(histogramSummaryPtr[channel].minNonSatValue + 
																				signedValueOffset);
      	tempInt = GetShortIntValue ((char*)&tempUInt16);
			BlockMoveData ((char*)&tempInt, &ioChanBufferPtr[38], 2);
			
					// Write total number of values.	(MultiSpec specific field)										
					
			tempLongInt = ConvertRealAT (
						(double)histogramSpecsPtr->totalPixels);
			BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[40], 4);
			
					// Write total number of bad values. (MultiSpec specific field)								
					
			tempLongInt = ConvertRealAT (
						(double)histogramSummaryPtr[channel].badValues);
			BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[44], 4);
			*/
			/*
			if (gImageFileInfoPtr->dataTypeCode == kIntegerType)
				{
						// Write 'VE3I' (MultiSpec specific field)																
				
				BlockMoveData ((char*)"VE3I", &ioChanBufferPtr[32], 4);
		
						// Write maximum value for band (4-byte format). 
						// (MultiSpec specific field)															
		
				tempSInt32 = (SInt32)(histogramSummaryPtr[channel].maxValue + 
																					signedValueOffset);
	      	tempLongInt = GetLongIntValue ((char*)&tempSInt32);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[36], 4);
		
						// Write maximum non-saturated value for band (4-byte format). 	
						// (MultiSpec specific field)															
		
				tempSInt32 = (SInt32)(histogramSummaryPtr[channel].maxNonSatValue + 
																					signedValueOffset);
	      	tempLongInt = GetLongIntValue ((char*)&tempSInt32);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[44], 4);
		
						// Write minimum value for band (4-byte format).			
						// (MultiSpec specific field)																
		
				tempSInt32 = (SInt32)(histogramSummaryPtr[channel].minValue + 
																					signedValueOffset);
	      	tempLongInt = GetLongIntValue ((char*)&tempSInt32);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[52], 4);
		
						// Write minimum non-saturated value for band (4-byte format).		
						// (MultiSpec specific field)																
		
				tempSInt32 = (SInt32)(histogramSummaryPtr[channel].minNonSatValue + 
																					signedValueOffset);
	      	tempLongInt = GetLongIntValue ((char*)&tempSInt32);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[60], 4);
				
						// Write total number of values.	(MultiSpec specific field)										
						
				tempLongInt = GetLongIntValue ((char*)&histogramSpecsPtr->totalPixels);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[68], 4);
				
				}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"
			*/
			//else if (gImageFileInfoPtr->dataTypeCode == kRealType)
			//	{
						// Write 'VE3R' (MultiSpec specific field)																
				
				BlockMoveData ((char*)"VE3R", &ioChanBufferPtr[32], 4);
		
						// Write maximum value for band (8-byte real format). 
						// (MultiSpec specific field)															
		
				tempDoubleValue = histogramSummaryPtr[channel].maxValue + signedValueOffset;
	      	tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[36], 8);
		
						// Write maximum non-saturated value for band (8-byte real format). 	
						// (MultiSpec specific field)																
		
				tempDoubleValue = histogramSummaryPtr[channel].maxNonSatValue + 
																								signedValueOffset;
	      	tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);	
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[44], 8);	
		
						// Write minimum value for band (8-byte float format).			
						// (MultiSpec specific field)																
		
				tempDoubleValue = histogramSummaryPtr[channel].minValue + signedValueOffset;
	      	tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[52], 8);
		
						// Write minimum non-saturated value for band (8-byte real format).		
						// (MultiSpec specific field)																
		
				tempDoubleValue = histogramSummaryPtr[channel].minNonSatValue + 
																								signedValueOffset;
	      	tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[60], 8);
				
						// Write total number of values.	(MultiSpec specific field)										
						
				tempLongInt = GetLongIntValue ((char*)&histogramSpecsPtr->totalPixels);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[68], 4);
	
						// Write channel average value for band (8-byte real format).								
						// (MultiSpec specific field)																						
		
				tempDoubleValue = histogramSummaryPtr[channel].averageValue + 
																								signedValueOffset;
				tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[82], 8);
	
						// Write channel medium value for band (8-byte real format).					
						// (MultiSpec specific field)																								
		
				tempDoubleValue = histogramSummaryPtr[channel].medianValue + 
																								signedValueOffset;
				tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[90], 8);
		
						// Write channel standard deviationvalue for band (8-byte real format).			
						// (MultiSpec specific field)																						
		
				tempDoubleValue = histogramSummaryPtr[channel].stdDeviation;
				tempDoubleValue = GetDoubleValue ((UCharPtr)&tempDoubleValue);
				BlockMoveData ((char*)&tempDoubleValue, &ioChanBufferPtr[98], 8);
				
			//	}	// end "else if (gImageFileInfoPtr->dataTypeCode == kRealType)"
			
					// Write histogram to statistics file.									
					
			if (medianArrayPtr != NULL)
				{
				numberBins = histogramSummaryPtr[channel].numberBins;
				
						// Write histogram values compaction value.	
						
				if (numberBins <= 256)
					compaction = 1;
				
				else	// numberBins > 256 
					{						
					if (histogramSummaryPtr[channel].binType == kDataValueIsBinIndex)
						{
						startValue = (SInt32)(histogramSummaryPtr[channel].minNonSatValue +
																					signedValueOffset);
						endValue = (SInt32)(histogramSummaryPtr[channel].maxNonSatValue +
																					signedValueOffset);
						
						}	// end "...[channel].binType == kDataValueIsBinIndex"
						
					else	// ...[channel].binType != kDataValueIsBinIndex
						{
						startValue = 1;
						endValue = numberBins - 2;
						
						}	// end " else ...[channel].binType != kDataValueIsBinIndex"
						
					compaction = ((double)endValue - startValue + 1)/254;
						
					}	// end "else numberBins > 256"
						
				//compaction = (histogramSummaryPtr[channel].numberBins - 2 + 254)/254;
				//tempSInt32 = (SInt32)compaction;
				//tempLongInt = GetLongIntValue ((CharPtr)&tempSInt32);
				
				tempLongInt = ConvertRealAT (compaction);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[72], 4);
				/*
				if (gImageFileInfoPtr->dataTypeCode == kIntegerType)
					{
							// Also write the compaction value into the old location as a two
							// byte number for backward compatibility for older versions of
							// MultiSpec
							
					tempUInt16 = (UInt16)compaction;
	      		tempInt = GetShortIntValue ((CharPtr)&tempUInt16);
					BlockMoveData ((char*)&tempInt, &ioChanBufferPtr[48], 2);
						
					}	// end "if (gImageFileInfoPtr->dataTypeCode == kIntegerType)"
				*/
						// Write number of bins in original histogram.
					
				tempSInt32 = (SInt32)histogramSummaryPtr[channel].numberBins;
      		tempLongInt = GetLongIntValue ((CharPtr)&tempSInt32);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[76], 4);
				
						// Write the bin type
						//		kBinWidthOfOne
						//		kBinWidthNotOne
						//		kDataValueIsBinIndex
						
				tempUInt16 = (UInt16)histogramSummaryPtr[channel].binType;
      		tempInt = GetShortIntValue ((CharPtr)&tempUInt16);
				BlockMoveData ((char*)&tempInt, &ioChanBufferPtr[80], 2);
			
						// Set up channel pointer for median array.						
							
				ioTempBufferPtr = (UInt32*)&ioChanBufferPtr[128];
				
						// Write number of values for minimum data value.				
				
				numberDataValues = GetNumberOfMinimumDataValues (
																	&histogramSummaryPtr[channel],
																	medianArrayPtr,
																	signedValueOffset);
				tempLongInt = ConvertRealAT ((double)numberDataValues);
				BlockMoveData ((char*)&tempLongInt, (char*)ioTempBufferPtr, 4);
				ioTempBufferPtr++;
				
						// Write the number of values for the nonsaturated data		
						// values if they exist.	
					
				if (histogramSummaryPtr[channel].minNonSatValue != -signedValueOffset ||
										signedValueOffset == 0)
					{
					if (numberBins <= 256)
						{
						if (numberBins > 1)
							endValue = numberBins - 2;
							
						else	// numberBins == 1
							endValue = 0;
						
						for (imageValue=1; 
								imageValue<=endValue;	// 254; 
								imageValue++)
							{
							tempLongInt =
								ConvertRealAT ((double)medianArrayPtr[imageValue]);
							BlockMoveData ((char*)&tempLongInt, (char*)ioTempBufferPtr, 4);
							ioTempBufferPtr++;
							
							}	// end "for (imageValue=0; imageValue<=255; ..."
							           
						tempLongInt = 0;
						tempLongInt = ConvertRealAT ((double)tempLongInt);
						for (imageValue=endValue+1; 
								imageValue<=254;	// 254; 
								imageValue++)
							{                
							BlockMoveData ((char*)&tempLongInt, (char*)ioTempBufferPtr, 4);
							ioTempBufferPtr++;
							
							}	// end "for (imageValue=0; imageValue<=255; ..." 
							
						}	// end "if (gImageWindowInfoPtr->...)" 
					
					else	// numberBins > 256 
						{
						count = 0;
							
						if (compaction <= 1)
							{
									// Just fill values in order.  There will be blank ones of
									// 0 fill at end.
									
							for (binIndex=startValue; binIndex<=endValue; binIndex++)
								{
								tempLongInt = medianArrayPtr[binIndex];
								tempLongInt = ConvertRealAT ((double)tempLongInt);
								BlockMoveData ((char*)&tempLongInt, (char*)ioTempBufferPtr, 4);
								ioTempBufferPtr++;
								count++;
								
								}	// end "for (binIndex=startValue; ..."
								
							tempLongInt = 0;
							tempLongInt = ConvertRealAT ((double)tempLongInt);
							while (count <= 254) 
								{
								BlockMoveData ((char*)&tempLongInt, (char*)ioTempBufferPtr, 4);
								ioTempBufferPtr++;
								count++;
								
								}	// end "for (binIndex=startValue; ..."
							
							}	// end "if (compaction <= 1)"
							
						else	// compaction > 1
							{
							tempLongInt = 0;
							compactionCount = 0;
							nextSTABinEnd = compaction;
							for (binIndex=startValue; binIndex<=endValue; binIndex++)
								{
								tempLongInt += medianArrayPtr[binIndex];
								compactionCount++;
								if (compactionCount > nextSTABinEnd || binIndex == endValue)
									{
											// Make sure we to not fill more than 254 elements in
											// ioTempBuffer.
											
									if (count >= 254)
										break;
											
									tempLongInt = ConvertRealAT ((double)tempLongInt);
									BlockMoveData ((char*)&tempLongInt, (char*)ioTempBufferPtr, 4);
									ioTempBufferPtr++;
									tempLongInt = 0;
									nextSTABinEnd += compaction;
									count++;
									
									}	// end "if (compactionCount > nextSTABinEnd || ...)"
								
								}	// end "for (binIndex=startValue; ..." 
								
							}			// end "else compaction > 1"
							
						}	// end "else ...->numberBins > ..." 
						
					}	// end "if (histogramSummaryPtr[channel]..." 
				
						// Write number of values for maximum data value.					
				
				numberDataValues = GetNumberOfMaximumDataValues (
																	&histogramSummaryPtr[channel],
																	medianArrayPtr,
																	signedValueOffset);
				tempLongInt = ConvertRealAT ((double)numberDataValues);
				BlockMoveData ((char*)&tempLongInt, &ioChanBufferPtr[1148], 4);
				
				medianArrayPtr += histogramSpecsPtr->maxNumberBins;
					
				}	// end "if (medianArrayPtr != NULL)" 
				
					// Write buffer of data	to statistics file							
						
			count = numberBytes;
			errCode = MWriteData (
							supportFileStreamPtr, &count, ioChanBufferPtr, kErrorMessages);
			if (errCode != noErr)
				{
				continueFlag = FALSE;
				break;
				
				}	// end "if (errCode != noErr)" 
			
			}	// end "for (channel=channelIndex; ..." 
			
		}	// end "if (continueFlag) 													
		
	return (continueFlag);

}	// end "CreateSTASupportFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetNumberOfMaximumDataValues
//
//	Software purpose:	This purpose of this routine is to get the number of maximum
//							data values.  The location of the number depends on the number
//							of bytes of data.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	Count of the number of maximum data values	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2006
//	Revised By:			Larry L. Biehl			Date: 03/25/2006

UInt32 GetNumberOfMaximumDataValues (
				HistogramSummaryPtr				histogramSummaryPtr,
				HUInt32Ptr							histogramArrayPtr,
				SInt32								signedValueOffset)

{
	SInt32								maximumBinIndex;
	
	
	maximumBinIndex = histogramSummaryPtr->numberBins - 1;
	
	if (histogramSummaryPtr->binType == kDataValueIsBinIndex &&
														histogramSummaryPtr->numberBins > 256)
		maximumBinIndex = (SInt32)(histogramSummaryPtr->maxValue + signedValueOffset);
		
	maximumBinIndex = MAX (0, maximumBinIndex);
	maximumBinIndex = MIN (maximumBinIndex, (SInt32)histogramSummaryPtr->numberBins-1);
		
	return (histogramArrayPtr[maximumBinIndex]);

}	// end "GetNumberOfMaximumDataValues" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetNumberOfMinimumDataValues
//
//	Software purpose:	This purpose of this routine is to get the number of minimum
//							data values.  The location of the number depends on the number
//							of bytes of data.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	Count of the number of minimum data values	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2006
//	Revised By:			Larry L. Biehl			Date: 03/25/2006

UInt32 GetNumberOfMinimumDataValues (
				HistogramSummaryPtr				histogramSummaryPtr,
				HUInt32Ptr							histogramArrayPtr,
				SInt32								signedValueOffset)

{
	SInt32								minimumBinIndex = 0;
	
	
	if (histogramSummaryPtr->binType == kDataValueIsBinIndex && 
													histogramSummaryPtr->numberBins > 256)
		minimumBinIndex = (SInt32)(histogramSummaryPtr->minValue + signedValueOffset);
		
	minimumBinIndex = MAX (0, minimumBinIndex);
	minimumBinIndex = MIN (minimumBinIndex, (SInt32)histogramSummaryPtr->numberBins-1);
		
	return (histogramArrayPtr[minimumBinIndex]);

}	// end "GetNumberOfMinimumDataValues" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DecodeSTASupportFile
//
//	Software purpose:	This routine decodes the information that was 
//							previously read from an ERDAS statistics file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			EqualAreaDataToDisplayLevels in displayMultispectral.c
//							GetClippedMinMaxValues in displayMultispectral.c
//							ReadERDAS_staFile in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 07/19/1990
//	Revised By:			Larry L. Biehl			Date: 12/04/2014

Boolean DecodeSTASupportFile (
				HistogramSpecsPtr					histogramSpecsPtr, 
				HUInt32Ptr							histogramArrayPtr, 
				CMFileStream*						supportFileStreamPtr, 
				UInt16*								channelListPtr, 
				SInt16								numberChannels, 
				Boolean								summaryFlag)

{
	double								compactionFactor,
											maxNonSatIndexValue,
											minNonSatIndexValue,
											nextBinForSTABinRead,
											tempDoubleValue;
	
	HistogramSummaryPtr				histogramSummaryPtr;
	
	HPtr									statBufferPtr;
		
	HSInt32Ptr							ioChanBufferPtr;
	
	HUInt32Ptr							savedHistogramArrayPtr;
	
	SInt32								compactionCount,
											count,
											numberBinsPerInputDataValue,
											numberBytesPerChannel,
											signedValueOffset,
											tempSInt32Value,
											totalCount;
											
	UInt32								binIndex,
											countHistogramFileVectorReads,
											endIndex,
											maxHistogramIndexValue,
											maxImageIndexValue,
											numberBins;
	
	SInt16								channel,
											index,
											versionCode;
										
	UInt16								tempUInt16Value;
	
	Boolean								okToReadFlag;
	
	char									theChar;

	
			
			// Check input values.																
			
	if (supportFileStreamPtr == NULL ||
				histogramSpecsPtr == NULL ||
						channelListPtr == NULL)
																					return (FALSE);
																					
	if (!summaryFlag && histogramArrayPtr == NULL)
																					return (FALSE);
																					
	if (histogramSpecsPtr->statBufferPtr == NULL)
																					return (FALSE);
																						
			// Initialize local variables.													
			
	statBufferPtr = histogramSpecsPtr->statBufferPtr;
	if (gImageFileInfoPtr->numberBits <= 16)
		maxImageIndexValue = (UInt32)(gImageFileInfoPtr->maxUsableDataValue - 
														gImageFileInfoPtr->minUsableDataValue);
	else	// gImageFileInfoPtr->numberBits > 16
		maxImageIndexValue = UInt16_MAX;
	signedValueOffset = gImageFileInfoPtr->signedValueOffset;
	maxHistogramIndexValue = MIN (255, maxImageIndexValue);
	numberBytesPerChannel = 9 * 128;
	channel = 0;
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	savedHistogramArrayPtr = histogramArrayPtr;
	
	if (summaryFlag)
		{
		gImageFileInfoPtr->maxNumberEDecimalDigits = 0;
		gImageFileInfoPtr->maxNumberFDecimalDigits = 0;
		gImageFileInfoPtr->maxDataValue = -DBL_MAX;
		gImageFileInfoPtr->minDataValue = DBL_MAX;
		
		}	// end "if (summaryFlag)"
			
			// Set up swap bytes flag depending upon the system architecture.
			
	gSwapBytesFlag = gBigEndianFlag;
			
	for (index=0; 
			index<numberChannels; 
			index++)
		{
		channel = channelListPtr[index];
		
				// Determine whether this is modified version, i.e. 'VER2'.			
			
		versionCode = 0;           
		if (strncmp (&statBufferPtr[32], "VER2", 4) == 0)
			versionCode = 1;         
		else if (strncmp (&statBufferPtr[32], "VE3I", 4) == 0)
			versionCode = 2;        
		else if (strncmp (&statBufferPtr[32], "VE3R", 4) == 0)
			versionCode = 3;
		
				// Get the histogram summary information if requested.				
				
		if (summaryFlag)
			{
					// Get channel number. 														
	
			BlockMoveData (&statBufferPtr[7], &theChar, 1);
	
					// Get channel mean.															
	
			histogramSummaryPtr[channel].averageValue = 
									(double)ConvertATRealtoReal (&statBufferPtr[12]);
			histogramSummaryPtr[channel].averageValue -= signedValueOffset;
	
					// Get channel medium.														
	
			histogramSummaryPtr[channel].medianValue = 
									(double)ConvertATRealtoReal (&statBufferPtr[20]);
			histogramSummaryPtr[channel].medianValue -= signedValueOffset;
	
					// Get channel standard deviation.										
	
			histogramSummaryPtr[channel].stdDeviation = 
									(double)ConvertATRealtoReal (&statBufferPtr[24]);
	
					// Get maximum value for band (two bytes format).					
			
			tempUInt16Value = (UInt16)GetShortIntValue (&statBufferPtr[28]);
			histogramSummaryPtr[channel].maxValue = (double)tempUInt16Value;
			histogramSummaryPtr[channel].maxValue -= signedValueOffset;
	
					// Get minimum value for band (two bytes format).	
									
			tempUInt16Value = (UInt16)GetShortIntValue (&statBufferPtr[30]);
			histogramSummaryPtr[channel].minValue = (double)tempUInt16Value;
			histogramSummaryPtr[channel].minValue -= signedValueOffset;
			
					// Initialize values for minimum and maximum non-saturated		
					// values.																		
					
			histogramSummaryPtr[channel].maxNonSatValue = SInt32_MAX;
			histogramSummaryPtr[channel].minNonSatValue = SInt32_MAX;
			

			if (versionCode == 0)		
				{
				histogramSummaryPtr[channel].numberBins = 256;
				histogramSummaryPtr[channel].binType = kDataValueIsBinIndex;
									
				}	// end "if (versionCode == 0)"

			else if (versionCode == 1)
				{
						// Get the maximum non saturated value for the band.			
				
				tempUInt16Value = (UInt16)GetShortIntValue (&statBufferPtr[36]);
				histogramSummaryPtr[channel].maxNonSatValue = (double)tempUInt16Value;
				histogramSummaryPtr[channel].maxNonSatValue -= signedValueOffset;
				
						// Get the minimum non saturated value for the band.			
				
				tempUInt16Value = (UInt16)GetShortIntValue (&statBufferPtr[38]);
				histogramSummaryPtr[channel].minNonSatValue = (double)tempUInt16Value;
				histogramSummaryPtr[channel].minNonSatValue -= signedValueOffset;
										
						// Get total number of values.										
						
				if (histogramSpecsPtr->totalPixels == 0)
					histogramSpecsPtr->totalPixels = 
														ConvertATRealtoInt (&statBufferPtr[40]);
				
						// Get total number of bad values.									
						
				histogramSummaryPtr[channel].badValues =  
														ConvertATRealtoInt (&statBufferPtr[44]);
														
				histogramSummaryPtr[channel].numberBins = gImageWindowInfoPtr->numberBins;
				//histogramSummaryPtr[channel].binType = kBinWidthOfOne;
				histogramSummaryPtr[channel].binType = kDataValueIsBinIndex;
															
				}	// end "else if (versionCode == 1)" 

			else if (versionCode == 2)
				{
						// Get the maximum value for the band.			
				
				tempSInt32Value = (SInt32)GetLongIntValue (&statBufferPtr[36]);
				histogramSummaryPtr[channel].maxValue = (double)tempSInt32Value;
				histogramSummaryPtr[channel].maxValue -= signedValueOffset;
				
						// Get the maximum non saturated value for the band.			
				
				tempSInt32Value = (SInt32)GetLongIntValue (&statBufferPtr[44]);
				histogramSummaryPtr[channel].maxNonSatValue = (double)tempSInt32Value;
				histogramSummaryPtr[channel].maxNonSatValue -= signedValueOffset;
				
						// Get the minimum value for the band.			
				
				tempSInt32Value = (SInt32)GetLongIntValue (&statBufferPtr[52]);
				histogramSummaryPtr[channel].minValue = (double)tempSInt32Value;
				histogramSummaryPtr[channel].minValue -= signedValueOffset;
				
						// Get the minimum non saturated value for the band.			
				
				tempSInt32Value = (SInt32)GetLongIntValue (&statBufferPtr[60]);
				histogramSummaryPtr[channel].minNonSatValue = (double)tempSInt32Value;
				histogramSummaryPtr[channel].minNonSatValue -= signedValueOffset;
										
						// Get total number of values.										
						
				if (histogramSpecsPtr->totalPixels == 0)
					histogramSpecsPtr->totalPixels = 
														GetLongIntValue (&statBufferPtr[68]);
														
						// Get number of bins in original histogram
														
				histogramSummaryPtr[channel].numberBins = 
														GetLongIntValue (&statBufferPtr[76]);
														
						// Get the bin type		
				
				tempUInt16Value = (UInt16)GetShortIntValue (&statBufferPtr[80]);
				/*
						// We need to do some verification to make sure that the value
						// makes sense.  Old versions of the .sta file may not have this
						// value set.
						
				if (tempUInt16Value > kDataValueIsBinIndex)
					tempUInt16Value = 0;
				
				if (histogramSummaryPtr[channel].numberBins == 65536)
					tempUInt16Value = kDataValueIsBinIndex;
				
				else if (histogramSummaryPtr[channel].maxNonSatValue - 
									histogramSummaryPtr[channel].minNonSatValue > 2046)
					histogramSummaryPtr[channel].binType = kBinWidthNotOne;
				*/
				histogramSummaryPtr[channel].binType = tempUInt16Value;
				
				}	// end "else if (versionCode == 2)" 

			else if (versionCode == 3)
				{
						// Get the maximum value for the band.			
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[36]);
				histogramSummaryPtr[channel].maxValue = 
																tempDoubleValue - signedValueOffset;
				
						// Get the maximum non saturated value for the band.			
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[44]);
				histogramSummaryPtr[channel].maxNonSatValue = 
																tempDoubleValue - signedValueOffset;
				
						// Get the minimum value for the band.			
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[52]);
				histogramSummaryPtr[channel].minValue = 
																tempDoubleValue - signedValueOffset;
				
						// Get the minimum non saturated value for the band.			
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[60]);
				histogramSummaryPtr[channel].minNonSatValue = 
																tempDoubleValue - signedValueOffset;
										
						// Get total number of values.										
						
				if (histogramSpecsPtr->totalPixels == 0)
					histogramSpecsPtr->totalPixels = 
															GetLongIntValue (&statBufferPtr[68]);
														
						// Get number of bins in original histogram
														
				histogramSummaryPtr[channel].numberBins = 
															GetLongIntValue (&statBufferPtr[76]);
														
						// Get the bin type		
				
				tempUInt16Value = (UInt16)GetShortIntValue (&statBufferPtr[80]);
				histogramSummaryPtr[channel].binType = tempUInt16Value;	
				
						// Get the average value for the band.		
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[82]);
				histogramSummaryPtr[channel].averageValue = 
																tempDoubleValue - signedValueOffset;
				
						// Get the median value for the band.		
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[90]);
				histogramSummaryPtr[channel].medianValue = 
																tempDoubleValue - signedValueOffset;
				
						// Get the standard deviation value for the band.		
				
				tempDoubleValue = GetDoubleValue ((UCharPtr)&statBufferPtr[98]);
				histogramSummaryPtr[channel].stdDeviation = tempDoubleValue;
				
						// Force binType to always be "kBinWidthNotOne" for real data.
														
				//histogramSummaryPtr[channel].binType = kBinWidthNotOne;
															
				}	// end "else if (versionCode == 3)"
					
					// Make sure that the number of bins make sense relative to the data
					// itself.  We want to catch case when .sta file is for another image
					// which may have more data bins than the image that the .sta file is
					// being loaded for.
					
			histogramSummaryPtr[channel].numberBins = 
											MIN (histogramSpecsPtr->maxNumberBins, 
													histogramSummaryPtr[channel].numberBins);
		
					// Update summary data if it is of the older format.				
					
			if (histogramSummaryPtr[channel].maxNonSatValue == SInt32_MAX)
				histogramSummaryPtr[channel].maxNonSatValue = 
														histogramSummaryPtr[channel].maxValue;
			
			double  minLimitValue = (double)(0 - 2147483647); // SInt32_MIN;
			if (histogramSummaryPtr[channel].minNonSatValue == minLimitValue)
				histogramSummaryPtr[channel].minNonSatValue = 
														histogramSummaryPtr[channel].minValue;
														
					// Get the bin width factor.
					// Bin width is 1 for kDataValueIsBinIndex and kDataValueIsBinIndex
			
			histogramSummaryPtr[channel].binFactor = 1; 
			if (histogramSummaryPtr[channel].binType == kBinWidthNotOne &&
						(histogramSummaryPtr[channel].maxNonSatValue - 
												histogramSummaryPtr[channel].minNonSatValue) > 0)
				histogramSummaryPtr[channel].binFactor = 
						(double)(histogramSummaryPtr[channel].numberBins-3)/
					 			(histogramSummaryPtr[channel].maxNonSatValue - 
					 								histogramSummaryPtr[channel].minNonSatValue);
																											
					// Get the number of decimal digits to use when listing data
					// values.
			
			GetNumberDecimalDigits (gImageFileInfoPtr->dataTypeCode,
											histogramSummaryPtr[channel].minValue,
											histogramSummaryPtr[channel].maxValue,
											1/histogramSummaryPtr[channel].binFactor,
											&histogramSummaryPtr[channel].numberEDecimalDigits,
											&histogramSummaryPtr[channel].numberFDecimalDigits);
				
			gImageFileInfoPtr->maxNumberEDecimalDigits = 
											MAX (gImageFileInfoPtr->maxNumberEDecimalDigits,
													histogramSummaryPtr[channel].numberEDecimalDigits);
				
			gImageFileInfoPtr->maxNumberFDecimalDigits = 
											MAX (gImageFileInfoPtr->maxNumberFDecimalDigits,
													histogramSummaryPtr[channel].numberFDecimalDigits);
					 								
			gImageFileInfoPtr->maxDataValue = 
					MAX (gImageFileInfoPtr->maxDataValue, histogramSummaryPtr[channel].maxValue);
			gImageFileInfoPtr->minDataValue = 
					MIN (gImageFileInfoPtr->minDataValue, histogramSummaryPtr[channel].minValue);
											
			histogramSummaryPtr[channel].availableFlag = TRUE;
				
			}	// end "if (summaryFlag)" 
				
				// Read histogram stastistics if needed.									
				// Set up channel pointer for median array.								
		
		if (histogramArrayPtr != NULL)
			{
					// Get the compaction factor.												
			
			compactionFactor = 0;
			if (versionCode == 1)
				compactionFactor = (UInt32)GetShortIntValue (&statBufferPtr[48]);
			else if (versionCode > 1)
				compactionFactor = (double)ConvertATRealtoReal (&statBufferPtr[72]);
			
			numberBins = histogramSummaryPtr[channel].numberBins;
				
			ioChanBufferPtr = (SInt32*)&statBufferPtr[128];
		
			if (numberBins <= 256 || compactionFactor == 0)
				{
				for (binIndex=0; binIndex<=maxHistogramIndexValue; binIndex++)
					{
					*histogramArrayPtr = ConvertATRealtoInt ((char*)ioChanBufferPtr);
					ioChanBufferPtr++;
					histogramArrayPtr++;
				
					}	// end "for (binIndex=0; ..." 
					
						// Zero out the rest of the histogram array, if needed.		
						
				if (numberBins > maxHistogramIndexValue+1)
					{
					for (binIndex=maxHistogramIndexValue+1; 
							binIndex<numberBins; 
							binIndex++)
						{
						*histogramArrayPtr = 0;
						histogramArrayPtr++;
						
						}	// end "for (binIndex=maxHistogramIndexValue+1; ..." 
						
					}	// end "if (numberBins > maxHistogramIndexValue+1)"
					
				}	// end "if (numberBins <= 256 || ...)" 
					
			else	// numberBins > 256 && ...
				{
						// Make certain that we have the correct min and max non-	
						// saturated data values.												
						
				minNonSatIndexValue = 
						histogramSummaryPtr[channel].minNonSatValue + signedValueOffset;
				maxNonSatIndexValue = 
						histogramSummaryPtr[channel].maxNonSatValue + signedValueOffset;
				
						// Make certain that the min and max non-saturated data values	
						// are consistant with number of bins expected in the				
						// histogram array.  It could be different, if the statistics	
						// file was created assuming that the image data was more than 
						// 8 bit data and the image file has now been defined to be 8	
						// bit data.																	
						
				minNonSatIndexValue = MIN (minNonSatIndexValue, maxImageIndexValue-1);
				maxNonSatIndexValue = MIN (maxNonSatIndexValue, maxImageIndexValue-1);
				
						// Get the number of minimum data values.							
						
				*histogramArrayPtr = ConvertATRealtoInt ((CharPtr)ioChanBufferPtr);
				ioChanBufferPtr++;
				countHistogramFileVectorReads = 1;
				histogramArrayPtr++;
				
				binIndex = 1;
				endIndex = numberBins - 2;
				okToReadFlag = TRUE;
				if (histogramSummaryPtr[channel].binType == kDataValueIsBinIndex)
					{
							// Zero out the histogram value array up to the minimum		
							// non saturated data value.				
							
					if (minNonSatIndexValue >= 0)							
						{
						while (binIndex < (UInt32)minNonSatIndexValue && binIndex <= endIndex)	
							{
							*histogramArrayPtr = 0;
							histogramArrayPtr++;
							binIndex++;
							
							}	// end "while (binIndex < minNonSatIndexValue && ..."
							
						}	// end "if (minNonSatIndexValue >= 0)" 
					
					if (maxNonSatIndexValue > 0 && maxNonSatIndexValue <= numberBins - 2)
						endIndex = (UInt32)maxNonSatIndexValue;
					
					else	// maxNonSatIndexValue <= 0 || ...
						okToReadFlag = FALSE;
						
					}	// end "if (...[channel].binType == kDataValueIsBinIndex)"
					
						// Get the number of data values from min-nonsaturated		
						// to max-nonsaturated taking the compaction factor into		
						// account.																	
				
				if (okToReadFlag && maxNonSatIndexValue < maxImageIndexValue)
					{
					if (compactionFactor <= 1)
						{
								// Note that if the compaction Factor is less than one then,
								// the data values are just loaded in order.  Force
								// compactionFactor to be one.
								
						while (binIndex <= endIndex)
							{
							*histogramArrayPtr = ConvertATRealtoInt ((char*)ioChanBufferPtr);
							ioChanBufferPtr++;
							histogramArrayPtr++;
							
							binIndex++;
						
							}	// end "while (binIndex <= endIndex)" 
						
						}	// end "if (compactionFactor <= 1)"
						
					else	// compactionFactor > 1
						{	
						totalCount = ConvertATRealtoInt ((CharPtr)ioChanBufferPtr);
						ioChanBufferPtr++;
						countHistogramFileVectorReads++;
						
						count = totalCount/(UInt32)compactionFactor;
						if (totalCount > 0)
							count = MAX (count, 1);
						
						compactionCount = 0;
						nextBinForSTABinRead = compactionFactor;	
						while (binIndex <= endIndex)
							{
							if (compactionCount >= (SInt32)nextBinForSTABinRead &&
																	countHistogramFileVectorReads < 255)
								{
								*histogramArrayPtr = totalCount;
								compactionCount++;
								
								nextBinForSTABinRead += compactionFactor;
							
								totalCount = ConvertATRealtoInt ((CharPtr)ioChanBufferPtr);
								ioChanBufferPtr++;
								countHistogramFileVectorReads++;
						
								numberBinsPerInputDataValue =
														(SInt32)(nextBinForSTABinRead-compactionCount);
								if (numberBinsPerInputDataValue > 0)
									count = totalCount/numberBinsPerInputDataValue;
								else	// numberBinsPerInputDataValue <= 0
									count = totalCount;

								if (totalCount > 0)
									count = MAX (count, 1);
								
								}	// end "if (compactionCount >= compactionFactor)" 
							
							else	// compactionCount < (UInt32)nextBinForSTABinRead || ...
								{
								if (totalCount > count)
									totalCount -= count;
									
								else	// totalCount <= count
									{
									count = totalCount;
									totalCount = 0;
									
									}	// end "else totalCount <= count"
									
								*histogramArrayPtr = count;
								compactionCount++;
								
								}	// end "else compactionCount < (UInt32)nextBinForSTABinRead" 
								
							histogramArrayPtr++;
							binIndex++;
							
							}	// end "while (binIndex <= endIndex)" 
							
						}	// end "else compactionFactor > 1"
						
					}	// end "if (okToReadFlag && ...)" 
					
						// If the count left has not been used (which is in totalCount),
						// add it to the last bin used.
						
				if (totalCount > 0)
					{
					histogramArrayPtr--;
					*histogramArrayPtr += totalCount;
					histogramArrayPtr++;
					totalCount = 0;
					
					}	// end "if (totalCount > 0)"
				
						// Zero out the histogram value array up to (but not including)	
						// the maximum data value.								
				
				while (binIndex < numberBins-1)
					{
					*histogramArrayPtr = 0;
					histogramArrayPtr++;
					binIndex++;
					
					}	// end "while (binIndex < numberBins-1)" 
				
				}	// end "else numberBins > 256 ..." 
				
			SetNumberOfMinimumDataValuesInArray (&histogramSummaryPtr[channel],
																savedHistogramArrayPtr,
																signedValueOffset);
			
					// Get the number of maximum data values.						
					
			savedHistogramArrayPtr[numberBins-1] = 
													ConvertATRealtoInt (&statBufferPtr[1148]);
			
			SetNumberOfMaximumDataValuesInArray (&histogramSummaryPtr[channel],
																savedHistogramArrayPtr,
																signedValueOffset);
				
			savedHistogramArrayPtr += histogramSpecsPtr->maxNumberBins;
			histogramArrayPtr = savedHistogramArrayPtr;
				
			}	// end "if (histogramArrayPtr != NULL)" 
		
		statBufferPtr += numberBytesPerChannel;
		
		channel++;
		
		}	// end "if (index=0; ..." 
		
	return (TRUE);
			
}	// end "DecodeSTASupportFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 DetermineBytesForHistogramText
//
//	Software purpose:	The purpose of this routine is to determine the
//							approximate number of bytes in the histogram text 
//							output.  If the estimate is for the text output window,
//							allow for overhead for each line of data and
//							each control character.
//
//	Parameters in:		Pointer to the histogram instruction structure.
//
//	Parameters out:	None
//
//	Value Returned:  Number of bytes needed for text output
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1993	
//	Revised By:			Larry L. Biehl			Date: 07/19/2006

UInt32 DetermineBytesForHistogramText (
				HistogramSpecsPtr					histogramSpecsPtr, 
				Boolean								textWindowFlag)

{
	UInt32								bytesNeeded,
											estimatedNumberOfBins,
											numberControlCharacters,
											numberLines;


	bytesNeeded = 0;
	numberControlCharacters = 0;
	numberLines = 0;
	
	if (histogramSpecsPtr->listHistSummaryFlag)
		{
		bytesNeeded += 300;
		bytesNeeded += histogramSpecsPtr->numberChannels * 100;
		
		numberLines += histogramSpecsPtr->numberChannels + 10;
		
		numberControlCharacters += 10 + 
							histogramSpecsPtr->numberChannels * 5;
		
		}	// end "if (...->histogramSpecs.listHistSummaryFlag)" 

	if (histogramSpecsPtr->listHistFlag)
		{
		estimatedNumberOfBins = 256;
		if (gImageWindowInfoPtr->numberBytes == 2)
			estimatedNumberOfBins = gImageWindowInfoPtr->numberBins/2;
		else if (gImageWindowInfoPtr->numberBytes > 2)
			estimatedNumberOfBins = gImageWindowInfoPtr->numberBins;
		
				// Allow for header lines.														
		
		bytesNeeded += 50;		
		numberLines += 2;
		numberControlCharacters += 2;
		
		bytesNeeded += (estimatedNumberOfBins + 1) *
							(histogramSpecsPtr->numberChannels+1) * 5;
		
		if (histogramSpecsPtr->lineFormatHistFlag)
			numberLines += histogramSpecsPtr->numberChannels + 1;
			
		else	// !histogramSpecsPtr->lineFormatHistFlag 
			numberLines += estimatedNumberOfBins + 1;
		
		numberControlCharacters += (estimatedNumberOfBins + 1) *
							(histogramSpecsPtr->numberChannels+1);
		
		}	// end "if (histogramSpecsPtr->listHistFlag)" 
		
	if (textWindowFlag)
		{
		bytesNeeded += numberLines * 10;	// Assume 10 for WASTE.
		
		bytesNeeded += numberControlCharacters * 6;
		
		}	// end "if (textWindowFlag)" 
		
	return (bytesNeeded);
			
}	// end "DetermineBytesForHistogramText" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DetermineMinAndMaxValuesForEachChannel
//
//	Software purpose:	The purpose of this routine is to get the requested
//							histogram information, save the histogram if
//							requested and list the information in the output
//							text window.
//
//	Parameters in:		Pointer to the histogram instruction structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			HistogramControl   in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 05/22/2003	
//	Revised By:			Larry L. Biehl			Date: 01/04/2006

Boolean DetermineMinAndMaxValuesForEachChannel (
				FileIOInstructionsPtr			fileIOInstructionsPtr,  
				HistogramSpecsPtr					histogramSpecsPtr,
				HistogramSummaryPtr				histogramSummaryPtr,
				UInt16								*channelListPtr,
				SInt16								channelOffset,
				double								maxSaturatedValue,
				Boolean								BISFlag,
				UInt32								interval,
				UInt32								numberSamples,
				UInt16								imageFileNumberChannels)

{
	double*								ioBuffer8Ptr;
											    
	double								dataValue;
	
	UInt32								columnEnd,
											columnInterval,
											columnStart,
											index,
											line,
											lineEnd,
											lineInterval,
											lineStart; 
	
	SInt16								chanIndex,
											errCode = noErr;
	
	UInt16								channel;
											
	#if defined multispec_mac
		Pattern								black;
	#endif	// defined multispec_mac    
	
				
	columnStart = histogramSpecsPtr->columnStart;
	columnEnd = histogramSpecsPtr->columnEnd;
	columnInterval = histogramSpecsPtr->columnInterval;
	lineStart = histogramSpecsPtr->lineStart;
	lineEnd = histogramSpecsPtr->lineEnd;
	lineInterval = histogramSpecsPtr->lineInterval;
	
	//integerFlag = FALSE;
	//if (fileIOInstructionsPtr->forceByteCode == kForce4Bytes)
	//	integerFlag = TRUE;

			// Read line of image as specified by HistogramSpecs.				

	for (line=lineStart; line<=lineEnd; line+=lineInterval)
		{
				// Get all channels for the line of image data.  Return 		
				// if there is a file IO error.										
			 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line, 
											columnStart,
											columnEnd,
											columnInterval,
											gInputBufferPtr,
											gOutputBufferPtr);

		if (errCode != noErr)
			{			
			gConvertSignedDataFlag = FALSE;	
			histogramSpecsPtr->loadedFlag = FALSE;					
																							return (FALSE);
																		
			}	// end "if (errCode != noErr)" 
	
				// This part of the program determines the min and max value for
				// the requested channels in this particular file.																
				
		for (channel=0; 
				channel<imageFileNumberChannels; 
				channel++)
			{
			chanIndex = channelListPtr[channelOffset+channel];
			/*
			if (integerFlag)
				{
				ioBuffer4Ptr = (HSInt32Ptr)gOutputBufferPtr;
				
				if (BISFlag)
						// File band interleave format is BIS; adjust the			
						// buffer pointer to start at the channel being 			
						// processed.															
						
					ioBuffer4Ptr = &ioBuffer4Ptr[chanIndex];
			
				else	// !BISFlag
					ioBuffer4Ptr = &ioBuffer4Ptr[channel*numberSamples];
				
				dataValue = (double)ioBuffer4Ptr[0];
				//histogramSummaryPtr[chanIndex].maxNonSatValue = dataValue;
				//histogramSummaryPtr[chanIndex].maxValue = dataValue;
				//histogramSummaryPtr[chanIndex].minNonSatValue = dataValue;
				//histogramSummaryPtr[chanIndex].minValue = dataValue;
			
				for (index=0; index<numberSamples; index+=interval)
					{
					dataValue = (double)ioBuffer4Ptr[index];
					
							// Get the maximum data values.	
							
					if (dataValue != histogramSummaryPtr[chanIndex].maxValue)
						{
						if (dataValue > histogramSummaryPtr[chanIndex].maxValue)
							{
							histogramSummaryPtr[chanIndex].maxNonSatValue =
															histogramSummaryPtr[chanIndex].maxValue;
										
							histogramSummaryPtr[chanIndex].maxValue = dataValue;
							
							}	// end "if (dataValue > ...[chanIndex].maxValue)"
							
						else if (dataValue > histogramSummaryPtr[chanIndex].maxNonSatValue &&
											dataValue != histogramSummaryPtr[chanIndex].maxValue)
							histogramSummaryPtr[chanIndex].maxNonSatValue = dataValue;
							
						}	// end "if (dataValue != ...[chanIndex].maxValue)"	
					
							// Get the minimum data values.		
							
					if (dataValue != histogramSummaryPtr[chanIndex].minValue)
						{
						if (dataValue < histogramSummaryPtr[chanIndex].minValue)
							{
							histogramSummaryPtr[chanIndex].minNonSatValue =
															histogramSummaryPtr[chanIndex].minValue;
										
							histogramSummaryPtr[chanIndex].minValue = dataValue;
							
							}	// end "if (dataValue > ...[chanIndex].maxValue)"
							
						else if (dataValue < histogramSummaryPtr[chanIndex].minNonSatValue &&
												dataValue != histogramSummaryPtr[chanIndex].minValue)
							histogramSummaryPtr[chanIndex].minNonSatValue = dataValue;
							
						}	// end "if (dataValue != ...[chanIndex].minValue)"
						
							// Compute the channel sum.								
						
					histogramSummaryPtr[chanIndex].averageValue += dataValue;

							// Compute the sum of the data value square for		
							// each channel												

					histogramSummaryPtr[chanIndex].stdDeviation +=  dataValue * dataValue;
		
					}	// end "for (index=0; ..." 
					
				}	// end "if (integerFlag)"
			
			else	// !integerFlag
				{ 
			*/
				ioBuffer8Ptr = (double*)gOutputBufferPtr;
				
				if (BISFlag)
						// File band interleave format is BIS; adjust the			
						// buffer pointer to start at the channel being 			
						// processed.															
						
					ioBuffer8Ptr = &ioBuffer8Ptr[chanIndex];
			
				else	// !BISFlag
					ioBuffer8Ptr = &ioBuffer8Ptr[channel*numberSamples];
			
				for (index=0; index<numberSamples; index+=interval)
					{
					dataValue = ioBuffer8Ptr[index];
					
							// Get the maximum data values.	
							
					if (dataValue > histogramSummaryPtr[chanIndex].maxValue)
						{
						histogramSummaryPtr[chanIndex].maxNonSatValue =
															histogramSummaryPtr[chanIndex].maxValue;
									
						histogramSummaryPtr[chanIndex].maxValue = dataValue;
						
						}	// end "if (dataValue > ...[chanIndex].maxValue)"
						
					else if (dataValue > histogramSummaryPtr[chanIndex].maxNonSatValue &&
											dataValue != histogramSummaryPtr[chanIndex].maxValue)
						histogramSummaryPtr[chanIndex].maxNonSatValue = dataValue;	
					
							// Get the minimum data values.	
							
					if (dataValue < histogramSummaryPtr[chanIndex].minValue)
						{
						histogramSummaryPtr[chanIndex].minNonSatValue =
															histogramSummaryPtr[chanIndex].minValue;
									
						histogramSummaryPtr[chanIndex].minValue = dataValue;
						
						}	// end "if (dataValue > ...[chanIndex].maxValue)"
						
					else if (dataValue < histogramSummaryPtr[chanIndex].minNonSatValue &&
											dataValue != histogramSummaryPtr[chanIndex].minValue)
						histogramSummaryPtr[chanIndex].minNonSatValue = dataValue;
							
							// Compute the channel sum.								
						
					histogramSummaryPtr[chanIndex].averageValue += dataValue;

							// Compute the sum of the data value square for		
							// each channel												

					histogramSummaryPtr[chanIndex].stdDeviation += dataValue * dataValue;
		
					}	// end "for (index=0; ..." 
					
			//	}	// end "if (integerFlag)"
				
			}	// end "for (channel=0; channel<..." 
			
				// Check if user wants to exit histogramming.					
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				histogramSpecsPtr->loadedFlag = FALSE;			
				gConvertSignedDataFlag = FALSE;	
																							return (FALSE);
					
				}	// end "if (!CheckSomeEvents (..." 
				
			}	// end "if (TickCount () >= nextTime)" 
			
		if (TickCount () >= gNextStatusTime)
			{
			gStatusGraphicsBox.right = (SInt16)gStatusGraphicsRight;

			#if defined multispec_mac
				if (gStatusProgressControlHandle != NULL)		
					SetControlValue (gStatusProgressControlHandle, 
											gStatusGraphicsBox.right);
				
				else	// gStatusProgressControlHandle == NULL
					FillRect (&gStatusGraphicsBox, GetQDGlobalsBlack (&black));
			#endif	// defined multispec_mac

			#if defined multispec_win
				((CProgressCtrl*)(gStatusDialogPtr->GetDlgItem (IDC_Status7)))->
																SetPos ((SInt32)gStatusGraphicsRight);
			#endif	// defined multispec_win

         #if defined multispec_lin
				//int gauge_range = ((wxGauge*) (gStatusDialogPtr->FindWindow (IDC_Status7)))->GetRange ();
				//gStatusGraphicsRight = MIN (gStatusGraphicsRight, gauge_range);
				/* int numberChars = sprintf (
						(char*)gTextString3,
						" SHistogram: DetermineMinAndMax... (gStatusGraphicsRight): %f%s",
						gStatusGraphicsRight,
						gEndOfLine);
				ListString ((char*)gTextString3, numberChars, gOutputTextH);
				*/
				((wxGauge*)(gStatusDialogPtr->FindWindow (IDC_Status7)))->SetValue ((SInt32)gStatusGraphicsRight);
				CheckSomeEvents (osMask+updateMask);
			#endif
			
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 

				// Update the new right location of the graphics box which
				// represents the portion of the computation that has been	
				// completed.																
				
		gStatusGraphicsRight += gStatusBoxIncrement;
						
		}	// end "line=lineStart; ..."
		
	return (TRUE);
			
}	// end "DetermineMinAndMaxValuesForEachChannel"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfEFormatRequired
//
//	Software purpose:	The purpose of this routine is to determine if E format is 
//							required for any of the min, max and median values for the histogram
//							summary.
//
//	Parameters in:		Pointer to the histogram instruction structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			HistogramControl   in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 03/21/2006	
//	Revised By:			Larry L. Biehl			Date: 03/24/2006

Boolean DetermineIfEFormatRequired (
				HistogramSummaryPtr				histogramSummaryPtr,
				UInt32								numberChannels,
				SInt16*								channelsPtr)

{
	double								absValue;
	
	UInt32								channelNum,
											index;
	
	Boolean								eFormatFlag = FALSE;
	
									
	for (index=0; 
			index<numberChannels; 
			index++)
		{
		channelNum = channelsPtr[index];
		
		if (histogramSummaryPtr[channelNum].availableFlag)
			{
			absValue = fabs (histogramSummaryPtr[channelNum].minValue);
			if (absValue != 0 &&
					(absValue < kMinValueToListWith_f || absValue > kMaxValueToListWith_f))
				eFormatFlag = TRUE;
			
			absValue = fabs (histogramSummaryPtr[channelNum].maxValue);
			if (absValue != 0 &&
					(absValue < kMinValueToListWith_f || absValue > kMaxValueToListWith_f))
				eFormatFlag = TRUE;
			
			absValue = fabs (histogramSummaryPtr[channelNum].medianValue);
			if (absValue != 0 &&
					(absValue < kMinValueToListWith_f || absValue > kMaxValueToListWith_f))
				eFormatFlag = TRUE;
								
			}	// end "if (...histogram[channelNum].availableFlag)" 
		
		}	// end "for (index=0; ... "
		
	return (eFormatFlag);
			
}	// end "DetermineIfEFormatRequired"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DoHistogramRequests
//
//	Software purpose:	The purpose of this routine is to get the requested
//							histogram information, save the histogram if
//							requested and list the information in the output
//							text window.
//
//	Parameters in:		Pointer to the histogram instruction structure.
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			HistogramControl   in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 10/14/1993	
//	Revised By:			Larry L. Biehl			Date: 05/02/2018

Boolean DoHistogramRequests (
				HistogramSpecsPtr					histogramSpecsPtr)

{
	Boolean								continueFlag,
											summaryFlag,
											twoPassFlag;
											
	CMFileStream*						resultsFileStreamPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	HistogramSummaryPtr				histogramSummaryPtr;
	HUInt32Ptr							histogramArrayPtr;
	UInt16*								channelsPtr;
	
	UInt32								numberLines;
	
	SInt16								forceByteCode,
											numberLoops,
											stringNumber;
	
	SInt16								cIndex,
											computeCode,
											loop;
											
	UInt16								index,
											numberChannels;
	
	
			// Initialize local variables.													
	
	fileIOInstructionsPtr = NULL;
	numberChannels = histogramSpecsPtr->numberChannels;
	histogramArrayPtr = histogramSpecsPtr->histogramArrayPtr; 						
	channelsPtr = (UInt16*)GetHandlePointer (histogramSpecsPtr->channelsHandle);
	
			// Get pointer to summary structure.
			
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	
	computeCode = 1;
	if (histogramArrayPtr != NULL)
		computeCode = 3;
	
	forceByteCode = kForce4Bytes;
	if (gImageFileInfoPtr->numberBytes >= 4)
		forceByteCode = kForceReal8Bytes;
		
	summaryFlag = TRUE;
	continueFlag = TRUE;
	
	twoPassFlag = !histogramSpecsPtr->allChannelsAtOnceFlag &&
											histogramSpecsPtr->listHistFlag;
	
	numberLoops = 1;		
	if (!histogramSpecsPtr->allChannelsAtOnceFlag)
		numberLoops = numberChannels;
		
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
				
			// Indicate that the image level to display level vector					
			// needs to be updated the next time this image is 						
			// displayed.																			
	
	if (histogramSpecsPtr->method == kComputeFromImage || 
							histogramSpecsPtr->method == kReadFromSupportFile)
		histogramSpecsPtr->histogramChangedSinceDisplayFlag = TRUE;
					
			// Now indicate that the statistics file will need to be					
			// read if the histogram values are to be listed.							 
		
	if (histogramSpecsPtr->method == kStoredInMemory && 
													histogramSpecsPtr->listHistFlag)
		histogramSpecsPtr->method = kReadFromSupportFile;
				
			// Intialize the nextTime variable to indicate when the next 			
			// check should occur for a command-.											
			
	gNextTime = TickCount ();
	gNextStatusTime = TickCount ();
	
	for (loop=0; loop<numberLoops; loop++)
		{
				// Get the requested histogram information, either by reading		
				// from a statistics disk file or computing from the image.			
				
				// If histogram method is 2, compute the histogram.					
					
		if (histogramSpecsPtr->method == kComputeFromImage)
			{
					// Get handle to block of memory to use as file IO buffer for 		
					// the requested channels of data.  Get a buffer that will handle	
					// 1 line of data for the image file that includes the most bytes	
					// for 1 line.																		
			
			if (loop == 0)	
				{
				if (continueFlag)
					{
					gConvertSignedDataFlag = TRUE;
					if (gImageWindowInfoPtr->numberBytes >= 4)
						gConvertSignedDataFlag = FALSE;
				
					continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
																	gImageWindowInfoPtr,
																	gImageLayerInfoPtr,
																	gImageFileInfoPtr,
																	&gInputBufferPtr, 
																	&gOutputBufferPtr,
																	histogramSpecsPtr->columnStart, 
																	histogramSpecsPtr->columnEnd, 
																	histogramSpecsPtr->columnInterval, 
																	(SInt16)numberChannels, 
																	channelsPtr,
																	kPackData, 
																	kDoNotForceBISFormat, 
																	forceByteCode,
																	kDoNotAllowForThreadedIO,
																	&fileIOInstructionsPtr);
					
					gConvertSignedDataFlag = FALSE;
												
					}	// end "if (continueFlag)"
				
				if (continueFlag)
					{					
							// Initialize the histogram summary values.					
							
					histogramSpecsPtr->loadedFlag = FALSE;
								
					for (index=0;
							index<numberChannels; 
							index++)
						{
						cIndex = channelsPtr[index];
						histogramSummaryPtr[cIndex].minValue = DBL_MAX;
						histogramSummaryPtr[cIndex].maxValue = -DBL_MAX;
						histogramSummaryPtr[cIndex].minNonSatValue = DBL_MAX;
						histogramSummaryPtr[cIndex].maxNonSatValue = -DBL_MAX;
						histogramSummaryPtr[cIndex].badValues = 0;
						histogramSummaryPtr[cIndex].stdDeviation = 0;
						histogramSummaryPtr[cIndex].averageValue = 0;
						histogramSummaryPtr[cIndex].medianValue = 0;
						histogramSummaryPtr[cIndex].availableFlag = FALSE;
					
						}	// end "for (index=0; ..."
						
							// Get the graphics status box increment.						
			
					numberLines = (histogramSpecsPtr->lineEnd - 
									histogramSpecsPtr->lineStart +
										histogramSpecsPtr->lineInterval)/
											histogramSpecsPtr->lineInterval;

					#if defined multispec_mac
						gStatusBoxIncrement = 
							(double)(gStatusGraphicsBox.right - gStatusGraphicsBox.left)/
																						numberLines;
					#endif	// defined multispec_mac

               #if defined multispec_win || defined multispec_lin
						gStatusGraphicsRight = numberLines;
						gStatusBoxIncrement = 1;
					#endif	// defined multispec_win
					
					if (!twoPassFlag)
						{																			
						if (!histogramSpecsPtr->allChannelsAtOnceFlag)					
							#if defined multispec_mac || defined multispec_mac_swift
								gStatusBoxIncrement /= histogramSpecsPtr->numberChannels;
							#endif	// defined multispec_mac || defined multispec_mac_swift

                     #if defined multispec_win | defined multispec_lin
								gStatusGraphicsRight *= histogramSpecsPtr->numberChannels;
							#endif	// defined multispec_win

						else	// ....allChannelsAtOnceFlag					
							#if defined multispec_mac || defined multispec_mac_swift
								gStatusBoxIncrement /= gImageWindowInfoPtr->numberImageFiles;
								if (gImageFileInfoPtr->numberBytes > 2)
									gStatusGraphicsRight /= 2;
							#endif	// defined multispec_mac || defined multispec_mac_swift

                     #if defined multispec_win | defined multispec_lin
								gStatusGraphicsRight *= gImageWindowInfoPtr->numberImageFiles;
							#endif	// defined multispec_win

						}	// end "if (!twoPassFlag)"

					#if defined multispec_win | defined multispec_lin
						if (gImageFileInfoPtr->numberBytes > 2)
							gStatusGraphicsRight *= 2;
					#endif	// defined multispec_win
						
					}	// end "if (continueFlag)" 

				ShowDialogItem (gStatusDialogPtr, IDC_Status7);
					                               
				#if defined multispec_win
					((CProgressCtrl*)(gStatusDialogPtr->GetDlgItem (IDC_Status7)))->
															SetRange32 (0, (SInt32)gStatusGraphicsRight);
					gStatusGraphicsRight = 0;
				#endif	// defined multispec_win
				
            #if defined multispec_lin
					/*int numberChars = sprintf (
							(char*)gTextString3,
							" SHistogram: DoHistogramRequests SetRange (twoPassFlag, allChannelsAtOnceFlag, gStatusGraphicsRight): %d, %d, %f%s",
							twoPassFlag,
							histogramSpecsPtr->allChannelsAtOnceFlag,
							gStatusGraphicsRight,
							gEndOfLine);
					ListString ((char*)gTextString3, numberChars, gOutputTextH);
					*/
               ((wxGauge*)(gStatusDialogPtr->FindWindow (IDC_Status7)))->
															SetRange ((SInt32)gStatusGraphicsRight);
					gStatusGraphicsRight = 0;
				#endif
				
				}	// end "if (loop == 0)"
				
					// Now get the statistics information from the image.  If 		
					// the information is being computed one channel at a time		
					//	and the histogram vector is to be listed,							
					// get the summary information for all channels first and		
					// then get the histogram values one channel at a time.			
				
			if (twoPassFlag)
				{
				LoadDItemStringNumber (kHistogramStrID, 
												IDS_HistogramStatus5, 
												gStatusDialogPtr, 
												IDC_Status2, 
												(Str255*)gTextString);
				
				histogramSpecsPtr->allChannelsAtOnceFlag = TRUE;
				if (continueFlag)
					continueFlag = ComputeHistogram (fileIOInstructionsPtr,
																	histogramSpecsPtr,
																	NULL,
																	loop,
																	0x0001);
				
						// Indicate that the summary information is loaded.			
						
				if (continueFlag)
					histogramSpecsPtr->loadedFlag = TRUE;
													
				histogramSpecsPtr->allChannelsAtOnceFlag = FALSE;
				computeCode = 0x0002;
				twoPassFlag = FALSE;

				#if defined multispec_mac
					gStatusBoxIncrement /= histogramSpecsPtr->numberChannels;
					gStatusGraphicsRight = gStatusGraphicsBox.left;
					InvalWindowRect (GetDialogWindow (gStatusDialogPtr), &gStatusGraphicsBox);
				#endif	// defined multispec_mac

				#if defined multispec_win
					gStatusGraphicsRight = numberLines * histogramSpecsPtr->numberChannels;
					((CProgressCtrl*)(gStatusDialogPtr->GetDlgItem (IDC_Status7)))->
																SetPos ((SInt32)gStatusGraphicsRight);
					gStatusGraphicsRight = 0;
				#endif	// defined multispec_win
													
				#if defined multispec_lin
                gStatusGraphicsRight = numberLines * histogramSpecsPtr->numberChannels;
					/* int numberChars2 = sprintf (
							(char*)gTextString3,
							" SHistogram: DoHistogramRequests2 (gStatusGraphicsRight): %f%s",
							gStatusGraphicsRight,
							gEndOfLine);
					ListString ((char*)gTextString3, numberChars2, gOutputTextH);
					*/
                ((wxGauge*)(gStatusDialogPtr->FindWindow (IDC_Status7)))->
															SetValue ((SInt32)gStatusGraphicsRight);
					gStatusGraphicsRight = 0;
				#endif

				}	// end "if (twoPassFlag)"
				
			if (loop == 0)
				{
				stringNumber = IDS_HistogramStatus1;
				if (!histogramSpecsPtr->allChannelsAtOnceFlag)
					stringNumber = IDS_HistogramStatus6;	// 18;
				
				LoadDItemStringNumber (kHistogramStrID, 
												stringNumber, 
												gStatusDialogPtr, 
												IDC_Status2, 
												(Str255*)gTextString);
												
				}	// end "if (loop == 0)"
				
			if (continueFlag)
				continueFlag = ComputeHistogram (fileIOInstructionsPtr,
																histogramSpecsPtr,
																histogramArrayPtr, 
																loop,
																computeCode);
			
					// Indicate that the summary information is loaded.			
					
			if (loop == numberLoops-1)
				{
				if (continueFlag)
					histogramSpecsPtr->loadedFlag = TRUE;
					
				MHideDialogItem (gStatusDialogPtr, IDC_Status7);
				
				}	// end "if (loop == numberLoops-1)" 
			
					// Write image statistics information to disk if the user 		
					// wants to save them.														
					
			if (numberChannels == gImageWindowInfoPtr->totalNumberChannels)
				{		
				if (continueFlag)
					{
					continueFlag = CreateSTASupportFile (gImageFileInfoPtr, 
																		histogramSpecsPtr,
																		histogramArrayPtr,
																		loop);
																	
							// If this is the last time through the loop allow the
							// already computed information to be listed if
							// requested.
							
					if (loop == numberLoops-1)
						continueFlag = TRUE;
																	
					}	// end "if (continueFlag)"
				
				else	// !continueFlag 
					{	
					CloseFile (gImageWindowInfoPtr->supportFileStreamPtr);			
					SetFileDoesNotExist (gImageWindowInfoPtr->supportFileStreamPtr, 
													kDoNotKeepUTF8CharName);
					
					}	// end "else !continueFlag" 
					
				}	// end "if (numberChannels == ..." 
			
			}	// end "if (histogramSpecsPtr->method == kComputeFromImage)" 
		
					// Read the image statistics from a disk file if needed.			
				
		if (histogramSpecsPtr->method == kReadFromSupportFile)
			{
					// Read image statistics from disk file.								
				
			if (twoPassFlag)
				{
						// Information is being read one channel at a time				
						//	and the histogram vector is to be listed,						
						// get the summary information for all channels first and	
						// then get the histogram values one channel at a time.		
					
				histogramSpecsPtr->allChannelsAtOnceFlag = TRUE;
				if (continueFlag)
					continueFlag = LoadSupportFile (gImageWindowInfoPtr,
																gImageFileInfoPtr, 
																histogramSpecsPtr,
																NULL,
																loop,
																summaryFlag);
				
				histogramSpecsPtr->allChannelsAtOnceFlag = FALSE;
				summaryFlag = FALSE;
				twoPassFlag = FALSE;
													
				}	// end "if (!...allChannelsAtOnceFlag && ..."
				
			if (continueFlag)
				continueFlag = LoadSupportFile (gImageWindowInfoPtr,
															gImageFileInfoPtr, 
															histogramSpecsPtr,
															histogramArrayPtr,
															loop,
															summaryFlag);
			
			}	// end "if (...->histogramSpecs.method == kReadFromSupportFile)" 
			
				// Dispose of the buffers if not needed anymore and close the		
				// statistics file.																
  			
		if (!continueFlag || loop == numberLoops-1)
			{
					// Dispose of the IO buffer.		
										
			DisposeIOBufferPointers (fileIOInstructionsPtr,
												&gInputBufferPtr, 
												&gOutputBufferPtr);
			
			if (gProcessorCode == kHistogramProcessor)
				histogramSpecsPtr->statBufferPtr = 
										CheckAndDisposePtr (histogramSpecsPtr->statBufferPtr);
			
			}	// end "if (!continueFlag || loop == numberLoops)" 
		
		if (continueFlag && loop == 0)	
			continueFlag = ListHistogramTitle (histogramSpecsPtr, resultsFileStreamPtr);
	
					// List the histograms for the requested channels.					
	
		if (continueFlag)
			continueFlag = ListHistogramValues (histogramSpecsPtr, 
																gImageFileInfoPtr,
																histogramArrayPtr,
																resultsFileStreamPtr,
																loop);
									
		if (!continueFlag)
			break;
					
		}	// end "for (loop=0; loop<numberLoops; loop++)" 
		
	if (continueFlag)
		{
		histogramSpecsPtr->loadedFlag = TRUE;
		histogramSpecsPtr->computeFlag = FALSE;
	
				// List the histogram statistics in output window if requested.	

		continueFlag = ListHistogramSummary (gImageFileInfoPtr,
															histogramSpecsPtr,
															resultsFileStreamPtr);
		
		}	// end "if (continueFlag)"
	
	return (continueFlag);
	
}	// end "DoHistogramRequests"
										

               
#if defined multispec_mac
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawHistogramMethodPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							histogram method selection for the 
//							histogram method pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
//	Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1993
//	Revised By:			Larry L. Biehl			Date: 10/26/1993	

pascal void DrawHistogramMethodPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpHistogramMethodMenu, 
								gHistogramMethod, 
								TRUE);
	
}	// end "DrawHistogramMethodPopUp"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ForceHistogramCodeResourceLoad
//
//	Software purpose:	The purpose of this is to force the 'Histogram' code
//							resource to be loaded into memory.  It is called by
//							DisplayControl in the 'Display' code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			DisplayControl in display.c
//
//	Coded By:			Larry L. Biehl			Date: 11/13/1990
//	Revised By:			Larry L. Biehl			Date: 11/13/1990

void ForceHistogramCodeResourceLoad ()

{
			// If spare memory had to be used to load code resources, then exit	
			// routine.																			
			
	if (gMemoryTypeNeeded < 0)
																							return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

}	// end "ForceHistogramCodeResourceLoad" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetClippedMinMaxValues
//
//	Software purpose:	The purpose of this routine is to determine the
//							minimum and maximum values to use for image enhancement 
//							based on the user specified percent of values to clip.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			FillDataToDisplayLevels in displayMultiSpectral.c
//							EqualAreaDataToDisplayLevels in displayMultiSpectral.c
//							UpdateEnhancementMinMaxes in displayMultiSpectral.c
//
//	Coded By:			Larry L. Biehl			Date:	09/30/1993
//	Revised By:			Larry L. Biehl			Date: 10/21/2006

Boolean GetClippedMinMaxValues (
				HistogramSpecsPtr					histogramSpecsPtr,
				SInt16	 							percentTailsClipped,
				SInt16 								channel, 
				UInt32				 				numberDataValues, 
				double* 								minValueIndexPtr, 
				double* 								maxValueIndexPtr,
				Boolean 								histLoadedFlag,
				Boolean								adjustDataFlag)

{
	HistogramSummaryPtr				histogramSummaryPtr; 
	
	SInt32								maxValueIndex,
											minValueIndex;
											
	Boolean								continueFlag;
											

	continueFlag = GetClippedMinMaxValueIndices (histogramSpecsPtr,
																	percentTailsClipped,
																	channel, 
																	numberDataValues, 
																	&minValueIndex, 
																	&maxValueIndex,
																	histLoadedFlag,
																	adjustDataFlag);
	
	if (continueFlag)
		{				
		histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	
		*minValueIndexPtr = GetDataValueForBinIndex (
												minValueIndex,
												&histogramSummaryPtr[channel],
												gImageFileInfoPtr->signedValueOffset);
			
		*maxValueIndexPtr = GetDataValueForBinIndex (
												maxValueIndex,
												&histogramSummaryPtr[channel],
												gImageFileInfoPtr->signedValueOffset);

		}	// end "if (continueFlag)"
		
	return (continueFlag);

}	// end "GetClippedMinMaxValues"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetClippedMinMaxValueIndices
//
//	Software purpose:	The purpose of this routine is to determine the
//							minimum and maximum values to use for image enhancement 
//							based on the user specified percent of values to clip.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			FillDataToDisplayLevels in displayMultiSpectral.c
//							EqualAreaDataToDisplayLevels in displayMultiSpectral.c
//							UpdateEnhancementMinMaxes in displayMultiSpectral.c
//
//	Coded By:			Larry L. Biehl			Date:	09/30/1993
//	Revised By:			Larry L. Biehl			Date: 03/01/2013

Boolean GetClippedMinMaxValueIndices (
				HistogramSpecsPtr					histogramSpecsPtr,
				SInt16	 							percentTailsClipped,
				SInt16 								channel, 
				UInt32				 				numberDataValues, 
				SInt32* 								minValueIndexPtr, 
				SInt32* 								maxValueIndexPtr,
				Boolean 								histLoadedFlag,
				Boolean								adjustDataFlag)

{
	SInt64								totalPixels;

	HistogramSummaryPtr				histogramSummaryPtr; 
	
	HUInt32Ptr							histogramArrayPtr;
	
	SInt32								endDataValue;
	
	UInt32								countOfClipValues,
											dataLevel,
											lastDataLevel,         
											numberClipValues;
	
	Boolean								continueFlag = FALSE,
											endValueFoundFlag;
	
	
			// Check input variables.															
			
	if (histogramSpecsPtr == NULL)
																					return (FALSE);
	
	if (channel >= gImageWindowInfoPtr->totalNumberChannels)
																					return (FALSE);
	
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	
			// Intialize the min and max bin indices.
			
	*minValueIndexPtr = GetBinIndexForDataValue (
											histogramSummaryPtr[channel].minNonSatValue + 
													gImageFileInfoPtr->signedValueOffset,
											&histogramSummaryPtr[channel]);
			
	*maxValueIndexPtr = GetBinIndexForDataValue (
											histogramSummaryPtr[channel].maxNonSatValue + 
													gImageFileInfoPtr->signedValueOffset,
											&histogramSummaryPtr[channel]);	
											
	continueFlag = TRUE;
	
			// If the minNonSatValue is the same as the maxNonSatValue then do
			// not do any clipping.
	
	if (percentTailsClipped > 0 && 
			histogramSummaryPtr[channel].minNonSatValue < 
										histogramSummaryPtr[channel].maxNonSatValue)
		{																				
		histogramArrayPtr = histogramSpecsPtr->histogramArrayPtr;
		
		if (!histLoadedFlag)
			continueFlag = GetHistogramVectorForChannel (histogramSpecsPtr,
																			(UInt16*)&channel, 
																			1); 
			
		if (continueFlag)
			{
					// If the total number of pixels is not known then compute it		
					// from the histogram values.													 
			
			if (histogramSpecsPtr->totalPixels <= 0)
					histogramSpecsPtr->totalPixels =
								CountTotalNumberHistogramPixels (histogramArrayPtr,
														histogramSummaryPtr[channel].numberBins);
			
			totalPixels = histogramSpecsPtr->totalPixels;
			if (totalPixels <= 0)
				continueFlag = FALSE;
				
			}	// end "if (continueFlag)" 
			
		if (continueFlag)
			{
					// Subtract the number of pixels in the first and last bins.		
					// The assumption is made that these are saturated values and 		
					// therefore will not be used.												
					
			totalPixels -= histogramArrayPtr[0];
			if (numberDataValues > 1)
				totalPixels -= histogramArrayPtr[numberDataValues-1];
						
					// Determine the number of pixels to "clip" at the each end of		
					// the histogram tail (not including the saturated pixels.			
			
			numberClipValues = 
						(UInt32)((double)percentTailsClipped * totalPixels/(2*100) + .5);
			
			lastDataLevel = 0;
			if (numberDataValues > 0)
				lastDataLevel = numberDataValues - 1;
			
					// Get the minumum enhancement data value. We will force it not to 
					// be the minimum value in the data, which may be a background or 
					// saturated data value.	
					
			endValueFoundFlag = FALSE;	
			if (histogramArrayPtr[0] > 0)
				{
				endValueFoundFlag = TRUE;	
				endDataValue = 0;
				
				}	// end "if (histogramArrayPtr[0] > 0)"	
			
			countOfClipValues = 0;
			for (dataLevel=1; dataLevel<lastDataLevel; dataLevel++)
				{
				countOfClipValues += histogramArrayPtr[dataLevel];
				
				if (!endValueFoundFlag && histogramArrayPtr[dataLevel] > 0)
					{
					endValueFoundFlag = TRUE;	
					endDataValue = dataLevel;
					
					}	// end "if (!endValueFoundFlag && ..."
				
				if (endValueFoundFlag && countOfClipValues > numberClipValues)
					{
					if (dataLevel > (UInt32)endDataValue && histogramArrayPtr[dataLevel] > 0)
						{
						*minValueIndexPtr = dataLevel;
						break;
						
						}	// end "if (dataLevel > endDataValue && ..."
					
					}	// end "if (numberValuesInDataLevel >= ...)" 
					
				}	// end "for (dataLevel=1; dataLevel<..." 
			
					// Get the maximum enhancement data value. We will force it not to 
					// be the minimum value in the data, which may be a background or 
					// saturated data value.
					
			endValueFoundFlag = FALSE;	
			if (histogramArrayPtr[lastDataLevel] > 0)
				{
				endValueFoundFlag = TRUE;	
				endDataValue = lastDataLevel;
				
				}	// end "if (histogramArrayPtr[0] > 0)"								
			
			countOfClipValues = 0;
			if (lastDataLevel > 0)
				{
				for (dataLevel=lastDataLevel-1; dataLevel>=1; dataLevel--)
					{
					countOfClipValues += histogramArrayPtr[dataLevel];
					
					if (!endValueFoundFlag && histogramArrayPtr[dataLevel] > 0)
						{
						endValueFoundFlag = TRUE;	
						endDataValue = dataLevel;
						
						}	// end "if (!endValueFoundFlag && ..."
					
					if (endValueFoundFlag && countOfClipValues > numberClipValues)
						{
						if (dataLevel < (UInt32)endDataValue && 
												histogramArrayPtr[dataLevel] > 0)
							{
							*maxValueIndexPtr = dataLevel;
							break;
							
							}	// end "if (dataLevel < endDataValue && ..."
						
						}	// end "if (numberValuesInDataLevel >= ...)" 
						
					}	// end "for (dataLevel=lastDataLevel; dataLevel>..." 
					
				}	// end "if (lastDataLevel > 0)"
				
					// Make certain that the values make sense relative to each		
					// other.																		
					
			if (*maxValueIndexPtr <= *minValueIndexPtr)
				{
				if (*maxValueIndexPtr >= (SInt32)lastDataLevel)
					*minValueIndexPtr = lastDataLevel - 1;
					
				else	// *maxValueIndexPtr < lastDataLevel 
					*maxValueIndexPtr = *minValueIndexPtr + 1;
					
				}	// end "if (*maxValueIndexPtr <= *minValueIndexPtr)"  
				
			}	// end "if (continueFlag)"
			
		}	// end "if (percentTailsClipped > 0)" 
				
					// Allow for signed data if needed.
				
	if (continueFlag && adjustDataFlag && gImageFileInfoPtr->signedDataFlag)
		{
		*minValueIndexPtr -= gImageFileInfoPtr->signedValueOffset;
		*maxValueIndexPtr -= gImageFileInfoPtr->signedValueOffset;
		
		}	// end "if (continueFlag && adjustDataFlag && ..."
		
	return (continueFlag);
		
}	// end "GetClippedMinMaxValueIndices"


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle* GetHistogramLoadedFlag
//
//	Software purpose:	This routine returns the flag indicating whether the histogram
//							statistics have been loaded.
//
//	Parameters in:		Pointer to the window information structure			
//
//	Parameters out:	None				
//
// Value Returned:	Flag in the histogram specs structure indicating whether the
//							histogram has been loaded.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/29/2012
//	Revised By:			Larry L. Biehl			Date: 01/31/2012

Boolean GetHistogramLoadedFlag (
				Handle								windowInfoHandle)

{
	Handle								histogramSpecsHandle;
	HistogramSpecsPtr					histogramSpecsPtr;
	
																	
	histogramSpecsHandle = GetHistogramSpecsHandle (windowInfoHandle);
	
	histogramSpecsPtr = (HistogramSpecsPtr)GetHandlePointer (histogramSpecsHandle);
		
	if (histogramSpecsPtr != NULL)
		return (histogramSpecsPtr->loadedFlag);
		
	else 
		return (FALSE);
							
}	// end "GetHistogramLoadedFlag"


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle* GetHistogramSpecsHandlePtr
//
//	Software purpose:	This routine gets the address of the handle containing
//							the histogram specifications structure.
//
//	Parameters in:		Pointer to the window information structure			
//
//	Parameters out:	None				
//
// Value Returned:	Pointer to the handle for the histogram 
//							specifications structure	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/09/1995
//	Revised By:			Larry L. Biehl			Date: 11/09/1995

Handle* GetHistogramSpecsHandlePtr (
				WindowInfoPtr						windowInfoPtr)

{
	Handle*								handlePtr = NULL;
	
	
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac  				
			handlePtr = &windowInfoPtr->histogramSpecsHandle;	
		#endif	// defined multispec_mac 
		              
      #if defined multispec_win | defined multispec_lin		
			handlePtr = &gActiveImageViewCPtr->m_histogramCPtr->mHistogramSpecsHandle;
		#endif	// defined multispec_win 
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (handlePtr);
		
}	// end "GetHistogramSpescHandlePtr" 	  


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Handle* GetHistogramSummaryHandlePtr
//
//	Software purpose:	This routine gets the address of the handle containing
//							the histogram summary structure.
//
//	Parameters in:		Pointer to the window information structure			
//
//	Parameters out:	None			
//
// Value Returned:	Pointer to the handle for the histogram summary structure
//
// Called By:			InitializeHistogramInfoStructure in SHistgrm.cpp
//							SetUpHistogramSpecsPtr in SHistgrm.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/09/1995
//	Revised By:			Larry L. Biehl			Date: 11/09/1995

Handle* GetHistogramSummaryHandlePtr (
				WindowInfoPtr						windowInfoPtr)

{
	Handle*								handlePtr = NULL;
	
	
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac  
			handlePtr = &windowInfoPtr->histogramSummaryHandle;
		#endif	// defined multispec_mac 
	
      #if defined multispec_win | defined multispec_lin
			handlePtr = &gActiveImageViewCPtr->m_histogramCPtr->mHistogramSummaryHandle;
		#endif	// defined multispec_win 
		
		}	// end "if (windowInfoPtr != NULL)"
		
	return (handlePtr);
		
}	// end "GetHistogramSummaryHandlePtr" 	 


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetHistogramValuesMemory
//
//	Software purpose:	This routine gets the buffer to use to read the sta 
//							statistics file information into.  Enough space is
//							obtained to read all the data for one channel.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:		None	
//
// Called By:			SetUpToReadHistogramArray in histogram.c
//							HistogramControl in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 07/19/1990
//	Revised By:			Larry L. Biehl			Date: 10/23/1999

HUInt32Ptr GetHistogramValuesMemory (
				UInt32								numberBins, 
				UInt16								numberChannels, 
				Handle*								histogramArrayHPtr)

{
	HUInt32Ptr          				histogramArrayPtr;
								
	UInt32								numberBytes;
	
	Boolean								changedFlag,
											continueFlag;
	
	
			// Dispose of the input handle if it exists.
			
	*histogramArrayHPtr = UnlockAndDispose (*histogramArrayHPtr);
	
			// Get the number of bytes needed.
	
	numberBytes = (UInt32)numberChannels * numberBins * sizeof (UInt32);
	
			// Get the memory for the handle.
		
	histogramArrayPtr = (HUInt32Ptr)CheckHandleSize (histogramArrayHPtr,
																		&continueFlag, 
																		&changedFlag, 
																		numberBytes);
	
	return (histogramArrayPtr);
		
}	// end "GetHistogramValuesMemory"


	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetHistogramVectorForChannel
//
//	Software purpose:	This routine the histogram information for the specified
//							channel from the sta statistics support file and
//							interprets the data.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:		None	
//
// Called By:			EqualAreaDataToDisplayLevels in MDisMult.c
//							GetClippedMinMaxValues in MHistgrm.c
//
//	Coded By:			Larry L. Biehl			Date: 05/09/1995
//	Revised By:			Larry L. Biehl			Date: 10/22/1999

Boolean GetHistogramVectorForChannel (
				HistogramSpecsPtr					histogramSpecsPtr,  
				UInt16*								channelListPtr, 
				SInt16								numberChannels)

{
	CMFileStream						*supportFileStreamPtr;
											
	UInt32								currentLayerChannel;
	
	Boolean								continueFlag;
	
	SignedByte							handleStatus;
	
	if (gImageWindowInfoPtr->numberImageFiles == 1 && 
										histogramSpecsPtr->statFileFormat == kImagineType)
		{
				// Set up swap bytes flag depending upon the system architecture
				                 
		gSwapBytesFlag = gBigEndianFlag;
			
				// Force the current layer channel to be the max number to force
				// the 'LoadImagineImageStatisticsForChannel' routine to start reading
				// from the first channel.
				
		currentLayerChannel = ULONG_MAX;

		continueFlag = LoadImagineImageStatisticsForChannel (
														gImageFileInfoPtr,
														histogramSpecsPtr->histogramSummaryPtr, 
														histogramSpecsPtr->histogramArrayPtr, 
														histogramSpecsPtr->statBufferPtr,
														*channelListPtr, 
														&currentLayerChannel,
														gImageFileInfoPtr->numberBins,
														FALSE);											
				
		gSwapBytesFlag = FALSE;
														
		}	// end "if (windowInfoPtr->numberImageFiles == 1 && ..."
																	
	else	// windowInfoPtr->numberImageFiles != 1 || ...
		{
		supportFileStreamPtr = GetActiveSupportFileStreamPointer (&handleStatus);
	
		continueFlag = ReadSTASupportFile (histogramSpecsPtr->statBufferPtr,
														supportFileStreamPtr,
														channelListPtr, 
														numberChannels, 
														FALSE);
		
				// Decode the statistics file histogram values for the channel.				
	
		if (continueFlag)	
			continueFlag = DecodeSTASupportFile (histogramSpecsPtr,
																histogramSpecsPtr->histogramArrayPtr,
																supportFileStreamPtr,
																channelListPtr, 
																numberChannels,
																FALSE);
		
		}	// end "else windowInfoPtr->numberImageFiles != 1 || ..."
						
	return (continueFlag);
		
}	// end "GetHistogramVectorForChannel"

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetSTASupportFile
//
//	Software purpose:	This routine allows the user to select the ERDAS 
//							statistics file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			HistogramDialogHandleMethod in SHistogrm.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1988
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean GetSTASupportFile (
				FileInfoPtr							fileInfoPtr)

{
	CMFileStream						savedSupportFileStream;
	
	SInt64								fileSize;
	
	char									*erdasStatHeader = (char*)"TRAIL";
	CMFileStream*						supportFileStreamPtr;
	FileStringPtr						supportFilePathPtr;
	
	OSType								fileType[3];
	
	UInt32								count;
	
	SInt16								errCode,
											numberFileTypes,
											stringCompare;
											
	Boolean								continueFlag;
	
	SignedByte							supportHandleStatus;
	
	
	continueFlag = TRUE;
	
			// Get a handle to a block of memory to be used for						
			// file information for the image statistics file.							
			// Lock the memory until the information has been loaded					
			// in. Then get the pointer to the file information block				
	
	if (gActiveImageWindowInfoH == NULL)									
																				return (FALSE);

	supportFileStreamPtr = GetSupportFileStreamPointer (
																gActiveImageWindowInfoH,
																&supportHandleStatus);
					
	if (supportFileStreamPtr != NULL)
		{
				// Save copy of current support file stream in case we need it
				// later, particularly the name.
				
		CopyFileStream (&savedSupportFileStream, supportFileStreamPtr);
		
				// Make sure that the GetFile routine does not think that the
				// selected file is already opened.
				
		IndicateFileClosed (supportFileStreamPtr);
				
				// Get the file name and volume to read erdas statistics file 		
				// from.	First make sure that we have a pointer cursor.				

		MInitCursor ();
		SetType (supportFileStreamPtr, kISTAFileType);
		numberFileTypes = 2;
		fileType[0] = kISTAFileType;
		fileType[1] = kTEXTFileType;		
		stringCompare = -1;
		
		do
			{
			errCode = GetFile (supportFileStreamPtr, 
										numberFileTypes, 
										fileType, 
										NULL, 
										NULL,
										NULL, 
										IDS_SelectImageStatistics);
										
			continueFlag = (errCode == noErr) & FileExists (supportFileStreamPtr);
		
					// If file is set up to read image statistics from, check if	
					// file is of correct format.  Check for 'TRAIL' in first 5		
					// bytes																			
				
			if (continueFlag)
				{
				errCode = MSetMarker (supportFileStreamPtr, 
												fsFromStart, 
												0, 
												kErrorMessages);
	
				count = 7;
				if (errCode == noErr)
					errCode = MReadData (supportFileStreamPtr,
												&count,
												gTextString,
												kErrorMessages);
												
				continueFlag = (errCode == noErr);
				if (continueFlag)
					{
					stringCompare = strncmp ((char*)gTextString, erdasStatHeader, 5);
						
					}	// end "if (continueFlag)" 
					
				if (stringCompare != 0 && fileInfoPtr->format == kImagineType)
					{
							// The selected file may still be okay if the image file type is Imagine
							// and the selected file is an Imagine type. We will not check if it
							// actually belongs to the image file.
					
					supportFilePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (supportFileStreamPtr);
					if (CompareSuffixNoCase ((char*)"\0.img", supportFilePathPtr, NULL))	
						stringCompare = 0;					
					
					}	// end "if (stringCompare != 0 && fileInfoPtr->format == kImagineType)"
					
				if (stringCompare == 0)
					{
							// Make certain that the file is of the correct size.		
					
					errCode = GetSizeOfFile (supportFileStreamPtr, &fileSize);
					continueFlag = (errCode == noErr);
					if (continueFlag && fileSize < 
										(UInt32)gImageWindowInfoPtr->totalNumberChannels*9*128)
						stringCompare = -1;
					
					}	// end "if (stringCompare == 0)" 
							
						// Beep to alert user that file is not an image 				
						// statistics file that matches the image parameters.			
					
				if (stringCompare != 0) 
					{ 
					SysBeep (10);
					CloseFile (supportFileStreamPtr);
					
					}	// end "if (stringCompare != 0)"
				
				}	// end "if (continueFlag)" 
			
			}	while (continueFlag && stringCompare != 0);
			
		if (continueFlag)
				// Make sure that the previous support file is closed.
			CloseFile (&savedSupportFileStream);
			
		else	// !continueFlag
				// Restore the previous support file stream information.
			CopyFileStream (supportFileStreamPtr, &savedSupportFileStream);

		}	// end "if (supportFileStreamPtr != NULL)" 

	else	// supportFileStreamPtr == NULL
		continueFlag = FALSE;
			
	UnlockSupportFileStream (gActiveImageWindowInfoH, supportHandleStatus);
  	
	return (continueFlag);
	
}	// end "GetSTASupportFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HCharPtr GetSTASupportFileBuffer
//
//	Software purpose:	This routine gets the buffer to use to read the ERDAS 
//							statistics file information into.  Enough space is
//							obtained to read all the data for all requested
//							channels if the channels in the channel list are in
//							order and space is available; otherwise space
//							is acquired for only one channel
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadSTASupportFile in histogram.c
//							SetUpToReadHistogramArray in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 07/19/1990
//	Revised By:			Larry L. Biehl			Date: 11/06/1995

HCharPtr GetSTASupportFileBuffer (
				UInt16*								channelListPtr, 
				SInt16*								numberChannelsPtr)

{
	CharPtr								statBufferPtr;
	
	UInt32								lContBlock,
											numberBytes;
											
	Boolean								continuousFlag;
	
		
			// Determine if the channels in the list are continuous.					
			
	continuousFlag = DetermineIfContinuousChannels (channelListPtr, *numberChannelsPtr);
				
			// Get buffer to load statistics into.  Try to get buffer to hold		
			// statistics information for the requested number of channels.
			
	if (!continuousFlag)
		*numberChannelsPtr = 1;		
			
	numberBytes = (UInt32)9 * 128 * *numberChannelsPtr;
	
			// Check if memory is available for the requested							
			// number of channels before attempting to allocate the memory.		
			// If the memory is not available, then try to allocate memory 		
			// for just one channel.															
			
	MGetFreeMemory (&lContBlock);
	
	if (lContBlock < numberBytes+500)
		{
				// Not enough memory available for all requested channels.			
				// Try one channel.																
				
		numberBytes = (UInt32)9 * 128;
		*numberChannelsPtr = 1;
		
		}	// end "if (lContBlock < numberBytes+500)" 
		
	statBufferPtr = (CharPtr)MNewPointer (numberBytes);
	
	return (statBufferPtr);
		
}	// end "GetSTASupportFileBuffer" 	



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetStatFileBuffer
//
//	Software purpose:	This routine gets the buffer to use to read the
//							statistics file information into taking into account the
//							format that the image statistics information is stored in. 
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			SetUpToReadHistogramArray in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1999
//	Revised By:			Larry L. Biehl			Date: 10/26/1999

Boolean GetStatFileBuffer (
				FileInfoPtr							fileInfoPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				UInt16*								channelListPtr, 
				SInt16								numberChannels)

{		
	if (histogramSpecsPtr->statBufferPtr == NULL)
		{
		if (histogramSpecsPtr->statFileFormat == kImagineType)
			{
			histogramSpecsPtr->numberStatBufferChannels = 1;
			
			histogramSpecsPtr->statBufferPtr = GetImagineHistogramBuffer (
																			fileInfoPtr->numberBins);
												
			}	// end "if (histogramSpecsPtr->statFileFormat == kImagineType)"
		
		else	// histogramSpecsPtr->statFileFormat != kImagineType
			{
			histogramSpecsPtr->numberStatBufferChannels = numberChannels;
		
			histogramSpecsPtr->statBufferPtr = GetSTASupportFileBuffer (
												channelListPtr,
												&histogramSpecsPtr->numberStatBufferChannels);
												
			}	// end "else histogramSpecsPtr->statFileFormat != kImagineType"
									
		}	// end "if (histogramSpecsPtr->statBufferPtr == NULL)" 
		
	return (histogramSpecsPtr->statBufferPtr!= NULL);
		
}	// end "GetStatFileBuffer" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetDataFormatString
//
//	Software purpose:	The purpose of this is to force the 'Histogram' code
//							resource to be loaded into memory.  It is called by
//							DisplayControl in the 'Display' code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/03/2006
//	Revised By:			Larry L. Biehl			Date: 03/21/2006

void GetDataFormatString (
				char*									stringPtr,
				double								value,
				UInt16								fieldSize,
				UInt16								ePrecision,
				UInt16								fPrecision,
				Boolean								forceEFormatFlag)

{
	char									format[16];
	
	double								absValue;
	
						
	strcat (stringPtr, "\t%");
	sprintf (format, "%d", fieldSize);
	strcat (stringPtr, format);
	strcat (stringPtr, ".");
			
	absValue = fabs (value);
	if ((!forceEFormatFlag && 
			(absValue >= kMinValueToListWith_f && absValue <= kMaxValueToListWith_f)) ||
																								absValue == 0)
		{
		sprintf (format, "%d", fPrecision);
		strcat (stringPtr, format);
		strcat (stringPtr, "f");
		
		}	// end if ((absValue >= kMinValueToListWith_f && ...
		
	else	// absValue < kMinValueToListWith_f || ...
		{
		sprintf (format, "%d", ePrecision);
		strcat (stringPtr, format);
		strcat (stringPtr, "E");
		
		}	// end if ((absValue < kMinValueToListWith_f || ...
	
}	// end "GetDataFormatString"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HistogramControl
//
//	Software purpose:	The purpose of this routine is to control the 
//							selection of the histogram specifications and the
//							computation of the histograms for a given image
//
//	Functional description:	The steps in this function follow.
//
//			1) Display a dialog box to allow the user to select the histogram
//				specifications.
//
//			2) Compute the histogram according to the user specifications if
//				the user has selected OK.
//
//			3) Print the histogram statistics in the output window
//
//	Parameters in:		computeCode = 0: Do not need to compute histogram.
//											= 1: Compute the histogram.
//											= 2: Read the histogram from disk file.
//
//													kDoNotComputeHistogram		0
//													kComputeHistogram				1
//													kLoadHistogramFromDisk		2
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:			Menus   in menu.c
//							DisplayMultispectralImage in displayMultiSpectral.c
//
//	Coded By:			Ravi S. Budruk			Date: 05/21/1988
//	Revised By:			Ravi S. Budruk			Date: 09/16/1988	
//	Revised By:			Larry L. Biehl			Date: 06/30/2016

Boolean HistogramControl (
				SInt16								computeCode)

{
	time_t								startTime;
	
	HistogramSpecsPtr					histogramSpecsPtr;
	HUInt32Ptr							histogramArrayPtr;
											
	Handle								histogramArrayH,
											windowInfoHandle;
								
	SInt16								imageHandleStatus,
											numberChannels;
	
	Boolean								continueFlag,
											returnFlag;
	
	SignedByte							histogramHandleStatus = -1,
											supportHandleStatus = -1;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																			
			
	if (gMemoryTypeNeeded < 0)
																					return (FALSE);
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;													
	
			// Initialize local variables.													
			
	histogramArrayH = NULL;
	histogramArrayPtr = NULL;
	histogramSpecsPtr = NULL;
			
	continueFlag = SetUpActiveImageInformationGlobals (
									&windowInfoHandle, &imageHandleStatus, kDoNotUseProject);
	
			// Get pointer to histogram specifications structure
	
	if (continueFlag)
		histogramSpecsPtr = SetUpHistogramSpecsPtr (kNoSetUpHistogramArray,
																	&histogramHandleStatus);
	
	if (histogramSpecsPtr != NULL)			
		gImageWindowInfoPtr->supportFileStreamPtr =
							GetActiveSupportFileStreamPointer (&supportHandleStatus);
			
				// Force computation of histogram if requested 							
	
	if (histogramSpecsPtr && computeCode != kDoNotComputeHistogram)  
		histogramSpecsPtr->computeFlag = TRUE;
	
	if (histogramSpecsPtr && histogramSpecsPtr->computeFlag)
		{
				// Load default specifications to compute histogram					
			
		LoadHistogramSpecs (gImageFileInfoPtr, histogramSpecsPtr);
		
		continueFlag = FALSE;
		if (computeCode == kLoadHistogramFromDisk)
			{
			histogramSpecsPtr->method = kReadFromSupportFile;
			continueFlag = TRUE;
			
			}	// end "if (computeCode == kLoadHistogramFromDisk)" 
			
		else	// computeCode != kLoadHistogramFromDisk 
			{
			if (computeCode == kComputeHistogram && 
													gProcessorCode == kDisplayProcessor)
				{
				histogramSpecsPtr->method = kComputeFromImage;
			
				if (gImageWindowInfoPtr->totalNumberChannels <= 12)
					histogramSpecsPtr->listHistSummaryFlag = TRUE;		
					
				}	// end "if (computeCode == kComputeHistogram && ..."
		
 					// Call the histogram dialog for entering user specifications.	
			
         if (!gFromToolParameterFileFlag)
            continueFlag = HistogramDialog (gImageFileInfoPtr, histogramSpecsPtr);
            
         else     // gFromToolParameterFileFlag
            continueFlag = TRUE;
			
			}	// end "else computeCode != kLoadHistogramFromDisk" 
			
		startTime = time (NULL);
																				
		if (continueFlag)
			{
					// Get handle to block of memory to use for histogram list		
					// information if needed.													
			
			if ((histogramSpecsPtr->method == kComputeFromImage &&
						!histogramSpecsPtr->minMaxMeanSDOnlyFlag) ||
														histogramSpecsPtr->listHistFlag)
				{
				numberChannels = histogramSpecsPtr->numberChannels;
				if (!histogramSpecsPtr->allChannelsAtOnceFlag)
					numberChannels = 1;
					
				histogramSpecsPtr->histogramArrayPtr = 
							GetHistogramValuesMemory (
									histogramSpecsPtr->maxNumberBins, 
									numberChannels, 
									&histogramSpecsPtr->histogramArrayH);
				
				continueFlag = (histogramSpecsPtr->histogramArrayH != NULL);
				
				}	// end "if ((histogramSpecsPtr->..."
				
			}	// end "if (continueFlag)" 
		
				// Initialize some global variables pertaining to output.				
				// Output will only be written to a disk file if histogram values		
				// are to be listed.																	
			
		gOutputCode = histogramSpecsPtr->outputStorageType;
		if (!histogramSpecsPtr->listHistFlag)
			gOutputCode = 0x0001;
		gOutputForce1Code = (gOutputCode | 0x0001);
		gOutputTextOKFlag = TRUE;
			
		if (continueFlag)
			continueFlag = CheckHistogramTextWindowSpaceNeeded (histogramSpecsPtr);
				
				// If the histogram values are to go to a disk file, 					
				// open the	disk file.	Trick 'CreateResultsDiskFiles' to 			
				// estimate the number of bytes on 'pseudo' lines and 				
				// columns.																			
		
		if (continueFlag && (gOutputCode & 0x0002))
			{
			InitializeAreaDescription (
										&gAreaDescription, 
										1, 
										DetermineBytesForHistogramText (histogramSpecsPtr, FALSE), 
										1, 
										1, 
										1, 
										1,
										1,
										1,
										0);
										
			continueFlag = CreateResultsDiskFiles (gOutputCode, 0, 0);
									
			}	// end "if (continueFlag && (gOutputCode & 0x0002))" 
				
		if (continueFlag)
			{
			MSetCursor (kWait);
			
					// Force text selection to start from end of present text.		
				
			ForceTextToEnd ();
							
						// Get histogram status dialog box.						 
		                                                    
			if (computeCode != kLoadHistogramFromDisk)
				{
						// If a status dialog box already exists, close it. It could exist
						// for situations when opening an image file using MultiSpec as a 
						// file handler.
						
				if (gStatusDialogPtr != NULL)
					CloseStatusDialog (TRUE);
					
				gStatusDialogPtr = GetStatusDialog (kGraphicStatusDialogID, TRUE);
				
				}	// end "if (computeCode != kLoadHistogramFromDisk)"
		
					// If the histogram method is kStoredInMemory, use the 		
					// statistics already in memory.																	
				
			if (histogramSpecsPtr->method == kStoredInMemory)
				histogramSpecsPtr->computeFlag = FALSE;
		
			continueFlag = DoHistogramRequests (histogramSpecsPtr);
				
			}	// end "if (continueFlag)" 
				
					// Dispose compute histogram status dialog							
		
		MHideDialogItem (gStatusDialogPtr, IDC_Status2);
		CloseStatusDialog (TRUE);
					
				// Close the  results file if needed and									
				// release the memory assigned to it.										
				
		CloseResultsFiles ();
			
		}	// end "if (histogramSpecsPtr && histogramSpecsPtr->..." 
		
	returnFlag = FALSE;
	if (histogramSpecsPtr != NULL)
		returnFlag = histogramSpecsPtr->loadedFlag;
	
	if (gProcessorCode == kHistogramProcessor && gImageWindowInfoPtr != NULL)
		{
				// Close up parameters if object was initiated  by the
				// histogram processor. The parameters will be closed by the
				// display processor object if it initiated the histogram object.
				
		CloseUpHistogramArrayReadParameters (); 
		ReleaseHistogramSpecsPtr (gImageWindowInfoPtr);
				
		//CloseFile (gImageWindowInfoPtr->supportFileStreamPtr);
		//UnlockActiveSupportFileStream (supportHandleStatus);
		//gImageWindowInfoPtr->supportFileStreamPtr = NULL;
		
				// Set cursor back to pointer if not called from display processor.
				
		MInitCursor ();
		
		}	// end "if (gProcessorCode == kHistogramProcessor && ...)"
						
			// List the CPU time taken for histogramming.		
	
	if (gProcessorCode == kHistogramProcessor)
		continueFlag = ListCPUTimeInformation (NULL,
																continueFlag, 
																startTime);
	
			// Scroll output window to the selection and adjust the scroll		
			// bar.																				
			
	UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
		
	UnlockActiveImageInformationGlobals (imageHandleStatus);
	
	return (returnFlag);
	
}	// end "HistogramControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean HistogramDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for the histogram for the
//							given image.
//
//	Functional description:	The steps in this function follow.
//
//			1) Set up default specifications in gActiveImageWindow->histogramSpecsHandle.
//
//			2) Display the histogram dialog box and moniter the user changes.
//				(See DisplayDialog in display.c and ModalFileSpecification in
//				menus.c for examples).
//
//			3) If the user selects OK, load the new values into gActiveImageWindow->
//				histogramSpecsHandle and return TRUE.  If the user selects Cancel then
//				exit the function returning FALSE.
//
//	Parameters in:		&gActiveImageWindow
//
//	Parameters out:	None
//
//	Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			HistogramControl
//
//	Coded By:			Ravi S. Budruk			Date: 06/01/1988
//	Revised By:			Ravi S. Budruk			Date: 07/19/1988	
//	Revised By:			Larry L. Biehl			Date: 10/22/2018
	
Boolean HistogramDialog (
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr)
	
{
	Boolean								returnFlag;
	
	#if defined multispec_mac
		DialogSelectArea					dialogSelectArea;
									
		Rect									theBox;
									
		DialogPtr							dialogPtr;
		
		UInt16*								localChannelsPtr;
										
		UserItemUPP							drawHistogramMethodPopUpPtr;
										
		Handle								okHandle,
												theHandle;
		
		SInt32								theNum;
		
		SInt16								itemHit,
												theType;
												
		UInt16								localNumberChannels;
										
		Boolean								allChannelsAtOnceFlag,
												defaultStatFileChangedFlag,
												diskFileFlag,
												histogramInMemoryAvailableFlag,
												includeEmptyBinsFlag,
												lineFormatHistFlag,
												listHistogramFlag,
												listHistSummaryFlag,
												minMaxetcOnlyFlag,
												modalDone,
												textWindowFlag,
												updateListHistogramItemsFlag;

		
				// Initialize some local variables.
		
		dialogPtr = NULL;					
		returnFlag = GetDialogLocalVectors (&localChannelsPtr,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL);
															
				// Get the modal dialog for the histogram specification.					
		
		if (returnFlag)			
			dialogPtr = LoadRequestedDialog (kHistogramSpecificationID, kCopyScrap, 1, 2);
			
		if (dialogPtr == NULL)
			{							
			ReleaseDialogLocalVectors (localChannelsPtr,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL);				
																	
																						return (FALSE);
																					
			}	// end "if (dialogPtr == NULL)"
			
				// Intialize local user item proc pointers.
				
		drawHistogramMethodPopUpPtr = NewUserItemUPP (DrawHistogramMethodPopUp);
		
				// Set Procedure pointers for those dialog items that need them.	
				
		SetDialogItemDrawRoutine (dialogPtr, 7, &theBox, drawHistogramMethodPopUpPtr);	
		
		SetDialogItemDrawRoutine (dialogPtr, 20, gDrawChannelsPopUpPtr);
		
				// Initialize the histogram dialog variables.

		HistogramDialogInitialize (dialogPtr, 
												histogramSpecsPtr,
												gImageWindowInfoPtr,
												fileInfoPtr,
												NULL,
												NULL,
												&gHistogramMethod,
												&histogramInMemoryAvailableFlag,
												&dialogSelectArea,
												localChannelsPtr, 
												&localNumberChannels,
												&gChannelSelection,
												&minMaxetcOnlyFlag,
												&listHistSummaryFlag,
												&listHistogramFlag,
												&lineFormatHistFlag,
												&includeEmptyBinsFlag,
												&textWindowFlag,
												&diskFileFlag,
												&defaultStatFileChangedFlag,
												&updateListHistogramItemsFlag,
												&allChannelsAtOnceFlag);
												
				// Save handle for the OK button for use later.								
				
		GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);

				// Setup the popup menu and default method. First, set all as enabled.
				
		EnableMenuItem (gPopUpHistogramMethodMenu, 1);
		EnableMenuItem (gPopUpHistogramMethodMenu, 2);
		EnableMenuItem (gPopUpHistogramMethodMenu, 4);									
			
		if (!histogramInMemoryAvailableFlag)
			DisableMenuItem (gPopUpHistogramMethodMenu, 1);
			
				// Only allow for computation for thematic type images.
			
		if (fileInfoPtr->thematicType)
			{ 
			DisableMenuItem (gPopUpHistogramMethodMenu, 1);
			DisableMenuItem (gPopUpHistogramMethodMenu, 4);
			
			}	// end "if (fileInfoPtr->thematicType)"
			
		SetDLogControl (dialogPtr, 18, minMaxetcOnlyFlag);
			
				// List Histogram Summary check box.											
				
		SetDLogControl (dialogPtr, 21, listHistSummaryFlag);
			
				// Histogram vector list format.													
				
		SetDLogControl (dialogPtr, 24, (SInt16)!lineFormatHistFlag);
		SetDLogControl (dialogPtr, 25, (SInt16)lineFormatHistFlag);
		
				// Set check box for "text output window".									
				
		SetDLogControl (dialogPtr, 28, (SInt16)textWindowFlag);
		
				// Set check box for "disk file".												
				
		SetDLogControl (dialogPtr, 29, (SInt16)diskFileFlag);
			
				// Center the dialog and then show it.											
				
		ShowDialogWindow (
							dialogPtr, kHistogramSpecificationID, kSetUpDFilterTable);
		
				// Set default text selection to first edit text item						
				
		SelectDialogItemText (dialogPtr, 12, 0, INT16_MAX);
		modalDone = FALSE;
		itemHit = 0;
		do
			{
			ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		
					// If itemHit was a number item, check for bad values.  If			
					// itemHit was a radio button make appropriate control settings	
					// to indicate to the user the present selection.  Get the handle	
					// to the itemHit.  For number value items, get the string and		
					// convert it to a number.														

			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 

			if (itemHit > 2)
				{
				switch (itemHit)
					{
					case 7:
						itemHit = StandardPopUpMenu (dialogPtr, 
																itemHit-1, 
																itemHit, 
																gPopUpHistogramMethodMenu, 
																gHistogramMethod, 
																kPopUpHistogramMethodMenuID);
																
						if (itemHit == 4)
							itemHit = kReadFromSupportFile;
						
						gHistogramMethod = HistogramDialogHandleMethod (
																dialogPtr,
																histogramSpecsPtr,
																gImageWindowInfoPtr,
																fileInfoPtr,
																NULL,
																itemHit,
																1,
																gHistogramMethod,
																&updateListHistogramItemsFlag,
																&defaultStatFileChangedFlag);
		
								// Make certain that the correct label is drawn in the	
								// classification procedure pop up box.
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 9:		// Entire area to selected area switch.
					case 31:		// Entire area to selected area switch. (Appearance Manager)
					case 12:		//	 lineStart
					case 13:		//	 lineEnd
					case 14:		//	 lineInterval
					case 15:		//	 columnStart
					case 16:		//	 columnEnd
					case 17:		//	 columnInterval
						DialogLineColumnHits (&dialogSelectArea,
														dialogPtr, 
														itemHit,
														theHandle,
														theNum);
						break;
						
					case 18:		// check button for "get only min, max, ..." 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						minMaxetcOnlyFlag = !minMaxetcOnlyFlag;
						updateListHistogramItemsFlag = TRUE;
						break;
						
					case 20:		// Selected channels 
						if (gChannelSelection <= 0)
							break;
							
						itemHit = StandardPopUpMenu (dialogPtr, 
																itemHit-1, 
																itemHit, 
																gPopUpAllSubsetMenu, 
																gChannelSelection, 
																kPopUpAllSubsetMenuID);
						if (itemHit == 2)
							{
									// Subset of channels to be used.							
									
							HiliteControl ((ControlHandle)okHandle, 255);
							ChannelsDialog ((SInt16*)&localNumberChannels,
													(SInt16*)localChannelsPtr,
													gImageLayerInfoPtr,
													fileInfoPtr,
													1,
													kNoTransformation,
													NULL,
													gImageWindowInfoPtr->totalNumberChannels,
													gChannelSelection);
								
							if (localNumberChannels == 
												gImageWindowInfoPtr->totalNumberChannels)
								itemHit = 1;
									
							HiliteControl ((ControlHandle)okHandle, 0);
							
							}	// end "if (itemHit == 2)" 
							
						if (itemHit == 1)
							localNumberChannels = gImageWindowInfoPtr->totalNumberChannels;
						
						if (itemHit != 0)
							{
							gChannelSelection = itemHit;
							allChannelsAtOnceFlag = HistogramDialogUpdateAllChannelsAtOnceFlag (
											dialogPtr, localNumberChannels, &lineFormatHistFlag);
										
							}	// end "if (itemHit != 0)" 
		
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						break;
						
					case 21:		// check button for "list histogram summary" 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						break;
						
					case 22:		// check button for "list histogram" 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						listHistogramFlag = !listHistogramFlag;
						updateListHistogramItemsFlag = TRUE;
						break;
						
					case 24:		// radio button for "list format = column" 
					case 25:		// radio button for "list format = line" 
						lineFormatHistFlag = (itemHit != 24);
						SetDLogControl (dialogPtr, 24, !lineFormatHistFlag);
						SetDLogControl (dialogPtr, 25, lineFormatHistFlag);
						updateListHistogramItemsFlag = TRUE;
						break;
						
					case 26:		// check button for "include empty bins" 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
						includeEmptyBinsFlag = !includeEmptyBinsFlag;
						break;
						
					case 28:		// check box for "write results to output window." 
					case 29:		// check box for "write results to disk file." 
						ChangeDLogCheckBox ((ControlHandle)theHandle);
												
						if (GetDLogControl (dialogPtr, 28) == 0 && 
														GetDLogControl (dialogPtr, 29) == 0)
							HiliteControl ((ControlHandle)okHandle, 255);
						else	// GetDLogControl (dialogPtr, 28) == 1 || ... 			
							HiliteControl ((ControlHandle)okHandle, 0);
						break;
							
					}	// end "switch (itemHit)"
						
				if (updateListHistogramItemsFlag)
					{
					HistogramDialogSetListAndEmptyBins (dialogPtr, 
																		gHistogramMethod,
																		minMaxetcOnlyFlag, 
																		listHistogramFlag, 
																		includeEmptyBinsFlag,
																		lineFormatHistFlag);
															
					if (listHistogramFlag)
						{							
						if (GetDLogControl (dialogPtr, 28) == 0 && 
														GetDLogControl (dialogPtr, 29) == 0)
							HiliteControl ((ControlHandle)okHandle, 255);
						else	// GetDLogControl (dialogPtr, 28) == 1 || ... 			
							HiliteControl ((ControlHandle)okHandle, 0);
							
						}	// end "if (listHistogramFlag)" 
						
					else	// !listHistogramFlag 			
						HiliteControl ((ControlHandle)okHandle, 0);
							
					updateListHistogramItemsFlag = FALSE;
						
					}	// end "if (updateListHistogramItemsFlag)" 
					
				}	// end "if (itemHit > 2)"
				
			else if (itemHit > 0)		// and itemHit <= 2 
				{
						// If item hit is 1, check if histogram line-column values make
						// sense.  If they don't, sound alert and make item hit equal	
						// to 0 to allow user to make changes.									
						
				if (itemHit == 1)
					itemHit = CheckLineColValues (&dialogSelectArea,
																histogramSpecsPtr->lineStart,
																histogramSpecsPtr->columnStart,
																dialogPtr);
				
				if (itemHit == 1)
					{
					modalDone = TRUE;   

					HistogramDialogOK (dialogPtr, 
												histogramSpecsPtr,
												gImageWindowInfoPtr,
												gHistogramMethod,
												&dialogSelectArea,
												localChannelsPtr, 
												localNumberChannels,
												gChannelSelection,
												(GetDLogControl (dialogPtr, 18) == 1),
												(GetDLogControl (dialogPtr, 21) == 1),
												(GetDLogControl (dialogPtr, 22) == 1),
												(GetDLogControl (dialogPtr, 25) == 1),
												(GetDLogControl (dialogPtr, 26) == 1),
												(GetDLogControl (dialogPtr, 28) == 1),
												(GetDLogControl (dialogPtr, 29) == 1),
												defaultStatFileChangedFlag,
												allChannelsAtOnceFlag);
												
					returnFlag = TRUE;

					}	// end if (itemHit == 1) 
				
				if (itemHit == 2)
					{
					modalDone = TRUE;
					returnFlag = FALSE;
					
							// Set computeFlag so that new histogram is not computed 	

					histogramSpecsPtr->computeFlag = FALSE;
					
					}	// end "if (itemHit == 2)" 
					
				}	// end "else if itemHit > 0 and itemHit <= 2" 
					
			}	while (!modalDone);
			
		DisposeUserItemUPP (drawHistogramMethodPopUpPtr);
			
		ReleaseDialogLocalVectors (localChannelsPtr,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL);
			
		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
		sControlRef = NULL;
	#endif // defined multispec_mac
	
	#if defined multispec_win
		CMHistogramSpecsDlg*		dialogPtr = NULL;

		TRY
			{ 
			dialogPtr = new CMHistogramSpecsDlg ();
									
			returnFlag = dialogPtr->DoDialog (histogramSpecsPtr,
															gImageWindowInfoPtr,
															fileInfoPtr);
									  
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL 	
	#endif // defined multispec_win
		
	#if defined multispec_lin
		CMHistogramSpecsDlg* dialogPtr = NULL;

		//dialogPtr = new CMHistogramSpecsDlg ((wxWindow *)GetMainFrame ());
		dialogPtr = new CMHistogramSpecsDlg (NULL);

		returnFlag = dialogPtr->DoDialog (histogramSpecsPtr,
														gImageWindowInfoPtr,
														fileInfoPtr);

		delete dialogPtr;
	#endif // defined multispec_lin

	return (returnFlag);
	
}	// end "HistogramDialog" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogHandleMethod
//
//	Software purpose:	The purpose of this routine is to handle the selection
//							in the histogram method popup menu.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/24/1999	
//	Revised By:			Larry L. Biehl			Date: 07/08/2009	

SInt16 HistogramDialogHandleMethod (
				DialogPtr							dialogPtr,
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				CString*								supportFileNamePtr,
				SInt16								itemHit,
				SInt16								okItemNumber,
				SInt16								previousMethodCode,
				Boolean*								updateListHistogramItemsFlagPtr,
				Boolean*								defaultStatFileChangedFlagPtr)

{  
	if (itemHit == kReadFromSupportFile)
		{
		itemHit = 0;
		
				// Get the image statistics file information				
     
		SetDLogControlHilite (dialogPtr, okItemNumber, 255); 
		
		if (GetSTASupportFile (fileInfoPtr))
			{
					// Make sure that the statistics file format is correct.
			
			UpdateSupportFileTypeSetting (windowInfoPtr, histogramSpecsPtr);
	
			HistogramDialogStatisticsFile (histogramSpecsPtr, 
														gImageWindowInfoPtr,
														fileInfoPtr,
														dialogPtr, 
														IDC_DefaultFile,
														supportFileNamePtr);
								
		
			if (CheckIfDefaultHistogramInfoExists (windowInfoPtr) ||
																histogramSpecsPtr->loadedFlag)
				{
				#if defined multispec_mac
					EnableMenuItem (gPopUpHistogramMethodMenu, 1);
				#endif	// defined multispec_mac 
				
				#if defined multispec_win
					CComboBox*			comboBoxPtr;
					SInt16				methodCode;
					
					comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_Method);
					methodCode = (SInt16)comboBoxPtr->GetItemData (0);
					if (methodCode != kStoredInMemory)
						{
		 				comboBoxPtr->InsertString (0, (LPCTSTR)_T("From default statistics file"));
						comboBoxPtr->SetItemData (0, kStoredInMemory);
						
						}	// end "if (methodCode != kStoredInMemory)"
				#endif	// defined multispec_win
				
            #if defined multispec_lin
                wxComboBox* comboBoxPtr;
                SInt16 methodCode;

                comboBoxPtr = (wxComboBox*) dialogPtr->FindWindow (IDC_Method);
                methodCode = (SInt16)((CMHistogramSpecsDlg*)dialogPtr)->
																GetItemCode (comboBoxPtr->GetString (0));
                if (methodCode != kStoredInMemory) {
                    comboBoxPtr->Insert (wxT("From default statistics file"),0);
                    //comboBoxPtr->SetItemData (0, kStoredInMemory);

                } // end "if (methodCode != kStoredInMemory)"
            #endif	// defined multispec_lin

				*updateListHistogramItemsFlagPtr = TRUE;
				*defaultStatFileChangedFlagPtr = TRUE;
				itemHit = kStoredInMemory; 
				
				}	// end "if (CheckIfDefaultHistogramInfoExists (... || ..." 
				
			}	// end "if (GetSTASupportFile (fileInfoPtr))" 
			
		SetDLogControlHilite (dialogPtr, okItemNumber, 0);
		
		}	// end "if (itemHit == kReadFromSupportFile)" 
	
	if (itemHit != previousMethodCode && itemHit > 0)
		{
		*updateListHistogramItemsFlagPtr = TRUE;
		
				// Set the text hilite.											
				
		if (itemHit == kComputeFromImage)
			{
			HistogramDialogShowAreaItems (dialogPtr);
			SelectDialogItemText (dialogPtr, IDC_LineStart ,0, SInt16_MAX);		 
			
			}	// end "if (itemHit == kComputeFromImage)" 
		
		else	// itemHit != kComputeFromImage 
			HistogramDialogHideAreaItems (dialogPtr);
		
		}	// end "if (itemHit != previousMethodCode && ...)" 
		
	else	// itemHit == previousMethodCode || itemHit == 0
		itemHit = previousMethodCode;
		
	return (itemHit);
	
}	// end "HistogramDialogHandleMethod" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogHideAreaItems
//
//	Software purpose:	The purpose of this routine is to hide area selected
//							dialog items.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 05/16/2016

void HistogramDialogHideAreaItems (
				DialogPtr							dialogPtr)

{                                          
	HideDialogItem (dialogPtr, IDEntireImage);
	HideDialogItem (dialogPtr, IDSelectedImage);
	HideDialogItem (dialogPtr, IDC_LineColFrame);
	HideDialogItem (dialogPtr, IDC_StartEndInterval);
	HideDialogItem (dialogPtr, IDC_LinePrompt);
	HideDialogItem (dialogPtr, IDC_ColumnPrompt);
	HideDialogItem (dialogPtr, IDC_LineStart);
	HideDialogItem (dialogPtr, IDC_LineEnd);
	HideDialogItem (dialogPtr, IDC_LineInterval);
	HideDialogItem (dialogPtr, IDC_ColumnStart);
	HideDialogItem (dialogPtr, IDC_ColumnEnd);  
	HideDialogItem (dialogPtr, IDC_ColumnInterval);
	
	#if defined multispec_lin
		HideDialogItem (dialogPtr, IDC_StartPrompt);
		HideDialogItem (dialogPtr, IDC_EndPrompt);
		HideDialogItem (dialogPtr, IDC_IntervalPrompt);
	#endif

	#if defined multispec_mac 
		HideDialogItem (dialogPtr, 31);
	#endif	// defined multispec_mac  
	
}	// end "HistogramDialogHideAreaItems"   



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogHideListItems
//
//	Software purpose:	The purpose of this routine is to show the dialog box
//							items that are only relevant when the list histogram
//							dialog box is checked.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1995
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void  HistogramDialogHideListItems (
				DialogPtr							dialogPtr)

{                             
		HideDialogItem (dialogPtr, IDC_ColumnLinePrompt);
		HideDialogItem (dialogPtr, IDC_ColumnLineFormat);
		#if defined multispec_mac
			HideDialogItem (dialogPtr, 25);
		#endif	// defined multispec_mac 
		HideDialogItem (dialogPtr, IDC_EmptyBins);
		HideDialogItem (dialogPtr, IDC_WriteResultsFrame);  
		HideDialogItem (dialogPtr, IDC_TextWindow);
		HideDialogItem (dialogPtr, IDC_DiskFile);
	
}	// end "HistogramDialogHideListItems"        



void HistogramDialogInitialize (
				DialogPtr							dialogPtr, 
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				CString*								fileNamePtr,
				CString*								supportFileNamePtr,
				SInt16*								histogramMethodCodePtr,
				Boolean*								histogramInMemoryAvailableFlagPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				UInt16*								localChannelsPtr, 
				UInt16*								localNumberChannelsPtr,
				SInt16*								channelSelectionPtr,
				Boolean*								minMaxetcOnlyFlagPtr,
				Boolean*								listHistogramSummaryFlagPtr,
				Boolean*								listHistogramFlagPtr,
				Boolean*								lineFormatHistFlagPtr,
				Boolean*								includeEmptyBinsFlagPtr,
				Boolean*								textWindowFlagPtr,
				Boolean*								diskFileFlagPtr,
				Boolean*								defaultStatFileChangedFlagPtr,
				Boolean*								updateListHistogramItemsFlagPtr,
				Boolean*								allChannelsAtOnceFlagPtr)

{
	Str255							dialogString,
										hdfDataSetName;

	UCharPtr							imageFileNamePtr;

	UInt16							numChars;
	SInt16							entireIconItem;
	
	
			// Update the modal dialog with static text items							
				
	dialogString[0] = 0;
	if (fileInfoPtr->hdfHandle != NULL)
		{
		GetHdfDataSetName (fileInfoPtr, 
									fileInfoPtr->hdfDataSetSelection,
									(StringPtr)&hdfDataSetName,
									NULL);		
		
		numChars = hdfDataSetName[0];
		
		if (numChars > 0)
			{
			CopyPToP (dialogString, (UCharPtr)"\0(hdf set) "); 
			ConcatPStrings ((StringPtr)&dialogString,
									(StringPtr)&hdfDataSetName,
									255);

			}	// end "if (numChars > 0)"
												
		}	// end "if (gImageFileInfoPtr != NULL && ...)"

	if (dialogString[0] == 0)
		{
		imageFileNamePtr = (FileStringPtr)GetFileNamePPointerFromFileInfo (fileInfoPtr);
		CopyPToP (dialogString, imageFileNamePtr);

		}	// end "if (dialogString[0] == 0)"

	#if defined multispec_mac
		if (CreateUnicodeStaticTextControl (
								dialogPtr, &dialogString[1], dialogString[0], 30, NULL) != noErr)
			LoadDItemString (dialogPtr, 30, (Str255*)&dialogString);
	#endif	// defined multispec_mac
	
	#if defined multispec_win
		WideFileStringPtr wFileNamePtr = 
				(WideFileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr, kReturnUnicode);
		if (wFileNamePtr != NULL)
				*fileNamePtr = CString (wFileNamePtr);
		//*fileNamePtr = CString (&dialogString[1]);
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		*fileNamePtr = wxString (&dialogString[1], wxConvUTF8);
	#endif

	#if defined multispec_mac
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 9);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 31);
	#endif	// defined multispec_mac
	
	entireIconItem = 9;
	if (gAppearanceManagerFlag)
		entireIconItem = 31;

			// Load the dialog local vectors

	LoadDialogLocalVectors (localChannelsPtr,
										NULL,
										histogramSpecsPtr->channelsHandle,
										histogramSpecsPtr->numberChannels,
										NULL,
										NULL,
										0,
										NULL,
										NULL,
										NULL,
										NULL,
										NULL,
										0,
										NULL,
										NULL,
										0,
										NULL);
				
			// Initialize selected area structure.		
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gActiveImageWindow,
											histogramSpecsPtr->columnStart,
											histogramSpecsPtr->columnEnd,
											histogramSpecsPtr->columnInterval,
											histogramSpecsPtr->lineStart,
											histogramSpecsPtr->lineEnd,
											histogramSpecsPtr->lineInterval,
											12,
											entireIconItem,
											kDontAdjustToBaseImage);
											
	*localNumberChannelsPtr = histogramSpecsPtr->numberChannels;
	
	*listHistogramSummaryFlagPtr = histogramSpecsPtr->listHistSummaryFlag;	
	*listHistogramFlagPtr = histogramSpecsPtr->listHistFlag;	
	*includeEmptyBinsFlagPtr = histogramSpecsPtr->includeEmptyBinsFlag;
	*lineFormatHistFlagPtr = histogramSpecsPtr->lineFormatHistFlag;
	*minMaxetcOnlyFlagPtr = histogramSpecsPtr->minMaxMeanSDOnlyFlag;		
		
			// Indicate the name of the current default statistics file.			
				
	HistogramDialogStatisticsFile (histogramSpecsPtr, 
												windowInfoPtr,
												fileInfoPtr, 
												dialogPtr,
												IDC_DefaultFile,
												supportFileNamePtr);
	
			// Histogram method.  Get the rectangles for the histogram method		
			// prompt and popup boxes.  Set "Compute Histogram" as the default.
	
	*histogramMethodCodePtr = histogramSpecsPtr->method;
	*defaultStatFileChangedFlagPtr = FALSE;

			// Setup the popup menu and default method.									
		
	if (CheckIfDefaultHistogramInfoExists (windowInfoPtr) ||
															histogramSpecsPtr->loadedFlag)
		{
		if (*histogramMethodCodePtr == kReadFromSupportFile)
			*histogramMethodCodePtr = kStoredInMemory;

		*histogramInMemoryAvailableFlagPtr = TRUE;
		
		}	// end "if (CheckIfDefaultHistogramInfoExists... || ..." 
	
	else	// !CheckIfDefaultHistogramInfoExists... && ... 
		{
		*histogramMethodCodePtr = kComputeFromImage;

		*histogramInMemoryAvailableFlagPtr = FALSE;
		
		}	// end "else !CheckIfDefaultHistogramInfoExists... && ..."
		
			// Only allow for computation for thematic type images.

			// To entire image icon.															
			//	Selected histogram area															
			
	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
		
	if (*histogramMethodCodePtr != kComputeFromImage)
		HistogramDialogHideAreaItems (dialogPtr);
		
	HideDialogItem (dialogPtr, IDC_ComputeOnly);
	
			//	Get the rectangles for the channel prompt and popup boxes.			
			//	Make all channels the default													
			
	if (gProcessorCode == kHistogramProcessor)
		{
		*channelSelectionPtr = histogramSpecsPtr->channelSet;
      #if defined multispec_win || defined multispec_lin
			HideDialogItem (dialogPtr, IDC_AllChannels);
		#endif	// defined multispec_win 

		}	// end "if (gProcessorCode == kHistogramProcessor)"
	
	else	// gProcessorCode != kHistogramProcessor 
		{
		#if defined multispec_mac
			*channelSelectionPtr = -1;
		#endif	// defined multispec_mac 

      #if defined multispec_win || defined multispec_lin
			*channelSelectionPtr = kAllMenuItem;
			HideDialogItem (dialogPtr, IDC_Channels);
		#endif	// defined multispec_win 

		}	// end "else gProcessorCode != kHistogramProcessor"
	
			// Set options for listing the histogram vector.							
					
	HistogramDialogSetListAndEmptyBins (dialogPtr,
														*histogramMethodCodePtr,
														*minMaxetcOnlyFlagPtr, 
														*listHistogramFlagPtr, 
														*includeEmptyBinsFlagPtr,
														*lineFormatHistFlagPtr);
														
	*updateListHistogramItemsFlagPtr = FALSE;
	
			// Determine if only the line format will be allowed.  This occurs	
			// when there is not enough memory available	to get the histogram		
			// vectors for all of the channels at the same time.						
			
	*allChannelsAtOnceFlagPtr = HistogramDialogUpdateAllChannelsAtOnceFlag (
																			dialogPtr,
																			*localNumberChannelsPtr, 
																			lineFormatHistFlagPtr);						
	
	*textWindowFlagPtr = FALSE;
	if (histogramSpecsPtr->outputStorageType & 0x0001)
		*textWindowFlagPtr = TRUE;						
			
	*diskFileFlagPtr = FALSE;
	if (histogramSpecsPtr->outputStorageType & 0x0002)
		*diskFileFlagPtr = TRUE;
	
	if (gProcessorCode != kHistogramProcessor)
		{
		HideDialogItem (dialogPtr, IDC_ListHistogram);
		HistogramDialogHideListItems (dialogPtr);

		}	// end "if (gProcessorCode != kHistogramProcessor)"
	
}	// end "HistogramDialogInitialize"       



void HistogramDialogOK (
				DialogPtr							dialogPtr, 
				HistogramSpecsPtr					histogramSpecsPtr,
				WindowInfoPtr						windowInfoPtr,
				SInt16								histogramMethodCode,
				DialogSelectArea*					dialogSelectAreaPtr,
				UInt16*								localChannelsPtr, 
				UInt16								localNumberChannels,
				SInt16								channelSelection,
				Boolean								minMaxMeanSDOnlyFlag,
				Boolean								listHistogramSummaryFlag,
				Boolean								listHistogramFlag,
				Boolean								lineFormatHistFlag,
				Boolean								includeEmptyBinsFlag,
				Boolean								textWindowFlag,
				Boolean								diskFileFlag,
				Boolean								defaultStatFileChangedFlag,
				Boolean								allChannelsAtOnceFlag)

{		
			// Histogram computation method:										

	histogramSpecsPtr->method = histogramMethodCode;
	if (histogramSpecsPtr->method == kStoredInMemory && defaultStatFileChangedFlag)
		histogramSpecsPtr->method = kReadFromSupportFile;
		
	if (histogramSpecsPtr->method == kStoredInMemory && 
																	!histogramSpecsPtr->loadedFlag)
		histogramSpecsPtr->method = kReadFromSupportFile;

			// Make sure that the statistics file format is correct.
			
	UpdateSupportFileTypeSetting (windowInfoPtr, histogramSpecsPtr);
			
			// Selected reformat  area.											
	
	histogramSpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	histogramSpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	histogramSpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	histogramSpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	histogramSpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	histogramSpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
									
			// Get only min, max, ...												
						
	histogramSpecsPtr->minMaxMeanSDOnlyFlag = minMaxMeanSDOnlyFlag;
		
			// Channels		
														
			// Load some common processor parameters
			// Channels
			// Classes
			
	channelSelection = abs (channelSelection);
	LoadProcessorVectorsFromDialogLocalVectors (
													channelSelection,
													FALSE,
													windowInfoPtr->totalNumberChannels,
													localNumberChannels,
													localChannelsPtr,
													&histogramSpecsPtr->channelSet,
													(UInt16*)&histogramSpecsPtr->numberChannels,
													histogramSpecsPtr->channelsHandle,
													NULL,
													NULL,
													0,
													0,
													NULL,
													NULL,
													NULL,
													NULL,
													0,
													NULL,
													NULL,
													NULL,
													0,
													NULL,
													NULL);
	
			// List Histogram Summary.												

	histogramSpecsPtr->listHistSummaryFlag = listHistogramSummaryFlag;
										
			// List Histogram.														
						
	histogramSpecsPtr->listHistFlag = listHistogramFlag;
										
			// Histogram vector list format.										
						
	histogramSpecsPtr->lineFormatHistFlag = lineFormatHistFlag;
										
			// Include Empty Bins in the list.									
						
	histogramSpecsPtr->includeEmptyBinsFlag = includeEmptyBinsFlag;
										
			// Write output to output text window and/or						
			// Write output to disk file.											
			
	histogramSpecsPtr->outputStorageType = 0;							
	if (textWindowFlag)				
		histogramSpecsPtr->outputStorageType += 1;
	
	if (diskFileFlag)			
		histogramSpecsPtr->outputStorageType += 2;
	
	histogramSpecsPtr->allChannelsAtOnceFlag = TRUE;
	if (histogramSpecsPtr->listHistFlag || 
											histogramSpecsPtr->method == kComputeFromImage)			
		histogramSpecsPtr->allChannelsAtOnceFlag = allChannelsAtOnceFlag;
		
	if (histogramSpecsPtr->listHistFlag && windowInfoPtr->numberBytes > 2)			
		histogramSpecsPtr->allChannelsAtOnceFlag = FALSE;
	
}	// end "HistogramDialogOK" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogSetListAndEmptyBins
//
//	Software purpose:	The purpose of this routine set the list and
//							empty bin dialog check boxes in the histogram
//							dialog window.
//
//	Parameters in:			
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/06/1993
//	Revised By:			Larry L. Biehl			Date: 11/23/1999	

void HistogramDialogSetListAndEmptyBins (
				DialogPtr							dialogPtr,
				SInt16								histogramMethodCode, 
				Boolean								minMaxetcOnlyFlag, 
				Boolean								listHistogramFlag, 
				Boolean								includeEmptyBinsFlag, 
				Boolean								lineFormatHistFlag)

{
	SInt16								setUpType;
	
	
	setUpType = 1;
	
	if (histogramMethodCode == kStoredInMemory)
		{
		if (!CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr))
			setUpType = 1;
			
		else	// CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr)
			setUpType = 2;
			
		}	// end "if (histogramMethodCode == kStoredInMemory)" 
	
	else	// histogramMethodCode != kStoredInMemory 
		{														
		if (minMaxetcOnlyFlag)
			setUpType = 1;
									
		else	// !minMaxetcOnlyFlag 
			setUpType = 2;
			
		}	// end "else histogramMethodCode != kStoredInMemory" 
	
		
	if (setUpType == 1)
		{
		SetDLogControl (dialogPtr, IDC_ListHistogram, 0);
		SetDLogControlHilite (dialogPtr, IDC_ListHistogram, 255);
		HistogramDialogHideListItems (dialogPtr);
		
		}	// end "if (setUpType == 1)" 
								
	else	// setUpType == 2 
		{
		SetDLogControl (dialogPtr, IDC_ListHistogram, listHistogramFlag);
		SetDLogControlHilite (dialogPtr, IDC_ListHistogram, 0);
		
		if (listHistogramFlag)
			{
			HistogramDialogShowListItems (dialogPtr);
			
			if (lineFormatHistFlag)
				{
				SetDLogControl (dialogPtr, IDC_EmptyBins, 1);
				SetDLogControlHilite (dialogPtr, IDC_EmptyBins, 255);
				
				}	// end "if (lineFormatHistFlag)" 
				
			else	// !lineFormatHistFlag 
				{
				SetDLogControl (dialogPtr, IDC_EmptyBins, includeEmptyBinsFlag);
				SetDLogControlHilite (dialogPtr, IDC_EmptyBins, 0);
				
				}	// end "else !lineFormatHistFlag" 
		
			}	// end "if (listHistogramFlag)" 
			
		else	// !listHistogramFlag 
			HistogramDialogHideListItems (dialogPtr);
		
		}	// end "else setUpType == 2" 
	
}	// end "HistogramDialogSetListAndEmptyBins" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogShowAreaItems
//
//	Software purpose:	The purpose of this routine is to hide area selected
//							dialog items.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/28/1995
//	Revised By:			Larry L. Biehl			Date: 05/16/2016	

void HistogramDialogShowAreaItems (
				DialogPtr							dialogPtr)

{                                          
	ShowDialogItem (dialogPtr, IDEntireImage);
	ShowDialogItem (dialogPtr, IDSelectedImage);
	ShowDialogItem (dialogPtr, IDC_LineColFrame);
	ShowDialogItem (dialogPtr, IDC_StartEndInterval);
	ShowDialogItem (dialogPtr, IDC_LinePrompt);
	ShowDialogItem (dialogPtr, IDC_ColumnPrompt);
	ShowDialogItem (dialogPtr, IDC_LineStart);
	ShowDialogItem (dialogPtr, IDC_LineEnd);
	ShowDialogItem (dialogPtr, IDC_LineInterval);
	ShowDialogItem (dialogPtr, IDC_ColumnStart);
	ShowDialogItem (dialogPtr, IDC_ColumnEnd);  
	ShowDialogItem (dialogPtr, IDC_ColumnInterval);
	
	#if defined multispec_lin
		ShowDialogItem (dialogPtr, IDC_StartPrompt);
		ShowDialogItem (dialogPtr, IDC_EndPrompt);
		ShowDialogItem (dialogPtr, IDC_IntervalPrompt);
	#endif
	
	#if defined multispec_mac
		if (gAppearanceManagerFlag)
			ShowDialogItem (dialogPtr, 31);
		else	// !gAppearanceManagerFlag
			ShowDialogItem (dialogPtr, 9);
	#endif	// defined multispec_mac  
	
}	// end "HistogramDialogShowAreaItems"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogShowListItems
//
//	Software purpose:	The purpose of this routine is to show the dialog box
//							items that are only relevant when the list histogram
//							dialog box is checked.
//
//	Parameters in:		None	
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1995
//	Revised By:			Larry L. Biehl			Date: 11/24/1999

void  HistogramDialogShowListItems (
				DialogPtr							dialogPtr)

{                             
		ShowDialogItem (dialogPtr, IDC_ColumnLinePrompt);
		ShowDialogItem (dialogPtr, IDC_ColumnLineFormat);
		#if defined multispec_mac
			ShowDialogItem (dialogPtr, 25);
		#endif	// defined multispec_mac 
		ShowDialogItem (dialogPtr, IDC_EmptyBins);
		ShowDialogItem (dialogPtr, IDC_WriteResultsFrame);  
		ShowDialogItem (dialogPtr, IDC_TextWindow);
		ShowDialogItem (dialogPtr, IDC_DiskFile);
	
}	// end "HistogramDialogShowListItems"        



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void HistogramDialogStatisticsFile
//
//	Software purpose:	This routine lists the histogram title in the output
//							window
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			ListHistogram
//
//	Coded By:			Larry L. Biehl			Date: 10/27/1993
//	Revised By:			Larry L. Biehl			Date: 09/01/2017	

void HistogramDialogStatisticsFile (
				HistogramSpecsPtr					histogramSpecsPtr, 
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr,
				DialogPtr							dialogPtr, 
				SInt16								itemNumber,
				CString*								supportFileNamePtr)

{
	FileStringPtr						fileNamePtr;
											
	CMFileStream*						fileStreamPtr;
	
	
	if (dialogPtr != NULL)
		{
		fileNamePtr = NULL;
												
		if (!CheckIfDefaultHistogramInfoExists (windowInfoPtr))
			{
			gTextString[0] = 0;
			
			if (!histogramSpecsPtr->loadedFlag)
				ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0'None'\0", 63);
				
			else	// histogramSpecsPtr->loadedFlag 
				ConcatPStrings ((UCharPtr)gTextString, (StringPtr)"\0'In memory only'\0", 63);

			fileNamePtr = (UCharPtr)&gTextString;
			
			}	// end "if (!CheckIfDefaultHistogramInfoExists (..." 
			
		else	// CheckIfDefaultHistogramInfoExists (... 
			{
			if (histogramSpecsPtr->statFileFormat == kImagineType)
				//	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
				fileStreamPtr = windowInfoPtr->supportFileStreamPtr;
							
			else	// histogramSpecsPtr->statFileFormat != kImagineType
				fileStreamPtr = windowInfoPtr->supportFileStreamPtr;
				
			fileNamePtr = (FileStringPtr)GetFileNamePPointerFromFileStream (fileStreamPtr);
							
			}	// end "else CheckIfDefaultHistogramInfoExists (..."
		
      #if defined multispec_lin
        *supportFileNamePtr = wxString (&fileNamePtr[1], wxConvUTF8);
      #endif
			
		#if defined multispec_mac
			if (sControlRef == NULL)
				{
				if (CreateUnicodeStaticTextControl (dialogPtr,
																&fileNamePtr[1],
																fileNamePtr[0],
																itemNumber,
																&sControlRef) != noErr)
					LoadDItemString (dialogPtr, itemNumber, (Str255*)fileNamePtr);
				
				}
			else	// sControlRef != NULL
					// Just update the text
				{
				CFStringRef							cfStringRef;
				OSErr									errorCode;
				//Size								size;
				
				cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
																		&fileNamePtr[1],
																		fileNamePtr[0],
																		kCFStringEncodingUTF8,
																		false);
				
				if (cfStringRef != NULL)
					{
					//		Note that the following line does not work.
					//errorCode = SetControlTitleWithCFString (sControlRef, cfStringRef);
					HideControl (sControlRef);
					errorCode = HIViewSetText (sControlRef, cfStringRef);
					/*
							// Following was for testing.
					errorCode = GetControlData (sControlRef, 
															0, 
															kControlStaticTextCFStringTag, 
															sizeof (CFStringRef),
															(Ptr)&cfStringRef2,
															&size);
					*/
					ShowControl (sControlRef);
					CFRelease (cfStringRef);
					
					}	// end "if (cfStringRef != NULL)"
				
				}	// end "else sControlRef != NULL"
		#endif	// defined multispec_mac
			
		#if defined multispec_win	
			TBYTE		wideFileName[256];
			int sizeNeeded = MultiByteToWideChar (
												CP_UTF8, 0, (LPCSTR)&fileNamePtr[1], -1, NULL, 0);
			sizeNeeded = MIN (sizeNeeded, 255);
			MultiByteToWideChar (
							CP_UTF8, 0, (LPCSTR)&fileNamePtr[1], -1, wideFileName, sizeNeeded);

			*supportFileNamePtr = CString (wideFileName);
		#endif	// defined multispec_win
		
		}	// end "if (dialogPtr != NULL)" 

}	// end "HistogramDialogStatisticsFile" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean HistogramDialogUpdateAllChannelsAtOnceFlag
//
//	Software purpose:	The purpose of this routine is to determine if there
//							is enough memory available to load the histograms
//							for all requested channels at the same time.
//							Affected histogram dialog box items are set appropriately.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:			HistogramDialog in histogram.c
//
//	Coded By:			Larry L. Biehl			Date:	11/02/1993
//	Revised By:			Larry L. Biehl			Date: 11/22/2005

Boolean HistogramDialogUpdateAllChannelsAtOnceFlag (
				DialogPtr									dialogPtr, 
				UInt16										numberChannels, 
				Boolean*										lineFormatHistFlagPtr)

{
	UInt32								longestContBlock,
											numberBytesNeeded;
	
	Boolean								returnFlag;

	
			// Determine if only the line format will be allowed.  This occurs	
			// when there is not enough memory available	to get the histogram		
			// vectors for all of the channels at the same time.						
			
	returnFlag = TRUE;
	
	numberBytesNeeded = (SInt32)numberChannels * 
										gImageWindowInfoPtr->numberBins * sizeof (SInt32);
	MGetFreeMemory (&longestContBlock);
	
	if (numberBytesNeeded >= longestContBlock + 50000)
		{
		if (numberChannels > 1)
			{
			#if defined multispec_mac
				SetDLogControlHilite (dialogPtr, 24, 255);
				SetDLogControl (dialogPtr, 24, FALSE);
				SetDLogControl (dialogPtr, 25, TRUE);
			#endif	// defined multispec_mac   
			                             
			#if defined multispec_win
				HideDialogItem (dialogPtr, IDC_ColumnLineFormat);
				ShowHideDialogItem (dialogPtr, IDC_Lines, *lineFormatHistFlagPtr);
			#endif	// defined multispec_win
			
			*lineFormatHistFlagPtr = TRUE;
				
			}	// end "if (numberChannels > 1)"
		
		returnFlag = FALSE;
		
		}	// end "if (numberBytesNeeded >= longestContBlock + 100000)" 
		
	else	// numberBytesNeeded < longestContBlock + 100000 
		{
		#if defined multispec_mac
			SetDLogControlHilite (dialogPtr, 24, 0);
		#endif	// defined multispec_mac   
		                             
      #if defined multispec_win || defined multispec_lin
			HideDialogItem (dialogPtr, IDC_Lines);
		#endif	// defined multispec_win
		
		}	// end "else numberBytesNeeded < longestContBlock + 20000"
		
	return (returnFlag);
		
}	// end "HistogramDialogUpdateAllChannelsAtOnceFlag" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		HistogramSpecsPtr InitializeHistogramInfoStructure
//
//	Software purpose:	This routine creates a structure for the histogram
//							information and initializes it.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	Pointer to the histogram structure.
//
// Called By:			HistogramControl in histogram.c
//							DisplayMultispectralImage in displayMultiSpectral.c
//
//	Coded By:			Larry L. Biehl			Date: 10/01/1993
//	Revised By:			Larry L. Biehl			Date: 06/09/2004

Handle InitializeHistogramInfoStructure (
				Handle								histogramSpecsHandle,
				Boolean								inputPtrFlag,
				Boolean								setUpHistogramArrayFlag)

{
	HistogramSpecsPtr					histogramSpecsPtr = NULL;
	
	HistogramSummaryPtr 				histogramSummaryPtr;
	
	SInt16*								featurePtr;
	
	UInt32								numberBytes;
	
	Boolean								changedFlag,
											continueFlag = TRUE,
											newHandleFlag = FALSE;
	
	SignedByte							handleStatus;
											
	
	if (gImageWindowInfoPtr == NULL)
																					return (NULL);
								
	
	if (histogramSpecsHandle == NULL)
		{                               
		histogramSpecsHandle = MNewHandle (sizeof (HistogramSpecs));
		inputPtrFlag = FALSE;
		newHandleFlag = TRUE;
		
		}	// end "if (histogramSpecsHandle == NULL)"
	
	if (histogramSpecsHandle != NULL)
		{
		if (inputPtrFlag)
			histogramSpecsPtr = (HistogramSpecsPtr)histogramSpecsHandle;
			
		else	// !inputPtrFlag
			{
			histogramSpecsPtr = (HistogramSpecsPtr)GetHandleStatusAndPointer (
																				histogramSpecsHandle,
																				&handleStatus);
			
			if (newHandleFlag)
				{
				histogramSpecsPtr->initializeStructureFlag = TRUE;
				histogramSpecsPtr->channelsHandle = NULL;
				
				}	// end "if (newHandleFlag)"
					
			}	// end "else !inputPtrFlag"
			
		}	// end "if (histogramSpecsHandle != NULL)"
							
	if (histogramSpecsPtr != NULL && histogramSpecsPtr->initializeStructureFlag)
		{
				// Make certain that the size of the histogram summary structure is			
				// consistant with the number of channels in the active image		
				// window.								
								
		numberBytes = (UInt32)gImageWindowInfoPtr->totalNumberChannels * 
																		sizeof (HistogramSummary);
																
		Handle* histogramSummaryHandlePtr = 
								GetHistogramSummaryHandlePtr (gImageWindowInfoPtr);
																
		histogramSummaryPtr = (HistogramSummaryPtr)CheckHandleSize (
																			histogramSummaryHandlePtr,
																			&continueFlag, 
																			&changedFlag, 
																			numberBytes);
		
		if (changedFlag)
			InitializeHistogramSummaryStructure (histogramSummaryPtr,
														gImageWindowInfoPtr->totalNumberChannels);
										
				// Verify that the size of the channels vector is consistant with
				// the number of channels in the acive image window.
		
		if (continueFlag)
			{
			numberBytes = (UInt32)gImageWindowInfoPtr->totalNumberChannels * 
																			2 * sizeof (short int);
			featurePtr = (SInt16*)CheckHandleSize (
										&histogramSpecsPtr->channelsHandle, 
										&continueFlag, 
										&changedFlag, 
										numberBytes);
						
			}	// end "if (continueFlag)" 
				
		if (continueFlag)
			{		
			histogramSpecsPtr->histogramArrayH = NULL;
			histogramSpecsPtr->statBufferPtr = NULL;
			histogramSpecsPtr->histogramSummaryPtr = NULL;
			histogramSpecsPtr->histogramArrayPtr = NULL;
			histogramSpecsPtr->totalPixels = 0;
			
			histogramSpecsPtr->maxNumberBins = gImageWindowInfoPtr->numberBins;
			if (gImageWindowInfoPtr->numberBytes > 2)
				histogramSpecsPtr->maxNumberBins = 2048;
			
			histogramSpecsPtr->numberChannels = 
													gImageWindowInfoPtr->totalNumberChannels;
			histogramSpecsPtr->channelSet = kAllMenuItem;
			histogramSpecsPtr->method = kComputeFromImage;
			histogramSpecsPtr->numBadLines = 0;
			histogramSpecsPtr->numberStatBufferChannels = 0;
			histogramSpecsPtr->numberSummaryChannels = 
													gImageWindowInfoPtr->totalNumberChannels;
			histogramSpecsPtr->outputStorageType = 1;
			
			histogramSpecsPtr->statFileFormat = kErdas74Type;
			if (gImageWindowInfoPtr->numberImageFiles == 1 &&
														gImageFileInfoPtr->format == kImagineType)
				histogramSpecsPtr->statFileFormat = kImagineType;
			
			histogramSpecsPtr->allChannelsAtOnceFlag = TRUE;
			histogramSpecsPtr->computeFlag = FALSE;
			histogramSpecsPtr->firstTime = TRUE;
			histogramSpecsPtr->histogramChangedSinceDisplayFlag = TRUE;
			histogramSpecsPtr->includeEmptyBinsFlag = FALSE;
			histogramSpecsPtr->initializeStructureFlag = FALSE;
			histogramSpecsPtr->lineFormatHistFlag = FALSE;
			histogramSpecsPtr->listHistFlag = FALSE;
			
			histogramSpecsPtr->listHistSummaryFlag = FALSE;
			
			histogramSpecsPtr->loadedFlag = FALSE;
			histogramSpecsPtr->minMaxMeanSDOnlyFlag = FALSE;
			
					// Make certain that feature vector is filled in properly.			
			
			if (histogramSpecsPtr->channelSet == kAllMenuItem)		
				LoadFeatureVector (kNoTransformation,
											(SInt16*)&histogramSpecsPtr->numberChannels,
											featurePtr,
											gImageWindowInfoPtr->totalNumberChannels);
				
					// This will be locked back again in SetUpHistogramSpecsPtr.
			
			}	// end "if (continueFlag)" 
				
				// These handles will be locked back again in SetUpHistogramSpecsPtr.
				
		CheckAndUnlockHandle (*histogramSummaryHandlePtr);
		CheckAndUnlockHandle (histogramSpecsPtr->channelsHandle);
		
		if (!inputPtrFlag)
			MHSetState (histogramSpecsHandle, handleStatus);
			
		}	// end "if (histogramSpecsPtr != NULL && ..."
		
			// Set up the memory to be used to read histogram information 		
			// from disk if requested.														
	
	if (setUpHistogramArrayFlag && 
					histogramSpecsPtr != NULL && histogramSpecsPtr->loadedFlag)
		continueFlag = SetUpToReadHistogramArray (histogramSpecsPtr);
																				
	return (histogramSpecsHandle);
									
}	// end "InitializeHistogramInfoStructure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void InitializeHistogramSummaryStructure
//
//	Software purpose:	This routine initializes the histogram summary structure.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None.
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 06/09/2004
//	Revised By:			Larry L. Biehl			Date: 12/16/2005

void InitializeHistogramSummaryStructure (
				HistogramSummaryPtr 				histogramSummaryPtr,
				UInt32								numberChannels)

{
	UInt32								chanIndex;
											
	
	if (histogramSummaryPtr == NULL)
																							return;
	
	for (chanIndex=0; chanIndex<numberChannels; chanIndex++)
		{
		histogramSummaryPtr[chanIndex].averageValue = 0.;
		histogramSummaryPtr[chanIndex].binFactor = 0.;
		histogramSummaryPtr[chanIndex].maxNonSatValue = 0.;
		histogramSummaryPtr[chanIndex].maxValue = 0.;
		histogramSummaryPtr[chanIndex].medianValue = 0.;
		histogramSummaryPtr[chanIndex].minNonSatValue = 0.;
		histogramSummaryPtr[chanIndex].minValue = 0.;
		histogramSummaryPtr[chanIndex].stdDeviation = 0.;
		histogramSummaryPtr[chanIndex].badValues = 0;
		histogramSummaryPtr[chanIndex].numberBins = 256;
		histogramSummaryPtr[chanIndex].binType = kDataValueIsBinIndex;
		histogramSummaryPtr[chanIndex].numberEDecimalDigits = 0;
		histogramSummaryPtr[chanIndex].numberFDecimalDigits = 0;
		histogramSummaryPtr[chanIndex].availableFlag = FALSE;
		
		}	// end "for (chanIndex=0; chanIndex<numberChannels; chanIndex++)"
									
}	// end "InitializeHistogramSummaryStructure"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHistogramSummary
//
//	Software purpose:	The purpose of this routine is to list the 
//							histogram statistics in the output window.
//
//	Functional description:	The steps in this function follow.
//
//			1) List the histogram block information, i.e. the line and column
//				information. For example:
//
//										Block Histogramed
//		First 		Last			Line		   First		   Last		 Column
//		Line			Line	     Interval     Column	  Column	   Interval
//		  1   		512			 1				  1			512	       1
//
//			2) List the statistics out to the output window for each channel i.
//				For example:
//				
//  Channel       Data Range     Mean          Standard      
//                                            Deviation     
//
//     1           90 to 233     150             10             70 to 250  
//     2
//    
//    etc
//
//	Parameters in:		&HistogramSummary[i]
//							&HistogramSpecs
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:			Menus   in menu.c
//							HistogramControl
//
//	Coded By:			Ravi S. Budruk			Date: 06/18/1988
//	Revised By:			Ravi S. Budruk			Date: 08/11/1988	
//	Revised By:			Larry L. Biehl			Date: 05/27/2015	

Boolean ListHistogramSummary (
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr,
				CMFileStream*						resultsFileStreamPtr)
				
{

			// Declare local parameters	

	char									formatString[256];
	char									string[1000];
	char									*blankPtr = (char*)"                ";
	
	ChannelDescriptionPtr			channelDescriptionPtr;
	FileInfoPtr							localFileInfoPtr;
	HistogramSummaryPtr				histogramSummaryPtr;
	GrafPtr								savedPort;
	
	SInt16								*channelsPtr;
	
	SInt32								signedValueOffset;
	
	#if use_mlte_for_text_window
		TXNObject							textH;
	#endif
	#if !use_mlte_for_text_window
		WEReference							textH;
	#endif	
	
	SInt16								aveLength,
											channelNum,
											fileChannelNumber, 
											fileInfoIndex,
											index,
											length,
											localImageType,
											maxLength,
											medianLength,
											minLength,
											numberChannels,
											ePrecision,
											fPrecision,
											statEPrecision,
											statFPrecision,
											stdDevLength;
	
	Boolean								continueFlag,
											forceEFormatFlag;
	
	
	continueFlag = TRUE;
	
	if (histogramSpecsPtr->loadedFlag && 
							histogramSpecsPtr->listHistSummaryFlag)
		{
				// Put message in histogram status dialog									
		
		LoadDItemStringNumber (kHistogramStrID, 
										IDS_HistogramStatus2, 
										gStatusDialogPtr, 
										IDC_Status2, 
										(Str255*)gTextString);
	
		GetPort (&savedPort);
		SetPortWindowPort (gOutputWindow);
	
		textH = gOutputTextH;
		localImageType = gImageWindowInfoPtr->imageType;
		histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
		
		continueFlag = ListSpecifiedStringNumber (kHistogramStrID, 
																IDS_HistogramTitle8, 	// 12
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputCode,
																continueFlag);
																						
				// List the title of the histogram block into the output window	
				// if the histogram was computed.											
		
		if (continueFlag && histogramSpecsPtr->method == kComputeFromImage)
			{
			continueFlag = ListSpecifiedStringNumber (kHistogramStrID, 
																	IDS_HistogramTitle9,		// 13,
																	(unsigned char*)gTextString, 
																	resultsFileStreamPtr, 
																	gOutputCode, 
																	continueFlag);
		
					// List the input histogram block data into the output window		
			
			sprintf (string, 
							//"    %5ld\t%8ld\t%8ld\t%10ld\t%9ld\t%7ld%s%s",
							"    %5u\t%8u\t%8u\t%10u\t%9u\t%7u%s%s",	// Need to use %u for linux
							(unsigned int)histogramSpecsPtr->lineStart,
							(unsigned int)histogramSpecsPtr->lineEnd,
							(unsigned int)histogramSpecsPtr->lineInterval,
							(unsigned int)histogramSpecsPtr->columnStart,
							(unsigned int)histogramSpecsPtr->columnEnd,
							(unsigned int)histogramSpecsPtr->columnInterval,
							gEndOfLine,
							gEndOfLine);
			
			continueFlag = OutputString (resultsFileStreamPtr,
													string,
													(UInt32)strlen (string),
													gOutputCode, 
													continueFlag);
			
			}	// end "if (... && ...->method == kComputeFromImage)" 
	
				// List the statistics in the output window for each channel [i]	

		gTextString2[16] = 0;
		fileInfoIndex = -1;
		numberChannels = histogramSpecsPtr->numberChannels;
		channelsPtr = (SInt16*)GetHandlePointer (histogramSpecsPtr->channelsHandle);
							
		forceEFormatFlag = DetermineIfEFormatRequired (histogramSummaryPtr,
																		numberChannels,
																		channelsPtr);
							
				// Determine the max length need for the histogram summary values.
		
		minLength = 6;
		maxLength = 6;
		aveLength = 8;
		medianLength = 6;
		stdDevLength = 10;
		
		ePrecision = fileInfoPtr->maxNumberEDecimalDigits;
		fPrecision = fileInfoPtr->maxNumberFDecimalDigits;
		statEPrecision = MAX (1, ePrecision);
		statFPrecision = MAX (1, fPrecision);
									
		for (index=0; 
				index<numberChannels; 
				index++)
			{
			channelNum = channelsPtr[index];
			
			if (histogramSummaryPtr[channelNum].availableFlag)
				{
				length = FormatHistogramSummaryString (
													string,
													histogramSummaryPtr[channelNum].minValue,
													ePrecision,
													fPrecision,
													forceEFormatFlag);
				minLength = MAX (minLength, length);
				
				length = FormatHistogramSummaryString (
													string,
													histogramSummaryPtr[channelNum].maxValue,
													ePrecision,
													fPrecision,
													forceEFormatFlag);
				maxLength = MAX (maxLength, length);
				
				length = FormatHistogramSummaryString (
													string,
													histogramSummaryPtr[channelNum].averageValue,
													statEPrecision,
													statFPrecision,
													forceEFormatFlag);
				aveLength = MAX (aveLength, length);
				
				length = FormatHistogramSummaryString (
													string,
													histogramSummaryPtr[channelNum].medianValue,
													ePrecision,
													fPrecision,
													forceEFormatFlag);
				medianLength = MAX (medianLength, length);
				
				length = FormatHistogramSummaryString (
													string,
													histogramSummaryPtr[channelNum].stdDeviation,
													statEPrecision,
													statFPrecision,
													forceEFormatFlag);
				stdDevLength = MAX (stdDevLength, length);
									
				}	// end "if (...histogram[channelNum].availableFlag)" 
			
			}	// end "for (index=0; ... "
				
		continueFlag = MGetString ((unsigned char*)gTextString,
												kHistogramStrID, 
												IDS_HistogramTitle10);
												
		if (continueFlag)
			{
			gTextString [gTextString[0]+1] = 0;
			
			SInt32 beforeRangeSpaces = (minLength+maxLength+4-12)/2;
			SInt32 beforeAverageSpaces = minLength+maxLength+4-10 - beforeRangeSpaces + 
																						(aveLength-4)/2;
			SInt32 beforeMedianSpaces = aveLength-4 - (aveLength-4)/2 + (medianLength-6)/2;
			SInt32 beforeStdDevSpaces = medianLength-6 - (medianLength-6)/2 + 
																						(stdDevLength-9)/2;
			SInt32 beforeDeviationSpaces = beforeRangeSpaces +
							beforeAverageSpaces + beforeMedianSpaces + beforeStdDevSpaces;
															
			sprintf (string, 
							(char*)&gTextString[1], 
							beforeRangeSpaces, " ", 
							beforeAverageSpaces, " ", 
							beforeMedianSpaces, " ", 
							beforeStdDevSpaces, " ", 
							beforeDeviationSpaces, " ");
				
			continueFlag = OutputString (resultsFileStreamPtr,
													string, 
													(UInt32)strlen (string),
													gOutputCode, 
													continueFlag);
				
			}	// end "if (continueFlag)"
			
		fileInfoIndex = -1;
		for (index=0; 
				index<numberChannels; 
				index++)
			{
			channelNum = channelsPtr[index];
			
					// Get the proper channel and fileInfoPtr for the specified		
					// window "channel".															
			
			if (fileInfoIndex != 
								(SInt16)gImageLayerInfoPtr[channelNum+1].fileInfoIndex)
				{
				if (fileInfoIndex != -1)	
					CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
						
				fileInfoIndex = gImageLayerInfoPtr[channelNum+1].fileInfoIndex;
				localFileInfoPtr = &fileInfoPtr[fileInfoIndex];
			
						// Get pointer to channel descriptions if they exist.			
			
				if (localFileInfoPtr->descriptionsFlag)
					channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
								localFileInfoPtr->channelDescriptionH, kLock);
		
						// Get the offset to use for signed data in case it is		
						// needed.																	
						
				signedValueOffset = localFileInfoPtr->signedValueOffset;
				
				}	// end "if (fileInfoIndex != ..." 
				
			fileChannelNumber = 
							gImageLayerInfoPtr[channelNum+1].fileChannelNumber;
									
			if (!localFileInfoPtr->descriptionsFlag || localImageType != kMultispectralImageType)
				BlockMoveData (blankPtr, gTextString2, 16);
											
			else	// localFileInfoPtr->descriptionsFlag && ... 
				BlockMoveData (
					&channelDescriptionPtr[fileChannelNumber-1], gTextString2, 16);
			/*
			if (localFileInfoPtr->dataTypeCode == kIntegerType)
				{		
				precision = 0;
				statPrecision = 1;
				
				}	//end "if (localFileInfoPtr->dataTypeCode == kIntegerType)"
				
			else	// localFileInfoPtr->dataTypeCode == kRealType
				{
				precision = 1;
				statPrecision = 2;
				
				}	// end "else localFileInfoPtr->dataTypeCode == kRealType"
			*/
			if (histogramSummaryPtr[channelNum].availableFlag)
				{
						// Build the format string
						
				strcpy (formatString, "    %5d\t  %s");
				
				//ePrecision = histogramSummaryPtr[channelNum].numberEDecimalDigits;
				//fPrecision = histogramSummaryPtr[channelNum].numberFDecimalDigits;
				//statEPrecision = MAX (1, ePrecision);
				//statFPrecision = MAX (1, fPrecision);
				
				GetDataFormatString (formatString, 
												histogramSummaryPtr[channelNum].minValue,
												minLength,
												ePrecision,
												fPrecision,
												forceEFormatFlag);
												
				strcat (formatString, "\t to");
				
				GetDataFormatString (formatString, 
												histogramSummaryPtr[channelNum].maxValue,
												maxLength,
												ePrecision,
												fPrecision,
												forceEFormatFlag);
				
				GetDataFormatString (formatString, 
												histogramSummaryPtr[channelNum].averageValue,
												aveLength,
												statEPrecision,
												statFPrecision,
												forceEFormatFlag);
				
				GetDataFormatString (formatString, 
												histogramSummaryPtr[channelNum].medianValue,
												medianLength,
												ePrecision,
												fPrecision,
												forceEFormatFlag);
				
				GetDataFormatString (formatString, 
												histogramSummaryPtr[channelNum].stdDeviation,
												stdDevLength,
												statEPrecision,
												statFPrecision,
												forceEFormatFlag);
												
				strcat (formatString, "%s");
			
				sprintf (string, 
								formatString,
								channelNum+1,
								gTextString2,
								histogramSummaryPtr[channelNum].minValue,
								histogramSummaryPtr[channelNum].maxValue,
								histogramSummaryPtr[channelNum].averageValue,
								histogramSummaryPtr[channelNum].medianValue,
								histogramSummaryPtr[channelNum].stdDeviation,
								gEndOfLine);
								
				continueFlag = OutputString (resultsFileStreamPtr,
														string,
														(UInt32)strlen (string),
														gOutputCode, 
														continueFlag);
				
				}	// end "if (...histogram[channelNum].availableFlag)" 
				
			if (!continueFlag)
				break;
			
			}	// end "for (index=0; ... " 
		
				// List 2 carriage returns.
			
		//continueFlag = OutputString (resultsFileStreamPtr,
		//											&gEndOfLine[0],
		//											gNumberOfEndOfLineCharacters,
		//											gOutputCode,
		//											continueFlag);
														
		//continueFlag = OutputString (resultsFileStreamPtr,
		//											&gEndOfLine[0],
		//											gNumberOfEndOfLineCharacters,
		//											gOutputCode,
		//											continueFlag);

		SetPort (savedPort);
		
		CheckAndUnlockHandle (localFileInfoPtr->channelDescriptionH);
	
		}	// end "if (histogramSpecsPtr->loadedFlag ..."
		
	return (continueFlag);

}	// end "ListHistogramSummary"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHistogramTitle
//
//	Software purpose:	This routine lists the histogram title in the output
//							window
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			ListHistogram
//
//	Coded By:			Larry L. Biehl			Date: 10/27/1993
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean ListHistogramTitle (
				HistogramSpecsPtr					histogramSpecsPtr, 
				CMFileStream*						resultsFileStreamPtr)

{
	char									string[128];
	
	CMFileStream						*fileStreamPtr,
											*supportFileStreamPtr;
	
	SInt16								numberChars,
											stringIndex;
	
	Boolean								continueFlag;
	
	SignedByte							handleStatus;
	
	
	continueFlag = TRUE;		
	supportFileStreamPtr = GetActiveSupportFileStreamPointer (&handleStatus);
	
	if (supportFileStreamPtr == NULL)
																				return (FALSE);
					
			// List the processor name, date, time and image file info.	
				
	if (gProcessorCode == kHistogramProcessor)
		continueFlag = ListHeaderInfo (resultsFileStreamPtr,
													kLImageInfo, 
													&gOutputCode, 
													kNoStatisticsUsed, 
													continueFlag);
	
			// Insert a blank line.
			
	continueFlag = OutputString (resultsFileStreamPtr, 
											gEndOfLine, 
											gNumberOfEndOfLineCharacters, 
											gOutputForce1Code, 
											continueFlag);
									
			// List "  Output Information:".
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID, 
																IDS_Shared8, 
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
	
	stringIndex = IDS_HistogramTitle2;	// 6		
	switch (histogramSpecsPtr->method)
		{
		case kStoredInMemory:
			if (!CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr))
				stringIndex = IDS_HistogramTitle2;	// 6
			
			else	// CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr)
				stringIndex = IDS_HistogramTitle3;		// 7
			break;
			
		case kComputeFromImage:
			if (!CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr))
				stringIndex = IDS_HistogramTitle4;	// 8
			
			else	// CheckIfDefaultHistogramInfoExists (gImageWindowInfoPtr)
				stringIndex = IDS_HistogramTitle5;	// 9
			break;
			
		case kReadFromSupportFile:
			stringIndex = IDS_HistogramTitle3;	// 7
			break;
			
		}	// end "switch (...histogramSpecs.method)"
		
	if (histogramSpecsPtr->statFileFormat == kImagineType)
		fileStreamPtr = supportFileStreamPtr;
		
	else	// histogramSpecsPtr->statFileFormat != kImagineType
		fileStreamPtr = supportFileStreamPtr;
		
	char* fileNamePtr = (char*)GetFileNameCPointerFromFileStream (fileStreamPtr);
	continueFlag = ListSpecifiedStringNumber (kHistogramStrID,
															stringIndex,
															resultsFileStreamPtr, 
															gOutputCode, 
															fileNamePtr,
															continueFlag,
															kUTF8CharString);

	if (histogramSpecsPtr->listHistSummaryFlag || 
										histogramSpecsPtr->listHistFlag)
		{
		if (histogramSpecsPtr->numBadLines >= 0)
			{
					// List the number of bad lines of data.
					
			continueFlag = ListSpecifiedStringNumber (kHistogramStrID,
																	IDS_HistogramTitle6,
																	resultsFileStreamPtr, 
																	gOutputCode, 
																	histogramSpecsPtr->numBadLines, 
																	continueFlag);
			
			}	// end "if (histogramSpecsPtr->...." 
			
				// List the total number of pixels in the histogram.	
			
		numberChars = sprintf (string,
										"%ld",  
										histogramSpecsPtr->totalPixels);

		numberChars = InsertCommasInNumberString (
										string, 
										numberChars, 
										-1,
										numberChars);
				
		continueFlag = ListSpecifiedStringNumber (
									kHistogramStrID, 
									IDS_HistogramTitle7,
									resultsFileStreamPtr, 
									gOutputCode, 
									string, 
									continueFlag);
		
		}	// end "if (...listHistSummaryFlag || ...listHistFlag)" 
												
	return (continueFlag);

}	// end "ListHistogramTitle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHistogramValues
//
//	Software purpose:	This routine prints the histogram to the output window
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			ListHistogram
//
//	Coded By:			Ravi S. Budruk			Date: 07/13/1988
//	Revised By:			Ravi S. Budruk			Date: 08/09/1988	
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

Boolean ListHistogramValues (
				HistogramSpecsPtr					histogramSpecsPtr,
				FileInfoPtr							fileInfoPtr, 
				HUInt32Ptr							histogramArrayPtr, 
				CMFileStream*						resultsFileStreamPtr, 
				SInt16								startChannelIndex)

{
			// Declare parameters																
	
	HPtr									charPtr;
											
	GrafPtr								savedPort;
	
	HistogramSummaryPtr				histogramSummaryPtr;
	HUInt32Ptr							savedHistogramArrayPtr;
	
	double								binFactor,
											dataValue,
											forLineMaxValue,
											forLineMinValue,
											maxValue,
											minValue;
	
	SInt32								endValue,
											index,
											m,
											numberBytes,
				 							signedValueOffset,
				 							startValue,
				 							value;
	
	UInt32								channelIndex,
											lastChannelIndex,
											numberBufferBytes,
											summaryIndex,
											textLength;
	
	SInt16								*featurePtr,
											numberChannels; 
	
	UInt16								channelTextWidth,
											countTextWidth,
											ePrecision,
											fPrecision,
											maxValueCountTextWidth,
											minValueCountTextWidth,
											stringIndex,
											valueCountTextWidth,
											valueMaxNonSatFieldSize,
											valueMinNonSatFieldSize;
	
	Boolean								continueFlag,
											includeEmptyBinsFlag,
											lineFormatHistFlag,
											oneTitleLinesForAllChannelsFlag;
	
	
	if (!histogramSpecsPtr->listHistFlag)			
																					return (TRUE);
																								
	if (histogramArrayPtr == NULL)
																					return (TRUE);
																					
	histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
	savedHistogramArrayPtr = histogramArrayPtr;
	
			// Get the channel feature pointer																		
			                                                       
	featurePtr = (SInt16*)GetHandlePointer (histogramSpecsPtr->channelsHandle);
	index = featurePtr[startChannelIndex];
	if (!histogramSummaryPtr[index].availableFlag)			
																					return (TRUE);
		
			// Put message in histogram status dialog										
	
	LoadDItemStringNumber (kHistogramStrID, 
									IDS_HistogramStatus2, 
									gStatusDialogPtr, 
									IDC_Status2, 
									(Str255*)gTextString);
	
			// Get the number of channels											
			
	numberChannels = histogramSpecsPtr->numberChannels;
	
	signedValueOffset = fileInfoPtr->signedValueOffset;
	
	continueFlag = TRUE;
	
			// Determine if one set of title lines will work for all channels.
			
	oneTitleLinesForAllChannelsFlag = TRUE;
	m = featurePtr[index];	 
	binFactor = histogramSummaryPtr[m].binFactor;
	for (index=0; index<numberChannels; index++)
		{
		m = featurePtr[index];
		if (binFactor != histogramSummaryPtr[m].binFactor)
			{
			oneTitleLinesForAllChannelsFlag = FALSE;
			break;
			
			}	// end "if (binFactor != histogramSummaryPtr[m].binFactor)"
				
		}	// end "for (index=0; index<..." 
		
	if (gImageWindowInfoPtr->numberBytes > 2)
		oneTitleLinesForAllChannelsFlag = FALSE;
	
	if (!histogramSpecsPtr->allChannelsAtOnceFlag)
		oneTitleLinesForAllChannelsFlag = FALSE;
	
			// Get the format for the histogram table - line or column, 			
			// include/not include empty bins, use one or multiple labels.			
	
	lineFormatHistFlag = histogramSpecsPtr->lineFormatHistFlag;
	includeEmptyBinsFlag = histogramSpecsPtr->includeEmptyBinsFlag;
	lastChannelIndex = numberChannels - 1;
							
	if (!oneTitleLinesForAllChannelsFlag)
		lastChannelIndex = startChannelIndex;
	
	numberBufferBytes = 0;
		
			// Make certain that the output window is the active port.				
			
	GetPort (&savedPort);
	SetPortWindowPort (gOutputWindow);
	
	for (channelIndex=startChannelIndex; 
			channelIndex<=lastChannelIndex; 
			channelIndex++)
		{
		summaryIndex = featurePtr[channelIndex];
		
		if (!oneTitleLinesForAllChannelsFlag || startChannelIndex == 0)
			{
			if (startChannelIndex == 0 && oneTitleLinesForAllChannelsFlag)
				{
						// Determine the the minimum and maximum index values for all		 
						// channels combined. Note that for now this is only for 1 and 2
						// byte data.  If 4-byte or more data are listed with one title
						// then this logic will have to be changed.														
			
				minValue = UInt16_MAX;
				maxValue = 0;
				 
				for (index=0; index<numberChannels; index++)
					{
					m = featurePtr[index];
					
					value = (SInt32)(histogramSummaryPtr[m].minValue + signedValueOffset);
					if (value == 0)
						value = (SInt32)(histogramSummaryPtr[m].minNonSatValue + 
																						signedValueOffset);
						
					minValue = MIN (minValue, value);
											
					value = (SInt32)(histogramSummaryPtr[m].maxValue + signedValueOffset);
					if (value == gImageWindowInfoPtr->numberBins - 1)
						value = (SInt32)(histogramSummaryPtr[m].maxNonSatValue + 
																						signedValueOffset);
											
					maxValue = MAX (maxValue, value);
							
					}	// end "for (index=0; index<..." 
					
						// Make certain that the minimum and maximum values are not out 	
						// of range if the bin index is the data value which it is when the
						// number of bytes of data is 2 or less.  Note that this will not handle
						// correctly cases for linked data when there is a mix of 2 and 4 byte
						// data.																		
						
				if (gImageWindowInfoPtr->numberBytes <= 2)
					{
					maxValue = MIN (maxValue, (SInt32)gImageWindowInfoPtr->numberBins-1);
					minValue = MIN (minValue, (SInt32)gImageWindowInfoPtr->numberBins-1);
						
					}	// end "if (gImageWindowInfoPtr->numberBytes <= 2)"
					
				binFactor = 1.;
	
						// Get the maximum length of the field expected for the data value.
						
				ePrecision = fileInfoPtr->maxNumberEDecimalDigits;
				fPrecision = fileInfoPtr->maxNumberFDecimalDigits;
					
				}	// end "if (channelIndex == 0 && oneTitleLinesForAllChannelsFlag)" 
				
			else	// !oneTitleLinesForAllChannelsFlag
				{
				minValue = histogramSummaryPtr[summaryIndex].minValue + signedValueOffset;
				maxValue = histogramSummaryPtr[summaryIndex].maxValue + signedValueOffset;
				binFactor = histogramSummaryPtr[summaryIndex].binFactor;
	
						// Get the maximum length of the field expected for the data value.
				
				ePrecision = histogramSummaryPtr[summaryIndex].numberEDecimalDigits;
				fPrecision = histogramSummaryPtr[summaryIndex].numberFDecimalDigits;
					
				}	// end "else !oneTitleLinesForAllChannelsFlag"
			
			histogramSpecsPtr->overallMaxValue = maxValue;
			histogramSpecsPtr->overallMinValue = minValue;
			
			if (fileInfoPtr->numberBytes <= 2)
				{
				NumToString ((UInt32)fileInfoPtr->numberBins-signedValueOffset, gTextString);
				maxValueCountTextWidth = gTextString[0];
				
				NumToString ((SInt64)(-signedValueOffset), gTextString);
				minValueCountTextWidth = gTextString[0];
													
				valueCountTextWidth = MAX (minValueCountTextWidth, maxValueCountTextWidth);
				minValueCountTextWidth = valueCountTextWidth;
				maxValueCountTextWidth = valueCountTextWidth;
				
				}	// end "if (fileInfoPtr->numberBytes <= 2)"
				
			else	// fileInfoPtr->numberBytes > 2
				{
				minValueCountTextWidth = FormatHistogramSummaryString (
															(char*)gTextString,
															histogramSpecsPtr->overallMinValue,
															ePrecision,
															fPrecision,
															FALSE);
				
				maxValueCountTextWidth = FormatHistogramSummaryString (
															(char*)gTextString,
															histogramSpecsPtr->overallMaxValue,
															ePrecision,
															fPrecision,
															FALSE);
				
				if (histogramSpecsPtr->overallMinValue < -kMaxValueToListWith_f &&
									histogramSpecsPtr->overallMaxValue > kMaxValueToListWith_f)
					{
							// Have to allow for some data being listed as f-format
							// As of now the max f-format length is 10 plus decimal and sign.
					
					maxValueCountTextWidth = MAX (maxValueCountTextWidth, 12 + fPrecision);
					
					}	// end "if (histogramSpecsPtr->overallMinValue < ..."
				
				valueMinNonSatFieldSize = FormatHistogramSummaryString (
													(char*)gTextString,
													histogramSummaryPtr[summaryIndex].minNonSatValue,
													ePrecision,
													fPrecision,
													FALSE);
				
				valueMaxNonSatFieldSize = FormatHistogramSummaryString (
													(char*)gTextString,
													histogramSummaryPtr[summaryIndex].maxNonSatValue,
													ePrecision,
													fPrecision,
													FALSE);
													
				valueCountTextWidth = MAX (valueMinNonSatFieldSize, valueMaxNonSatFieldSize);
				
				}	// end "if (fileInfoPtr->numberBytes > 2 || ...)"
		
					// Get pointer to buffer to use to store count of data values to 	
					// be printed.  Make certain that there is a minimum of 128 bytes 	
					// and at least (length of count plus tab digits per column or
					// valueCountTextWidth per column.										
			
			NumToString (histogramSpecsPtr->totalPixels, gTextString);
			countTextWidth = gTextString[0];
			
					// Get width for channel number.
			
			NumToString ((SInt64)histogramSpecsPtr->numberChannels, gTextString);
			channelTextWidth = gTextString[0];
			
					// Get the size to allow for each column of data in the table.
					// And the maximum number of bytes needed for the line of data
			
			if (lineFormatHistFlag)
				{
						// Make sure the channelTextWidth is at least 7 characters to allow for
						// the 'Channel' heading.
				
				channelTextWidth = MAX (channelTextWidth, 7);
				
				value = histogramSpecsPtr->maxNumberBins - 2;
				
						// Make sure that the length of the min and max values are at least
						// as long as the rest of the values.
				
				minValueCountTextWidth = MAX (valueCountTextWidth, minValueCountTextWidth);
				maxValueCountTextWidth = MAX (valueCountTextWidth, maxValueCountTextWidth);
				
				minValueCountTextWidth = MAX (countTextWidth, minValueCountTextWidth);
				valueCountTextWidth = MAX (countTextWidth, valueCountTextWidth);
				maxValueCountTextWidth = MAX (countTextWidth, maxValueCountTextWidth);
			
				numberBytes = (SInt32)channelTextWidth+1 + minValueCountTextWidth+1 +
							maxValueCountTextWidth+1 + value * (valueCountTextWidth+1) + 128;
				
				}	// end "if (lineFormatHistFlag)"
				
			else	// !lineFormatHistFlag 
				{
				value = numberChannels;
				if (!oneTitleLinesForAllChannelsFlag)
					value = 1;
					
				countTextWidth = MAX (channelTextWidth, countTextWidth);
				
				valueCountTextWidth = MAX (minValueCountTextWidth, valueCountTextWidth);
				valueCountTextWidth = MAX (valueCountTextWidth, maxValueCountTextWidth);
				
						// Make sure the valueCountTextWidth is at least 5 characters to allow for the
						// 'Value' column heading.
				
				valueCountTextWidth = MAX (valueCountTextWidth, 5);
					
				numberBytes = (SInt32)valueCountTextWidth+1 +
																	value * (countTextWidth+1) + 128;
				
				}	// end "else !lineFormatHistFlag"
			
			if (numberBytes > (SInt32)numberBufferBytes)
				{
				gCharBufferHandle1 = UnlockAndDispose (gCharBufferHandle1);
				gCharBufferPtr1 = NULL;
				
				gCharBufferHandle1 = (Handle)MNewHandle (numberBytes);
				
				}	// end "if (numberBytes > numberBufferBytes)"
			
					// Exit routine if out of memory.											
					
			if (gCharBufferHandle1 == NULL)
																						return (TRUE);
																						
			numberBufferBytes = numberBytes;
			gCharBufferPtr1 = (HPtr)GetHandlePointer (gCharBufferHandle1, kLock);
			
					// List the Histogram title lines 1 and 2.								
			
			if (lineFormatHistFlag)
				stringIndex = IDS_HistogramTitle11;	// 15
				
			else	// !lineFormatHistFlag 
				stringIndex = IDS_HistogramTitle12;	// 16

			continueFlag = ListSpecifiedStringNumber (kHistogramStrID, 
																	stringIndex,
																	(UCharPtr)gCharBufferPtr1, 
																	resultsFileStreamPtr, 
																	gOutputCode, 
																	continueFlag);
		
					// List the Histogram title line 3.											
			
			if (lineFormatHistFlag)
				{
				charPtr = strcpy (gCharBufferPtr1, "           ");
				charPtr += 11;
				
				if (oneTitleLinesForAllChannelsFlag)
					{
					startValue = (SInt32)minValue;
					endValue = (SInt32)maxValue;
					
					}	// end "if (oneTitleLinesForAllChannelsFlag)"
					
				else	//!oneTitleLinesForAllChannelsFlag
					{
					if (gImageWindowInfoPtr->numberBytes > 2)
						{
						startValue = 1;
						endValue = histogramSpecsPtr->maxNumberBins - 2;
						
						}	// end "if (gImageWindowInfoPtr->numberBytes > 2)"
					
					else	// gImageWindowInfoPtr->numberBytes <= 2
						{
						startValue = GetBinIndexForDataValue (
										histogramSummaryPtr[summaryIndex].minNonSatValue,
										&histogramSummaryPtr[summaryIndex]);
										
						endValue = GetBinIndexForDataValue (
										histogramSummaryPtr[summaryIndex].maxNonSatValue,
										&histogramSummaryPtr[summaryIndex]);
										
						}	// end "else gImageWindowInfoPtr->numberBytes <= 2"
					
					}	// end "else !oneTitleLinesForAllChannelsFlag"
				
				}	// end "if (lineFormatHistFlag)" 
				
			else	// !lineFormatHistFlag 
				{
				textLength = MAX (9, 4+valueCountTextWidth);
				textLength = sprintf (gCharBufferPtr1,
												"%*s",
												(int)textLength,
												"Value");
				charPtr = gCharBufferPtr1 + textLength;
				
				if (oneTitleLinesForAllChannelsFlag)
					{
					startValue = 1;
					endValue = numberChannels;
					
					}	// end "if (oneTitleLinesForAllChannelsFlag)"
					
				else	//!oneTitleLinesForAllChannelsFlag
					{
					startValue = startChannelIndex + 1;
					endValue = lastChannelIndex + 1;
					
					}	// end "else !oneTitleLinesForAllChannelsFlag"
				
				}	// end "else !lineFormatHistFlag" 
		
						// List the values in the 2nd title line.								
			
			if (lineFormatHistFlag)
				{
						// List minimum saturated data value.
				
				if (oneTitleLinesForAllChannelsFlag)
					dataValue = -signedValueOffset;
				
				else	// !oneTitleLinesForAllChannelsFlag
					dataValue = histogramSummaryPtr[summaryIndex].minValue;
																						
				textLength = LoadRealValueString (charPtr,
																dataValue,
																minValueCountTextWidth,
																fPrecision,
																ePrecision,
																(char*)"\t",
																(char*)"");
												
				charPtr += textLength;
				
				}	// end "if (lineFormatHistFlag)" 
		
			for (index=startValue; index<=endValue; index++)
				{
				if (lineFormatHistFlag)
					{
					dataValue = GetDataValueForBinIndex (
																index,
																&histogramSummaryPtr[summaryIndex],
																signedValueOffset);
					
					textLength = LoadRealValueString (charPtr,
																	dataValue,
																	valueCountTextWidth,
																	fPrecision,
																	ePrecision,
																	(char*)"\t",
																	(char*)"");
													
					charPtr += textLength;
													
					}	// end "if (lineFormatHistFlag)"
				
				else	// !lineFormatHistFlag 
					{
					m = featurePtr[index-1] + 1;
					textLength = sprintf (charPtr, 
													"%c%*d",
													gTextTab,
													countTextWidth,
													(int)m);
					charPtr += textLength;
					
					}	// end "else !lineFormatHistFlag"
				
				}	// end "for (index=startValue; index<=...)" 
			
			if (lineFormatHistFlag)
				{
						// List maximum saturated data value.
				
				if (oneTitleLinesForAllChannelsFlag)
					dataValue = gImageWindowInfoPtr->numberBins - 1 - signedValueOffset;
				
				else	// !oneTitleLinesForAllChannelsFlag
					dataValue = histogramSummaryPtr[summaryIndex].maxValue;
																						
				textLength = LoadRealValueString (charPtr,
																dataValue,
																maxValueCountTextWidth,
																fPrecision,
																ePrecision,
																(char*)"\t",
																(char*)"");
												
				charPtr += textLength;
				
				}	// end "if (lineFormatHistFlag)" 
				
			*charPtr = gEndOfLine[0];
			charPtr++;
			if (gNumberOfEndOfLineCharacters == 2)
				{                     
				*charPtr = gEndOfLine[1];
				charPtr++;
				
				}	// end "if (gNumberOfEndOfLineCharacters == 2)"
				
			#ifndef multispec_lin
				textLength = (UInt32)(charPtr - gCharBufferPtr1);
			#else
            textLength = (UInt64)charPtr - (UInt64)gCharBufferPtr1;
			#endif
			continueFlag = OutputString (resultsFileStreamPtr, 
													gCharBufferPtr1, 
													textLength, 
													gOutputCode, 
													continueFlag);
			
			}	// end "if (!oneTitleLinesForAllChannelsFlag || channelIndex == 0)"
		
		if (continueFlag)
			{
					// Start with 4 spaces at beginning of the line.
					
			sprintf (gCharBufferPtr1, "    ");
				
			if (lineFormatHistFlag)
				{				
				if (gImageWindowInfoPtr->numberBytes <= 2)
					{
					forLineMinValue = histogramSpecsPtr->overallMinValue;
					forLineMaxValue = histogramSpecsPtr->overallMaxValue;
						
					}	// end "if (gImageWindowInfoPtr->numberBytes <= 2)"
					
				else	// gImageWindowInfoPtr->numberBytes > 2
					{
					forLineMinValue = histogramSummaryPtr[summaryIndex].minNonSatValue;
					forLineMaxValue = histogramSummaryPtr[summaryIndex].maxNonSatValue;
					
					}	// end "else gImageWindowInfoPtr->numberBytes > 2
				
				continueFlag = ListHistogramValuesInLines (histogramSpecsPtr,
																			resultsFileStreamPtr,
																			fileInfoPtr,
																			forLineMinValue, 
																			forLineMaxValue,
																			histogramSummaryPtr, 
																			histogramArrayPtr, 
																			startChannelIndex,
																			lastChannelIndex,
																			featurePtr,
																			channelTextWidth,
																			minValueCountTextWidth,
																			valueCountTextWidth,
																			maxValueCountTextWidth,
																			ePrecision,
																			fPrecision);
					
				}	// end ""
			
			else	// !lineFormatHistFlag 
				continueFlag = ListHistogramValuesInColumns (
																histogramSpecsPtr,
																resultsFileStreamPtr,
																fileInfoPtr,
																histogramSpecsPtr->overallMinValue, 
																histogramSpecsPtr->overallMaxValue, 
																includeEmptyBinsFlag,
																histogramSummaryPtr, 
																histogramArrayPtr, 
																startChannelIndex,
																lastChannelIndex,
																featurePtr,
																valueCountTextWidth,
																countTextWidth,
																ePrecision,
																fPrecision);
			
			}	// end "if (continueFlag)" 
			
		if (histogramSpecsPtr->allChannelsAtOnceFlag && oneTitleLinesForAllChannelsFlag)
			break;
			
		if (!oneTitleLinesForAllChannelsFlag)
			continueFlag = OutputString (resultsFileStreamPtr, 
													&gEndOfLine[0], 
													gNumberOfEndOfLineCharacters, 
													gOutputCode, 
													continueFlag);
													
		histogramArrayPtr += histogramSpecsPtr->maxNumberBins;
			
		}	// end "for (channelIndex=startChannelIndex; channelIndex<..."
			
	if (oneTitleLinesForAllChannelsFlag && lastChannelIndex == numberChannels-1)
		continueFlag = OutputString (resultsFileStreamPtr, 
												&gEndOfLine[0], 
												gNumberOfEndOfLineCharacters, 
												gOutputCode, 
												continueFlag);
	
			// Make certain that the character buffer memory has been released.	
			
	gCharBufferHandle1 = UnlockAndDispose (gCharBufferHandle1);
	gCharBufferPtr1 = NULL;
	
	SetPort (savedPort);
	
	return (continueFlag);
		
}	// end "ListHistogramValues" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHistogramValuesInColumns
//
//	Software purpose:	This routine lists the histogram values in a column format.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	TRUE if everything is okay,
//							FALSE if not.	
//
// Called By:			ListHistogramValues
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1993
//	Revised By:			Larry L. Biehl			Date: 08/04/2016	

Boolean ListHistogramValuesInColumns (
				HistogramSpecsPtr					histogramSpecsPtr,
				CMFileStream* 						resultsFileStreamPtr,
				FileInfoPtr							fileInfoPtr,
				double	 							minValue, 
				double	 							maxValue, 
				Boolean 								includeEmptyBinsFlag,
				HistogramSummaryPtr				histogramSummaryPtr,  
				HUInt32Ptr			 				medianArrayPtr, 
				UInt32								startChannelIndex,
				UInt32	 							lastChannelIndex,
				SInt16*								featurePtr,
				UInt16								valueCountTextWidth,
				UInt16								columnCountTextWidth,
				UInt16								ePrecision,
				UInt16								fPrecision)

{
			// Declare parameters																
	
	HPtr									charPtr;
	HUInt32Ptr							medianArrayChanPtr;
	
	double								dataValue;
	
	UInt32								channelIndex,
											m,
											maxBinIndex,
											maxValueIndex,
											minValueIndex,
											numberBins,
											summaryIndex;
	
	SInt32								countFlag,
											medianArrayIndex,
											signedValueOffset,
											textLength,
											value,
											valueAfter,
											valueBefore;
					
	Boolean								continueFlag;
	
			
	continueFlag = TRUE;
	maxBinIndex = fileInfoPtr->numberBins - 1;
	
			// Get the offset to use in case the data are signed values.  Note	
			// that this code will not handle more than one image file with		
			// mixed signed and unsigned data values correctly yet.					
			
	signedValueOffset = fileInfoPtr->signedValueOffset;
	
			// Get the summary index for the first channel to be listed.
			
	summaryIndex = featurePtr[startChannelIndex];
	
			// Get the minimum index and the maximum index for the min and max
			// values.
			
	minValueIndex = GetBinIndexForDataValue (minValue,
															&histogramSummaryPtr[summaryIndex]);
			
	maxValueIndex = GetBinIndexForDataValue (maxValue,
															&histogramSummaryPtr[summaryIndex]);
															
	numberBins = histogramSummaryPtr[summaryIndex].numberBins;
	
			// Note: make certain that the number of saturated data values get	
			// listed.																				

	m = 0; 
	while (m < numberBins) 
		{
		medianArrayIndex = 0;
		
		if (includeEmptyBinsFlag)
			countFlag = 1;
			
		else	// !includeEmptyBinsFlag 
			{
			countFlag = 0;
			for (channelIndex=startChannelIndex; 
					channelIndex<=lastChannelIndex; 
					channelIndex++)
				{
						// Set up channel pointer for median array						
					
				medianArrayChanPtr = &medianArrayPtr[medianArrayIndex];
				
						// Determine if there is a count for the bin before or 		
						// after the current bin.												
				
				if (m > 0)		 	
					valueBefore = medianArrayChanPtr[m-1];
				else	// m <= 0 
					valueBefore = 0;
					
				value = medianArrayChanPtr[m];
				
				if (m < maxBinIndex)
					valueAfter = medianArrayChanPtr[m+1];
				else	// m >= maxBinIndex 
					valueAfter = 0;
							
				if (value > 0)							
					countFlag++;
				
				else	// value <= 0 
					{
					if (valueBefore>0 || valueAfter>0)    
						countFlag++;
						
					}	// end "else value <= 0" 
					
				if (countFlag > 0)
					break;
					
				medianArrayIndex += histogramSpecsPtr->maxNumberBins;
				
				}	//	End "for (channelIndex=startChannelIndex; channelIndex<=..." 
			
			}	// end "else !includeEmptyBinsFlag" 
		
		if (countFlag > 0)
			{			
					// Get the data value that represents the center of the bin.
					// 'startChannelIndex' will be good for when all channels are 
					// being listed because this implies they have the same conversion.
			
			summaryIndex = featurePtr[startChannelIndex];
					
			dataValue = GetDataValueForBinIndex (m,
																&histogramSummaryPtr[summaryIndex],
																signedValueOffset);
				
			textLength = LoadRealValueString (gCharBufferPtr1,
															dataValue,
															valueCountTextWidth,
															fPrecision,
															ePrecision,
															(char*)"    ",
															(char*)"");
											
			charPtr = gCharBufferPtr1 + textLength;
			
			medianArrayIndex = 0;
			for (channelIndex=startChannelIndex;
						channelIndex<=lastChannelIndex;
						channelIndex++)
				{
						// Set up channel pointer for median array						
					
				medianArrayChanPtr = &medianArrayPtr [medianArrayIndex];
				textLength = sprintf (charPtr, 
												"%c%*d",
												gTextTab,
												columnCountTextWidth,
												(unsigned int)medianArrayChanPtr[m]);
				charPtr += textLength;
				
				medianArrayIndex += histogramSpecsPtr->maxNumberBins;
				
				}	// end "for (channelIndex=0; channelIndex<..." 
				
			//charPtr--;
			*charPtr = gEndOfLine[0];
			charPtr++;
			if (gNumberOfEndOfLineCharacters == 2)
				{                     
				*charPtr = gEndOfLine[1];
				charPtr++;
				
				}	// end "if (gNumberOfEndOfLineCharacters == 2)"
				
			#ifndef multispec_lin
				textLength = (UInt32)(charPtr - gCharBufferPtr1);
			#else
            textLength = (UInt64)charPtr - (UInt64)gCharBufferPtr1;
			#endif
			continueFlag = OutputString (resultsFileStreamPtr, 
													gCharBufferPtr1, 
													textLength, 
													gOutputCode, 
													continueFlag);
												
			if (!continueFlag)
				break;
				
			}	// End "if (countFlag > 0)" 
												
		if (!continueFlag)
			break;
		
				// Increment the bin index and allow for the saturated data values to be 
				// listed plus all those between the min and max values of the data.												
		
		m++;
		if (m < minValueIndex)
			m = minValueIndex;
			
		else	// m >= minValueIndex 
			{
			if (m > maxValueIndex && m < (numberBins - 1))
				m = numberBins - 1;
				
			}	// end "else m >= minValueIndex" 
	   	
		if (TickCount () >= gNextTime)
			{
	   			// This code checks to see if the user has requested 				
	   			// computation to stop; i.e. the user has entered command-.		
	   			// on the keyboard.	
   			
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				continueFlag = FALSE;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
				
			}	// end "if (TickCount () >= nextTime)"
			
		}	// end "while (m < numberBins)" 
		
	return (continueFlag);
		
}	// end "ListHistogramValuesInColumns" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListHistogramValuesInLines
//
//	Software purpose:	This routine lists the histogram values in a column
//							format.  It is recognized that m-1 and m+1 for min 
//							and max values equal to 0 and 255 or 4095 will access 
//							data outside the range of the structure.  It was 
//							decided that that was okay since the count for those 
//							values will be listed anyhow.  A check would only slow 
//							the system down.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	TRUE if everything is okay,
//							FALSE if not.	
//
// Called By:			ListHistogramValues
//
//	Coded By:			Larry L. Biehl			Date: 10/26/1993
//	Revised By:			Larry L. Biehl			Date: 02/28/2018

Boolean ListHistogramValuesInLines (
				HistogramSpecsPtr					histogramSpecsPtr,
				CMFileStream*						resultsFileStreamPtr,
				FileInfoPtr							fileInfoPtr,
				double								minValue, 
				double								maxValue, 
				HistogramSummaryPtr				histogramSummaryPtr, 
				HUInt32Ptr							medianArrayPtr, 
				UInt32								startChannelIndex,
				UInt32								lastChannelIndex,
				SInt16*								featurePtr,
				UInt16								channelTextWidth,
				UInt16								minValueCountTextWidth,
				UInt16								valueCountTextWidth,
				UInt16								maxValueCountTextWidth,
				UInt16								ePrecision,
				UInt16								fPrecision)

{
			// Declare parameters	
	
	HPtr									charPtr;
	HUInt32Ptr							medianArrayChanPtr;															
	
	SInt32								channelOffsetCount,
											textLength;
	
	UInt32								binIndex,
											channelIndex,
											maxValueIndex,
											minValueIndex;
					
	Boolean								continueFlag;
	
			
	channelOffsetCount = histogramSpecsPtr->maxNumberBins;
	continueFlag = TRUE;
	
	for (channelIndex=startChannelIndex; 
			channelIndex<=lastChannelIndex; 
			channelIndex++)
		{
				// Include the channel number.														

		charPtr = &gCharBufferPtr1[4];											
		textLength = sprintf (charPtr,
										"%*d%c", 
										channelTextWidth,
										featurePtr[channelIndex]+1,
										gTextTab);
		charPtr += textLength;											

				// Lower saturated value.														

		textLength = sprintf (charPtr,
										"%*d%c", 
										minValueCountTextWidth,
										(unsigned int)*medianArrayPtr,
										gTextTab);
		charPtr += textLength;
					
		if (gImageWindowInfoPtr->numberBytes > 2)
			{
			minValueIndex = 1;
			maxValueIndex = histogramSpecsPtr->maxNumberBins - 2;
			
			}	// end "if (gImageWindowInfoPtr->numberBytes > 2)"
			
		else	// gImageWindowInfoPtr->numberBytes <= 2
			{
			minValueIndex = GetBinIndexForDataValue (minValue,
									&histogramSummaryPtr[featurePtr[channelIndex]]);
			
			maxValueIndex = GetBinIndexForDataValue (maxValue,
									&histogramSummaryPtr[featurePtr[channelIndex]]);
			
			}	// end "else gImageWindowInfoPtr->numberBytes <= 2"
		
		medianArrayChanPtr = &medianArrayPtr [minValueIndex];
		for (binIndex=minValueIndex; binIndex<=maxValueIndex; binIndex++)
			{
			textLength = sprintf (charPtr,
											"%*d%c", 
											valueCountTextWidth,
											(unsigned int)*medianArrayChanPtr,
											gTextTab);
			charPtr += textLength;
			
			medianArrayChanPtr++;
			
			}	// End "for (binIndex=minValue;.." loop 
		
				// Upper saturated value.														

		textLength = sprintf (charPtr, 
										"%*d%c", 
										maxValueCountTextWidth,
										(unsigned int)medianArrayPtr[channelOffsetCount-1],
										gTextTab);
		charPtr += textLength;
		
		charPtr--;		
		*charPtr = gEndOfLine[0];
		charPtr++;
		if (gNumberOfEndOfLineCharacters == 2)
			{                     
			*charPtr = gEndOfLine[1];
			charPtr++;
				
			}	// end "if (gNumberOfEndOfLineCharacters == 2)"
		
				// Now get the total number of characters in the buffer.				
		#ifndef multispec_lin
			textLength = (UInt32)(charPtr - gCharBufferPtr1);
		#else
			textLength = (UInt64)charPtr - (UInt64)gCharBufferPtr1;
		#endif
		continueFlag = OutputString (resultsFileStreamPtr, 
												gCharBufferPtr1, 
												textLength, 
												gOutputCode, 
												continueFlag);
												
		if (!continueFlag)
			break;
				
		medianArrayPtr += channelOffsetCount;
				
		}	// End "for (channelIndex=startChannelIndex; channelIndex<..." 
		
	return (continueFlag);
		
}	// end "ListHistogramValuesInLines" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void LoadHistogramSpecs
//
//	Software purpose:	This routine loads the default values for the 
//							histogramSpecs structure. If user specifies the
//							variables for histogramSpecs, then default values are
//							not loaded.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			HistogramDialog
//
//	Coded By:			Ravi S. Budruk			Date: 06/12/1988
//	Revised By:			Ravi S. Budruk			Date: 08/09/1988	
//	Revised By:			Larry L. Biehl			Date: 02/16/2000	

void LoadHistogramSpecs (
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr)
	
{
	DisplaySpecsPtr					displaySpecsPtr = NULL;
	
	
	if (gProcessorCode == kHistogramProcessor || displaySpecsPtr == NULL)
		{
				// Define default values for HistogramSpecs from file information	
				// about image.  The histogram was called from the histogram		
				// processor and the structure hasn't been initialized yet			
			
		if (histogramSpecsPtr->firstTime)
			{
			SetHistogramLineColumnSpecs (histogramSpecsPtr,
													1, 
													gImageWindowInfoPtr->maxNumberLines, 
													1, 
													gImageWindowInfoPtr->maxNumberColumns,
													gImageWindowInfoPtr->totalNumberChannels);
			
			}	// end "if (histogramSpecsPtr->firstTime)" 
				
		else	// !histogramSpecsPtr->firstTime	
			{
			histogramSpecsPtr->lineEnd = MIN (
													histogramSpecsPtr->lineEnd,
													gImageWindowInfoPtr->maxNumberLines);
													
			histogramSpecsPtr->columnEnd = MIN (
													histogramSpecsPtr->columnEnd,
													gImageWindowInfoPtr->maxNumberColumns);
		
					// load drag gray rgn information into histogram 					
					// specification information if the selection rectangle has 	
					// been set up																	
				
			GetSelectedAreaInfo (
								gActiveImageWindow,
								gImageWindowInfoPtr,
								(SInt32*)&histogramSpecsPtr->lineStart,
								(SInt32*)&histogramSpecsPtr->lineEnd,
								(SInt32*)&histogramSpecsPtr->columnStart,
								(SInt32*)&histogramSpecsPtr->columnEnd,
								kDontClearSelectionArea,
								kUseThreshold,
								kDontAdjustToBaseImage);
										
			}	// end "else  !histogramSpecsPtr->firstTime"	
			
		}	// end "if (gProcessorCode == kHistogramProcessor)" 
		
	else	// gProcessorCode != kHistogramProcessor 
		{
				// Define default values for HistogramSpecs from image display		
				// specs.  The histogram was called from the image display			
				// processor.																		
				// Get handle to display specification	
				
		displaySpecsPtr = GetActiveDisplaySpecsPtr ();
		
		SetHistogramLineColumnSpecs (histogramSpecsPtr,
												displaySpecsPtr->lineStart, 
												displaySpecsPtr->lineEnd, 
												displaySpecsPtr->columnStart, 
												histogramSpecsPtr->columnEnd,
												gImageWindowInfoPtr->totalNumberChannels);
		
		}	// end "else gProcessorCode != kHistogramProcessor" 
		
			// Indicate that histogram specification have been load					
			//  the firsttime.																	

	histogramSpecsPtr->firstTime = FALSE;

}	// end "LoadHistogramSpecs"
 

	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSTASupportFile
//
//	Software purpose:	This routine loads information from the requested image
//							statistics support file into memory. The format of the
//							support file is based on the ERDAS 73 and 74 .sta formats.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadSupportFile in SHistogram.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/01/1988
//	Revised By:			Larry L. Biehl			Date: 08/03/2016

Boolean LoadSTASupportFile (
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr,
				HUInt32Ptr							histogramArrayPtr, 
				UInt32								channelIndex, 
				Boolean								summaryFlag)

{
	HCharPtr								statBufferPtr;
	HistogramSummaryPtr				histogramSummaryPtr;
	
	CMFileStream*						supportFileStreamPtr; 
	
	HUInt32Ptr							localHistoArrayPtr;
	
	UInt16								*channelListPtr,
											*lChannelListPtr;
	
	UInt32								channel,
											channelOffsetCount,
											numberChannels,
											numberChannelLoops;
	
	Boolean								continueFlag = TRUE,
											readAllChannelsAtOneTimeFlag;
	
	
			// The channels handle is already locked.
																								
	channelListPtr = (UInt16*)GetHandlePointer (histogramSpecsPtr->channelsHandle);
	
	if (channelIndex == 0)
		{
				// Get buffer to load statistics into.  Try to get buffer to read	
				// all of the histogram data.  If space is not available, then 	
				// get space for one channel.													
		
		if (histogramSpecsPtr->statBufferPtr == NULL)
			{
			histogramSpecsPtr->numberStatBufferChannels = 
																histogramSpecsPtr->numberChannels;
			if (!histogramSpecsPtr->allChannelsAtOnceFlag)
				histogramSpecsPtr->numberStatBufferChannels = 1;
			
			histogramSpecsPtr->statBufferPtr = GetSTASupportFileBuffer (
													channelListPtr,
													&histogramSpecsPtr->numberStatBufferChannels);
			
			}	// end "if (...statBufferPtr == NULL)" 
		
				// Open the sta statistics support file.											
		
		if (histogramSpecsPtr->statBufferPtr != NULL)
			gImageWindowInfoPtr->supportFileStreamPtr = OpenSupportFile ();
	  	
	  			// Force the total number of pixels in the histogram to be loaded.
	  			
	  	histogramSpecsPtr->totalPixels = 0;
						
		}	// end "if (channelIndex == 0)" 

			// Continue if everything is okay.												

	supportFileStreamPtr = gImageWindowInfoPtr->supportFileStreamPtr;
	if (supportFileStreamPtr != NULL)
		{
	  	statBufferPtr = histogramSpecsPtr->statBufferPtr;
	  	
				// Make cursor the watch cursor to indicate to the user that work	
				// is in process.																	

		MSetCursor (kWait);
		
				// Put appropriate message in the status box								
				
		LoadDItemStringNumber (kHistogramStrID, 
										IDS_HistogramStatus4, 
										gStatusDialogPtr, 
										IDC_Status2, 
										(Str255*)gTextString);

		channelOffsetCount = 0;
		readAllChannelsAtOneTimeFlag = FALSE;
		
		if (histogramSpecsPtr->allChannelsAtOnceFlag)
			{
			numberChannels = 1;
			numberChannelLoops = histogramSpecsPtr->numberChannels;
			readAllChannelsAtOneTimeFlag = FALSE;
			
			if (histogramSpecsPtr->numberStatBufferChannels == 
									histogramSpecsPtr->numberChannels)
				{
				numberChannels = histogramSpecsPtr->numberChannels;
				numberChannelLoops = 1;
				readAllChannelsAtOneTimeFlag = TRUE;
				
				}	// end "if (...gImageWindowInfoPtr->totalNumberChannels)" 
			
			}	// end "...histogramSpecs.allChannelsAtOnceFlag" 
			
		else	// !...->histogramSpecs.allChannelsAtOnceFlag 
			{	
			numberChannels = 1;
			numberChannelLoops = channelIndex + 1;
			readAllChannelsAtOneTimeFlag = FALSE;
			
			}	// end "else !...->histogramSpecs.allChannelsAtOnceFlag" 
			
		histogramSummaryPtr = histogramSpecsPtr->histogramSummaryPtr;
			
		for (channel=channelIndex; 
				channel<numberChannelLoops; 
				channel++)
			{
			lChannelListPtr = &channelListPtr[channel];
			
			continueFlag = ReadSTASupportFile (statBufferPtr, 
															supportFileStreamPtr,
															lChannelListPtr, 
															(SInt16)numberChannels, 
															readAllChannelsAtOneTimeFlag);
			
			localHistoArrayPtr = histogramArrayPtr;
			if (histogramArrayPtr)
				localHistoArrayPtr = &histogramArrayPtr[channelOffsetCount];
			
			if (continueFlag)
				continueFlag = DecodeSTASupportFile (histogramSpecsPtr,
																	localHistoArrayPtr, 
																	supportFileStreamPtr,
																	lChannelListPtr, 
																	(SInt16)numberChannels,
																	summaryFlag);
			
			if (!continueFlag)
				break;
				
			channelOffsetCount += histogramSpecsPtr->maxNumberBins;
				
			}	// end "for (channel=0; ..." 
			
		if (channelIndex == 0)
			{
					// Find the total number of pixels. We will just count 			
					// those for the first channel.											 
	
			if (histogramSpecsPtr->totalPixels <= 0)
				histogramSpecsPtr->totalPixels = CountTotalNumberHistogramPixels (
																histogramArrayPtr,
																histogramSummaryPtr[0].numberBins);
		
					// Set number bad lines to -1 since that information is 			
					// not stored in the image statistics file.							
			
			histogramSpecsPtr->numBadLines = -1;
			
			}	// end "if (channelIndex == 0)" 
			
		}	// end "if (supportFileStreamPtr != NULL)
		
	else	// supportFileStreamPtr == NULL
		continueFlag = FALSE;
		
	return (continueFlag);
	
}	// end "LoadSTASupportFile"

	
	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadSupportFile
//
//	Software purpose:	This routine loads information from the requested image
//							statistics support file into memory. The format of the
//							support file is based on the ERDAS 73 and 74 .sta formats.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			DoHistogramRequests in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 10/18/1999
//	Revised By:			Larry L. Biehl			Date: 10/18/1999

Boolean LoadSupportFile (
				WindowInfoPtr						windowInfoPtr,
				FileInfoPtr							fileInfoPtr, 
				HistogramSpecsPtr					histogramSpecsPtr,
				HUInt32Ptr							histogramArrayPtr, 
				UInt32								channelIndex, 
				Boolean								summaryFlag)

{
	Boolean								continueFlag = TRUE;
	
																	
	if (windowInfoPtr->numberImageFiles == 1 && 
										histogramSpecsPtr->statFileFormat == kImagineType)
		continueFlag = LoadImagineImageStatistics (fileInfoPtr, 
																	histogramSpecsPtr,
																	histogramArrayPtr,
																	(SInt16)channelIndex,
																	summaryFlag);
	
	else	// numberImageFiles != 1 || format != kImagineType
		continueFlag = LoadSTASupportFile (fileInfoPtr, 
														histogramSpecsPtr,
														histogramArrayPtr,
														(SInt16)channelIndex,
														summaryFlag);
		
	return (continueFlag);
	
}	// end "LoadSupportFile"

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMFileStream* OpenSupportFile
//
//	Software purpose:	This routine opens the ERDAS statistics file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadSTASupportFile in histogram.c
//							SetUpToReadHistogramArray in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 07/19/1990
//	Revised By:			Larry L. Biehl			Date: 03/12/2008

CMFileStream* OpenSupportFile (void)

{
	CMFileStream*						supportFileStreamPtr = NULL;
	
	SInt16								errCode = -1;
	
	SignedByte							handleStatus;
	

	supportFileStreamPtr = GetActiveSupportFileStreamPointer (&handleStatus);
	
	if (FileExists (supportFileStreamPtr))
		errCode = OpenFileReadOnly (supportFileStreamPtr, 
												kDontResolveAliasChains, 
												kNoLockFile,
												kDoNotVerifyFileStream);
			
	if (errCode != noErr)
		supportFileStreamPtr = NULL;
		
	return (supportFileStreamPtr);
		
}	// end "OpenSupportFile"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ReadSTASupportFile
//
//	Software purpose:	This routine reads the data for the requested
//							channels from the ERDAS statistics (.sta) file.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			EqualAreaDataToDisplayLevels in displayMultispectral.c
//							GetClippedMinMaxValues in displayMultispectral.c
//							LoadSTASupportFile in histogram.c
//
//	Coded By:			Larry L. Biehl			Date: 12/11/1991
//	Revised By:			Larry L. Biehl			Date: 10/01/1993

Boolean ReadSTASupportFile (
				CharPtr								statBufferPtr, 
				CMFileStream*						supportFileStreamPtr, 
				UInt16*								channelListPtr, 
				SInt16								numberChannels, 
				Boolean								readAllAtOneTimeFlag)

{
	UInt32								count,
											numberBytesPerChannel,
											totalNumberBytes;
	
	SInt16								channelIndex,
											errCode,
											index,
											numberLoops;
	
	
	if (channelListPtr == NULL ||
					supportFileStreamPtr == NULL ||
								statBufferPtr == NULL)
																					return (FALSE);
	
			// Initialize local variables.													
		
	totalNumberBytes = numberBytesPerChannel = (UInt32)9 * 128;
	numberLoops = numberChannels;
	
			// Get number of bytes to be read.												
																						
	if (readAllAtOneTimeFlag)
		{
		totalNumberBytes *= numberChannels;
		numberLoops = 1;
		
		}	// end "if (readAllAtOneTimeFlag)" 
		
	for (index=0; index<numberLoops; index++)
		{
		channelIndex = channelListPtr[index];
			
				// Set start point to read data from the file.							
	  	
		count = channelIndex * numberBytesPerChannel;
		errCode = MSetMarker (
							supportFileStreamPtr, fsFromStart, count, kErrorMessages);
		if (errCode != noErr)													
																					return (FALSE);
		
		count = totalNumberBytes;
		errCode = MReadData (
							supportFileStreamPtr, &count, statBufferPtr, kErrorMessages);
		if (errCode != noErr)																
																					return (FALSE);
																					
		statBufferPtr += numberBytesPerChannel;
				
		channelIndex++;
		
		}	// end "for (index=0; index<numberLoops; index++)" 
																					
	return (TRUE);
																					
}	// end ReadSTASupportFile 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseHistogramSpecsPtr
//
//	Software purpose:	The purpose of this routine is to unlock the
//							histogram specs handlee and set the static pointer
//							to the block of memory to NULL.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:	11/07/1995
//	Revised By:			Larry L. Biehl			Date: 11/07/1995

void ReleaseHistogramSpecsPtr (
				WindowInfoPtr						windowInfoPtr)

{
	if (windowInfoPtr != NULL)
		{
		#if defined multispec_mac
			HistogramSpecsPtr histogramSpecsPtr = 
							(HistogramSpecsPtr)GetHandlePointer (
															windowInfoPtr->histogramSpecsHandle);
		
			if (histogramSpecsPtr != NULL)
				{									
				histogramSpecsPtr->histogramSummaryPtr = NULL;
				CheckAndUnlockHandle (histogramSpecsPtr->channelsHandle);
				
				}	// end "if (histogramSpecsPtr != NULL)"
											
			CheckAndUnlockHandle (windowInfoPtr->histogramSpecsHandle);
			CheckAndUnlockHandle (windowInfoPtr->histogramSummaryHandle);	
		#endif	// defined multispec_mac 	

		#if defined multispec_win	
			if (gActiveImageViewCPtr != NULL)
				gActiveImageViewCPtr->m_histogramCPtr->ReleaseHistogramSpecsPtr ();
		#endif	// defined multispec_win 
			
		}	// end "if (windowInfoPtr != NULL)"

}	// end "ReleaseHistogramSpecsPtr" 
 

	
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetHistogramLineColumnSpecs
//
//	Software purpose:	This routine loads histogram line, column and interval default
//							settings.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None	
//
// Called By:			LoadHistogramSpecs in SHistgrm.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/16/2000
//	Revised By:			Larry L. Biehl			Date: 10/18/2017

void SetHistogramLineColumnSpecs (
				HistogramSpecsPtr					histogramSpecsPtr,
				SInt32								lineStart, 
				SInt32								lineEnd, 
				SInt32								columnStart, 
				SInt32								columnEnd,
				SInt32								numberChannels)

{
		
	UInt32								columnInterval,
											lineInterval,
											maxNumberChannelPixels;
											

			// Use a line interval such that no more than 6 million pixels
			// (lines x columns x number channels) will be used. Do not
			// allow less than 20,000 pixels per channel.
			
	maxNumberChannelPixels = MAX (6000000, numberChannels * 20000);
			
	histogramSpecsPtr->lineStart = lineStart;
	histogramSpecsPtr->lineEnd	= lineEnd;
	
	lineInterval = (UInt32)((double)(lineEnd - lineStart + 1) *
			(columnEnd - columnStart + 1) * numberChannels / 
																	maxNumberChannelPixels + .5);
	lineInterval = MAX (1, lineInterval);
	lineInterval = MIN (100, lineInterval);
	histogramSpecsPtr->lineInterval = lineInterval;
	
	histogramSpecsPtr->columnStart = columnStart;
	histogramSpecsPtr->columnEnd = columnEnd;
	
	columnInterval = (UInt32)((double)(lineEnd - lineStart + 1) *
			(columnEnd - columnStart + 1) * numberChannels / lineInterval /
																	maxNumberChannelPixels + .5);
	
	histogramSpecsPtr->columnInterval = MAX (1, columnInterval);

}	// end "SetHistogramLineColumnSpecs"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetNumberOfMaximumDataValuesInArray
//
//	Software purpose:	This purpose of this routine is to set the number of maximum
//							data values in the correct location within the histogram array.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	Count of the number of maximum data values	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2006
//	Revised By:			Larry L. Biehl			Date: 03/25/2006

void SetNumberOfMaximumDataValuesInArray (
				HistogramSummaryPtr				histogramSummaryPtr,
				HUInt32Ptr							histogramArrayPtr,
				SInt32								signedValueOffset)

{
	SInt32								maximumBinIndex;
	
	UInt32								numberMaximumDataValues;
	
	
	if (histogramSummaryPtr->binType == kDataValueIsBinIndex &&
														histogramSummaryPtr->numberBins > 256)
		{
				// The number of maximum data values for this case will be located
				// at the position in the array identified by the maximum data value.
				// This is for 16-bit integer data.
				
		numberMaximumDataValues = histogramArrayPtr[histogramSummaryPtr->numberBins-1];
		histogramArrayPtr[histogramSummaryPtr->numberBins-1] = 0;
		
		maximumBinIndex = (SInt32)(histogramSummaryPtr->maxValue + signedValueOffset);
		maximumBinIndex = MAX (0, maximumBinIndex);
		maximumBinIndex = MIN (maximumBinIndex, (SInt32)histogramSummaryPtr->numberBins-1);
		
		histogramArrayPtr[maximumBinIndex] = numberMaximumDataValues;
		
		}	// end "if (histogramSummaryPtr->binType == kDataValueIsBinIndex && ..."

}	// end "SetNumberOfMaximumDataValuesInArray" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetNumberOfMinimDataValuesInArray
//
//	Software purpose:	This purpose of this routine is to set the number of maximum
//							data values in the correct location within the histogram array.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	Count of the number of maximum data values	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/25/2006
//	Revised By:			Larry L. Biehl			Date: 03/25/2006

void SetNumberOfMinimumDataValuesInArray (
				HistogramSummaryPtr				histogramSummaryPtr,
				HUInt32Ptr							histogramArrayPtr,
				SInt32								signedValueOffset)

{
	SInt32								minimumBinIndex;
	
	UInt32								numberMinimumDataValues;
	
	
	if (histogramSummaryPtr->binType == kDataValueIsBinIndex &&
														histogramSummaryPtr->numberBins > 256)
		{
				// The number of maximum data values for this case will be located
				// at the position in the array identified by the maximum data value.
				// This is for 16-bit integer data.
				
		numberMinimumDataValues = histogramArrayPtr[0];
		histogramArrayPtr[0] = 0;
		
		minimumBinIndex = (SInt32)(histogramSummaryPtr->minValue + signedValueOffset);
		minimumBinIndex = MAX (0, minimumBinIndex);
		minimumBinIndex = MIN (minimumBinIndex, (SInt32)histogramSummaryPtr->numberBins-1);
		
		histogramArrayPtr[minimumBinIndex] = numberMinimumDataValues;
		
		}	// end "if (histogramSummaryPtr->binType == kDataValueIsBinIndex && ..."

}	// end "SetNumberOfMinimumDataValuesInArray" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetUpHistogramSpecsPtr
//
//	Software purpose:	The purpose of this routine is to move the
//							histogram specs handle block high in memory,
//							lock the handle and set the static pointer
//							to the block of memory.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:
//
// Called By:			DisplayMultispectralImage in SDisMulc.cpp
//							HistogramControl in SHistgrm.cpp
//
//	Coded By:			Larry L. Biehl			Date:	04/26/1995
//	Revised By:			Larry L. Biehl			Date: 06/04/1996

HistogramSpecsPtr SetUpHistogramSpecsPtr (
				Boolean								setUpHistogramArrayFlag,
				SignedByte*							histogramStatusPtr)

{
	Handle								*histogramSpecsHandlePtr,
											*histogramSummaryHandlePtr;
											
	HistogramSpecsPtr					histogramSpecsPtr;
						
	Boolean								continueFlag = FALSE;
	
	
	if (gImageWindowInfoPtr == NULL)
																						return (NULL);
	
	
	histogramSpecsHandlePtr = GetHistogramSpecsHandlePtr (gImageWindowInfoPtr);
	
	*histogramSpecsHandlePtr = InitializeHistogramInfoStructure (
																			*histogramSpecsHandlePtr,
																			kNotPointer,
																			setUpHistogramArrayFlag);
	
	histogramSpecsPtr = (HistogramSpecsPtr)GetHandleStatusAndPointer (
																			*histogramSpecsHandlePtr,
																			histogramStatusPtr,
																			kMoveHi);
	
	if (histogramSpecsPtr != NULL)
		{
		histogramSummaryHandlePtr = GetHistogramSummaryHandlePtr (gImageWindowInfoPtr);
		
		histogramSpecsPtr->histogramSummaryPtr = (HistogramSummaryPtr)
						GetHandlePointer (*histogramSummaryHandlePtr, kLock, kMoveHi);
						
		continueFlag = (histogramSpecsPtr->histogramSummaryPtr != NULL);
						
		}	// end "if (histogramSpecsPtr != NULL)"
	
	if (continueFlag)
		{
		SInt16* channelsPtr = (SInt16*)GetHandlePointer (
																	histogramSpecsPtr->channelsHandle,
																	kLock, 
																	kMoveHi);
		
		continueFlag = (channelsPtr != NULL);
										
		}	// end "if (continueFlag)"
		
   #if defined multispec_win	| defined multispec_lin
		if (continueFlag && gActiveImageViewCPtr != NULL)
			gActiveImageViewCPtr->m_histogramCPtr->SetUpHistogramSpecsPtr ();
			
		else	// !continueFlag || gActiveImageViewCPtr == NULL
			continueFlag = FALSE;
	#endif	// defined multispec_win
		
	if (!continueFlag)
		histogramSpecsPtr = NULL;
	
	return (histogramSpecsPtr); 
	
}	// end "SetUpHistogramSpecsPtr"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetUpToReadHistogramArray
//
//	Software purpose:	The purpose of this routine is to obtain the memory
//							to store the histogram array for one channel and the buffer
//							to use to load the statistics information for one channel from 
//							disk. Also the statistics disk file is opened.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:
//
// Called By:			HistogramVector in displayMultiSpectral.c
//							InitializeHistogramInfoStructure in histogram.c
//
//	Coded By:			Larry L. Biehl			Date:	10/01/1993
//	Revised By:			Larry L. Biehl			Date: 06/12/2003

Boolean SetUpToReadHistogramArray (
				HistogramSpecsPtr					histogramSpecsPtr)

{
	Boolean								continueFlag;
	
	
			// Check input variables.															
			
	if (histogramSpecsPtr == NULL)
																					return (FALSE);
	
			// Get to pointer to histogram data values.									
	
	if (histogramSpecsPtr->histogramArrayPtr == NULL)
		histogramSpecsPtr->histogramArrayPtr = GetHistogramValuesMemory (
																histogramSpecsPtr->maxNumberBins,
																1, 
																&histogramSpecsPtr->histogramArrayH);
	
	continueFlag = (histogramSpecsPtr->histogramArrayPtr != NULL);
	
			// Get buffer to load statistics into.  Try to get buffer to hold		
			// statistics information for one channel.								
	
	if (continueFlag)
		continueFlag = GetStatFileBuffer (gImageFileInfoPtr,
														histogramSpecsPtr,
														NULL, 
														1);
	
			// Open the sta support statistics file.												
	
	if (continueFlag && histogramSpecsPtr->statFileFormat != kImagineType)
		{	
		gImageWindowInfoPtr->supportFileStreamPtr = OpenSupportFile ();
		continueFlag = (gImageWindowInfoPtr->supportFileStreamPtr != NULL);
												
		}	// end "if (continueFlag && ..." 
												
	return (continueFlag);
		
}	// end "SetUpToReadHistogramArray"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateSupportFileTypeSetting
//
//	Software purpose:	The purpose of this routine is to update the support file type
//							setting. It may have changed depending on the selection that the
//							user made for the image histogram statistic file.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	07/08/2009
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void UpdateSupportFileTypeSetting (
				WindowInfoPtr						windowInfoPtr,
				HistogramSpecsPtr					histogramSpecsPtr)

{ 
	FileStringPtr						supportFilePathPtr;
	
			
	if (FileExists (windowInfoPtr->supportFileStreamPtr))
		{
		supportFilePathPtr = (FileStringPtr)GetFilePathPPointerFromFileStream (
																windowInfoPtr->supportFileStreamPtr);
		if (CompareSuffixNoCase ((char*)"\0.sta", supportFilePathPtr, NULL))
			histogramSpecsPtr->statFileFormat = kErdas74Type;
			
		else if (CompareSuffixNoCase ((char*)"\0.img", supportFilePathPtr, NULL))
			histogramSpecsPtr->statFileFormat = kImagineType;
		
		}	// end "if (FileExists (windowInfoPtr->supportFileStreamPtr))"
		
}	// end "UpdateSupportFileTypeSetting"
