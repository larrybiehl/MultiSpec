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
//	File:						SProject.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			04/12/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains routines that open, close, save and read 
//								project files.
//
//------------------------------------------------------------------------------------
/*
Template for debugging.
	int numberChars = sprintf ((char*)gTextString3,
										" SProject:xxx (hdfDataSetSelection): %d%s",
										fileInfoPtr->hdfDataSetSelection,
										gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/

#include "SMultiSpec.h"

#include	"Ssvm.h"

#if defined multispec_wx
	#include "wx/wx.h"
	#include "SImageWindow_class.h"
	#include "xImageView.h"
	#include "xMultiSpec.h"
#endif

#if defined multispec_mac
#endif	// defined multispec_mac  
  
#if defined multispec_win
	#include "SImageWindow_class.h"
	#include "WImageView.h" 
	#include "WMultiSpec.h"
#endif	// defined multispec_win 

#include "errno.h" 
/*
extern SInt16 LoadGDALInformation (
				FileInfoPtr 						fileInfoPtr, 
				char*									headerRecordPtr, 
				SInt16								format);
*/
extern Boolean VerifyImageFileCanBeForProject (
				Handle								imageWindowInfoH);



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													

void AdjustFieldBoundaries (
				FileInfoPtr							fileInfoPtr);

void AdjustLabelPoints (
				FileInfoPtr							fileInfoPtr);
								
void CloseProjectImageFile (
				ProjectInfoPtr						inputProjectInfoPtr);

SInt16 CompareImageDimensionsWithProjectFields (
				FileInfoPtr							fileInfoPtr);

Boolean CompareImageProjectFileInfo (
				Handle								fileInfoHandle);

void ComputeSumSquaresMatrix (
				UInt32								numberChannels,
				HChannelStatisticsPtr			channelStatsPtr,
				HCovarianceStatisticsPtr		covariancePtr,
				SInt64								numberPixels);

void ComputeSumVector (
				HChannelStatisticsPtr			channelStatsPtr,
				UInt32								numberChannels,
				SInt64								numberPixels);
								
SInt16 CopyFileInfoHandles (
				FileInfoPtr							inputFileInfoPtr,
				FileInfoPtr							outputFileInfoPtr);

void DeactivateProject (void);

double GetDefaultZeroVarianceFactor (
				UInt32								numberChannels);

void GetProjectSelectionWindow (void);
									
Boolean GetProjectStructure (
				Handle								projectInfoHandle,
				SInt16								numberChannels);
			
Boolean GetSpecifiedImageFile (
				FileStringPtr						inputFileNamePtr,
				Boolean								userPrompt);

void InitializeProjectStructure (
				Handle								projectInfoHandle,
				UInt32								numberChannels);

SInt16 ReadChannelInformation (
				HChannelStatisticsPtr			chanStatsPtr,
				HPtr* 								inputStringPtrPtr,
				SInt16								numberStatisticsChannels,
				SInt16								type);
																			
SInt16 ReadCovarianceInformation (
				HSumSquaresStatisticsPtr		sumSquaresStatsPtr,
				ParmBlkPtr 							paramBlockPtr,
				HPtr*									inputStringPtrPtr,
				SInt16								numberStatisticsChannels,
				char*									fieldClassStringPtr,
				double*								boxRightPtr,
				double								boxIncrement);
								
SInt16 ReadModifiedStats (
				ParmBlkPtr 							paramBlockPtr,
				HPtr*									inputStringPtrPtr,
				SInt16	 							numberChannels,
				HChannelStatisticsPtr 			modifiedClassChanStatsPtr,
				HCovarianceStatisticsPtr 		modifiedClassCovStatsPtr);
																	
SInt16 ReadProjectFile (void);

SInt16 ReadStatistics (
				HPClassNamesPtr 					classNamesPtr,
				SInt16	 							storageIndex,
				ParmBlkPtr 							paramBlockPtr,
				HPtr*									inputStringPtrPtr,
				SInt16	 							classFieldCode,
				double* 								boxRightPtr,
				double 								boxIncrement,
				Boolean*								statsLoadedFlagPtr);
/*
UCharPtr ReadStringFromFile (
				UCharPtr								inputStringPtr,
				UCharPtr								outputStringPtr,
				SInt16								numberSkipTabs,
				SInt16								maxStringLength);
*/
void ReleaseProjectHandles (
				ProjectInfoPtr						inputProjectInfoPtr);

UCharPtr SkipNTabs (
				UCharPtr								inputStringPtr,
				SInt16								numberTabs);

Boolean SetupClassFieldPointMemory (void);

void UpdateGraphicStatusBox (
				double*								rightBoxPtr,
				double								boxIncrement);

Boolean UserLocateProjectBaseImage (
				Handle								fileInfoHandle,
				SInt16								promptStringNumber,
				SInt16								option);

SInt16 WriteProjectFile (
				SInt16								saveCode);

Boolean WriteChannelInformation (
				HChannelStatisticsPtr			chanStatsPtr,
				CMFileStream* 						fileStreamPtr,
				SInt16								numberStatisticsChannels,
				SInt16								type,
				HPtr									stringPtr);
										
Boolean WriteCovarianceInformation (
				HCovarianceStatisticsPtr		matrixStatsPtr,
				HChannelStatisticsPtr			channelStatsPtr,
				SInt64								numberPixelsLessOne,
				CMFileStream* 						fileStreamPtr,
				SInt16								numberStatisticsChannels,
				SInt16								type,
				HPtr	 								stringPtr,
				double*								boxRightPtr,
				double								boxIncrement);
									
Boolean WriteModifiedStats (
				CMFileStream* 						fileStreamPtr,
				SInt16								numberChannels,
				HChannelStatisticsPtr 			modifiedClassChanStatsPtr,
				HCovarianceStatisticsPtr 		modifiedClassCovStatsPtr);
									
SInt16 WriteStatistics (
				HChannelStatisticsPtr			channelStatsPtr,
				HSumSquaresStatisticsPtr		sumSquaresStatsPtr,
				SInt64								numberPixels,
				CMFileStream* 						fileStreamPtr,
				Boolean								saveMeansCovFlag,
				Boolean								saveSumsSquaresFlag,
				SInt16								classFieldCode,
				double*								boxRightPtr,
				double								boxIncrement);


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AdjustFieldBoundaries
//
//	Software purpose:	The purpose of this routine is to adjust the field boundaries 
//							based on the difference in the project start line
//							and column values and the newly selected base image start line 
//							and column values.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ChangeProjectBaseImage in SProject
//
//	Coded By:			Larry L. Biehl			Date: 06/26/1990
//	Revised By:			Larry L. Biehl			Date: 07/14/1997	

void AdjustFieldBoundaries (
				FileInfoPtr							fileInfoPtr)

{
	HPFieldPointsPtr					fieldPointsPtr;
	
	SInt32								columnAdjust,
											lineAdjust;
									
	UInt32								point,
											totalNumberPoints;
	
	
			// Check input parameters.															
			
	if (gProjectInfoPtr == NULL || fileInfoPtr == NULL)
																									return;
																							
	if ((gProjectInfoPtr->startLine == fileInfoPtr->startLine) &&
						(gProjectInfoPtr->startColumn == fileInfoPtr->startColumn))
																									return;
	
	lineAdjust = gProjectInfoPtr->startLine - fileInfoPtr->startLine;
	columnAdjust = gProjectInfoPtr->startColumn - fileInfoPtr->startColumn;
	totalNumberPoints = gProjectInfoPtr->numberStoragePoints;
	
	fieldPointsPtr = gProjectInfoPtr->fieldPointsPtr;
	
			// Need to adjust all possible field points even if the fields have been
			// deleted in case any of the fields are 'uncut'.
			
	for (point=0; 
			point<totalNumberPoints; 
				point++)
		{
		fieldPointsPtr[point].line += lineAdjust;
		fieldPointsPtr[point].col += columnAdjust;
		
		}	// end "for (point=0; ..."
		
	return;																						
																				
}	// end "AdjustFieldBoundaries" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AdjustLabelPoints
//
//	Software purpose:	The purpose of this routine is to adjust the label points for
//							polygon fields based on the difference in the project start line
//							and column values and the newly selected base image start line 
//							and column values.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			ChangeProjectBaseImage in SProject
//
//	Coded By:			Larry L. Biehl			Date: 04/01/2004
//	Revised By:			Larry L. Biehl			Date: 04/01/2004	

void AdjustLabelPoints (
				FileInfoPtr							fileInfoPtr)

{
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt32								columnAdjust,
											lineAdjust;
									
	UInt32								field,
											totalNumberFields;
	
	
			// Check input parameters.															
			
	if (gProjectInfoPtr == NULL || fileInfoPtr == NULL)
																									return;
																							
	if ((gProjectInfoPtr->startLine == fileInfoPtr->startLine) &&
						(gProjectInfoPtr->startColumn == fileInfoPtr->startColumn))
																									return;
	
	lineAdjust = gProjectInfoPtr->startLine - fileInfoPtr->startLine;
	columnAdjust = gProjectInfoPtr->startColumn - fileInfoPtr->startColumn;
	totalNumberFields = gProjectInfoPtr->numberStorageFields;
	
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
	
			// Need to adjust all possible field points even if the fields have been
			// deleted in case any of the fields are 'uncut'.
			
	for (field=0; 
			field<totalNumberFields; 
				field++)
		{
		if (fieldIdentPtr[field].pointType == kPolygonType)
			{		
			fieldIdentPtr[field].labelPoint.v += lineAdjust;
			fieldIdentPtr[field].labelPoint.h += columnAdjust;
			
			}	// end "if (fieldIdentPtr[field].pointType == kPolygonType)"
		
		}	// end "for (field=0; ..."
		
	return;																						
																				
}	// end "AdjustLabelPoints" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeProjectBaseImage
//
//	Software purpose:	The purpose of this routine is to allow the user to
//							change the project base image file.  This routine will
//							check if the project train and test fields fit
//							within the new base image dimensions.  If the fields do
//							not, that image file will be allowed as a base image for
//							the project.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			Menus in MMenus.c
//							OnProjChangeBaseImageFile in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/21/1990
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void ChangeProjectBaseImage (void)

{
	FileInfoPtr							newImageFileInfoPtr;
	
	Handle								newImageFileInfoH,
											newWindowInfoHandle,
											windowInfoHandle;
	
	SInt16								window,
											windowIndex;
	
	Boolean								continueFlag,
											disposeMemoryFlag;
	
	
			// Check input parameters.															
			
	if (gProjectInfoPtr == NULL)
																									return;
																							
	gProcessorCode = kChangeBaseImageProcessor;
																							
			// Initialize variables.															
			
	newImageFileInfoH = NULL;
	newWindowInfoHandle = NULL;
	disposeMemoryFlag = FALSE;
	
			// Get a new handle to a block of memory to be used for file			
			// information for the new base image file.		
				// Initialize the variables and handles in the structure.			
				
	newImageFileInfoH = InitializeFileInfoStructure (NULL, kNotPointer);
	
	if (newImageFileInfoH != NULL)
		{
				// Lock the memory for newImageFileInfoH.									
				// Then get the pointer to the file information block.				
					
		newImageFileInfoPtr = (FileInfoPtr)GetHandlePointer (
																		newImageFileInfoH, kLock);
		
				// Get the new project base image file.									
				
		continueFlag = UserLocateProjectBaseImage (
											newImageFileInfoH, IDS_SelectNewBaseImage, 2);
											
				// Allocate structures for the window information to be certain	
				// that there is enough memory for them in case they are needed.	
		
		if (continueFlag)				
			newWindowInfoHandle = GetWindowInfoStructures (newImageFileInfoH, 
																			kMultispectralImageType,
																			kImageWindowType);
		continueFlag = (newWindowInfoHandle != NULL);
				
				// Update the project channel list.											
				// Assume for now that all channels will be used.						
		
		if (continueFlag)								
			continueFlag = 
					ChangeProjectChannelsList (newImageFileInfoPtr->numberChannels);
					
				// Change affected project variables.										
						
		if (continueFlag)
			{		
					// Clear the statistics.													
						
			ClearProjectStatistics (1);
		
					// Adjust the line and columns of the project fields to take	
					// into account any difference in the start line and columns	
					// the old base image file and the new base image file.			
					
			AdjustFieldBoundaries (newImageFileInfoPtr);
		
					// Adjust the line and columns of the project polygon fields to take	
					// into account any difference in the start line and columns	
					// the old base image file and the new base image file.
			
			AdjustLabelPoints (newImageFileInfoPtr);
					
						// Change the settings for any currently displayed 			
						// image windows for the previous base image file.				
				
			window = 0;
			windowIndex = kImageWindowStart;
			while (window<gNumberOfIWindows)
				{
				windowInfoHandle = (Handle)GetWindowInfoHandle (gWindowList[windowIndex]);
				
				WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																						windowInfoHandle);
				if (windowInfoPtr != NULL)		
					windowInfoPtr->projectBaseImageFlag = FALSE;
				
				window++;
				windowIndex++;
															
				}	// end "while (window<gNumberOfIWindows)" 
				
			CloseProjectImageFile (gProjectInfoPtr);
			/*			
			if (gProjectInfoPtr->windowInfoHandle != NULL)
				{
				windowInfoHandle = gProjectInfoPtr->windowInfoHandle;
				CloseWindowImageFiles (windowInfoHandle);
				UnlockAndDispose (GetLayerInfoHandle (windowInfoHandle));    
				windowInfoHandle = UnlockAndDispose (windowInfoHandle);
				
				}	// end "if (gProjectInfoPtr->windowInfoHandle != ...)" 
			*/		
			gProjectInfoPtr->windowInfoHandle = newWindowInfoHandle;
			
			GetCopyOfPFileNameFromFileInfo (
									newImageFileInfoPtr, gProjectInfoPtr->imageFileName);
			
			gProjectInfoPtr->startLine = newImageFileInfoPtr->startLine;
			gProjectInfoPtr->startColumn = newImageFileInfoPtr->startColumn;
			gProjectInfoPtr->signedDataFlag = newImageFileInfoPtr->signedDataFlag;
				
					// Determine if any image windows are currently open for			
					// the new project base image and update their parameter			
					// settings.																	
					
			if (FindProjectImageWindows (FALSE))
						// Outline the fields in the image windows for the new 		
						// base image.
				OutlineFieldsInProjectBaseWindows (gProjectInfoPtr->statsWindowMode);
			
					// Document change to the text window.									
						
			ForceTextToEnd (); 
			
			continueFlag = ListSpecifiedStringNumber (
														kProjectStrID,
														IDS_Project39,
														NULL, 
														gOutputForce1Code, 
														(char*)&gProjectInfoPtr->imageFileName[1], 
														continueFlag,
														kUTF8CharString);
					
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
		
					// Update menu items.														
								
			gUpdateProjectMenuItemsFlag = TRUE;
					
			}	// end "if (continueFlag)" 
			
		else	// !continueFlag 
			disposeMemoryFlag = TRUE;
			
		if (disposeMemoryFlag)
			{
			DisposeFileInfoHandle (newImageFileInfoH);
			newImageFileInfoH = NULL;
			
			if (newWindowInfoHandle != NULL)
				UnlockAndDispose (GetLayerInfoHandle (newWindowInfoHandle));
			UnlockAndDispose (newWindowInfoHandle);
			
			}	// end "if (disposeMemoryFlag)" 
					
		CheckAndUnlockHandle (newImageFileInfoH);
		
		}	// end "if (newImageFileInfoH != NULL)" 
																				
}	// end "ChangeProjectBaseImage"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfProjectAssociatedImage
//
//	Software purpose:	The purpose of this routine is to check if the image in the 
//							window belongs to one of the associated image files.
//
// 						NOTE: This routine has not been completed!
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/10/1990
//	Revised By:			Larry L. Biehl			Date: 05/10/1990	

Boolean CheckIfProjectAssociatedImage (
				Handle								fileInfoHandle)

{
	Boolean								returnFlag;
	
	
	returnFlag = FALSE;
	
	if (fileInfoHandle != NULL && gProjectInfoPtr != NULL)
		{
				// This routine has not been completed.
				
		returnFlag = FALSE;
		
		}	// end "if (fileInfoHandle != NULL && gProjectInfoPtr != NULL)"
		
	return (returnFlag);
																				
}	// end "CheckIfProjectAssociatedImage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckIfProjectBaseImage
//
//	Software purpose:	The purpose of this routine is to check if the image
//							in the window belongs to the project base image file.
//
//	Parameters in:		Handle to file information
//
//	Parameters out:	None
//
// Value Returned:	True if the input file information is for the
//											base project image file.
//							False if not.
// 
// Called By:			CreateImageWindow in SImageWindow_class.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/10/1990
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean CheckIfProjectBaseImage (
				Handle								fileInfoHandle)

