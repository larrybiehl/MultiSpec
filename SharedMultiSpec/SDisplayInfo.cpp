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
//	File:						SDisplayInfo.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//
//	Brief description:	The purpose of the routines in this file is to
//								provide routines to support the Display Specs structure.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_wx
#endif
  
#if defined multispec_win
	#include "SDisplay_class.h"
#endif	// defined multispec_win   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseDisplayPaletteMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the display
//							support memory - handles for:
//								symbol to palette vector
//								channels vector
//								display class to group vector
//								class color table structure
//								group color table structure
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	09/28/2006
//	Revised By:			Larry L. Biehl			Date: 09/28/2006

void ReleaseDisplayPaletteMemory (
				DisplaySpecsPtr 					displaySpecsPtr)

{  
	if (displaySpecsPtr != NULL)
		{  
		displaySpecsPtr->symbolToPaletteEntryH = 
					UnlockAndDispose (displaySpecsPtr->symbolToPaletteEntryH); 
		
		displaySpecsPtr->displayClassGroupsHandle =
					UnlockAndDispose (displaySpecsPtr->displayClassGroupsHandle);
		
		displaySpecsPtr->savedClassCTableHandle = (CTabHandle)
					UnlockAndDispose ((Handle)displaySpecsPtr->savedClassCTableHandle);
		
		displaySpecsPtr->savedGroupCTableHandle = (CTabHandle)                           
					UnlockAndDispose ((Handle)displaySpecsPtr->savedGroupCTableHandle);
		
		displaySpecsPtr->localGroupNameHandle =
					UnlockAndDispose (displaySpecsPtr->localGroupNameHandle);
		
		#if defined multispec_win 
			if (displaySpecsPtr->paletteObject != NULL)
				{ 
				delete displaySpecsPtr->paletteObject;
				displaySpecsPtr->paletteObject = NULL;
				
				}	// end "if (displaySpecsPtr->paletteObject != NULL)"     
			
			if (displaySpecsPtr->backgroundPaletteObject != NULL)
				{ 
				delete displaySpecsPtr->backgroundPaletteObject;
				displaySpecsPtr->backgroundPaletteObject = NULL;
				
				}	// end "if (displaySpecsPtr->backgroundPaletteObject != NULL)" 
		#endif	// defined multispec_win 
								                		
		}	// end "if (displaySpecsPtr != NULL)"

}	// end "ReleaseDisplayPaletteMemory"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseDisplaySupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the display
//							support memory - handles for:
//								symbol to palette vector
//								channels vector
//								display class to group vector
//								class color table structure
//								group color table structure
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	11/02/1999
//	Revised By:			Larry L. Biehl			Date: 11/02/1999

void ReleaseDisplaySupportMemory (
				WindowInfoPtr						windowInfoPtr)

{  
	Handle								displaySpecsHandle;
	
	displaySpecsHandle = GetDisplaySpecsHandle (windowInfoPtr);
	                                
	ReleaseDisplaySupportMemory (displaySpecsHandle);

}	// end "ReleaseDisplaySupportMemory"   



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseDisplaySupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the display
//							support memory - handles for:
//								channels vector
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned: 
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date:	06/12/1995
//	Revised By:			Larry L. Biehl			Date: 11/02/1999

void ReleaseDisplaySupportMemory (
				Handle								displaySpecsHandle)

{  
	SignedByte							handleStatus;
	
	                                
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr) 
						GetHandleStatusAndPointer (displaySpecsHandle, &handleStatus); 
	
	if (displaySpecsPtr != NULL)
		{  
		ReleaseDisplayPaletteMemory (displaySpecsPtr);
							
		displaySpecsPtr->channelsHandle =
					UnlockAndDispose (displaySpecsPtr->channelsHandle);
			
		displaySpecsPtr->initializeStructureFlag = TRUE;
		displaySpecsPtr->numberDisplayClasses = 0;
		displaySpecsPtr->numberDisplayGroups = 0;
		
		displaySpecsPtr->magnification = 1; 
		displaySpecsPtr->origin[kVertical] = 0;
		displaySpecsPtr->origin[kHorizontal] = 0;
		displaySpecsPtr->imageDimensions[kVertical] = 0;
		displaySpecsPtr->imageDimensions[kHorizontal] = 0;
		
		MHSetState (displaySpecsHandle, handleStatus);
		
		}	// end "if (displaySpecsPtr != NULL)"

}	// end "ReleaseDisplaySupportMemory"   

