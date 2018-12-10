//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SStatisticsEnhancement.cpp
//
//	Authors:					Behzad M Shahshahani
//								Larry L. Biehl
//
//	Revision date:			12/07/2018
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file are to enhance
//								the class statistics by adding information in from
//								unlabeled sample.  Behzad M Shahshahani developed the 
//								ideas for this processor and the code.
//
//	Functions in file:	SInt16	 			behzad_ModifyStatistics
//								void 					CopyEnhancedStatsToProject
//								Boolean 				Gaussian
//								UInt32	 			GetStatisticsEnhancePixelInterval
//								UInt32				GetTotalNumberTrainPixels
//								UInt32	 			GetUnlabeledLogLikes
//								Boolean 				LoadStatEnhanceClassStatistics
//								Boolean	 			LoadStatisticsEnhanceSpecs
//								Boolean	 			StatisticsEnhance
//								void		 			StatisticsEnhanceControl
//								Boolean				StatisticsEnhanceDialog
//								void 					UpdateNumberLabeledSamples
//								void					UpdateNumberUnlabeledSamples
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		StatisticsEnhanceControl
//			GetProjectImageFileInfo (in SProject.cpp)*
//			LoadStatisticsEnhanceSpecs
//			StatisticsEnhanceDialog
//				StatisticsEnhanceDialogInitialize
//				StatisticsEnhanceDialogUpdateChiSquaredValue
//				UpdateNumberLabeledSamples
//				UpdateNumberUnlabeledSamples
///			StatisticsEnhanceDialogOK
//				
//			VerifyProjectStatsUpToDate (in SProjectUtilities.cpp)*
//			GetStatusDialog (in SDlgUtil.cpp)
//			ListHeaderInfo (in SStrings.cpp)
//			CheckClassEnhancedStats (in SProjectUtilities.cpp)
//			CheckNumberOfPixelsInClass (in SProjectUtilities.cpp)
//
//			StatisticsEnhance
//				GetClassInfoStructure (in SProjectUtilities.cpp)
//				AssignClassInfoMemory (in SProjectUtilities.cpp)
//				GetTotalNumberTrainPixels
//				GetNumberPixelsInArea (in SUtility.cpp)
//				GetIOBufferPointers (in SMemUtil.cpp)
//				InitializeAreaDescription (in SUtilities.cpp)*
//				GetFieldDataValues (in SPMatUtl.cpp)
//				SetupMatrixInversionMemory (in SPMatUtl.cpp)
//				InitializeStatisticsVariables
//					LoadStatEnhanceClassStatistics
//						GetClassCovarianceMatrix (in SStatCom.cpp)
//						ReduceMeanVector (in SMatUtil.cpp)
//						InvertLowerTriangularMatrix (in SPMatUtl.cpp)
//						ListClassInformationMessage (in SProjectUtilities.cpp)
//						GetClassWeightsIndex (in SProjectUtilities.cpp)
//						GetTotalProbability (in SProjectUtilities.cpp)
//						GetClassWeightValue (in SProjectUtilities.cpp)
//						
//					GetUnlabeledLogLikes
//						Gaussian
//						CheckSomeEvents (in multiSpec.c and CStubs.cpp)
//						
//					LoadStatEnhanceClassStatistics
//						(See above)
//
//					GetLongestDistanceProject
//						GetLongestDistanceClass
//							GetLongestDistanceField
//								GetFieldBoundary (in SProjectUtilities.cpp)
//								SetUpFileIOInstructions (in SFileIO.cpp)
//								GetLineOfData (in SFileIO.cpp)
//								CloseUpFileIOInstructions (in SFileIO.cpp)
//								CloseUpAreaDescription (in SUtility.cpp)
//								Gaussian
//								UpdateLabeledValueList
//									BlockMoveData (in CStubs.cpp)
//									
//								CheckSomeEvents (in multiSpec.c and CStubs.cpp)
//								CloseUpFileIOInstructions (in SFileIO.cpp)
//								CloseUpAreaDescription (in SUtility.cpp)
//
//				DisposeIOBufferPointers (in SMemUtil.cpp)
//				ListStatEnhanceSpecifications
//
//				behzad_ModifyStatistics
//					InitializeGlobalAlertVariables (in SProjectUtilities.cpp)
//					GetUnlabeledLogLikes
//						(See above)
//
//					GetClassSumsSquares (in SStatCom.cpp)
//					CheckSomeEvents (in multiSpec.c and CStubs.cpp)
//					UpdateOutputWScrolls (in controls.c and)
//					CheckSomeEvents (in multiSpec.c and CStubs.cpp)
//					GetClassSumsSquares (in SStatCom.cpp)
//					CheckSomeEvents (in multiSpec.c and CStubs.cpp)
//					ResetZeroVariances (in SStatCom.cpp)
//					InvertLowerTriangularMatrix (in SPMatUtl.cpp)
//					ListClassInformationMessage (in SProjectUtilities.cpp)
//					GetMaximumClassDistances
//						(See above)
//
//				ClearGlobalAlertVariables (in SProjectUtilities.cpp)
//				ReleaseMatrixInversionMemory (SPMatUtl.cpp)
//				CopyEnhancedStatsToProject
//				ListClassesUsed (in SProjectUtilities.cpp)
//				ReleaseClassInfoMemory (in SProjectUtilities.cpp)
//				
//			
//			ListCPUTimeInformation (in SStrings.cpp)
//			UpdateOutputWScrolls (in controls.c and)
//			CloseStatusDialog (in SDlgUtil.cpp)
//			MInitCursor (in SUtility.cpp)
//			CheckAndUnlockHandle (in SMemUtil.cpp)
//			UnlockProjectWindowInfoHandles (in SProject.cpp)
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_UnlabeledSamples 					4
	#define IDC_LabeledSamples						6
	#define IDC_UseEnhancedStats 					21
	#define IDC_SoftChiChiThreshold				26
	#define IDC_SoftPercentThresholdPrompt		27
	#define IDC_SoftPercentThreshold				28
	#define IDC_HardChiChiThreshold				31
	#define IDC_HardPercentThresholdPrompt		32
	#define IDC_HardPercentThreshold				33
	#define IDC_WeightingPrompt 					35
	#define IDC_Weighting 							36
#endif	// defined multispec_mac || defined multispec_mac_swift    
  
#if defined multispec_win  
	#include "WEnhanceStatisticsDialog.h"   
#endif	// defined multispec_win  

#if defined multispec_lin 
   #include "LMultiSpec.h"
	#include "LEnhanceStatisticsDialog.h"   
#endif	// defined multispec_lin


//#include	"SExtGlob.h"

		// Enhance Statistics constants												
	
#define	kUseNoThreshold					1
#define	kUseChiChiThreshold				2
#define	kUsePercentThreshold				3
#define	kUseFarthestPixelThreshold		4


	
extern void StatisticsEnhanceDialogInitialize (
				DialogPtr							dialogPtr,
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				UInt16*								localClassPtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				Boolean*								useEnhancedStatisticsFlagPtr,
				SInt16*								classWeightSetPtr,
				SInt16*								weightsSelectionPtr,
				SInt16*								softThresholdSelectionPtr,
				double*								softChiChiThresholdPtr,
				double*								softPercentThresholdPtr,
				SInt16*								hardThresholdSelectionPtr,
				double*								hardChiChiThresholdPtr,
				double*								hardPercentThresholdPtr,
				Boolean*								weightLabeledFlagPtr,
				double*								labelWeightPtr,
				SInt32*								iterationMaxPtr,
				SInt32*								iterationStopLengthPtr,
				double*								logLikeStopPercentPtr);

extern void StatisticsEnhanceDialogOK (
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				UInt16*								localClassPtr,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				Boolean								useEnhancedStatisticsFlag,
				SInt16								weightsSelection,
				float*								classWeightsPtr,
				SInt16								softThresholdSelection,
				double								softChiChiThreshold,
				double								softPercentThreshold,
				SInt16								hardThresholdSelection,
				double								hardChiChiThreshold,
				double								hardPercentThreshold,
				Boolean								weightLabeledFlag,
				double								labelWeight,
				SInt32								iterationMax,
				SInt32								iterationStopLength,
				double								logLikeStopPercent);

extern void StatisticsEnhanceDialogUpdateChiSquaredValue (
				DialogPtr							dialogPtr,
				SInt16								thresholdCode,
				SInt16								chiChiThresholdItemNumber,
				double								thresholdPercent,
				double*								chiChiThresholdPtr);

extern void StatisticsEnhanceDialogUpdateThresholdItems (
				DialogPtr							dialogPtr,
				SInt16								thresholdCode,
				SInt16								chiChiThresholdItemNumber,
				SInt16								percentThresholdPromptItemNumber,
				SInt16								percentThresholdItemNumber,
				double								thresholdPercent,
				double*								chiChiThresholdPtr);


	
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													

SInt16	behzad_ModifyStatistics (
				StatEnhanceSpecsPtr 				gStatEnhanceSpecsPtr,
				ClassInfoPtr 						classInfoPtr);

void 		CopyEnhancedStatsToProject (
				ClassInfoPtr 						classInfoPtr);

PascalVoid DrawHardThresholdPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawSoftThresholdPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

double 	Gaussian (
				HDoublePtr   						dataValuesPtr,
				HDoublePtr		 					invCovPtr,
				HDoublePtr 							meanPtr, 
				HDoublePtr 							diffPtr,
				UInt32			   				numberFeatures);

SInt16 	GetLongestDistanceClass (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				TypicalitySummaryPtr				classTypicalityPtr,
				HDoublePtr							labeledDataPtr,
				ClassInfoPtr						classInfoPtr,
				SInt16								classNumber);

SInt16 	GetLongestDistanceField (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				DistancesSummaryPtr				labeledDistancesSummaryPtr,
				HDoublePtr							labeledDataPtr,
				ClassInfoPtr						classInfoPtr,
				SInt16								classNumberCode, 
				SInt16								fieldNumber);

Boolean 	GetLongestDistanceProject (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				ClassInfoPtr						classInfoPtr);

void 		GetMaximumClassDistances (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr,
				ClassInfoPtr 						classInfoPtr);

SInt64	GetTotalNumberTrainPixels (
				SInt16*								classPtr,
				UInt32								numberClasses);

UInt32	 GetStatisticsEnhancePixelInterval (void);

UInt32	 GetUnlabeledLogLikes (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr,
				ClassInfoPtr 						classInfoPtr,
				SInt32	 							iount);

Boolean 	InitializeStatisticsVariables (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr,
				ClassInfoPtr						classInfoPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr);

Boolean 	ListStatEnhanceSpecifications (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr);

Boolean 	LoadStatEnhanceClassStatistics (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr,
				ClassInfoPtr						classInfoPtr,
				SInt16								covarianceStatsToUse);

Boolean	LoadStatisticsEnhanceSpecs (void);

Boolean	StatisticsEnhance (void);

Boolean	StatisticsEnhanceDialog (void);

void		UpdateLabeledValueList (
				DistancesSummaryPtr				distanceSummaryPtr,
				double								distance,
				HDoublePtr							inputBufferPtr,
				HDoublePtr							labeledDataPtr,
				UInt32								numberChannels);



		// Declarations of structures used only in this file.							
		
typedef struct DistancesList
	{
	double								distanceValue;
	
	SInt32								listIndex;
	SInt32								previousMaximum;
	SInt32								nextMaximum;
	
	} DistancesList, *DistancesListPtr;						
		
typedef struct DistancesSummary
	{
	DistancesList						distancesList[6];
	
	SInt32								freeIndex;
	
	UInt32								endIndex;
	UInt32								numberLabeledValues;
	UInt32								startIndex;
	
	} DistancesSummary, *DistancesSummaryPtr;				
		
typedef struct TypicalitySummary
	{
	double								distance;
	UInt32								numberSamples;
	
	} TypicalitySummary, *TypicalitySummaryPtr;
	
	
	
		// Declarations of variables used only in this file.

		// Variable indicating what the current hard threhold selection is.  
		// It is used by enhance statistics dialog. 
		//		=1 is None 									
		//		=2 is Chi*Chi
		//		=3 is percent
SInt16									gHardThresholdSelection;

		// Variable indicating what the current hard threhold selection is.  
		// It is used by enhance statistics dialog.
		//		=1 is None 									  									
		//		=2 is Chi*Chi
		//		=3 is percent
		//		=4 is Farthest labeled pixel	
SInt16									gSoftThresholdSelection;
								


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 behzad_ModifyStatistics
//
//	Software purpose:	The purpose of this routine is to modify the class
//							statistics using unlabeled samples.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Behzad M Shahshahani	Date: ??/??/1993
//	Revised By:			Larry L. Biehl			Date: 12/15/2010

SInt16 behzad_ModifyStatistics (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr, 
				ClassInfoPtr						classInfoPtr)

