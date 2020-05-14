//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						SClassifyDialogs.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			05/12/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that handle classification
//								dialogs.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"  

#if defined multispec_wx    
	#include "xClassifyCEMDialog.h" 
	#include "xClassifyCorrelationDialog.h"
	#include "xClassifyDialog.h"
	#include "xClassifyEchoDialog.h"
   #include "xClassifyKNNDialog.h"
   #include "xClassifySVMDialog.h"
	#include "xListResultsOptionsDialog.h"
#endif	// defined multispec_wx 

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDC_FeatureTransformation		6
	#define	IDC_ChannelPrompt					7
	#define	IDC_TargetCombo					10
	#define	IDC_Training						13
	#define	IDC_TestAreas						14
	#define	IDC_ImageWindowOverlay			32
	#define	IDC_ImageOverlayCombo			39
	#define	IDC_PalettePrompt					40
	#define	IDC_PaletteCombo					41
	#define	IDC_TrainingLOO					43
	#define	IDS_Classify67						67
#endif	// defined multispec_mac || defined multispec_mac_swift    
  
#if defined multispec_win  
	#include	"SMultiSpec.h" 
	#include "WClassifyDialog.h" 
	#include "WClassifyCEMDialog.h"
	#include "WClassifyCorrelationDialog.h"
	#include "WClassifyEchoDialog.h" 
   #include "WClassifyKNNDialog.h"
   #include "WClassifySVMDialog.h"
	#include "WListResultsOptionsDialog.h"   
#endif	// defined multispec_win   
	
		// Classify Output File Format Popup Menu; Note these are 1 based.
		                            
#define	kClassifyOutputASCIIMenuItem			1
#define	kClassifyOutputERDAS74MenuItem		2
#define	kClassifyOutputGAIAMenuItem			3 
#define	kClassifyOutputTIFFGeoTIFFMenuItem	4

SInt16			gCorrelationMatrixClassAreaSelection = kAllMenuItem;
SInt16			gCovarianceEstimate = 0;
SInt16			gEchoAlgorithmProcedure = 0;
									
									
#if defined multispec_wx || defined multispec_mac || defined multispec_win
	typedef struct 	DecisionTreeVar 
		{
		double				startThreshold;
		double				minimumThreshold;
		double				thresholdIncrement;
		
		SInt16				classifyAlgorithm;
		SInt16				distanceAlgorithm;
		SInt16				nodeProcedure;
		
		}	DecisionTreeVar, *DecisionTreeVarPtr;  
#endif


                                 	
								
			// Prototypes for routines in this file that are only called by		
			// other routines in this file.

Boolean CheckIfClassesToUseForClassificationChanged (
				SInt16								newClassSelection,
				UInt32								newNumberClasses,
				UInt16*								newClassPtr,
				SInt16								previousClassSelection,
				UInt32								previousNumberClasses,
				Handle								previousClassHandle);

Boolean ClassifyDialog (
				FileInfoPtr							fileInfoPtr);
								
Boolean CorrelationClassifyDialog (
				SInt16*								covarianceEstimatePtr);

Boolean DetermineIfChannelClassWeightListChanged (
				SInt16								channelSelection,
				Boolean								featureTransformationFlag,
				UInt16								numberAllChannels,
				UInt16								newNumberFeatures,
				UInt16*								newFeaturesPtr,
				SInt16								channelSet,
				UInt16								currentNumberFeatures,
				Handle								currentFeatureHandle,
				UInt16								numberChannels,
				Handle								channelsHandle,
				SInt16								classSelection,
				UInt32								newNumberClasses,
				UInt16*								newClassPtr,
				SInt16								classSet,
				UInt32								currentNumberClasses,
				Handle								currentClassHandle,
				SInt16								weightsSelection,
				float*								classWeightsPtr);

PascalVoid DrawClassificationProcedurePopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
								
PascalVoid DrawCorrelationMatrixClassAreaPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);
								
PascalVoid DrawCovarianceEstimatePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawEchoAlgorithmPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);
											
PascalVoid DrawDiskFilePopUp (
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
								
Boolean DecisionTreeDialog (void);

Boolean KNNClassifyDialog (
				SInt16*								nearestNeighborKValuePtr);
								
Boolean LoadCEMParameterSpecs (
				UInt16								classifyProcedureEnteredCode);

Boolean SVMClassifyDialog ();
								
								
		// Global variables for file.
		
SInt16								gfile_EntireIconItem;


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CEMClassifyDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the CEM classification
//							specifications.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/28/1997
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

Boolean CEMClassifyDialog (
				UInt16*								classifyProcedureEnteredCodePtr)

{	
	CEMParametersPtr					cemParametersPtr;  
										
	Boolean								returnFlag = FALSE;                                          
	
	
			// Make certain that we have memory for the CEM parameter				
			// structure.  If not get memory and initialize the parameters.		
	
	if (!LoadCEMParameterSpecs (*classifyProcedureEnteredCodePtr))
																				return (FALSE);
	                                              	
	cemParametersPtr = (CEMParametersPtr)GetHandlePointer (
															gClassifySpecsPtr->cemParametersH,
															kLock);
	
#if defined multispec_mac  								
	DialogSelectArea						dialogSelectArea;
	
	DialogPtr								dialogPtr;
	 
	SInt16									*correlationMatrixClassPtr,
												*localClassAreaPtr;
												
	UserItemUPP								drawCorrelationMatrixClassesPtr;
	
	Handle									okHandle,
												theHandle;
												
	Rect										theBox;
	
	SInt32									theNum;
												
	UInt32									localNumberClassAreas;
	
	SInt16									controlValue,
												entireIconItem,
												itemHit,
												theType;
	
	Boolean									modalDone;
												
	
	dialogPtr = NULL;
	
			// Get memory for the local class area vector and initialize it.
		
	localClassAreaPtr = (SInt16*)MNewPointer (
						gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16));
												
												
			// Get the modal dialog for the reformat specification					
	
	if (localClassAreaPtr != NULL)			
		dialogPtr = LoadRequestedDialog (kCEMParameterDialogID, kCopyScrap, 1, 2);
		
	if (dialogPtr == NULL)	
		{
		CheckAndDisposePtr (localClassAreaPtr);										
																					return (FALSE); 
																					
		}	// end "if (dialogPtr == NULL)"
		
			// Intialize local user item proc pointers.	
			
	drawCorrelationMatrixClassesPtr =
										NewUserItemUPP (DrawCorrelationMatrixClassAreaPopUp);
				
			// Initialize selected area structure.
			
	if (gAppearanceManagerFlag)
		{
		entireIconItem = 19;
		HideDialogItem (dialogPtr, 9);
		
		}	// if (gAppearanceManagerFlag)
		
	else	// !gAppearanceManagerFlag
		{
		entireIconItem = 9;
		HideDialogItem (dialogPtr, 19);
		
		}	// else !gAppearanceManagerFlag

	InitializeDialogSelectArea (&dialogSelectArea,
											gImageWindowInfoPtr,
											gProjectSelectionWindow,
											cemParametersPtr->columnStart,
											cemParametersPtr->columnEnd,
											cemParametersPtr->columnInterval,
											cemParametersPtr->lineStart,
											cemParametersPtr->lineEnd,
											cemParametersPtr->lineInterval,
											12,
											entireIconItem,
											kAdjustToBaseImage);
											
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Indicate whether the class areas are to be used to compute the
			// correlation matrix.
	
	controlValue = (cemParametersPtr->correlationMatrixCode & kBothFieldTypes)  ? 1 : 0;
	SetDLogControl (dialogPtr, 4, controlValue);
	ShowHideDialogItem (
				dialogPtr, 4, (gProjectInfoPtr->statisticsCode == kMeanCovariance));
	ShowHideDialogItem (
				dialogPtr, 5, (cemParametersPtr->correlationMatrixCode & kBothFieldTypes));
	
			// Use all or subset of train fields to compute the correlation matrix							
			//	Make all classes the default													
			
	SetDialogItemDrawRoutine (dialogPtr, 5, drawCorrelationMatrixClassesPtr);
	gCorrelationMatrixClassAreaSelection = 
										cemParametersPtr->correlationMatrixClassAreaSet;
										
			// Note that the handle is already locked.
			
	correlationMatrixClassPtr = (SInt16*)GetHandlePointer (
										cemParametersPtr->correlationMatrixClassHandle);
								
	BlockMoveData (correlationMatrixClassPtr, 
						localClassAreaPtr, 
						gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16));
						
	localNumberClassAreas = cemParametersPtr->numbercorrelationMatrixClasses;
										
			// Hide these options for now. Will always use the training field.
	
	HideDialogItems (dialogPtr, 6, 7);
	
			// Indicate whether the selected area is to be used to compute the
			// correlation matrix.
	
	controlValue = 0;
	if (cemParametersPtr->correlationMatrixCode & kAreaType)
		controlValue = 1;
	
	SetDLogControl (dialogPtr, 8, controlValue);

			// To entire image icon.															
			//	Selected area to use to compute the correlation matrix.	
	
	LoadLineColumnItems (&dialogSelectArea, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
		
	ShowHideDialogItem (dialogPtr,
								entireIconItem,
								(cemParametersPtr->correlationMatrixCode & kAreaType));
	ShowHideDialogItems (
				dialogPtr, 10, 17, (cemParametersPtr->correlationMatrixCode & kAreaType));
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kCEMParameterDialogID, kSetUpDFilterTable);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 12, 0, SHRT_MAX);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				//theNum = CFStringGetIntValue (gTextString);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 4:
				case 8:
					SetDLogControl (dialogPtr, 4, (itemHit==4));
					SetDLogControl (dialogPtr, 8, (itemHit==8));
						
					ShowHideDialogItem (dialogPtr, 5, (itemHit==4));
					ShowHideDialogItem (dialogPtr, entireIconItem, (itemHit == 8));
					ShowHideDialogItems (dialogPtr, 10, 18, (itemHit == 8));
					
					if (itemHit == 8)
						SelectDialogItemText (dialogPtr, 12, 0, SHRT_MAX);
					break;
					
				case 5:		// Class areas 
					itemHit = StandardPopUpMenu (dialogPtr, 
															4, 
															5, 
															gPopUpAllSubsetMenu, 
															gCorrelationMatrixClassAreaSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of classes to be used.								
								
						itemHit = ClassDialog (&localNumberClassAreas,
											(SInt16*)localClassAreaPtr, 
											1,
											gProjectInfoPtr->numberStatisticsClasses,
											-1,
											okHandle);
							
						}	// end "if (itemHit == kSubsetMenuItem)"
					
					if (itemHit != 0)
						gCorrelationMatrixClassAreaSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 9:				// Entire area to selected area switch.			
				case 12:				//	 background correlation matrix lineStart  
				case 13:				//	 cluster from lineEnd  
				case 14:				//	 cluster from lineInterval  
				case 15:				//	 cluster from columnStart  
				case 16:				//	 cluster from columnEnd  
				case 17:				//	 cluster from columnInterval  
				case 19:				// Entire area to selected area switch.
										// (for Appearance Manager)
					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
					// If item hit is 1, check if background correlation matrix line-column 	
					// values make sense.  If they don't, sound an alert and make 	
					// item  hit equal to 0 to allow user to make changes.			
					
			if (itemHit == 1 && GetDLogControl (dialogPtr, 8) == 1)
				itemHit = CheckLineColValues (
										&dialogSelectArea,
										cemParametersPtr->lineStart,
										cemParametersPtr->columnStart,
										dialogPtr);
										
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE; 
				
				CEMClassifyDialogOK (cemParametersPtr,
												&dialogSelectArea,
												correlationMatrixClassPtr,
												localClassAreaPtr,
												localNumberClassAreas,
												gCorrelationMatrixClassAreaSelection,
												(GetDLogControl (dialogPtr, 4) == 1),
												classifyProcedureEnteredCodePtr);
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		}	while (!modalDone);
		
	DisposeUserItemUPP (drawCorrelationMatrixClassesPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable); 
		
	CheckAndDisposePtr (localClassAreaPtr);	
#endif	// defined multispec_mac 

	#if defined multispec_win   
		CMCEMClassifyDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMCEMClassifyDialog ();
			
			returnFlag = dialogPtr->DoDialog (
											cemParametersPtr, classifyProcedureEnteredCodePtr);
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL  
	#endif	// defined multispec_win 

	#if defined multispec_wx
		CMCEMClassifyDialog* dialogPtr = NULL;
		dialogPtr = new CMCEMClassifyDialog ();
		if (dialogPtr != NULL)
			{
			returnFlag = dialogPtr->DoDialog (
											cemParametersPtr, classifyProcedureEnteredCodePtr);
			delete dialogPtr;
			}
	#endif	// defined multispec_wx
	
	CheckAndUnlockHandle (gClassifySpecsPtr->cemParametersH); 
	
	return (returnFlag);
	
}	// end "CEMClassifyDialog"  



void CEMClassifyDialogOK (
				CEMParametersPtr					cemParametersPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16*								correlationMatrixClassPtr,
				SInt16*								localClassAreaPtr,
				SInt16								localNumberClassAreas,
				SInt16								correlationMatrixClassAreaSelection,
				Boolean								useClassAreasFlag,
				UInt16*								classifyProcedureEnteredCodePtr)

{                                          
				
			// Classes or selected area to be used to compute the correlation
			// matrix.
						
	if (useClassAreasFlag)
		{	
		cemParametersPtr->correlationMatrixCode = kTrainingType;
						
				// All or subset of classes to be used											
				
		cemParametersPtr->correlationMatrixClassAreaSet = 
												correlationMatrixClassAreaSelection;
					
				// Save any new class vector information.
																	
		cemParametersPtr->numbercorrelationMatrixClasses = localNumberClassAreas;
		BlockMoveData (localClassAreaPtr,
					correlationMatrixClassPtr, 
					gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16));
				
		if (correlationMatrixClassAreaSelection == kAllMenuItem)
			LoadTrainClassVector (
						&cemParametersPtr->numbercorrelationMatrixClasses,
						&cemParametersPtr->correlationMatrixClassAreaSet, 
						correlationMatrixClassPtr);
									
		}	// end "if (useClassAreasFlag)"
					
	else	// use selected area
		{
		cemParametersPtr->correlationMatrixCode = kAreaType;
				
				// Selected area to be used for computing the correlation matrix.												
					
		cemParametersPtr->lineStart = dialogSelectAreaPtr->lineStart;
		cemParametersPtr->lineEnd = dialogSelectAreaPtr->lineEnd;
		cemParametersPtr->lineInterval = dialogSelectAreaPtr->lineInterval;
																
		cemParametersPtr->columnStart = dialogSelectAreaPtr->columnStart;
		cemParametersPtr->columnEnd = dialogSelectAreaPtr->columnEnd;
		cemParametersPtr->columnInterval = dialogSelectAreaPtr->columnInterval;
					
		}	// end "else use selected area" 
		
			// Indicate that the user has set the area to be used. Do not used defaults
			// if the user re-enters this dialog box from the same classify dialog box
			// session.
		
	*classifyProcedureEnteredCodePtr = 1;
	
}	// end "CEMClassifyDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfClassesToUseForClassificationChanged
//
//	Software purpose:	The purpose of this routine is to Check if the classes to be used
//							has changed.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:  	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/12/2019
//	Revised By:			Larry L. Biehl			Date: 08/12/2019

Boolean CheckIfClassesToUseForClassificationChanged (
				SInt16								newClassSelection,
				UInt32								newNumberClasses,
				UInt16*								newClassPtr,
				SInt16								previousClassSelection,
				UInt32								previousNumberClasses,
				Handle								previousClassHandle)

{
	UInt16* 							previousClassPtr;
	
	UInt32							index;
	
	Boolean							classListChangedFlag = FALSE;
	
	
	if (newClassSelection != previousClassSelection ||
			newNumberClasses != previousNumberClasses)
		classListChangedFlag = TRUE;

	else if (newClassSelection == kSubsetMenuItem)
		{
				// Verify that the class list is the same.
		
		previousClassPtr = (UInt16*)GetHandlePointer (previousClassHandle);
		
		for (index=0; index<newNumberClasses; index++)
			{
			if (previousClassPtr[index] != newClassPtr[index])
				{
				classListChangedFlag = TRUE;
				break;
				
				}	// end "if (previousClassPtr[index] == newClassPtr[index])"
			
			}	// end "for (index=0; index<newNumberClasses; index++)"
		
		}	// end "else classSelection == kSubsetMenuItem"
	
	return (classListChangedFlag);

}	// end "CheckIfClassesToUseForClassificationChanged"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassifyDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the classification
//							specifications. 
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:		
//
// Value Returned:  	None
//
// Called By:			ClassifyControl   in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/06/1988
//	Revised By:			Larry L. Biehl			Date: 05/04/2019

Boolean ClassifyDialog (
				FileInfoPtr							fileInfoPtr)