{
	FileInfoPtr							fileInfoPtr;
	
	Boolean								returnFlag;
	
	
	returnFlag = FALSE;
	
	if (fileInfoHandle != NULL && gProjectInfoPtr != NULL)
		{
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
		
		if (!fileInfoPtr->thematicType)
			{
					// First check if the image belongs to the project base image file.
					// Get pointer to image window file name.		
			
			FileStringPtr fileNamePtr =
								(FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);

					// Get pointer to project image file name.
					
			FileStringPtr projectImageNamePtr = gProjectInfoPtr->imageFileName;
				
					// Check if the file names agree.										
						
			if (CompareStringsNoCase (&projectImageNamePtr[1], fileNamePtr) == 0)
						// The file names agree; now check if the file information	
						// agrees with the project information.
				returnFlag = CompareImageProjectFileInfo (fileInfoHandle);
				
			}	// end "if (!fileInfoPtr->thematicType)" 
			
		CheckAndUnlockHandle (fileInfoHandle);
		
		}	// end "if (fileInfoHandle != NULL && ...)" 
		
	return (returnFlag);
																				
}	// end "CheckIfProjectBaseImage" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseProjectImageFile
//
//	Software purpose:	The purpose of this routine is to close the project
//							image files.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1992
//	Revised By:			Larry L. Biehl			Date: 01/14/1999

void CloseProjectImageFile (
				ProjectInfoPtr						inputProjectInfoPtr)

{				
	WindowInfoPtr						windowInfoPtr;
	
								
			// Check input variables. 															
			
	if (inputProjectInfoPtr != NULL)
		{                
		#if defined multispec_mac 		
			CloseWindowImageFiles (inputProjectInfoPtr->windowInfoHandle);
		
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															inputProjectInfoPtr->windowInfoHandle,
															kLock);
			
			if (windowInfoPtr != NULL)
				{
				UnlockAndDispose (windowInfoPtr->layerInfoHandle);
								
						// Dispose of memory for the image sums and sums of squares.
												
				UnlockAndDispose (windowInfoPtr->imageSecondOrderStats.sumsHandle);
				UnlockAndDispose (windowInfoPtr->imageSecondOrderStats.sumSquaresHandle);
				
				}	// end "if (windowInfoPtr != NULL)"
		
					// Unlock handle to window information structure						
		
			inputProjectInfoPtr->windowInfoHandle = 
								UnlockAndDispose (inputProjectInfoPtr->windowInfoHandle);
		#endif	// defined multispec_mac 
		     
      #if defined multispec_win || defined multispec_wx
			CMImageWindow* imageWindowCPtr = NULL;
			                                                 
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															inputProjectInfoPtr->windowInfoHandle);
			
			if (windowInfoPtr != NULL)		
				{				
						// Dispose of memory for the image sums and sums of squares.
				
				windowInfoPtr->imageSecondOrderStats.sumsHandle = 								
						UnlockAndDispose (windowInfoPtr->imageSecondOrderStats.sumsHandle);
				
				windowInfoPtr->imageSecondOrderStats.sumSquaresHandle = 	
					UnlockAndDispose (windowInfoPtr->imageSecondOrderStats.sumSquaresHandle);
				
				imageWindowCPtr = windowInfoPtr->cImageWindowPtr;
				
				}	// end "if (windowInfoPtr != NULL)"								
			                    
			if (imageWindowCPtr != NULL)
				delete imageWindowCPtr;                             						
		
			inputProjectInfoPtr->windowInfoHandle = NULL;
		#endif	// defined multispec_win 
		
		}	// end "if (inputProjectInfoPtr)" 
																				
}	// end "CloseProjectImageFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseProjectStructure
//
//	Software purpose:	The purpose of this routine is to close the input project
//							structure.  The memory for the project structure will
//							be released.  If the input project structure is
//							active, the related window and menu items will be
//							updated.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1988
//	Revised By:			Larry L. Biehl			Date: 09/17/2019

void CloseProjectStructure (
				ProjectInfoPtr						inputProjectInfoPtr)

{
	Handle								projectInfoHandle;
	
	Boolean								projectActiveFlag;
	
	
	projectActiveFlag = (inputProjectInfoPtr == gProjectInfoPtr);
	
	if (inputProjectInfoPtr != NULL)
		{
		ReleaseProjectHandles (inputProjectInfoPtr);
		
      #if defined multispec_win || defined multispec_wx
			if (inputProjectInfoPtr->fileStreamCPtr != NULL)
				delete inputProjectInfoPtr->fileStreamCPtr;
				
			inputProjectInfoPtr->fileStreamCPtr = NULL;
			
			if (inputProjectInfoPtr->trainingMask.fileStreamPtr != NULL)
				delete inputProjectInfoPtr->trainingMask.fileStreamPtr;
				
			inputProjectInfoPtr->trainingMask.fileStreamPtr = NULL;
			
			if (inputProjectInfoPtr->testMask.fileStreamPtr != NULL)
				delete inputProjectInfoPtr->testMask.fileStreamPtr;
				
			inputProjectInfoPtr->testMask.fileStreamPtr = NULL;
		#endif	// defined multispec_win
		
		if (gProjectInfoPtr->svmModel != NULL)
			{
			svm_free_and_destroy_model (&gProjectInfoPtr->svmModel);
			gProjectInfoPtr->svmModel = NULL;
			
			}	// end "if (gProjectInfoPtr->svmModel != NULL)"
	
		if (gProjectInfoPtr->svm_x != NULL)
			{
			free (gProjectInfoPtr->svm_x);
			gProjectInfoPtr->svm_x = NULL;
			
			}	// end "if (gProjectInfoPtr->svm_x != NULL)"
		
				// Save handle to project structure.										
				
		projectInfoHandle = inputProjectInfoPtr->projectInfoHandle;
		
				// If project is active, deactivate the project.						
				
		if (projectActiveFlag)
			{
			CloseProjectImageFile (inputProjectInfoPtr);
			
			DeactivateProject ();
		
					// Set the project information pointer so that the menus will	
					// be updated properly.														
					
			gProjectInfoPtr = NULL;
		
					// Force update of menu items.														
			                                                      
			gUpdateFileMenuItemsFlag = TRUE; 
			gUpdateProjectMenuItemsFlag = TRUE;
			gUpdateProcessorMenuItemsFlag = TRUE;

			if (gActiveImageWindow == NULL)
				gRedrawMenuBar = TRUE;
			
			}	// end "if (projectActiveFlag)" 
		
				// Dispose of handle to project information strucure.					
				
		UnlockAndDispose (projectInfoHandle);
	
		}	// end "if (inputProjectInfoPtr != NULL)" 

}	// end "CloseProjectStructure"


    
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClusterStatisticsVerification
//
//	Software purpose:	The purpose of this routine is to check if the
//							project file being saved contain any cluster
//							statistics.  If so, then if no statistics are
//							being saved to disk, then a warning is presented
//							to the user indicating such and determines
//							whether the user wants to continue.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/19/1994
//	Revised By:			Larry L. Biehl			Date: 10/27/1994	

Boolean ClusterStatisticsVerification (void)

{
	SInt16								fieldTypesPresentCode,
											returnCode;
											
	Boolean								continueFlag;
	

	if (gProjectInfoPtr == NULL)
																						return (FALSE);
																			
			// Intialize local variables.												
			
	continueFlag = TRUE;
	
			// Determine if any cluster type fields exist.						
								
	fieldTypesPresentCode = DetermineFieldTypes ();
	
	if (fieldTypesPresentCode & 0x0004)
		{
		if (gProjectInfoPtr->keepClassStatsOnlyFlag)
			{
			if (!gProjectInfoPtr->saveClassMeansCovFlag &&
													!gProjectInfoPtr->saveClassSumsSquaresFlag)
				continueFlag = FALSE;
			
			}	// end "if (...->keepClassStatsOnlyFlag)" 
			
		else	// !gProjectInfoPtr->keepClassStatsOnlyFlag 
			{
			if (!gProjectInfoPtr->saveFieldMeansCovFlag &&
													!gProjectInfoPtr->saveFieldSumsSquaresFlag)
				continueFlag = FALSE;
			
			}	// end "if (...->keepClassStatsOnlyFlag)" 
			
		if (!continueFlag)
			{
					// Cluster fields exist and the statistics are not 		
					// being saved to disk.												
			
			returnCode = DisplayAlert (1164, kStopAlert, 158, 26, 0, NULL);
			
			if (returnCode == 3)
				continueFlag = TRUE;
			
			}	// end "if (!continueFlag)" 
		
		}	// end "if (fieldTypesPresentCode & 0x0004)" 
		
	return (continueFlag);
		
}	// end "ClusterStatisticsVerification"  
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CompareImageDimensionsWithProjectFields
//
//	Software purpose:	The purpose of this routine is to compare the project 
//							field dimensions with the image line and column dimensions.
//							If the project field dimensions are not within the image 
//							file dimensions, then FALSE is returned.
//							Otherwise TRUE is returned.
//							Note that this is set up for only non-linked image files which
//							is currently the only ones that this routine is called for since
//							it is for project image files.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			UserLocateProjectBaseImage in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/21/1990
//	Revised By:			Larry L. Biehl			Date: 12/31/2003	

SInt16 CompareImageDimensionsWithProjectFields (
				FileInfoPtr							fileInfoPtr)

{    
	AreaDescription					areaDescription;
	                           
	SInt16								classStorage,
											fieldNumber,
											returnCode; 
							
	UInt16								classIndex;
	
	
			// Check input parameters.															
			
	if (gProjectInfoPtr == NULL || fileInfoPtr == NULL)
																							return (-1);
	
	returnCode = 1;
	InitializeAreaDescription (&areaDescription);
	
	for (classIndex=0; 
			classIndex<gProjectInfoPtr->numberStatisticsClasses; 
			classIndex++)
		{
				// Get the class storage number.												
						
		classStorage = gProjectInfoPtr->storageClass[classIndex];
		
		fieldNumber = gProjectInfoPtr->classNamesPtr[classStorage].firstFieldNumber;
		
		while ((fieldNumber != -1) && (returnCode == 1))
			{
			if (gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType != kClusterType)
				{
						// Get the field coordinates.												
				
				GetFieldBoundary (gProjectInfoPtr, &areaDescription, fieldNumber);
				
						// Compare the coordinates of the field.								
				
				if (areaDescription.lineStart + gProjectInfoPtr->startLine - 1 <
																		fileInfoPtr->startLine)
					returnCode = 0;
				if (areaDescription.lineEnd + gProjectInfoPtr->startLine > 
									fileInfoPtr->numberLines + fileInfoPtr->startLine)
					returnCode = 0;
					
				if (areaDescription.columnStart + gProjectInfoPtr->startColumn - 1 <
																		fileInfoPtr->startColumn)
					returnCode = 0;
				if (areaDescription.columnEnd + gProjectInfoPtr->startColumn > 
									fileInfoPtr->numberColumns + fileInfoPtr->startColumn)
					returnCode = 0;
							
						// Dispose of the region if it exists.									
				
				CloseUpAreaDescription (&areaDescription);
					
				}	// end "if (...fieldIdentPtr[fieldNumber].pointType != kClusterType)" 
			
			else	// ...fieldIdentPtr[fieldNumber].pointType == kClusterType
				{
				if (returnCode != 0)
					returnCode = 2;
					
				}	// end "else ...[fieldNumber].pointType == kClusterType" 
			
			fieldNumber = gProjectInfoPtr->fieldIdentPtr[fieldNumber].nextField;
			
			}	// end "while ((fieldNumber != -1) && ..."
			
		if (returnCode < 1)
			break;
			
		}	// end "for (classIndex=0; classIndex<..." 
		
			// Display an alert indicating that the project fields do not fit	
			// within the lines and columns of the proposed new base image.	
			// Indicate that the user wants to cancel with a return code of 	
			// -1.																				
	
	if (returnCode == 0)
		{
		if (DisplayAlert (
				kOKCancelAlertID, kCautionAlert, kAlertStrID, IDS_Alert10, 0, NULL) == 2)
			returnCode = -1;
		
		}	// end "if (returnCode == 0)"  
		
			// Display an alert indicating that the cluster fields will have
			// to be deleted.	
			// Indicate that the user wants to cancel with a return code of -1.																				
	
	if (returnCode == 2)
		{
		if (DisplayAlert (
				kOKCancelAlertID, kCautionAlert, kAlertStrID, IDS_Alert48, 0, NULL) == 2)
			returnCode = -1;
			
		else	// DisplayAlert (... != 2
			returnCode = 1;
		
		}	// end "if (returnCode == 2)" 
	
	return (returnCode);																						
																				
}	// end "CompareImageDimensionsWithProjectFields"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CompareImageProjectFileInfo
//
//	Software purpose:	The purpose of this routine is to compare the image
//							file information with the project file information.
//							True is returned if the two compare.  False is returned
//							if they do not compare.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/02/1989
//	Revised By:			Larry L. Biehl			Date: 01/31/2006	

Boolean CompareImageProjectFileInfo (
				Handle								fileInfoHandle)

{
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						projectWindowInfoPtr;
	
	Boolean								returnFlag;
	
	
	returnFlag = FALSE;
	if (fileInfoHandle != NULL && gProjectInfoPtr->windowInfoHandle != NULL)
		{
		returnFlag = TRUE;
		
		projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
														
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		
				// Check if image window file information compares to project		
				// image file information.														
	
		if (fileInfoPtr->numberLines != projectWindowInfoPtr->maxNumberLines)    
			returnFlag = FALSE;
																						
		else if (fileInfoPtr->numberColumns != projectWindowInfoPtr->maxNumberColumns)    
			returnFlag = FALSE;
																						
		else if (fileInfoPtr->numberChannels !=
														projectWindowInfoPtr->totalNumberChannels)
			returnFlag = FALSE;

		else if (fileInfoPtr->numberBytes != projectWindowInfoPtr->numberBytes)    
			returnFlag = FALSE;
		
		else if (fileInfoPtr->startLine != gProjectInfoPtr->startLine)    
			returnFlag = FALSE;
		
		else if (fileInfoPtr->startColumn != gProjectInfoPtr->startColumn)   
			returnFlag = FALSE;
			
		else if (fileInfoPtr->signedDataFlag != gProjectInfoPtr->signedDataFlag)  
			returnFlag = FALSE;
			
		else if (fileInfoPtr->hdfDataSetSelection !=
																gProjectInfoPtr->hdfDataSetSelection)
			returnFlag = FALSE;
		
		if (returnFlag && gProjectInfoPtr->version < 911106)
			{
					// Update project structure for earlier disk file versions.	
					
			projectWindowInfoPtr->numberBits = fileInfoPtr->numberBits;
			gProjectInfoPtr->signedDataFlag = fileInfoPtr->signedDataFlag;
			gProjectInfoPtr->swapBytesFlag = fileInfoPtr->swapBytesFlag;
			projectWindowInfoPtr->bandInterleave = fileInfoPtr->bandInterleave;
			gProjectInfoPtr->numberHeaderBytes = fileInfoPtr->numberHeaderBytes;
			gProjectInfoPtr->numberTrailerBytes = fileInfoPtr->numberTrailerBytes;
			gProjectInfoPtr->numberPreLineBytes = fileInfoPtr->numberPreLineBytes;
			gProjectInfoPtr->numberPostLineBytes = fileInfoPtr->numberPostLineBytes;
			
			gProjectInfoPtr->changedFlag = TRUE;
			
			}	// end "if (returnFlag && ..." 
		
		}	// end "if (fileInfoHandle != NULL)" 
		
	return (returnFlag);
																				
}	// end "CompareImageProjectFileInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean	CopyFileInfoFromProject
//
//	Software purpose:	The purpose of this routine is to copy the file 
//							information from the project structure.
//							True is returned if the copy was successful; False is 
//							returned if they do not compare.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			LoadImageInformation in SOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/06/1991
//	Revised By:			Larry L. Biehl			Date: 06/04/1996	

Boolean	CopyFileInfoFromProject (
				Handle								fileInfoHandle)

{
	FileInfoPtr 						fileInfoPtr;
	WindowInfoPtr						projectWindowInfoPtr;
	
	Boolean								returnFlag;
	SignedByte							handleStatus;
	
	
			// Check input variables.															
			
	if (fileInfoHandle == NULL ||
				gProjectInfoPtr == NULL ||
						gProjectInfoPtr->windowInfoHandle == NULL)
																							return (FALSE);
																				
			// Initialize local variables.													
			
	returnFlag = FALSE;
	
			// Get the handle status and then lock the memory, in case it			
			// isn't already locked, until the information has been loaded			
			// in. Then get the pointer to the file information block				
	
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																	fileInfoHandle, &handleStatus);
	
	projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													gProjectInfoPtr->windowInfoHandle, kLock);
	
	if (gProjectInfoPtr != NULL)
		{
		fileInfoPtr->numberLines = projectWindowInfoPtr->maxNumberLines;
		fileInfoPtr->numberColumns = projectWindowInfoPtr->maxNumberColumns;
		fileInfoPtr->numberChannels = projectWindowInfoPtr->totalNumberChannels;
		fileInfoPtr->startLine = gProjectInfoPtr->startLine;
		fileInfoPtr->startColumn = gProjectInfoPtr->startColumn;
		fileInfoPtr->numberBytes = projectWindowInfoPtr->numberBytes;
		fileInfoPtr->numberBits = projectWindowInfoPtr->numberBits;
		fileInfoPtr->numberBins = 0;
		if (fileInfoPtr->numberBits > 0)
			fileInfoPtr->numberBins = (UInt32)ldexp ((double)1, fileInfoPtr->numberBits);
		
		fileInfoPtr->signedDataFlag = gProjectInfoPtr->signedDataFlag;		
		fileInfoPtr->swapBytesFlag = gProjectInfoPtr->swapBytesFlag;
		fileInfoPtr->bandInterleave = projectWindowInfoPtr->bandInterleave;
		fileInfoPtr->numberHeaderBytes 	= gProjectInfoPtr->numberHeaderBytes;
		fileInfoPtr->numberTrailerBytes = gProjectInfoPtr->numberTrailerBytes;
		fileInfoPtr->numberPreLineBytes = gProjectInfoPtr->numberPreLineBytes;
		fileInfoPtr->numberPostLineBytes = gProjectInfoPtr->numberPostLineBytes;
		
		IntermediateFileUpdate (fileInfoPtr);
		
		returnFlag = TRUE;
			
		}	// end "if (gProjectInfoPtr)" 
		
	MHSetState (fileInfoHandle, handleStatus);
		
	return (returnFlag);

}	// end "CopyFileInfoFromProject" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CopyFileInfoHandles
//
//	Software purpose:	The purpose of this routine is to get new memory for the 
//							output file information handles and copy the information from 
//							the input handles to the output handles. The input and
//							output handles will be the same. This will be verified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			LoadProjectFileAndLayerInformation in SProject.cpp
//							OpenProjectImageWindow in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/05/2000
//	Revised By:			Larry L. Biehl			Date: 10/23/2008	

