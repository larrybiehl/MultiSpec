// ===========================================================================
//	WDisplay.cpc1995 Purdue Research Foundation. All rights reserved.
// ===========================================================================
//	WDisplay.h		<- double-click + Command-D to see class declaration
//
// Revised by Larry Biehl on 03/27/2017
//
   
#include "SMulSpec.h"

#include "CImagWin.h"
#include "CDisplay.h"
#if defined multispec_win
#include "CImagVew.h"
#endif

#ifdef multispec_win
#include "WMainFrm.h"
#endif

#ifdef multispec_lin
#include "LMainFrame.h"
#include "LImageView.h"
#endif

#include "SExtGlob.h"

extern void 		ReleaseDisplaySupportMemory(
							Handle										displaySpecsHandle);


// === Static Member Variable ===

DisplaySpecsPtr		CMDisplay::sDisplaySpecsPtr = NULL;

                                                     					

// ---------------------------------------------------------------------------
//		CMDisplay 
// ---------------------------------------------------------------------------
//	Constructor

CMDisplay::CMDisplay()
{ 
	mDisplaySpecsHandle = NULL;
	
	mDisplaySpecsHandle = GetDisplaySpecsStructure (mDisplaySpecsHandle, TRUE);
													
	m_initializedFlag = (mDisplaySpecsHandle != NULL);

}		// end "CMDisplay"


// ---------------------------------------------------------------------------
//		~CMDisplay
// ---------------------------------------------------------------------------
//	Destructor
//

CMDisplay::~CMDisplay()
{  
	ReleaseDisplaySupportMemory ();
		
	UnlockAndDispose (mDisplaySpecsHandle);
	
	if (gToDisplayLevels.window == gActiveImageViewCPtr)
		gToDisplayLevels.vectorHandle = 
										UnlockAndDispose (gToDisplayLevels.vectorHandle);

}		// end "~CMDisplay"
											

/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl			Date: 06/05/95
//	Revised By:			Larry L. Biehl			Date: 02/26/97	

void 
CMDisplay::ActivatePalette (               
				CMImageView			*imageViewCPtr)
 
{
// oul: added the condition of _AMD64_ to be compatible with x64
// _AMD64_ is the predefined macro for x64 machines according to MSDN
#if !defined _X86_ && !defined _AMD64_

//	::ActivatePalette (imageViewCPtr);  

#endif	// !defined _X86_ && !defined _AMD64_
			      
#if defined _X86_ || defined _X86_
		
	imageViewCPtr->SendMessage(
							WM_DOREALIZE, (WPARAM)imageViewCPtr->m_hWnd, 1);  

#endif	// defined _X86_ || defined _X86_                            

}		// end "ActivatePalette" 
*/


//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								� Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	05/09/95
//	Revised By:	Larry L. Biehl					Date: 05/09/95

DisplaySpecsPtr 
CMDisplay::GetDisplaySpecsPtr (void)

{
	return (sDisplaySpecsPtr);

}		// end "GetDisplaySpecsPtr"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								� Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	05/17/95
//	Revised By:	Larry L. Biehl					Date: 05/17/95

void 
CMDisplay::GetOffscreenDimensions (
				UInt16			*heightPtr,
				UInt16			*widthPtr)

{                                        
	        			
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
												mDisplaySpecsHandle, kNoLock, kNoMoveHi);
	
	*heightPtr = (UInt16)displaySpecsPtr->imageDimensions[kVertical];
	*widthPtr = (UInt16)displaySpecsPtr->imageDimensions[kHorizontal]; 

}		// end "GetOffscreenDimensions" 




//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								� Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	05/17/95
//	Revised By:	Larry L. Biehl					Date: 05/17/95

double 
CMDisplay::GetMagnification (void)

{                                        
	        			
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
												mDisplaySpecsHandle, kNoLock, kNoMoveHi);
	
	return (displaySpecsPtr->magnification);

}		// end "GetMagnification" 




//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								� Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	07/16/95
//	Revised By:	Larry L. Biehl					Date: 07/16/95

double 
CMDisplay::GetMaxMagnification (void)

{                                        
	        			
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
												mDisplaySpecsHandle, kNoLock, kNoMoveHi);
	
	return (displaySpecsPtr->maxMagnification);

}		// end "GetMaxMagnification" 




double 
CMDisplay::GetMaxZoomValue(
				DisplaySpecsPtr			displaySpecsPtr)

