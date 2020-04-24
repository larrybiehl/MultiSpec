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
//	File:						SStatistics.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/18/2020
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	This file contains functions that compute and operate 
//								on field and class statistics.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//		StatisticsControl
//			InitializeNewProject (in SProject.cpp)
//
//			StatisticsDialog
//				SetDLogControl	(in SUtilities.cpp)
//				SetDLogControlHilite	(in SUtilities.cpp)
//				ShowDialogWindow (in SUtilities.cpp)
//				CheckSomeEvents (in MMultiSpec.c)
//				HiliteOKButton (in dialogUtilities.c)
//				ChangeDLogCheckBox (in SUtilities.cpp)
//				GetDLogControl (in SUtilities.cpp)
//
//			CreateProjectWindow
//		
//			NewFieldStatMode
//				(see below under "StatisticsWControlEvent")
//
//			CloseProjectStructure (in SProject.cpp)
//
//		UpdateStatisticsWindow
//			DrawClassPopUp
//			DrawClassPrompt
//			DrawFieldPrompt
//
//		ActivateStatControls
//
//		StatisticsWMouseDn
//			UpdateStatWEditMenu
//			UpdateToClassControl
//
//		GetCurrentField
//
//		StatisticsWControlEvent
//			ButtonDownEvent
//			AddFieldToProject
//				NewClassFieldDialog
//					.
//					.
//				AddClassToProject
//					CheckHandleSize
//					CopyPToP
//
//				CheckHandleSize
//				GetLineColFromList
//				CopyPToP
//				RemoveListCells
//				ClearSelectionArea (in MMultiSpec.c)
//
//			ClassListStatMode
//				RemoveListCells
//				LoadClassNamesIntoList
//				DoStatWActivateEvent
//
//			EditClassFieldDialog (in SEditStatistics.cpp)
//
//			EditCoordinatesDialog (in SEditStatistics.cpp)
//
//			FieldListStatMode
//				RemoveListCells
//				GetFieldNameWithType
//				DoStatWActivateEvent
//
//			NewFieldStatMode
//				UpdateToClassControl
//				RemoveListCells
//				DrawClassPopUp
//					DrawClassPrompt
//						CheckStringLength
//				DoStatWActivateEvent
//
//			PolygonListStatMode
//				RemoveListCells
//				NumToString
//				DoStatWActivateEvent
//				ForceFieldOutlineUpdate
//
//			ListStatsControl
//			UpdateStatsControl
//
//		
//		LoadPolyStatNewFieldW
//		LoadRectStatNewFieldW
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_wx
	#define	IDOK	1
	#include "xImageView.h"
   #include "xLeaveOneOutMixingDialog.h"   
	#include	"xMultiSpec.h"
	#include "xNewClassFieldDialog.h"
	#include "xStatisticsDialog.h"
	#include "xStatisticsOptionsDialog.h"
	#include "xStatisticsFrame.h"
	#include "xStatisticsView.h"
	#include "wx/wx.h"
	#include "wx/docview.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#define	IDOK									1
	#define	IDC_ClassNamePrompt				3
	#define	IDC_ClassName						4
	#define	IDC_FieldNamePrompt				5
	#define	IDC_FieldName						6   
	#define	IDC_Training						7 
	#define	IDC_TestField						8
	#define	IDC_ClassListPrompt				9
	#define	IDC_ClassList						10
	#define	IDC_ClassNumberPixels			11
	#define	IDC_FieldNumberPixels			12

	#define	IDC_LayerPrompt					16
	#define	IDC_TrainMaskPopUp				18	
	#define	IDC_TrainMaskLayer				19		
	#define	IDC_TestMaskPopUp					21
	#define	IDC_TestMaskLayer					22
	#define	IDC_TrainMaskLayerEdit			23
	#define	IDC_TestMaskLayerEdit			24

	#define	IDC_OptimumValue					5
	#define	IDC_UserValue						7
#endif	// defined multispec_mac || defined multispec_mac_swift

#if defined multispec_win
	#include "WImageView.h" 
	#include "WLeaveOneOutMixingDialog.h"
	#include "WMultiSpec.h"
	#include "WNewClassFieldDialog.h"
	#include "WStatisticsDialog.h"
	#include "WStatisticsOptionsDialog.h"
	#include "WStatisticsView.h"
#endif	// defined multispec_win



#define	kStatisticsWindowWidth		169 // 134



		// Global variables for this routine.

SInt16 gOutlineColorSelection = 1;
SInt16 gMaskTestImageSelection = 1;
SInt16 gMaskTrainImageSelection = 1;

SInt16 gStatWindowTextFont = 0;
SInt16 gStatisticsWindowControlHeight = 263;
SInt16 gStatisticsWindHeight = 147;  // 117


		// Prototypes for routines in this file that are only called by
		// other routines in this file.

Boolean AddPointsToProject (
				SInt16								pointType,
				SInt16								currentStorageField);

void ClassListStatMode (void);

Boolean CreateProjectWControls (void);

void DrawClassPopUp (void);

void DrawClassPrompt (void);

void DrawFieldPrompt (void);

PascalVoid DrawDialogClassPopUp2 (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawSelectTestImageMaskPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

PascalVoid DrawSelectTrainImageMaskPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

void DrawStatPopUp (
				SInt16								statsWindowMode);

void DrawStatPrompt (
				SInt16								menuItem);

SInt16 GetControlValue (
				WindowPtr							windowPtr,
				ControlHandle						controlHandle);

SInt16 GetCurrentClassField (
				SInt16								classNumber,
				SInt16								inputFieldNumber);

SInt16 GetCurrentField (
				SInt16								classNumber,
				SInt16								classFieldNumber);

void HideStatControl (
				ControlHandle						controlHandle);

void LoadPolyStatNewFieldW (void);

void LoadRectStatNewFieldW (void);

PascalVoid DrawOutlineColorPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber);

void SetOutlineAreaOptions (
				DialogPtr							dialogPtr,
				SInt16								labelFieldCode);

void SetStatControlTitle (
				ControlHandle						controlHandle,
				char*									titleStringPtr);

void ShowStatControl (
				ControlHandle						controlHandle);

SInt16 StatisticsDialog (
				SInt16*								featurePtr,
				SInt16								totalNumberChannels);

void StatisticsDialogSetMaskFileName (
				UCharPtr								fileNamePPointer,
				MenuHandle							popUpSelectMaskImageMenu,
				DialogPtr							dialogPtr,
				SInt16								maskPopupItemNumber);

SInt16 StatisticsDialogSetMaskItems (
				MaskInfoPtr							maskInfoPtr,
				MenuHandle							popUpSelectMaskImageMenu,
				DialogPtr							dialogPtr,
				SInt16								maskPopupItemNumber,
				SInt16								staticLayerItemNumber,
				UInt16*								maxNumberLayersPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ActivateStatControls
//
//	Software purpose:	The purpose of this routine is to activate the
//							appropriate controls for the current statistics
//							window mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassListStatMode
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1988
//	Revised By:			Larry L. Biehl			Date: 12/05/2019

void ActivateStatControls (void) 
{
			// Declare local variables and structures										

   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								currentStorageClass,
											hiliteValue;


			// Check variables.																	

   if (gProjectInfoPtr == NULL)
																									return;

			// Initialize local variables.													

   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

   switch (gProjectInfoPtr->statsWindowMode)
		{
      case kSelectFieldMode: // "Select New Field" mode 

					// Hide the controls that are not a part of the "Select New		
					// Field" mode																	

         HideStatControl (gProjectInfoPtr->newFieldControlH);
         HideStatControl (gProjectInfoPtr->updateControlH);
         HideStatControl (gProjectInfoPtr->listControlH);
         HideStatControl (gProjectInfoPtr->histogramControlH);

					// Show the controls that are part of the "New Field" mode 

         ShowStatControl (gProjectInfoPtr->toFieldControlH);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);

         ShowStatControl (gProjectInfoPtr->addToControlH);
         ShowStatControl (gProjectInfoPtr->polygonTypeControlH);
         ShowStatControl (gProjectInfoPtr->toProjectControlH);
         ShowStatControl (gProjectInfoPtr->toClassControlH);

			#if defined multispec_win || defined multispec_wx
				ShowStatControl ((ControlHandle)IDC_STATIC_Class);
				ShowStatControl ((ControlHandle)IDC_ClassList);
				HideStatControl ((ControlHandle)IDC_ClassName);
				HideStatControl ((ControlHandle)IDC_STATIC_Field);
				HideStatControl ((ControlHandle)IDC_FieldName);
				HideStatControl ((ControlHandle)IDC_StatsPrompt);
				HideStatControl ((ControlHandle)IDC_StatsType);
				HideStatControl ((ControlHandle)IDC_StatsCombo);
			#endif	// defined multispec_win || ...

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Edit Selection...", gTextString);
				SetStatControlTitle (
									gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_mac
		
			#if defined multispec_win || defined multispec_wx
				CtoPstring ((UCharPtr)"Edit Selection...", gTextString);
				SetStatControlTitle (
									gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_win || ...
         
			ShowStatControl (gProjectInfoPtr->editNameControlH);
         
			hiliteValue = 255;
         if (gStatisticsMode == kStatNewFieldRectMode)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->editNameControlH, hiliteValue);
         break;

      case kClassListMode: // "Class List" mode (List of Classes)

					// Hide the controls that are not a part of the "Class List" mode

         HideStatControl (gProjectInfoPtr->toProjectControlH);
         HideStatControl (gProjectInfoPtr->addToControlH);
         HideStatControl (gProjectInfoPtr->polygonTypeControlH);

			#if defined multispec_win || defined multispec_wx
				HideStatControl ((ControlHandle)IDC_STATIC_Class);
				HideStatControl ((ControlHandle)IDC_ClassList);
				HideStatControl ((ControlHandle)IDC_ClassName);
				HideStatControl ((ControlHandle)IDC_STATIC_Field);
				HideStatControl ((ControlHandle)IDC_FieldName);
				ShowStatControl ((ControlHandle)IDC_StatsPrompt);
				HideStatControl ((ControlHandle)IDC_StatsType);
				ShowStatControl ((ControlHandle)IDC_StatsCombo);
				MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);
			#endif	// defined multispec_win || ...

					// Show the controls that are part of the "Class List" mode 

         ShowStatControl (gProjectInfoPtr->toFieldControlH);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);

         ShowStatControl (gProjectInfoPtr->toClassControlH);
         hiliteValue = 255;
         if (gProjectInfoPtr->currentClass >= 0)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->toClassControlH, hiliteValue);

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Update Project Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->updateControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			
			#if defined multispec_win || defined multispec_wx
				CtoPstring ((UCharPtr)"Update Project Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->updateControlH, (char*)gTextString);
			#endif	// defined multispec_win || ...
         
			ShowStatControl (gProjectInfoPtr->updateControlH);
         
			hiliteValue = 0;
         if (gProjectInfoPtr->statsUpToDate)
            hiliteValue = 255;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, hiliteValue);

         ShowStatControl (gProjectInfoPtr->newFieldControlH);

					// Set control for listing statistics.			

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"List Classes Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->listControlH, (char*)gTextString);
			#endif	// defined multispec_mac

			#if defined multispec_win  || multispec_wx
				// This is handled by WStatisticsFrame and xStatisticsFrame
			#endif	// defined multispec_win
			
         ShowStatControl (gProjectInfoPtr->listControlH);

					// Set control for histogram list.

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Histogram Classes", gTextString);
				SetStatControlTitle (
									gProjectInfoPtr->histogramControlH, (char*)gTextString);
			#endif	// defined multispec_mac

			#if defined multispec_win  || defined multispec_wx
				// This is handled by WStatisticsFrame and xStatisticsFrame
			#endif	// defined multispec_win
         ShowStatControl (gProjectInfoPtr->histogramControlH);
         hiliteValue = 255;
         if (gProjectInfoPtr->numberStatTrainFields > 0)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->histogramControlH, hiliteValue);
			
			if (hiliteValue == 0 && gProjectInfoPtr->keepClassStatsOnlyFlag)
				gProjectInfoPtr->histogramClassFieldCode =
											gProjectInfoPtr->histogramClassListClassFieldCode;

					// set edit class name control.

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Edit Class Name...", gTextString);
				SetStatControlTitle (
									gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_mac

			#if defined multispec_win || multispec_wx
				CtoPstring ((UCharPtr)"Edit Class Name...", gTextString);
				SetStatControlTitle (
										gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_win

         ShowStatControl (gProjectInfoPtr->editNameControlH);
         hiliteValue = 255;
         if (gProjectInfoPtr->currentClass >= 0)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->editNameControlH, hiliteValue);

         break;

      case kFieldListMode: // "Field List" mode (List of Fields)

					// Hide the controls that are not a part of the "Field List" mode

         HideStatControl (gProjectInfoPtr->addToControlH);
         HideStatControl (gProjectInfoPtr->polygonTypeControlH);
         HideStatControl (gProjectInfoPtr->toClassControlH);

			#if defined multispec_win || multispec_wx
				ShowStatControl ((ControlHandle)IDC_STATIC_Class);
				HideStatControl ((ControlHandle)IDC_ClassList);
				ShowStatControl ((ControlHandle)IDC_ClassName);
				HideStatControl ((ControlHandle)IDC_STATIC_Field);
				HideStatControl ((ControlHandle)IDC_FieldName);
				ShowStatControl ((ControlHandle)IDC_StatsPrompt);
				HideStatControl ((ControlHandle)IDC_StatsType);
				ShowStatControl ((ControlHandle)IDC_StatsCombo);
				MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);
			#endif	// defined multispec_win

					// Get the storage location for the current class.

         currentStorageClass =
            gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];

					// Show the controls that are part of the "Field List" mode

         ShowStatControl (gProjectInfoPtr->toFieldControlH);
         hiliteValue = 255;
         if (gProjectInfoPtr->currentField >= 0 &&
					fieldIdentPtr[gProjectInfoPtr->currentField].pointType != kClusterType)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->toFieldControlH, hiliteValue);

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Update Class Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->updateControlH, (char*)gTextString);
			#endif	// defined multispec_mac

			#if defined multispec_win || multispec_wx
				CtoPstring ((UCharPtr)"Update Class Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->updateControlH, (char*)gTextString);
			#endif	// defined multispec_win

         ShowStatControl (gProjectInfoPtr->updateControlH);
         hiliteValue = 0;
         if (classNamesPtr[currentStorageClass].statsUpToDate)
				{
            if (classNamesPtr[currentStorageClass].covarianceStatsToUse !=
																					kLeaveOneOutStats ||
						classNamesPtr[currentStorageClass].looCovarianceValue >= 0)
               hiliteValue = 255;

				}	// end "if (classNamesPtr[currentStorageClass].statsUpToDate)"
         
			MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, hiliteValue);
         ShowStatControl (gProjectInfoPtr->newFieldControlH);
         ShowStatControl (gProjectInfoPtr->toProjectControlH);

					// Set control for listing statistics.

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"List Class Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->listControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			
         ShowStatControl (gProjectInfoPtr->listControlH);
         if (!classNamesPtr[currentStorageClass].numberOfTrainFields)
            gProjectInfoPtr->listFieldFlag = FALSE;

					// Set controls for histogram list.

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Histogram Class", gTextString);
				SetStatControlTitle (
									gProjectInfoPtr->histogramControlH, (char*)gTextString);
			#endif	// defined multispec_mac
		
         ShowStatControl (gProjectInfoPtr->histogramControlH);
         hiliteValue = 255;
         if (classNamesPtr[currentStorageClass].numberOfTrainFields > 0)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->histogramControlH, hiliteValue);
			
		  if (hiliteValue == 0 && gProjectInfoPtr->keepClassStatsOnlyFlag)
				gProjectInfoPtr->histogramClassFieldCode =
											gProjectInfoPtr->histogramFieldListClassFieldCode;

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Edit Field Name...", gTextString);
				SetStatControlTitle (
										gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			#if defined multispec_win || multispec_wx
				CtoPstring ((UCharPtr)"Edit Field Name...", gTextString);
				SetStatControlTitle (
										gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_win
		
         ShowStatControl (gProjectInfoPtr->editNameControlH);
         hiliteValue = 255;
         if (gProjectInfoPtr->currentField >= 0)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->editNameControlH, hiliteValue);
         break;

      case kCoordinateListMode: // "Coordinate List" mode (List of Polygon Points)

					// Hide the controls that are not a part of the "Polygon Point
					// List" mode

         HideStatControl (gProjectInfoPtr->toFieldControlH);
         HideStatControl (gProjectInfoPtr->addToControlH);
         HideStatControl (gProjectInfoPtr->polygonTypeControlH);

			#if defined multispec_win || multispec_wx
				ShowStatControl ((ControlHandle)IDC_STATIC_Class);
				HideStatControl ((ControlHandle)IDC_ClassList);
				ShowStatControl ((ControlHandle)IDC_ClassName);
				ShowStatControl ((ControlHandle)IDC_STATIC_Field);
				ShowStatControl ((ControlHandle)IDC_FieldName);
				HideStatControl ((ControlHandle)IDC_StatsPrompt);
				HideStatControl ((ControlHandle)IDC_StatsType);
				HideStatControl ((ControlHandle)IDC_StatsCombo);
			#endif	// defined multispec_win

					// Show the controls that are part of the "Polygon Point List" mode

         ShowStatControl (gProjectInfoPtr->toProjectControlH);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toProjectControlH, 0);

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Update Field Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->updateControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			#if defined multispec_win || multispec_wx
				CtoPstring ((UCharPtr)"Update Field Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->updateControlH, (char*)gTextString);
			#endif	// defined multispec_win
		
         ShowStatControl (gProjectInfoPtr->updateControlH);
         hiliteValue = 0;
         if (fieldIdentPtr[gProjectInfoPtr->currentField].fieldType ==
																							kTestingType ||
					fieldIdentPtr[gProjectInfoPtr->currentField].statsUpToDate ||
															gProjectInfoPtr->keepClassStatsOnlyFlag)
            hiliteValue = 255;
         MHiliteControl (gProjectWindow, gProjectInfoPtr->updateControlH, hiliteValue);

         ShowStatControl (gProjectInfoPtr->newFieldControlH);

         ShowStatControl (gProjectInfoPtr->toClassControlH);
         MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 0);

					// Set control for listing statistics.

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"List Field Stats", gTextString);
				SetStatControlTitle (gProjectInfoPtr->listControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			#if defined multispec_win
				//CtoPstring ((char*)"List Field Stats", (char*)gTextString);
				//SetStatControlTitle (gProjectInfoPtr->listControlH, (char*)gTextString);
			#endif	// defined multispec_win
		
					// Cannot list fields stats if only the class stats are being stored
			
			if (gProjectInfoPtr->keepClassStatsOnlyFlag)
         	HideStatControl (gProjectInfoPtr->listControlH);
			
			else	// !gProjectInfoPtr->keepClassStatsOnlyFlag
         	ShowStatControl (gProjectInfoPtr->listControlH);

					// Set controls for histogram list.

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Histogram Field", gTextString);
				SetStatControlTitle (
									gProjectInfoPtr->histogramControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			#if defined multispec_win || defined multispec_wx
				// Not done here.
			#endif	// defined multispec_win
		
         ShowStatControl (gProjectInfoPtr->histogramControlH);
         hiliteValue = 255;
         if (fieldIdentPtr[gProjectInfoPtr->currentField].fieldType == kTrainingType)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->histogramControlH, hiliteValue);

         if (hiliteValue == 0)
				gProjectInfoPtr->histogramClassFieldCode = kHistogramField;

			#if defined multispec_mac
				CtoPstring ((UCharPtr)"Edit Coordinates...", gTextString);
				SetStatControlTitle (
										gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_mac
			#if defined multispec_win   || multispec_wx
				CtoPstring ((UCharPtr)"Edit Coordinates...", gTextString);
				SetStatControlTitle (
										gProjectInfoPtr->editNameControlH, (char*)gTextString);
			#endif	// defined multispec_win
		
         ShowStatControl (gProjectInfoPtr->editNameControlH);
         hiliteValue = 255;
         if (fieldIdentPtr[gProjectInfoPtr->currentField].pointType ==
            kRectangleType || gProjectInfoPtr->currentCoordinate >= 0)
            hiliteValue = 0;
         MHiliteControl (
							gProjectWindow, gProjectInfoPtr->editNameControlH, hiliteValue);

			#if defined multispec_wx
				// Load the current field name
				SInt16 currentfield = gProjectInfoPtr->currentField; 
				CtoPstring ((UCharPtr)fieldIdentPtr[currentfield].name, gTextString);
				SetStatControlTitle ((Handle)IDC_FieldName, (char*)&gTextString[1]);
			#endif
         break;

		}	// end "switch (gProjectInfoPtr->statsWindowMode)"
	
	#if defined multispec_wx
		((CMStatisticsView*)gProjectWindow)->GetFrame()->Layout ();
		((CMStatisticsView*)gProjectWindow)->GetFrame()->Refresh ();
	#endif
	
}	// end "ActivateStatControls" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean AddClassToProject
//
//	Software purpose:	This routine adds a new class to the open project.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			SaveClusterStatistics in SCluster.cpp
//							PasteFieldToNewClass in SEditStatistics.cpp
//							AddFieldToProject in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/22/1989
//	Revised By:			Larry L. Biehl			Date: 11/21/2019

Boolean AddClassToProject (
				UCharPtr								classNamePtr)

{
	HPClassNamesPtr					classNamesPtr;

   UInt32								bytesNeeded,
											currentClass,
											currentStorageClass;

   Boolean								changedFlag,
											continueFlag;


	#if defined multispec_win
		USES_CONVERSION;
	#endif

			// Initialize local variables.													

   continueFlag = TRUE;
   currentClass = gProjectInfoPtr->numberStatisticsClasses;
   currentStorageClass = gProjectInfoPtr->numberStorageClasses;

			// Check number of classes is at maximum.  Exit if it is.
			// Note allow one class as a background class.

   if (currentClass >= (kMaxNumberStatClasses - 1))
																							return (FALSE);

			// Unlock some project handles if needed so as not to fragment memory.

   UnlockProjectMemory (&gProjectInfoPtr, 2, NULL);

			// Change size of handle for class names if needed.

   bytesNeeded = (UInt32)(currentStorageClass + 1) * sizeof (ProjectClassNames);
   gProjectInfoPtr->classNamesPtr = (HPClassNamesPtr)CheckHandleSize (
																	&gProjectInfoPtr->classNamesHandle,
																	&continueFlag,
																	&changedFlag,
																	(SInt64)bytesNeeded);

   LockProjectMemory (NULL, 0, &gProjectInfoPtr);

   if (!continueFlag)
																							return (FALSE);

   classNamesPtr = gProjectInfoPtr->classNamesPtr;

			// Copy class name to class structure for list

   CopyPToP (classNamesPtr[currentStorageClass].name, classNamePtr);

			// Update project information variables.

   gProjectInfoPtr->storageClass[currentClass] = (SInt16)currentStorageClass;
   gProjectInfoPtr->numberStatisticsClasses++;
   gProjectInfoPtr->numberStorageClasses++;

   classNamesPtr[currentStorageClass].classNumber =
											(SInt16)gProjectInfoPtr->numberStatisticsClasses;

			// Set the type of statistics to be used for the class. If the project stats
			// are mixed then set to original stats. Also if the project stats are
			// enhanced, then set to original stats.

   classNamesPtr[currentStorageClass].covarianceStatsToUse =
																gProjectInfoPtr->covarianceStatsToUse;

   if (gProjectInfoPtr->covarianceStatsToUse == kMixedStats)
      classNamesPtr[currentStorageClass].covarianceStatsToUse = kOriginalStats;

   else if (gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats)
      classNamesPtr[currentStorageClass].covarianceStatsToUse = kOriginalStats;

   classNamesPtr[currentStorageClass].priorWeights[0] = (float)10.; // original stats-uequal 
   classNamesPtr[currentStorageClass].priorWeights[1] = (float)10.; // enhanced stats-uequal
   classNamesPtr[currentStorageClass].priorWeights[2] = (float)10.; // equal weighting
   classNamesPtr[currentStorageClass].priorWeights[3] = (float)10.; // common covariance weight
   classNamesPtr[currentStorageClass].looCovarianceValue = -1.;
   classNamesPtr[currentStorageClass].userMixingParameter = 1.0;
   classNamesPtr[currentStorageClass].mixingParameterCode = kComputedOptimum;
   classNamesPtr[currentStorageClass].numberOfFields = 0;
   classNamesPtr[currentStorageClass].numberOfTrainFields = 0;
   classNamesPtr[currentStorageClass].numberOfTestFields = 0;
   classNamesPtr[currentStorageClass].firstFieldNumber = -1;
   classNamesPtr[currentStorageClass].lastFieldNumber = -1;
   classNamesPtr[currentStorageClass].numberStatisticsPixels = 0;
   classNamesPtr[currentStorageClass].numberTrainPixels = 0;
   classNamesPtr[currentStorageClass].modifiedStatsStorage = -1;
   classNamesPtr[currentStorageClass].listMessageFlag = TRUE;
   classNamesPtr[currentStorageClass].modifiedStatsFlag = FALSE;
   classNamesPtr[currentStorageClass].statsUpToDate = TRUE;

   gProjectInfoPtr->currentClass = (SInt16)currentClass;

			// Add the class name to the class pop up menu.								
			// An AppendMenu and then SetMenuItemText is used so that no characters		
			// will be interpreted as "metacharacters" by AppendMenu.												

	#if defined multispec_mac
		AppendMenu (gPopUpClassMenu, "\pNewClass");
		SetMenuItemText (gPopUpClassMenu, currentClass + 2, classNamePtr);

		if (gProjectInfoPtr->numberStatisticsClasses >= kMaxNumberStatClasses - 1)
			DisableMenuItem (gPopUpClassMenu, 1);
	#endif	// defined multispec_mac 

			// Update the project window if it exists.

   if (gProjectWindow != NULL)
		{
		#if defined multispec_win                                   
			((CComboBox*)gProjectWindow->GetDlgItem (IDC_ClassList))->
											AddString (((LPCTSTR)A2T((LPSTR)&classNamePtr[1])));
			//comboBoxPtr->AddString ((LPCTSTR)A2T((LPSTR)stringPtr));
		#endif	// defined multispec_win

		#if defined multispec_wx                                   
			((wxChoice*)gProjectWindow->GetFrame()->FindWindow (IDC_ClassList))->
														AppendString (&((char*)classNamePtr)[1]);
			//AddString (&((char*)classNamePtr)[1]);
		#endif	// defined multispec_wx
      
				// Hilite the "to Class Field List" button.								

      MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 0);

				// Make certain that the "To Project" control is hilited				

      MHiliteControl (gProjectWindow, gProjectInfoPtr->toProjectControlH, 0);

				// Force class pop up menu box to be redrawn
						
      //InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);

		}	// end "if (gProjectWindow != NULL)" 

   return (TRUE);

}	// end "AddClassToProject" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean AddFieldToProject
//
//	Software purpose:	This routine adds the selected field to the open
//							project.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			SaveClusterStatistics in SCluster.cpp
//							LoadMask in SMask.cpp
//							StatisticsWControlEvent SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1988
//	Revised By:			Larry L. Biehl			Date: 05/05/2019

Boolean AddFieldToProject (
				SInt16								currentClass,
				SInt16								pointType,
				SInt16								fieldType,
				UCharPtr								fieldIdentifierPtr)
	