{  							
	Boolean								returnFlag = FALSE;
	
#if defined multispec_mac
	DialogSelectArea					dialogSelectArea;
	                  
	double								saveAngleThreshold,
											saveCEMThreshold,
											saveCorrelationThreshold,
											saveThresholdPercent, 
											thresholdValue;
	
	Rect									diskFilePopUpBox,
											theBox,
											theChannelPopupBox,
											weightsPopUpBox;
	
	DialogPtr							dialogPtr;
	
	float*								localClassWeightsPtr = NULL;
	
	UInt16								*localActiveFeaturesPtr = NULL,
											*localClassPtr = NULL,
											*localClassAreaPtr = NULL,
											*localFeaturesPtr = NULL,
											*localTransformFeaturesPtr = NULL;
	
	Handle								okHandle,
											targetWindowInfoHandle,
											theHandle;
											
	unsigned char*						localSymbolsPtr;
								
	SignedByte							handleStatus;
									
	UserItemUPP							drawClassificationProcedurePtr,
											drawChannelsPopUp2Ptr,
											drawDialogClassAreaPopUpPtr,
											drawDiskFilePopUpPtr,
											drawImageOverlayPopUpPtr,
											drawPalettePopUpPtr;
	
	SInt32								theNum;
								
	UInt32								localNumberClassAreas,
											localNumberClasses;
	
	SInt16								covarianceEstimate,
											itemHit,
											itemHit2, 
											listResultsTestCode,
											listResultsTrainingCode,
											lOutputAsciiCode,
											parallelPipedCode,
											theType;
								
	UInt16								classifyProcedureEnteredCode,
											localActiveNumberFeatures,
											localNumberFeatures,
											localNumberTransformFeatures,
											numberEigenvectors;

	Boolean								checkFeatureTransformFlag,
											checkLeaveOneOutFlag,
											checkOKFlag,
											checkThresholdItemsFlag,
											createImageOverlayFlag,
											createProbabilityFileFlag,
											diskFileFlag,
											featureTransformAllowedFlag,
											featureTransformationFlag,
											imageAreaFlag,
											leaveOneOutSettingFlag,
											modalDone,
											optionKeyFlag,
											testAreaFlag,
											thresholdAllowedFlag,
											thresholdResultsFlag,
											trainingAreaFlag,
											trainingFieldsExistFlag,
											updateDialogWindowFlag,
											userLeaveOneOutFlag;
	
	
			// Initialize local variables.													
			
	dialogPtr = NULL;
	
	returnFlag = GetDialogLocalVectors (&localFeaturesPtr,
														&localTransformFeaturesPtr,
														&localClassPtr,
														&localClassAreaPtr,
														&localClassWeightsPtr,
														&localSymbolsPtr,
														NULL,
														NULL);
	
			// Get the modal dialog for the classify specification					
		
	if (returnFlag)			
		dialogPtr = LoadRequestedDialog (kClassifySpecificationID, kCopyScrap, 1, 2);
			
	if (dialogPtr == NULL)
		{
		ReleaseDialogLocalVectors (localFeaturesPtr,
											localTransformFeaturesPtr,
											localClassPtr,
											localClassAreaPtr,
											localClassWeightsPtr,
											localSymbolsPtr,
											NULL,
											NULL);											
																				return (FALSE);
																				
		}	// end "if (dialogPtr == NULL)" 
		
			// Intialize local user item proc pointers.	
			
	drawClassificationProcedurePtr = NewUserItemUPP (DrawClassificationProcedurePopUp);
	drawChannelsPopUp2Ptr = NewUserItemUPP (DrawChannelsPopUp2);
	drawDialogClassAreaPopUpPtr = NewUserItemUPP (DrawDialogClassAreaPopUp);
	drawDiskFilePopUpPtr = NewUserItemUPP (DrawDiskFilePopUp);
	drawImageOverlayPopUpPtr = NewUserItemUPP (DrawImageOverlayPopUp);
	drawPalettePopUpPtr = NewUserItemUPP (DrawPalettePopUp);
	
			// Set Procedure pointers for those dialog items that need them.
	
	SetDialogItemDrawRoutine (dialogPtr, 5, drawClassificationProcedurePtr);				
	SetDialogItemDrawRoutine (dialogPtr, 8, &theChannelPopupBox, drawChannelsPopUp2Ptr);	
	SetDialogItemDrawRoutine (dialogPtr, 10, gDrawImageFilePopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 12, drawDialogClassAreaPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 26, gDrawDialogClassPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 28, &weightsPopUpBox, gDrawWeightsPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 30, gDrawSymbolsPopUpPtr);
	SetDialogItemDrawRoutine (dialogPtr, 34, &diskFilePopUpBox, drawDiskFilePopUpPtr);	
	SetDialogItemDrawRoutine (dialogPtr, 39, drawImageOverlayPopUpPtr);	
	SetDialogItemDrawRoutine (dialogPtr, 41, drawPalettePopUpPtr);
	
			// Initialize dialog variables.
	
	ClassifyDialogInitialize (dialogPtr,
										localFeaturesPtr,
										localTransformFeaturesPtr,
										localClassPtr,
										localClassAreaPtr,
										localSymbolsPtr,
										&gClassificationProcedure,
										&thresholdAllowedFlag,
										&covarianceEstimate,
										&numberEigenvectors,
										&featureTransformAllowedFlag,
										&featureTransformationFlag,
										&gChannelSelection,
										&localActiveNumberFeatures,
										&gFileNamesSelection,
										&targetWindowInfoHandle,
										&gClassAreaSelection,
										&localNumberClassAreas,
										&trainingAreaFlag,
										&trainingFieldsExistFlag,
										&userLeaveOneOutFlag,
										&leaveOneOutSettingFlag,
										&testAreaFlag,
										&imageAreaFlag,
										&dialogSelectArea,
										&gClassSelection,
										&localNumberClasses,
										&gWeightsSelection,
										&gSymbolSelection,
										&lOutputAsciiCode,
										&createImageOverlayFlag,
										&gSelectImageOverlaySelection,
										&gOutputFormatCode,
										&diskFileFlag,
										&gPaletteSelection,
										&thresholdResultsFlag,
										&createProbabilityFileFlag,
										&saveThresholdPercent,
										&saveAngleThreshold,
										&saveCorrelationThreshold,
										&saveCEMThreshold,
										&saveKNNThreshold,
										&listResultsTestCode,
										&listResultsTrainingCode,
										&parallelPipedCode);
	
			// Set feature/transform feature parameters  

	InitializeDialogFeatureParameters (featureTransformationFlag,
													localActiveNumberFeatures, 
													gProjectInfoPtr->numberStatisticsChannels, 
													gTransformationMatrix.numberFeatures,
													localFeaturesPtr,
													localTransformFeaturesPtr,
													&localNumberFeatures,
													&localNumberTransformFeatures,
													&localActiveFeaturesPtr);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Set the check box for the feature transformation option.	
		
	SetDLogControl (dialogPtr, 6, featureTransformationFlag);
												
			// Training fields (Resubstitution and Leave-One-Out Methods)																
			// Unhilite these controls if there are no training fields for the 		
			// project. 																			
			
	SetDLogControl (dialogPtr, 
							13, 
							(SInt16)gClassifySpecsPtr->trainingFldsResubstitutionFlag);
										
	if (!trainingFieldsExistFlag)
		SetDLogControl (dialogPtr, 13, 0);
	
			// Test fields																			
			// Unhilite this control if there are no test fields for the project 
			
	SetDLogControl (dialogPtr, 14, (SInt16)testAreaFlag);
				
			// Image file																			
			
	SetDLogControl (dialogPtr, 15, (SInt16)imageAreaFlag);
	
			// Classification output to text output window.								
	
	SetDLogControl (dialogPtr, 32, (SInt16)createImageOverlayFlag);
	
			// Classification output to disk file.	
		
	SetDLogControl (dialogPtr, 33, (SInt16)diskFileFlag);
	
			// Threshold output data. 
			
			// Threshold percent.																
		
	LoadDItemRealValue (dialogPtr, 36, saveThresholdPercent, 1);
	
			// Thresholds for Correlation Classifier.
			
	LoadDItemRealValue (dialogPtr, 46, saveCorrelationThreshold, 4);
	                                                                         
	LoadDItemRealValue (dialogPtr, 47, saveAngleThreshold, 1);
	
			// Thresholds for CEM Classifier.
			
	LoadDItemRealValue (dialogPtr, 49, saveCEMThreshold, 3);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kClassifySpecificationID, kSetUpDFilterTable);
		
	gDialogItemDescriptorPtr[36] = kDItemReal;
	gDialogItemDescriptorPtr[46] = kDItemReal;
	gDialogItemDescriptorPtr[47] = kDItemReal;
	gDialogItemDescriptorPtr[49] = kDItemReal;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 19, 0, SHRT_MAX);
	/*
	if (GetDLogControl (dialogPtr, 13) +
					GetDLogControl (dialogPtr, 14) +
								GetDLogControl (dialogPtr, 15) +
											GetDLogControl (dialogPtr, 43) == 0) 
		HiliteControl ((ControlHandle)okHandle, 255);
	*/
	classifyProcedureEnteredCode = 0;
	checkFeatureTransformFlag = FALSE;
	checkLeaveOneOutFlag = FALSE;
	checkThresholdItemsFlag = FALSE;
	checkOKFlag = FALSE;
	updateDialogWindowFlag = FALSE;
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 5:		// Classification procedure.
							// If the option key down or the correlation classifier is using
							// covariance information indicate that the selection of the
							// correlation classifier will open a dialog box.
							
	 				optionKeyFlag = FALSE;
					if ((gEventRecord.modifiers & optionKey) > 0)
						{
	 					SetMenuItemText (gPopUpClassifyProcedureMenu, 
	 											kCorrelationMode,
	 											"\pCorrelation (SAM)...");
	 					optionKeyFlag = TRUE;
	 								
	 					}	// end "if ((gEventRecord.modifiers & optionKey) > 0 || ..."
	 								
					itemHit = StandardPopUpMenu (dialogPtr, 
															4, 
															5, 
															gPopUpClassifyProcedureMenu, 
															gClassificationProcedure, 
															kPopUpClassifyProcedureMenuID);
	                
					itemHit = ClassifyDialogOnClassificationProcedure (
															dialogPtr,
															1,
															&thresholdAllowedFlag,
															&featureTransformAllowedFlag,
															&gWeightsSelection,
															&parallelPipedCode,      
															itemHit,
															&covarianceEstimate,
															numberEigenvectors,
															&classifyProcedureEnteredCode,
															correlationComboListItem,
															optionKeyFlag);
															         
					if (covarianceEstimate == kNoCovarianceUsed)
						{                              
						SetMenuItemText (gPopUpClassifyProcedureMenu, 
													kCorrelationMode, 
													"\pCorrelation (SAM)");
						 								
						}	// end "if (covarianceEstimate == kNoCovarianceUsed)"
		
					if (itemHit != 0)
						{ 
						gClassificationProcedure = itemHit; 
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &weightsPopUpBox);
					
						checkFeatureTransformFlag = TRUE;
						checkThresholdItemsFlag = TRUE;
						checkLeaveOneOutFlag = TRUE;
						
						}	// end "if (itemHit != 0)" 
						
							// Make certain that the correct label is drawn in the	
							// classification procedure pop up box.						
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				case 6:		// Feature transformation flag. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					featureTransformationFlag = !featureTransformationFlag;
					checkFeatureTransformFlag = TRUE;
					break;
					
				case 8:		// Channels 
					itemHit = StandardPopUpMenu (dialogPtr, 
															7, 
															8, 
															gPopUpAllAvailableSubsetMenu, 
															gChannelSelection, 
															kPopUpAllAvailableSubsetMenuID);
													
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of channels to be used.							
								
						HiliteControl ((ControlHandle)okHandle, 255);
						ChannelsDialog (
										(SInt16*)&localActiveNumberFeatures,
										(SInt16*)localActiveFeaturesPtr,
										gImageLayerInfoPtr,
										fileInfoPtr,
										1,
										featureTransformationFlag,
										(SInt16*)gProjectInfoPtr->channelsPtr,
										gProjectInfoPtr->numberStatisticsChannels,
										gChannelSelection);
						HiliteControl ((ControlHandle)okHandle, 0);
							
						}	// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						{
						gChannelSelection = UpdateDialogNumberFeatureParameters (
															featureTransformationFlag, 
															&localActiveNumberFeatures,
															&localNumberFeatures,
															gProjectInfoPtr->numberStatisticsChannels, 
															&localNumberTransformFeatures,
															gTransformationMatrix.numberFeatures,
															itemHit);
		
								// Make certain that the correct label is drawn in the	
								// channel pop up box.												
						
						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);

						}	// end "if (itemHit != 0)"
					break;
					
				case 10:				// Target image file popup box 
					if (gFileNamesSelection<0)
						break;
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															itemHit-1, 
															itemHit, 
															gPopUpTemporaryMenu, 
															gFileNamesSelection,
															kColorMenuID);
													
					if (itemHit2 != gFileNamesSelection && itemHit2 != 0)
						{
						gFileNamesSelection = ClassifyDialogOnTargetFile (
																	dialogPtr,
																	itemHit2,
																	&targetWindowInfoHandle,
																	&checkOKFlag,
																	&dialogSelectArea,
																	&createImageOverlayFlag);
																	
						}	// end "if (itemHit2 != gFileNamesSelection ..." 
	
							// Make certain that the correct label is drawn in the	
							// header options pop up box.										
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 12:		// Class areas 
					itemHit = StandardPopUpMenu (dialogPtr, 
															11, 
															12, 
															gPopUpAllSubsetMenu, 
															gClassAreaSelection, 
															kPopUpAllSubsetMenuID);
					if (itemHit == kSubsetMenuItem)
						{
								// Subset of classes to be used.								
								
						itemHit = ClassDialog (&localNumberClassAreas,
															(SInt16*)localClassAreaPtr, 
															1,
															gProjectInfoPtr->numberStatisticsClasses,
															gClassAreaSelection,
															okHandle);
							
						}	// end "if (itemHit == kSubsetMenuItem)" 
					
					if (itemHit != 0)
						gClassAreaSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 13:		// Classify training fields (resubstitution method)
				case 14:		// Classify test fields (holdout method) 
				case 43:		// Classify training fields (leave-one-out method)
				case 15:		// Classify image area. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
						
					if (itemHit == 15)
						{
						imageAreaFlag = 
										(GetControlValue ((ControlHandle)theHandle) == 1);
						if (imageAreaFlag)
							{
							ShowDialogItem (dialogPtr, gfile_EntireIconItem);
							ShowDialogItems (dialogPtr, 17, 24);
							SelectDialogItemText (dialogPtr, 19, 0, SHRT_MAX);
							
							}	// end "if (imageAreaFlag)" 
							
						else	// !imageAreaFlag
							{
							HideDialogItem (dialogPtr, gfile_EntireIconItem);
							HideDialogItems (dialogPtr, 17, 24);
							
							}	// end "else !imageAreaFlag" 
							
						checkThresholdItemsFlag = TRUE;
							
						}	// end "if (itemHit == 15)"
						
					if (itemHit == 43)
						userLeaveOneOutFlag = !userLeaveOneOutFlag;
						
					if (GetDLogControl (dialogPtr, 13) + 
										GetDLogControl (dialogPtr, 14) + 
													GetDLogControl (dialogPtr, 43) > 0)
						ShowDialogItem (dialogPtr,12);
						
					else	// ... <= 0 
						HideDialogItem (dialogPtr,12);
					break;
					
				case 16:				// Entire area to selected area switch.			
				case 19:				//	 lineStart  
				case 20:				//	 lineEnd  
				case 21:				//	 lineInterval  		
				case 22:				//	 columnStart  
				case 23:				//	 columnEnd  
				case 24:				//	 columnInterval  	
				case 50:				// Entire area to selected area switch.
										// (Appearance Manager version)
					dialogSelectArea.imageWindowInfoPtr = (WindowInfoPtr)
								GetHandleStatusAndPointer (targetWindowInfoHandle,
																		&handleStatus);	

					DialogLineColumnHits (&dialogSelectArea,
													dialogPtr, 
													itemHit,
													theHandle,
													theNum);
	
					MHSetState (targetWindowInfoHandle, handleStatus);
					dialogSelectArea.imageWindowInfoPtr = NULL;
					break;
					
				case 26:		// Classes 
					itemHit = StandardPopUpMenu (dialogPtr, 
															25, 
															26, 
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
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 28:		// Use equal weights 
					if (gWeightsSelection < 0)
						break;
					itemHit = StandardPopUpMenu (dialogPtr, 
															27, 
															28, 
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
										gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats);
						HiliteControl ((ControlHandle)okHandle, 0);
						
						itemHit = gWeightsSelection;
							
						}	// end "if (itemHit == kUnequalWeightMenuItem)" 
					
					if (itemHit != 0)
						gWeightsSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// weights pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 30:		// Symbol selection. 
					itemHit2 = StandardPopUpMenu (dialogPtr, 
															29, 
															30, 
															gPopUpSymbolsMenu, 
															gSymbolSelection, 
															kPopUpSymbolsMenuID);
													
					if (itemHit2 == kUserMenuItem)
						{
								// User specified symbols to be used.						
								
						HiliteControl ((ControlHandle)okHandle, 255);
						if (!SymbolsDialog ((UInt16)localNumberClasses,
														(SInt16*)localClassPtr,
														localSymbolsPtr,
														FALSE))
							itemHit2 = gSymbolSelection;
						HiliteControl ((ControlHandle)okHandle, 0);
							
						}	// end "if (itemHit2 == kUserMenuItem)" 
					
					if (itemHit2 != 0)
						gSymbolSelection = itemHit2;
	
							// Make certain that the correct label is drawn in the	
							// symbols pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 32:		// Write classification to image overlay 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					createImageOverlayFlag = !createImageOverlayFlag;
					
					if (createImageOverlayFlag)
						ShowDialogItem (dialogPtr, 39);
					else	// !createImageOverlayFlag
						HideDialogItem (dialogPtr, 39);
					
					ClassifyDialogSetPaletteItems (dialogPtr,
																gOutputFormatCode,
																createImageOverlayFlag);
																
					break;
						
				case 33:		// Write classification to disk file 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					
					gOutputFormatCode = -gOutputFormatCode;
					InvalWindowRect (GetDialogWindow (dialogPtr), &diskFilePopUpBox);
					
					ClassifyDialogSetPaletteItems (dialogPtr,
																gOutputFormatCode,
																createImageOverlayFlag);
					
					checkThresholdItemsFlag = TRUE;
					break;
					
				case 34:		// Disk file format 
					itemHit = DiskFilePopUpMenu (dialogPtr, gPopUpDiskFileMenu, 34);
					if (itemHit != 0)
						{
						gOutputFormatCode = itemHit;
					
						ClassifyDialogSetPaletteItems (dialogPtr,
																	gOutputFormatCode,
																	createImageOverlayFlag);
						
						}	// end "if (itemHit != 0)"
						
					itemHit = 34;
					
					checkThresholdItemsFlag = TRUE;
					break;
					
				case 35:		// Threshold results. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					thresholdResultsFlag = !thresholdResultsFlag;
					checkThresholdItemsFlag = TRUE;
					break;
					
				case 36:				// Threshold percent	
					thresholdValue = GetDItemRealValue (dialogPtr, 36);
					if (thresholdValue > kMaxThreshold)
						RealNumberErrorAlert (saveThresholdPercent, dialogPtr, 36, 1);
					if (thresholdValue >= 0 && thresholdValue <= kMaxThreshold)
						saveThresholdPercent = thresholdValue;
					break;
					
				case 38:		// Create ERDAS probability results file. 
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					createProbabilityFileFlag = !createProbabilityFileFlag;
					break;
					
				case 39:			// select overlay index
					itemHit = StandardPopUpMenu (dialogPtr, 
															32, 
															39, 
															gPopUpImageOverlayMenu, 
															gSelectImageOverlaySelection, 
															kPopUpImageOverlayMenuID);
													
					if (itemHit != 0)
						gSelectImageOverlaySelection = itemHit;
															
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 41:		// Palette Popup menu. 
					SetUpPalettePopUpMenu (dialogPtr);
					itemHit = StandardPopUpMenu (dialogPtr, 
															40, 
															41, 
															gPopUpPaletteMenu, 
															gPaletteSelection, 
															kPopUpPaletteMenuID);
													
					if (itemHit == kFalseColors)
						{
						HiliteControl ((ControlHandle)okHandle, 255);
						if (!FalseColorPaletteDialog ())
							itemHit = 0;
						HiliteControl ((ControlHandle)okHandle, 0);
					
								// Force the main dialog box to be redrawn.
								
						updateDialogWindowFlag = TRUE;
						
						}	// end "if (itemHit == kFalseColors)"
													
					if (itemHit != 0)
						gPaletteSelection = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// palette pop up box.												
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
					
				case 42:		// Results List Options. 
					HiliteControl ((ControlHandle)okHandle, 255);
					ListResultsOptionsDialog (&listResultsTrainingCode,
														&listResultsTestCode);
					HiliteControl ((ControlHandle)okHandle, 0);
					break;
					
				case 46:				// Correlation coefficient threshold	
					thresholdValue = GetDItemRealValue (dialogPtr, 46);
					if (thresholdValue > 1)
						RealNumberErrorAlert (saveCorrelationThreshold, dialogPtr, 46, 4);
					if (thresholdValue >= 0 && thresholdValue <= 1)
						{
						saveCorrelationThreshold = thresholdValue;
						saveAngleThreshold = 
										acos (saveCorrelationThreshold) * kRadiansToDegrees;
						LoadDItemRealValue (dialogPtr, 47, saveAngleThreshold, 1);
				
						}	// end "if (thresholdValue >= 0 && ..."
					break;
					
				case 47:				// Angle threshold	
					thresholdValue = GetDItemRealValue (dialogPtr, 47);
					if (thresholdValue > 90)
						RealNumberErrorAlert (saveAngleThreshold, dialogPtr, 47, 1);
					if (thresholdValue >= 0 && thresholdValue <= 90)
						{
						saveAngleThreshold = thresholdValue;
						saveCorrelationThreshold = cos (
														saveAngleThreshold * kDegreesToRadians);
						LoadDItemRealValue (dialogPtr, 46, saveCorrelationThreshold, 4);
						
						}	// end "if (thresholdValue >= 0 && ..."
					break;
					
				case 49:				// CEM threshold	
					thresholdValue = GetDItemRealValue (dialogPtr, 49);
					if (thresholdValue > 1)
						RealNumberErrorAlert (saveCEMThreshold, dialogPtr, 49, 3);
					if (thresholdValue >= 0 && thresholdValue <= 1)
						saveCEMThreshold = thresholdValue;
					break;
						
				}	// end "switch (itemHit)" 
			/*
			if (checkOKFlag)
				{
				if (GetDLogControl (dialogPtr, 13) +
								GetDLogControl (dialogPtr, 14) +
											GetDLogControl (dialogPtr, 15) +
														GetDLogControl (dialogPtr, 43) == 0) 
					HiliteControl ((ControlHandle)okHandle, 255);
												
				else 		// at least one item set 
					HiliteControl ((ControlHandle)okHandle, 0);
				checkOKFlag = FALSE;
				
				}	// end "if (checkOKFlag)"  
			*/
			if (checkLeaveOneOutFlag)
				checkLeaveOneOutFlag = ClassifyDialogSetLeaveOneOutItems (
																		dialogPtr,
																		gClassificationProcedure,
																		gFileNamesSelection,
																		userLeaveOneOutFlag,
																		trainingFieldsExistFlag,
																		&leaveOneOutSettingFlag);
			
			if (checkFeatureTransformFlag)
				{
				checkFeatureTransformFlag = CheckFeatureTransformationDialog (
																		dialogPtr,
																		featureTransformAllowedFlag,
																		6, 
																		7, 
																		&featureTransformationFlag);

				gChannelSelection = UpdateDialogFeatureParameters (
															featureTransformationFlag, 
															&localActiveNumberFeatures,
															&localActiveFeaturesPtr, 
															localNumberFeatures,
															localFeaturesPtr,
															gProjectInfoPtr->numberStatisticsChannels, 
															localNumberTransformFeatures,
															localTransformFeaturesPtr,
															gTransformationMatrix.numberFeatures);
				
						// Force the channel popup box to be updated.
						
				InvalWindowRect (GetDialogWindow (dialogPtr), &theChannelPopupBox);
															
				}	// end "if (checkFeatureTransformFlag)"
				
					// Make certain that disk file output formats and thresholding 	
					// are consistant with the requested classification 					
					// specifications.	
					
			if (checkThresholdItemsFlag)
				{
				/*
				if (itemHit>=32 && itemHit<=34)
					{
					if (gOutputFormatCode != kClassifyASCIIMenuItem)
						lOutputAsciiCode = (lOutputAsciiCode & 0xfffd);
					else	// gOutputFormatCode == kClassifyASCIIMenuItem
						lOutputAsciiCode = (lOutputAsciiCode | 0x0002);
						
					}	// end "if (itemHit>=32 && itemHit<=34)"
				*/
				checkThresholdItemsFlag = ClassifyDialogSetThresholdItems (
													dialogPtr,
													gClassificationProcedure,
													imageAreaFlag,
													createProbabilityFileFlag,
													thresholdResultsFlag,
													thresholdAllowedFlag);
													
				}	// end "if (checkThresholdItemsFlag)"
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
			if (itemHit == 1 && (GetDLogControl (dialogPtr, 13) +
								GetDLogControl (dialogPtr, 14) +
											GetDLogControl (dialogPtr, 15) +
														GetDLogControl (dialogPtr, 43) == 0))
				{
				HiliteControl ((ControlHandle)okHandle, 255);
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert115,
									0, 
									NULL);
				HiliteControl ((ControlHandle)okHandle, 0);
									
				itemHit = 0;
				updateDialogWindowFlag = TRUE;
										
				}	// end "if (itemHit == 1 && ...)"
				
			imageAreaFlag = (GetDLogControl (dialogPtr, 15) == 1);

					// If item hit is 1, check if classify area line-column values	
					// make sense.  If they don't, sound an alert and make item 	
					// hit equal to 0 to allow user to make changes.					
					
			if (itemHit == 1 && imageAreaFlag)
				itemHit = CheckLineColValues (
										&dialogSelectArea,
										gClassifySpecsPtr->imageLineStart,
										gClassifySpecsPtr->imageColumnStart,
										dialogPtr);
						
					// Make certain that if only training and/or test fields are 	
					// being classified, that the results are not being written		
					// to an ERDAS formatted file.  If so send message to the user.
					
			if (itemHit == 1 && !imageAreaFlag && gOutputFormatCode > 0 &&
												gOutputFormatCode != kClassifyASCIIOutputFormat)
				{
				itemHit = DisplayAlert (
							kErrorAlertID, 1, kClassifyStrID, IDS_Classify2, 0, NULL);
				itemHit = 0;
				
				}	// end "if (itemHit == 1 && !imageAreaFlag && ..."
			
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
				
				ClassifyDialogOK (gClassificationProcedure,
											covarianceEstimate,
											featureTransformationFlag,
											gChannelSelection,
											localActiveNumberFeatures,
											(SInt16*)localActiveFeaturesPtr,
											targetWindowInfoHandle,
											gFileNamesSelection,
											gClassAreaSelection,
											localNumberClassAreas,
											(SInt16*)localClassAreaPtr,
											GetDLogControl (dialogPtr, 13),
											GetDLogControl (dialogPtr, 43),
											GetDLogControl (dialogPtr, 14),
											imageAreaFlag,
											&dialogSelectArea,
											gClassSelection,
											localNumberClasses,
											(SInt16*)localClassPtr,
											gWeightsSelection,
											localClassWeightsPtr,
											gSymbolSelection,
											localSymbolsPtr,
											//GetDLogControl (dialogPtr, 32),
											GetDLogControl (dialogPtr, 33),
											createImageOverlayFlag,
											gSelectImageOverlaySelection,
											gOutputFormatCode,
											GetDLogControl (dialogPtr, 35),
											saveCorrelationThreshold,
											saveAngleThreshold,
											saveCEMThreshold,
											saveThresholdPercent,
											GetDLogControl (dialogPtr, 38),
											gPaletteSelection,
											listResultsTestCode,
											listResultsTrainingCode,
											parallelPipedCode);
											
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
			
		if (updateDialogWindowFlag)
			{		
					// Force the main dialog box to be redrawn.
					
			InvalWindowRect (GetDialogWindow (dialogPtr),
								GetPortBounds (GetDialogPort (dialogPtr), &gTempRect));
								
			updateDialogWindowFlag = FALSE;
								
			}	// end "if (updateDialogWindowFlag)"
				
		}	while (!modalDone);
		
	ReleaseDialogLocalVectors (localFeaturesPtr,
											localTransformFeaturesPtr,
											localClassPtr,
											localClassAreaPtr,
											localClassWeightsPtr,
											localSymbolsPtr,
											NULL,
											NULL);	
		
			// Remove file names from temporary popup menu.								

	for (itemHit=0; itemHit<=gNumberOfIWindows; itemHit++)
		DeleteMenuItem (gPopUpTemporaryMenu, 1);
	
	DisposeUserItemUPP (drawClassificationProcedurePtr);
	DisposeUserItemUPP (drawChannelsPopUp2Ptr);
	DisposeUserItemUPP (drawDialogClassAreaPopUpPtr);
	DisposeUserItemUPP (drawDiskFilePopUpPtr);
	DisposeUserItemUPP (drawPalettePopUpPtr);
	DisposeUserItemUPP (drawImageOverlayPopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac

	#if defined multispec_win   
		CMClassifyDialog*		dialogPtr = NULL;
		
		TRY
			{               
			dialogPtr = new CMClassifyDialog ();
			
			returnFlag = dialogPtr->DoDialog (); 
		                       
			delete dialogPtr;      
			
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL  
	#endif	// defined multispec_win  

	#if defined multispec_wx
		CMClassifyDialog* dialogPtr = NULL;
		dialogPtr = new CMClassifyDialog ();
		if (dialogPtr != NULL)
			{
			returnFlag = dialogPtr->DoDialog ();
			delete dialogPtr;
			}
	#endif	// defined multispec_wx 
	
	return (returnFlag);
	
}	// end "ClassifyDialog"


void ClassifyDialogInitialize (
				DialogPtr							dialogPtr,
				UInt16*								localFeaturesPtr, 
				UInt16*								localTransformFeaturesPtr,
				UInt16*								localClassPtr,
				UInt16*								localClassAreaPtr,
				unsigned char*						localSymbolsPtr,
				SInt16*								classificationProcedurePtr,
				Boolean*								thresholdAllowedFlagPtr,
				SInt16*								covarianceEstimatePtr,
				UInt16*								numberEigenvectorsPtr,
				Boolean*								featureTransformAllowedFlagPtr,
				Boolean*								featureTransformationFlagPtr,
				SInt16*								channelSelectionPtr,
				UInt16*								localNumberChannelsPtr,
				SInt16*								fileNamesSelectionPtr,
				Handle*								targetWindowInfoHandlePtr,
				SInt16*								classAreaSelectionPtr,
				UInt32*								localNumberClassAreasPtr,
				Boolean*								trainingAreaFlagPtr,
				Boolean*								trainingFieldsExistFlagPtr,
				Boolean*								userLeaveOneOutFlagPtr,
				Boolean*								leaveOneOutSettingFlagPtr,
				Boolean*								testAreaFlagPtr,
				Boolean*								imageAreaFlagPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16*								classSelectionPtr,
				UInt32*								localNumberClassesPtr,
				SInt16*								weightsSelectionPtr,
				SInt16*								symbolSelectionPtr,
				SInt16*								outputAsciiCodePtr,
				Boolean*								createImageOverlayFlagPtr,
				SInt16*								selectImageOverlaySelectionPtr,
				SInt16*								outputFormatCodePtr,
				Boolean*								diskFileFlagPtr,
				SInt16*								paletteSelectionPtr,
				Boolean*								thresholdResultsFlagPtr,
				Boolean*								createProbabilityFileFlagPtr,
				double*								saveThresholdPercentPtr,
				double*								saveAngleThresholdPtr,
				double*								saveCorrelationThresholdPtr,
				double*								saveCEMThresholdPtr,
				SInt16*								saveKNNThresholdPtr,
				SInt16*								listResultsTestCodePtr,
				SInt16*								listResultsTrainingCodePtr,
				SInt16*								parallelPipedCodePtr,
				SInt16*								nearestNeighborKValuePtr)
	
{
	SInt16								eigenSource,							
											listCount;
											
	Boolean								mapInfoExistsFlag;
			
	SignedByte							handleStatus;
	
	#if defined multispec_win  
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win
		
			// Load the dialog local vectors

	LoadDialogLocalVectors (localFeaturesPtr,
										localTransformFeaturesPtr,
										gClassifySpecsPtr->featureHandle,
										gClassifySpecsPtr->numberChannels,
										localClassPtr,
										gClassifySpecsPtr->classHandle,
										gClassifySpecsPtr->numberClasses,
										localClassAreaPtr,
										gClassifySpecsPtr->classAreaHandle,
										gClassifySpecsPtr->numberClassAreas,
										localSymbolsPtr,
										gClassifySpecsPtr->symbolsHandle,
										gClassifySpecsPtr->numberClasses,
										NULL,
										NULL,
										0,
										NULL);
	
			// Classification procedure.														
			//	Get the rectangles for the classification procedure prompt and 	
			// popup boxes. 																		
			
	*classificationProcedurePtr = gClassifySpecsPtr->mode;
	*thresholdAllowedFlagPtr = 
						ClassifyDialogGetThresholdAllowedFlag (gClassifySpecsPtr->mode);
	
			// Initialize local parameters used by correlation classifier
	
	*covarianceEstimatePtr = gClassifySpecsPtr->correlationCovarianceCode;
	
			// Set up classification popup menu list
		
			// Set the text for the correlation classifier menu item. If the
			// covariance information is not to be used then do not include the
			// '...', otherwise do.
							
	if (gClassifySpecsPtr->correlationCovarianceCode == kNoCovarianceUsed)
		CtoPstring ((UCharPtr)"Correlation (SAM)", gTextString);
			
	else	// ...correlationCovarianceCode != kNoCovarianceUsed
		CtoPstring ((UCharPtr)"Correlation (SAM)...", gTextString);
					
	#if defined multispec_mac 
		SetMenuItemText (gPopUpClassifyProcedureMenu, 
								kCorrelationMode, 
								gTextString);
	#endif	// defined multispec_mac  
	
	#if defined multispec_win  
		dialogPtr->SetComboItemText (IDC_ClassificationProcedure, 
												kCorrelationMode - 1,
												&gTextString[1]); 
	
		comboBoxPtr = 
					(CComboBox*)dialogPtr->GetDlgItem (IDC_ClassificationProcedure);
		
		comboBoxPtr->SetItemData (0, kMaxLikeMode);
		comboBoxPtr->SetItemData (1, kMahalanobisMode);
		comboBoxPtr->SetItemData (2, kFisherMode);
		comboBoxPtr->SetItemData (3, kEchoMode);
		comboBoxPtr->SetItemData (4, kSupportVectorMachineMode);
		comboBoxPtr->SetItemData (5, kKNearestNeighborMode);
		comboBoxPtr->SetItemData (6, kEuclideanMode);
		comboBoxPtr->SetItemData (7, kCorrelationMode);
		comboBoxPtr->SetItemData (8, kCEMMode);
		comboBoxPtr->SetItemData (9, kParallelPipedMode); 

		if (gProjectInfoPtr->includesStatisticsFromClusterOperationFlag)
			{
			comboBoxPtr->DeleteString (kKNearestNeighborMode-1);
			comboBoxPtr->DeleteString (kSupportVectorMachineMode-1);

			}	// end "if (gProjectInfoPtr->includesStatisticsFromClusterOperationFlag)"

	#endif	// defined multispec_win  		

	#if defined multispec_wx
		int index = 0;

		#if defined multispec_wxlin
			wxComboBox*		procedureCtrl;
			procedureCtrl =
					(wxComboBox*)dialogPtr->FindWindowById (IDC_ClassificationProcedure);
		#endif
		#if defined multispec_wxmac
			wxChoice*		procedureCtrl;
			procedureCtrl =
					(wxChoice*)dialogPtr->FindWindowById (IDC_ClassificationProcedure);
		#endif
	
		procedureCtrl->SetClientData (index, (void*)kMaxLikeMode);
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kMahalanobisMode);
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kFisherMode);
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kEchoMode);
	
		if (!gProjectInfoPtr->includesStatisticsFromClusterOperationFlag)
			{
			index++;
      	procedureCtrl->SetClientData (index, (void*)kSupportVectorMachineMode);
			
			index++;
      	procedureCtrl->SetClientData (index, (void*)kKNearestNeighborMode);
			
      	}	// end "if (!gProjectInfoPtr->includesStatisticsFromClusterOperationFlag)"
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kEuclideanMode);
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kCorrelationMode);
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kCEMMode);
	
		index++;
		procedureCtrl->SetClientData (index, (void*)kParallelPipedMode);
	#endif	// multispec_wx
	
	if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
		{
		#if defined multispec_mac 
			DisableMenuItem (gPopUpClassifyProcedureMenu, kMaxLikeMode);
			DisableMenuItem (gPopUpClassifyProcedureMenu, kMahalanobisMode);
			DisableMenuItem (gPopUpClassifyProcedureMenu, kFisherMode);
			DisableMenuItem (gPopUpClassifyProcedureMenu, kEchoMode);
		#endif	// defined multispec_mac  
		
		#if defined multispec_win                   
			comboBoxPtr->DeleteString (kEchoMode-1);
			comboBoxPtr->DeleteString (kFisherMode-1);
			comboBoxPtr->DeleteString (kMahalanobisMode-1);
			comboBoxPtr->DeleteString (kMaxLikeMode-1);
		#endif	// defined multispec_win 
		 			
		#if defined multispec_wx        
			procedureCtrl->Delete (kEchoMode - 1);
			procedureCtrl->Delete (kFisherMode - 1);
			procedureCtrl->Delete (kMahalanobisMode - 1);
			procedureCtrl->Delete (kMaxLikeMode - 1);
		#endif	// defined multispec_wx		
		
				// Force covariance estimate that is to be used with the correlation 
				// classifier to 'None' since no second order statistics are available.
				
		*covarianceEstimatePtr = kNoCovarianceUsed;
		
		}	// end "if (...->statisticsCode != kMeanCovariance)"
		
			// Set the check box for the feature transformation option.				
			
	EigenvectorInfoExists (kProject, &eigenSource, numberEigenvectorsPtr);
	*featureTransformAllowedFlagPtr = 
						ClassifyDialogGetFeatureTransformAllowedFlag (
										gClassifySpecsPtr->mode, *numberEigenvectorsPtr);
										
	if (*numberEigenvectorsPtr <= 0 || 
									gProjectInfoPtr->statisticsCode != kMeanCovariance)
		{		
		*featureTransformationFlagPtr = FALSE;
		HideDialogItem (dialogPtr, IDC_FeatureTransformation);
		
		}	// end "if (*numberEigenvectorsPtr <= 0 || ...)" 
		
	else	// numberEigenvectors > 0 
		*featureTransformationFlagPtr = gClassifySpecsPtr->featureTransformationFlag;
									
	if (*featureTransformAllowedFlagPtr)
		SetDLogControlHilite (dialogPtr, IDC_FeatureTransformation, 0);
		
	else	// !featureTransformAllowedFlagPtr 
		SetDLogControlHilite (dialogPtr, IDC_FeatureTransformation, 255);
	
	if (*featureTransformationFlagPtr)
		{
		LoadDItemStringNumber (kDialogStrID, 
										IDS_Dialog16, 
										dialogPtr, 
										IDC_ChannelPrompt, 
										(Str255*)gTextString);
		
		}	// end "if (*featureTransformationFlagPtr)"
	
			//	Get the rectangles for the channel prompt and popup boxes.			
			//	Make all channels the default													
			
	*channelSelectionPtr = gClassifySpecsPtr->channelSet;
	*localNumberChannelsPtr = gClassifySpecsPtr->numberFeatures;
	
			// Get the default target file window information handle.				
	
	*targetWindowInfoHandlePtr = gClassifySpecsPtr->targetWindowInfoHandle;
	*fileNamesSelectionPtr = GetImageList (
				dialogPtr, *targetWindowInfoHandlePtr, IDC_TargetCombo, TRUE, &listCount);
		
	if (*fileNamesSelectionPtr <= 1)
		*targetWindowInfoHandlePtr = gProjectInfoPtr->windowInfoHandle;
		
	else  // *fileNamesSelectionPtr > 1 
		{
		#if defined multispec_mac  
			*targetWindowInfoHandlePtr = GetWindowInfoHandle (
								gWindowList [kImageWindowStart+(*fileNamesSelectionPtr)-2]);
		#endif	// defined multispec_mac   
		
		#if defined multispec_win  
			comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_TargetCombo);
			UInt32 windowIndex =
								(UInt32)comboBoxPtr->GetItemData (*fileNamesSelectionPtr-1);
			*targetWindowInfoHandlePtr = GetWindowInfoHandle (gWindowList [windowIndex]);
		#endif	// defined multispec_win   

		#if defined multispec_wx
			wxChoice* targetCtrl = (wxChoice*)dialogPtr->FindWindow (IDC_TargetCombo);
			SInt64 windowIndex64 =
					(SInt64)((int*)targetCtrl->GetClientData (*fileNamesSelectionPtr - 1));
			UInt32 windowIndex = (UInt32)windowIndex64;
			*targetWindowInfoHandlePtr = GetWindowInfoHandle (gWindowList [windowIndex]);
		#endif	// defined multispec_wx
		}	// end "else *fileNamesSelectionPtr > 1" 
	
	if (listCount <= 1)
		*fileNamesSelectionPtr = -*fileNamesSelectionPtr;
	
	#if defined multispec_win || defined multispec_wx		
		if (*fileNamesSelectionPtr <= 0)	 
			HideDialogItem (dialogPtr, IDC_TargetCombo);
			
		else	// *fileNamesSelectionPtr > 0
			HideDialogItem (dialogPtr, IDC_TargetBase);
	#endif	// defined multispec_win || ...
	
			// Classifiy all or subset of train/test fields.							
			//	Make all classes the default													
			
	*classAreaSelectionPtr = gClassifySpecsPtr->classAreaSet;
	*localNumberClassAreasPtr = gClassifySpecsPtr->numberClassAreas;
												
			// Training fields (Resubstitution and Leave-One-Out Methods)																
			// Unhilite these controls if there are no training fields for the 		
			// project. 																			
			
	*trainingAreaFlagPtr = gClassifySpecsPtr->trainingFldsResubstitutionFlag;
	
	SInt16 lastClassIndex	= -1;			
	SInt16 nextArea = GetNextFieldArea (
										gProjectInfoPtr,
										NULL, 
										gProjectInfoPtr->numberStatisticsClasses, 
										&lastClassIndex, 
										-1, 
										kTrainingType,
										kDontIncludeClusterFields);
										
	*trainingFieldsExistFlagPtr = (nextArea != -1);
	if (!*trainingFieldsExistFlagPtr || *fileNamesSelectionPtr >= 2)
		{
		*trainingAreaFlagPtr = FALSE;
		SetDLogControlHilite (dialogPtr, IDC_Training, 255);
			
		}	// end "if (!*trainingFieldsExistFlagPtr || ..."
		
	ClassifyDialogSetLeaveOneOutItems (
												dialogPtr,
												gClassifySpecsPtr->mode,
												*fileNamesSelectionPtr,
												gClassifySpecsPtr->trainingFldsLOOFlag,
												*trainingFieldsExistFlagPtr,
												leaveOneOutSettingFlagPtr);
	
	*userLeaveOneOutFlagPtr = *leaveOneOutSettingFlagPtr;
	
			// Test fields																			
			// Unhilite this control if there are no test fields for the project 
			
	*testAreaFlagPtr = gClassifySpecsPtr->testFldsFlag;
	if (gProjectInfoPtr->numberStatTestFields <= 0 || *fileNamesSelectionPtr >= 2)
		{
		*testAreaFlagPtr = FALSE;
		SetDLogControlHilite (dialogPtr, IDC_TestAreas, 255);
		
		}	// end "if (gProjectInfoPtr->numberStatTestFields <= 0 || ..."
	
			// Hide the class area popup if not training or test are to be
			// classified.
												
	#if defined multispec_mac  
		if (!*trainingAreaFlagPtr && 
						!*testAreaFlagPtr && 
									!*leaveOneOutSettingFlagPtr)
			HideDialogItem (dialogPtr,12);
	#endif	// defined multispec_mac  	
	
	#if defined multispec_win || defined multispec_wx
		((CMClassifyDialog*)dialogPtr)->HideShowClassAreasItem ();
	#endif	// defined multispec_win || ...
															
			//	Selected area to classify.										
			          
	*imageAreaFlagPtr = gClassifySpecsPtr->imageAreaFlag;
			
	gfile_EntireIconItem = 16;
	if (gAppearanceManagerFlag)
		gfile_EntireIconItem = 50;
	
	InitializeDialogSelectArea (dialogSelectAreaPtr,
											NULL,
											gProjectSelectionWindow,
											gClassifySpecsPtr->imageColumnStart,
											gClassifySpecsPtr->imageColumnEnd,
											gClassifySpecsPtr->imageColumnInterval,
											gClassifySpecsPtr->imageLineStart,
											gClassifySpecsPtr->imageLineEnd,
											gClassifySpecsPtr->imageLineInterval,
											19,
											gfile_EntireIconItem,
											kAdjustToBaseImage);
											
	dialogSelectAreaPtr->imageWindowInfoPtr = 
		(WindowInfoPtr)GetHandleStatusAndPointer (*targetWindowInfoHandlePtr,
																	&handleStatus);	
	
	if (*fileNamesSelectionPtr >= 2)
		{								
		dialogSelectAreaPtr->windowPtr = GetWindowPtr (*targetWindowInfoHandlePtr);
		dialogSelectAreaPtr->adjustToBaseImageFlag = kDontAdjustToBaseImage;
																				
				// Get the selection again if the target file is not the base image.
				// The current line/column values are for the base image.												
		
		if (!GetSelectedAreaInfo (
							dialogSelectAreaPtr->windowPtr,
							dialogSelectAreaPtr->imageWindowInfoPtr,
							(SInt32*)&dialogSelectAreaPtr->lineStart,
							(SInt32*)&dialogSelectAreaPtr->lineEnd,
							(SInt32*)&dialogSelectAreaPtr->columnStart,
							(SInt32*)&dialogSelectAreaPtr->columnEnd,
							kDontClearSelectionArea,
							kUseThreshold,
							dialogSelectAreaPtr->adjustToBaseImageFlag))
	   	{
			dialogSelectAreaPtr->lineStart = 1;
			dialogSelectAreaPtr->lineEnd = 
											GetMaxNumberLines (*targetWindowInfoHandlePtr);
			dialogSelectAreaPtr->columnStart = 1;
			dialogSelectAreaPtr->columnEnd = 
											GetMaxNumberColumns (*targetWindowInfoHandlePtr);
								
			}	// end "if (!GetSelectedAreaInfo (..."
							
		}	// end "if (*fileNamesSelectionPtr >= 2)"                                      
				
			// Image file	
											
	#if defined multispec_mac 
		LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1); 
		
		if (gAppearanceManagerFlag)
			HideDialogItem (dialogPtr, 16);
			
		else	// !gAppearanceManagerFlag
			HideDialogItem (dialogPtr, 50);
	
		if (*imageAreaFlagPtr)
			{
			ShowDialogItem (dialogPtr, gfile_EntireIconItem);
			ShowDialogItems (dialogPtr, 17, 24);
			
			}	// end "if (*imageAreaFlagPtr)"
			
		else	// !*imageAreaFlagPtr 
			{
			HideDialogItem (dialogPtr, gfile_EntireIconItem);
			HideDialogItems (dialogPtr, 17, 24);
			
			}	// end "else !*imageAreaFlagPtr"
	#endif	// defined multispec_mac  	
			
	#if defined multispec_win || defined multispec_wx
		LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kDoNotInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1); 
	                                      
		dialogPtr->HideShowAreaItems (*imageAreaFlagPtr);
	#endif	// defined multispec_win || ... 
		
	MHSetState (*targetWindowInfoHandlePtr, handleStatus);
	dialogSelectAreaPtr->imageWindowInfoPtr = NULL;
	
			// Classes to use.													
			
	*classSelectionPtr = gClassifySpecsPtr->classSet;
	*localNumberClassesPtr = gClassifySpecsPtr->numberClasses;
	
			// Class weights.																		
			//	Equal weights will always be used for minimum distance
	
	*weightsSelectionPtr = UpdateDialogClassWeightsInfo (
								kEqualWeights,
								gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
								NULL,
								TRUE);
								
	if (*classificationProcedurePtr == kEuclideanMode ||
				*classificationProcedurePtr == kCorrelationMode ||
						*classificationProcedurePtr == kCEMMode ||
							*classificationProcedurePtr == kParallelPipedMode)
		*weightsSelectionPtr = -*weightsSelectionPtr;
	
			// Symbols to use.																	
			
	*symbolSelectionPtr = gClassifySpecsPtr->symbolSet;
	
			// Classification output to text output window.								
	
	*outputAsciiCodePtr = gClassifySpecsPtr->outputStorageType;
		
			// Classification output image window overlay.
	
	*createImageOverlayFlagPtr = FALSE;
	gProjectInfoPtr->overlayImageWindowInfoHandle = 
												GetTargetOverlayImageWindowInfoHandle (
																			*fileNamesSelectionPtr,
																			*targetWindowInfoHandlePtr);
	
	if (gProjectInfoPtr->overlayImageWindowInfoHandle != NULL)
		{
		if (gClassifySpecsPtr->outputStorageType & kCreateImageOverlayCode)
			*createImageOverlayFlagPtr = TRUE;
			
		}	// end "if (...->overlayImageWindowInfoHandle != NULL)"
						
	else	// ...->overlayImageWindowInfoHandle== NULL
		SetDLogControlHilite (dialogPtr, IDC_ImageWindowOverlay, 255);
					
	if (*createImageOverlayFlagPtr)
		ShowDialogItem (dialogPtr, IDC_ImageOverlayCombo);
		
	else	// !*createImageOverlayFlagPtr
		HideDialogItem (dialogPtr, IDC_ImageOverlayCombo);	
	
	*selectImageOverlaySelectionPtr = GetWindowImageOverlayIndex (
													gProjectInfoPtr->overlayImageWindowInfoHandle,
													gClassifySpecsPtr->imageOverlayIndex) + 2;
				
	#if defined multispec_win 
		comboBoxPtr = (CComboBox*)(dialogPtr->GetDlgItem (IDC_ImageOverlayCombo));
		gPopUpImageOverlayMenu = (MenuHandle)comboBoxPtr;
	#endif	// defined multispec_win			

	#if defined multispec_wx 
		wxChoice*		overlayCtrl;
		overlayCtrl = (wxChoice*)(dialogPtr->FindWindow (IDC_ImageOverlayCombo));
		gPopUpImageOverlayMenu = (MenuHandle)overlayCtrl;
	#endif 

	SetUpImageOverlayPopUpMenu (gPopUpImageOverlayMenu, 
											gProjectInfoPtr->overlayImageWindowInfoHandle,
											*selectImageOverlaySelectionPtr);
													
			// Set text indicating whether the output file format could be GeoTIFF
			// or TIFF
			
	mapInfoExistsFlag = SetTIFF_GeoTIFF_MenuItemString (
												gImageWindowInfoPtr,
												dialogPtr,
												#if defined multispec_mac  
													gPopUpDiskFileMenu,
												#endif	// defined multispec_mac
												#if defined multispec_win || defined multispec_wx
													IDC_DiskCombo,
												#endif	// defined multispec_win || ...
												kClassifyOutputTIFFGeoTIFFMenuItem);
	
			// Classification output to disk file.											
	
	*outputFormatCodePtr = 4;
	if (gImageFileInfoPtr->format == kErdas74Type)
		*outputFormatCodePtr = 2;
		
	if (gClassifySpecsPtr->diskFileFormat == kMultiSpecClassificationType)
		*outputFormatCodePtr = 1;
	else if (gClassifySpecsPtr->diskFileFormat == kErdas74Type)
		*outputFormatCodePtr = 2;
	else if (gClassifySpecsPtr->diskFileFormat == kGAIAType)
		*outputFormatCodePtr = 3;
	else if (gClassifySpecsPtr->diskFileFormat == kTIFFType)
		*outputFormatCodePtr = 4;
		
	*diskFileFlagPtr = FALSE;	         
	if (gClassifySpecsPtr->outputStorageType & 0x0006) 
		*diskFileFlagPtr = TRUE;	         
		
	else	// !(lOutputStorageType & 0x0006)
		*outputFormatCodePtr = -*outputFormatCodePtr;

			// Palette dialog items.
		
	ClassifyDialogSetPaletteItems (dialogPtr,
												*outputFormatCodePtr,
												*createImageOverlayFlagPtr);
	
			// Palette to use. Draw the default palette selection.
	
	*paletteSelectionPtr = gProjectInfoPtr->imagePalettePopupMenuSelection;
	SetUpPalettePopUpMenu (dialogPtr);
	
			// Threshold output data. 															
			// Threshold percent (to be used for output files).						
			
	*thresholdResultsFlagPtr = gClassifySpecsPtr->thresholdFlag;
	
			// Create probabability results file in ERDAS format. 					
			
	*createProbabilityFileFlagPtr = FALSE;		                                                       
	if (*outputAsciiCodePtr & 0x0008)		
		*createProbabilityFileFlagPtr = TRUE;
	
			// Now set the dialog items that depend on thresholding and 
			// creating the probability map.
	
	ClassifyDialogSetThresholdItems (dialogPtr,
													*classificationProcedurePtr,
													*imageAreaFlagPtr,
													*createProbabilityFileFlagPtr,
													*thresholdResultsFlagPtr,
													*thresholdAllowedFlagPtr);
			// Threshold percent.																
		
	*saveThresholdPercentPtr = gClassifySpecsPtr->probabilityThreshold;
	
	*nearestNeighborKValuePtr = gClassifySpecsPtr->nearestNeighborKValue;
	
			// Thresholds for Correlation Classifier.
			
	*saveAngleThresholdPtr = gClassifySpecsPtr->correlationAngleThreshold; 
	*saveCorrelationThresholdPtr = cos (*saveAngleThresholdPtr * kDegreesToRadians);
	
			// Threshold for CEM Classifier.
			
	*saveCEMThresholdPtr = gClassifySpecsPtr->cemThreshold;
	
			// Threshold for KNN Classifier.
	
	*saveKNNThresholdPtr = gClassifySpecsPtr->knnThreshold;
	
			// Codes for training and test listings.
			 
	*listResultsTestCodePtr = gProjectInfoPtr->listResultsTestCode;
	*listResultsTrainingCodePtr = gProjectInfoPtr->listResultsTrainingCode;
	
	*parallelPipedCodePtr = kPPStandardDeviationCode;
																	
}	// end "ClassifyDialogInitialize"