{	   
	//	integer m is the number of classes (=number of components in mixture).
	//	integer Nx is the number of unlabeled samples used
	//	array x contians the unlabeled samples.
	//	array N contains the number of training samples from classes.
	//	array z contains the training samples from the classes.
	//	array NT contains the number of test samples from classes.
	//	array test contains the test samples from the classes.
	//	array alpha contains the prior probabilties of the classes.
	//	array mean contains the mean vectors.
	//	array covar contains the covariance matrices.
	//	array inv contains the inverse covariances.
	//	array det contains the determinans of the covariances.
	//	array xlogl contains the value of the log likelihood at each iteration.

	double								alim,
											betha,			// Bezhad's ratio of the 	
																	// number of unlabeled and	
																	// labeled samples.
											diff,	
											likelihoodConstant,
											stopSlopeHeightFactor,
											tempDValue1,
											tempDValue2,
											tempDValue3,
											threshold2;			// Bezhad's 'c'
	
	HDoublePtr							covPtr,
											dataValuePtr,
											likePostPtr,		// Bezhad's 'p' & 'post'
											meanPtr,
											priorsPtr,				// Bezhad's 'alpha'
											sumSquarePtr,
											totalPtr,				// Bezhad's 'total'	
											totalLikePtr,			// Bezhad's 'pr' 			
											typicalityWeightPtr,	// Saldju's 'w'	
											xloglPtr;				// Bezhad's 'xlogl' 	
											
	SInt16*								classPtr;
	TypicalitySummaryPtr				classTypicalityPtr;

	SInt64								numberSamples,
											numcount;
					
	SInt32				 				iount;
											
	UInt32								i,
											k,
											l,
											numberChannels,
											numberClasses,
											numberUnlabeled,
											s,
											unLabeledCount;	
					
	SInt16								returnCode,
											stopSlopeLength;
     
	Boolean								continueFlag,
											determinantOKFlag,
											useSoftThresholdFlag;
	

			// Initialize local variables.												
			
	continueFlag = TRUE;
	returnCode = 0;
	diff = 0.0;
	classPtr = (SInt16*)GetHandlePointer (statEnhanceSpecsPtr->classHandle);
														
	numberClasses = statEnhanceSpecsPtr->numberClasses;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
														
	xloglPtr = statEnhanceSpecsPtr->totalLogLikePtr;
	priorsPtr = statEnhanceSpecsPtr->priorsPtr;
	totalPtr = &statEnhanceSpecsPtr->priorsPtr[numberClasses];
	numberUnlabeled = (UInt32)statEnhanceSpecsPtr->numberUnlabeled;
	threshold2 = 0.0; 
	likelihoodConstant = numberChannels * log (kTWO_PI) / 2;
	stopSlopeLength = statEnhanceSpecsPtr->iterationStopLength;
	stopSlopeHeightFactor = statEnhanceSpecsPtr->logLikeStopPercentChange/100;
	unLabeledCount = (UInt32)statEnhanceSpecsPtr->unlabeledCount;
	classTypicalityPtr = statEnhanceSpecsPtr->classTypicalityPtr;
	
	useSoftThresholdFlag = TRUE;
	if (statEnhanceSpecsPtr->softThresholdCode == kUseNoThreshold)
		useSoftThresholdFlag = FALSE;
							
			// Initialize some global variables.										
			
	gNextTime = TickCount ();
			
			// Set up information in the status dialog for the user.				
			
	DrawDialog (gStatusDialogPtr);
	
	ShowStatusDialogItemSet (kStatusCluster); 
	
	LoadDItemStringNumber (kEnhanceStatStrID, 
									IDS_StatEnhance9, 
									gStatusDialogPtr, 
									IDC_Status15, 
									(Str255*)gTextString);
									
	LoadDItemValue (gStatusDialogPtr, IDC_Status16, 0);
	
	ShowStatusDialogItemSet (kStatusMinutes);
	
	LoadDItemStringNumber (kEnhanceStatStrID, 
									IDS_StatEnhance10, 
									gStatusDialogPtr, 
									IDC_Status13, 
									(Str255*)gTextString);
									
	LoadDItemString (gStatusDialogPtr, IDC_Status14, (Str255*)"\0");
	        	                                                                                               
	LoadDItemStringNumber (kEnhanceStatStrID, 
									IDS_StatEnhance26,  // "\0Labeled:"
									gStatusDialogPtr, 
									IDC_Status2, 
									(Str255*)gTextString);
										
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status3, 
							(SInt32)gStatEnhanceSpecsPtr->numberLabeled);
	ShowStatusDialogItemSet (kStatusTitle3);
	
	if (unLabeledCount > 0)
		{                                                                        
		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance27,  // "\0Unlabeled:"
										gStatusDialogPtr, 
										IDC_Status7, 
										(Str255*)gTextString);
		
		LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)unLabeledCount);
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, (SInt32)numberUnlabeled);
		ShowStatusDialogItemSet (kStatusField);
		
		}	// end "if (unLabeledCount > 0)"
							
			// Initialize some global variables to allow the user to cancel	
			// the operation at the end of the current iteration instead of	
			// canceling immediately.														
	
	InitializeGlobalAlertVariables (kFinishDontCancelAlertID, 
												kAlertStrID,
												IDS_Alert23);

			// Start the EM iterations.													

	for (iount=0; iount<=statEnhanceSpecsPtr->iterationMax; iount++) 
		{	
				// Unlabeled samples.

		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance8, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);														
				
		likePostPtr =  statEnhanceSpecsPtr->likePtr;
		totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;
      xloglPtr[iount] = 0.0;
      
		unLabeledCount = GetUnlabeledLogLikes (
														statEnhanceSpecsPtr, classInfoPtr, iount);
			
				// Check if user cancelled.												
												
		if (gOperationCanceledFlag)
																					return (returnCode);
        	
    	xloglPtr[iount] -= (unLabeledCount * likelihoodConstant);
    								
				// Labeled samples.	

		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance21,
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);														

		numcount = 0;
    	for (i=0; i<numberClasses; i++)
    		{
        	if (priorsPtr[i] > threshold2)
        		{
    			numberSamples = (SInt32)classInfoPtr[i].no_samples;
    		
						// Get the raw labeled statistics for the class.
						// 'gInverseMatrixMemory.inversePtr' is used here for		
						// temporary storage.												
				
				meanPtr = classInfoPtr[i].meanPtr;
				covPtr = classInfoPtr[i].inversePtr;
				sumSquarePtr = gInverseMatrixMemory.inversePtr;
						
				continueFlag = GetClassSumsSquares ((UInt16)numberChannels,
																gTempChannelStatsPtr, 
																sumSquarePtr, 
																NULL, 
																classPtr[i]-1, 
																kTriangleOutputMatrix,
																kMeanCovariance);
				
    			alim = 0.;
       		for (l=0; l<numberChannels; l++)
       			{
       			tempDValue1 = meanPtr[l];
       			tempDValue2 = gTempChannelStatsPtr[l].sum - numberSamples * tempDValue1;
       			tempDValue3 = 0.;
       			
          		for (s=0; s<l; s++) 
          			{
          			tempDValue3 += *covPtr * (*sumSquarePtr - 
												tempDValue1 * gTempChannelStatsPtr[s].sum -
																				meanPtr[s] * tempDValue2);
          			
          			sumSquarePtr++;
          			covPtr++;
              	
             		}	// end "for (s=0; s<r; s++)" 
             		
             	tempDValue3 += tempDValue3;
          												
          		alim += tempDValue3 + *covPtr * (*sumSquarePtr - tempDValue1 * 
													(2 * gTempChannelStatsPtr[l].sum -
																		numberSamples * tempDValue1));
          			
          		sumSquarePtr++;
          		covPtr++;
		             	
            	}	// end "for (l=0; l<numberChannels; l++)" 
					
						// Check if user wants to exit.								
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (returnCode);
																			
					}	// end "if (TickCount () >= gNextTime)"
            	
     			xloglPtr[iount] += -0.5 * (alim + 
												numberSamples * classInfoPtr[i].logDeterminant);
      		
      		numcount += numberSamples;
      								
      		}	// end "if (priorsPtr[i] > threshold2)" 
        
 			}	// end "for (i=0; i<numberClasses; i++)" 
               
      xloglPtr[iount] -= (numcount * likelihoodConstant);
 			
 				// Break out of loop of the total log likelihood is 0.			
 				
 		if (xloglPtr[iount] == 0.)
 			break;

				// Check for convergence. 													
				
		if (iount == 0)
       	{
			if (statEnhanceSpecsPtr->covarianceStatsToUse != kEnhancedStats)
				{
	      			// If this is the first pass, threshold those unlabeled 	
	      			// pixels which did not belong to any class.  They will 	
	      			// not be considered any more.									
	      		
				totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;
	    		for (k=0; k<numberUnlabeled; k++)
	    			{
	        		if (*totalLikePtr == 0.0)
	        			*totalLikePtr = -1;
	        		
	        		totalLikePtr++;
	        		
	        		}	// end "for (k=0; k<numberUnlabeled; k++)" 
	        		
	        	}	// end "if (...->covarianceStatsToUse != kEnhancedStats)" 
	        	                                                                                                       
			LoadDItemStringNumber (kEnhanceStatStrID, 
											IDS_StatEnhance27,  // "\0Unlabeled:"
											gStatusDialogPtr, 
											IDC_Status7, 
											(Str255*)gTextString);
			
			LoadDItemValue (gStatusDialogPtr, IDC_Status10, (SInt32)numberUnlabeled);
			ShowStatusDialogItemSet (kStatusField);
        		
					// List the number of unlabel samples that are being used.		
															
			continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																	IDS_StatEnhance15,
																	NULL, 
																	gOutputForce1Code, 
																	(SInt32)unLabeledCount, 
																	continueFlag);
  
		  			// List header for the total log likelihood table.					
		  			
			continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																	IDS_StatEnhance16, 
																	(unsigned char*)gTextString, 
																	NULL, 
																	gOutputForce1Code, 
																	continueFlag);
			
        	}	// end "if (iount == 0)" 
       	
   	else	// iount > 0 
			{
      	diff = xloglPtr[iount] - xloglPtr[iount-1];
			if (diff == 0.)
           	returnCode = 1;

	  		if (diff > -0.0001 && iount >= stopSlopeLength) 
	  			{
	       	alim = stopSlopeHeightFactor * xloglPtr[iount - stopSlopeLength];
	        	diff = xloglPtr[iount] - xloglPtr[iount - stopSlopeLength];
	        	if (fabs (diff) <= fabs (alim) || fabs (diff) < 1) 
	           	returnCode = 1;
	       		
	    		}	// end "if (diff > 0. && iount >= stopSlopeLength)" 
	    		
	    	if (iount >= statEnhanceSpecsPtr->iterationMax)
	      	returnCode = 1;
       		
       	}	// end "else iount > 0" 
       	
		LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)unLabeledCount);
     	
				// List information to the output text window and status window.

     	if (iount == 0)
     		diff = 0.;
     		
     	else	// iount > 0 
     		{
      	diff = xloglPtr[iount] - xloglPtr[iount-1];
			LoadDItemRealValue (gStatusDialogPtr, IDC_Status14, diff, 5);
			
			}	// end "else iount > 0" 
      	
		sprintf ((char*)gTextString, 
					"    %6ld          %.1f        %.2f%s",
					iount,
					xloglPtr[iount],
					diff,
					gEndOfLine);
			
		continueFlag = OutputString ((CMFileStream*)NULL, 
												(char*)gTextString, 
												0, 
												gOutputForce1Code, 
												continueFlag);
				
				// Scroll output window to the selection and adjust the 		
				// scroll bar. This allows the user to see the list of total
				// likelihoods.																
		
		UpdateOutputWScrolls (gOutputWindow, 1, kDisplayNoMessage); 
			
		if (gAlertReturnCode == 1)
					// User requested that that this be the last iteration.			
			returnCode = 1;
       	
      if (returnCode == 1)
      	break;
      	
		LoadDItemValue (gStatusDialogPtr, IDC_Status16, iount+1);

   			// Compute the posteriors for the unlabeled samples. 

		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance22, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);	
										
				// Prepare for computing the priors for each class and the total
				// for the unlabeled samples.

    	for (i=0; i<numberClasses; i++)
    		{
        	priorsPtr[i] = 0.0;
        	totalPtr[i] = 0.0;
        	
        	}	// end "for (i=0; i<numberClasses; i++)"
        	
		likePostPtr = statEnhanceSpecsPtr->likePtr;
		totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;	
		
				// Note that "typicalityWeightPtr" will be Null when the soft threshold
				// is not being used. If so "typicalityWeightPtr" will be set the
				// same as "likePostPtr". This will not cause a problem as long as
				// "typicalityWeightPtr" is not actually used to store data.
				// This is done to reduce the number of if statements required.
				
		typicalityWeightPtr = statEnhanceSpecsPtr->typicalityWeightPtr;
	   if (!useSoftThresholdFlag)
			typicalityWeightPtr = likePostPtr;

    	for (k=0; k<numberUnlabeled; k++) 
    		{
        	if (*totalLikePtr > 0.0)
        		{
	   		for (i=0; i<numberClasses; i++)  
	   			{
	   			if (*likePostPtr > 0)
	   				{
		           	*likePostPtr /= *totalLikePtr;
	        			priorsPtr[i] += *likePostPtr;
	        			
	        			if (useSoftThresholdFlag)
		        			{
			        		if (*typicalityWeightPtr > classTypicalityPtr[i].distance)
			        			{
			        			*typicalityWeightPtr = sqrt (
			        					classTypicalityPtr[i].distance/(*typicalityWeightPtr));
			        					
			        			*likePostPtr *= *typicalityWeightPtr;
			        					
			        			}	// end "if (*typicalityWeightPtr > ...[i].distance)"
			        			
			        		else	// *typicalityWeightPtr <= classTypicalityPtr[i].distance
			        			*typicalityWeightPtr = 1.;
			        			
			        		}	// end "if (useSoftThresholdFlag)"
		        			
	         		totalPtr[i] += *likePostPtr;
		        			
		        		}	// end "if (*likePostPtr > 0)"
	        			
	        		likePostPtr++;
	        		typicalityWeightPtr++;
	        		
	        		}	// end "for (i=0; i<numberClasses; i++)" 
	        		
	        	}	// end "if (*totalLikePtr > 0.0)" 
	         
	       else	// *totalLikePtr <= 0.0 
	       	{
	   		for (i=0; i<numberClasses; i++)
	   			{
	          	*likePostPtr = 0.0;
	        		likePostPtr++;
	          		
	   			}	// end "for (i=0; i<numberClasses; i++)" 
	   			
	        	typicalityWeightPtr += numberClasses;
	   			
	   		}	// end " else *totalLikePtr <= 0.0" 
	   		
	   	totalLikePtr++;
        
  			}	// end "for (k=0; k<numberUnlabeled; k++)" 
			
				// Check if user wants to exit.											
		
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																					return (returnCode);
																			
			}	// end "if (TickCount () >= gNextTime)"

   			// Compute the the new priors, means and covariances.	

		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance23, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);			
   			
 		for (i=0; i<numberClasses; i++) 
 			{
    		numberSamples = classInfoPtr[i].no_samples;
													
					// Get the weighting (if any) for this class.					
			
			if (statEnhanceSpecsPtr->weightLabeledFlag)
				betha = statEnhanceSpecsPtr->labelWeight * totalPtr[i]/numberSamples;
				
			else	// !statEnhanceSpecsPtr->weightLabeledFlag 		
				betha = 1.;
				
					// The 'enhanced total' for the class.								
					
    		tempDValue1 = totalPtr[i] + betha * numberSamples;
    		
			meanPtr = classInfoPtr[i].meanPtr;
			covPtr = classInfoPtr[i].covariancePtr;
			
					// Get the raw labeled statistics for the class.				
						
			continueFlag = GetClassSumsSquares ((UInt16)numberChannels,
															gTempChannelStatsPtr, 
															covPtr, 
															NULL, 
															classPtr[i]-1, 
															kTriangleOutputMatrix,
															kMeanCovariance);
			
         		// Compute the new mean. 
	          	// 	- Get labeled sample portion.

       	for (l=0; l<numberChannels; l++)
				{ 
            *meanPtr = betha * gTempChannelStatsPtr[l].sum;
	         meanPtr++;

				}	// end "for (l=0; l<numberChannels; l++)"

         		// Continue computing the new mean. 
	          	// 	- Get unlabeled sample portion.	

			dataValuePtr = statEnhanceSpecsPtr->unlabelDataPtr;
	    	likePostPtr = &statEnhanceSpecsPtr->likePtr[i];
			
         for (k=0; k<numberUnlabeled; k++)
         	{	
				if (*likePostPtr > 0.0)
					{
					meanPtr = classInfoPtr[i].meanPtr;
	
		       	for (l=0; l<numberChannels; l++) 
		       		{
		            *meanPtr += *likePostPtr * *dataValuePtr;
		            dataValuePtr++;
		          	meanPtr++;
	          	
	          		}	// end "for (l=0; l<numberChannels; l++)"

					}	// end "if (*totalLikePtr > 0)"

				else	// totalLikePtr <= 0.0
					dataValuePtr += numberChannels;

	         likePostPtr += numberClasses;
						
						// Check if user wants to exit.									
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (returnCode);
																			
					}	// end "if (TickCount () >= gNextTime)"
	            	
	      	}	// end "for (k=0; k<numberUnlabeled; k++)"

			meanPtr = classInfoPtr[i].meanPtr;
       	for (l=0; l<numberChannels; l++) 
       		{
          	*meanPtr /= tempDValue1;
          	meanPtr++;
       	
				}	// end "for (l=0; l<numberChannels; l++)"
																			
					// Get the unlabeled "total" for the new covariance if the soft 
					// threshold is being used. If not use the same as those for the 
					// means.
			
		   if (useSoftThresholdFlag)
		   	{ 
		      totalPtr[i] = 0.0;
		        	
				totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;
		    	likePostPtr = &statEnhanceSpecsPtr->likePtr[i];
				typicalityWeightPtr = &statEnhanceSpecsPtr->typicalityWeightPtr[i];
		    	for (k=0; k<numberUnlabeled; k++)
		    		{
	        		if (*likePostPtr > 0)
	        			{
	         		*likePostPtr *= *typicalityWeightPtr;
	         		totalPtr[i] += *likePostPtr;
	         		
	         		}	// end "if (*likePostPtr > 0)"
			         		
			      likePostPtr += numberClasses;
			      typicalityWeightPtr += numberClasses;
		         	
		         }	// end "for (k=0; k<numberUnlabeled; k++)" 
		         
		      }	// end "if (useSoftThresholdFlag)"
					
					// Check if user wants to exit.											
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																					return (returnCode);
																		
				}	// end "if (TickCount () >= gNextTime)"		
    		
         		// compute the new covariance matrix 
													
					// Get the weighting (if any) for this class.					
			
			if (statEnhanceSpecsPtr->weightLabeledFlag)
				betha = statEnhanceSpecsPtr->labelWeight * totalPtr[i]/numberSamples;
				
			else	// !statEnhanceSpecsPtr->weightLabeledFlag 		
				betha = 1.;
          	
          		// Get labeled sample portion.										
          		// Cov*N = Sum (ViVj) + Sum (MiMj) - MiSum (Vj) - MjSum (Vi)
          		// Where V are data values, i,j are channel numbers, M		
          		// are Means, and N is the number of labeled samples.	
          		
			meanPtr = classInfoPtr[i].meanPtr;
       	for (l=0; l<numberChannels; l++)
       		{
       		tempDValue2 = meanPtr[l] * numberSamples - gTempChannelStatsPtr[l].sum;
          	tempDValue3 = meanPtr[l];
          	
          	for (s=0; s<=l; s++) 
          		{
					*covPtr += meanPtr[s] * tempDValue2 -
														tempDValue3 * gTempChannelStatsPtr[s].sum;
											
					*covPtr *= betha;		
					covPtr++;
              	
             	}	// end "for (s=0; s<=r; s++)" 
             	
            }	// end "for (l=0; l<numberChannels; l++)" 
					
					// Check if user wants to exit.									
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (returnCode);
																			
				}	// end "if (TickCount () >= gNextTime)"
          	
          		// Get unlabeled sample portion.										
            
			meanPtr = classInfoPtr[i].meanPtr;
	    	likePostPtr = &statEnhanceSpecsPtr->likePtr[i];
			dataValuePtr = statEnhanceSpecsPtr->unlabelDataPtr;
	     	for (k=0; k<numberUnlabeled; k++)
	     		{
				if (*likePostPtr > 0.0)
					{
					covPtr = classInfoPtr[i].covariancePtr;
	
		       	for (l=0; l<numberChannels; l++)
		       		{
		          	tempDValue2 = *likePostPtr * (dataValuePtr[l] - meanPtr[l]);
		          	
		          	for (s=0; s<=l; s++) 
		          		{
		               *covPtr += tempDValue2 * (dataValuePtr[s] - meanPtr[s]);
		              	covPtr++;
		              	
		             	}	// end "for (s=0; s<=l; s++)"
			             	
		            }	// end "for (l=0; l<numberChannels; l++)"

					}	// end "if (*totalLikePtr > 0.0)" 
						
						// Check if user wants to exit.								
				
				if (TickCount () >= gNextTime)
					{
					if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (returnCode);
																			
					}	// end "if (TickCount () >= gNextTime)"

	         likePostPtr += numberClasses; 
	         dataValuePtr += numberChannels;
                                                  
            }	// end "for (k=0; k<numberUnlabeled; k++)"

    		tempDValue1 = totalPtr[i] + betha * numberSamples;
			covPtr = classInfoPtr[i].covariancePtr;
       	for (l=0; l<numberChannels; l++)
       		{
          	for (s=0; s<=l; s++) 
          		{                   
	     			*covPtr /= tempDValue1;
					covPtr++;
              	
             	}	// end "for (s=0; s<=l; s++)" 
	             	
            }	// end "for (l=0; l<numberChannels; l++)" 
					
					// Check if user wants to exit.								
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																						return (returnCode);
																			
				}	// end "if (TickCount () >= gNextTime)"
	                               
 					// New priors.																
 			  		
			priorsPtr[i] /= unLabeledCount;

    		}	// end "for (i=0; i<numberClasses; i++)" 

				// Compute new class determinants

		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance24, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);

 		for (i=0; i<numberClasses; i++)
			{
			if (gProjectInfoPtr->setZeroVarianceFlag)
		
						// Set any zero variance to requested factor.							
						// This is done so that the matrix may be inverted.					
							
				ResetZeroVariances (classInfoPtr[i].covariancePtr, 
											kTriangleOutputMatrix,
											kMeanCovariance, 
											numberChannels);
									
			determinantOKFlag = InvertLowerTriangularMatrix (
																	classInfoPtr[i].covariancePtr,
																	classInfoPtr[i].inversePtr,
																	(SInt16)numberChannels,
																	&classInfoPtr[i].determinant, 
																	&classInfoPtr[i].logDeterminant,
																	kReturnMatrixInverse);
			
					// Make certain that the determinant is reasonable.				
					
			if (!determinantOKFlag)
				{
				returnCode = 3;
	
						// List message that covariance matrix for this class 		
						// could not be inverted.												
					
				continueFlag = ListClassInformationMessage (kProjectStrID,
																			IDS_Project31,
																			NULL, 
																			gOutputForce1Code,
																			classPtr[i]-1, 
																			continueFlag);
				
				}	// end "if (!determinantOKFlag)"
				
			if (gOperationCanceledFlag)
																					return (returnCode);
											
			}	// end "for (i=0; i<numberClasses; i++)"
			
		if (statEnhanceSpecsPtr->softThresholdCode == kUseFarthestPixelThreshold)
			GetMaximumClassDistances (statEnhanceSpecsPtr, classInfoPtr);
					
				// Check if user wants to exit.	
				
		if (gOperationCanceledFlag)
																					return (returnCode);										
		/*
		if (TickCount () >= gNextTime)
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																					return (returnCode);
		*/
		if (returnCode == 3)
			break;

		}    // end "for (iount=0; iount<=...->iterationMax; iount++)" 
     	
	if (returnCode == 3)
		continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																IDS_StatEnhance25, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
	
	return (returnCode);

}	// end "behzad_ModifyStatistics" 

		

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyEnhancedStatsToProject
//
//	Software purpose:	The purpose of this routine is to copy the enhanced
//							statistics back to the project.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1993
//	Revised By:			Larry L. Biehl			Date: 03/04/2001

void CopyEnhancedStatsToProject (
				ClassInfoPtr 						classInfoPtr)