{
			// Allow 31 characters for class name and 1 for pascal character count
			// and 1 for c-string terminator.

   Str63									className;

   GrafPtr								savedPort;
   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   UInt32								bytesNeeded;

   SInt16								currentStorageClass,
											currentStorageField,
											savedLastField;

   Boolean								changedFlag,
											continueFlag,
											OKClickedFlag;


   // Initialize local variables.													

   continueFlag = TRUE;

   // Get field identifier and also class name if needed						

   if (pointType == kRectangleType || pointType == kPolygonType)
		{
      fieldType = gProjectInfoPtr->lastFieldTypeForAddFieldSelection;
      OKClickedFlag = NewClassFieldDialog (
											FALSE, className, fieldIdentifierPtr, &fieldType);

				// Continue only if OK was clicked in dialog box otherwise exit	
				// routine																			

      if (OKClickedFlag)
			{
         if (gProjectInfoPtr->lastClassForAddFieldSelection == 1)
				{
            if (!AddClassToProject (className))
																							return (FALSE);

            currentClass = gProjectInfoPtr->currentClass;

				}	// end "if (gProjectInfoPtr->lastClassForAddFieldSelection == 1)" 

         else	// gProjectInfoPtr->lastClassForAddFieldSelection > 1
            currentClass = gProjectInfoPtr->lastClassForAddFieldSelection - 2;

         gProjectInfoPtr->lastFieldTypeForAddFieldSelection = fieldType;

			}	// end "if (OKClicked)" 

		}	// end "if (pointType == kRectangleType || kPolygonType)" 

   else	// pointType == kClusterType || pointType == kMaskType
		{
      OKClickedFlag = TRUE;

		}	// end "else pointType == kClusterType || pointType == kMaskType" 

   if (OKClickedFlag)
		{
      currentStorageClass = gProjectInfoPtr->storageClass[currentClass];

				// Exit if we are at the limit of the number of fields for a		
				// class.																			

      if (gProjectInfoPtr->classNamesPtr[currentStorageClass].numberOfFields >=
																					kMaxFieldsPerClass)
         return (FALSE);

				// Get identifier name for new field.
				// Continue only if OK was clicked in dialog box otherwise exit routine.

      currentStorageField = gProjectInfoPtr->numberStorageFields;

				// Unlock some project handles if needed so as not to fragment memory.

      UnlockProjectMemory (&gProjectInfoPtr, 2, NULL);

				// Change size of handle for field names if needed

      bytesNeeded = ((SInt32)currentStorageField + 1) *
																	sizeof (ProjectFieldIdentifiers);

      gProjectInfoPtr->fieldIdentPtr = (HPFieldIdentifiersPtr)CheckHandleSize (
															&gProjectInfoPtr->fieldIdentifiersHandle,
															&continueFlag,
															&changedFlag,
															(SInt64)bytesNeeded);

				// Add points to project if rectangular or polygonal field.

      if (continueFlag && (pointType == kRectangleType || pointType == kPolygonType))
         continueFlag = AddPointsToProject (pointType, currentStorageField);

				// Make certain that project memory is locked.

      LockProjectMemory (NULL, 0, &gProjectInfoPtr);

				// Exit if we should not continue.  Memory is probably full.

      if (!continueFlag)
																							return (FALSE);

      classNamesPtr = gProjectInfoPtr->classNamesPtr;
      fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

				// Copy field identifier to field structure for list

      CopyPToP (fieldIdentPtr[currentStorageField].name, fieldIdentifierPtr);

				// Update project information variables

      savedLastField = classNamesPtr[currentStorageClass].lastFieldNumber;

      gProjectInfoPtr->changedFlag = TRUE;
      gProjectInfoPtr->numberStatisticsFields++;
      gProjectInfoPtr->numberStorageFields++;

      if (fieldType == kTrainingType)
			{
         gProjectInfoPtr->numberStatTrainFields++;
         gProjectInfoPtr->numberStorageStatFields++;

			}	// end "if (fieldType == kTrainingType)"

      else	// fieldType == kTestingType 
         gProjectInfoPtr->numberStatTestFields++;

      if (fieldType == kTrainingType &&
								classNamesPtr[currentStorageClass].numberOfTrainFields == 0)
         gProjectInfoPtr->numberStatTrainClasses++;

      gProjectInfoPtr->currentClass = currentClass;
      gProjectInfoPtr->currentField = currentStorageField;

				// Update class information variables.

      classNamesPtr[currentStorageClass].numberOfFields++;

      if (fieldType == kTrainingType)
			{
         classNamesPtr[currentStorageClass].numberOfTrainFields++;
         classNamesPtr[currentStorageClass].modifiedStatsFlag = FALSE;

			}	// end "if (fieldType == kTrainingType)"

      else	// fieldType == kTestingType 
         classNamesPtr[currentStorageClass].numberOfTestFields++;

      classNamesPtr[currentStorageClass].lastFieldNumber = currentStorageField;

				// Update field information variables.

				// Get the label point.

      if (pointType == kPolygonType)
         GetPolygonLabelPoint (&fieldIdentPtr[currentStorageField],
											gProjectInfoPtr->fieldPointsPtr,
											&fieldIdentPtr[currentStorageField].labelPoint);

      if (pointType >= kClusterType)
			{
         fieldIdentPtr[currentStorageField].numberOfPolygonPoints = 0;
         fieldIdentPtr[currentStorageField].firstPolygonPoint = -1;

			}	// end "if (pointType >= kClusterType)"

      fieldIdentPtr[currentStorageField].maskValue = 0;
      fieldIdentPtr[currentStorageField].classStorage = currentStorageClass;
      fieldIdentPtr[currentStorageField].fieldType = fieldType;
      fieldIdentPtr[currentStorageField].pointType = pointType;
      fieldIdentPtr[currentStorageField].nextField = -1;
      fieldIdentPtr[currentStorageField].trainingStatsNumber = -1;

      fieldIdentPtr[currentStorageField].numberPixels = 0;
      fieldIdentPtr[currentStorageField].numberPixelsUsedForStats = 0;
      if (fieldIdentPtr[currentStorageField].pointType < kClusterType)
         fieldIdentPtr[currentStorageField].numberPixels =
											GetSelectionNumberPixels (gProjectSelectionWindow);

      fieldIdentPtr[currentStorageField].loadedIntoClassStats = FALSE;
      fieldIdentPtr[currentStorageField].statsUpToDate = FALSE;

      if (classNamesPtr[currentStorageClass].numberOfFields == 1)
			{
         classNamesPtr[currentStorageClass].firstFieldNumber = currentStorageField;
         fieldIdentPtr[currentStorageField].previousField = -1;

			}	// end "if (classNamesPtr[currentStorageClass]...."

      else	// classNamesPtr[currentStorageClass].numberOfFields != 1 
			{
         fieldIdentPtr[savedLastField].nextField = currentStorageField;
         fieldIdentPtr[currentStorageField].previousField = savedLastField;

			}	// end "else classNamesPtr[currentStorageClass]..."

				// Set the statistics up-to-date flags depending on whether field
				// is for testing or training.

      if (fieldType == kTrainingType)
			{
         gProjectInfoPtr->statsUpToDate = FALSE;

         classNamesPtr[currentStorageClass].statsUpToDate = FALSE;
         classNamesPtr[currentStorageClass].looCovarianceValue = -1;
         classNamesPtr[currentStorageClass].numberTrainPixels +=
            								fieldIdentPtr[currentStorageField].numberPixels;
			/*
      	if (pointType == kMaskType)
      		{
      		fieldIdentPtr[currentStorageField].loadedIntoClassStats = TRUE;
      		fieldIdentPtr[currentStorageField].numberPixelsUsedForStats =
												fieldIdentPtr[currentStorageField].numberPixels;
				
      		}	// end "if (pointType == kMaskType)"
			*/
         fieldIdentPtr[currentStorageField].trainingStatsNumber =
										(SInt16)(gProjectInfoPtr->numberStorageStatFields - 1);

			}	// end "if (fieldType == kTrainingType)"

				// If this routine was call from 'Add New Field' in Statistics
				// window (implied by point type not being cluster type, then		
				// update some statistcs window parameters.								

      if (gProjectWindow != NULL &&
									(pointType == kRectangleType || pointType == kPolygonType))
			{
         GetPort (&savedPort);
         SetPortWindowPort (gProjectWindow);

					// Unhilite the "Add to List" control until the next field is selected

         MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);

					// Make certain that the "To Field" control is hilited

         //HiliteControl (gProjectInfoPtr->toFieldControlH, 0);

					// Force class pop up menu box to be redrawn

         InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);

			#if defined multispec_win
				((CComboBox*)gProjectWindow->GetDlgItem (IDC_ClassList))->
					SetCurSel ((int)(gProjectInfoPtr->lastClassForAddFieldSelection - 1));
			#endif	// defined multispec_win

					// Update menu items.

         gUpdateProcessorMenuItemsFlag = TRUE;
         gUpdateProjectMenuItemsFlag = TRUE;

					// Remove the area as a selection rectangle so that everything
					// will be clear for the next selection.  This will also 		
					// remove the outline of the selection area and the data in 	
					// the list so that the next field can be selected.				

         ClearSelectionArea (gProjectSelectionWindow);

					// Make sure that the coordinates in the project window are removed
					// from the list.	They may not have been if no project image selection 
					// window exists.											

         if (gProjectSelectionWindow == NULL)
            ClearNewFieldList ();

					// Outline the field in the project image windows if requested.

         OutlineFieldsInProjectWindows (kUpdateField, kDoNotClearArea);

         SetPort (savedPort);

			}	// end "if (gProjectWindow != NULL && ..."

		}	// end "if (OKClickedFlag)"

   return (OKClickedFlag);

}	// end "AddFieldToProject" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean AddPointsToProject
//
//	Software purpose:	This routine adds the boundary points for a 
//							rectangular or polygonal field to the open project.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			AddFieldToProject in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/24/1990
//	Revised By:			Larry L. Biehl			Date: 04/08/1998			

Boolean AddPointsToProject (
				SInt16								pointType,
				SInt16								currentStorageField)
	
{
	LongPoint							lineColPoint;

   HPFieldIdentifiersPtr			fieldIdentPtr;

   UInt32								bytesNeeded;

   UInt32								firstPolygonPoint,
											newStoragePoints,
											numberPoints,
											point,
											pointIndex;

   Boolean								changedFlag,
											continueFlag;


			// Initialize local variables.

   numberPoints = GetNumberListLines (gStatisticsListHandle);
   pointIndex = gProjectInfoPtr->numberStoragePoints;
   firstPolygonPoint = pointIndex;
   fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[currentStorageField];

			// Get the new total number of storage points.
			// Allow for the first two points for polygons to represent
			// the bounding rectangle.

   newStoragePoints = firstPolygonPoint + numberPoints;
   if (pointType == kPolygonType)
		{
      pointIndex += 2;
      newStoragePoints += 2;

		}	// end "if (pointType == kPolygonType)"

			// Change size of handle for field coordinates if needed.				

   bytesNeeded = (UInt32)newStoragePoints * sizeof (ProjectFieldPoints);
   gProjectInfoPtr->fieldPointsPtr = (HPFieldPointsPtr)CheckHandleSize (
														&gProjectInfoPtr->fieldCoordinatesHandle,
														&continueFlag,
														&changedFlag,
														(SInt64)bytesNeeded);
   if (!continueFlag)
      return (FALSE);

			// Load the polygon coordinates into the structure for the list.

   for (point = 0; point < numberPoints; point++)
		{
				// Get the image line and column number.									

      GetLineColFromList ((SInt16)point, &lineColPoint);
      gProjectInfoPtr->fieldPointsPtr[pointIndex].line = lineColPoint.v;
      gProjectInfoPtr->fieldPointsPtr[pointIndex].col = lineColPoint.h;

      pointIndex++;

		}	// end "for (point=0; point<numberPoints; point++)" 

			// Update project information.													

   gProjectInfoPtr->numberTotalPoints += (SInt16)numberPoints;
   if (pointType == kPolygonType)
      gProjectInfoPtr->numberTotalPoints += 2;

   gProjectInfoPtr->numberStoragePoints = (SInt16)pointIndex;

   fieldIdentPtr->numberOfPolygonPoints = (SInt16)numberPoints;
   fieldIdentPtr->firstPolygonPoint = (SInt16)firstPolygonPoint;

   return (TRUE);

}	// end "AddPointsToProject" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		AddPolyPointStatNewFieldW
//
//	Software purpose:	The purpose of this routine is to add the line-
//							column coordinates of points around the polygon into
//							the NewField mode stat window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			PolygonSelection.c in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1988
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

void AddPolyPointStatNewFieldW (
				LongPoint							selectedLineCol)
	
{
			// Local variables and structures												

	FileInfoPtr							fileInfoPtr;

   Handle								fileInfoHandle;

   SInt32								offset;


	#if defined multispec_win
		USES_CONVERSION;
	#endif

			// Exit routine if the Project window is not in Select Field mode.

   if (gStatisticsMode != kStatNewFieldPolyMode)
																									return;

			// Adjust for any difference between start line and column in		
			// base image file and start line and column in associated image	
			// file if needed.
			// Get pointer to file specification.  We do not need 	
			// to lock it here since no other routines are called.	

   fileInfoHandle = GetFileInfoHandle (gActiveImageWindowInfoH);
   fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);

   if (fileInfoPtr == NULL || gProjectInfoPtr == NULL)
																									return;

   offset = (SInt32)(fileInfoPtr->startColumn - gProjectInfoPtr->startColumn);
   selectedLineCol.h += offset;

   offset = (SInt32)(fileInfoPtr->startLine - gProjectInfoPtr->startLine);
   selectedLineCol.v += offset;

   // Save currently active window and make active window that				
   // Project window																	

	#if defined multispec_mac
		GrafPtr savedPort;
		GetPort (&savedPort);
		SetPortWindowPort (gProjectWindow);
	#endif

			// Turn drawing off until done with making changes							

   //LSetDrawingMode (false, gStatisticsListHandle);

			// Check if the new field coordinate list need to be cleared for the	
			// case of switching coordinate type in the middle of making			
			// a field selection																	

   if (gProjectInfoPtr->lastPointType != kPolygonType)
		{
      RemoveListCells ();
      gProjectInfoPtr->lastPointType = kPolygonType;

				// UnhHilite the "Add to List" control until at least three			
				// three have been selected.													

      MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);

		}	// end "if (gProjectInfoPtr->lastPointType != kPolygonType)"

	#if defined multispec_mac
		// Make certain that we have two columns	

		Cell cell;
		Point cellSize;

		SInt16 numberOfPoints,
			rowAdded;

		if (((ListPtr)* gStatisticsListHandle)->dataBounds.right == 1)
			LAddColumn (1, 1, gStatisticsListHandle);

		numberOfPoints = ((ListPtr)* gStatisticsListHandle)->dataBounds.bottom;

		if (numberOfPoints == 0)
			{
			cellSize.h = ((ListPtr)* gStatisticsListHandle)->rView.right -
												((ListPtr)* gStatisticsListHandle)->rView.left;
			cellSize.h /= 2;
			cellSize.v = ((ListPtr)* gStatisticsListHandle)->cellSize.v;
			LCellSize (cellSize, gStatisticsListHandle);

			}	// end "if (numberOfPoints == 0)"

				// Add row to list for new point													

		rowAdded = LAddRow (1, numberOfPoints, gStatisticsListHandle);

				// Put the coordinates for the selected point into the polygon list	

		cell.h = 0;
		cell.v = numberOfPoints;
		NumToString ((UInt32)selectedLineCol.v, gTextString);
		LSetCell (
				&gTextString[1], (short int)gTextString[0], cell, gStatisticsListHandle);

		cell.h = 1;
		NumToString ((UInt32)(selectedLineCol.h), gTextString);
		LSetCell (
				&gTextString[1], (short int)gTextString[0], cell, gStatisticsListHandle);

				// Hilite the "Add to List" control when at least three points have	
				// been selected.																		

		if (rowAdded >= 2 &&
					gProjectInfoPtr->numberStorageFields < kMaxNumberStatFields &&
						gProjectInfoPtr->numberStoragePoints + rowAdded + 1 <
																					kMaxNumberStatPoints)
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);

		else	// conditions not met;  field can not be added.   
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);

				// Turn list drawing mode back on.												

		//LSetDrawingMode (true, gStatisticsListHandle);

				// If list length is more than the 8 that can be displayed in the	
				// list, scroll the list up by one so that the last point will be		
				// displayed.  Also highlight the control if there is more than 8	
				//	items in the list.  This is done here because the vertical scroll	
				// bar does not get updated properly for this stat window mode when	
				// the list is longer than can be displayed.  I suspect it is 			
				// because at this point the project window is not active.			

		if (rowAdded > 7)
			{
			LScroll (0, 1, gStatisticsListHandle);
			MHiliteControl (gProjectWindow, (*gStatisticsListHandle)->vScroll, 0);

			}	// end "if (rowAdded > 7)" 

				// Invalidate the list rectangle so that it will be updated				

		//gGrayRect = ((ListPtr)*gStatisticsListHandle)->rView;
		//InvalRect (&gGrayRect);

				// Reset window back to saved window											

		SetPort (savedPort);
	#endif	// defined multispec_mac

	#if defined multispec_win 
		SInt16 index;

		index = (SInt16)gStatisticsListHandle->GetCount ();

				// Put the selection points coordinates into the polygon list	

		sprintf ((char*)gTextString,
						"%ld\t%ld",
						selectedLineCol.v,
						selectedLineCol.h);
		index = (SInt16)gStatisticsListHandle->
									InsertString (index, (LPCTSTR)A2T((char*)gTextString));

		if (index >= 2 &&
					gProjectInfoPtr->numberStorageFields < kMaxNumberStatFields &&
							gProjectInfoPtr->numberStoragePoints + index + 1 <
																					kMaxNumberStatPoints)
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);

		else	// conditions not met;  field can not be added.   
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);
	#endif	// defined multispec_win 

	#if defined multispec_wx
		SInt16 index;

		index = (SInt16)gStatisticsListHandle->GetCount ();

				// Put the selection points coordinates into the polygon list	

		sprintf ((char*)gTextString,
					"%d\t%d",
					selectedLineCol.v,
					selectedLineCol.h);
		//index = (SInt16)gStatisticsListHandle->InsertString (
		//																	index, (char*)gTextString);
		gStatisticsListHandle->AppendString ((char*)gTextString);

		if (index >= 2 &&
					gProjectInfoPtr->numberStorageFields < kMaxNumberStatFields &&
							gProjectInfoPtr->numberStoragePoints + index + 1 <
																					kMaxNumberStatPoints)
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);

		else	// conditions not met;  field can not be added.   
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);
	#endif
   
}	// end "AddPolyPointStatNewFieldW" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 AddSpecifiedStringToClassName
//
//	Software purpose:	This routine adds the input specified string to the 
//							output string within parentheses. The limit for the output
//							string is 31 characters.
//
//	Parameters in:		Base string to be modified (added to),
//							Base string length
//							Flag indicating whether the base string is to be added to
//							The string to be added to the base string
//
//	Parameters out:	Modified base string	
//
// Value Returned:	The new length of the ouput string.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/05/2000
//	Revised By:			Larry L. Biehl			Date: 10/05/2000			

SInt16 AddSpecifiedStringToClassName (
				UCharPtr								baseStringPtr,
				SInt16								stringLength,
				Boolean								addStringFlag,
				UCharPtr								inputStringPtr)
	
{

   if (addStringFlag && inputStringPtr != NULL)
		{
      sprintf ((char*)&baseStringPtr[stringLength + 1], " (");
      stringLength += 2;

      BlockMoveData (&inputStringPtr[1],
							(Ptr)&baseStringPtr[stringLength + 1],
							inputStringPtr[0]);
      stringLength += inputStringPtr[0];

      if (stringLength > 30)
         stringLength = 30;

      sprintf ((char*)&baseStringPtr[stringLength + 1], ")");
      stringLength += 1;

		}	// end "if (addStringFlag && inputStringPtr != NULL)"

   return (stringLength);

}	// end "AddSpecifiedStringToClassName"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckForDuplicateClassName
//
//	Software purpose:	The purpose of this routine is to determine if
//							the proposed input class name is the same as any of
//							the present class names.  If the name is a duplicate 
//							then true is returned, otherwise false is returned.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			EditClassFieldDialog in SEDtStat.cpp
//							DetermineIfClassesSameAsProjectClassNames in SMask.cpp
//							GetDefaultClassName in SStatistics.cpp
//							GetUniqueClassName in SStatistics.cpp
//							NewClassFieldDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/18/1989
//	Revised By:			Larry L. Biehl			Date: 06/14/2002	

SInt16 CheckForDuplicateClassName (
				SInt16								currentStorage,
				Str255*								namePtr)
	
{
   char*									charPtr;
   HPClassNamesPtr					classNamesPtr;

   SInt16								classStorage,
											numChars;

   UInt16								classIndex;

   SignedByte							handleStatus;


   // Make sure that the first character in the class name is non-blank.

   charPtr = (char*)namePtr;
   if (charPtr[0] == 0 || charPtr[1] == 0x20)
																								return (-1);

   if (gProjectInfoPtr->numberStatisticsClasses > 0)
		{
				// Make certain that the pointer to class names structure is good.

      classNamesPtr = (HPClassNamesPtr)GetHandleStatusAndPointer (
											gProjectInfoPtr->classNamesHandle, &handleStatus);

      for (classIndex = 0;
				classIndex < gProjectInfoPtr->numberStatisticsClasses;
					classIndex++)
			{
					// Get the class storage number.

         classStorage = gProjectInfoPtr->storageClass[classIndex];

					// Make the check as long as the name is not being compared against
					// itself.

         if (currentStorage != classStorage)
				{
						// Get number of characters in present class name to use to
						// make the comparison.													

            numChars = (SInt16)(classNamesPtr[classStorage].name[0] + 1);

						// Return true if the strings compare.

            if (CompareStringsNoCase ((UCharPtr)classNamesPtr[classStorage].name,
													(UCharPtr)namePtr,
													numChars) == 0)
																				return (classIndex+1);

				}	// end "if (currentStorage != classStorage)"

			}	// end "for (classIndex=0; ...

				// Unlock the class name structure handle if needed.

      MHSetState (gProjectInfoPtr->classNamesHandle, handleStatus);

		}	// end "if (numberOfClasses > 0)"

   return (0);

}	// end "CheckForDuplicateClassName" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckForDuplicateFieldName
//
//	Software purpose:	The purpose of this routine is to determine if
//							the proposed input field name is the same as any of
//							the present field names.  If the name is a duplicate 
//							then true is returned, otherwise false is returned.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/18/1989
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

SInt16 CheckForDuplicateFieldName (
				SInt16								currentField,
				Str255*								namePtr)
	
{
   char*									charPtr;
   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classStorage,
											field,
											numChars;

   UInt16 classIndex;

   SignedByte handleStatus1,
      handleStatus2;


   // Make sure that the first character in the class name is non-blank.

   charPtr = (char*)namePtr;
   if (charPtr[0] == 0 || charPtr[1] == 0x20)
																								return (1);

   if (gProjectInfoPtr->numberStatisticsFields > 0)
		{
      // Make certain that the pointer to class names and field			
      // names structures are good.													

      classNamesPtr = (HPClassNamesPtr)GetHandleStatusAndPointer (
											gProjectInfoPtr->classNamesHandle, &handleStatus1);

      fieldIdentPtr = (HPFieldIdentifiersPtr)GetHandleStatusAndPointer (
									gProjectInfoPtr->fieldIdentifiersHandle, &handleStatus2);

      for (classIndex = 0;
				classIndex < gProjectInfoPtr->numberStatisticsClasses;
				classIndex++)
			{
					// Get the class storage number.

         classStorage = gProjectInfoPtr->storageClass[classIndex];

					// Get the first field number for the class.  Then loop
					// through all the field numbers.

         field = classNamesPtr[classStorage].firstFieldNumber;
         while (field > -1)
				{
						// Make the check as long as the name is not being compared
						// against itself.

            if (currentField != field)
					{
							// Get number of characters in present class name to use
							// to make the comparison.

               numChars = (SInt16)(fieldIdentPtr[field].name[0] + 1);

							// Return true if the strings compare.

               if (CompareStringsNoCase ((UCharPtr)fieldIdentPtr[field].name,
														(UCharPtr)namePtr,
														numChars) == 0)
																								return (2);

					}	// end "if (currentField != field)"

            field = fieldIdentPtr[field].nextField;

				}	// end "while (field > -1)"

			}	// end "for (classIndex=0; ...

				// Unlock the field and class name structure handles if needed.

      MHSetState (gProjectInfoPtr->classNamesHandle, handleStatus1);

      MHSetState (gProjectInfoPtr->fieldIdentifiersHandle, handleStatus2);

		}	// end "if (gProjectInfoPtr->numberStatisticsFields > 0)"

   return (0);

}	// end "CheckForDuplicateFieldName" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClassListStatMode
//
//	Software purpose:	The purpose of this routine is to put the 
//							project window in the "Class List" mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			StatisticsWControlEvent  in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1988
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

void ClassListStatMode (void)

{
			// Local variables and structures

   SInt16 previousStatsMode;


			// Save previous stats mode.

   previousStatsMode = gProjectInfoPtr->statsWindowMode;

			// Invalidate the class pop up box and the class prompt box.

   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);
   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptClassBox);

			// Force drawing of statistics pop up and prompt boxes.

   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);
   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptCovarianceToUseBox);

			// Invalidate the field pop up box and the field prompt box.

   if (previousStatsMode == kCoordinateListMode)
		{
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpFieldBox);
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptFieldBox);

		}	// end "if (previousStatsMode == kCoordinateListMode)"

			// Invalidate the "add to project" button area to get rid of the
			// default button outline.

	#if defined multispec_mac
		if (!gOSXFlag && previousStatsMode == kSelectFieldMode)
			{
			GetControlBounds (gProjectInfoPtr->addToControlH, &gTempRect);
			InsetRect (&gTempRect, -4, -4);
			InvalWindowRect (gProjectWindow, &gTempRect);

			}	// end "if (!gOSXFlag && previousStatsMode == kSelectFieldMode)"
	#endif	// defined multispec_mac

			// Indicate the stats window now contains a class list.

   gProjectInfoPtr->statsWindowMode = kClassListMode;

			// Remove cells from previous list

   LSetDrawingMode (FALSE, gStatisticsListHandle);
   RemoveListCells ();

			// Load the class names into the list.

   LoadClassNamesIntoList (gStatisticsListHandle);

			// Turn list drawing mode back on

   LSetDrawingMode (TRUE, gStatisticsListHandle);

			// Invalidate the list rectangle so that it will be updated

	#if defined multispec_mac
		gTempRect = ((ListPtr)* gStatisticsListHandle)->rView;
		InvalWindowRect (gProjectWindow, &gTempRect);
	#endif	// defined multispec_mac

			// Force the project window mode title to be redrawn.

   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->projectWindowModeTitleBox);

			// Force the list title to be redrawn.

   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->listTitleBox);

	#if defined multispec_win
		(gProjectWindow->GetDlgItem (IDC_ListTitle))->
															SetWindowText ((LPCTSTR)_T("Classes"));

		(gProjectWindow->GetDlgItem (IDC_ProjectWindowMode))->
														SetWindowText ((LPCTSTR)_T("CLASS LIST"));

		((CMStatisticsForm*)gProjectWindow)->UpdateStatsTypeCombo (kClassListMode);

		((CMStatisticsForm*)gProjectWindow)->UpdateHistogramStatsCombo ();

		((CMStatisticsForm*)gProjectWindow)->UpdateListStatsCombo ();
	#endif	// defined multispec_win 

   #if defined multispec_wx 
		wxStaticText* list_title =
				(wxStaticText*)gProjectWindow->GetFrame()->FindWindow (IDC_ListTitle);
		list_title->SetLabel (wxT("Classes"));

		(gProjectWindow->GetFrame()->
						FindWindow (IDC_ProjectWindowMode))->SetLabel (wxT("CLASS LIST"));

		((CMStatisticsView*)gProjectWindow)->
												m_frame->UpdateStatsTypeCombo (kClassListMode);

		((CMStatisticsView*)gProjectWindow)->m_frame->UpdateHistogramStatsCombo ();

		((CMStatisticsView*)gProjectWindow)->m_frame->UpdateListStatsCombo ();
	#endif	// defined multispec_wx
   
			// Change the window title

	CtoPstring ((UCharPtr)"Project", gTextString);
	#if defined multispec_win || defined multispec_mac || defined multispec_mac_swift
		MSetWindowTitle ((StatisticsWindowPtr)gProjectWindow,
								(UChar*)gTextString);
	#else
		MSetWindowTitle ((CMImageView*)gProjectWindow,
								(UChar*)gTextString);
	#endif

			// Set processor mode to indicate that Statistics processor is
			// active in "Project Class List" mode.

   gStatisticsMode = kStatClassListMode;

			// Do activate events for "Project Class List" mode.

   DoStatWActivateEvent ();

			// Hilite the current active class if one is selected.

   if (gProjectInfoPtr->currentClass >= 0)
		{
		#if defined multispec_mac
			Cell cell;

			cell.h = 0;
			cell.v = gProjectInfoPtr->currentClass;
			LSetSelect (true, cell, gStatisticsListHandle);
		#endif	// defined multispec_mac

		#if defined multispec_win
			gStatisticsListHandle->SetCurSel (gProjectInfoPtr->currentClass);
		#endif	// defined multispec_win
			
		#if defined multispec_wx
			gStatisticsListHandle->SetSelection (gProjectInfoPtr->currentClass);
		#endif
		
		}	// end "if (gProjectInfoPtr->currentClass >= 0)"	

			// Scroll to the hilited list item.

   LAutoScroll (gStatisticsListHandle);

			// If project image window is displayed and previous stats mode was
			// the field list or coordinate list outline all requested fields.

   if (previousStatsMode > kClassListMode)
      OutlineFieldsInProjectWindows (gProjectInfoPtr->statsWindowMode, kDoClearArea);

}	// end "ClassListStatMode"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseProjectWindow
//
//	Software purpose:	The purpose of this routine is to close the
//							project window.  This involves releasing the
//							memory associated with handles and pointers.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClusterControl in SCluster.cpp
//							CloseTheProject in MMenus.c
//							StatisticsControl in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1988
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

void CloseProjectWindow (void)

{
   if (gProjectWindow != NULL)
		{
		#if defined multispec_mac    
			Handle windowInfoHandle;


			windowInfoHandle = (Handle)GetWRefCon (gProjectWindow);

			HUnlock (windowInfoHandle);

			HideWindow (gProjectWindow);

					// Get deactivate event for the window that was hidden and handle it.

			CheckSomeEvents (activMask);

			UnlockAndDispose (windowInfoHandle);

			if (gStatisticsListHandle)
				LDispose (gStatisticsListHandle);

			DisposeWindow (gProjectWindow);

					// Set project window pointer and statistics list handle			
					// globals to NULL to indicate that there is no memory				
					// associated with them any more.

			gProjectWindow = NULL;
		#endif	// defined multispec_mac 

		#if defined multispec_win
			CMStatisticsDoc* statisticsDocPtr =
										((CMStatisticsForm*)gProjectWindow)->GetDocument ();
			CMStatisticsFrame* statisticsFrameCPtr =
														statisticsDocPtr->GetStatisticsFrame ();
			statisticsFrameCPtr->MDIDestroy ();
		#endif	// defined multispec_win 	

		#if defined multispec_wx	
			if (gProjectWindow != NULL)
				{
				CMStatisticsDoc* statisticsDocPtr =
											((CMStatisticsView*)gProjectWindow)->GetDocument ();
				CMStatisticsFrame* statisticsFrameCPtr =
															statisticsDocPtr->GetStatisticsFrame ();
				if (statisticsFrameCPtr != NULL)
					statisticsFrameCPtr->Destroy ();   
				}	// end "if (gProjectWindow != NULL)"
		#endif 
			
		gStatisticsListHandle = NULL;

		}	// end "if (gProjectWindow != NULL)"

			// If project image window is displayed and previous stats mode was	
			// the field list or coordinate list outline all requested fields.	

   if (gProjectInfoPtr != NULL)
		{
      if (gProjectInfoPtr->statsWindowMode > kClassListMode)
         OutlineFieldsInProjectWindows (0, kDoClearArea);

      gProjectInfoPtr->statsWindowMode = 0;

				// Set project window control handles to NULL to indicate that they do
				// not exist anymore.						

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
		
		gProjectInfoPtr->projectNameControlRef = NULL;

		}	// end "if (gProjectInfoPtr != NULL)" 

			// Update global variables.														

   gWindowList[2] = NULL;
   gStatisticsMode = 0;

			// Force update of menu items.													

   gUpdateWindowsMenuItemsFlag = TRUE;

}	// end "CloseProjectWindow" 



#if defined multispec_mac 
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CreateProjectWControls
//
//	Software purpose:	This routine creates the controls for the
//							project window.
//
//	Functional description:
//										
// 		Create the controls for the project window							
// 		statistics type window is defined.											
// 		Select New Field: "Add To List", "->Classes", "Polygon Type".		
//
// 		Project:			 	"->Fields", "->Select",									
//									"Update Stats", "List",									
//									"Histogram".												
//		
// 		Class:			 	"->Coord.", "->Classes", "->Select". 				
//									"Update Stats", "Edit Class Name", "List".		
//									"Histogram".												
//
// 		Field:				"->Fields", "->Classes", "->Select",					
//									"Update Stats", "Edit Field Name", "List"			
//									"Histogram".												
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CreateProjectWindow
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1988
//	Revised By:			Larry L. Biehl			Date: 03/24/2017			

