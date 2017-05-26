// ===========================================================================
// MWinInfo.cp	c1995 Purdue Research Foundation. All rights reserved.
// ===========================================================================
//	MWinInfo.h		<- double-click + Command-D to see class declaration     
        
#include "SMulSpec.h"

#include "CWinInfo.h"

extern void 		ReleaseOffscreenSupportMemory (
							WindowInfoPtr						windowInfoPtr);
                      
 
 
 
                       


// ---------------------------------------------------------------------------
//		CMWindowInfo 			// replace this with your App type
// ---------------------------------------------------------------------------
//	Constructor

CMWindowInfo::CMWindowInfo()
{
	m_windowInfoHandle = NULL;
	m_initializedFlag = FALSE;
	
			// Get a new handle to a block of memory to be used for					
			// file information for the new image window.								
	
//	m_windowInfoHandle = MNewHandle ( sizeof(WindowInfo) );
	
			// Initialize the variables and handles in the window info structure.
			
	m_windowInfoHandle = InitializeWindowInfoStructure (
											m_windowInfoHandle, 
											kNotPointer,
											NULL,
											kNoImageType,
											0);
	
	m_initializedFlag = (m_windowInfoHandle != NULL);

}		// end "CMWindowInfo"



// ---------------------------------------------------------------------------
//		CMWindowInfo 			// replace this with your App type
// ---------------------------------------------------------------------------
//	Constructor

CMWindowInfo::CMWindowInfo (
				Handle			fileInfoHandle)
{
	m_windowInfoHandle = NULL;
	m_initializedFlag = FALSE;
	
			// Initialize the variables and handles in the file info structure
			// if needed.
	
//	if (fileInfoHandle == NULL)
//		fileInfoHandle = InitializeFileInfoStructure (
//																fileInfoHandle, kNotPointer);
	
			// Initialize the variables and handles in the window info structure.
	
//	if (fileInfoHandle != NULL)		
		m_windowInfoHandle = InitializeWindowInfoStructure (
											m_windowInfoHandle, 
											kNotPointer,
											fileInfoHandle,
											kNoImageType,
											0);
	
	m_initializedFlag = (m_windowInfoHandle != NULL);

}		// end "CMWindowInfo"



// ---------------------------------------------------------------------------
//		~CMultiSpecImage
// ---------------------------------------------------------------------------
//	Destructor
//

CMWindowInfo::~CMWindowInfo()
{
	WindowInfoPtr		windowInfoPtr;
	SignedByte			handleStatus;
	
	
	if (this == NULL)
																						return;
	
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
										m_windowInfoHandle,
										&handleStatus,
										kNoMoveHi);
	
	if (windowInfoPtr != NULL)              	
		{
				// Get handle to image file information and close the file(s).		
				//	Then dispose of the file information handle.							
		
		CloseWindowImageFiles (m_windowInfoHandle);
		
		windowInfoPtr->layerInfoHandle =
								UnlockAndDispose (windowInfoPtr->layerInfoHandle);
//		windowInfoPtr->descriptionH =
//								UnlockAndDispose (windowInfoPtr->descriptionH);
//		windowInfoPtr->graphRecordHandle=
//								UnlockAndDispose (windowInfoPtr->graphRecordHandle);
		windowInfoPtr->mapProjectionHandle = 
								UnlockAndDispose (windowInfoPtr->mapProjectionHandle); 
		windowInfoPtr->selectionInfoHandle = 
								UnlockAndDispose (windowInfoPtr->selectionInfoHandle);
		windowInfoPtr->windowRecordHandle = 
								UnlockAndDispose (windowInfoPtr->windowRecordHandle); 
		windowInfoPtr->textH =
								UnlockAndDispose (windowInfoPtr->textH); 
	   
	   		// Offscreen support memory.
	   		//		offscreenGWorld      
	   		//		offScreenMapHandle  
	   		//		imageBaseAddressH	
	   		
		ReleaseOffscreenSupportMemory ();
		
				// Dispose of the list handle if it exists, Thematic type
				// image windows. This is disposed of as part of the
				// legend split window
		
//		if ( windowInfoPtr->legendListHandle != NULL )
//			LDispose ( windowInfoPtr->legendListHandle );
		
		}		// end "if (windowInfoPtr != NULL)"
		
	m_windowInfoHandle = UnlockAndDispose (m_windowInfoHandle);

}		// end "~CMWindowInfo" 


      
Handle CMWindowInfo::GetWindowInfoHandle ()

{ 
	return (m_windowInfoHandle); 
	
}		// end "GetWindowInfoHandle"



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
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
//	Coded By:			Larry L. Biehl			Date: 06/12/95
//	Revised By:			Larry L. Biehl			Date: 07/03/96	

void
CMWindowInfo::ReleaseOffscreenSupportMemory(void)

{  
	SignedByte				handleStatus;
	
    
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)
			GetHandleStatusAndPointer (m_windowInfoHandle, &handleStatus, kNoMoveHi);
			
	::ReleaseOffscreenSupportMemory (windowInfoPtr);
/*				                  
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN
	
	#if !defined _X86_ && !defined _AMD64_
	
		if (windowInfoPtr->offscreenGWorld != NULL)
			{
			delete windowInfoPtr->offscreenGWorld;
			windowInfoPtr->offscreenGWorld = NULL;
			windowInfoPtr->offScreenMapHandle = NULL;
			windowInfoPtr->offscreenMapSize = 0;
			
			}		// end "if (windowInfoPtr->offscreenGWorld != NULL)"
			                               
	#endif // !defined _X86_ && !defined _AMD64_
	
	// oul: added the condition of _AMD64_ to be compatible with x64
	// _AMD64_ is the predefined macro for x64 machines according to MSDN

	#if defined _X86_ || defined _AMD64_
	                
		windowInfoPtr->offScreenMapHandle =  
									UnlockAndDispose (windowInfoPtr->offScreenMapHandle);
		
		windowInfoPtr->imageBaseAddressH = 							
									UnlockAndDispose (windowInfoPtr->imageBaseAddressH);
		
		windowInfoPtr->offscreenMapSize = 0;
			                               
	#endif // defined _X86_ || defined _AMD64_
*/		
	MHSetState (m_windowInfoHandle, handleStatus);

	
}		// end "ReleaseOffscreenSupportMemory"  



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
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
//	Coded By:			Larry L. Biehl			Date: 11/03/95
//	Revised By:			Larry L. Biehl			Date: 11/03/95

void 
CMWindowInfo::SetImageWindowCPtrInWindowInfo (
				CMImageWindow*			imageWindowCPtr)

{
	WindowInfoPtr			windowInfoPtr;
	
	
			// Initialize the some variables and handles in the structure.
	
	windowInfoPtr = (WindowInfoPtr)
							GetHandlePointer (m_windowInfoHandle, kNoLock, kNoMoveHi);
	
	if (windowInfoPtr != NULL)	
		windowInfoPtr->cImageWindowPtr = imageWindowCPtr;
		
}		// end "SetImageWindowCPtrInWindowInfo" 

