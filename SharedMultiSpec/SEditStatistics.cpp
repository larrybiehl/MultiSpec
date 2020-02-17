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
//	File:						SEditStatistics.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/19/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains routines that edit the field and
//								class statistics.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"     

#if defined multispec_wx
	#include "wx/wx.h"
	#include "xDialog.h"
	#include "xImageView.h"
	#include "xNewClassFieldDialog.h"
	#include "xEditClassFieldDialog.h"
	#include "xStatisticsFrame.h"
	#include "xStatisticsView.h"
#endif	// defined multispec_wx

#if defined multispec_mac || defined multispec_mac_swift
	#define IDC_ClassNamePrompt		3
	#define IDC_ClassName				4
	#define IDC_FieldNamePrompt		5
	#define IDC_FieldName				6   
	#define IDC_Training					7 
	#define IDC_TestField				8
	#define IDC_ClassListPrompt		9
	#define IDC_ClassList				10
	#define IDC_ClassNumberPixels		11
	#define IDC_FieldNumberPixels		12
#endif	// defined multispec_mac || defined multispec_mac_swift
  
#if defined multispec_win 
	#include "WImageView.h" 
	#include "WNewClassFieldDialog.h"
	#include "WEditClassFieldDialog.h"
	#include "WStatisticsView.h"
#endif	// defined multispec_win



		// Prototypes for routines in this file that are only called by
		// other routines in this file.
	
void AddField (
				SInt16								addField,
				SInt16								currentStorageClass);

void AddFieldStatsToClassStats (
				SInt16								storageClass,
				SInt16								currentField);

void ChangeFieldType (
				SInt16								classStorage,
				SInt16								currentField, 
				SInt16								fieldType);

void CutClass (
				SInt16								removeClass);
							
void CutEmptyClassesFromProject (void);

void CutField (
				SInt16								removeField,
				SInt16								currentClass);

void CutPolygonPoint (
				SInt16	 							removePoint,
				SInt16	 							currentField, 
				SInt16	 							currentClass);

void PasteClassFields (void);

void PasteField (void);

void PasteFieldToNewClass (void);

void RemoveFieldStatsFromClassStats (
				SInt16								storageClass,
				SInt16								currentField,
				Boolean								fieldSizeChangeFlag);

void SetLoadedIntoClassStats (
				SInt16								storageClass,
				Boolean								loadedIntoClassStatsFlag);

void UndoCutClass (void);

void UndoCutField (void);


                                  	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddField
//
//	Software purpose:	The purpose of this routine is to add the input
// 						field into the input class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			UndoCutField in SEditStatistics.cpp
//							PasteField in SEditStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/22/1989
//	Revised By:			Larry L. Biehl			Date: 02/17/1999			

void AddField (
				SInt16								addField, 
				SInt16								currentStorageClass)

{
	Cell									cell;
	
	HPClassNamesPtr					classNamesPtr;
		
	SInt16								classNumber,
											fieldType,
											previousFieldNumber,
											row;
	
	
			// Update the field structure variables.										
	
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldType = gProjectInfoPtr->fieldIdentPtr[addField].fieldType;
	previousFieldNumber = classNamesPtr[currentStorageClass].lastFieldNumber;
	
	gProjectInfoPtr->fieldIdentPtr[addField].previousField = previousFieldNumber;
	gProjectInfoPtr->fieldIdentPtr[addField].nextField = -1;
	
	if (previousFieldNumber != -1)
		gProjectInfoPtr->fieldIdentPtr[previousFieldNumber].nextField = addField;
												
	gProjectInfoPtr->fieldIdentPtr[addField].loadedIntoClassStats = FALSE;
	
			// Update the class structure variables.										
	
	classNamesPtr[currentStorageClass].numberOfFields++;
	
	if (classNamesPtr[currentStorageClass].firstFieldNumber == -1)
		classNamesPtr[currentStorageClass].firstFieldNumber = addField;
		
	classNamesPtr[currentStorageClass].lastFieldNumber = addField;
	
	if (fieldType == kTrainingType)
		AddFieldStatsToClassStats (currentStorageClass, addField);
						
	else if (fieldType == kTestingType)
		{
		classNamesPtr[currentStorageClass].numberOfTestFields++;
		gProjectInfoPtr->numberStatTestFields++;
		
		}	// end "else if (fieldType == kTestingType)"
	
			// Update the project structure variables.									
			
	gProjectInfoPtr->numberStatisticsFields++;
									
	gProjectInfoPtr->numberTotalPoints +=
					gProjectInfoPtr->fieldIdentPtr[addField].numberOfPolygonPoints;
	
			// Indicate that project has changed.											
	
	gProjectInfoPtr->changedFlag = TRUE;
	
			// Add field name to the class field list if in the proper class.		
	
	classNumber = classNamesPtr[currentStorageClass].classNumber - 1;
	if (gProjectInfoPtr->statsWindowMode == 3 &&
											classNumber == gProjectInfoPtr->currentClass)
		{
		LSetDrawingMode (FALSE, gStatisticsListHandle);
		cell.h = 0;
		
				// If there is a currentField, then deselect it from the list.		
				
		if (gProjectInfoPtr->currentField >= 0)
			{
			cell.v = 0;
			if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
				LSetSelect (FALSE, cell, gStatisticsListHandle);
			
			}	// end "if (gProjectInfoPtr->currentField >= 0)"
			
		cell.v = classNamesPtr[currentStorageClass].numberOfFields - 1;
		row = LAddRow (1, cell.v, gStatisticsListHandle);
		
		GetFieldNameWithType (addField, gTextString);
		
		LSetCell ((char*)&gTextString[1], 
						(SInt16)gTextString[0], 
						cell, 
						gStatisticsListHandle);
			
		LSetSelect (TRUE, cell, gStatisticsListHandle);
			
				// Turn list drawing mode back on.											
		
		LSetDrawingMode (TRUE, gStatisticsListHandle);
		
				// Invalidate the list rectangle so that it will be updated.		
				                          
		#if defined multispec_mac
			InvalWindowRect (gProjectWindow, 
									GetListViewBounds (gStatisticsListHandle, &gTempRect));
		#endif	// defined multispec_mac
		
		gProjectInfoPtr->currentField = addField;
		
				// Hilite button so that user can update the statistics if the 	
				// user wants.																		
				
		if (fieldType == kTrainingType)
			HiliteControl (gProjectInfoPtr->updateControlH, 0);
		
		}	// end "if (gProjectInfoPtr->statsWindowMode == 3 && ...)"
	
			// Redo outlining the fields so that the 'cut' field will be 			
			// removed.																				
			
	ForceFieldOutlineUpdate (FALSE);
	
			// Save the pointer to the removed class so that it can be used		
			// later for 'undo cut class' or 'paste class fields'.					
			
	gProjectInfoPtr->editClassStorageNumber = -1;
	gProjectInfoPtr->editFieldNumber = -1;
	
			// Update Edit menu items.															
			
	UpdateStatWEditMenu ();
			
			// Force update of project and processor menu items.						
			
	gUpdateProjectMenuItemsFlag = TRUE;
	gUpdateProcessorMenuItemsFlag = TRUE;
	
			// Update statistics window control hiliting.								
	
	if (gProjectInfoPtr->toFieldControlH != NULL)		
		HiliteControl (gProjectInfoPtr->toFieldControlH, 0);
	
}	// end "AddField"

	
                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void AddFieldStatsToClassStats
//
//	Software purpose:	The purpose of this routine is to add the field
//							statistics to the class statistics.  The routine
//							takes into account whether the statistics have been
//							loaded into the class and whether only class 
//							statistics are kept.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			AddField in SEditStatistics.cpp
//							ChangeFieldType in SEditStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/17/1994
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void AddFieldStatsToClassStats (
					SInt16							storageClass,
					SInt16							currentField)				

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
				
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
	
			// Change project structure information.										
			
	if (classNamesPtr[storageClass].numberOfTrainFields == 0)
		gProjectInfoPtr->numberStatTrainClasses++;
		
	gProjectInfoPtr->numberStatTrainFields++;
			
			// Change class structure information.											
	
	classNamesPtr[storageClass].numberOfTrainFields++;
	classNamesPtr[storageClass].modifiedStatsFlag = FALSE;
		
			// Change field structure information.											
	
	if (fieldIdentPtr[currentField].statsUpToDate)
		fieldIdentPtr[currentField].loadedIntoClassStats = TRUE;
		
	else	// !fieldIdentPtr[currentField].statsUpToDate
		{				
		classNamesPtr[storageClass].statsUpToDate = FALSE;
		fieldIdentPtr[currentField].loadedIntoClassStats = FALSE;
		gProjectInfoPtr->statsUpToDate = FALSE;
		gProjectInfoPtr->pixelDataLoadedFlag = FALSE;
	
		if (gProjectInfoPtr->statsWindowMode <= 3 && 
													gProjectInfoPtr->updateControlH)
			HiliteControl (gProjectInfoPtr->updateControlH, 0);
			
		}	// end "else !fieldIdentPtr[currentField].statsUpToDate"
								
			// Update the count of the number of pixels in the class.
	
	classNamesPtr[storageClass].numberStatisticsPixels =  
					GetNumberOfPixelsLoadedInClass (&classNamesPtr[storageClass],
																fieldIdentPtr,
																kMeanCovariance);
				
	classNamesPtr[storageClass].numberTrainPixels =  
					GetNumberOfTrainPixelsInClass (
											&classNamesPtr[storageClass], fieldIdentPtr);
			
			// Force any leave-one-out stats to be recalculated.
			
	classNamesPtr[storageClass].looCovarianceValue = -1;
	
			// Update any statistics variables if needed.
	
	SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
		
}	// end "AddFieldStatsToClassStats"


                  
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeFieldType
//
//	Software purpose:	The purpose of this routine is to make the changes
//							required in the project, class, field, and statistics 
//							structures when the field type is changed from test
//							to train and vice-versa.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			EditClassFieldDialog
//
//	Coded By:			Larry L. Biehl			Date: 02/23/1989
//	Revised By:			Larry L. Biehl			Date: 03/19/1993	

void ChangeFieldType (
				SInt16								classStorage, 
				SInt16								currentField, 
				SInt16								fieldType)
				
{
	HPFieldIdentifiersPtr			fieldIdentPtr;
	HPClassNamesPtr					classNamesPtr;


			// Continue if input class and field make sense.							
			
	if (classStorage >= 0 && 
							classStorage < gProjectInfoPtr->numberStorageClasses &&
		currentField >= 0 && currentField < gProjectInfoPtr->numberStorageFields)
		{
				// Initialize local variables.												
				
		classNamesPtr = gProjectInfoPtr->classNamesPtr;	
		fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
				
				// Change from test field to training field.								
			
		if (fieldType == kTrainingType)
			{
					// Update project test field information.								
					
			gProjectInfoPtr->numberStatTestFields--;
			classNamesPtr[classStorage].numberOfTestFields--;
				
					// Update project training field information.						
					
			fieldIdentPtr[currentField].fieldType = kTrainingType;
			
			if (fieldIdentPtr[currentField].trainingStatsNumber == -1)
				{
				gProjectInfoPtr->numberStorageStatFields++;
				fieldIdentPtr[currentField].trainingStatsNumber =
											gProjectInfoPtr->numberStorageStatFields - 1;
											
				}	// end "if (...trainingStatsNumber == -1)"
											
			AddFieldStatsToClassStats (classStorage, currentField);
			
			}	// end "if (fieldType == kTrainingType)"
			
		if (fieldType == kTestingType)
			{
					// Change project structure information.								
					
			if (classNamesPtr[classStorage].numberOfTrainFields == 1)
				gProjectInfoPtr->numberStatTrainClasses--;
				
			gProjectInfoPtr->numberStatTrainFields--;
			gProjectInfoPtr->numberStatTestFields++;
					
					// Change class structure information.									
			
			classNamesPtr[classStorage].numberOfTrainFields--;
			classNamesPtr[classStorage].numberOfTestFields++;
			
			RemoveFieldStatsFromClassStats (classStorage, currentField, FALSE);
				
					// Change field structure information.									
					// And change class statistics information if needed.				
					
			fieldIdentPtr[currentField].fieldType = kTestingType;
			
			}	// end "if (fieldType == kTestingType)"
			
		ForceFieldOutlineUpdate (TRUE);
			
		gProjectInfoPtr->changedFlag = TRUE;
		
				// Force update of the project and processor menu items.				
		
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
			
		}	// end "if (classStorage >= 0 && ..."

}	// end "ChangeFieldType"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ChangeProjectChannelsList
//
//	Software purpose:	The purpose of this routine is to change the project
//							channel list to include all the channels that are
//							input as the number of channels.  First the routine
//							checks if the memory allocated for the project channels 
//							list is enough. If not then more memory is allocated 
//							for the channels list handle if possible.  If the amount 
//							of memory changed then the project memory move flag is 
//							set to TRUE;
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ChangeProjectBaseImage in SProject.cpp
//							InitializeNewProject in SProject.cpp
//							ReadProjectFile in SProject.cpp
//							ClearProjectStatistics in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/01/1991
//	Revised By:			Larry L. Biehl			Date: 04/01/1991	