Boolean CreateProjectWControls (void)

{
   Rect									rect;

   ControlHandle						theRootControl;

   OSStatus								errorCode;

   SInt16								border,
											controlSpacing = 7,
											checkBoxHeight = 18,
											pushButtonHeight = 17,
											topStart,
											windowCenter,
											windowWidth;


   if (gAppearanceManagerFlag)
		{
				// Create the Root control

      errorCode = CreateRootControl (gProjectWindow, &theRootControl);
      if (errorCode != noErr)
																							return (FALSE);

		}	// end "if (gAppearanceManagerFlag)"

   SetProjectWindowBoxes (&border,
									&windowCenter,
									&windowWidth,
									&topStart,
									pushButtonHeight + controlSpacing);
									
	errorCode = CreateStaticTextControl (gProjectWindow, 
														&gProjectInfoPtr->projectNameBox,
														CFSTR ("Untitled Project"), 
														NULL, 
														&gProjectInfoPtr->projectNameControlRef);
	if (errorCode != noErr)
																							return (false);

			// Make the "Polygon Type" control												

   rect.left = border;
   rect.top = 57;
   rect.right = windowWidth - border;
   rect.bottom = rect.top + checkBoxHeight;

   gProjectInfoPtr->polygonTypeControlH = NewControl (
											gProjectWindow,
											&rect,
											"\pPolygon Enter",
											false,
											0,
											0,
											1,
											checkBoxProc + kControlUsesOwningWindowsFontVariant,
											kPolygonEnterControl);

   if (gProjectInfoPtr->polygonTypeControlH == NULL)
																							return (false);

			// Get the control rectangle for the "Edit Class/Field Name" control.

   rect.left = border;
   rect.top = rect.bottom + 2;
   rect.right = windowWidth - border;
   rect.bottom = rect.top + pushButtonHeight;

			// Make the "Edit Class/Field Name" control.

   gProjectInfoPtr->editNameControlH = NewControl (
											gProjectWindow,
											&rect,
											"\pEdit Name",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kEditNameControl);

   if (gProjectInfoPtr->editNameControlH == NULL)
																							return (false);

			// Make the "Histogram stats" control.

   rect.left = border;
   rect.top = rect.bottom + pushButtonHeight + controlSpacing + 4;
   rect.right = windowWidth - border;
   rect.bottom = rect.top + pushButtonHeight;

   gProjectInfoPtr->histogramControlH = NewControl (
											gProjectWindow,
											&rect,
											"\pHistogram",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kStatHistogramControl);

   if (gProjectInfoPtr->histogramControlH == NULL)
																							return (false);

			// Get the control rectangle for the "List stats" control.

   rect.left = border;
   rect.top = rect.bottom + controlSpacing;
   rect.right = windowWidth - border;
   rect.bottom = rect.top + pushButtonHeight;

			// Make the "List stats" control.

   gProjectInfoPtr->listControlH = NewControl (
											gProjectWindow,
											&rect,
											"\pList",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kStatPrintControl);

   if (gProjectInfoPtr->listControlH == NULL)
																							return (false);

			// Get the control rectangle for the "Add To List" control and
			// and the "Update Stats" control

   rect.left = border;
   rect.top = rect.bottom + controlSpacing;
   rect.right = windowWidth - border;
   rect.bottom = rect.top + pushButtonHeight;

			// Make the "Add To List" control

   gProjectInfoPtr->addToControlH = NewControl (
											gProjectWindow,
											&rect,
											"\pAdd To List...",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kAddToListControl);

   if (gProjectInfoPtr->addToControlH == NULL)
																							return (false);

			// Make the "Update Stats" control

   gProjectInfoPtr->updateControlH = NewControl (
											gProjectWindow,
											&rect,
											"\pUpdate Stats",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kUpdateStatsControl);

   if (gProjectInfoPtr->updateControlH == NULL)
																							return (false);

			// Make the ">Classes" list control.

   rect.left = border;
   rect.top = rect.bottom + controlSpacing + 2;
   rect.right = windowCenter - 5;
   rect.bottom = rect.top + pushButtonHeight;

   gProjectInfoPtr->toProjectControlH = NewControl (
											gProjectWindow,
											&rect,
											"\p>Classes",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kToProjectControl);

   if (gProjectInfoPtr->toProjectControlH == NULL)
																							return (false);

			// Make the ">Fields"  list control.

   rect.left = windowCenter + 5;
   rect.right = windowWidth - border;

   gProjectInfoPtr->toClassControlH = NewControl (
											gProjectWindow,
											&rect,
											"\p>Fields",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kToClassControl);

   if (gProjectInfoPtr->toClassControlH == NULL)
																							return (false);

			// Make the ">Select" list control

   rect.left = border;
   rect.top = rect.bottom + controlSpacing;
   rect.right = windowCenter - 5;
   rect.bottom = rect.top + pushButtonHeight;

   gProjectInfoPtr->newFieldControlH = NewControl (
											gProjectWindow,
											&rect,
											"\p>Select",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kNewFieldControl);

   if (gProjectInfoPtr->newFieldControlH == NULL)
																							return (false);

			// Make the ">Coordinates" control

   rect.left = windowCenter + 5;
   rect.right = windowWidth - border;

   gProjectInfoPtr->toFieldControlH = NewControl (
											gProjectWindow,
											&rect,
											"\p>Coord.",
											false,
											0,
											0,
											1,
											pushButProc + kControlUsesOwningWindowsFontVariant,
											kToFieldControl);

   if (gProjectInfoPtr->toFieldControlH == NULL)
																							return (false);

   return (true);

}	// end "CreateProjectWControls"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		CreateProjectWindow
//
//	Software purpose:	The purpose of this routine is to create a
//							project window with a list within it.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DoOpenProjectFile in MMenus.c
//							StatisticsControl in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1988
//	Revised By:			Larry L. Biehl			Date: 09/08/2006	

void CreateProjectWindow (void)

{
	#if defined multispec_mac
			// Declare local variables 

		BitMap								bitMap;

		Rect									dataBounds,
												rect,
												rView;

		Cell									cell;

		WindowInfoPtr						windowInfoPtr;
		WindowPtr							front;

		Handle								windowInfoHandle;

		SInt16								cellHeight,
												newListHeight;

		Boolean								continueFlag;


		//rect = qd.screenBits.bounds;
		GetQDGlobalsScreenBits (&bitMap);
		rect = bitMap.bounds;

		rect.top = 46;
		rect.right = rect.right - 8;
		rect.left = rect.right - kStatisticsWindowWidth - kSBarWidth;
		rect.bottom = rect.top + gStatisticsWindowControlHeight + gStatisticsWindHeight;

		front = (WindowPtr)- 1;

		gProjectWindow = NewCWindow (NULL,
												&rect,
												"\pProject",
												TRUE,
												noGrowDocProc,
												front,
												TRUE,
												0L);

			// Exit routine if not enough memory.											

   if (gProjectWindow == NULL)
																									return;

			// Initialize variables.															

   windowInfoHandle = NULL;

			// Create the controls for the project window.							

   continueFlag = CreateProjectWControls ();

			// Allocate WindowInfo record to store handle to structures and		
			// variables which provide additional information about the 			
			// project window																	

   if (continueFlag)
      windowInfoHandle = (Handle)MNewHandle (sizeof (WindowInfo));

   continueFlag = (windowInfoHandle != NULL);

   if (continueFlag)
		{
				// Everything is okay for now. Set some window parameters.

      SetPortWindowPort (gProjectWindow);

				// Get the font number to be used for the project window.  		
				// Assume the geneva font for now.		

      if (gFontMgrPresentFlag)
         gStatWindowTextFont = FMGetFontFamilyFromName ("\pgeneva");

      else	// !gCarbonFlag
         GetFNum ("\pgeneva", &gStatWindowTextFont);

      if (gStatWindowTextFont == 0)
         gStatWindowTextFont = 3;
      TextFont (gStatWindowTextFont);

				// Set the window font style and size										

      TextFace (bold);
      TextSize (10);

				// Create the list for the project window.	

      SetRect (&rView,
					0,
					gStatisticsWindowControlHeight,
					kStatisticsWindowWidth,
					gStatisticsWindowControlHeight + gStatisticsWindHeight);

      SetRect (&dataBounds, 0, 0, 1, 0);
      cell.h = 0;
      cell.v = 0;
      gStatisticsListHandle = LNew (&rView,
												&dataBounds,
												cell,
												0,
												gProjectWindow,
												true,
												false,
												false,
												true);

      continueFlag = (gStatisticsListHandle != NULL);

		}	// end "if (continueFlag)" 

			// Finish initialization if everything is okay.								

   if (continueFlag)
		{
				// Make certain that the list view is just tall enough for an		
				// integer number of lines.													

      cellHeight = ((ListPtr)* gStatisticsListHandle)->cellSize.v;
      GetWindowPortBounds (gProjectWindow, &gTempRect);
      newListHeight = ((gTempRect.bottom - gStatisticsWindowControlHeight) /
																				cellHeight) * cellHeight;
      LSize (kStatisticsWindowWidth, newListHeight, gStatisticsListHandle);

				// Make sure that the window height is okay in relation to the list height.

      SizeWindow (gProjectWindow,
						rect.right - rect.left + 1,
						gStatisticsWindowControlHeight + newListHeight,
						TRUE);

				// Force selecting in the list to allow only one selection at a time.																				

      (*gStatisticsListHandle)->selFlags = lOnlyOne;

				// Everything is okay to this point.
				// Initialize window information structure.	

      SetWRefCon (gProjectWindow, (SInt32)windowInfoHandle);

      HLock (windowInfoHandle);
      windowInfoPtr = (WindowInfoPtr)* windowInfoHandle;

				// Initialize the window information record.								

      InitializeWindowInfoStructure (windowInfoHandle,
													kNotPointer,
													NULL,
													kNoImageType,
													kStatisticsWindowType);

				// Initialize items specific for project window information structure.

      windowInfoPtr->windowRecordHandle = gProjectWindowHandle;
      windowInfoPtr->grafPortType = kGrafType;
      windowInfoPtr->windowMenuItem = kStatisticsWItem;

      HUnlock (windowInfoHandle);

				// Reset the application window global variables. 						

      gTheActiveWindow = gProjectWindow;
      gActiveWindowInfoH = windowInfoHandle;

				// Force changes in the window menu.										

      gUpdateWindowsMenuItemsFlag = true;

		}	// end "!continueFlag" 

   else	// !continueFlag 
		{
		DisposeWindow (gProjectWindow);

      gProjectWindow = NULL;
      UnlockAndDispose (windowInfoHandle);

		}	// end "else !continueFlag" 
	#endif	// defined multispec_mac 

	#if defined multispec_win
				// Open a project window

		gProjectInfoPtr->toFieldControlH = (Handle)IDC_Field;
		gProjectInfoPtr->updateControlH = (Handle)IDC_Update;
		gProjectInfoPtr->newFieldControlH = (Handle)IDC_Select;
		gProjectInfoPtr->listControlH = (Handle)IDC_ListStatsCombo;
		gProjectInfoPtr->editNameControlH = (Handle)IDC_EditName;
		gProjectInfoPtr->histogramControlH = (Handle)IDC_HistogramStatsCombo;
		gProjectInfoPtr->addToControlH = (Handle)IDC_AddToList;
		gProjectInfoPtr->polygonTypeControlH = (Handle)IDC_Polygon;
		gProjectInfoPtr->toProjectControlH = (Handle)IDC_Project;
		gProjectInfoPtr->toClassControlH = (Handle)IDC_Class;

		CMultiDocTemplate* statisticsDocTemplatePtr =
								((CMultiSpecApp*)AfxGetApp ())->GetStatisticsDocTemplate ();

		if (statisticsDocTemplatePtr == NULL ||
											!statisticsDocTemplatePtr->OpenDocumentFile (NULL))
			gProjectWindow = NULL;
	#endif	// defined multispec_win 

	#if defined multispec_wx
				// Open a project window

		gProjectInfoPtr->toFieldControlH = (Handle)IDC_Field;
		gProjectInfoPtr->updateControlH = (Handle)IDC_Update;
		gProjectInfoPtr->newFieldControlH = (Handle)IDC_Select;
		gProjectInfoPtr->listControlH = (Handle)IDC_ListStatsCombo;
		gProjectInfoPtr->editNameControlH = (Handle)IDC_EditName;
		gProjectInfoPtr->histogramControlH = (Handle)IDC_HistogramStatsCombo;
		gProjectInfoPtr->addToControlH = (Handle)IDC_AddToList;
		gProjectInfoPtr->polygonTypeControlH = (Handle)IDC_Polygon;
		gProjectInfoPtr->toProjectControlH = (Handle)IDC_Project;
		gProjectInfoPtr->toClassControlH = (Handle)IDC_Class;
		
		((CMultiSpecApp*)wxTheApp)->ActivateProjectView ();
		((CMStatisticsView*)gProjectWindow)->GetFrame()->Update ();
	#endif
   
			// Set the project window title	

   SetProjectWTitle ();

			// Load the window list with the project window.					

   gWindowList[2] = gProjectWindow;

			// Set up the proper window mode.

   if (gProjectWindow != NULL)
		{
      if (gProjectInfoPtr->numberStatTrainClasses == 0)
         NewFieldStatMode ();

      else	// gProjectInfoPtr->numberStatTrainClasses > 0
         ClassListStatMode ();

		}	// end "if (gProjectWindow != NULL)"

}	// end "CreateProjectWindow" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DeletePolyPointStatNewFieldW
//
//	Software purpose:	The purpose of this routine is to delete the last
//							line-column coordinate that describes the polygon in
//							the NewField mode stat window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/16/1993
//	Revised By:			Larry L. Biehl			Date: 09/29/1998

void DeletePolyPointStatNewFieldW (
				SInt16								entryLine)

{
			// Local variables and structures

   GrafPtr								savedPort;


			// Save currently active window and make active window that Project window

   GetPort (&savedPort);
   SetPortWindowPort (gProjectWindow);

			// Delete row for the point to be removed.

   LDelRow (1, entryLine, gStatisticsListHandle);

			// Unhilite the "Add to List" control if there fewer than three points have
			// been selected.

	#if defined multispec_mac
		if (((ListPtr)* gStatisticsListHandle)->dataBounds.bottom < 3)
	#endif	// defined multispec_mac
	#if defined multispec_win || multispec_wx 
		if (gStatisticsListHandle->GetCount () < 3)
	#endif	// defined multispec_win     
			MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);

			// Reset window back to saved window

   SetPort (savedPort);

}	// end "DeletePolyPointStatNewFieldW" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoProjectWindowGrow
//
//	Software purpose:	This routine handles mouse down events in the
//							grow box for project windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			main
//
//	Coded By:			Larry L. Biehl			Date: 12/14/2000
//	Revised By:			Larry L. Biehl			Date: 12/14/2000		

void DoProjectWindowGrow (
				WindowPtr							window)
	
{
   SInt32								newSize;


			// Get the current maximum rectangular display area.

   Rect growArea;
   GetDisplayRegionRectangle (&growArea);

   growArea.right = kStatisticsWindowWidth + kSBarWidth;
   growArea.bottom -= growArea.top;
   growArea.left = kStatisticsWindowWidth + kSBarWidth;
   growArea.top = 117 + 216;

   newSize = GrowWindow (window, gEventRecord.where, &growArea);
   SizeWindow (window, LoWord (newSize), HiWord (newSize), TRUE);
   InvalWindowRect (window, GetWindowPortBounds (window, &gTempRect));
   EraseRect (&gTempRect);

}	// end "DoProjectWindowGrow" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				DoStatWActivateEvent
//
//	Software purpose:	This routine handles the activate events for
//							project windows.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
// Revised By:			Ravi S. Budruk			Date: 08/16/1988	
//	Revised By:			Larry L. Biehl			Date: 02/15/1989			

void DoStatWActivateEvent (void)

{
	#if defined multispec_mac
				// Reset the font style. It gets changed in OSX after opening a dialog
				// box with edit text items.

		TextFont (gStatWindowTextFont);
		TextFace (bold);
		TextSize (10);
	#endif	// defined multispec_mac

			// Activate appropriation controls for the project window mode.

   ActivateStatControls ();

			// Do activate for list in project window.

	#if defined multispec_mac
		LActivate (TRUE, gStatisticsListHandle); 
	#endif	// defined multispec_mac

			// Force update of the "Edit" and "Window" menu.

   gUpdateEditMenuItemsFlag = TRUE;
   gUpdateWindowsMenuItemsFlag = TRUE;

}	// end "DoStatWActivateEvent"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoStatWUpdateEvent
//
//	Software purpose:	This routine handles the update events for the
//							statistics type window.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/06/1988
//	Revised By:			Larry L. Biehl			Date: 01/24/1989			

void DoStatWUpdateEvent (
				WindowPtr							window)

{
   BeginUpdate (window);
   EraseRect (GetWindowPortBounds (window, &gTempRect));
   LUpdate (GetPortVisibleRegion (
							GetWindowPort (window), gTempRegion1), gStatisticsListHandle);
   SetEmptyRgn (gTempRegion1);
   UpdateStatisticsWindow ();
   EndUpdate (window);

			// Draw the controls.																

   ClipRect (GetWindowPortBounds (window, &gTempRect));
   DrawControls (window);

}	// end "DoStatWUpdateEvent"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DrawClassPopUp
//
//	Software purpose:	The purpose of this routine is to draw the popup
//							menu prompt and box in the statistics-new field
//							mode window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			NewFieldStatMode
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1988
//	Revised By:			Larry L. Biehl			Date: 12/14/2000	

void DrawClassPopUp (void)

{
			// Declare local variables and structures

   Rect									popUpBox;


			// Draw the prompt string and current class name

   DrawClassPrompt ();

			// Draw the pop up box and currently selected class name
			// Subtract one pixel from right and bottom to allow for drop shadow with
			// the pop up box rectangle

   popUpBox = gProjectInfoPtr->popUpClassBox;

   DrawDropShadowBox (popUpBox);

   DrawDropDownTriangle2 (&popUpBox);

}	// end "DrawClassPopUp" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawClassPrompt
//
//	Software purpose:	The purpose of this routine is to draw the class
//							prompt for some of the statistics mode windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DrawClassPopUp
// 						UpdateStatisticsWindow
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1988
//	Revised By:			Larry L. Biehl			Date: 08/19/2011	

void DrawClassPrompt (void)

{
			// Declare local variables and structures

   SInt16								classStorage,
											currentClass,
											leftSpace = 2,
											bottomSpace = 4;


			// Draw the prompt (title)string

   MoveTo (gProjectInfoPtr->promptClassBox.left + leftSpace,
				gProjectInfoPtr->promptClassBox.bottom - bottomSpace);
   DrawString ("\pClass:");

			// Get the current class

   if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode)
		{
      currentClass = -1;
      if (gProjectInfoPtr->lastClassForAddFieldSelection > 1)
         currentClass = gProjectInfoPtr->lastClassForAddFieldSelection - 2;

		}	// end "if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode)"

   else	// gProjectInfoPtr->statsWindowMode != kSelectFieldMode
      currentClass = gProjectInfoPtr->currentClass;

			// Draw the current class name
			// First, get the class storage number.

   if (currentClass >= 0)
		{
      classStorage = gProjectInfoPtr->storageClass[currentClass];
      CheckStringLength (gProjectInfoPtr->popUpClassBox.right - 13 -
														gProjectInfoPtr->popUpClassBox.left,
									(Str255*)gProjectInfoPtr->classNamesPtr[classStorage].name,
									(Str255*)gTextString);

		}	// end "if (currentClass >= 0)" 

   else	// currentClass < 0
      CopyPToP (gTextString, (UCharPtr)"\p'New'");

   MoveTo (gProjectInfoPtr->popUpClassBox.left + leftSpace + 1,
							gProjectInfoPtr->popUpClassBox.bottom - bottomSpace - 1);
   DrawString (gTextString);

}	// end "DrawClassPrompt" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				pascal void DrawDialogClassPopUp2
//
//	Software purpose:	The purpose of this routine is to draw the class
//							prompt and new class - class
//							pop up menu.  This routine will also draw a drop
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
//	Coded By:			Larry L. Biehl			Date: 05/15/1991
//	Revised By:			Larry L. Biehl			Date: 02/20/2003	

pascal void DrawDialogClassPopUp2 (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.

   DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpClassMenu,
								gProjectInfoPtr->lastClassForAddFieldSelection,
								true);

}	// end "DrawDialogClassPopUp2" 
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DrawFieldPrompt
//
//	Software purpose:	The purpose of this routine is to draw the field
//							prompt for some of the statistics mode windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1988
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void DrawFieldPrompt (void)

{
			// Declare local variables and structures

   SInt16								fieldNumber,
											leftSpace = 2,
											bottomSpace = 4;


			// Draw the prompt (title)string

   MoveTo (gProjectInfoPtr->promptFieldBox.left + leftSpace,
				gProjectInfoPtr->promptFieldBox.bottom - bottomSpace);
   DrawString ("\pField:");

			// Draw the current field name with the field type identifier.

   fieldNumber = gProjectInfoPtr->currentField;

   if (fieldNumber >= 0)
		{
      GetFieldNameWithType (fieldNumber, gTextString);
      CheckStringLength (gProjectInfoPtr->popUpFieldBox.right -
														gProjectInfoPtr->popUpFieldBox.left,
									(Str255*)gTextString, 
									(Str255*)gTextString2);

      MoveTo (gProjectInfoPtr->popUpFieldBox.left + leftSpace,
					gProjectInfoPtr->popUpFieldBox.bottom - bottomSpace - 1);
      DrawString (gTextString2);

		}	// end "if (fieldNumber >= 0)"

}	// end "DrawFieldPrompt"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawOutlineColorPopUp
//
//	Software purpose:	The purpose of this routine is to draw the outline color
//							selection for the outline color pop up menu.  This routine will 
//							also draw a drop shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/31/1998
//	Revised By:			Larry L. Biehl			Date: 03/31/1998	

pascal void DrawOutlineColorPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
   // Use the generic pop up drawing routine.									

   DrawPopUpMenuItem (dialogPtr, 
								itemNumber,
								gPopUpTemporaryMenu, 
								gOutlineColorSelection, 
								true);

}	// end "DrawOutlineColorPopUp"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawSelectTestImageMaskPopUp
//
//	Software purpose:	The purpose of this routine is to draw the image mask
//							selection for the select image mask popup menu.  This routine will 
//							also draw a drop shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1998
//	Revised By:			Larry L. Biehl			Date: 12/08/1998	

pascal void DrawSelectTestImageMaskPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
   // Use the generic pop up drawing routine.									

   DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpSelectTestMaskImageMenu,
								gMaskTestImageSelection,
								true);

}	// end "DrawSelectTestImageMaskPopUp"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawSelectTrainImageMaskPopUp
//
//	Software purpose:	The purpose of this routine is to draw the image mask
//							selection for the select image mask popup menu.  This routine will 
//							also draw a drop shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1998
//	Revised By:			Larry L. Biehl			Date: 12/08/1998	

pascal void DrawSelectTrainImageMaskPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.

   DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpSelectTrainMaskImageMenu,
								gMaskTrainImageSelection,
								true);

}	// end "DrawSelectTrainImageMaskPopUp"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		DrawStatPopUp
//
//	Software purpose:	The purpose of this routine is to draw the popup
//							menu prompt and box in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			NewFieldStatMode
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1997
//	Revised By:			Larry L. Biehl			Date: 12/14/2000	

void DrawStatPopUp (
				SInt16								statsWindowMode)

{
			// Declare local variables and structures									

   Rect									popUpBox;

   SInt16								menuItem;


			// Draw the prompt string and current covariance stats to be used.

   if (statsWindowMode == kClassListMode)
      menuItem = GetProjectStatisticsTypeMenuItem ();

   else	// statsWindowMode == kFieldListMode
      menuItem = GetClassStatisticsTypeMenuItem ();

   DrawStatPrompt (menuItem);

			// Draw the pop up box and currently selected class name				
			// Subtract one pixel from right and bottom to allow for drop		
			// shadow with the pop up box rectangle									

   popUpBox = gProjectInfoPtr->popUpCovarianceToUseBox;

   DrawDropShadowBox (popUpBox);

   DrawDropDownTriangle2 (&popUpBox);

}	// end "DrawStatPopUp"			



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DrawStatPrompt
//
//	Software purpose:	The purpose of this routine is to draw the class
//							prompt for some of the statistics mode windows.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			DrawClassPopUp
// 						UpdateStatisticsWindow
//
//	Coded By:			Larry L. Biehl			Date: 12/09/1997
//	Revised By:			Larry L. Biehl			Date: 12/14/2000	

void DrawStatPrompt (
				SInt16								menuItem)

{
			// Declare local variables and structures										

   SInt16								bottomSpace = 4,
											leftSpace = 2;


			// Draw the prompt (title)string												

   MoveTo (gProjectInfoPtr->promptCovarianceToUseBox.left + leftSpace,
				gProjectInfoPtr->promptCovarianceToUseBox.bottom - bottomSpace);
   DrawString ("\pStats:");

			// Draw the covariance statistics to be used.								

   GetProjectStatisticsTypeText (gMenuItemString, menuItem);

   MoveTo (gProjectInfoPtr->popUpCovarianceToUseBox.left + leftSpace + 1,
				gProjectInfoPtr->popUpCovarianceToUseBox.bottom - bottomSpace - 1);

   DrawString (gMenuItemString);

}	// end "DrawStatPrompt" 
#endif	// defined multispec_mac  


//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				FieldListStatMode
//
//	Software purpose:	The purpose of this routine is to put the 
//							project window in the "Field List" mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			MouseDnEvents  in MMultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1988
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

void FieldListStatMode (
				SInt16								classNumber)

{
			// Local variables and structures												

	Cell									cell;

   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classStorage,
											currentClassField,
											field,
											numberOfFields,
											previousStatsMode,
											row;


			// Execute routine if input class number is within proper range		

   if (classNumber < (SInt16)gProjectInfoPtr->numberStatisticsClasses &&
																						classNumber >= 0)
		{
      classNamesPtr = gProjectInfoPtr->classNamesPtr;
      fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

				// Force drawing of class prompt and class name.						

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptClassBox);

				// Invalidate the field pop up box and the field prompt box.		

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpFieldBox);
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptFieldBox);

				// Force drawing of statistics pop up and prompt boxes.

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptCovarianceToUseBox);

				// Indicate the stats window now contains a field list 				

      previousStatsMode = gProjectInfoPtr->statsWindowMode;
      gProjectInfoPtr->statsWindowMode = kFieldListMode;

				// Invalidate the "add to project" button area to get rid of the	
				// default button outline.														

		#if defined multispec_mac  
			if (!gOSXFlag && previousStatsMode == kSelectFieldMode)
				{
				GetControlBounds (gProjectInfoPtr->addToControlH, &gTempRect);
				InsetRect (&gTempRect, -4, -4);
				InvalWindowRect (gProjectWindow, &gTempRect);

				}	// end "if (previousStatsMode == kSelectFieldMode)"
		#endif	// defined multispec_mac   

				// Make the input class number the current class						

      gProjectInfoPtr->currentClass = classNumber;

				// Remove cells from previous list											

      LSetDrawingMode (FALSE, gStatisticsListHandle);
      RemoveListCells ();

				// Change the list to reflect the fields for the selected class.	

				// First, get the class storage number.									

      classStorage = gProjectInfoPtr->storageClass[classNumber];

      numberOfFields = classNamesPtr[classStorage].numberOfTrainFields +
												classNamesPtr[classStorage].numberOfTestFields;

      if (numberOfFields > 0)
			{
         row = LAddRow (numberOfFields, 0, gStatisticsListHandle);
         cell.h = 0;
         cell.v = 0;

         field = classNamesPtr[classStorage].firstFieldNumber;
         while (field != -1)
				{
						// Add name to the field list.  Include identifier at beginning to
						// indicate the field type.

            GetFieldNameWithType (field, gTextString);

            LSetCell ((char*)&gTextString[1],
								(short int)gTextString[0],
								cell,
								gStatisticsListHandle);
            field = fieldIdentPtr[field].nextField;
            cell.v++;

				}	// end "while (field != -1)" 

			}	// end "if (numberOfFields > 0)" 

				// Turn list drawing mode back on											

      LSetDrawingMode (TRUE, gStatisticsListHandle);

				// Invalidate the list rectangle so that it will be updated			

		#if defined multispec_mac		
			gTempRect = ((ListPtr)*gStatisticsListHandle)->rView;
			InvalWindowRect (gProjectWindow, &gTempRect);
		#endif	// defined multispec_mac

				// Force the project window mode title to be redrawn.	

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->projectWindowModeTitleBox);

				// Force the list title to be redrawn.										

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->listTitleBox);

		#if defined multispec_win 
			(gProjectWindow->GetDlgItem (IDC_ListTitle))->
															SetWindowText ((LPCTSTR)_T("Fields"));

			(gProjectWindow->GetDlgItem (IDC_ProjectWindowMode))->
														SetWindowText ((LPCTSTR)_T("FIELD LIST"));
		#endif	// defined multispec_win

		#if defined multispec_wx
			(gProjectWindow->GetFrame()->
												FindWindow (IDC_ListTitle))->SetLabel ("Fields");

			(gProjectWindow->GetFrame()->FindWindow (IDC_ProjectWindowMode))->
																				SetLabel ("FIELD LIST");
			
					// Load the current class name
		
			 CtoPstring (gProjectInfoPtr->classNamesPtr[classStorage].name, gTextString);
			 SetStatControlTitle ((Handle)IDC_ClassName, (char*)&gTextString[1]);
		#endif	// defined multispec_wx
      
				// Change the window title														

      //CtoPstring ("Class", (char*)gTextString);     
      //MSetWindowTitle ((StatisticsWindowPtr)gProjectWindow, (UChar*)gTextString);

				// Set processor mode to indicate that Statistics processor is 	
				// active in "Class Fields List" mode. 									

      gStatisticsMode = kStatFieldListMode;

				// Get the current field class list index number.

      currentClassField = GetCurrentClassField (gProjectInfoPtr->currentClass,
																gProjectInfoPtr->currentField);

      if (gProjectInfoPtr->currentField >= 0 && currentClassField < 0)
         gProjectInfoPtr->currentField = -1;

				// Hilite the current active field if one is selected.

      if (currentClassField >= 0)
			{
			#if defined multispec_mac
				Cell cell;

				cell.h = 0;
				cell.v = currentClassField;
				LSetSelect (true, cell, gStatisticsListHandle);
			#endif	// defined multispec_mac 

			#if defined multispec_win                              
				gStatisticsListHandle->SetCurSel (currentClassField);
			#endif	// defined multispec_win

			#if defined multispec_wx                         
				gStatisticsListHandle->SetSelection (currentClassField);
			#endif	// defined multispec_wx 
         
			}	// end "if (gProjectInfoPtr->currentClass >= 0)"	

				// Scroll to the hilited list item.											

      LAutoScroll (gStatisticsListHandle);

				// Do activate events for "Class Fields List" mode.					

      DoStatWActivateEvent ();

				// If project image window is displayed and fields are to be		
				// outlined in the image window, force an update event so that		
				// the requested fields are outlined for the current class.			

      if (previousStatsMode <= kClassListMode)
         ForceFieldOutlineUpdate (FALSE);

      else	// previousStatsMode > kClassListMode 
         OutlineFieldsInProjectWindows (gProjectInfoPtr->statsWindowMode, kDoClearArea);

		}	// end "if (classNumber<=gProjectInfoPtr->numberStatistics..." 
		
	else if (classNumber < 0)
      RemoveListCells ();

}	// end "FieldListStatMode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ForceStatisticsCodeResourceLoad
//
//	Software purpose:	The purpose of this is to force the 'Statistics' code
//							resource to be loaded into memory.  It is called by
//							routines in the 'Project' code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/05/1990
//	Revised By:			Larry L. Biehl			Date: 04/06/1990