SInt16 CopyFileInfoHandles (
				FileInfoPtr							inputFileInfoPtr, 
				FileInfoPtr							outputFileInfoPtr)

{ 
	SInt16								errCode = NULL;
	
	   	
			// Don't use the same handles for the channel descriptions,		
			// channel values, hiecherarchical info and map projection 
			// for the output file information handle.	
	
	if (outputFileInfoPtr->channelValuesHandle != NULL &&
					outputFileInfoPtr->channelValuesHandle == 
															inputFileInfoPtr->channelValuesHandle)
		{
		errCode = HandToHand (&outputFileInfoPtr->channelValuesHandle);

		}	// end "inputFileInfoPtr->channelValuesHandle != NULL && ..."

	if (errCode == noErr && 
			outputFileInfoPtr->channelDescriptionH != NULL &&
				outputFileInfoPtr->channelDescriptionH ==
															inputFileInfoPtr->channelDescriptionH)
		{
		errCode = HandToHand (&outputFileInfoPtr->channelDescriptionH);

		}	// end "if (errCode == noErr && ..."

	if (errCode == noErr && 
			outputFileInfoPtr->classDescriptionH != NULL &&
				outputFileInfoPtr->classDescriptionH ==
																inputFileInfoPtr->classDescriptionH)
		{
		errCode = HandToHand (&outputFileInfoPtr->classDescriptionH);

		}	// end "if (errCode == noErr && ..."

	if (errCode == noErr && 
			outputFileInfoPtr->controlPointsHandle != NULL &&
				outputFileInfoPtr->controlPointsHandle ==
																inputFileInfoPtr->controlPointsHandle)
		{
		errCode = HandToHand (&outputFileInfoPtr->controlPointsHandle);

		}	// end "if (errCode == noErr && ..."

	if (errCode == noErr && 
			outputFileInfoPtr->groupTablesHandle != NULL &&
				outputFileInfoPtr->groupTablesHandle ==
																inputFileInfoPtr->groupTablesHandle)
		{
		errCode = HandToHand (&outputFileInfoPtr->groupTablesHandle);

		}	// end "if (errCode == noErr && ..."

	if (errCode == noErr && 
			outputFileInfoPtr->groupNameHandle != NULL &&
				outputFileInfoPtr->groupNameHandle == inputFileInfoPtr->groupNameHandle)
		{
		errCode = HandToHand (&outputFileInfoPtr->groupNameHandle);

		}	// end "if (errCode == noErr && ..."

	if (errCode == noErr && 
			outputFileInfoPtr->hdfHandle != NULL &&
				outputFileInfoPtr->hdfHandle == inputFileInfoPtr->hdfHandle)
		{
		if (inputFileInfoPtr->callGetHDFLineFlag)
			{
			errCode = HandToHand (&outputFileInfoPtr->hdfHandle);
			
			if (errCode == noErr)
				{
				errCode = HandToHand (&outputFileInfoPtr->channelToHdfDataSetHandle);
				
				}	// end "if (errCode == noErr)"
			
			}	// end "inputFileInfoPtr->callGetHDFLineFlag"
			
		else	// !inputFileInfoPtr->callGetHDFLineFlag
			{
			outputFileInfoPtr->channelToHdfDataSetHandle = NULL;
			outputFileInfoPtr->hdfHandle = NULL;
				
			}	// end "else !inputFileInfoPtr->callGetHDFLineFlag"

		}	// end "if (errCode == noErr && ..."
	
	if (errCode == noErr && 
			outputFileInfoPtr->hfaHandle != NULL &&
					outputFileInfoPtr->hfaHandle == inputFileInfoPtr->hfaHandle)
		{
		errCode = HandToHand (&outputFileInfoPtr->hfaHandle);

		}	// end "if (errCode == noErr && ..." 
	
	if (errCode == noErr && 
			outputFileInfoPtr->mapProjectionHandle != NULL &&
					outputFileInfoPtr->mapProjectionHandle == 
											inputFileInfoPtr->mapProjectionHandle)
		{
		errCode = CopyMapProjectionHandle (inputFileInfoPtr->mapProjectionHandle,
														&outputFileInfoPtr->mapProjectionHandle);

		}	// end "if (errCode == noErr && ..."
		
			// Make sure that all output file handles are null if there is an error and
			// the output handle is the same as the input handle. We do not want an 
			// output handle to be released that is being used by the input file
			// information structure.
			
	if (errCode != noErr)
		{
		if (outputFileInfoPtr->channelValuesHandle == 
														inputFileInfoPtr->channelValuesHandle)
			outputFileInfoPtr->channelValuesHandle = NULL;
		
		if (outputFileInfoPtr->channelDescriptionH == 
														inputFileInfoPtr->channelDescriptionH)
			outputFileInfoPtr->channelDescriptionH = NULL;
			
		if (outputFileInfoPtr->classDescriptionH == 
														inputFileInfoPtr->classDescriptionH)
			outputFileInfoPtr->classDescriptionH = NULL;
			
		if (outputFileInfoPtr->controlPointsHandle == 
														inputFileInfoPtr->controlPointsHandle)
			outputFileInfoPtr->controlPointsHandle = NULL;
			
		if (outputFileInfoPtr->groupTablesHandle == 
														inputFileInfoPtr->groupTablesHandle)
			outputFileInfoPtr->groupTablesHandle = NULL;
			
		if (outputFileInfoPtr->groupNameHandle == 
														inputFileInfoPtr->groupNameHandle)
			outputFileInfoPtr->groupNameHandle = NULL;
			
		if (outputFileInfoPtr->hdfHandle == inputFileInfoPtr->hdfHandle)
			outputFileInfoPtr->hdfHandle = NULL;
			
		if (outputFileInfoPtr->channelToHdfDataSetHandle == 
														inputFileInfoPtr->channelToHdfDataSetHandle)
			outputFileInfoPtr->channelToHdfDataSetHandle = NULL;
			
		if (outputFileInfoPtr->hfaHandle == inputFileInfoPtr->hfaHandle)
			outputFileInfoPtr->hfaHandle = NULL;
			
		if (outputFileInfoPtr->mapProjectionHandle == 
														inputFileInfoPtr->mapProjectionHandle)
			outputFileInfoPtr->mapProjectionHandle = NULL;
		
		}	// end "if (errCode != noErr)"
		
	return (errCode);

}	// end "CopyFileInfoHandles"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateNewProject
//
//	Software purpose:	The purpose of this routine is to close the current
//							project and open a new project. A check is made to 
//							give the user a chance to save the project file if 
//							it has been changed since the last save.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned:				
// 
// Called By:			ProjectChangesPopUpMen in SProjectUtilities.cpp
//							ClusterDialog in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1989
//	Revised By:			Larry L. Biehl			Date: 03/24/2017

Boolean CreateNewProject (
				Boolean								saveProjectFlag)

{
	ProjectInfoPtr						savedProjectInfoPtr;
	
	Handle								windowInfoHandle;
	SInt16								returnCode;
	
	
			// Initialize local variables.												
			
	returnCode = 0;
	
			// Now check if project has been changed.  If so allow the user	
			// a chance to save it before the project is replaced.				
			
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->changedFlag)
			returnCode = SaveProjectFile (2);
			
		}	// end "if (gProjectInfoPtr != NULL)" 
			
	if (returnCode == 0)
		{
				// If the old project is not to be saved, release the handles	
				// to increase available memory.  The rest of the structure		
				// will be kept until some of the settings of the old project	
				// variables can be copied to the new project.						
		
		savedProjectInfoPtr = gProjectInfoPtr;
				
		DeactivateProject ();
			
		if (gProjectInfoPtr != NULL && !saveProjectFlag)
			ReleaseProjectHandles (gProjectInfoPtr);
			
				// Now create the new project.											
				
		InitializeNewProject (NULL, savedProjectInfoPtr->windowInfoHandle);
		
				// Copy some of the variables from the old project to the new	
				// project.																		
				
		if (gProjectInfoPtr && savedProjectInfoPtr)
			{
			gProjectInfoPtr->listMatrixType = savedProjectInfoPtr->listMatrixType;
			gProjectInfoPtr->listMeanStdPrecision =
														savedProjectInfoPtr->listMeanStdPrecision;
			gProjectInfoPtr->listCovCorPrecision =
														savedProjectInfoPtr->listCovCorPrecision;
			
			gProjectInfoPtr->outlineFieldType = savedProjectInfoPtr->outlineFieldType;
			gProjectInfoPtr->labelFieldCode = savedProjectInfoPtr->labelFieldCode;
			gProjectInfoPtr->outlineColorCode = savedProjectInfoPtr->outlineColorCode;
			gProjectInfoPtr->selectionType = savedProjectInfoPtr->selectionType;
			gProjectInfoPtr->labelSize = savedProjectInfoPtr->labelSize;
			
			gProjectInfoPtr->statsWindowMode = savedProjectInfoPtr->statsWindowMode;
			
					// Decided not to save the processor handles.  3-27-91.  Too big
					// of a chance that the specs will not make sense for the new
					// project.

			gProjectInfoPtr->toFieldControlH = savedProjectInfoPtr->toFieldControlH;
			gProjectInfoPtr->updateControlH = savedProjectInfoPtr->updateControlH;
			gProjectInfoPtr->newFieldControlH = savedProjectInfoPtr->newFieldControlH;
			gProjectInfoPtr->polygonTypeControlH =
														savedProjectInfoPtr->polygonTypeControlH;
			gProjectInfoPtr->toClassControlH = savedProjectInfoPtr->toClassControlH;
			gProjectInfoPtr->addToControlH = savedProjectInfoPtr->addToControlH;
			gProjectInfoPtr->toProjectControlH = savedProjectInfoPtr->toProjectControlH;
			gProjectInfoPtr->listControlH = savedProjectInfoPtr->listControlH;
			gProjectInfoPtr->histogramControlH = savedProjectInfoPtr->histogramControlH;
			gProjectInfoPtr->editNameControlH = savedProjectInfoPtr->editNameControlH;
			
			gProjectInfoPtr->projectNameControlRef =
														savedProjectInfoPtr->projectNameControlRef;
			
					// Exchange the project window information handles.  The		
					// current project window information structure may be in	
					// use.																		
					// Then close the project window information structure		
					// that will not be needed.											
			
			windowInfoHandle = gProjectInfoPtr->windowInfoHandle;
			gProjectInfoPtr->windowInfoHandle = savedProjectInfoPtr->windowInfoHandle;
			savedProjectInfoPtr->windowInfoHandle = windowInfoHandle;
			CloseProjectImageFile (savedProjectInfoPtr);
			
			}	// end "if (gProjectInfoPtr && savedProjectInfoPtr)" 
			
				// Now close the rest of the old project if it is not to be		
				// saved.																		
				
		if (savedProjectInfoPtr && !saveProjectFlag)
			CloseProjectStructure (savedProjectInfoPtr);
			
				// Update the statistics window if it exists.						
				
		if (gProjectWindow != NULL)
			{
			GrafPtr savedPort;
			GetPort (&savedPort);
			SetPortWindowPort (gProjectWindow);
			InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);
			SetPort (savedPort);
			//NewFieldStatMode ();
			
			}	// end "if (gProjectWindow != NULL)"
			
		return (TRUE);
		
		}	// end "if (returnCode == 0)" 
		
	return (FALSE);

}	// end "CreateNewProject" 


                  
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DeactivateProject
//
//	Software purpose:	The purpose of this routine is to deactivate the 
//							active project structure.  The related window and 
//							menu items will be updated.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CloseProjectStructure in SProject.cpp
//							CreateNewProject in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/20/1991
//	Revised By:			Larry L. Biehl			Date: 02/15/2003	

void DeactivateProject (void)

{                                         
	UInt16								classNumber;
	
	
	if (gProjectInfoPtr != NULL)
		{
				// Reset the global project selection window.							
				
		gProjectSelectionWindow = NULL;
			
				// Force the image window to be redrawn to get rid of field			
				// boundaries.																		
		
		if (gProjectInfoPtr->outlineFieldType)
			ForceFieldOutlineUpdate (FALSE);
		
				// Remove class names from class popup menu.					
	
		for (classNumber=1;
					classNumber<=gProjectInfoPtr->numberStatisticsClasses;
						classNumber++)
			{
			#if defined multispec_mac
				DeleteMenuItem (gPopUpClassMenu, 2);
			#endif	// defined multispec_mac
																			
			#if defined multispec_win
				if (gProjectWindow != NULL)
					((CComboBox*)gProjectWindow->
													GetDlgItem (IDC_ClassList))->DeleteString (1);
			#endif	// defined multispec_win
			
			}	// end "for (classNumber=1; ..."
		
				// Set project related flags in window information structure
				// to FALSE.																			
					           
		Handle					windowInfoHandle;
			
		SInt16					windowCount,
									windowListIndex;
									
		WindowInfoPtr			windowInfoPtr; 
								
		
		if (gNumberOfIWindows > 0)
			{
			windowListIndex = kImageWindowStart;
			for (windowCount=0; windowCount<gNumberOfIWindows; windowCount++)
				{
				windowInfoHandle = 
							(Handle)GetWindowInfoHandle (gWindowList[windowListIndex]);
				windowListIndex++;
				
				windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
					
				windowInfoPtr->projectBaseImageFlag = FALSE;
				windowInfoPtr->projectWindowFlag = FALSE;
																	
				}	// end "for (windowCount=0; windowCount<gNumberOfIWindows;..." 
					
			}	// end "if (gNumberOfIWindows > 0)"
			
		}	// end "if (gProjectInfoPtr != NULL)" 

}	// end "DeactivateProject" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle FindProjectBaseImageFileInfoHandle
//
//	Software purpose:	The purpose of this routine is to find the file information
//							handle for the project base image window if it is open.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1999
//	Revised By:			Larry L. Biehl			Date: 01/13/1999	

Handle FindProjectBaseImageFileInfoHandle ()

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								fileInfoHandle,
											windowInfoHandle; 
	
	SInt16								window,
											windowIndex;
	
	
			// Initialize local variables.													
	
	fileInfoHandle = NULL;
	
	if (gNumberOfIWindows > 0)
		{
		window = 0;
		windowIndex = kImageWindowStart;
		do
			{
			windowPtr = gWindowList[windowIndex];
			windowInfoHandle = (Handle)GetWindowInfoHandle (windowPtr);
				
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
						
			if (windowInfoPtr->projectBaseImageFlag)
				{
				fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
				break;
				
				}	// end "if (windowInfoPtr->projectBaseImageFlag)"
			
			window++;
			windowIndex++;
														
			}		while (window<gNumberOfIWindows);
			
		}	// end "if (gNumberOfIWindows > 0)" 
		
	return (fileInfoHandle);
			
}	// end "FindProjectBaseImageFileInfoHandle"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle FindProjectBaseImageWindowInfoHandle
//
//	Software purpose:	The purpose of this routine is to find the window information
//							handle for the project base image window if it is open.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			InitializeClassifierVarStructure in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/31/2002
//	Revised By:			Larry L. Biehl			Date: 02/23/2003	

Handle FindProjectBaseImageWindowInfoHandle ()

{
	WindowInfoPtr						windowInfoPtr;
	WindowPtr							windowPtr;
	
	Handle								activeWindowInfoHandle,
											returnWindowInfoHandle,
											windowInfoHandle; 
	
	SInt16								window,
											windowIndex;
	
	
			// Initialize local variables.													
	
	returnWindowInfoHandle = NULL;
	activeWindowInfoHandle = GetActiveImageWindowInfoHandle ();
	
	if (gNumberOfIWindows > 0)
		{
		window = 0;
		windowIndex = kImageWindowStart;
		do
			{
			windowPtr = gWindowList[windowIndex];
			windowInfoHandle = (Handle)GetWindowInfoHandle (windowPtr);
				
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
						
			if (windowInfoPtr->projectBaseImageFlag)
				{
				returnWindowInfoHandle = windowInfoHandle;
				
				if (windowInfoHandle == activeWindowInfoHandle)
					break;
				
				}	// end "if (windowInfoPtr->projectBaseImageFlag)"
			
			window++;
			windowIndex++;
														
			}		while (window<gNumberOfIWindows);
			
		}	// end "if (gNumberOfIWindows > 0)" 
		
	return (returnWindowInfoHandle);
			
}	// end "FindProjectBaseImageWindowInfoHandle"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Handle FindProjectBaseImageWindowPtr
//
//	Software purpose:	The purpose of this routine is to find the window pointer
//							for the project base image window if it is open.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/16/2003
//	Revised By:			Larry L. Biehl			Date: 01/16/2003	