{
	HChannelStatisticsPtr			modChanStatsPtr;
	HPClassNamesPtr					classNamesPtr;
	HCovarianceStatisticsPtr		modCovStatsPtr;
	SInt16*								classPtr;
	
	SInt32								numberChannels;
	
	UInt32								bytesNeeded,
											index,
											numberClasses;
	
	SInt16								channel,
											classStorage,
											classWeightsIndex,
											statClassNumber;
	
	Boolean								continueFlag;
										

			// Initialize local variables.												
			
	classPtr = (SInt16*)GetHandlePointer (gStatEnhanceSpecsPtr->classHandle);
														
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	numberClasses = gStatEnhanceSpecsPtr->numberClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	ShowStatusDialogItemSet (kStatusTitle2);
	HideDialogItem (gStatusDialogPtr, IDC_Status22); 
	
	LoadDItemStringNumber (kEnhanceStatStrID, 
									IDS_StatEnhance11, 
									gStatusDialogPtr, 
									IDC_Status21, 
									(Str255*)gTextString); 
									
	DrawDialog (gStatusDialogPtr);
	
			// Make certain that memory is allocated for enhanced statistics.	
			
	continueFlag = SetupModifiedStatsMemory (numberClasses);
	
	if (continueFlag)
		{		
				// Copy the enhanced covariance matrices back to the project 	
				// class statistics.															
		
		bytesNeeded = gProjectInfoPtr->numberCovarianceEntries * sizeof (double);
		for (index=0; index<numberClasses; index++)
			{
					// Get the class storage number.										
					
			statClassNumber = classPtr[index] - 1;
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			classNamesPtr[classStorage].modifiedStatsStorage = (SInt16)index;
			
					// Get pointers to the memory for the first order  			
					// and second order class statistics.								
					
			GetProjectStatisticsPointers (kClassStatsOnly,
													classStorage, 
													NULL, 
													NULL,
													&modChanStatsPtr,
													&modCovStatsPtr);
		
					// Copy the new input triangular matrix.							
	
			BlockMoveData (classInfoPtr[index].covariancePtr, 
								modCovStatsPtr,
								bytesNeeded);
						
					// Copy new mean data into the class structure.					
			
			for (channel=0; channel<numberChannels; channel++)
				{
				modChanStatsPtr->mean = classInfoPtr[index].meanPtr[channel];
				modChanStatsPtr->sum = -1;
				modChanStatsPtr->minimum = DBL_MAX;
				modChanStatsPtr->maximum = -DBL_MAX;
				
				if (*modCovStatsPtr > 0)
					modChanStatsPtr->standardDev = sqrt (*modCovStatsPtr);
					
				else	// *modCovStatsPtr <= 0 
					modChanStatsPtr->standardDev = 0;
				
				modChanStatsPtr++;
				modCovStatsPtr += channel + 2;
				
				}	// end "for (channel=0; channel<..." 
			
			classNamesPtr[classStorage].modifiedStatsFlag = TRUE;
								
			}	// end "for (index=0; index<numberClasses; index++)" 
		
		if (gStatEnhanceSpecsPtr->covarianceStatsToUse != kEnhancedStats)
			{
			if (gProjectInfoPtr->covarianceStatsToUse != kEnhancedStats)
				gProjectInfoPtr->enhancedStatsOrigin = 
														gProjectInfoPtr->covarianceStatsToUse;
														
			else	// gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats
				gProjectInfoPtr->enhancedStatsOrigin = kOriginalStats;
				
			}	// end "if (...->covarianceStatsToUse != kEnhancedStats)"
			
		gProjectInfoPtr->enhancedStatsExistFlag = TRUE;
		gProjectInfoPtr->modifiedClassWeightSet = kUnequalWeights;
		
				// Set the covariance-to-use code for each class and the project;
				
		SetProjectCovarianceStatsToUse (kEnhancedStats);
	
				// Save new priori probabilities.											
				// Note that negative priors are used to indicate to the 			
				// user interface that these are unequal priors.						
				
		//classWeightsIndex = GetProjectClassWeightsIndex ();
 		classWeightsIndex = 1;
				
		for (index=0; index<numberClasses; index++)
			{
			statClassNumber = classPtr[index] - 1;
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			
			classNamesPtr[classStorage].priorWeights[classWeightsIndex] =
							(float)(-fabs (gStatEnhanceSpecsPtr->priorsPtr[index] * 100.));
									
			}	// end "for (index=0; index<numberClasses; index++)" 
	
				// Indicate that project information has changed.						
	 
		gProjectInfoPtr->changedFlag = TRUE;
		
		gUpdateProjectMenuItemsFlag = TRUE;
		
		}	// end "if (continueFlag)" 

}	// end "CopyEnhancedStatsToProject" 



#if defined multispec_mac                                                              
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawHardThresholdPopUp
//
//	Software purpose:	The purpose of this routine is to draw the hard threshold
//							popup menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1999
//	Revised By:			Larry L. Biehl			Date: 12/02/1999

pascal void DrawHardThresholdPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)
                             			
{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpHardThresholdMenu, 
								gHardThresholdSelection, 
								TRUE);
	
}	// end "DrawHardThresholdPopUp" 
#endif	// defined multispec_mac 



#if defined multispec_mac                                                              
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		pascal void DrawSoftThresholdPopUp
//
//	Software purpose:	The purpose of this routine is to draw the soft threshold
//							popup menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1999
//	Revised By:			Larry L. Biehl			Date: 12/02/1999

pascal void DrawSoftThresholdPopUp (
				DialogPtr							dialogPtr, 
				SInt16								itemNumber)
                             			
{
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpSoftThresholdMenu, 
								gSoftThresholdSelection, 
								TRUE);
	
}	// end "DrawSoftThresholdPopUp" 
#endif	// defined multispec_mac
											
		

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double Gaussian
//
//	Software purpose:	The purpose of this routine is to compute the
//							gaussian likelihood value for a given pixel for
//							input class statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			GetUnlabeledLogLikes in SStatEnh.cpp
//
//	Coded By:			Behzad M Shahshahani	Date: ??/??/1993
//	Revised By:			Larry L. Biehl			Date: 11/30/1999

double Gaussian (
				HDoublePtr   						dataValuesPtr, 
				HDoublePtr		 					invCovPtr, 
				HDoublePtr 							meanPtr, 
				HDoublePtr 							diffPtr,
				UInt32			   				numberFeatures)

{
	double								dTempValue,
											out;

	UInt32			 					i,
											j;

	
	for (i=0; i<numberFeatures; i++)
		{
		*diffPtr = *dataValuesPtr - *meanPtr;
		dataValuesPtr++;
		meanPtr++;
		diffPtr++;
		
		}	// end "for (i=0; i<numberFeatures; i++)" 
		
	diffPtr -= numberFeatures;

	out = 0.0;
	for (i=0; i<numberFeatures; i++)
		{
		dTempValue = 0.;
		
		for (j=0; j<i; j++)
			{
     		dTempValue += *invCovPtr * *diffPtr;
     		invCovPtr++;
     		diffPtr++;
     		
     		}	// end "for (j=0; j<i; j++)" 
     		
     	dTempValue += dTempValue;
		out += *diffPtr * (dTempValue + *invCovPtr * *diffPtr);
		invCovPtr++;
		
		diffPtr -= i;
     		
     	}	// end "for (i=0; i<numberFeatures; i++)" 
	/*
	if (out > outlierThreshold) 
																							return (FALSE);

	else	// out <= outlierThreshold

				// exp (-745) is the smallest number possible for the PPC for
				// doubles based on example tests.  This computes to be around 	
				// 10e-324.																			
				//																						
				// exp (-11400) is the smallest number possible for the 68K macs
				// for 'native' double based on example tests.  This computes to	
				// be around 10e-4951.															
				// Larry Biehl 2/27/95															
		{
		*typicalityWeightPtr = out;
		
		*logLikePtr = out + logDeterminant;
    																						return (TRUE);
    	
  		}	// end "else out <= outlierThreshold" 
	*/
	return (out);
	
}	// end "Gaussian"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLongestDistanceClass
//
//	Software purpose:	The purpose of this routine is to list the class
//							data for the requested channels to the output text
//							window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1999
//	Revised By:			Larry L. Biehl			Date: 12/29/2005	

SInt16 GetLongestDistanceClass (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				TypicalitySummaryPtr				classTypicalityPtr,
				HDoublePtr							labeledDataPtr,
				ClassInfoPtr						classInfoPtr,
				SInt16								classNumber)

{
	DistancesSummary					distancesSummary;
	
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classStorage,
											fieldCount,
											fieldNumber,
											numberOfFields;
	
	
			// Initialize local variables.													
			
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	distancesSummary.distancesList[0].distanceValue = -DBL_MAX;
	
	distancesSummary.freeIndex 				= 0;
											
	distancesSummary.endIndex 					= 0;
	distancesSummary.numberLabeledValues 	= 0;
	distancesSummary.startIndex 				= 0;
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[classNumber];
	
			// Continue only if the number of fields in the class is one or more.
	
	numberOfFields = classNamesPtr[classStorage].numberOfTrainFields;
		
	if (numberOfFields > 0)
		{
				// Set up status dialog. Class name, and number of						
				// training fields for the class.											
				
		LoadDItemString (gStatusDialogPtr, 
								IDC_Status6, 
								(Str255*)&classNamesPtr[classStorage].name);
		LoadDItemValue (gStatusDialogPtr, IDC_Status10, numberOfFields);
			
		fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
		fieldCount = 1;
		while (fieldNumber != -1)
			{
			fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
					// Make certain that field is training field.						
					
			if (fieldIdentPtr->fieldType & kTrainingType)
				{
						// Set up field count in status dialog.							
				
				LoadDItemValue (gStatusDialogPtr, IDC_Status8, (SInt32)fieldCount);
					
						// List the requested data for the field.							
					
				if (GetLongestDistanceField (fileIOInstructionsPtr,
														&distancesSummary,
														labeledDataPtr,
														classInfoPtr, 
														classNumber+1, 
														fieldNumber) == 0)
																									return (0);
				
				fieldCount++;
								
				}	// end "if (fieldIdentPtr->fieldType & kTrainingType)" 
				
			fieldNumber = fieldIdentPtr->nextField;
			
			}	// end "while (fieldNumber != -1)" 
			
		}	// end "if (numberOfFields > 0)" 
			
	classTypicalityPtr->distance = 
			distancesSummary.distancesList[distancesSummary.startIndex].distanceValue;
		
	classTypicalityPtr->numberSamples = distancesSummary.numberLabeledValues;
	
			// Indicate that routine completed normally.									
			
	return (1);
		
}	// end "GetLongestDistanceClass"     



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 GetLongestDistanceField
//
//	Software purpose:	The purpose of this routine is to transfer the
//							field data for the requested channels to the output
//							text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1999
//	Revised By:			Larry L. Biehl			Date: 12/29/2005

SInt16 GetLongestDistanceField (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				DistancesSummaryPtr				distancesSummaryPtr,
				HDoublePtr							labeledDataPtr,
				ClassInfoPtr						classInfoPtr,
				SInt16								classNumberCode, 
				SInt16								fieldNumber)

{
	double								distance;
	
	HDoublePtr							buffer8BytePtr;
	
	HUCharPtr							inputBufferPtr,
											inputBufferPtr2;
											
	HUInt16Ptr							maskBufferPtr;
	
	Point									point;
	RgnHandle							rgnHandle;
	
	SInt32								line,
											lineEnd,
											lineStart;
								
	UInt32								charIndex,
											column,
											columnEnd,
											columnStart,
											lineCount,
											numberChannels;
	
	SInt16								errCode,
											pointType;
											
	UInt16								maskRequestValue;
	
	Boolean								continueFlag,
											includePixelFlag;
	
	
			// Make certain that input values make sense									
			
	if (fieldNumber < -1)
																								return (0);
	if (gProjectInfoPtr != NULL &&
							fieldNumber >= gProjectInfoPtr->numberStorageFields)
																								return (0);
	if (fileIOInstructionsPtr->fileInfoPtr == NULL)													
																								return (0);
																						
			// Exit if this is a cluster type field.										
	
	pointType = kRectangleType;		
	if (fieldNumber > -1 && gProjectInfoPtr != NULL)
		pointType = gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType;
		
	if (pointType == kClusterType)
																								return (1);
	
			// Initialize local variables.													
	
	continueFlag = 	TRUE;		
	inputBufferPtr = 	(HUCharPtr)gInputBufferPtr;
	inputBufferPtr2 = (HUCharPtr)gOutputBufferPtr;
	numberChannels = 	gProjectInfoPtr->numberStatisticsChannels;
	
			// Determine if field is described by a polygon or a rectangle.  If	
			// the field is described by a polygon then create a region for the	
			// field.	
			
	GetFieldBoundary (gProjectInfoPtr, &gAreaDescription, fieldNumber);
		
	if (gAreaDescription.columnStart == 0)
																								return (1);
										
	rgnHandle = 	gAreaDescription.rgnHandle;
	lineStart = 	gAreaDescription.lineStart;
	lineEnd = 		gAreaDescription.lineEnd;
	columnStart = 	gAreaDescription.columnStart;
	columnEnd = 	gAreaDescription.columnEnd;
	
			// Load the total number of lines into the status dialog.				
				
	LoadDItemValue (gStatusDialogPtr, 
							IDC_Status20, 
							lineEnd-lineStart+1);
	lineCount = 0;
					
			// Intialize the nextTime variable to indicate when the next check	
			// should occur for a command-.													
			
	gNextTime = TickCount ();
	gNextStatusTime = TickCount ();
			
			// Load some of the File IO Instructions structure that pertain
			// to the specific area being used.
			
	errCode = SetUpFileIOInstructions (fileIOInstructionsPtr,
													&gAreaDescription,
													lineStart,
													lineEnd,
													1,
													columnStart,
													columnEnd,
													1, 
													(UInt16)numberChannels,
													gProjectInfoPtr->channelsPtr,
													kDetermineSpecialBILFlag);
													
	maskRequestValue = (UInt16)fileIOInstructionsPtr->maskValueRequest;
									
			// Loop through the lines for the field.										
	
	for (line=lineStart; line<=lineEnd; line++)
		{
		if (!continueFlag)
			break;
					
				// Load the line count into the status dialog.							
		
		lineCount++;	
		if (TickCount () >= gNextStatusTime)
			{
			LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
			
			}	// end "if (TickCount () >= gNextStatusTime)" 
		
		point.v = (SInt16)line;
			
				// Get data for the requested channels for the line of image 		
				// data.  Return if there is a file IO error.							
			 
		errCode = GetLineOfData (fileIOInstructionsPtr,
											line, 
											columnStart,
											columnEnd,
											1,
											(unsigned char*)inputBufferPtr,
											(unsigned char*)inputBufferPtr2);
								
		if (errCode < noErr)
			{	
			CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
			CloseUpAreaDescription (&gAreaDescription);												
																								return (0);
			
			}	// end "if (errCode < noErr)"
			
		if (errCode != kSkipLine)
			{
		   buffer8BytePtr = (HDoublePtr)inputBufferPtr2;
			
			if (fileIOInstructionsPtr->maskBufferPtr != NULL)
				maskBufferPtr = &fileIOInstructionsPtr->maskBufferPtr[
														fileIOInstructionsPtr->maskColumnStart];
		   
			for (column=columnStart; column<=columnEnd; column++)
				{
				charIndex = 0;
				point.h = (SInt16)column;
			
				includePixelFlag = FALSE;
				if (pointType == kRectangleType)
					includePixelFlag = TRUE;
					
				else if (pointType == kPolygonType && PtInRgn (point, rgnHandle))
					includePixelFlag = TRUE;
				
				else if (pointType == kMaskType && *maskBufferPtr == maskRequestValue)
					includePixelFlag = TRUE;
					
				if (includePixelFlag)
					{
					distance = Gaussian (buffer8BytePtr,
												classInfoPtr->inversePtr,
												classInfoPtr->meanPtr, 
												gInverseMatrixMemory.pivotPtr,
												numberChannels);
									
					UpdateLabeledValueList (distancesSummaryPtr,
														distance,
														buffer8BytePtr, 
														labeledDataPtr,
														numberChannels);
				
					}	// end "if (includePixelFlag)"
					
				buffer8BytePtr += numberChannels;
			
				if (!continueFlag)
					break;
					
				if (pointType == kMaskType)
					maskBufferPtr++;
						
				} 		// end "for (column=columnStart; column<=..."
					
			}	// end "if (errCode != kSkipLine)"
			
				// Exit routine if user has "command period" down							
			
		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				continueFlag = FALSE;
				
			}	// end "if (TickCount () >= gNextTime)" 
			
		if (!continueFlag)
			break;
			
		if (pointType == kMaskType)
			fileIOInstructionsPtr->maskBufferPtr += 
												fileIOInstructionsPtr->numberMaskColumnsPerLine;
	      
		}	// end "for (line=lineStart; line<=lineEnd; line+=...)" 
		
	LoadDItemValue (gStatusDialogPtr, IDC_Status18, lineCount);
		
			// Close up any File IO Instructions structure that pertain to the 
			// specific area used for the list data.
			
	CloseUpFileIOInstructions (fileIOInstructionsPtr, &gAreaDescription);
	
	CloseUpAreaDescription (&gAreaDescription);
		
	return ((SInt16)continueFlag);
		
}	// end "GetLongestDistanceField" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetLongestDistanceProject
//
//	Software purpose:	The purpose of this routine is to list the
//							data for the requested channels, classes and fields
//							to the output text window or disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1999
//	Revised By:			Larry L. Biehl			Date: 12/29/2005

Boolean GetLongestDistanceProject (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				ClassInfoPtr						classInfoPtr)

{
	TypicalitySummaryPtr				classTypicalityPtr;
	HDoublePtr							labeledDataPtr;
	SInt16*								classPtr;
	
	UInt32								classIndex,
											numberChannels,
											numberClasses;
	
	SInt16								classNumber;
	
	Boolean								returnFlag;
	
	
			// Check if the soft threshold is to be used. If not just return.
			
	if (statEnhanceSpecsPtr->softThresholdCode == kUseNoThreshold)
																							return (TRUE);
																							
			// Initialize local variables.													
			
	numberClasses = statEnhanceSpecsPtr->numberClasses;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	returnFlag = TRUE;
	classTypicalityPtr = statEnhanceSpecsPtr->classTypicalityPtr;
	labeledDataPtr = statEnhanceSpecsPtr->labeledDataPtr;
	
			// Continue only if number of classes is one or more.						
	
	if (numberClasses > 0)
		{				
		classPtr = (SInt16*)GetHandlePointer (statEnhanceSpecsPtr->classHandle);
														
				// Set up status dialog.  Load in number of classes.					
				
		LoadDItemValue (gStatusDialogPtr, IDC_Status5, (SInt32)numberClasses);
						
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
			if (statEnhanceSpecsPtr->softThresholdCode == kUseFarthestPixelThreshold)
				{
						// Set up status dialog.  Load in class count.						
					
				LoadDItemValue (gStatusDialogPtr, IDC_Status3, (SInt32)classIndex+1);
				
				classNumber = classPtr[classIndex] - 1;
				
						// Transfer data for the class.											
							
				if (GetLongestDistanceClass (fileIOInstructionsPtr,
														statEnhanceSpecsPtr,
														&classTypicalityPtr[classIndex],
														&labeledDataPtr[classIndex*5*numberChannels],
														&classInfoPtr[classIndex],
														classNumber) == 0)
					{
					returnFlag = FALSE;
					break;
					
					}	// end "if (GetLongestDistanceClass (..."
					
				}	// end "if (...->softThresholdCode == kUseFarthestPixel)" 
				
			else	// ...->softThresholdCode != kUseFarthestPixel
				{
				classTypicalityPtr[classIndex].distance = 
														statEnhanceSpecsPtr->softChiChiThreshold;
													
				classTypicalityPtr[classIndex].numberSamples = 0;
				
				}	// end "else ...->softThresholdCode != kUseFarthestPixel"
			
			}	// end "for (classIndex=0; ... 
			
		}	// end "if (numberClasses > 0)" 	
	
			// Indicate that routine completed normally.									
			
	return (returnFlag);
		
}	// end "GetLongestDistanceProject"  
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetMaximumClassDistances
//
//	Software purpose:	The purpose of this routine is to compute the log like
//							values for the unlabeled samples.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Behzad M Shahshahani	Date: 11/30/1999
//	Revised By:			Larry L. Biehl			Date: 12/29/2005

void GetMaximumClassDistances (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr, 
				ClassInfoPtr 						classInfoPtr)