Boolean ChangeProjectChannelsList (
				SInt16								numberChannels)

{
	SInt32								bytesNeeded;
	SInt16								index;
	
	Boolean								changedFlag,
											continueFlag;
	
	
			// Initialize local variables.													
			
		
			// Make certain that the project channels information now				
			// includes all channels in the image file.									
			// Make certain that there is enough space for the new channel			
			// list.																					
			
	numberChannels = MIN (numberChannels, kMaxNumberStatChannels);
			
	bytesNeeded = numberChannels * sizeof (short int);
	gProjectInfoPtr->channelsPtr = (UInt16*)CheckHandleSize (
						&gProjectInfoPtr->channelHandle, 
						&continueFlag, 
						&changedFlag, 
						bytesNeeded);
						
	if (!continueFlag)
																							return (FALSE);
																				
	if (changedFlag)
		gProjectInfoPtr->moveMemoryFlag = TRUE;
		
	for (index=0; index<numberChannels; index++)
		gProjectInfoPtr->channelsPtr[index] = index;
		
	gProjectInfoPtr->numberStatisticsChannels = numberChannels;
													
	if (gProjectInfoPtr->statisticsCode == kMeanStdDevOnly)
		gProjectInfoPtr->numberCovarianceEntries = numberChannels;
								
	else	// gProjectInfoPtr->statisticsCode != kMeanStdDevOnly
		gProjectInfoPtr->numberCovarianceEntries = 
										(SInt32)numberChannels * (numberChannels + 1)/2;
	
	return (TRUE);
		
}	// end "ChangeProjectChannelsList"


/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CheckEnhancedStatistics
//
//	Software purpose:	The purpose of this routine is to verify that modified
//							statistics exist for at least one class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			AddFieldStatsToClassStats in SEditStatistics.cpp
//							CutClass in SEditStatistics.cpp
//							RemoveFieldStatsFromClassStats in SEditStatistics.cpp
//							UndoCutClass in SEditStatistics.cpp 
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1994
//	Revised By:			Larry L. Biehl			Date: 01/26/1994	

void CheckEnhancedStatistics (void)

{
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								classIndex,
											classStorage,
											numberClasses;
	
	Boolean								modifiedStatsExistFlag;
	
	
			// Initialize local variables.													
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	modifiedStatsExistFlag = FALSE;
	
	for (classIndex=0; classIndex<numberClasses; classIndex++)
		{
				// Get the class storage number.												
					
		classStorage = gProjectInfoPtr->storageClass[classIndex];
		
		if (classNamesPtr[classStorage].modifiedStatsFlag)
			modifiedStatsExistFlag = TRUE;
			
		}	// end "for (classIndex=0; classIndex<..."
		
	if (modifiedStatsExistFlag)
		gProjectInfoPtr->enhancedStatsExistFlag = TRUE;
		
	else	// !modifiedStatsExistFlag
		{
		gProjectInfoPtr->covarianceStatsToUse = kOriginalStats;
		gProjectInfoPtr->enhancedStatsExistFlag = FALSE;
		
		}	// end "else !modifiedStatsExistFlag"
			
			// Update processor menu items.													
			
	gUpdateProjectMenuItemsFlag = TRUE;
		
}	// end "CheckEnhancedStatistics"
*/



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClearPixelDataMemory
//
//	Software purpose:	The purpose of this routine is to clear the memory for the
//							training pixel data.
//
//	Parameters in:
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 08/15/2019
//	Revised By:			Larry L. Biehl			Date: 08/15/2019

void ClearPixelDataMemory (
				Boolean								disposePointersFlag)

{
	if (disposePointersFlag)
		{
		gProjectInfoPtr->knnDistancesPtr =
						(knnType*)CheckAndDisposePtr ((Ptr)gProjectInfoPtr->knnDistancesPtr);
		gProjectInfoPtr->knnLabelsPtr =
										CheckAndDisposePtr (gProjectInfoPtr->knnLabelsPtr);
		gProjectInfoPtr->knnDataValuesPtr =
										CheckAndDisposePtr (gProjectInfoPtr->knnDataValuesPtr);
		
		}	// end "if (disposePointersFlag)"
	
	gProjectInfoPtr->knnCounter = 0;
	gProjectInfoPtr->pixelDataLoadedFlag = FALSE;

}	// end "ClearPixelDataMemory"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClearProjectStatistics
//
//	Software purpose:	The purpose of this routine is to clear the
//							statistics for the project.
//
//	Parameters in:		=1, called from change project base image.
//							=2, called from clear statistics menu item.
//							=3, called from changing from keeping field stats to class stats
//									or vice versa.
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ChangeProjectBaseImage in SProject.cpp
//							ProjectMenuClearStatistics in SProjectUtilities.cpp
//							StatisticsDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1989
//	Revised By:			Larry L. Biehl			Date: 08/15/2019

void ClearProjectStatistics (
				SInt16								clearCode)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classIndex,
											classStorage,
											fieldNumber,
											numberClasses,
											statCode;
											
	Boolean								continueFlag = TRUE;
	
	
			// Initialize local variables.													
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
			// Continue only if number of classes is one or more and statistics	
			// have been loaded into the project.											
	
	//if (numberClasses > 0)
	if (numberClasses > 0 && gProjectInfoPtr->statsLoaded)
		{
				// Get pointer to the project file name and list a message in the
				// output text window that the statistics for the project were clear.
		
		char* projectNamePtr = 
								(char*)GetFileNameCPointerFromProjectInfo (gProjectInfoPtr);
						
		ForceTextToEnd (); 
						
		continueFlag = ListSpecifiedStringNumber (
											kProjectStrID,
											IDS_Project44,
											NULL, 
											gOutputForce1Code, 
											projectNamePtr,
											continueFlag,
											kUTF8CharString);
		
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
					// Clear class statistics variables.
			
			if (classNamesPtr[classStorage].numberOfTrainFields > 0)
				classNamesPtr[classStorage].statsUpToDate = FALSE;
				
			if (clearCode != 3)
				{	
				classNamesPtr[classStorage].numberStatisticsPixels = 0;
				//classNamesPtr[classStorage].numberTrainPixels = 0;
				classNamesPtr[classStorage].modifiedStatsStorage = -1;
				classNamesPtr[classStorage].modifiedStatsFlag = FALSE;
				
				if (classNamesPtr[classStorage].numberOfTrainFields > 0)
					classNamesPtr[classStorage].looCovarianceValue = -1;
				
				}	// end "if (clearCode != 3)"
	
				// Continue only if the number of fields in the class is one 		
				// or more.																			
	
			if (classNamesPtr[classStorage].numberOfTrainFields >=1)
				{
				fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				while (fieldNumber != -1)
					{
					fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
							// Make certain that field is training field.				
							
					if (fieldIdentPtr->fieldType == kTrainingType)
						{
								// Clear field statistics information.						
						
						if (fieldIdentPtr->pointType == kClusterType)
							fieldIdentPtr->numberPixels = 0;
							
						fieldIdentPtr->numberPixelsUsedForStats = 0;
						fieldIdentPtr->loadedIntoClassStats = FALSE;
						fieldIdentPtr->statsUpToDate = FALSE;
								
						}	// end "if (fieldIdentPtr->field..."
				
					fieldNumber = fieldIdentPtr->nextField;
			
					}	// end "while (fieldNumber != -1)"
			
				}	// end "if (classNamesPtr[classStorage].number ..."
			
			}	// end "for (classIndex=0; ...
			
				// Now cut any cluster fields from the project since no				
				// coordinates exist to compute new statistics for the field.		
				
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
	
			if (classNamesPtr[classStorage].numberOfTrainFields >= 1)
				{
				fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				while (fieldNumber != -1)
					{
					fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
					
						// If the field is a cluster type of field, cut the field	
						// since there are no coordinates to obtain statistics for.	
						
					if (fieldIdentPtr->pointType == kClusterType)
						{
						continueFlag = ListSpecifiedStringNumber (
									kProjectStrID, 
									IDS_Project42,
									NULL, 
									gOutputForce1Code, 
									(UCharPtr)fieldIdentPtr->name, 
									continueFlag);
									
						CutField (fieldNumber, classIndex);
													
						}	// end "if (...->pointType == kClusterType)"
				
					fieldNumber = fieldIdentPtr->nextField;
			
					}	// end "while (fieldNumber != -1)"
			
				}	// end "if (classNamesPtr[classStorage].number ..."
			
			}	// end "for (classIndex=0; ...
			
				// Now cut the classes that have no fields.								
				// If there are no fields for the class then cut the class from 	
				// the project.  This class was generated from a nonsupervised 	
				// type processor such as cluster.  There is no description of 	
				// the points in the class.
				
		CutEmptyClassesFromProject ();
		/*
		classIndex = 0;
		do
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
					
			if (classNamesPtr[classStorage].numberOfFields == 0)
				{
				continueFlag = ListSpecifiedStringNumber (
							kProjectStrID, 
							IDS_Project43,
							NULL, 
							gOutputForce1Code, 
							(UCharPtr)classNamesPtr[classStorage].name, 
							continueFlag);
									
				CutClass (classIndex);
				numberClasses = gProjectInfoPtr->numberStatisticsClasses;
				
				}	// end "if (!classNamesPtr[classStorage].numberOfFields == 0)"
				
			else	// classNamesPtr[classStorage].numberOfFields > 0
				classIndex++;
																		
			}		while (classIndex < numberClasses);
		*/										
		if (gProcessorCode != kChangeBaseImageProcessor)
			UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);
			
		}	// end "if (numberClasses > 0 && gProjectInfoPtr->..."
		
			// Set project statistics information.											
			
	gProjectInfoPtr->statsUpToDate = FALSE;
	if (gProjectInfoPtr->numberStatisticsClasses == 0)
		gProjectInfoPtr->statsUpToDate = TRUE;
				
	gProjectInfoPtr->changedFlag = TRUE;
	gProjectInfoPtr->statsLoaded = FALSE;

			// Clear knn training sample values
	
	//gProjectInfoPtr->knn_distances.clear ();
	//gProjectInfoPtr->knnLabelsPtr.clear ();
	//gProjectInfoPtr->knnDataValuesPtr.clear ();
	
	ClearPixelDataMemory (kDisposePointers);
	
	if (clearCode != 3)
		{
		gProjectInfoPtr->enhancedStatsExistFlag = FALSE;
	
		if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
			gProjectInfoPtr->covarianceStatsToUse = kOriginalStats;
			
		}	// end "if (clearCode != 3)"
	
			// Hilite the 'update statistics' control if needed.						
	
	if (!gProjectInfoPtr->statsUpToDate && gProjectInfoPtr->updateControlH)
		MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, 0);
	
			// Release the memory for the statistics.										
	
	statCode = kOriginalStats+kLeaveOneOutStats+kEnhancedStats;
	if (clearCode == 3)
		statCode = kOriginalStats;
	
	ReleaseStatisticsHandles (gProjectInfoPtr, statCode);
			
			// Update processor menu items.													
			
	gUpdateProcessorMenuItemsFlag = TRUE;
	gUpdateProjectMenuItemsFlag = TRUE;
		
}	// end "ClearProjectStatistics"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClearProjectMaskStatistics
//
//	Software purpose:	The purpose of this routine is to clear any mask statistics 
//							for the project.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			ReadProjectFile in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/08/1999
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void ClearProjectMaskStatistics (void)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classIndex,
											classStorage,
											fieldNumber,
											numberClasses;
											
	Boolean								classStatsChangedFlag,
											fieldStatsChangedFlag;
	
	
			// Initialize local variables.													
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
			// Continue only if number of classes is one or more and statistics	
			// have been loaded into the project.											
	
	if (numberClasses > 0 && gProjectInfoPtr->statsLoaded)
		{	
		classStatsChangedFlag = FALSE;
									
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];			
	
			if (classNamesPtr[classStorage].numberOfTrainFields > 0)
				{
				fieldStatsChangedFlag = FALSE;
				fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				while (fieldNumber != -1)
					{
					fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
							// Make certain that field is training field.				
							
					if (fieldIdentPtr->fieldType == kTrainingType &&
														fieldIdentPtr->pointType == kMaskType)
						{
								// Clear field statistics information.
							
						fieldIdentPtr->numberPixelsUsedForStats = 0;
						fieldIdentPtr->loadedIntoClassStats = FALSE;
						fieldIdentPtr->statsUpToDate = FALSE;
						fieldStatsChangedFlag = TRUE;
								
						}	// end "if (fieldIdentPtr->field..."
				
					fieldNumber = fieldIdentPtr->nextField;
			
					}	// end "while (fieldNumber != -1)"
					
				if (fieldStatsChangedFlag)
					{
							// Clear class statistics variables.
					
					classStatsChangedFlag = TRUE;
					
					classNamesPtr[classStorage].statsUpToDate = FALSE;
					
					classNamesPtr[classStorage].modifiedStatsStorage = -1;
					classNamesPtr[classStorage].modifiedStatsFlag = FALSE;
					
					classNamesPtr[classStorage].looCovarianceValue = -1;
								
							// Update the count of the number of pixels in the class.
					
					classNamesPtr[classStorage].numberStatisticsPixels =  
							GetNumberOfPixelsLoadedInClass (&classNamesPtr[classStorage], 
																		gProjectInfoPtr->fieldIdentPtr,
																		kMeanCovariance);
								
					classNamesPtr[classStorage].numberTrainPixels =  
							GetNumberOfTrainPixelsInClass (&classNamesPtr[classStorage], 
																		gProjectInfoPtr->fieldIdentPtr);
					
					}	// end "if (fieldStatsChangedFlag)"
			
				}	// end "if (classNamesPtr[classStorage].number ..."
			
			}	// end "for (classIndex=0; ...
			
		if (classStatsChangedFlag)
			{
					// Set project statistics information.											
					
			gProjectInfoPtr->statsUpToDate = FALSE;
			gProjectInfoPtr->changedFlag = TRUE;
			gProjectInfoPtr->pixelDataLoadedFlag = FALSE;
			
					// Hilite the 'update statistics' control if needed.						
			
			if (!gProjectInfoPtr->statsUpToDate && gProjectInfoPtr->updateControlH)
				MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, 0);
				
					// Get pointer to the project file name and list a message in the
					// output text window that the statistics for the project were clear.
			
			char* projectNamePtr = 
								(char*)GetFileNameCPointerFromProjectInfo (gProjectInfoPtr);
							
			ForceTextToEnd (); 
							
			ListSpecifiedStringNumber (kProjectStrID,
												IDS_Project62,
												NULL, 
												gOutputForce1Code, 
												projectNamePtr,
												TRUE,
												kUTF8CharString);
			
			}	// end "if (classStatsChangedFlag)"
			
		}	// end "if (numberClasses > 0 && gProjectInfoPtr->statsLoaded)"
		
}	// end "ClearProjectMaskStatistics"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CutClass
//
//	Software purpose:	The purpose of this routine is to cut the
//							selected class from the current project statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  2/15/1989
//	Revised By:			Larry L. Biehl			Date: 12/28/1995			
// TODO: For Linux