WindowPtr FindProjectBaseImageWindowPtr ()

{
	WindowPtr							windowPtr;
	
	Handle								windowInfoHandle;
											
			
	windowInfoHandle = FindProjectBaseImageWindowInfoHandle ();
	windowPtr = GetWindowPtr (windowInfoHandle);	
		
	return (windowPtr);
			
}	// end "FindProjectBaseImageWindowPtr"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FindProjectImageWindows
//
//	Software purpose:	The purpose of this routine is to find the image
//							window files that match the name of the image file
//							associated with the project file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ChangeProjectBaseImage in SProject.cpp
//							GetProjectImageFileInfo in SProject.cpp
//							OpenProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/02/1989
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean FindProjectImageWindows (
				Boolean								loadFileAndLayerInfoFlag)

{
	FileStringPtr						fileNamePtr;
	WindowInfoPtr						projectWindowInfoPtr,
											windowInfoPtr;
									
	WindowPtr							windowPtr;
	
	Handle								fileInfoHandle,
											windowInfoHandle; 
	
	SInt16								returnCode,
							 				//strLength,
							 				window,
											windowIndex;
											
	Boolean								continueFlag;
	
	SignedByte							handleStatus;
	
	
			// Initialize local variables.													
	
	continueFlag = FALSE;
	
	if (gNumberOfIWindows > 0)
		{
		continueFlag = loadFileAndLayerInfoFlag;
	
		projectWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
											gProjectInfoPtr->windowInfoHandle, &handleStatus);
		
		returnCode = 1;
		window = 0;
		windowIndex = kImageWindowStart;
		do
			{
			windowPtr = gWindowList[windowIndex];
			windowInfoHandle = (Handle)GetWindowInfoHandle (windowPtr);
			fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
			
					// Load image window file name into a temporary C string.		
			
			FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
			fileNamePtr	= (FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);
			
			if (CompareStringsNoCase (&gProjectInfoPtr->imageFileName[1],
												fileNamePtr) == 0)
				{
						// Check if image window file information compares to 		
						// project image file information.
				
				windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle);
						
				if (windowInfoPtr->numberImageFiles == 1 &&
													CompareImageProjectFileInfo (fileInfoHandle))
					{
							// The image window corresponding to the image file in  	
							// the project file was found.  Indicate that the image 	
							// window is displayed.												
						
					windowInfoPtr->projectWindowFlag = TRUE;
					windowInfoPtr->projectBaseImageFlag = TRUE;
					
							// Also take advantage of the fact that the file			
							// information for the project image is available; load	
							// the project file information structure if it has		
							// not already been done (fileInfo and layerInfo 			
							// structures).
					
					if (continueFlag && projectWindowInfoPtr->fileInfoHandle == NULL)
						continueFlag = LoadProjectFileAndLayerInformation (windowInfoHandle);
					
					}	// end "if (CompareImageProjectFileInfo (..." 
				
				}	// end "if CompareStringsNoCase ((char*)gTextString, ..." 
			
			window++;
			windowIndex++;
														
			}	while (window<gNumberOfIWindows);
			
				// Set return flag to indicate whether the project file and layer	
				// structures have been loaded.												
				
		continueFlag = (projectWindowInfoPtr->fileInfoHandle != NULL);
		
		MHSetState (gProjectInfoPtr->windowInfoHandle, handleStatus);
			
		}	// end "if (gNumberOfIWindows > 0)" 
		
	return (continueFlag);
			
}	// end "FindProjectImageWindows"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ForceProjectCodeResourceLoad
//
//	Software purpose:	The purpose of this is to force the 'Project' code
//							resource to be loaded into memory.  It is called by
//							routines in the 'Menu' code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/03/1993
//	Revised By:			Larry L. Biehl			Date: 02/03/1993

void ForceProjectCodeResourceLoad (void)

{
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																								return;
																							
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;

}	// end "ForceProjectCodeResourceLoad" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetDefaultZeroVarianceFactor
//
//	Software purpose:	The purpose of this routine is to determine the default value 
//							to substitute for 0 variances. Select a value which will work 
//							for worst case when all variances are 0. We want a variance value 
//							which will not cause the determinant to be less than the minimum 
//							allowed. Keep no more than 3 significant digits..
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 03/09/2000
//	Revised By:			Larry L. Biehl			Date: 03/09/2000	

double GetDefaultZeroVarianceFactor (
				UInt32								numberChannels)

{  
	double								defaultZeroVariance;
	
					
	defaultZeroVariance = exp ((gLowerDeterminantOffsetAllowed+2)/numberChannels);
	defaultZeroVariance = MIN (1, defaultZeroVariance);
	defaultZeroVariance = MAX (.01, defaultZeroVariance);
	if (defaultZeroVariance > .01)
		{
		defaultZeroVariance = (double)((UInt32)(defaultZeroVariance * 1000));
		defaultZeroVariance /= 1000;
		
		}	// end "if (gProjectInfoPtr->zeroVarianceFactor > .01)"
			
	return (defaultZeroVariance);

}	// end "GetDefaultZeroVarianceFactor" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNameCPointerFromProjectInfo
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list. It returns a pointer to a C string.
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

void* GetFileNameCPointerFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr)

{  
	return (GetFileNameCPointerFromProjectInfo (projectInfoPtr,
																kDefaultFileStringCode));
	
}	// end "GetFileNameCPointerFromProjectInfo"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void* GetFileNameCPointerFromProjectInfo
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list. It returns a pointer to a C string.
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

void* GetFileNameCPointerFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr,
				SInt16								returnCode)

{  
	FileStringPtr						fileNamePtr = NULL;
	
	
	if (projectInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (projectInfoPtr);
		fileNamePtr = (FileStringPtr)GetFileNameCPointerFromFileStream (fileStreamPtr,
																								returnCode);
		
		}	// end "if (projectInfoPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFileNameCPointerFromProjectInfo"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromProjectInfo
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the project file name represented by the input project info
//							pointer. It returns a pointer to a Pascal string.
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

void* GetFileNamePPointerFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr)

{  
	return (GetFileNamePPointerFromProjectInfo (projectInfoPtr,
																kDefaultFileStringCode));
	
}	// end "GetFileNamePPointerFromProjectInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFileNamePPointerFromProjectInfo
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the project file name represented by the input project info
//							pointer.  It return a pointer to a Pascal string.
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
//	Revised By:			Larry L. Biehl			Date: 09/01/2017	

void* GetFileNamePPointerFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr,
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	
	
	if (projectInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (projectInfoPtr);
		
		fileNamePtr = GetFileNamePPointerFromFileStream (fileStreamPtr, returnCode);
		
		}	// end "if (projectInfoPtr != NULL)" 

	return (fileNamePtr);
	
}	// end "GetFileNamePPointerFromProjectInfo"    



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathPPointerFromProjectInfo
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to the full file path represented by the input project info
//							pointer. It returns a pointer to a Pascal string.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 11/23/2019
//	Revised By:			Larry L. Biehl			Date: 11/23/2019

void* GetFilePathPPointerFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr)

{  
	return (GetFilePathPPointerFromProjectInfo (projectInfoPtr,
																kDefaultFileStringCode));
	
}	// end "GetFilePathPPointerFromProjectInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFilePathPPointerFromProjectInfo
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
//	Coded By:			Larry L. Biehl			Date: 11/23/2019
//	Revised By:			Larry L. Biehl			Date: 11/23/2019

void* GetFilePathPPointerFromProjectInfo (
				ProjectInfoPtr						projectInfoPtr,
				SInt16								returnCode)

{  
	void*									filePathPtr = NULL;
	
	
	if (projectInfoPtr != NULL)
		{              
		CMFileStream* fileStreamPtr = GetFileStreamPointer (projectInfoPtr);
		
		filePathPtr = GetFilePathPPointerFromFileStream (fileStreamPtr, returnCode);
		
		}	// end "if (projectInfoPtr != NULL)" 

	return (filePathPtr);
	
}	// end "GetFilePathPPointerFromProjectInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CMFileStream* GetFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to return the pointer
//							to the file stream structure or class.
//
//	Parameters in:		Project Information Structure pointer		
//
//	Parameters out:	None
//
//	Value Returned:	File Stream Pointer 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1995
//	Revised By:			Larry L. Biehl			Date: 03/18/1998

CMFileStream* GetFileStreamPointer (
				ProjectInfoPtr						projectInfoPtr)
				
{
	if (projectInfoPtr != NULL)
		{
		#if defined multispec_mac || defined multispec_mac_swift
			return (&projectInfoPtr->fileStream);
		#endif	// defined multispec_mac || defined multispec_mac_swift
				
      #if defined multispec_win	|| defined multispec_wx	
			return (projectInfoPtr->fileStreamCPtr);		
		#endif	// defined multispec_win
		
		}	// end "if (projectInfoPtr != NULL)"
		
	else	// projectInfoPtr == NULL
		return (NULL);

}	// end "GetFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectImageFileInfo
//
//	Software purpose:	The purpose of this routine is to get the image
//							file information that is associated with the open 
//							project.  This involves using the image file name
//							that is stored with the project file.
//
//	Parameters in:		userPromptFlag = 1 - allow user to locate the project
//																base image file if it can not
//																be found in the current folder.
//										 		= 0 - do not allow user to select the 
//																project base image file if it
//																can not be found in the current
//																folder.
//
//							setupGlobalInfoPointers = 0 - do not setup globals
//															= 1 - Set up globals
//															= 2 - Set up globals if they can be.
//
//	Parameters out:	None
//
// Value Returned:	True if the information was set up.
//							False if the information could not be set up.
// 
// Called By:			ClassifyControl in SClassify.cpp
//							ClusterControl in SCluster.cpp
//							LoadListDataSpecs in SListData.cpp
//							Menus in MMenus.c
//							EvaluateCovariancesControl in SOther.cpp
//							LoadPrincipalComponentSpecs in SPrincipalComponents.cpp
//							SeparabilityControl in SFeatureSelection.cpp
//							UpdateStatsControl in SProjectComputeStatistics.cpp
//							HistogramStatsControl in SProjectHistogramStatistics.cpp
//							StatisticsControl in SStatistics.cpp
//							ListStatsControl in SProjectListStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/03/1989
//	Revised By:			Larry L. Biehl			Date: 03/20/1999

Boolean GetProjectImageFileInfo (
				Boolean								userPromptFlag, 
				SInt16								setupGlobalInfoPointers)

{ 
	SInt16								projectHandleStatus;
	
	Boolean 								returnFlag;
	
	
			// Initialize local variables.													
			
	returnFlag = FALSE;
	
			// Check some variables.															
			
	if (gProjectInfoPtr != NULL && gProjectInfoPtr->windowInfoHandle != NULL)
		{
		if (gImageWindowInfoPtr == NULL && 
						gImageLayerInfoPtr == NULL && 
									gImageFileInfoPtr == NULL)
			{
			returnFlag = GetProjectImageFileInfo (userPromptFlag,
																setupGlobalInfoPointers,
																&gImageWindowInfoPtr,
																&gImageLayerInfoPtr,
																&gImageFileInfoPtr,
																&projectHandleStatus);
			
			}	// end "if (gImageWindowInfoPtr == NULL && ..." 
				
		if (gImageWindowInfoPtr != NULL && 
					gImageLayerInfoPtr != NULL && 
							gImageFileInfoPtr != NULL)
			returnFlag = TRUE;
			
		}	// end "if (gProjectInfoPtr != NULL && ..."
		
	return (returnFlag);
								
}	// end "GetProjectImageFileInfo" 



Boolean GetProjectImageFileInfo (
				Boolean								userPromptFlag, 
				SInt16								setupGlobalInfoPointers,
				WindowInfoPtr*						projectWindowInfoPtrPtr,
				LayerInfoPtr*						projectLayerInfoPtrPtr,
				FileInfoPtr*						projectFileInfoPtrPtr,
				SInt16*								handleStatusPtr)

{  
	WindowInfoPtr						projectWindowInfoPtr;
	
	Handle 								fileInfoHandle;
	
	Boolean								fileFoundFlag,
			 								returnFlag;
			 								
	SignedByte							handleStatus;
	
	
			// Initialize local variables.													
			
	returnFlag = FALSE;
	*projectWindowInfoPtrPtr = NULL;
	*projectLayerInfoPtrPtr = NULL;
	*projectFileInfoPtrPtr = NULL;
	*handleStatusPtr = 0;
	
			// Check some variables.															
			
	if (gProjectInfoPtr == NULL || gProjectInfoPtr->windowInfoHandle == NULL)
																					return (returnFlag);
																			
	projectWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
							gProjectInfoPtr->windowInfoHandle, &handleStatus, kMoveHi);
		
			// Continue if image file information handle for project is NULL,	
			// i.e., the information has not been loaded.							
		
	fileInfoHandle = projectWindowInfoPtr->fileInfoHandle;
				
	if (fileInfoHandle == NULL)
		{
				// First check if project image file is already one of the 		
				// opened image files.														

		if (!FindProjectImageWindows (TRUE))
			{
					// Project image file is not one of the opened image			
					// windows.  Look for the file on the disk.						
		
			fileFoundFlag = GetSpecifiedImageFile (
								gProjectInfoPtr->imageFileName, userPromptFlag);
								
			}	// end "if (!FindProjectImageWindows (TRUE))" 
			
		fileInfoHandle = projectWindowInfoPtr->fileInfoHandle; 
			
		}	// end "if (projectWindowInfoPtr->fileInfoHandle == NULL)" 
				
	if (fileInfoHandle != NULL &&
							setupGlobalInfoPointers == kDoNotSetupGlobalInfoPointers)
																						returnFlag = TRUE;
			
	if (fileInfoHandle == NULL && 
							setupGlobalInfoPointers == kSetupGlobalInfoPointersIfCan)
																						returnFlag = TRUE;
			
	if (fileInfoHandle != NULL && 
							setupGlobalInfoPointers != kDoNotSetupGlobalInfoPointers)
		{
				// Put the handle status back to the original state so that the
				// output handle state correctly reflects what it was upon input
				// to this routine.
				
		if (handleStatus >= 0)
			CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
		
		returnFlag = GetImageInformationPointers (handleStatusPtr,
																gProjectInfoPtr->windowInfoHandle,
																projectWindowInfoPtrPtr, 
																projectLayerInfoPtrPtr, 
																projectFileInfoPtrPtr);
		
		}	// end "if (fileInfoHandle != NULL && ..." 
		
	if (*projectWindowInfoPtrPtr == NULL && handleStatus >= 0)
		CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
				
	if (*projectWindowInfoPtrPtr != NULL && 
						*projectLayerInfoPtrPtr != NULL && 
										*projectFileInfoPtrPtr != NULL)
		returnFlag = TRUE;
		
	return (returnFlag);
								
}	// end "GetProjectImageFileInfo" 


                      
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectSelectionWindow
//
//	Software purpose:	The purpose of this routine is to set the 
//							'gProjectSelectionWindow' global variable to be the
//							topmost base project image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 07/17/1990
//	Revised By:			Larry L. Biehl			Date: 03/25/1999	

void GetProjectSelectionWindow (void)

{                    
	if (gNumberOfIWindows > 0)
		{        
		#if defined multispec_mac 		
			WindowPtr					window,
											imageWindowPtr;
			                               
			Handle						windowInfoHandle; 
			
			SInt16						imageWindowCount,
											imageWindowIndex;
			
			Boolean						foundFlag;
	
	                          
			window = (WindowPtr)GetWindowList ();
			while (window != NULL && gProjectSelectionWindow == NULL)
				{
						// Find if this window is one of the image windows.				
						
				imageWindowCount = 0;
				imageWindowIndex = kImageWindowStart;
				foundFlag = FALSE;
				do
					{
					imageWindowPtr = gWindowList[imageWindowIndex];
					if (imageWindowPtr == window)
						foundFlag = TRUE;
				
					imageWindowCount++;
					imageWindowIndex++;
															
					}		while (imageWindowCount<gNumberOfIWindows && !foundFlag);
				
				if (foundFlag && IsWindowVisible (window))
					{
					windowInfoHandle = (Handle)GetWindowInfoHandle (imageWindowPtr);
					
					if (windowInfoHandle != NULL &&	
								 ((WindowInfoPtr)*windowInfoHandle)->projectWindowFlag)
						gProjectSelectionWindow = imageWindowPtr;
					
					}	// end "if (foundFlag && ((WindowPeek)window)->visible)" 
				
				window = GetNextWindow (window);
															
				}	// end "while (window != NULL && ..."				  
		#endif	// defined multispec_mac
		                           
      #if defined multispec_win || defined multispec_wx
					// I do not know how to go through a list if image views in
					// from top to bottom order. For now I will just check the
					// active image window to see if it is a project image window. 
		
			Handle activeImageWindowInfoH = GetActiveImageWindowInfoHandle ();
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																				activeImageWindowInfoH);
					
			if (windowInfoPtr != NULL && windowInfoPtr->projectWindowFlag)
				gProjectSelectionWindow = gActiveImageViewCPtr;				  
		#endif	// defined multispec_win || defined multispec_wx
			
		}	// end "if (gNumberOfIWindows > 0)" 
					
}	// end "GetProjectSelectionWindow"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectStatisticsPointers
//
//	Software purpose:	The purpose of this routine is to obtain the channel
//							and sum of squares statistics pointers for the
//							respective input class or field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
//														
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/23/1993
//	Revised By:			Larry L. Biehl			Date: 06/03/1994	

