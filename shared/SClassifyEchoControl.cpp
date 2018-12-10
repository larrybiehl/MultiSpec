//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SClassifyEchoControl.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that controls echo classification
//								processes.
//
//	Functions in file:	void 							EchoClsfierControl
//								SInt16 						GetLinesOfData
//								Boolean 						interface
//								Boolean 						SetupClsfierStats
//								SInt16 						PostEchoClassifier
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------


#include	"SMultiSpec.h" 
  
#if defined multispec_win
	#include "WImageView.h"
#endif	// defined multispec_win 
  
#if defined multispec_lin
	#include "LImageView.h"
#endif	// defined multispec_lin    

//#include "SExtGlob.h"
#include "SEcho.h"


// External variables defined in this file 

common_classifier_informationPtr		gCommon_InfoVariablePtr;
// char											*gError_message;

		// Function Prototypes

Boolean			Interface (
						FileInfoPtr 						fileInfoPtr);
				
Boolean			SetupClsfierStats (void);
						

					
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EchoClsfierControl
//
// Software purpose:	Control Echo classification process
//
//	Parameters in:
//
// Parameters out:
//
// Value Returned:	
// 
//	Called by: 			ClassifyControl in classify.c
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/1989
//	Revised By:			Byeungwoo Jeon			Date: 06/20/1991	

void EchoClsfierControl (
				FileInfoPtr							fileInfoPtr)
				

{
  	ClassifierVar						classifierVar;
	CMFileStream*						resultsFileStreamPtr;
   										
   UInt32								bytes;
	 
	SInt16								cellDegreesOfFreedomEcho,
   										errorCode;
   
	Boolean								continueFlag;
	
	
			// If spare memory had to be used to load code resources, then exit
			// routine.																			
			
	if (gMemoryTypeNeeded < 0)
																							return;
		    		
			// Code resources loaded okay, so set flag back for non-Code
			// resources.																
			
	gMemoryTypeNeeded = 0;
	
			// Put description in the status dialog.
				
	LoadDItemStringNumber (kClassifyStrID,		// \pSetting up Statistics in ECHO"
										IDS_Classify46,
										gStatusDialogPtr,
										IDC_Status11,
										(Str255*)gTextString);

	InitializeClassifierVarStructure (&classifierVar);
   
			// Make certain that the default volume is the same as for the
			// output results file.													
		
	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
	if (resultsFileStreamPtr == NULL)
		resultsFileStreamPtr = 	GetResultsFileStreamPtr (1);
		
	if (resultsFileStreamPtr != NULL)
		{
		errorCode = SetVolume (resultsFileStreamPtr, kErrorMessages);
   	if (errorCode != noErr)
   																							return;
   																							
   	}	// end "if (resultsFileStreamPtr != NULL)"
   
   		// Lock the echoClassifierVar handle.									
		
	gEchoClassifierVariablePtr = (EchoClassifierVarPtr)GetHandlePointer (
							gClassifySpecsPtr->echoClassifierVarH, kLock, kMoveHi);
							
	if (gEchoClassifierVariablePtr == NULL)
																								return;	

   bytes = (UInt32)sizeof (common_classifier_information);
   gCommon_InfoVariablePtr = (common_classifier_information *)MNewPointer (bytes);
 	continueFlag = (gCommon_InfoVariablePtr != NULL);
 	
   if (!continueFlag)
   																							return;
   
   initialize_common_info (gCommon_InfoVariablePtr);

	if (continueFlag)
  		{
	  
				// Set up some of variables in gCommon_InfoVariablePtr
	  
		continueFlag = Interface (fileInfoPtr);

				// Set up some of variables in gCommon_InfoVariablePtr->parameter_info
	  
   	gCommon_InfoVariablePtr->parameter_info.icelwth = 
											gEchoClassifierVariablePtr->cell_width;
   	
   	gCommon_InfoVariablePtr->parameter_info.thd1 = 
								gEchoClassifierVariablePtr->homogeneityThreshold;
    	
   	gCommon_InfoVariablePtr->parameter_info.thd2 = 
								gEchoClassifierVariablePtr->annexationThreshold;
    	
   	gCommon_InfoVariablePtr->parameter_info.fixed_threshold_option = 
							gEchoClassifierVariablePtr->fixed_homogeneityThreshold_option;

		}	//	"if (continueFlag)"

			// Set up class_stat
  			// Assign memories for statistics and prepare reduced set of
  			// statistics

	if (continueFlag)
	   continueFlag = SetupClsfierStats ();

			// setup_basic_echo_information
			// Memory Allocation to "threshold_phase1" and set up ECHO information
			// Echo_information related to the "image selection" is set up later 
			// at echospec.c

   if (continueFlag)
   	{
	   continueFlag = setup_basic_echo_information (gCommon_InfoVariablePtr,
																	gEchoClassifierVariablePtr);

   	}	// end "if (continueFlag)"

			// setup_basic_echo_information
			//	If thresholding is to be used, get table of threshold values corresponding to
			// fractions rejected from 0.1 to 1.0 percent by 0.1 percent increments and 
			// from 1.0 to 99 percent by 1.0 percent increments.

	if (continueFlag && gClassifySpecsPtr->createThresholdTableFlag)
		{
		cellDegreesOfFreedomEcho = gEchoClassifierVariablePtr->cell_width;	
		cellDegreesOfFreedomEcho *= cellDegreesOfFreedomEcho;
		cellDegreesOfFreedomEcho *= gEchoClassifierVariablePtr->nfeature;
		
		continueFlag = CreateThresholdTables (	gEchoClassifierVariablePtr->nfeature, 
															cellDegreesOfFreedomEcho);
				
		}	// end "if (...->createThresholdTable)"

			// Perform Echo Classification
  
  if (continueFlag)
  		{
  				// Clear subtitle strings and show them.
  				
  		LoadDItemString (gStatusDialogPtr, IDC_Status21, (Str255*)"\0");
  		LoadDItemString (gStatusDialogPtr, IDC_Status22, (Str255*)"\0");
		ShowStatusDialogItemSet (kStatusTitle2);
 
      ClassifyAreasControl (fileInfoPtr, &classifierVar);
      
      }	// end "if (continueFlag)"		
   	   
			// Release Memories assigned at this routine
			// setup_basic_echo_information ()	: gEchoClassifierVariablePtr->thresholds_phase1 
			// SetupClsfierStats ()					: gCommon_InfoVariablePtr->class_stat
			// EchoLikeClsfierControl ()			: gCommon_InfoVariablePtr
  	
	gEchoClassifierVariablePtr->thresholds_phase1 = 
					CheckAndDisposePtr (gEchoClassifierVariablePtr->thresholds_phase1);
					
	free_class_stat (&(gCommon_InfoVariablePtr->stat_info),
								&(gCommon_InfoVariablePtr->class_stat));
								
   gCommon_InfoVariablePtr = (common_classifier_informationPtr)
   									CheckAndDisposePtr ((char*)gCommon_InfoVariablePtr);
   
   gEchoClassifierVariablePtr = NULL;
   
}	// end "EchoLikeClsfierControl"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLinesOfData
//
//	Software purpose:	The purpose of this routine is to obtain the requested
//							lines and channels of data.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1992
//	Revised By:			Larry L. Biehl			Date: 12/29/2005	