void CutClass (
				SInt16								removeClass)

{
	#if defined multispec_win 	
		CComboBox*							classListComboBoxPtr;
	#endif	// defined multispec_win 
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								classStorage,
											field,
											tempStorageClass;
										
	UInt16								classIndex;
	
	
	if (removeClass >= 0 &&
						removeClass <= (SInt16)gProjectInfoPtr->numberStatisticsClasses)
		{
				// Initialize local variables.												
						
		classStorage = gProjectInfoPtr->storageClass[removeClass];
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		
				// Update the project structure variables.								
				
		gProjectInfoPtr->numberStatisticsClasses--;
		if (classNamesPtr[classStorage].numberOfTrainFields > 0)
			gProjectInfoPtr->numberStatTrainClasses--;
		gProjectInfoPtr->numberStatisticsFields -=
												classNamesPtr[classStorage].numberOfFields;
		gProjectInfoPtr->numberStatTrainFields -=
										classNamesPtr[classStorage].numberOfTrainFields;
		gProjectInfoPtr->numberStatTestFields -=
										classNamesPtr[classStorage].numberOfTestFields;
										
				// Update number of coordinate points to define fields.				
										
		field = classNamesPtr[classStorage].firstFieldNumber;
		while (field > -1)
			{
			gProjectInfoPtr->numberTotalPoints -=
						gProjectInfoPtr->fieldIdentPtr[field].numberOfPolygonPoints;
			
					// Indicate that these are now "inactive" fields.					
					
			gProjectInfoPtr->fieldIdentPtr[field].fieldType = 
										-gProjectInfoPtr->fieldIdentPtr[field].fieldType;
				
			field = gProjectInfoPtr->fieldIdentPtr[field].nextField;
					
			}	// end "while (field > -1)"
	
				// Update any statistics variables if needed.			
						
//		CheckEnhancedStatistics ();
			
		SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
			
		gProjectInfoPtr->currentClass = -1;
		
				// Indicate that project has changed.										
		
		gProjectInfoPtr->changedFlag = TRUE;
		
				// Remove this class from the list of project classes and change	
				// the class numbers for all classes above this class.				
				
		for (classIndex=removeClass;
				classIndex<gProjectInfoPtr->numberStatisticsClasses; 
				classIndex++)
			{
			tempStorageClass = gProjectInfoPtr->storageClass[classIndex+1];
			gProjectInfoPtr->storageClass[classIndex] = tempStorageClass;
			classNamesPtr[tempStorageClass].classNumber = classIndex+1;
			
			}	// end "for (classIndex=removeClass; ..."
	
		#if defined multispec_mac 		
					// Remove the class name from the class pop up menu.					

			if (CountMenuItems (gPopUpClassMenu) > 1)
				::DeleteMenuItem (gPopUpClassMenu, removeClass+2);
				
					// Make certain that the 'New Class' item in the popup menu			
					// is hilited.																		
					
	 		::EnableMenuItem (gPopUpClassMenu, 1);
		#endif	// defined multispec_mac 
	
		#if defined multispec_win 		
					// Remove the class name from the list.	
					
			classListComboBoxPtr = 
										(CComboBox*)gProjectWindow->GetDlgItem (IDC_ClassList);				

			if (classListComboBoxPtr->GetCount () > 1)
				classListComboBoxPtr->DeleteString (removeClass+1);
				
					// Make certain that the 'New Class' item in the popup menu			
					// is hilited.		
													
			classListComboBoxPtr->SetCurSel (0);   
		#endif	// defined multispec_win 
	
				// Update the statistics window if it exists.							
				
		if (gProjectWindow)
			{
					// Remove class name from class list.									
					
			if (gProjectInfoPtr->statsWindowMode == 2)
				{
				LDelRow (1, removeClass, gStatisticsListHandle);
				
						// Invalidate the list rectangle so that it will be updated.
				
				#if defined multispec_mac 		
					InvalWindowRect (
									gProjectWindow,
									GetListViewBounds (gStatisticsListHandle, &gTempRect));
				#endif	// defined multispec_mac 
				
				}	// end "if (gProjectInfoPtr->statsWindowMode == 2)"
			
					// Redo outlining the fields so that the fields for the 'cut'	
					// class will be removed.													
					
			ForceFieldOutlineUpdate (TRUE);
			
					// Save the pointer to the removed class so that it can be used
					// later for 'undo cut class' or 'paste class fields'.			
					
			gProjectInfoPtr->editClassStorageNumber = classStorage;
			gProjectInfoPtr->editFieldNumber = -1;
			
					// Update Edit menu items.													
					
			UpdateStatWEditMenu ();
			
					// Force update of project and processor menu items.				
					
			gUpdateProjectMenuItemsFlag = TRUE;
			gUpdateProcessorMenuItemsFlag = TRUE;
			
					// Update statistics window control hiliting.						
			
			if (gProjectInfoPtr->toClassControlH)		
				MHiliteControl (
					gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
			
			}	// end "if (gProjectWindow)"
		
		}	// end "if (removeClass >= 0 && ..."
			
}	// end "CutClass"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CutEmptyClassesFromProject
//
//	Software purpose:	The purpose of this routine is to cut empty classes from the 
//							project. Empty classes occur when cluster or mask fields are
//							cut from the project and statistics cannot be calculated.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/03/2017
//	Revised By:			Larry L. Biehl			Date: 05/03/2017			

void CutEmptyClassesFromProject ()

{		
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								classIndex,
											classStorage,
											numberClasses;
											
	Boolean								continueFlag = TRUE;
	
	
			// Initialize local variables.													
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
		
	if (numberClasses > 0)
		{
		classIndex = 0;
		do
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
					
			if (classNamesPtr[classStorage].numberOfFields == 0)
				{
				continueFlag = ListSpecifiedStringNumber (
							kProjectStrID, 
							IDS_Project43,
							NULL, 
							gOutputForce1Code, 
							(UCharPtr)classNamesPtr[classStorage].name, 
							continueFlag);
									
				CutClass (classIndex);
				numberClasses = gProjectInfoPtr->numberStatisticsClasses;
				
				}	// end "if (!classNamesPtr[classStorage].numberOfFields == 0)"
				
			else	// classNamesPtr[classStorage].numberOfFields > 0
				classIndex++;
																		
			}		while (classIndex < numberClasses);
			
		}	// end "if (numberClasses > 0)"
			
}	// end "CutEmptyClassesFromProject"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CutField
//
//	Software purpose:	The purpose of this routine is to cut the selected
// 						field from the current class statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:  2/21/1989
//	Revised By:			Larry L. Biehl			Date: 12/08/1993			

void CutField (
				SInt16								removeField, 
				SInt16								currentClass)