void ForceStatisticsCodeResourceLoad (void)

{
			// If spare memory had to be used to load code resources, then exit
			// routine.

   if (gMemoryTypeNeeded < 0)
																									return;

			// Code resources loaded okay, so set flag back for non-Code resources.

   gMemoryTypeNeeded = 0;

}	// end "ForceStatisticsCodeResourceLoad"                               



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetControlValue
//
//	Software purpose:	The purpose of this routine is to set the value for the
//							specified control to the requested value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassListStatMode
//
//	Coded By:			Larry L. Biehl			Date: 02/08/1996
//	Revised By:			Larry L. Biehl			Date: 02/08/1996	

SInt16 GetControlValue (
				WindowPtr							windowPtr,
				ControlHandle						controlHandle)

{
			// Declare local variables and structures

	#if defined multispec_mac
		return (GetControlValue (controlHandle));
	#endif	// defined multispec_mac

	#if defined multispec_mac_swift
		return (0);
	#endif	// defined multispec_mac_swift

	#if defined multispec_win   
		int	nID = (int)controlHandle;
		return ((SInt16)((CButton*)windowPtr->GetDlgItem (nID))->GetCheck ());
	#endif	// defined multispec_win  

	#if defined multispec_wx
		SInt64 windowid64 = (SInt64)((int*)controlHandle);
		SInt64 data_value = (SInt64)((int*)(windowPtr->
						GetFrame()->FindWindow ((SInt16)windowid64)->GetClientData ()));
		return (SInt16)(data_value);
	#endif
	
}	// end "GetControlValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetCurrentClassField
//
//	Software purpose:	The purpose of this routine is to convert a field number to the
//							item number for that field in its respective class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/01/1998
//	Revised By:			Larry L. Biehl			Date: 04/01/1998

SInt16 GetCurrentClassField (
				SInt16								classNumber,
				SInt16								inputFieldNumber)

{
   // Local variables and structures												

   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classFieldNumber,
											classStorage,
											fieldNumber;


			// Initialize local variables.

   classFieldNumber = -1;
   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

			// Get the class storage number.

   classStorage = gProjectInfoPtr->storageClass[classNumber];

			// Execute routine if input class number and relative field number
			// are within proper range.

   if (classNumber < (SInt16)gProjectInfoPtr->numberStatisticsClasses &&
																						classNumber >= 0)
		{
      if (inputFieldNumber >= 0 &&
								inputFieldNumber < gProjectInfoPtr->numberStatisticsFields)
			{
					// Convert the field number to the (classFieldNumber) within the class.

         fieldNumber = gProjectInfoPtr->classNamesPtr[classStorage].firstFieldNumber;
         if (fieldNumber >= 0)
				{
            classFieldNumber = 0;
            while (inputFieldNumber != fieldNumber && fieldNumber >= 0)
					{
               fieldNumber = fieldIdentPtr[fieldNumber].nextField;
               classFieldNumber++;

					}	// end "while (inputFieldNumber != fieldNumber && ..."

				}	// end "if (fieldNumber >= 0)"

					// Check if field number is within the proper range.

         if (classFieldNumber >=
						gProjectInfoPtr->classNamesPtr[classStorage].numberOfFields ||
					classFieldNumber < 0)
            classFieldNumber = -1;

			}	// end "if (classFieldNumber >= 0 &&..."

		}	// end "if (classNumber<(SInt16)gProjectInfoPtr->..."

   return (classFieldNumber);

}	// end "GetCurrentClassField"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetCurrentField
//
//	Software purpose:	The purpose of this routine is to convert a 
//							selected field for a class to the field number for
//							all classes.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/15/1989
//	Revised By:			Larry L. Biehl			Date: 03/20/1991	

SInt16 GetCurrentField (
				SInt16								classNumber,
				SInt16								classFieldNumber)

{
			// Local variables and structures

   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classStorage,
											fieldCount,
											fieldNumber;


			// Initialize local variables.

   fieldNumber = -1;
   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

			// Get the class storage number.

   classStorage = gProjectInfoPtr->storageClass[classNumber];

			// Execute routine if input class number and relative field number
			// are within proper range.

   if (classNumber < (SInt16)gProjectInfoPtr->numberStatisticsClasses &&
																						classNumber >= 0)
		{
      if (classFieldNumber >= 0 && classFieldNumber <
								gProjectInfoPtr->classNamesPtr[classStorage].numberOfFields)
			{
					// Convert the class and relative field number
					// (classFieldNumber) within the class to the number of the 	
					// field in the overall list.												

         fieldNumber = gProjectInfoPtr->classNamesPtr[classStorage].firstFieldNumber;
         if (classFieldNumber > 0)
				{
            fieldCount = 1;
            do
					{
               fieldNumber = fieldIdentPtr[fieldNumber].nextField;
               fieldCount++;

					}	while (fieldCount <= classFieldNumber);

				}	// end "if (fieldNumber > 0)"

					// Check if field number is within the proper range.

         if (fieldNumber >= gProjectInfoPtr->numberStorageFields || fieldNumber < 0)
            fieldNumber = -1;

			}	// end "if (classFieldNumber >= 0 &&..."
		
		}	// end "if (classNumber < ..."

   return (fieldNumber);

}	// end "GetCurrentField"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDefaultClassName
//
//	Software purpose:	The purpose of this routine is to get a default class
//							name that is not a duplicate of those already being used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1996
//	Revised By:			Larry L. Biehl			Date: 11/25/2019	

void GetDefaultClassName (
				char*									stringPtr)

{
   SInt32								classNumber;


   classNumber = gProjectInfoPtr->numberStorageClasses + 1;

   do
		{
      sprintf (&stringPtr[1], "Class %d", (int)classNumber);
      stringPtr[0] = (char)strlen (&stringPtr[1]);
      classNumber++;

		}	while (CheckForDuplicateClassName (-1, (Str255*)stringPtr) != 0);

}	// end "GetDefaultClassName"       



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetDefaultFieldName
//
//	Software purpose:	The purpose of this routine is to get a default field
//							name that is not a duplicate of those already being used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1996
//	Revised By:			Larry L. Biehl			Date: 02/09/1996	

void GetDefaultFieldName (
				char*									stringPtr)

{
   SInt32								fieldNumber;


   fieldNumber = gProjectInfoPtr->numberStorageFields + 1;

   do
		{
      sprintf (&stringPtr[1], "Field %d", (int)fieldNumber);
      stringPtr[0] = (char)strlen (&stringPtr[1]);
      fieldNumber++;

		}	while (CheckForDuplicateFieldName (-1, (Str255*)stringPtr));

}	// end "GetDefaultFieldName" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				void GetFieldNameWithType
//
//	Software purpose:	The purpose of this routine is to put a string
//							together with the field type at the beginning ('n '
//							for training fields and 't ' for test fields)
//							followed by the field name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/23/1989
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void GetFieldNameWithType (
				SInt16								field,
				UCharPtr								stringPtr)

{
   HPFieldIdentifiersPtr			fieldIdentPtr;


   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

			// Add name to the field list.  Include 'n ' at beginning				
			// for identifying training field type and 't ' at beginning			
			// for identifying test field type.												

   if (fieldIdentPtr[field].fieldType == kTrainingType)
      CopyPToP (stringPtr, (UCharPtr)"\0n ");

   else	// fieldIdentPtr[field].fieldType == testType 
      CopyPToP (stringPtr, (UCharPtr)"\0t ");

   ConcatPStrings ((UCharPtr)stringPtr, (StringPtr)&fieldIdentPtr[field].name, 255);

}	// end "GetFieldNameWithType" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetLineColFromList
//
//	Software purpose:	The purpose of this routine is to get the image
//							line and column number from specified line in the 
//							list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			EditSelectionDialog in SSelectionUtility.cpp
//							AddPointsToProject in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/04/1988
//	Revised By:			Larry L. Biehl			Date: 03/02/2017	

void GetLineColFromList (
				SInt16								listLine,
				LongPoint*							lineColPoint)

{
	#if defined multispec_mac
			// Declare local variables and structures										

		Point									cell;
		SInt32								theNum;
		SInt16								dataLength;


				// Get the line number																

		cell.h = 0;
		cell.v = listLine;
		dataLength = 32;
		LGetCell (&gTextString[1], &dataLength, cell, gStatisticsListHandle);
		gTextString[0] = (char)dataLength;
		StringToNum (gTextString, &theNum);
		lineColPoint->v = theNum;

				// Get the column number													

		cell.h = 1;
		dataLength = 32;
		LGetCell (&gTextString[1], &dataLength, cell, gStatisticsListHandle);
		gTextString[0] = (char)dataLength;
		StringToNum (gTextString, &theNum);
		lineColPoint->h = theNum;
	#endif	// defined multispec_mac 

	#if defined multispec_win
		char*		returnStringPtr;

		returnStringPtr = (char*)gWideTextString;

		if (gStatisticsListHandle->GetText (listLine, (LPTSTR)returnStringPtr) > 0)
			{
			wcstombs ((char*)gTextString, (TBYTE*)returnStringPtr, 254);
			returnStringPtr = (char*)gTextString;

			sscanf (returnStringPtr,
						"%ld\t%ld",
						&lineColPoint->v,
						&lineColPoint->h);
			
			}	// end "if (gStatisticsListHandle->GetText ..."

		else	// ...<= 0)
			{
			lineColPoint->v = 0;
			lineColPoint->h = 0;
			
			}	// end "else ...<= 0)" 
	#endif	// defined multispec_win 

	#if defined multispec_wx
		if (gStatisticsListHandle->GetCount () > 0)
			{
			wxString text = gStatisticsListHandle->GetString (listLine);
			strcpy ((char*)gTextString, (const char*)(text.mb_str (wxConvUTF8)));
			sscanf ((char*)gTextString,
						"%d\t%d",
						&lineColPoint->v,
						&lineColPoint->h);
			
			}	// end "if (gStatisticsListHandle->GetCount () > 0)"
	
		else	// ...<= 0)
			{
			lineColPoint->v = 0;
			lineColPoint->h = 0;
			
			}	// end "else ...<= 0)" 
	#endif	// defined multispec_wx
   
}	// end "GetLineColFromList"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetNumberListLines
//
//	Software purpose:	The purpose of this routine is return the number of list
//							lines in the list box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			EditSelectionDialog in SSelectionUtility.cpp
//							AddPointsToProject in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1996
//	Revised By:			Larry L. Biehl			Date: 11/30/2017

SInt16 GetNumberListLines (
				ListHandle							listHandle)

{
			// Declare local variables and structures
	
	#if defined multispec_wx
		SInt16 numberLines = (SInt16)listHandle->GetCount ();
		if (numberLines == wxNOT_FOUND)
			numberLines = 0;

		return (numberLines);
	#endif

	#if defined multispec_mac || defined multispec_mac_swift
		return (((ListPtr)*listHandle)->dataBounds.bottom);
	#endif	// defined multispec_mac || defined multispec_mac_swift

	#if defined multispec_win
		SInt16 numberLines = (SInt16)listHandle->GetCount ();
		if (numberLines == LB_ERR)
			numberLines = 0;

		return (numberLines);
	#endif	// defined multispec_win

}	// end "GetNumberListLines"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetCovarianceStatsFromMenuItem
//
//	Software purpose:	The purpose of this routine is to get the covariance statistics 
//							to be used from the input menu item.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			StatisticsWMouseDn in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1997
//	Revised By:			Larry L. Biehl			Date: 02/23/2000	

SInt16 GetCovarianceStatsFromMenuItem (
				SInt16								menuItem)

{

   SInt16 covarianceStatsToUse;


   covarianceStatsToUse = menuItem;

   if (menuItem == 3)
      covarianceStatsToUse = kEnhancedStats;

   else if (menuItem == 4)
      covarianceStatsToUse = kMixedStats;

   return (covarianceStatsToUse);

}	// end "GetCovarianceStatsFromMenuItem" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetClassStatisticsTypeMenuItem
//
//	Software purpose:	The purpose of this routine is to get the statistics type menu 
//							item associated with the current class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			StatisticsPopUpMenu in SProjectUtilities.cpp
// 						DrawStatPopUp in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1997
//	Revised By:			Larry L. Biehl			Date: 12/18/1997	

SInt16 GetClassStatisticsTypeMenuItem (void){

   UInt32 classStorage,
      covarianceStatsToUse,
      menuItem;


   classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
   covarianceStatsToUse =
      gProjectInfoPtr->classNamesPtr[classStorage].covarianceStatsToUse;

   menuItem = covarianceStatsToUse;
   if (covarianceStatsToUse == kEnhancedStats)
      menuItem = 3;

   return ((SInt16)menuItem);

}	// end "GetClassStatisticsTypeMenuItem" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetProjectStatisticsTypeMenuItem
//
//	Software purpose:	The purpose of this routine is to get the statistics type menu 
//							item associated with the current class.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			UpdateStatisticsWindow in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1997
//	Revised By:			Larry L. Biehl			Date: 12/19/1997	

SInt16 GetProjectStatisticsTypeMenuItem (void){

   UInt32 covarianceStatsToUse,
      menuItem;


   covarianceStatsToUse = gProjectInfoPtr->covarianceStatsToUse;

   menuItem = covarianceStatsToUse;
   if (covarianceStatsToUse == kEnhancedStats)
      menuItem = 3;
   else if (covarianceStatsToUse == kMixedStats)
      menuItem = 4;

   return ((SInt16)menuItem);

}	// end "GetProjectStatisticsTypeMenuItem" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectStatisticsTypeText
//
//	Software purpose:	The purpose of this routine is to load the text description in
//							the text output location for the current project statistics type.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			UpdateStatisticsWindow in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/04/1998
//	Revised By:			Larry L. Biehl			Date: 03/15/2017	

void GetProjectStatisticsTypeText (
				UCharPtr								textStringPtr,
				SInt16								menuItem)

{
			// Initialize string to no characters.

   textStringPtr[0] = 0;

   switch (menuItem)
		{
		#if defined multispec_mac
			case 1:
			case 3:
				GetMenuItemText (gPopUpStatisticsTypeMenu,
										menuItem,
										textStringPtr);
				break;
		#endif	// defined multispec_mac 				

		#if defined multispec_win || multispec_wx
			case 1:
				CopyPToP (textStringPtr, (UCharPtr)"\0Original");
				break;

			case 3:
				CopyPToP (textStringPtr, (UCharPtr)"\0Enhanced");
				break;
		#endif	// defined multispec_win

      case 2:
         CopyPToP (textStringPtr, (UCharPtr)"\0LOOC");
         break;

      case 4:
         CopyPToP (textStringPtr, (UCharPtr)"\0Mixed Stats");
         break;

		}	// end "switch (menuItem)"

}	// end "GetProjectStatisticsTypeText" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetUniqueClassName
//
//	Software purpose:	The purpose of this routine is to create a unique class name
//							using the input prefix which may already be stored in the output
//							name.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1998
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

void GetUniqueClassName (
				SInt32								suffixNumber,
				UCharPtr								outputStringPtr,
				SInt16								prefixLength,
				Boolean								useProjectClassNameFlag,
				CharPtr								namePtr)

{
   Str255								tempString;

   SInt16								stringLength;


			// Get unique name for the class.
   do
		{
      NumToString ((UInt32)suffixNumber, tempString);
      stringLength = (SInt16)tempString[0];
      BlockMoveData ((Ptr)&tempString[1],
							(Ptr)&outputStringPtr[prefixLength + 1],
							stringLength);

      stringLength += prefixLength;

				// If the cluster was initialized with a project class, then
				// include the project class name within the cluster name.

      stringLength = AddSpecifiedStringToClassName (outputStringPtr,
																		stringLength,
																		useProjectClassNameFlag,
																		(UCharPtr)namePtr);
		/*
		if (useProjectClassNameFlag && namePtr != NULL)
			{	
			sprintf ((char*)&outputStringPtr[stringLength+1], " (");
			stringLength += 2;
	
			BlockMoveData (
							&namePtr[1], (Ptr)&outputStringPtr[stringLength+1], namePtr[0]);
			stringLength += namePtr[0];
	
			if (stringLength > 30)
				stringLength = 30;
	
			sprintf ((char*)&outputStringPtr[stringLength+1], ")");
			stringLength += 1;
	
			}	// end "if (useProjectClassNameFlag && ..."
		*/
      outputStringPtr[0] = (UInt8)stringLength;
      suffixNumber++;

		}	while (CheckForDuplicateClassName (-1, (Str255*)outputStringPtr) != 0);

			// Add c-string terminator.

   outputStringPtr[outputStringPtr[0] + 1] = 0;

}	// end "GetUniqueClassName" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetUniqueFieldName
//
//	Software purpose:	The purpose of this routine is to create a unique field name
//							using the input prefix which may already be stored in the 
//							output string.
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:				
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1998
//	Revised By:			Larry L. Biehl			Date: 12/21/2016	

void GetUniqueFieldName (
				SInt32								suffixNumber,
				UCharPtr								outputStringPtr,
				SInt16								prefixLength)

{
   Str255								tempString;

   UInt32								stringLength;


			// Get unique name for the field.
   do
		{
      NumToString ((UInt32)suffixNumber, tempString);
      stringLength = (UInt32)MIN (tempString[0], 5);
      prefixLength = (SInt16)MIN (30 - stringLength, (UInt32)prefixLength);

      BlockMoveData ((Ptr)&tempString[1],
							(Ptr)&outputStringPtr[prefixLength + 1],
							stringLength);

      outputStringPtr[0] = (UInt8)(prefixLength + stringLength);
      suffixNumber++;

		}	while (CheckForDuplicateFieldName (-1, (Str255*)outputStringPtr));

   outputStringPtr[outputStringPtr[0]+1] = 0;

}	// end "GetUniqueFieldName" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void HideStatControl
//
//	Software purpose:	The purpose of this routine is to hide the input control
//							in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ActivateStatControls
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1996
//	Revised By:			Larry L. Biehl			Date: 02/28/2017	

void HideStatControl (
			ControlHandle							controlHandle)

{
			// Declare local variables and structures

	#if defined multispec_mac	
		HideControl (controlHandle);
	#endif	// defined multispec_mac 

	#if defined multispec_win
		if (controlHandle != NULL)
			((CMStatisticsForm*)gProjectWindow)->
										GetDlgItem ((int)controlHandle)->ShowWindow (SW_HIDE);
	#endif	// defined multispec_win

	#if defined multispec_wx
		if (controlHandle != NULL)
			{
			SInt64 windowid64 = (SInt64)((int*)(controlHandle));
			SInt16 windowid = (SInt16)windowid64;
			((CMStatisticsView*)gProjectWindow)->GetFrame()->
																FindWindow (windowid)->Show (false);
			//((CMStatisticsView*)gProjectWindow)->GetFrame()->Layout ();
   
			}
	#endif	// defined multispec_wx 
   
}	// end "HideStatControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InvalPopUpCovarianceToUse
//
//	Software purpose:	The purpose of this routine is force the 'covariance to use'
//							popup to be updated if it is being shown.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/19/1997
//	Revised By:			Larry L. Biehl			Date: 01/03/2020

void InvalPopUpCovarianceToUse (void)

{
	if (gProjectWindow != NULL &&
					(gProjectInfoPtr->statsWindowMode == kClassListMode ||
								gProjectInfoPtr->statsWindowMode == kFieldListMode))
		{
		#if defined multispec_mac 
			GrafPtr savedPort;

			GetPort (&savedPort);
			SetPortWindowPort (gProjectWindow);

			InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);

			SetPort (savedPort);
		#endif	// defined multispec_mac	

		#if defined multispec_win
			gProjectWindow->Invalidate (TRUE);
		#endif	// defined multispec_win
	
		#if defined multispec_wx
			((CMStatisticsView*)gProjectWindow)->m_frame->
									UpdateStatsTypeCombo (gProjectInfoPtr->statsWindowMode);
		
			#if defined multispec_wxlin
				wxComboBox* statsTypeCntrlPtr =
					(wxComboBox*)gProjectWindow->GetFrame()->FindWindow (IDC_StatsCombo);
			#endif
			#if defined multispec_wxmac
				wxChoice* statsTypeCntrlPtr =
					(wxChoice*)gProjectWindow->GetFrame()->FindWindow (IDC_StatsCombo);
			#endif
		
         int menuItem = GetProjectStatisticsTypeMenuItem();
         statsTypeCntrlPtr->SetSelection (menuItem - 1);
		#endif	// defined multispec_wx

		}	// end "if (gProjectWindow != NULL && ...)"

}	// end "InvalPopUpCovarianceToUse" 	



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassNamesIntoList
//
//	Software purpose:	The purpose of this routine is to load the class
//							names into a list specified by the input handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1989
//	Revised By:			Larry L. Biehl			Date: 05/04/2017	

void LoadClassNamesIntoList (
				ListHandle							listHandle)

{
	Cell									cell;

   HPClassNamesPtr					classNamesPtr;

   UInt32								classIndex,
											classStorage,
											numberOfClasses,
											row;
											
	
			//	Continue if list handle is not null.										

   if (listHandle != NULL)
		{
				// Put the class names into the list										

      numberOfClasses = gProjectInfoPtr->numberStatisticsClasses;
      classNamesPtr = gProjectInfoPtr->classNamesPtr;

      if (numberOfClasses > 0)
			{
         row = LAddRow ((SInt16)numberOfClasses, 0, listHandle);

         cell.h = 0;
         for (classIndex=0; classIndex<numberOfClasses; classIndex++)
				{
						// Get the class storage number.										

            classStorage = gProjectInfoPtr->storageClass[classIndex];

						// Add name to the class list											

            cell.v = (SInt16)classIndex;
            LSetCell ((Ptr)&(classNamesPtr[classStorage].name[1]),
								(short int)(classNamesPtr[classStorage].name[0]),
								cell,
								listHandle);

				}	// end "for (classIndex=0; ... 

			}	// end "if (numberOfClasses > 0)" 

		}	// end "if (listHandle != NULL)" 

}	// end "LoadClassNamesIntoList" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadPolyStatNewFieldW
//
//	Software purpose:	The purpose of this routine is to load the line-
//							column coordinates of points around the polygon into
//							the NewField mode stat window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			LoadNewFieldListBox
//							DoImageWActivateEvent in MMultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/05/1989
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

void LoadPolyStatNewFieldW (void)

{
			// Local variables and structures												

	Cell									cell;

   FileInfoPtr							fileInfoPtr;
   GrafPtr								savedPort = NULL;
   HPFieldPointsPtr					selectionPointsPtr;
   SelectionInfoPtr					selectionInfoPtr;

   Handle								fileInfoHandle,
											polygonCoordinatesHandle,
											selectionInfoH = NULL,
											windowInfoHandle;

   SInt32								column,
											columnOffset,
											line,
											lineOffset;

   SInt16								numberOfPoints,
											point;


	#if defined multispec_win
		USES_CONVERSION;
	#endif

			// Make certain that we have a valid project image window 				
			// information handle.																

   if (gProjectSelectionWindow == NULL || gProjectWindow == NULL)
																									return;

			// Get handle to window information handle.									

   windowInfoHandle = GetWindowInfoHandle (gProjectSelectionWindow);

			// Get the pointer to the selection area information.		 			

   selectionInfoH = GetSelectionInfoHandle (windowInfoHandle);
   if (selectionInfoH != NULL)
		{
      selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoH);
      polygonCoordinatesHandle = selectionInfoPtr->polygonCoordinatesHandle;
      numberOfPoints = selectionInfoPtr->numberPoints;

				// Get pointer to the polygon points.										

      if (selectionInfoPtr->typeFlag == kPolygonType &&
         polygonCoordinatesHandle != NULL)
			{
         selectionPointsPtr = (HPFieldPointsPtr)GetHandlePointer (
																			polygonCoordinatesHandle);

					// Don't forget that both the offscreen coordinates and the	
					// line-column coordinates are saved into the structure.		
					// The offscreen coordinates for a point is first and then	
					// the line-column coordinates are next.							

			#if defined multispec_mac 
						// Save currently active window and make the Statistics		
						// window the active window.											

				GetPort (&savedPort);
				SetPortWindowPort (gProjectWindow);
			#endif	// defined multispec_mac 

					// Turn drawing off until done with making changes.			

         LSetDrawingMode (FALSE, gStatisticsListHandle);

					// Remove cells from previous list in case any existed.		

         RemoveListCells ();

			#if defined multispec_mac 
				Point			cellSize;
				SInt16		rowAdded;

						// Make certain that we have two columns.							

				if (((ListPtr)* gStatisticsListHandle)->dataBounds.right == 1)
					LAddColumn (1, 1, gStatisticsListHandle);

				cellSize.h = ((ListPtr)* gStatisticsListHandle)->cellSize.h / 2;
				cellSize.v = ((ListPtr)* gStatisticsListHandle)->cellSize.v;
				LCellSize (cellSize, gStatisticsListHandle);

						// Add rows to list for the currently describing polygon.	

				rowAdded = LAddRow (numberOfPoints, 0, gStatisticsListHandle);
			#endif	// defined multispec_mac 

					// Check if any adjustment is needed to allow for any 	
					// difference between start line and column in base 		
					// image file and start line and column in associated 	
					// image file if needed.											

         columnOffset = 0;
         lineOffset = 0;
         if (!GetProjectBaseImageFlag (windowInfoHandle))
				{
						// Get pointer to file specification.  We do not need 
						// to lock it here since no other routines are called.

            fileInfoHandle = GetFileInfoHandle (windowInfoHandle);
            fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);

            columnOffset = (SInt32)
								(fileInfoPtr->startColumn - gProjectInfoPtr->startColumn);

            lineOffset = (SInt32)
								(fileInfoPtr->startLine - gProjectInfoPtr->startLine);

				}	// end "if (!GetProjectBaseImageFlag (windowInfoHandle))" 

					// Put the coordinates for the polygon into the list.			

         for (point = 0; point < numberOfPoints; point++)
				{
            cell.h = 0;
            cell.v = point;
            line = selectionPointsPtr->line + lineOffset;
            column = selectionPointsPtr->col + columnOffset;

				#if defined multispec_mac 
					NumToString (line, gTextString);
					LSetCell	(&(gTextString[1]),
									(SInt16)gTextString[0],
									cell,
									gStatisticsListHandle);

					cell.h = 1;
					NumToString (column, gTextString);
					LSetCell (&(gTextString[1]),
									(SInt16)gTextString[0],
									cell,
									gStatisticsListHandle);
				#endif	// defined multispec_mac 

				#if defined multispec_win
					sprintf ((char*)gTextString,
								"%ld\t%ld",
								line,
								column);
					cell.v = (SInt16)gStatisticsListHandle->
							InsertString (cell.v, (LPCTSTR)A2T((char*)gTextString));
				#endif	// defined multispec_win 

				#if defined multispec_wx
					sprintf ((char*)gTextString,
									"%d\t%d",
									line,
									column);
					cell.v = (SInt16)gStatisticsListHandle->Insert (
																			(char*)gTextString, cell.v);
				#endif	// defined multispec_wx 
            
            selectionPointsPtr++;

				}	// end "for (point=0; point<numberOfPoints; point++)" 

					// Hilite the "Add to List" control when at least three 		
					// points have	been selected.											

         if (numberOfPoints >= 3)
            MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);

					// Turn list drawing mode back on.									

         LSetDrawingMode (TRUE, gStatisticsListHandle);

			#if defined multispec_mac 
						// If list length is more than the 8 that can be displayed 	
						// in the list, scroll the list up by one so that the last 	
						// point will be displayed.  Also highlight the control if 	
						// there is more than 7 items in the list.  This is done 	
						// here because the vertical scroll bar does not get			
						// updated properly for this stat window mode when the list 
						// is longer than can be displayed.  I suspect it is 			
						// because at this point the project window is not 		
						// active.																	

				if (rowAdded > 7)
					{
					LScroll (0, 1, gStatisticsListHandle);
					HiliteControl ((*gStatisticsListHandle)->vScroll, 0);

					}	// end "if (rowAdded > 8)" 
			#endif	// defined multispec_mac 

					// Invalidate the list rectangle so that it will be updated	

			#if defined multispec_mac
				gTempRect = ((ListPtr)* gStatisticsListHandle)->rView;
				InvalWindowRect (gProjectWindow, &gTempRect);
			#endif	// defined multispec_mac 

					// Reset window back to saved window								

         SetPort (savedPort);
         CheckAndUnlockHandle (polygonCoordinatesHandle);

			}	// end "if (selectionInfoPtr->typeFlag == kPolygonType ..."

		}	// end "if (selectionInfoH != NULL)" 

}	// end "LoadPolyStatNewFieldW" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		LoadRectStatNewFieldW
//
//	Software purpose:	The purpose of this routine is to load the line-
//							column coordinates of the selection rectangle into 
//							the NewField mode stat window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DoImageWActivateEvent in MMultiSpec.c
//							NewFieldStatMode in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/03/1988
//	Revised By:			Larry L. Biehl			Date: 11/09/1990	