Boolean ClassifyDialogGetFeatureTransformAllowedFlag (
				SInt16								classificationProcedure,
				UInt16								numberEigenvectors)
	
{								
	Boolean featureTransformAllowedFlag = 
							((classificationProcedure == kMaxLikeMode) ||
								(classificationProcedure == kMahalanobisMode) ||
								(classificationProcedure == kFisherMode) ||
								(classificationProcedure == kEchoMode) ||
								(classificationProcedure == kCorrelationMode) ||
								(classificationProcedure == kCEMMode)) && 
																	(numberEigenvectors > 0);
																	
	return (featureTransformAllowedFlag);
																	
}	// end "ClassifyDialogGetFeatureTransformAllowedFlag"



Boolean ClassifyDialogGetThresholdAllowedFlag (
				SInt16								classificationProcedure)
	
{
	Boolean thresholdAllowedFlag = 
										(classificationProcedure == kMaxLikeMode) ||
										(classificationProcedure == kMahalanobisMode) ||
										(classificationProcedure == kFisherMode) ||
										(classificationProcedure == kEchoMode) ||
										(classificationProcedure == kKNearestNeighborMode) ||
										//(classificationProcedure == kEuclideanMode) ||
										(classificationProcedure == kCorrelationMode) ||
										(classificationProcedure == kCEMMode);
										
	return (thresholdAllowedFlag); 
	
}	// end "ClassifyDialogGetThresholdAllowedFlag"



