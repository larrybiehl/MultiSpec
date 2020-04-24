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
//	File:						SImageWindow)class.cpp
//	Class Definition:		SImageWindow_class.h
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/21/2019
//
//	Language:				C++
//
//	System:					Linux, MacOS, and Windows Operating Systems
//
//	Brief description:	This file contains routines for the CMImageWindow class
//
//------------------------------------------------------------------------------------
  
#include "SMultiSpec.h"
                    
#include "SDisplay_class.h"
#include "SHistogram_class.h"
#include "SImageWindow_class.h"

#if defined multispec_wx
	#include "xImageView.h"
#endif
 
#if defined multispec_win      
	#include "WImageDoc.h"
	#include "WFileFormatDialog.h" 
#endif   //	defined multispec_win

extern Handle GetNewSelectionInfoHandle (void);
							
extern void AdjustImageWSize (
				Handle								windowInfoHandle);

/* 
	// Sun Screen Dump file format constants
	
#define	kSunScreenDumpMagicNumber			0x59a66a95
 
	// GAIA file format constants

#define	kGAIACreator							0x47414941	// 'GAIA'
#define	kGAIAFileType							0x47636C73	// 'Gcls'
 
	// MacSADIE file format constants      

#define	kMacSADIECreator						0x474D3839	// 'GM89'
#define	kMacSADIEFileType						0x72494D47	// 'rIMG'
 
	// TIFF file format constants       
	
#define	kTIFFMagicNumber1						0x4d4d002a
#define	kTIFFMagicNumber2						0x49492a00
 
	// TGA file format constants      
                                                
#define	kTGAFileType							0x54504943	// 'TPIC'
 
	// LAS file format constants      
                                                
#define	kIMAGCode								0x494D4147	// 'IMAG'
*/                                                     					

//------------------------------------------------------------------------------------
//		CMImageWindow 
//------------------------------------------------------------------------------------
//	Constructor

CMImageWindow::CMImageWindow (
				Handle		fileInfoHandle) : CMWindowInfo (fileInfoHandle)

{           

	mTheWindow = NULL;
	
	mImageDocCPtr = NULL;
	
	mImageViewCPtr = NULL; 
	
	mSupportFileStreamCPtr = NULL;
	
	m_initializedFlag = FALSE;
	
	//if (CMDataFile::m_initializedFlag && CMWindowInfo::m_initializedFlag)
	if (CMWindowInfo::m_initializedFlag)
		m_initializedFlag = TRUE;

}	// end "CMImageWindow::CMImageWindow ()"



//------------------------------------------------------------------------------------
//		~CMImageWindow
//------------------------------------------------------------------------------------
//	Destructor
//

CMImageWindow::~CMImageWindow ()

{
	if (mSupportFileStreamCPtr != NULL)
		delete mSupportFileStreamCPtr;
		
			// Make sure that the document reference to this class is NULL.
	
	if (mImageDocCPtr != NULL)
		mImageDocCPtr->SetImageWindowCPtr (NULL);
	
	mTheWindow = NULL;
	mImageDocCPtr = NULL;
	mImageViewCPtr = NULL;
	mSupportFileStreamCPtr = NULL;
		
}	// end "~CMImageWindow"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CloseSupportFile
//
//	Software purpose:	This routine closes the open image file and disposes of the 
//							description handle and file information handle associated with 
//							the image file.
//
//	Parameters in:					
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 05/05/1995
//	Revised By:			Larry L. Biehl			Date: 11/06/1995 

void CMImageWindow::CloseSupportFile (void)

{                                              
	if (mSupportFileStreamCPtr != NULL)
		mSupportFileStreamCPtr->MCloseFile (); 
			
}	// end "CloseSupportFile" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		WindowPtr CreateImageWindow
//
//	Software purpose:	The purpose of this routine is to create an image window and 
//							allocate a structure that contains handles to other structures 
//							which will be connected with the window and create the scroll 
//							bars for the window.
//
//	Parameters in:		Handle to window information for image.
//
//	Parameters out:	None
//
//	Value Returned:	Window pointer
// 
// Called By:			SetUpImageWindow in SOpenImage.cpp
// 						SetUpThematicImageWindow inSOpenImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/27/1988
//	Revised By:			Ravi S. Budruk			Date: 08/02/1988	
//	Revised By:			Larry L. Biehl			Date: 01/26/2018	

Boolean CMImageWindow::CreateImageWindow (
				Boolean								thematicWindowFlag)
				