void LoadRectStatNewFieldW (void)

{
			// Local variables and structures

   LongRect selectionRectangle;


   if (gProjectSelectionWindow == NULL || gProjectWindow == NULL)
																									return;

   if (GetSelectionRectangle (gProjectSelectionWindow,
										&selectionRectangle,
										kDontClearSelectionArea,
										kDontUseThreshold,
										kAdjustToBaseImage))
		
		{
      LoadRectangleInStatList (&selectionRectangle);

				// Indicate that the last field selection was done with rectangle
				// type coordinates

      gProjectInfoPtr->lastPointType = kRectangleType;

				// Hilite the "Add to List" control since a field is selected

      MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 0);

   }	// end "if (GetSelectionRectangle (..." 

}	// end "LoadRectStatNewFieldW"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadRectangleInStatList
//
//	Software purpose:	The purpose of this routine is to load the line-
//							column coordinates of the input rectangle into 
//							the list box in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			EditCoordinatesDialog in SEditStatistics.cpp
//							DoEditSelectAllImage in SMenus.cpp
//							EditSelectionDialog in SSelectionUtility.cpp
//							LoadRectStatNewFieldW in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/13/1998
//	Revised By:			Larry L. Biehl			Date: 03/02/2017	

void LoadRectangleInStatList (
				LongRect*							selectionRectanglePtr)

{
	#if defined multispec_win
		USES_CONVERSION;
	#endif

   if (gStatisticsListHandle == NULL)
																									return;

	#if defined multispec_mac
		Cell									cell;
		GrafPtr								savedPort;
		Point									cellSize;

		SInt16								column,
												numberOfPoints,
												row;


				// Save currently active window and make active window that
				// Project window

		GetPort (&savedPort);
		SetPortWindowPort (gProjectWindow);

				// Remove cells from previous list in case any existed

		LSetDrawingMode (false, gStatisticsListHandle);

		RemoveListCells ();

				// Add columns and cells to list for 2 points

		numberOfPoints = 2;

		row = LAddRow (numberOfPoints, 0, gStatisticsListHandle);

		if (((ListPtr)* gStatisticsListHandle)->dataBounds.right == 1)
			column = LAddColumn (1, 1, gStatisticsListHandle);

		cellSize.h = ((ListPtr)*gStatisticsListHandle)->cellSize.h / 2;
		cellSize.v = ((ListPtr)*gStatisticsListHandle)->cellSize.v;
		LCellSize (cellSize, gStatisticsListHandle);

				// Put the selection rectangle coordinates into the polygon list

		cell.v = 0;
		cell.h = 0;
		NumToString (selectionRectanglePtr->top, gTextString);
		LSetCell (
				&(gTextString[1]), (SInt16)gTextString[0], cell, gStatisticsListHandle);

		cell.h = 1;
		NumToString (selectionRectanglePtr->left, gTextString);
		LSetCell (
				&(gTextString[1]), (SInt16)gTextString[0], cell, gStatisticsListHandle);

		cell.v = 1;
		cell.h = 0;
		NumToString (selectionRectanglePtr->bottom, gTextString);
		LSetCell (
				&(gTextString[1]), (SInt16)gTextString[0], cell, gStatisticsListHandle);

		cell.h = 1;
		NumToString (selectionRectanglePtr->right, gTextString);
		LSetCell (
				&(gTextString[1]), (SInt16)gTextString[0], cell, gStatisticsListHandle);

				// Turn list drawing mode back on

		LSetDrawingMode (true, gStatisticsListHandle);

				// Invalidate the list rectangle so that it will be updated

		gTempRect = ((ListPtr)* gStatisticsListHandle)->rView;
		InvalWindowRect (gProjectWindow, &gTempRect);

				// Reset window back to saved window

		SetPort (savedPort);
	#endif	// defined multispec_mac 

	#if defined multispec_win 
		SInt16								index;

				// Remove cells from previous list in case any existed				

		gStatisticsListHandle->ResetContent ();

				// Put the selection rectangle coordinates into the polygon list	

		index = 0;
		sprintf ((char*)gTextString,
					"%ld\t%ld",
					selectionRectanglePtr->top,
					selectionRectanglePtr->left);
		gStatisticsListHandle->InsertString (index, (LPCTSTR)A2T((char*)gTextString));

		index = 1;
		sprintf ((char*)gTextString,
					"%ld\t%ld",
					selectionRectanglePtr->bottom,
					selectionRectanglePtr->right);
		gStatisticsListHandle->InsertString (index, (LPCTSTR)A2T((char*)gTextString));
	#endif	// defined multispec_win 

	#if defined multispec_wx
		SInt16								index;

				// Remove cells from previous list in case any existed

		//gStatisticsListHandle->ResetContent ();
		if (gStatisticsListHandle != NULL)
			gStatisticsListHandle->Clear ();

				// Put the selection rectangle coordinates into the polygon list

		index = 0;
		sprintf ((char*)gTextString,
					"%d\t%d",
					selectionRectanglePtr->top,
					selectionRectanglePtr->left);
					gStatisticsListHandle->AppendString ((wxString)gTextString);

		index = 1;
		sprintf ((char*)gTextString,
					"%d\t%d",
					selectionRectanglePtr->bottom,
					selectionRectanglePtr->right);
					gStatisticsListHandle->AppendString ((wxString)gTextString);
	#endif	// defined multispec_wx
   
}	// end "LoadRectangleInStatList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadNewFieldListBox
//
//	Software purpose:	The purpose of this routine is to load the line-
//							column coordinates of the selection area into 
//							the NewField mode list box in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			DoImageWActivateEvent in MMultiSpec.c
//							NewFieldStatMode in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/09/1996
//	Revised By:			Larry L. Biehl			Date: 10/01/1998	

void LoadNewFieldListBox (void)

{
			// Load in the selected area if there is one.

   if (gStatisticsMode == kStatNewFieldRectMode)
      LoadRectStatNewFieldW ();

   else if (gStatisticsMode == kStatNewFieldPolyMode)
      LoadPolyStatNewFieldW ();

}	// end "LoadNewFieldListBox"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean LOOCOptionsDialog
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
// Called By:			StatisticsPopUpMenu in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2000
//	Revised By:			Larry L. Biehl			Date: 02/24/2000	

Boolean LOOCOptionsDialog (
				SInt16								statsWindowMode)

{
	Boolean								returnFlag = FALSE;

	#if defined multispec_mac
		double								loocMixingParameter,
												userMixingParameter,
												value;

		DialogPtr							dialogPtr;

		Handle								okHandle,
												theHandle;

		Rect									theBox;

		SInt32								theNum;

		SInt16								itemHit,
												mixingParameterCode,
												theType;

		Boolean								modalDone;


		dialogPtr = NULL;

				// Get the modal dialog for the looc options specification

		dialogPtr = LoadRequestedDialog (
											kSetLOOCParameterSpecificationID, kCopyScrap, 1, 2);

		if (dialogPtr == NULL)
																							return (FALSE);

				// Save handle for the OK button for use later.

		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

		LOOCOptionsDialogInitialize (dialogPtr,
												statsWindowMode,
												&mixingParameterCode,
												&loocMixingParameter,
												&userMixingParameter);

				// Mixing parameter type setting.

		SetDLogControl (dialogPtr, 4, mixingParameterCode == 1);
		SetDLogControl (dialogPtr, 6, mixingParameterCode == 2);
		SetDLogControl (dialogPtr, 8, mixingParameterCode == 3);

		LoadDItemRealValue (dialogPtr, 5, loocMixingParameter, 3);

		LoadDItemRealValue (dialogPtr, 7, userMixingParameter, 3);

				// Center the dialog and then show it.

		ShowDialogWindow (
						dialogPtr, kSetLOOCParameterSpecificationID, kSetUpDFilterTable);

		gDialogItemDescriptorPtr[7] = kDItemReal;

				// Set default text selection to first edit text item

		SelectDialogItemText (dialogPtr, 7, 0, SHRT_MAX);

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
					case 4:
					case 6:
					case 8:
						SetDLogControl (dialogPtr, 4, (itemHit == 4));
						SetDLogControl (dialogPtr, 6, (itemHit == 6));
						SetDLogControl (dialogPtr, 8, (itemHit == 8));

						ShowHideDialogItem (dialogPtr, 7, (itemHit == 6));

						if (itemHit == 6)
							SelectDialogItemText (dialogPtr, 7, 0, SHRT_MAX);

						mixingParameterCode = itemHit / 2 - 1;
						break;

					case 7:
						value = GetDItemRealValue (dialogPtr, 7);
						if (value < 0 || value > 3)
							RealNumberErrorAlert (userMixingParameter, dialogPtr, 7, 3);

						else	// value >= 0 && value <= 3
							userMixingParameter = value;
						break;

					}	// end "switch (itemHit)"

				}	// end "if (itemHit > 2)"

			else	// itemHit <= 2 
				{
				if (itemHit == 1) // User selected OK for information 
					{
					modalDone = TRUE;
					returnFlag = TRUE;

					LOOCOptionsDialogOK (statsWindowMode,
												mixingParameterCode,
												userMixingParameter);

					}	// end "if (itemHit == 1)"

				if (itemHit == 2) // User selected Cancel for information 
					{
					modalDone = TRUE;
					returnFlag = FALSE;

					}	// end "if	(itemHit == 2)"

				}	// end "else itemHit <= 2"

			}	while (!modalDone);

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac 

	#if defined multispec_win
		CMLOOMixingDialog* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMLOOMixingDialog ();

			returnFlag = dialogPtr->DoDialog (statsWindowMode);

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
      CMLOOMixingDialog* dialogPtr = NULL;

		try
			{
			dialogPtr = new CMLOOMixingDialog ();

			returnFlag = dialogPtr->DoDialog (statsWindowMode);

			delete dialogPtr;
			}

		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			returnFlag = FALSE;
			}
	
   
   #endif	// defined multispec_wx    
      
   return (returnFlag);

}	// end "LOOCOptionsDialog"



void LOOCOptionsDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								statsWindowMode,
				SInt16*								mixingParameterCodePtr,
				double*								loocMixingParameterPtr,
				double*								userMixingParameterPtr)

{
	SInt16								classStorage;


   if (statsWindowMode == kClassListMode)
		{
      *mixingParameterCodePtr = gProjectInfoPtr->mixingParameterCode;
      *userMixingParameterPtr = gProjectInfoPtr->userMixingParameter;

      HideDialogItem (dialogPtr, IDC_OptimumValue);

		}	// end "if (statsWindowMode == kClassListMode)"

   else	// statsWindowMode == kFieldListMode  
		{
				// Get the class storage number.

      classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];

      *loocMixingParameterPtr =
						gProjectInfoPtr->classNamesPtr[classStorage].looCovarianceValue;

      *mixingParameterCodePtr =
						gProjectInfoPtr->classNamesPtr[classStorage].mixingParameterCode;

      *userMixingParameterPtr =
						gProjectInfoPtr->classNamesPtr[classStorage].userMixingParameter;

      if (*loocMixingParameterPtr < 0)
         HideDialogItem (dialogPtr, IDC_OptimumValue);

		}	// end "else statsWindowMode == kFieldListMode"

   if (*mixingParameterCodePtr != kUserSet)
      HideDialogItem (dialogPtr, IDC_UserValue);

}	// end "LOOCOptionsDialogInitialize"



void LOOCOptionsDialogOK (
				SInt16								statsWindowMode,
				SInt16								mixingParameterCode,
				double								userMixingParameter)

{
   if (statsWindowMode == kClassListMode)
		{
      SetProjectMixingParameter (mixingParameterCode, userMixingParameter);

		}	// end "if (statsWindowMode == kClassListMode)"

   else	// statsWindowMode == kFieldListMode  
		{
      SetClassMixingParameter (gProjectInfoPtr->currentClass,
											mixingParameterCode,
											userMixingParameter);

		}	// end "else statsWindowMode == kFieldListMode"

}	// end "LOOCOptionsDialogOK"                        



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void MSetControlValue
//
//	Software purpose:	The purpose of this routine is to set the value for the
//							specified control to the requested value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ReadArcViewShapeFile in SArcView.cpp
//							GetNumberPixelsInSelection in SSelectionUtility.cpp
//							StatisticsWControlEvent
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1996
//	Revised By:			Larry L. Biehl			Date: 03/16/2018
//	Revised By:			Wei-Kang Hsu			Date: 10/31/2015

void MSetControlValue (
				WindowPtr							windowPtr,
				ControlHandle						controlHandle,
				SInt32								value)

{
   if (controlHandle != NULL)
		{
				// Declare local variables and structures

		#if defined multispec_mac
			SetControl32BitValue (controlHandle, value);
		#endif	// defined multispec_mac 

		#if defined multispec_win   
			((CButton*)windowPtr->GetDlgItem ((int)controlHandle))->SetCheck (value);
		#endif	// defined multispec_win
			
		#if defined multispec_wx   
			SInt64 windowid64 = (SInt64)((int*)(controlHandle));
			SInt16 windowid = (SInt16)windowid64;
			windowPtr->GetFrame()->FindWindow (windowid)->
															SetClientData ((void*)(SInt64)value);
		#endif	// defined multispec_wx
			
		}	// end "if (controlHandle != NULL)"

}	// end "MSetControlValue"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean NewClassFieldDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog entering the name/identifier of a new
//							class or field. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			AddFieldToProject   in SStatistics.cpp
//							PasteFieldToNewClass in SEditStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date:  09/27/1988
//	Revised By:			Larry L. Biehl			Date:  03/15/2017

Boolean NewClassFieldDialog (
				Boolean								newClassOnlyFlag,
				UCharPtr								classNamePtr,
				UCharPtr								fieldNamePtr,
				SInt16*								fieldTypePtr)

{
	SInt16								savedLastClassSelection;

   Boolean								OKFlag = FALSE;


			// Initialize local variables.

   savedLastClassSelection = gProjectInfoPtr->lastClassForAddFieldSelection;

   if (newClassOnlyFlag)
      gProjectInfoPtr->lastClassForAddFieldSelection = 1;

	#if defined multispec_mac
		Rect									theBox;

		SInt64								numberSelectionPixels;

		DialogPtr							dialogPtr;
		UserItemUPP							drawDialogClassPopUp2Ptr;
	
		Handle								okHandle,
												theHandle;

		SInt16								fieldType,
												itemHit,
												theType;

		UInt16								selectedItem;

		Boolean								badClassNameFlag,
												badFieldNameFlag,
												modalDone,
												setNumberClassTrainPixelsFlag;


				// Get the modal dialog for the reformat specification

		dialogPtr = LoadRequestedDialog (kNewClassFieldDialogID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)
																							return (false);

				// Intialize local user item proc pointers.

		drawDialogClassPopUp2Ptr = NewUserItemUPP (DrawDialogClassPopUp2);

		if (!newClassOnlyFlag)
			SetDialogItemDrawRoutine (dialogPtr, 10, drawDialogClassPopUp2Ptr);

				// Initialize the dialog box parameters that are common between the
				// Mac and Windows versions.

		NewClassFieldDialogInitialize (dialogPtr,
													newClassOnlyFlag,
													&selectedItem,
													fieldNamePtr,
													*fieldTypePtr,
													&numberSelectionPixels);

				// Save handle for the OK button for use later.

		GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

		fieldType = *fieldTypePtr;

		if (selectedItem > 0)
			SelectDialogItemText (dialogPtr, selectedItem, 0, INT16_MAX);

				// Center the dialog and then show it.

		ShowDialogWindow (dialogPtr, kNewClassFieldDialogID, kSetUpDFilterTable);

		gDialogItemDescriptorPtr[4] = kDItemString + kDItemSType2;
		gDialogItemDescriptorPtr[6] = kDItemString + kDItemSType2;

		badClassNameFlag = false;
		badFieldNameFlag = false;
		modalDone = false;
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
					case 4: // Class Name 
						GetDialogItemText (theHandle, gTextString);

						if (gTextString[0] <= 0)
							badClassNameFlag = true;

						else	// gTextString[0] > 0 
							badClassNameFlag = false;

						if (gTextString[0] > 31)
							{
							gTextString[0] = (char)31;
							SetDialogItemText (theHandle, gTextString);
							SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);
							SysBeep (2);

							}	// end "if (gTextString[0] > 31)"
						break;

					case 6: // Field Identifier 
						GetDialogItemText (theHandle, gTextString);

						// Make certain that there are some characters in the		
						// field name.															

						if (gTextString[0] <= 0)
							badFieldNameFlag = true;

						else	// gTextString[0] > 0 
							badFieldNameFlag = false;

						if (gTextString[0] > 31)
							{
							gTextString[0] = (char)31;
							SetDialogItemText (theHandle, (ConstStr255Param)&gTextString);
							SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);
							SysBeep (2);

							}	// end "if (gTextString[0] > 31)"
						break;

					case 7: // Radio button for training field
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 8, 0);
						fieldType = 1;
						setNumberClassTrainPixelsFlag = TRUE;
						break;

					case 8: // Radio button for test field
						SetControlValue ((ControlHandle)theHandle, 1);
						SetDLogControl (dialogPtr, 7, 0);
						fieldType = 2;
						setNumberClassTrainPixelsFlag = TRUE;
						break;

					case 10: // Display spectral classes/information groups Popup menu.
						itemHit = StandardPopUpMenu (
													dialogPtr,
													9,
													10,
													gPopUpClassMenu,
													gProjectInfoPtr->lastClassForAddFieldSelection,
													kPopUpClassMenuID);

						if (itemHit != 0)
							{
							NewClassFieldDialogChangeClass (dialogPtr,
																		itemHit,
																		fieldType,
																		numberSelectionPixels);

							}	// end "if (itemHit != 0)"

								// Make certain that the correct label is drawn in the
								// class pop up box.

						InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);

					}	// end "switch (itemHit)"

				if (setNumberClassTrainPixelsFlag)
					{
					SetNumberClassTrainPixels (
													dialogPtr, numberSelectionPixels, fieldType);
					setNumberClassTrainPixelsFlag = FALSE;

					}	// end "if (setNumberClassTrainPixelsFlag)"

						// If both the class and field name have some characters,
						// hilite the 'OK' button.

				if (badClassNameFlag || badFieldNameFlag)
					HiliteControl ((ControlHandle)okHandle, 255);

				else	// !badClassNameFlag && !badFieldNameFlag 
					HiliteControl ((ControlHandle)okHandle, 0);


				}	// end "if (itemHit > 2)"

			else	// itemHit < 2 
				{
				if (itemHit == 1) // User selected OK for information 
					{
					modalDone = true;
					OKFlag = true;

							// Item 4; Class Name

					GetDialogItem (dialogPtr, 4, &theType, &theHandle, &theBox);
					GetDialogItemText (theHandle, (UCharPtr)classNamePtr);

							// Check if proposed new class name is a duplicate.	 If it is,
							// allow user to change it again.

					if (gProjectInfoPtr->lastClassForAddFieldSelection == 1)
						{
						if (CheckForDuplicateClassName (-1, (Str255*)classNamePtr) != 0)
							{
							HiliteControl ((ControlHandle)okHandle, 255);
							DupClassFieldNameAlert (1, classNamePtr);
							HiliteControl ((ControlHandle)okHandle, 0);
							SelectDialogItemText (dialogPtr, 4, 0, INT16_MAX);
							modalDone = false;
							OKFlag = false;

							}	// end "else if (CheckForDuplicateClassName..."

						}	// end "if (gProjectInfoPtr->lastClassForAddFieldSelection == 1)"

							// Item 6; Field Identifier

					if (modalDone)
						{
						GetDialogItem (dialogPtr, 6, &theType, &theHandle, &theBox);
						GetDialogItemText (theHandle, (UCharPtr)fieldNamePtr);

								// Check if proposed field name is a duplicate.	 If it is,
								// allow user to change it again.

						if (CheckForDuplicateFieldName (-1, (Str255*)fieldNamePtr))
							{
							HiliteControl ((ControlHandle)okHandle, 255);
							DupClassFieldNameAlert (2, fieldNamePtr);
							HiliteControl ((ControlHandle)okHandle, 0);
							SelectDialogItemText (dialogPtr, 6, 0, INT16_MAX);
							modalDone = false;
							OKFlag = false;

							}	// end "else if (CheckForDuplicateFieldName..."

						}	// end "if (modalDone)"

							// Items 7,8; Training field or test field

					if (modalDone)
						{
						if (GetDLogControl (dialogPtr, 7) == 1)
							*fieldTypePtr = kTrainingType;

						if (GetDLogControl (dialogPtr, 8) == 1)
							*fieldTypePtr = kTestingType;

						}	// end "if (modalDone)"

					}	// end "if (itemHit == 1)"

				if (itemHit == 2) // User selected Cancel for information 
					{
					modalDone = true;
					OKFlag = false;

					}	// end "if	(itemHit == 2)"

				}	// end "else itemHit < 2"

			}	while (!modalDone);

		DisposeUserItemUPP (drawDialogClassPopUp2Ptr);

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac  

	#if defined multispec_win
		CMNewClassFieldDlg* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMNewClassFieldDlg ();

			OKFlag = dialogPtr->DoDialog (newClassOnlyFlag,
													classNamePtr,
													fieldNamePtr,
													fieldTypePtr);

			delete dialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif	// defined multispec_win  

	#if defined multispec_wx
		CMNewClassFieldDlg* dialogPtr = NULL;

		try
			{
			dialogPtr = new CMNewClassFieldDlg ();

			OKFlag = dialogPtr->DoDialog (
										newClassOnlyFlag,
										(char*)classNamePtr,
										(char*)fieldNamePtr,
										fieldTypePtr);

			delete dialogPtr;
			}

		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif	// defined multispec_win
   
   if (!OKFlag)
      gProjectInfoPtr->lastClassForAddFieldSelection = savedLastClassSelection;

   return (OKFlag);

}	// end "NewClassFieldDialog" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void NewClassFieldDialogChangeClass
//
//	Software purpose:	The purpose of this routine is to handle a change in the
//							dialog parameters due to a change in the current class. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			NewClassFieldDialog in SStatistics.cpp
//							CMNewClassFieldDlg::OnInitDialog in WNewClassFieldDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1999
//	Revised By:			Larry L. Biehl			Date: 08/23/2010

void NewClassFieldDialogChangeClass (
				DialogPtr							dialogPtr,
				SInt16								newCurrentClass,
				SInt16								fieldType,
				SInt64								numberSelectionPixels)

{
   UInt16								selectedItem;


   gProjectInfoPtr->lastClassForAddFieldSelection = newCurrentClass;

   if (newCurrentClass == 1)
		{
      ShowDialogItem (dialogPtr, IDC_ClassNamePrompt);
      ShowDialogItem (dialogPtr, IDC_ClassName);

      selectedItem = IDC_ClassName;

		}	// end "if (newCurrentClass == 1)"

   else	// newCurrentClass != 1 
		{
      HideDialogItem (dialogPtr, IDC_ClassNamePrompt);
      HideDialogItem (dialogPtr, IDC_ClassName);

      selectedItem = IDC_FieldName;

		}	// end "else newCurrentClass != 1"

   SelectDialogItemText (dialogPtr, selectedItem, 0, (SInt16)INT_MAX);

   SetNumberClassTrainPixels (dialogPtr, numberSelectionPixels, fieldType);

}	// end "NewClassFieldDialogChangeClass"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean NewClassFieldDialogInitialize
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog entering the name/identifier of a new
//							class or field. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			NewClassFieldDialog in SStatistics.cpp
//							CMNewClassFieldDlg::OnInitDialog in WNewClassFieldDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void NewClassFieldDialogInitialize (
				DialogPtr							dialogPtr,
				Boolean								newClassOnlyFlag,
				UInt16*								selectedItemPtr,
				UCharPtr								fieldNamePtr,
				SInt16								fieldType,
				SInt64*								numberSelectionPixelsPtr)

{
   *selectedItemPtr = 0;

			// Static Text with "Enter Class Name"
			// Edit field for entering class name
			// Hilight this item if showing													
			// Get a default class name.														

   GetDefaultClassName ((char*)gTextString2);

   LoadDItemString (dialogPtr, IDC_ClassName, (Str255*)gTextString2);

   if (gProjectInfoPtr->lastClassForAddFieldSelection > 1)
		{
      HideDialogItem (dialogPtr, IDC_ClassNamePrompt);
      HideDialogItem (dialogPtr, IDC_ClassName);

		}	// end "if (gProjectInfoPtr->lastClassForAddFieldSelection > 1)"

   else	// gProjectInfoPtr->lastClassForAddFieldSelection == 1 
      *selectedItemPtr = IDC_ClassName;

			// Static Text with "Enter Field Identifier"
			// Edit field for entering field identifier						
			// Hilight this item if class name not showing.								
			// Use input field name if it exists; otherwise make a default			
			// name.										

   if (fieldNamePtr[0] != 0)
      LoadDItemString (dialogPtr, IDC_FieldName, (Str255*)fieldNamePtr);

   else	// fieldNamePtr[0] == 0 
		{
      GetDefaultFieldName ((char*)gTextString3);

      LoadDItemString (dialogPtr, IDC_FieldName, (Str255*)gTextString3);

		}	// end "else fieldNamePtr[0] == 0"

   if (gProjectInfoPtr->lastClassForAddFieldSelection > 1)
      *selectedItemPtr = IDC_FieldName;

			// Radio button for training field

   SetDLogControl (dialogPtr, IDC_Training, (fieldType != 2));

			// Radio button for test field

   SetDLogControl (dialogPtr, IDC_TestField, (fieldType == 2));

			// Items 10 & 11: Classes.

   if (newClassOnlyFlag)
		{
      MHideDialogItem (dialogPtr, IDC_ClassListPrompt);
      MHideDialogItem (dialogPtr, IDC_ClassList);

		}	// end "if (newClassOnlyFlag)"

			// Get the number of pixels in the selection.

   *numberSelectionPixelsPtr = GetSelectionNumberPixels (gProjectSelectionWindow);

			// Set the number of train pixels for the class.

   SetNumberClassTrainPixels (dialogPtr, *numberSelectionPixelsPtr, fieldType);

			// Set the number of pixels for the selection area.

   CreateNumberWithCommasInString ((char*)gTextString2,
												*numberSelectionPixelsPtr);

   gTextString[0] = sprintf ((char*)&gTextString[1],
										"Number pixels: %s",
										(char*)gTextString2);
										//"Number pixels: %ld",
										//(SInt32)*numberSelectionPixelsPtr);
	
   LoadDItemString (dialogPtr, IDC_FieldNumberPixels, (Str255*)gTextString);

}	// end "NewClassFieldDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetNumberClassTrainPixels
//
//	Software purpose:	The purpose of this routine is to set the number of training
//							pixels for the class
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			NewClassFieldDialog in SStatistics.cpp
//							CMNewClassFieldDlg::OnInitDialog in WNewClassFieldDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/11/1999
//	Revised By:			Larry L. Biehl			Date: 03/15/2017

void SetNumberClassTrainPixels (
				DialogPtr							dialogPtr,
				SInt64								numberSelectionPixels,
				SInt16								fieldType)

{
	SInt64								numberClassTrainPixels;


   numberClassTrainPixels = 0;
   if (gProjectInfoPtr->currentClass != -1)
      numberClassTrainPixels = (UInt32)gProjectInfoPtr->
      classNamesPtr[gProjectInfoPtr->currentClass].numberTrainPixels;

   if (fieldType != 2)
      numberClassTrainPixels += numberSelectionPixels;

   CreateNumberWithCommasInString ((char*)gTextString2, numberClassTrainPixels);

   gTextString[0] = (UInt8)sprintf ((char*)&gTextString[1],
												"Number train pixels in class: %s",
												(char*)gTextString2);
												//"Number train pixels in class: %ld",
												//numberClassTrainPixels);
	
   LoadDItemString (dialogPtr, IDC_ClassNumberPixels, (Str255*)gTextString);

}	// end "SetNumberClassTrainPixels"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void NewFieldStatMode
//
//	Software purpose:	The purpose of this routine is to put the 
//							project window in the "Select New Field" mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			SaveClusterStatistics in SCluster.cpp
//							CreateNewProject in SProject.cpp
//							StatisticsControl in SStatistics.cpp
//							StatisticsWControlEvent in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1988
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

void NewFieldStatMode (void)