void ClassifyDialogOK (
				SInt16								classificationProcedure,
				SInt16								covarianceEstimate,
				Boolean								featureTransformationFlag,
				SInt16								channelSelection,
				SInt16								localNumberFeatures,
				SInt16*								localFeaturesPtr,
				Handle								targetWindowInfoHandle,
				SInt16								fileNamesSelection,
				SInt16								classAreaSelection,
				SInt16								localNumberClassAreas,
				SInt16*								localClassAreaPtr,
				Boolean								trainingFldsResubstitutionFlag,
				Boolean								trainingFldsLOOFlag,
				Boolean								testFldsFlag,
				Boolean								imageAreaFlag,
				DialogSelectArea*					dialogSelectAreaPtr,
				SInt16								classSelection,
				SInt16								localNumberClasses,
				SInt16*								localClassPtr,
				SInt16								weightsSelection,
				float*								classWeightsPtr,
				SInt16								symbolSelection,
				unsigned char*						localSymbolsPtr,
				Boolean								diskFileFlag,
				Boolean								createImageOverlayFlag,
				SInt16								selectImageOverlaySelection,
				SInt16								outputFormatCode,
				Boolean								thresholdFlag,
				double								saveCorrelationThreshold,
				double								saveAngleThreshold,
				double								saveCEMThreshold,
				double								saveThresholdPercent,
				SInt16								saveKNNThreshold,
				Boolean								probabilityFileFlag,
				SInt16								paletteSelection,
				SInt16								listResultsTestCode,
				SInt16								listResultsTrainingCode,
				SInt16								parallelPipedCode,
				SInt16								nearestNeighborKValue)

