//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CWindowInfo.cpp
//	Class Definition:		CWindowInfo.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/09/2018
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide a
//								class for supporting the window info structure.
//
//	Functions in file:								CMWindowInfo
//															~CMWindowInfo
//								Handle					GetWindowInfoHandle
//								void						ReleaseOffscreenSupportMemory
//								void						SetImageWindowCPtrInWindowInfo
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "CWindowInfo.h"

extern void ReleaseOffscreenSupportMemory (
				WindowInfoPtr						windowInfoPtr);
                      


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMWindowInfo
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

CMWindowInfo::CMWindowInfo ()

{
	m_windowInfoHandle = NULL;
	m_initializedFlag = FALSE;
	
			// Initialize the variables and handles in the window info structure.
			
	m_windowInfoHandle = InitializeWindowInfoStructure (m_windowInfoHandle,
																			kNotPointer,
																			NULL,
																			kNoImageType,
																			0);
	
	m_initializedFlag = (m_windowInfoHandle != NULL);

}	// end "CMWindowInfo"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		CMWindowInfo
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

CMWindowInfo::CMWindowInfo (
				Handle								fileInfoHandle)
{
	m_windowInfoHandle = NULL;
	m_initializedFlag = FALSE;
	
			// Initialize the variables and handles in the window info structure.
	
	m_windowInfoHandle = InitializeWindowInfoStructure (m_windowInfoHandle,
																			kNotPointer,
																			fileInfoHandle,
																			kNoImageType,
																			0);
	
	m_initializedFlag = (m_windowInfoHandle != NULL);

}	// end "CMWindowInfo"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		~CMWindowInfo
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

CMWindowInfo::~CMWindowInfo ()

{
	WindowInfoPtr						windowInfoPtr;
	SignedByte							handleStatus;
	
	
	if (this == NULL)
																									return;
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (m_windowInfoHandle,
																					&handleStatus);
	
	if (windowInfoPtr != NULL)              	
		{
				// Get handle to image file information and close the file(s).		
				//	Then dispose of the file information handle.							
		
		CloseWindowImageFiles (m_windowInfoHandle);
		
		windowInfoPtr->layerInfoHandle =
										UnlockAndDispose (windowInfoPtr->layerInfoHandle);
		windowInfoPtr->mapProjectionHandle =
										UnlockAndDispose (windowInfoPtr->mapProjectionHandle);
		windowInfoPtr->selectionInfoHandle = 
										UnlockAndDispose (windowInfoPtr->selectionInfoHandle);
		windowInfoPtr->windowRecordHandle = 
										UnlockAndDispose (windowInfoPtr->windowRecordHandle);
		windowInfoPtr->textH = UnlockAndDispose (windowInfoPtr->textH);
	   
	   		// Offscreen support memory.
	   		//		offscreenGWorld      
	   		//		offScreenMapHandle  
	   		//		imageBaseAddressH	
	   		
		ReleaseOffscreenSupportMemory ();
		
		}	// end "if (windowInfoPtr != NULL)"
		
	m_windowInfoHandle = UnlockAndDispose (m_windowInfoHandle);

}	// end "~CMWindowInfo" 


      
Handle CMWindowInfo::GetWindowInfoHandle ()

{ 
	return (m_windowInfoHandle); 
	
}	// end "GetWindowInfoHandle"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ReleaseOffscreenSupportMemory
//
//	Software purpose:	The purpose of this routine is to dispose of the offscreen
//							memory for the window.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 06/12/1995
//	Revised By:			Larry L. Biehl			Date: 07/03/1996	

void CMWindowInfo::ReleaseOffscreenSupportMemory (void)

{  
	SignedByte				handleStatus;
	
    
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)
							GetHandleStatusAndPointer (m_windowInfoHandle, &handleStatus);
			
	::ReleaseOffscreenSupportMemory (windowInfoPtr);

	MHSetState (m_windowInfoHandle, handleStatus);

	
}	// end "ReleaseOffscreenSupportMemory"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SetImageWindowCPtrInWindowInfo
//
//	Software purpose:	The purpose of this routine is to set the image window
//							class pointer into the window information structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			CMImageDoc::OnOpenDocument in CWinInfo.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/03/1995
//	Revised By:			Larry L. Biehl			Date: 11/03/1995

void CMWindowInfo::SetImageWindowCPtrInWindowInfo (
				CMImageWindow*						imageWindowCPtr)

{
	WindowInfoPtr						windowInfoPtr;
	
	
			// Initialize the some variables and handles in the structure.
	
	windowInfoPtr = (WindowInfoPtr)GetHandlePointer (m_windowInfoHandle);
	
	if (windowInfoPtr != NULL)	
		windowInfoPtr->cImageWindowPtr = imageWindowCPtr;
		
}	// end "SetImageWindowCPtrInWindowInfo" 