{
	double								distance;         
	                                                                        
	HDoublePtr							labeledDataPtr;                             
//	HSInt32Ptr							labeledDataPtr;         
	TypicalitySummary*				classTypicalityPtr; 
					
	UInt32			 					i,
											j,
											numberChannels,
											numberClasses,
											numberSamples;
	

	numberClasses = statEnhanceSpecsPtr->numberClasses;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
				
	classTypicalityPtr = statEnhanceSpecsPtr->classTypicalityPtr;
	labeledDataPtr = statEnhanceSpecsPtr->labeledDataPtr;
      
   for (i=0; i<numberClasses; i++) 
   	{
   	classTypicalityPtr[i].distance = DBL_MIN;
   	numberSamples = classTypicalityPtr[i].numberSamples;
   	
		for (j=0; j<numberSamples; j++) 
      	{
         distance = Gaussian (labeledDataPtr,
            							classInfoPtr[i].inversePtr,
            							classInfoPtr[i].meanPtr,
            							gInverseMatrixMemory.pivotPtr,
            							numberChannels);
            							
         classTypicalityPtr[i].distance = MAX (classTypicalityPtr[i].distance, 
         													distance);
         
     		labeledDataPtr += numberChannels;
         
			}	// end "for (j=0; j<numberSamples; j++)"
			
		classTypicalityPtr[i].distance = classTypicalityPtr[i].distance;
		
		if (numberSamples < 5)
     		labeledDataPtr += (5-numberSamples) * numberChannels;
               
      }	// end "for (i=0; i<numberClasses; i++)" 
			
			// Allow the user to cancel.											
	
	if (TickCount () >= gNextTime)
		CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask);

}	// end "GetMaximumClassDistances" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetStatisticsEnhancePixelInterval
//
//	Software purpose:	The purpose of this routine is to determine a default
//							pixel interval to use for unlabeled samples that will
//							use as many pixels as memory will reasonably allow.
//		
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:  Default pixel interval to use
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/03/1994
//	Revised By:			Larry L. Biehl			Date: 08/18/2010

UInt32 GetStatisticsEnhancePixelInterval (void)

{
	double								ratio;
	
	SInt16*								classPtr;
	
	SInt64								numberPixels;
											
	UInt32								bytesNeeded,
											interval,
											lContBlock,
											memoryLeft,
											numberChannels,
											numberClasses,
//											numberLPixels,
											numberULPixels;
											
			
			// Initialize local variables.
			
	numberClasses = gStatEnhanceSpecsPtr->numberClasses;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;	
	classPtr = (SInt16*)GetHandlePointer (gStatEnhanceSpecsPtr->classHandle);
	
			// Determine memory for the structure which contains pointers to the 	
			// class statistics and labeled data values.
			
			// Memory for classInfo structure.										

	bytesNeeded = numberClasses * sizeof (ClassInfo);
			
	bytesNeeded += (2 * gProjectInfoPtr->numberCovarianceEntries + numberChannels) * 
																	numberClasses * sizeof (double);
		
	if (gStatEnhanceSpecsPtr->softThresholdCode != kUseNoThreshold)
		{
				// Memory for soft threshold value to use for each class.
				
		bytesNeeded += numberClasses * sizeof (TypicalitySummary);
		
		if (gStatEnhanceSpecsPtr->softThresholdCode == kUseFarthestPixelThreshold)
					// Memory for some (5) of the labeled samples.
			bytesNeeded += numberClasses * 5 * numberChannels * sizeof (SInt32);
				
		}	// end "if (...->softThresholdCode != kNoUseThreshold)"
																
			// Memory for "totalLogLikePtr".
								
	bytesNeeded += 200 * sizeof (double);
	
			// Memory for priors and unlabeled total vectors, "priorsPtr", "totalPtr".
	
	bytesNeeded += 2 * numberClasses * sizeof (double);
	
			// Memory for "classThresholdFlagPtr".
	
	bytesNeeded += numberClasses * sizeof (Boolean);
	
			// Memory for structure for labeled mean statistics for a class,
			// "gTempChannelStatsPtr".
	
	bytesNeeded += numberChannels * sizeof (ChannelStatistics);
	
			// Memory "gInverseMatrixMemory".
			
	bytesNeeded += numberChannels * numberChannels * sizeof (double);
	bytesNeeded += numberChannels * sizeof (double);
	bytesNeeded += 3 * numberChannels * sizeof (SInt16);
																			
			// Get the amount of memory available.	
	
	MGetFreeMemory (&lContBlock);
	
	if (lContBlock > (bytesNeeded + kMinFreeMemory))
		{												
		memoryLeft = lContBlock - (bytesNeeded + kMinFreeMemory);
		
				// Get the number of bytes of memory required for each unlabeled
				// pixel.
				
				// First is the bytes required for the unlabeled data values, i.e.
				// "unlabelDataPtr".
				
		bytesNeeded = numberChannels * sizeof (double);
			
				// Now add the bytes required for "likePtr".
				
		bytesNeeded += numberClasses * sizeof (double);
				
				// Now add the bytes required for "totalLikePtr".
				
		if (gStatEnhanceSpecsPtr->softThresholdCode != kUseNoThreshold)
			bytesNeeded += sizeof (double);
				
				// Now add the bytes required for the soft threshold, 
				// "typicalityWeightPtr".
				
		if (gStatEnhanceSpecsPtr->softThresholdCode != kUseNoThreshold)
			bytesNeeded += numberClasses * sizeof (double);
	
		numberULPixels = memoryLeft / bytesNeeded;
		
				// The default is for the number of unlabeled pixels to be not a lot
				// more than the number labeled samples.
		
		//numberLPixels = GetTotalNumberTrainPixels (classPtr, numberClasses);
		//numberULPixels = MIN (numberULPixels, numberLPixels);
		
				// 3.08.2001
				// Limiting the number of unlabeled pixels did not work well when the
				// number of training pixels was small. This next approach is to allow
				// the number of unlabel training pixels to be many as 10 times the number 
				// of channels times the number classes or the number of training 
				// pixels if that is more, if memory will allow.
				
		//numberLPixels = MAX (numberLPixels, 10*numberClasses*numberChannels);
		//numberULPixels = MIN (numberULPixels, numberLPixels);
																			
				// Get total number of pixels in the image.							

		numberPixels = GetNumberPixelsInArea (gStatEnhanceSpecsPtr->lineStart,
															gStatEnhanceSpecsPtr->lineEnd,
															1,
															gStatEnhanceSpecsPtr->columnStart,
															gStatEnhanceSpecsPtr->columnEnd,
															1,
															kDefaultAlgorithm);
			
		ratio = (double)numberPixels / numberULPixels;
		interval = (UInt32)sqrt (ratio);
		
		if (interval * interval < ratio)
			interval++;
		
		}	// end "if (memoryLeft > 0)" 
		
	else	// if (memoryLeft <= 0)
				// There is not enough memory left to even run statistics enhancement.
				// Just return the interval equal to the number of columns in the 
				// selected area. 
		interval = (UInt32)(gStatEnhanceSpecsPtr->columnEnd -
															gStatEnhanceSpecsPtr->columnStart + 1);
								
	interval = MAX (interval, 1);
								
	interval = MIN (interval, (UInt32)(gStatEnhanceSpecsPtr->columnEnd -
															gStatEnhanceSpecsPtr->columnStart + 1));
												
	return (interval);
	
}	// end "GetStatisticsEnhancePixelInterval" 


		
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetTotalNumberTrainPixels
//
//	Software purpose:	The purpose of this routine is to compute the total
//							number of selected training pixels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/11/1994
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

SInt64 GetTotalNumberTrainPixels (
				SInt16*								classPtr,
				UInt32								numberClasses)

{
	SInt64								numberPixels;
	
	UInt32								classStorage,
											index,
											statClassNumber;


	numberPixels = 0;	
	numberClasses = MIN (numberClasses, gProjectInfoPtr->numberStatisticsClasses);
		
	for (index=0; index<numberClasses; index++)
		{
		if (classPtr != NULL)
			statClassNumber = classPtr[index] - 1;
			
		else	// classPtr == NULL 
			statClassNumber = index;
			
		classStorage = gProjectInfoPtr->storageClass[statClassNumber];
			
		numberPixels += 
					gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
		
		}	// end "for (index=0; index<..." 
		
	return (numberPixels);
	
}	// end "GetTotalNumberTrainPixels" 
	


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		UInt32 GetUnlabeledLogLikes
//
//	Software purpose:	The purpose of this routine is to compute the log like
//							values for the unlabeled samples.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Behzad M Shahshahani	Date: ??/??/1993
//	Revised By:			Larry L. Biehl			Date: 12/15/2010

UInt32 GetUnlabeledLogLikes (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr, 
				ClassInfoPtr 						classInfoPtr,
				SInt32	 							iount)

{
	double								distance,
											minClassDistance,
											threshold;
											
	Boolean*								classThresholdFlagPtr;
                                                             
	HDoublePtr							dataValuePtr,
											likePostPtr,		// Bezhad's 'p' & 'post' 
											priorsPtr,				// Bezhad's 'alpha'
											totalLikePtr,			// Bezhad's 'pr' 		
											typicalityWeightPtr,	// Saldju's 'w'
											xloglPtr;				// Bezhad's 'xlogl' 
	                                                                         
	SInt64								numberUnlabeled;
					
	UInt32			 					i,
											k,
											numberChannels,
											numberClasses,
											unLabeledCount;
	

	xloglPtr = statEnhanceSpecsPtr->totalLogLikePtr;
	priorsPtr = statEnhanceSpecsPtr->priorsPtr;
	classThresholdFlagPtr = statEnhanceSpecsPtr->classThresholdFlagPtr;
	numberUnlabeled = statEnhanceSpecsPtr->numberUnlabeled;
	numberClasses = statEnhanceSpecsPtr->numberClasses;
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	
	threshold = statEnhanceSpecsPtr->hardChiChiThreshold;
	if (statEnhanceSpecsPtr->hardThresholdCode == kUseNoThreshold)
		threshold = DBL_MAX;
				
   unLabeledCount = 0;
	dataValuePtr = statEnhanceSpecsPtr->unlabelDataPtr;
	totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;
	likePostPtr =  statEnhanceSpecsPtr->likePtr;
	
	typicalityWeightPtr = statEnhanceSpecsPtr->typicalityWeightPtr;
	if (statEnhanceSpecsPtr->softThresholdCode == kUseNoThreshold)
		typicalityWeightPtr = likePostPtr;
      
   for (k=0; k<numberUnlabeled; k++) 
   	{
      if (*totalLikePtr >= 0.)
      	{
      	minClassDistance = DBL_MAX;
      	
         *totalLikePtr = 0.0;
			
         for (i=0; i<numberClasses; i++) 
         	{
            distance = Gaussian (dataValuePtr,
											classInfoPtr[i].inversePtr,
											classInfoPtr[i].meanPtr,
											gInverseMatrixMemory.pivotPtr,
											numberChannels);
						            							
				if (distance <= threshold)
					{
							// exp (-745) is the smallest number possible for the PPC for
							// doubles based on example tests.  This computes to be around 	
							// 10e-324.																			
							//																						
							// exp (-11400) is the smallest number possible for the 68K macs
							// for 'native' double based on example tests.  This computes to	
							// be around 10e-4951.															
							// Larry Biehl 2/27/95			
				
					classThresholdFlagPtr[i] = TRUE;
					
	        		*typicalityWeightPtr = distance;
					*likePostPtr = distance + classInfoPtr[i].logDeterminant;
					
	            		// The minClassDistance value here will be used later to
	            		// "normalize" the likelihood value to a range where numerical
	            		// problems will be reduced.
            									
          		if (priorsPtr[i] > 0)
            		minClassDistance = MIN (*likePostPtr, minClassDistance);
            	
            	}	// end "if (distance <= threshold)"
            	
            else	// distance > threshold
					classThresholdFlagPtr[i] = FALSE;
               
            typicalityWeightPtr++;
            likePostPtr++;
               
          	}	// end "for (i=0; i<numberClasses; i++)"
          	
          		// Adjust the gaussian exponent values down so that the likelihood
          		// value can be computed with the fewest numeric problems.
          		
    		likePostPtr -= numberClasses;
    		typicalityWeightPtr -= numberClasses;
          		
         for (i=0; i<numberClasses; i++) 
         	{	
          	if (classThresholdFlagPtr[i] && priorsPtr[i] > 0)
           			{
	          		*likePostPtr -= minClassDistance; 
	            								
						*likePostPtr = exp (-0.5 * *likePostPtr);
						
	              	*likePostPtr *= priorsPtr[i];
	               *totalLikePtr += *likePostPtr;
	               
	               }	// end "if (priorsPtr[i] > 0)"
	               
            else	// !classThresholdFlagPtr[i] || priorsPtr[i] <= 0
            			// Make sure that *likePostPtr is zero.
            	{
            	*typicalityWeightPtr = 0;
            	*likePostPtr = 0;
            	
            	}	// end " else !classThresholdFlagPtr[i] || priorsPtr[i] <= 0"
               
            typicalityWeightPtr++;
            likePostPtr++;
               
          	}	// end "for (i=0; i<numberClasses; i++)"
			
					// Allow the user to cancel.											
			
			if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (0);
					
				}	// end "if (TickCount () >= gNextTime)"
               
        	if (*totalLikePtr > 0.0) 
        		{
            xloglPtr[iount] += log (*totalLikePtr) - .5*minClassDistance;
           	unLabeledCount++;
            
            }	// end "if (*totalLikePtr > 0.0)" 
        		
        	}	// end "if (*totalLikePtr >= 0.)" 
        		
  		else	// *totalLikePtr < 0. 
  			{
         typicalityWeightPtr += numberClasses;
      	likePostPtr += numberClasses;
         
         }	// end "else *totalLikePtr < 0."
            
      dataValuePtr += numberChannels;
      totalLikePtr++;
            
   	}	// end "for (k=0; k<numberUnlabeled; k++)" 
        	
	return (unLabeledCount);

}	// end "GetUnlabeledLogLikes" 
								


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean InitializeStatisticsVariables
//
//	Software purpose:	The purpose of this routine is to modify the class
//							statistics using unlabeled samples.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Behzad M Shahshahani	Date: 11/30/1999
//	Revised By:			Larry L. Biehl			Date: 12/15/2010

Boolean InitializeStatisticsVariables (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr, 
				ClassInfoPtr						classInfoPtr,
				FileIOInstructionsPtr			fileIOInstructionsPtr)

{
	
	HDoublePtr							totalLikePtr,			// Bezhad's 'pr' 		
											xloglPtr;				// Bezhad's 'xlogl'

	SInt64								numberUnlabeled;
											
	UInt32								k,
											unLabeledCount;	
	
	Boolean								continueFlag;
	

			// Initialize local variables.												
			
	continueFlag = TRUE;
	unLabeledCount = 0;
														
	xloglPtr = statEnhanceSpecsPtr->totalLogLikePtr;
	numberUnlabeled = statEnhanceSpecsPtr->numberUnlabeled;
							
			// Initialize some global variables.										
			
	gNextTime = TickCount ();
					
			// Initialize the total likelihood vector.  This vector will also	
			// be used to determine whether the unlabeled pixel has been		
			// thresholded after the first pass.										
		
	totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;
	for (k=0; k<numberUnlabeled; k++)
    	{
    	*totalLikePtr = 0.;
    	totalLikePtr++;
    	
    	}	// end "for (k=0; k<numberUnlabeled; k++)" 
	
	if (statEnhanceSpecsPtr->covarianceStatsToUse == kEnhancedStats)
		{
				// Determine the unlabeled samples to be thresholded using		
				// the original statistics.												

		continueFlag = LoadStatEnhanceClassStatistics (statEnhanceSpecsPtr,
																		classInfoPtr, 
																		kNoStatisticsUsed);
		
      if (continueFlag)
      	{
      	xloglPtr[0] = 0.0;
      
			unLabeledCount = GetUnlabeledLogLikes (
															statEnhanceSpecsPtr, classInfoPtr, 0);
												
			continueFlag = (!gOperationCanceledFlag);
        		
        	}	// end "if (continueFlag)" 
        	
      if (continueFlag)
      	{				
      			// Threshold those unlabeled pixels which were more than		
      			// threshold value from all classes.  They will not be 		
      			// considered any more.													
      		
			totalLikePtr = statEnhanceSpecsPtr->totalLikePtr;
    		for (k=0; k<numberUnlabeled; k++)
    			{
        		if (*totalLikePtr == 0.0)
        			*totalLikePtr = -1;
        		
        		totalLikePtr++;
        		
        		}	// end "for (k=0; k<numberUnlabeled; k++)" 
        		
        	}	// end "if (continueFlag)" 
		
		}	// end "if (...->covarianceStatsToUse == kEnhancedStats)" 
												
	statEnhanceSpecsPtr->unlabeledCount = unLabeledCount;
		
			// Load the initial statistics.												

	if (continueFlag)
		continueFlag = LoadStatEnhanceClassStatistics (
														statEnhanceSpecsPtr,
														classInfoPtr, 
														statEnhanceSpecsPtr->covarianceStatsToUse);
	
			// Load labeled data values.	

	if (continueFlag && statEnhanceSpecsPtr->softThresholdCode != kUseNoThreshold)
		{
		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance32, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
										
		continueFlag = GetLongestDistanceProject (fileIOInstructionsPtr,
																statEnhanceSpecsPtr,
																classInfoPtr);
																	
		}	// end "if (continueFlag && ..."
											
	return (continueFlag);

}	// end "InitializeStatisticsVariables" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ListStatEnhanceSpecifications
//
//	Software purpose:	The purpose of this routine is to list the specifications
//							being used for this run of Enhance Statistics in the output
//							text window.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	True if listing went okay
//							False if listing did not (such as ran out of memory).
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1999
//	Revised By:			Larry L. Biehl			Date: 03/12/2012

Boolean ListStatEnhanceSpecifications (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr)

