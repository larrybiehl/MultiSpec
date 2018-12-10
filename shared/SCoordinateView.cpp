//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SCoordinateView.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh Operating System and Windows Operating System
//
//	Brief description:	 
//
//	Functions in file:	void UpdateCoordinateViewControls
//
//	Include files:			"MultiSpecHeaders"
//								"SMultiSpec.h"
//
//-----------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_lin
	#include "SMultiSpec.h"
	//#include	"SExternalGlobals.h"
#endif	

#if defined multispec_mac  
	class  CCmdUI;	
#endif	// defined multispec_mac   
                             
#if defined multispec_win
	#include "SMultiSpec.h"
	//#include	"SExternalGlobals.h"
#endif	// defined multispec_win 
	 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void UpdateCoordinateViewControls
//
//	Software purpose:	This routine updates the coordinate view controls for the
//							image window.  The routine is called after the 
//							image is first drawn and when the image window is activated.
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			ShowHideCoordinateView in menus.c
//							DoImageWActivateEvent in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 11/07/2000
//	Revised By:			Larry L. Biehl			Date: 11/22/2000			

void UpdateCoordinateViewControls (
				WindowPtr							theWindow)

{
	#if defined multispec_mac  
		ControlHandle						controlHandle;
		Handle								windowInfoHandle;
		
		
				// Check input parameters.															
				
		if (theWindow == NULL)
																								return;
																								
				// Save the current clip region.													
				
		GetClip (gTempRegion1);				
				
				// Set clip region of the window to include the scroll bars so			
				// that the controls can be changed.											
					
		ClipRect (GetWindowPortBounds (theWindow, &gTempRect));
		
				// Update coordinate units and area units controls	
							
		windowInfoHandle = GetWindowInfoHandle (theWindow);	
			
		controlHandle = GetCoordinateViewUnitsControl (windowInfoHandle);
		if (controlHandle != NULL)
			{				
					// Hilite the coordinate units control appropriately.	

			if (gActiveOffscreenMapExists && theWindow == FrontWindow ())
				HiliteControl (controlHandle, 0);
			
			else		// !gActiveOffscreenMapExists || ...
				HiliteControl (controlHandle, 255);
				
			}		// end "if (controlHandle != NULL)"
					
		controlHandle = GetCoordinateViewAreaUnitsControl (windowInfoHandle);
		if (controlHandle != NULL)
			{
					// Hilite the area units control appropriately.				

			if (GetCoordinateHeight (windowInfoHandle) > 0 && 
												GetSelectionTypeCode (theWindow) != 0)
				{
				HiliteControl (controlHandle, 0);	
				ShowControl (controlHandle);
				
				}		// end "if (GetSelectionTypeCode (theWindow) != 0)"
			
			else		// coordinateHeight == 0 || ...
				HideControl (controlHandle);
				
			}		// end "if (controlHandle != NULL)"
																								
				// Reset the clip region back to that upon entry to this routine.		
				
		SetClip (gTempRegion1);
		SetEmptyRgn (gTempRegion1);
	#endif	// defined multispec_mac 			
     	
}		// end "UpdateCoordinateViewControls" 

