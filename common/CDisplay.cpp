//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CDisplay.cpp
//	Class Definition:		CDisplay.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		2.7
//
//	Revision date:			11/28/2017
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	This file contains routines for the CMImageWindow class
//
//	Functions in file:	
//
//	Include files:			"SMultiSpec.h"
//
//------------------------------------------------------------------------------------
   
#include "SMultiSpec.h"

#include "CDisplay.h"
#include "CImageWindow.h"

#ifdef multispec_lin
	#include "LMainFrame.h"
	#include "LImageView.h"
#endif

#if defined multispec_win
	#include "WImageView.h"
	#include "WMainFrame.h"
#endif

extern void ReleaseDisplaySupportMemory (
				Handle								displaySpecsHandle);


// === Static Member Variable ===

DisplaySpecsPtr		CMDisplay::sDisplaySpecsPtr = NULL;

                                                     					

//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMDisplay
//
//	Software purpose:	Constructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMDisplay::CMDisplay ()

{ 
	mDisplaySpecsHandle = NULL;
	
	mDisplaySpecsHandle = GetDisplaySpecsStructure (mDisplaySpecsHandle, TRUE);
													
	m_initializedFlag = (mDisplaySpecsHandle != NULL);

}	// end "CMDisplay"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		~CMDisplay
//
//	Software purpose:	Destructor 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

CMDisplay::~CMDisplay ()

{  
	ReleaseDisplaySupportMemory ();
		
	UnlockAndDispose (mDisplaySpecsHandle);
	
	if (gToDisplayLevels.window == gActiveImageViewCPtr)
		gToDisplayLevels.vectorHandle = 
											UnlockAndDispose (gToDisplayLevels.vectorHandle);

}	// end "~CMDisplay"
											

/*
//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ActivatePalette
//
//	Software purpose:	The purpose of this routine is to activate the palette
//							for the specified window. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None  				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/05/1995
//	Revised By:			Larry L. Biehl			Date: 02/26/1997	

void CMDisplay::ActivatePalette (
				CMImageView							*imageViewCPtr)
 
{
		// oul: added the condition of _AMD64_ to be compatible with x64
		// _AMD64_ is the predefined macro for x64 machines according to MSDN
		
	#if !defined _X86_ && !defined _AMD64_
		//	::ActivatePalette (imageViewCPtr);  
	#endif	// !defined _X86_ && !defined _AMD64_
			      
	#if defined _X86_ || defined _X86_
		imageViewCPtr->SendMessage (WM_DOREALIZE, (WPARAM)imageViewCPtr->m_hWnd, 1); 
	#endif	// defined _X86_ || defined _X86_                            

}	// end "ActivatePalette" 
*/


//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetDisplaySpecsPtr
//
//	Software purpose:	The purpose of this routine is to return the
//							current static display specification pointer.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	05/09/1995
//	Revised By:	Larry L. Biehl					Date: 05/09/1995

DisplaySpecsPtr CMDisplay::GetDisplaySpecsPtr (void)

{
	return (sDisplaySpecsPtr);

}	// end "GetDisplaySpecsPtr"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetOffscreenDimensions
//
//	Software purpose:	The purpose of this routine is to return the
//							current magnification value.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	05/17/1995
//	Revised By:	Larry L. Biehl					Date: 05/17/1995

void CMDisplay::GetOffscreenDimensions (
				UInt16								*heightPtr,
				UInt16								*widthPtr)

{                                        
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					mDisplaySpecsHandle);
	
	*heightPtr = (UInt16)displaySpecsPtr->imageDimensions[kVertical];
	*widthPtr = (UInt16)displaySpecsPtr->imageDimensions[kHorizontal]; 

}	// end "GetOffscreenDimensions" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetMagnification
//
//	Software purpose:	The purpose of this routine is to return the
//							current magnification value.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	05/17/1995
//	Revised By:	Larry L. Biehl					Date: 05/17/1995

double CMDisplay::GetMagnification (void)

{                                        
	        			
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					mDisplaySpecsHandle);
	
	return (displaySpecsPtr->magnification);

}	// end "GetMagnification" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double GetMaxMagnification
//
//	Software purpose:	The purpose of this routine is to return the
//							maximum magnification value possible.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	07/16/1995
//	Revised By:	Larry L. Biehl					Date: 07/16/1995

double CMDisplay::GetMaxMagnification (void)

{                                        
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					mDisplaySpecsHandle);
	
	return (displaySpecsPtr->maxMagnification);

}	// end "GetMaxMagnification" 



double CMDisplay::GetMaxZoomValue (
				DisplaySpecsPtr					displaySpecsPtr)

{  
	double								maxZoom;      
	UInt32								offscreenSize;
							
							
   offscreenSize = (displaySpecsPtr->lineEnd - displaySpecsPtr->lineStart + 
								displaySpecsPtr->lineInterval)/displaySpecsPtr->lineInterval; 
                          
	maxZoom = SInt32_MAX/offscreenSize;
   
   offscreenSize = (displaySpecsPtr->columnEnd - displaySpecsPtr->columnStart + 
							displaySpecsPtr->columnInterval)/displaySpecsPtr->columnInterval;
																		
	if (displaySpecsPtr->displayType == 7)
		offscreenSize = displaySpecsPtr->numberChannels * (offscreenSize + 2);
                                              
	maxZoom = MIN (maxZoom, SInt32_MAX/offscreenSize);
    
   maxZoom = MIN (gMaxMagnification, maxZoom);
   
   maxZoom = MAX (gMinMagnification, maxZoom);
   
   return (maxZoom); 
			
}	// end "GetMaxZoomValue"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
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
// Called By:			~CMHistogram in MHistGrm.cpp
//
//	Coded By:			Larry L. Biehl					Date:	06/12/1995
//	Revised By:			Larry L. Biehl					Date: 11/02/1999

void CMDisplay::ReleaseDisplaySupportMemory (void)

{  
	::ReleaseDisplaySupportMemory (mDisplaySpecsHandle);

}	// end "ReleaseDisplaySupportMemory"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetDisplaySpecsPtr
//
//	Software purpose:	The purpose of this routine is to set the
//							current static display specification pointer.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	11/10/1995
//	Revised By:	Larry L. Biehl					Date: 11/10/1995

void CMDisplay::SetDisplaySpecsPtr (
				DisplaySpecsPtr					displaySpecsPtr)

{
	sDisplaySpecsPtr = displaySpecsPtr;

}	// end "SetDisplaySpecsPtr"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetMagnification
//
//	Software purpose:	The purpose of this routine is to return the
//							current magnification value.
//
//	Parameters in:			
//
//	Parameters out:	None
//
//	Value Returned:  	
//
// Called By:
//
//	Coded By:	Larry L. Biehl					Date:	05/17/1995
//	Revised By:	Larry L. Biehl					Date: 11/16/1995

void CMDisplay::SetMagnification (
				double								magnification)

{                                        
	        			
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					mDisplaySpecsHandle);
												      
	magnification = MAX (gMinMagnification, magnification);
	
	displaySpecsPtr->magnification = magnification;

}	// end "SetMagnification" 