{
	CMFileStream*						resultsFileStreamPtr;
	UInt16*								classPtr;
	TypicalitySummaryPtr				classTypicalityPtr;

	UInt32								index;
	
	SInt16								classWeightsIndex,
											statClassNumber;
	
	Boolean								continueFlag;
	
	
			// Initialize local variables.
			
	resultsFileStreamPtr = NULL;
	continueFlag = TRUE;									
			
	classPtr = (UInt16*)GetHandlePointer (statEnhanceSpecsPtr->classHandle);
	
			// List the soft outlier threshold and threshold value if used.
					
	continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
															IDS_StatEnhance29,
															(unsigned char*)gTextString, 
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															continueFlag);
	
	if (gStatEnhanceSpecsPtr->softThresholdCode == kUseNoThreshold)
		continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																IDS_StatEnhance30,
																(unsigned char*)gTextString,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);		
	
	else if (gStatEnhanceSpecsPtr->softThresholdCode != kUseFarthestPixelThreshold)				
		continueFlag = ListSpecifiedStringNumber (
														kEnhanceStatStrID,
														IDS_StatEnhance1,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														statEnhanceSpecsPtr->softChiChiThreshold, 
														continueFlag);		
	
	else 	// statEnhanceSpecsPtr->softThresholdCode == kUseFarthestPixelThreshold)
		{
		continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																IDS_StatEnhance31,
																(unsigned char*)gTextString, 
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);
		
		for (index=0; index<statEnhanceSpecsPtr->numberClasses; index++)
			{
			statClassNumber = classPtr[index];
			classTypicalityPtr = statEnhanceSpecsPtr->classTypicalityPtr;
				
			sprintf ((char*)gTextString,
							"      Chi*Chi threshold value for class %d = %g%s",
							(int)statClassNumber,
							classTypicalityPtr[index].distance,
							gEndOfLine);
						
			continueFlag = OutputString (resultsFileStreamPtr,
													(char*)gTextString, 
													0, 
													gOutputForce1Code, 
													continueFlag);
			
			}	// end "for (index=0; index...)"	
					
		}	// end "else ...->softThresholdCode == kUseFarthestPixelThreshold"
	
	if (statEnhanceSpecsPtr->softThresholdCode == kUsePercentThreshold)
		continueFlag = ListSpecifiedStringNumber (
														kEnhanceStatStrID,
														IDS_StatEnhance2,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														statEnhanceSpecsPtr->softPercentThreshold, 
														continueFlag);
	
			// List the hard outlier threshold and threshold value if used.
					
	continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
															IDS_StatEnhance28,
															(unsigned char*)gTextString,
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															continueFlag);
	
	if (gStatEnhanceSpecsPtr->hardThresholdCode == kUseNoThreshold)
		continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																IDS_StatEnhance30,
																(unsigned char*)gTextString,
																resultsFileStreamPtr, 
																gOutputForce1Code, 
																continueFlag);															
	

	else if (gStatEnhanceSpecsPtr->hardThresholdCode != kUseFarthestPixelThreshold)
		continueFlag = ListSpecifiedStringNumber (
														kEnhanceStatStrID,
														IDS_StatEnhance1,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														statEnhanceSpecsPtr->hardChiChiThreshold, 
														continueFlag);
	
	if (gStatEnhanceSpecsPtr->hardThresholdCode == kUsePercentThreshold)
		continueFlag = ListSpecifiedStringNumber (
														kEnhanceStatStrID,
														IDS_StatEnhance2,
														resultsFileStreamPtr, 
														gOutputForce1Code, 
														statEnhanceSpecsPtr->hardPercentThreshold, 
														continueFlag); 
	
			// List the labeled data weighting factor if used.			

	if (statEnhanceSpecsPtr->weightLabeledFlag)
		index = IDS_StatEnhance3;
				
	else	// !...weightLabeledFlag 
		index = IDS_StatEnhance4;
				
	continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
															(SInt16)index,
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															statEnhanceSpecsPtr->labelWeight, 
															continueFlag);
	
			// List the maximum number of interations allowed.			
											
	continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
															IDS_StatEnhance5,
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															statEnhanceSpecsPtr->iterationMax, 
															continueFlag);
	
			// List the factors used for determining whether the		
			// results have saturated sufficiently.						
			
	continueFlag = GetSpecifiedStringNumber (kEnhanceStatStrID,
															IDS_StatEnhance6, 
															gTextString2, 
															continueFlag);
	
	sprintf ((char*)gTextString,
				(char*)&gTextString2[1],
				statEnhanceSpecsPtr->iterationStopLength,
				statEnhanceSpecsPtr->logLikeStopPercentChange);

	continueFlag = OutputString (resultsFileStreamPtr, 
											(char*)gTextString, 
											0, 
											gOutputForce1Code, 
											continueFlag); 
		
			// Insert a blank line.
		
	continueFlag = OutputString (resultsFileStreamPtr, 
											gEndOfLine, 
											gNumberOfEndOfLineCharacters, 
											gOutputForce1Code, 
											continueFlag);
	
				// List the channels used.											
	
	if (continueFlag)
		continueFlag = ListChannelsUsed (gImageFileInfoPtr,
													NULL,
													(SInt16*)gProjectInfoPtr->channelsPtr,
													gProjectInfoPtr->numberStatisticsChannels, 
													resultsFileStreamPtr,
													&gOutputForce1Code,
													kNoTransformation);
			
			// List the classes used.												
							
	classWeightsIndex = GetClassWeightsIndex (
			statEnhanceSpecsPtr->covarianceStatsToUse == kEnhancedStats,
			(statEnhanceSpecsPtr->weightSet == kEqualWeights));
						
	if (continueFlag)		
		continueFlag =  ListClassesUsed (NULL,
													gImageFileInfoPtr, 
													(SInt16*)classPtr, 
													kListNoSymbols,
													classWeightsIndex, 
													statEnhanceSpecsPtr->numberClasses, 
													resultsFileStreamPtr, 
													&gOutputForce1Code,
													FALSE);
				
	if (continueFlag)	
		continueFlag = CheckNumberOfPixelsInClass (
													statEnhanceSpecsPtr->numberClasses,
													(SInt16*)classPtr,
													gProjectInfoPtr->numberStatisticsChannels);
			
			// List the number of label samples										
														
	continueFlag = ListSpecifiedStringNumber (
													kEnhanceStatStrID, 
													IDS_StatEnhance17,
													NULL, 
													gOutputForce1Code, 
													(SInt32)statEnhanceSpecsPtr->numberLabeled,
													continueFlag); 
		
			// List the number of unlabel samples									
														
	continueFlag = ListSpecifiedStringNumber (
													kEnhanceStatStrID, 
													IDS_StatEnhance18,
													NULL, 
													gOutputForce1Code, 
													(SInt32)statEnhanceSpecsPtr->numberUnlabeled,
													continueFlag); 
										
			// List "  Output Information:"
			
	continueFlag = ListSpecifiedStringNumber (kSharedStrID,
															IDS_Shared8, 
															(unsigned char*)gTextString, 
															resultsFileStreamPtr, 
															gOutputForce1Code, 
															continueFlag);
	
	return (continueFlag); 
		
}	// end "ListStatEnhanceSpecifications" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadStatEnhanceClassStatistics
//
//	Software purpose:	The purpose of this routine is to load the requested
//							class statistics for the Enhance Statistics processor.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	True if statistics loaded okay.
//							False if statistics did not load okay.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/04/1994
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

Boolean LoadStatEnhanceClassStatistics (
				StatEnhanceSpecsPtr 				statEnhanceSpecsPtr, 
				ClassInfoPtr						classInfoPtr,
				SInt16								covarianceStatsToUse)

{
	double								totalProbability;
	
	HPClassNamesPtr					classNamesPtr;
										
	SInt16*								classPtr;
	
	UInt32								index,
											numberChannels,
											numberClasses,
											statClassNumber;
	
	SInt16								classStorage,
											weightsIndex;
	
	UInt16								localCovarianceStatsToUse;
	
	Boolean								continueFlag,
											determinantOKFlag;
										
			
			// Initialize local variables.													
										
	classPtr = (SInt16*)GetHandlePointer (gStatEnhanceSpecsPtr->classHandle);
														
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	numberClasses = gStatEnhanceSpecsPtr->numberClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	continueFlag = TRUE;
		
			// Load the class covariance matrix and mean vector. Be certain that the 
			// requested class statistics are used as the starting point. Use enhanced
			// statistics as the starting point if requested, otherwise use the
			// original or leave-one-out statistics depending on the class setting.
			// If the setting for a class is enhanced statistics then force to
			// the original statistics.
			
	if (covarianceStatsToUse == kEnhancedStats)
		localCovarianceStatsToUse = kEnhancedStats;
	
	for (index=0; index<numberClasses; index++)
		{
		statClassNumber = classPtr[index] - 1;
		classStorage = gProjectInfoPtr->storageClass[statClassNumber];
		
		if (covarianceStatsToUse != kEnhancedStats)
			{
			localCovarianceStatsToUse = 
						gProjectInfoPtr->classNamesPtr[classStorage].covarianceStatsToUse;
						
			if (localCovarianceStatsToUse == kEnhancedStats)
				localCovarianceStatsToUse = gProjectInfoPtr->enhancedStatsOrigin;
						
			if (localCovarianceStatsToUse != kOriginalStats &&
												localCovarianceStatsToUse != kLeaveOneOutStats)
				localCovarianceStatsToUse = kOriginalStats;
						
			}	// end "if (covarianceStatsToUse != kEnhancedStats)"
			
		GetClassCovarianceMatrix ((UInt16)numberChannels,
											gTempChannelStatsPtr, 
											classInfoPtr[index].covariancePtr, 
											NULL, 
											(UInt16)statClassNumber,
											kTriangleOutputMatrix,
											kMeanCovariance,
											localCovarianceStatsToUse);
		
				// Copy mean data into mean vector.										
			
		ReduceMeanVector (gTempChannelStatsPtr,
								classInfoPtr[index].meanPtr,
								(SInt16)numberChannels,
								NULL);
		
		}	// end "for (index=0; index<numberClasses; index++)" 
				
			// Get the inverse of the covariance matrices in lower triangular	
			// form.																				
		
	determinantOKFlag = TRUE;
		
	for (index=0; index<numberClasses; index++)
		{
		determinantOKFlag = InvertLowerTriangularMatrix (
																classInfoPtr[index].covariancePtr,
																classInfoPtr[index].inversePtr,
																(SInt16)numberChannels, 
																&classInfoPtr[index].determinant,
																&classInfoPtr[index].logDeterminant,
																kReturnMatrixInverse);
		
		if (!determinantOKFlag)
			{
			statClassNumber = classPtr[index] - 1;
			
			continueFlag = ListClassInformationMessage (kProjectStrID,
																		IDS_Project31,
																		NULL, 
																		gOutputForce1Code,
																		statClassNumber, 
																		continueFlag);
			
			}	// end "if (!determinantOKFlag)" 
			
		if (!continueFlag)
			break;
					
		}	// end "for (index=0; index<numberClasses; index++)" 
		
	if (!determinantOKFlag)
		{
		continueFlag = FALSE;		
		SysBeep (10);
		
		}	// end "if (!determinantOKFlag)"
	
			// Check if user has cancelled the operation.							
			
	if (continueFlag)
		continueFlag = !gOperationCanceledFlag;
		
			// Get the prior probabilities.												
			
	if (continueFlag)
		{
		weightsIndex = GetClassWeightsIndex (
											covarianceStatsToUse == kEnhancedStats,
											(statEnhanceSpecsPtr->weightSet == kEqualWeights));
									
		totalProbability =  GetTotalProbability (
												(UInt16*)classPtr, numberClasses, weightsIndex);
				
		for (index=0; index<numberClasses; index++)
			{
			statClassNumber = classPtr[index] - 1;
			
			statEnhanceSpecsPtr->priorsPtr[index] = GetClassWeightValue (
						(UInt16)statClassNumber, (UInt16)weightsIndex, totalProbability);
																		
			}	// end "for (index=0; index<numberClasses; index++)" 
		
		}	// end "if (continueFlag)" 
		
	return (continueFlag);
	
}	// end "LoadStatEnhanceClassStatistics" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean LoadStatisticsEnhanceSpecs
//
//	Software purpose:	The purpose of this routine is to load the Statistics 
//							Enhancement Specification structure with the default 
//							set of parameters.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			StatisticsEnhanceControl
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1993
//	Revised By:			Larry L. Biehl			Date: 09/20/2000

Boolean LoadStatisticsEnhanceSpecs (void)	

{
	SInt16*								classPtr;
	
	Handle								windowInfoHandle;
	UInt32								bytesNeeded;
	
	Boolean								changedFlag,
											continueFlag,
											firstTimeFlag;
	
	
			// Check important input variable.												
			
	if (gProjectInfoPtr == NULL)
																							return (FALSE);
	
			// Assume routine will work okay.  If there isn't enough memory		
			// then the flag will be changed.												
			
	continueFlag = TRUE;
	firstTimeFlag = FALSE;
			
			// Get pointer to window information.											
	
	windowInfoHandle = gProjectInfoPtr->windowInfoHandle;
	gImageWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																	windowInfoHandle, kLock, kMoveHi);
		
			// Exit if pointer to file information does not exist.					
		
	if (gImageWindowInfoPtr == NULL)
																							return (FALSE);
			
			// Get pointer to layer and file information.	
			
	gImageFileInfoPtr = (FileInfoPtr)GetHandlePointer (
										gImageWindowInfoPtr->fileInfoHandle, kLock, kMoveHi);
			
	gImageLayerInfoPtr = (LayerInfoPtr)GetHandlePointer (
										gImageWindowInfoPtr->layerInfoHandle, kLock, kMoveHi);
		
			// If a statistics enhancement structure already exists, then check 	
			// if it is for the current window.  If not release the old 			
			// structure and initialize a new one.											
			
	if (gProjectInfoPtr->statisticsEnhanceSpecsH)
		{
				// Lock the handle to the feature extraction specifications and 	
				// get a pointer to the structure.						
			
		gStatEnhanceSpecsPtr = (StatEnhanceSpecsPtr)GetHandlePointer (
									gProjectInfoPtr->statisticsEnhanceSpecsH, kLock, kMoveHi);
							
		if (gStatEnhanceSpecsPtr->targetWindowInfoHandle != windowInfoHandle || 
			gImageWindowInfoPtr->fileInfoVersion != 
														gStatEnhanceSpecsPtr->fileInfoVersion)
			{
			ReleaseStatisticsEnhanceSpecsMemory (
														&gProjectInfoPtr->statisticsEnhanceSpecsH);
			gStatEnhanceSpecsPtr = NULL;
			
			}	// end "if (gStatisticsImagePtr->fileInfoHandle != ..." 
				
		}	// end "if (gProjectInfoPtr->statisticsEnhanceSpecsH)" 
	
	if (gProjectInfoPtr->statisticsEnhanceSpecsH == NULL)
		{
				// Set up handle to statistics enhancement information.				
			
		gProjectInfoPtr->statisticsEnhanceSpecsH = 
														MNewHandle (sizeof (StatEnhanceSpecs));
		if (gProjectInfoPtr->statisticsEnhanceSpecsH)
			{
			gStatEnhanceSpecsPtr = (StatEnhanceSpecsPtr)GetHandlePointer (
									gProjectInfoPtr->statisticsEnhanceSpecsH, kLock, kMoveHi);
			
					// Initialize the statistics enhancement specification structure.
				
			gStatEnhanceSpecsPtr->hardChiChiThreshold = 80.0;
			gStatEnhanceSpecsPtr->hardPercentThreshold = .0001;
			gStatEnhanceSpecsPtr->labelWeight = 1.0;
			gStatEnhanceSpecsPtr->logLikeStopPercentChange = 0.01;
			gStatEnhanceSpecsPtr->softChiChiThreshold = 40.0;
			gStatEnhanceSpecsPtr->softPercentThreshold = .01;
			
			gStatEnhanceSpecsPtr->classHandle = NULL;
			gStatEnhanceSpecsPtr->targetWindowInfoHandle = windowInfoHandle;
			
			gStatEnhanceSpecsPtr->classThresholdFlagPtr = NULL;
			
			gStatEnhanceSpecsPtr->likePtr = NULL;
			gStatEnhanceSpecsPtr->priorsPtr = NULL;
			gStatEnhanceSpecsPtr->totalLogLikePtr = NULL;
			
			gStatEnhanceSpecsPtr->totalLikePtr = NULL;
			gStatEnhanceSpecsPtr->typicalityWeightPtr = NULL;
			
			gStatEnhanceSpecsPtr->labeledDataPtr = NULL;
			gStatEnhanceSpecsPtr->unlabelDataPtr = NULL;
			
			gStatEnhanceSpecsPtr->classTypicalityPtr = NULL;
			
			gStatEnhanceSpecsPtr->lineStart = 1;
			gStatEnhanceSpecsPtr->lineEnd = (SInt32)
										GetMaxNumberLines (gProjectInfoPtr->windowInfoHandle);
			gStatEnhanceSpecsPtr->lineInterval = 1;
			gStatEnhanceSpecsPtr->columnStart = 1;
			gStatEnhanceSpecsPtr->columnEnd = (SInt32)
										GetMaxNumberColumns (gProjectInfoPtr->windowInfoHandle);
			gStatEnhanceSpecsPtr->columnInterval = 1;
			gStatEnhanceSpecsPtr->iterationMax = 50;
			
			gStatEnhanceSpecsPtr->numberClasses =
														gProjectInfoPtr->numberStatisticsClasses;
			gStatEnhanceSpecsPtr->numberLabeled = 0;
			gStatEnhanceSpecsPtr->numberUnlabeled = 0;
			gStatEnhanceSpecsPtr->unlabeledCount = 0;
			
			gStatEnhanceSpecsPtr->classSet = kAllMenuItem;
			gStatEnhanceSpecsPtr->covarianceStatsToUse = kOriginalStats;
			gStatEnhanceSpecsPtr->fileInfoVersion = 
															gImageWindowInfoPtr->fileInfoVersion;
			gStatEnhanceSpecsPtr->hardThresholdCode = kUsePercentThreshold;
			gStatEnhanceSpecsPtr->iterationStopLength = 4;
			gStatEnhanceSpecsPtr->softThresholdCode = kUseFarthestPixelThreshold;
			gStatEnhanceSpecsPtr->weightSet = kEqualWeights;
			
			gStatEnhanceSpecsPtr->weightLabeledFlag = FALSE;  
			
			if (gStatEnhanceSpecsPtr->softThresholdCode == kUsePercentThreshold) 
				gStatEnhanceSpecsPtr->softChiChiThreshold = GetChiSquaredValue (
									gProjectInfoPtr->numberStatisticsChannels, 
									(100.-gStatEnhanceSpecsPtr->softPercentThreshold)/100.); 
			
			if (gStatEnhanceSpecsPtr->hardThresholdCode == kUsePercentThreshold) 
				gStatEnhanceSpecsPtr->hardChiChiThreshold = GetChiSquaredValue (
									gProjectInfoPtr->numberStatisticsChannels, 
									(100.-gStatEnhanceSpecsPtr->hardPercentThreshold)/100.);
			
			firstTimeFlag = TRUE;
			
			}	// end "if (gProjectInfoPtr->statisticsEnhanceSpecsH)" 
		
		else	// else !gProjectInfoPtr->statisticsEnhanceSpecsH 
			continueFlag = FALSE;
			
		}	// end "if (....statisticsEnhanceSpecsH == NULL)" 
		
	if (continueFlag)
		{	
				// Lock the handle to the classification specifications and get	
				// a pointer to the structure.												
				
		gStatEnhanceSpecsPtr = (StatEnhanceSpecsPtr)GetHandlePointer (
									gProjectInfoPtr->statisticsEnhanceSpecsH, kLock, kMoveHi);
									
				// Check memory for classes vector.											
			
		bytesNeeded = gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		classPtr = (SInt16*)CheckHandleSize (&gStatEnhanceSpecsPtr->classHandle,
															&continueFlag, 
															&changedFlag, 
															bytesNeeded);
			
		if (changedFlag)
			gStatEnhanceSpecsPtr->classSet = kAllMenuItem;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Load selection rectangle information into classification 		
				// specification information if the selection rectangle has 		
				// been set up																		

		GetSelectedAreaInfo (gProjectSelectionWindow,
									gImageWindowInfoPtr,
									&gStatEnhanceSpecsPtr->lineStart,
									&gStatEnhanceSpecsPtr->lineEnd,
									&gStatEnhanceSpecsPtr->columnStart,
									&gStatEnhanceSpecsPtr->columnEnd,
									kDontClearSelectionArea,
									kUseThreshold,
									kAdjustToBaseImage);
		
		if (gStatEnhanceSpecsPtr->classSet == kAllMenuItem)								
			LoadTrainClassVector (&gStatEnhanceSpecsPtr->numberClasses,
											&gStatEnhanceSpecsPtr->classSet, 
											classPtr);
			
				// Get a default interval based on available memory if this is 	
				// the first time for the structure.										
		
		if (firstTimeFlag)
			{		
			gStatEnhanceSpecsPtr->lineInterval = 
													(SInt32)GetStatisticsEnhancePixelInterval ();
			gStatEnhanceSpecsPtr->columnInterval = gStatEnhanceSpecsPtr->lineInterval;
													
			}	// end "if (firstTimeFlag)" 
			
		}	// end "if (continueFlag)" 
	
			// If unable to set up information for statistics enhancement			
			// specifications, release memory allocated to it (if any).				
			
	else	// !continueFlag 
		{
		ReleaseStatisticsEnhanceSpecsMemory (
													&gProjectInfoPtr->statisticsEnhanceSpecsH);
		gStatEnhanceSpecsPtr = NULL;
		
		}	// end "else !continueFlag" 
		
	return (continueFlag);

}	// end "LoadStatisticsEnhanceSpecs" 
		


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean StatisticsEnhance
//
//	Software purpose:	The purpose of this routine is to control the
//							computation for statistics enhancement.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1993
//	Revised By:			Larry L. Biehl			Date: 08/27/2010	