void GetProjectStatisticsPointers (
				SInt16								classFieldCode, 
				SInt32								storageIndex, 
				HChannelStatisticsPtr*			chanStatsPtrPtr, 
				HSumSquaresStatisticsPtr*		sumSquaresStatsPtrPtr, 
				HChannelStatisticsPtr*			modChanStatsPtrPtr, 
				HCovarianceStatisticsPtr*		modCovStatsPtrPtr)

{
	SInt32								modifiedIndex;


	if (chanStatsPtrPtr)
		*chanStatsPtrPtr = NULL;
		
	if (sumSquaresStatsPtrPtr)
		*sumSquaresStatsPtrPtr = NULL;
		
	if (modChanStatsPtrPtr)
		*modChanStatsPtrPtr = NULL;
		
	if (modCovStatsPtrPtr)
		*modCovStatsPtrPtr = NULL;
	
	if (classFieldCode == kFieldStatsOnly)
		{
		if (gProjectInfoPtr->keepClassStatsOnlyFlag)
			storageIndex = 0;
			
		if (gProjectInfoPtr->fieldChanStatsPtr && chanStatsPtrPtr)
			*chanStatsPtrPtr = &gProjectInfoPtr->fieldChanStatsPtr[
								storageIndex * gProjectInfoPtr->numberStatisticsChannels];
										
		if (gProjectInfoPtr->fieldSumSquaresStatsPtr && sumSquaresStatsPtrPtr)
			*sumSquaresStatsPtrPtr = &gProjectInfoPtr->fieldSumSquaresStatsPtr[
								storageIndex * gProjectInfoPtr->numberCovarianceEntries];
						
		}	// end "if (classFieldCode == kFieldStatsOnly)" 
		
	else	// classFieldCode != kFieldStatsOnly 
		{
		if (gProjectInfoPtr->classChanStatsPtr && chanStatsPtrPtr)
			*chanStatsPtrPtr = &gProjectInfoPtr->classChanStatsPtr[
								storageIndex * gProjectInfoPtr->numberStatisticsChannels];
										
		if (gProjectInfoPtr->classSumSquaresStatsPtr && sumSquaresStatsPtrPtr)
			*sumSquaresStatsPtrPtr = &gProjectInfoPtr->classSumSquaresStatsPtr[
								storageIndex * gProjectInfoPtr->numberCovarianceEntries];

		modifiedIndex = 
					gProjectInfoPtr->classNamesPtr[storageIndex].modifiedStatsStorage;
										
		if (gProjectInfoPtr->modifiedClassChanStatsPtr && modChanStatsPtrPtr)
			*modChanStatsPtrPtr = &gProjectInfoPtr->modifiedClassChanStatsPtr[
								modifiedIndex * gProjectInfoPtr->numberStatisticsChannels];
										
		if (gProjectInfoPtr->modifiedClassCovStatsPtr && modCovStatsPtrPtr)
			*modCovStatsPtrPtr = &gProjectInfoPtr->modifiedClassCovStatsPtr[
								modifiedIndex * gProjectInfoPtr->numberCovarianceEntries];
						
		}	// end "else classFieldCode != kFieldStatsOnly" 
	
}	// end "GetProjectStatisticsPointers" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetProjectStructure
//
//	Software purpose:	The purpose of this routine is to get memory for
//							structures which will contain the project 
//							information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			InitializeNewProject in SProject.cpp
//							OpenProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/18/1990
//	Revised By:			Larry L. Biehl			Date: 09/08/2006	

Boolean GetProjectStructure (
				Handle								projectInfoHandle, 
				SInt16								numberChannels)
				
{                               
	#if defined multispec_win || defined multispec_wx
		CMImageWindow* 					imageWindowCPtr = NULL;
	#endif	// defined multispec_win || defined multispec_wx
	
	Handle								channelHandle,
											fileInfoHandle,
											storageClassHandle,
											windowInfoHandle;
									
	SInt32								numberBytes; 
	
	Boolean								newProjectHandleFlag,
											setUpOkay;


			// Initialize local  variables.													
			
	setUpOkay = FALSE;
	channelHandle = NULL;
	storageClassHandle = NULL;
	windowInfoHandle = NULL;
	fileInfoHandle = NULL;
	
			// Set up handle to project file information									
	
	if (projectInfoHandle == NULL)
		{
		projectInfoHandle = MNewHandle (sizeof (ProjectInfo));
		newProjectHandleFlag = TRUE;
		
		}	// end "if (projectInfoHandle == NULL)" 
		
	else	// projectInfoHandle != NULL 
		{
		gProjectInfoPtr = (ProjectInfoPtr)GetHandlePointer (projectInfoHandle);
		
		channelHandle = gProjectInfoPtr->channelHandle;
		storageClassHandle = gProjectInfoPtr->storageClassHandle;
		windowInfoHandle = gProjectInfoPtr->windowInfoHandle;
		fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
		newProjectHandleFlag = FALSE;
		CheckAndUnlockHandle (channelHandle);
		CheckAndUnlockHandle (storageClassHandle);
		CheckAndUnlockHandle (windowInfoHandle);
		CheckAndUnlockHandle (projectInfoHandle);
		setUpOkay = TRUE;
		
		}	// end "else projectInfoHandle != NULL" 
		
	if (projectInfoHandle != NULL && newProjectHandleFlag)
		{             
		setUpOkay = TRUE;
		
      #if defined multispec_win || defined multispec_wx
			gProjectInfoPtr = (ProjectInfoPtr)GetHandlePointer (
																		projectInfoHandle, kLock);
			gProjectInfoPtr->fileStreamCPtr = InitializeFileStream ((CMFileStream*)NULL);
			
			if (gProjectInfoPtr->fileStreamCPtr == NULL)
				setUpOkay = FALSE;

			CheckAndUnlockHandle (projectInfoHandle);			
		#endif	// defined multispec_win || defined multispec_wx
		
				// Get storage for some of project structure information.			
				
				// Get storage for the channel list.  If numberChannels is 0,		
				// get enough space for 10 channels.  										
		
		if (numberChannels <= 0)
			numberChannels = 10;
			
		numberBytes = (SInt32)numberChannels * sizeof (SInt16);
		
		if (setUpOkay)				
			channelHandle = MNewHandleClear (numberBytes);
					
		if (channelHandle != NULL)
				// Get storage for class list.												
			storageClassHandle = MNewHandleClear ((SInt32)256 * sizeof (SInt16));
			
		if (storageClassHandle != NULL)
			{               
			#if defined multispec_mac
				windowInfoHandle = InitializeWindowInfoStructure (windowInfoHandle,
																					kNotPointer, 
																					fileInfoHandle, 
																					kNoImageType,
																					kProjectWindowType);
			#endif	// defined multispec_mac
			                  
         #if defined multispec_win || defined multispec_wx
				imageWindowCPtr = new CMImageWindow (fileInfoHandle);
				if (imageWindowCPtr != NULL)
					{
					imageWindowCPtr->SetImageWindowCPtrInWindowInfo (imageWindowCPtr);
					windowInfoHandle = imageWindowCPtr->GetWindowInfoHandle ();
					
					}	// end "if (imageWindowCPtr != NULL)" 
			#endif	// defined multispec_win
			
			}	// end "if (storageClassHandle != NULL)"
			
		if (windowInfoHandle == NULL)
			setUpOkay = FALSE;
		
		}	// end "if (projectInfoHandle != NULL && newProjectHandleFlag)" 
		
	if (setUpOkay)
		{
		gProjectInfoPtr = (ProjectInfoPtr)GetHandlePointer (
																projectInfoHandle, kLock, kMoveHi);
		
				// Initialize the project structure.										
				
		InitializeProjectStructure (projectInfoHandle, numberChannels);
		
		gProjectInfoPtr->channelHandle = channelHandle;
		gProjectInfoPtr->channelsPtr = (UInt16*)GetHandlePointer (
																channelHandle, kLock, kMoveHi);
		
		gProjectInfoPtr->storageClassHandle = storageClassHandle;
		gProjectInfoPtr->storageClass = (SInt16*)GetHandlePointer (
															storageClassHandle, kLock, kMoveHi);
			
				// Initialize the window information record.								
		                                           
		gProjectInfoPtr->windowInfoHandle = windowInfoHandle;
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																					windowInfoHandle);
		windowInfoPtr->numberImageFiles = 1;    
		
		gProjectInfoPtr->handlesLockedFlag = TRUE;
		
		}	// end "if (setUpOkay)" 
		
	else	// !setUpOkay 
		{
		UnlockAndDispose (projectInfoHandle);
		UnlockAndDispose (channelHandle);
		UnlockAndDispose (storageClassHandle);
		
		#if defined multispec_mac
			UnlockAndDispose (windowInfoHandle); 
		#endif	// defined multispec_mac 
		
      #if defined multispec_win || defined multispec_wx
			delete imageWindowCPtr;
		#endif	// defined multispec_win || defined multispec_wx
		
		}	// end "else !setUpOkay" 
		
	return (setUpOkay);
		
}	// end "GetProjectStructure" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetSpecifiedImageFile
//
//	Software purpose:	This routine tries to locate a file with the input 
//							file name in the same folder as the project file is 
//							in.  The image specifications must be the same as 
//							those saved for the base image in the project file.
//							If the file cannot be located then a dialog
//							box is presented to the user to let him/her locate
//							the file.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			OpenProjectFile in SProject.cpp
//							GetProjectImageFileInfo in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/02/1989
//	Revised By:			Larry L. Biehl			Date: 04/12/2020

Boolean GetSpecifiedImageFile (
				UInt8*								inputFileNamePtr,
				Boolean								userPrompt)

{
	CMFileStream*						fileStreamPtr;
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						projectWindowInfoPtr;
	
	Handle								fileInfoHandle,
											windowInfoHandle;
											
	SInt16								errCode;
	
	Boolean								getFileInfoFromProjectFlag,
											fileFound;
											
	SignedByte							handleStatus;
	

			// Initialize local variables														
			
	fileFound = FALSE;
	windowInfoHandle = NULL;
	
			// Get a new handle to a block of memory to be used for file			
			// information for the new image file.
			// Note that the memory has already been obtained for the
			// Windows version.		
			            	
	fileInfoHandle = InitializeFileInfoStructure (NULL, kNotPointer);
		
	if (inputFileNamePtr != NULL && 
						gProjectInfoPtr != NULL && 
									fileInfoHandle != NULL)
		{
				// Lock the memory until the information has been loaded in.		
				// Then get the pointer to the file information block.				
					
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
												
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		CMFileStream* projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
		
				// Initialize variables in the structure.	
				
		InitializeFileStream (fileStreamPtr, projectFileStreamPtr);
		FileStringPtr fileNamePtr =
						(FileStringPtr)GetFileNameCPointerFromFileStream (fileStreamPtr);
		memcpy (fileNamePtr, (char*)&inputFileNamePtr[1], inputFileNamePtr[0]+1);
		FileStringPtr filePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
		SetFileStringLength (filePathPtr, strlen ((char*)&filePathPtr[2]));
		
				// Copy the HDF Data set information from the project structure to the
				// file information structure. It will be need if an hdf file is the
				// base image for the project.
				
		fileInfoPtr->hdfDataSetSelection = gProjectInfoPtr->hdfDataSetSelection;
		
				// Check if image file is in the same volume as the project
				// file. If it is, open a path to the file and return the			
				// handle to the file information.											
			
		errCode = OpenFileReadOnly (fileStreamPtr, 
												kResolveAliasChains, 
												kLockFile, 
												kVerifyFileStream);		
												
		if (errCode == noErr)
			{
					// Load in the image file information.									
					// If the file information was loaded in, check if image			
					// file information agrees with the project image file			
					// information.																
			
			gGetFileImageType = kMultispectralImageType;
			
			getFileInfoFromProjectFlag = FALSE;
			if (gProjectInfoPtr->version >= 911106)
				getFileInfoFromProjectFlag = TRUE;
				
			if (LoadImageInformation (
								NULL, fileInfoHandle, getFileInfoFromProjectFlag, NULL, 0))
				fileFound = CompareImageProjectFileInfo (fileInfoHandle);
				
			if (!fileFound)
				CloseFile (fileInfoPtr);
				
			}	// end "if (errCode == noErr)" 
			
				// If file not found at this point, allow user to locate the		
				// image file.																		
		
		if (!fileFound && userPrompt)
			fileFound = UserLocateProjectBaseImage (
														fileInfoHandle, IDS_WhereIsProjectImage, 1);
			
		CheckAndUnlockHandle (fileInfoHandle);
		
		}	// end "if (fileNamePtr && ...)" 
	
	if (fileFound)
		{		
		projectWindowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
												gProjectInfoPtr->windowInfoHandle, &handleStatus);
		
		projectWindowInfoPtr->numberImageFiles = 0;
		projectWindowInfoPtr->totalNumberChannels = 0;
		if (UpdateLayerInfoStructure (projectWindowInfoPtr, fileInfoHandle))
			{			
					// The following assignment will indicate that the base
					// image information has been loaded.
					
			SetFileInfoHandle (projectWindowInfoPtr, fileInfoHandle);
			
			}	// end "if (UpdateLayerInfoStructure (..."
		
		else	// !UpdateLayerInfoStructure... 
			fileFound = FALSE;
			
		MHSetState (gProjectInfoPtr->windowInfoHandle, handleStatus);
			
		}	// end "if (fileFound)" 
		
	if (!fileFound)
		{  		
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
												
		if (fileInfoPtr != NULL)
			UnlockAndDispose (fileInfoPtr->channelDescriptionH);
		                                                      
		UnlockAndDispose (fileInfoHandle);
		
		}	// end "if (!fileFound)" 	
		
	return (fileFound);

}	// end "GetSpecifiedImageFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl			Date: 10/26/1995
//	Revised By:			Larry L. Biehl			Date: 10/26/1995	

SInt16 GetVolumeReferenceNumber (
				ProjectInfoPtr						projectInfoPtr)

{  
	SInt16								vRefNum = 0;
	
	
	CMFileStream* fileStreamPtr = GetFileStreamPointer (projectInfoPtr);
	
	vRefNum = GetVolumeReferenceNumber (fileStreamPtr);

	return (vRefNum);
	
}	// end "GetVolumeReferenceNumber" 


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		InitializeNewProject
//
//	Software purpose:	The purpose of this routine is to set up the
//							structures which will contain the statistics 
//							information.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			Menus in MMenus.c
//							CreateNewProject in SProject.cpp
//							StatisticsControl in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/28/1988
//	Revised By:			Larry L. Biehl			Date: 09/05/2017	

void InitializeNewProject (
				Handle								projectInfoHandle, 
				Handle								imageWindowInfoH)