{
	double								thresholdValue;
	
	SInt16								*classAreaPtr;
	
	SInt16								index;
	
	
			// Classifier options.

	gClassifySpecsPtr->mode = classificationProcedure;
	
	if (classificationProcedure == kCorrelationMode)
		gClassifySpecsPtr->correlationCovarianceCode = covarianceEstimate;
	
			// Feature transformation option.
	
	gClassifySpecsPtr->featureTransformationFlag = featureTransformationFlag;
	
			// Determine if channel and/or class list changed. If so SVM model
			// needs to be re-created
	
	if (DetermineIfChannelClassWeightListChanged (
									channelSelection,
									featureTransformationFlag,
									gProjectInfoPtr->numberStatisticsChannels,
									localNumberFeatures,
									(UInt16*)localFeaturesPtr,
									gClassifySpecsPtr->channelSet,
									gClassifySpecsPtr->numberFeatures,
									gClassifySpecsPtr->featureHandle,
									gClassifySpecsPtr->numberChannels,
									gClassifySpecsPtr->channelsHandle,
									classSelection,
									localNumberClasses,
									(UInt16*)localClassPtr,
									gClassifySpecsPtr->classSet,
									gClassifySpecsPtr->numberClasses,
									gClassifySpecsPtr->classHandle,
									weightsSelection,
									classWeightsPtr))
		gClassifySpecsPtr->supportVectorMachineModelAvailableFlag = FALSE;
	
			// Load some common processor parameters
			// Channels
			// Classes
			// Class symbols
	
	LoadProcessorVectorsFromDialogLocalVectors (
									channelSelection,
									featureTransformationFlag,
									gProjectInfoPtr->numberStatisticsChannels,
									localNumberFeatures,
									(UInt16*)localFeaturesPtr,
									&gClassifySpecsPtr->channelSet,
									(UInt16*)&gClassifySpecsPtr->numberFeatures,
									gClassifySpecsPtr->featureHandle,
									(UInt16*)&gClassifySpecsPtr->numberChannels,
									gClassifySpecsPtr->channelsHandle,
									classSelection,
									localNumberClasses,
									(UInt16*)localClassPtr,
									&gClassifySpecsPtr->classSet,
									&gClassifySpecsPtr->numberClasses,
									gClassifySpecsPtr->classHandle,
									symbolSelection,
									localSymbolsPtr,
									&gClassifySpecsPtr->symbolSet,
									gClassifySpecsPtr->symbolsHandle,
									0,
									NULL,
									NULL);
	
			// Load the classVectorPtr list.
	
	for (index=0; index<=gProjectInfoPtr->numberStatisticsClasses; index++)
		gClassifySpecsPtr->classVectorPtr[index] = 0;
	
	for (index=0; index<gClassifySpecsPtr->numberClasses; index++)
		gClassifySpecsPtr->classVectorPtr[gClassifySpecsPtr->classPtr[index]] = 1;
	
			// Target window information handle.
	
	gClassifySpecsPtr->targetWindowInfoHandle = targetWindowInfoHandle;
	
			// Class areas

	classAreaPtr = (SInt16*)GetHandlePointer (gClassifySpecsPtr->classAreaHandle);

	gClassifySpecsPtr->classAreaSet = classAreaSelection;
	if (classAreaSelection == kAllMenuItem)		// All classes
		LoadClassAreaVector (&gClassifySpecsPtr->numberClassAreas,
										classAreaPtr);
	
	else	// classAreaSelection == kSubsetMenuItem
		{
		gClassifySpecsPtr->numberClassAreas = localNumberClassAreas;
		for (index=0; index<localNumberClassAreas; index++)
			classAreaPtr[index] = (SInt16)localClassAreaPtr[index];
		
		}	// end "else classAreaSelection == kSubsetMenuItem"
	
			// Area to Classify

	gClassifySpecsPtr->trainingFldsResubstitutionFlag =
														trainingFldsResubstitutionFlag;

	gClassifySpecsPtr->trainingFldsLOOFlag = trainingFldsLOOFlag;

	gClassifySpecsPtr->testFldsFlag = testFldsFlag;

	gClassifySpecsPtr->imageAreaFlag = imageAreaFlag;
	
			// Classification area
	
	gClassifySpecsPtr->imageLineStart = dialogSelectAreaPtr->lineStart;
	gClassifySpecsPtr->imageLineEnd = dialogSelectAreaPtr->lineEnd;
	gClassifySpecsPtr->imageLineInterval =
												dialogSelectAreaPtr->lineInterval;
	
	gClassifySpecsPtr->imageColumnStart = dialogSelectAreaPtr->columnStart;
	gClassifySpecsPtr->imageColumnEnd = dialogSelectAreaPtr->columnEnd;
	gClassifySpecsPtr->imageColumnInterval =
												dialogSelectAreaPtr->columnInterval;
	
			// Class Weights
	
	UpdateProjectClassWeights (
					gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
					weightsSelection,
					classWeightsPtr);
	
			// Write classification to output window.
	
	gClassifySpecsPtr->outputStorageType = 0;
	
			// Write classification to disk file.
	
	gClassifySpecsPtr->diskFileFormat = 0;
	if (diskFileFlag)
		{
		switch (outputFormatCode)
			{
			/*
			case kClassifyArcViewMenuItem:
				gClassifySpecsPtr->outputStorageType += kClassifyFileCode;
				gClassifySpecsPtr->diskFileFormat = kArcViewType;
				break;
			*/
			case kClassifyASCIIOutputFormat:
				gClassifySpecsPtr->outputStorageType += kAsciiFormatCode;
				gClassifySpecsPtr->diskFileFormat = kMultiSpecClassificationType;
				break;
				
			case kClassifyERDAS74OutputFormat:
				gClassifySpecsPtr->outputStorageType += kClassifyFileCode;
				gClassifySpecsPtr->diskFileFormat = kErdas74Type;
				break;
				
			case kClassifyGAIAOutputFormat:
				gClassifySpecsPtr->outputStorageType += kClassifyFileCode;
				gClassifySpecsPtr->diskFileFormat = kGAIAType;
				break;
				
			case kClassifyTIFFGeoTIFFOutputFormat:
				gClassifySpecsPtr->outputStorageType += kClassifyFileCode;
				gClassifySpecsPtr->diskFileFormat = kTIFFType;
				break;
				
			}	// end "switch (outputFormatCode)"
		
		gOutputFormatCode = 0;
		if (gClassifySpecsPtr->outputStorageType & kClassifyFileCode)
			gOutputFormatCode = gClassifySpecsPtr->diskFileFormat;
		
		}	// end "if (diskFileFlag)"
	
	if (createImageOverlayFlag)
		gClassifySpecsPtr->outputStorageType += kCreateImageOverlayCode;
	
	gProjectInfoPtr->overlayImageWindowInfoHandle =
											GetTargetOverlayImageWindowInfoHandle (
																			fileNamesSelection,
																			targetWindowInfoHandle);
	
	gClassifySpecsPtr->imageOverlayIndex = GetImageOverlayIndex (
												gProjectInfoPtr->overlayImageWindowInfoHandle,
												selectImageOverlaySelection - 2);
	
	if (gClassifySpecsPtr->mode == kEchoMode &&
											gClassifySpecsPtr->imageAreaFlag)
		{
		EchoClassifierVarPtr	echoClassifierVarPtr =
			(EchoClassifierVarPtr)GetHandlePointer (
														gClassifySpecsPtr->echoClassifierVarH);
			
		if (echoClassifierVarPtr->createHomogeneousFilesFlag)
			gClassifySpecsPtr->outputStorageType +=
								kEchoFieldsCode + kEchoClassesCode;
		
		}	// end "if (gClassifySpecsPtr->mode == ..."
	
			// Get threshold results flag.
			// Get probability threshold.
	
	gClassifySpecsPtr->thresholdFlag = thresholdFlag;
	
	if (gClassifySpecsPtr->thresholdFlag)
		{
		if (gClassifySpecsPtr->mode == kCorrelationMode)
			{
			gClassifySpecsPtr->correlationCoefficientThreshold =
															saveCorrelationThreshold;
			gClassifySpecsPtr->correlationAngleThreshold = saveAngleThreshold;
			thresholdValue = saveAngleThreshold;
			
			}	// end "if (gClassifySpecsPtr->mode == kCorrelationMode)"
		
		else if (gClassifySpecsPtr->mode == kCEMMode)
			{
			gClassifySpecsPtr->cemThreshold = saveCEMThreshold;
			thresholdValue = saveCEMThreshold;
			
			}	// end "else if (gClassifySpecsPtr->mode == kCEMMode)"
		
		else if (gClassifySpecsPtr->mode == kKNearestNeighborMode)
			{
			gClassifySpecsPtr->knnThreshold = saveKNNThreshold;
			thresholdValue = saveKNNThreshold;
			
			}	// end "else if (gClassifySpecsPtr->mode == kKNearestNeighborMode)"
		
		else	// ...->mode != kCorrelationMode && ...
			{
			gClassifySpecsPtr->probabilityThreshold = saveThresholdPercent;
			thresholdValue = saveThresholdPercent;
			
			}	// end "else ...->mode != kCorrelationMode && ..."
		
				// Get the image data value which corresponds to the given
				// threshold value.
		
		gClassifySpecsPtr->probabilityThresholdCode =
										GetThresholdCode (thresholdValue,
																	NULL,
																	gClassifySpecsPtr->mode);
		
		}	// end "if (gClassifySpecsPtr->thresholdFlag)"
	
			// Create ERDAS probability file.
	
	if (probabilityFileFlag)
		gClassifySpecsPtr->outputStorageType += kProbFormatCode;
	
			// Determine if a threshold table will need to be
			// generated.
	
	gClassifySpecsPtr->createThresholdTableFlag =
			gClassifySpecsPtr->thresholdFlag ||
					(gClassifySpecsPtr->outputStorageType & kProbFormatCode);
	
			// Get the number of probability classes.
	
	gClassifySpecsPtr->numberProbabilityClasses = 0;
	if (gClassifySpecsPtr->createThresholdTableFlag)
		{
		gClassifySpecsPtr->numberProbabilityClasses = gNumberProbabilityClasses;
		
		if (gClassifySpecsPtr->mode == kCEMMode)
			gClassifySpecsPtr->numberProbabilityClasses = 102;
		
		else if (gClassifySpecsPtr->mode == kCorrelationMode)
			gClassifySpecsPtr->numberProbabilityClasses = 91;
		
		else if (gClassifySpecsPtr->mode == kKNearestNeighborMode)
			gClassifySpecsPtr->numberProbabilityClasses = nearestNeighborKValue;
		
		}	// end "if (gClassifySpecsPtr->createProbabilityFileFlag)"
	
	gProjectInfoPtr->imagePalettePopupMenuSelection = paletteSelection;

			// Codes for training and test listings.
	
	gProjectInfoPtr->listResultsTestCode = listResultsTestCode;
	gProjectInfoPtr->listResultsTrainingCode = listResultsTrainingCode;
	
	gClassifySpecsPtr->parallelPipedCode = parallelPipedCode;
	
	gClassifySpecsPtr->nearestNeighborKValue = nearestNeighborKValue;
	
}	// end "ClassifyDialogOK"

                          
	                
SInt16 ClassifyDialogOnClassificationProcedure (
				DialogPtr							dialogPtr,
				SInt16								okItem, 
				Boolean*								thresholdAllowedFlagPtr,
				Boolean*								featureTransformAllowedFlagPtr,                     
				SInt16*								weightsSelectionPtr,
				SInt16*								parallelPipedCodePtr,
				SInt16								classificationSelection,
				SInt16*								covarianceEstimatePtr,
				SInt16								numberEigenvectors,
				SInt16*								nearestNeighborKValuePtr,
				UInt16*								classifyProcedureEnteredCodePtr,
				SInt16								correlationComboListItem,
				Boolean								optionKeyFlag)
	
{  
	Boolean								returnFlag = TRUE,
											savedEntireImageFlag;
	
	#if defined multispec_win  
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win  
	#if defined multispec_wx  
		//wxComboBox*							comboBoxPtr;
	#endif	// defined multispec_wx  
	
													
	if (classificationSelection == kEchoMode)
		{
				// Echo procedure to be used.	 Get specifications.		
					                                    
		SetDLogControlHilite (dialogPtr, okItem, 255); 
		returnFlag = EchoClassifyDialog ();        
		SetDLogControlHilite (dialogPtr, okItem, 0); 
								
		}	// end "if (classificationSelection == kEcho)"
	
   else if (classificationSelection == kSupportVectorMachineMode && optionKeyFlag)
   	{
      		// SVM procedure to be used; get specifications.
	 
      SetDLogControlHilite (dialogPtr, okItem, 255);
      returnFlag = SVMClassifyDialog ();
      SetDLogControlHilite (dialogPtr, okItem, 0);
	 
   	}	// end "else if (classificationSelection == kSupportVectorMachineMode && ...)"
	
   else if (classificationSelection == kKNearestNeighborMode && optionKeyFlag)
   	{
      		// KNN procedure to be used; get specifications.
		
      SetDLogControlHilite (dialogPtr, okItem, 255);
      returnFlag = KNNClassifyDialog (nearestNeighborKValuePtr);
      SetDLogControlHilite (dialogPtr, okItem, 0);
		
   	}	// end "else if (classificationSelection == kCorrelationMode && ...)"
														
	else if (classificationSelection == kCorrelationMode && 
				(optionKeyFlag || *covarianceEstimatePtr != kNoCovarianceUsed))
		{
				// Correlation (SAM) procedure to be used; get specifications.		
					                                     
		SetDLogControlHilite (dialogPtr, okItem, 255);
		returnFlag = CorrelationClassifyDialog (covarianceEstimatePtr); 
		SetDLogControlHilite (dialogPtr, okItem, 0); 
								
		}	// end "else if (classificationSelection == kCorrelationMode && ...)"
														
	else if (classificationSelection == kCEMMode)
		{
				// CEM procedure to be used; get specifications.		
				// Save the value of the global entire image flag. It may get change
				// by CEMClassify Dialog. 
					                            
		savedEntireImageFlag = gEntireImageFlag;
		SetDLogControlHilite (dialogPtr, okItem, 255);
		returnFlag = CEMClassifyDialog (classifyProcedureEnteredCodePtr);          
		SetDLogControlHilite (dialogPtr, okItem, 0); 
		gEntireImageFlag = savedEntireImageFlag;
								
		}	// end "else if (classificationSelection == kCEMMode)"
														
	else if (classificationSelection == kParallelPipedMode && optionKeyFlag)
		{
				// Paralllel Piped procedure to be used; get specifications.	
				// For now just changed method to min/max data values for desiginating
				// the limits of the class box	
					                                     
		*parallelPipedCodePtr = kPPMinMaxCode;
								
		}	// end "else if (classificationSelection == kParallelPipedMode && ..."
	/*													
	else if (classificationSelection == kDecisionTreeMode)
		{
				// Echo procedure to be used.	 Get specifications.		
					
		HiliteControl ((ControlHandle)okHandle, 255);
							
		savedClassificationProcedure = *classificationProcedurePtr;
		returnFlag = DecisionTreeDialog ();
		*classificationProcedurePtr = savedClassificationProcedure;
							
		HiliteControl ((ControlHandle)okHandle, 0); 
								
		}	// end "if (classificationSelection == kDecisionTreeMode)" 
	*/
	if (*covarianceEstimatePtr == kNoCovarianceUsed)
		{              
		#if defined multispec_mac             
			SetMenuItemText (gPopUpClassifyProcedureMenu, 
									kCorrelationMode,
									"\pCorrelation (SAM)"); 
		#endif	// defined multispec_mac  
		
		#if defined multispec_win                                
			dialogPtr->SetComboItemText (IDC_ClassificationProcedure, 
													correlationComboListItem,
													(UCharPtr)"Correlation (SAM)"); 
			
			comboBoxPtr = 
					(CComboBox*)dialogPtr->GetDlgItem (IDC_ClassificationProcedure);
								
			comboBoxPtr->SetItemData (correlationComboListItem, kCorrelationMode);
		#endif	// defined multispec_win   

		#if defined multispec_wx
					// This is not needed now since the menu item name does not change.
					// The shift symbol and ... are always a part of the name.
			/*
			dialogPtr->SetComboItemText (IDC_ClassificationProcedure,
													correlationComboListItem,
													(char*)"Correlation (SAM)");
			comboBoxPtr = (wxComboBox*)dialogPtr->FindWindow (IDC_ClassificationProcedure);

			comboBoxPtr->SetClientData (correlationComboListItem, (void*)kCorrelationMode);
			*/
		#endif		// defined multispec_wx
		 								
		}	// end "if (*covarianceEstimatePtr == kNoCovarianceUsed)"
		
	if (!returnFlag)
		classificationSelection = 0;
						
	if (classificationSelection != 0)
		{                                                 
		*thresholdAllowedFlagPtr = 
			ClassifyDialogGetThresholdAllowedFlag (
												classificationSelection);
								
		*featureTransformAllowedFlagPtr = 
				ClassifyDialogGetFeatureTransformAllowedFlag (
						classificationSelection, numberEigenvectors);
						
				// Make certain that 'Weights' are set properly.		
							
		*weightsSelectionPtr = abs (*weightsSelectionPtr);
		if (classificationSelection == kKNearestNeighborMode ||
				classificationSelection == kEuclideanMode ||
					classificationSelection == kCorrelationMode ||
						classificationSelection == kCEMMode ||
							classificationSelection == kParallelPipedMode)
			*weightsSelectionPtr = -(*weightsSelectionPtr); 
							
		}	// end "if (classificationSelection != 0)"
		
	return (classificationSelection); 
	
}	// end "ClassifyDialogOnClassificationProcedure"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClassifyDialogOnOverlay
//
//	Software purpose:	The purpose of this routine is to handle a user change in the 
//							overlay available choices.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2006
//	Revised By:			Larry L. Biehl			Date: 11/12/2019
	                
void ClassifyDialogOnOverlay (
				DialogPtr							dialogPtr,
				SInt16								fileNamesSelection,
				Handle								targetWindowInfoHandle,
				Boolean*								createImageOverlayFlagPtr)
	
{
	Handle								overlayImageWindowInfoHandle;
	
	
	overlayImageWindowInfoHandle = GetTargetOverlayImageWindowInfoHandle (
																			fileNamesSelection,
																			targetWindowInfoHandle);
	
	if (overlayImageWindowInfoHandle != NULL)
		SetDLogControlHilite (dialogPtr, IDC_ImageWindowOverlay, 0);
						
	else	// ...->overlayImageWindowInfoHandle == NULL
		{
		SetDLogControl (dialogPtr, IDC_ImageWindowOverlay, 0);
		SetDLogControlHilite (dialogPtr, IDC_ImageWindowOverlay, 255);
		*createImageOverlayFlagPtr = FALSE;
		
		}	// end "else	// ...->overlayImageWindowInfoHandle == NULL"
	
	if (*createImageOverlayFlagPtr)
		ShowDialogItem (dialogPtr, IDC_ImageOverlayCombo);
	
	else	// !*createImageOverlayFlagPtr
		HideDialogItem (dialogPtr, IDC_ImageOverlayCombo);
	
	ClassifyDialogSetPaletteItems (dialogPtr,
												gOutputFormatCode,
												*createImageOverlayFlagPtr);
	
}	// end "ClassifyDialogOnOverlay"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassifyDialogOnTargetFile
//
//	Software purpose:	The purpose of this routine is to handle a user change in the 
//							target file dialog selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/18/1999
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	
	                
SInt16 ClassifyDialogOnTargetFile (
				DialogPtr							dialogPtr,
				SInt16								fileNamesSelection,
				Handle*								targetWindowInfoHandlePtr,
				Boolean*								checkOKFlagPtr,
				DialogSelectArea*					dialogSelectAreaPtr,
				Boolean*								createImageOverlayFlagPtr)
	
{
	#if defined multispec_win                                
		CComboBox* 							comboBoxPtr;
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		wxComboBox*							comboBoxPtr;
	#endif	// defined multispec_wx
	
	UInt32								windowIndex;
											
	SignedByte							handleStatus;
	
	  
	if (fileNamesSelection < 2)
		{
		*targetWindowInfoHandlePtr = FindProjectBaseImageWindowInfoHandle ();
		
		if (*targetWindowInfoHandlePtr == NULL)
			*targetWindowInfoHandlePtr = gProjectInfoPtr->windowInfoHandle;
		
		dialogSelectAreaPtr->windowPtr = gProjectSelectionWindow;
		dialogSelectAreaPtr->adjustToBaseImageFlag = kAdjustToBaseImage;
		
		SetDLogControlHilite (dialogPtr, IDC_Training, 0);
		SetDLogControlHilite (dialogPtr, IDC_TestAreas, 0);
		SetDLogControlHilite (dialogPtr, IDC_TrainingLOO, 0);

		}	// end "if (gFileNamesSelection < 2)" 
		
	else	// fileNamesSelection >= 2 
		{
				// Target is different than the base image file. Force only
				// the target image to be classified; Not the training and test
				// areas.
				
		#if defined multispec_mac  
			windowIndex = kImageWindowStart + fileNamesSelection - 2;
		#endif	// defined multispec_mac   

		#if defined multispec_win  
			comboBoxPtr = (CComboBox*)dialogPtr->GetDlgItem (IDC_TargetCombo);
			windowIndex = (UInt32)comboBoxPtr->GetItemData (fileNamesSelection - 1);
		#endif	// defined multispec_win     

		#if defined multispec_wx  
			comboBoxPtr = (wxComboBox*) dialogPtr->FindWindow (IDC_TargetCombo);
			SInt64 windowIndex64 =
						(SInt64)((int*)comboBoxPtr->GetClientData (fileNamesSelection - 1));
			windowIndex = (UInt32)windowIndex64;
		#endif	// defined multispec_wx 
				
		*targetWindowInfoHandlePtr = GetWindowInfoHandle (gWindowList[windowIndex]);
		dialogSelectAreaPtr->windowPtr = GetWindowPtr (*targetWindowInfoHandlePtr);
		dialogSelectAreaPtr->adjustToBaseImageFlag = kDontAdjustToBaseImage;
		
		SetDLogControl (dialogPtr, IDC_Training, 0);
		SetDLogControl (dialogPtr, IDC_TestAreas, 0);
		SetDLogControl (dialogPtr, IDC_TrainingLOO, 0);
		SetDLogControlHilite (dialogPtr, IDC_Training, 255);
		SetDLogControlHilite (dialogPtr, IDC_TestAreas, 255);
		SetDLogControlHilite (dialogPtr, IDC_TrainingLOO, 255);
		
		}	// end "else gFileNamesSelection >= 2" 

	dialogSelectAreaPtr->imageWindowInfoPtr = 
			(WindowInfoPtr)GetHandleStatusAndPointer (*targetWindowInfoHandlePtr,
																		&handleStatus);	
		
			// Get any selected area for the target image.																	
	
	if (!GetSelectedAreaInfo (
						dialogSelectAreaPtr->windowPtr,
						dialogSelectAreaPtr->imageWindowInfoPtr,
						(SInt32*)&dialogSelectAreaPtr->lineStart,
						(SInt32*)&dialogSelectAreaPtr->lineEnd,
						(SInt32*)&dialogSelectAreaPtr->columnStart,
						(SInt32*)&dialogSelectAreaPtr->columnEnd,
						kDontClearSelectionArea,
						kUseThreshold,
						dialogSelectAreaPtr->adjustToBaseImageFlag))
   	{
		dialogSelectAreaPtr->lineStart = 1;
		dialogSelectAreaPtr->lineEnd = 
										GetMaxNumberLines (*targetWindowInfoHandlePtr);
		dialogSelectAreaPtr->columnStart = 1;
		dialogSelectAreaPtr->columnEnd = 
										GetMaxNumberColumns (*targetWindowInfoHandlePtr);
							
		}	// end "if (!GetSelectedAreaInfo (..."

	LoadLineColumnItems (dialogSelectAreaPtr, 
									dialogPtr, 
									kInitializeLineColumnValues, 
									kIntervalEditBoxesExist,
									1);
	
	MHSetState (*targetWindowInfoHandlePtr, handleStatus);
	
	dialogSelectAreaPtr->imageWindowInfoPtr = NULL;
	
	#if defined multispec_mac 
		InvalWindowRect (
						GetDialogWindow (dialogPtr), &dialogSelectAreaPtr->toSelectionBox);
	#endif	// defined multispec_mac   
	
	ClassifyDialogOnOverlay (dialogPtr,
										fileNamesSelection,
										*targetWindowInfoHandlePtr,
										createImageOverlayFlagPtr);
	
	return (fileNamesSelection);
	
}	// end "ClassifyDialogOnTargetFile"

                          
	                
