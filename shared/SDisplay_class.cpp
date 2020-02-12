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
//	File:						SDisplay_class.cpp
//	Class Definition:		SDisplay_class.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/13/2019
//
//	Language:				C++
//
//	System:					Linux, MacOS and Windows Operating Systems
//
//	Brief description:	This file contains routines for the CMImageWindow class
//
//------------------------------------------------------------------------------------
   
#include "SMultiSpec.h"

#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#ifdef multispec_wx
	#include "xMainFrame.h"
	#include "xImageView.h"
#endif

#if defined multispec_win
	#include "WImageView.h"
	#include "WMainFrame.h"
#endif


// === Static Member Variable ===

DisplaySpecsPtr		CMDisplay::sDisplaySpecsPtr = NULL;

                                                     					

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//                   Copyright 1988-2020 Purdue Research Foundation
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



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//                   Copyright 1988-2020 Purdue Research Foundation
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
//	Revised By:	Larry L. Biehl					Date: 12/17/2019

void CMDisplay::GetOffscreenDimensions (
				UInt32								*heightPtr,
				UInt32								*widthPtr)

{                                        
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer (
																					mDisplaySpecsHandle);
	
	*heightPtr = displaySpecsPtr->imageDimensions[kVertical];
	*widthPtr = displaySpecsPtr->imageDimensions[kHorizontal]; 

}	// end "GetOffscreenDimensions" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//                   Copyright 1988-2020 Purdue Research Foundation
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
																		
	if (displaySpecsPtr->displayType == kSideSideChannelDisplayType)
		offscreenSize = displaySpecsPtr->numberChannels * (offscreenSize + 2);
                                              
	maxZoom = MIN (maxZoom, SInt32_MAX/offscreenSize);
    
   maxZoom = MIN (gMaxMagnification, maxZoom);
   
   maxZoom = MAX (gMinMagnification, maxZoom);
   
   return (maxZoom); 
			
}	// end "GetMaxZoomValue"



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
// Called By:			DisposeImageWindowSupportMemory in xImageView.cpp
//
//	Coded By:			Larry L. Biehl					Date:	06/12/1995
//	Revised By:			Larry L. Biehl					Date: 11/02/1999

void CMDisplay::ReleaseDisplaySupportMemory (void)

{  
	::ReleaseDisplaySupportMemory (mDisplaySpecsHandle);

}	// end "ReleaseDisplaySupportMemory"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
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
//                   Copyright 1988-2020 Purdue Research Foundation
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