{
			// Local variables and structures

	GrafPtr								savedPort;
   SInt16								previousStatsMode;


   if (gProjectWindow == NULL)
																									return;

   GetPort (&savedPort);
   SetPortWindowPort (gProjectWindow);

	#if defined multispec_mac
				// Force drawing of class prompt and class name.

		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);
		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptClassBox);

				// Invalidate the field pop up box and the field prompt box.

		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpFieldBox);
		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptFieldBox);

				// Invalidate the statistics pop up and prompt boxes.

		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);
		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptCovarianceToUseBox);
	#endif	// defined multispec_mac

			// Indicate the stats window is now in "Select New Field" mode

   previousStatsMode = gProjectInfoPtr->statsWindowMode;
   gProjectInfoPtr->statsWindowMode = kSelectFieldMode;

			// Unhilite the "To Project" control if there are no classes defined

   if (gProjectInfoPtr->numberStatisticsClasses <= 0)
      MHiliteControl (gProjectWindow, gProjectInfoPtr->toProjectControlH, 255);

			// Unhilite the "Add to List" control until a field is selected

   MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 255);

			// Update the hiliting status of the "To Class" control.

   UpdateToClassControl ();

			// Remove cells from previous list

   LSetDrawingMode (FALSE, gStatisticsListHandle);
   RemoveListCells ();

			// Turn list drawing mode back on

   LSetDrawingMode (TRUE, gStatisticsListHandle);

	#if defined multispec_mac
				// Invalidate the list rectangle so that it will be updated

		gTempRect = ((ListPtr)* gStatisticsListHandle)->rView;
		InvalWindowRect (gProjectWindow, &gTempRect);

				// Draw Pop Up Menu prompt and current setting

		DrawClassPopUp ();

				// Force the project window mode title to be redrawn.

		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->projectWindowModeTitleBox);

				// Force the list title to be redrawn.

		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->listTitleBox);
	#endif	// defined multispec_mac 

	#if defined multispec_win
		(gProjectWindow->GetDlgItem (IDC_ListTitle))->
												SetWindowText ((LPCTSTR)_T("Coordinates (L,C)"));

		(gProjectWindow->GetDlgItem (IDC_ProjectWindowMode))->
												SetWindowText ((LPCTSTR)_T("SELECT FIELD"));
	#endif	// defined multispec_win
	
	#if defined multispec_wx 
		(gProjectWindow->GetFrame()->FindWindow (IDC_ListTitle))->
																		SetLabel ("Coordinates (L,C)");

		(gProjectWindow->GetFrame()->FindWindow (IDC_ProjectWindowMode))->
																		SetLabel ("SELECT FIELD");
	#endif	// defined multispec_wx
	
			// Set processor mode to indicate that Statistics processor is
			// active in New Field mode and rectangle or polygonon selection		
			// is on depending on last setting. 

   if (gProjectInfoPtr->selectionType == kRectangleType)
				// rectangle type input
      gStatisticsMode = kStatNewFieldRectMode;

   else	// polygon type input 
      gStatisticsMode = kStatNewFieldPolyMode;

			// Do activate events for "Select New Field" mode.

   if (gProjectWindow == FrontWindow ())
      DoStatWActivateEvent ();

   else	// gProjectWindow != FrontWindow ()
      ActivateStatControls ();

			// Load in the selected area if there is one.

   LoadNewFieldListBox ();

   SetPort (savedPort);

			// If project image window is displayed and previous stats mode was
			// the field list or coordinate list, force an update event so that	
			// all requested fields will be outlined.										

   if (previousStatsMode > kClassListMode)
      OutlineFieldsInProjectWindows (gProjectInfoPtr->statsWindowMode, kDoClearArea);

}	// end "NewFieldStatMode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void PolygonListStatMode
//
//	Software purpose:	The purpose of this routine is to put the 
//							project window in the "Polygon Coordinate List" 
//							mode.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			MouseDnEvents  in MMultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/30/1988
//	Revised By:			Larry L. Biehl			Date: 03/02/2017	

void PolygonListStatMode (
				SInt16								classFieldNumber)

{
			// Local variables and structures 

	HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								fieldNumber;


	#if defined multispec_win
		USES_CONVERSION;
	#endif

   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

			// Convert the current class and relative field number					
			// (classFieldNumber) within the class to the number of the field in	
			// the overall list																	

   fieldNumber = GetCurrentField (gProjectInfoPtr->currentClass, classFieldNumber);

			// Execute routine if input field number is within proper range		

   if (fieldNumber < gProjectInfoPtr->numberStorageFields &&
			fieldNumber >= 0 &&
				fieldIdentPtr[fieldNumber].pointType != kClusterType)
		{
				// Save current field number																		

      gProjectInfoPtr->currentField = fieldNumber;

				// Indicate the stats window now contains a list of points for		
				// the current field 															

      gProjectInfoPtr->statsWindowMode = kCoordinateListMode;

				// Force drawing of field prompt and field name.						

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpFieldBox);
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptFieldBox);

				// Force drawing of statistics pop up and prompt boxes.

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);
      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->promptCovarianceToUseBox);

				// Remove cells from previous list											

      LSetDrawingMode (FALSE, gStatisticsListHandle);
      RemoveListCells ();


      Cell		cell;

      SInt16	numberOfPoints,
					point,
					pointIndex;

				// Put the polygon coordinates into the list									

      numberOfPoints = fieldIdentPtr[fieldNumber].numberOfPolygonPoints;

      if (numberOfPoints > 0)
			{
			#if defined multispec_mac 
				Point cellSize;

				LAddRow (numberOfPoints, 0, gStatisticsListHandle);

				if (((ListPtr)* gStatisticsListHandle)->dataBounds.right == 1)
					LAddColumn (1, 1, gStatisticsListHandle);

				cellSize.h = ((ListPtr)* gStatisticsListHandle)->cellSize.h / 2;
				cellSize.v = ((ListPtr)* gStatisticsListHandle)->cellSize.v;
				LCellSize (cellSize, gStatisticsListHandle);
			#endif	// defined multispec_mac 

         pointIndex = fieldIdentPtr[fieldNumber].firstPolygonPoint;
         if (fieldIdentPtr[fieldNumber].pointType == kPolygonType)
            pointIndex += 2;

         for (point = 0; point < numberOfPoints; point++)
				{
						// Add line-column to the polygon coordinate list				

            cell.h = 0;
            cell.v = point;

				#if defined multispec_mac 
					NumToString (
						(SInt32)gProjectInfoPtr->fieldPointsPtr[pointIndex].line,
						gTextString);
					LSetCell (&(gTextString[1]),
									(SInt16)gTextString[0],
									cell,
									gStatisticsListHandle);

					cell.h = 1;
					NumToString (
						(SInt32)gProjectInfoPtr->fieldPointsPtr[pointIndex].col,
						gTextString);
					LSetCell (&(gTextString[1]), 
									(short int)gTextString[0],
									cell, 
									gStatisticsListHandle);
				#endif	// defined multispec_mac 

				#if defined multispec_win
					sprintf ((char*)gTextString,
								"%ld\t%ld",
								gProjectInfoPtr->fieldPointsPtr[pointIndex].line,
								gProjectInfoPtr->fieldPointsPtr[pointIndex].col);
					cell.v = (SInt16)gStatisticsListHandle->
							InsertString (cell.v, (LPCTSTR)A2T((LPCSTR)gTextString));
				#endif	// defined multispec_win 

				#if defined multispec_wx
					sprintf ((char*)gTextString,
								"%d\t%d",
								gProjectInfoPtr->fieldPointsPtr[pointIndex].line,
								gProjectInfoPtr->fieldPointsPtr[pointIndex].col);
					cell.v = (SInt16)gStatisticsListHandle->
						Insert ((char*)gTextString , cell.v);
				#endif	// defined multispec_wx 
            
            pointIndex++;

				}	// end "for (point=0; ... 

			}	// end "if (numberOfPoints > 0)" 

				// Turn list drawing mode back on											

      LSetDrawingMode (TRUE, gStatisticsListHandle);

				// Invalidate the list rectangle so that it will be updated			

		#if defined multispec_mac 
			gTempRect = ((ListPtr)* gStatisticsListHandle)->rView;
			InvalWindowRect (gProjectWindow, &gTempRect);
		#endif	// defined multispec_mac 

				// Force the project window mode title to be redrawn.	

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->projectWindowModeTitleBox);

				// Force the list title to be redrawn.										

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->listTitleBox);

		#if defined multispec_win 
			(gProjectWindow->GetDlgItem (IDC_ListTitle))->
												SetWindowText ((LPCTSTR)_T("Coordinates (L,C)"));

			(gProjectWindow->GetDlgItem (IDC_ProjectWindowMode))->
												SetWindowText ((LPCTSTR)_T("COORDINATE LIST"));
		#endif	// defined multispec_win 

		#if defined multispec_wx 
			(gProjectWindow->GetFrame()->FindWindow (IDC_ListTitle))->
																		SetLabel ("Coordinates (L,C)");

			(gProjectWindow->GetFrame()->FindWindow (IDC_ProjectWindowMode))->
																			SetLabel ("COORDINATE LIST");
		#endif	// defined multispec_wx 
		
				// Set processor mode to indicate that Statistics processor is 	
				// active in "Field Coordinate List" mode. 								

      gStatisticsMode = kStatPolygonListMode;

				// Do activate events for "Field Coordinate List" mode.				

      DoStatWActivateEvent ();

				// If project image window is displayed and fields are to be		
				// outlined in the image window, force an update event so that		
				// only the current field is outlined.										

      ForceFieldOutlineUpdate (FALSE);

		}	// end "if (fieldNumber<=gProjectInfoPtr->numberStatistics..." 

}	// end "PolygonListStatMode" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void RemoveListCells
//
//	Software purpose:	The purpose of this routine is to remove all cells
//							in the statistics list to prepare for a new list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClearNewFieldList in SSelectionUtility.cpp
//							AddPolyPointStatNewFieldW in SStatistics.cpp
//							ClassListStatMode in SStatistics.cpp
//							FieldListStatMode in SStatistics.cpp
//							LoadPolyStatNewFieldW in SStatistics.cpp
//							LoadRectangleInStatList in SStatistics.cpp
//							NewFieldStatMode in SStatistics.cpp
//							PolygonListStatMode in SStatistics.cpp
//							StatisticsWControlEvent in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/29/1988
//	Revised By:			Larry L. Biehl			Date: 02/09/1996	

void RemoveListCells (void)

{
	#if defined multispec_mac
				// Declare local variables and structures

		Point									cellSize;

		SInt16								count;


				// Delete rows in the list

		count = ((ListPtr)* gStatisticsListHandle)->dataBounds.bottom;
		LDelRow (count, 0, gStatisticsListHandle);

				// Delete column in case last statistics mode was in polygon list.
				// Keep one column in the list.

		if (((ListPtr)* gStatisticsListHandle)->dataBounds.right >= 2)
			{
			count = ((ListPtr)* gStatisticsListHandle)->dataBounds.right - 1;
			LDelColumn (count, 1, gStatisticsListHandle);

			}	// end "if (((ListPtr)*gStatisticsListHandle)->dataBounds.right >= 2)"

				// Adjust cell size for one column.  Use one column as the default

		cellSize.h = ((ListPtr)*gStatisticsListHandle)->rView.right -
													((ListPtr)*gStatisticsListHandle)->rView.left;
		cellSize.v = ((ListPtr)*gStatisticsListHandle)->cellSize.v;
		LCellSize (cellSize, gStatisticsListHandle);
	#endif	// defined multispec_mac 

	#if defined multispec_win
		gStatisticsListHandle->ResetContent ();
	#endif	// defined multispec_win 

	#if defined multispec_wx 
		if (gStatisticsListHandle != NULL)
			gStatisticsListHandle->Clear ();
	#endif	// defined multispec_wx
   
}	// end "RemoveListCells"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetListControlHilite
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for statistics operations on an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1996
//	Revised By:			Larry L. Biehl			Date: 02/10/1996

void SetListControlHilite (void)

{
   SInt16								setting = 255;


   if (gProjectInfoPtr->listFieldFlag || gProjectInfoPtr->listClassFlag)
      setting = 0;

   MHiliteControl (gProjectWindow, gProjectInfoPtr->listControlH, setting);

}	// end "SetListControlHilite"  



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetOutlineAreaOptions
//
//	Software purpose:	The purpose of this routine is to set the options for
//							outlining the training and test areas.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1996
//	Revised By:			Larry L. Biehl			Date: 02/10/1996

void SetOutlineAreaOptions (
				DialogPtr							dialogPtr,
				SInt16								labelFieldCode)

{
   SInt16 setting;


   if (GetDLogControl (dialogPtr, 9) == 1 || GetDLogControl (dialogPtr, 10) == 1)
		{
      setting = (labelFieldCode & 0x0001) ? 1 : 0;
      SetDLogControl (dialogPtr, 11, setting);

      setting = (labelFieldCode & 0x0002) ? 1 : 0;
      SetDLogControl (dialogPtr, 12, setting);

      setting = (labelFieldCode & 0x0004) ? 1 : 0;
      SetDLogControl (dialogPtr, 13, setting);

      SetDLogControlHilite (dialogPtr, 11, 0);
      SetDLogControlHilite (dialogPtr, 12, 0);
      SetDLogControlHilite (dialogPtr, 13, 0);

      ShowDialogItems (dialogPtr, 14, 15);

		}	// end "if (GetDLogControl (dialogPtr, 9) == 1 || ..."

   else	// GetDLogControl (dialogPtr, 9) != 1 && ... 
		{
      SetDLogControl (dialogPtr, 11, 0);
      SetDLogControl (dialogPtr, 12, 0);
      SetDLogControl (dialogPtr, 13, 0);

      SetDLogControlHilite (dialogPtr, 11, 255);
      SetDLogControlHilite (dialogPtr, 12, 255);
      SetDLogControlHilite (dialogPtr, 13, 255);

      HideDialogItems (dialogPtr, 14, 15);

		}	// end "if (GetDLogControl (dialogPtr, 9) == 1 || ..."

}	// end "SetOutlineAreaOptions" 
#endif	// defined multispec_mac



#if defined multispec_mac
//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SetProjectWindowBoxes
//
//	Software purpose:	This routine sets the project windows boxes that are used to
//							draw popup menus in.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			InitializeProjectStructure in SProject.cpp
//							CreateStatisticsWControls in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/29/2001
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

void SetProjectWindowBoxes (
				SInt16*								borderPtr,
				SInt16*								windowCenterPtr,
				SInt16*								windowWidthPtr,
				SInt16*								topStartPtr,
				SInt16								pushButtonSpacing)

{
	SInt16								border,
											top,
											windowCenter,
											windowWidth;


	border = 11;

   if (gProjectWindow != NULL)
		{
				// Initialize some local variables.

      GetWindowPortBounds (gProjectWindow, &gTempRect);
      windowWidth = gTempRect.right - gTempRect.left;
      windowCenter = windowWidth / 2;
      top = 3;

				// Set box for project name

      SetRect (&gProjectInfoPtr->projectNameBox,
						5,
						top,
						windowWidth - 5,
						top + 16);

				// Set box for project window mode title

      top += 18;
      SetRect (&gProjectInfoPtr->projectWindowModeTitleBox,
						border,
						top,
						windowWidth - border,
						top + 16);

				// Set boxes for class prompt and class popup box.

      top += 18;
      SetRect (&gProjectInfoPtr->promptClassBox,
						border,
						top + 1,
						46,
						top + 17);

      SetRect (&gProjectInfoPtr->popUpClassBox,
						47,
						top,
						windowWidth - border,
						top + 18);

				// Set boxes for field prompt and field popup box.

      top += 18;
      SetRect (&gProjectInfoPtr->promptFieldBox,
						border,
						top + 1,
						44,
						top + 17);

      SetRect (&gProjectInfoPtr->popUpFieldBox,
						44,
						top,
						windowWidth - border,
						top + 18);

				// Set boxes for statistics selection prompt and popup menu.

      top += 18;
      top += pushButtonSpacing + 2;
      SetRect (&gProjectInfoPtr->promptCovarianceToUseBox,
						border,
						top + 1,
						44,
						top + 17);

      SetRect (&gProjectInfoPtr->popUpCovarianceToUseBox,
						48,
						top,
						windowWidth - border,
						top + 18);

				// Load in coordinates of list title rectangle.							

      SetRect (&gProjectInfoPtr->listTitleBox,
						border,
						gStatisticsWindowControlHeight - 17,
						windowWidth - border,
						gStatisticsWindowControlHeight - 2);

				// Return values if needed.

      if (borderPtr != NULL)
			{
         *borderPtr = border;
         *windowCenterPtr = windowCenter;
         *windowWidthPtr = windowWidth;
         *topStartPtr = top - pushButtonSpacing;

			}	// end "if (borderPtr != NULL)"

		}	// end "if (gProjectWindow != NULL)"

}	// end "SetProjectWindowBoxes" 
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetProjectWTitle
//
//	Software purpose:	This routine set the title for the project window.
//
//	Parameters in:		Pointer to title string.
//
//	Parameters out:	None.
//
// Value Returned:	None.
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 01/19/2001
//	Revised By:			Larry L. Biehl			Date: 11/27/2019

void SetProjectWTitle (void)

{
			// Set the project window title

	//GetCopyOfPFileNameFromProjectInfo (gProjectInfoPtr, (UCharPtr)gTextString);

	gTextString[0] = 0;
	ConcatPStrings (gTextString, (StringPtr)"\0Project\0", 255);

	MSetWindowTitle ((StatisticsWindowPtr)gProjectWindow,
							(UChar*)gTextString);

}	// end "SetProjectWTitle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetStatControlTitle
//
//	Software purpose:	The purpose of this routine is to hide the input control
//							in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ActivateStatControls
//							FieldListStatMode
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1996
//	Revised By:			Larry L. Biehl			Date: 10/19/2018

void SetStatControlTitle (
			ControlHandle							controlHandle,
			char*										titleStringPtr)

{
			// Declare local variables and structures

	#if defined multispec_mac
		//titleStringPtr[0] = (UInt8)strlen (&titleStringPtr[1]);
		SetControlTitle (controlHandle, (UCharPtr)titleStringPtr);
	#endif	// defined multispec_mac 

	#if defined multispec_win
		USES_CONVERSION;
	
		((CMStatisticsForm*)gProjectWindow)->
					SetDlgItemText ((int)controlHandle, (LPCTSTR)A2T(&titleStringPtr[1]));
	#endif	// defined multispec_win

	#if defined multispec_wx
		SInt64 windowid64 = (SInt64)((int*)(controlHandle));
		SInt16 windowid = (SInt16)windowid64;
		wxString titlestring (&titleStringPtr[1], wxConvUTF8);
		((CMStatisticsView*)gProjectWindow)->GetFrame()->
													FindWindow (windowid)->SetLabel (titlestring);
	#endif	// defined multispec_wx
   
}	// end "SetStatControlTitle"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ShowStatControl
//
//	Software purpose:	The purpose of this routine is to hide the input control
//							in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ActivateStatControls
//
//	Coded By:			Larry L. Biehl			Date: 02/01/1996
//	Revised By:			Larry L. Biehl			Date: 02/01/1996	

void ShowStatControl (
				ControlHandle						controlHandle)

{
			// Declare local variables and structures

	#if defined multispec_mac
		ShowControl (controlHandle);
	#endif	// defined multispec_mac

	#if defined multispec_win
		((CMStatisticsForm*)gProjectWindow)->
										GetDlgItem ((int)controlHandle)->ShowWindow (SW_SHOW);
	#endif	// defined multispec_win

  #if defined multispec_wx
		SInt64 windowid64 = (SInt64)((int*)(controlHandle));
		SInt16 windowid = (SInt16)windowid64;
		((CMStatisticsView*)gProjectWindow)->GetFrame()->
																FindWindow (windowid)->Show (true);
	#endif	// defined multispec_wx
	
}	// end "ShowStatControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void StatisticsControl
//
//	Software purpose:	The purpose of this routine is to control the user
//							requests for statistics operations on an image file.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 09/26/1988
//	Revised By:			Larry L. Biehl			Date: 04/28/2017

void StatisticsControl (void)

{
	WindowInfoPtr						projectWindowInfoPtr;
	SInt16*								channelPtr;

	SInt16								totalNumberChannels,
											statisticsRequest;


			// Initialize local variables.

			// Load in project information file if needed and set up 				
			// project window if it hasn't been set up already. 

   if (gProjectInfoPtr == NULL)
		{
      OpenNewProject ();
 
		}	// end "if gProjectInfoPtr == NULL)"

			// Display statistics dialog for user settings.

			// Get file information pointer if we can.  In this case it is
			// not mandantory that we have it.  It will be used to display 	
			// channel description in the channels dialog list if the 			
			// descriptions exist.															

   if (GetProjectImageFileInfo (kDoNotPrompt, kSetupGlobalInfoPointersIfCan))
		{
				// Get pointer to memory for temporary storage of channel list.

      projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
															gProjectInfoPtr->windowInfoHandle);
      totalNumberChannels = (SInt16)projectWindowInfoPtr->totalNumberChannels;
      channelPtr =
					(SInt16*)MNewPointer ((SInt32)totalNumberChannels * sizeof (SInt16));
      statisticsRequest = 0;

      if (channelPtr != NULL)
         statisticsRequest = StatisticsDialog (channelPtr, totalNumberChannels);

      CheckAndDisposePtr ((Ptr)channelPtr);

      if (statisticsRequest > 0)
			{
         if (gProjectWindow == NULL)
            CreateProjectWindow ();

         gProjectInfoPtr->newProjectFlag = FALSE;

			}	// end "if (statisticsRequest > 0)"

      UnlockProjectWindowInfoHandles ();

				// If new project was initialized at the beginning, then	dispose of the
				// project structure.

      if (gProjectInfoPtr->newProjectFlag)
         CloseProjectStructure (gProjectInfoPtr);

				// Move project memory if needed to keep from fragmenting the memory.
				// The size of the project class and field structures may have changed
				// if masks were set up.

      if (gProjectInfoPtr != NULL && gProjectInfoPtr->moveMemoryFlag)
			{
         UnlockProjectMemory (&gProjectInfoPtr, 2, NULL);
         LockProjectMemory (NULL, 0, &gProjectInfoPtr);

         gProjectInfoPtr->moveMemoryFlag = FALSE;

			}	// end "else if (gProjectInfoPtr != NULL && ..."

				// Scroll output window to the selection and adjust the scroll bar
				// in case text was written to the text window.															

      UpdateOutputWScrolls (gOutputWindow, 1, kDisplayMessage);

		}	// end "if (GetProjectImageFileInfo (kDoNotPrompt, ..."

   else	// if (!GetProjectImageFileInfo (kDoNotPrompt, ..." 
      UnlockProjectWindowInfoHandles ();

}	// end "StatisticsControl" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 StatisticsDialog
//
//	Software purpose:	The purpose of this routine is to handle the 		
//							modal dialog for selecting the statistics function
//							to be done. 
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
//	Coded By:			Larry L. Biehl			Date: 09/26/1988
//	Revised By:			Larry L. Biehl			Date: 04/08/2019

SInt16 StatisticsDialog (
				SInt16*								featurePtr,
				SInt16								totalNumberChannels)

{
	Handle								testMaskFileInfoHandle,
											trainMaskFileInfoHandle;

   SInt16								savedLabelFieldCode,
											savedOutlineFieldType,
											statisticsRequest = 0;

   Boolean								labelAreasFlagChanged;


   savedOutlineFieldType = gProjectInfoPtr->outlineFieldType;

   savedLabelFieldCode = gProjectInfoPtr->labelFieldCode;

   testMaskFileInfoHandle = NULL;
   trainMaskFileInfoHandle = NULL;

#if defined multispec_mac
   Rect									theBox;

   double								minLogDeterminant,
											variance;

   DialogPtr							dialogPtr;

   UserItemUPP							drawProjectChangesPopUpPtr,
											outlineColorPopUpPtr,
											selectTestImageMaskPopUpPtr,
											selectTrainImageMaskPopUpPtr;

   Handle								okHandle,
											theHandle;

   SInt16								channelListType,
											index,
											itemHit,
											localLabelFieldCode,
											localNumberChannels,
											localOutlineFieldType,
											localStatCode,
											savedChannelListType,
											setting,
											theType;
											
	UInt16								maxNumberTestLayers,
											maxNumberTrainLayers,
											trainLayerNumber,
											testLayerNumber;

   Boolean								checkChannelNumberFlag,
											keepClassStatsFlag,
											setLayerPromptFlag,
											modalDone,
											useCommonCovarianceInLOOCFlag;


			// Get the modal dialog for the reformat specification

   dialogPtr = LoadRequestedDialog (kStatisticsSpecificationID, kCopyScrap, 1, 2);
   if (dialogPtr == NULL)
																								return (0);

	checkChannelNumberFlag = FALSE;
	
					// Initialize local user item proc pointers.

   drawProjectChangesPopUpPtr = NewUserItemUPP (DrawProjectChangesPopUp);
   outlineColorPopUpPtr = NewUserItemUPP (DrawOutlineColorPopUp);
   selectTestImageMaskPopUpPtr = NewUserItemUPP (DrawSelectTestImageMaskPopUp);
   selectTrainImageMaskPopUpPtr = NewUserItemUPP (DrawSelectTrainImageMaskPopUp);

			// Set Procedure pointers for those dialog items that need them.

   SetDialogItemDrawRoutine (dialogPtr, 4, drawProjectChangesPopUpPtr);

   SetDialogItemDrawRoutine (dialogPtr, 6, gDrawChannelsPopUpPtr);

   SetDialogItemDrawRoutine (dialogPtr, 15, outlineColorPopUpPtr);

   SetDialogItemDrawRoutine (
							dialogPtr, IDC_TrainMaskPopUp, selectTrainImageMaskPopUpPtr);

   SetDialogItemDrawRoutine (dialogPtr, 21, selectTestImageMaskPopUpPtr);

			// Initialize dialog variables.

   StatisticsDialogInitialize (dialogPtr,
											totalNumberChannels,
											&localStatCode,
											&keepClassStatsFlag,
											&useCommonCovarianceInLOOCFlag,
											&variance,
											&minLogDeterminant,
											&gChannelSelection,
											&channelListType,
											(UInt16*)featurePtr,
											(UInt16*)&localNumberChannels,
											&localOutlineFieldType,
											&localLabelFieldCode,
											&gOutlineColorSelection,
											&gMaskTrainImageSelection,
											&gMaskTestImageSelection,
											&maxNumberTrainLayers,
											&maxNumberTestLayers);

			// Save handle for the OK button for use later.

   GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

			// Outline training areas.

   setting = (localOutlineFieldType & 0x0001) ? 1 : 0;
   SetDLogControl (dialogPtr, 9, setting);

			// Outline test areas.

   setting = (localOutlineFieldType & 0x0002) ? 1 : 0;
   SetDLogControl (dialogPtr, 10, setting);

			// Show labels with area outlines.

   SetOutlineAreaOptions (dialogPtr, localLabelFieldCode);

			// Set up the outline color popup menu.

   AppendMenu (gPopUpTemporaryMenu, "\pBlack");
   AppendMenu (gPopUpTemporaryMenu, "\pWhite");

			// Center the dialog and then show it.

   ShowDialogWindow (
					dialogPtr, kStatisticsSpecificationID, kSetUpDFilterTable);
	
	if (gMaskTrainImageSelection == 3)
		SelectDialogItemText (dialogPtr, IDC_TrainMaskLayer, 0, SInt16_MAX);
	else if (gMaskTestImageSelection == 3)
		SelectDialogItemText (dialogPtr, IDC_TestMaskLayer, 0, SInt16_MAX);

   modalDone = FALSE;
	setLayerPromptFlag = FALSE;
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
            case 4: // Project changes pop up menu
               savedChannelListType = channelListType;
               channelListType = ProjectChangesPopUpMenu (
																	dialogPtr,
																	&theBox,
																	&gProjectInfoPtr->newProjectFlag,
																	channelListType);

							// If project stats were cleared, then reload the local
							// feature vector to include all channels.

               if (savedChannelListType == kSelectedItemsListOnly &&
																channelListType == kSelectItemsList)
						{
                  gChannelSelection = kAllMenuItem;
                  for (index = 0;
								index < gProjectInfoPtr->numberStatisticsChannels;
									index++)
                     featurePtr[index] = index;
                  localNumberChannels = gProjectInfoPtr->numberStatisticsChannels;

                  GetDialogItem (dialogPtr, 6, &theType, &theHandle, &theBox);
                  InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);

						}	// end "if (savedChannelListType == 3 && ..."
               break;

            case 6: // Selected channels 
               itemHit = ChannelsPopUpMenu (dialogPtr,
															6,
															1,
															gChannelSelection,
															channelListType);

               if (itemHit == 2 ||
								(itemHit == 1 && channelListType == kSelectedItemsListOnly))
						{
								// Subset of channels to be used.

                  HiliteControl ((ControlHandle)okHandle, 255);

                  ChannelsDialog (&localNumberChannels,
												featurePtr,
												gImageLayerInfoPtr,
												gImageFileInfoPtr,
												channelListType,
												kNoTransformation,
												NULL,
												totalNumberChannels,
												gChannelSelection);

                  HiliteControl ((ControlHandle)okHandle, 0);

                  if (localNumberChannels == totalNumberChannels)
                     itemHit = 1;

						}	// end "if (itemHit == 2 || ...)"

               if (channelListType == kSelectItemsList && itemHit != 0)
                  gChannelSelection = itemHit;

							// Make certain that the correct label is drawn in the
							// channel pop up box.

               InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
               break;

            case 7: // Project statistics options. 
               HiliteControl ((ControlHandle)okHandle, 255);
               StatisticsOptionsDialog (
                  &localStatCode,
                  &keepClassStatsFlag,
                  &variance,
                  &minLogDeterminant,
                  &useCommonCovarianceInLOOCFlag);

               HiliteControl ((ControlHandle)okHandle, 0);
               //itemHit = 0;
               break;

            case 9: // Outline training fields. 
            case 10: // Outline test fields. 
               ChangeDLogCheckBox ((ControlHandle)theHandle);

               SetOutlineAreaOptions (dialogPtr, localLabelFieldCode);
               break;

            case 11: // List class names with field outlines. 
            case 12: // List field names with field outlines. 
            case 13: // List train/test identifier with field outlines. 
               ChangeDLogCheckBox ((ControlHandle)theHandle);

               localLabelFieldCode = 0;
               if (GetDLogControl (dialogPtr, 11) == 1)
                  localLabelFieldCode += 1;

               if (GetDLogControl (dialogPtr, 12) == 1)
                  localLabelFieldCode += 2;

               if (GetDLogControl (dialogPtr, 13) == 1)
                  localLabelFieldCode += 4;
               break;

            case 15: // Outline color
               itemHit = StandardPopUpMenu (dialogPtr,
                  itemHit - 1,
                  itemHit,
                  gPopUpTemporaryMenu,
                  gOutlineColorSelection,
                  kColorMenuID);

               if (itemHit != 0)
                  gOutlineColorSelection = itemHit;
               break;

            case IDC_TrainMaskPopUp: // Mask image for training
					itemHit = StandardPopUpMenu (dialogPtr,
															itemHit - 1,
															itemHit,
															gPopUpSelectTrainMaskImageMenu,
															gMaskTrainImageSelection,
															kPopUpSelectMaskImage1MenuID);

               if (itemHit != 0)
						{
                  itemHit = StatisticsDialogSelectMaskItem (
																	&trainMaskFileInfoHandle,
																	dialogPtr,
																	gPopUpSelectTrainMaskImageMenu,
																	itemHit,
																	gMaskTrainImageSelection,
																	IDS_FileIO95,
																	IDC_TrainMaskPopUp,
																	IDC_TrainMaskLayer,
																	&maxNumberTrainLayers);

                  gMaskTrainImageSelection = itemHit;
                  InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						setLayerPromptFlag = TRUE;

						}	// end "if (itemHit != 0)"
               break;

            case 21: // Mask image for testing
               itemHit = StandardPopUpMenu (dialogPtr,
															itemHit - 1,
															itemHit,
															gPopUpSelectTestMaskImageMenu,
															gMaskTestImageSelection,
															kPopUpSelectMaskImage2MenuID);

               if (itemHit != 0)
						{
                  itemHit = StatisticsDialogSelectMaskItem (
																		&testMaskFileInfoHandle,
																		dialogPtr,
																		gPopUpSelectTestMaskImageMenu,
																		itemHit,
																		gMaskTestImageSelection,
																		IDS_FileIO96,
																		21,
																		IDC_TestMaskLayer,
																		&maxNumberTestLayers);

                  gMaskTestImageSelection = itemHit;
                  InvalWindowRect (GetDialogWindow (dialogPtr), &theBox);
						
						setLayerPromptFlag = TRUE;

						}	// end "if (itemHit != 0)"
               break;
					
				case IDC_TrainMaskLayer: // channel for training mask
					break;
					
				case IDC_TestMaskLayer: // channel for test mask
					break;

				}	// end "switch (itemHit)"
			
			if (setLayerPromptFlag)
				{
				if (gMaskTrainImageSelection != 1 || gMaskTestImageSelection != 1)
					ShowDialogItem (dialogPtr, IDC_LayerPrompt);
				else
					HideDialogItem (dialogPtr, IDC_LayerPrompt);
					
				setLayerPromptFlag = FALSE;
					
				}	// end "if (setLayerPromptFlag)"

			}	// end "if (itemHit > 2)"

      else	// itemHit <= 2 
			{
					// User selected OK for information.

					// Check channels used for train and test mask
			
			trainLayerNumber = 1;
			if (itemHit == 1 &&
					gMaskTrainImageSelection == 3 &&
							maxNumberTrainLayers > 1)
				{
				itemHit = CheckMaxValue (dialogPtr,
												  IDC_TrainMaskLayer,
												  1,
												  maxNumberTrainLayers,
												  kDisplayRangeAlert);
													  
				trainLayerNumber =  GetDItemValue (dialogPtr, IDC_TrainMaskLayer);
													  
				}	// end "if (itemHit == 1 && gMaskTrainImageSelection == 3 && ..."
													  
			testLayerNumber = 1;
			if (itemHit == 1 && gMaskTestImageSelection == 3 && maxNumberTestLayers > 1)
				{
				itemHit = CheckMaxValue (dialogPtr,
												  IDC_TestMaskLayer,
												  1,
												  maxNumberTestLayers,
												  kDisplayRangeAlert);	
													  
				testLayerNumber =  GetDItemValue (dialogPtr, IDC_TestMaskLayer);
							
				}	// end "if (itemHit == 1 && gMaskTestImageSelection == 3 && ..."
			
					// Load the mask data into memory if a mask file was selected.

         if (itemHit == 1)
            itemHit = StatisticsDialogMaskCheck (trainMaskFileInfoHandle,
																	testMaskFileInfoHandle,
																	gMaskTrainImageSelection,
																	gMaskTestImageSelection,
																	trainLayerNumber,
																	testLayerNumber);

         if (itemHit == 1)
				{
						// User selected OK for information and any mask file was read
						// into memory okay.

            modalDone = true;
            statisticsRequest = 1;

            StatisticsDialogOK (gChannelSelection,
											totalNumberChannels,
											(UInt16*)featurePtr,
											localNumberChannels,
											(GetDLogControl (dialogPtr, 9) == 1),
											(GetDLogControl (dialogPtr, 10) == 1),
											(GetDLogControl (dialogPtr, 11) == 1),
											(GetDLogControl (dialogPtr, 12) == 1),
											(GetDLogControl (dialogPtr, 13) == 1),
											gOutlineColorSelection,
											localStatCode,
											keepClassStatsFlag,
											variance,
											minLogDeterminant,
											useCommonCovarianceInLOOCFlag);

				}	// end "if (itemHit == 1)"

         if (itemHit == 2) // User selected Cancel for information 
				{
            modalDone = true;
            statisticsRequest = 0;

				}	// end "if	(itemHit == 2)"

			}	// end "else itemHit <= 2"

		}	while (!modalDone);

   DisposeUserItemUPP (drawProjectChangesPopUpPtr);
   DisposeUserItemUPP (outlineColorPopUpPtr);
   DisposeUserItemUPP (selectTestImageMaskPopUpPtr);
   DisposeUserItemUPP (selectTrainImageMaskPopUpPtr);

			// Now delete the menu items.

   UInt32 item;
   for (item = 0; item < 2; item++)
      DeleteMenuItem (gPopUpTemporaryMenu, 1);

   //MHSetState (gProjectInfoPtr->windowInfoHandle, handleStatus);

   CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac

	#if defined multispec_win
		CMStatisticsDialog* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMStatisticsDialog ();

			statisticsRequest = dialogPtr->DoDialog (featurePtr,
																	totalNumberChannels,
																	&trainMaskFileInfoHandle,
																	&testMaskFileInfoHandle);

			delete dialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			statisticsRequest = 0;
			}
		END_CATCH_ALL
	#endif // defined multispec_win

	#if defined multispec_wx
		CMStatisticsDialog* dialogPtr = NULL;

		dialogPtr = new CMStatisticsDialog (NULL);
		statisticsRequest = dialogPtr->DoDialog (featurePtr,
																totalNumberChannels,
																&trainMaskFileInfoHandle,
																&testMaskFileInfoHandle);

		delete dialogPtr;
	#endif // defined multispec_wx

			// Dispose of temporary handle for mask file structures if needed.

   DisposeFileInfoHandle (trainMaskFileInfoHandle);
   trainMaskFileInfoHandle = NULL;

   DisposeFileInfoHandle (testMaskFileInfoHandle);
   testMaskFileInfoHandle = NULL;

			// Draw field boundaries or inval the project image window				
			// to get the correct set of field boundaries drawn or 					
			// removed.																				

   labelAreasFlagChanged = (savedLabelFieldCode != 0);
   if (savedLabelFieldCode != gProjectInfoPtr->labelFieldCode)
      labelAreasFlagChanged = TRUE;

   if ((savedOutlineFieldType != gProjectInfoPtr->outlineFieldType ||
																			labelAreasFlagChanged))
		{
      if ((savedOutlineFieldType != 0 && gProjectInfoPtr->outlineFieldType != 3) ||
									((savedLabelFieldCode & 0x0001) &&
												!(gProjectInfoPtr->labelFieldCode & 0x0001)) ||
									((savedLabelFieldCode & 0x0002) &&
												!(gProjectInfoPtr->labelFieldCode & 0x0002)))
         ForceFieldOutlineUpdate (TRUE);

      else	// Just draw in the changes to what is already there. 
         OutlineFieldsInProjectWindows (gProjectInfoPtr->statsWindowMode,
														kDoClearArea);

		}	// end "if (outlineFieldType != ..."

   return (statisticsRequest);

}	// end "StatisticsDialog"