Boolean StatisticsEnhance (void)

{
	ClassInfoPtr						classInfoPtr;
	FileIOInstructionsPtr			fileIOInstructionsPtr;
										
	SInt16*								classPtr;
	
	SInt64								numberPixels = 0;
	
	UInt32								index,
											numberChannels,
											numberClasses;
											
	SInt16								classStorage,
											classWeightsIndex,
											returnCode,
											statClassNumber;
											
	Boolean								continueFlag;
	
	
			// Initialize local variables.												
			
	classPtr = (SInt16*)GetHandlePointer (gStatEnhanceSpecsPtr->classHandle);
														
	numberChannels = gProjectInfoPtr->numberStatisticsChannels;
	numberClasses = gStatEnhanceSpecsPtr->numberClasses;
	returnCode = 0;
	fileIOInstructionsPtr = NULL;
	
	ShowStatusDialogItemSet (kStatusTitle2);
	HideDialogItem (gStatusDialogPtr, IDC_Status22);
	
	LoadDItemStringNumber (kEnhanceStatStrID, 
									IDS_StatEnhance12, 
									gStatusDialogPtr, 
									IDC_Status21, 
									(Str255*)gTextString);
									
	DrawDialog (gStatusDialogPtr);
	
			// Get memory for the structure which contains pointers to the class
			// statistics and data values.
				
	classInfoPtr = GetClassInfoStructure ((UInt16)gStatEnhanceSpecsPtr->numberClasses);
	continueFlag = (classInfoPtr != NULL);
			
				// Allocate memory for variables within the structure.				
				// Memory will be allocated for all classes at one time so that	
				// that same block of memory may be used later when the 				
				// enhanced statistics are copied back to the project. 
	
	if (continueFlag)	
		continueFlag = AssignClassInfoMemory (
							classInfoPtr,
							classPtr, 
							(SInt32)numberClasses,
							(SInt32)numberChannels,
							(SInt32)numberChannels,
							0,													// no float data Values
							kAssignMemory+kNumberChannels+kTriangular,	// covariances
							kAssignMemory+kNumberChannels+kTriangular,	// inverse matrices
							kAssignMemory+kNumberChannels,			// means
							0,													// no transformed cov
							0);												// no transformed means
	
			// Get the total number of labeled pixels.
					
	if (continueFlag)
		gStatEnhanceSpecsPtr->numberLabeled = 
											GetTotalNumberTrainPixels (classPtr, numberClasses);
		
	if (continueFlag)
		{
				// Get memory for unlabeled samples.									
		
		numberPixels = GetNumberPixelsInArea (gStatEnhanceSpecsPtr->lineStart,
															gStatEnhanceSpecsPtr->lineEnd,
															gStatEnhanceSpecsPtr->lineInterval,
															gStatEnhanceSpecsPtr->columnStart,
															gStatEnhanceSpecsPtr->columnEnd,
															gStatEnhanceSpecsPtr->columnInterval,
															kDefaultAlgorithm);
		
		gStatEnhanceSpecsPtr->unlabelDataPtr = (HDoublePtr)
								MNewPointer (numberPixels * numberChannels * sizeof (double));
		continueFlag = (gStatEnhanceSpecsPtr->unlabelDataPtr != NULL);
		
		gStatEnhanceSpecsPtr->numberUnlabeled = numberPixels;
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Get memory for likelihood values for each unlabeled sample
				// and class.		
													
		gStatEnhanceSpecsPtr->likePtr = (double*)
								MNewPointer (numberPixels * numberClasses * sizeof (double));
		continueFlag = (gStatEnhanceSpecsPtr->likePtr != NULL);
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Get memory for total likelihood value for each unlabeled sample.			
													
		gStatEnhanceSpecsPtr->totalLikePtr = (double*)
													MNewPointer (numberPixels * sizeof (double));
		continueFlag = (gStatEnhanceSpecsPtr->totalLikePtr != NULL);
		
		}	// end "if (continueFlag)" 
		
			// Get memory to use for the soft threshold if used.
		
	if (continueFlag && gStatEnhanceSpecsPtr->softThresholdCode != kUseNoThreshold)
		{
				// Get memory for the typicality weight for each sample and class.
													
		gStatEnhanceSpecsPtr->typicalityWeightPtr = (HDoublePtr)
							MNewPointer (numberPixels * numberClasses * sizeof (double));
		continueFlag = (gStatEnhanceSpecsPtr->typicalityWeightPtr != NULL);
	
		if (continueFlag)
			{
					// Get memory for the largest distance for a labeled sample in 
					// each class.
														
			gStatEnhanceSpecsPtr->classTypicalityPtr = (TypicalitySummary*)
								MNewPointer (numberClasses * sizeof (TypicalitySummary));
			continueFlag = (gStatEnhanceSpecsPtr->classTypicalityPtr != NULL);
					
			}	// end "if (continueFlag)"
		
		if (continueFlag &&
				gStatEnhanceSpecsPtr->softThresholdCode == kUseFarthestPixelThreshold)
			{			
					// Get memory for some (5) of the labeled samples.
													
			gStatEnhanceSpecsPtr->labeledDataPtr = (HDoublePtr)
					MNewPointer (numberClasses * 5 * numberChannels * sizeof (double));
			continueFlag = (gStatEnhanceSpecsPtr->labeledDataPtr != NULL);
				
			}	// end "if (...->softThresholdCode == kUseFarthestPixelThreshold)"
				
		}	// end "if (continueFlag && ...->softThresholdCode != kNoUseThreshold)"
	
	if (continueFlag)
		{
				// Get memory for total loglikelihood value for pixels - one		
				// element for each interation.												
													
		gStatEnhanceSpecsPtr->totalLogLikePtr = (double*)
				MNewPointer ((gStatEnhanceSpecsPtr->iterationMax+1) * sizeof (double));
		continueFlag = (gStatEnhanceSpecsPtr->totalLogLikePtr != NULL);
		
		}	// end "if (continueFlag)" 
	
	if (continueFlag)
		{
				// Get memory for the priors value for each class and the 
				// total unlabeled value for each class												
													
		gStatEnhanceSpecsPtr->priorsPtr = (double*)
											MNewPointer (2 * numberClasses * sizeof (double));
		continueFlag = (gStatEnhanceSpecsPtr->priorsPtr != NULL);
		
		}	// end "if (continueFlag)"  
		
	if (continueFlag)
		{
				// Get pointer to memory to use as temporary vector by 
				// 'GetUnlabeledLogLikes'.																
													
		gStatEnhanceSpecsPtr->classThresholdFlagPtr = (Boolean*)
												MNewPointer (numberClasses * sizeof (Boolean));
		continueFlag = (gStatEnhanceSpecsPtr->classThresholdFlagPtr != NULL);
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Get pointer to memory to use for class channel statistics		
				// that will be used in computing the covariance matrix and			
				// getting the data for the class mean vector.							
					
		gTempChannelStatsPtr = (ChannelStatisticsPtr)MNewPointer (
													numberChannels * sizeof (ChannelStatistics));
		continueFlag = (gTempChannelStatsPtr != NULL);	
		
		}	// end "if (continueFlag)" 
			
	if (continueFlag)
	
					// Get buffers to read data from image file into.					
				 				
		continueFlag = GetIOBufferPointers (&gFileIOInstructions[0],
														gImageWindowInfoPtr,
														gImageLayerInfoPtr,
														gImageFileInfoPtr,
														&gInputBufferPtr, 
														&gOutputBufferPtr,
														1,
														gImageWindowInfoPtr->maxNumberColumns,
														1,
														(UInt16)numberChannels,
														gProjectInfoPtr->channelsPtr,
														kPackData,
														kForceBISFormat,
														kForceReal8Bytes,
														kDoNotAllowForThreadedIO,
														&fileIOInstructionsPtr);
	
	if (continueFlag)
		{								
		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance13, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString);
										
		DrawDialog (gStatusDialogPtr);
												
				// Turn spin cursor on

		gPresentCursor = kSpin;
	
				// Load unlabeled data values.	
				
		InitializeAreaDescription (&gAreaDescription, 
												gStatEnhanceSpecsPtr->lineStart, 
												gStatEnhanceSpecsPtr->lineEnd, 
												gStatEnhanceSpecsPtr->columnStart, 
												gStatEnhanceSpecsPtr->columnEnd, 
												gStatEnhanceSpecsPtr->lineInterval, 
												gStatEnhanceSpecsPtr->columnInterval,
												1,
												1,
												0);

		numberPixels = GetFieldDataValues (&gAreaDescription,
														fileIOInstructionsPtr, 
														gProjectInfoPtr->channelsPtr, 
														(SInt16)numberChannels, 
														gStatEnhanceSpecsPtr->unlabelDataPtr, 
														NULL,
														NULL,
														NULL,
														0,
														kNoTrainingSamples);

		if (numberPixels <= 0 || gStatEnhanceSpecsPtr->numberUnlabeled < numberPixels)
			continueFlag = FALSE;
		
		else	// numberPixels > 0 && ...
			gStatEnhanceSpecsPtr->numberUnlabeled = numberPixels;
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
		LoadDItemStringNumber (kEnhanceStatStrID, 
										IDS_StatEnhance14, 
										gStatusDialogPtr, 
										IDC_Status21, 
										(Str255*)gTextString); 
										
		DrawDialog (gStatusDialogPtr);
		
				// Get the number of pixels in each class.								
		
		for (index=0; index<numberClasses; index++)
			{
			statClassNumber = classPtr[index] - 1;
			classStorage = gProjectInfoPtr->storageClass[statClassNumber];
		
						// Get the number of pixels in the class.							
						
			classInfoPtr[index].no_samples = 
					gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;
						
			}	// end "for (index=0; index<numberClasses; index++)" 
			
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Get pointer to memory to use for inverse matrix computation.	
				
		continueFlag = SetupMatrixInversionMemory ((UInt16)numberChannels,
																	TRUE,
																	&gInverseMatrixMemory.inversePtr, 
																	&gInverseMatrixMemory.pivotPtr, 
																	&gInverseMatrixMemory.indexRowPtr, 
																	&gInverseMatrixMemory.indexColPtr, 
																	&gInverseMatrixMemory.ipvotPtr);
		
		}	// end "if (continueFlag)" 

	if (continueFlag)
		continueFlag = InitializeStatisticsVariables (gStatEnhanceSpecsPtr, 
																		classInfoPtr,
																		fileIOInstructionsPtr);
				
			// Dispose of the IO buffer.		
								
	DisposeIOBufferPointers (fileIOInstructionsPtr,
										&gInputBufferPtr, 
										&gOutputBufferPtr);
	
			// List the specifications in the output text window.
			
	if (continueFlag)
		continueFlag = ListStatEnhanceSpecifications (gStatEnhanceSpecsPtr);
	
	if (continueFlag)
		returnCode = behzad_ModifyStatistics (gStatEnhanceSpecsPtr, classInfoPtr);

			// Turn spin cursor off

	gPresentCursor = kWait;
												
			// Reset the cancel operation globals.	
												
	ClearGlobalAlertVariables ();

	if (classInfoPtr != NULL)
		classInfoPtr[0].inversePtr = CheckAndDisposePtr (classInfoPtr[0].inversePtr);
		
	gTempChannelStatsPtr = (ChannelStatisticsPtr)CheckAndDisposePtr (
													(Ptr)gTempChannelStatsPtr);
	
	gStatEnhanceSpecsPtr->unlabelDataPtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->unlabelDataPtr);
											
	gStatEnhanceSpecsPtr->likePtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->likePtr);
											
	gStatEnhanceSpecsPtr->totalLikePtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->totalLikePtr);
											
	gStatEnhanceSpecsPtr->totalLogLikePtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->totalLogLikePtr);
											
	gStatEnhanceSpecsPtr->classThresholdFlagPtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->classThresholdFlagPtr);
	
	gStatEnhanceSpecsPtr->typicalityWeightPtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->typicalityWeightPtr);
	
	gStatEnhanceSpecsPtr->classTypicalityPtr =
									(TypicalitySummaryPtr)CheckAndDisposePtr (
													(Ptr)gStatEnhanceSpecsPtr->classTypicalityPtr);
	
	gStatEnhanceSpecsPtr->labeledDataPtr = CheckAndDisposePtr (
													gStatEnhanceSpecsPtr->labeledDataPtr);
	
	ReleaseMatrixInversionMemory ();
	
	if (returnCode == 1)
		{
		CopyEnhancedStatsToProject (classInfoPtr);
							
				// List the final class weights.												
				
		continueFlag = ListSpecifiedStringNumber (kEnhanceStatStrID,
																IDS_StatEnhance19, 
																(unsigned char*)gTextString, 
																NULL, 
																gOutputForce1Code, 
																continueFlag);
		
		//classWeightsIndex = GetProjectClassWeightsIndex ();
		classWeightsIndex = 1;
								
		continueFlag =  ListClassesUsed (NULL,
													gImageFileInfoPtr,
													classPtr, 
													kListNoSymbols,
													classWeightsIndex, 
													gStatEnhanceSpecsPtr->numberClasses, 
													NULL, 
													&gOutputForce1Code,
													FALSE);
		
		}	// end "if (returnCode == 1)" 
		
	ReleaseClassInfoMemory (classInfoPtr, (UInt16)numberClasses);
	
	gStatEnhanceSpecsPtr->priorsPtr = CheckAndDisposePtr (
																	gStatEnhanceSpecsPtr->priorsPtr);
		
	return (continueFlag);

}	// end "StatisticsEnhance" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void StatisticsEnhanceControl
//
//	Software purpose:	The purpose of this routine is to handle the
//							"Statistics Enhancement" event.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1993
//	Revised By:			Larry L. Biehl			Date: 12/07/2018

void StatisticsEnhanceControl (void)

{
	CMFileStream*						resultsFileStreamPtr;
	UInt16*								classPtr;
	
	time_t								startTime;
											
	UInt16								covarianceStatsToUse;
	
	Boolean								continueFlag;
										
		    		
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																									return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
 			
 			// Initialize variables.															
 			
 	gStatusDialogPtr = NULL;
 	gStatEnhanceSpecsPtr = NULL;
 	resultsFileStreamPtr = NULL;
	
			// Check handle to image file information.  If handle to image			
			// file information doesn't exists. Find the	image file and get		
			// the information for it.															
				
	if (GetProjectImageFileInfo (kPrompt, kSetupGlobalInfoPointers))
		{
				// Set up statistics enhancement specification structure.			
				
		if (LoadStatisticsEnhanceSpecs ())
			{
			if (gImageWindowInfoPtr == NULL)
																									return;
																								
			if (StatisticsEnhanceDialog ())
				{
						// Initialize some global variables pertaining to output.	
				
				gOutputTextOKFlag = TRUE;
				gOutputForce1Code = (gOutputCode | 0x0001);
				continueFlag = TRUE;											
			
				classPtr = (UInt16*)GetHandlePointer (gStatEnhanceSpecsPtr->classHandle);
					
						// Update statistics for project if needed. Remember, do not allow
						// enhanced statistics as the base unless the user specifically
						// requests them.
						
				covarianceStatsToUse = gProjectInfoPtr->covarianceStatsToUse;
				if (covarianceStatsToUse == kEnhancedStats)
					covarianceStatsToUse = gProjectInfoPtr->enhancedStatsOrigin;
						
				if (gStatEnhanceSpecsPtr->covarianceStatsToUse == kEnhancedStats)
					covarianceStatsToUse = kEnhancedStats;			
						
				continueFlag = VerifyProjectStatsUpToDate (
																&gStatEnhanceSpecsPtr->numberClasses,
																(SInt16*)classPtr,
																1,
																(SInt16)covarianceStatsToUse, 
																kSetupGlobalInfoPointers,
																NULL);	
				
						// Continue statistics enhancement if everything is okay.	
				
				if (continueFlag)
					{
							// Change cursor to watch cursor until done with process.
						
					MSetCursor (kWait);
			
							// Force text selection to start from end of present text.
							
					ForceTextToEnd ();
									
					}	// end "if (continueFlag)" 
						
						// Get dialog box to display statistics enhancement status.		
				
				if (continueFlag)
					{
					gStatusDialogPtr = GetStatusDialog (kUpdateStatsInfoID, TRUE);
					
					continueFlag = (gStatusDialogPtr != NULL);
					
					}	// end "if (continueFlag)" 
					
				if (continueFlag)
					LoadDItemStringNumber (kEnhanceStatStrID, 
													IDS_StatEnhance7, 
													gStatusDialogPtr, 
													IDC_Status11, 
													(Str255*)gTextString);
					
						// List the processor name, date and time.			
						
				continueFlag = ListHeaderInfo (
										resultsFileStreamPtr,
										kLImageInfo+kLProjectName+kLProjectImageName+kLStatType, 
										&gOutputForce1Code, 
										(SInt16)covarianceStatsToUse, 
										continueFlag);
				
						// If enhanced statistics to be used, verify that they exist 
						// for the requested classes. If not list a message.
				
				if (continueFlag && 
								gStatEnhanceSpecsPtr->covarianceStatsToUse == kEnhancedStats)
					continueFlag = CheckClassEnhancedStats (
																gStatEnhanceSpecsPtr->numberClasses,
																(SInt16*)classPtr);
								
				if (continueFlag)	
					continueFlag = CheckNumberOfPixelsInClass (
														gStatEnhanceSpecsPtr->numberClasses,
														(SInt16*)classPtr,
														gProjectInfoPtr->numberStatisticsChannels);
								
				if (continueFlag)
					{
					startTime = time (NULL);
					
					continueFlag = StatisticsEnhance ();
						
							// List the CPU time taken for feature extraction.			
							
					continueFlag = ListCPUTimeInformation (
															resultsFileStreamPtr, TRUE, startTime);
							
					}	// end "if (continueFlag)" 
				
						// Scroll output window to the selection and adjust the 		
						// scroll bar.																
				
				UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
					
						// Dispose of status dialog box.										
				
				CloseStatusDialog (TRUE);
				
						// Change cursor back to pointer.									
							
				MInitCursor ();
					
				}	// end "if (StatisticsEnhanceDialog ())" 
				
			}	// end "if (LoadStatisticsEnhanceSpecs ())" 
					
		if (gStatEnhanceSpecsPtr)
			{	
			CheckAndUnlockHandle (gStatEnhanceSpecsPtr->targetWindowInfoHandle);
			CheckAndUnlockHandle (gStatEnhanceSpecsPtr->classHandle);
				
			}	// end "if (gStatEnhanceSpecsPtr)" 
			
				// Unlock memory for statistics enhancement specifications.			
			
		CheckAndUnlockHandle (gProjectInfoPtr->statisticsEnhanceSpecsH);
		gStatEnhanceSpecsPtr = NULL;
			
		UnlockProjectWindowInfoHandles ();
		
		gOperationCanceledFlag = FALSE;
		
		}	// end "if (GetProjectImageFileInfo (..." 

}	// end "StatisticsEnhanceControl" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean StatisticsEnhanceDialog
//
//	Software purpose:	The purpose of this routine is to obtain the
//							user specifications for statistics enhancement processor.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	True if user selects OK in dialog box
//							False if user selects Cancel in dialog box
//
// Called By:			StatisticsEnhanceControl
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1993
//	Revised By:			Larry L. Biehl			Date: 10/22/2018
	
Boolean StatisticsEnhanceDialog (void)	
	