Boolean ClassifyDialogSetLeaveOneOutItems (
				DialogPtr							dialogPtr,
				SInt16								classificationProcedure,
				SInt16								fileNamesSelection,
				Boolean								userLeaveOneOutFlag,
				Boolean								trainingFieldExistFlag,
				Boolean*								leaveOneOutSettingFlagPtr)
	
{  
	if (trainingFieldExistFlag &&
				fileNamesSelection < 2 &&
					((classificationProcedure == kMaxLikeMode) ||
									(classificationProcedure == kEuclideanMode)))
		{
		SetDLogControl (dialogPtr, IDC_TrainingLOO, (SInt16)userLeaveOneOutFlag);
		SetDLogControlHilite (dialogPtr, IDC_TrainingLOO, 0);
		*leaveOneOutSettingFlagPtr = userLeaveOneOutFlag;
		
		}	// end "if (trainingFieldExistFlag && ..."
		
	else	// !trainingFieldExistFlag || ...
		{
		*leaveOneOutSettingFlagPtr = FALSE;
		SetDLogControl (dialogPtr, IDC_TrainingLOO, 0);
		SetDLogControlHilite (dialogPtr, IDC_TrainingLOO, 255);
		
		}	// end "else !trainingFieldExistFlag || ..."
										
	return (FALSE); 
	
}	// end "ClassifyDialogSetLeaveOneOutItems"


                    
Boolean ClassifyDialogSetThresholdItems (
				DialogPtr							dialogPtr,
				SInt16								classificationProcedure,
				Boolean								imageAreaFlag,
				Boolean								createProbabilityFileFlag,
				Boolean								thresholdResultsFlag,
				Boolean								thresholdAllowedFlag)
				
{	
	#if defined multispec_mac || defined multispec_mac_swift
		SInt16 				IDC_LineStart = 19,
								IDC_ThresholdResults = 35,
								IDC_ThresholdValue = 36,
								IDC_PercentSymbol = 37,
								IDC_CreateProbabilityFile = 38,
								IDC_correlationPrompt = 45,
								IDC_CorrelationCoefficient = 46,
								IDC_CorrelationThresold = 47,
								IDC_DegreeSymbol = 48,
								IDC_CEMThreshold = 49; 
	#endif	// defined multispec_mac || defined multispec_mac_swift 
	
	SInt16									selectedItem = 0;
	                            
	Boolean									correlationItemsFlag,
												cemItemsFlag,
				                        kKNNItemsFlag,
				                        maxLikeItemsFlag;
												
												
	cemItemsFlag = FALSE;
	correlationItemsFlag = FALSE;
	kKNNItemsFlag = FALSE;
	maxLikeItemsFlag = FALSE;

	if (thresholdAllowedFlag)
		{	
		SetDLogControl (dialogPtr, IDC_ThresholdResults, thresholdResultsFlag);
		SetDLogControlHilite (dialogPtr, IDC_ThresholdResults, 0);
		ShowDialogItem (dialogPtr, IDC_ThresholdResults);
		
		if (thresholdResultsFlag)
			{
			if (classificationProcedure == kKNearestNeighborMode)
				{
				kKNNItemsFlag = TRUE;
				selectedItem = IDC_NearestNeighborThreshold;
				
				}	// end "if (classificationProcedure == kCorrelationMode)"
			
			else if (classificationProcedure == kCorrelationMode)
				{
				correlationItemsFlag = TRUE;
				selectedItem = IDC_CorrelationAngleThreshold;
				
				}	// end "if (classificationProcedure == kCorrelationMode)"
				
			else if (classificationProcedure == kCEMMode)
				{      
				cemItemsFlag = TRUE;
				selectedItem = IDC_CEMThreshold;                                
				
				}	// end "else  ...!= kCorrelationMode"
				
			else	//  ...!= kCorrelationMode && ...
				{
				maxLikeItemsFlag = TRUE; 
				selectedItem = IDC_ThresholdValue;                                 
				
				}	// end "else  ...!= kCorrelationMode"
			
			}	// end "if (thresholdResultsFlag && ...)" 
			
		else	// !thresholdResultsFlag 
			{       
			selectedItem = IDC_LineStart;
			
			}	// end "if (thresholdResultsFlag && ...)" 
		
		}	// end "if (thresholdAllowedFlag)" 
			
	else	// !thresholdAllowedFlag 
		{
		SetDLogControl (dialogPtr, IDC_ThresholdResults, 0);
		SetDLogControlHilite (dialogPtr, IDC_ThresholdResults, 255); 
		
		}	// end "else !thresholdAllowedFlag"
	
			// Set the dialog items as requested above.
				               
	ShowHideDialogItem (dialogPtr, IDC_CEMThreshold, cemItemsFlag);
			
	ShowHideDialogItem (dialogPtr, IDC_correlationPrompt, correlationItemsFlag); 
	ShowHideDialogItem (
					dialogPtr, IDC_CorrelationCoefficientThreshold, correlationItemsFlag);
	ShowHideDialogItem (dialogPtr, IDC_CorrelationAngleThreshold, correlationItemsFlag);
	ShowHideDialogItem (dialogPtr, IDC_DegreeSymbol, correlationItemsFlag);
	
	ShowHideDialogItem (dialogPtr, IDC_NearestNeighborThreshold, kKNNItemsFlag);
	ShowHideDialogItem (dialogPtr, IDC_NearestNeighbor, kKNNItemsFlag);
	
	ShowHideDialogItem (dialogPtr, IDC_ThresholdValue, maxLikeItemsFlag);
	ShowHideDialogItem (dialogPtr, IDC_PercentSymbol, maxLikeItemsFlag);
	
	if (selectedItem != 0)
		SelectDialogItemText (dialogPtr, selectedItem, 0, SHRT_MAX); 
		
				// Create threshold map.											
		
	if (thresholdAllowedFlag && imageAreaFlag)         
		{
		SetDLogControl (dialogPtr, IDC_CreateProbabilityFile, createProbabilityFileFlag);
		SetDLogControlHilite (dialogPtr, IDC_CreateProbabilityFile, 0);
			
		}	// end "if (thresholdAllowedFlag && ..." 
		
	else	// !thresholdAllowedFlag || ...
		{
		SetDLogControl (dialogPtr, IDC_CreateProbabilityFile, 0);
		SetDLogControlHilite (dialogPtr, IDC_CreateProbabilityFile, 255);
		
		}	// end "else !thresholdAllowedFlag || ..."
		
	if (classificationProcedure == kCEMMode)
		CtoPstring ((UCharPtr)"Create CEM Values File", gTextString);
	
	else if (classificationProcedure == kKNearestNeighborMode)
		CtoPstring ((UCharPtr)"Create KNN Values File", gTextString);
		
	else if (classificationProcedure == kCorrelationMode) 
		CtoPstring ((UCharPtr)"Create SAM Values File", gTextString);
		
	else	// classificationProcedure != kCEMMode || ... != kCorrelationMode  
		CtoPstring ((UCharPtr)"Create Probability Results File", gTextString); 
										
	SetDLogControlTitle (dialogPtr, 
									IDC_CreateProbabilityFile, 
									(CharPtr)gTextString,
									kASCIICharString);
					
	return (FALSE);
	
}	// end "ClassifyDialogSetThresholdItems" 


                    
void ClassifyDialogSetPaletteItems (
				DialogPtr							dialogPtr,
				SInt16								outputFormatCode,
				Boolean								createImageOverlayFlag)
				
{				
	if (abs (outputFormatCode) == kClassifyERDAS74OutputFormat || 
					abs (outputFormatCode) == kClassifyTIFFGeoTIFFOutputFormat ||
								createImageOverlayFlag)
		{
		ShowDialogItem (dialogPtr, IDC_PalettePrompt);
		ShowDialogItem (dialogPtr, IDC_PaletteCombo);
		
		}	// end "if (abs (outputFormatCode) == kClassifyERDAS74OutputFormat || ..."
		
	else	// abs (outputFormatCode) != kClassifyERDAS74OutputFormat && ...
		{
		HideDialogItem (dialogPtr, IDC_PalettePrompt);
		HideDialogItem (dialogPtr, IDC_PaletteCombo);
		
		}	// end "else abs (outputFormatCode) != kClassifyERDAS74OutputFormat && ..."
	
}	// end "ClassifyDialogSetPaletteItems"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CorrelationClassifyDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the correleation classification
//							specifications.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1997
//	Revised By:			Larry L. Biehl			Date: 03/25/1999	

Boolean CorrelationClassifyDialog (
				SInt16*								covarianceEstimatePtr)

{	
	Boolean							returnFlag = FALSE;           
	
#if defined multispec_mac   
	UserItemUPP								drawCovariancePopUpPtr;
	
	DialogPtr								dialogPtr;
	
	Handle									theHandle;
	Rect										theBox;
	
	SInt32									theNum;
	
	SInt16									itemHit,
												theType;
	
	Boolean									modalDone;
												
	
	dialogPtr = NULL;
												
												
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kSAMParameterDialogID, kCopyScrap, 1, 2);
		
	if (dialogPtr == NULL)										
																					return (FALSE); 
		
			// Intialize local user item proc pointers.									

	drawCovariancePopUpPtr = NewUserItemUPP (DrawCovarianceEstimatePopUp);
	
			// Use all or subset of train fields to compute the correlation matrix							
			//	Make all classes the default													
			
	SetDialogItemDrawRoutine (dialogPtr, 4, drawCovariancePopUpPtr);
	
	if (gProjectInfoPtr->statisticsCode == kMeanCovariance)
		{
		EnableMenuItem (gPopUpCovarianceEstimateMenu, kCommonCovariance);
		EnableMenuItem (gPopUpCovarianceEstimateMenu, kSampleCovariance);
		
		}	// end "if (gProjectInfoPtr->statisticsCode == kMeanCovariance)"
		
	else	// gProjectInfoPtr->statisticsCode != kMeanCovariance
		{
		DisableMenuItem (gPopUpCovarianceEstimateMenu, kCommonCovariance);
		DisableMenuItem (gPopUpCovarianceEstimateMenu, kSampleCovariance);
		
		}	// end "if (gProjectInfoPtr->statisticsCode == kMeanCovariance)"
		
	gCovarianceEstimate = *covarianceEstimatePtr;
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kSAMParameterDialogID, kDoNotSetUpDFilterTable);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 4:		// Covariance to be used 
					itemHit = StandardPopUpMenu (dialogPtr, 
															3, 
															4, 
															gPopUpCovarianceEstimateMenu, 
															gCovarianceEstimate, 
															kPopUpCovarianceMenuID);
													
					if (itemHit != 0)
						gCovarianceEstimate = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// class pop up box.													
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else if (itemHit > 0) 
			{		
			modalDone = TRUE;	
				
			if	(itemHit == 1)      // User selected OK for information
				{ 
				returnFlag = TRUE;
				*covarianceEstimatePtr = gCovarianceEstimate;
				
				}	// end "if	(itemHit == 1)"
				
			else	//	(itemHit == 2)      // User selected Cancel for information 
				returnFlag = FALSE;
			
			}	// end "else if (itemHit > 0)" 
				
		}	while (!modalDone); 
		
	DisposeUserItemUPP (drawCovariancePopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
#endif	// defined multispec_mac 

	#if defined multispec_win 
		returnFlag = TRUE;  
	
		CMCorrelationClassifyDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMCorrelationClassifyDialog ();
			
			returnFlag = dialogPtr->DoDialog (covarianceEstimatePtr); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL  
	#endif	// defined multispec_win 

	#if defined multispec_wx 
		CMCorrelationClassifyDialog* dialogPtr = NULL;
		dialogPtr = new CMCorrelationClassifyDialog ();
		if (dialogPtr != NULL)
			{
			returnFlag = dialogPtr->DoDialog (covarianceEstimatePtr);
			delete dialogPtr;
			}
	#endif	// defined multispec_wx
	
	return (returnFlag);
	
}	// end "CorrelationClassifyDialog" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DecisionTreeDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the echo classification
//							specifications.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:		
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1994
//	Revised By:			Larry L. Biehl			Date: 12/09/1994	

Boolean DecisionTreeDialog (void)

{
	Handle								okHandle,
											theHandle;
	
	Rect									theBox;
	
	UserItemUPP							drawClassificationProcedurePtr,
											drawSeparabilityDistancePopUpPtr;
	                  
	double								realValue,
											savedMinimumThreshold,
											savedStartThreshold,
											savedThresholdIncrement;
	
	SInt32								theNum;
	
	SInt16								itemHit,
											theType;
	
	DialogPtr							dialogPtr;
	DecisionTreeVarPtr				decisionTreeVarPtr;
	
	Boolean								modalDone,
											returnFlag,
											tempFlag;
	
	
			// Make certain that we have memory for the echo parameter				
			// structure.  If not get memory and initialize the parameters.		
			
	tempFlag = FALSE;
	if (gClassifySpecsPtr->decisionTreeVarH == NULL)
		{
		gClassifySpecsPtr->decisionTreeVarH = 
												MNewHandle (sizeof (DecisionTreeVar));
												
		if (gClassifySpecsPtr->decisionTreeVarH == NULL)
																				return (FALSE);
		
		tempFlag = TRUE;
		
		}	// end "if (gClassifySpecsPtr->decisionTreeVarH == NULL)" 
	
	HLock (gClassifySpecsPtr->decisionTreeVarH);	
	decisionTreeVarPtr = 
					(DecisionTreeVarPtr)*gClassifySpecsPtr->decisionTreeVarH;
		
			// Initialize variables if this is the first time.						
													
	if (tempFlag)
		{
		decisionTreeVarPtr->minimumThreshold = 1200.;
		decisionTreeVarPtr->startThreshold = 400.;
		decisionTreeVarPtr->thresholdIncrement = 100.;
		
		decisionTreeVarPtr->classifyAlgorithm = kMaxLikeMode;
		decisionTreeVarPtr->distanceAlgorithm = kTransformedDivergence;
		decisionTreeVarPtr->nodeProcedure = 1;
		
		}	// end "if (tempFlag)" 
	
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kDecisionTreeDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)											
																					return (FALSE);
		
			// Intialize local user item proc pointers.	
			
	drawClassificationProcedurePtr = NewUserItemUPP (DrawClassificationProcedurePopUp);
	drawSeparabilityDistancePopUpPtr = NewUserItemUPP (DrawSeparabilityDistancePopUp);
		
			// Initialize local variables.													
																						
	savedMinimumThreshold = decisionTreeVarPtr->minimumThreshold;
	savedStartThreshold = decisionTreeVarPtr->startThreshold;
	savedThresholdIncrement = decisionTreeVarPtr->thresholdIncrement;
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Classification procedure.														
			//	Get the rectangles for the classification procedure prompt and 	
			// popup boxes. 																		
			
	SetDialogItemDrawRoutine (dialogPtr, 5, drawClassificationProcedurePtr);
	gClassificationProcedure = -decisionTreeVarPtr->classifyAlgorithm;
	
	if (gProjectInfoPtr->statisticsCode == kMeanCovariance)
		{
		EnableMenuItem (gPopUpClassifyProcedureMenu, kMaxLikeMode);
		EnableMenuItem (gPopUpClassifyProcedureMenu, kMahalanobisMode);
		EnableMenuItem (gPopUpClassifyProcedureMenu, kFisherMode);
		EnableMenuItem (gPopUpClassifyProcedureMenu, kEuclideanMode);
		EnableMenuItem (gPopUpClassifyProcedureMenu, kEchoMode);
		EnableMenuItem (gPopUpClassifyProcedureMenu, kParallelPipedMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kSupportVectorMachineMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kKNearestNeighborMode);
		
		}	// end "if (...->statisticsCode != kMeanCovariance)" 

	else	// gProjectInfoPtr->statisticsCode != kMeanCovariance 
		{
		DisableMenuItem (gPopUpClassifyProcedureMenu, kMaxLikeMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kMahalanobisMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kEchoMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kFisherMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kSupportVectorMachineMode);
		DisableMenuItem (gPopUpClassifyProcedureMenu, kKNearestNeighborMode);
		
		}	// end "if (...->statisticsCode != kMeanCovariance)" 
	
			// Distance measure.																	
			//	Get the rectangles for the distance measure prompt and popup 		
			// boxes. 																				
			
	SetDialogItemDrawRoutine (dialogPtr, 8, drawSeparabilityDistancePopUpPtr);
	gSeparabilityDistance = -decisionTreeVarPtr->distanceAlgorithm;
	
	if (gProjectInfoPtr->statisticsCode == kMeanCovariance)
		{
		EnableMenuItem (gPopUpSeparabilityDistanceMenu, kBhattacharyya);
		EnableMenuItem (gPopUpSeparabilityDistanceMenu, kTransformedDivergence);
		EnableMenuItem (gPopUpSeparabilityDistanceMenu, kDivergence);
		EnableMenuItem (gPopUpSeparabilityDistanceMenu, kEFBhattacharyya);
		
		}	// end "if (...->statisticsCode != kMeanCovariance)" 

	else	// gProjectInfoPtr->statisticsCode != kMeanCovariance 
		{
		DisableMenuItem (gPopUpSeparabilityDistanceMenu, kBhattacharyya);
		DisableMenuItem (gPopUpSeparabilityDistanceMenu, kTransformedDivergence);
		DisableMenuItem (gPopUpSeparabilityDistanceMenu, kDivergence);
		DisableMenuItem (gPopUpSeparabilityDistanceMenu, kEFBhattacharyya);
		
		}	// end "if (...->statisticsCode != kMeanCovariance)" 
	
			// Start distance threshold.														
	
	LoadDItemRealValue (dialogPtr, 10, savedStartThreshold, 1);
	
			// Minimum distance threshold.													
	
	LoadDItemRealValue (dialogPtr, 12, savedMinimumThreshold, 1);
	
			// Distance threshold increment.													
	
	LoadDItemRealValue (dialogPtr, 14, savedThresholdIncrement, 1);
	
			// Set routine to draw outline on "OK" box.									
			
	SetDialogItemDrawRoutine (dialogPtr, 15, gHiliteOKButtonPtr);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kDecisionTreeDialogID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[10] = kDItemReal;
	gDialogItemDescriptorPtr[12] = kDItemReal;
	gDialogItemDescriptorPtr[14] = kDItemReal;
		
			// Get deactivate event for the window that is now behind the dialog 
			// and handle it.																		
		
	CheckSomeEvents (activMask);
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 10, 0, SHRT_MAX);
	
	modalDone = FALSE;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 10:		// Start Threshold
					realValue = GetDItemRealValue (dialogPtr, 10);
					if (realValue <= 0)
						{
						RealNumberErrorAlert (savedStartThreshold, dialogPtr, 10, 1);
							
						}	// end "if (realValue <= 0)" 
					
					else	// realValue > 0 
						savedStartThreshold = realValue;
						
					break;
					
				case 12:		// Start Threshold
					realValue = GetDItemRealValue (dialogPtr, 12);
					if (realValue <= 0)
						{
						RealNumberErrorAlert (savedMinimumThreshold, dialogPtr, 12, 1);
							
						}	// end "if (realValue <= 0)" 
					
					else	// realValue > 0 
						savedMinimumThreshold = realValue;
						
					break;
					
				case 14:		// Start Threshold
					realValue = GetDItemRealValue (dialogPtr, 14);
					if (realValue <= 0)
						{
						RealNumberErrorAlert (savedThresholdIncrement, dialogPtr, 14, 1);
							
						}	// end "if (realValue <= 0)" 
					
					else	// realValue > 0 
						savedThresholdIncrement = realValue;
						
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
					
						// Classify algorithm.													

				decisionTreeVarPtr->classifyAlgorithm = 
																abs (gClassificationProcedure);
					
						// Distance algorithm.													

				decisionTreeVarPtr->distanceAlgorithm = 
																abs (gSeparabilityDistance);
	
						// Start distance threshold.											
				
				decisionTreeVarPtr->startThreshold = savedStartThreshold;
				
						// Minimum distance threshold.										
				
				decisionTreeVarPtr->minimumThreshold = savedMinimumThreshold;
				
						// Distance threshold increment.										
				
				decisionTreeVarPtr->thresholdIncrement = savedThresholdIncrement;
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
	
	CheckAndUnlockHandle (gClassifySpecsPtr->decisionTreeVarH);
	
	DisposeUserItemUPP (drawClassificationProcedurePtr);
	DisposeUserItemUPP (drawSeparabilityDistancePopUpPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	
	return (returnFlag);
	
}	// end "DecisionTreeDialog" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfChannelClassWeightListChanged
//
//	Software purpose:	The purpose of this routine is to determine if the channels
//							or class list or the class weights have changed.
//							Note that this needs to be tested for case when enhanced
//							statistics are selected but SVM classifier is being used which
// 						will not use enhanced statistics.
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned: 	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/17/2019
//	Revised By:			Larry L. Biehl			Date: 09/20/2019