{
	FileInfoPtr							fileInfoPtr;
	
	WindowInfoPtr						projectWindowInfoPtr,
											windowInfoPtr;
	
	SInt16								numberChannels;
	
	Boolean								continueFlag;
	SignedByte							handleStatus;
	
	
	if (!VerifyImageFileCanBeForProject (imageWindowInfoH))
																									return;
	
	numberChannels = 0;
	if (imageWindowInfoH != NULL)
		{
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (imageWindowInfoH);
		numberChannels = windowInfoPtr->totalNumberChannels;
		
		}	// end "if (imageWindowInfoH != NULL)"
	
			// Set up handle to project file information									
			
	if (GetProjectStructure (projectInfoHandle, numberChannels))
		{
		if (imageWindowInfoH != NULL)
			{
					// Get pointer to file information for active image window.		
					
			windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																	imageWindowInfoH, &handleStatus);
			
					// Load channel array.  Assume for now that all channels will  
					// be used.																		
					// The returnFlag here should always be 'TRUE', since the 		
					// channel list handle was set up for 									
					// windowInfoPtr->totalNumberChannels in 'GetProjectStructure'.
					
			continueFlag = 
					ChangeProjectChannelsList (windowInfoPtr->totalNumberChannels);
			
					// Store handle to image window file information.  Assume 		
					// image file is for the active image window.						
					
			Handle fileInfoHandle = GetFileInfoHandle (imageWindowInfoH);
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
			
			
					// Initialize the file stream information to that for the
					// image file.
					
			CMFileStream* projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
			CMFileStream* fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
			InitializeFileStream (projectFileStreamPtr, fileStreamPtr);
			
					// Force the project file type to TEXT
					
			SetType (projectFileStreamPtr, kTEXTFileType);

					// Now set the project file name to nothing.
					// Note that for the Windows side this will set the file type to -1.
					// It will be forced to TEXT when the project file is saved to disk.
			
			SetFileDoesNotExist (projectFileStreamPtr, kDoNotKeepUTF8CharName);
			
					// Store the image file name.
					
			GetCopyOfPFileNameFromFileStream (
												fileStreamPtr, gProjectInfoPtr->imageFileName);
			
			gProjectInfoPtr->startLine = fileInfoPtr->startLine;
			gProjectInfoPtr->startColumn = fileInfoPtr->startColumn;
			gProjectInfoPtr->signedDataFlag = fileInfoPtr->signedDataFlag;
			gProjectInfoPtr->swapBytesFlag = fileInfoPtr->swapBytesFlag;
			gProjectInfoPtr->numberHeaderBytes = fileInfoPtr->numberHeaderBytes;
			gProjectInfoPtr->numberTrailerBytes = fileInfoPtr->numberTrailerBytes;
			gProjectInfoPtr->numberPreLineBytes = fileInfoPtr->numberPreLineBytes;
			gProjectInfoPtr->numberPostLineBytes = fileInfoPtr->numberPostLineBytes;
			gProjectInfoPtr->hdfDataSetSelection = fileInfoPtr->hdfDataSetSelection;
			
			projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
			projectWindowInfoPtr->maxNumberLines = fileInfoPtr->numberLines;
			projectWindowInfoPtr->maxNumberColumns = fileInfoPtr->numberColumns;
			projectWindowInfoPtr->totalNumberChannels = fileInfoPtr->numberChannels;
			projectWindowInfoPtr->numberBytes = fileInfoPtr->numberBytes;
			projectWindowInfoPtr->numberBits = fileInfoPtr->numberBits;
			projectWindowInfoPtr->numberBins = fileInfoPtr->numberBins;
			projectWindowInfoPtr->bandInterleave = fileInfoPtr->bandInterleave;
			projectWindowInfoPtr->numberImageFiles = 1;
			projectWindowInfoPtr->projectWindowFlag = TRUE;
				
					// Indicate that the image window is displayed.						
			
			Handle activeImageWindowInfoH = GetActiveImageWindowInfoHandle ();
			if (activeImageWindowInfoH == imageWindowInfoH)
				{
				windowInfoPtr->projectWindowFlag = TRUE;
				windowInfoPtr->projectBaseImageFlag = TRUE;
				gProjectSelectionWindow = gActiveImageWindow;
				
				}	// end "if (activeImageWindowInfoH == imageWindowInfoH)" 
					
					// Unlock the file information handle if needed.					
			
			MHSetState (imageWindowInfoH, handleStatus);
				
			}	// end "if (imageWindowInfoH)" 
				
				// Indicate that the project information has not changed.  			
				// This will be used to determine whether to offer user chance 	
				// to save the project information when the project is closed.		
				
		gProjectInfoPtr->changedFlag = FALSE;
		
				// Update menu items.															
							
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
		}	// end "if (GetProjectStructure (..." 

}	// end "InitializeNewProject" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeProjectStructure
//
//	Software purpose:	The purpose of this routine is to initialize the
//							project structure.
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
//	Revised By:			Larry L. Biehl			Date: 12/06/2019

void InitializeProjectStructure (
				Handle								projectInfoHandle,
				UInt32								numberChannels)

{
	if (gProjectInfoPtr != NULL)
		{
				// Initialize the some variables and handles in the					
				// structure.			
				
		#if defined multispec_mac
			InitializeFileStream (&gProjectInfoPtr->fileStream);
		#endif	// defined multispec_mac 
		
      #if defined multispec_win	|| defined multispec_wx
			gProjectInfoPtr->fileStreamCPtr = 
		    						InitializeFileStream (gProjectInfoPtr->fileStreamCPtr);
		#endif	// defined multispec_win
	
				// Parameters for train and/or test mask files.
				
		InitializeMaskStructure (&gProjectInfoPtr->testMask);
		InitializeMaskStructure (&gProjectInfoPtr->trainingMask);

				// used in SVM classify training
		
		gProjectInfoPtr->svmModel = NULL;
		gProjectInfoPtr->svm_x = NULL;

				// used in KNN classify training

		gProjectInfoPtr->knnDistancesPtr = NULL;
		gProjectInfoPtr->knnLabelsPtr = NULL;
		gProjectInfoPtr->knnDataValuesPtr = NULL;
		gProjectInfoPtr->knnCounter = 0;
			
		gProjectInfoPtr->startLine = 1;
		gProjectInfoPtr->startColumn = 1;
		gProjectInfoPtr->signedDataFlag = FALSE;
		gProjectInfoPtr->swapBytesFlag = FALSE;
		gProjectInfoPtr->numberHeaderBytes = 0;
		gProjectInfoPtr->numberTrailerBytes = 0;
		gProjectInfoPtr->numberPreLineBytes = 0;
		gProjectInfoPtr->numberPostLineBytes = 0;
		gProjectInfoPtr->numberPreChannelBytes = 0;
		gProjectInfoPtr->numberPostChannelBytes = 0;
		gProjectInfoPtr->imageFileName[0] = 0;
		gProjectInfoPtr->hdfDataSetSelection = 0;
				
		gProjectInfoPtr->zeroVarianceFactor = 
												GetDefaultZeroVarianceFactor (numberChannels);
			
		gProjectInfoPtr->userMixingParameter = 1.0;
		
		gProjectInfoPtr->version = 20060124;
		gProjectInfoPtr->otherInformationPosition = 0;
		gProjectInfoPtr->numberModifiedStorageClasses = 0;
		gProjectInfoPtr->commonCovarianceWeightSet = kEqualWeights;
		gProjectInfoPtr->numberCommonCovarianceClasses = 0;
		gProjectInfoPtr->numberStatisticsClasses = 0;
		gProjectInfoPtr->numberStatTrainClasses = 0;
		gProjectInfoPtr->numberStatisticsFields = 0;
		gProjectInfoPtr->numberStatTrainFields = 0;
		gProjectInfoPtr->numberStatTestFields = 0;
		gProjectInfoPtr->numberTotalPoints = 0;
		gProjectInfoPtr->numberStatisticsChannels = 0;
		gProjectInfoPtr->numberCovarianceEntries = 0;
		gProjectInfoPtr->projectInfoHandle = projectInfoHandle;
		gProjectInfoPtr->overlayImageWindowInfoHandle = NULL;
		gProjectInfoPtr->channelHandle = NULL;
		gProjectInfoPtr->channelsPtr = NULL;
		gProjectInfoPtr->storageClassHandle = NULL;
		gProjectInfoPtr->storageClass = NULL;
		gProjectInfoPtr->currentClass = -1;
		gProjectInfoPtr->currentField = -1;
		gProjectInfoPtr->currentCoordinate = -1;
		gProjectInfoPtr->includesStatisticsFromClusterOperationFlag = FALSE;
		gProjectInfoPtr->newProjectFlag = TRUE;
		gProjectInfoPtr->pixelDataLoadedFlag = FALSE;
		gProjectInfoPtr->statsUpToDate = TRUE;
		gProjectInfoPtr->statsLoaded = FALSE;
		gProjectInfoPtr->changedFlag = FALSE;
		gProjectInfoPtr->handlesLockedFlag = FALSE;
		gProjectInfoPtr->moveMemoryFlag = FALSE;
		gProjectInfoPtr->numberStorageClasses = 0;
		gProjectInfoPtr->numberStorageFields = 0;
		gProjectInfoPtr->numberStoragePoints = 0;
		gProjectInfoPtr->numberStorageStatFields = 0;
		gProjectInfoPtr->editFieldNumber = -1;
		gProjectInfoPtr->editClassStorageNumber = -1;
		gProjectInfoPtr->lastClassForAddFieldSelection = 1;
		gProjectInfoPtr->lastFieldTypeForAddFieldSelection = kTrainingType;
		gProjectInfoPtr->falseColorPaletteBlue = -1;
		gProjectInfoPtr->falseColorPaletteGreen = -1;
		gProjectInfoPtr->falseColorPaletteRed = -1;
		gProjectInfoPtr->imagePalettePopupMenuSelection = kDefaultColors;
		gProjectInfoPtr->lastPointType = 0;
		gProjectInfoPtr->listResultsTestCode = kClassSummary+kAreasUsed+kAreasSummary;
		gProjectInfoPtr->listResultsTrainingCode = 
															kClassSummary+kAreasUsed+kAreasSummary;
		gProjectInfoPtr->listClassFlag = TRUE;
		gProjectInfoPtr->listFieldFlag = FALSE;
		gProjectInfoPtr->histogramClassFieldCode = kHistogramClass;
		gProjectInfoPtr->histogramClassListClassFieldCode = kHistogramClass;
		gProjectInfoPtr->histogramFieldListClassFieldCode = kHistogramClass;
		gProjectInfoPtr->numberGroups = 0;
		gProjectInfoPtr->classGroupPtr = NULL;
		gProjectInfoPtr->groupNameHandle = NULL;
		gProjectInfoPtr->statisticsCode = kMeanCovariance;
		gProjectInfoPtr->keepClassStatsOnlyFlag = TRUE;
		gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
		gProjectInfoPtr->saveClassSumsSquaresFlag = TRUE;
		gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
		gProjectInfoPtr->saveFieldSumsSquaresFlag = FALSE;
		
		gProjectInfoPtr->mixingParameterCode = 1;
		gProjectInfoPtr->useCommonCovarianceInLOOCFlag = TRUE;
		
		gProjectInfoPtr->classPairWeightSet = kEqualWeights;
		gProjectInfoPtr->classWeightSet = kEqualWeights;
		gProjectInfoPtr->modifiedClassWeightSet = kEqualWeights;
		gProjectInfoPtr->numberProjectClassPairs = 0;
		gProjectInfoPtr->defaultClassPairWeight = 10;
		gProjectInfoPtr->classPairWeightsHandle = NULL;
		gProjectInfoPtr->classPairWeightsListHandle = NULL;
				
				// Set parameters for listing no matrix in the output window.		
				
		gProjectInfoPtr->listMatrixType = 4;
		gProjectInfoPtr->listMeanStdPrecision = 1;
		gProjectInfoPtr->listCovCorPrecision = 2;
				
				// Set parameters for outlining both training and test fields		
				// in the project image window.												
				
		gProjectInfoPtr->outlineFieldType = 3;
		
				// Set flags showing labels with the field/area outlines.				
				
		gProjectInfoPtr->labelFieldCode = 3;
		
				// Set code for the color to be used to outline the train/test areas.
				
		gProjectInfoPtr->outlineColorCode = 2;
		
		gProjectInfoPtr->labelSize = 10;
		
				// Set code for type of selection input to use.
				
		gProjectInfoPtr->selectionType = kRectangleType;
	
				// Indicate the stats window mode has not been defined yet			
			
		gProjectInfoPtr->statsWindowMode = 0;
		
		gProjectInfoPtr->setZeroVarianceFlag = TRUE;
		
		gProjectInfoPtr->enhancedStatsExistFlag = FALSE;
		gProjectInfoPtr->covarianceStatsToUse = kOriginalStats;
		gProjectInfoPtr->enhancedStatsOrigin = kOriginalStats;
		
		gProjectInfoPtr->statSourceDefinitionCode = 0;
		
				// Set up handle to image window information.							
				
		gProjectInfoPtr->windowInfoHandle = NULL;
		
				// Set up handle to class name information								
				
		gProjectInfoPtr->classNamesHandle = NULL;
		gProjectInfoPtr->classNamesPtr = NULL;
		
				// Initialize handle to class channel statistics						
			
		gProjectInfoPtr->classChanStatsHandle = NULL;
		gProjectInfoPtr->classChanStatsPtr = NULL;
		
				// Initialize handles to the common covariance statistics
		
		gProjectInfoPtr->commonChannelStatsHandle = NULL;
		gProjectInfoPtr->commonCovarianceStatsHandle = NULL;
		gProjectInfoPtr->localCommonCovarianceLoadedFlag = FALSE;
		
				// Initialize handle to modified class channel statistics			
			
		gProjectInfoPtr->modifiedClassChanStatsHandle = NULL;
		gProjectInfoPtr->modifiedClassChanStatsPtr = NULL;
		
				// Initialize handle to modified class covariance statistics		
			
		gProjectInfoPtr->modifiedClassCovStatsHandle = NULL;
		gProjectInfoPtr->modifiedClassCovStatsPtr = NULL;
		
				// Initialize handle to class sum squares statistics					
			
		gProjectInfoPtr->classSumSquaresStatsHandle = NULL;
		gProjectInfoPtr->classSumSquaresStatsPtr = NULL;
		
				// Set up handle to field name information								
				
		gProjectInfoPtr->fieldIdentifiersHandle = NULL;
		gProjectInfoPtr->fieldIdentPtr = NULL;
		
				// Set up handle to field coordinates										
				
		gProjectInfoPtr->fieldCoordinatesHandle = NULL;
		gProjectInfoPtr->fieldPointsPtr = NULL;
		
				// Initialize handle to field channel statistics						
			
		gProjectInfoPtr->fieldChanStatsHandle = NULL;
		gProjectInfoPtr->fieldChanStatsPtr = NULL;
		
				// Initialize handle to field covariance statistics					
			
		gProjectInfoPtr->fieldSumSquaresStatsHandle = NULL;
		gProjectInfoPtr->fieldSumSquaresStatsPtr = NULL;
		
				// Initialize handle to classification specification.					
		
		gProjectInfoPtr->classifySpecsH = NULL;
		
				// Initialize handle to evaluate covariance specification.			
		
		gProjectInfoPtr->evalCovarianceSpecsHandle = NULL;
		
				// Initialize handle to feature extraction specification.			
		
		gProjectInfoPtr->featureExtractionSpecsH = NULL;
		
				// Initialize handle to separability specification.					
		
		gProjectInfoPtr->separabilitySpecsH = NULL;
		
				// Initialize handle to statistics histogram specification.			
		
		gProjectInfoPtr->statHistogramSpecsH = NULL;
		
				// Initialize handle to statistics enhancement specification.		
		
		gProjectInfoPtr->statisticsEnhanceSpecsH = NULL;
		
		#if defined multispec_mac			
			SetProjectWindowBoxes (NULL, NULL, NULL, NULL, 24);			
		#endif	// defined multispec_mac
			
				// Initialize statistics window control handles.						
				
		gProjectInfoPtr->toFieldControlH = NULL;
		gProjectInfoPtr->updateControlH = NULL;
		gProjectInfoPtr->newFieldControlH = NULL;
		gProjectInfoPtr->polygonTypeControlH = NULL;
		gProjectInfoPtr->toClassControlH = NULL;
		gProjectInfoPtr->addToControlH = NULL;
		gProjectInfoPtr->toProjectControlH = NULL;
		gProjectInfoPtr->listControlH = NULL;
		gProjectInfoPtr->histogramControlH = NULL;
		gProjectInfoPtr->editNameControlH = NULL;
		
		}	// end "if (gProjectInfoPtr != NULL)" 
	
}	// end "InitializeProjectStructure" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LoadProjectFileAndLayerInformation
//
//	Software purpose:	The purpose of this routine is to make a copy of 
//							the input file information structure.  The memory for the 
//							structure will be obtained and copied from the input
//							window information structure.  Also the layer structure
//							will be created.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			FindProjectImageWindows in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1991
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean	LoadProjectFileAndLayerInformation (
				Handle								windowInfoHandle)