{                                  
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								currentStorageClass,
											fieldType,
											nextFieldNumber,
											previousFieldNumber;
	
	
	if (removeField >= 0 &&
								removeField < gProjectInfoPtr->numberStorageFields)
		{
				// Initialize local variables.												
				
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		fieldType = gProjectInfoPtr->fieldIdentPtr[removeField].fieldType;
		
				// Update the field structure variables.									
		
		previousFieldNumber = 
							gProjectInfoPtr->fieldIdentPtr[removeField].previousField;
		nextFieldNumber = gProjectInfoPtr->fieldIdentPtr[removeField].nextField;
		if (previousFieldNumber != -1)	
			gProjectInfoPtr->fieldIdentPtr[previousFieldNumber].nextField = 
																				nextFieldNumber;
					
		if (nextFieldNumber != -1)	
			gProjectInfoPtr->fieldIdentPtr[nextFieldNumber].previousField = 
																			previousFieldNumber;
		
				// Update the class structure variables.									
				// First get the class storage number.										
						
		currentStorageClass = gProjectInfoPtr->storageClass[currentClass];
		
		classNamesPtr[currentStorageClass].numberOfFields--;
		
		if (fieldType == kTrainingType)
			classNamesPtr[currentStorageClass].numberOfTrainFields--;
		else	// fieldType == kTestingType 
			classNamesPtr[currentStorageClass].numberOfTestFields--;
		
		if (nextFieldNumber == -1)
			classNamesPtr[currentStorageClass].lastFieldNumber = previousFieldNumber;
																				
		if (previousFieldNumber == -1)
			classNamesPtr[currentStorageClass].firstFieldNumber = nextFieldNumber;
		
				// Update the project structure variables.								
				
		if (fieldType == kTrainingType &&
						classNamesPtr[currentStorageClass].numberOfTrainFields == 0)
			gProjectInfoPtr->numberStatTrainClasses--;
		gProjectInfoPtr->numberStatisticsFields--;
		
		if (fieldType == kTrainingType)
			gProjectInfoPtr->numberStatTrainFields--;
		else	// fieldType == kTestingType
			gProjectInfoPtr->numberStatTestFields--;
										
		gProjectInfoPtr->numberTotalPoints -=
					gProjectInfoPtr->fieldIdentPtr[removeField].numberOfPolygonPoints;
			
		gProjectInfoPtr->currentField = -1;
		
				// Remove field statistics from class sums and sum of squares if 	
				// needed.																			
				
		RemoveFieldStatsFromClassStats (currentStorageClass, removeField, FALSE);
		
				// Indicate that project has changed.										
		
		gProjectInfoPtr->changedFlag = TRUE;
		
				// Remove field name from the field list.									
				
		if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
			{ 
			Cell cell; 
			
			cell.h = 0;
			cell.v = 0;
				
			if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
				{
				LDelRow (1, cell.v, gStatisticsListHandle);
					
							// Invalidate the list rectangle so that it will be updated.	
							 
				#if defined multispec_mac 
					InvalWindowRect (
										gProjectWindow, 
										GetListViewBounds (gStatisticsListHandle, &gTempRect));
				#endif	// defined multispec_mac 
					
				}	// end "if (LGetSelect (TRUE, &cell, gStatisticsListHandle))"
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"
		
				// Redo outlining the fields so that the 'cut' field will be 		
				// removed.																			
				
		ForceFieldOutlineUpdate (TRUE);
		
				// Save the pointer to the removed field so that it can be used	
				// later for 'undo cut field'.												
				
		gProjectInfoPtr->editClassStorageNumber = -1;
		gProjectInfoPtr->editFieldNumber = removeField;
		
				// Indicate that this is not an "active" field anymore.				
				
		gProjectInfoPtr->fieldIdentPtr[removeField].fieldType = -fieldType;
		
				// Update Edit menu items.														
				
		UpdateStatWEditMenu ();
		
				// Force update of the project and processor menu items.				
				
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
				// Update statistics window control hiliting.							
				
		if (gProjectInfoPtr->toFieldControlH)
			MHiliteControl (
					gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
		
		}	// end "if (removeField >= 0 && ..."
			
}	// end "CutField"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void CutPolygonPoint
//
//	Software purpose:			The purpose of this routine is to cut the selected
//									polygon point from the current class and field.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/23/1993
//	Revised By:			Larry L. Biehl			Date: 04/08/1998			

void CutPolygonPoint (
				SInt16								removePoint, 
				SInt16								currentField, 
				SInt16								currentClass)

{
	Cell									cell;
	ProjectFieldPoints				savedFieldPoint;
	
	HPFieldPointsPtr					fieldPointsPtr;
	
	SInt16								currentStorageClass,
											nextFieldNumber;
	
	
	if ((removePoint >= 0) && 
			(gProjectInfoPtr->fieldIdentPtr[currentField].pointType == 
																				kPolygonType) &&
			(gProjectInfoPtr->fieldIdentPtr[currentField].numberOfPolygonPoints > 3))
		{
				// Initialize local variables.												
				
		currentStorageClass = gProjectInfoPtr->storageClass[currentClass];
		
				// Update the field structure variables.									
				
		gProjectInfoPtr->fieldIdentPtr[currentField].numberOfPolygonPoints--;
		
				// Update the project structure variables.								
										
		gProjectInfoPtr->numberTotalPoints--;
		
				// Remove field statistics from class sums and sum of squares if 	
				// needed.																			
				
		RemoveFieldStatsFromClassStats (currentStorageClass, currentField, TRUE);
		
				// Indicate that project has changed.										
		
		gProjectInfoPtr->changedFlag = TRUE;
		
			// Remove point from the polygon point list for the field.				
			// 'nextFieldNumber' is used here as a temporary variable.
			// Note that the first two points in the list represent the bounding 
			// rectangle around the polygon.				
		
		fieldPointsPtr = gProjectInfoPtr->fieldPointsPtr;
		fieldPointsPtr = &fieldPointsPtr[
				gProjectInfoPtr->fieldIdentPtr[currentField].firstPolygonPoint + 2];
		fieldPointsPtr = &fieldPointsPtr[removePoint];
				
		savedFieldPoint = *fieldPointsPtr;
		
		nextFieldNumber = 
			gProjectInfoPtr->fieldIdentPtr[currentField].numberOfPolygonPoints -
																						removePoint;
		while (nextFieldNumber > 0)
			{
			*fieldPointsPtr = fieldPointsPtr[1];
			fieldPointsPtr++;
			nextFieldNumber--;
			 
			}	// end "while (nextFieldNumber > 0)"
			
		*fieldPointsPtr = savedFieldPoint;
		
			// Remove point from the polygon point list in the statistics			
			// window.																				
			
		cell.h = 0;
		cell.v = removePoint;
		LDelRow (1, cell.v, gStatisticsListHandle);
		
				// Invalidate the list rectangle so that it will be 					
				// updated.																			
				     
		#if defined multispec_mac 
			InvalWindowRect (gProjectWindow, 
									GetListViewBounds (gStatisticsListHandle, &gTempRect));
		#endif	// defined multispec_mac 
		
				// Get the new label point.
				
		GetPolygonLabelPoint (
								&gProjectInfoPtr->fieldIdentPtr[currentField],
								gProjectInfoPtr->fieldPointsPtr,
								&gProjectInfoPtr->fieldIdentPtr[currentField].labelPoint);
		
				// Redo outlining the fields so that the 'cut' field will be 		
				// removed.																			
				
		ForceFieldOutlineUpdate (TRUE);
		
				// Save the pointer to the removed point so that it can be used	
				// later for 'undo cut polygon point'.										
				
		gProjectInfoPtr->editPointNumber = removePoint;
		
				// Update Edit menu items.														
				
		UpdateStatWEditMenu ();
		
				// Force update of the project menu items.								
				
		gUpdateProjectMenuItemsFlag = TRUE;
		
		}	// end "if (removePoint >= 0 && ...)"
			
}	// end "CutPolygonPoint"

	
/*
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CutMaskClasses
//
//	Software purpose:	The purpose of this routine is to remove classes from the
//							project that now contain no training or test fields. This happens
//							if the training and or test mask is changed.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			LoadNewMaskFields in SMask.cpp
//							StatisticsDialogMaskCheck in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1998
//	Revised By:			Larry L. Biehl			Date: 03/16/2017

void CutMaskClasses (
				SInt16								maskSetCode)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classIndex,
											classStorage,
											fieldNumber,
											fieldType,
											numberClasses,
											stringID;
											
	Boolean								continueFlag = TRUE,
											fieldsCutFlag = FALSE,
											statsLoadedFlag = FALSE;
	
	
			// Initialize local variables.													
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
			// Continue only if number of classes is one or more.											
	
	if (numberClasses > 0)
		{				
		ForceTextToEnd (); 
									
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
					// Clear class statistics variables.
				
			if (maskSetCode == kTrainingMaskSet)
				fieldType = kTrainingType;
				
			else if (maskSetCode == kTestMaskSet)
				fieldType = kTestingType;
			
			if (classNamesPtr[classStorage].numberOfFields > 0)
				{
				fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				while (fieldNumber != -1)
					{
					fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
					
							// Make certain that field is requested training or test field
							// type.				
							
					if (fieldIdentPtr->pointType == kMaskType &&
										fieldIdentPtr->fieldType == fieldType)
						{
						CutField (fieldNumber, classIndex);
						fieldsCutFlag = TRUE;
								
						}	// end "if (fieldIdentPtr->field..."
				
					fieldNumber = fieldIdentPtr->nextField;
			
					}	// end "while (fieldNumber != -1)"
				
						// Check if any statistics are still loaded for the class.
						
				if (classNamesPtr->numberStatisticsPixels > 0)
					 statsLoadedFlag = TRUE;
			
				}	// end "if (classNamesPtr[classStorage].number ..."
			
			}	// end "for (classIndex=0; ...
			
		}	// end "if (numberClasses > 0)"
		
	if (fieldsCutFlag)
		{
				// Get pointer to the project file name and list a message in the
				// output text window that the mask fields for the project were cleared.
		
		char* projectNamePtr = (char*)GetFileNameCPointer (gProjectInfoPtr);
			
		stringID = IDS_Project60;
		if (maskSetCode == kTestMaskSet)
			stringID = IDS_Project61;
		
		continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																	stringID,
																	NULL, 
																	gOutputForce1Code, 
																	projectNamePtr,
																	continueFlag,
																	kUTF8CharString);
		
				// Set project statistics information.											
		
		if (fieldType == kTrainingType)
			{		
			gProjectInfoPtr->statsUpToDate = FALSE;
			if (gProjectInfoPtr->numberStatisticsClasses == 0)
				gProjectInfoPtr->statsUpToDate = TRUE;
						
			gProjectInfoPtr->statsLoaded = statsLoadedFlag;
	
					// Make any changes needed in the stats to use parameters.
					
			SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
			
			if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
				gProjectInfoPtr->covarianceStatsToUse = kOriginalStats;
				
			gProjectInfoPtr->changedFlag = TRUE;
				
			}	// end "if (fieldType == kTrainingType)"
					
		}	// end "if (fieldsCutFlag)"
	
			// Hilite the 'update statistics' control if needed.						
	
	if (!gProjectInfoPtr->statsUpToDate && gProjectInfoPtr->updateControlH)
		MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, 0);
			
			// Update processor menu items.													
			
	gUpdateProcessorMenuItemsFlag = TRUE;
	gUpdateProjectMenuItemsFlag = TRUE;
		
}	// end "CutMaskClasses"
*/
	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CutMaskFields
//
//	Software purpose:	The purpose of this routine is to remove the specified mask
//							fields from the project.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:			LoadNewMaskFields in SMask.cpp
//							StatisticsDialogMaskCheck in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1998
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void CutMaskFields (
				SInt16								maskSetCode)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classIndex,
											classStorage,
											fieldNumber,
											fieldType,
											numberClasses,
											stringID;
											
	Boolean								continueFlag = TRUE,
											fieldsCutFlag = FALSE,
											statsLoadedFlag = FALSE;
	
	
			// Initialize local variables.													
			
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
			// Continue only if number of classes is one or more.											
	
	if (numberClasses > 0)
		{				
		ForceTextToEnd (); 
									
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.											
						
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
					// Clear class statistics variables.
				
			if (maskSetCode == kTrainingMaskSet)
				fieldType = kTrainingType;
				
			else if (maskSetCode == kTestMaskSet)
				fieldType = kTestingType;
			
			if (classNamesPtr[classStorage].numberOfFields > 0)
				{
				fieldNumber = classNamesPtr[classStorage].firstFieldNumber;
		
				while (fieldNumber != -1)
					{
					fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
					
							// Make certain that field is requested training or test field
							// type.				
							
					if (fieldIdentPtr->pointType == kMaskType &&
										fieldIdentPtr->fieldType == fieldType)
						{
						CutField (fieldNumber, classIndex);
						fieldsCutFlag = TRUE;
								
						}	// end "if (fieldIdentPtr->field..."
				
					fieldNumber = fieldIdentPtr->nextField;
			
					}	// end "while (fieldNumber != -1)"
				
						// Check if any statistics are still loaded for the class.
						
				if (classNamesPtr->numberStatisticsPixels > 0)
					 statsLoadedFlag = TRUE;
			
				}	// end "if (classNamesPtr[classStorage].number ..."
			
			}	// end "for (classIndex=0; ...
			
				// Now make sure any empty classes are cut from the project
				
		CutEmptyClassesFromProject ();
			
		}	// end "if (numberClasses > 0)"
		
	if (fieldsCutFlag)
		{
				// Get pointer to the project file name and list a message in the
				// output text window that the mask fields for the project were cleared.
		
		char* projectNamePtr = 
								(char*)GetFileNameCPointerFromProjectInfo (gProjectInfoPtr);
			
		stringID = IDS_Project60;
		if (maskSetCode == kTestMaskSet)
			stringID = IDS_Project61;
		
		continueFlag = ListSpecifiedStringNumber (kProjectStrID,
																stringID,
																NULL, 
																gOutputForce1Code, 
																projectNamePtr,
																continueFlag,
																kUTF8CharString);
		
				// Set project statistics information.											
		
		if (fieldType == kTrainingType)
			{		
			gProjectInfoPtr->statsUpToDate = FALSE;
			if (gProjectInfoPtr->numberStatisticsClasses == 0)
				gProjectInfoPtr->statsUpToDate = TRUE;
						
			gProjectInfoPtr->statsLoaded = statsLoadedFlag;
	
					// Make any changes needed in the stats to use parameters.
					
			SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
			
			if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
				gProjectInfoPtr->covarianceStatsToUse = kOriginalStats;
				
			gProjectInfoPtr->changedFlag = TRUE;
				
			}	// end "if (fieldType == kTrainingType)"
					
		}	// end "if (fieldsCutFlag)"
	
			// Hilite the 'update statistics' control if needed.						
	
	if (!gProjectInfoPtr->statsUpToDate && gProjectInfoPtr->updateControlH)
		MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, 0);
			
			// Update processor menu items.													
			
	gUpdateProcessorMenuItemsFlag = TRUE;
	gUpdateProjectMenuItemsFlag = TRUE;
		
}	// end "CutMaskFields"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean DetermineIfStatisticsFromClustering
//
//	Software purpose:	The purpose of this routine is to determine if any of the
//							statistics for the project are from the cluster operation.
//							If so, then one does not know the actual pixels from which the
//							statistics were computed.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/04/2019
//	Revised By:			Larry L. Biehl			Date: 06/04/2019

Boolean DetermineIfStatisticsFromClustering ()

{
	SInt64								numberOfTrainPixelsInProject = 0;
	
	UInt32								classIndex,
											classStorage,
											numberClasses;
	
	
			// Initialize local variables.
	
	numberClasses = gProjectInfoPtr->numberStatisticsClasses;
	
			// Continue only if number of classes is one or more and if project
			//	statistics are not up-to-date
	
	if (numberClasses > 0)
		{
		for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
					// Get the class storage number.
			
			classStorage = gProjectInfoPtr->storageClass[classIndex];
			
			numberOfTrainPixelsInProject += GetNumberOfTrainPixelsInClass (
												&gProjectInfoPtr->classNamesPtr[classStorage],
												gProjectInfoPtr->fieldIdentPtr);
			
			}	// end "for (classIndex=0; ...
		
		}	// end "if (numberClasses > 0)"
	
	return (numberOfTrainPixelsInProject);
	
}	// end "DetermineIfStatisticsFromClustering"


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoStatisticsWCut
//
//	Software purpose:	The purpose of this routine is to handle the Edit
//							menu item cut for the statistics window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/16/1996
//	Revised By:			Larry L. Biehl			Date: 02/16/1996			

void DoStatisticsWCut (void)