SInt16 GetLinesOfData (
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
	HUCharPtr							outputBufferPtr;
	
	UInt32								bytesLoaded,
											line;
									
	SInt16								errCode;
	
	
	outputBufferPtr = fileIOInstructionsPtr->outputBufferPtrs[0];
	bytesLoaded = fileIOInstructionsPtr->numberColumns *
												fileIOInstructionsPtr->numberChannels * 8;
	
	for (line=fileIOInstructionsPtr->lineStart;
			line<=(UInt32)fileIOInstructionsPtr->lineEnd;
			line+=fileIOInstructionsPtr->lineInterval)
		{
				// Get all requested channels for line of image data.  Return
				// if there is a file IO error.										
					 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line,
											fileIOInstructionsPtr->columnStart,
											fileIOInstructionsPtr->columnEnd,
											fileIOInstructionsPtr->columnInterval,
											fileIOInstructionsPtr->inputBufferPtrs[0],
											outputBufferPtr);
							
		if (errCode)
																				return (errCode);
																				
		outputBufferPtr += bytesLoaded;
							
		}	// end "for (line=fileIOInstructionsPtr->lineStart ..."
		
	fileIOInstructionsPtr->nextLineToRead = line;
		
	return (errCode);
			
}	// end "GetLinesOfData"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean Interface
//
// Software purpose:	Copy information from structures defined in multispec.h
//							to "gCommon_InfoVariable" defined in mytype.h
//
//	Parameters in:
//
// Parameters out:
//
// Value Returned:	Error_code (0 : no error)
// 
//	Called by: 			EchoClsfierControl in	echo_classifiy.c
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/1989
//	Revised By:			Byeungwoo Jeon			Date: 11/01/1991