{
	Boolean								returnFlag = TRUE;
									
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
									
	Rect									theBox;
									
	
			// The limit is driven by the small positive value that can
			// be computed with e-x. The smallest value for the 68K
			// processor is 11400. The small value for the PowerPC
			// processor is 740. In the gaussian routine, the likelihood
			// is computed as e-(y/2), therefore the largest y value that
			// can be represented without being 0 is x*2 or 22800 for
			// the 68K and 1480 for the PowerPC. As of 12/19/96 22800 will
			// be the threshold limit allowed both processors. At one
			// time I got underflow errors on the PowerPC, I am not getting
			// them now. I will set the high threshold to see what happens.
	/*
	#ifdef _80BitDoubles_
		double								maxOutlierThreshold = 22800;
	#else
			double								maxOutlierThreshold = 1480;
			double								maxOutlierThreshold = 22800;
	#endif
	*/
	double								maxOutlierThreshold = DBL_MAX;
									
	double								hardChiChiThreshold,
											hardPercentThreshold,
											savedLabelWeight,
											savedLogLikeStopPercent,
											softChiChiThreshold,
											softPercentThreshold,
											variable;
	
	DialogPtr							dialogPtr;
	float*								localClassWeightsPtr = NULL;
	UInt16								*localClassPtr = NULL;
									
	UserItemUPP							drawHardThresholdPopUpPtr,
											drawSoftThresholdPopUpPtr;
	
	Handle								okHandle,
											theHandle;
	
	SInt32								theNum,
											savedIterationMax,
											savedIterationStopLength;
	
	UInt32								localNumberClasses;
	
	SInt16								itemHit,
											savedClassWeightSet,
											theType;
	
	Boolean								modalDone,
											savedUseEnhancedStatisticsFlag,
											savedWeightLabeledFlag;

	
	if (gImageFileInfoPtr == NULL)
																							return (FALSE);
	
	
			// Initialize local variables.													
			
	dialogPtr = NULL;
	
	returnFlag = GetDialogLocalVectors (NULL,
														NULL,
														&localClassPtr,
														NULL,
														&localClassWeightsPtr,
														NULL,
														NULL,
														NULL);
	
			// Get the modal dialog for the statistics enhancement 					
			// specification.																		
	
	if (returnFlag)			
		dialogPtr = LoadRequestedDialog (kStatEnhanceSpecificationID, kCopyScrap, 1, 2);
		
	if (dialogPtr == NULL)
		{
		ReleaseDialogLocalVectors (NULL,
											NULL,
											localClassPtr,
											NULL,
											localClassWeightsPtr,
											NULL,
											NULL,
											NULL);															
																							return (FALSE);
																				
		}	// end "if (dialogPtr == NULL)" 
		
			// Intialize local user item proc pointers.	
			
	drawHardThresholdPopUpPtr = NewUserItemUPP (DrawHardThresholdPopUp);	
	drawSoftThresholdPopUpPtr = NewUserItemUPP (DrawSoftThresholdPopUp);
	
			// Set Procedure pointers for those dialog items that need them.												
			
	SetDialogItemDrawRoutine (dialogPtr, 17, gDrawDialogClassPopUpPtr);																		
			
	SetDialogItemDrawRoutine (dialogPtr, 19, gDrawWeightsPopUpPtr);												
			
	SetDialogItemDrawRoutine (dialogPtr, 25, drawSoftThresholdPopUpPtr);											
			
	SetDialogItemDrawRoutine (dialogPtr, 30, drawHardThresholdPopUpPtr);	
	
	
			// Initialize dialog variables.
	
	StatisticsEnhanceDialogInitialize (dialogPtr,
													gStatEnhanceSpecsPtr,
													&dialogSelectArea,
													localClassPtr,
													&gClassSelection,
													&localNumberClasses,
													&savedUseEnhancedStatisticsFlag,
													&savedClassWeightSet,
													&gWeightsSelection,
													&gSoftThresholdSelection,
													&softChiChiThreshold,
													&softPercentThreshold,
													&gHardThresholdSelection,
													&hardChiChiThreshold,
													&hardPercentThreshold,
													&savedWeightLabeledFlag,
													&savedLabelWeight,
													&savedIterationMax,
													&savedIterationStopLength,
													&savedLogLikeStopPercent);
											
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Use enhanced statistics for initial conditions.							
		
	SetDLogControl (dialogPtr, 21, savedUseEnhancedStatisticsFlag);
	
			// Soft outlier threshold value.														
		
	LoadDItemRealValue (dialogPtr, 26, softChiChiThreshold, 1);
	
			// Percent probability correct value to use to define soft outlier			
			// threshold.																			
	
	LoadDItemRealValue (dialogPtr, 28, softPercentThreshold, 8);
	
			// Hard outlier threshold value.														
		
	LoadDItemRealValue (dialogPtr, 31, hardChiChiThreshold, 1);
	
			// Percent probability correct value to use to define hard outlier			
			// threshold.																			
	
	LoadDItemRealValue (dialogPtr, 33, hardPercentThreshold, 8);
	
			// Weight label samples.															
	
	SetDLogControl (dialogPtr, 34, savedWeightLabeledFlag);
	
			// Labeled sample weight.
	
	LoadDItemRealValue (dialogPtr, 36, savedLabelWeight, 3);
	
			// Maximum number of iterations allowed.										
		
	LoadDItemValue (dialogPtr, 39, savedIterationMax);
	
			// Iteration stop length.															
		
	LoadDItemValue (dialogPtr, 41, savedIterationStopLength);
	
			// Log like stop percent change.													
		
	LoadDItemRealValue (dialogPtr, 43, savedLogLikeStopPercent, 2);
	
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kStatEnhanceSpecificationID, kSetUpDFilterTable);
		
	gDialogItemDescriptorPtr[26] = kDItemReal;
	gDialogItemDescriptorPtr[28] = kDItemReal;
	gDialogItemDescriptorPtr[31] = kDItemReal;
	gDialogItemDescriptorPtr[33] = kDItemReal;
	gDialogItemDescriptorPtr[36] = kDItemReal;
	gDialogItemDescriptorPtr[42] = kDItemReal;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 10, 0, SHRT_MAX);
		
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
				case 7:		// Entire area to selected area switch.
				case 44:		// Entire area to selected area switch. (Appearance Manager)
				case 10:		//	 lineStart
				case 11:		//	 lineEnd
				case 12:		//	 lineInterval
				case 13:		//	 columnStart
				case 14:		//	 columnEnd
				case 15:		//	 columnInterval
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
					
				case 17:		// Classes 
					if (gClassSelection > 0)
						{
						itemHit = StandardPopUpMenu (dialogPtr, 
																16, 
																17, 
																gPopUpAllSubsetMenu, 
																gClassSelection, 
																kPopUpAllSubsetMenuID);
													
						if (itemHit == kSubsetMenuItem)
							{
									// Subset of classes to be used.							
									
							itemHit = ClassDialog (&localNumberClasses,
															(SInt16*)localClassPtr, 
															1,
															gProjectInfoPtr->numberStatisticsClasses,
															gClassSelection,
															okHandle);
							
							}	// end "if (itemHit == kSubsetMenuItem)" 
						
						if (itemHit != 0)
							gClassSelection = itemHit;
					
						if (gClassSelection == kAllMenuItem)
										// Make sure that the class settings are setup for all
										// classes in case they will be used by the symbols
										// or weights popup menu.
										
							LoadTrainClassVector (&localNumberClasses,
															&gClassSelection, 
															(SInt16*)localClassPtr);
							
						UpdateNumberLabeledSamples (dialogPtr,
																gClassSelection,
																(SInt16*)localClassPtr,
																localNumberClasses);
		
								// Make certain that the correct label is drawn in 		
								// the class pop up box.											
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						}	// end "if (gClassSelection > 0)"
					break;
					
				case 19:		// Use equal class weights 
					itemHit = StandardPopUpMenu (dialogPtr, 
															18, 
															19, 
															gPopUpWeightsMenu, 
															gWeightsSelection, 
															kPopUpWeightsMenuID);
													
					if (itemHit == kUnequalWeightMenuItem)
						{
								// Unequal weights to be used.								
								
						HiliteControl ((ControlHandle)okHandle, 255);
						
						gWeightsSelection = ClassWeightsDialog (
																		localNumberClasses,
																		(SInt16*)localClassPtr,
																		localClassWeightsPtr,
																		gWeightsSelection, 
																		savedUseEnhancedStatisticsFlag);
						
						HiliteControl ((ControlHandle)okHandle, 0);
						
						itemHit = gWeightsSelection;
							
						}	// end "if (itemHit == kUnequalWeightMenuItem)" 
					
					if (itemHit != 0)
						gWeightsSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// weights pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 21:		// check box for "Used enhanced statistics" 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					savedUseEnhancedStatisticsFlag = !savedUseEnhancedStatisticsFlag;
														
					gWeightsSelection = UpdateDialogClassWeightsInfo (
																		gWeightsSelection,
																		savedUseEnhancedStatisticsFlag,
																		&savedClassWeightSet,
																		FALSE);
					
					InvalidateDialogItemRect (dialogPtr, 19);
					break;
					
				case 25:		// Soft outlier threshold 
					itemHit = StandardPopUpMenu (dialogPtr, 
															24, 
															25, 
															gPopUpSoftThresholdMenu, 
															gSoftThresholdSelection, 
															kPopUpSoftThresholdMenuID);
						
					if (itemHit != 0 && itemHit != gSoftThresholdSelection)
						{
						gSoftThresholdSelection = itemHit;
						
						StatisticsEnhanceDialogUpdateThresholdItems (
																		dialogPtr,
																		gSoftThresholdSelection,
																		IDC_SoftChiChiThreshold,
																		IDC_SoftPercentThresholdPrompt,
																		IDC_SoftPercentThreshold,
																		softPercentThreshold,
																		&softChiChiThreshold);
						
						}	// end "if (itemHit != 0 && itemHit != ...)"
	
							// Make certain that the correct label is drawn in 		
							// the soft threshold pop up box.											
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
					break;
					
				case 26:				// Soft outlier threshold variable	
					variable = GetDItemRealValue (dialogPtr, itemHit);
					if (variable > maxOutlierThreshold)
						RealNumberErrorAlert (softChiChiThreshold, dialogPtr, itemHit, 1);

					if (variable > 0 && 
								variable <= maxOutlierThreshold &&
										softChiChiThreshold != variable)
						{
						softChiChiThreshold = variable;
						
						if (gSoftThresholdSelection != kUseChiChiThreshold)
							{
							gSoftThresholdSelection = kUseChiChiThreshold;
							
							StatisticsEnhanceDialogUpdateThresholdItems (
																		dialogPtr,
																		gSoftThresholdSelection,
																		IDC_SoftChiChiThreshold,
																		IDC_SoftPercentThresholdPrompt,
																		IDC_SoftPercentThreshold,
																		softPercentThreshold,
																		&softChiChiThreshold);
							
							InvalidateDialogItemRect (dialogPtr, 25);
							
							}	// end "if (gSoftThresholdSelection != ..."
						
						}	// end "if (variable > 0 && variable <= ..."
					break;
					
				case 28:				// Soft percent probability correct threshold	
					variable = GetDItemRealValue (dialogPtr, itemHit);
					if (variable > 100.0)
						RealNumberErrorAlert (softPercentThreshold, dialogPtr, itemHit, 8);
						
					if (variable >= 0 && variable <= 100.0)
						{
						softPercentThreshold = variable;
						
						StatisticsEnhanceDialogUpdateChiSquaredValue (
														dialogPtr,
														gSoftThresholdSelection,
														IDC_SoftChiChiThreshold,
														softPercentThreshold,
														&softChiChiThreshold);
						
						}	// end "if (variable >= 0 && variable <= 100.0)"
					break;
					
				case 30:		// Hard outlier threshold 
					itemHit = StandardPopUpMenu (dialogPtr, 
															29, 
															30, 
															gPopUpHardThresholdMenu, 
															gHardThresholdSelection, 
															kPopUpHardThresholdMenuID);
						
					if (itemHit != 0 && itemHit != gHardThresholdSelection)
						{
						gHardThresholdSelection = itemHit;
				
						StatisticsEnhanceDialogUpdateThresholdItems (
																		dialogPtr,
																		gHardThresholdSelection,
																		IDC_HardChiChiThreshold,
																		IDC_HardPercentThresholdPrompt,
																		IDC_HardPercentThreshold,
																		hardPercentThreshold,
																		&hardChiChiThreshold);
						
						}	// end "if (itemHit != 0 && itemHit != ...)"
	
							// Make certain that the correct label is drawn in 		
							// the hard threshold pop up box.											
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 31:				// Soft outlier threshold variable	
					variable = GetDItemRealValue (dialogPtr, itemHit);
					if (variable > maxOutlierThreshold)
						RealNumberErrorAlert (hardChiChiThreshold, dialogPtr, itemHit, 1);

					if (variable > 0 && 
								variable <= maxOutlierThreshold &&
										hardChiChiThreshold != variable)
						{
						hardChiChiThreshold = variable;
						
						if (gHardThresholdSelection != kUseChiChiThreshold)
							{
							gHardThresholdSelection = kUseChiChiThreshold;
						
							StatisticsEnhanceDialogUpdateThresholdItems (
																		dialogPtr,
																		gHardThresholdSelection,
																		IDC_HardChiChiThreshold,
																		IDC_HardPercentThresholdPrompt,
																		IDC_HardPercentThreshold,
																		hardPercentThreshold,
																		&hardChiChiThreshold);
							
							InvalidateDialogItemRect (dialogPtr, 30);
							
							}	// end "if (gSoftThresholdSelection != ..."
						
						}	// end "if (variable > 0 && variable <= maxOutlierThreshold)"
					break;
					
				case 33:				// Hard percent probability correct threshold	
					variable = GetDItemRealValue (dialogPtr, itemHit);
					if (variable > 100.0)
						RealNumberErrorAlert (hardPercentThreshold, dialogPtr, itemHit, 8);
						
					if (variable >= 0 && variable <= 100.0)
						{
						hardPercentThreshold = variable;
						
						StatisticsEnhanceDialogUpdateChiSquaredValue (
																			dialogPtr,
																			gHardThresholdSelection,
																			IDC_HardChiChiThreshold,
																			hardPercentThreshold,
																			&hardChiChiThreshold);
						
						}	// end "if (variable >= 0 && variable <= 100.0)"
					break;
					
				case 34:		// check box for "Weight labeled samples" 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					savedWeightLabeledFlag = !savedWeightLabeledFlag;
					ShowHideDialogItem (dialogPtr, 35, savedWeightLabeledFlag);
					ShowHideDialogItem (dialogPtr, 36, savedWeightLabeledFlag);
					break;
					
				case 36:				// Label sample weighting	
					variable = GetDItemRealValue (dialogPtr, itemHit);
					if (variable > 1000.0)
						RealNumberErrorAlert (savedLabelWeight, dialogPtr, itemHit, 1);
					if (variable > 0 && variable <= 1000.0)
						savedLabelWeight = variable;
					break;
					
				case 39:				// Maximum number of iterations	
				   if (theNum == 0 || theNum > 200)	
						NumberErrorAlert ((SInt32)savedIterationMax,
						 								dialogPtr, 
						 								itemHit);
						
					else
						savedIterationMax = theNum;		
					break;
					
				case 41:				// Iteration stop length	
				   if (theNum == 0 || theNum > 200)	
						NumberErrorAlert ((SInt32)savedIterationStopLength,
						 								dialogPtr, 
						 								itemHit);
						
					else
						savedIterationStopLength = theNum;		
					break;
					
				case 43:				// Log like stop change	
					variable = GetDItemRealValue (dialogPtr, itemHit);
					if (variable > 10.0)
						RealNumberErrorAlert (savedLabelWeight, dialogPtr, itemHit, 2);
					if (variable > 0 && variable <= 10.0)
						savedLogLikeStopPercent = variable;
					break;
						
				}	// end "switch (itemHit)"
				
			if ((itemHit >= 7 && itemHit <= 15) || itemHit == 44)
				UpdateNumberUnlabeledSamples (&dialogSelectArea,
														dialogPtr);
			
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{	
					// If item hit is 1, check if line-column values make 			
					// sense.  If they don't, sound an alert and make item hit 		
					// equal to 0 to allow user to make changes.							
					
			if (itemHit == 1)
				itemHit = CheckLineColValues (&dialogSelectArea,
														gStatEnhanceSpecsPtr->lineStart,
														gStatEnhanceSpecsPtr->columnStart,
														dialogPtr);
				
			if (itemHit == 1)
				{
				      // User selected OK for information 
				      
				modalDone = TRUE;
				returnFlag = TRUE;
	
				StatisticsEnhanceDialogOK (gStatEnhanceSpecsPtr,
													&dialogSelectArea,
													localClassPtr,
													gClassSelection,
													localNumberClasses,
													savedUseEnhancedStatisticsFlag,
													gWeightsSelection,
													localClassWeightsPtr,
													gSoftThresholdSelection,
													softChiChiThreshold,
													softPercentThreshold,
													gHardThresholdSelection,
													hardChiChiThreshold,
													hardPercentThreshold,
													savedWeightLabeledFlag,
													savedLabelWeight,
													savedIterationMax,
													savedIterationStopLength,
													savedLogLikeStopPercent);
														
				}	// end if (itemHit == 1) 
			
			if (itemHit == 2)
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if (itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);	
	
	DisposeUserItemUPP (drawHardThresholdPopUpPtr);	
	DisposeUserItemUPP (drawSoftThresholdPopUpPtr);							   
		
	ReleaseDialogLocalVectors (NULL,
										NULL,
										localClassPtr,
										NULL,
										localClassWeightsPtr,
										NULL,
										NULL,
										NULL);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac
									
	#if defined multispec_win    
		CMEnhanceStatisticsDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMEnhanceStatisticsDialog ();
			
			returnFlag = dialogPtr->DoDialog (gStatEnhanceSpecsPtr); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
	#endif	// defined multispec_win
	
	#if defined multispec_lin
		CMEnhanceStatisticsDialog*		dialogPtr = NULL;
     
		//dialogPtr = new CMEnhanceStatisticsDialog ((wxWindow*)GetMainFrame ());
		dialogPtr = new CMEnhanceStatisticsDialog (NULL);
		
		returnFlag = dialogPtr->DoDialog (gStatEnhanceSpecsPtr); 
	                       
		delete dialogPtr;
	#endif

	return (returnFlag);

}	// end "StatisticsEnhanceDialog" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void StatisticsEnhanceDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the parameters
//							in the Statistics Enhancement dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsEnhanceDialog
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1999
//	Revised By:			Larry L. Biehl			Date: 09/05/2017
	
void StatisticsEnhanceDialogInitialize (
				DialogPtr							dialogPtr,
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				UInt16*								localClassPtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				Boolean*								useEnhancedStatisticsFlagPtr,
				SInt16*								classWeightSetPtr,
				SInt16*								weightsSelectionPtr,
				SInt16*								softThresholdSelectionPtr,
				double*								softChiChiThresholdPtr,
				double*								softPercentThresholdPtr,
				SInt16*								hardThresholdSelectionPtr,
				double*								hardChiChiThresholdPtr,
				double*								hardPercentThresholdPtr,
				Boolean*								weightLabeledFlagPtr,
				double*								labelWeightPtr,
				SInt32*								iterationMaxPtr,
				SInt32*								iterationStopLengthPtr,
				double*								logLikeStopPercentPtr)
	