{
			// Declare local variables
	
	Boolean								continueFlag = TRUE;
	
	CMFileStream						*fileStreamCPtr;
	
	LWindow								*theWindow = NULL; 
	                                      
	FileInfoPtr							fileInfoPtr;
	WindowInfoPtr						windowInfoPtr;
	
	Handle								fileInfoHandle = NULL,
											selectionInfoH = NULL,
											offScreenMapH = NULL,
											statFileInfoHandle = NULL;


	if (continueFlag)
		{
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																m_windowInfoHandle, kLock, kMoveHi);
		
		if (windowInfoPtr == NULL)
																							return (FALSE);
		
		windowInfoPtr->grafPortType = kCGrafType;
		
		if (thematicWindowFlag) 
			windowInfoPtr->showLegend = TRUE;	// TRUE; 
		
				// Set the window title and then show the window on the screen
		
		fileInfoHandle = windowInfoPtr->fileInfoHandle;
		
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
													
				// Set description code
				
		if (fileInfoPtr->descriptionsFlag)
			windowInfoPtr->descriptionCode |= fileInfoPtr->descriptionCode;
													 
		fileStreamCPtr = fileInfoPtr->fileStreamCPtr;
		
		if (gProjectInfoPtr != NULL)
			{
			windowInfoPtr->projectBaseImageFlag = 
														CheckIfProjectBaseImage (fileInfoHandle);
												
			windowInfoPtr->projectWindowFlag = 
								windowInfoPtr->projectBaseImageFlag ||
												CheckIfProjectAssociatedImage (fileInfoHandle);
									
			}	// end "if (gProjectInfoPtr != NULL)"
			
		}	// end "if (continueFlag)"
		
	if (continueFlag)
		{
				// Set up handle to drag gray rectangle region.
			
		selectionInfoH = GetNewSelectionInfoHandle ();
		continueFlag = (selectionInfoH != NULL);

		windowInfoPtr->selectionInfoHandle = selectionInfoH;
									
		}	// end "if (continueFlag)"
	/*		
	if (continueFlag)
		{
		windowInfoPtr->selectionInfoHandle = selectionInfoH;
		
				// Set selection area structure variables.
		
		selectionInfoPtr = (SelectionInfoPtr)GetHandlePointer (selectionInfoH);
		selectionInfoPtr->typeFlag = 0;
		selectionInfoPtr->numberPoints = 0;
		selectionInfoPtr->polygonCoordinatesHandle = NULL;
		
		}	// end "if (continueFlag)"
	*/		
	if (continueFlag)
		{
		windowInfoPtr->offScreenMapHandle = offScreenMapH = NULL;
		
		}	// end "if (continueFlag)"
   
   		// If there was a lack of memory, make certain that all handles have
   		// been disposed of properly and dipose of the window.
   		
   if (!continueFlag)
		{
		UnlockAndDispose (selectionInfoH);
		UnlockAndDispose (offScreenMapH);
		UnlockAndDispose (statFileInfoHandle);
		theWindow = NULL;
		
		}	// end "if (!continueFlag)"
	
	return (continueFlag); 
	
}	// end "CreateImageWindow" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FinishCreateSelf
//
//	Software purpose:	The selected image file has been accepted.  Finish creating the 
//							Image Window objects.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	
// 
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 04/01/1995
//	Revised By:			Larry L. Biehl			Date: 05/05/1995	

Boolean CMImageWindow::FinishCreateSelf	()

{
	Boolean								continueFlag; 
	
	
			// Initialize the variables and handles in the layer info structure.
				
	continueFlag = UpdateLayerInfoStructure (); 
	
			// Create Support File Stream Object for this image
			
	if (continueFlag)
		mSupportFileStreamCPtr = new CMFileStream (GetImageFileStreamPointer ());
		
	continueFlag = (mSupportFileStreamCPtr != NULL);
		
	return (continueFlag);

}	// end "FinishCreateSelf"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean FlushSupportFile
//
//	Software purpose:	This routine flushes any data left in the I/O buffer to the 
//							disk file.
//
//	Parameters in:					
//
//	Parameters out:	None
//
//	Value Returned:	None
//
// Called By:						
//
//	Coded By:			Larry L. Biehl			Date: 05/22/1995
//	Revised By:			Larry L. Biehl			Date: 05/22/1995 

Boolean CMImageWindow::FlushSupportFile (void)

{
	return (mSupportFileStreamCPtr->MFlushFile ());
			
}	// end "FlushSupportFile"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetImageFileNamePointer
//
//	Software purpose:	The purpose of this routine is to return the file name 
//							for the file stream associated with the ImageWindow class. It
//							will return the file name for the first file only if ImageWindow
//							represents a linked list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/24/1995
//	Revised By:			Larry L. Biehl			Date: 03/16/2017	

void* CMImageWindow::GetImageFileNamePointer (
				SInt16								returnCode)

{  
	void*									fileNamePtr = NULL;
	CMFileStream*						fileStreamCPtr; 
	                                                	
													              
	fileStreamCPtr = GetImageFileStreamPointer ();
	if (fileStreamCPtr != NULL)
		fileNamePtr = (FileStringPtr)fileStreamCPtr->GetFileNameCPtr (returnCode);

	return (fileNamePtr);
	
}	// end "GetImageFileNamePointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetImageFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to return the input file 
//							stream class pointer for the image window.  Note that
//							this routine  returns the file stream object for the
//							first file only if the image window represents a linked 
//							list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/05/1995
//	Revised By:			Larry L. Biehl			Date: 03/01/1999	

CMFileStream* CMImageWindow::GetImageFileStreamPointer ()