{
	Boolean								tempFlag = FALSE;
	
	
	if (gProjectInfoPtr->statsWindowMode == kClassListMode)
		{
		CutClass (gProjectInfoPtr->currentClass);
		tempFlag = (gProjectInfoPtr->currentClass < 0);
			
		}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"
									
	if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
		{
		CutField (gProjectInfoPtr->currentField,
														gProjectInfoPtr->currentClass);
		tempFlag = (gProjectInfoPtr->currentField < 0);
		
		}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)"
									
	if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
		{
		Cell				cell;
		
		cell.h = 0;	
		cell.v = 0;
		if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
			CutPolygonPoint (cell.v,
									gProjectInfoPtr->currentField,
									gProjectInfoPtr->currentClass);
		
		}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)"
		
	if (tempFlag)
		MHiliteControl (
					gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

}	// end "DoStatisticsWCut"


	
#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoStatisticsWEdit
//
//	Software purpose:	The purpose of this routine is to cut or clear the
//							selected class from the current project statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  2/16/1989
//	Revised By:			Larry L. Biehl			Date: 10/06/1998			

void DoStatisticsWEdit (
				SInt16								menuItem)

{
	switch (menuItem)
		{
		case kUndo:
			DoStatisticsWUndo ();
			break;
	
		case kCut:
			DoStatisticsWCut ();
			break;
	
		case kPaste:
			DoStatisticsWPaste ();
			break;
					
		case kSelectAll:
			DoEditSelectAllImage (gProjectSelectionWindow);
			break;
					
   	case kEditSelectionRect:
			EditSelectionDialog (kRectangleType, TRUE);
   		break;
		
		}	// end "switch (menuItem)"
		
			// Force update of processor menu items.										
	
	gUpdateProcessorMenuItemsFlag = TRUE;

}	// end "DoStatisticsWEdit"
#endif	// defined multispec_mac


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoStatisticsWPaste
//
//	Software purpose:	The purpose of this routine is to handle the Edit
//							menu item paste for the statistics window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/16/1996
//	Revised By:			Larry L. Biehl			Date: 12/16/2016			

void DoStatisticsWPaste (void)

{
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->statsWindowMode == 1 &&
												gProjectInfoPtr->editFieldNumber >= 0)
			PasteFieldToNewClass ();
	
		if (gProjectInfoPtr->statsWindowMode >= 2)
			{	
			if (gProjectInfoPtr->editFieldNumber >= 0)
				PasteField ();
			
			if (gProjectInfoPtr->editClassStorageNumber >= 0)
				PasteClassFields ();
				
			}	// end "if (gProjectInfoPtr->statsWindowMode >= 2)"
		
		}	// end "if (gProjectInfoPtr != NULL)"

}	// end "DoStatisticsWPaste"


	
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoStatisticsWUndo
//
//	Software purpose:	The purpose of this routine is to handle the Edit
//							menu item undo for the statistics window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/16/1996
//	Revised By:			Larry L. Biehl			Date: 12/16/2016			

void DoStatisticsWUndo (void)

{
	if (gProjectInfoPtr != NULL)
		{
		if (gProjectInfoPtr->editClassStorageNumber >= 0)
			UndoCutClass ();
		
		else if (gProjectInfoPtr->editFieldNumber >= 0)
			UndoCutField ();
			
		}	// end "if (gProjectInfoPtr != NULL)"

}	// end "DoStatisticsWUndo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean EditClassFieldDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							to allow the user to edit the class or field name.
//							If the field name is to be edited, then the user
//							can also change the field from test field to train
//							field or vice versa. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsWControlEvent	in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/13/1989
//	Revised By:			Larry L. Biehl			Date: 05/02/2019

Boolean EditClassFieldDialog (
				SInt16								classFieldFlag, 
				SInt16								currentClass, 
				SInt16								currentField)

{  
	Str255								titleString;                        
	Cell									cell; 
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	SInt16								classStorage,
											oldFieldType; 
											
	Boolean								OKFlag = FALSE; 
	
	                                         
	fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[currentField];
	
	if (classFieldFlag == 2)
		CtoPstring ((UCharPtr)"Edit Class Name", titleString); 
			   
	else if (classFieldFlag == 3)
		{ 	
		CtoPstring ((UCharPtr)"Edit Field Description", titleString); 
		
		oldFieldType = fieldIdentPtr->fieldType;
		
		}	// end "else if (classFieldFlag == 3)"
	
			// Get the class storage number.													
						
	classStorage = gProjectInfoPtr->storageClass[currentClass];
	
#if defined multispec_mac
	Rect									theBox;
	
	DialogPtr							dialogPtr;  

	Handle								okHandle,
											theHandle;
	
	SInt16								fieldType,
											itemHit,
											returnCode,
											theType;
											
	UInt16								selectedItem;
	
	Boolean								modalDone;
	

			// Get the modal dialog for the channel description specification.	
				
	dialogPtr = LoadRequestedDialog (kNewClassFieldDialogID, kCopyScrap, 1, 2);
	if (dialogPtr == NULL)													
																							return (FALSE);
	
			// Initialize the dialog box parameters that are common between the
			// Mac and Windows versions.
																					
	EditClassFieldDialogInitialize (dialogPtr, 
												classFieldFlag,
												classStorage,
												currentField,
												&selectedItem);
	
			// Save handle for the OK button for use later.								
			
	GetDialogItem (dialogPtr, 1,  &theType, &okHandle, &theBox);
	
			// Change the window title	 														
		               
	SetWTitle (GetDialogWindow (dialogPtr), titleString);

			// Set the selected item.
			
	SelectDialogItemText (dialogPtr, selectedItem, 0, INT16_MAX);
		
			// Center the dialog and then show it.											
			
	ShowDialogWindow (dialogPtr, kNewClassFieldDialogID, kSetUpDFilterTable);
	
	gDialogItemDescriptorPtr[4] = kDItemString + kDItemSType2;
	gDialogItemDescriptorPtr[6] = kDItemString + kDItemSType2;
	
	modalDone = FALSE;
	itemHit = 0;
	do 
		{
	   ModalDialog (gProcessorDialogFilterPtr, &itemHit);
		if (itemHit > 2)
			{
					// If itemHit was a radio button make appropriate control		
					// settings to indicate to the user the present selection.		
					// Get the handle to the itemHit.										
					
			GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
			
			switch (itemHit)
				{	
				case 4:				// Class Name 
					GetDialogItemText (theHandle, gTextString);
					if (gTextString[0] > 31)
						{
						gTextString[0] = (char)31;
						SetDialogItemText (theHandle, (ConstStr255Param)gTextString);
						SelectDialogItemText (dialogPtr, 4, 0, INT16_MAX);
						SysBeep (2);
						}
					break;
						
				case 6:				// Field Identifier 
					GetDialogItemText (theHandle, gTextString);
					if (gTextString[0] > 31)
						{
						gTextString[0] = (char)31;
						SetDialogItemText (theHandle, (ConstStr255Param)gTextString);
						SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);
						SysBeep (2);
						}
					break;
					
				case 7:				// Radio button for training field 
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 8, 0);
					break;
					
				case 8:				// Radio button for test field
					SetControlValue ((ControlHandle)theHandle, 1);
					SetDLogControl (dialogPtr, 7, 0);
					break;
						
				}	// end "switch (itemHit)"
				
			}	// end "if (itemHit > 2)"
		else
			if	(itemHit == 1)      // User selected OK for information 
				{
				modalDone = TRUE;
				OKFlag = TRUE;
				
				if (classFieldFlag == 2)
					{
							// Item 4; Class Name												
	
					GetDialogItem (dialogPtr, 4,  &theType, &theHandle, &theBox);
					GetDialogItemText (theHandle, gTextString);
					
							// Check if proposed class name is a duplicate.	 If it  	
							// is, allow user to change it again.							
							
					returnCode = CheckForDuplicateClassName (
																classStorage, (Str255*)gTextString);
					if (returnCode != 0)
						{
						HiliteControl ((ControlHandle)okHandle, 255);
						 
						if (returnCode < 0)
							DisplayAlert (
										kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
						else	// returnCode > 0
							DupClassFieldNameAlert (1, gTextString);
							
						HiliteControl ((ControlHandle)okHandle, 0);
						SelectDialogItemText (dialogPtr, 4, 0, INT16_MAX);
						modalDone = FALSE;
						OKFlag = FALSE;
						
						}	// end "if (returnCode != 0)"
						
					else	// returnCode == 0
						CopyPToP (gProjectInfoPtr->classNamesPtr[classStorage].name,
										gTextString);
					
							// If name is not duplicate, change name in class list	
							// and menu list.														
					
					if (modalDone)
						{ 	
						SetMenuItemText (gPopUpClassMenu, 
									currentClass+2, 
									gProjectInfoPtr->classNamesPtr[classStorage].name);    
														
						}	// end "if (modalDone)"
					
					}	// end "if (classFieldFlag == 2)"
					
				if (classFieldFlag == 3)
					{
							// Item 6; Field Identifier										
	
					GetDialogItem (dialogPtr, 6,  &theType, &theHandle, &theBox);
					GetDialogItemText (theHandle, gTextString);
						
							// Check if proposed field name is a duplicate.	 If it  	
							// is, allow user to change it again.							
						                                   
					returnCode = CheckForDuplicateFieldName (
																currentField, (Str255*)gTextString);
					if (returnCode > 0)
						{
						HiliteControl ((ControlHandle)okHandle, 255);
						 
						if (returnCode == 1)
							DisplayAlert (
										kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
						else	// returnCode == 2
							DupClassFieldNameAlert (2, gTextString);
							                                       
						HiliteControl ((ControlHandle)okHandle, 0);
						SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);
						modalDone = FALSE;
						OKFlag = FALSE;
						
						}	// end "if (returnCode > 0)"
						
					else	// returnCode == 0
						CopyPToP (fieldIdentPtr->name, (UCharPtr)gTextString);
						
							// Items 7,8; Training field or test field					
					
					if (modalDone)
						{
						if (GetDLogControl (dialogPtr, 7) == 1)
							fieldType = kTrainingType;
							
						if (GetDLogControl (dialogPtr, 8) == 1)
							fieldType = kTestingType;
						
						fieldIdentPtr->fieldType = fieldType;	
																		
						}	// end "if (modalDone)"
																		
					}	// end "if (classFieldFlag == 3)"
												
				}	// end "if (itemHit == 1)"
				
			if	(itemHit == 2)      // User selected Cancel for information 
				{
				modalDone = TRUE;
				OKFlag = FALSE;
				
				}	// end "if	(itemHit == 2)"
				
		} while (!modalDone);
		
	CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac  

	#if defined multispec_win
		CMEditClassFieldDlg*		dialogPtr = NULL; 
		
		TRY
			{ 
			dialogPtr = new CMEditClassFieldDlg (); 
			                                                        
			OKFlag = dialogPtr->DoDialog (classFieldFlag, 
														currentClass, 
														currentField,
														(char*)&titleString[1]); 
		                      
			delete dialogPtr;
			}
			
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif	// defined multispec_win

	#if defined multispec_wx
		CMEditClassFieldDlg*		dialogPtr = NULL; 
		
		try
			{
			dialogPtr = new CMEditClassFieldDlg (); 			                                                        
			OKFlag = dialogPtr->DoDialog (classFieldFlag, 
														currentClass, 
														currentField,
														(char*)&titleString[1]); 
		                      
			delete dialogPtr;
			}
	
		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif	// defined multispec_wx

	if (OKFlag)
		{
		if (classFieldFlag == 2)
			{     
			cell.h = 0;
			cell.v = currentClass;
			LSetCell ((char*)&gProjectInfoPtr->classNamesPtr[classStorage].name[1], 
							(SInt16)gProjectInfoPtr->classNamesPtr[classStorage].name[0], 
							cell,
							gStatisticsListHandle); 
						
					// Force class/field name to be redrawn.
								
			if (gProjectInfoPtr->outlineFieldType > 0) 		
				ForceFieldOutlineUpdate (TRUE);
			
			}	// end "if (classFieldFlag == 2)"
			     	
		if (classFieldFlag == 3)
			{  														
			if (fieldIdentPtr->fieldType != oldFieldType)
				{
				ChangeFieldType (classStorage, 
										currentField, 
										fieldIdentPtr->fieldType);
								
				if (fieldIdentPtr->fieldType == kTrainingType
										&& !fieldIdentPtr->statsUpToDate)
					MHiliteControl (
									gProjectWindow, gProjectInfoPtr->updateControlH, 0);
							
				}	// end "if (...fieldIdentPtr->...."
							
					// Change name in field list.									
						
			cell.h = 0;	
			cell.v = 0;
			if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
				{
				GetFieldNameWithType (currentField, gTextString);
				LSetCell ((char*)&gTextString[1], 
								(SInt16)gTextString[0], 
								cell,
								gStatisticsListHandle);
							
				}	// end "if (LGetSelect (TRUE, &cell, ..."
					
					// Force class/field name to be redrawn.										
				
			if (gProjectInfoPtr->outlineFieldType &
												fieldIdentPtr->fieldType) 		
				ForceFieldOutlineUpdate (TRUE);
			
			}	// end "if (classFieldFlag == 3)"
					
					      
				// Indicate that the project has been changed.					
						
		gProjectInfoPtr->changedFlag = TRUE;
			
				// Force update of project and processor menu items.			
						
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
		}	// end "if (OKFlag)"
	
	return (OKFlag); 
	
}	// end "EditClassFieldDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void EditClassFieldDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the edit 
//							class/field dialog parameters. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			EditClassFieldDialog in SEditStatistics.cpp
//							CMEditClassFieldDlg::OnInitDialog in WProjectDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1999
//	Revised By:			Larry L. Biehl			Date: 12/21/2017	