Boolean Interface (
				FileInfoPtr							fileInfoPtr)
				
{
	pixel_information* 				pixel_info_ptr;
	stat_information* 				stat_info_ptr;
	
	SInt16								returnCode;
	
	Boolean								continueFlag = TRUE;
		

			// Set up some local variables 

	pixel_info_ptr = &(gCommon_InfoVariablePtr->pixel_info);
	stat_info_ptr  = &(gCommon_InfoVariablePtr->stat_info);
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr); 
 	returnCode = SetVolume (fileStreamPtr, kErrorMessages);
	
   if (returnCode == 0) 
   	{  
				// Copy appropriate information from the structure of
				// "fileInfoPtr" to that of "pixel_info_ptr".
	
	   pixel_info_ptr->nrow 	 		= fileInfoPtr->numberLines;
	   pixel_info_ptr->ncol 	 		= fileInfoPtr->numberColumns;
	   pixel_info_ptr->nband 		   = gClassifySpecsPtr->numberChannels;
	  	stat_info_ptr->ncls 	  			= gClassifySpecsPtr->numberClasses;
	  	stat_info_ptr->nfeatures 		= gClassifySpecsPtr->numberFeatures;
			
   	}	//	"if (returnCode == 0)"
   
   return (continueFlag);

}	// end "Interface"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 PostEchoClassifier
//
// Software purpose:	Given a row of class assignments, count the number of pixels
//   						classified to each class.
//
//	Parameters in:
//
// Parameters out:
//
// Value Returned:	
// 
//	Called by: 			ClassifyArea in SClassfy.cpp
//
// Coded by				Byeungwoo Jeon			Date: 01/01/1989
// Revised by			Larry L. Biehl			Date: 02/14/2014

SInt16 PostEchoClassifier (
				SInt16								classPointer, 
				AreaDescriptionPtr				areaDescriptionPtr,
				CMFileStream*						resultsFilePtr, 
				CMFileStream*						clProbabilityFilePtr, 
				HUCharPtr 							ioBuffer1Ptr, 
				EchoClassifierVar*				echo_info, 
				ClassifierVarPtr 					clsfyVariablePtr, 
				SInt64*	 							countVectorPtr)