{  
	double				maxZoom;      
	UInt32				offscreenSize;
							
							
   offscreenSize = (displaySpecsPtr->lineEnd - displaySpecsPtr->lineStart + 
                          displaySpecsPtr->lineInterval)/displaySpecsPtr->lineInterval; 
                          
//new
//	#ifdef _WIN32
		maxZoom = SInt32_MAX/offscreenSize;
//	#else
//		maxZoom = SInt16_MAX/offscreenSize;
//	#endif
//end new
   
   offscreenSize = (displaySpecsPtr->columnEnd - displaySpecsPtr->columnStart + 
                        displaySpecsPtr->columnInterval)/displaySpecsPtr->columnInterval;
																			
	if (displaySpecsPtr->displayType == 7)
		offscreenSize = displaySpecsPtr->numberChannels * (offscreenSize + 2);
                                              
//new
//	#ifdef _WIN32
	   maxZoom = MIN( maxZoom, SInt32_MAX/offscreenSize);
//	#else
//		maxZoom = MIN( maxZoom, SInt16_MAX/offscreenSize);
//	#endif
//end new
    
   maxZoom = MIN( gMaxMagnification, maxZoom );
   
   maxZoom = MAX( gMinMagnification, maxZoom );
   
   return ( maxZoom ); 
			
}		// end "GetMaxZoomValue"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								c Purdue Research Foundation
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
//	Coded By:			Larry L. Biehl					Date:	06/12/95
//	Revised By:			Larry L. Biehl					Date: 11/02/99

void
CMDisplay::ReleaseDisplaySupportMemory(void)

{  
	::ReleaseDisplaySupportMemory (mDisplaySpecsHandle);
	
/*	SignedByte		handleStatus;
	
	                                
	DisplaySpecsPtr displaySpecsPtr = (DisplaySpecsPtr) 
						GetHandleStatusAndPointer (mDisplaySpecsHandle, &handleStatus, kNoMoveHi); 
	
	if (displaySpecsPtr != NULL)
		{  
		displaySpecsPtr->symbolToPaletteEntryH = 
					UnlockAndDispose (displaySpecsPtr->symbolToPaletteEntryH); 
							
		displaySpecsPtr->channelsHandle =
					UnlockAndDispose (displaySpecsPtr->channelsHandle);
		
		displaySpecsPtr->displayClassGroupsHandle =
					UnlockAndDispose (displaySpecsPtr->displayClassGroupsHandle);
		
		displaySpecsPtr->savedClassCTableHandle = (CTabHandle)
					UnlockAndDispose ( (Handle)displaySpecsPtr->savedClassCTableHandle );
		
		displaySpecsPtr->savedGroupCTableHandle = (CTabHandle)                           
					UnlockAndDispose ( (Handle)displaySpecsPtr->savedGroupCTableHandle );
		
		displaySpecsPtr->localGroupNameHandle =
					UnlockAndDispose (displaySpecsPtr->localGroupNameHandle);
		
		if (displaySpecsPtr->paletteObject != NULL)
			{ 
			delete displaySpecsPtr->paletteObject;
			displaySpecsPtr->paletteObject = NULL;
			
			}		// end "if (displaySpecsPtr->paletteObject != NULL)"     
		
		if (displaySpecsPtr->backgroundPaletteObject != NULL)
			{ 
			delete displaySpecsPtr->backgroundPaletteObject;
			displaySpecsPtr->backgroundPaletteObject = NULL;
			
			}		// end "if (displaySpecsPtr->backgroundPaletteObject != NULL)" 
								                
		displaySpecsPtr->initializeStructureFlag = TRUE;
		displaySpecsPtr->numberDisplayClasses = 0;
		displaySpecsPtr->numberDisplayGroups = 0;
		
		MHSetState (mDisplaySpecsHandle, handleStatus);
		
		}		// end "if (displaySpecsPtr != NULL)"
*/
}		// end "ReleaseDisplaySupportMemory"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								� Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	11/10/95
//	Revised By:	Larry L. Biehl					Date: 11/10/95

void 
CMDisplay::SetDisplaySpecsPtr (
				DisplaySpecsPtr				displaySpecsPtr)

{
	sDisplaySpecsPtr = displaySpecsPtr;

}		// end "SetDisplaySpecsPtr"



//-----------------------------------------------------------------------------
//								 Copyright (1988-2010)
//								� Purdue Research Foundation
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
//	Coded By:	Larry L. Biehl					Date:	05/17/95
//	Revised By:	Larry L. Biehl					Date: 11/16/95

void 
CMDisplay::SetMagnification (
				double			magnification)

{                                        
	        			
			// Get handle and pointer to display specification for window		
		                                                                      
	DisplaySpecsPtr	displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
												mDisplaySpecsHandle, kNoLock, kNoMoveHi);
												      
	magnification = MAX(gMinMagnification, magnification);
	
	displaySpecsPtr->magnification = magnification;

}		// end "SetMagnification" 