void EditClassFieldDialogInitialize (
				DialogPtr							dialogPtr, 
				SInt16								classFieldFlag,
				SInt16								classStorage,
				SInt16								currentField,
				UInt16*								selectedItemPtr)

{  
			// Hide the dialog items pertaining to the class popup and prompt.
							
	HideDialogItem (dialogPtr, IDC_ClassListPrompt);
	HideDialogItem (dialogPtr, IDC_ClassList);	
					
			// Present dialog for editing the class name.	
			
	if (classFieldFlag == 2)
		{	
				// Load the current class name							
			
		CopyPToP ((UCharPtr)gTextString,
						gProjectInfoPtr->classNamesPtr[classStorage].name);
						
		LoadDItemString (dialogPtr, IDC_ClassName, (Str255*)gTextString);
							
		*selectedItemPtr = IDC_ClassName;
							
				// Static Text with "Enter Field Identifier"				
				// Edit field for entering field identifier					
				// Radio button for training field								
				// Radio button for test field	
							
		HideDialogItem (dialogPtr, IDC_FieldNamePrompt);
		HideDialogItem (dialogPtr, IDC_FieldName);   

        // In linux, instead of having separate controls, there is
        // only one radiobox
		
		#if defined multispec_mac || defined mutispec_win
			HideDialogItem (dialogPtr, IDC_Training); 
			HideDialogItem (dialogPtr, IDC_TestField);
		#endif	// defined multispec_mac || ...
		#if defined multispec_wx
			HideDialogItem (dialogPtr, IDC_AreaTypeOutline);
		#endif	// defined multispec_wx

	
				// Show the number of training pixels in this class.
				
		SetNumberClassTrainPixels (dialogPtr, 0, classFieldFlag);
		
				// Hide the number of field pixels dialog item.
				
		HideDialogItem (dialogPtr, IDC_FieldNumberPixels);
		
				// Hide the area type grouping outline for Windows version.
				
		#if defined multispec_win 
			HideDialogItem (dialogPtr, IDC_AreaTypeOutline);
		#endif		// defined multispec_win 
		
		}	// end "if (classFieldFlag == 2)"
					
			// Present dialog for editing the field name and changing test/train	
			// field choice.																		
			
	if (classFieldFlag == 3)
		{		
		HPFieldIdentifiersPtr fieldIdentPtr = 
												&gProjectInfoPtr->fieldIdentPtr[currentField];
	
				// Static Text with "Enter Class Name"						
				// Edit field for entering class name	
							
		HideDialogItem (dialogPtr, IDC_ClassNamePrompt);
		HideDialogItem (dialogPtr, IDC_ClassName);		
	
				// Load the current field name					
	
		CopyPToP (gTextString, fieldIdentPtr->name);
						
		LoadDItemString (dialogPtr, IDC_FieldName, (Str255*)gTextString);
							
		*selectedItemPtr = IDC_FieldName;  
							
				// Radio button for training field.							
			
		SetDLogControl (dialogPtr, 
								IDC_Training,
								(fieldIdentPtr->fieldType == kTrainingType));
							
				// Radio button for test field.									
			
		SetDLogControl (dialogPtr, 
								IDC_TestField,
								(fieldIdentPtr->fieldType == kTestingType));
						
		if (fieldIdentPtr->pointType >= kClusterType)
			{
			if (fieldIdentPtr->fieldType == kTrainingType)
				SetDLogControlHilite (dialogPtr, IDC_TestField, 255);
				
			else if (fieldIdentPtr->fieldType == kTestingType)
				SetDLogControlHilite (dialogPtr, IDC_Training, 255);
				
			}	// end "if (fieldIdentPtr->pointType >= kClusterType)"
			
				// Hide the number of class pixels dialog item. 
			
		HideDialogItem (dialogPtr, IDC_ClassNumberPixels);
	
				// Show the number of pixels in this field.
		
		CreateNumberWithCommasInString ((char*)gTextString2, 
													fieldIdentPtr->numberPixels);
			
		gTextString[0] = sprintf ((char*)&gTextString[1],
											"Number pixels: %s",
											(char*)gTextString2);
		LoadDItemString (dialogPtr, IDC_FieldNumberPixels, (Str255*)gTextString);
		
		}	// end "if (classFieldFlag == 3)"
	
}	// end "EditClassFieldDialogInitialize"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean EditCoordinatesDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							to allow the user to edit the coordinates that 
//							describe a field or polygon. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsWControlEvent   in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/06/1989
//	Revised By:			Larry L. Biehl			Date: 03/17/2005

Boolean EditCoordinatesDialog (
				SInt16								currentClass,
				SInt16								currentField,
				SInt16								polygonPoint)				

{   
	DoubleRect							coordinateRectangle;
	LongRect								selectionRectangle;             
	
	Cell									cell;
	
	HPFieldIdentifiersPtr			fieldIdentPtr;   
	HPFieldPointsPtr					fieldPointsPtr;
	WindowInfoPtr						projectWindowInfoPtr;
												
	SInt16								classStorage,
											pointType,
											stringID,
											unitsDisplayCode,
											upperLeftStorage;
												
	Boolean								applyToAllWindowsFlag = FALSE,
											changedFlag = FALSE,
											OKFlag = FALSE,
											useStartLineColumnFlag = TRUE;
										
	
			// Check input parameters and set some local variables up.
			// Exit routine if current field or current class is not valid.		
					
	if (currentField<=-1 || currentField>gProjectInfoPtr->numberStorageFields)
																							return (FALSE);
																					
	if (currentClass<=-1 || currentClass>gProjectInfoPtr->numberStorageClasses)
																							return (FALSE);
														
	projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
													gProjectInfoPtr->windowInfoHandle, kLock);
	
	if (projectWindowInfoPtr == NULL)
																							return (FALSE);
																						
			// A project area/field is to be edited.
			
	classStorage = gProjectInfoPtr->storageClass[currentClass];
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;       
	fieldPointsPtr = gProjectInfoPtr->fieldPointsPtr;
	
	pointType = gProjectInfoPtr->fieldIdentPtr[currentField].pointType;
	
			// Set polygon point for rectangular field to start at 0 offset	
			// to upper left line-column storage.	
			// Note that if a polygon point then allow for the first two points
			// which represent the bounding rectangle.									
	
	if (pointType == kRectangleType)		
		polygonPoint = 0;
		
	else if (pointType == kPolygonType)	
		polygonPoint += 2;
	
			// Get rectangle coordinate storage locations.							
	
	upperLeftStorage = fieldIdentPtr[currentField].firstPolygonPoint + polygonPoint;
	
	if (pointType == kRectangleType)
		{
				// Get the rectangle coordinates.	
	
		selectionRectangle.top = fieldPointsPtr[upperLeftStorage].line;
		selectionRectangle.left = fieldPointsPtr[upperLeftStorage].col;
		selectionRectangle.bottom = fieldPointsPtr[upperLeftStorage+1].line;
		selectionRectangle.right = fieldPointsPtr[upperLeftStorage+1].col;
		
		stringID = IDS_Dialog24;
			
		}	// end "if (pointType == kRectangleType)"
		
	if (pointType == kPolygonType)
		{
				// Get coordinate values for the selected polygon coordinate.		
				
		selectionRectangle.top = fieldPointsPtr[upperLeftStorage].line;
		selectionRectangle.left = fieldPointsPtr[upperLeftStorage].col;
		selectionRectangle.bottom = projectWindowInfoPtr->maxNumberLines;
		selectionRectangle.right = projectWindowInfoPtr->maxNumberColumns;
		
		stringID = IDS_Dialog25;
			
		//CtoPstring ("Edit Polygonal Field Coordinate", (char*)gTextString);
		
		}	// end "if (pointType == kRectangleType)"
		
	unitsDisplayCode = kLineColumnUnits;

	if (EditLineColumnDialog (NULL,
										projectWindowInfoPtr,
										&selectionRectangle,
										&coordinateRectangle,
										pointType,
										&unitsDisplayCode,
										&changedFlag,
										&applyToAllWindowsFlag,
										&useStartLineColumnFlag,
										stringID))
		{	
				// If coordinate have changed, indicate changes in 			
				// project information structures.									
				
		if (changedFlag)
			{
					// Load new coordinate into project structure if a project field is
					// being edited.	
				
			fieldPointsPtr[upperLeftStorage].line =  selectionRectangle.top;
			fieldPointsPtr[upperLeftStorage].col =  selectionRectangle.left;
			
			if (pointType == kRectangleType)
				{
				fieldPointsPtr[upperLeftStorage+1].line =  selectionRectangle.bottom;
				fieldPointsPtr[upperLeftStorage+1].col = selectionRectangle.right;
				
				}	// end "if (pointType == kRectangleType)"
				
					// Load new coordinates into the statistics coordinate list window.
				
			if (pointType == kRectangleType)
				LoadRectangleInStatList (&selectionRectangle);
			
			if (pointType == kPolygonType)
				{
				cell.h = 0;
				cell.v = polygonPoint - 2;

				#if defined multispec_mac
					NumToString (selectionRectangle.top, gTextString);
					LSetCell ((char*)&(gTextString[1]), 
									(SInt16)gTextString[0],
									cell, 
									gStatisticsListHandle);
			 														
					cell.h = 1;
					cell.v = polygonPoint - 2;
					NumToString (selectionRectangle.left, gTextString);
					LSetCell ((char*)&(gTextString[1]), 
									(SInt16)gTextString[0],
									cell,
									gStatisticsListHandle);
				#endif	// defined multispec_mac 
					
				#if defined multispec_win || defined multispec_wx
					gTextString[0] = sprintf ((char*)&gTextString[1], 
														"%d\t%d",
														(int)selectionRectangle.top,
														(int)selectionRectangle.left);
					
					LSetCell ((char*)&gTextString[1], 
									(SInt16)gTextString[0],
									cell,
									gStatisticsListHandle);
				#endif	// defined multispec_win || defined multispec_wx
			 						
				GetPolygonLabelPoint (&fieldIdentPtr[currentField],
								gProjectInfoPtr->fieldPointsPtr,
								&fieldIdentPtr[currentField].labelPoint);	
				
				}	// end "if (pointType == kPolygonType)"
				
					// An existing project field was changed.
					// Indicate that project has changed.							
								
			gProjectInfoPtr->changedFlag = TRUE;
						
					// If field is a training field, make appropriate 			
					// changes relative to statistics.								
								
			RemoveFieldStatsFromClassStats (
												classStorage, currentField, TRUE);
						
					// Force field to be redrawn.										
						
			if (gProjectInfoPtr->outlineFieldType &
										fieldIdentPtr[currentField].fieldType) 		
				ForceFieldOutlineUpdate (TRUE);
			
					// Force update of the project menu items.					
								
			gUpdateProjectMenuItemsFlag = TRUE;
						
			}	// end "if (changedFlag)"
			
		}	// end "if (EditLineColumnDialog (..."
		
	CheckAndUnlockHandle (gProjectInfoPtr->windowInfoHandle);
	
	return (OKFlag);
	
}	// end "EditCoordinatesDialog"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetNumberOfTrainPixelsInClass
//
//	Software purpose:	The purpose of this routine is to determine the number of
//							training pixels in the class
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			DetermineIfSpecifiedStatisticsExist in SProjectComputeStatistics.cpp
//							UpdateClassStats in SProjectComputeStatistics.cpp
//							
//	Coded By:			Larry L. Biehl			Date: 12/18/1998
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

SInt64 GetNumberOfTrainPixelsInClass (
				HPClassNamesPtr					classNamesPtr,
				HPFieldIdentifiersPtr			fieldIdentPtr)

{									
	SInt64								numberOfTrainPixelsInClass = 0;
	
	SInt16								fieldNumber;
	
		
	fieldNumber = classNamesPtr->firstFieldNumber;
	while (fieldNumber != -1)
		{
				// Make certain that field is training field							
				
		if (fieldIdentPtr[fieldNumber].fieldType == kTrainingType)
			numberOfTrainPixelsInClass += fieldIdentPtr[fieldNumber].numberPixels;
			
		fieldNumber = fieldIdentPtr[fieldNumber].nextField;
		
		}	// end "while (fieldNumber != -1)"
	
	return (numberOfTrainPixelsInClass); 
			
}	// end "GetNumberOfTrainPixelsInClass"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetTotalNumberPixelsInField
//
//	Software purpose:	The purpose of this routine is to obtain the total number of 
//							pixels in the field.
//							Note that so far, the number of pixels in a mask area is
//							computed in "LoadMask" in SMask.cpp
//
//	Parameters in:		fieldNumber - the project field number.
//							areaDescriptionPtr - points to structure to use to
//											store the field coordinates.
//										
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			RemoveFieldStatsFromClassStats in SEditStatistics.cpp
//							VerifyClassAndFieldParameters in SProjectFileIO.cpp
//							AddFieldToProject in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/07/1991
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