Boolean DetermineIfChannelClassWeightListChanged (
				SInt16								channelSelection,
				Boolean								featureTransformationFlag,
				UInt16								numberAllChannels,
				UInt16								newNumberFeatures,
				UInt16*								newFeaturesPtr,
				SInt16								channelSet,
				UInt16								currentNumberFeatures,
				Handle								currentFeatureHandle,
				UInt16								numberChannels,
				Handle								channelsHandle,
				SInt16								classSelection,
				UInt32								newNumberClasses,
				UInt16*								newClassPtr,
				SInt16								classSet,
				UInt32								currentNumberClasses,
				Handle								currentClassHandle,
				SInt16								weightsSelection,
				float*								classWeightsPtr)

{
   HPClassNamesPtr					classNamesPtr;
	
	UInt32								classStorage,
											index;
	
	Boolean								listChangedFlag = FALSE;
	
	
			// Channels
	
	if (currentNumberFeatures != newNumberFeatures)
		listChangedFlag = TRUE;
	
	else	// currentNumberFeatures == newNumberFeatures
		{
				// Verify that the channel list is the same
		
		SInt16* currentFeaturesPtr = (SInt16*)GetHandlePointer (currentFeatureHandle);
		for (index=0; index<newNumberFeatures; index++)
			{
			if (currentFeaturesPtr[index] != newFeaturesPtr[index])
				{
				listChangedFlag = TRUE;
				break;
				
				}	// end "if (currentFeaturesPtr[index] != newFeaturesPtr[index])"
	
			}	// end "for (index=0; index<newNumberFeatures; index++)"
		
		}	// end "else currentNumberFeatures == newNumberFeatures"

	if (!listChangedFlag)
		{
				// Classes

		if (currentNumberClasses != newNumberClasses)
			listChangedFlag = TRUE;
		
		else	// currentNumberClasses == newNumberClasses
			{
					// Verify that the class list is the same
			
			SInt16* currentClassPtr = (SInt16*)GetHandlePointer (currentClassHandle);
			for (index=0; index<newNumberClasses; index++)
				{
				if (currentClassPtr[index] != newClassPtr[index])
					{
					listChangedFlag = TRUE;
					break;
					
					}	// end "if (currentClassPtr[index] != newClassPtr[index])"
		
    			}	// end "for (index=0; index<newNumberClasses; index++)"
			
			}	// end "else currentNumberClasses == newNumberClasses"
		
		}	// end "if (!listChangedFlag)"

	if (!listChangedFlag)
		{
				// Weights

		if (gProjectInfoPtr->classWeightSet != weightsSelection)
			listChangedFlag = TRUE;
		
		else if (weightsSelection == kUnequalWeights)
			{
					// Verify that the weight list is the same
			
      	classNamesPtr = gProjectInfoPtr->classNamesPtr;

			for (index=0;
					index<gProjectInfoPtr->numberStatisticsClasses;
						index++)
				{
				classStorage = gProjectInfoPtr->storageClass[index];

				if (classNamesPtr[classStorage].priorWeights[0] != *classWeightsPtr)
					{
					listChangedFlag = TRUE;
					break;
					
					}	// end "if (if (classNamesPtr[classStorage].priorWeights[weightsIndex] != ..."

				classWeightsPtr++;

				}	// end "for (index=0; index<numberClasses; index++)"
			
			}	// end "else if (weightsSelection == kUnequalWeights)"
		
		}	// end "if (!listChangedFlag)"
		
	return (listChangedFlag);
	
}	// end "DetermineIfChannelClassWeightListChanged"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawClassificationProcedurePopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							classification pocedure selection for 
//							classification procedure pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/22/1993
//	Revised By:			Larry L. Biehl			Date: 04/22/1993	

pascal void DrawClassificationProcedurePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpClassifyProcedureMenu, 
								gClassificationProcedure, 
								TRUE);
	
}	// end "DrawClassificationProcedurePopUp"
#endif	// defined multispec_mac 

#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawCorrelationMatrixClassAreaPopUp
//
//	Software purpose:	The purpose of this routine is to draw the class
//							area prompt and class area set selection for the correlation
//							matrix class area pop up menu.  This routine will also draw a drop
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
//	Coded By:			Larry L. Biehl			Date: 08/29/1997
//	Revised By:			Larry L. Biehl			Date: 08/29/1997	

pascal void DrawCorrelationMatrixClassAreaPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{	
			// Use the generic pop up drawing routine.									
			
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpAllSubsetMenu, 
								gCorrelationMatrixClassAreaSelection, 
								TRUE);
	
}	// end "DrawCorrelationMatrixClassAreaPopUp"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawCovarianceEstimatePopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							selection for covariance estimate pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1997
//	Revised By:			Larry L. Biehl			Date: 08/29/1997	

pascal void DrawCovarianceEstimatePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpCovarianceEstimateMenu, 
								gCovarianceEstimate, 
								TRUE);
	
}	// end "DrawCovarianceEstimatePopUp"
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawDiskFilePopUp
//
//	Software purpose:	The purpose of this routine is to draw the
//							disk file selection for Classify To Disk
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1990
//	Revised By:			Larry L. Biehl			Date: 10/30/1990

pascal void DrawDiskFilePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.
	
	DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpDiskFileMenu,
								gOutputFormatCode,
								(gOutputFormatCode > 0));
	
}	// end "DrawDiskFilePopUp"
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawEchoAlgorithmPopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							selection for echo algorithm pop up menu.  This routine 
//							will also draw a drop shadow box around the 
//							pop up selection.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/22/1993
//	Revised By:			Larry L. Biehl			Date: 04/22/1993	

pascal void DrawEchoAlgorithmPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.									
	
	DrawPopUpMenuItem (dialogPtr, 
								itemNumber, 
								gPopUpEchoAlgorithmMenu, 
								gEchoAlgorithmProcedure, 
								TRUE);
	
}	// end "DrawEchoAlgorithmPopUp"
#endif	// defined multispec_mac


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean EchoClassifyDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the echo classification
//							specifications.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	
//
// Called By:			ClassifyDialog   in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/29/1991
//	Revised By:			Larry L. Biehl			Date: 10/22/2018

Boolean EchoClassifyDialog (void)

{		
	EchoClassifierVarPtr				echoClassifierVarPtr;
										
	Boolean								returnFlag = FALSE;
	
	
			// Make certain that we have memory for the echo parameter				
			// structure.  If not get memory and initialize the parameters.		
			
	Boolean initializeEchoInfoFlag = FALSE;
	if (gClassifySpecsPtr->echoClassifierVarH == NULL)
		{
		gClassifySpecsPtr->echoClassifierVarH = 
												MNewHandle (sizeof (EchoClassifierVar));
												
		if (gClassifySpecsPtr->echoClassifierVarH == NULL)
																				return (FALSE);
		
		initializeEchoInfoFlag = TRUE;
		
		}	// end "if (gClassifySpecsPtr->echoClassifierVarH == NULL)" 
	                                              	
	echoClassifierVarPtr = (EchoClassifierVarPtr)GetHandlePointer (
															gClassifySpecsPtr->echoClassifierVarH,
															kLock);
		
			// Initialize variables if this is the first time.						
													
	if (initializeEchoInfoFlag)
		{
		echoClassifierVarPtr->algorithmCode = kMaxLikeMode;
		echoClassifierVarPtr->cell_width = 2; 
		echoClassifierVarPtr->cell_size = 
							echoClassifierVarPtr->cell_width * echoClassifierVarPtr->cell_width;
		echoClassifierVarPtr->homogeneityThreshold = 2.;
		echoClassifierVarPtr->annexationThreshold = 2.;
		
		echoClassifierVarPtr->tempFileStreamPtr = NULL;
		echoClassifierVarPtr->thresholds_phase1 = NULL;
  		echoClassifierVarPtr->epix_ibufPtr = NULL;
  		echoClassifierVarPtr->epix_ibuf2Ptr = NULL;
   	echoClassifierVarPtr->fldlikPtr = NULL;
   	echoClassifierVarPtr->fieldClassNumberPtr = NULL;
   	echoClassifierVarPtr->fieldLikelihoodTableIndexPtr = NULL;
   	echoClassifierVarPtr->field_number_table = NULL;
   	echoClassifierVarPtr->work1 = NULL;
   	echoClassifierVarPtr->work2 = NULL;
   	echoClassifierVarPtr->cellClassPtr = NULL;
   	echoClassifierVarPtr->fieldLikeIndicesPtr = NULL;
   	echoClassifierVarPtr->fieldLikeFlagsPtr = NULL;
   	
  		echoClassifierVarPtr->combineLikeFieldsFlag = TRUE;
   	echoClassifierVarPtr->createHomogeneousFilesFlag = FALSE;
		echoClassifierVarPtr->fixed_homogeneityThreshold_option = FALSE;
  		echoClassifierVarPtr->mixCellsFlag = TRUE;
		echoClassifierVarPtr->tempDiskFileWasUsedFlag = FALSE;
		echoClassifierVarPtr->useTempDiskFileFlag = FALSE;
		
		}	// end "if (initializeEchoInfoFlag)" 
	
#if defined multispec_mac  
	UserItemUPP								drawEchoAlgorithmPtr;
	
	Handle									theHandle;
	Rect										theBox;
	
	double									maxPhase1,
												minPhase1,
												savedPhase2Threshold;
	                  
	double									realValue,
												savedPhase1LogLikelihoodThreshold,
												savedPhase1PercentThreshold,
												savedPhase1Threshold;
	
	SInt32									savedCellWidth,
												theNum;
	
	SInt16									itemHit,
												theType;
	
	DialogPtr								dialogPtr;            
	
	Boolean									localFixedPhase1Option,
												modalDone;
												
												
			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (kEchoParametersDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)											
																					return (FALSE);
		
			// Intialize local user item proc pointers.									

	drawEchoAlgorithmPtr = NewUserItemUPP (DrawEchoAlgorithmPopUp);
	
			// Get default values.  Note a possible algorithm for the default		
			// fixed likelihood threshold is 7.125 * no. channels/2 *				
			// no. pixels in cell/1.2.  This is based on some experiments with	
			// flc1.  A better estimate can be made using the number of classes,	
			// number of channels used, cell size and class determinants.			
			
	localFixedPhase1Option = 
							echoClassifierVarPtr->fixed_homogeneityThreshold_option;
	savedPhase1PercentThreshold = 2.;
	savedPhase1LogLikelihoodThreshold = 95.;
	
			// Cell Width.																
			
	LoadDItemValue (dialogPtr, 5, (SInt32)echoClassifierVarPtr->cell_width);
	savedCellWidth = echoClassifierVarPtr->cell_width;
	
			// Homogeniety Threshold.												

	minPhase1 = 0;
	if (localFixedPhase1Option)
		{
		maxPhase1 = 1000000;
		savedPhase1LogLikelihoodThreshold = 
											echoClassifierVarPtr->homogeneityThreshold;
		savedPhase1Threshold = savedPhase1LogLikelihoodThreshold;
											
		}	// end "if (localFixedPhase1Option)" 
		
	else	// !localFixedPhase1Option 
		{
		maxPhase1 = 100;
		savedPhase1PercentThreshold = 
											echoClassifierVarPtr->homogeneityThreshold;
		savedPhase1Threshold = savedPhase1PercentThreshold;
											
		}	// end "else !localFixedPhase1Option)" 
	
	LoadDItemRealValue (dialogPtr, 7, savedPhase1Threshold, 8);
	
			// Homogeniety (phase 1) Threshold specified in percent		
			// probability correct. (> 0.0 && < 100.0)									
	
	SetDLogControl (dialogPtr, 8, !localFixedPhase1Option);
	
			// Homogeniety Threshold specified in -Log							
			// Likelihood.	 (> 0.0 && < 1000000.0)											
	
	SetDLogControl (dialogPtr, 9, localFixedPhase1Option);
	
			// Annexation Threshold. (>= 0.0 && < 10.0)						
		
	savedPhase2Threshold = echoClassifierVarPtr->annexationThreshold;
	LoadDItemRealValue (dialogPtr, 11, savedPhase2Threshold, 1);
	
			// Flag indicating whether to merge like fields.				
	
	SetDLogControl (dialogPtr, 12, echoClassifierVarPtr->combineLikeFieldsFlag);
	
			// Flag indicating whether to create image files with the				
			// homogeneous fields and classes.												
	
	SetDLogControl (
				dialogPtr, 13, echoClassifierVarPtr->createHomogeneousFilesFlag);
	
			// Flag indicating whether to allow merging of cells and 				
			// fields which are of different classes.										
	
	HideDialogItem (dialogPtr, 14);
	SetDLogControl (dialogPtr, 14, echoClassifierVarPtr->mixCellsFlag);
	
			// Classification procedure.														
			//	Get the rectangles for the echo algorithm prompt and popup boxes. 																		
			
	SetDialogItemDrawRoutine (dialogPtr, 17, drawEchoAlgorithmPtr);
	gEchoAlgorithmProcedure = echoClassifierVarPtr->algorithmCode;
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kEchoParametersDialogID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[7] = kDItemReal;
	gDialogItemDescriptorPtr[11] = kDItemReal;
	
			// Set default text selection to first edit text item						
			
	SelectDialogItemText (dialogPtr, 5, 0, SHRT_MAX);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a number item, check for bad values.  If		
					// itemHit was a radio button make appropriate control 			
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.  For number value items, get	
					//	the string and	convert it to a number.								
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			if (theType == 16)
				{
				GetDialogItemText (theHandle, gTextString);	
				StringToNum (gTextString, &theNum);
				
				}	// end "if (theType == 16)" 
			
			switch (itemHit)
				{
				case 5:				// Cell width
				   if (theNum <= 0 || theNum > 100)
				   	{
				   	if (theNum > 100)
				   		savedCellWidth = 100;	
						NumberErrorAlert (savedCellWidth, dialogPtr, itemHit);
						 									
						}	// end "if (theNum == 0 || theNum > 100)" 
					
					else	// theNum > 0 && theNum <= 100 
						savedCellWidth = theNum;
					break;
					
				case 7:				// Phase 1 Threshold	
					realValue = GetDItemRealValue (dialogPtr, 7);
					if (realValue > maxPhase1 || realValue < minPhase1)
						{
						if (realValue > maxPhase1)
							savedPhase1Threshold = maxPhase1;
						RealNumberErrorAlert (savedPhase1Threshold, dialogPtr, 7, 1);
							
						}	// end "if (realValue > maxPhase1 || ..." 
					
					else	// phase1Threshold >= minPhase1 && ... 
						savedPhase1Threshold = realValue;
					break;
					
				case 8:				// Threshold in percent probability
					if (localFixedPhase1Option)
						{
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 9, 0);
						localFixedPhase1Option = FALSE;
						maxPhase1 = 100;
						savedPhase1LogLikelihoodThreshold = savedPhase1Threshold;
						savedPhase1Threshold = savedPhase1PercentThreshold;
						LoadDItemRealValue (dialogPtr, 7, savedPhase1Threshold, 1);
									
						}	// end "if (localFixedPhase1Option)" 
					break;
					
				case 9:				// Threshold in - Log Likelihood
					if (!localFixedPhase1Option)
						{
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 8, 0);
						localFixedPhase1Option = TRUE;
						maxPhase1 = 1000000;
						savedPhase1PercentThreshold = savedPhase1Threshold;
						savedPhase1Threshold = savedPhase1LogLikelihoodThreshold;
						LoadDItemRealValue (dialogPtr, 7, savedPhase1Threshold, 8);
									
						}	// end "if (!localFixedPhase1Option)" 
					break;
					
				case 11:				// Annexation Threshold	
					realValue = GetDItemRealValue (dialogPtr, 11);
					if (realValue < 0 || realValue > 100)
						{
						if (realValue > 100)
							savedPhase2Threshold = 100;
						RealNumberErrorAlert (savedPhase2Threshold, dialogPtr, 11, 1);
						
						}	// end "if (realValue < 0 || realValue > 100)" 
					
					else	// realValue >= 0 && realValue <= 100.0 
						savedPhase2Threshold = realValue;
					break;
					
				case 12:				// Merge like fields.	
				case 13:				// Allow merging of unlike cells and fields.	
				case 14:				// Create files for homogeneous fields/classes.
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
					
				case 17:		// Echo algorithm.	
					itemHit = StandardPopUpMenu (dialogPtr, 
															16, 
															17, 
															gPopUpEchoAlgorithmMenu, 
															gEchoAlgorithmProcedure, 
															kPopUpEchoAlgorithmMenuID);
						
					if (itemHit != 0)
						gEchoAlgorithmProcedure = itemHit;
	
							// Make certain that the correct label is drawn in the	
							// echo algorithm pop up box.						
					
					InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
					break;
						
				}	// end "switch (itemHit)" 
				
			}	// end "if (itemHit > 2)" 
			
		else	// itemHit <= 2 
			{
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
	
						// Cell Width.													
						
				echoClassifierVarPtr->cell_width = 
												(SInt16)GetDItemValue (dialogPtr, 5);
												
				echoClassifierVarPtr->cell_size = 
					echoClassifierVarPtr->cell_width * echoClassifierVarPtr->cell_width;
					
						// Homogeneity (phase 1) threshold.						
						
				echoClassifierVarPtr->homogeneityThreshold = savedPhase1Threshold;
				
						// Homogeniety (phase 1) Threshold specified in 		
						// percent probability correct.										
				
				echoClassifierVarPtr->fixed_homogeneityThreshold_option = 
													(GetDLogControl (dialogPtr, 9) == 1);
					
						// Annexation threshold.									
						
				echoClassifierVarPtr->annexationThreshold = savedPhase2Threshold;
	
						// Flag indicating whether to merge like fields.	
				
				echoClassifierVarPtr->combineLikeFieldsFlag = 
																GetDLogControl (dialogPtr, 12); 
	
						// Flag indicating whether to create image files with the	
						// homogeneous fields and classes.									
																
				echoClassifierVarPtr->createHomogeneousFilesFlag =  
																GetDLogControl (dialogPtr, 13); 
	
						// Flag indicating whether to allow merging of 					
						// cells and fields which are of different classes.			
				
				echoClassifierVarPtr->mixCellsFlag = GetDLogControl (dialogPtr, 14);  
				
				echoClassifierVarPtr->algorithmCode = gEchoAlgorithmProcedure;
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
			
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone); 
		
	DisposeUserItemUPP (drawEchoAlgorithmPtr);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable); 
	