{
	SInt16								entireIconItem;
	
	
			// Load the dialog local vectors

	LoadDialogLocalVectors (NULL,
									NULL,
									NULL,
									0,
									localClassPtr,
									statEnhanceSpecsPtr->classHandle,
									statEnhanceSpecsPtr->numberClasses,
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

	entireIconItem = 7;
	if (gAppearanceManagerFlag)
		entireIconItem = 44;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											gImageWindowInfoPtr,
											gProjectSelectionWindow,
											statEnhanceSpecsPtr->columnStart,
											statEnhanceSpecsPtr->columnEnd,
											statEnhanceSpecsPtr->columnInterval,
											statEnhanceSpecsPtr->lineStart,
											statEnhanceSpecsPtr->lineEnd,
											statEnhanceSpecsPtr->lineInterval,
											10,
											entireIconItem,
											kAdjustToBaseImage);

	#if defined multispec_mac 
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 7);
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 44);
	#endif	// defined multispec_mac  

			// To entire image icon.															
			//	Selected area for statistics enhancement									
			
	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	
			// Classes to use.
			
	*classSelectionPtr = statEnhanceSpecsPtr->classSet;
	*localNumberClassesPtr = statEnhanceSpecsPtr->numberClasses;
	
			// Class weights.	
	
	*useEnhancedStatisticsFlagPtr = 
							(statEnhanceSpecsPtr->covarianceStatsToUse == kEnhancedStats);
	if (gProjectInfoPtr->modifiedClassChanStatsHandle == NULL)
		*useEnhancedStatisticsFlagPtr = FALSE;
	
	*classWeightSetPtr = kEqualWeights;													
	*weightsSelectionPtr = UpdateDialogClassWeightsInfo (kEqualWeights,
																			*useEnhancedStatisticsFlagPtr,
																			classWeightSetPtr,
																			TRUE);
	
			// Use enhanced statistics for initial conditions.							
		
	if (gProjectInfoPtr->modifiedClassChanStatsHandle == NULL)
		{
		SetDLogControlHilite (dialogPtr, IDC_UseEnhancedStats, 255);
		/*
		#if defined multispec_win 
			if (gProjectInfoPtr->modifiedClassChanStatsHandle == NULL)
				{
				CComboBox* comboBoxPtr = 
									(CComboBox*)dialogPtr->GetDlgItem (IDC_WeightCombo);
				if (comboBoxPtr->GetCount () > 1)
					comboBoxPtr->DeleteString (1);

				}	// end "if (...->modifiedClassChanStatsHandle == NULL)"
		#endif	// defined multispec_win  
		*/
		}	// end "if (gProjectInfoPtr->modifiedClassChanStatsHandle == NULL)"
	
			// Soft outlier threshold value.	
			
	*softThresholdSelectionPtr = statEnhanceSpecsPtr->softThresholdCode;	
	*softChiChiThresholdPtr = statEnhanceSpecsPtr->softChiChiThreshold;
	*softPercentThresholdPtr = statEnhanceSpecsPtr->softPercentThreshold;
						
	StatisticsEnhanceDialogUpdateThresholdItems (dialogPtr,
																*softThresholdSelectionPtr,
																IDC_SoftChiChiThreshold,
																IDC_SoftPercentThresholdPrompt,
																IDC_SoftPercentThreshold,
																*softPercentThresholdPtr,
																softChiChiThresholdPtr);
	
			// Hard outlier threshold value.	
			
	*hardThresholdSelectionPtr = statEnhanceSpecsPtr->hardThresholdCode;	
	*hardChiChiThresholdPtr = statEnhanceSpecsPtr->hardChiChiThreshold;
	*hardPercentThresholdPtr = statEnhanceSpecsPtr->hardPercentThreshold;
						
	StatisticsEnhanceDialogUpdateThresholdItems (dialogPtr,
																*hardThresholdSelectionPtr,
																IDC_HardChiChiThreshold,
																IDC_HardPercentThresholdPrompt,
																IDC_HardPercentThreshold,
																*hardPercentThresholdPtr,
																hardChiChiThresholdPtr);
	
			// Weight label samples.															
	
	*weightLabeledFlagPtr = statEnhanceSpecsPtr->weightLabeledFlag;
	
			// Weighting for labeled samples.												
		
	*labelWeightPtr = statEnhanceSpecsPtr->labelWeight;
	if (!*weightLabeledFlagPtr)
		{
		HideDialogItem (dialogPtr, IDC_WeightingPrompt);
		HideDialogItem (dialogPtr, IDC_Weighting);
		
		}	// end "if (!*weightLabeledFlagPtr)"
	
			// Maximum number of iterations allowed.										
		
	*iterationMaxPtr = statEnhanceSpecsPtr->iterationMax;
	
			// Iteration stop length.															
		
	*iterationStopLengthPtr = statEnhanceSpecsPtr->iterationStopLength;
	
			// Log like stop percent change.													
		
	*logLikeStopPercentPtr = statEnhanceSpecsPtr->logLikeStopPercentChange;

			// Display the number of selected unlabeled samples.						
			
	UpdateNumberUnlabeledSamples (dialogSelectAreaPtr,
											dialogPtr);

			// Display the number of selected labeled samples.							
			
	UpdateNumberLabeledSamples (dialogPtr,
											*classSelectionPtr,
											(SInt16*)localClassPtr,
											statEnhanceSpecsPtr->numberClasses);

}	// end "StatisticsEnhanceDialogInitialize" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void StatisticsEnhanceDialogOK
//
//	Software purpose:	The purpose of this routine is to initialize the parameters
//							in the Statistics Enhancement dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsEnhanceDialog
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999
	
void StatisticsEnhanceDialogOK (
				StatEnhanceSpecsPtr				statEnhanceSpecsPtr,
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				UInt16*								localClassPtr,
				SInt16								classSelection,
				UInt32								localNumberClasses,
				Boolean								useEnhancedStatisticsFlag,
				SInt16								weightsSelection,
				float*								classWeightsPtr,
				SInt16								softThresholdSelection,
				double								softChiChiThreshold,
				double								softPercentThreshold,
				SInt16								hardThresholdSelection,
				double								hardChiChiThreshold,
				double								hardPercentThreshold,
				Boolean								weightLabeledFlag,
				double								labelWeight,
				SInt32								iterationMax,
				SInt32								iterationStopLength,
				double								logLikeStopPercent)
	
{
			// Statistics enhancement area.										
	
	statEnhanceSpecsPtr->lineStart = dialogSelectAreaPtr->lineStart;
	statEnhanceSpecsPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
	statEnhanceSpecsPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
	statEnhanceSpecsPtr->columnStart = dialogSelectAreaPtr->columnStart;
	statEnhanceSpecsPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
	statEnhanceSpecsPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
											
			// Load some common processor parameters
			// Channels
			// Classes	
			// Class symbols	
			
	LoadProcessorVectorsFromDialogLocalVectors (0,
																FALSE,
																0,
																0,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																classSelection,
																localNumberClasses,
																(UInt16*)localClassPtr,
																&statEnhanceSpecsPtr->classSet,
																&statEnhanceSpecsPtr->numberClasses,
																statEnhanceSpecsPtr->classHandle,
																0,
																NULL,
																NULL,
																NULL,
																0,
																NULL,
																NULL);
	
			// Class weights.															
			
	UpdateProjectClassWeights (useEnhancedStatisticsFlag,
										weightsSelection,
										classWeightsPtr);
	
	statEnhanceSpecsPtr->weightSet = weightsSelection;

			// Use enhanced statistics for initial conditions.				

	statEnhanceSpecsPtr->covarianceStatsToUse = kOriginalStats;
	if (useEnhancedStatisticsFlag)
		statEnhanceSpecsPtr->covarianceStatsToUse = kEnhancedStats;

			// Soft outlier threshold.													
	
	statEnhanceSpecsPtr->softThresholdCode = softThresholdSelection;
	statEnhanceSpecsPtr->softChiChiThreshold = softChiChiThreshold;
	statEnhanceSpecsPtr->softPercentThreshold = softPercentThreshold;

			// Hard outlier threshold.													
	
	statEnhanceSpecsPtr->hardThresholdCode = hardThresholdSelection;
	statEnhanceSpecsPtr->hardChiChiThreshold = hardChiChiThreshold;
	statEnhanceSpecsPtr->hardPercentThreshold = hardPercentThreshold;

			// Weighting to use for labeled pixels.							
			
	statEnhanceSpecsPtr->weightLabeledFlag = weightLabeledFlag;
	statEnhanceSpecsPtr->labelWeight = labelWeight;
		
			// Iteration stop conditions.											
			
	statEnhanceSpecsPtr->iterationMax = iterationMax;
	statEnhanceSpecsPtr->iterationStopLength = (SInt16)iterationStopLength;
	statEnhanceSpecsPtr->logLikeStopPercentChange = logLikeStopPercent;

}	// end "StatisticsEnhanceDialogOK"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void StatisticsEnhanceDialogUpdateThresholdItems
//
//	Software purpose:	The purpose of this routine is to setup the appropriate 
//							threshold dialog items for the input threshold code.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsEnhanceDialog
//
//	Coded By:			Larry L. Biehl			Date: 12/02/1999
//	Revised By:			Larry L. Biehl			Date: 12/03/1999
	
void StatisticsEnhanceDialogUpdateThresholdItems (
				DialogPtr							dialogPtr,
				SInt16								thresholdCode,
				SInt16								chiChiThresholdItemNumber,
				SInt16								percentThresholdPromptItemNumber,
				SInt16								percentThresholdItemNumber,
				double								thresholdPercent,
				double*								chiChiThresholdPtr)
	
{
	switch (thresholdCode)
		{
		case kUseNoThreshold:
			HideDialogItem (dialogPtr, chiChiThresholdItemNumber);
			HideDialogItem (dialogPtr, percentThresholdPromptItemNumber);
			HideDialogItem (dialogPtr, percentThresholdItemNumber);
			break;
			
		case kUseChiChiThreshold:
			ShowDialogItem (dialogPtr, chiChiThresholdItemNumber);
			HideDialogItem (dialogPtr, percentThresholdPromptItemNumber);
			HideDialogItem (dialogPtr, percentThresholdItemNumber);
			break;
			
		case kUsePercentThreshold:
			ShowDialogItem (dialogPtr, chiChiThresholdItemNumber);
			ShowDialogItem (dialogPtr, percentThresholdPromptItemNumber);
			ShowDialogItem (dialogPtr, percentThresholdItemNumber);
			
			StatisticsEnhanceDialogUpdateChiSquaredValue (dialogPtr,
																			thresholdCode,
																			chiChiThresholdItemNumber,
																			thresholdPercent,
																			chiChiThresholdPtr);
			break;
			
		case kUseFarthestPixelThreshold:
			HideDialogItem (dialogPtr, chiChiThresholdItemNumber);
			HideDialogItem (dialogPtr, percentThresholdPromptItemNumber);
			HideDialogItem (dialogPtr, percentThresholdItemNumber);
			break;
		
		}	// end "switch (thresholdCode)"

}	// end "StatisticsEnhanceDialogUpdateThresholdItems" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void StatisticsEnhanceDialogUpdateChiSquaredValue
//
//	Software purpose:	The purpose of this routine is to update the chi square value
//							to reflect the input percent value setting.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsEnhanceDialog
//
//	Coded By:			Larry L. Biehl			Date: 11/04/1999
//	Revised By:			Larry L. Biehl			Date: 12/02/1999
	
void StatisticsEnhanceDialogUpdateChiSquaredValue (
				DialogPtr							dialogPtr,
				SInt16								thresholdCode,
				SInt16								chiChiThresholdItemNumber,
				double								thresholdPercent,
				double*								chiChiThresholdPtr)
	
{
	if (thresholdCode == kUsePercentThreshold)
		{
		*chiChiThresholdPtr = GetChiSquaredValue (
														gProjectInfoPtr->numberStatisticsChannels,
														(100.-thresholdPercent)/100.);

		LoadDItemRealValue (dialogPtr, 
									chiChiThresholdItemNumber,
									*chiChiThresholdPtr, 
									1);
		
		}	// end "if (thresholdCode == kUsePercentThreshold)" 

}	// end "StatisticsEnhanceDialogUpdateChiSquaredValue" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateNumberUnlabeledSamples
//
//	Software purpose:	The purpose of this routine is to update the number
//							of unlabeled samples that is displayed in the dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsEnhanceDialog
//
//	Coded By:			Larry L. Biehl			Date: 01/11/1994
//	Revised By:			Larry L. Biehl			Date: 08/27/2010
	
void UpdateNumberUnlabeledSamples (
				DialogSelectAreaPtr				dialogSelectAreaPtr,
				DialogPtr							dialogPtr)	
	
{
	SInt64								numberPixels;
	

			// Display the number of selected unlabeled samples.							
			
	numberPixels = GetNumberPixelsInArea (dialogSelectAreaPtr->lineStart,
														dialogSelectAreaPtr->lineEnd,
														dialogSelectAreaPtr->lineInterval,
														dialogSelectAreaPtr->columnStart,
														dialogSelectAreaPtr->columnEnd,
														dialogSelectAreaPtr->columnInterval,
														kDefaultAlgorithm);
	
	LoadDItemValueWithCommas (dialogPtr, IDC_UnlabeledSamples, numberPixels);

}	// end "UpdateNumberUnlabeledSamples" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateNumberLabeledSamples
//
//	Software purpose:	The purpose of this routine is to update the number
//							of labeled samples that is displayed in the dialog.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsEnhanceDialog
//
//	Coded By:			Larry L. Biehl			Date: 01/11/1994
//	Revised By:			Larry L. Biehl			Date: 08/27/2010
	
void UpdateNumberLabeledSamples (
				DialogPtr							dialogPtr,
				SInt16								classSelection,
				SInt16*								classPtr,
				UInt32								numberClasses)	
	
{
	SInt64								numberPixels;
	
	
	if (classSelection == kAllMenuItem)
		{
		classPtr = NULL;
		numberClasses = gProjectInfoPtr->numberStatisticsClasses;
		
		}	// end "if (classSelection == kAllMenuItem)" 

			// Display the number of selected labeled samples.							
			
	numberPixels = GetTotalNumberTrainPixels (classPtr, numberClasses);
	LoadDItemValueWithCommas (dialogPtr, IDC_LabeledSamples, numberPixels);

}	// end "UpdateNumberLabeledSamples"  


                                                                                 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 UpdateLabeledValueList
//
//	Software purpose:	The purpose of this routine is to check the input distance
//							value for the input labeled value vector and check if it is
//							one of the five farthest values from the respective class
//                   mean. If so the labeled value vector is included in the 
//							the list and the vector that it replaces is replaced.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1999
//	Revised By:			Larry L. Biehl			Date: 12/29/2005	

void UpdateLabeledValueList (
				DistancesSummaryPtr				distancesSummaryPtr, 
				double								distance,
				HDoublePtr							inputBufferPtr,
				HDoublePtr							labeledDataPtr,
				UInt32								numberChannels)

{
	double								maxDistance,
											minDistance;
											
	DistancesListPtr					distancesListPtr;
	
	SInt32								endIndex,
											index,
											listIndex,
											previousIndex,
											startIndex,
											sumIndex;
											
	UInt32								distancesToStore = 5;
	
	Boolean								removeSmallestDistanceFlag;
	
	
			// Initialize local variables.
			
	distancesListPtr = &distancesSummaryPtr->distancesList[0];
	
	startIndex = distancesSummaryPtr->startIndex;
	maxDistance = distancesListPtr[startIndex].distanceValue;
	
	endIndex = distancesSummaryPtr->endIndex;
	minDistance = distancesListPtr[endIndex].distanceValue;
	
	sumIndex = distancesSummaryPtr->freeIndex;
	removeSmallestDistanceFlag = FALSE;
	listIndex = -1;
				
			// Update the list pointers.	
	
	if (distance >= maxDistance)
		{
		distancesListPtr[startIndex].previousMaximum = sumIndex;
		
		distancesListPtr[sumIndex].distanceValue = distance;
		distancesListPtr[sumIndex].nextMaximum = distancesSummaryPtr->startIndex;
		distancesListPtr[sumIndex].previousMaximum = -1;
		
		distancesSummaryPtr->startIndex = sumIndex;
		
		if (distancesSummaryPtr->numberLabeledValues == 0)
			distancesListPtr[0].nextMaximum = -1;
			
		if (distancesSummaryPtr->numberLabeledValues >= distancesToStore)
			removeSmallestDistanceFlag = TRUE;
		
		}	// end "if (distance >= maxDistance" 
	
	else	// distance < distanceSummaryPtr->maxDistance	
		{
		if (distance <= minDistance)
			{
			if (distancesSummaryPtr->numberLabeledValues < distancesToStore)
				{
				distancesListPtr[endIndex].nextMaximum = sumIndex;
				
				distancesListPtr[sumIndex].distanceValue = distance;
				distancesListPtr[sumIndex].nextMaximum = -1;
				distancesListPtr[sumIndex].previousMaximum = distancesSummaryPtr->endIndex;
				distancesSummaryPtr->endIndex = sumIndex;
				
				}	// end "if (...->numberLabeledValues < distancesToStore)"
		
			}	// end "if (distance <= distanceSummaryPtr->minDistance" 
			
		else	// ...->minDistance < distance < ...->maxDistance 
			{
			index = distancesListPtr[startIndex].nextMaximum;
			while (index != -1)
				{
				if (distance > distancesListPtr[index].distanceValue)
					{
					previousIndex = distancesListPtr[index].previousMaximum;
					distancesListPtr[previousIndex].nextMaximum = sumIndex;
								
					distancesListPtr[sumIndex].distanceValue = distance;
					distancesListPtr[sumIndex].nextMaximum = index;
					distancesListPtr[sumIndex].previousMaximum = previousIndex;
																				
					distancesListPtr[index].previousMaximum = sumIndex;
					index = distancesSummaryPtr->endIndex;
					
					}	// end "classPairMean > distancesListPtr..." 
				
				index = distancesListPtr[index].nextMaximum;
				
				}	// end "while (index != -1)" 
			
			if (distancesSummaryPtr->numberLabeledValues >= distancesToStore)
				removeSmallestDistanceFlag = TRUE;
			
			}	// end "else ...->minDistance < distance < ...->maxDistance 
			
		}	// end "else classPairMinimum < maxMinimum" 
			
			// Check if the last maximum distance needs to be removed from			
			// the list.																			
			
	if (removeSmallestDistanceFlag)
		{
				// Remove the last maximum distance from the list.										
		
		previousIndex = distancesListPtr[endIndex].previousMaximum;
		distancesListPtr[previousIndex].nextMaximum = -1;
			
		listIndex = (SInt32)distancesListPtr[endIndex].listIndex;
		distancesListPtr[sumIndex].listIndex = listIndex;
		
		distancesSummaryPtr->endIndex = previousIndex;
		distancesSummaryPtr->freeIndex = endIndex;
		
		}	// end "if (removeSmallestDistanceFlag)" 							
	
	else if (distancesSummaryPtr->numberLabeledValues < (UInt32)distancesToStore)
		{ 	
				// Update the count of the number of feature combinations  			
				// that have been loaded into memory.			
			
		distancesSummaryPtr->numberLabeledValues++;
		distancesListPtr[sumIndex].listIndex = sumIndex;
		distancesSummaryPtr->freeIndex = distancesSummaryPtr->numberLabeledValues;
		
		listIndex = sumIndex;
		
		}	// end "else if (...->numberLabeledValues < (UInt32)distancesToStore)"
	
			// If the data for the input pixel is to be saved. ...
			// Get the index into the list vector and save the data values for
			// the input labeled pixel.
	
	if (listIndex >= 0)
		{			
		listIndex *= numberChannels;
	
		BlockMoveData (inputBufferPtr, 
							&labeledDataPtr[listIndex], 
							numberChannels*sizeof (SInt32));
								
		}	// end "if (listIndex >= 0)"
				
}	// end "UpdateSeparabilityLists" 