SInt64 GetTotalNumberPixelsInField (
				SInt16								fieldNumber)

{
	AreaDescription					areaDescription;
	Point									point;
	
	SInt64								totalNumberPixels;
	
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	RgnHandle							rgnHandle;
	
	SInt32								column,
											columnEnd,
											columnStart,
											line,
											lineEnd;
	
	
	fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];
	
	if (fieldIdentPtr->pointType >= kClusterType)
																								return (0);
	
	InitializeAreaDescription (&areaDescription);
	GetFieldBoundary (gProjectInfoPtr, &areaDescription, fieldNumber);
	
			// Check if rectangular field or polygonal field.							
			
	if (areaDescription.pointType != kPolygonType)
		totalNumberPixels = 
			(SInt64)(areaDescription.lineEnd - areaDescription.lineStart + 1) *
					(areaDescription.columnEnd - areaDescription.columnStart + 1);
			
	else	// areaDescription.pointType == kPolygonType
		{
		rgnHandle = areaDescription.rgnHandle;
		totalNumberPixels = 0;
		
		if (rgnHandle != NULL)
			{
			lineEnd = areaDescription.lineEnd;
			columnStart = areaDescription.columnStart;
			columnEnd = areaDescription.columnEnd;
				
			point.v = (SInt16)areaDescription.lineStart;
			for (line=areaDescription.lineStart; line<=lineEnd; line++)
				{
				point.h = (SInt16)columnStart;
				for (column=columnStart; column<=columnEnd; column++)
					{
					if (PtInRgn (point, rgnHandle))
						totalNumberPixels++;
						
					point.h++;
						
					}	// end "for (column=columnStart; column<=..."
					
				point.v++;
					
				}	// end "for (line=areaDescriptionPtr->lineStart; ..."
				
			}	// end "if (rgnHandle != NULL)"
						
				// Dispose of the region.						

		CloseUpAreaDescription (&areaDescription);
			
		}	// end "else areaDescription.pointType == kPolygonType"
	
	//if (totalNumberPixels != fieldIdentPtr->numberPixels)
	//	gProjectInfoPtr->changedFlag = TRUE;
	
	return (totalNumberPixels);
			
}	// end "GetTotalNumberPixelsInField"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PasteClassFields
//
//	Software purpose:	The purpose of this routine is to paste the fields
//							for the 'cut class' into the current class statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  2/22/1989
//	Revised By:			Larry L. Biehl			Date: 01/13/1994			

void PasteClassFields (void)

{
	SInt16								addField,
											classIndex,
											currentStorageClass,
											nextField,
											pasteClassStorage;
	
	
			// Get the class storage number.													
				
	classIndex = gProjectInfoPtr->currentClass;
	currentStorageClass = gProjectInfoPtr->storageClass[classIndex];
		
	if (currentStorageClass >= 0 &&
							currentStorageClass < gProjectInfoPtr->numberStorageClasses)
		{
		pasteClassStorage = gProjectInfoPtr->editClassStorageNumber;
		addField = gProjectInfoPtr->classNamesPtr[pasteClassStorage].firstFieldNumber;
		
		while (addField != -1)
			{
			if (addField >= 0 && addField < gProjectInfoPtr->numberStorageFields)
				{
				gProjectInfoPtr->fieldIdentPtr[addField].classStorage = 
																			currentStorageClass;
			
						// Indicate that this is now an "active" field.					
					
				gProjectInfoPtr->fieldIdentPtr[addField].fieldType =
									-gProjectInfoPtr->fieldIdentPtr[addField].fieldType;
															
				nextField = gProjectInfoPtr->fieldIdentPtr[addField].nextField;
				AddField (addField, currentStorageClass);
		
				}	// end "if (addField >= 0 && ..."
				
			addField = nextField;
			
			}	// end "while (addField != -1)"
			
		}	// end "if (currentStorageClass >= 0 && ..."
		
}	// end "PasteClassFields"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PasteField
//
//	Software purpose:	The purpose of this routine is to paste the 'cut
// 						field' into the current class statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  2/22/1989
//	Revised By:			Larry L. Biehl			Date:  3/09/1990			

void PasteField (void)

{
	SInt16								addField,
											classIndex,
											currentStorageClass;
	
	
	addField = gProjectInfoPtr->editFieldNumber;
	if (addField >= 0 && addField < gProjectInfoPtr->numberStorageFields)
		{
				// Get the class storage number.												
				
		classIndex = gProjectInfoPtr->currentClass;
		currentStorageClass = gProjectInfoPtr->storageClass[classIndex];
		
		if (currentStorageClass >= 0 &&
						currentStorageClass < gProjectInfoPtr->numberStorageClasses)
			{
					// Indicate that this is an "active" field.							
					
			gProjectInfoPtr->fieldIdentPtr[addField].fieldType = 
							abs (gProjectInfoPtr->fieldIdentPtr[addField].fieldType);
			
			gProjectInfoPtr->fieldIdentPtr[addField].classStorage = currentStorageClass;
			AddField (addField, currentStorageClass);
			
			}	// end "if (currentStorageClass >= 0 && ..."
		
		}	// end "if (addField >= 0 && ..."
		
}	// end "PasteField"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PasteFieldToNewClass
//
//	Software purpose:	This routine pastes the 'cut' field to a new class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/22/1989
//	Revised By:			Larry L. Biehl			Date: 03/15/2017			

void PasteFieldToNewClass (void)

{
	Str31									className;
	
	SInt16								addField,
											fieldType;
	
	Boolean								newClassFlag,
											continueFlag;
	
	
	if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
		{
		addField = gProjectInfoPtr->editFieldNumber;
		newClassFlag = TRUE;
		fieldType = abs (gProjectInfoPtr->fieldIdentPtr[addField].fieldType);
		
				// Get class name and allow user to edit field name.					
					
		continueFlag = NewClassFieldDialog (
								newClassFlag, 
								className,
								gProjectInfoPtr->fieldIdentPtr[addField].name, 
								&fieldType);
			
				// Continue only if OK was clicked in dialog box otherwise exit	
				// routine.																			
					
		if (continueFlag)
			{
			continueFlag = AddClassToProject (className);
			
			if (continueFlag)
				{
				gProjectInfoPtr->fieldIdentPtr[addField].fieldType = fieldType;
				PasteField ();
				
				}	// end "if (continueFlag)"
			
			}	// end "if (continueFlag)"
			
		}	// end "if (...->numberStatisticsClasses < ..."
		
}	// end "PasteFieldToNewClass"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RemoveFieldStatsFromClassStats
//
//	Software purpose:	The purpose of this routine is to remove the field
//							statistics from the class statistics.  The routine
//							takes into account whether the statistics have been
//							loaded into the class and whether only class 
//							statistics are kept.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/13/1994
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

void RemoveFieldStatsFromClassStats (
				SInt16								storageClass,
				SInt16								currentField,
				Boolean								fieldSizeChangeFlag)

{
	HPClassNamesPtr					classNamesPtr;
	HPFieldIdentifiersPtr			fieldIdentPtr;
	
	Boolean								setUpdateControlFlag = FALSE;
	
				
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
		
			// Remove field statistics from class sums and sum of squares if 	
			// they have been loaded in.													
		
	if (fieldIdentPtr[currentField].fieldType == kTrainingType)
		{
		if (fieldIdentPtr[currentField].loadedIntoClassStats)
			{										
			fieldIdentPtr[currentField].loadedIntoClassStats = FALSE;
			classNamesPtr[storageClass].modifiedStatsFlag = FALSE;

			if (gProjectInfoPtr->keepClassStatsOnlyFlag)
				{											
				classNamesPtr[storageClass].numberStatisticsPixels = 0;
				classNamesPtr[storageClass].numberTrainPixels = 0;
				classNamesPtr[storageClass].statsUpToDate = FALSE;
				gProjectInfoPtr->statsUpToDate = FALSE;
				
				SetLoadedIntoClassStats (storageClass, FALSE);
			
				setUpdateControlFlag = TRUE;
					
				}	// end "if (gProjectInfoPtr->keepClassStatsOnlyFlag)"
			
			}	// end "if (...loadedIntoClassStats)"
			
				// Force any leave-one-out stats to be recalculated.
				
		classNamesPtr[storageClass].looCovarianceValue = -1;
			
				// Get the correct count for the number of pixels relative to the
				// statistics that are loaded in the class.
				
		classNamesPtr[storageClass].numberStatisticsPixels =  
						GetNumberOfPixelsLoadedInClass (&classNamesPtr[storageClass],
																	fieldIdentPtr,
																	kMeanCovariance);
				
		classNamesPtr[storageClass].numberTrainPixels =  
						GetNumberOfTrainPixelsInClass (
												&classNamesPtr[storageClass], fieldIdentPtr);
												
				// If the field size has changed, then make sure that these parameters
				// are set properly.
			
		if (fieldSizeChangeFlag)
			{							
			fieldIdentPtr[currentField].statsUpToDate = FALSE;
			classNamesPtr[storageClass].statsUpToDate = FALSE;
			gProjectInfoPtr->statsUpToDate = FALSE;
			gProjectInfoPtr->pixelDataLoadedFlag = FALSE;
			
			setUpdateControlFlag = TRUE;
			
			}	// end "if (fieldSizeChangeFlag)"
			
		if (setUpdateControlFlag && gProjectInfoPtr->updateControlH != NULL)
			MHiliteControl (
						gProjectWindow, gProjectInfoPtr->updateControlH, 0);
			
		}	// end "if (...fieldType == kTrainingType)"
							
	if (fieldSizeChangeFlag)		
		fieldIdentPtr[currentField].numberPixels = 
										GetTotalNumberPixelsInField (currentField);
	
			// Update any statistics variables if needed.	
	
	SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
		
}	// end "RemoveFieldStatsFromClassStats"

	

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetLoadedIntoClassStats
//
//	Software purpose:	The purpose of this routine is to set the 'loadedIntoStats'
//							flag for each of the training fields for the input
//							class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:					
//
//	Coded By:			Larry L. Biehl			Date: 01/12/1994
//	Revised By:			Larry L. Biehl			Date: 01/12/1994			

void SetLoadedIntoClassStats (
				SInt16								storageClass,
				Boolean								loadedIntoClassStatsFlag)

{
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								nextFieldNumber;
	
				
	classNamesPtr = gProjectInfoPtr->classNamesPtr;
	
	nextFieldNumber = classNamesPtr[storageClass].firstFieldNumber;
	while (nextFieldNumber != -1)
		{
				// Make certain that field is training field.							
		
		if (gProjectInfoPtr->fieldIdentPtr[nextFieldNumber].fieldType == 
																						kTrainingType)
			gProjectInfoPtr->fieldIdentPtr[nextFieldNumber].loadedIntoClassStats = 
																				loadedIntoClassStatsFlag;
	
		nextFieldNumber = gProjectInfoPtr->fieldIdentPtr[nextFieldNumber].nextField;

		}	// end "while (nextFieldNumber != -1)"
			
}	// end "SetLoadedIntoClassStats"


                   
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UndoCutClass
//
//	Software purpose:	The purpose of this routine is to undo the cut
//							class.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/16/1989
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

void UndoCutClass (void)