void StatisticsDialogInitialize (
				DialogPtr								dialogPtr,
				SInt16									totalNumberChannels,
				SInt16*									localStatCodePtr,
				Boolean*									keepClassStatsFlagPtr,
				Boolean*									useCommonCovarianceInLOOCFlagPtr,
				double*									variancePtr,
				double*									minLogDeterminantPtr,
				SInt16*									channelSelectionPtr,
				SInt16*									channelListTypePtr,
				UInt16*									featurePtr,
				UInt16*									localNumberChannelsPtr,
				SInt16*									localOutlineFieldTypePtr,
				SInt16*									localLabelFieldCodePtr,
				SInt16*									outlineColorSelectionPtr,
				SInt16*									maskTrainImageSelectionPtr,
				SInt16*									maskTestImageSelectionPtr,
				UInt16*									maxNumberTrainLayersPtr,
				UInt16*									maxNumberTestLayersPtr)

{
	UInt32									index;


			// Initialize local variables.

	*localStatCodePtr = gProjectInfoPtr->statisticsCode;

   *keepClassStatsFlagPtr = gProjectInfoPtr->keepClassStatsOnlyFlag;
   *useCommonCovarianceInLOOCFlagPtr = gProjectInfoPtr->useCommonCovarianceInLOOCFlag;

   *variancePtr = (double)gProjectInfoPtr->zeroVarianceFactor;
   if (!gProjectInfoPtr->setZeroVarianceFlag)
      *variancePtr = -(double)gProjectInfoPtr->zeroVarianceFactor;

   *minLogDeterminantPtr = gLowerDeterminantOffsetAllowed;

			//	Get the rectangles for the channel prompt and popup boxes.
			// Draw the default channel selection.											

   *channelSelectionPtr = kAllMenuItem;
   if (gProjectInfoPtr->numberStatisticsChannels != totalNumberChannels)
      *channelSelectionPtr = kSubsetMenuItem;

   *channelListTypePtr = kSelectItemsList;
   if (gProjectInfoPtr->statsLoaded || gProjectInfoPtr->pixelDataLoadedFlag)
      *channelListTypePtr = kSelectedItemsListOnly;

			// Load current feature pointer into a temporary vector and number
			// channels into temporary variable.  This is so that information in	
			// project structure will not be changed unless user click OK.			

   for (index=0; index<(UInt32)gProjectInfoPtr->numberStatisticsChannels; index++)
      featurePtr[index] = gProjectInfoPtr->channelsPtr[index];

   *localNumberChannelsPtr = (UInt16)gProjectInfoPtr->numberStatisticsChannels;

   *localOutlineFieldTypePtr = gProjectInfoPtr->outlineFieldType;

			// Show labels with area outlines.

   *localLabelFieldCodePtr = gProjectInfoPtr->labelFieldCode;

			// Set global for drawing the "outline color" popup menu.

   *outlineColorSelectionPtr = gProjectInfoPtr->outlineColorCode;

			// Set the train and test mask dialog items.
	
   *maskTrainImageSelectionPtr = StatisticsDialogSetMaskItems (
																		&gProjectInfoPtr->trainingMask,
																		gPopUpSelectTrainMaskImageMenu,
																		dialogPtr,
																		IDC_TrainMaskPopUp,
																		IDC_TrainMaskLayer,
																		maxNumberTrainLayersPtr);

   *maskTestImageSelectionPtr = StatisticsDialogSetMaskItems (
																		&gProjectInfoPtr->testMask,
																		gPopUpSelectTestMaskImageMenu,
																		dialogPtr,
																		IDC_TestMaskPopUp,
																		IDC_TestMaskLayer,
																		maxNumberTestLayersPtr);
	
	if (*maskTrainImageSelectionPtr != 1 || *maskTestImageSelectionPtr != 1)
      ShowDialogItem (dialogPtr, IDC_LayerPrompt);
	else
      HideDialogItem (dialogPtr, IDC_LayerPrompt);
		
	HideDialogItem (dialogPtr, IDC_TrainMaskLayerEdit);
	HideDialogItem (dialogPtr, IDC_TestMaskLayerEdit);

}	// end "StatisticsDialogInitialize"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 StatisticsDialogMaskCheck
//
//	Software purpose:	The purpose of this routine is to handle the mask requests by
//							the user. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	0 if something did not get handled. Allow user chance to change
//									the statistics dialog box request
//							1 if everything handled okay.
//
// Called By:			StatisticsDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/??/1999
//	Revised By:			Larry L. Biehl			Date: 05/04/2017

SInt16 StatisticsDialogMaskCheck (
				Handle								trainMaskFileInfoHandle,
				Handle								testMaskFileInfoHandle,
				SInt16								maskTrainImageSelection,
				SInt16								maskTestImageSelection,
				UInt16								trainLayerNumber,
				UInt16								testLayerNumber)

{
	CMFileStream						*maskFileStreamPtr;
											
   SInt16								alertString,
											itemHit = 1;

   Boolean								maskLoadedFlag = FALSE,
											testMaskFileInfoHandleFlag = FALSE,
											trainMaskFileInfoHandleFlag = FALSE;
											
	SignedByte							maskFileStreamHandleStatus;


   alertString = IDS_Alert55;

   if (maskTrainImageSelection == 3 || maskTestImageSelection == 3)
		{
				// Change cursor to spin cursor until done with process.

      MSetCursor (kWait);
      gPresentCursor = kSpin;

      if (maskTrainImageSelection == 3)
			{
					// if the trainMaskFileInfoHandle is null, then this implies
					// that the same mask file is being used but with a different
					// layer. Verify this and if so, get a file info handle.
					
			maskFileStreamPtr = GetMaskFileStreamPointer (&gProjectInfoPtr->trainingMask,
																			&maskFileStreamHandleStatus);
					
			if (trainMaskFileInfoHandle == NULL && 
								gProjectInfoPtr->trainingMask.fileLayer != trainLayerNumber &&
											maskFileStreamPtr != NULL)
				{
				trainMaskFileInfoHandle = LoadMaskFileInfo (maskFileStreamPtr);					
				trainMaskFileInfoHandleFlag = TRUE;
					
				}	// end "if (trainMaskFileInfoHandle == NULL && ..."
			
			if (trainMaskFileInfoHandle != NULL)
				itemHit = LoadMask (kTrainingMaskSet,
											trainMaskFileInfoHandle,
											&gProjectInfoPtr->trainingMask,
											trainLayerNumber,
											kNewMaskFields);

         if (trainMaskFileInfoHandle != NULL)
            maskLoadedFlag = TRUE;
								
			MHSetState (gProjectInfoPtr->trainingMask.fileStreamHandle,
								maskFileStreamHandleStatus);
			
			if (trainMaskFileInfoHandleFlag)
						// It was defined in this routine. Dispose of the handle.
				DisposeFileInfoHandle (trainMaskFileInfoHandle);

			}	// end "if (maskTrainImageSelection == 3)"

      if (itemHit == 1 && maskTestImageSelection == 3)
			{
         alertString = IDS_Alert56;
			
					// if the trainMaskFileInfoHandle is null, then this implies
					// that the same mask file is being used but with a different
					// layer. Verify this and if so, get a file info handle.
					
			maskFileStreamPtr = GetMaskFileStreamPointer (&gProjectInfoPtr->testMask,
																			&maskFileStreamHandleStatus);
					
			if (testMaskFileInfoHandle == NULL && 
								gProjectInfoPtr->testMask.fileLayer != testLayerNumber &&
											maskFileStreamPtr != NULL)
				{
				testMaskFileInfoHandle = LoadMaskFileInfo (maskFileStreamPtr);					
				testMaskFileInfoHandleFlag = TRUE;
					
				}	// end "if (trainMaskFileInfoHandle == NULL && ..."
				
			if (testMaskFileInfoHandle != NULL)
				itemHit = LoadMask (kTestMaskSet,
											testMaskFileInfoHandle,
											&gProjectInfoPtr->testMask,
											testLayerNumber,
											kNewMaskFields);

         if (testMaskFileInfoHandle != NULL)
            maskLoadedFlag = TRUE;
			
			if (testMaskFileInfoHandleFlag)
						// It was defined in this routine. Dispose of the handle.
				DisposeFileInfoHandle (testMaskFileInfoHandle);

			}	// end "if (itemHit == 1 && maskTestImageSelection == 3)"

      MInitCursor ();

		}	// end "if (maskTrainImageSelection == 3 || ..."

   if (itemHit == 0)
      DisplayAlert (kErrorAlertID,
							kCautionAlert,
							kAlertStrID,
							alertString,
							0,
							NULL);

   if (itemHit == 1)
		{
		if (maskTrainImageSelection == 1)
			{
			CutMaskFields (kTrainingMaskSet);
			CloseMaskStructure (&gProjectInfoPtr->trainingMask);

			}	// end "if (maskTrainImageSelection == 1)"

		if (maskTestImageSelection == 1)
			{
			CutMaskFields (kTestMaskSet);
			CloseMaskStructure (&gProjectInfoPtr->testMask);

			}	// end "if (maskTestImageSelection == 1)"

		if (gProjectWindow != NULL)
			{
					// Update the class list if needed
				
			if (gProjectInfoPtr->statsWindowMode == kClassListMode)
				ClassListStatMode ();

					// Update the field list if needed
				
			else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
				FieldListStatMode (gProjectInfoPtr->currentClass);
				
			}	// end "if (gProjectWindow != NULL)"
			
		if (maskLoadedFlag)
			{
					// Get pointer to the project file name and list a message in the
					// output text window that mask file(s) were read in.
					// The project file name is the same as that for the output text
					// window.

			GetProjectWindowTitle ((UCharPtr)gTextString);

			ForceTextToEnd ();

			ListSpecifiedStringNumber (kProjectStrID,
												IDS_Project63,
												NULL,
												gOutputForce1Code,
												gTextString,
												TRUE);
			
			}	// end "if (maskLoadedFlag)"

		}	// end "if (itemHit == 1)"

   return (itemHit);

}	// end "StatisticsDialogMaskCheck"



SInt16 StatisticsDialogSetMaskItems (
				MaskInfoPtr								maskInfoPtr,
				MenuHandle								popUpSelectMaskImageMenu,
				DialogPtr								dialogPtr,
				SInt16									maskPopupItemNumber,
				SInt16									layerItemNumber,
				UInt16*									maxNumberLayersPtr)

{
	FileStringPtr							fileNamePPointer;

   SInt16									maskSelection;

   SignedByte								maskFileStreamHandleStatus;


			// Set the train and test mask dialog items.

	#if defined multispec_mac
		SetMenuItemText (popUpSelectMaskImageMenu,
								1,
								"\pNone");
	#endif	// defined multispec_mac

	#if defined multispec_win
		dialogPtr->SetComboItemText (maskPopupItemNumber,
												0,
												(UCharPtr)"None");
	#endif	// defined multispec_win 

	#if defined multispec_mac
		SetMenuItemText (popUpSelectMaskImageMenu,
								2,
								"\pSelect mask image...");
	#endif	// defined multispec_mac

	#if defined multispec_win
		dialogPtr->SetComboItemText (maskPopupItemNumber,
												1,
												(UCharPtr)"Select mask image...");
	#endif	// defined multispec_win 

   if (maskInfoPtr->maskHandle != NULL)
		{
      maskSelection = 3;

      fileNamePPointer = GetMaskFileNamePPointer (maskInfoPtr,
																	&maskFileStreamHandleStatus);

      StatisticsDialogSetMaskFileName (fileNamePPointer,
													popUpSelectMaskImageMenu,
													dialogPtr,
													maskPopupItemNumber);

      MHSetState (maskInfoPtr->fileStreamHandle, maskFileStreamHandleStatus);

		if (maskInfoPtr->numberLayers == 1)
			{
			SetDialogItemToStaticText (dialogPtr, layerItemNumber);
			
			}	// end "if (maskInfoPtr->numberLayers == 1)"
		
		else	// maskInfoPtr->numberLayers > 1
			{
			SetDialogItemToEditText (dialogPtr, layerItemNumber);
			
			}	// end "else maskInfoPtr->numberLayers > 1"
		
		LoadDItemValue (dialogPtr, layerItemNumber, maskInfoPtr->fileLayer);
		
		*maxNumberLayersPtr = (UInt16)maskInfoPtr->numberLayers;

		}	// end "if (maskInfoPtr->maskHandle != NULL)"

   else	// maskInfoPtr->maskHandle == NULL
		{
      maskSelection = 1;

		#if defined multispec_mac
			DeleteMenuItem (popUpSelectMaskImageMenu, 3);
		#endif	// defined multispec_mac

		#if defined multispec_win
			if (((CComboBox*)dialogPtr->GetDlgItem (
															maskPopupItemNumber))->GetCount () == 3)
				((CComboBox*)dialogPtr->GetDlgItem (
													maskPopupItemNumber))->DeleteString (2);
		#endif	// defined multispec_win

      HideDialogItem (dialogPtr, layerItemNumber);
		
		*maxNumberLayersPtr = 1;

		}	// end "else maskInfoPtr->maskHandle == NULL"

	return (maskSelection);

}	// end "StatisticsDialogSetMaskItems"



void StatisticsDialogSetMaskFileName (
				UCharPtr									fileNamePPointer,
				MenuHandle								popUpSelectMaskImageMenu,
				DialogPtr								dialogPtr,
				SInt16									maskPopupItemNumber)

{
	#if defined multispec_mac
		if (CountMenuItems (popUpSelectMaskImageMenu) < 3)
			InsertMenuItem (popUpSelectMaskImageMenu, "\p1", 3);

		MSetMenuItemText (popUpSelectMaskImageMenu,
								3,
								fileNamePPointer,
								kASCIICharString);
	#endif	// defined multispec_mac

	#if defined multispec_win
		int count = ((CComboBox*)dialogPtr->GetDlgItem (
																	maskPopupItemNumber))->GetCount ();
		if (count < 3)
			dialogPtr->SetComboItemText (maskPopupItemNumber,
													count+1,
													&fileNamePPointer[2],
													kUTF8CharString);
	#endif	// defined multispec_win
   
	#if defined multispec_wx
		wxChoice* m_maskCtrl = (wxChoice*)(dialogPtr->FindWindow (maskPopupItemNumber));
		if (m_maskCtrl->GetCount () < 3)
			m_maskCtrl->Append (&fileNamePPointer[2]);
	#endif	// defined multispec_win

}	// end "StatisticsDialogSetMaskFileName"



SInt16 StatisticsDialogSelectMaskItem (
				Handle*									maskFileInfoHandlePtr,
				DialogPtr								dialogPtr,
				MenuHandle								popUpSelectMaskImageMenu,
				SInt16									itemHit,
				SInt16									maskImageSelection,
				SInt16									selectStringNumber,
				SInt16									maskPopupItemNumber,
				SInt16									layerItemNumber,
				UInt16*									maxNumberLayersPtr)

{
	FileInfoPtr								fileInfoPtr;
	FileStringPtr							fileNamePPointer;

	UInt16									layerSetting;

   SignedByte								handleStatus;


	layerSetting = 1;

   if (itemHit == 1)
		{
      HideDialogItem (dialogPtr, layerItemNumber);
		
		}	// end "if (itemHit == 1)"

   else if (itemHit == 2)
		{
				// Select a new training mask file.

      SetDLogControlHilite (dialogPtr, IDOK, 255);

      *maskFileInfoHandlePtr = GetMaskFile (*maskFileInfoHandlePtr,
															selectStringNumber);

      SetDLogControlHilite (dialogPtr, IDOK, 0);

      if (*maskFileInfoHandlePtr != NULL)
			{
         fileNamePPointer = (FileStringPtr)GetFileNamePPointerFromFileHandle (
																				*maskFileInfoHandlePtr,
																				&handleStatus);
			
         StatisticsDialogSetMaskFileName (fileNamePPointer,
														popUpSelectMaskImageMenu,
														dialogPtr,
														maskPopupItemNumber);
														
					// Get the max number of layers (channels)
			
			fileInfoPtr = (FileInfoPtr)GetHandlePointer (*maskFileInfoHandlePtr);
			*maxNumberLayersPtr = fileInfoPtr->numberChannels;

			if (fileInfoPtr->format == kPNGType && fileInfoPtr->numberChannels == 4)
				layerSetting = 4;

					// Unlock the class name structure handle if needed.					

         MHSetState (*maskFileInfoHandlePtr, handleStatus);

         itemHit = 3;

			}	// end "if (*maskFileInfoHandlePtr != NULL)"

      else	// *maskFileInfoHandlePtr == NULL
					// No change was made.
         itemHit = maskImageSelection;

		}	// end "if (itemHit == 2)"

   if (itemHit == 3)
		{
		ShowDialogItem (dialogPtr, layerItemNumber);
		
		if (*maxNumberLayersPtr == 1)
			SetDialogItemToStaticText (dialogPtr, layerItemNumber);
			
		else	// *maxNumberLayersPtr > 1
			SetDialogItemToEditText (dialogPtr, layerItemNumber);
			
		LoadDItemValue (dialogPtr, layerItemNumber, layerSetting);

		if (*maxNumberLayersPtr > 1)
			SelectDialogItemText (dialogPtr, layerItemNumber, 0, SInt16_MAX);
				
		}	// end "if (itemHit == 3)"

   return (itemHit);

}	// end "StatisticsDialogSelectMaskItem"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void StatisticsDialogOK
//
//	Software purpose:	The purpose of this routine is to update the parameters that
//							the user specified in the statistics dialog box. 		     
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			StatisticsDialog in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/02/1997
//	Revised By:			Larry L. Biehl			Date: 08/14/2019

void StatisticsDialogOK (
				SInt16								channelSelection,
				SInt16								totalNumberChannels,
				UInt16*								featurePtr,
				UInt32								localNumberChannels,
				Boolean								showTrainingFieldsFlag,
				Boolean								showTestFieldsFlag,
				Boolean								showClassNamesFlag,
				Boolean								showFieldNamesFlag,
				Boolean								showTrainTestTextFlag,
				SInt16								outlineColorSelection,
				SInt16								localStatCode,
				Boolean								keepClassStatsFlag,
				double								variance,
				double								minLogDeterminant,
				Boolean								useCommonCovarianceInLOOCFlag)

{
   UInt32								index;


			// Channels.

   Boolean channelsChangedFlag = FALSE;
   if (channelSelection == kAllMenuItem) // All channels 
		{
      localNumberChannels = (UInt32)MIN (totalNumberChannels, kMaxNumberStatChannels);

      for (index = 0;
				index < localNumberChannels;
					index++)
         gProjectInfoPtr->channelsPtr[index] = (UInt16)index;

		}	// end "if (gChannelSelection == kAllMenuItem)"

   else	// gChannelSelection == kSubsetMenuItem 
		{
      localNumberChannels = MIN (localNumberChannels, kMaxNumberStatChannels);
      for (index = 0; index < localNumberChannels; index++)
			{
         if (gProjectInfoPtr->channelsPtr[index] != featurePtr[index])
				{
            channelsChangedFlag = TRUE;
            gProjectInfoPtr->channelsPtr[index] = featurePtr[index];

				}	// end "if (...->channel[index] != featurePtr[index])"

			}	// end "for (index=0; index<localNumberChannels; index++)"

		}	// end "else gChannelSelection == kSubsetMenuItem

   if (gProjectInfoPtr->numberStatisticsChannels != (SInt16)localNumberChannels)
      channelsChangedFlag = TRUE;

   gProjectInfoPtr->numberStatisticsChannels = (SInt16)localNumberChannels;

			// If the channels to be used for statistics have changed, indicate
			// that the false color channels will need to be reset.

   if (channelsChangedFlag)
      gProjectInfoPtr->falseColorPaletteRed = -1;

			// If the channels to be used for statistics have changed, make sure
			// that pixels are reloaded if needed for classifiers using pixel values.

   if (channelsChangedFlag && gProjectInfoPtr->pixelDataLoadedFlag)
		ClearProjectStatisticsMemory (TRUE);

			// Outline training and/or test fields/areas.

   gProjectInfoPtr->outlineFieldType = 0;
   if (showTrainingFieldsFlag)
      gProjectInfoPtr->outlineFieldType += 1;

   if (showTestFieldsFlag)
      gProjectInfoPtr->outlineFieldType += 2;

			// Label area outlines.
			// Only update the label field code if the fields are to be outlined.

   if (gProjectInfoPtr->outlineFieldType != 0)
		{
      gProjectInfoPtr->labelFieldCode = 0;
      if (showClassNamesFlag)
         gProjectInfoPtr->labelFieldCode += 1;

      if (showFieldNamesFlag)
         gProjectInfoPtr->labelFieldCode += 2;

      if (showTrainTestTextFlag)
         gProjectInfoPtr->labelFieldCode += 4;

		}	// end "if (gProjectInfoPtr->outlineFieldType != 0)"

			// Set color to be used for outlining.

   gProjectInfoPtr->outlineColorCode = outlineColorSelection;

			// Statistics to be computed.

   if (gProjectInfoPtr->statisticsCode != localStatCode)
		{
      gProjectInfoPtr->statisticsCode = localStatCode;
      gUpdateProcessorMenuItemsFlag = TRUE;

		}	// end "if (...->statisticsCode != localStatCode)"

   if (localStatCode == kMeanStdDevOnly)
      gProjectInfoPtr->numberCovarianceEntries =
											(UInt32)gProjectInfoPtr->numberStatisticsChannels;

   else	// localStatCode == kMeanCovariance 
      gProjectInfoPtr->numberCovarianceEntries =
							(UInt32)gProjectInfoPtr->numberStatisticsChannels *
											(gProjectInfoPtr->numberStatisticsChannels + 1) / 2;

   if (gProjectInfoPtr->keepClassStatsOnlyFlag != keepClassStatsFlag)
		{
      gProjectInfoPtr->keepClassStatsOnlyFlag = keepClassStatsFlag;
      ClearProjectStatistics (3);

      if (gProjectInfoPtr->keepClassStatsOnlyFlag)
			{
         gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
         gProjectInfoPtr->saveFieldSumsSquaresFlag = FALSE;
         gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
         gProjectInfoPtr->saveClassSumsSquaresFlag = TRUE;

			}	// end "if (...->keepClassStatsOnlyFlag)"

      else	// !gProjectInfoPtr->keepClassStatsOnlyFlag 
			{
         gProjectInfoPtr->saveFieldMeansCovFlag = FALSE;
         gProjectInfoPtr->saveFieldSumsSquaresFlag = TRUE;
         gProjectInfoPtr->saveClassMeansCovFlag = FALSE;
         gProjectInfoPtr->saveClassSumsSquaresFlag = FALSE;

			}	// end "else !...->keepClassStatsOnlyFlag"

		}	// end "...->keepClassStatsOnlyFlag ..."

   gProjectInfoPtr->setZeroVarianceFlag = (variance > 0);
   if (gProjectInfoPtr->setZeroVarianceFlag)
      gProjectInfoPtr->zeroVarianceFactor = variance;

   gLowerDeterminantOffsetAllowed = minLogDeterminant;

   if (useCommonCovarianceInLOOCFlag !=
												gProjectInfoPtr->useCommonCovarianceInLOOCFlag)
		{
				// Reset any leave-one-out mixing parameters that need to be.

      ResetProjectLOOStats (useCommonCovarianceInLOOCFlag);

      gProjectInfoPtr->useCommonCovarianceInLOOCFlag = useCommonCovarianceInLOOCFlag;

		}	// end "if (useLOOCommonCovarianceFlag != ..."

}	// end "StatisticsDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean StatisticsOptionsDialog
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
//	Coded By:			Larry L. Biehl			Date: 07/19/1993
//	Revised By:			Larry L. Biehl			Date: 07/07/2017