{
	CMFileStream*						fileStreamPtr;
	
	FileInfoPtr							fileInfoPtr = NULL,
											inputFileInfoPtr;
											
	WindowInfoPtr						projectWindowInfoPtr;
	
	Handle								inputFileInfoHandle,
											projectImageFileInfoHandle;
	
	SInt16								errCode =  noErr,
											//savedHdfDataSetSelection,
											//savedNumberChannels,
											savedRefNum;
											
	Boolean								continueFlag;
	SignedByte							handleStatus;
	
	
			// Check input variables.															
			
	if (windowInfoHandle == NULL)
																						return (FALSE);
																				
			// Initialize local variables.													
			                                                      
	continueFlag = TRUE;  
	
			// Get the file information handle for the input image file.
			
	inputFileInfoHandle = GetFileInfoHandle (windowInfoHandle);
			 
			// Get the file info handle for the project base image file.	
			
	projectImageFileInfoHandle = InitializeFileInfoStructure (NULL, kNotPointer);
	
	if (projectImageFileInfoHandle != NULL)
		{ 				             
      #if defined multispec_win || defined multispec_wx 	 
					// For the Windows version we need to save the stream pointer in the
					// structure represented by "projectImageFileInfoHandle". It will
					// be written over in the following handle copy.
				
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (projectImageFileInfoHandle);
			fileStreamPtr = GetFileStreamPointer (fileInfoPtr);    								
		#endif	// defined multispec_win           
	
				// Now copy the information from the input image file information to
				// the file info structure for the base image for the project. 
			
		errCode = CopyHandleToHandle (inputFileInfoHandle, projectImageFileInfoHandle);
		
		if (errCode != noErr)
			continueFlag = FALSE;
		      
      #if defined multispec_win || defined multispec_wx 		  
			if (continueFlag)
				{	
						// Reset the project image file stream pointer. It was changed
						// during the copy above.
				
				fileInfoPtr = (FileInfoPtr)GetHandlePointer (projectImageFileInfoHandle);
				fileInfoPtr->fileStreamCPtr = fileStreamPtr;  
				
						// Now copy the input image file stream information to the project
						// image file stream information.         
								
				fileInfoPtr = (FileInfoPtr)GetHandlePointer (inputFileInfoHandle);
				CMFileStream* inputFileStreamPtr = fileInfoPtr->fileStreamCPtr; 
						
				InitializeFileStream (fileStreamPtr, inputFileStreamPtr);
				
				}	// end "else errCode == noErr"  								
		#endif	// defined multispec_win    
		
		}	// end "if (projectImageFileInfoHandle != NULL)"
		                   
	if (continueFlag)
		{
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (
															projectImageFileInfoHandle, kLock);
												
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
				// Set up so that a new stream will be opened to the input image file
				// that will be used by the project information structure.
												    
		savedRefNum = GetReferenceNumber (fileStreamPtr);
		SetReferenceNumber (fileStreamPtr, 0);
		
				// Get copies of some of the file information structure handles.
		
		inputFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (inputFileInfoHandle,
																						&handleStatus);
		
		errCode = CopyFileInfoHandles (inputFileInfoPtr, fileInfoPtr);
		if (errCode != noErr)
			continueFlag = FALSE;
			
				// Save the input data set selection and the number of channels in
				// case needed later to reset the parameters being used for the input
				// image when it represents hdf4/hdf5/netcdf data sets.
				// Decided not to do.
				
		//savedHdfDataSetSelection = inputFileInfoPtr->hdfDataSetSelection;
		//savedNumberChannels = inputFileInfoPtr->numberChannels;
			
		MHSetState (inputFileInfoHandle, handleStatus);
		
		}	// end "if (continueFlag)" 
		
	if (continueFlag)
		{
				// Get a reference number to access the file by.					
		
		errCode = OpenFileReadOnly (fileStreamPtr, 
												kResolveAliasChains, 
												kLockFile, 
												kDoNotVerifyFileStream);			
		IOCheck (errCode, fileStreamPtr);  
		
		if (errCode != noErr)
			continueFlag = FALSE;
		
		}	// end "if (continueFlag)" 
		
	if (fileInfoPtr != NULL && fileInfoPtr->hdf4FileID != 0)
		{
		fileInfoPtr->hdf4FileID = 0;
		
		if (fileInfoPtr->callGetHDFLineFlag)
			{
					// Need to set up new hdf references to access the file by for the
					// project. This is only needed for hdf files with compressed data.
			
			#if include_hdf_capability
				continueFlag = GetNewHDF4FileReferences (fileInfoPtr);
			#endif		// include_hdf_capability
				
			}	// end "if (fileInfoPtr->callGetHDFLineFlag)"
		
		}	// end "fileInfoPtr->hdf4FileID != 0"
		
	#if include_gdal_capability
		if (fileInfoPtr != NULL && fileInfoPtr->gdalDataSetH != 0)
			{
					// Need to set up new gdal references to access the file by for the
					// project.
						
			fileInfoPtr->gdalDataSetH = 0;		
			continueFlag = GetNewGDALFileReference (fileInfoPtr);
			/*
			if (continueFlag && fileInfoPtr->hdfHandle != NULL)
				continueFlag = (LoadGDALInformation (
											fileInfoPtr, NULL, fileInfoPtr->format) == noErr);
				
			if (continueFlag)
				{
						// Force the data set selection and the number of channels to be the 
						// same as that being used for the input image.
				
				fileInfoPtr->hdfDataSetSelection = savedHdfDataSetSelection;
				fileInfoPtr->numberChannels = savedNumberChannels;
				
				}	// end "if (continueFlag)"
			*/
			}	// end "fileInfoPtr->gdalDataSetH != 0"
	#endif	// include_gdal_capability
		
	CheckAndUnlockHandle (projectImageFileInfoHandle);
			
	if (continueFlag)
		{		
		projectWindowInfoPtr	= (WindowInfoPtr)GetHandleStatusAndPointer (
											gProjectInfoPtr->windowInfoHandle, &handleStatus);
		
		projectWindowInfoPtr->numberImageFiles = 0;
		projectWindowInfoPtr->totalNumberChannels = 0;
		continueFlag = 
			UpdateLayerInfoStructure (projectWindowInfoPtr, projectImageFileInfoHandle);
		
		if (continueFlag)
			SetFileInfoHandle (projectWindowInfoPtr, projectImageFileInfoHandle);
			
				// The following line is used in the Windows version to indicate that
				// the file information has been loaded.
			
		projectWindowInfoPtr->fileInfoHandle = projectImageFileInfoHandle;
			
		MHSetState (gProjectInfoPtr->windowInfoHandle, handleStatus);
			
		}	// end "if (continueFlag)" 
		
	if (!continueFlag && projectImageFileInfoHandle != NULL)
		{
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (projectImageFileInfoHandle, kLock);
												
		fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
		
		if (GetReferenceNumber (fileStreamPtr) != savedRefNum)
			CloseFile (fileStreamPtr);
		
		fileInfoPtr->channelDescriptionH =
											UnlockAndDispose (fileInfoPtr->channelDescriptionH);
		UnlockAndDispose (projectImageFileInfoHandle);
		
		}	// end "if (!continueFlag && ..." 
	
	return (continueFlag);

}	// end "LoadProjectFileAndLayerInformation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void OpenNewProject
//
//	Software purpose:	The purpose of this routine is to open a new project base
//							on the active image window. It assumes that no project 
//							is currently open.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			Menus in MMenus.c
//							main in MMultiSpec.c
//							OnProcCluster in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/01/1996
//	Revised By:			Larry L. Biehl			Date: 08/01/1996	

void OpenNewProject (void)

{   
	if (gProjectInfoPtr == NULL)
		{              
				// Make certain that the statistics window is closed at this point.
				
		CloseProjectWindow ();
		
		Handle windowInfoHandle = GetActiveImageWindowInfoHandle ();
		InitializeNewProject (NULL, windowInfoHandle);
		
		}	// end "if (gProjectInfoPtr == NULL)"  

}	// end "OpenNewProject"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void OpenProjectFile
//
//	Software purpose:	The purpose of this routine is to open a project
//							disk file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			Menus in MMenus.c
//							main in MMultiSpec.c
//							OnFileOpenProject in xMainFrame.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/29/1988
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

void OpenProjectFile (
				LocalAppFile*						localAppFilePtr)

{
	GrafPtr								savedPort;
	WindowPtr							projectImageWindow;
	   
	SInt16								returnCode;
	
	Boolean								okFlag; 
	
	
			// Exit if a project is already open.											
			
	if (gProjectInfoPtr != NULL)
																								return;
	
			// Initialize local variables.													
			
	okFlag = FALSE;
	projectImageWindow = NULL;
	
			// Set up handle to project file information									
	
	if (GetProjectStructure (NULL, 0))
		{
				// Get the name of the project file to read the information from.	
				
		if (GetProjectFile (localAppFilePtr))
			{
					// Get status information dialog box.									
						
			gStatusDialogPtr = GetStatusDialog (kGraphicStatusDialogID, FALSE);
		
					// Change cursor to watch cursor until done with process.		
				
			GetPort (&savedPort);
			MSetCursor (kWait);
					            
			returnCode = ReadProjectFile ();
			
			SetPort (savedPort);
				
					// Dispose of status box.													
					
			CloseStatusDialog (TRUE);
			
					// Update any windows that were behind the dialog box so that they
					// are ready for any training and test field to be outlined.
			
			CheckSomeEvents (updateMask);  
			
					// Dispose of the buffer used to read the data into here.		
					
			gCharBufferPtr1 = CheckAndDisposePtr (gCharBufferPtr1);
		
			MInitCursor ();
			
			if (returnCode)
				{
				if (returnCode != -2)
					{
					if (LoadSpecifiedStringNumberLongP (kAlertStrID,
																	IDS_Alert35, 
																	(char*)gTextString,
																	(char*)gTextString2,
																	TRUE,
																	(SInt32)returnCode,
																	0))
						returnCode = DisplayAlert (
										kErrorAlertID, kStopAlert, 0, 0, 0, gTextString);
					
					}	// end "if (returnCode != -2)" 
				
				}	// end "if (returnCode != NULL)" 
			
			else	// returnCode == NULL 
					okFlag = TRUE;
					
			CloseFile (gProjectInfoPtr);
	
					// If project file loaded, try to get handle to image file	
					// information. First check image window files.					
			
			if (okFlag)
				{
						// Reset the zero variance factor. It was original computed with
						// a guess at the number channels when the project information
						// structure was initialized before the project was read in.
				
				gProjectInfoPtr->zeroVarianceFactor = GetDefaultZeroVarianceFactor (
														gProjectInfoPtr->numberStatisticsChannels);
										
						// Get image file information handle for project if the	
						// image file is already displayed in one of the open		
						// image windows.  If the image file is not displayed then	
						// wait to get image file information until it is needed.
						
				if (!FindProjectImageWindows (TRUE))
					{
							// The image file is not displayed.  Ask user if 		
							// he/she wants the file displayed.	 A returnCode of	
							// 1 means display image, 2 means cancel open project	
							// request, and 3 means do not display image.			
							
							//	"\pOpen image window for project: "
					                  
					returnCode = 0;   
					FileStringPtr projectFileNameCPtr = 
						(FileStringPtr)GetFileNameCPointerFromProjectInfo (gProjectInfoPtr);
					if (LoadSpecifiedStringNumberStringP (kAlertStrID,
																		IDS_Alert36, 
																		(char*)gTextString,
																		(char*)gTextString2,
																		TRUE,
																		projectFileNameCPtr))
						returnCode = DisplayAlert (kOpenImageWAlertID,
																kCautionAlert,
																0,
																0,
																0,
																gTextString,
																NULL,
																kUTF8CharString);
						
					if (returnCode == 2)		
						okFlag = FALSE;
						
					if (returnCode == 1)
						{
						if (GetSpecifiedImageFile (gProjectInfoPtr->imageFileName, TRUE))
							OpenProjectImageWindow ();
																
						}	// end "if (returnCode == 1)" 
					
					}	// end "if (!FindProjectImageWindows (TRUE))"
					
				}	// end "if (okFlag)" 
				
			}	// end "if (GetProjectFile ())" 
			
		}	// end "if (GetProjectStructure (NULL, 0))" 
		
			// If routine was not successful in loading the file information	
			// structure, release the handle and set the file pointer to NULL.	
			
	if (!okFlag)  
		CloseProjectStructure (gProjectInfoPtr);
		
	else	// okFlag 
		{
				// Force update of menu items.															
		                          
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
		#if defined multispec_mac 			
					// Add the class names to the class pop up menu. 
					
			UInt16				classIndex;						
			
			for (classIndex=0;
					classIndex<gProjectInfoPtr->numberStatisticsClasses; 
						classIndex++)
				{
				AppendMenu (gPopUpClassMenu, "\pNewClass");
				SetMenuItemText (
						gPopUpClassMenu,
						classIndex+2,
						(ConstStr255Param)&gProjectInfoPtr->classNamesPtr[classIndex].name);
				
				}	// end "for (classIndex=0; classIndex<..." 
		
			if (gProjectInfoPtr->numberStatisticsClasses >= kMaxNumberStatClasses-1)
				{
	 			DisableMenuItem (gPopUpClassMenu, 1);
	 			gProjectInfoPtr->currentClass = 0;
	 			
	 			}	// end "if (... >= kMaxNumberStatClasses-1)" 	 			
		#endif	// defined multispec_mac  
															
				// Outline field boundaries if requested.									
				
		OutlineFieldsInProjectWindows (gProjectInfoPtr->statsWindowMode, kDoClearArea);
		
				// Set the 'gProjectSelectionWindow' window to be the top-most		
				// base image window.															
				
		GetProjectSelectionWindow ();

		}	// end "else okFlag" 

}	// end "OpenProjectFile" 


                  
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void OpenProjectImageWindow
//
//	Software purpose:	The purpose of this routine is to open a project image window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			Menus in MMenus.c
//							OnFileOpenProjectImage in xMainFrame.cpp
//							OpenProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/18/1991
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

void	OpenProjectImageWindow (void)

{                         
	Handle								fileInfoHandle, 
											projectFileInfoHandle,
											projectWindowInfoHandle,
											windowInfoHandle;                  
		
	FileInfoPtr							fileInfoPtr = NULL;
		
		
	gProcessorCode = kOpenProjectImageProcessor;

			// Initialize local variables.													
				
	windowInfoHandle = NULL;
		
			// Check handle to image file information.  If handle to image file
			// information doesn't exists. Find the image file and get the information
			// for it.
	
	if (GetProjectImageFileInfo (kPrompt, kDoNotSetupGlobalInfoPointers))
		{
		projectWindowInfoHandle = gProjectInfoPtr->windowInfoHandle;
		fileInfoHandle = projectFileInfoHandle =
													GetFileInfoHandle (projectWindowInfoHandle);
												
		#if defined multispec_mac   			
			CMFileStream*				fileStreamPtr;
			FileInfoPtr					projectFileInfoPtr; 
			SignedByte					projectHandleStatus;
			SInt16						errCode;
			
					// Get a new handle to a block of memory to be used for				
					// file information for the new image window.							
				
			errCode = HandToHand (&fileInfoHandle);
				
			if (errCode == noErr)
				{
				fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
				fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
				SetReferenceNumber (fileStreamPtr, 0);
		
				projectFileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
													projectFileInfoHandle, &projectHandleStatus);
					
						// Don't use the same handles in the project and image 
						// file information structures.		
						
				errCode = CopyFileInfoHandles (projectFileInfoPtr, fileInfoPtr);	
				
				MHSetState (projectFileInfoHandle, projectHandleStatus);
					
				}	// end "if (errCode == noErr)" 	
				
			#if include_gdal_capability
				if (fileInfoPtr != NULL && fileInfoPtr->gdalDataSetH != 0)
					{
							// Need to set up new gdal references to access the file for
							// the project.
								
					fileInfoPtr->gdalDataSetH = 0;		
					if (!GetNewGDALFileReference (fileInfoPtr))
						errCode = 1;
						
					//if (errCode == noErr && fileInfoPtr->hdfHandle != NULL)
					//	errCode = LoadGDALInformation (
					//									fileInfoPtr, NULL, fileInfoPtr->format);
					
					}	// end "fileInfoPtr->gdalDataSetH != 0"
			#endif	// include_gdal_capability

			#if include_hdf_capability
				if (errCode == noErr && 
								(fileInfoPtr->format == kHDF4Type ||
																fileInfoPtr->format == kNETCDFType))
					errCode = LoadHDF4Information (fileInfoPtr, fileInfoPtr->format, TRUE);
			#endif		// include_hdf_capability
				
			if (errCode == noErr)
				{
						// Get a reference number to access the file by.					
					
				errCode = OpenFileReadOnly (fileStreamPtr, 
														kResolveAliasChains, 
														kLockFile, 
														kDoNotVerifyFileStream);		
				IOCheck (errCode, fileStreamPtr);
						
				}	// end "if (errCode == noErr)" 
					
			CheckAndUnlockHandle (fileInfoHandle);
				
			if (errCode == noErr)
				{
						// Get block of memory to store information about the image.	
						
				windowInfoHandle = GetWindowInfoStructures (fileInfoHandle, 
																			kMultispectralImageType, 
																			kImageWindowType);
						
				}	// end "if (errCode == noErr)" 
				
			if (windowInfoHandle != NULL)
				SetUpImageWindow (windowInfoHandle);
			
			if (windowInfoHandle == NULL && fileInfoHandle != NULL)
				{
				fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle, kLock);
				CloseFile (fileInfoPtr);
				CheckAndUnlockHandle (fileInfoHandle);
					
				}	// end "if (windowInfoHandle == NULL && ..."		
		#endif	// defined multispec_mac 				
				
		#if defined multispec_win  
			CDocument* 		documentPtr = NULL;
				 
				                                        
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
				
			TBYTE* filePathPointer = (TBYTE*)GetFilePathPPointerFromFileInfo (
																		fileInfoPtr, kReturnUnicode);
					
			((CMultiSpecApp*)AfxGetApp ())->OpenImageFileWin (
																filePathPointer,
																&documentPtr,
																TRUE,
																fileInfoPtr->hdfDataSetSelection);				
		#endif	// defined multispec_win

		#if defined multispec_wx
			fileInfoPtr = (FileInfoPtr) GetHandlePointer (fileInfoHandle);

			wchar_t* filePathPointer =
					(wchar_t*)GetFilePathPPointerFromFileInfo (fileInfoPtr, kReturnUnicode);

			((CMultiSpecApp*)wxTheApp)->OpenImageFileLin (
																filePathPointer,
																TRUE,
																fileInfoPtr->hdfDataSetSelection);
		#endif	// defined multispec_wx
		
		}	// end "if (GetProjectImageFileInfo (..."
   
}	// end "OpenProjectImageWindow"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseProjectHandles
//
//	Software purpose:	The purpose of this routine is to release the memory
//							for the handles in the input project structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			CloseProjectStructure in SProject.cpp
//							CreateNewProject in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/22/1991
//	Revised By:			Larry L. Biehl			Date: 08/15/2019

void ReleaseProjectHandles (
				ProjectInfoPtr						inputProjectInfoPtr)