{  
	CMFileStream						*imageFileStreamPtr = NULL;
	
	Handle 								fileInfoHandle;
	
	
	fileInfoHandle = GetFileInfoHandle (m_windowInfoHandle);

	if (fileInfoHandle != NULL)
		{                
		FileInfoPtr				fileInfoPtr;
	
		fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
		imageFileStreamPtr = fileInfoPtr->fileStreamCPtr;
		
		}	// end "if (fileInfoHandle != NULL)" 
			
	return (imageFileStreamPtr);
			
}	// end "GetImageFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetSupportFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to return the support file 
//							stream class pointer for the image window.  
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:		
//
//	Coded By:			Larry L. Biehl			Date: 05/05/1995
//	Revised By:			Larry L. Biehl			Date: 05/09/1995	

CMFileStream* CMImageWindow::GetSupportFileStreamPointer ()

{	
	//if (this != NULL)
		return (mSupportFileStreamCPtr);

	//else	// this == NULL
	//	return (NULL);
			
}	// end "GetSupportFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 OpenImageWindow
//
//	Software purpose:	The purpose of this routine is to open an image window
//							either multispectral type or Thematic (classification)
//							type.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	0 = no error
// 
// Called By:			OnOpenDocument in WImageDoc.cpp and xImageDoc.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/18/1989
//	Revised By:			Larry L. Biehl			Date: 03/01/1999	

SInt16 CMImageWindow::OpenImageWindow (void)

{
	SInt16								returnCode = noErr;
	
	Boolean								continueFlag;
										
	
			//		Finish creating objects for the image window.
			
	continueFlag = FinishCreateSelf ();
	
	if (continueFlag)
		{
				// Set the document size
				
		::AdjustImageWSize (m_windowInfoHandle);
		
		}	// end "if (continueFlag)"
	/*		
	if (continueFlag)
		{
		fileInfoHandle = GetFileInfoHandle (m_windowInfoHandle);
		thematicType = GetThematicType (fileInfoHandle);
	
		if (!thematicType)
			continueFlag = SetUpImageWindow (m_windowInfoHandle);
				
		else	// thematicType
			continueFlag = SetUpThematicImageWindow (m_windowInfoHandle);
				
   	}	// end "if (continueFlag)"
	*/    		
	if (!continueFlag)
		returnCode = 1;
    
	return (returnCode);
	
}	// end "OpenImageWindow"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetImageFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to store the input file 
//							stream class pointer for the image window represented 
//							by the image window class pointer within the respective 
//							file information handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/20/1995
//	Revised By:			Larry L. Biehl			Date: 03/01/1999

void CMImageWindow::SetImageFileStreamPointer (
				CMFileStream						*fileStreamPtr)

{  
	FileInfoPtr							fileInfoPtr;
	
	Handle 								fileInfoHandle;
	
	
	fileInfoHandle = GetFileInfoHandle (m_windowInfoHandle);
	
	fileInfoPtr = (FileInfoPtr)GetHandlePointer (fileInfoHandle);
	   
	if (fileInfoPtr != NULL)
		fileInfoPtr->fileStreamCPtr = fileStreamPtr;
			
}	// end "SetImageFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void SetSupportFileStreamPointer
//
//	Software purpose:	The purpose of this routine is to store the support file 
//							stream class pointer for the image window represented 
//							by the image window class pointer within the respective 
//							file information handle.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 03/02/1999
//	Revised By:			Larry L. Biehl			Date: 11/24/1999

void CMImageWindow::SetSupportFileStreamPointer (void)

{  
			// Create Support File Stream Object for this image window object if
			// needed.
			
	if (mSupportFileStreamCPtr == NULL)
		mSupportFileStreamCPtr = new CMFileStream ();
			
}	// end "SetSupportFileStreamPointer"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateLayerInfoStructure
//
//	Software purpose:	The purpose of this routine is to update the layer information 
//							structure.  The update may be to the active Image Window Class 
//							(if new image is to be logically linked to it) or to the new 
//							Image Window class pointer created for this new image window.
//
//	Parameters in:		ImageWindowClass pointer to be used in the update.
//
//	Parameters out:	None
//
//	Value Returned:	True if update was successful.
//							False if update was not possible because of lack of
//											available.
// 
// Called By:			CreateImageWindow in SImageWindow_class.cpp
//							AddToImageWindowFile in SOpenFileDialog.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/15/1991
//	Revised By:			Larry L. Biehl			Date: 03/01/1999	

Boolean CMImageWindow::UpdateLayerInfoStructure (void)

{
	Boolean								continueFlag = TRUE;
	
	SignedByte							wHandleStatus;
										
	WindowInfoPtr						windowInfoPtr;
	
	
			// Get pointers to the needed structures.
			
	windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
																m_windowInfoHandle, &wHandleStatus);
							
	continueFlag = ::UpdateLayerInfoStructure (windowInfoPtr, 
																windowInfoPtr->fileInfoHandle);
	
	MHSetState (m_windowInfoHandle, wHandleStatus); 
		
	return (continueFlag);
			
}	// end "UpdateLayerInfoStructure"