{
	SInt64								numberSamplesPerChan;

  	HSInt16Ptr							fieldClassNumberPtr;
	HPtr									offScreenBufferPtr;
	HSInt32Ptr							epix_ptr;
	HUCharPtr 							probabilityBufferPtr;
  	HUInt32Ptr							field_number_table;
	ImageOverlayInfoPtr				imageOverlayInfoPtr;	
	SInt16*		 						classPtr;
	WindowPtr							windowPtr;
  	
	Point			 						point;
  	
	RgnHandle							rgnHandle;
	
	SInt32								ibuf;
  	
	UInt32								channelSampleSkip,
											count,
											epixLineBufferCount,
											line,
											lineCount,
											lineEnd,
											lineInterval,
											lineLoopNextTime,
											sample,
											sampleInterval;
	
	SInt16								errCode,
											returnCode;
	
	UInt16								maxClass;
	
	Boolean								polygonField,
											probFormatFlag,
											savedThresholdFlag,
											useTempDiskFileFlag;
	
	
			// Initialize local variables.														
	
	polygonField 	= areaDescriptionPtr->polygonFieldFlag;
	rgnHandle 		= areaDescriptionPtr->rgnHandle;
	classPtr 		= gClassifySpecsPtr->classPtr;
	lineEnd 			= areaDescriptionPtr->lineEnd;
	lineInterval 	= areaDescriptionPtr->lineInterval;
	lineCount 		= 0;
	
	sampleInterval 		= 1;
	channelSampleSkip 	= 1;
	numberSamplesPerChan = areaDescriptionPtr->numSamplesPerChan;
	windowPtr = 	GetWindowPtr (gProjectInfoPtr->overlayImageWindowInfoHandle);	
	
	imageOverlayInfoPtr = GetImageOverlayInfoPtr (gClassifySpecsPtr->imageOverlayIndex,
																 kLock,
																 NULL);
	
	epix_ptr = echo_info->epix_ibufPtr;
	useTempDiskFileFlag = echo_info->useTempDiskFileFlag;
   fieldClassNumberPtr	= echo_info->fieldClassNumberPtr;
   field_number_table 	= echo_info->field_number_table;
	epixLineBufferCount = (UInt32)(numberSamplesPerChan * sizeof (SInt32)); 
   
	probabilityBufferPtr = 
								&ioBuffer1Ptr[gClassifySpecsPtr->probabilityBufferOffset];
	
			// Write carriage return and null character to classification record		
	
	sample = (UInt32)numberSamplesPerChan;		
   ioBuffer1Ptr[sample] = kCarriageReturn;
   sample++; 
	
	#if defined multispec_win  
		ioBuffer1Ptr[sample] = kLineFeed;
		sample++; 
	#endif	// defined multispec_win 
	
   ioBuffer1Ptr[sample] = kNullTerminator;
   
			// Turn the write probability flag off since that file has 				
			// already been written if requested.											
	
	probFormatFlag = (gOutputCode & kProbFormatCode);
	if (probFormatFlag)		
		gOutputCode -= kProbFormatCode;
   
			// Turn the threshold data flag off if this is a training or test		
			// field or the probability information was not written to disk.  	
			// The data cannot be thresholded at this time.								
	
	savedThresholdFlag = gClassifySpecsPtr->thresholdFlag;
	gClassifySpecsPtr->thresholdFlag = FALSE;
	
	lineLoopNextTime = gNextTime;
	
	offScreenBufferPtr = GetImageOverlayOffscreenPointer (imageOverlayInfoPtr);
	
	errCode = noErr;
	if (useTempDiskFileFlag)
		{
		errCode = MSetMarker (echo_info->tempFileStreamPtr, 
										fsFromStart, 
										0, 
										kErrorMessages);
		
		if (errCode != noErr)
																				return (errCode);
		
		}	// end "if (useTempDiskFileFlag)"
	
	for (line=areaDescriptionPtr->lineStart; line<=lineEnd; line+=lineInterval)
		{
				// Display line status information.											
		
		lineCount++;
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		
				// Exit routine if user has "command period" down				
		
		if (TickCount () >= lineLoopNextTime)
			{
			if (gOutputCode & kCreateImageOverlayCode && 
				 line > (UInt32)areaDescriptionPtr->lineStart)
				{
				InvalidateWindow (windowPtr, kImageArea, FALSE);
				
				#if defined multispec_win  
					windowPtr->UpdateWindow ();
				#endif	// defined multispec_win  
				
				}	// end "if (gOutputCode & kCreateImageOverlayCode && ..."
			
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				returnCode = 3;
				break;
				
				}	// end "if (!CheckSomeEvents (..."
			
			lineLoopNextTime = gNextTime;
			
			}	// end "if (TickCount () >= lineLoopNextTime)" 
			
		if (useTempDiskFileFlag)
			{
			count = epixLineBufferCount;
			epix_ptr = echo_info->epix_ibufPtr;
			errCode = MReadData (echo_info->tempFileStreamPtr, 
											&count, 
											epix_ptr,
											kErrorMessages);
			
			if (errCode != noErr)
				break;
			
			}	// end "if (useTempDiskFileFlag)"
		
				// Loop through the number of samples in the line of data			
		
		point.v = (SInt16)line;
		point.h = (SInt16)areaDescriptionPtr->columnStart;
		
		for (sample=0; sample<numberSamplesPerChan; sample++)
			{
			if (!polygonField || PtInRgn (point, rgnHandle))
				{
				ibuf = *epix_ptr;
				
				if (!(ibuf & 0x40000000))
					{
							// Pixel is not thresholded.											
					
					if (ibuf > 0)		// Homogeneous cell 
      				maxClass = (UInt8)fieldClassNumberPtr[field_number_table[ibuf]];
					
      			else	// ibuf < ZERO 
      				{
						ibuf |= 0x40000000;
	   				maxClass = (UInt16)-ibuf;
						
	   				}	// end "else ibuf < ZERO" 
	   			
							// Loop through the classes and get the discriminant value 	
							// for each class.														
					
					maxClass--;
			   	countVectorPtr[classPtr[maxClass]]++;
			   	ioBuffer1Ptr[sample] = (UInt8)classPtr[maxClass];
			   	
			   	}	// end "if (!(ibuf & 0x40000000))" 
				
			   else	// ibuf & 0x40000000 
							// Pixel is thresholded.												
			   	{		
					countVectorPtr[0]++;
					ioBuffer1Ptr[sample] = 0;
						
			   	}	// end "else ibuf & 0x40000000" 
			   
			   }	// end "if (!polygonField || PtInRgn (point, rgnHandle))" 
			
			else	// polygonField && !PtInRgn (point, rgnHandle) 
				ioBuffer1Ptr[sample] = 0;
			
			point.h++;
			epix_ptr++;
			
			}	// end "for (sample=0; sample<..." 
		
		returnCode = WriteClassificationResults (gOutputBufferPtr,
															  areaDescriptionPtr, 
															  resultsFilePtr, 
															  clProbabilityFilePtr, 
															  countVectorPtr, 
															  lineCount);
		if (returnCode == 1)
																							return (1);
		
		if (gOutputCode & kCreateImageOverlayCode)
			{
			CopyToOffscreenBuffer (NULL, // fileIOInstructionsPtr,
											imageOverlayInfoPtr,
											gClassifySpecsPtr->imageOverlayIndex,
											gProjectInfoPtr->overlayImageWindowInfoHandle,
											line,
											(UInt32)areaDescriptionPtr->columnStart,
											(UInt32)areaDescriptionPtr->columnInterval,
											(UInt32)areaDescriptionPtr->numSamplesPerChan,
											areaDescriptionPtr->lineStart,
											areaDescriptionPtr->rgnHandle,
											gOutputBufferPtr,
											offScreenBufferPtr,
											1,
											FALSE);
			
			}	// end "if (gOutputCode & kCreateImageOverlayCode)"
		
		}	// end "for (line=areaDescriptionPtr->lineStart; ..." 
	
			// Force overlay to be drawn if it has not been already.
	
	if (gOutputCode & kCreateImageOverlayCode)
		{
		InvalidateWindow (windowPtr, kImageArea, FALSE);
		
		if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
			returnCode = 3;
		
		}	// end "if (gOutputCode & kCreateImageOverlayCode)"
	
  	UnlockImageOverlayOffscreenBuffer (imageOverlayInfoPtr);
	
	if (!gOSXCoreGraphicsFlag)
		{
		WindowInfoPtr imageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
												gProjectInfoPtr->overlayImageWindowInfoHandle);	
		if (imageWindowInfoPtr != NULL)
			imageWindowInfoPtr->drawBaseImageFlag = TRUE;
		
		}	// end "if (!gOSXCoreGraphicsFlag)"
	
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineCount);
	
	if (probFormatFlag)
		gOutputCode += kProbFormatCode;
	
			// If area is a training or test field, list summary of 					
			// classification results.															 
	
	if (areaDescriptionPtr->classNumber > 0)
		returnCode = ListTrainTestSummary (classPointer, 
													  resultsFilePtr, 
													  areaDescriptionPtr, 
													  countVectorPtr,
													  clsfyVariablePtr, 
													  gTempDoubleVariable1);
	
	gClassifySpecsPtr->thresholdFlag = savedThresholdFlag;
	
	return (returnCode);
	
}	// End routine "PostEchoClassifier" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean SetupClsfierStats ()
//
//	Software purpose:	The purpose of this routine is to set up statistics used
//							in ECHO Classifier. The class statistics are stored in
//							"clas_stat" structure.
//
//	Parameters in:		None		
//
//	Parameters out:	None
//
// Value Returned:	int	"err_code"	
//			err_code = 0	:	no error
//			err_code = 1	:	Memory Allocation error
//			err_code = 2	:	Non-positive determinant error
//
// Called by			EchoLikeClsfierControl () in echo_classify.c
//
//	Coded By:			Byeungwoo Jeon			Date: 06/10/1991
// Modified by			Larry L. Biehl			Date: 01/21/2016