#endif	// defined multispec_mac 

	#if defined multispec_win   
		CMEchoClassifyDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMEchoClassifyDialog ();
			
			returnFlag = dialogPtr->DoDialog (echoClassifierVarPtr); 
		                       
			delete dialogPtr;
			
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			
			}
	
		END_CATCH_ALL  
	#endif	// defined multispec_win 

	#if defined multispec_wx
		CMEchoClassifyDialog* dialogPtr = NULL;
		dialogPtr = new CMEchoClassifyDialog ();
		if (dialogPtr != NULL)
			{
			returnFlag = dialogPtr->DoDialog (echoClassifierVarPtr);
			delete dialogPtr;
			
			}	// end "if (dialogPtr != NULL)"
	#endif	// defined multispec_wx
	
	CheckAndUnlockHandle (gClassifySpecsPtr->echoClassifierVarH); 
	
	return (returnFlag);
	
}	// end "EchoClassifyDialog" 



Boolean KNNClassifyDialog (
				SInt16*								nearestNeighborKValuePtr)

{
   Boolean                     returnFlag = FALSE;
	
	
	#if defined multispec_wx
		CMKNNClassifyDialog* dialogPtr = NULL;
		dialogPtr = new CMKNNClassifyDialog ();
		if (dialogPtr != NULL)
			{
			returnFlag = dialogPtr->DoDialog (nearestNeighborKValuePtr);
			delete dialogPtr;
			
			}	// end "if (dialogPtr != NULL)"
	#endif	// defined multispec_wx

	#if defined multispec_win 
		CMKNNClassifyDialog*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMKNNClassifyDialog ();
			
			returnFlag = dialogPtr->DoDialog (nearestNeighborKValuePtr); 
		                       
			delete dialogPtr;
			
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			
			}
	
		END_CATCH_ALL  
	#endif	// defined multispec_win 
	
   return (returnFlag);
	
}	// end "KNNClassifyDialog"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ListResultsOptionsDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the project statistics options.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	0 if user clicked on cancel
//							1 if user clicked on ok.
//
// Called By:			StatisticsControl   in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/08/1997
//	Revised By:			Larry L. Biehl			Date: 04/08/1997

void ListResultsOptionsDialog (
				SInt16*			listResultsTrainingCodePtr, 
				SInt16*			listResultsTestCodePtr)

{
	
#if defined multispec_mac
	Rect							theBox;
	
	Handle						theHandle;
	
	SInt16						itemHit,
									setting,
									theType;
									
	DialogPtr					dialogPtr;
	
	Boolean						modalDone,
									returnFlag = FALSE;


			// Get the modal dialog for the reformat specification					
				
	dialogPtr = LoadRequestedDialog (
										kListResultsOptionsDialogID, kDoNotCopyScrap, 1, 2);
	if (dialogPtr == NULL)														
																							return;
	
			// Set current settings	for training set												
					
	setting = (*listResultsTrainingCodePtr & kAreasUsed)  ? 1 : 0;
	SetDLogControl (dialogPtr, 4, setting);
	
	setting = (*listResultsTrainingCodePtr & kAreasSummary)  ? 1 : 0;
	SetDLogControl (dialogPtr, 5, setting);
	
	setting = (*listResultsTrainingCodePtr & kFieldSummary)  ? 1 : 0;
	SetDLogControl (dialogPtr, 6, setting);
	
	setting = (*listResultsTrainingCodePtr & kClassSummary)  ? 1 : 0;
	SetDLogControl (dialogPtr, 7, setting);
	
			// Set current settings	for test set												
					
	setting = (*listResultsTestCodePtr & kAreasUsed)  ? 1 : 0;
	SetDLogControl (dialogPtr, 9, setting);
	
	setting = (*listResultsTestCodePtr & kAreasSummary)  ? 1 : 0;
	SetDLogControl (dialogPtr, 10, setting);
	
	setting = (*listResultsTestCodePtr & kFieldSummary)  ? 1 : 0;
	SetDLogControl (dialogPtr, 11, setting);
	
	setting = (*listResultsTestCodePtr & kClassSummary)  ? 1 : 0;
	SetDLogControl (dialogPtr, 12, setting);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (
					dialogPtr, kListResultsOptionsDialogID, kDoNotSetUpDFilterTable);
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{	
			switch (itemHit)
				{
				case 4: 
				case 5: 
				case 6: 
				case 7: 
				case 9: 
				case 10: 
				case 11:
				case 12:  
					GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
					ChangeDLogCheckBox ((ControlHandle)theHandle);
					break;
				
				}	// end "switch (itemHit)"
			
			}	// end "if (itemHit > 2)"
			
		else	// itemHit <= 2
			
			{
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				returnFlag = TRUE;
				
						// List Results options for training areas.	
				
				*listResultsTrainingCodePtr = 0;		
				if (GetDLogControl (dialogPtr, 4))
					*listResultsTrainingCodePtr += kAreasUsed;
							
				if (GetDLogControl (dialogPtr, 5))
					*listResultsTrainingCodePtr += kAreasSummary;	
						
				if (GetDLogControl (dialogPtr, 6))
					*listResultsTrainingCodePtr += kFieldSummary;	
						
				if (GetDLogControl (dialogPtr, 7))
					*listResultsTrainingCodePtr += kClassSummary;
				
						// List Results options for test areas.	
				
				*listResultsTestCodePtr = 0;		
				if (GetDLogControl (dialogPtr, 9))
					*listResultsTestCodePtr += kAreasUsed;
							
				if (GetDLogControl (dialogPtr, 10))
					*listResultsTestCodePtr += kAreasSummary;	
						
				if (GetDLogControl (dialogPtr, 11))
					*listResultsTestCodePtr += kFieldSummary;	
						
				if (GetDLogControl (dialogPtr, 12))
					*listResultsTestCodePtr += kClassSummary;
												
				}	// end "if (itemHit == 1)" 
				
			if	(itemHit == 2)		// User selected Cancel for information
				{
				modalDone = TRUE;
				returnFlag = FALSE;
				
				}	// end "if	(itemHit == 2)" 
				
			}	// end "else itemHit <= 2" 
				
		} while (!modalDone);
		
	CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
#endif	// defined multispec_mac  
                   
	
	#if defined multispec_win  	
		CMListResultsOptionsDlg*		dialogPtr = NULL;
		
		TRY
			{ 
			dialogPtr = new CMListResultsOptionsDlg ();
			
			dialogPtr->DoDialog (
									listResultsTrainingCodePtr, 
									listResultsTestCodePtr); 
		                       
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif // defined multispec_win

	#if defined multispec_wx
		CMListResultsOptionsDlg* dialogPtr = NULL;
		dialogPtr = new CMListResultsOptionsDlg ();
		if (dialogPtr != NULL)
			{
			dialogPtr->DoDialog (listResultsTrainingCodePtr,
										listResultsTestCodePtr);

			delete dialogPtr;
			}
	#endif // defined multispec_wx
	
}	// end "ListResultsOptionsDialog"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadCEMParameterSpecs
//
//	Software purpose:	The purpose of this routine is to load the Classify 
//							Specification structure with the default set of
//							parameters.
//
//	Parameters in:		Pointer to image file information structure
//
//	Parameters out:	None
//
// Value Returned:			
// 
// Called By:			CEMClassifyDialog in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/29/1997
//	Revised By:			Larry L. Biehl			Date: 05/13/2016

Boolean LoadCEMParameterSpecs (
				UInt16					classifyProcedureEnteredCode)

{
	CEMParametersPtr					cemParametersPtr;
	
	Boolean								changedFlag,
											continueFlag = TRUE,
											initializeCEMInfoFlag = FALSE;
	
	        
			// Make certain that we have memory for the CEM parameter				
			// structure.  If not get memory and initialize the parameters.		
			
	if (gClassifySpecsPtr->cemParametersH == NULL)
		{
		gClassifySpecsPtr->cemParametersH = MNewHandle (sizeof (CEMParameters));
		initializeCEMInfoFlag = TRUE;
		
		}	// end "if (gClassifySpecsPtr->cemParametersH == NULL)" 
	                                             	
	cemParametersPtr = (CEMParametersPtr)GetHandlePointer (
															gClassifySpecsPtr->cemParametersH,
															kLock);
	
	if (cemParametersPtr != NULL)
		{	
				// Initialize variables if this is the first time.						
														
		if (initializeCEMInfoFlag)
			{
			cemParametersPtr->correlationMatrixClassHandle = NULL;						
			cemParametersPtr->lineStart = 1;						
			cemParametersPtr->lineEnd = gImageWindowInfoPtr->maxNumberLines;			
			cemParametersPtr->lineInterval = 1; 					
			cemParametersPtr->columnStart = 1; 					
			cemParametersPtr->columnEnd = gImageWindowInfoPtr->maxNumberColumns;				
			cemParametersPtr->columnInterval = 1;
			cemParametersPtr->numbercorrelationMatrixClasses = 
												gProjectInfoPtr->numberStatisticsClasses;
			cemParametersPtr->correlationMatrixClassAreaSet = kAllMenuItem;
			cemParametersPtr->correlationMatrixCode = kAreaType;
			
			}	// end "if (initializeCEMInfoFlag)" 
						
				// Check memory for correlation matrix classes area vector.						
		
		UInt32 bytesNeeded =
			(UInt32)gProjectInfoPtr->numberStatisticsClasses * sizeof (SInt16);
		SInt16* correlationMatrixClassPtr = (SInt16*)CheckHandleSize (
								&cemParametersPtr->correlationMatrixClassHandle,
								&continueFlag, 
								&changedFlag, 
								bytesNeeded);

		if (changedFlag)
			cemParametersPtr->correlationMatrixClassAreaSet = kAllMenuItem;
	
		if (cemParametersPtr->correlationMatrixClassAreaSet == kAllMenuItem)								
			LoadTrainClassVector (
									&cemParametersPtr->numbercorrelationMatrixClasses,
									&cemParametersPtr->correlationMatrixClassAreaSet, 
									correlationMatrixClassPtr);
		
				// Force the area option to be used if the statistics available for the 
				// project does not contain the second order statistics.
										
		if (gProjectInfoPtr->statisticsCode != kMeanCovariance)
			cemParametersPtr->correlationMatrixCode = kAreaType;
											
				// Check if structure exist for image second order statistics. If so
				// then copy the respective line and column values to the CEM parameter
				// structure so that the user knows where the currently available
				// image statistics are from.
	
		ImageSecondOrderStatsPtr imageSecondOrderStatsPtr = 
													&gImageWindowInfoPtr->imageSecondOrderStats;
													
		if (imageSecondOrderStatsPtr->totalPixels > 0 && classifyProcedureEnteredCode == 0)
			{
			cemParametersPtr->lineStart = imageSecondOrderStatsPtr->lineStart;						
			cemParametersPtr->lineEnd = imageSecondOrderStatsPtr->lineEnd;			
			cemParametersPtr->lineInterval = imageSecondOrderStatsPtr->lineInterval; 					
			cemParametersPtr->columnStart = imageSecondOrderStatsPtr->columnStart; 					
			cemParametersPtr->columnEnd = imageSecondOrderStatsPtr->columnEnd;				
			cemParametersPtr->columnInterval = imageSecondOrderStatsPtr->columnInterval;
			
			}	// end "if (imageSecondOrderStatsPtr->numberPixels > 0 && ..."
			
		CheckAndUnlockHandle (gClassifySpecsPtr->cemParametersH);
									
		}	// end "if (cemParametersPtr != NULL)"
		
	else	// cemParametersPtr == NULL
		continueFlag = FALSE;
		
	return (continueFlag);

}	// end "LoadCEMParameterSpecs" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetUpPalettePopUpMenu
//
//	Software purpose:	The purpose of this routine is to set up the
//							palette popup menu for the classify dialog.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1996
//	Revised By:			Larry L. Biehl			Date: 11/09/2019

void SetUpPalettePopUpMenu (
				DialogPtr					dialogPtr)

{    
	#if defined multispec_mac
		EnableMenuItem (gPopUpPaletteMenu, kDefaultColors);
		EnableMenuItem (gPopUpPaletteMenu, kDefaultGrays);
		EnableMenuItem (gPopUpPaletteMenu, kCorrelationMatrixColors);
		EnableMenuItem (gPopUpPaletteMenu, kAVHRR_NDVI_Colors);
		EnableMenuItem (gPopUpPaletteMenu, kMODIS_NDVI_Colors);
		EnableMenuItem (gPopUpPaletteMenu, kFalseColors);
		DisableMenuItem (gPopUpPaletteMenu, kImageDefaultColorTable);
		DisableMenuItem (gPopUpPaletteMenu, kUserDefinedColors);
	#endif	// defined multispec_mac 
	
	#if defined multispec_win
				// Set user defined values for palette combo box.
				
		CComboBox* comboBoxPtr = (CComboBox*)(dialogPtr->GetDlgItem (IDC_PaletteCombo));
		 
		comboBoxPtr->SetItemData (0, kDefaultColors);
		comboBoxPtr->SetItemData (1, kDefaultGrays);
		comboBoxPtr->SetItemData (2, kCorrelationMatrixColors);
		comboBoxPtr->SetItemData (3, kAVHRR_NDVI_Colors);  
		comboBoxPtr->SetItemData (4, kMODIS_NDVI_Colors);  
		comboBoxPtr->SetItemData (5, kFalseColors);  
		
				// Make sure that the 6th and 7th strings do not exist. 
		                       
		comboBoxPtr->DeleteString (7);
		comboBoxPtr->DeleteString (6);
	#endif	// defined multispec_win 

	#if defined multispec_wx
		#if defined multispec_wxlin
			wxComboBox*			paletteCtrl;
			paletteCtrl =
					wxDynamicCast (dialogPtr->FindWindow (IDC_PaletteCombo), wxComboBox);
		#endif
		#if defined multispec_wxmac
			wxChoice*			paletteCtrl;
			paletteCtrl =
					wxDynamicCast (dialogPtr->FindWindow (IDC_PaletteCombo), wxChoice);
		#endif
	
				// Set user defined values for palette combo box.

		UInt32 numbermenuitem = paletteCtrl->GetCount ();

		paletteCtrl->SetClientData (0, (void*)kDefaultColors);
		paletteCtrl->SetClientData (1, (void*)kDefaultGrays);
		paletteCtrl->SetClientData (2, (void*)kCorrelationMatrixColors);
		paletteCtrl->SetClientData (3, (void*)kAVHRR_NDVI_Colors);
		paletteCtrl->SetClientData (4, (void*)kMODIS_NDVI_Colors);
		paletteCtrl->SetClientData (5, (void*)kFalseColors);

				// Make sure that the 6th and 7th strings do not exist. 

		while (numbermenuitem > 6) // changed from 5
			{
			paletteCtrl->Delete (numbermenuitem - 1);
			numbermenuitem--;
			}
	#endif	// defined multispec_wx
			
}	// end Routine "SetUpPalettePopUpMenu"



//------------------------------------------------------------------------------------
//                   Copyright 2019-2020 Purdue Research Foundation
//
//	Function name:		Boolean SVMClassifyDialog
//
//	Software purpose:	The purpose of this routine is to set up the SVM classify
//							dialog
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Tsung Tai Yeh			Date: 08/??/2019
//	Revised By:			Larry L. Biehl			Date: 09/28/2019

Boolean SVMClassifyDialog ()

{
   Boolean                     returnFlag = FALSE;
	
	
	#if defined multispec_wx
		CMSVMClassifyDialog* dialogPtr = NULL;

		dialogPtr = new CMSVMClassifyDialog ();
		if (dialogPtr != NULL)
			{
			returnFlag = dialogPtr->DoDialog ();
			delete dialogPtr;
			
			}
	#endif   // defined multispec_wx

	#if defined multispec_win
		CMSVMClassifyDialog*		dialogPtr = NULL;
	
		TRY
			{
			dialogPtr = new CMSVMClassifyDialog ();
			
			returnFlag = dialogPtr->DoDialog ();
			
			delete dialogPtr;
			}
	
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
		END_CATCH_ALL
	#endif	// defined multispec_win
	
   return (returnFlag);
	
}	// end "SVMClassifyDialog"



//------------------------------------------------------------------------------------
//                   Copyright 2019-2020 Purdue Research Foundation
//
//	Function name:		Boolean SVMClassifyDialog
//
//	Software purpose:	The purpose of this routine is to set up the SVM classify
//							dialog
//
//	Parameters in:
//
//	Parameters out:
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/20/2019
//	Revised By:			Larry L. Biehl			Date: 09/21/2019

void SVMClassifyDialogShowHideParameters (
				DialogPtr							dialogPtr,
				int									svmType,
				int									svmKernel)

{
	Boolean							showHideFlag;
	
	
	showHideFlag = (svmKernel == kPolynomialKernel);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_DEGREE_PROMPT,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_DEGREE,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_DEGREE_INFO,
								showHideFlag);

	showHideFlag = (svmKernel == kPolynomialKernel ||
							svmKernel == kRBFKernel ||
								svmKernel == kSigmoidKernel);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_GAMMA_PROMPT,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_GAMMA,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_GAMMA_INFO,
								showHideFlag);
	
	showHideFlag = (svmKernel == kPolynomialKernel ||
							svmKernel == kSigmoidKernel);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_COEF0_PROMPT,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_COEF0,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_COEF0_INFO,
								showHideFlag);
	
	showHideFlag = (svmType == kC_SVC_Type ||
							svmType == kEpsilon_SVR_Type ||
								svmType == kNU_SVR_Type);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_COST_PROMPT,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_COST,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_COST_INFO,
								showHideFlag);
	
	showHideFlag = (svmType == kNU_SVC_Type ||
							svmType == kOne_Class_Type ||
								svmType == kNU_SVR_Type);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_NU_PROMPT,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_NU,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_NU_INFO,
								showHideFlag);
	
	showHideFlag = (svmType == kEpsilon_SVR_Type);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_P_PROMPT,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_P,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_P_INFO,
								showHideFlag);
	
	showHideFlag = (svmType == kC_SVC_Type ||
							svmType == kNU_SVC_Type ||
								svmType == kEpsilon_SVR_Type ||
									svmType == kNU_SVR_Type);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_PROBABILITY,
								showHideFlag);
	ShowHideDialogItem (dialogPtr,
								IDC_SVM_PROBABILITY_INFO,
								showHideFlag);

}	// end "SVMClassifyDialogShowHideParameters"

