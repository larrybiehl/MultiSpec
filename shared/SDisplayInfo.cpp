//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SDisInfo.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//
//	Brief description:	The purpose of the routines in this file is to
//								provide routines to support the Display Specs structure.
//	Functions in file:	CMFileStream* 	GetFileStreamPointer
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_lin
	#include "SMultiSpec.h"
#endif
  
#if defined multispec_win
	#include "CDisplay.h"
#endif	// defined multispec_win    

//#include "SExtGlob.h"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
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
		
		//displaySpecsPtr->symbolToPaletteEntryH = 
		//			UnlockAndDispose (displaySpecsPtr->symbolToPaletteEntryH); 
							
		displaySpecsPtr->channelsHandle =
					UnlockAndDispose (displaySpecsPtr->channelsHandle);
		
		//displaySpecsPtr->displayClassGroupsHandle =
		//			UnlockAndDispose (displaySpecsPtr->displayClassGroupsHandle);
		
		//displaySpecsPtr->savedClassCTableHandle = (CTabHandle)
		//			UnlockAndDispose ((Handle)displaySpecsPtr->savedClassCTableHandle);
		
		//displaySpecsPtr->savedGroupCTableHandle = (CTabHandle)                           
		//			UnlockAndDispose ((Handle)displaySpecsPtr->savedGroupCTableHandle);
		
		//displaySpecsPtr->localGroupNameHandle =
		//			UnlockAndDispose (displaySpecsPtr->localGroupNameHandle);
		/*
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
		*/								                
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