{
	Cell									cell;
	
	HPClassNamesPtr					classNamesPtr;
	
	SInt16								classStorage,
											field,
											newClass,
											row;
	
	
	classStorage = gProjectInfoPtr->editClassStorageNumber;
	if (classStorage >= 0 &&
			classStorage < gProjectInfoPtr->numberStorageClasses &&
			gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
		{
				// Initialize local variables.												
				
		classNamesPtr = gProjectInfoPtr->classNamesPtr;
		newClass	= gProjectInfoPtr->numberStatisticsClasses;
		
				// Update the project structure variables.								
				
		gProjectInfoPtr->numberStatisticsClasses++;
		if (classNamesPtr[classStorage].numberOfTrainFields > 0)
			gProjectInfoPtr->numberStatTrainClasses++;
		gProjectInfoPtr->numberStatisticsFields +=
												classNamesPtr[classStorage].numberOfFields;
		gProjectInfoPtr->numberStatTrainFields +=
										classNamesPtr[classStorage].numberOfTrainFields;
		gProjectInfoPtr->numberStatTestFields +=
										classNamesPtr[classStorage].numberOfTestFields;
										
				// Update number of coordinate points to define fields.				
										
		field = classNamesPtr[classStorage].firstFieldNumber;
		while (field > -1)
			{
			gProjectInfoPtr->numberTotalPoints +=
						gProjectInfoPtr->fieldIdentPtr[field].numberOfPolygonPoints;
			
					// Indicate that this is now an "active" field.						
					
			gProjectInfoPtr->fieldIdentPtr[field].fieldType = 
										-gProjectInfoPtr->fieldIdentPtr[field].fieldType;
				
			field = gProjectInfoPtr->fieldIdentPtr[field].nextField;
					
			}	// end "while (field > -1)"
		
				// Add this class back to the list of project classes.				
		
		gProjectInfoPtr->storageClass[newClass] = classStorage;
		classNamesPtr[classStorage].classNumber =
													gProjectInfoPtr->numberStatisticsClasses;
		
				// Indicate that project has changed.										
		
		gProjectInfoPtr->changedFlag = TRUE;
		
				// Add the class name to the class pop up menu							
      
      CopyPToP (gTextString, (UCharPtr)"\0NewClass\0");
		#if defined multispec_mac
			AppendMenu (gPopUpClassMenu, gTextString);
			SetMenuItemText (gPopUpClassMenu, 
									newClass+2, 
									classNamesPtr[classStorage].name);
		
			if (gProjectInfoPtr->numberStatisticsClasses >= kMaxNumberStatClasses-1)
	 			::DisableMenuItem (gPopUpClassMenu, 1);
		#endif	// defined multispec_mac
															
				// Add name to the class list													
		
		if (gProjectInfoPtr->statsWindowMode == 2)
			{
			LSetDrawingMode (FALSE, gStatisticsListHandle);
			cell.h = 0;
			
			if (gProjectInfoPtr->currentClass >= 0)
				{
				cell.v = gProjectInfoPtr->currentClass;
				LSetSelect (FALSE, cell, gStatisticsListHandle);
				
				}	// end "if (gProjectInfoPtr->currentClass >= 0)"
				
			row = LAddRow (1, newClass, gStatisticsListHandle);
							
			cell.v = newClass;
			LSetCell ((char*)&(classNamesPtr[classStorage].name[1]),
							(short int)(classNamesPtr[classStorage].name[0]), 
							cell, 
							gStatisticsListHandle);
				
					// Turn list drawing mode back on.											
			
			LSetDrawingMode (TRUE, gStatisticsListHandle);
			
					// Invalidate the list rectangle so that it will be updated.		
			 
			#if defined multispec_mac                  		
				InvalWindowRect (gProjectWindow, 
										GetListViewBounds (gStatisticsListHandle, &gTempRect));
			#endif	// defined multispec_mac
			
			gProjectInfoPtr->currentClass = newClass;
			UpdateToClassControl ();
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 2)"
		
				// Update current class depending on statistics window mode.		
				
		if (gProjectInfoPtr->statsWindowMode == 1)
			{
			gProjectInfoPtr->currentClass = newClass;
			
					// Invalidate the class pop up box.										
			
			InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);
			UpdateToClassControl ();
			
			}	// end "if (gProjectInfoPtr->statsWindowMode == 1)"
	
				// Make certain that the "To Project" control is hilited				
	
		MHiliteControl (gProjectWindow, gProjectInfoPtr->toProjectControlH, 0);
		
				// Redo outlining the fields so that the fields for the 'UnCut'	
				// class will be outlined.														
				
		OutlineFieldsInProjectWindows (gProjectInfoPtr->statsWindowMode, kDoClearArea);
		
				// Indicate that there is now no cut class to be undone.				
				// Also indicate that the class can not be pasted.						
				
		gProjectInfoPtr->editClassStorageNumber = -1;
		
				// Update Edit menu items.														
				
		UpdateStatWEditMenu ();
	
				// Update any statistics variables if needed.			
				
		//CheckEnhancedStatistics ();
	
		SetProjectCovarianceStatsToUse (gProjectInfoPtr->covarianceStatsToUse);
			
				// Force update of project and processor menu items.					
				
		gUpdateProjectMenuItemsFlag = TRUE;
		gUpdateProcessorMenuItemsFlag = TRUE;
		
		}	// end "if (classStorage >= 0 && ..."
			
}	// end "UndoCutClass"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UndoCutField
//
//	Software purpose:	The purpose of this routine is to undo the 'cut
// 						field' from the current class statistics.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date:  2/21/1989
//	Revised By:			Larry L. Biehl			Date:  3/20/1991			

void UndoCutField (void)

{
	SInt16								addField,
											currentStorageClass;
	
	
	addField = gProjectInfoPtr->editFieldNumber;
	if (addField >= 0 && addField < gProjectInfoPtr->numberStorageFields)
		{
				// Get the class storage number.												
				
		currentStorageClass = gProjectInfoPtr->fieldIdentPtr[addField].classStorage;
		
		if (currentStorageClass >= 0 &&
							currentStorageClass < gProjectInfoPtr->numberStorageClasses)
			{
					// Indicate that this is an "active" field.							
					
			gProjectInfoPtr->fieldIdentPtr[addField].fieldType = 
									-gProjectInfoPtr->fieldIdentPtr[addField].fieldType;
			
					// Everything checks out; add the field back in.					
					
			AddField (addField, currentStorageClass);
			
			}	// end "if (currentStorageClass >= 0 && ..."
		
		}	// end "if (addField >= 0 && ..."
	
			
}	// end "UndoCutField"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateStatWEditMenu
//
//	Software purpose:	This routine updates the edit menu for the
//							statistics window mode.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None	
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/15/1989
//	Revised By:			Larry L. Biehl			Date: 10/06/1998			

void UpdateStatWEditMenu (void)

{
	Cell									cell;
	
	SInt16								classNumber,
											editFieldClassStorage;
	
	Boolean								cutPolygonPointFlag;
	

	if (gProjectInfoPtr == NULL)
																									return;
																						
			// Initialize local variables.													
	
	cell.h = 0;
	
			// First disable all possible edit menu items that may be on.			
					
   DisableMenuItem (gMultiSpecMenus[kEditM], kUndo);
   DisableMenuItem (gMultiSpecMenus[kEditM], kCut);
   DisableMenuItem (gMultiSpecMenus[kEditM], kCopy);
   DisableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   DisableMenuItem (gMultiSpecMenus[kEditM], kSelectAll);
  	
	DisableMenuItem (gMultiSpecMenus[kEditM], kEditSelectionRect);
	DisableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
   
   		// Make sure that the copy and select menu items are generic.
   		
 	SetMenuItemText (gMultiSpecMenus[kEditM], kCopy, "\pCopy");
 	SetMenuItemText (gMultiSpecMenus[kEditM], kSelectAll, "\pSelect All");
	
			// Enable the Edit menu items.													
			
			// The Undo/Redo Cut Class item will be allowed for only the 'Select	
			// Field' and 'Project Class List' stat window modes.						
			
	if (gProjectInfoPtr->editClassStorageNumber < 0 &&
														gProjectInfoPtr->editFieldNumber < 0)
 		SetMenuItemText (gMultiSpecMenus[kEditM], kUndo, "\pUndo");
 		
 	else	// gProjectInfoPtr->editClassStorageNumber >= 0 || ...
 		{
		if (gProjectInfoPtr->editClassStorageNumber >= 0)
 			SetMenuItemText (gMultiSpecMenus[kEditM], kUndo, "\pUndo Cut Class");
 					
		if (gProjectInfoPtr->editFieldNumber >= 0)
	 		SetMenuItemText (gMultiSpecMenus[kEditM], kUndo, "\pUndo Cut Field");
		
		if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
  	 		EnableMenuItem (gMultiSpecMenus[kEditM], kUndo);
  	 				
  	 	}	// end "else gProjectInfoPtr->editClassStorageNumber != 0 || ..."
   		
	DisableMenuItem (gMultiSpecMenus[kEditM], kClearSelectionRect);
			
			// For 'Select field' mode.														
			
	if (gProjectInfoPtr->statsWindowMode == 1)
		{
 		SetMenuItemText (gMultiSpecMenus[kEditM], kCut, "\pCut");
 		
 		SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste");
  	 	if (gProjectInfoPtr->editFieldNumber >= 0 &&
  	 													gProjectInfoPtr->currentClass == -1)
  	 		{
 			SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste Field");
			if (gProjectInfoPtr->numberStatisticsClasses < kMaxNumberStatClasses-1)
   			EnableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   		
   		}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ...)"
 		
 		}	// end "if (gProjectInfoPtr->statsWindowMode == 1)"
 		
			// For 'List Project Classes' mode.												
			
	if (gProjectInfoPtr->statsWindowMode == 2)
		{		
 		SetMenuItemText (gMultiSpecMenus[kEditM], kCut, "\pCut Class");
  	 	if (gProjectInfoPtr->currentClass >= 0)
  	 		{
   		EnableMenuItem (gMultiSpecMenus[kEditM], kCut);
  	 		
			cell.v = gProjectInfoPtr->currentClass;
			LSetSelect (TRUE, cell, gStatisticsListHandle);
  	 		
  	 		}	// end "if (gProjectInfoPtr->currentClass >= 0)"
  	 		
 		SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste");
  	 	if (gProjectInfoPtr->editFieldNumber >= 0 &&
  	 														gProjectInfoPtr->currentClass >= 0)
  	 		{
  	 		editFieldClassStorage = gProjectInfoPtr->fieldIdentPtr[
  	 									gProjectInfoPtr->editFieldNumber].classStorage;
			classNumber = 
				gProjectInfoPtr->classNamesPtr[editFieldClassStorage].classNumber - 1;
 			if (classNumber != gProjectInfoPtr->currentClass)
 				{
 				SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste Field");
   			EnableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   			
   			}	// end "if (...fieldIdentPtr[gProjectInfoPtr->..."
   		
   		}	// end "if (gProjectInfoPtr->editFieldNumber >= 0 && ...)"
   		
  	 	if (gProjectInfoPtr->editClassStorageNumber >= 0 &&
  	 														gProjectInfoPtr->currentClass >= 0)
  	 		{
 			SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste Class Fields");
   		EnableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   		
   		}	// end "if (gProjectInfoPtr->editClassStorageNumber >= ...)"
  	 	
  	 	}	// end "if (gProjectInfoPtr->statsWindowMode == 2)"
  	 	
			// For 'List Class Fields' mode.													
			
	if (gProjectInfoPtr->statsWindowMode == 3)
		{
 		SetMenuItemText (gMultiSpecMenus[kEditM], kCut, "\pCut Field");
  	 	if (gProjectInfoPtr->currentField >= 0)
   		EnableMenuItem (gMultiSpecMenus[kEditM], kCut);
  	 		
  	 	if (gProjectInfoPtr->editFieldNumber >= 0)
  	 		{
 			SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste Field");
  	 		editFieldClassStorage = gProjectInfoPtr->fieldIdentPtr[
  	 									gProjectInfoPtr->editFieldNumber].classStorage;
			classNumber = 
				gProjectInfoPtr->classNamesPtr[editFieldClassStorage].classNumber - 1;
 			if (classNumber != gProjectInfoPtr->currentClass)
   			EnableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   		
   		}	// end "if (gProjectInfoPtr->editFieldNumber >= 0)"
   		
  	 	if (gProjectInfoPtr->editClassStorageNumber >= 0)
  	 		{
 			SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste Class Fields");
   		EnableMenuItem (gMultiSpecMenus[kEditM], kPaste);
   		
   		}	// end "if (gProjectInfoPtr->editClassStorageNumber >= 0)"
  	 	
  	 	}	// end "if (gProjectInfoPtr->statsWindowMode == 3)"
  	 	
			// For field coordinates mode.													
			
	if (gProjectInfoPtr->statsWindowMode == 4)
		{
				// Determine if this is a polygon field and if so if the number	
				// of points is greater than 3.												
		
		cutPolygonPointFlag = FALSE;
		if (gProjectInfoPtr->fieldIdentPtr[
  	 					gProjectInfoPtr->currentField].pointType == kPolygonType &&
  	 			gProjectInfoPtr->fieldIdentPtr[
  	 						gProjectInfoPtr->currentField].numberOfPolygonPoints > 3)
  	 		{
			cell.v = 0;
			if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
				cutPolygonPointFlag = TRUE;
				
			}	// end "if (gProjectInfoPtr->fieldIdentPtr[..."
		
		if (cutPolygonPointFlag)
			{
 			SetMenuItemText (gMultiSpecMenus[kEditM], kCut, "\pCut Point");
   		EnableMenuItem (gMultiSpecMenus[kEditM], kCut);
   		
   		}	// end "if (cutPolygonPointFlag)"
 			
 		else	// !cutPolygonPointFlag
 			SetMenuItemText (gMultiSpecMenus[kEditM], kCut, "\pCut");
 			
 		SetMenuItemText (gMultiSpecMenus[kEditM], kPaste, "\pPaste");
  	 	
  	 	}	// end "if (gProjectInfoPtr->statsWindowMode == 4)"
   
}	// end "UpdateStatWEditMenu"
#endif	// defined multispec_mac 