{
	if (inputProjectInfoPtr != NULL)
		{
				// Dispose of handle to class list information.							
				
		inputProjectInfoPtr->storageClassHandle = 
						UnlockAndDispose (inputProjectInfoPtr->storageClassHandle);
		
				// Dispose of handle to channel information.								
				
		inputProjectInfoPtr->channelHandle = 
						UnlockAndDispose (inputProjectInfoPtr->channelHandle);
		
				// Dispose of handle to class name information							
				
		inputProjectInfoPtr->classNamesHandle = 
						UnlockAndDispose (inputProjectInfoPtr->classNamesHandle);
		
				// Dispose of handle to field name information							
				
		inputProjectInfoPtr->fieldIdentifiersHandle = 
						UnlockAndDispose (inputProjectInfoPtr->fieldIdentifiersHandle);
		
				// Dispose of handle to field coordinates									
				
		inputProjectInfoPtr->fieldCoordinatesHandle = 
						UnlockAndDispose (inputProjectInfoPtr->fieldCoordinatesHandle);
				
				// Release memory for class pair weights vector if it has been 	
				// allocated.																		
		
		inputProjectInfoPtr->classPairWeightsHandle =		
						UnlockAndDispose (inputProjectInfoPtr->classPairWeightsHandle);
				
				// Release memory for user class pair weights list vector if 		
				// it has been allocated.														
		
		inputProjectInfoPtr->classPairWeightsListHandle = 		
						UnlockAndDispose (inputProjectInfoPtr->classPairWeightsListHandle);
				
				// Close test and training mask files and release memory.
		
		CloseMaskStructure (&inputProjectInfoPtr->testMask);
		
		CloseMaskStructure (&inputProjectInfoPtr->trainingMask);	
		
				// Dispose of the handle for the statistics.								
				
		ReleaseStatisticsHandles (inputProjectInfoPtr, 
											kOriginalStats+kLeaveOneOutStats+kEnhancedStats);
		
				// Dispose of classification specifications if they exist.			
				
		ReleaseClassifySpecsMemory (&inputProjectInfoPtr->classifySpecsH);
		
				// Dispose of feature extraction specifications if they exist.		
				
		ReleaseFeatureExtractionSpecsMemory (
											&inputProjectInfoPtr->featureExtractionSpecsH);
		
				// Dispose of separability specifications if they exist.				
				
		ReleaseSeparabilitySpecsMemory (&inputProjectInfoPtr->separabilitySpecsH);
		
				// Dispose of statistics histogram specifications if they exist.	
				
		ReleaseStatHistogramSpecsMemory (&inputProjectInfoPtr->statHistogramSpecsH);
		
				// Dispose of statistics histogram specifications if they exist.	
				
		ReleaseStatisticsEnhanceSpecsMemory (
											&inputProjectInfoPtr->statisticsEnhanceSpecsH);
											
				// Dispose of 'covariance check' specification structure.

		ReleaseEvalCovarianceHandleMemory (
											&inputProjectInfoPtr->evalCovarianceSpecsHandle);
		
				// Dispose of pointers.
		
		gProjectInfoPtr->knnDistancesPtr =
						(knnType*)CheckAndDisposePtr ((Ptr)gProjectInfoPtr->knnDistancesPtr);
		
		gProjectInfoPtr->knnLabelsPtr =
										CheckAndDisposePtr (gProjectInfoPtr->knnLabelsPtr);
		
		gProjectInfoPtr->knnDataValuesPtr =
										CheckAndDisposePtr (gProjectInfoPtr->knnDataValuesPtr);

		}	// end "if (inputProjectInfoPtr != NULL)" 
		
}	// end "ReleaseProjectHandles" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseStatisticsHandles
//
//	Software purpose:	The purpose of this routine is to release the memory
//							for the handles for the project statistics.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ReleaseProjectHandles in SProject.cpp
//							ClearProjectStatistics in SEditStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/31/1992
//	Revised By:			Larry L. Biehl			Date: 12/08/1999	

void ReleaseStatisticsHandles (
				ProjectInfoPtr						inputProjectInfoPtr,
				SInt16								statsCode)

{
	if (inputProjectInfoPtr != NULL)
		{
		if (statsCode & kOriginalStats)
			{
					// Dispose of handle to class channel statistics						
				
			inputProjectInfoPtr->classChanStatsHandle =
						UnlockAndDispose (inputProjectInfoPtr->classChanStatsHandle);
			inputProjectInfoPtr->classChanStatsPtr = NULL;
			
					// Dispose of handle to class sum of squares statistics				
				
			inputProjectInfoPtr->classSumSquaresStatsHandle = 
					UnlockAndDispose (inputProjectInfoPtr->classSumSquaresStatsHandle);
			inputProjectInfoPtr->classSumSquaresStatsPtr = NULL;
			
					// Dispose of handle to field channel statistics						
				
			inputProjectInfoPtr->fieldChanStatsHandle =
							UnlockAndDispose (inputProjectInfoPtr->fieldChanStatsHandle);
			inputProjectInfoPtr->fieldChanStatsPtr = NULL;
			
					// Dispose of handle to field sum of squares statistics				
				
			inputProjectInfoPtr->fieldSumSquaresStatsHandle = 
					UnlockAndDispose (inputProjectInfoPtr->fieldSumSquaresStatsHandle);
			inputProjectInfoPtr->fieldSumSquaresStatsPtr = NULL;
			
			}	// end "if (statsCode & kOriginalStats)"
			
		if (statsCode & kLeaveOneOutStats)
			{
					// Dispose of handle to common covariance channels statistics	
					
			inputProjectInfoPtr->commonChannelStatsHandle = 
					UnlockAndDispose (inputProjectInfoPtr->commonChannelStatsHandle);
			
					// Dispose of handle to common covariance statistics	
					
			inputProjectInfoPtr->commonCovarianceStatsHandle = 
					UnlockAndDispose (inputProjectInfoPtr->commonCovarianceStatsHandle);
					
			}	// end "if (statsCode & kLeaveOneOutStats)"
			
		if (statsCode & kEnhancedStats)
			{
					// Dispose of handle to modified class channels statistics			
				
			inputProjectInfoPtr->modifiedClassChanStatsHandle = 
					UnlockAndDispose (inputProjectInfoPtr->modifiedClassChanStatsHandle);
			inputProjectInfoPtr->modifiedClassChanStatsPtr = NULL;
			
					// Dispose of handle to modified class covariance statistics		
				
			inputProjectInfoPtr->modifiedClassCovStatsHandle = 
					UnlockAndDispose (inputProjectInfoPtr->modifiedClassCovStatsHandle);
			inputProjectInfoPtr->modifiedClassCovStatsPtr = NULL;
			
			}	// end "if (statsCode & kEnhancedStats)"
		
		}	// end "if (inputProjectInfoPtr != NULL)" 
		
}	// end "ReleaseStatisticsHandles" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UnlockProjectWindowInfoHandles
//
//	Software purpose:	This routine unlocks the window information handles
//							associated with the project.  The global pointers the those
//							structures are set to NULL.
//
//	Parameters in:		None.
//
//	Parameters out:	None.		
//
// Value Returned:	None.
//
// Called By:			ClassifyControl in SClassify.cpp
//							ClusterControl in SCluster.cpp
//							LoadListDataSpecs in SListData.cpp
//							Menus in MMenus.c
//							EvaluateCovariancesControl in SOther.cpp
//							LoadPrincipalComponentSpecs in SPrincipalComponents.cpp
//							SeparabilityControl in SFeatureSelection.cpp
//							UpdateStatsControl in SProjectComputeStatistics.cpp
//							HistogramStatsControl in SProjectHistogramStatistics.cpp
//							StatisticsControl in SStatistics.cpp
//							ListStatsControl in SProjectListStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/18/1991
//	Revised By:			Larry L. Biehl			Date: 12/29/1995

void UnlockProjectWindowInfoHandles (void)

{
	WindowInfoPtr						windowInfoPtr;
	
			
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->windowInfoHandle)
			{                                          
			windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															gProjectInfoPtr->windowInfoHandle,
															kLock);
			
			Handle fileInfoHandle = GetFileInfoHandle (windowInfoPtr);
			CheckAndUnlockHandle (fileInfoHandle);
		                                               
			Handle layerInfoHandle = GetLayerInfoHandle (windowInfoPtr);
			CheckAndUnlockHandle (layerInfoHandle);
		
			CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
			
			if (windowInfoPtr == gImageWindowInfoPtr)
				{
				gImageWindowInfoPtr = NULL;
				gImageLayerInfoPtr = NULL;
				gImageFileInfoPtr = NULL;
				
				}	// end "if (windowInfoPtr == gImageWindowInfoPtr)" 
				
			}	// end "if (gProjectInfoPtr->windowInfoHandle)" 
			
		}	// end "if (gProjectInfoPtr != NULL)" 
		
}	// end "UnlockProjectWindowInfoHandles" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean UpdateProjectMapProjectionHandle
//
//	Software purpose:	This routine updates the project map projection handle with
//							the information in the input map projection handle.
//
//	Parameters in:		inputMapProjectionHandle - handle to input map projection info.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 08/02/2010
//	Revised By:			Larry L. Biehl			Date: 08/02/2010

void UpdateProjectMapProjectionHandle (
				Handle								inputMapProjectionHandle)

{
	Handle								projectCoefficientsHandle,
											projectMapProjectionHandle;
	
	
	if (gProjectInfoPtr != NULL && inputMapProjectionHandle != NULL)
		{
		projectMapProjectionHandle = 
							GetFileMapProjectionHandle2 (gProjectInfoPtr->windowInfoHandle);
		
				// Release the current project map projection information.
		
		projectCoefficientsHandle = GetCoefficientsHandle (projectMapProjectionHandle);
		UnlockAndDispose (projectCoefficientsHandle);
		UnlockAndDispose (projectMapProjectionHandle);
		projectMapProjectionHandle = NULL;
		
		CopyMapProjectionHandle (inputMapProjectionHandle,
											&projectMapProjectionHandle);
		
		SetFileMapProjectionHandle2 (gProjectInfoPtr->windowInfoHandle,
												projectMapProjectionHandle);
		
		}	// end "if (gProjectInfoPtr != NULL && inputMapProjectionHandle != NULL"

}	// end "UpdateProjectMapProjectionHandle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean UserLocateProjectBaseImage
//
//	Software purpose:	This routine allows the user to locate the project
//							base image file.
//
//	Parameters in:		fileInfoPtr - pointer to structure for file information.
//
//							promptStringNumber - resource string number of prompt.
//	
//							option - if 1, then the selected base image is compared
//											to the project file information to check if
//											it is the correct file.
//
//									 - if 2, then the selected base image is to be a
//											new base image for the project.  A check is
//											made to see if the project train/test fields fit
//											within the new base image file.
//
//	Parameters out:	None.		
//
// Value Returned:	FALSE if the project base image file is not located.
//							TRUE if the project base image file is located.
//
// Called By:			GetSpecifiedImageFile in SProject.cpp	
//							ChangeProjectBaseImage in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/21/1990
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

Boolean UserLocateProjectBaseImage (
				Handle								fileInfoHandle, 
				SInt16								promptStringNumber, 
				SInt16								option)

{
	char									alertString[1000];
	
	CMFileStream*						fileStreamPtr;
	
	FileInfoPtr							fileInfoPtr;
	
	SInt16								errCode,
											itemHit,
											numberFileTypes,
											returnCode;
	
	Boolean								continueFlag,
				 							fileFound;
	
	SignedByte							handleStatus;
	

		// Check input variables.															
			
	if (fileInfoHandle == NULL)
																							return (FALSE);
			
	if (option < 1 || option > 2)
																							return (FALSE);
																						
			// Initialize local variables.													
			
	fileFound = FALSE;
	
			// Lock handle to file information and get pointer to it.				
			
	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
																fileInfoHandle, &handleStatus);
																
	fileStreamPtr = GetFileStreamPointer (fileInfoPtr);
	
			// Make sure the mUTF8PathLength in fileStreamPtr is set to 0. It may
			// still represent the path length for the project file. The user
			// may select a different location for the project image file in
			// which case this length needs to be recalcuated.
	
	#if defined multispec_wx || multispec_win
		fileStreamPtr->mUTF8PathLength = 0;
	#endif	 // defined multispec_wx || multispec_win
	
			// If the option key was down when the 'Open Image' menu item			
			// was selected then include all files in the file list.  				
			// Otherwise just show the selected file types.								
			
	numberFileTypes = GetNumberFileTypes ();

			// Loop until an acceptable base image file is located or the user	
			// cancels.																				
			
	do
		{	
		SetType (fileStreamPtr, kTEXTFileType);
		errCode = GetFile (fileStreamPtr,
									numberFileTypes, 
									gListTypes, 
									NULL, 
									NULL,
									NULL, 
									promptStringNumber);
	   continueFlag = ((errCode == noErr) & FileExists (fileStreamPtr));
	   				
	   		// Continue if no IO error and user did not cancel.					
	   
	   if (continueFlag)
	   	{
					// Load in the image file information.									
					// If the file information was loaded in, check if 				
					// image file information agrees with the project					
					// image file information.													
			
			gGetFileImageType = kMultispectralImageType;
			if (LoadImageInformation (NULL, fileInfoHandle, FALSE, NULL, 0))
				{
				if (option == 1)
					{									
					fileFound = CompareImageProjectFileInfo (fileInfoHandle);
					
					if (fileFound)
						{
								// Now check if the image file is the same as that 
								// identified in the project file. If not then change
								// the image file name in the project structure, indicate
								// that the project has changed and for the statistics
								// to be recomputed.		
						
						FileStringPtr fileNamePtr =
								(FileStringPtr)GetFileNameCPointerFromFileInfo (fileInfoPtr);
						FileStringPtr projectImageNamePtr =
													(FileStringPtr)gProjectInfoPtr->imageFileName;
						
								// Check if the file names agree.										
									
						if (CompareStringsNoCase (&projectImageNamePtr[1],
																			fileNamePtr) != 0)
							{	
							MGetString (gTextString3, kAlertStrID, IDS_Alert75);
							itemHit = 2;
							if (gTextString3[0] != 0)
								{
								SInt16 length = sprintf (&alertString[1],
																	(char*)&gTextString3[1],
																	&projectImageNamePtr[1],
																	fileNamePtr);
																	
								length = MIN (length, 254);
								alertString[0] = (UInt8)length;
	
								itemHit = DisplayAlert (kYesNoAlertID, 
																kStopAlert,
																0, 
																0,
																0, 
																(UCharPtr)alertString);
													
								}	// end "if (gTextString3[0] != 0)"
								
							if (itemHit == 1)
								{
								ClearProjectStatistics (1);
								GetCopyOfPFileNameFromFileInfo (
													fileInfoPtr, gProjectInfoPtr->imageFileName);
								
								}	// end "if (itemHit == 1)"
								
							else	// itemHit != 1
								fileFound = FALSE;
							
							}	// end "if (CompareStringsNoCase (..."
																		
						}	// end "if (fileFound)"
						
					else	// !fileFound
							// Display an alert indicating that the selected image 	
							// file description does not match that expected as the 	
							// base image for the project file.								
					
						returnCode = DisplayAlert (
								kErrorAlertID, kStopAlert, kAlertStrID, IDS_Alert12, 0, NULL);
						
					}	// end "if (option == 1)" 
					
				if (option == 2)
					{
					returnCode = CompareImageDimensionsWithProjectFields (fileInfoPtr);
					if (returnCode == 1)
						fileFound = TRUE;
						
					if (returnCode < 0)
						continueFlag = FALSE;
					
					}	// end "if (option == 2)" 
					
				if (!fileFound)
					CloseFile (fileStreamPtr);
					
				}	// end "if (LoadImageInformation (fileInfoHandle, ..." 
					
			}	// end "if (continueFlag)" 
			
		}		while (continueFlag && !fileFound);
				
			// Unlock the file information handle if needed.							
	
	MHSetState (fileInfoHandle, handleStatus);
		
	return (fileFound);

}	// end "UserLocateProjectBaseImage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean VerifyImageFileCanBeForProject
//
//	Software purpose:	This routine determines if the input image window represents
//							a file that can be used as a base image for a project.
//
//	Parameters in:		
//
//	Parameters out:	None.		
//
// Value Returned:	FALSE if the image file cannot be used as base image for project.
//							TRUE if the image file can be used as base image for project.
//
// Called By:			GetSpecifiedImageFile in SProject.cpp	
//							ChangeProjectBaseImage in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/2001
//	Revised By:			Larry L. Biehl			Date: 04/19/2019

Boolean VerifyImageFileCanBeForProject (
				Handle								imageWindowInfoH)

{
	Boolean								returnFlag = FALSE;
	
	
	if (imageWindowInfoH != NULL)
		{
		returnFlag = DetermineIfHDFProjectPossible (imageWindowInfoH);
		
		if (!returnFlag)
					// Display an alert indicating that MultiSpec cannot use this HDF file
					// as a base image for a project.
			
			DisplayAlert (kErrorAlertID,
								kStopAlert,
								kAlertStrID,
								IDS_Alert105,
								IDS_Alert106,
								NULL);
		
		}	// end "if (imageWindowInfoH != NULL) "
	
	return (returnFlag);

}	// end "VerifyImageFileCanBeForProject" 