Boolean SetupClsfierStats (void)

{               
	double								clsfierConstant,
											determinant,
											totalProbability;
										
	HChannelStatisticsPtr			classChannelStatsPtr;
	CMFileStream*						resultsFileStreamPtr;
	
   double								*log_prior,
   										*sprd,
   										*Log_determinant,
   										*classConstantPtr,
   										*tempCovariancePtr;
										
	HDoublePtr							covinv,
											pivotPtr,
											vmean,
											vprd;
	
	SInt16								*featurePtr,
											*indexColPtr,
											*indexRowPtr,
  											*ipivotPtr,
											*statFeaturePtr;
	
  	statistics							*sta_ptr,
   										*class_stat;
	
   UInt32								*number_of_training_samples;
										
	UInt32								bytes,
											classIndex,
											clsfyChanMeanStart,
											clsfyCovStart,
											ncls;
										
   SInt16								cellSize,
   										nband,
  											numberFeatures,
											weightsIndex;
	
	UInt16								classStorage,
											statClassNumber;
										
	Boolean								continueFlag = FALSE,
											determinantOKFlag = TRUE;
   
      
			// Set up some of the local varibles
			
 	resultsFileStreamPtr = GetResultsFileStreamPtr (0);
   
	clsfyChanMeanStart	= 0;
	clsfyCovStart 			= 0;
 	ncls						= gCommon_InfoVariablePtr->stat_info.ncls;
	nband						= gCommon_InfoVariablePtr->pixel_info.nband;
	cellSize					= gEchoClassifierVariablePtr->cell_size;
	numberFeatures 		= gClassifySpecsPtr->numberFeatures;
	
	pivotPtr = 					NULL;
	indexRowPtr =				NULL;
	indexColPtr = 				NULL;
	ipivotPtr = 				NULL;
	classChannelStatsPtr = 	NULL;
	tempCovariancePtr = 		NULL;
	
			// featureHandle is already locked.
			
	featurePtr = (SInt16*)GetHandlePointer (gClassifySpecsPtr->featureHandle);

			// Memory Allocation to Class_Stats

	continueFlag = malloc_class_stat (
								&(gCommon_InfoVariablePtr->class_stat),
								ncls,
								nband,
								&(gCommon_InfoVariablePtr->stat_info.already_read_stat));	 
	class_stat = gCommon_InfoVariablePtr->class_stat;
	 
			// Allocate Memory to the variables

	bytes = (UInt32)numberFeatures * sizeof (SInt16);
	
   if (continueFlag)
   	{
	   indexRowPtr = (SInt16*)MNewPointer (bytes);
		continueFlag = (indexRowPtr != NULL);
		
		}	// end "if (continueFlag)"
	   
   if (continueFlag)
   	{
	   indexColPtr = (SInt16*)MNewPointer (bytes);
		continueFlag = (indexColPtr != NULL);
		
		}	// end "if (continueFlag)"
	
   if (continueFlag)
   	{
	   ipivotPtr = (SInt16*)MNewPointer (bytes);
		continueFlag = (ipivotPtr != NULL);
		
		}	// end "if (continueFlag)"
	   
   bytes = (UInt32)numberFeatures * sizeof (double);
   if (continueFlag)
   	{
	   pivotPtr = (HDoublePtr)MNewPointer (bytes);
		continueFlag = (pivotPtr != NULL);
		
		}	// end "if (continueFlag)"
	   											
			// Get pointer to memory to use for class channel statistics		
			// that will be used in computing the covariance matrix and			
			// getting the data for the class mean vector.						
				
	bytes = (UInt32)nband * sizeof (ChannelStatistics);
   if (continueFlag)
   	{
		classChannelStatsPtr = (HChannelStatisticsPtr)MNewPointer (bytes);
		continueFlag = (classChannelStatsPtr != NULL);
		
		}	// end "if (continueFlag)"
					
   if (continueFlag)
		continueFlag = SetupFeatureTransformationMemory (
											gClassifySpecsPtr->featureTransformationFlag, 
											numberFeatures, 
											nband, 
											&gTransformationMatrix.eigenVectorPtr, 
											&gTransformationMatrix.tempMatrixPtr,
											NULL, 
											&gTransformationMatrix.eigenFeaturePtr,
											kLoadMatricesVectors,
											(UInt16*)featurePtr);
								
			//	Normalize a priori probabilities and Get the constant for a classification.
			//	
			//	log (prior) - (number-of-features/2 * log (2pi)) - Log (determinant) / 2
			//	
			// Since " log (prior) - Log (determinant) / 2" term is dependent on class, this
			// term is addded inside the loop below.
				
	if (continueFlag) 
		{
   	clsfierConstant = (double)numberFeatures / 2 * log (kTWO_PI);
   	
		weightsIndex = GetProjectClassWeightsIndex ();
				
		totalProbability = GetTotalProbability (
										(UInt16*)gClassifySpecsPtr->classPtr,
										ncls,
										weightsIndex);
   		
  		}	//	"if (continueFlag)"
		
	statFeaturePtr = (SInt16*)GetStatisticsFeatureVector (
										gClassifySpecsPtr->featureTransformationFlag, 
										(UInt16*)featurePtr);
										
			// Get the common covariance matrix.
	
	if (continueFlag && gEchoClassifierVariablePtr->algorithmCode == kFisherMode)
		{		
				// Get temporary memory for computing the common covariance matrix
							
		bytes = (UInt32)nband * nband * sizeof (double);
	   tempCovariancePtr = (double*)MNewPointer (bytes);
		continueFlag = (tempCovariancePtr != NULL);
		
		if (continueFlag)
			continueFlag = GetCommonCovariance (class_stat->covinv,
															tempCovariancePtr,
															classChannelStatsPtr,
															(UInt16*)gClassifySpecsPtr->classPtr,
															(UInt16*)statFeaturePtr,  
															gClassifySpecsPtr->numberClasses,
															nband, 
															kSquareOutputMatrix,
															kMeanCovariance,
															kMixedStats,
															FALSE);
		
		if (continueFlag)
			{ 
			if (gClassifySpecsPtr->featureTransformationFlag)
				TransformSymmetricMatrix (gTransformationMatrix.eigenVectorPtr, 
													class_stat->covinv, 
													tempCovariancePtr, 
													class_stat->covinv, 
													numberFeatures,
													nband,
													kSquareOutputMatrix);
									
					// Get the inverse of the covariance matrix and the 				
					// determinant																	
		
	 		InvertSymmetricMatrix (class_stat->covinv,
											(SInt16)numberFeatures,
											pivotPtr,
											indexRowPtr,
											indexColPtr,
											ipivotPtr,
											&determinant,
											&class_stat->Log_determinant,
											kReturnMatrixInverse);
				
			}	// end "if (continueFlag && ..."
												
				// Do not need memory for temporary Covariance any more.
			
  		tempCovariancePtr = CheckAndDisposePtr (tempCovariancePtr);	
						
		bytes = (UInt32)nband * nband * sizeof (double);
			
		}	// end "if (continueFlag && ...->algorithmCode == kFisherMode"
	
			//	Loop through the classes to set up class statistics including
			//		mean vector
			//		covariance matrix
			//		determinant of covariance matrix
			//		inverse of covariance matrix
			//		discriminant constant

   if (continueFlag) 
   	{
	   for (classIndex=0; classIndex<ncls; classIndex++) 
	   	{
			sta_ptr 				= class_stat + classIndex;
			log_prior 			= &(sta_ptr->log_prior);
			vmean 	  			= sta_ptr->vmean; 
			vprd 	  				= sta_ptr->vprd;
			covinv 	  			= sta_ptr->covinv;
			sprd 	  				= &(sta_ptr->sprd);
			Log_determinant 	= &(sta_ptr->Log_determinant);
			classConstantPtr	= &(sta_ptr->classConstant);
			number_of_training_samples = &(sta_ptr->number_of_training_samples);
			statClassNumber = gClassifySpecsPtr->classPtr[classIndex] - 1;
			
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];	
			
			*log_prior = GetClassWeightValue (
										statClassNumber, weightsIndex, totalProbability);
										
			*log_prior = log (*log_prior);
				
			if (gEchoClassifierVariablePtr->algorithmCode != kFisherMode)
				{
						// Get the class covariance matrix - full form, not triangular	
	
				GetTransformedClassCovarianceMatrix (
														nband, 
														classChannelStatsPtr, 
														covinv, 
														(UInt16*)statFeaturePtr, 
														statClassNumber,
														kSquareOutputMatrix,
														kMeanCovariance,
														gTransformationMatrix.eigenVectorPtr,
														gTransformationMatrix.tempMatrixPtr,
														numberFeatures);
							
						// Get the class mean vector												
				
				ReduceMeanVector (classChannelStatsPtr, 
										vmean,
										(SInt16)nband,
										NULL);
									
						// Get the inverse of the covariance matrix and the 				
						// determinant																	
			
		 		determinantOKFlag = InvertSymmetricMatrix (covinv,
																			(SInt16)numberFeatures,
																			pivotPtr,
																			indexRowPtr,
																			indexColPtr,
																			ipivotPtr,
																			&determinant,
																			Log_determinant,
																			kReturnMatrixInverse);
										
				if (gOperationCanceledFlag)
					{
					continueFlag = FALSE;
					break;
					
					}	// end "if (gOperationCanceledFlag)"
	 				
						// Check the determinant
		  
		   	if (determinantOKFlag) 
		   		{
					if (gEchoClassifierVariablePtr->algorithmCode == kMahalanobisMode)
						*Log_determinant = 0;
						
					if (gClassifySpecsPtr->featureTransformationFlag)
						{
						MatrixMultiply (gTransformationMatrix.eigenVectorPtr, 
												covinv, 
												gTransformationMatrix.tempMatrixPtr, 
												nband,
												numberFeatures,
												numberFeatures,
												kTransposeMat1ByMat2);
												
						MatrixMultiply (gTransformationMatrix.tempMatrixPtr,
												gTransformationMatrix.eigenVectorPtr, 
												covinv, 
												nband,
												numberFeatures,
												nband,
												kMat1ByMat2);
						
						}	// end "if (...->featureTransformationFlag)"	
					/*					
		       	MatrixMultiply (vmean, covinv, vprd, 1, nband, nband, 1);
					symtrix (vmean, covinv, sprd, nband);
					
					*sprd	*= cellSize;
					
		   	 			// Class constant for maximum likelihood classifier.
		   	 			
		   	 	sta_ptr->classConstantML = 
		   	 								*log_prior - clsfierConstant - *Log_determinant/2;
		   	 	
		   	 			// Class constant for echo classifier - includes priors 
		   	 			// and -n*log (2pi)/2.
		   	 	
		   	 	*classConstantPtr  = (double)cellSize * sta_ptr->classConstantML;
					*/
	  				}	// end "if (determinantOKFlag"

	  			}	// end "if (classIndex == 0 || ...->algorithmCode != kFisherMode)" 
			
			else	// ...->algorithmCode == kFisherMode
				{
						// Copy the common covariance into each class covariance.
				
				if (classIndex != 0)
					BlockMoveData (class_stat->covinv, covinv, bytes);
		
						// Get the class mean vector
					
				GetClassMeanVector (nband, 
											classChannelStatsPtr, 
											vmean, 
											(UInt16*)statFeaturePtr, 
											statClassNumber);
								
				*Log_determinant = class_stat->Log_determinant;
											
				}	// end "if (...->algorithmCode == kFisherMode)"
				
	   	if (determinantOKFlag) 
	   		{
	       	MatrixMultiply (vmean, covinv, vprd, 1, nband, nband, 1);
				symtrix (vmean, covinv, sprd, nband);
				
				*sprd	*= cellSize;
				
	   	 			// Class constant for maximum likelihood classifier.
	   	 			
	   	 	sta_ptr->classConstantML = 
	   	 								*log_prior - clsfierConstant - *Log_determinant/2;
	   	 	
	   	 			// Class constant for echo classifier - includes priors 
	   	 			// and -n*log (2pi)/2.
	   	 	
	   	 	*classConstantPtr  = (double)cellSize * sta_ptr->classConstantML;
	   	 	
	   	 	}	// end "if (determinantOKFlag)"
	  			
	  		else	// !determinantOKFlag
	  			{
						// List message that covariance matrix for 						
						// this class could not be inverted.								
					
				continueFlag = ListClassInformationMessage (
																	kProjectStrID, 
																	IDS_Project31,
																	resultsFileStreamPtr, 
																	gOutputForce1Code,
																	statClassNumber, 
																	continueFlag);
						
				}	// end "else !determinantOKFlag"
				
			}	// end of "for (classIndex=0; classIndex<ncls...)"
		
   	}	//	"if (continueFlag)"

			//  Release the memories allocated in this function before exiting 
			//   Memory assigned to "class_stat" is not releases here.

   CheckAndDisposePtr (pivotPtr);
   CheckAndDisposePtr (ipivotPtr);
   CheckAndDisposePtr (indexColPtr);
   CheckAndDisposePtr (indexRowPtr);
		
			// Release memory for the classify channels statistics.
			
   CheckAndDisposePtr ((char*)classChannelStatsPtr);
	
			// Release memory for transformation matrix information.
			
	ReleaseFeatureTransformationMemory ();

	return (continueFlag);
	
}	// end  "SetupClsifierStats"