Boolean StatisticsOptionsDialog (
				SInt16*								statCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				Boolean*								useCommonCovarianceInLOOCFlagPtr) 
{
	Boolean returnFlag = FALSE;

#if defined multispec_mac
	Rect									theBox;

   double								localMinLogDeterminant,
											localVariance,
											//maximumNaturalLogValue,
											//minimumNaturalLogValue,
											theNum;

   DialogPtr							dialogPtr;

   Handle								theHandle;

   SInt16								itemHit,
											localStatCode,
											theType;

   Boolean								localSetZeroVarianceFlag,
											localUseCommonCovarianceInLOOCFlag,
											modalDone;


			// Get the modal dialog for the reformat specification					

   dialogPtr = LoadRequestedDialog (kStatisticsOptionsDialogID, kCopyScrap, 1, 2);
   if (dialogPtr == NULL)
																									return (0);

			// Statistics to be computed.														

   localStatCode = *statCodePtr;
   SetDLogControl (dialogPtr, 5, (localStatCode == kMeanStdDevOnly));
   SetDLogControl (dialogPtr, 6, (localStatCode == kMeanCovariance));

			// Keep only class statistics in memory.										

   SetDLogControl (dialogPtr, 7, *keepClassStatsFlagPtr);

   localSetZeroVarianceFlag = (*variancePtr > 0);
   if (localStatCode == kMeanCovariance) 
		{
      SetDLogControl (dialogPtr, 8, localSetZeroVarianceFlag);

      ShowHideDialogItem (dialogPtr, 9, localSetZeroVarianceFlag);

		}	// end "if (localStatCode == kMeanCovariance)" 

   else	// localStatCode != kMeanCovariance 
		{
      SetDLogControl (dialogPtr, 8, 0);
      SetDLogControlHilite (dialogPtr, 8, 255);
      HideDialogItems (dialogPtr, 9, 11);

		}	// end "else localStatCode != kMeanCovariance" 

   localVariance = fabs (*variancePtr);
   LoadDItemRealValue (dialogPtr, 9, localVariance, 6);

			// Minimum log determinant allowed for valid matrix inversion.

   //maximumNaturalLogValue = DBL_MAX_10_EXP * log (10);
   //minimumNaturalLogValue = DBL_MIN_10_EXP * log (10);
   localMinLogDeterminant = *minLogDeterminantPtr;
   LoadDItemRealValue (dialogPtr, 11, localMinLogDeterminant, 6);

   localUseCommonCovarianceInLOOCFlag = (*useCommonCovarianceInLOOCFlagPtr);
   if (localStatCode == 1) 
		{
      SetDLogControlHilite (dialogPtr, 12, 255);
      localUseCommonCovarianceInLOOCFlag = false;

		}	// end "if (localStatCode == 1)" 

			// Use covariance in leave-one-out calculations

   SetDLogControl (dialogPtr, 12, localUseCommonCovarianceInLOOCFlag);

			// Center the dialog and then show it.											

   ShowDialogWindow (dialogPtr, kStatisticsOptionsDialogID, kSetUpDFilterTable);

   gDialogItemDescriptorPtr[9] = kDItemReal;
   gDialogItemDescriptorPtr[11] = kDItemReal + kDItemMinus;

   itemHit = 11;
   if (localStatCode == kMeanCovariance && localSetZeroVarianceFlag)
      itemHit = 9;

   SelectDialogItemText (dialogPtr, itemHit, 0, INT16_MAX);

   modalDone = false;
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
            case 5: // Compute mean and standard deviation only
               if (localStatCode == 2) 
						{
                  if (ProjectMenuClearStatistics ()) 
							{
                     SetControlValue ((ControlHandle)theHandle, 1);
                     SetDLogControl (dialogPtr, 6, 0);

                     SetDLogControl (dialogPtr, 8, 0);
                     SetDLogControlHilite (dialogPtr, 8, 255);
                     HideDialogItems (dialogPtr, 9, 11);
                     localStatCode = 1;

							}	// end "if (ProjectMenuClearStatistics ())" 

						}	// end "if (localStatCode == 2)" 
               break;

            case 6: // Compute mean, standard deviation and covariance
               if (localStatCode == 1)
						{
                  if (ProjectMenuClearStatistics ())
							{
                     SetControlValue ((ControlHandle)theHandle, 1);
                     SetDLogControl (dialogPtr, 5, 0);

                     SetDLogControl (dialogPtr, 8, (localSetZeroVarianceFlag));
                     SetDLogControlHilite (dialogPtr, 8, 0);

                     if (localSetZeroVarianceFlag)
								{
                        ShowDialogItem (dialogPtr, 9);
                        SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);

								}	// end "if (localSetZeroVarianceFlag)" 

                     else	// !localSetZeroVarianceFlag 
                        HideDialogItem (dialogPtr, 9);

                     ShowDialogItems (dialogPtr, 10, 11);

                     localStatCode = 2;

							}	// end "if (ProjectMenuClearStatistics ())" 

						}	// end "if (localStatCode == 1)" 
               break;

            case 7: // Keep class statistics only. 
            case 12: // Use modified class statistics. 
               ChangeDLogCheckBox ((ControlHandle)theHandle);
               break;

            case 8: // Set zero variances. 
               ChangeDLogCheckBox ((ControlHandle)theHandle);

               if (GetDLogControl (dialogPtr, 8))
						{
                  ShowDialogItem (dialogPtr, 9);
                  SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);

						}	// end "if (GetDLogControl (dialogPtr, 8))" 

               else	// !GetDLogControl (dialogPtr, 8)
                  HideDialogItem (dialogPtr, 9);
               break;

            case 9: // Zero variance set value 
               localVariance = GetDItemRealValue (dialogPtr, 9);
               break;

            case 11: // Minimum log determinant 
               theNum = GetDItemRealValue (dialogPtr, 11);
               if (theNum < gMinimumNaturalLogValue ||
																theNum > gMaximumNaturalLogValue)
                  RealNumberErrorAlert (localMinLogDeterminant, dialogPtr, itemHit, 1);

               else	// theNum is okay
                  localMinLogDeterminant = GetDItemRealValue (dialogPtr, 11);
               break;

				}	// end "switch (itemHit)" 

			}	// end "if (itemHit > 2)" 

      else	// itemHit <= 2 
			{
         if (itemHit == 1) // User selected OK for information 
				{
            modalDone = true;
            returnFlag = true;

						// Save the updated parameters.

            StatisticsOptionsDialogOK (localStatCode,
													GetDLogControl (dialogPtr, 7),
													GetDLogControl (dialogPtr, 8),
													localVariance,
													localMinLogDeterminant,
													GetDLogControl (dialogPtr, 12),
													statCodePtr,
													keepClassStatsFlagPtr,
													variancePtr,
													minLogDeterminantPtr,
													useCommonCovarianceInLOOCFlagPtr);

				}	// end "if (itemHit == 1)" 

         if (itemHit == 2) // User selected Cancel for information 
				{
            modalDone = true;
            returnFlag = false;

				}	// end "if	(itemHit == 2)" 

			}	// end "else itemHit <= 2" 

		}	while (!modalDone);

   CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
#endif	// defined multispec_mac  

	#if defined multispec_win  
		CMStatOptionsDlg* dialogPtr = NULL;

		TRY
			{
			dialogPtr = new CMStatOptionsDlg ();

			returnFlag = dialogPtr->DoDialog (statCodePtr,
															keepClassStatsFlagPtr,
															variancePtr,
															minLogDeterminantPtr,
															useCommonCovarianceInLOOCFlagPtr);

			delete dialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif // defined multispec_win  

	#if defined multispec_wx
		CMStatOptionsDlg* dialogPtr = NULL;

		try
			{
			dialogPtr = new CMStatOptionsDlg ();

			returnFlag = dialogPtr->DoDialog (statCodePtr,
															keepClassStatsFlagPtr,
															variancePtr,
															minLogDeterminantPtr,
															useCommonCovarianceInLOOCFlagPtr);

			delete dialogPtr;
			}

		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif // defined multispec_wx
   
   return (returnFlag);

}	// end "StatisticsOptionsDialog"



void StatisticsOptionsDialogOK (
				SInt16								localStatCode,
				Boolean								localKeepStatsFlag,
				Boolean								localZeroVarianceFlag,
				double								localVariance,
				double								localMinLogDeterminant,
				Boolean								localCommonCovarianceInLOOCFlag,
				SInt16*								statCodePtr,
				Boolean*								keepClassStatsFlagPtr,
				double*								variancePtr,
				double*								minLogDeterminantPtr,
				Boolean*								useCommonCovarianceInLOOCFlagPtr)

{
			// Statistics to be computed.										

   *statCodePtr = localStatCode;

   *keepClassStatsFlagPtr = localKeepStatsFlag;

   if (localZeroVarianceFlag)
      *variancePtr = localVariance;

   else	// !localZeroVarianceFlag 
      *variancePtr = -fabs (*variancePtr);

   *minLogDeterminantPtr = localMinLogDeterminant;

   *useCommonCovarianceInLOOCFlagPtr = localCommonCovarianceInLOOCFlag;

}	// end "StatisticsOptionsDialogOK"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void StatisticsWControlEvent
//
//	Software purpose:	This routine handles control events in the
//							project window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
// 
// Called By:			DoStatisticsMouseDnContent in multiSpec.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/28/1988
//	Revised By:			Larry L. Biehl			Date:	01/16/2019

Boolean StatisticsWControlEvent (
				ControlHandle						theControl,
				Boolean								waitOnMouseUpFlag,
				Point									location,
				SInt32								refCon) 
{
			// Local Structures and Variables	

			// Allow 31 characters for field name, 1 for pascal character count and
			// 1 for c-string terminator.

   Str63									fieldIdentifier;

   Cell									cell;

	#if defined multispec_mac
		SInt16								itemHit;
		Boolean								doubleClick;
	#endif	// defined multispec_mac   				

   SInt16								returnCode;

   Boolean								continueFlag,
											returnFlag = TRUE;


   if (gProjectInfoPtr == NULL)
																					return (FALSE);

			// Get the refCon of the control. This will contain a code for which 
			// control it is.																		
			// refCon = default:	 Vertical scroll bar of statistics list window.	
			// refCon = 12: To Field control													
			// refCon = 13: Add Field To Project control									
			// refCon = 14: Update Stats control											
			// refCon = 15: Select New Field control										
			// refCon = 16: To Project control												
			// refCon = 17: To Class control													
			// refCon = 18: Polygon type entry control									
			// refCon = 19: Print statistics control										
			// refCon = 20: Print class statistics check box							
			// refCon = 21: Print field statistics check box							
			// refCon = 22: Edit class/field name											
			// refCon = 23: Histogram statistics control									
			// refCon = 24: Histogram class statistics check box						
			// refCon = 25: Histogram field statistics check box

   continueFlag = TRUE;

	#if defined multispec_mac
		refCon = (short int)GetControlReference (theControl);

		if (refCon >= 12 && refCon <= 25 &&
					refCon != kStatHistogramControl && refCon != kStatPrintControl)
			{
			if (waitOnMouseUpFlag)
				{
				if (TrackControl (theControl, location, NIL) == 0)
					continueFlag = false;

				}	// if (waitOnMouseUpFlag)

			else	// !waitOnMouseUpFlag 
				{
				//HLock ((Handle)theControl);
				//continueFlag = ButtonDownEvent (&((*theControl)->contrlRect), false);
				//HUnlock ((Handle)theControl);
				GetControlBounds (theControl, &gTempRect);
				continueFlag = ButtonDownEvent (&gTempRect, FALSE);

				}	// end "else !waitOnMouseUpFlag" 

			}	// end "if (refCon >= 12 && refCon <= 25 && ...)"
	#endif	// defined multispec_mac 

   if (continueFlag)
		{
      switch (refCon)
			{
         case kToFieldControl:
            cell.h = 0;
            cell.v = 0;
            if (LGetSelect (TRUE, &cell, gStatisticsListHandle))
               PolygonListStatMode (cell.v);
            break;

         case kAddToListControl:
            fieldIdentifier[0] = 0;
            returnFlag = AddFieldToProject (gProjectInfoPtr->currentClass,
															gStatisticsMode - 310,
															kTrainingType,
															fieldIdentifier);
				UpdateStatWEditMenu ();
            break;

         case kUpdateStatsControl:
            gProcessorCode = kComputeStatsProcessor;
            gOutputForce1Code = 0x0001;
            returnCode = UpdateStatsControl (gProjectInfoPtr->statsWindowMode,
															FALSE,
															FALSE);
            break;

         case kNewFieldControl:
            NewFieldStatMode ();
            break;

         case kToProjectControl:
            ClassListStatMode ();
            break;

         case kToClassControl:
            FieldListStatMode (gProjectInfoPtr->currentClass);
            break;

         case kPolygonEnterControl:
            if (gStatisticsMode == kStatNewFieldRectMode)
					{
               MSetControlValue (gProjectWindow, theControl, 1);
               gStatisticsMode = kStatNewFieldPolyMode;
               gProjectInfoPtr->selectionType = kPolygonType;
               MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
					}	// end "if (gStatisticsMode == kStatNewFieldRectMode)"
				
				else	// gStatisticsMode != kStatNewFieldRectMode
					{
               MSetControlValue (gProjectWindow, theControl, 0);
               gStatisticsMode = kStatNewFieldRectMode;
               gProjectInfoPtr->selectionType = kRectangleType;
               MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0);
					
					}	// else gStatisticsMode != kStatNewFieldRectMode
				
            RemoveListCells ();

            ClearSelectionArea (gProjectSelectionWindow);

            gUpdateEditMenuItemsFlag = TRUE;
            break;

         case kStatPrintControl:
				#if defined multispec_mac           
					itemHit = StatListPopUpMenu (gProjectInfoPtr->statsWindowMode);

					if (itemHit >= 1) 
						{
						gProcessorCode = kListStatsProcessor;
						ListStatsControl (gProjectInfoPtr->statsWindowMode, itemHit);

						}	// end "if (itemHit >= 1)"
				#endif	// defined multispec_mac   				 
            break;

         case kEditNameControl:
            switch (gProjectInfoPtr->statsWindowMode)
					{
               case 1:
                  returnFlag = EditSelectionDialog (kRectangleType, TRUE);
                  break;

               case 2:
               case 3:
                  returnFlag =
                     EditClassFieldDialog (gProjectInfoPtr->statsWindowMode,
															gProjectInfoPtr->currentClass,
															gProjectInfoPtr->currentField);
                  break;

               case 4:
                  returnFlag = EditCoordinatesDialog (
																gProjectInfoPtr->currentClass,
																gProjectInfoPtr->currentField,
																gProjectInfoPtr->currentCoordinate);
                  break;

					}	// end "switch (gProjectInfoPtr->statsWindowMode)" 
            break;

         case kStatHistogramControl:
				#if defined multispec_mac
					itemHit = StatHistogramPopUpMenu (gProjectInfoPtr->statsWindowMode);

					if (itemHit >= 1)
						{
								// Make sure the graphics code is loaded.

						ForceGraphCodeResourceLoad ();
						if (gMemoryTypeNeeded == 0)
							{
							gMemoryTypeNeeded = 1;
							gProcessorCode = kHistogramStatsProcessor;
							HistogramStatsControl (gProjectInfoPtr->statsWindowMode,
															itemHit);

							gMemoryTypeNeeded = 0;

							}	// end "if (gMemoryTypeNeeded == 0)"

						GetSpareMemory ();

						}	// end "if (itemHit >= 1)"
				#endif	// defined multispec_mac 
            break;

         default: // Mouse down in vertical scroll bar
				#if defined multispec_mac 
					doubleClick = LClick (location,
													gEventRecord.modifiers,
													gStatisticsListHandle);
				#endif	// defined multispec_mac 
            break;

			}	// end "switch (refCon)"  

      gProcessorCode = 0;
      gTextMemoryShortCode = 0;

				// Reset the global memory error values.										

      gMemoryError = noErr;

		}	// end "if (continueFlag)"
	
	else	// !continueFlag
		returnFlag = FALSE;
	
	return (returnFlag);

}	// end "StatisticsWControlEvent" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void StatisticsWMouseDn
//
//	Software purpose:	The purpose of this routine is to handle mouse
//							down events in the project window.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			MouseDnEvents  in MMultiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 09/27/1988
//	Revised By:			Larry L. Biehl			Date: 02/28/2017

void StatisticsWMouseDn (
				Point									mousePt)

{
   Rect									rect;

   Cell									cell;

   Point									cellLocation,
											classPopUpLoc;

   SInt32								menuData;

   SInt16								covarianceStatsToUse,
											listNumber,
											menuItem,
											newClass,
											savedStatsMode;

   Boolean								doubleClick;


   rect = ((ListPtr)*gStatisticsListHandle)->rView;
   rect.right = rect.right + kSBarWidth;
   if (PtInRect (mousePt, &rect))
		{
      doubleClick = LClick (mousePt,
										gEventRecord.modifiers,
										gStatisticsListHandle);
		
      if (doubleClick)
			{
         cellLocation = LLastClick (gStatisticsListHandle);
         listNumber = cellLocation.v;
         savedStatsMode = gProjectInfoPtr->statsWindowMode;

         if (savedStatsMode == kClassListMode)
            FieldListStatMode (listNumber);

         else if (savedStatsMode == kFieldListMode)
            PolygonListStatMode (listNumber);

         else if (savedStatsMode == kCoordinateListMode)
				{
            EditCoordinatesDialog (gProjectInfoPtr->currentClass,
												gProjectInfoPtr->currentField,
												gProjectInfoPtr->currentCoordinate);

				}	// end "else if (savedStatsMode == 4)"

			}	// end "if (doubleClick)"

      else	// !doubleClick 
			{
         cell.h = 0;
         cell.v = 0;

         if (LGetSelect (true, &cell, gStatisticsListHandle))
				{
            if (gProjectInfoPtr->statsWindowMode == kClassListMode)
					{
               gProjectInfoPtr->currentClass = cell.v;
               HiliteControl (gProjectInfoPtr->toClassControlH, 0);
               HiliteControl (gProjectInfoPtr->editNameControlH, 0);

					}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"

            else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
					{
               gProjectInfoPtr->currentField =
									GetCurrentField (gProjectInfoPtr->currentClass, cell.v);

               if (gProjectInfoPtr->fieldIdentPtr[
									gProjectInfoPtr->currentField].pointType != kClusterType)
                  HiliteControl (gProjectInfoPtr->toFieldControlH, 0);
					
               else	// ...pointType >= kClusterType 
                  HiliteControl (gProjectInfoPtr->toFieldControlH, 255);

               HiliteControl (gProjectInfoPtr->editNameControlH, 0);

					}	// end "else if (gProjectInfoPtr->statsWindowMode == ..."

            else if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
					{
               cell.h = 1 - cell.h;
               LSetSelect (true, cell, gStatisticsListHandle);
               gProjectInfoPtr->currentCoordinate = cell.v;
               HiliteControl (gProjectInfoPtr->editNameControlH, 0);

					}	// end "else if (...->statsWindowMode == kCoordinateListMode"

				}	// end "if (LGetSelect (true, &cell,..."

         else	// !LGetSelect (true, &cell, gStatisticsListHandle) 
				{
            if (gProjectInfoPtr->statsWindowMode == kClassListMode)
					{
               gProjectInfoPtr->currentClass = -1;
               HiliteControl (gProjectInfoPtr->toClassControlH, 255);
               HiliteControl (gProjectInfoPtr->editNameControlH, 255);

					}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)"

            else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
					{
               gProjectInfoPtr->currentField = -1;
               HiliteControl (gProjectInfoPtr->toFieldControlH, 255);
               HiliteControl (gProjectInfoPtr->editNameControlH, 255);

					}	// end "else if (...->statsWindowMode == kFieldListMode)"

            else if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
					{
               gProjectInfoPtr->currentCoordinate = -1;
               if (gProjectInfoPtr->fieldIdentPtr[
									gProjectInfoPtr->currentField].pointType == kPolygonType)
                  HiliteControl (gProjectInfoPtr->editNameControlH, 255);

					}	// end "else if (...->statsWindowMode == kCoordinateListMode)"

         }	// end "else !LGetSelect (true, &cell,..." 

         UpdateStatWEditMenu ();

			}	// end "else !doubleClick"

		}	// end "if (PtInRect (mousePt, &rect))"

   else	// !PtInRect (mousePt, &rect) 
		{
				// Check if mousedown was in class pop up menu box.  If so handle

      if (PtInRect (mousePt, &gProjectInfoPtr->popUpClassBox)&
									(gProjectInfoPtr->statsWindowMode == kSelectFieldMode))
			{
					// Get location of class pop up box and convert it to global

         classPopUpLoc.v = gProjectInfoPtr->popUpClassBox.top + 1;
         classPopUpLoc.h = gProjectInfoPtr->popUpClassBox.left;
         LocalToGlobal (&classPopUpLoc);

					// Invert the prompt box

         InvertRect (&gProjectInfoPtr->promptClassBox);

					// Insert the pop up menu and then let user make a selection

         InsertMenu (gPopUpClassMenu, -1);
         EraseRect (&gProjectInfoPtr->popUpClassBox);
         menuData = PopUpMenuSelect (gPopUpClassMenu,
													classPopUpLoc.v,
													classPopUpLoc.h,
													gProjectInfoPtr->lastClassForAddFieldSelection);

					// Invert the prompt box again to make normal and remove the
					// the class pop up menu

         InvertRect (&gProjectInfoPtr->promptClassBox);
         DeleteMenu (kPopUpClassMenuID);

					// Make the selection the new current class if a selection was
					// made; then redraw the class pop up box to reflect the			
					// new current class in case it is different.						

         newClass = LoWord (menuData);
         if (newClass > 0)
            gProjectInfoPtr->lastClassForAddFieldSelection = newClass;

         InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpClassBox);

					// Update the hiliting of the "->Class" control

         UpdateToClassControl ();

					// Update the edit menu items.

         UpdateStatWEditMenu ();

			}	// end "if (PtInRect (mousePt, &projectInfo..."

				// Check if mousedown was in statistics pop up menu box.  If so handle

      else if (PtInRect (mousePt, &gProjectInfoPtr->popUpCovarianceToUseBox) &&
										gProjectInfoPtr->statsWindowMode == kClassListMode) 
			{
         menuItem = StatisticsPopUpMenu (kClassListMode);

         if (menuItem > 0) 
				{
            covarianceStatsToUse = GetCovarianceStatsFromMenuItem (menuItem);
            SetProjectCovarianceStatsToUse (covarianceStatsToUse);

				}	// end "if (menuItem > 0)"

			}	// end "if (PtInRect (mousePt, &projectInfo..." 

				// Check if mousedown was in statistics pop up menu box.  If so handle	

      else if (PtInRect (mousePt, &gProjectInfoPtr->popUpCovarianceToUseBox) &&
											gProjectInfoPtr->statsWindowMode == kFieldListMode)
			{
         menuItem = StatisticsPopUpMenu (kFieldListMode);

         if (menuItem > 0) 
				{
            covarianceStatsToUse = GetCovarianceStatsFromMenuItem (menuItem);
            SetClassCovarianceStatsToUse (covarianceStatsToUse);

				}	// end "if (menuItem > 0)"

			}	// end "if (PtInRect (mousePt, &projectInfo..." 

		}	// end "else !PtInRect (mousePt, &rect)"  

}	// end "StatisticsWMouseDn" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateStatisticsWindow
//
//	Software purpose:	The purpose of this routine is to update the
//							graphics in the project window based.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1988
//	Revised By:			Larry L. Biehl			Date: 02/07/2018

void UpdateStatisticsWindow (void) 
{
	Str63									modeTitleString;

   SInt16								modeTitleCenter,
											modeTitleYStart,
											numModeTitleChars,
											stringWidth,
											listTitleHeightLocation;


   if (gProjectInfoPtr == NULL)
																									return;

   listTitleHeightLocation = gStatisticsWindowControlHeight - 6;

			// Draw the graphics for the "Select Field" mode window	

   numModeTitleChars = 0;
   if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode) 
		{
				// Get the project window mode title

      numModeTitleChars = sprintf ((char*)modeTitleString, "SELECT FIELD");

				// Draw the list title.															

      MoveTo (15, listTitleHeightLocation);
      DrawString ("\pCoordinates (L,C)");

      DrawClassPopUp ();

		}	// end "if (gProjectInfoPtr->statsWindowMode == kSelectFieldMode)" 

			// Draw the graphics for the "Class List" mode window						

   else if (gProjectInfoPtr->statsWindowMode == kClassListMode)
		{
				// Get the project window mode title	

      numModeTitleChars = sprintf ((char*)modeTitleString, "CLASS LIST");

      //SInt16 menuItem = GetProjectStatisticsTypeMenuItem ();                                                  
      //DrawStatPrompt (menuItem);
      DrawStatPopUp (kClassListMode);

				// Draw the list title.															

      MoveTo (34, listTitleHeightLocation);
      DrawString ("\pClasses");

		}	// end "if (gProjectInfoPtr->statsWindowMode == kClassListMode)" 

			// Draw the graphics for the "Field List" mode window						

   else if (gProjectInfoPtr->statsWindowMode == kFieldListMode)
		{
				// Get the project window mode title		

      numModeTitleChars = sprintf ((char*)modeTitleString, "FIELD LIST");

      DrawStatPopUp (kFieldListMode);

				// Draw the list title.															

      MoveTo (34, listTitleHeightLocation);
      DrawString ("\pFields");

      DrawClassPrompt ();

		}	// end "if (gProjectInfoPtr->statsWindowMode == kFieldListMode)" 

      // Draw the graphics for the "Point List" mode window						

   else if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)
		{
				// Get the project window mode title	

      numModeTitleChars = sprintf ((char*)modeTitleString, "COORDINATE LIST");

				// Draw the list title.															

      MoveTo (15, listTitleHeightLocation);
      DrawString ("\pCoordinates (L,C)");

      DrawClassPrompt ();
      DrawFieldPrompt ();

		}	// end "if (gProjectInfoPtr->statsWindowMode == kCoordinateListMode)" 

			// List the project window mode title.

   modeTitleYStart = gProjectInfoPtr->projectWindowModeTitleBox.bottom - 5;
   modeTitleCenter = (gProjectInfoPtr->projectWindowModeTitleBox.left +
							gProjectInfoPtr->projectWindowModeTitleBox.right) / 2;

   stringWidth = TextWidth (modeTitleString, 0, numModeTitleChars);

   MoveTo (modeTitleCenter - stringWidth / 2, modeTitleYStart);
   DrawText (modeTitleString, 0, numModeTitleChars);

			// Set the project window title	

   GetCopyOfPFileNameFromProjectInfo (gProjectInfoPtr, (FileStringPtr)gTextString);

   CheckStringLength (gProjectInfoPtr->projectNameBox.right -
															gProjectInfoPtr->projectNameBox.left,
								(Str255*)gTextString,
								(Str255*)gTextString3);
								
	CFStringRef							cfStringRef;
	OSErr									errorCode;
	
	cfStringRef = CFStringCreateWithBytes (kCFAllocatorDefault,
														&gTextString[1],
														gTextString[0],
														kCFStringEncodingUTF8,
														false);
	
	if (cfStringRef != NULL)
		{
		HideControl (gProjectInfoPtr->projectNameControlRef);
		errorCode = HIViewSetText (gProjectInfoPtr->projectNameControlRef, 
											cfStringRef);

		ShowControl (gProjectInfoPtr->projectNameControlRef);
		CFRelease (cfStringRef);
		
		}	// end "if (cfStringRef != NULL)"
	/*
   modeTitleYStart = gProjectInfoPtr->projectNameBox.bottom - 5;
   modeTitleCenter = (gProjectInfoPtr->projectNameBox.left +
								gProjectInfoPtr->projectNameBox.right) / 2;

   stringWidth = TextWidth ((char*)&gTextString3[1], 0, gTextString3[0]);

   MoveTo (modeTitleCenter - stringWidth / 2, modeTitleYStart);
   DrawText ((char*)&gTextString3[1], 0, gTextString3[0]);
	*/
	}	// end "UpdateStatisticsWindow" 
#endif	// defined multispec_mac 	




//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateToClassControl
//
//	Software purpose:	This routine updates the hiliting of the "->Class"
//							control.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1988
//	Revised By:			Larry L. Biehl			Date: 02/02/1996			

void UpdateToClassControl (void)

{
   SInt16								classStorage,
											controlHilite,
											currentClass;


			// Unhilite the "To Class" control if the current class is "New" or
			// the current class has no fields.												
			// Otherwise hilite "To Class" control											

   controlHilite = 255;
   currentClass = gProjectInfoPtr->currentClass;
   if (currentClass != -1)
		{
				// Get the class storage number.

      classStorage = gProjectInfoPtr->storageClass[currentClass];

      if (gProjectInfoPtr->classNamesPtr[classStorage].numberOfFields > 0)
         controlHilite = 0;

		}	// end "if (currentClass != -1)"

   MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, controlHilite);

}	// end "UpdateToClassControl"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void WriteProjectName
//
//	Software purpose:	The purpose of this routine is to write the project name in the
// .
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1988
//	Revised By:			Larry L. Biehl			Date: 09/01/2017

void WriteProjectName (void)

{
	#if defined multispec_mac 
		InvalWindowRect (gProjectWindow, &gProjectInfoPtr->projectNameBox);
	#endif	// defined multispec_mac 

	#if defined multispec_win 
		USES_CONVERSION;

		GetCopyOfPFileNameFromProjectInfo (gProjectInfoPtr,
														(FileStringPtr)gWideTextString,
														kUnicodeCharString);

		//(gProjectWindow->GetDlgItem (IDC_ProjectName))->
		//						SetWindowText (A2T((LPCSTR)&gTextString[1]));
		
		SetDLogControlTitle ((DialogPtr)gProjectWindow, 
									IDC_ProjectName, 
									(char*)gWideTextString,
									kUnicodeCharString);
	#endif	// defined multispec_win 

	#if defined multispec_wx 
		GetCopyOfPFileNameFromProjectInfo (gProjectInfoPtr, (UCharPtr)gTextString);

		(gProjectWindow->GetFrame()->FindWindow (IDC_ProjectName))->
																SetLabel ((char*)&gTextString[1]);
	#endif	// defined multispec_wx 
   
}	// end "WriteProjectName" 
